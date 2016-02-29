/*
 * common.h
 *
 *  Created on: Feb 23, 2016
 *      Author: luoxq
 */

#ifndef COMMON_H_
#define COMMON_H_

#include <linux/types.h>

enum CSU_CMD {
  CMD_START = 0,
  CMD_STOP = 1,
};

enum DMA_CMD {
  CMD_EXT2INT = 0,
  CMD_INT2EXT = 1,
  CMD_EXT2EXT = 2,
};

enum DMA_QUERY {
  DMAQ_OR = 1,
  DMAQ_AND = 2,
};

#pragma pack(push, 4)
struct dma_if {
  uint32_t DMALocalAddr;
  uint32_t pad0;
  uint32_t DMALocalXNum;
  uint32_t pad1;
  int DMALocalYStep;
  uint32_t pad2;
  uint32_t DMALocalYNum;
  uint32_t pad3;
  int DMALocalZStep;
  uint32_t pad4;
  uint32_t DMALocalYAllNum;
  uint32_t pad5;
  uint32_t DMAGlobalAddr;
  uint32_t pad6;
  uint32_t DMAGlobalXNum;
  uint32_t pad7;
  int DMAGlobalYStep;
  uint32_t pad8;
  uint32_t DMAGlobalYNum;
  uint32_t pad9;
  int DMAGlobalZStep;
  uint32_t pad10;
  uint32_t DMAGlobalYAllNum;
  uint32_t pad11;
  uint32_t DMAGroupNum;
  uint32_t pad12;
  uint32_t DMACmd;        //0x78
  uint32_t pad13;
  uint32_t DMACommandStatus;       //0x80
};

struct csu_interface {
  uint32_t VPUControl;             //0x00
  uint32_t pad0;
  uint32_t VPUStatus;              //0x08
  uint32_t pad1;
  struct dma_if dma;                   //0x10
  uint32_t pad3;
  uint32_t DMAQueueNum;            //0x88
  uint32_t pad4;
  uint32_t DMAQueryMask;           //0x90
  uint32_t pad5;
  uint32_t DMAQueryType;           //0x98
  uint32_t pad6;
  uint32_t DMAQueryStatus;         //0xA0
  uint32_t pad7;
  uint32_t MailBoxIn;              //0xA8
  uint32_t pad8;
  uint32_t MailBoxOut;             //0xB0
  uint32_t pad9;
  uint32_t MailBoxOutInt;          //0xB8
  uint32_t pad10;
  uint32_t MailNum;                //0xC0
  uint32_t pad11;
  uint32_t DMAGrpIntClr;           //0xC8
};

union csu_mmap {
  struct csu_interface csu_if;
  uint32_t mem[49];
};
#pragma pack(pop)

#define APE0_REG_START 0x41000000

#define MAPU_PRCM_BASE 0x50340000
#define APC_PWSW_OFFSET 0x78
#define APC_ISOSW_OFFSET 0x7C
#define APC_RESET_OFFSET 0x38

#define MAX_NUM_CORES 8
#define MAX_NUM_MAIL_RECV 1
#define MAX_NUM_MAIL_SEND 4
#define MAX_NUM_GROUP 32
#define MAX_NUM_DMA 4

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

#endif /* COMMON_H_ */
