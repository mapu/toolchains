/*
 * core.c
 *
 *  Created on: Nov 15, 2014
 *      Author: wangl
 */

#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/clkdev.h>
#include <linux/dw_apb_timer.h>
#include <linux/gfp.h>
#include <linux/mtd/physmap.h>
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/of_platform.h>
#include <linux/of_address.h>

#include <asm/irq.h>
#include <asm/setup.h>
#include <asm/sched_clock.h>
#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <asm/mach/irq.h>
#include <asm/mach/map.h>
#include <asm/mach/time.h>

#include <asm/hardware/gic.h>

#include <mach/platform.h>
#include <mach/hardware.h>
#include <mach/irqs.h>

#include <linux/amba/bus.h>
#include <linux/amba/clcd.h>
#include <plat/clcd.h>

static struct map_desc mapu_a8_io_desc[] __initdata = {
  {
    .virtual  = IO_ADDRESS(MAPU_SYS_BASE),
    .pfn    = __phys_to_pfn(MAPU_SYS_BASE),
    .length   = SZ_4K,
    .type   = MT_DEVICE,
  }, {
    .virtual  = IO_ADDRESS(MAPU_A8_GIC_CPU_BASE),
    .pfn    = __phys_to_pfn(MAPU_A8_GIC_CPU_BASE),
    .length   = SZ_4K,
    .type   = MT_DEVICE,
  }, {
    .virtual  = IO_ADDRESS(MAPU_A8_GIC_DIST_BASE),
    .pfn    = __phys_to_pfn(MAPU_A8_GIC_DIST_BASE),
    .length   = SZ_4K,
    .type   = MT_DEVICE,
  }, {
    .virtual  = IO_ADDRESS(MAPU_A8_TIMER0_BASE),
    .pfn    = __phys_to_pfn(MAPU_A8_TIMER0_BASE),
    .length   = SZ_4K,
    .type   = MT_DEVICE,
  },
#ifdef CONFIG_DEBUG_LL
  {
    .virtual  = IO_ADDRESS(MAPU_A8_UART0_BASE),
    .pfn    = __phys_to_pfn(MAPU_A8_UART0_BASE),
    .length   = SZ_4K,
    .type   = MT_DEVICE,
  },
#endif
};
/*
 * Setup the memory banks.
 */
void mapu_fixup(struct tag *tags, char **from, struct meminfo *meminfo)
{
  meminfo->bank[0].start = MAPU_A8_DDR3_BANK0_BASE;
  meminfo->bank[0].size = SZ_512M;	//luoxq
  //meminfo->bank[1].start = MAPU_A8_DDR3_BANK1_BASE;
  //meminfo->bank[1].size = SZ_256M;
  meminfo->nr_banks = 1;
}

static void __init mapu_a8_map_io(void)
{
  iotable_init(mapu_a8_io_desc, ARRAY_SIZE(mapu_a8_io_desc));
}

/*
 * These are fixed clocks.
 */
static struct clk ref24_clk = {
  .rate = 41200000,
};

static struct clk dw_timer_clk = {
  .rate = 41200000,
};

static struct clk dummy_apb_pclk;

static struct clk_lookup lookups[] = {
  { /* Bus clock */
    .con_id   = "apb_pclk",
    .clk    = &dummy_apb_pclk,
  }, {  /* UART0 */
    .dev_id   = "dev:uart0",
    .clk    = &ref24_clk,
  }, {  /* UART1 */
    .dev_id   = "dev:uart1",
    .clk    = &ref24_clk,
  }, {  /* UART2 */
    .dev_id   = "dev:uart2",
    .clk    = &ref24_clk,
  }, {  /* DesignWare timers */
    .dev_id   = "dw_timer",
    .clk    = &dw_timer_clk,
  },{  /* DesignWare wdt */
	.dev_id   = "50440000.wdt",
	.con_id   = "watchdog",
	.clk    = &dw_timer_clk,
},
#ifdef CONFIG_MAPU_SIM
  {  /* Pl110 */
    .dev_id   = "clcd-pl110",
    .clk    = &ref24_clk,
  }, {  /* Pl050 */
    .dev_id   = "keyboard-pl050",
    .clk    = &ref24_clk,
  },
#endif
};

static void __iomem *ctr;

static u32 notrace mapu_read_sched_clock(void)
{
  if (ctr)
    return readl(ctr);

  return 0;
}

void __init mapu_sched_clock_init(void __iomem *reg, unsigned long rate)
{
  ctr = reg;
  setup_sched_clock(mapu_read_sched_clock, 32, rate);
}

