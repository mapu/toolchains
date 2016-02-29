/*
 * ape.h
 *
 *  Created on: Jun 3, 2014
 *      Author: wangl
 */

#ifndef APE_H_
#define APE_H_

#include <linux/types.h>
#include "apc/common.h"

int32_t ape_dma_request(struct dma_if *dma_conf, uint32_t cpuid);
int32_t ape_dma_wait(uint32_t group, uint32_t cpuid);
int32_t ape_dma_status(uint32_t cpuid);
int32_t ape_mail_status(uint32_t cpuid);
int32_t ape_status(uint32_t cpuid);
int32_t ape_stop(uint32_t cpuid);
int32_t ape_start(uint32_t cpuid);
int32_t ape_wait(uint32_t cpuid);
void apc_init(void);

#endif /* APE_H_ */
