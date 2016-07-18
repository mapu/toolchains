/*
 * lowlevel_init.c
 *
 *  Created on: Jan 15, 2014
 *      Author: wangl
 */

#include <stdint.h>
#include "soc_mmap.h"
#include "intr_ctrl.h"
#include "dma_ctrl.h"
#include "ddr_ctrl.h"

extern unsigned int MaPU_Default_IRQ_handler;
extern void wait();

#define PHY0
#if defined(PHY0) || defined(PHY1)
#define NEED_PHY
#endif

void initPLL();
void initPLL() {
  volatile unsigned int *prcm_addr = (unsigned int *)PRCM_START;
  volatile int i = 0;
  prcm_addr[0x1]=0x02011f01; //PD
  for (i = 0; i < 50; i++);
  prcm_addr[0x1]=0x00022701;//ddr pll low freq, 333MHz  


}

#ifdef NEED_PHY
void initPHY();
void initPHY() {
  volatile unsigned int *phy_base;
#  ifdef PHY0
  phy_base = (unsigned int *)PHY0_START;
  phy_base[0x6030 / 4] = 0x2; // mode_sel
  phy_base[0x6000 / 4] = 0x001FFFFF; // phy_en
#  endif

#  ifdef PHY1
  phy_base = (unsigned int *)PHY1_START;
  phy_base[0x6030 / 4] = 0x2; // mode_sel
  phy_base[0x6000 / 4] = 0x001FFFFF; // phy_en
#  endif
}
#endif

#define NEED_DDR0
#define NEED_DDR1
#define NEED_DDR2
#if defined(NEED_DDR0) || defined(NEED_DDR1) || defined(NEED_DDR2)
#  define NEED_DDR
#endif
#ifdef NEED_DDR
extern const unsigned int DDR_PHY_CONF[140];
extern const unsigned int DDR_CH_CTRL_CONF[162];
extern const unsigned int DDR_SYS_CTRL_CONF[162];
void initDDR(unsigned char idx);
void initDDR(unsigned char idx) {
  static const unsigned int DDR_CTRL_BASE[3] = {
    DDRCTRL0_START,
    DDRCTRL1_START,
    DDRCTRL2_START,
  };
  static const unsigned int DDR_CTRL_INT[3] = {
    INTR_DDRCTRL,
    INTR_DDR0CTRL,
    INTR_DDR1CTRL,
  };
  volatile unsigned int *gic_dist_base = (unsigned int *)GIC_DIST_START;
  // Initialize timer0
  volatile unsigned int *timer0_base = (unsigned int *)TIMER_START;
  *(timer0_base + 2) = 0;
  *(timer0_base + 3);
  *timer0_base = 0x1000000;
  
  // Enable DDR controller interrupt
  *(gic_dist_base + GIC_DIST_ENABLE_SET / 4 + DDR_CTRL_INT[idx] / 32) = 1 << (DDR_CTRL_INT[idx] & 0x1F);
  // Enable Timer interrupt in case of running on Simulator
  *(gic_dist_base + GIC_DIST_ENABLE_SET / 4 + INTR_TIMERS / 32) = 1 << (INTR_TIMERS & 0x1F);
  
  volatile unsigned int *ddr_reg_base = (unsigned int *)(DDR_CTRL_BASE[idx]);
  if (idx == 0) //SYS DDR
    DMAtrans((unsigned int)DDR_SYS_CTRL_CONF,(unsigned int)ddr_reg_base, 162, TR_W_32, TR_W_32);
  else  //CH DDR
    DMAtrans((unsigned int)DDR_CH_CTRL_CONF,(unsigned int)ddr_reg_base, 162, TR_W_32, TR_W_32);

  volatile unsigned int *ddr_phy_reg_base = ddr_reg_base + 0x100 + 128;
  //DMAtrans((unsigned int)&DDR_PHY_CONF[128], (unsigned int)ddr_phy_reg_base, 12, TR_W_32, TR_W_32);
  ddr_phy_reg_base = ddr_reg_base + 0x100;
  DMAtrans((unsigned int)DDR_PHY_CONF, (unsigned int)ddr_phy_reg_base, 140, TR_W_32, TR_W_32);
  ddr_reg_base[0] = 0x0601;
  *(timer0_base + 2) = 0x11;
  wait();
  // Clear DDR controller interrupt
  *(ddr_reg_base + 48) = 0x10;
  // Disable Timer
  *(timer0_base + 2) = 0;
  *(timer0_base + 3);
  adjust_ddr_regs(ddr_phy_reg_base, ddr_reg_base);
}
#endif

