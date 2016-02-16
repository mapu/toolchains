/*
 * ape.c
 *
 *  Created on: Jun 3, 2014
 *      Author: wangl
 */

#include <common.h>
#include <asm/io.h>
#include <apc/ape.h>

uint32_t num_cores = 0;
static union csu_mmap *ape_cores[MAX_NUM_CORES];

uint32_t current_ape = MAX_NUM_CORES;

int ape_dma_request(struct dma_if *dma_conf, uint32_t cpuid) {
  if (cpuid >= num_cores)
    return -1;
  if (dma_conf->DMAGroupNum >= MAX_NUM_GROUP)
    return -1;
  while (readl((uint32_t)&(ape_cores[cpuid]->csu_if.dma.DMACommandStatus)) != 0 ||
         readl((uint32_t)&(ape_cores[cpuid]->csu_if.DMAQueueNum)) == 0);
  writel(dma_conf->DMALocalAddr, (uint32_t)&(ape_cores[cpuid]->csu_if.dma.DMALocalAddr));
  writel(dma_conf->DMALocalXNum, (uint32_t)&(ape_cores[cpuid]->csu_if.dma.DMALocalXNum));
  writel(dma_conf->DMALocalYStep, (uint32_t)&(ape_cores[cpuid]->csu_if.dma.DMALocalYStep));
  writel(dma_conf->DMALocalYNum, (uint32_t)&(ape_cores[cpuid]->csu_if.dma.DMALocalYNum));
  writel(dma_conf->DMALocalZStep, (uint32_t)&(ape_cores[cpuid]->csu_if.dma.DMALocalZStep));
  writel(dma_conf->DMALocalYAllNum, (uint32_t)&(ape_cores[cpuid]->csu_if.dma.DMALocalYAllNum));
  writel(dma_conf->DMAGlobalAddr, (uint32_t)&(ape_cores[cpuid]->csu_if.dma.DMAGlobalAddr));
  writel(dma_conf->DMAGlobalXNum, (uint32_t)&(ape_cores[cpuid]->csu_if.dma.DMAGlobalXNum));
  writel(dma_conf->DMAGlobalYStep, (uint32_t)&(ape_cores[cpuid]->csu_if.dma.DMAGlobalYStep));
  writel(dma_conf->DMAGlobalYNum, (uint32_t)&(ape_cores[cpuid]->csu_if.dma.DMAGlobalYNum));
  writel(dma_conf->DMAGlobalZStep, (uint32_t)&(ape_cores[cpuid]->csu_if.dma.DMAGlobalZStep));
  writel(dma_conf->DMAGlobalYAllNum, (uint32_t)&(ape_cores[cpuid]->csu_if.dma.DMAGlobalYAllNum));
  writel(dma_conf->DMAGroupNum, (uint32_t)&(ape_cores[cpuid]->csu_if.dma.DMAGroupNum));
  writel(dma_conf->DMACmd, (uint32_t)&(ape_cores[cpuid]->csu_if.dma.DMACmd));
  return 0;
}

int32_t ape_dma_wait(uint32_t group, uint32_t cpuid) {
  if (group >= MAX_NUM_GROUP || cpuid >= num_cores)
    return -1;
  while (readl((uint32_t)&(ape_cores[cpuid]->csu_if.dma.DMACommandStatus)) != 0);
  uint32_t mask = ~(1UL << group);
  writel(mask, (uint32_t)&(ape_cores[cpuid]->csu_if.DMAQueryMask));
  do {
    writel(DMAQ_OR, (uint32_t)&(ape_cores[cpuid]->csu_if.DMAQueryType));
  } while (readl((uint32_t)&(ape_cores[cpuid]->csu_if.DMAQueryStatus)) == 0);
  return 0;
}

int32_t ape_dma_status(uint32_t cpuid) {
  if (cpuid >= num_cores) return -1;
  uint32_t mask = 0;
  writel(mask, (uint32_t)&(ape_cores[cpuid]->csu_if.DMAQueryMask));
  return readl((uint32_t)&(ape_cores[cpuid]->csu_if.DMAQueryStatus));
}

int32_t ape_mail_status(uint32_t cpuid) {
  if (cpuid >= num_cores) return -1;
  return readl((uint32_t)&(ape_cores[cpuid]->csu_if.MailNum)); // {rev., out, in, out_int}
}

int32_t ape_status(uint32_t cpuid) {
  if (cpuid >= num_cores) return -1;
  return readl((uint32_t)&(ape_cores[cpuid]->csu_if.VPUStatus)); // 1: Busy, 0: Idle
}

int32_t ape_stop(uint32_t cpuid) {
  if (cpuid >= num_cores) return -1;
  writel(CMD_STOP, (uint32_t)&(ape_cores[cpuid]->csu_if.VPUControl));
  return 0;
}

int32_t ape_start(uint32_t cpuid) {
  if (cpuid >= num_cores) return -1;
  writel(CMD_START, (uint32_t)&(ape_cores[cpuid]->csu_if.VPUControl));
  return 0;
}

int32_t ape_wait(uint32_t cpuid) {
  if (cpuid >= num_cores) return -1;
  while (readl((uint32_t)&(ape_cores[cpuid]->csu_if.VPUStatus)));
  return 0;
}

/* Initial ape cores */
void apc_init(void) {
  int i = 0;
  num_cores = 4;

  for (i = 0; i < num_cores; i++) {
    ape_cores[i] = (union csu_mmap *)(APE0_REG_START + i * 0x400000);
    while ((readl((uint32_t)&(ape_cores[i]->csu_if.MailNum)) & 0xFF00FF) != 0) {
      readl((uint32_t)&(ape_cores[i]->csu_if.MailBoxOut));
      readl((uint32_t)&(ape_cores[i]->csu_if.MailBoxOutInt));
    }
    if ((readl((uint32_t)&(ape_cores[i]->csu_if.MailNum)) & 0xFF00) !=
        (MAX_NUM_MAIL_SEND << 8))
      debug("warning: APE%d has pending incoming mails.\n", i);
    writel(0xFFFFFFFF, (uint32_t)&(ape_cores[i]->csu_if.DMAGrpIntClr));
  }
}
