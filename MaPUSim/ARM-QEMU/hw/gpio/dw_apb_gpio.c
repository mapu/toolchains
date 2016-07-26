#include "hw/sysbus.h"

#ifdef DEBUG_DW_GPIO
#define DPRINTF(fmt, ...) \
do { printf("dw_gpio: " fmt , ## __VA_ARGS__); } while (0)
#define BADF(fmt, ...) \
do { fprintf(stderr, "dw_gpio: error: " fmt , ## __VA_ARGS__); exit(1);} while (0)
#else
#define DPRINTF(fmt, ...) do {} while(0)
#define BADF(fmt, ...) \
do { fprintf(stderr, "dw_gpio: error: " fmt , ## __VA_ARGS__);} while (0)
#endif

#define TYPE_DW_GPIO "dw_gpio"
#define DW_GPIO(obj) OBJECT_CHECK(DW_GPIOState, (obj), TYPE_DW_GPIO)

typedef struct DW_GPIOState {
    SysBusDevice parent_obj;
    MemoryRegion iomem;
    qemu_irq irq;
    qemu_irq out[32];
    uint32_t adr;
    uint32_t addr;
    uint32_t actl;
    uint32_t bdr;
    uint32_t bddr;
    uint32_t bctl;
    uint32_t cdr;
    uint32_t cddr;
    uint32_t cctl;
    uint32_t ddr;
    uint32_t dddr;
    uint32_t dctl;
    uint32_t inten;
    uint32_t intmask;
    uint32_t inttype;
    uint32_t intpo;
    uint32_t intst;
    uint32_t intrst;
    uint32_t intdeb;
    uint32_t eporta;
    uint32_t eportb;
    uint32_t eportc;
    uint32_t eportd;
} DW_GPIOState;

static const VMStateDescription vmstate_dw_gpio = {
    .name = "dw_gpio",
    .version_id = 3,
    .minimum_version_id = 3,
    .fields = (VMStateField[]) {
        VMSTATE_UINT32(adr, DW_GPIOState),
        VMSTATE_UINT32(addr, DW_GPIOState),
        VMSTATE_UINT32(actl, DW_GPIOState),
        VMSTATE_UINT32(bdr, DW_GPIOState),
        VMSTATE_UINT32(bddr, DW_GPIOState),
        VMSTATE_UINT32(bctl, DW_GPIOState),
        VMSTATE_UINT32(cdr, DW_GPIOState),
        VMSTATE_UINT32(cddr, DW_GPIOState),
        VMSTATE_UINT32(cctl, DW_GPIOState),
        VMSTATE_UINT32(ddr, DW_GPIOState),
        VMSTATE_UINT32(dddr, DW_GPIOState),
        VMSTATE_UINT32(dctl, DW_GPIOState),
        VMSTATE_UINT32(inten, DW_GPIOState),
        VMSTATE_UINT32(intmask, DW_GPIOState),
        VMSTATE_UINT32(inttype, DW_GPIOState),
        VMSTATE_UINT32(intpo, DW_GPIOState),
        VMSTATE_UINT32(intst, DW_GPIOState),
        VMSTATE_UINT32(intrst, DW_GPIOState),
        VMSTATE_UINT32(intdeb, DW_GPIOState),
        VMSTATE_UINT32(eporta, DW_GPIOState),
        VMSTATE_UINT32(eportb, DW_GPIOState),
        VMSTATE_UINT32(eportc, DW_GPIOState),
        VMSTATE_UINT32(eportd, DW_GPIOState),
        VMSTATE_END_OF_LIST()
    }
};
/*
 * when there is an interrupt; intst should be read to
 * know what happened.
 */
static void dw_gpio_update(DW_GPIOState *s)
{
    s->intst=s->intrst & s->intmask;
	if (s->intst){
	qemu_set_irq(s->irq, 1);
    }
	else{
		qemu_set_irq(s->irq, 0);
	}
}

static uint64_t dw_gpio_read(void *opaque, hwaddr offset,
                           unsigned size)
{
    DW_GPIOState *s = (DW_GPIOState *)opaque;
    BADF("read: offset=%#x\n", offset);
    switch (offset) {
    case 0x0:
        return s->adr;
    case 0x4:
        return s->addr;
    case 0x8:
        return s->actl;
    case 0xc:
        return s->bdr;
    case 0x10:
        return s->bddr;
    case 0x14:
        return s->bctl;
    case 0x18:
        return s->cdr;
    case 0x1c:
        return s->cddr;
    case 0x20:
        return s->cctl;
    case 0x24:
        return s->ddr;
    case 0x28:
        return s->dddr;
    case 0x2c:
        return s->dctl;
    case 0x30:
        return s->inten;
    case 0x34:
        return s->intmask;
    case 0x38:
        return s->inttype;
    case 0x3c:
        return s->intpo;
    case 0x40:
        return s->intst;
    case 0x44:
        return s->intrst;
    case 0x48:
        return s->intdeb;
    case 0x50:
        return (s->eporta&(~s->addr))|(s->adr&(s->addr));
    case 0x54:
        return (s->eportb&(~s->bddr))|(s->bdr&(s->bddr));
    case 0x58:
        return (s->eportc&(~s->cddr))|(s->cdr&(s->cddr));
    case 0x5c:
        return (s->eportd&(~s->dddr))|(s->ddr&(s->dddr));
    case 0x64:
        return 5;
    case 0x6c:
        return 5;
    case 0x70:
        return 5;
    case 0x74:
        return 5;
    default:
        qemu_log_mask(LOG_GUEST_ERROR,
                      "dw_gpio_read: Bad offset %x\n", (int)offset);
        return 0;
    }
}

static void dw_gpio_write(void *opaque, hwaddr offset,
                        uint64_t value, unsigned size)
{
	DW_GPIOState *s = (DW_GPIOState *)opaque;

	    switch (offset) {
	    case 0x0:
	        s->adr=value;
	        break;
	    case 0x4:
	        s->addr=value;
	        break;
	    case 0x8:
	        s->actl=value;
	        break;
	    case 0xc:
	        s->bdr=value;
	        break;
	    case 0x10:
	        s->bddr=value;
	        break;
	    case 0x14:
	        s->bctl=value;
	        break;
	    case 0x18:
	        s->cdr=value;
	        break;
	    case 0x1c:
	        s->cddr=value;
	        break;
	    case 0x20:
	        s->cctl=value;
	        break;
	    case 0x24:
	        s->ddr=value;
	        break;
	    case 0x28:
	        s->dddr=value;
	        break;
	    case 0x2c:
	        s->dctl=value;
	        break;
	    case 0x30:
	        s->inten=value;
	        break;
	    case 0x34:
	        s->intmask=value;
	        break;
	    case 0x38:
	        s->inttype=value;
	        break;
	    case 0x3c:
	        s->intpo=value;
	        break;
	    case 0x40:
	        s->intst=value;
	        break;
	    case 0x44:
	        s->intrst=value;
	        break;
	    case 0x48:
	        s->intdeb=value;
	        break;
	    case 0x4c:
	    	if(value){
	    		s->intrst &= (value&0xff);
	    	}
	        break;
	    default:
	        qemu_log_mask(LOG_GUEST_ERROR,
	                      "dw_gpio_read: Bad offset %x\n", (int)offset);
	        break;
	    }
    dw_gpio_update(s);
    return 0;
}

static void dw_gpio_reset(DW_GPIOState *s)
{
	s->adr=0;
	s->addr=0;
	s->actl=0;
	s->bdr=0;
	s->bddr=0;
	s->bctl=0;
	s->cdr=0;
	s->cddr=0;
	s->cctl=0;
	s->ddr=0;
	s->dddr=0;
	s->dctl=0;
	s->inten=0;
	s->intrst=0;
	s->intst=0;
	s->intmask=0;
}

static void dw_gpio_set_irq(void * opaque, int irq, int level)
{
    DW_GPIOState *s = (DW_GPIOState *)opaque;
    uint8_t mask;
    if (s->inten && irq < 8){
        mask = 1 << irq;
        if ((s->addr & mask) == 0) {
            dw_gpio_update(s);
        }
    }
}

static const MemoryRegionOps dw_gpio_ops = {
    .read = dw_gpio_read,
    .write = dw_gpio_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
};

static int dw_gpio_init(SysBusDevice *sbd)
{
    DeviceState *dev = DEVICE(sbd);
    DW_GPIOState *s = DW_GPIO(dev);

    memory_region_init_io(&s->iomem, OBJECT(s), &dw_gpio_ops, s, "dw_gpio", 0x1000);
    sysbus_init_mmio(sbd, &s->iomem);
    sysbus_init_irq(sbd, &s->irq);
    qdev_init_gpio_in(dev, dw_gpio_set_irq, 32);
    qdev_init_gpio_out(dev, s->out, 32);
    dw_gpio_reset(s);
    return 0;
}
static void dw_gpio_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    SysBusDeviceClass *k = SYS_BUS_DEVICE_CLASS(klass);
    k->init = dw_gpio_init;
    dc->vmsd = &vmstate_dw_gpio;
}

static const TypeInfo dw_gpio_info = {
    .name          = TYPE_DW_GPIO,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(DW_GPIOState),
    .class_init    = dw_gpio_class_init,
};
static void dw_gpio_register_types(void)
{
    type_register_static(&dw_gpio_info);
}
type_init(dw_gpio_register_types)
