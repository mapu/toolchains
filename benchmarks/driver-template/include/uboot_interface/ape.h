/*
 * ape.h
 *
 *  Created on: Jun 3, 2014
 *      Author: wangl
 */

#ifndef APE_H_
#define APE_H_

#include "apc_common.h"

/**
 * this function load spu image to your denoted ape, and set params segment for your argument purpose.
 * after the load, start executing. and the mpu codes are loaded by spu in your spu code.
 */
int ape_load_and_start(char *spuImgPath, uint32_t cpuid, void *params);
/**
 * Allocate a memory area of data for DMA use.
 * DMA should use a special prepared memory region,
 * memory allocated by standard C library functions malloc etc. won't be accepted.
 */
void* dma_malloc(uint32_t size);
/**
 * free the allocated memory
 */
void dma_free(void* addr);
/**
 * if this ape is not acquired, try to acquire it and return result.
 * if already acquired, return true.
 */
int32_t ape_assert_acquire(uint32_t cpuid);
/**
 * release the exclusion priority
 */
void ape_release(uint32_t cpuid);
/**
 * request a dma by directly specify a dma_if.
 * for security purposes, you can only specify a DMAGlobalAddr to an area allocated by dma_malloc
 */
int32_t ape_dma_request(struct dma_if *dma_conf, uint32_t cpuid);
/**
 * wait all the started dma requests to finish.
 */
int32_t ape_dma_wait(uint32_t cpuid);

/**
 * the status of ape (running or not)
 */
int32_t ape_status(uint32_t cpuid);
/**
 * this function will do nothing
 */
int32_t ape_stop(uint32_t cpuid);
/**
 * start ape without loading spu img into it.
 * user should be very careful when using this function.
 */
int32_t ape_start(uint32_t cpuid);
/**
 * wait this ape to finish
 */
int32_t ape_wait(uint32_t cpuid);

#endif /* APE_H_ */
