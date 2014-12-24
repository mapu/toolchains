/*
 * mapu.h
 *
 *  Created on: Nov 16, 2013
 *      Author: wangl
 */

#ifndef MAPU_H_
#define MAPU_H_

#include <linux/ioctl.h>
#ifdef __KERNEL__
#include <linux/cdev.h>
#endif

#undef PDEBUG
#ifdef MAPU_DEBUG
#  ifdef __KERNEL__
#    define PDEBUG(fmf, args...) printk( KERN_DEBUG "mapu: ",
       fmt, ## args)
#  else
#    define PDEBUG(fmt, args...) fprintf(stderr, fmt, ## args)
#  endif
#else
#  define PDEBUG(fmt, args...)
#endif

#undef PDEBUGG
#define PDEBUGG(fmt,args...)

#ifdef __KERNEL__
#define MAPU_MAJOR 0
#define MAPU_DEVS 1
#define APE_CORES 8
#define MAX_NUM_PAIRS 16
#endif

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
};

union csu_mmap {
  struct csu_interface csu_if;
  unsigned int mem[49];
};
#pragma pack(pop)

#ifdef __KERNEL__

const long APE_CR_BASE   = 0x41000000;
const long APE_CR_LEN    = 0x10000;
const long APE_CR_OFFSET = 0x400000;

struct packet {
  unsigned char *data;
  unsigned int len;
  long u_addr;
};

struct ape_dev {
  struct packet ape2ddr;
  struct packet ddr2ape;
  union csu_mmap csu_if;
};

struct mapu_dev {
  struct ape_dev *ape_if;
  struct page *ddr[16];
  struct page *shm;
  int pair_id;
  struct semaphore sem;
  struct cdev cdev;
};

extern struct file_operations mapu_fops;

int mapu_trim(struct mapu_dev *dev);

#  ifdef MAPU_DEBUG
#    define MAPU_USE_PROC
#  endif
#endif

#define MAPU_IOC_MAGIC 'M'

#define MAPU_IOCVERIFY _IOW(MAPU_IOC_MAGIC, 0, int)
#define MAPU_IOCAPE2DDR _IOR(MAPU_IOC_MAGIC, 1, int)
#define MAPU_IOCDDR2APE _IOW(MAPU_IOC_MAGIC, 2, int)
#define MAPU_IOCSTATUS _IOW(MAPU_IOC_MAGIC, 3, int)
#define MAPU_IOCSTATUS_CHANGE _IOW(MAPU_IOC_MAGIC, 4, int)
#define MAPU_IOCSIMREG _IOW(MAPU_IOC_MAGIC, 5, int)
#define MAPU_IOCCHECKPAIR _IOR(MAPU_IOC_MAGIC, 6, int)

#define MAPU_IOC_MAXNR 7


#endif /* MAPU_H_ */
