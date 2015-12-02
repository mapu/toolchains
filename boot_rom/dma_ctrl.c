/*
 * dma_ctrl.c
 *
 *  Created on: Jan 24, 2014
 *      Author: wangl
 */
#include "dma_ctrl.h"
#include "soc_mmap.h"

extern void wait();
void DMAtrans(unsigned long long src, unsigned long long dest,
              unsigned long long len, enum TRWIDTH stsize,
              enum TRWIDTH dtsize) {
  const unsigned long long ctrl = 0ULL
    | (1ULL << INT_EN) // Enable interrupt
    | (dtsize << DEST_TR_WIDTH) // 128b width
    | (stsize << SRC_TR_WIDTH)  // 128b width
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
    | (0ULL << LLP_SRC_EN) // Disable link list pointer
    | (0x800ULL << BLOCK_TS) // Block size
    | (0ULL << DONE); // Clear DONE bit
  const unsigned long long cfg = 0ULL
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
    | (0ULL << RELOAD_DST) // Single
    | (0ULL << FCMODE) // Enable prefetch
    | (0ULL << FIFO_MODE) // Serve source request when FIFO is available for a AHB transfer
    | (0ULL << PROTCTL); // Hprot signal ?
  volatile struct dma_conf_strt *dma_conf =
    (struct dma_conf_strt *)(DMA_CTRL_START + CH_CFG_START);
  volatile unsigned long long *ch_enable =
    (unsigned long long *)(DMA_CTRL_START + CH_EN_REG);
  volatile struct int_type_strt *clear_base =
    (struct int_type_strt *)(DMA_CTRL_START + CLEAR_START);
  volatile struct int_type_strt *mask_base =
    (struct int_type_strt *)(DMA_CTRL_START + MASK_START);
  volatile unsigned long long *status_int =
    (unsigned long long *)(DMA_CTRL_START + STATUS_INT);
  volatile struct int_type_strt *status_base =
    (struct int_type_strt *)(DMA_CTRL_START + STATUS_START);

  // Configure ARM DMA
  while (len != 0) {
    // Read CH_EN_REG to find an available channel
    unsigned char ch_num;
    do {
      for (ch_num = 0; ch_num < DMAH_NUM_CHANNELS; ch_num++)
        if (!((*ch_enable) & (0x1 << ch_num))) break;
    } while (ch_num == DMAH_NUM_CHANNELS);

    // Clear all status
    clear_base->transfer = -1ULL;
    clear_base->block = -1ULL;
    clear_base->src_tran = -1ULL;
    clear_base->dest_tran = -1ULL;
    clear_base->error = -1ULL;
    // Wait until clear operation is validated
    while (*status_int);

    dma_conf[ch_num].src_addr_reg = src;
    dma_conf[ch_num].dest_addr_reg = dest;
    if (len > 0x800) {
      dma_conf[ch_num].ctrl_reg = ctrl;
      len -= 0x800;
      src += 0x800 << stsize;
      dest += 0x800 << stsize;
    } else {
      dma_conf[ch_num].ctrl_reg = (ctrl & (-1ULL >> 32)) | (len << 32);
      len = 0;
    }
    dma_conf[ch_num].conf_reg = cfg;

    dma_conf[ch_num].link_list_pointer_reg = 0ULL;

    *ch_enable |= 0x101 << ch_num;

    // Unmask transfer interrupt
    mask_base->transfer = (0x101ULL << ch_num);

    // Wait until DMA finished
    do {
      while (!(*status_int & (1 << TFR))) 
      wait();
    } while (!(status_base->transfer & (1 << ch_num)));

    // Mask transfer interrupt
    mask_base->transfer = 0x100ULL << ch_num;
  }
}
