/*
 * (C) Copyright 2012 SAMSUNG Electronics
 * Jaehoon Chung <jh80.chung@samsung.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <malloc.h>
#include <sdhci.h>
#include <asm/arch/mmc.h>
//#include <asm/arch/clk.h>

void set_mmc_clk(int dev_index, unsigned int div)
{
	/* Do NOTHING */
}

static char *MAPU_NAME = "MAPU SDHCI";

int mapu_sdhci_init(u32 regbase, int index, int bus_width)
{
	struct sdhci_host *host = NULL;
	host = (struct sdhci_host *)malloc(sizeof(struct sdhci_host));
	if (!host) {
		printf("sdhci__host malloc fail!\n");
		return 1;
	}

	host->name = MAPU_NAME;
	host->ioaddr = (void *)regbase;

	host->quirks = SDHCI_QUIRK_NO_HISPD_BIT | SDHCI_QUIRK_BROKEN_VOLTAGE |
		SDHCI_QUIRK_BROKEN_R1B | SDHCI_QUIRK_32BIT_DMA_ADDR |
		SDHCI_QUIRK_WAIT_SEND_CMD | SDHCI_QUIRK_USE_WIDE8;
	host->voltages = MMC_VDD_32_33 | MMC_VDD_33_34 | MMC_VDD_165_195;
	host->version = sdhci_readw(host, SDHCI_HOST_VERSION);

	host->set_control_reg = NULL;
	host->set_clock = set_mmc_clk;
	host->index = index;

	host->host_caps = MMC_MODE_HC;
	if (bus_width == 8)
		host->host_caps |= MMC_MODE_8BIT;

	return add_sdhci(host, 52000000, 400000);
}