#ifdef GIC
void initGicDist(void);
void initGicDist(void) {
  int i;
  unsigned int cpumask;
  unsigned int gic_irqs = NUM_INTR;
  volatile unsigned int *gic_dist_base = (unsigned int *)GIC_DIST_START;
  unsigned int cpu = 0;
  unsigned int gpri = ((0xa << PRIORITY_SHIFT) & 0xFF);
  gpri |= gpri << 8;
  gpri |= gpri << 16;

  cpumask = 1 << cpu;
  cpumask |= cpumask << 8;
  cpumask |= cpumask << 16;

  // Disable interrupt controller
  *(gic_dist_base + GIC_DIST_CTRL / 4) = 0;

  /*
   * Set all global interrupts to be level triggered, active low.
   */
  for (i = 32; i < gic_irqs; i += 16)
    *(gic_dist_base + GIC_DIST_CONFIG / 4 + i / 16) = 0;

  /*
   * Set all global interrupts to this CPU only.
   */
  for (i = 32; i < gic_irqs; i += 4)
    *(gic_dist_base + GIC_DIST_TARGET / 4 + i / 4) = cpumask;

  /*
   * Set priority on all global interrupts.
   */
  for (i = 32; i < gic_irqs; i += 4)
    *(gic_dist_base + GIC_DIST_PRI / 4 + i / 4) = gpri;
  /*
   * Disable all interrupts.  Leave the PPI and SGIs alone
   * as these enables are banked registers.
   */
  for (i = 32; i < gic_irqs; i += 32)
    *(gic_dist_base + GIC_DIST_ENABLE_CLEAR / 4 + i / 32) = 0xffffffff;

  *(gic_dist_base + GIC_DIST_CTRL / 4) = 1;
}
void initGicCpu(void);
void initGicCpu(void) {
  volatile unsigned int *gic_dist_base = (unsigned int *)GIC_DIST_START;
  volatile unsigned int *gic_cpu_base = (unsigned int *)GIC_CPU_START;
  int i;
  unsigned int gpri = ((0xa << PRIORITY_SHIFT) & 0xFF);
  gpri |= gpri << 8;
  gpri |= gpri << 16;

  /*
   * Deal with the banked PPI and SGI interrupts - disable all
   * PPI interrupts, ensure all SGI interrupts are enabled.
   */
  *(gic_dist_base + GIC_DIST_ENABLE_CLEAR / 4) = 0xffff0000;
  *(gic_dist_base + GIC_DIST_ENABLE_SET / 4) = 0x0000ffff;

  /*
   * Set priority on PPI and SGI interrupts
   */
  
  for (i = 0; i < 32; i += 4)
    *(gic_dist_base + GIC_DIST_PRI / 4 + i / 4) = gpri;

  *(gic_cpu_base + GIC_CPU_PRIMASK / 4) = LOWEST_PRIORITY;
  *(gic_cpu_base + GIC_CPU_CTRL / 4) = 1;
}
#else
void initIntrCtrl(void);
void initIntrCtrl() {
  volatile unsigned long long *irq_inten =
    (unsigned long long *)(INTRCTRL_START + IRQ_INTEN_L);
  volatile unsigned int *fiq_inten =
    (unsigned int *)(INTRCTRL_START + FIQ_INTEN);
  // Disable all irqs
  *irq_inten = 0ULL;
  *fiq_inten = 0UL;
  // Initialize all interrupt handlers as default handler
  // Initialize the priorities of all irq vectors as corresponding index
  volatile unsigned int *irq_vectors =
    (unsigned int *)(INTRCTRL_START + IRQ_VECTOR_0);
  volatile unsigned int *irq_pr_base =
    (unsigned int *)(INTRCTRL_START + IRQ_PR_START);
  for (unsigned int i = 0; i < 16; i++)
    irq_vectors[i << 1] = &MaPU_Default_IRQ_handler;
  for (unsigned int i = 0; i < 64; i++)
    irq_pr_base[i] = i;
  volatile unsigned int *irq_vector_default =
    (unsigned int *)(INTRCTRL_START + IRQ_VECTOR_DEFAULT);
  *irq_vector_default = MaPU_Default_IRQ_handler;
  // Allow all levels of interrupt
  volatile unsigned int *irq_plevel =
    (unsigned int *)(INTRCTRL_START + IRQ_PLEVEL);
  *irq_plevel = 0;
  // Mask all irqs until a real device interrupt handler is implemented
  volatile unsigned long long *irq_intmask =
    (unsigned long long *)(INTRCTRL_START + IRQ_INTMASK_L);
  *irq_intmask = -1ULL;
  volatile unsigned int *fiq_intmask =
    (unsigned int *)(INTRCTRL_START + FIQ_INTMASK);
  *fiq_intmask = -1UL;
  // Enable all irqs
  *irq_inten = -1ULL;
  *fiq_inten = -1UL;
}
#endif

