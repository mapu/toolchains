/*
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * (C) Copyright 2002
 * David Mueller, ELSOFT AG, <d.mueller@elsoft.ch>
 *
 * (C) Copyright 2003
 * Texas Instruments, <www.ti.com>
 * Kshitij Gupta <Kshitij@ti.com>
 *
 * (C) Copyright 2004
 * ARM Ltd.
 * Philippe Robin, <philippe.robin@arm.com>
 *
 * (C) Copyright 2011
 * Linaro
 * Linus Walleij <linus.walleij@linaro.org>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */
#include <common.h>
#include <pci.h>
#include <asm/io.h>
#include "pci_v3.h"

/*
 * These are in the physical addresses on the CPU side, i.e.
 * where we read and write stuff - you don't want to try to
 * move these around
 */
#define PHYS_PCI_MEM_BASE	0x30000000
#define PHYS_PCI_IO_BASE	0x30000000	/* PCI I/O space base */
#define PHYS_PCI_CONFIG_BASE	0x30000000
#define PHYS_PCI_V3_BASE	0x30000000	/* V360EPC registers */
#define SZ_256M			0x10000000

/*
 * Initialize PCI Devices, report devices found.
 */

#ifndef CONFIG_PCI_PNP
#define PCI_ENET0_IOADDR	0x60000000 /* First card in PCI I/O space */
#define PCI_ENET0_MEMADDR	0x40000000 /* First card in PCI memory space */
static struct pci_config_table pci_integrator_config_table[] = {
	{ PCI_ANY_ID, PCI_ANY_ID, PCI_ANY_ID, PCI_ANY_ID, 0x0f, PCI_ANY_ID,
	  pci_cfgfunc_config_device, { PCI_ENET0_IOADDR,
				       PCI_ENET0_MEMADDR,
				       PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER }},
	{ }
};
#endif /* CONFIG_PCI_PNP */

/* V3 access routines */
#define v3_writeb(o, v) __raw_writeb(v, PHYS_PCI_V3_BASE + (unsigned int)(o))
#define v3_readb(o)    (__raw_readb(PHYS_PCI_V3_BASE + (unsigned int)(o)))

#define v3_writew(o, v) __raw_writew(v, PHYS_PCI_V3_BASE + (unsigned int)(o))
#define v3_readw(o)    (__raw_readw(PHYS_PCI_V3_BASE + (unsigned int)(o)))

#define v3_writel(o, v) __raw_writel(v, PHYS_PCI_V3_BASE + (unsigned int)(o))
#define v3_readl(o)    (__raw_readl(PHYS_PCI_V3_BASE + (unsigned int)(o)))

static unsigned long v3_open_config_window(pci_dev_t bdf, int offset)
{
	unsigned int address, mapaddress;
	unsigned int busnr = PCI_BUS(bdf);
	unsigned int devfn = PCI_FUNC(bdf);

	/*
	 * Trap out illegal values
	 */
	if (offset > 255)
		BUG();
	if (busnr > 255)
		BUG();
	if (devfn > 255)
		BUG();

	if (busnr == 0) {
		/*
		 * Linux calls the thing U-Boot calls "DEV" "SLOT"
		 * instead, but it's the same 5 bits
		 */
		int slot = PCI_DEV(bdf);

		/*
		 * local bus segment so need a type 0 config cycle
		 *
		 * build the PCI configuration "address" with one-hot in
		 * A31-A11
		 *
		 * mapaddress:
		 *  3:1 = config cycle (101)
		 *  0   = PCI A1 & A0 are 0 (0)
		 */
		address = PCI_FUNC(bdf) << 16;
		address |= slot << 19;
	} else {
		/*
		 * not the local bus segment so need a type 1 config cycle
		 *
		 * address:
		 *  23:16 = bus number
		 *  15:11 = slot number (7:3 of devfn)
		 *  10:8  = func number (2:0 of devfn)
		 *
		 * mapaddress:
		 *  3:1 = config cycle (101)
		 *  0   = PCI A1 & A0 from host bus (1)
		 */
		mapaddress = V3_LB_MAP_TYPE_CONFIG | V3_LB_MAP_AD_LOW_EN;
		address = (busnr << 16) | (devfn << 8);
	}

	return PHYS_PCI_CONFIG_BASE + address + offset;
}

static void v3_close_config_window(void)
{
}

static int pci_integrator_read_byte(struct pci_controller *hose, pci_dev_t bdf,
				    int offset, unsigned char *val)
{
	unsigned long addr;

	addr = v3_open_config_window(bdf, offset);
	*val = __raw_readb(addr);
	v3_close_config_window();
	return 0;
}

static int pci_integrator_read__word(struct pci_controller *hose,
				     pci_dev_t bdf, int offset,
				     unsigned short *val)
{
	unsigned long addr;

	addr = v3_open_config_window(bdf, offset);
	*val = __raw_readw(addr);
	v3_close_config_window();
	return 0;
}

static int pci_integrator_read_dword(struct pci_controller *hose,
				     pci_dev_t bdf, int offset,
				     unsigned int *val)
{
	unsigned long addr;

	addr = v3_open_config_window(bdf, offset);
	*val = __raw_readl(addr);
	v3_close_config_window();
	return 0;
}

static int pci_integrator_write_byte(struct pci_controller *hose,
				     pci_dev_t bdf, int offset,
				     unsigned char val)
{
	unsigned long addr;

	addr = v3_open_config_window(bdf, offset);
	__raw_writeb((u8)val, addr);
	v3_close_config_window();
	return 0;
}

static int pci_integrator_write_word(struct pci_controller *hose,
				     pci_dev_t bdf, int offset,
				     unsigned short val)
{
	unsigned long addr;

	addr = v3_open_config_window(bdf, offset);
	__raw_writew(val, addr);
	v3_close_config_window();
	return 0;
}

static int pci_integrator_write_dword(struct pci_controller *hose,
				      pci_dev_t bdf, int offset,
				      unsigned int val)
{
	unsigned long addr;

	addr = v3_open_config_window(bdf, offset);
	__raw_writel(val, addr);
	v3_close_config_window();
	return 0;
}

struct pci_controller integrator_hose = {
#ifndef CONFIG_PCI_PNP
	config_table: pci_integrator_config_table,
#endif
};

void pci_init_board(void)
{
	struct pci_controller *hose = &integrator_hose;
	u16 val;

	/* setting this register will take the V3 out of reset */
	//__raw_writel(SC_PCI_PCIEN, SC_PCI);

	/* Wait for 230 ms (from spec) before accessing any V3 registers */
	mdelay(230);

	/*
	 * Configure and register the PCI hose
	 */
	hose->first_busno = 0;
	hose->last_busno = 0xff;

	/* PCI I/O space */
	pci_set_region(hose->regions,
		       0x00000000, PHYS_PCI_IO_BASE, 0x01000000,
		       PCI_REGION_IO);

	/* PCI Memory - config space */
	pci_set_region(hose->regions + 1,
		       0x00000000, PHYS_PCI_CONFIG_BASE, 0x01000000,
		       PCI_REGION_MEM);

	hose->region_count = 5;

	pci_set_ops(hose,
		    pci_integrator_read_byte,
		    pci_integrator_read__word,
		    pci_integrator_read_dword,
		    pci_integrator_write_byte,
		    pci_integrator_write_word,
		    pci_integrator_write_dword);

	pci_register_hose(hose);

	pciauto_config_init(hose);
	pciauto_config_device(hose, 0);

	hose->last_busno = pci_hose_scan(hose);
}
