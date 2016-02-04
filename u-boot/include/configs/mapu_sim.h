/*
 * (C) Copyright 2011 Linaro
 * Ryan Harkin, <ryan.harkin@linaro.org>
 *
 * Configuration for Versatile Express. Parts were derived from other ARM
 *   configurations.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __MAPU_SIM_H
#define __MAPU_SIM_H

/* NET command */
#define MAPU_SIM

#define CONFIG_E1000
#define CONFIG_E1000_NO_NVM
/* #define CONFIG_E1000_SPI */
/* #define CONFIG_E1000_SPI_GENERIC */
/* #define CONFIG_CMD_E1000 */

/* PCI-E dual-port E1000 (external ethernet ports) */
#define CONFIG_PCI
#define CONFIG_PCI1

/*SD Card*/
#define MAPU_SDHC		0x50D00000
#define CONFIG_ARM_MAPU_SDHC
#define CONFIG_ARM_MAPU_SDHC_BASE	MAPU_SDHC

/* MMC */
#define CONFIG_GENERIC_MMC
#define CONFIG_MMC
#define CONFIG_SDHCI
#define CONFIG_MAPU_SDHCI
#define CONFIG_MMC_TRACE
#define CONFIG_CMD_MMC

#define CONFIG_SYS_TIMER_RATE   100000000ULL  // 1000*100 is suitable for simulator

#include "mapu_common.h"

#endif /* MAPU_SIM_H */
