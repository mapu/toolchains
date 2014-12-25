/*
 * sdproto.hh
 *
 *  Created on: Mar 24, 2014
 *      Author: wangl
 */

#ifndef SDPROTO_HH_
#define SDPROTO_HH_

/* Standard SD commands (3.1)           type  argument     response */
   /* class 0 */
#define SD_GO_IDLE_STATE         0   /* bc                          */
#define SD_SEND_OP_COND          1   /* bcr  [31:0] OCR         R3  */
#define SD_ALL_SEND_CID          2   /* bcr                     R2  */
#define SD_SET_RELATIVE_ADDR     3   /* ac   [31:16] RCA        R1  */
#define SD_SET_DSR               4   /* bc   [31:16] RCA            */
#define SPIO_               5
#define SD_SWITCH      6   /* ac   <Complex>          R1b */
#define SD_SELECT_CARD           7   /* ac   [31:16] RCA        R1  */
#define SD_SEND_EXT_CSD    8   /* adtc      R1  */
#define SD_SEND_INTERFACE_COND    8   /*       R7  */

#define SD_SEND_CSD              9   /* ac   [31:16] RCA        R2  */
#define SD_SEND_CID             10   /* ac   [31:16] RCA        R2  */
#define SD_READ_DAT_UNTIL_STOP  11   /* adtc [31:0] dadr        R1  */
#define SD_STOP_TRANSMISSION    12   /* ac                      R1b */
#define SD_SEND_STATUS          13   /* ac   [31:16] RCA        R1  */
#define SD_BUSTEST_R    14   /* adtc      R1  */
#define SD_GO_INACTIVE_STATE    15   /* ac   [31:16] RCA            */

  /* class 2 */
#define SD_SET_BLOCKLEN         16   /* ac   [31:0] block len   R1  */
#define SD_READ_SINGLE_BLOCK    17   /* adtc [31:0] data addr   R1  */
#define SD_READ_MULTIPLE_BLOCK  18   /* adtc [31:0] data addr   R1  */
#define SD_BUSTEST_W    19   /* adtc      R1  */

  /* class 3 */
#define SD_WRITE_DAT_UNTIL_STOP 20   /* adtc [31:0] data addr   R1  */

  /* class 4 */
#define SD_SET_BLOCK_COUNT      23   /* adtc [31:0] data addr   R1  */
#define SD_WRITE_SINGLE_BLOCK   24   /* adtc [31:0] data addr   R1  */
#define SD_WRITE_MULTIPLE_BLOCK 25   /* adtc                    R1  */
#define SD_PROGRAM_CID          26   /* adtc                    R1  */
#define SD_PROGRAM_CSD          27   /* adtc                    R1  */

  /* class 6 */
#define SD_SET_WRITE_PROT       28   /* ac   [31:0] data addr   R1b */
#define SD_CLR_WRITE_PROT       29   /* ac   [31:0] data addr   R1b */
#define SD_SEND_WRITE_PROT      30   /* adtc [31:0] wpdata addr R1  */

  /* class 5 */
#define SD_ERASE_WR_BLK_START   32   /* ac      R1  */
#define SD_ERASE_WR_BLK_END     33   /* ac      R1  */
#define SD_ERASE_GROUP_START    35   /* ac   [31:0] data addr   R1  */
#define SD_ERASE_GROUP_END      36   /* ac   [31:0] data addr   R1  */
#define SD_ERASE                38   /* ac                      R1b */

 /* class 9 */
#define SD_FAST_IO              39   /* ac   <Complex>          R4  */
#define SD_GO_IRQ_STATE         40   /* bcr                     R5  */

  /* class 7 */
#define SD_LOCK_UNLOCK          42   /* adtc                    R1b */


#define SDIO_RW_DIRECT             52   /*    [39:8]         R5  */

  /* class 8 */
#define SD_APP_CMD              55   /* ac   [31:16] RCA        R1  */
#define SD_GEN_CMD              56   /* adtc [0] RD/WR          R1  */
#define SD_READ_OCR     58   /*       R3  */
#define SD_CRC_ON_OFF     59

/* SD commands                           type  argument     response */
  /* class 8 */
/* This is basically the same command as for SD with some quirks. */
#define SD_SEND_RELATIVE_ADDR     3   /* ac                      R6  */

