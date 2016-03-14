/*
 * DW APB Timers
 *
 * Copyright (c) 2007 CodeSourcery
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Contributions after 2012-01-13 are licensed under the terms of the
 * GNU GPL, version 2 or (at your option) any later version.
 */

#include "hw/sysbus.h"
#include "qemu/timer.h"
#include "qemu-common.h"
#include "qemu/main-loop.h"
#include "hw/ptimer.h"

//#define DEBUG_DW_APB_TIMER

#ifdef DEBUG_DW_APB_TIMER
#define DPRINTF(fmt, ...) \
do { fprintf(stderr, "dw_apb_timer: " fmt , ## __VA_ARGS__); } while (0)
#else
#define DPRINTF(fmt, ...) \
do {} while (0)
#endif

#define DAT_T1_LC   0x00    /* Timer1 Load Count */
#define DAT_T1_CV   0x04    /* Timer1 Current Value */
#define DAT_T1_CR   0x08    /* Timer1 Control Register */
#define DAT_T1_EOI  0x0C    /* Clears the interrupt from Timer1 */
#define DAT_T1_IS   0x10    /* Contains the interrupt status for Timer1 */

#define DAT_T2_LC   0x14    /* Timer2 Load Count */
#define DAT_T2_CV   0x18    /* Timer2 Current Value */
#define DAT_T2_CR   0x1C    /* Timer2 Control Register */
#define DAT_T2_EOI  0x20    /* Clears the interrupt from Timer2 */
#define DAT_T2_IS   0x24    /* Contains the interrupt status for Timer2 */

#define DAT_T3_LC   0x28    /* Timer3 Load Count */
#define DAT_T3_CV   0x2C    /* Timer3 Current Value */
#define DAT_T3_CR   0x30    /* Timer3 Control Register */
#define DAT_T3_EOI  0x34    /* Clears the interrupt from Timer3 */
#define DAT_T3_IS   0x38    /* Contains the interrupt status for Timer3 */

#define DAT_T4_LC   0x3C    /* Timer4 Load Count */
#define DAT_T4_CV   0x40    /* Timer4 Current Value */
#define DAT_T4_CR   0x44    /* Timer4 Control Register */
#define DAT_T4_EOI  0x48    /* Clears the interrupt from Timer4 */
#define DAT_T4_IS   0x4C    /* Contains the interrupt status for Timer4 */

#define DAT_T5_LC   0x50    /* Timer5 Load Count */
#define DAT_T5_CV   0x54    /* Timer5 Current Value */
#define DAT_T5_CR   0x58    /* Timer5 Control Register */
#define DAT_T5_EOI  0x5C    /* Clears the interrupt from Timer5 */
#define DAT_T5_IS   0x60    /* Contains the interrupt status for Timer5 */

#define DAT_T6_LC   0x64    /* Timer6 Load Count */
#define DAT_T6_CV   0x68    /* Timer6 Current Value */
#define DAT_T6_CR   0x6C    /* Timer6 Control Register */
#define DAT_T6_EOI  0x70    /* Clears the interrupt from Timer6 */
#define DAT_T6_IS   0x74    /* Contains the interrupt status for Timer6 */

#define DAT_T7_LC   0x78    /* Timer7 Load Count */
#define DAT_T7_CV   0x7C    /* Timer7 Current Value */
#define DAT_T7_CR   0x80    /* Timer7 Control Register */
#define DAT_T7_EOI  0x84    /* Clears the interrupt from Timer7 */
#define DAT_T7_IS   0x88    /* Contains the interrupt status for Timer7 */

#define DAT_T8_LC   0x8C    /* Timer8 Load Count */
#define DAT_T8_CV   0x90    /* Timer8 Current Value */
#define DAT_T8_CR   0x94    /* Timer8 Control Register */
#define DAT_T8_EOI  0x98    /* Clears the interrupt from Timer8 */
#define DAT_T8_IS   0x9C    /* Contains the interrupt status for Timer8 */

#define DAT_TIS     0xA0    /* Contains the interrupt status of all timers in the component */
#define DAT_TEOI    0xA4    /* Returns all zeroes (0) and clears all active interrupts */
#define DAT_TRIS    0xA8    /* Contains the unmasked interrupt status of all timers in the component */
#define DAT_TCV     0xAC    /* Current revision number of the DW_apb_timers component */


#define TYPE_DW_APB_TIMER "dw_apb_timer"
#define DW_APB_TIMER(obj) OBJECT_CHECK(DwApbTimerState, (obj), TYPE_DW_APB_TIMER)

typedef struct DwApbTimer
{
	ptimer_state *ptimer;
	qemu_irq  irq;
	uint32_t  id;
	uint32_t  lc;
	uint32_t  cv;
	uint32_t  cr;
	uint32_t  eoi;
	uint32_t  is;

	void *father;

}DwApbTimer;



typedef struct DwApbTimerState
{
    SysBusDevice parent_obj;

    MemoryRegion iomem;

    uint32_t tis;
    uint32_t teoi;
    uint32_t tris;
    uint32_t tcv;

    uint32_t    freq;           /* timer frequency */

    DwApbTimer timer[8];
}DwApbTimerState;

/*** VMState ***/
static const VMStateDescription vmstate_dw_apb_timer= {
    .name = "dw_apb_timer.timer",
    .version_id = 1,
    .minimum_version_id = 1,
    .fields = (VMStateField[]) {
        VMSTATE_PTIMER(ptimer, DwApbTimer),
        VMSTATE_UINT32(id, DwApbTimer),
        VMSTATE_UINT32(lc, DwApbTimer),
        VMSTATE_UINT32(cv, DwApbTimer),
		VMSTATE_UINT32(cr, DwApbTimer),
		VMSTATE_UINT32(eoi, DwApbTimer),
		VMSTATE_UINT32(is, DwApbTimer),
        VMSTATE_END_OF_LIST()
    }
};

static const VMStateDescription vmstate_dw_apb_timer_state = {
    .name = "dw_apb_timer",
    .version_id = 1,
    .minimum_version_id = 1,
    .fields = (VMStateField[]) {
        VMSTATE_UINT32(tis, DwApbTimerState),
        VMSTATE_UINT32(teoi, DwApbTimerState),
        VMSTATE_UINT32(tris, DwApbTimerState),
		VMSTATE_UINT32(tcv, DwApbTimerState),
		VMSTATE_UINT32(freq, DwApbTimerState),
        VMSTATE_STRUCT_ARRAY(timer, DwApbTimerState,
            8, 0, vmstate_dw_apb_timer, DwApbTimer),
        VMSTATE_END_OF_LIST()
    }
};

/*
 * Counter tick handler
 */
static void dw_apb_timer_tick(void *opaque)
{
    DwApbTimer *s = (DwApbTimer *)opaque;
    uint32_t id = s->id;

    /* raise IRQ */
    ((DwApbTimerState*)(s->father))->tris |= (1 << id);

    if (s->cr & 0b100)
    {
    	;
    }
    else
    {

    	((DwApbTimerState*)(s->father))->tis |= 1 << id;
    	s->is = 1;
    	qemu_irq_raise(s->irq);
    }

    /* reload timer */
    if (((s->cr >> 1) & 0x01) == 0)  //free mode
    {
    	ptimer_set_limit(s->ptimer, 0xffffffff, 1);
    	ptimer_run(s->ptimer, 0);
    }
    else   //user-defined mode
    {
    	ptimer_set_limit(s->ptimer, s->lc, 1);
    	ptimer_run(s->ptimer, 0);
    }
}


static uint64_t dw_apb_timer_read(void *opaque, hwaddr offset, unsigned size)
{
	DwApbTimerState *s = (DwApbTimerState *)opaque;
	uint32_t i;

  DPRINTF("timer read: offset=0x%x size=0x%x\n", offset, size);


	if (offset < 0xA0)
	{
		switch(offset % 20)
		{
		case 0: //Load Count
			if((s->timer[offset/20].cr & 0b1) == 1)
				return s->timer[offset/20].lc;
			else
				return 0;
		case 4: //Current Value
			if((s->timer[offset/20].cr & 0b1) == 1)
				return ptimer_get_count(s->timer[offset/20].ptimer);
			else
				return 0;
		case 8: //Control register
			if((s->timer[offset/20].cr & 0b1) == 1)
				return s->timer[offset/20].cr;
			else
				return 0;
		case 12: //EOI
			if((s->timer[offset/20].cr & 0b1) == 1)
			{
		    	s->tris &= ~(1 << (offset / 20));
		    	s->tis  &= ~(1 << (offset / 20));
		    	s->timer[offset/20].is = 0;
		    	qemu_irq_lower(s->timer[offset/20].irq);
			}
			return 0;
		case 16: //Interrupt status
			if((s->timer[offset/20].cr & 0b1) == 1)
				return s->timer[offset/20].is;
			else
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
		case 0xA0: //Interrupt status
			return s->tis;
		case 0xA4: //EOI
			for (i=0; i<8; i++)
			{
				qemu_irq_lower(s->timer[i].irq);
				s->timer[i].is = 0;
			}
			s->tis = 0;
			s->tris = 0;
			return 0;
		case 0xA8: //Raw Interrupt status
			return s->tris;
		case 0xAC:
			return s->tcv;
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

static void dw_apb_timer_write(void * opaque, hwaddr offset,
                        uint64_t value, unsigned size)
{
	DwApbTimerState *s = (DwApbTimerState *)opaque;

    DPRINTF("timer write: offset=0x%x value=0x%x, size=0x%x\n", offset, value, size);


    if (offset < 0xA0)
    {
    	switch (offset % 20)
    	{
    	case 0: //Load count
            s->timer[offset/20].lc = value;
    		break;
    	case 8: //Control register
        	s->timer[offset/20].cr = value;
        	if ((value & 0b1) == 1)
        	{
            	ptimer_set_limit(s->timer[offset/20].ptimer, s->timer[offset/20].lc, 1);
        		ptimer_run(s->timer[offset/20].ptimer, 0);
        	}
        	else
        	{
        		s->timer[offset/20].is = 0;
        		s->timer[offset/20].cv = 0;
        		s->timer[offset/20].cr = 0;
        		s->timer[offset/20].lc = 0;
        		s->tis &= ~(1 << (offset/20));
        		s->tris &= ~(1 << (offset/20));
        		ptimer_stop(s->timer[offset/20].ptimer);
        		qemu_irq_lower(s->timer[offset/20].irq);
        	}
    		break;
    	default:
    		qemu_log_mask(LOG_GUEST_ERROR, "dw_apb_timer_write: Bad offset 0x%x\n", (int)offset);
    	}
    }
    else
    	qemu_log_mask(LOG_GUEST_ERROR, "dw_apb_timer_write: Bad offset 0x%x\n", (int)offset);

}

static const MemoryRegionOps dw_apb_timer_ops = {
    .read = dw_apb_timer_read,
    .write = dw_apb_timer_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
};

/*
 * Set default values to timer fields and registers
 */
static void dw_apb_timer_reset(DeviceState *d)
{
    DwApbTimerState *s = DW_APB_TIMER(d);
    int i;

    s->tis = 0;
    s->teoi = 0;
    s->tris = 0;
    s->tcv = 0;
//    s->freq = 200000000; //Set frequency to 200MHz
    s->freq = 41200000; //match the chip
    for (i = 0; i < 8; i++)
    {
        s->timer[i].lc = 0;
        s->timer[i].cv = 0;
        s->timer[i].cr = 0;
        s->timer[i].eoi = 0;
        s->timer[i].is = 0;
        ptimer_set_freq(s->timer[i].ptimer, s->freq);
        ptimer_stop(s->timer[i].ptimer);
    }
}

static int dw_apb_timer_init(SysBusDevice *dev)
{
    DwApbTimerState *s = DW_APB_TIMER(dev);
    int i;
    QEMUBH *bh;

    //s->freq = 200000000; //Set frequency to 200MHz

    for (i = 0; i < 8; i++)
    {
        bh = qemu_bh_new(dw_apb_timer_tick, &s->timer[i]);
        sysbus_init_irq(dev, &s->timer[i].irq);
        s->timer[i].ptimer = ptimer_init(bh);
        s->timer[i].id = i;
        s->timer[i].father = s;
        //ptimer_set_freq(s->timer[i].ptimer, s->freq);
    }

    memory_region_init_io(&s->iomem, OBJECT(s), &dw_apb_timer_ops, s, "dw_apb_timer", 0x40000);
    sysbus_init_mmio(dev, &s->iomem);

    return 0;
}

static void dw_apb_timer_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    SysBusDeviceClass *k = SYS_BUS_DEVICE_CLASS(klass);

    k->init = dw_apb_timer_init;
    dc->reset = dw_apb_timer_reset;
    dc->vmsd = &vmstate_dw_apb_timer_state;
}

static const TypeInfo dw_apb_timer_info = {
    .name          = TYPE_DW_APB_TIMER,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(DwApbTimerState),
    .class_init    = dw_apb_timer_class_init,
};

static void dw_apb_timer_register_types(void)
{
    type_register_static(&dw_apb_timer_info);
}

type_init(dw_apb_timer_register_types);
