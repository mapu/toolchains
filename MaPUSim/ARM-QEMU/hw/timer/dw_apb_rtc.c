/*
 * ARM AMBA PrimeCell DW_APB_RTC RTC *
 */

#include "hw/sysbus.h"
#include "qemu/timer.h"
#include "sysemu/sysemu.h"
#include "hw/char/serial.h"
//#define DEBUG_DW_APB_RTC

#ifdef DEBUG_DW_APB_RTC
#define DPRINTF(fmt, ...) \
do { printf("dw_apb_rtc: " fmt , ## __VA_ARGS__); } while (0)
#else
#define DPRINTF(fmt, ...) do {} while(0)
#endif

#define RTC_CCVR      0x00    /* Current counter value*/
#define RTC_CMR       0x04    /* Match register   writable*/
#define RTC_CLR       0x08    /* Data load register   writable*/
#define RTC_CCR       0x0c    /* Control register    writable*/
#define RTC_STAT      0x10    /* Interrupt status register */
#define RTC_RSTAT     0x14    /* Raw interrupt status register */
#define RTC_EOI       0x18    /* End of interrupt */
#define RTC_COVE      0x1c    /* Compoment version register */
//#define RTC_FRQ       1000000000

#define TYPE_DW_APB_RTC "dw_apb_rtc"
#define DW_APB_RTC(obj) OBJECT_CHECK(DW_APB_RTCState, (obj), TYPE_DW_APB_RTC)

typedef struct DW_APB_RTCState {
    SysBusDevice parent_obj;

    MemoryRegion iomem;
    QEMUTimer *timer;
    qemu_irq irq;

    /* Needed to preserve the tick_count across migration, even if the
     * absolute value of the rtc_clock is different on the source and
     * destination.
     */
    uint32_t tick_offset_vmstate;
    uint32_t tick_offset; //seconds

    uint32_t cmr;//ticks
    uint32_t clr;
    uint32_t ccr;
    uint32_t st;
    uint32_t rs;
} DW_APB_RTCState;

static const unsigned char dw_apb_rtc_id[] = {
    0x33, 0x10, 0x14, 0x00,         /* Device ID        */
    0x0d, 0xf0, 0x05, 0xb1          /* Cell ID      */
};


static void dw_apb_rtc_update(DW_APB_RTCState *s)
{
   qemu_set_irq(s->irq, s->st);

}

static void dw_apb_rtc_interrupt(void * opaque)
{
	DW_APB_RTCState *s = (DW_APB_RTCState *)opaque;
	if (s->ccr&1)
	{
		s->rs = 1;
		if (!(s->ccr&2))
		{
			s->st=1;
			dw_apb_rtc_update(s);
		}
		else
			s->st=0;
	}

}

static uint32_t dw_apb_rtc_get_count(DW_APB_RTCState *s)
{
    int64_t now = qemu_clock_get_ns(rtc_clock);
    return s->tick_offset + now / get_ticks_per_sec();
}

static void dw_apb_rtc_set_alarm(DW_APB_RTCState *s)
{
    uint32_t ticks,tmp;
    if(!(s->ccr&4))
    {
    	timer_del(s->timer);
    	return;
    }
    tmp=dw_apb_rtc_get_count(s);

    if(s->cmr<tmp)
    {
    	timer_del(s->timer);
    	return;
    }
    ticks = (s->cmr - tmp);


    if (ticks == 0) {
        timer_del(s->timer);
        dw_apb_rtc_interrupt(s);

    } else {

        int64_t now = qemu_clock_get_ns(rtc_clock);
        timer_mod(s->timer, now + (int64_t)ticks * get_ticks_per_sec());

    }
}

static uint64_t dw_apb_rtc_read(void *opaque, hwaddr offset,
                           unsigned size)
{
    DW_APB_RTCState *s = (DW_APB_RTCState *)opaque;

if (offset >= 0xfe0  &&  offset < 0x1000)
        {
    	return dw_apb_rtc_id[(offset - 0xfe0) >> 2];
        }
    switch (offset) {
    case RTC_CCVR:

        return dw_apb_rtc_get_count(s);
    case RTC_CMR:
        return s->cmr;
    case RTC_EOI:
    	s->rs=0;
    	s->st=0;
    	dw_apb_rtc_update(s);
        return s->st;
    case RTC_RSTAT:
        return s->rs;
    case RTC_CLR:
        return s->clr;
    case RTC_CCR:

        return s->ccr;
    case RTC_STAT:
        return s->st;
    case RTC_COVE:
        break;
    default:
        qemu_log_mask(LOG_GUEST_ERROR,
                      "dw_apb_rtc_read: Bad offset 0x%x\n", (int)offset);
        break;
    }

    return 0;
}

static void dw_apb_rtc_write(void * opaque, hwaddr offset,
                        uint64_t value, unsigned size)
{
    DW_APB_RTCState *s = (DW_APB_RTCState *)opaque;


    switch (offset) {
    case RTC_CLR:
        s->tick_offset += (value - dw_apb_rtc_get_count(s));

        dw_apb_rtc_set_alarm(s);

        break;
    case RTC_CMR:
        s->cmr = value;
        dw_apb_rtc_set_alarm(s);

        break;
    case RTC_CCR:
        s->ccr=value;
        if (value&4)
        	timer_mod(s->timer, s->timer->expire_time);
        else
        {
        	timer_del(s->timer);
        	s->rs=0;
        	s->st=0;
        }
        break;

    case RTC_CCVR:
    case RTC_STAT:
    case RTC_RSTAT:
    case RTC_EOI:
    case RTC_COVE:
        qemu_log_mask(LOG_GUEST_ERROR,
                      "dw_apb_rtc: write to read-only register at offset 0x%x\n",
                      (int)offset);
        break;

    default:
        qemu_log_mask(LOG_GUEST_ERROR,
                      "dw_apb_rtc_write: Bad offset 0x%x\n", (int)offset);
        break;
    }
}

static const MemoryRegionOps dw_apb_rtc_ops = {
    .read = dw_apb_rtc_read,
    .write = dw_apb_rtc_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
};

static int dw_apb_rtc_init(SysBusDevice *dev)
{
    DW_APB_RTCState *s = DW_APB_RTC(dev);
    struct tm tm;
    memory_region_init_io(&s->iomem, OBJECT(s), &dw_apb_rtc_ops, s, "dw_apb_rtc", 0x1000);
    sysbus_init_mmio(dev, &s->iomem);

    sysbus_init_irq(dev, &s->irq);
    qemu_get_timedate(&tm, 0);
    s->tick_offset = (mktimegm(&tm) -
        qemu_clock_get_ns(rtc_clock) / get_ticks_per_sec());

    s->timer = timer_new_ns(rtc_clock, dw_apb_rtc_interrupt, s);


    return 0;
}
static void dw_apb_rtc_pre_save(void *opaque)
{
    DW_APB_RTCState *s = opaque;

    /* tick_offset is base_time - rtc_clock base time.  Instead, we want to
     * store the base time relative to the QEMU_CLOCK_VIRTUAL for backwards-compatibility.  */
    int64_t delta = qemu_clock_get_ns(rtc_clock) - qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL);
    s->tick_offset_vmstate = s->tick_offset + delta / get_ticks_per_sec();
}

static int dw_apb_rtc_post_load(void *opaque, int version_id)
{
    DW_APB_RTCState *s = opaque;

    int64_t delta = qemu_clock_get_ns(rtc_clock) - qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL);
    s->tick_offset = s->tick_offset_vmstate - delta / get_ticks_per_sec();

    return 0;
}


