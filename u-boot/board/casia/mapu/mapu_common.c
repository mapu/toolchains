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
 * SPDX-License-Identifier:	GPL-2.0+
 */
#include <common.h>
#include <malloc.h>
#include <errno.h>
#include <netdev.h>
#include <pci.h>
#include <asm/io.h>
#ifdef CONFIG_DESIGNWARE_ETH
#include <miiphy.h>
#endif
#ifdef CONFIG_ARM_MAPU_SDHC
#include <asm/arch/mmc.h>
#endif
#include <asm/arch/systimer.h>
#include <asm/arch/wdt.h>
#include <asm/arch/dma.h>
#include <asm/armv7.h>


#include "dm/uclass.h"
#include "dm/device.h"
#include "asm-generic/gpio.h"

static struct systimer *systimer_base = (struct systimer *)V2M_TIMER01;
extern void apc_init(void);

static inline unsigned int get_copro_access(void)
{
  unsigned int val;
  asm("mrc p15, 0, %0, c1, c0, 2 @ get copro access"
      : "=r" (val) : : "cc");
  return val;
}

static inline void set_copro_access(unsigned int val)
{
  asm volatile("mcr p15, 0, %0, c1, c0, 2 @ set copro access"
               : : "r" (val) : "cc");
  CP15ISB;
}

#define vfpreg(_vfp_) #_vfp_

#define fmrx(_vfp_) ({      \
  unsigned int __v;     \
  asm("mrc p10, 7, %0, " vfpreg(_vfp_) ", cr0, 0 @ fmrx %0, " #_vfp_  \
      : "=r" (__v) : : "cc"); \
  __v;        \
 })

#define fmxr(_vfp_,_var_)   \
  asm("mcr p10, 7, %0, " vfpreg(_vfp_) ", cr0, 0 @ fmxr " #_vfp_ ", %0" \
      : : "r" (_var_) : "cc")
#define FPEXC     cr8
#define FPEXC_EN    (1 << 30)
#define CPACC_FULL(n)   (3 << (n * 2))

static void enable_vfp(void) {
  unsigned int access = get_copro_access();
  set_copro_access(access | CPACC_FULL(10) | CPACC_FULL(11));
  unsigned int fpexc = fmrx(FPEXC);
  fmxr(FPEXC, fpexc | FPEXC_EN);
}

static void mapu_timer_init(void);
DECLARE_GLOBAL_DATA_PTR;

#if defined(CONFIG_SHOW_BOOT_PROGRESS)
void show_boot_progress(int progress)
{
	printf("Boot reached stage %d\n", progress);
}
#endif

static inline void delay(ulong loops)
{
	__asm__ volatile ("1:\n"
		"subs %0, %1, #1\n"
		"bne 1b" : "=r" (loops) : "0" (loops));
}

/*
 * Peripheral Test
 */

void peri_test(void)
{
  /*
   * spi test
   */
  struct udevice *dev;
  char out[] = "out";
  char in[20];
  int ret = spi_xfer(0, 32, out, in, 0);
  printf("\nAfter spi_xfer ret=%d\n", ret);

  /*
   * gpio test
   */
  struct gpio_desc desc;
  ret = dm_gpio_lookup_name("porta0", &desc);
  printf("\nAfter dm_gpio_lookup_name ret=%d\n", ret);
  ret = dm_gpio_request(&desc, "dw_test");
  printf("\nAfter dm_gpio_request ret=%d\n", ret);
  ret = dm_gpio_set_dir_flags(&desc, GPIOD_IS_OUT);
  printf("\nAfter dm_gpio_set_dir_flags ret=%d\n", ret);
  ret = dm_gpio_set_value(&desc, 1);
  printf("\nAfter dm_gpio_set_value ret=%d\n", ret);
  ret = gpio_direction_output(0, 1);
  printf("\nAfter gpio_direction_output ret=%d\n", ret);
  ret = gpio_direction_input(0);
  printf("\nAfter gpio_direction_input ret=%d\n", ret);

  ret = dm_gpio_lookup_name("porta1", &desc);
  printf("\nAfter dm_gpio_lookup_name ret=%d\n", ret);
  ret = dm_gpio_request(&desc, "prota");
  printf("\nAfter dm_gpio_request ret=%d\n", ret);
  ret = dm_gpio_set_dir_flags(&desc, GPIOD_IS_OUT);
  printf("\nAfter dm_gpio_set_dir_flags ret=%d\n", ret);
  ret = dm_gpio_set_value(&desc, 1);
  printf("\nAfter dm_gpio_set_value ret=%d\n", ret);
  ret = gpio_direction_output(1, 1);
  printf("\nAfter gpio_direction_output ret=%d\n", ret);
  ret = gpio_direction_input(1);
  printf("\nAfter gpio_direction_input ret=%d\n", ret);

  /*
   * i2c test
   */
  ret = i2c_probe(0);
  printf("\nAfter i2c_probe ret=%d\n", ret);
}

