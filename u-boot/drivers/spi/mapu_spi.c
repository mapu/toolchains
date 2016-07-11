/*
 * mapu_spi.c
 *
 *  Created on: Jul 3, 2016
 *      Author: luoxq
 */
#include <common.h>
#include <asm/io.h>
#include <errno.h>

DECLARE_GLOBAL_DATA_PTR;

/* Register offsets */
#define DW_SPI_CTRL0      0x00
#define DW_SPI_CTRL1      0x04
#define DW_SPI_SSIENR     0x08
#define DW_SPI_MWCR     0x0c
#define DW_SPI_SER      0x10
#define DW_SPI_BAUDR      0x14
#define DW_SPI_TXFLTR     0x18
#define DW_SPI_RXFLTR     0x1c
#define DW_SPI_TXFLR      0x20
#define DW_SPI_RXFLR      0x24
#define DW_SPI_SR     0x28
#define DW_SPI_IMR      0x2c
#define DW_SPI_ISR      0x30
#define DW_SPI_RISR     0x34
#define DW_SPI_TXOICR     0x38
#define DW_SPI_RXOICR     0x3c
#define DW_SPI_RXUICR     0x40
#define DW_SPI_MSTICR     0x44
#define DW_SPI_ICR      0x48
#define DW_SPI_DMACR      0x4c
#define DW_SPI_DMATDLR      0x50
#define DW_SPI_DMARDLR      0x54
#define DW_SPI_IDR      0x58
#define DW_SPI_VERSION      0x5c
#define DW_SPI_DR     0x60

/* Bit fields in CTRLR0 */
#define SPI_DFS_OFFSET      0

#define SPI_FRF_OFFSET      4
#define SPI_FRF_SPI     0x0
#define SPI_FRF_SSP     0x1
#define SPI_FRF_MICROWIRE   0x2
#define SPI_FRF_RESV      0x3

#define SPI_MODE_OFFSET     6
#define SPI_SCPH_OFFSET     6
#define SPI_SCOL_OFFSET     7

#define SPI_TMOD_OFFSET     8
#define SPI_TMOD_MASK     (0x3 << SPI_TMOD_OFFSET)
#define SPI_TMOD_TR     0x0   /* xmit & recv */
#define SPI_TMOD_TO     0x1   /* xmit only */
#define SPI_TMOD_RO     0x2   /* recv only */
#define SPI_TMOD_EPROMREAD    0x3   /* eeprom read mode */

#define SPI_SLVOE_OFFSET    10
#define SPI_SRL_OFFSET      11
#define SPI_CFS_OFFSET      12

/* Bit fields in SR, 7 bits */
#define SR_MASK       0x7f    /* cover 7 bits */
#define SR_BUSY       (1 << 0)
#define SR_TF_NOT_FULL      (1 << 1)
#define SR_TF_EMPT      (1 << 2)
#define SR_RF_NOT_EMPT      (1 << 3)
#define SR_RF_FULL      (1 << 4)
#define SR_TX_ERR     (1 << 5)
#define SR_DCOL       (1 << 6)

#define RX_TIMEOUT      1000    /* timeout in ms */

#define MAPU_SPI_BASE   0x50ff0000

struct mapu_spi_priv {
  void __iomem *regs;
  unsigned int freq;    /* Default frequency */
  unsigned int mode;

  int bits_per_word;
  u8 cs;      /* chip select pin */
  u8 tmode;   /* TR/TO/RO/EEPROM */
  u8 type;    /* SPI/SSP/MicroWire */
  int len;

  u32 fifo_len;   /* depth of the FIFO buffer */
  void *tx;
  void *tx_end;
  void *rx;
  void *rx_end;
};

struct mapu_spi_priv priv;

static void mapu_spi_init(void)
{
  writel(MAPU_SPI_BASE+DW_SPI_SSIENR, 0);  // Disable SPI
  writel(MAPU_SPI_BASE+DW_SPI_IMR, 0xff);  // Set NOT mask to all interrupt
  writel(MAPU_SPI_BASE+DW_SPI_SSIENR, 1);  // Enable SPI

}

/* Return the max entries we can fill into tx fifo */
static inline unsigned int tx_max(void)
{
  unsigned int tx_left, tx_room, rxtx_gap;

  tx_left = (priv.tx_end - priv.tx) / (priv.bits_per_word >> 3);
  tx_room = priv.fifo_len - (unsigned short)readl(MAPU_SPI_BASE+DW_SPI_TXFLR);

  /*
   * Another concern is about the tx/rx mismatch, we
   * thought about using (priv.fifo_len - rxflr - txflr) as
   * one maximum value for tx, but it doesn't cover the
   * data which is out of tx/rx fifo and inside the
   * shift registers. So a control from sw point of
   * view is taken.
   */
  rxtx_gap = ((priv.rx_end - priv.rx) - (priv.tx_end - priv.tx)) /
    (priv.bits_per_word >> 3);

  return min3(tx_left, tx_room, (unsigned int)(priv.fifo_len - rxtx_gap));
}

