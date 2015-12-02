/*
 * intr_ctrl.h
 *
 *  Created on: Jan 15, 2014
 *      Author: wangl
 */

#ifndef INTR_CTRL_H_
#define INTR_CTRL_H_

#define GIC
#ifdef GIC
enum GIC_DIST_MMAP {
  GIC_DIST_CTRL          = 0x000,
  GIC_DIST_CTR           = 0x004,
  GIC_DIST_ENABLE_SET    = 0x100,
  GIC_DIST_ENABLE_CLEAR  = 0x180,
  GIC_DIST_PENDING_SET   = 0x200,
  GIC_DIST_PENDING_CLEAR = 0x280,
  GIC_DIST_ACTIVE_BIT    = 0x300,
  GIC_DIST_PRI           = 0x400,
  GIC_DIST_TARGET        = 0x800,
  GIC_DIST_CONFIG        = 0xc00,
  GIC_DIST_SOFTINT       = 0xf00,
};
enum GIC_CPU_MMAP {
  GIC_CPU_CTRL       = 0x00,
  GIC_CPU_PRIMASK    = 0x04,
  GIC_CPU_BINPOINT   = 0x08,
  GIC_CPU_INTACK     = 0x0c,
  GIC_CPU_EOI        = 0x10,
  GIC_CPU_RUNNINGPRI = 0x14,
  GIC_CPU_HIGHPRI    = 0x18,
};
#define PRIORITY_LEVEL_WIDTH 5
#define PRIORITY_SHIFT (8 - PRIORITY_LEVEL_WIDTH)
#define LOWEST_PRIORITY ((0xFF << PRIORITY_SHIFT) & 0xFF)
#else
enum INTR_CTRL_MMAP {
  IRQ_INTEN_L       = 0x00,
  IRQ_INTEN_H       = 0x04,
  IRQ_INTMASK_L     = 0x08,
  IRQ_INTMASK_H     = 0x0C,
  IRQ_FORCE_L       = 0x10,
  IRQ_FORCE_H       = 0x14,
  IRQ_RAWSTATUS_L   = 0x18,
  IRQ_RAWSTATUS_H   = 0x1C,
  IRQ_STATUS_L      = 0x20,
  IRQ_STATUS_H      = 0x24,
  IRQ_MASKSTATUS_L  = 0x28,
  IRQ_MASKSTATUS_H  = 0x2C,
  IRQ_FINALSTATUS_L = 0x30,
  IRQ_FINALSTATUS_H = 0x34,
  IRQ_VECTOR        = 0x38,
  IRQ_VECTOR_0      = 0x40,
  IRQ_VECTOR_1      = 0x48,
  IRQ_VECTOR_2      = 0x50,
  IRQ_VECTOR_3      = 0x58,
  IRQ_VECTOR_4      = 0x60,
  IRQ_VECTOR_5      = 0x68,
  IRQ_VECTOR_6      = 0x70,
  IRQ_VECTOR_7      = 0x78,
  IRQ_VECTOR_8      = 0x80,
  IRQ_VECTOR_9      = 0x88,
  IRQ_VECTOR_10     = 0x90,
  IRQ_VECTOR_11     = 0x98,
  IRQ_VECTOR_12     = 0xA0,
  IRQ_VECTOR_13     = 0xA8,
  IRQ_VECTOR_14     = 0xB0,
  IRQ_VECTOR_15     = 0xB8,
  FIQ_INTEN         = 0xC0,
  FIQ_INTMASK       = 0xC4,
  FIQ_FORCE         = 0xC8,
  FIQ_RAWSTATUS     = 0xCC,
  FIQ_STATUS        = 0xD0,
  FIQ_FINALSTATUS   = 0xD4,
  IRQ_PLEVEL        = 0xD8,
  APB_ICTL_COMP_VERSION = 0xE0,
  IRQ_PR_START      = 0xE8,
  IRQ_VECTOR_DEFAULT = 0x1E8,
};
#endif

#endif /* INTR_CTRL_H_ */
