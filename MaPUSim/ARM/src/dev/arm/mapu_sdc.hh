/*
 * mapu_sdc.hh
 *
 *  Created on: Mar 22, 2014
 *      Author: wangl
 */

#ifndef MAPU_SDC_HH_
#define MAPU_SDC_HH_

#include "dev/arm/amba_device.hh"
#include "dev/arm/sdcard.hh"
#include "params/SdController.hh"
#include "sim/serialize.hh"

#define UDELAY_CMD    (20)
#define UDELAY_DATA   (20)
#define UDELAY_WRITE  (10)

#define SD_SDMA(base) ((base) + 0x000)
#define SD_BSIZE(base)  ((base) + 0x004)
#define SD_BCNT(base)   ((base) + 0x006)
#define SD_ARG(base)  ((base) + 0x008)
#define SD_TMODE(base)  ((base) + 0x00C)
#define   TMODE_CMDINDEX_MSK  (0x3f << 24)
#define   TMODE_CMDINDEX_SHIFT  (24)
#define   TMODE_CTYPE_MSK   (0x3 << 22)
#define   TMODE_DATSEL    (1 << 21)
#define   TMODE_CICHK   (1 << 20)
#define   TMODE_CRCHK   (1 << 19)
#define   TMODE_RTYPE_MSK   (0x3 << 16)
#define   RTYPE_NO_RESP   (0 << 16)
#define   RTYPE_R2    (1 << 16)
#define   RTYPE_134567    (2 << 16)
#define   RTYPE_R1BR5B    (3 << 16)
#define   TMODE_SPI   (1 << 7)
#define   TMODE_ATACMD    (0x3 << 6)
#define   TMODE_MS    (1 << 5)
#define   TMODE_DIR_R   (1 << 4)
#define   TMODE_ACMD12    (1 << 2)
#define   TMODE_BCNTEN    (1 << 1)
#define   TMODE_DMAEN   (1 << 0)
#define SD_CMD(base)  ((base) + 0x00E)
#define SD_RESP0(base)  ((base) + 0x010)
#define SD_RESP1(base)  ((base) + 0x012)
#define SD_RESP2(base)  ((base) + 0x014)
#define SD_RESP3(base)  ((base) + 0x016)
#define SD_RESP4(base)  ((base) + 0x018)
#define SD_RESP5(base)  ((base) + 0x01A)
#define SD_RESP6(base)  ((base) + 0x01C)
#define SD_RESP7(base)  ((base) + 0x01E)
#define SD_DATAL(base)  ((base) + 0x020)
#define SD_DATAH(base)  ((base) + 0x022)
#define SD_STATE(base)  ((base) + 0x024)
#define   STATE_DAT_MSK (0x1ef00000)
#define   STATE_CMD (1 << 24)
#define   STATE_SDWP  (1 << 19)
#define   STATE_SDCD  (1 << 18)
#define   STATE_CDST  (1 << 17)
#define   STATE_CDIN  (1 << 16)
#define   STATE_RDEN  (1 << 11)
#define   STATE_WREN  (1 << 10)
#define   STATE_RDACT (1 << 9)
#define   STATE_WRACT (1 << 8)
#define   STATE_DATACT  (1 << 2)
#define   STATE_NODAT (1 << 1)
#define   STATE_NOCMD (1 << 0)
#define SD_CONT1(base)  ((base) + 0x028)
#define SD_POWER(base)  ((base) + 0x029)
#define SD_BLOCKGAP(base)  ((base) + 0x02A)
#define SD_WAKEUP(base)  ((base) + 0x02B)
#define SD_CLK(base)  ((base) + 0x02C)
#define   CLK_CLKEN (1 << 0)
#define   CLK_CLKRDY  (1 << 1)
#define   CLK_SCKEN (1 << 2)
#define   CLK_SDCLKSEL_MSK  (0xff << 8)
#define   CLK_SDCLKSEL_SHIFT  (8)
#define   CLK_TIMEOUT_MSK   (0xf << 16)
#define   CLK_TIMEOUT_SHIFT (16)
#define   CLK_RSTALL    (1 << 24)
#define   CLK_RSTCMD    (1 << 25)
#define   CLK_RSTDAT    (1 << 26)
#define SD_TIME(base)   ((base) + 0x02E)
#define SD_SWRESET(base)  ((base) + 0x02F)
#define SD_STSL(base) ((base) + 0x030)
#define SD_STSH(base) ((base) + 0x032)
#define   STS_ERR_ADMA  (1 << 25)
#define   STS_ERR_ACMD12  (1 << 24)
#define   STS_ERR_CLIMIT  (1 << 23)
#define   STS_ERR_DATEND  (1 << 22)
#define   STS_ERR_DATCRC  (1 << 21)
#define   STS_ERR_DATTIME (1 << 20)
#define   STS_ERR_CMDEND  (1 << 18)
#define   STS_ERR_CMDCRC  (1 << 17)
#define   STS_ERR_CMDTIME (1 << 16)

