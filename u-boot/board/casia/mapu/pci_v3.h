/*
 *  arch/arm/include/asm/hardware/pci_v3.h
 *
 *  Internal header file PCI V3 chip
 *
 *  Copyright (C) ARM Limited
 *  Copyright (C) 2000-2001 Deep Blue Solutions Ltd.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */
#ifndef ASM_ARM_HARDWARE_PCI_V3_H
#define ASM_ARM_HARDWARE_PCI_V3_H

/* -------------------------------------------------------------------------------
 *  V3 Local Bus to PCI Bridge definitions
 * -------------------------------------------------------------------------------
 *  Registers (these are taken from page 129 of the EPC User's Manual Rev 1.04
 *  All V3 register names are prefaced by V3_ to avoid clashing with any other
 *  PCI definitions.  Their names match the user's manual.
 *
 *  I'm assuming that I20 is disabled.
 *
 */
#define V3_PCI_VENDOR                   0x00000000
#define V3_PCI_DEVICE                   0x00000002
#define V3_PCI_CMD                      0x00000004
#define V3_PCI_STAT                     0x00000006
#define V3_PCI_CC_REV                   0x00000008
#define V3_PCI_HDR_CFG                  0x0000000C
#define V3_PCI_IO_BASE                  0x00000010
#define V3_PCI_BASE0                    0x00000014
#define V3_PCI_BASE1                    0x00000018
#define V3_PCI_SUB_VENDOR               0x0000002C
#define V3_PCI_SUB_ID                   0x0000002E
#define V3_PCI_ROM                      0x00000030
#define V3_PCI_BPARAM                   0x0000003C

/*  PCI COMMAND REGISTER bits
 */
#define V3_COMMAND_M_FBB_EN             (1 << 9)
#define V3_COMMAND_M_SERR_EN            (1 << 8)
#define V3_COMMAND_M_PAR_EN             (1 << 6)
#define V3_COMMAND_M_MASTER_EN          (1 << 2)
#define V3_COMMAND_M_MEM_EN             (1 << 1)
#define V3_COMMAND_M_IO_EN              (1 << 0)

/*  SYSTEM REGISTER bits
 */
#define V3_SYSTEM_M_RST_OUT             (1 << 15)
#define V3_SYSTEM_M_LOCK                (1 << 14)

/*  PCI_CFG bits
 */
#define V3_PCI_CFG_M_I2O_EN		(1 << 15)
#define V3_PCI_CFG_M_IO_REG_DIS		(1 << 14)
#define V3_PCI_CFG_M_IO_DIS		(1 << 13)
#define V3_PCI_CFG_M_EN3V		(1 << 12)
#define V3_PCI_CFG_M_RETRY_EN           (1 << 10)
#define V3_PCI_CFG_M_AD_LOW1            (1 << 9)
#define V3_PCI_CFG_M_AD_LOW0            (1 << 8)

/*  PCI_BASE register bits (PCI -> Local Bus)
 */
#define V3_PCI_BASE_M_ADR_BASE          0xFFF00000
#define V3_PCI_BASE_M_ADR_BASEL         0x000FFF00
#define V3_PCI_BASE_M_PREFETCH          (1 << 3)
#define V3_PCI_BASE_M_TYPE              (3 << 1)
#define V3_PCI_BASE_M_IO                (1 << 0)

/*  PCI MAP register bits (PCI -> Local bus)
 */
#define V3_PCI_MAP_M_MAP_ADR            0xFFF00000
#define V3_PCI_MAP_M_RD_POST_INH        (1 << 15)
#define V3_PCI_MAP_M_ROM_SIZE           (3 << 10)
#define V3_PCI_MAP_M_SWAP               (3 << 8)
#define V3_PCI_MAP_M_ADR_SIZE           0x000000F0
#define V3_PCI_MAP_M_REG_EN             (1 << 1)
#define V3_PCI_MAP_M_ENABLE             (1 << 0)

