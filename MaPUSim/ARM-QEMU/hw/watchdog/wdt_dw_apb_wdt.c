
#include <inttypes.h>

#include "qemu-common.h"
#include "qemu/timer.h"
#include "sysemu/watchdog.h"
#include "hw/hw.h"
#include "hw/sysbus.h"
#include "qemu/main-loop.h"
#include "hw/ptimer.h"

#define WDT_CR     0x00
#define WDT_TORR   0x04
#define WDT_CCVR   0x08
#define WDT_CRR    0x0C
#define WDT_STAT   0x10
#define WDT_EOI    0x14
struct ptimer_state
{
    uint8_t enabled; /* 0 = disabled, 1 = periodic, 2 = oneshot.  */
    uint64_t limit;
    uint64_t delta;
    uint32_t period_frac;
    int64_t period;
    int64_t last_event;
    int64_t next_event;
    QEMUBH *bh;
    QEMUTimer *timer;
};
/* Device state. */
struct DW_APB_WDTState {
	SysBusDevice parent_obj;
	MemoryRegion iomem;
	uint32_t freq;           /* timer frequency */
	ptimer_state *ptimer;
	qemu_irq  irq;
	uint32_t  cr;
	uint32_t  to;
	uint32_t  crr;//write only
	uint32_t  stat;
};

typedef struct DW_APB_WDTState DW_APB_WDTState;

#define TYPE_WATCHDOG_DW_APB_WDT_DEVICE "dw_apb_wdt"
#define WATCHDOG_DW_APB_WDT_DEVICE(obj) \
    OBJECT_CHECK(DW_APB_WDTState, (obj), TYPE_WATCHDOG_DW_APB_WDT_DEVICE)



