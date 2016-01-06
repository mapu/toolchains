#ifndef __SDMA_H
#define __SDMA_H

/* Copyright (C) 2011
 * Corscience GmbH & Co. KG - Simon Schwarz <schwarz@corscience.de>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

/* DMA */
struct dma4_chan {
  uint32_t src_addr_reg;
  uint32_t pad0;
  uint32_t dest_addr_reg;
  uint32_t pad1;
  uint32_t link_list_pointer_reg;
  uint32_t pad2;
  uint32_t ctrl_reg_l;
  uint32_t ctrl_reg_h;
  uint32_t src_stat_reg;
  uint32_t pad4;
  uint32_t dest_stat_reg;
  uint32_t pad5;
  uint32_t src_stat_addr_reg;
  uint32_t pad6;
  uint32_t dest_stat_addr_reg;
  uint32_t pad7;
  uint32_t conf_reg_l;
  uint32_t conf_reg_h;
  uint32_t src_gather_reg;
  uint32_t pad9;
  uint32_t dest_scatter_reg;
  uint32_t pad10;
};

struct int_type_strt {
  uint32_t transfer;
  uint32_t pad0;
  uint32_t block;
  uint32_t pad1;
  uint32_t src_tran;
  uint32_t pad2;
  uint32_t dest_tran;
  uint32_t pad3;
  uint32_t error;
  uint32_t pad4;
};

struct dma4 {
  struct dma4_chan chan[8];
  struct int_type_strt raw_int;
  struct int_type_strt status_int;
  struct int_type_strt mask_int;
  struct int_type_strt clear_int;
  uint32_t stat_int;
  uint32_t pad;
  uint32_t req_src_reg;
  uint32_t pad0;
  uint32_t req_dest_reg;
  uint32_t pad1;
  uint32_t sgl_req_src_reg;
  uint32_t pad2;
  uint32_t sgl_req_dest_reg;
  uint32_t pad3;
  uint32_t last_src_reg;
  uint32_t pad4;
  uint32_t last_dest_reg;
  uint32_t pad5;
  uint32_t dma_conf_reg;
  uint32_t pad6;
  uint32_t ch_en_reg;
  uint32_t pad7;
  uint32_t dma_id_reg;
};

enum CTRL_BITFIELD {
  INT_EN = 0,
  DEST_TR_WIDTH,
  SRC_TR_WIDTH = 4,
  DINC = 7,
  SINC = 9,
  DEST_MSIZE = 11,
  SRC_MSIZE = 14,
  SRC_GATHER_EN = 17,
  DST_SCATTER_EN = 18,
  TT_FC = 20,
  DMS = 23,
  SMS = 25,
  LLP_DEST_EN = 27,
  LLP_SRC_EN = 28,
  BLOCK_TS = 32,
  DMA_DONE = 44,
};

enum CFG_BITFIELD {
  CH_PRIOR = 5,
  CH_SUSP = 8,
  FIFO_EMPTY,
  HS_SEL_DST,
  HS_SEL_SRC,
  LOCK_CH_L,
  LOCK_B_L = 14,
  LOCK_CH = 16,
  LOCK_B,
  DST_HS_POL,
  SRC_HS_POL,
  MAX_ABRST,
  RELOAD_SRC = 30,
  RELOAD_DST,
  FCMODE,
  FIFO_MODE,
  PROTCTL,
  DS_UPD_EN = 37,
  SS_UPD_EN,
  SRC_PER,
  DEST_PER = 43,
};

enum STATUS_BITFIELD {
  TFR = 0,
  BLOCK,
  SRCT,
  DSTT,
  ERR,
};

enum TRWIDTH {
  TR_W_8 = 0,
  TR_W_16 = 1,
  TR_W_32 = 2,
  TR_W_64 = 3,
  TR_W_128 = 4,
  TR_W_256 = 5,
};

/* Functions */
void mapu_dma_init(void);
int mapu_dma_conf_transfer(uint32_t chan, uint32_t src, uint32_t dst, uint32_t len,
		uint32_t sze);
int mapu_dma_start_transfer(uint32_t chan);
int mapu_dma_wait_for_transfer(uint32_t chan);
int mapu_dma_conf_chan(uint32_t chan, struct dma4_chan *config);
int mapu_dma_get_conf_chan(uint32_t chan, struct dma4_chan *config);

/* others */
#define CHAN_NR_MIN			0
#define CHAN_NR_MAX			7

static const int DMAH_NUM_CHANNELS = 8;

#define CH_EN_REG       0x3A0

/* DMA4/SDMA */
#define MAPU_DMA4_BASE              0x50000000

#endif /* __SDMA_H */
