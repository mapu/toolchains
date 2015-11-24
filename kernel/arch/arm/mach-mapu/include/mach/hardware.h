/*
 * hardware.h
 *
 *  Created on: Nov 15, 2014
 *      Author: wangl
 */

#ifndef __ASM_ARCH_HARDWARE_H_
#define __ASM_ARCH_HARDWARE_H_

#include <asm/sizes.h>

/* macro to get at IO space when running virtually */
#ifdef CONFIG_MMU


#define IO_APC_PHYS  0x40000000
#define IO_APC_VIRT  0xF9000000
#define IO_APC_SIZE  SZ_64M

#define IO_AMBA_PHYS  0x50000000
#define IO_AMBA_VIRT  0xFD000000
#define IO_AMBA_SIZE  SZ_16M

#define IO_AXI_PHYS  0x54000000
#define IO_AXI_VIRT  0xFE000000
#define IO_AXI_SIZE  SZ_16M

#define IO_TO_VIRT_BETWEEN(p, st, sz) ((p) >= (st) && (p) < ((st) + (sz)))
#define IO_TO_VIRT_XLATE(p, pst, vst) (((p) - (pst) + (vst)))

#define IO_TO_VIRT(n) ( \
  IO_TO_VIRT_BETWEEN((n), IO_APC_PHYS, IO_APC_SIZE) ?   \
    IO_TO_VIRT_XLATE((n), IO_APC_PHYS, IO_APC_VIRT) : \
  IO_TO_VIRT_BETWEEN((n), IO_AMBA_PHYS, IO_AMBA_SIZE) ?   \
    IO_TO_VIRT_XLATE((n), IO_AMBA_PHYS, IO_AMBA_VIRT) : \
  IO_TO_VIRT_BETWEEN((n), IO_AXI_PHYS, IO_AXI_SIZE) ?   \
    IO_TO_VIRT_XLATE((n), IO_AXI_PHYS, IO_AXI_VIRT) : \
  0)

#define IO_ADDRESS(n) (IO_TO_VIRT(n))
#else
#define IO_ADDRESS(x)   (x)
#endif
#define __io_address(n)   IOMEM(IO_ADDRESS(n))



#endif /* __ASM_ARCH_HARDWARE_H_ */
