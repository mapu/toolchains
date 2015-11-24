/*
 * sdhci-mapu.c
 *
 *  Created on: Nov 19, 2014
 *      Author: wangl
 */

#include <linux/err.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/clk.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_gpio.h>
#include <linux/gpio.h>
#include <linux/mmc/card.h>
#include <linux/mmc/host.h>

#include "sdhci-pltfm.h"

#define NVQUIRK_FORCE_SDHCI_SPEC_200  BIT(0)
#define NVQUIRK_ENABLE_BLOCK_GAP_DET  BIT(1)

struct sdhci_mapu_soc_data {
  struct sdhci_pltfm_data *pdata;
  u32 nvquirks;
};

struct sdhci_mapu {
  const struct sdhci_mapu_soc_data *soc_data;
};

static struct sdhci_ops mapu_sdhci_ops = {
};

static struct sdhci_pltfm_data sdhci_mapu_pdata = {
  .quirks = SDHCI_QUIRK_BROKEN_TIMEOUT_VAL |
      SDHCI_QUIRK_SINGLE_POWER_WRITE |
      SDHCI_QUIRK_NO_HISPD_BIT |
      SDHCI_QUIRK_FORCE_DMA,
  .ops  = &mapu_sdhci_ops,
};

static struct sdhci_mapu_soc_data soc_data_mapu = {
  .pdata = &sdhci_mapu_pdata,
  .nvquirks = NVQUIRK_ENABLE_BLOCK_GAP_DET,
};

static const struct of_device_id sdhci_mapu_dt_match[] __devinitdata = {
  { .compatible = "iacas,mapu-sdhci", .data = &soc_data_mapu },
  {}
};
MODULE_DEVICE_TABLE(of, sdhci_dt_ids);

static int __devinit sdhci_mapu_probe(struct platform_device *pdev)
{
  const struct of_device_id *match;
  const struct sdhci_mapu_soc_data *soc_data;
  struct sdhci_host *host;
  struct sdhci_pltfm_host *pltfm_host;
  struct sdhci_mapu *mapu_host;
  struct clk *clk;
  int rc;

  match = of_match_device(sdhci_mapu_dt_match, &pdev->dev);
  if (match)
    soc_data = match->data;
  else
    soc_data = &soc_data_mapu;

  host = sdhci_pltfm_init(pdev, soc_data->pdata);
  if (IS_ERR(host))
    return PTR_ERR(host);

  pltfm_host = sdhci_priv(host);

  mapu_host = devm_kzalloc(&pdev->dev, sizeof(*mapu_host), GFP_KERNEL);
  if (!mapu_host) {
    dev_err(mmc_dev(host->mmc), "failed to allocate mapu_host\n");
    rc = -ENOMEM;
    goto err_no_plat;
  }

  mapu_host->soc_data = soc_data;

  pltfm_host->priv = mapu_host;

  clk = clk_get(mmc_dev(host->mmc), NULL);
  clk_enable(clk);
  pltfm_host->clk = clk;

  host->mmc->pm_caps = 0;

  rc = sdhci_add_host(host);
  if (rc)
    goto err_add_host;

  return 0;

err_add_host:
  clk_disable(pltfm_host->clk);
  clk_put(pltfm_host->clk);
err_no_plat:
  sdhci_pltfm_free(pdev);
  return rc;
}

static int __devexit sdhci_mapu_remove(struct platform_device *pdev)
{
  struct sdhci_host *host = platform_get_drvdata(pdev);
  struct sdhci_pltfm_host *pltfm_host = sdhci_priv(host);
  int dead = (readl(host->ioaddr + SDHCI_INT_STATUS) == 0xffffffff);

  sdhci_remove_host(host, dead);

  clk_disable(pltfm_host->clk);
  clk_put(pltfm_host->clk);

  sdhci_pltfm_free(pdev);

  return 0;
}

static struct platform_driver sdhci_mapu_driver = {
  .driver   = {
    .name = "sdhci-mapu",
    .owner  = THIS_MODULE,
    .of_match_table = sdhci_mapu_dt_match,
    .pm = SDHCI_PLTFM_PMOPS,
  },
  .probe    = sdhci_mapu_probe,
  .remove   = __devexit_p(sdhci_mapu_remove),
};

module_platform_driver(sdhci_mapu_driver);

MODULE_DESCRIPTION("SDHCI driver for MaPU");
MODULE_AUTHOR("IACAS");
MODULE_LICENSE("GPL v2");