void __init mapu_init_early(void)
{
  void __iomem *timer0 = __io_address(MAPU_A8_TIMER0_BASE);

  clkdev_add_table(lookups, ARRAY_SIZE(lookups));

  mapu_sched_clock_init(timer0 + 0x18, dw_timer_clk.rate);
}

/*
 * Where is the timer (VA)?
 */
void __iomem *timer0_va_base;
void __iomem *timer1_va_base;
void __iomem *timer2_va_base;
void __iomem *timer3_va_base;

/*
 * Set up the clock source and clock events devices
 */
void __init mapu_timer_init(unsigned int timer_irq)
{
  void __iomem *iobase;
  struct dw_apb_clock_event_device *ced;
  struct dw_apb_clocksource *cs;

  iobase = timer0_va_base;

  ced = dw_apb_clockevent_init(0, "dw_timer0", 300, iobase, timer_irq,
                               41200000);
  if (!ced)
    panic("Unable to initialize clockevent device");

  dw_apb_clockevent_register(ced);

  iobase = timer1_va_base;

  cs = dw_apb_clocksource_init(300, "dw_timer0", iobase, 41200000);
  if (!cs)
    panic("Unable to initialize clocksource device");

  dw_apb_clocksource_start(cs);
  dw_apb_clocksource_register(cs);
}

static void __init mapu_a8_timer_init(void)
{
  timer0_va_base = __io_address(MAPU_A8_TIMER0_BASE);
  timer1_va_base = timer0_va_base + 0x14;
  timer2_va_base = timer1_va_base + 0x14;
  timer3_va_base = timer2_va_base + 0x14;

  mapu_timer_init(INTR_TIMERS);
}

static struct sys_timer mapu_a8_timer = {
  .init   = mapu_a8_timer_init,
};


static int mapu_clcd_setup(struct clcd_fb *fb)
{

  fb->panel = versatile_clcd_get_panel("XVGA");
  if (!fb->panel)
    return -EINVAL;
  return versatile_clcd_setup_dma(fb, 1024 * 768 * 2);
}

static void mapu_clcd_enable(struct clcd_fb *fb)
{
  ;
}

static void mapu_clcd_disable(struct clcd_fb *fb)
{
  ;
}

static struct clcd_board clcd_plat_data = {
  .name   = "iacas,mapu",
  .caps   = CLCD_CAP_ALL,
  .check    = clcdfb_check,
  .decode   = clcdfb_decode,
  .disable  = mapu_clcd_disable,
  .enable   = mapu_clcd_enable,
  .setup    = mapu_clcd_setup,
  .mmap   = versatile_clcd_mmap_dma,
  .remove   = versatile_clcd_remove_dma,
};

struct of_dev_auxdata mapu_auxdata_lookup[] __initdata = {
    OF_DEV_AUXDATA("arm,primecell", 0x50500000, "clcd-pl110", &clcd_plat_data),
    OF_DEV_AUXDATA("arm,primecell", 0x50a20000, "keyboard-pl050", NULL),
};


static void __init mapu_a8_init(void)
{
  int i;
#if (defined CONFIG_MAPU_CHIP)
  of_platform_populate(NULL, of_default_bus_match_table, NULL, NULL);
#elif (defined CONFIG_MAPU_SIM)
  of_platform_populate(NULL, of_default_bus_match_table, mapu_auxdata_lookup, NULL);
#endif
}

static void mapu_a8_restart(char mode, const char *cmd)
{
  dsb();
}

static const struct of_device_id mapu_dt_irq_match[] = {
  { .compatible = "arm,cortex-a9-gic", .data = gic_of_init },
  { }
};

static void __init mapu_dt_init_irq(void)
{
  of_irq_init(mapu_dt_irq_match);
}

static void __init gic_init_irq(void)
{
  /* ARM PB-A8 on-board GIC */
  gic_init(0, IRQ_A8_GIC_START,
     __io_address(MAPU_A8_GIC_DIST_BASE),
     __io_address(MAPU_A8_GIC_CPU_BASE));
}

static const char *mapu_match[] __initconst = {
  "iacas,mapu",
  NULL,
};

MACHINE_START(MAPU_A8, "MaPU A8")
  /* Maintainer: IA, CAS */
  .fixup    = mapu_fixup,
  .map_io   = mapu_a8_map_io,
  .init_early = mapu_init_early,
  .init_irq = mapu_dt_init_irq,
  .timer    = &mapu_a8_timer,
  .handle_irq = gic_handle_irq,
  .init_machine = mapu_a8_init,
  .dt_compat  = mapu_match,
#ifdef CONFIG_ZONE_DMA
  .dma_zone_size  = SZ_256M,
#endif
  .restart  = mapu_a8_restart,
MACHINE_END
