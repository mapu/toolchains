/*
 * cmd_dma.c
 *
 *  Created on: Jun 10, 2014
 *      Author: huangw
 */

#include <common.h>
#include <asm/arch/dma.h>
#include <asm/io.h>
#include <asm/errno.h>

static struct dma4 *dma4_cfg = (struct dma4 *)MAPU_DMA4_BASE;
uint32_t dma_active; /* if a transfer is started the respective
	bit is set for the logical channel */

/* Check if we have the given channel
 * PARAMETERS:
 * chan: Channel number
 *
 * RETURN of non-zero means error */
static inline int check_channel(uint32_t chan)
{
	if (chan < CHAN_NR_MIN || chan > CHAN_NR_MAX)
		return -EINVAL;
	return 0;
}

static inline void reset_irq(uint32_t chan)
{
	/* reset IRQ */
	writel((1 << chan), &dma4_cfg->clear_int.block);
	writel((1 << chan), &dma4_cfg->clear_int.dest_tran);
	writel((1 << chan), &dma4_cfg->clear_int.src_tran);
	writel((1 << chan), &dma4_cfg->clear_int.error);
	writel((1 << chan), &dma4_cfg->clear_int.transfer);
	dma_active &= ~(1 << chan);
}

/* Set Source, Destination and Size of DMA transfer for the
 * specified channel.
 * PARAMETERS:
 * chan: channel to use
 * src: source of the transfer
 * dst: destination of the transfer
 * len: length of the transfer
 * sze: TRWIDTH(0,1,2,3,4,5)
 *
 * RETURN of non-zero means error */
int mapu_dma_conf_transfer(uint32_t chan, uint32_t src, uint32_t dst,
		uint32_t len, uint32_t sze)
{
  const uint32_t ctrl_l = 0ULL
    | (1ULL << INT_EN) // Enable interrupt
    | (sze << DEST_TR_WIDTH) // 32b width
    | (sze << SRC_TR_WIDTH)  // 32b width
    | (0ULL << DINC) // increment
    | (0ULL << SINC) // increment
    | (1ULL << DEST_MSIZE) // Burst size 4
    | (1ULL << SRC_MSIZE)  // Burst size 4
    | (0ULL << SRC_GATHER_EN) // Disable gather
    | (0ULL << DST_SCATTER_EN) // Disable scatter
    | (0ULL << TT_FC) // Memory to memory
    | (0ULL << DMS) // AMBA master 1?
    | (0ULL << SMS) // AMBA master 1?
    | (0ULL << LLP_DEST_EN) // Disable link list pointer
    | (0ULL << LLP_SRC_EN); // Disable link list pointer
  const uint32_t cfg_l = 0ULL
    | (7ULL << CH_PRIOR) // Priority is 7 (highest)
    | (0ULL << CH_SUSP) // Clear suspend
    | (0ULL << HS_SEL_DST)  // Hardware handshaking
    | (0ULL << HS_SEL_SRC) // Hardware handshaking
    | (0ULL << LOCK_CH_L) // Channel lock over complete DMA transfer
    | (0ULL << LOCK_B_L) // Bus lock over complete DMA transfer
    | (0ULL << LOCK_CH)  // Unlock channel
    | (0ULL << LOCK_B) // Unlock bus
    | (0ULL << DST_HS_POL) // Handshaking active high
    | (0ULL << SRC_HS_POL) // Handshaking active high
    | (0ULL << MAX_ABRST) // Burst size is not limited
    | (0ULL << RELOAD_SRC) // Single
    | (0ULL << RELOAD_DST); // Single

  const uint32_t ctrl_h = 0ULL
    | (len << (BLOCK_TS - 32)) // Block size
    | (0ULL << (DMA_DONE - 32)); // Clear DONE bit
  const uint32_t cfg_h = 0ULL
    | (0ULL << (FCMODE - 32)) // Enable prefetch
    | (0ULL << (FIFO_MODE - 32)) // Serve source request when FIFO is available for a AHB transfer
    | (0ULL << (PROTCTL - 32)); // Hprot signal ?

  debug("len = %d\n", len);
  debug("ctrl_l = %x\n ctrl_h = %x\n", ctrl_l,ctrl_h);
  debug("cfg_l = %x\n cfg_h = %x\n", cfg_l,cfg_h);

  if (check_channel(chan))
	return -EINVAL;

  writel(src, &dma4_cfg->chan[chan].src_addr_reg);
  writel(dst, &dma4_cfg->chan[chan].dest_addr_reg);

  writel(ctrl_l, &dma4_cfg->chan[chan].ctrl_reg_l);
  writel(ctrl_h, &dma4_cfg->chan[chan].ctrl_reg_h);

  writel(cfg_l, &dma4_cfg->chan[chan].conf_reg_l);
  writel(cfg_h, &dma4_cfg->chan[chan].conf_reg_h);

  return 0;
}