static const VMStateDescription vmstate_dw_apb_rtc = {
    .name = "dw_apb_rtc",
    .version_id = 1,
    .minimum_version_id = 1,
	.pre_save = dw_apb_rtc_pre_save,
	.post_load = dw_apb_rtc_post_load,
    .fields = (VMStateField[]) {
        VMSTATE_UINT32(tick_offset_vmstate, DW_APB_RTCState),
        VMSTATE_UINT32(cmr, DW_APB_RTCState),
        VMSTATE_UINT32(clr, DW_APB_RTCState),
        VMSTATE_UINT32(ccr, DW_APB_RTCState),
        VMSTATE_UINT32(rs, DW_APB_RTCState),
        VMSTATE_END_OF_LIST()
    }
};

static void dw_apb_rtc_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    SysBusDeviceClass *k = SYS_BUS_DEVICE_CLASS(klass);

    k->init = dw_apb_rtc_init;
    dc->vmsd = &vmstate_dw_apb_rtc;

}

static const TypeInfo dw_apb_rtc_info = {
    .name          = TYPE_DW_APB_RTC,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(DW_APB_RTCState),
    .class_init    = dw_apb_rtc_class_init,
};

static void dw_apb_rtc_register_types(void)
{
    type_register_static(&dw_apb_rtc_info);
}

type_init(dw_apb_rtc_register_types)