void initDMACtrl(void);
void initDMACtrl() {
  // Mask all status
  volatile struct int_type_strt *mask_base =
    (struct int_type_strt *)(DMA_CTRL_START + MASK_START);
  mask_base->transfer = 0xFF00ULL;
  mask_base->block = 0xFF00ULL;
  mask_base->src_tran = 0xFF00ULL;
  mask_base->dest_tran = 0xFF00ULL;
  mask_base->error = 0xFF00ULL;
  // Clear all status
  volatile struct int_type_strt *clear_base =
    (struct int_type_strt *)(DMA_CTRL_START + CLEAR_START);
  clear_base->transfer = -1ULL;
  clear_base->block = -1ULL;
  clear_base->src_tran = -1ULL;
  clear_base->dest_tran = -1ULL;
  clear_base->error = -1ULL;
  // Wait until clear operation is validated
  volatile unsigned long long *status_int =
    (unsigned long long *)(DMA_CTRL_START + STATUS_INT);
  while (*status_int);
  // Open DMA
  volatile unsigned long long *dma_cfg_reg =
    (unsigned long long *)(DMA_CTRL_START + DMA_CFG_REG);
  *dma_cfg_reg = 1ULL;

  // Unmask DMA interrupt
#ifdef GIC
  volatile unsigned int *gic_dist_base = (unsigned int *)GIC_DIST_START;
  *(gic_dist_base + GIC_DIST_ENABLE_SET / 4 + INTR_DMACTRL / 32) = 1 << (INTR_DMACTRL & 0x1F);
#else
  volatile unsigned long long *irq_intmask =
    (unsigned long long *)(INTRCTRL_START + IRQ_INTMASK_L);

  *irq_intmask = (*irq_intmask) & ~(1ULL << INTR_DMACTRL);
#endif
}

extern uint8_t __data_load;
extern uint8_t __data_start;
extern uint8_t _edata;

void LowLevelInit(void (*reset_addr)(), void (*return_addr)());
void LowLevelInit(void (*reset_addr)(), void (*return_addr)()) {
  initPLL();
#ifdef NEED_PHY
  initPHY();
#endif
#ifdef GIC
  initGicDist();
  initGicCpu();
#else
  initIntrCtrl();
#endif
  initDMACtrl();

#ifdef NEED_DDR0
  initDDR(0);
#endif

#ifdef NEED_DDR1
  initDDR(1);
#endif

#ifdef NEED_DDR2
  initDDR(2);
#endif

  DMAtrans((unsigned int)&__data_load,
           (unsigned int)&__data_start,
           (&_edata - &__data_start) / 4,
           TR_W_32, TR_W_32);
}
