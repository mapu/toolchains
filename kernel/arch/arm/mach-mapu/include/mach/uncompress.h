/*
 * uncompress.h
 *
 *  Created on: Nov 17, 2014
 *      Author: wangl
 */

#include <linux/types.h>
#include <linux/serial_reg.h>
#include <mach/platform.h>
#include <mach/hardware.h>

#define IOMEM(x)  ((void __force __iomem *)(x))

u32 *uart;

/* PORT_16C550A, in polled non-fifo mode */
static void putc(char c)
{
  while (!(uart[UART_LSR] & UART_LSR_THRE))
    barrier();
  uart[UART_TX] = c;
}

static inline void flush(void)
{
  while (!(uart[UART_LSR] & UART_LSR_THRE))
    barrier();
}

/*
 * nothing to do
 */
#define arch_decomp_setup() do { uart = IOMEM(MAPU_A8_UART0_BASE); } while(0)
#define arch_decomp_wdog()
