/*
 * apc.h
 *
 *  Created on: Nov 26, 2015
 *      Author: wangl
 */

#ifndef DRIVERS_APC_APC_H_
#define DRIVERS_APC_APC_H_

#include <linux/ioctl.h>

enum CSU_CMD {
  CMD_NONE = 0,
  CMD_START = 1,
  CMD_STOP = 2,
  CMD_SHUTDOWN = 3
};

#pragma pack(push, 4)
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
  long DMAGlobalAddr;
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
  unsigned int DMACmd;        //0x78
  union {
    struct {
      unsigned int pad13;
      unsigned int DMACommandStatus;       //0x80
    };
    struct dma_if *Next;
  };
};

struct csu_interface {
  unsigned int VPUControl;             //0x00
  unsigned int pad0;
  unsigned int VPUStatus;              //0x08
  unsigned int pad1;
  struct dma_if dma;                   //0x10
  unsigned int pad3;
  unsigned int DMAQueueNum;            //0x88
  unsigned int pad4;
  unsigned int DMAQueryMask;           //0x90
  unsigned int pad5;
  unsigned int DMAQueryType;           //0x98
  unsigned int pad6;
  unsigned int DMAQueryStatus;         //0xA0
  unsigned int pad7;
  unsigned int MailBoxIn;              //0xA8
  unsigned int pad8;
  unsigned int MailBoxOut;             //0xB0
  unsigned int pad9;
  unsigned int MailBoxOutInt;          //0xB8
  unsigned int pad10;
  unsigned int MailNum;                //0xC0
  unsigned int pad11;
  unsigned int DMAGrpIntClr;           //0xC8
};

union csu_mmap {
  struct csu_interface csu_if;
  unsigned int mem[52];
};
#pragma pack(pop)


struct memalloc_args{
  dma_addr_t phy;
  void *virt;
};


#define APC_IOC_MAGIC 'M'
#define APC_IOCWAITDMA _IOW(APC_IOC_MAGIC, 0, int)
#define APC_IOCMEMALLOC _IO(APC_IOC_MAGIC, 1)
#define APC_IOCMEMFREE _IO(APC_IOC_MAGIC, 2)

#endif /* DRIVERS_APC_APC_H_ */
