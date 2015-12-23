/*
 * loader.h
 *
 *  Created on: Jun 4, 2014
 *      Author: wangl
 */

#ifndef LOADER_H_
#define LOADER_H_

enum DMA_GROUPS {
  SPU_LD_SECT_GP = 0,
  MPU_LD_SECT_GP,
  DATA_E2I_GP,
  DATA_I2E_GP,
  DATA_E2E_GP,
};

enum DATA_TYPE {
  DT_BYTE = 0,
  DT_HALF,
  DT_WORD,
  DT_DWORD,
  DT_QWORD,
};

#define MIM_BASE 0x200000
#define MC_LENGTH 41
#define MAX_MC_NUM 16

inline int32_t apcapi_core(const char *api_name, void *param, uint32_t cpuid);
int32_t load_spu(const char *funcname, void *param, uint32_t cpuid);
int32_t set_spu_section(uint32_t dst, uint32_t value, uint32_t size, uint32_t cpuid);
int32_t load_spu_section(uint32_t dst, uint32_t src, uint32_t size, uint32_t cpuid);
int32_t load_mpu_section(uint32_t src, uint32_t size, uint32_t cpuid);
int32_t send_vector(uint32_t dst, uint32_t src, uint32_t len, uint32_t size,
                   uint32_t cpuid);
int32_t send_matrix(uint32_t dst, uint32_t src, uint32_t width, uint32_t height,
                    uint32_t size, uint32_t cpuid);
int32_t fetch_vector(uint32_t dst, uint32_t src, uint32_t len, uint32_t size,
                     uint32_t cpuid);
int32_t fetch_matrix(uint32_t dst, uint32_t src, uint32_t width, uint32_t height,
                     uint32_t size, uint32_t cpuid);
#endif /* LOADER_H_ */
