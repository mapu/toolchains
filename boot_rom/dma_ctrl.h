/*
 * dma_ctrl.h
 *
 *  Created on: Jan 15, 2014
 *      Author: wangl
 */

#ifndef DMA_CTRL_H_
#define DMA_CTRL_H_

struct dma_conf_strt {
  unsigned long long src_addr_reg;
  unsigned long long dest_addr_reg;
  unsigned long long link_list_pointer_reg;
  unsigned long long ctrl_reg;
  unsigned long long src_stat_reg;
  unsigned long long dest_stat_reg;
  unsigned long long src_stat_addr_reg;
  unsigned long long dest_stat_addr_reg;
  unsigned long long conf_reg;
  unsigned long long src_gather_reg;
  unsigned long long dest_scatter_reg;
};

struct int_type_strt {
  unsigned long long transfer;
  unsigned long long block;
  unsigned long long src_tran;
  unsigned long long dest_tran;
  unsigned long long error;
};

enum DMA_CTRL_MMAP {
  CH_CFG_START = 0x0,
  RAW_START = 0x2C0,
  STATUS_START = 0x2E8,
  MASK_START = 0x310,
  CLEAR_START =0x338,
  STATUS_INT = 0x360,
  REQ_SRC_REG = 0x368,
  REQ_DEST_REG = 0x370,
  SINGLE_REQ_SRC_REG = 0x378,
  SINGLE_REQ_DEST_REG = 0x380,
  LAST_REQ_SRC_REG = 0x388,
  LAST_REQ_DEST_REG = 0x390,
  DMA_CFG_REG = 0x398,
  CH_EN_REG = 0x3A0,
  DMA_ID_REG = 0x3A8,
  DMA_TEST_REG = 0x3B0,
  DMA_COMP_PARAMS_6 = 0x3C8,
  DMA_COMP_PARAMS_5 = 0x3D0,
  DMA_COMP_PARAMS_4 = 0x3D8,
  DMA_COMP_PARAMS_3 = 0x3E0,
  DMA_COMP_PARAMS_2 = 0x3E8,
  DMA_COMP_PARAMS_1 = 0x3F0,
  DMA_COMPONENT_ID = 0x3F8,
};

static const int DMAH_NUM_CHANNELS = 8;

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
  DONE = 44,
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

void DMAtrans(unsigned long long src, unsigned long long dest,
              unsigned long long len, enum TRWIDTH stsize, enum TRWIDTH dtsize);
#endif /* DMA_CTRL_H_ */
