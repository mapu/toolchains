/*
 * drivers/rtc/rtc-dw_apb_rtc.c
 *
 * Real Time Clock interface for ARM AMBA PrimeCell 031 RTC
 *
 * Author: Deepak Saxena <dsaxena@plexity.net>
 *
 * Copyright 2006 (c) MontaVista Software, Inc.
 *
 * Author: Mian Yousaf Kaukab <mian.yousaf.kaukab@stericsson.com>
 * Copyright 2010 (c) ST-Ericsson AB
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#include <linux/module.h>
#include <linux/rtc.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/amba/bus.h>
#include <linux/io.h>
#include <linux/bcd.h>
#include <linux/delay.h>
#include <linux/slab.h>

/*
 * Register definitions
 */
#define RTC_CCVR      0x00    /* Current counter value*/
#define RTC_CMR       0x04    /* Match register   writable*/
#define RTC_CLR       0x08    /* Data load register   writable*/
#define RTC_CCR       0x0c    /* Control register    writable*/
#define RTC_STAT      0x10    /* Interrupt status register */
#define RTC_RSTAT     0x14    /* Raw interrupt status register */
#define RTC_EOI       0x18    /* End of interrupt */
#define RTC_COVE      0x1c    /* Compoment version register */

/* Common bit definitions for Interrupt status and control registers */
#define RTC_BIT_AI	(1 << 0) /* Alarm interrupt bit */
#define RTC_BIT_MASK	(1 << 1) /* Alarm interrupt bit */
#define RTC_BIT_EN	(1 << 2) /* user control bit */
#define RTC_BIT_WEN	(1 << 3) /* Alarm interrupt bit */
/* Common bit definations for ST v2 for reading/writing time */
#define RTC_SEC_SHIFT 0
#define RTC_SEC_MASK (0x3F << RTC_SEC_SHIFT) /* Second [0-59] */
#define RTC_MIN_SHIFT 6
#define RTC_MIN_MASK (0x3F << RTC_MIN_SHIFT) /* Minute [0-59] */
#define RTC_HOUR_SHIFT 12
#define RTC_HOUR_MASK (0x1F << RTC_HOUR_SHIFT) /* Hour [0-23] */
#define RTC_WDAY_SHIFT 17
#define RTC_WDAY_MASK (0x7 << RTC_WDAY_SHIFT) /* Day of Week [1-7] 1=Sunday */
#define RTC_MDAY_SHIFT 20
#define RTC_MDAY_MASK (0x1F << RTC_MDAY_SHIFT) /* Day of Month [1-31] */
#define RTC_MON_SHIFT 25
#define RTC_MON_MASK (0xF << RTC_MON_SHIFT) /* Month [1-12] 1=January */

#define RTC_TIMER_FREQ 41200000

struct dw_apb_rtc_local {
	struct rtc_device *rtc;
	void __iomem *base;
	u8 hw_designer;
	u8 hw_revision:4;
};

static int dw_apb_rtc_alarm_irq_enable(struct device *dev,
	unsigned int enabled)
{
	struct dw_apb_rtc_local *ldata = dev_get_drvdata(dev);
	unsigned long imsc;

	/* Clear any pending alarm interrupts. */
	readl(ldata->base + RTC_EOI);

	imsc = readl(ldata->base + RTC_CCR);

	if (enabled == 1)
		writel(imsc | 5, ldata->base + RTC_CCR);
	else
		writel(imsc & ~RTC_BIT_AI, ldata->base + RTC_CCR);

	return 0;
}


static irqreturn_t dw_apb_rtc_interrupt(int irq, void *dev_id)
{
	struct dw_apb_rtc_local *ldata = dev_id;
	unsigned long rtcmis;
	unsigned long events = 0;

	rtcmis = readl(ldata->base + RTC_STAT);
	if (rtcmis) {


		if (rtcmis & RTC_BIT_AI)
			events |= (RTC_AF | RTC_IRQF);

		rtc_update_irq(ldata->rtc, 1, events);
		readl(ldata->base + RTC_EOI);
		return IRQ_HANDLED;
	}

	return IRQ_NONE;
}

static int dw_apb_rtc_read_time(struct device *dev, struct rtc_time *tm)
{
	struct dw_apb_rtc_local *ldata = dev_get_drvdata(dev);

	rtc_time_to_tm(readl(ldata->base + RTC_CCVR), tm);

	return 0;
}

static int dw_apb_rtc_set_time(struct device *dev, struct rtc_time *tm)
{
	unsigned long time;
	struct dw_apb_rtc_local *ldata = dev_get_drvdata(dev);
	int ret;

	ret = rtc_tm_to_time(tm, &time);

	if (ret == 0)
		writel(time, ldata->base + RTC_CLR);

	return ret;
}