static const VMStateDescription vmstate_dw_apb_wdt= {
    .name = "dw_apb_wdt",
    .version_id = 1,
    .minimum_version_id = 1,
    .fields = (VMStateField[]) {
        VMSTATE_PTIMER(ptimer, DW_APB_WDTState),
        VMSTATE_UINT32(cr, DW_APB_WDTState),
        VMSTATE_UINT32(to, DW_APB_WDTState),
		VMSTATE_UINT32(crr, DW_APB_WDTState),
        VMSTATE_END_OF_LIST()
    }
};
static void dw_apb_wdt_expired(void *opaque)
{
	DW_APB_WDTState *s = (DW_APB_WDTState *)opaque;



    static int rmod=0;
    if (s->cr & 1)
    {
    	    	s->stat = 1;
    	    	if (s->cr&0b10)
    	    	{
    	    		if (rmod<2)
    	    		{
    	    			rmod++;
    	    			s->stat=1;
    	    			qemu_irq_raise(s->irq);
    	    		}
    	    		else
    	    		{
    	    			rmod=0;
    	    			select_watchdog_action("debug");//set to reset after debug
    	    			watchdog_perform_action();

    	    		}
    	    	}
    	    	else
    	    	{
    	    		select_watchdog_action("debug");//set to reset after debug
    	    		watchdog_perform_action();

    	    	}
    }
    else
    {
    	;
    }

    /* reload timer */
    ptimer_set_limit(s->ptimer, 1<<((s->to&0x0f)+16), 1);
    ptimer_run(s->ptimer, 0);

}
static const unsigned char dw_apb_wdt_id[] = {
    0x44, 0x57, 0x01, 0x20,         /* Device ID        */
    0x0d, 0xf0, 0x05, 0xb1          /* Cell ID      */
};
static uint64_t dw_apb_wdt_read(void *opaque, hwaddr offset, unsigned size)
{
	DW_APB_WDTState *s = (DW_APB_WDTState *)opaque;



  if (offset >= 0xfe0  &&  offset < 0x1000)
          {
      	return dw_apb_wdt_id[(offset - 0xfe0) >> 2];
          }

	if (offset < 0xA0)
	{
		switch(offset )
		{
		case 0:
				return s->cr;
		case 4:

			return s->to;
		case 8:
				return (s->ptimer->next_event-qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL))/s->ptimer->period;

		case 0x0c:
			return 0;
		case 0x10:
			return s->stat;
		case 0x14:
			s->stat=0;
			qemu_irq_lower(s->irq);
			return 0;

			default:
	        qemu_log_mask(LOG_GUEST_ERROR, "dw_apb_timer_read: Bad offset 0x%x\n", (int)offset);
	        return 0;
		}
	}
	else if (offset >= 0xA0)
	{
		switch(offset)
		{
		case 0xe4:
			return 1<<16-1;
		case 0xe8:

			return 1<<16-1;
		case 0xec:
			return 0;
		case 0xf0:
			return 1<<16-1;
		case 0xf4:
			return 0x8000120;
		case 0xf8:
			return 0x3230312A;
		case 0xfc:
			return 0x44570120;
		default: //Current revision number
			qemu_log_mask(LOG_GUEST_ERROR, "dw_apb_timer_read: Bad offset 0x%x\n", (int)offset);
			return 0;
		}
	}
	else
	{
		qemu_log_mask(LOG_GUEST_ERROR, "dw_apb_timer_read: Bad offset 0x%x\n", (int)offset);
		return 0;
	}

}
static void dw_apb_wdt_write(void * opaque, hwaddr offset,
                        uint64_t value, unsigned size)
{
	DW_APB_WDTState *s = (DW_APB_WDTState *)opaque;



	static int tempflag=0;
    if (offset < 0x20)
    {
    	switch (offset )
    	{
    	case WDT_CR:
            s->cr = value;

    		break;
    	case 4:
    	    s->to = value;
    	    tempflag=1;

    	    break;
    	case 0xc:
        	if ((value==0x76)&&(s->cr&1))
        	{
        	   	ptimer_stop(s->ptimer);
        	   	s->stat=0;
        		qemu_irq_lower(s->irq);
        		if(tempflag)
        		{
        			ptimer_set_limit(s->ptimer, 1<<(((s->to&0xf0)>>4)+16), 1);
        			tempflag=0;
        		}
        		else
        		{
        			ptimer_set_limit(s->ptimer, 1<<(((s->to&0x0f))+16), 1);
        		}
        		ptimer_run(s->ptimer, 0);

        	}
    		break;
    	default:
    		qemu_log_mask(LOG_GUEST_ERROR, "dw_apb_timer_write: Bad offset 0x%x\n", (int)offset);
    	}
    }
    else
    	qemu_log_mask(LOG_GUEST_ERROR, "dw_apb_timer_write: Bad offset 0x%x\n", (int)offset);

}
static WatchdogTimerModel model = {
    .wdt_name = "dw_apb_wdt",
    .wdt_description = "DW APB_WDT",
};
static const MemoryRegionOps dw_apb_wdt_ops = {
    .read = dw_apb_wdt_read,
    .write = dw_apb_wdt_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
};
static void dw_apb_wdt_reset(DeviceState *d)
{
	DW_APB_WDTState *s = WATCHDOG_DW_APB_WDT_DEVICE(d);

    s->freq = 41200000;
    s->cr = 1;
    s->to= 0;
    s->crr = 0;
    s->stat=0;
    ptimer_set_freq(s->ptimer, s->freq);
   //s->ptimer->enabled=1;// start watchdog
    ptimer_set_limit(s->ptimer, 1<<16-1, 1);

}
static int dw_apb_wdt_init(SysBusDevice *dev)
{
	DW_APB_WDTState *s = WATCHDOG_DW_APB_WDT_DEVICE(dev);
        QEMUBH *bh;

        bh = qemu_bh_new(dw_apb_wdt_expired, s);
        sysbus_init_irq(dev, &s->irq);
        s->ptimer = ptimer_init(bh);
        s->cr = 1;

    memory_region_init_io(&s->iomem, OBJECT(s), &dw_apb_wdt_ops, s, "dw_apb_wdt", 0x1000);
    sysbus_init_mmio(dev, &s->iomem);

    return 0;
}

static void dw_apb_wdt_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    SysBusDeviceClass *k = SYS_BUS_DEVICE_CLASS(klass);

    k->init = dw_apb_wdt_init;
    dc->reset = dw_apb_wdt_reset;
    dc->vmsd = &vmstate_dw_apb_wdt;

}

static const TypeInfo dw_apb_wdt_info = {
    .name          = TYPE_WATCHDOG_DW_APB_WDT_DEVICE,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(DW_APB_WDTState),
    .class_init    = dw_apb_wdt_class_init,
};

static void dw_apb_wdt_register_types(void)
{
    watchdog_add_model(&model);
    type_register_static(&dw_apb_wdt_info);
}

type_init(dw_apb_wdt_register_types)