#define   STS_ERRINT (1 << 15)
#define   STS_CDINT (1 << 8)
#define   STS_CDOUT (1 << 7)
#define   STS_CDIN  (1 << 6)
#define   STS_RDRDY   (1 << 5)
#define   STS_WRRDY   (1 << 4)
#define   STS_DMA   (1 << 3)
#define   STS_BLKGAP  (1 << 2)
#define   STS_TDONE   (1 << 1)
#define   STS_CDONE   (1 << 0)
#define SD_STSENL(base) ((base) + 0x034)
#define SD_STSENH(base) ((base) + 0x036)
#define SD_INTENL(base) ((base) + 0x038)
#define SD_INTENH(base) ((base) + 0x03A)
#define SD_CMD12ERR(base) ((base) + 0x03C)
#define SD_CTRL2(base) ((base) + 0x03E)
#define SD_CAPL(base) ((base) + 0x040)
#define SD_CAPH(base) ((base) + 0x044)
#define SD_CURL(base) ((base) + 0x048)
#define SD_CURH(base) ((base) + 0x04A)
#define SD_FORCEL(base) ((base) + 0x050)
#define SD_FORCEH(base) ((base) + 0x052)
#define SD_ADMAERR(base) ((base) + 0x054)
#define SD_ADDR0(base)  ((base) + 0x058)
#define SD_ADDR1(base)  ((base) + 0x05A)
#define SD_ADDR2(base)  ((base) + 0x05C)
#define SD_ADDR3(base)  ((base) + 0x05E)
#define SD_SLOT(base)   ((base) + 0x0FC)
#define SD_VERSION(base) ((base) + 0x0FE)

#define TCC_SDC_MAGIC   0x62381233

#define OBUF_SIZE (1024U)
#define OBUF_RP  (outbuf_rp % OBUF_SIZE)
#define OBUF_WP  (outbuf_wp % OBUF_SIZE)
#define OBUF_CNT (uint32_t)(outbuf_wp - outbuf_rp)
#define OBUF_SPACE (OBUF_SIZE - OBUF_CNT)

#define IBUF_SIZE (1024U)
#define IBUF_RP  (inbuf_rp % IBUF_SIZE)
#define IBUF_WP  (inbuf_wp % IBUF_SIZE)
#define IBUF_CNT (uint32_t)(inbuf_wp - inbuf_rp)
#define IBUF_SPACE (IBUF_SIZE - IBUF_CNT)

#define MMCARD_MAX_RESPLEN 80 /* 16 + CRC == 17, 1 for word alignment because of fifos  */

class SdController : public AmbaDmaDevice {
protected:
  uint32_t magic;

  uint32_t clk_in;
  uint32_t sdclk;

  // MMCDev *card;
  //CycleTimer data_delay_timer;
  uint32_t transfer_blks;
  uint32_t dma_expected_bytes;
  uint32_t sdma_bufsize;

  uint8_t outbuf[OBUF_SIZE];
  uint32_t outbuf_wp;
  uint32_t outbuf_rp;
  uint8_t inbuf[IBUF_SIZE];
  uint32_t inbuf_wp;
  uint32_t inbuf_rp;
  uint32_t regSDMA;
  uint16_t regBSIZE;
  uint16_t regBCNT;
  uint32_t regARG;
  uint32_t regTMDCMD;
  uint16_t regRESP[8];
  uint16_t regDATAL;
  uint16_t regDATAH;
  uint32_t regSTATE;
  uint32_t regCONT;
  uint16_t regCLK;
  uint8_t regTIME;
  uint8_t regSWRESET;
  uint32_t regSTS;
  uint32_t regSTSEN;
  uint32_t regINTEN;
  uint32_t regCMD12ERR;
  uint32_t regCAP;
  uint16_t regCURL;
  uint16_t regCURH;
  uint16_t regFORCEL;
  uint16_t regFORCEH;
  uint16_t regADMAERR;
  uint16_t regADDR0;
  uint16_t regADDR1;
  uint16_t regADDR2;
  uint16_t regADDR3;
  uint16_t regSLOT;
  uint16_t regVERSION;

  SdCard *card;

  /**
   * Event wrapper for dmaDone()
   *
   * This event calls pushes its this pointer onto the freeDoneEvent
   * vector and calls dmaDone() when triggered.
   */
  class DmaDoneEvent: public Event {
  private:
    SdController &obj;

  public:
    DmaDoneEvent(SdController *_obj) :
        Event(), obj(*_obj) {
    }

    void process() {
      obj.dmaDone();
    }

    const std::string name() const {
      return obj.name() + ".DmaDoneEvent";
    }
  };

  DmaDoneEvent dmaDoneEvent;

  uint32_t cmd_read();
  uint32_t datal_read(int32_t rqlen);
  uint32_t datah_read(int32_t rqlen);

  void tmode_write(uint32_t value);
  void cmd_write(uint32_t value);
  void datal_write(uint32_t value, int32_t rqlen);
  void datah_write(uint32_t value, int32_t rqlen);
  void clk_write(uint32_t value);
  void swreset_write(uint32_t value);
  void stsl_write(uint32_t value);
  void stsh_write(uint32_t value);
  void stsenl_write(uint32_t value);
  void stsenh_write(uint32_t value);
  void intenl_write(uint32_t value);
  void intenh_write(uint32_t value);

  void do_transfer_write();

  void do_cmd_delayed();

  //void update_clock();

  void update_interrupt();

  /** start the dma*/
  void startDma();

  /** DMA done event */
  void dmaDone();

public:
  uint8_t buf[1024];

  typedef SdControllerParams Params;
  SdController(const Params *p);

  virtual Tick read(PacketPtr pkt);
  virtual Tick write(PacketPtr pkt);
  void setTDone();

  virtual void serialize(std::ostream &os);
  virtual void unserialize(Checkpoint *cp, const std::string &section);

  int receive_data(int len);

  /**
   * Determine the address ranges that this device responds to.
   *
   * @return a list of non-overlapping address ranges
   */
  AddrRangeList getAddrRanges() const;
};

#endif /* MAPU_SDC_HH_ */
