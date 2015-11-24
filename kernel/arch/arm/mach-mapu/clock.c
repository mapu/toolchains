/*
 * clock.c
 *
 *  Created on: Nov 17, 2014
 *      Author: wangl
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/clk.h>
#include <linux/mutex.h>

#include <mach/clkdev.h>

int clk_enable(struct clk *clk)
{
  return 0;
}
EXPORT_SYMBOL(clk_enable);

void clk_disable(struct clk *clk)
{
}
EXPORT_SYMBOL(clk_disable);

unsigned long clk_get_rate(struct clk *clk)
{
  return clk->rate;
}
EXPORT_SYMBOL(clk_get_rate);

long clk_round_rate(struct clk *clk, unsigned long rate)
{
  long ret = -EIO;
  return ret;
}
EXPORT_SYMBOL(clk_round_rate);

int clk_set_rate(struct clk *clk, unsigned long rate)
{
  int ret = -EIO;
  return ret;
}
EXPORT_SYMBOL(clk_set_rate);