/* Application commands, See Toshiba SD-Card specification */
#define SD_APP_SET_BUS_WIDTH    6     /* ac   [1:0] bus width    R1  */
#define SD_APP_SEND_STATUS    13  /* adtc        R2  */
#define SD_APP_SECURE_READ_MULTI_BLOCK  18
#define SD_APP_SEND_NUM_WR_BLKS   22    /* adtc        R1  */
#define SD_APP_SET_WR_BLK_ERASE_COUNT 23  /* ac              R1  */
#define SD_APP_SECURE_WRITE_MULT_BLOCK  25
#define SD_APP_SECURE_WRITE_MKB   26
#define SD_APP_SECURE_ERASE   38
#define SD_APP_OP_COND            41    /* bcr  [31:0] OCR         R3  */
#define SD_APP_SET_CLR_CARD_DETECT  42  /* ac        R1  */
#define SD_APP_GET_MKB      43
#define SD_APP_GET_MID      44
#define SD_APP_SET_CER_RN1    45
#define SD_APP_GET_CER_RN2    46
#define SD_APP_SET_CER_RES2   47
#define SD_APP_GET_CER_RES1   48
#define SD_APP_CHANGE_SECURE_AREA 49
#define SD_APP_SEND_SCR           51  /* adtc                    R1  */

/*
  SD status in R1
  Type
        e : error bit
        s : status bit
        r : detected and set for the actual command response
        x : detected and set during command execution. the host must poll
            the card by sending status command in order to read these bits.
  Clear condition
        a : according to the card state
        b : always related to the previous command. Reception of
            a valid command will clear it (with a delay of one command)
        c : clear by read
 */


#define STATUS_OUT_OF_RANGE         (1 << 31)       /* er, c */
#define STATUS_ADDRESS_ERROR        (1 << 30)       /* erx, c */
#define STATUS_BLOCK_LEN_ERROR      (1 << 29)       /* er, c */
#define STATUS_ERASE_SEQ_ERROR      (1 << 28)       /* er, c */
#define STATUS_ERASE_PARAM          (1 << 27)       /* ex, c */
#define STATUS_WP_VIOLATION         (1 << 26)       /* erx, c */
#define STATUS_CARD_IS_LOCKED       (1 << 25)       /* sx, a */
#define STATUS_LOCK_UNLOCK_FAILED   (1 << 24)       /* erx, c */
#define STATUS_COM_CRC_ERROR        (1 << 23)       /* er, b */
#define STATUS_ILLEGAL_COMMAND      (1 << 22)       /* er, b */
#define STATUS_CARD_ECC_FAILED      (1 << 21)       /* ex, c */
#define STATUS_CC_ERROR             (1 << 20)       /* erx, c */
#define STATUS_ERROR                (1 << 19)       /* erx, c */
#define STATUS_UNDERRUN             (1 << 18)       /* ex, c */
#define STATUS_OVERRUN              (1 << 17)       /* ex, c */
#define STATUS_CID_CSD_OVERWRITE    (1 << 16)       /* erx, c, CID/CSD overwrite */
#define STATUS_WP_ERASE_SKIP        (1 << 15)       /* sx, c */
#define STATUS_CARD_ECC_DISABLED    (1 << 14)       /* sx, a */
#define STATUS_ERASE_RESET          (1 << 13)       /* sr, c */
#define STATUS_STATUS(x)            (x & 0xFFFFE000)
#define STATUS_CURRENT_STATE(x)     ((x & 0x00001E00) >> 9) /* sx, b (4 bits) */
#define STATUS_STATE_MASK           (0xf<<9)
#define STATUS_STATE_SHIFT          (9)
#define STATUS_READY_FOR_DATA       (1 << 8)        /* sx, a */
#define STATUS_APP_CMD              (1 << 5)        /* sr, c */

/*
 * Card Command Classes (CCC)
 */
#define CCC_BASIC               (1<<0)  /* (0) Basic protocol functions */
                                        /* (CMD0,1,2,3,4,7,9,10,12,13,15) */
#define CCC_STREAM_READ         (1<<1)  /* (1) Stream read commands */
                                        /* (CMD11) */
#define CCC_BLOCK_READ          (1<<2)  /* (2) Block read commands */
                                        /* (CMD16,17,18) */
#define CCC_STREAM_WRITE        (1<<3)  /* (3) Stream write commands */
                                        /* (CMD20) */
