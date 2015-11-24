/*
 * debug-macro.S
 *
 *  Created on: Nov 17, 2014
 *      Author: wangl
 */

#include <linux/serial_reg.h>
#include <mach/hardware.h>
#include <mach/platform.h>

#define UART_SHIFT 2

		.macro	addruart, rp, rv, tmp
		ldr	\rv, =(MAPU_A8_UART0_BASE - 0x50000000 + 0xFD000000)
		ldr	\rp, =MAPU_A8_UART0_BASE
		.endm

		.macro	senduart,rd,rx
		str	\rd, [\rx, #UART_TX << UART_SHIFT]
		.endm

		.macro	busyuart,rd,rx
1002:
    ldr	\rd, [\rx, #UART_LSR << UART_SHIFT]
		and	\rd, \rd, #UART_LSR_TEMT | UART_LSR_THRE
		teq	\rd, #UART_LSR_TEMT | UART_LSR_THRE
		bne	1002b
		.endm

		/* The UART's don't have any flow control IO's wired up. */
		.macro	waituart,rd,rx
		.endm