static int dw_apb_rtc_read_alarm(struct device *dev, struct rtc_wkalrm *alarm)
{
	struct dw_apb_rtc_local *ldata = dev_get_drvdata(dev);

	rtc_time_to_tm(readl(ldata->base + RTC_CMR), &alarm->time);

	alarm->pending = readl(ldata->base + RTC_RSTAT) & RTC_BIT_AI;
	alarm->enabled = (readl(ldata->base + RTC_CCR) & RTC_BIT_MASK)>>1;

	return 0;
}

static int dw_apb_rtc_set_alarm(struct device *dev, struct rtc_wkalrm *alarm)
{
	struct dw_apb_rtc_local *ldata = dev_get_drvdata(dev);
	unsigned long time;
	int ret;

	/* At the moment, we can only deal with non-wildcarded alarm times. */
	ret = rtc_valid_tm(&alarm->time);

	if (ret == 0) {
		ret = rtc_tm_to_time(&alarm->time, &time);
		if (ret == 0) {
			writel(time, ldata->base + RTC_CMR);
			dw_apb_rtc_alarm_irq_enable(dev, alarm->enabled);

		}
	}

	return ret;
}

static int dw_apb_rtc_remove(struct amba_device *adev)
{
	struct dw_apb_rtc_local *ldata = dev_get_drvdata(&adev->dev);

	amba_set_drvdata(adev, NULL);
	free_irq(adev->irq[0], ldata->rtc);
	rtc_device_unregister(ldata->rtc);
	iounmap(ldata->base);
	kfree(ldata);
	amba_release_regions(adev);

	return 0;
}

static int dw_apb_rtc_probe(struct amba_device *adev, const struct amba_id *id)
{
	int ret;
	struct dw_apb_rtc_local *ldata;
	struct rtc_class_ops *ops = id->data;
	unsigned long time;

	ret = amba_request_regions(adev, NULL);
	if (ret)
		goto err_req;

	ldata = kzalloc(sizeof(struct dw_apb_rtc_local), GFP_KERNEL);
	if (!ldata) {
		ret = -ENOMEM;
		goto out;
	}

	ldata->base = ioremap(adev->res.start, resource_size(&adev->res));

	if (!ldata->base) {
		ret = -ENOMEM;
		goto out_no_remap;
	}

	amba_set_drvdata(adev, ldata);

	ldata->hw_designer = amba_manf(adev);
	ldata->hw_revision = amba_rev(adev);

	dev_dbg(&adev->dev, "designer ID = 0x%02x\n", ldata->hw_designer);
	dev_dbg(&adev->dev, "revision = 0x%01x\n", ldata->hw_revision);

	ldata->rtc = rtc_device_register("dw_apb_rtc", &adev->dev, ops,
					THIS_MODULE);
	if (IS_ERR(ldata->rtc)) {
		ret = PTR_ERR(ldata->rtc);

		goto out_no_rtc;
	}

	if (request_irq(adev->irq[0], dw_apb_rtc_interrupt,
			0, "rtc-dw_apb_rtc", ldata)) {
		ret = -EIO;
		goto out_no_irq;
	}

	return 0;

out_no_irq:
	rtc_device_unregister(ldata->rtc);
out_no_rtc:
	iounmap(ldata->base);
	amba_set_drvdata(adev, NULL);
out_no_remap:
	kfree(ldata);
out:
	amba_release_regions(adev);
err_req:

	return ret;
}

/* Operations for the original ARM version */
static struct rtc_class_ops arm_dw_apb_rtc_ops = {
	.read_time = dw_apb_rtc_read_time,
	.set_time = dw_apb_rtc_set_time,
	.read_alarm = dw_apb_rtc_read_alarm,
	.set_alarm = dw_apb_rtc_set_alarm,
	.alarm_irq_enable = dw_apb_rtc_alarm_irq_enable,
};





static struct amba_id dw_apb_rtc_ids[] = {
	{
		.id = 0x00041033,
		.mask = 0x000fffff,
		.data = &arm_dw_apb_rtc_ops,
	},

	{0, 0},
};

MODULE_DEVICE_TABLE(amba, dw_apb_rtc_ids);

static struct amba_driver dw_apb_rtc_driver = {
	.drv = {
		.name = "dw_apb_rtc",
	},
	.id_table = dw_apb_rtc_ids,
	.probe = dw_apb_rtc_probe,
	.remove = dw_apb_rtc_remove,
};

module_amba_driver(dw_apb_rtc_driver);

MODULE_AUTHOR("Deepak Saxena <dsaxena@plexity.net");
MODULE_DESCRIPTION("ARM AMBA DW_APB_TIMER RTC Driver");
MODULE_LICENSE("GPL");
