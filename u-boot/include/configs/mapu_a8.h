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

/******************** NET command ************************/
/* Ethernet driver configuration */
#define CONFIG_MII
#define CONFIG_DESIGNWARE_ETH
//#define CONFIG_DW_SEARCH_PHY
#define CONFIG_SPEAR_ETHBASE        0x50C00000
#define CONFIG_DW0_PHY				1
#define CONFIG_NET_MULTI
#define CONFIG_PHY_RESET_DELAY	    10000		/* in usec */
#define CONFIG_DW_ALTDESCRIPTOR
//#define CONFIG_DW_AUTONEG
//#define CONFIG_PHY_GIGE			/* Include GbE speed/duplex detection */

#define CONFIG_SYS_TIMER_RATE   100000000ULL

#include "mapu_common.h"

#endif /* MAPU_A8_H */