#define CCC_BLOCK_WRITE         (1<<4)  /* (4) Block write commands */
                                        /* (CMD16,24,25,26,27) */
#define CCC_ERASE               (1<<5)  /* (5) Ability to erase blocks */
                                        /* (CMD32,33,34,35,36,37,38,39) */
#define CCC_WRITE_PROT          (1<<6)  /* (6) Able to write protect blocks */
                                        /* (CMD28,29,30) */
#define CCC_LOCK_CARD           (1<<7)  /* (7) Able to lock down card */
                                        /* (CMD16,CMD42) */
#define CCC_APP_SPEC            (1<<8)  /* (8) Application specific */
                                        /* (CMD55,56,57,ACMD*) */
#define CCC_IO_MODE             (1<<9)  /* (9) I/O mode */
                                        /* (CMD5,39,40,52,53) */
#define CCC_SWITCH              (1<<10) /* (10) High speed switch */
                                        /* (CMD6,34,35,36,37,50) */
                                        /* (11) Reserved */
                                        /* (CMD?) */

/*
 * CSD field definitions
 */
#define CSD_STRUCT_VER_1_0  (0)   /* Version 1.0 - 1.2 */
#define CSD_STRUCT_VER_1_1  (1) /* Version 1.4 - 2.2 */
#define CSD_STRUCT_VER_1_2  (2) /* Version 3.1       */

#define CSD_SPEC_VER_0  (0) /* Version 1.0 - 1.2 */
#define CSD_SPEC_VER_1  (1) /* Version 1.4       */
#define CSD_SPEC_VER_2  (2) /* Version 2.0 - 2.2 */
#define CSD_SPEC_VER_3  (3) /* Version 3.0 - 3.1 */

#define SD_MANFACT_PANASONIC (1) /* ??? */
#define SD_MANFACT_TOSHIBA (2)
#define SD_MANFACT_SANDISK (3) /* Found in Sandisk v2.2 table 3.10 */
#define SD_MANFACT_UNKNOWN1  (6)
#define SD_MANFACT_UNKWOWN2  (0x27)  /* This manufacturer builds bad cards */

#define CSD_BITS (8*16)
#define CSD_BIT_DSR_IMP ((CSD_BITS-1) - 76)

#define OCR_NOTBUSY (1<<31)
#define OCR_CCS   (1<<30) /* Card capacity status */
#define OCR_VOLT_27_28      (1 << 15)
#define OCR_VOLT_28_29      (1 << 16)
#define OCR_VOLT_29_30      (1 << 17)
#define OCR_VOLT_30_31      (1 << 18)
#define OCR_VOLT_31_32      (1 << 19)
#define OCR_VOLT_32_33      (1 << 20)
#define OCR_VOLT_33_34      (1 << 21)
#define OCR_VOLT_34_35      (1 << 22)
#define OCR_VOLT_35_36      (1 << 23)
#define OCR_VOLT_MSK    (0x00ff8000)


/*
 * Definitions for SPI mode
 */

#define SPIR1_IDLE    (1 << 0)
#define SPIR1_ERASE_RESET (1 << 1)
#define SPIR1_ILLEGAL_CMD (1 << 2)
#define SPIR1_COM_CRC_ERR (1 << 3)
#define SPIR1_ERASE_SEQ_ERR (1 << 4)
#define SPIR1_ADDR_ERR    (1 << 5)
#define SPIR1_PARAM_ERR   (1 << 6)

#define SPIR2_CARD_LCKD   (1<<0)
#define SPIR2_WPERSKIPLCK_FAIL  (1<<1)
#define SPIR2_ERROR   (1<<2)
#define SPIR2_CC_ERR    (1<<3)
#define SPIR2_CARD_ECC_FAIL (1<<4)
#define SPIR2_WP_VIOLATION  (1<<5)
#define SPIR2_ERASE_PARAM (1<<6)
#define SPIR2_OOR_CSD_OVER  (1<<7)



#define SDIO_CCCR_IOEx    0x02
#define SDIO_CCCR_IORx    0x03

#define SDIO_CCCR_IENx    0x04  /* Function/Master Interrupt Enable */
#define SDIO_CCCR_INTx    0x05  /* Function Interrupt Pending */

#define SDIO_CCCR_ABORT   0x06  /* function abort/card reset */

#define SDIO_CCCR_IF    0x07  /* bus interface controls */


#endif /* SDPROTO_HH_ */