/* Return the max entries we should read out of rx fifo */
static inline unsigned int rx_max(void)
{
  unsigned int rx_left = (priv.rx_end - priv.rx) / (priv.bits_per_word >> 3);

  return min_t(unsigned int, rx_left, readl(MAPU_SPI_BASE+DW_SPI_RXFLR));
}

static void dw_writer(void)
{
  unsigned int max = tx_max();
  unsigned short txw = 0;

  while (max--) {
    /* Set the tx word if the transfer's original "tx" is not null */
    if (priv.tx_end - priv.len) {
      if (priv.bits_per_word == 8)
        txw = *(unsigned char *)(priv.tx);
      else
        txw = *(unsigned short *)(priv.tx);
    }
    writel(MAPU_SPI_BASE+DW_SPI_DR, (unsigned int)txw);
    debug("%s: tx=0x%02x\n", __func__, txw);
    priv.tx += priv.bits_per_word >> 3;
  }
}

static int dw_reader(void)
{
  unsigned long long start = get_timer(0);
  unsigned int max;
  unsigned short rxw;

  /* Wait for rx data to be ready */
  while (rx_max() == 0) {
    if (get_timer(start) > RX_TIMEOUT)
      return -ETIMEDOUT;
  }

  max = rx_max();

  while (max--) {
    rxw = (unsigned short)readl(MAPU_SPI_BASE+DW_SPI_DR);
    debug("%s: rx=0x%02x\n", __func__, rxw);

    /*
     * Care about rx only if the transfer's original "rx" is
     * not null
     */
    if (priv.rx_end - priv.len) {
      if (priv.bits_per_word == 8)
        *(unsigned char *)(priv.rx) = rxw;
      else
        *(unsigned short *)(priv.rx) = rxw;
    }
    priv.rx += priv.bits_per_word >> 3;
  }

  return 0;
}

static int poll_transfer(void)
{
  int ret;

  do {
    dw_writer();
    ret = dw_reader();
    if (ret < 0)
      return ret;
  } while (priv.rx_end > priv.rx);

  return 0;
}

static int mapu_spi_xfer(unsigned int cs, unsigned int bitlen,
           const void *dout, void *din, unsigned long flags)
{
  const unsigned char *tx = dout;
  unsigned char *rx = din;
  int ret = 0;
  unsigned int cr0 = 0;

  /* spi core configured to do 8 bit transfers */
  if (bitlen % 8) {
    debug("Non byte aligned SPI transfer.\n");
    return -1;
  }

  cr0 = (priv.bits_per_word - 1) | (priv.type << SPI_FRF_OFFSET) |
    (priv.mode << SPI_MODE_OFFSET) |
    (priv.tmode << SPI_TMOD_OFFSET);

  if (rx && tx)
    priv.tmode = SPI_TMOD_TR;
  else if (rx)
    priv.tmode = SPI_TMOD_RO;
  else
    priv.tmode = SPI_TMOD_TO;

  cr0 &= ~SPI_TMOD_MASK;
  cr0 |= (priv.tmode << SPI_TMOD_OFFSET);

  priv.len = bitlen >> 3;
  debug("%s: rx=%p tx=%p len=%d [bytes]\n", __func__, rx, tx, priv.len);

  priv.tx = (void *)tx;
  priv.tx_end = priv.tx + priv.len;
  priv.rx = rx;
  priv.rx_end = priv.rx + priv.len;

  /* Disable controller before writing control registers */
  writel(MAPU_SPI_BASE+DW_SPI_SSIENR, 0);  // Disable SPI

  debug("%s: cr0=%08x\n", __func__, cr0);
  /* Reprogram cr0 only if changed */
  if ((unsigned short)readl(MAPU_SPI_BASE+DW_SPI_CTRL0) != cr0)
    writel(MAPU_SPI_BASE+DW_SPI_CTRL0, (unsigned int)cr0);

  /*
   * Configure the desired SS (slave select 0...3) in the controller
   * The DW SPI controller will activate and deactivate this CS
   * automatically. So no cs_activate() etc is needed in this driver.
   */
  writel(MAPU_SPI_BASE+DW_SPI_SER, 1 << cs);

  /* Enable controller after writing control registers */
  writel(MAPU_SPI_BASE+DW_SPI_SSIENR, 1);

  /* Start transfer in a polling loop */
  ret = poll_transfer();

  return ret;
}