#define V3_PCI_MAP_M_ADR_SIZE_1MB	(0 << 4)
#define V3_PCI_MAP_M_ADR_SIZE_2MB	(1 << 4)
#define V3_PCI_MAP_M_ADR_SIZE_4MB	(2 << 4)
#define V3_PCI_MAP_M_ADR_SIZE_8MB	(3 << 4)
#define V3_PCI_MAP_M_ADR_SIZE_16MB	(4 << 4)
#define V3_PCI_MAP_M_ADR_SIZE_32MB	(5 << 4)
#define V3_PCI_MAP_M_ADR_SIZE_64MB	(6 << 4)
#define V3_PCI_MAP_M_ADR_SIZE_128MB	(7 << 4)
#define V3_PCI_MAP_M_ADR_SIZE_256MB	(8 << 4)
#define V3_PCI_MAP_M_ADR_SIZE_512MB	(9 << 4)
#define V3_PCI_MAP_M_ADR_SIZE_1GB	(10 << 4)
#define V3_PCI_MAP_M_ADR_SIZE_2GB	(11 << 4)

/*
 *  LB_BASE0,1 register bits (Local bus -> PCI)
 */
#define V3_LB_BASE_ADR_BASE		0xfff00000
#define V3_LB_BASE_SWAP			(3 << 8)
#define V3_LB_BASE_ADR_SIZE		(15 << 4)
#define V3_LB_BASE_PREFETCH		(1 << 3)
#define V3_LB_BASE_ENABLE		(1 << 0)

#define V3_LB_BASE_ADR_SIZE_1MB		(0 << 4)
#define V3_LB_BASE_ADR_SIZE_2MB		(1 << 4)
#define V3_LB_BASE_ADR_SIZE_4MB		(2 << 4)
#define V3_LB_BASE_ADR_SIZE_8MB		(3 << 4)
#define V3_LB_BASE_ADR_SIZE_16MB	(4 << 4)
#define V3_LB_BASE_ADR_SIZE_32MB	(5 << 4)
#define V3_LB_BASE_ADR_SIZE_64MB	(6 << 4)
#define V3_LB_BASE_ADR_SIZE_128MB	(7 << 4)
#define V3_LB_BASE_ADR_SIZE_256MB	(8 << 4)
#define V3_LB_BASE_ADR_SIZE_512MB	(9 << 4)
#define V3_LB_BASE_ADR_SIZE_1GB		(10 << 4)
#define V3_LB_BASE_ADR_SIZE_2GB		(11 << 4)

#define v3_addr_to_lb_base(a)	((a) & V3_LB_BASE_ADR_BASE)

/*
 *  LB_MAP0,1 register bits (Local bus -> PCI)
 */
#define V3_LB_MAP_MAP_ADR		0xfff0
#define V3_LB_MAP_TYPE			(7 << 1)
#define V3_LB_MAP_AD_LOW_EN		(1 << 0)

#define V3_LB_MAP_TYPE_IACK		(0 << 1)
#define V3_LB_MAP_TYPE_IO		(1 << 1)
#define V3_LB_MAP_TYPE_MEM		(3 << 1)
#define V3_LB_MAP_TYPE_CONFIG		(5 << 1)
#define V3_LB_MAP_TYPE_MEM_MULTIPLE	(6 << 1)

/* PCI MAP register bits (PCI -> Local bus) */
#define v3_addr_to_lb_map(a)	(((a) >> 16) & V3_LB_MAP_MAP_ADR)

/*
 *  LB_BASE2 register bits (Local bus -> PCI IO)
 */
#define V3_LB_BASE2_ADR_BASE		0xff00
#define V3_LB_BASE2_SWAP		(3 << 6)
#define V3_LB_BASE2_ENABLE		(1 << 0)

#define v3_addr_to_lb_base2(a)	(((a) >> 16) & V3_LB_BASE2_ADR_BASE)

/*
 *  LB_MAP2 register bits (Local bus -> PCI IO)
 */
#define V3_LB_MAP2_MAP_ADR		0xff00

#define v3_addr_to_lb_map2(a)	(((a) >> 16) & V3_LB_MAP2_MAP_ADR)

#endif
