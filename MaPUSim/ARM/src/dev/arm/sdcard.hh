/*
 * sdcard.hh
 *
 *  Created on: Mar 23, 2014
 *      Author: wangl
 */

#include "base/statistics.hh"
#include "dev/arm/card_image.hh"
#include "dev/io_device.hh"
#include "dev/arm/sdproto.hh"
#include "sim/eventq.hh"
#include "params/SdCard.hh"

#ifndef SDCARD_HH_
#define SDCARD_HH_


#define CARD_TYPE_MMC           (2)
#define CARD_TYPE_MMCPLUS       (3)
#define CARD_TYPE_SD            (4)

#define STATE_IDLE    (0)
#define STATE_READY   (1)
#define STATE_IDENT   (2)
#define STATE_STBY    (3)
#define STATE_TRANSFER    (4)
#define STATE_DATA    (5)
#define STATE_RCV   (6)
#define STATE_PRG   (7)
#define STATE_DIS   (8)
#define STATE_INACTIVE    (10)
#define GET_STATUS() (card_status & ~STATUS_STATE_MASK) | (state << STATUS_STATE_SHIFT)

#define RST_NOT_STARTED   (7)
#define RST_STARTED     (8)
#define RST_DONE    (9)

#ifndef O_LARGEFILE
/* O_LARGEFILE does not exist and is not neededon FreeBSD, define
 * it so that it does not have any effect*/
#define O_LARGEFILE 0
#endif

#define _FILE_OFFSET_BITS 64
#define SDCARD_MAX_RESPLEN 80 /* 16 + CRC == 17, 1 for word alignment because of fifos  */

#define SD_ERR_NONE    0
#define SD_ERR_TIMEOUT 1


/*
 * ------------------------------------------------------------------------
 * Bit definitions for the upper field of the cmd (Additional information
 * for the data transfer
 * ------------------------------------------------------------------------
 */

#define CMD_FLAG_IS_APPCMD  (1<<8)

class SdController;

typedef struct SdResponse {
        unsigned int len;
        uint8_t data[SDCARD_MAX_RESPLEN];
} SdResponse;

class SdCard : public SimObject {
  typedef int SdCmdProc(uint32_t cmd, uint32_t arg, SdResponse *);
  typedef struct SdCardSpec {
    const char *comment;
    const char *producttype;
    const char *description;
    int type;
    int usec_reset;
    uint32_t ocr;
    uint8_t cid[16];
    uint8_t csd[16];
    uint8_t scr[8];
    uint8_t ssr[64]; /* ACMD13 */
    // secure: uint8_t idmedia[8];
    uint16_t rca; /* Initial RCA */
  } SdCardSpec;

  static const SdCardSpec cardspec[];
  //MMCDev mmcdev;  /* must be the first entry */
  //Clock_t *clk; /* The input from the slot */
  int type;   /* SD/MMC .. */
  int spimode;  /* In Spimode State is not observed ! */
  int state;
  int host_is_2_0;
  int usec_reset;
  int is_app_cmd; /* > 0 app cmd state */
  int crc_on;
  uint32_t card_status;
  uint32_t ocr;
  uint8_t cid[16];
  uint8_t csd[16];
  int spec_vers; /* copy from csd on startup */
  uint16_t rca;
  uint16_t dsr;
  uint8_t scr[8];
  uint8_t ssr[64]; /* sd_status register (queried with ACMD13) */
  uint16_t initial_rca; /* The start value when CMD3 is called the first time */
  uint32_t blocklen;
  uint32_t well_written_blocks; /* counter for multisector write */
  uint64_t cmdcount;    /* Counter incremented on every command to the card */
  uint64_t set_block_count_time; /* cmd counter when set block count cmd was issued */
  uint32_t block_count; /* for following read/write_multiple, 0=infinite */
  uint64_t erase_start;
  uint64_t erase_end;
  CardImage *disk_image;
  uint64_t capacity;

  uint16_t cmd;   /* command attribute for data read/write operations, ACMD flag in high */
  uint64_t address; /* address attribute for data read/write operations */
  uint32_t transfer_count;  /* transfer_count incremented during data read/write operations */
  /* Assembly buffer for incoming data (PGM_CSD/CID for example)  */
  uint8_t dataAssBuf[16];
  //uint16_t dataCrcAss; /* Assembly buffer for incoming data CRC */
  //uint16_t dataCrc;    /* The calculated crc of the incoming data */

  /* Reset delay timer: real card needs some undocumented time for reset */
  SdCmdProc SdCard::*cmdProc[64];
  SdCmdProc SdCard::*appCmdProc[64];
  int reset_state;

  SdController *ctrl;
  //CycleCounter_t reset_start_time;
  //CycleTimer transmissionTimer;
  //Listener *listener_head;

private:
  uint8_t translate_status_to_spir1(uint32_t card_status);

  void translate_status_to_spir2(uint32_t card_status, uint8_t *spir2);

