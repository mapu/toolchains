/*
 * loader.h
 *
 *  Created on: Jun 4, 2014
 *      Author: wangl
 */

#ifndef LOADER_H_
#define LOADER_H_

#include "apc_common.h"
#include "uboot_interface/ape.h"

/**
 * send and fetch, remember to call ape_dma_wait to ensure your data transfer has been completed.
 */
int32_t send_vector(uint32_t dst, uint32_t src, uint32_t len, uint32_t size,
                   uint32_t cpuid);
int32_t send_matrix(uint32_t dst, uint32_t src, uint32_t width, uint32_t height,
                    uint32_t size, uint32_t cpuid);
int32_t send_sub_matrix(uint32_t dst, uint32_t src, uint32_t width, uint32_t sub_width, uint32_t height,
                    uint32_t size, uint32_t cpuid);
int32_t fetch_vector(uint32_t dst, uint32_t src, uint32_t len, uint32_t size,
                     uint32_t cpuid);
int32_t fetch_matrix(uint32_t dst, uint32_t src, uint32_t width, uint32_t height,
                     uint32_t size, uint32_t cpuid);
int32_t fetch_sub_matrix(uint32_t dst, uint32_t src, uint32_t width, uint32_t sub_width, uint32_t height,
                     uint32_t size, uint32_t cpuid);
#endif /* LOADER_H_ */
