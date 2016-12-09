/*
 * apc.h
 *
 *  Created on: Nov 26, 2015
 *      Author: wangl
 */

#ifndef DRIVERS_APC_APC_H_
#define DRIVERS_APC_APC_H_

#include <linux/ioctl.h>
#include <linux/types.h>
#include "types.h"

#define APC_DMA_PHY_ADDR 0x80000000
#define APC_DMA_PHY_SIZE 65535
#define APE_IRQID (32+40)
#define NUM_OF_APES 4
#define APE_BUS_BASE 0x41000000
#define APE_REGION_SIZE 0x400000
#define SM_BUS_BASE 0x40400000
#define SM_REGION_SIZE 0x40000
#define DMA_QUENE_CAPACITY 4

#pragma pack(push, 8)
struct dma_if {
  unsigned int DMALocalAddr;
  unsigned int pad0;
  unsigned int DMALocalXNum;
  unsigned int pad1;
  int DMALocalYStep;
  unsigned int pad2;
  unsigned int DMALocalYNum;
  unsigned int pad3;
  int DMALocalZStep;
  unsigned int pad4;
  unsigned int DMALocalYAllNum;
  unsigned int pad5;
  unsigned long long DMAGlobalAddr;
  unsigned int DMAGlobalXNum;
  unsigned int pad7;
  int DMAGlobalYStep;
  unsigned int pad8;
  unsigned int DMAGlobalYNum;
  unsigned int pad9;
  int DMAGlobalZStep;
  unsigned int pad10;
  unsigned int DMAGlobalYAllNum;
  unsigned int pad11;
  unsigned int DMAGroupNum;
  unsigned int pad12;
  unsigned int DMACmd;                 //0x78
  unsigned int pad13;
};

struct csu_if {
  unsigned int VPUControl;             //0x00 0-启动, 1-迫使停止
  unsigned int pad0;
  unsigned int VPUStatus;              //0x08 0-已停止, 1-正在运行
  unsigned int pad1;
  struct dma_if dma;                   //0x10
  unsigned int DMACommandStatus;       //0x80 前一命令是否发送成功 0为发送成功
  unsigned int pad14;
  unsigned int DMAQueueNum;            //0x88
  unsigned int pad2;
  unsigned int DMAQueryMask;           //0x90  对应位为1表示关注
  unsigned int pad3;
  unsigned int DMAQueryType;           //0x98  写: 4是产生中断 | 0是与, 1是或 ;;;;; 读:0完成, 1未完成
  unsigned int pad4;
  unsigned int DMAQueryStatus;         //0xA0  对应位0为未完成
  unsigned int pad5;
  unsigned int MailBoxIn;              //0xA8
  unsigned int pad6;
  unsigned int MailBoxOut;             //0xB0
  unsigned int pad7;
  unsigned int MailBoxOutInt;          //0xB8
  unsigned int pad8;
  unsigned int MailNum;                //0xC0
  unsigned int pad9;
  unsigned int DMAGrpIntClr;           //0xC8  清除对应位的中断, 实现有问题, 不要使用
  unsigned int pad10;
};

union csu_mmap {
  struct csu_if csu_if;
  unsigned int mem[52];
};
#pragma pack(pop)

#define APC_IOC_MAGIC 'M'
#define APC_IOC_QUERY_APE_NUM         _IOWR(APC_IOC_MAGIC, 0, int)
#define APC_IOC_RESET_APE             _IOW(APC_IOC_MAGIC, 2, int)
#define APC_IOC_QUERY_FREE_APES       _IOWR(APC_IOC_MAGIC, 1, int)
#define APC_IOC_ACQUIRE_APE           _IOWR(APC_IOC_MAGIC, 3, int)
#define APC_IOC_RELEASE_APE           _IOWR(APC_IOC_MAGIC, 4, int)
#define APC_IOC_GET_RETURN_VAL        _IOR(APC_IOC_MAGIC, 5, int)
#define APC_IOC_WRITE_DB_ENTRY        _IOW(APC_IOC_MAGIC, 6, int)
#define APC_IOC_WAIT_LAUNCH           _IOR(APC_IOC_MAGIC, 7, int)
#define APC_IOC_WAIT_DMA              _IOR(APC_IOC_MAGIC, 8, int)

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

#endif /* DRIVERS_APC_APC_H_ */
