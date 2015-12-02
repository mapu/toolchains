/*
 * partitions.h
 *
 *  Created on: Jan 16, 2014
 *      Author: wangl
 */

#ifndef PARTITIONS_H_
#define PARTITIONS_H_

enum PARTITION {
  BOOT_PARTITION = 0x0,
  SPU_ELF_PARTITION = 0x400000,
  MPU_ELF_PARTITION = 0x800000,
  DATA_PARTITION = 0xC00000,
};

#define IMSIZE 0x20000
#define MIMSIZE 0x10000
#define DMSIZE 0x20000

#define MAX_SELF_SIZE 0x60000 // (IMSIZE + DMSIZE * 2)
#define MAX_MELF_SIZE 0x10000 // MIMSIZE

#endif /* PARTITIONS_H_ */
