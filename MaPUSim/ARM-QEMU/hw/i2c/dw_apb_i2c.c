#include "qemu/timer.h"
#include "hw/sysbus.h"
#include "hw/i2c/i2c.h"
#include "qemu/fifo8.h"

#define DW_I2C_DEBUG

#ifdef DW_I2C_DEBUG
#define DPRINT(fmt, args...)              \
    do { fprintf(stderr, "DW I2C: "fmt, ## args); } while (0)
#else
# define DPRINTF(format, ...)     do { } while (0)
#endif

#define TYPE_DW_I2C                  "dw_i2c"
#define DW_I2C(obj)                  \
    OBJECT_CHECK(DWI2CState, (obj), TYPE_DW_I2C)


typedef struct DWI2CState {
    SysBusDevice parent_obj;

    MemoryRegion iomem;
    I2CBus *bus;
    qemu_irq irq;

    Fifo8 rx_fifo;
    Fifo8 tx_fifo;
    uint32_t ic_con;
	uint32_t ic_tar;
	uint32_t ic_sar;
	uint32_t ic_hs_maddr;
	uint32_t ic_cmd_data;
	uint32_t ic_ss_scl_hcnt;
	uint32_t ic_ss_scl_lcnt;
	uint32_t ic_fs_scl_hcnt;
	uint32_t ic_fs_scl_lcnt;
	uint32_t ic_hs_scl_hcnt;
	uint32_t ic_hs_scl_lcnt;
	uint32_t ic_intr_stat;
	uint32_t ic_intr_mask;
	uint32_t ic_raw_intr_stat;
	uint32_t ic_rx_tl;
	uint32_t ic_tx_tl;
	uint32_t ic_clr_intr;
	uint32_t ic_clr_rx_under;
	uint32_t ic_clr_rx_over;
	uint32_t ic_clr_tx_over;
	uint32_t ic_clr_rd_req;
	uint32_t ic_clr_tx_abrt;
	uint32_t ic_clr_rx_done;
	uint32_t ic_clr_activity;
	uint32_t ic_clr_stop_det;
	uint32_t ic_clr_start_det;
	uint32_t ic_clr_gen_call;
	uint32_t ic_enable;
	uint32_t ic_status;
	uint32_t ic_txflr;
	uint32_t ix_rxflr;
	uint32_t ic_sda_hold;
	uint32_t ic_slv_data_nack_only;
	uint32_t ic_sda_setup;
	uint32_t reserved_1;
	uint32_t ic_tx_abrt_source;

} DWI2CState;

static inline void dw_i2c_raise_interrupt(DWI2CState *s)
{
	if (fifo8_num_used(&s->rx_fifo) >= s->ic_rx_tl)
	    s->ic_raw_intr_stat &= 0xfef;
	if (fifo8_num_used(&s->tx_fifo )<= s->ic_tx_tl)
		s->ic_raw_intr_stat &= 0xf7;
	s->ic_intr_stat=s->ic_raw_intr_stat & s->ic_intr_mask;
	if(s->ic_intr_stat)
	        qemu_set_irq(s->irq, 1);
	    else
	        qemu_set_irq(s->irq, 0);
}

static void dw_i2c_data_receive(void *opaque)
{
    DWI2CState *s = (DWI2CState *)opaque;
    int ret;
    if (s->ic_enable &&(s->ic_cmd_data&0x100)){
    	ret = i2c_recv(s->bus);
    	fifo8_push(&s->rx_fifo,ret);
    }
    dw_i2c_raise_interrupt(s);
}

static void dw_i2c_data_send(void *opaque)
{
    DWI2CState *s = (DWI2CState *)opaque;
    while(fifo8_num_used(&s->tx_fifo))
    	i2c_send(s->bus,fifo8_pop(&s->tx_fifo));
    dw_i2c_raise_interrupt(s);
    if (!(s->ic_cmd_data&0x200))
    	while(s->ic_enable){
    		while(fifo8_num_used(&s->tx_fifo))
    		    	i2c_send(s->bus,fifo8_pop(&s->tx_fifo));
    		dw_i2c_raise_interrupt(s);
    	}

}

static uint64_t dw_i2c_read(void *opaque, hwaddr offset,
                                 unsigned size)
{
    DWI2CState *s = (DWI2CState *)opaque;
    uint8_t value;

    switch (offset) {
    case 0:
        value = s->ic_con;
        break;
    case 4:
    	value = s->ic_tar;
    	break;
    case 8:
       	value = s->ic_sar;
       	break;
    case 0x0c:
        value = s->ic_hs_maddr;
        break;
    case 0x10:
        value =fifo8_pop(&s->tx_fifo);
        dw_i2c_raise_interrupt(s);
        break;
    case 0x14:
        value = s->ic_ss_scl_hcnt;
        break;
    case 0x18:
        value = s->ic_ss_scl_lcnt;
        break;
    case 0x1c:
        value = s->ic_fs_scl_hcnt;
        break;
    case 0x20:
        value = s->ic_fs_scl_lcnt;
        break;
    case 0x24:
        value = s->ic_hs_scl_hcnt;
        break;
    case 0x28:
        value = s->ic_hs_scl_lcnt;
        break;
    case 0x2c:
        value = s->ic_intr_stat;
        break;
    case 0x30:
        value = s->ic_intr_mask;
        break;
    case 0x34:
        value = s->ic_raw_intr_stat;
        break;
    case 0x38:
        value = s->ic_rx_tl;
        break;
    case 0x3c:
        value = s->ic_tx_tl;
        break;
    case 0x40:
        s->ic_intr_stat=0;
        s->ic_raw_intr_stat=0;
        dw_i2c_raise_interrupt(s);
        value=0;
        break;
    case 0x44:
        s->ic_raw_intr_stat &= 0xfffe;
        s->ic_intr_stat=s->ic_raw_intr_stat & s->ic_intr_mask;
        dw_i2c_raise_interrupt(s);
        value=0;
        break;
    case 0x48:
        s->ic_raw_intr_stat &= 0xfffd;
        s->ic_intr_stat=s->ic_raw_intr_stat & s->ic_intr_mask;
        dw_i2c_raise_interrupt(s);
        value=0;
        break;
    case 0x4c:
        s->ic_raw_intr_stat &= 0xfff7;
        s->ic_intr_stat=s->ic_raw_intr_stat & s->ic_intr_mask;
        dw_i2c_raise_interrupt(s);
        value=0;
        break;
    case 0x50:
        s->ic_raw_intr_stat &= 0xffdf;
        s->ic_intr_stat=s->ic_raw_intr_stat & s->ic_intr_mask;
        dw_i2c_raise_interrupt(s);
        value=0;
        break;
    case 0x54:
        s->ic_raw_intr_stat &= 0xffbf;
        s->ic_intr_stat=s->ic_raw_intr_stat & s->ic_intr_mask;
        dw_i2c_raise_interrupt(s);
        value=0;
        break;
    case 0x58:
        s->ic_raw_intr_stat &= 0xff7f;
        s->ic_intr_stat=s->ic_raw_intr_stat & s->ic_intr_mask;
        dw_i2c_raise_interrupt(s);
        value=0;
        break;
    case 0x5c:
        value=s->ic_intr_stat&0x100;
        break;
    case 0x60:
        s->ic_raw_intr_stat &= 0xfdff;
        s->ic_intr_stat=s->ic_raw_intr_stat & s->ic_intr_mask;
        dw_i2c_raise_interrupt(s);
        value=0;
        break;
    case 0x64:
        s->ic_raw_intr_stat &= 0xfbff;
        s->ic_intr_stat=s->ic_raw_intr_stat & s->ic_intr_mask;
        dw_i2c_raise_interrupt(s);
        value=0;
        break;
    case 0x68:
        s->ic_raw_intr_stat &= 0xf7ff;
        s->ic_intr_stat=s->ic_raw_intr_stat & s->ic_intr_mask;
        dw_i2c_raise_interrupt(s);
        value=0;
        break;
    case 0x6c:
        value = s->ic_enable;
        break;
    case 0x70:
    	if (fifo8_num_used(&s->rx_fifo)==8)
    		s->ic_status|=0x10;
    	else
    		s->ic_status&=0xef;
    	if (fifo8_num_used(&s->rx_fifo)>0)
    	    s->ic_status|=0x8;
    	else
    		s->ic_status&=0xf7;
    	if (fifo8_num_used(&s->tx_fifo)==0)
    	    s->ic_status|=0x4;
    	else
    	    s->ic_status&=0xfb;
    	if (fifo8_num_used(&s->tx_fifo)<8)
    	    s->ic_status|=0x2;
    	else
    	    s->ic_status&=0xfd;
    	if (s->ic_raw_intr_stat & 0x100)
    	    s->ic_status|=0x1;
    	else
    	    s->ic_status&=0xfe;
        value = s->ic_status;
        break;
    case 0x74:
        value = fifo8_num_used(&s->tx_fifo);
        break;
    case 0x78:
        value = fifo8_num_used(&s->rx_fifo);
        break;
    case 0x7c:
        value = s->ic_sda_hold;
        break;
    case 0x84:
        value = s->ic_slv_data_nack_only;
        break;
    case 0x94:
        value = s->ic_sda_setup;
        break;
    case 0xf4:
        value = 0xffffffff;
        break;
    case 0xf8:
        value = 0xffffffff;
        break;
    case 0xfc:
        value = 0x44570140;
        break;
    default:
        value = 0;
        if(offset>0xa4 || offset%4)   //sometimes it's read-only 0 in register
        	DPRINT("ERROR: Bad read offset 0x%x\n", (unsigned int)offset);
        break;
    }
    DPRINT("read: offset=%#x, value=%#x\n", offset, value);
    return value;
}

static void dw_i2c_write(void *opaque, hwaddr offset,
                              uint64_t value, unsigned size)
{
    DWI2CState *s = (DWI2CState *)opaque;

    DPRINT("write: offset=%#x, value=%#x\n", offset, value);
    switch (offset) {
        case 0:
            if (! s->ic_enable)
            	s->ic_con=value;
            break;
        case 4:
        	if (! s->ic_enable)
        		s->ic_tar=value;
        	break;
        case 8:
        	if (! s->ic_enable)
        		s->ic_sar=value;
           	break;
        case 0x0c:
        	if (! s->ic_enable)
        		s->ic_hs_maddr=value;
            break;
        case 0x10:
             s->ic_cmd_data=value;
             if (s->ic_cmd_data&0x100){
            	 dw_i2c_data_receive(s);
             }
             else{
            	 if(s->ic_enable){
            		 fifo8_push(&s->tx_fifo,value);
            		 dw_i2c_data_send(s);
            	 }
             }
            break;
        case 0x14:
        	if (! s->ic_enable)
        		s->ic_ss_scl_hcnt=value;
            break;
        case 0x18:
        	if (! s->ic_enable)
        		s->ic_ss_scl_lcnt=value;
            break;
        case 0x1c:
        	if (! s->ic_enable)
        		s->ic_fs_scl_hcnt=value;
            break;
        case 0x20:
        	if (! s->ic_enable)
        		s->ic_fs_scl_lcnt = value;
            break;
        case 0x24:
        	if (! s->ic_enable)
        		s->ic_hs_scl_hcnt = value;
            break;
        case 0x28:
        	if (! s->ic_enable)
        		s->ic_hs_scl_lcnt = value;
            break;
        case 0x30:
            s->ic_intr_mask = value;
            break;
        case 0x38:
            s->ic_rx_tl = value;
            dw_i2c_raise_interrupt(s);
            break;
        case 0x3c:
            s->ic_tx_tl = value;
            dw_i2c_raise_interrupt(s);
            break;
        case 0x6c:
            s->ic_enable = value;
            if (!s->ic_enable ){
            	s->ic_raw_intr_stat &=0xeff;
            	s->ic_cmd_data=0;
            	fifo8_reset(&s->tx_fifo);
            }
            else{
            	if (s->ic_cmd_data&0x100){
            		dw_i2c_data_receive(s);
            	}
            }
            s->ic_intr_stat=s->ic_raw_intr_stat & s->ic_intr_mask;
            dw_i2c_raise_interrupt(s);
            break;
        case 0x70:
            s->ic_status = value;
            break;
        case 0x7c:
            s->ic_sda_hold = value;
            break;
        case 0x84:
            s->ic_slv_data_nack_only = value;
            break;
        case 0x94:
            s->ic_sda_setup = value;
            break;
        default:
        	DPRINT("ERROR: Bad write offset 0x%x\n", (unsigned int)offset);
            break;
        }
}

static const MemoryRegionOps dw_i2c_ops = {
    .read = dw_i2c_read,
    .write = dw_i2c_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
};

static const VMStateDescription dw_i2c_vmstate = {
    .name = "dw_i2c",
    .version_id = 1,
    .minimum_version_id = 1,
    .fields = (VMStateField[]) {
    	VMSTATE_UINT32(ic_con, DWI2CState),
    	VMSTATE_UINT32(ic_tar, DWI2CState),
		VMSTATE_UINT32(ic_sar, DWI2CState),
		VMSTATE_UINT32(ic_hs_maddr, DWI2CState),
		VMSTATE_UINT32(ic_cmd_data, DWI2CState),
		VMSTATE_UINT32(ic_ss_scl_hcnt, DWI2CState),
		VMSTATE_UINT32(ic_ss_scl_lcnt, DWI2CState),
		VMSTATE_UINT32(ic_fs_scl_hcnt, DWI2CState),
		VMSTATE_UINT32(ic_fs_scl_lcnt, DWI2CState),
		VMSTATE_UINT32(ic_hs_scl_hcnt, DWI2CState),
		VMSTATE_UINT32(ic_hs_scl_lcnt, DWI2CState),
		VMSTATE_UINT32(ic_intr_stat, DWI2CState),
		VMSTATE_UINT32(ic_intr_mask, DWI2CState),
		VMSTATE_UINT32(ic_raw_intr_stat, DWI2CState),
		VMSTATE_UINT32(ic_rx_tl, DWI2CState),
		VMSTATE_UINT32(ic_tx_tl, DWI2CState),
		VMSTATE_UINT32(ic_clr_intr, DWI2CState),
		VMSTATE_UINT32(ic_clr_rx_under, DWI2CState),
		VMSTATE_UINT32(ic_clr_rx_over, DWI2CState),
		VMSTATE_UINT32(ic_clr_tx_over, DWI2CState),
		VMSTATE_UINT32(ic_clr_rd_req , DWI2CState),
		VMSTATE_UINT32(ic_clr_tx_abrt , DWI2CState),
		VMSTATE_UINT32(ic_clr_rx_done , DWI2CState),
		VMSTATE_UINT32(ic_clr_activity , DWI2CState),
		VMSTATE_UINT32(ic_clr_stop_det , DWI2CState),
		VMSTATE_UINT32(ic_clr_start_det , DWI2CState),
		VMSTATE_UINT32(ic_clr_gen_call , DWI2CState),
		VMSTATE_UINT32(ic_enable , DWI2CState),
		VMSTATE_UINT32(ic_status , DWI2CState),
		VMSTATE_UINT32(ic_txflr , DWI2CState),
		VMSTATE_UINT32(ix_rxflr , DWI2CState),
		VMSTATE_UINT32(ic_sda_hold , DWI2CState),
		VMSTATE_UINT32(ic_slv_data_nack_only , DWI2CState),
		VMSTATE_UINT32(ic_sda_setup , DWI2CState),
		VMSTATE_UINT32(reserved_1 , DWI2CState),
		VMSTATE_UINT32(ic_tx_abrt_source, DWI2CState),
    	VMSTATE_FIFO8(rx_fifo,DWI2CState),
    	VMSTATE_FIFO8(tx_fifo,DWI2CState),
        VMSTATE_END_OF_LIST()
    }
};
/*
 * reset for 100MHz
*/
static void dw_i2c_reset(DeviceState *d)
{
    DWI2CState *s = DW_I2C(d);
    s->ic_con= 0xff;
    s->ic_tar=0x055;
    s->ic_sar=0x055;
    s->ic_hs_maddr=1;
    s->ic_cmd_data=0;
    s->ic_ss_scl_hcnt=0x003c;
    s->ic_ss_scl_lcnt=0x01d6;
    s->ic_fs_scl_hcnt=0x003c;
    s->ic_fs_scl_lcnt=0x0082;
    s->ic_hs_scl_hcnt=0x006;
    s->ic_hs_scl_lcnt=0x0010;
    s->ic_intr_stat=0;
    s->ic_intr_mask=0x8ff;
    s->ic_raw_intr_stat=0;
    s->ic_rx_tl=0;
    s->ic_tx_tl=0;
    s->ic_enable=0;
    s->ic_status=6;
    s->ic_sda_hold=1;
    s->ic_slv_data_nack_only=0;
    s->ic_sda_setup=0x64;
    fifo8_reset(&s->rx_fifo);
    fifo8_reset(&s->tx_fifo);
}

static int dw_i2c_realize(SysBusDevice *sbd)
{
    DeviceState *dev = DEVICE(sbd);
    DWI2CState *s = DW_I2C(dev);

    memory_region_init_io(&s->iomem, OBJECT(s), &dw_i2c_ops, s,
                          TYPE_DW_I2C, 0);
    sysbus_init_mmio(sbd, &s->iomem);
    sysbus_init_irq(sbd, &s->irq);
    s->bus = i2c_init_bus(dev, "i2c");
    dw_i2c_reset(s);
    fifo8_create(&s->tx_fifo, 8);
    fifo8_create(&s->rx_fifo, 8);
    return 0;
}

static void dw_i2c_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    SysBusDeviceClass *sbdc = SYS_BUS_DEVICE_CLASS(klass);

    dc->vmsd = &dw_i2c_vmstate;
    dc->reset = dw_i2c_reset;
    sbdc->init = dw_i2c_realize;
}

static const TypeInfo dw_i2c_type_info = {
    .name = TYPE_DW_I2C,
    .parent = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(DWI2CState),
    .class_init = dw_i2c_class_init,
};

static void dw_i2c_register_types(void)
{
    type_register_static(&dw_i2c_type_info);
}

type_init(dw_i2c_register_types)