int board_init(void)
{
	gd->bd->bi_boot_params = LINUX_BOOT_PARAM_ADDR;
	gd->bd->bi_arch_number = MACH_TYPE_MAPU;
	gd->flags = 0;

	//icache_enable();
	enable_vfp();
	mapu_timer_init();
	mapu_dma_init();  // dma init
	apc_init();

	peri_test();

	return 0;
}

/*******************************************************************/
int board_eth_init(bd_t *bis)
{
	int rc = 0;
#ifdef CONFIG_DESIGNWARE_ETH
	setenv("ethaddr", "74:D4:35:C0:C5:C9");
	u32 interface = PHY_INTERFACE_MODE_MII;
	if (designware_initialize(0, CONFIG_SPEAR_ETHBASE, CONFIG_DW0_PHY,
				interface) >= 0)
		rc++;
#endif
	return rc;
}

int cpu_mmc_init(bd_t *bis)
{
	int rc = 0;
	(void) bis;
#ifdef CONFIG_ARM_MAPU_SDHC
	rc = mapu_sdhci_init(CONFIG_ARM_MAPU_SDHC_BASE, 0, 4);
#endif
	return rc;
}

int dram_init(void)
{
	gd->ram_size =
		get_ram_size((long *)CONFIG_SYS_SDRAM_BASE, PHYS_SDRAM_1_SIZE);
	return 0;
}

void dram_init_banksize(void)
{
	gd->bd->bi_dram[0].start = PHYS_SDRAM_1;
	gd->bd->bi_dram[0].size =
			get_ram_size((long *)PHYS_SDRAM_1, PHYS_SDRAM_1_SIZE);
	/*gd->bd->bi_dram[1].start = PHYS_SDRAM_2;
	gd->bd->bi_dram[1].size =
			get_ram_size((long *)PHYS_SDRAM_2, PHYS_SDRAM_2_SIZE);*/
}

int timer_init(void)
{
	return 0;
}

/*
 * Start timer:
 *    Setup a 32 bit timer, running at 1KHz
 *    Versatile Express Motherboard provides 1 MHz timer
 */
static void mapu_timer_init(void)
{
	/*
	 * Set Timer0 to be:
	 *   Enabled, free running, no interrupt, user-mode
	 */
	writel(SYSTIMER_RELOAD, &systimer_base->timer0load);
	writel(SYSTIMER_EN | 4, &systimer_base->timer0control);
}

int v2m_cfg_write(u32 devfn, u32 data)
{
	/* Configuration interface broken? */
	u32 val;

	devfn |= SYS_CFG_START | SYS_CFG_WRITE;

	val = readl(V2M_SYS_CFGSTAT);
	writel(val & ~SYS_CFG_COMPLETE, V2M_SYS_CFGSTAT);

	writel(data, V2M_SYS_CFGDATA);
	writel(devfn, V2M_SYS_CFGCTRL);

	do {
		val = readl(V2M_SYS_CFGSTAT);
	} while (val == 0);

	return !!(val & SYS_CFG_ERR);
}

/* Use the ARM Watchdog System to cause reset */
void reset_cpu(ulong addr)
{
	if (v2m_cfg_write(SYS_CFG_REBOOT | SYS_CFG_SITE_MB, 0))
		printf("Unable to reboot\n");
}

void lowlevel_init(void)
{
}

ulong get_board_rev(void){
	return readl((u32 *)SYS_ID);
}

#if defined(CONFIG_ARMV7_NONSEC) || defined(CONFIG_ARMV7_VIRT)
/* Setting the address at which secondary cores start from.
 * Versatile Express uses one address for all cores, so ignore corenr
 */
void smp_set_core_boot_addr(unsigned long addr, int corenr)
{
	/* The SYSFLAGS register on VExpress needs to be cleared first
	 * by writing to the next address, since any writes to the address
	 * at offset 0 will only be ORed in
	 */
	writel(~0, CONFIG_SYSFLAGS_ADDR + 4);
	writel(addr, CONFIG_SYSFLAGS_ADDR);
}
#endif