  uint32_t getbits(const uint8_t *arr, int arrlen, int from, int to);

  void copybits(uint32_t val, uint8_t *arr, int arrlen, int from, int to);

#define GETBITS(a, from, to) getbits((a), sizeof((a)), (from), (to))
#define COPYBITS(value, a, from, to) copybits((value), (a), sizeof((a)), (from), (to))

  uint32_t csd_get_blocklen(const uint8_t *csd, int cardtype);

  uint64_t csd_get_capacity(const uint8_t *csd,int cardtype);

  static const char *curr_max[8];

  int sd_go_idle(uint32_t cmd,uint32_t arg, SdResponse *resp);

  int spi_go_idle(uint32_t cmd,uint32_t arg, SdResponse *resp);

  int sd_send_op_cond(uint32_t cmd, uint32_t arg, SdResponse *resp);

  int spi_send_op_cond(uint32_t cmd, uint32_t arg, SdResponse *resp);

  int sd_all_send_cid(uint32_t cmd, uint32_t arg, SdResponse *resp);

  int sd_set_relative_addr(uint32_t cmd, uint32_t arg, SdResponse *resp);

  int sd_send_relative_addr(uint32_t cmd,uint32_t arg, SdResponse *resp);

  int sd_set_dsr(uint32_t cmd, uint32_t arg, SdResponse *resp);

  int sd_switch(uint32_t cmd, uint32_t arg, SdResponse *resp);

  int sd_select_card(uint32_t cmd, uint32_t arg, SdResponse *resp);

  int sd_send_interface_cond(uint32_t cmd, uint32_t arg, SdResponse *resp);

  int spi_send_interface_cond(uint32_t cmd, uint32_t arg, SdResponse *resp);

  int sd_send_csd(uint32_t cmd, uint32_t arg, SdResponse *resp);

  int spi_send_csd(uint32_t cmd, uint32_t arg, SdResponse *resp);

  int sd_send_cid(uint32_t cmd, uint32_t arg, SdResponse *resp);

  int spi_send_cid(uint32_t cmd, uint32_t arg, SdResponse *resp);

  int sd_read_dat_until_stop(uint32_t cmd, uint32_t arg, SdResponse *resp);

  int sd_stop_transmission(uint32_t cmd, uint32_t arg, SdResponse *resp);

  int spi_stop_transmission(uint32_t cmd, uint32_t arg, SdResponse *resp);

  int sd_send_status(uint32_t cmd, uint32_t arg, SdResponse *resp);

  int spi_send_status(uint32_t cmd, uint32_t arg, SdResponse *resp);

  int sd_app_send_status(uint32_t cmd, uint32_t arg, SdResponse *resp);

  int sd_go_inactive_state(uint32_t cmd, uint32_t arg, SdResponse *resp);

  int sd_set_blocklen(uint32_t cmd, uint32_t arg, SdResponse *resp);

  int spi_set_blocklen(uint32_t cmd, uint32_t arg, SdResponse *resp);

  int sd_read_single_block(uint32_t cmd, uint32_t arg, SdResponse *resp);

  int spi_read_single_block(uint32_t cmd, uint32_t arg, SdResponse *resp);

  int sd_read_multiple_block(uint32_t cmd, uint32_t arg, SdResponse *resp);

  int spi_read_multiple_block(uint32_t cmd, uint32_t arg, SdResponse *resp);

  int sd_write_dat_until_stop(uint32_t cmd, uint32_t arg, SdResponse *resp);

  int sd_set_block_count(uint32_t cmd, uint32_t arg, SdResponse *resp);

  int sd_write_single_block(uint32_t cmd, uint32_t arg, SdResponse *resp);

  int spi_write_single_block(uint32_t cmd, uint32_t arg, SdResponse *resp);

  int sd_write_multiple_block(uint32_t cmd, uint32_t arg, SdResponse *resp);

  int spi_write_multiple_block(uint32_t cmd, uint32_t arg, SdResponse *resp);

  int sd_program_cid(uint32_t cmd, uint32_t arg, SdResponse *resp);

  int sd_program_csd(uint32_t cmd, uint32_t arg, SdResponse *resp);

  int sd_set_write_prot(uint32_t cmd, uint32_t arg, SdResponse *resp);

  int sd_clr_write_prot(uint32_t cmd, uint32_t arg, SdResponse *resp);

  int sd_send_write_prot(uint32_t cmd, uint32_t arg, SdResponse *resp);

  int sd_erase_wr_blk_start(uint32_t cmd, uint32_t arg, SdResponse *resp);

  int spi_erase_wr_blk_start(uint32_t cmd, uint32_t arg, SdResponse *resp);

  int sd_erase_wr_blk_end(uint32_t cmd, uint32_t arg, SdResponse *resp);

  int spi_erase_wr_blk_end(uint32_t cmd, uint32_t arg, SdResponse *resp);

  int sd_erase_group_start(uint32_t cmd, uint32_t arg, SdResponse *resp);

