/*
 * mapu.hh
 *
 *  Created on: Nov 16, 2013
 *      Author: wangl
 */

#ifndef MAPU_HH_
#define MAPU_HH_

#include <linux/ioctl.h>
#include "../../../../ext/mapudrv/mapu.h"

struct inbox_if {
  unsigned int Message[4];
  unsigned int queue_len;
  unsigned int ptr;
};
struct csu_internal_if {
  unsigned int *EventStatus;
  unsigned int EventMaskWrite;
  unsigned int EventAck;
  unsigned int EventMaskRead;
  struct dma_if dma;
  struct dma_if *dma_queue;
  unsigned int dma_queue_len;
  unsigned int GroupMaskWrite;
  unsigned int GroupMaskRead;
  unsigned int GroupStatusUpdate;
  unsigned int *GroupStatus;
  unsigned int *GroupListStalled;
  unsigned int GroupListAck;
  unsigned int *MailboxOut;
  struct inbox_if *MailboxIn;
  unsigned int *MailboxOutInt;
};

struct csu_external_if {
  bool VPUStatus;
  struct dma_if *dma_queue;
  unsigned int dma_queue_len;
};

enum CSU_Space {
  APE0START = 0,
  APE0END   = 0xFFFFFF,
  APE1START = 0x1000000,
  APE1END   = 0x1FFFFFF,
  APE2START = 0x2000000,
  APE2END   = 0x2FFFFFF,
  APE3START = 0x3000000,
  APE3END   = 0x3FFFFFF,
  APE4START = 0x4000000,
  APE4END   = 0x4FFFFFF,
  APE5START = 0x5000000,
  APE5END   = 0x5FFFFFF,
  APE6START = 0x6000000,
  APE6END   = 0x6FFFFFF,
  APE7START = 0x7000000,
  APE7END   = 0x7FFFFFF,
  SHMSTART  = 0x40400000,
  SHMEND    = 0x407FFFFF,
  DDR0START = 0x60000000,
  DDR0RESV  = 0x70000000,
  DDR0END   = 0x7FFFFFFF,
};

#endif /* MAPU_H_ */
