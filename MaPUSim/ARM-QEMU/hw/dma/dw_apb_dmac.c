/*
 * dw_apb_dmac.c

 *
 *  Created on: Oct 26, 2015
 *      Author: luoxq
 */

#include "hw/sysbus.h"
#include "qemu-common.h"
#include "qemu/main-loop.h"
#include "qemu/timer.h"

//#define DEBUG_DW_APB_DMAC

#ifdef DEBUG_DW_APB_DMAC
#define DPRINTF(fmt, ...) \
do { fprintf(stderr, "dw_apb_dmac: " fmt , ## __VA_ARGS__); } while (0)
#else
#define DPRINTF(fmt, ...) \
do {} while (0)
#endif

#define TYPE_DW_APB_DMAC "dw_apb_dmac"
#define DW_APB_DMAC(obj) OBJECT_CHECK(DwApbDmacState, (obj), TYPE_DW_APB_DMAC)

#define TRTIME(size) ((size) / 3)   // size / (128bit * 200MHz) * 10^10 ns


enum DMA_CTRL_MMAP {
  RAW_START = 0x2C0,
  STATUS_START = 0x2E8,
  MASK_START = 0x310,
  CLEAR_START = 0x338,
  STATUS_INT = 0x360,
  REQ_SRC_REG = 0x368,
  REQ_DEST_REG = 0x370,
  SINGLE_REQ_SRC_REG = 0x378,
  SINGLE_REQ_DEST_REG = 0x380,
  LAST_REQ_SRC_REG = 0x388,
  LAST_REQ_DEST_REG = 0x390,
  DMA_CFG_REG = 0x398,
  CH_EN_REG = 0x3A0,
  DMA_ID_REG = 0x3A8,
  DMA_TEST_REG = 0x3B0,
  DMA_COMP_PARAMS_6 = 0x3C8,
  DMA_COMP_PARAMS_5 = 0x3D0,
  DMA_COMP_PARAMS_4 = 0x3D8,
  DMA_COMP_PARAMS_3 = 0x3E0,
  DMA_COMP_PARAMS_2 = 0x3E8,
  DMA_COMP_PARAMS_1 = 0x3F0,
  DMA_COMPONENT_ID = 0x3F8,
};

#define OFS2R(offset) (((offset)-RAW_START)>>3)

enum DMAC_CHANNEL_REGS_MAP
{
  SAR = 0x0,
  DAR = 0x8,
  LLP = 0x10,
  CTL = 0x18,
  SSTAT = 0x20,
  DSTAT = 0x28,
  SSTATAR = 0x30,
  DSTATAR = 0x38,
  CFG = 0x40,
  SGR = 0x48,
  DSR = 0x50,

};

#define COFS2R(offset) ((offset)>>3)

typedef struct DwApbDmacChannel
{
  QEMUTimer *timer;
  uint64_t chnRegs[11];

  void *father;
  uint32_t id;

}DwApbDmacChannel;



typedef struct DwApbDmacState
{
    SysBusDevice parent_obj;

    MemoryRegion iomem;
    qemu_irq  irq;

    DwApbDmacChannel channel[8];

    uint64_t Regs[40];

}DwApbDmacState;

/*** VMState ***/
static const VMStateDescription vmstate_dw_apb_dmac_channel= {
    .name = "dw_apb_dmac.channel",
    .version_id = 1,
    .minimum_version_id = 1,
    .fields = (VMStateField[]) {
        VMSTATE_UINT64_ARRAY(chnRegs, DwApbDmacChannel, 11),
        VMSTATE_UINT32(id, DwApbDmacChannel),
        VMSTATE_END_OF_LIST()
    }
};

static const VMStateDescription vmstate_dw_apb_dmac_state = {
  .name = "dw_apb_dmac",
  .version_id = 1,
  .minimum_version_id = 1,
  .fields = (VMStateField[]) {
      VMSTATE_STRUCT_ARRAY(channel, DwApbDmacState,
          8, 0, vmstate_dw_apb_dmac_channel, DwApbDmacChannel),
      VMSTATE_UINT64_ARRAY(Regs, DwApbDmacState, 40),
      VMSTATE_END_OF_LIST()
  }
};

static void dmaStart(void *opaque)
{
  DwApbDmacChannel *s = opaque;
  uint32_t num = (s->chnRegs[COFS2R(CTL)] >> 32) & 0xfff;
  uint32_t swidth = (s->chnRegs[COFS2R(CTL)] >> 4) & 7;
  uint32_t dwidth = (s->chnRegs[COFS2R(CTL)] >> 1) & 7;
  uint32_t width = swidth > dwidth ? swidth : dwidth;
  uint32_t pt = 0;
  uint8_t buf[1024*1024];

  width = 1 << width;

  DPRINTF("Starting DMA for addr: %#x to addr: %#x, size: %d \n", s->chnRegs[COFS2R(SAR)], s->chnRegs[COFS2R(DAR)], num * width);

  cpu_physical_memory_read( s->chnRegs[COFS2R(SAR)], buf, width*num);
  cpu_physical_memory_write( s->chnRegs[COFS2R(DAR)], buf, width*num);

  ((DwApbDmacState*)(s->father))->Regs[OFS2R(RAW_START)] |= 1 << s->id;
  if((((DwApbDmacState*)(s->father))->Regs[OFS2R(MASK_START)] >> s->id) & 1)
  {
    ((DwApbDmacState*)(s->father))->Regs[OFS2R(STATUS_START)] |= 1 << s->id;
    ((DwApbDmacState*)(s->father))->Regs[OFS2R(STATUS_INT)] = 1;
    ((DwApbDmacState*)(s->father))->Regs[OFS2R(CH_EN_REG)] &= ~(1 << s->id);
    qemu_irq_raise(((DwApbDmacState*)(s->father))->irq);
  }
}

static void dmaRun(void *opaque, int i)
{
  DwApbDmacState *s = opaque;
  uint32_t num = (s->channel[i].chnRegs[COFS2R(CTL)] >> 32) & 0xfff;
  uint32_t swidth = (s->channel[i].chnRegs[COFS2R(CTL)] >> 4) & 7;
  uint32_t dwidth = (s->channel[i].chnRegs[COFS2R(CTL)] >> 1) & 7;
  uint32_t width = swidth > dwidth ? swidth : dwidth;

  width = 1 << width;

  timer_mod(s->channel[i].timer, TRTIME(num * width));
}

static void updateStatus(void *opaque)
{
  DwApbDmacState *s = opaque;
  int i;
  if (s->Regs[OFS2R(CLEAR_START)] & 0xff)
    qemu_irq_lower(s->irq);
  for (i=0; i<5; i++)
  {
    s->Regs[OFS2R(RAW_START)+i] &= ~s->Regs[OFS2R(CLEAR_START)+i];
    s->Regs[OFS2R(CLEAR_START)+i] = 0;
    s->Regs[OFS2R(STATUS_START)+i] = s->Regs[OFS2R(RAW_START)+i] & s->Regs[OFS2R(MASK_START)+i];
    s->Regs[OFS2R(STATUS_INT)] = (s->Regs[OFS2R(STATUS_INT)] & ~(1 << i)) | (((s->Regs[OFS2R(STATUS_START)+i] & 0xff) ? 1 : 0) << i);
  }
}


static uint64_t dw_apb_dmac_read(void *opaque, hwaddr offset, unsigned size)
{
  DwApbDmacState *s = (DwApbDmacState *)opaque;
  uint64_t ret = 0;

  DPRINTF("dmac read: offset=0x%x size=0x%x\n", offset, size);

  if (offset < RAW_START)
  {
    switch (size)
    {
    case 8:
      ret = s->channel[offset/0x58].chnRegs[(offset%0x58)>>3];
      break;
    case 4:
      ret = *(uint32_t*)((uint8_t*)&(s->channel[offset/0x58].chnRegs) + offset % 0x58);
      break;
    case 2:
      ret = *(uint16_t*)((uint8_t*)&(s->channel[offset/0x58].chnRegs) + offset % 0x58);
      break;
    case 1:
      ret = *(uint8_t*)((uint8_t*)&(s->channel[offset/0x58].chnRegs) + offset % 0x58);
      break;
    }
  }
  else if(offset >= STATUS_INT)
  {
    switch (offset)
    {
    case STATUS_INT:
    case DMA_CFG_REG:
    case CH_EN_REG:
      switch (size)
      {
      case 8:
        ret = s->Regs[OFS2R(offset)];
        break;
      case 4:
        ret = *(uint32_t*)((uint8_t*)&(s->Regs) + offset - RAW_START);
        break;
      case 2:
        ret = *(uint16_t*)((uint8_t*)&(s->Regs) + offset - RAW_START);
        break;
      case 1:
        ret = *(uint8_t*)((uint8_t*)&(s->Regs) + offset - RAW_START);
        break;
      }
      break;
    default:
      ret = 0;
//      qemu_log_mask(LOG_GUEST_ERROR, "ERROR: dw_apb_dmac_read: Bad offset 0x%x\n", (int)offset);
      break;
    }
  }
  else
    switch (size)
    {
    case 8:
      ret = s->Regs[OFS2R(offset)];
      break;
    case 4:
      ret = *(uint32_t*)((uint8_t*)&(s->Regs) + offset - RAW_START);
      break;
    case 2:
      ret = *(uint16_t*)((uint8_t*)&(s->Regs) + offset - RAW_START);
      break;
    case 1:
      ret = *(uint8_t*)((uint8_t*)&(s->Regs) + offset - RAW_START);
      break;
    }

  DPRINTF("dmac read: ret=0x%x\n", ret);
  return ret;
}

static void dw_apb_dmac_write(void * opaque, hwaddr offset,
                        uint64_t value, unsigned size)
{
  DwApbDmacState *s = (DwApbDmacState *)opaque;
  int i;
  static uint64_t tempMask;
  static uint64_t tempChEn;

  DPRINTF("dmac write: offset=0x%x value=0x%llx size=0x%x\n", offset, value, size);

  if (offset < RAW_START) //channel registers
  {
    switch (size)
    {
    case 8:
      s->channel[offset/0x58].chnRegs[(offset%0x58)>>3] = value;
      break;
    case 4:
      *(uint32_t*)((uint8_t*)&(s->channel[offset/0x58].chnRegs) + offset % 0x58) = value & 0xffffffff;
      break;
    case 2:
      *(uint16_t*)((uint8_t*)&(s->channel[offset/0x58].chnRegs) + offset % 0x58) = value & 0xffff;
      break;
    case 1:
      *(uint8_t*)((uint8_t*)&(s->channel[offset/0x58].chnRegs) + offset % 0x58) = value & 0xff;
      break;
    }
  }
  else if (offset < CLEAR_START) // mask registers
  {
    switch (size)
    {
    case 8:
    case 4:
    case 2:
      if ((offset - RAW_START) % 8 == 0)
      {
        s->Regs[OFS2R(offset)] = ((~(value >> 8) & s->Regs[OFS2R(offset)]) |
              ((value >> 8) & value)) & 0xff;
        updateStatus(s);
      }
      break;
    case 1:
      if (offset % 8 == 0)
        tempMask = value & 0xff;
      else if (offset % 8 == 1)
      {
        s->Regs[OFS2R(offset-1)] = ((~value & s->Regs[OFS2R(offset-1)]) |
              (value & tempMask)) & 0xff;
        updateStatus(s);
      }
      break;
    }
  }
  else if (offset < STATUS_INT) // clear registers
  {
    if (offset % 8 == 0)
    {
      s->Regs[OFS2R(offset)] = value;
      updateStatus(s);
    }
  }
  else
  {
    if (offset >= STATUS_INT)
    {
      switch (offset)
      {
      case DMA_CFG_REG:
        s->Regs[OFS2R(DMA_CFG_REG)] = value;
        if (s->Regs[OFS2R(DMA_CFG_REG)] & 0xff)
          for(i=0; i<8; i++)
            if ((s->Regs[OFS2R(CH_EN_REG)] >> i) & 0x1)
              dmaRun(s, i);
        break;
      case CH_EN_REG:
        switch (size)
        {
        case 8:
        case 4:
        case 2:
          s->Regs[OFS2R(CH_EN_REG)] = ((~(value >> 8) & s->Regs[OFS2R(CH_EN_REG)]) |
              (((value >> 8) & value))) & 0xFF;
          if (s->Regs[OFS2R(DMA_CFG_REG)])
            for (i=0; i<8; i++)
              if((((value >> 8) & value) >> i) & 0x1)
                dmaRun(s, i);
          break;
        case 1:
          tempChEn = value & 0xff;
          break;
        }
        break;
      case 929: //CH_EN_REG+1, for size = 1 byte
        s->Regs[OFS2R(CH_EN_REG)] = ((~value & s->Regs[OFS2R(CH_EN_REG)]) |
            ((value & tempChEn))) & 0xFF;
        if (s->Regs[OFS2R(DMA_CFG_REG)] & 0x1)
          for (i=0; i<8; i++)
            if((s->Regs[OFS2R(CH_EN_REG)] >> i) & 0x1)
              dmaRun(s, i);
        break;
      default:
//        qemu_log_mask(LOG_GUEST_ERROR,
//            "ERROR: dmac write at offset=0x%llx. value=0x%llx size=0x%x\n",
//            (long long unsigned int)offset, (long long unsigned int)value, size);
        break;
      }
    }
  }
}

static const MemoryRegionOps dw_apb_dmac_ops = {
    .read = dw_apb_dmac_read,
    .write = dw_apb_dmac_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
};

/*
 * Set default values to timer fields and registers
 */
static void dw_apb_dmac_reset(DeviceState *d)
{
  DwApbDmacState *s = DW_APB_DMAC(d);
  int i;
  int j;


  for (i = 0; i < 8; i++)
  {
    s->channel[i].father = s;
    s->channel[i].id = i;
    for(j=0; j<11; j++)
      s->channel[i].chnRegs[j] = 0;
  }

  for (i=0; i<40; i++)
    s->Regs[i] = 0;
}

static int dw_apb_dmac_init(SysBusDevice *dev)
{
    DwApbDmacState *s = DW_APB_DMAC(dev);
    int i;
    int j;


    for (i = 0; i < 8; i++)
    {
      s->channel[i].timer = timer_new_ns(QEMU_CLOCK_VIRTUAL, dmaStart, &s->channel[i]);
      s->channel[i].father = s;
      s->channel[i].id = i;
      for(j=0; j<11; j++)
        s->channel[i].chnRegs[j] = 0;
    }

    for (i=0; i<40; i++)
      s->Regs[i] = 0;

    memory_region_init_io(&s->iomem, OBJECT(s), &dw_apb_dmac_ops, s, "dw_apb_dmac", 0x100000);
    sysbus_init_mmio(dev, &s->iomem);
    sysbus_init_irq(dev, &s->irq);

    return 0;
}

static void dw_apb_dmac_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    SysBusDeviceClass *k = SYS_BUS_DEVICE_CLASS(klass);

    k->init = dw_apb_dmac_init;
    dc->reset = dw_apb_dmac_reset;
    dc->vmsd = &vmstate_dw_apb_dmac_state;
}

static const TypeInfo dw_apb_dmac_info = {
    .name          = TYPE_DW_APB_DMAC,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(DwApbDmacState),
    .class_init    = dw_apb_dmac_class_init,
};

static void dw_apb_dmac_register_types(void)
{
    type_register_static(&dw_apb_dmac_info);
}

type_init(dw_apb_dmac_register_types);

