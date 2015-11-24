/*
 * clkdev.h
 *
 *  Created on: Nov 17, 2014
 *      Author: wangl
 */

#ifndef __ASM_MACH_CLKDEV_H
#define __ASM_MACH_CLKDEV_H

struct clk {
  unsigned long   rate;
  void __iomem    *vcoreg;
};

#define __clk_get(clk) ({ 1; })
#define __clk_put(clk) do { } while (0)

#endif
