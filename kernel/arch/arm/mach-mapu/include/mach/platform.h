/*
 * platform.h
 *
 *  Created on: Nov 15, 2014
 *      Author: wangl
 */

#ifndef __ASM_ARCH_PLATFORM_H
#define __ASM_ARCH_PLATFORM_H

/*
 * Memory definitions
 */
#define MAPU_NOR_FLASH_BASE 0x00000000
#define MAPU_NOR_FLASH_SIZE SZ_256M

/*
 *  SDRAM
 */
#define MAPU_SDRAM_BASE           0x20000000

/*
 *  DDR 3
 */

#define MAPU_A8_DDR3_BANK0_BASE   0x70000000
#define MAPU_A8_DDR3_BANK1_BASE   0x80000000

/*
 *  Logic expansion modules
 *
 */

#define MAPU_SYS_BASE             0x40000000

/*
 * RealView common peripheral addresses
 */
#define MAPU_A8_DMACTRL_BASE      0x50000000  /* DMA controller */
#define MAPU_A8_UART0_BASE        0x50900000  /* UART 0 */
#define MAPU_A8_UART1_BASE        0x50910000  /* UART 1 */
#define MAPU_A8_UART2_BASE        0x50920000  /* UART 2 */
#define MAPU_A8_TIMER0_BASE       0x50400000  /* Timer */
#define MAPU_A8_WATCHDOG_BASE     0x50440000  /* Watchdog */
#define MAPU_A8_RTC_BASE          0x50480000  /* RTC */
#define MAPU_A8_GIC_CPU_BASE      0x547F0000  /* Generic interrupt controller CPU interface */
#define MAPU_A8_GIC_DIST_BASE     0x547F1000  /* Generic interrupt controller distributor */

#endif /* __ASM_ARCH_PLATFORM_H */
