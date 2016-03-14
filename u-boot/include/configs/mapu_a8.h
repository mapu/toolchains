/*
 * (C) Copyright 2011 Linaro
 * Ryan Harkin, <ryan.harkin@linaro.org>
 *
 * Configuration for Versatile Express. Parts were derived from other ARM
 *   configurations.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __MAPU_A8_H
#define __MAPU_A8_H

#define CONFIG_SYS_TIMER_RATE   41200000ULL
#define CONFIG_SYS_NS16550_COM1   0x50920000 /* Base EVM has UART0 */
#include "mapu_common.h"

#endif /* MAPU_A8_H */
