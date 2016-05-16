/*
 * loader.h
 *
 *  Created on: Jun 4, 2014
 *      Author: wangl
 */

#ifndef LOADER_H_
#define LOADER_H_

#include "apc/common.h"

inline int32_t apcapi_core(const char *api_name, void *param, uint32_t cpuid);
int32_t load_spu(const char *funcname, void *param, uint32_t cpuid);
int32_t set_spu_section(uint32_t dst, uint32_t value, uint32_t size, uint32_t cpuid);
int32_t load_spu_section(uint32_t dst, uint32_t src, uint32_t size, uint32_t cpuid);
int32_t load_mpu_section(uint32_t src, uint32_t size, uint32_t cpuid);
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