/* Start the DMA transfer */
int mapu_dma_start_transfer(uint32_t chan)
{
	uint32_t val;

	if (check_channel(chan))
		return -EINVAL;

	val = readl(&dma4_cfg->ch_en_reg);

	/* Test for channel already in use */
	if (val & (1 << chan))
		return -EBUSY;

	writel((val | (0x101 << chan)), &dma4_cfg->ch_en_reg);
	dma_active |= (1 << chan);
	debug("started transfer...\n");
	return 0;
}

/* Busy-waiting for a DMA transfer
 * This has to be called before another transfer is started
 * PARAMETER
 * chan: Channel to wait for
 *
 * RETURN of non-zero means error*/
int mapu_dma_wait_for_transfer(uint32_t chan)
{
	uint32_t val;

	if (!(dma_active & (1 << chan))) {
		val = readl(&dma4_cfg->ch_en_reg);

		debug("val = %d\n",val);

		if (!(val & (1 << chan))) {
			debug("dma: The channel you are trying to wait for "
				"was never activated - ERROR\n");
			return -1; /* channel was never active */
		}
	}

	/* all irqs on line 0 */
	do {
		while (!(val = readl(&dma4_cfg->stat_int)))
			asm("nop");
		debug("val1 = %x\n",val);  //debug1
		if (val & (1 << ERR)) {
			val = readl(&dma4_cfg->raw_int.error);
			if (val & (1 << chan))
				debug("dma: transfer error detected\n");
			reset_irq(chan);
			return -1;
		}
		if (val & (1 << TFR)) {
			val = readl(&dma4_cfg->raw_int.transfer);
			debug("val2 = %d\n",val);  //debug2
			if (val & (1 << chan)) {
				reset_irq(chan);
				debug("dma transfer successfully !\n");
				return 0;
			}
		}
	} while (1);
}

/* Initial config of mapu dma
 */
void mapu_dma_init(void)
{
	dma_active = 0;
	/* All interrupts on channel 0 */
	writel(0xFFFFUL,  &dma4_cfg->mask_int.transfer);
	writel(0xFFFFUL,  &dma4_cfg->mask_int.error);
	writel(0xFF00UL,  &dma4_cfg->mask_int.block);
	writel(0xFF00UL,  &dma4_cfg->mask_int.src_tran);
	writel(0xFF00UL,  &dma4_cfg->mask_int.dest_tran);
	writel(0xFFUL,    &dma4_cfg->clear_int.block);
	writel(0xFFUL,    &dma4_cfg->clear_int.dest_tran);
	writel(0xFFUL,    &dma4_cfg->clear_int.src_tran);
	writel(0xFFUL,    &dma4_cfg->clear_int.error);
	writel(0xFFUL,    &dma4_cfg->clear_int.transfer);
	writel(0xFF00UL,  &dma4_cfg->ch_en_reg);
	writel(0x1UL,     &dma4_cfg->dma_conf_reg);
}

/* set channel config to config
 *
 * RETURN of non-zero means error */
int mapu_dma_conf_chan(uint32_t chan, struct dma4_chan *config)
{
	if (check_channel(chan))
		return -EINVAL;

	dma4_cfg->chan[chan] = *config;
	return 0;
}

/* get channel config to config
 *
 * RETURN of non-zero means error */
int mapu_dma_get_conf_chan(uint32_t chan, struct dma4_chan *config)
{
	if (check_channel(chan))
		return -EINVAL;
	*config = dma4_cfg->chan[chan];
	return 0;
}
