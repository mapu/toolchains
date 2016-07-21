/*
**   this is a dw_apb_ssi master device **
**               by zhenglt at 20160708 **
*/
#include "hw/sysbus.h"
#include "hw/ssi.h"
#include "qemu/fifo8.h"

#define TYPE_DWSSI "dwssi"
#define FIFO_CAPACITY 8
#define DWSSI(obj) OBJECT_CHECK(DWSSIState, (obj), TYPE_DWSSI)

//#define DEBUG_DW_SSI
#ifdef DEBUG_DW_SSI
#define DPRINTF(fmt, ...) \
do { fprintf(stderr, "dw_ssi: " fmt , ## __VA_ARGS__); } while (0)
#else
#define DPRINTF(fmt, ...) \
do {} while (0)
#endif

typedef struct DWSSIState {
    SysBusDevice parent_obj;

    MemoryRegion iomem;
    uint32_t cr0;
    uint32_t cr1;
    uint32_t sen;
    uint32_t mwcr;
    uint32_t ser;
    uint32_t baudr;
    uint32_t txftlr;
    uint32_t rxftlr;
    uint32_t sr;
    uint32_t imr;
    uint32_t isr;
    uint32_t risr;
    uint32_t txoicr;
    uint32_t rxoicr;
    uint32_t rxuicr;
    uint32_t msticr;
    uint32_t icr;
    uint32_t dr;
    Fifo8 rx_fifo;
    Fifo8 tx_fifo;
    qemu_irq irq;
    SSIBus *ssi;
} DWSSIState;
/*
**   only one irq are connected to the cpu, thus the driver should  **
**   resd s->isr to know what hapend right now.                     **
*/
static void updateirq(DWSSIState *s)
{
    if (fifo8_num_used(&s->rx_fifo) <= 8)
        s->risr &= 0xf7;
    if (fifo8_num_used(&s->rx_fifo )> 0)
        s->risr &= 0xfb;
    if (fifo8_num_used(&s->rx_fifo )< s->rxftlr)
        s->risr &= 0xef;
    if (fifo8_num_used(&s->tx_fifo )<= 8)
        s->risr &= 0xfd;
    if (fifo8_num_used(&s->tx_fifo )> s->txftlr)
        s->risr &= 0xfe;
    s->isr=s->risr & s->imr;
    if(s->isr)
        qemu_set_irq(s->irq, 1);
    else
        qemu_set_irq(s->irq, 0);
}
static void dw_xfer(DWSSIState *s)
{
    int val;
    int fl=0;
    if (s->sen && s->ser && !fifo8_is_empty(&s->tx_fifo))
    {
        switch ((s->cr0 & 0x30)>>4){
        case 0 :
        case 1 :/*spi,ssp*/
            switch ((s->cr0 & 0x300)>>8){
            case 0 :/*t&r*/
                fl=1;
            case 1 : /*to*/           
                while (fifo8_num_used(&s->tx_fifo) > 0){
                    val = ssi_transfer(s->ssi, fifo8_pop(&s->tx_fifo));
                    if(fl)
                        fifo8_push(&s->rx_fifo,val);
                    if(fifo8_num_used(&s->tx_fifo) <= s->txftlr)
                        {s->risr |= 1;s->isr=s->risr & s->imr;updateirq(s);}
                }
                fl=0;
                break;
            case 2 :/*ro*/  
                while (fifo8_num_used(&s->rx_fifo) <= s->cr1+1){
                    fifo8_push(&s->tx_fifo,0);
                    val = ssi_transfer(s->ssi, fifo8_pop(&s->tx_fifo));
                    fifo8_push(&s->rx_fifo,val);
                    if(fifo8_num_used(&s->rx_fifo) >= s->rxftlr)
                        {s->risr |= 0x10;s->isr=s->risr & s->imr;updateirq(s);}
                    if(fifo8_num_used(&s->rx_fifo)> 8)
                        {s->risr |= 0x8;s->isr=s->risr & s->imr;updateirq(s);}
                }
                break;
            case 3 :/*eeprom*/  
                while (fifo8_num_used(&s->tx_fifo) > 0){
                    val = ssi_transfer(s->ssi, fifo8_pop(&s->tx_fifo));
                    if(fifo8_num_used(&s->tx_fifo)<= s->txftlr)
                        {s->risr |= 1;s->isr=s->risr & s->imr;updateirq(s);}
                }
                while (fifo8_num_used(&s->rx_fifo) <= s->cr1+1){
                    fifo8_push(&s->tx_fifo,0);
                    val = ssi_transfer(s->ssi, fifo8_pop(&s->tx_fifo));
                    fifo8_push(&s->rx_fifo,val);
                    if(fifo8_num_used(&s->rx_fifo)>= s->rxftlr)
                        {s->risr |= 0x10;s->isr=s->risr & s->imr;updateirq(s);}
                    if(fifo8_num_used(&s->rx_fifo) > 8)
                        {s->risr |= 0x8;s->isr=s->risr & s->imr;updateirq(s);}
                }
                break;
            }
            break;
        case 2 :/* microwire*/
            if (!(s->mwcr&2)){
            val = ssi_transfer(s->ssi, fifo8_pop(&s->tx_fifo));
            fifo8_push(&s->rx_fifo,val);
            while (fifo8_num_used(&s->rx_fifo) <= s->cr1+1){
                    if(s->mwcr&1)
                        fifo8_push(&s->tx_fifo,0);
                    else
                        ;
                    
                    val = ssi_transfer(s->ssi, fifo8_pop(&s->tx_fifo));
                    fifo8_push(&s->rx_fifo,val);
                    if(fifo8_num_used(&s->rx_fifo)>= s->rxftlr)
                        {s->risr |= 0x10;s->isr=s->risr & s->imr;updateirq(s);}
                    if(fifo8_num_used(&s->rx_fifo)> 8)
                        {s->risr |= 0x8;s->isr=s->risr & s->imr;updateirq(s);}
                }
            }
            else{
                while (fifo8_num_used(&s->rx_fifo)<= s->cr1+1){
                    val = ssi_transfer(s->ssi, fifo8_pop(&s->tx_fifo));
                    if(fifo8_num_used(&s->rx_fifo) >= s->rxftlr)
                        {s->risr |= 0x10;s->isr=s->risr & s->imr;updateirq(s);}
                    if(fifo8_num_used(&s->rx_fifo) > 8)
                        {s->risr |= 0x8;s->isr=s->risr & s->imr;updateirq(s);}
                }
            }
            break;
        }
    }
    return 0;
}
static uint64_t dwssi_read(void *opaque, hwaddr offset,
                           unsigned size)
{
    DWSSIState *s = (DWSSIState *)opaque;
    uint64_t val;

    DPRINTF("Reading at offset: %#x\n", offset);

    if (offset>=0x60)
        if (offset<=0xec)
            offset=0x60;
    switch (offset) {
    case 0x00: /* CR0 */
        return s->cr0;
       
    case 0x04: /* CR1 */
        return s->cr0;
        
    case 0x08: /* SSI_EN */
        return s->sen;
       
    case 0x0c: /* mwcr */
        return s->mwcr;
        
    case 0x10: /* ser */
        return s->ser;
        
    case 0x14: /* baudr */
        return s->baudr;
        
    case 0x18: /* txftlr */
        return s->txftlr;
        
    case 0x1c: /* rxftlr */
        return s->rxftlr;
        
    case 0x20: /* txflr */
        return fifo8_num_used(&s->tx_fifo);
        
    case 0x24: /* rxflr */
        return fifo8_num_used(&s->rx_fifo);
        
    case 0x28: /* sr */
        return s->sr;
        
    case 0x2c: /* imr */
        return s->imr;
        
    case 0x30: /* isr */
        return s->isr;
        
    case 0x34: /* risr */
        return s->risr;
        
    case 0x38: /* txoicr */
        s->risr &=0xfffffffd;
        s->isr=s->risr & s->imr;
        updateirq(s);
        return s->txoicr;
        
    case 0x3c: /* rxoicr */
        s->risr &=0xfffffff7;
        s->isr=s->risr & s->imr;
        updateirq(s);
        return s->txoicr;
        
    case 0x40: /* rxuicr */
        s->risr &=0xfffffffb;
        s->isr=s->risr & s->imr;
        updateirq(s);
        return s->txoicr;
       
    case 0x44: /* msticr */
        s->risr &=0xffffffdf;
        s->isr=s->risr & s->imr;
        updateirq(s);
        return s->txoicr;
        
    case 0x48: /* icr */
        s->risr &=0xffffffd1;
        s->isr=s->risr & s->imr;
        updateirq(s);
        return s->txoicr;
        
    case 0x58: /* idr */
        return 0xffffffff;
        
    case 0x60: /* DR */
        if(fifo8_is_empty(&s->rx_fifo)){
            s->risr |= 0x4;
            s->isr=s->risr & s->imr;
            updateirq(s);
            return 0;
        }
        if(fifo8_num_used(&s->rx_fifo)<=s->rxftlr){
            s->risr &= 0xef;
            s->isr=s->risr & s->imr;
            updateirq(s);            
        }
        val=fifo8_pop(&s->rx_fifo);
        updateirq(s);
        return val;
    default:    
        qemu_log_mask(LOG_GUEST_ERROR,
                      "dwssi_write: Bad offset %x\n", (int)offset);
    }
}

static void dwssi_write(void *opaque, hwaddr offset,
                        uint64_t value, unsigned size)
{
    DWSSIState *s = (DWSSIState *)opaque;

    DPRINTF("Writing value: %#x at offset: %#x\n", value, offset);

    if (offset>=0x60)
        if (offset<=0xec)
            offset=0x60;
    switch (offset) {
    case 0x00: /* CR0 */
        s->cr0=value;
        break;
    case 0x04: /* CR1 */
        s->cr1=value;
        break;
    case 0x08: /* SSI_EN  */
        s->sen=value;
        if (!(s->sen)){
            fifo8_reset(&s->rx_fifo);
            fifo8_reset(&s->tx_fifo);
            s->txftlr=0;
            s->rxftlr=0;
        }
        break;
    case 0x0c: /* mwcr */
        if (!s->sen)
        s->mwcr=value;
        break;
    case 0x10: /* ser */
        if (!(s->sr & 1))
            {s->ser=value;dw_xfer(s);}
        break;
    case 0x14: /* baudr */
        s->baudr=value;
        break;
    case 0x18: /* txftlr */
        if(value<8)
        s->txftlr=value;
        break;
    case 0x1c: /* rxftlr */
        if(value<8)
        s->rxftlr=value;
        break;
    case 0x2c: /* imr*/
        s->imr=value;
        break;
    case 0x60: /* DR */
        if(fifo8_is_full(&s->tx_fifo)){
            s->risr |= 0x2;
            s->isr=s->risr & s->imr;
            updateirq(s);
            return 0;
        }
        if(fifo8_num_used(&s->tx_fifo) >= s->txftlr){
            s->risr &= 0xfe;
            s->isr=s->risr & s->imr;
            updateirq(s);
        }
        fifo8_push(&s->tx_fifo,value);        
        dw_xfer(s);
        updateirq(s);
        break;
    
    default:
            qemu_log_mask(LOG_GUEST_ERROR,
                      "dwssi_write: Bad offset %x\n", (int)offset);
    }
}

static void dwssi_reset(DWSSIState *s)
{
    fifo8_reset(&s->rx_fifo);
    fifo8_reset(&s->tx_fifo);
    s->isr=0;
    s->imr=0x37;
    s->risr=0;
    s->cr0=7;
    s->cr1=0;
    s->sen=0;
    s->ser=0;
    s->baudr=0;
    s->txftlr=0;
    s->rxftlr=0;
    s->dr=0;    
}
static const MemoryRegionOps dwssi_ops = {
    .read = dwssi_read,
    .write = dwssi_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
};
static const VMStateDescription vmstate_dwssi = {
    .name = "dwssi_ssp",
    .version_id = 1,
    .minimum_version_id = 1,    
    .fields = (VMStateField[]) {
        VMSTATE_UINT32(cr0, DWSSIState),
        VMSTATE_UINT32(cr1, DWSSIState),
        VMSTATE_UINT32(sen, DWSSIState),
        VMSTATE_UINT32(mwcr, DWSSIState),
        VMSTATE_UINT32(ser, DWSSIState),
        VMSTATE_UINT32(baudr, DWSSIState),
        VMSTATE_UINT32(txftlr, DWSSIState),
        VMSTATE_UINT32(rxftlr, DWSSIState),
        VMSTATE_UINT32(sr, DWSSIState),
        VMSTATE_UINT32(imr, DWSSIState),
        VMSTATE_UINT32(isr, DWSSIState),
        VMSTATE_UINT32(risr, DWSSIState),
        VMSTATE_UINT32(txoicr, DWSSIState),
        VMSTATE_UINT32(rxoicr, DWSSIState),
        VMSTATE_UINT32(rxuicr, DWSSIState),
        VMSTATE_UINT32(msticr, DWSSIState),
        VMSTATE_UINT32(icr, DWSSIState),
        VMSTATE_UINT32(dr, DWSSIState),
        VMSTATE_FIFO8(rx_fifo,DWSSIState),
        VMSTATE_FIFO8(tx_fifo,DWSSIState),
        VMSTATE_END_OF_LIST()
    }
};
static int dwssi_init(SysBusDevice *sbd)
{
    DeviceState *dev = DEVICE(sbd);
    DWSSIState *s = DWSSI(dev);

    memory_region_init_io(&s->iomem, OBJECT(s), &dwssi_ops, s, "dwssi", 0x1000);
    sysbus_init_mmio(sbd, &s->iomem);
    sysbus_init_irq(sbd, &s->irq);
    s->ssi = ssi_create_bus(dev, "ssi");
    fifo8_create(&s->tx_fifo, FIFO_CAPACITY);
    fifo8_create(&s->rx_fifo, FIFO_CAPACITY);
    dwssi_reset(s);
    vmstate_register(dev, -1, &vmstate_dwssi, s);
    return 0;
}

static void dwssi_class_init(ObjectClass *klass, void *data)
{
    SysBusDeviceClass *sdc = SYS_BUS_DEVICE_CLASS(klass);
    sdc->init = dwssi_init;
}

static const TypeInfo dwssi_info = {
    .name          = TYPE_DWSSI,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(DWSSIState),
    .class_init    = dwssi_class_init,
};

static void dwssi_register_types(void)
{
    type_register_static(&dwssi_info);
}

type_init(dwssi_register_types)