  int sd_erase_group_end(uint32_t cmd, uint32_t arg, SdResponse *resp);

  int sd_erase(uint32_t cmd, uint32_t arg, SdResponse *resp);

  int spi_erase(uint32_t cmd, uint32_t arg, SdResponse *resp);

  int sd_fast_io(uint32_t cmd, uint32_t arg, SdResponse *resp);

  int sd_go_irq_state(uint32_t cmd, uint32_t arg, SdResponse *resp);

  int sd_lock_unlock(uint32_t cmd, uint32_t arg, SdResponse *resp);

  int sdio_rw_direct(uint32_t cmd, uint32_t arg, SdResponse *resp);

  int sd_app_cmd(uint32_t cmd, uint32_t arg, SdResponse *resp);

  int spi_app_cmd(uint32_t cmd, uint32_t arg, SdResponse *resp);

  int sd_gen_cmd(uint32_t cmd, uint32_t arg, SdResponse *resp);

  int sd_read_ocr(uint32_t cmd, uint32_t arg, SdResponse *resp);

  int spi_read_ocr(uint32_t cmd, uint32_t arg, SdResponse *resp);

  int sd_crc_on_off(uint32_t cmd, uint32_t arg, SdResponse *resp);

  int spi_crc_on_off(uint32_t cmd, uint32_t arg, SdResponse *resp);

  int sd_app_set_bus_width(uint32_t cmd, uint32_t arg, SdResponse *resp);

  int sd_app_secure_read_multi_block(uint32_t cmd, uint32_t arg, SdResponse *resp);

  int sd_app_send_num_wr_blks(uint32_t cmd, uint32_t arg, SdResponse *resp);

  int spi_app_send_num_wr_blks(uint32_t cmd, uint32_t arg, SdResponse *resp);

  int sd_app_set_wr_blk_erase_count(uint32_t cmd, uint32_t arg, SdResponse *resp);

  int sd_app_secure_write_mult_block(uint32_t cmd, uint32_t arg, SdResponse *resp);

  int sd_app_secure_write_mkb(uint32_t cmd, uint32_t arg, SdResponse *resp);

  int sd_app_secure_erase(uint32_t cmd, uint32_t arg, SdResponse *resp);

  int sd_app_op_cond(uint32_t cmd, uint32_t arg, SdResponse *resp);

  int spi_app_op_cond(uint32_t cmd, uint32_t arg, SdResponse *resp);

  int sd_app_set_clr_card_detect(uint32_t cmd, uint32_t arg, SdResponse *resp);

  int sd_app_get_mkb(uint32_t cmd, uint32_t arg, SdResponse *resp);

  int sd_app_get_mid(uint32_t cmd, uint32_t arg, SdResponse *resp);

  int sd_app_set_cer_rn1(uint32_t cmd, uint32_t arg, SdResponse *resp);

  int sd_app_get_cer_rn2(uint32_t cmd, uint32_t arg, SdResponse *resp);

  int sd_app_set_cer_res2(uint32_t cmd, uint32_t arg, SdResponse *resp);

  int sd_app_get_cer_res1(uint32_t cmd, uint32_t arg, SdResponse *resp);

  int sd_app_change_secure_area(uint32_t cmd, uint32_t arg, SdResponse *resp);

  int sd_app_send_scr(uint32_t cmd, uint32_t arg, SdResponse *resp);

  int spi_app_send_scr(uint32_t cmd, uint32_t arg, SdResponse *resp);

  void dump_cardtypes();

  int read(uint8_t *buf);

  void DoTransmission();

  void InitCmds();

  void GotoSpi();

  int init_from_template(const char *cardtype);

  void create_cardname(const char *prefix);

  int csd2_0_set_capacity(uint64_t capacity);

  int csd1_0_set_capacity(uint64_t capacity);

  int csd_set_capacity(uint64_t capacity);

  void init_auto_card_from_filesize(char *filename);

  void SD_CRC16Init(uint16_t *crc, uint16_t initval);
  void SD_CRC7Init(uint8_t *crc, uint16_t initval);
  void SD_CRC16(uint16_t *crc, const uint8_t *vals, int len);
  void SD_CRC7(uint8_t *crc, const uint8_t *vals, int len);

  inline uint8_t SD_RespCRCByte(SdResponse *resp);

public:
  typedef SdCardParams Params;
  SdCard(const Params *p);

  int DoCmd(uint32_t cmd, uint32_t arg, SdResponse *resp);

  int write(const uint8_t *buf, int count);

  void StartTransmission();

  void setController(SdController *c) {
    ctrl = c;
  }

  /**
   * Serialize this object to the given output stream.
   * @param os The stream to serialize to.
   */
  void serialize(std::ostream &os);

  /**
   * Reconstruct the state of this object from a checkpoint.
   * @param cp The checkpoint to use.
   * @param section The section name describing this object.
   */
  void unserialize(Checkpoint *cp, const std::string &section);

};

#endif /* SDCARD_HH_ */
