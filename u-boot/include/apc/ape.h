/*
 * ape.h
 *
 *  Created on: Jun 3, 2014
 *      Author: wangl
 */

#ifndef APE_H_
#define APE_H_

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

#define MAX_NUM_CORES 8
#define MAX_NUM_MAIL_RECV 1
#define MAX_NUM_MAIL_SEND 4
#define MAX_NUM_GROUP 32
#define MAX_NUM_DMA 4

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
