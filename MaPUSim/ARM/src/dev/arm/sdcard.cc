/*
 * sdcard.cc
 *
 *  Created on: Mar 24, 2014
 *      Author: wangl
 */

#include "dev/arm/sdcard.hh"
#include "dev/arm/mapu_sdc.hh"
#include "debug/SdCard.hh"

const SdCard::SdCardSpec SdCard::cardspec[] = {
  {
    /*
     * ----------------------------------------------
     * This card was sent by amazon as a substitute
     * for the Kingston1G.bad with Manuf. ID 0x27
     * Kingston Elite Pro 1GB 50x 3.3V
     * This card is working reliable
     * ----------------------------------------------
     */
    "", "Kingston1G", "", CARD_TYPE_SD, 5000, 0x80ff8000,
    { 0x1f, 0x53, 0x4b, 0x53, 0x44, 0x31, 0x47, 0x42,
      0x10, 0x21, 0x98, 0x07, 0xa0, 0x00, 0x68, 0x00, },
    { 0x00, 0x2f, 0x01, 0x32, 0x5f, 0x59, 0x83, 0xce,
      0x36, 0xdb, 0xdf, 0xff, 0x9e, 0x40, 0x00, 0x00, },
    { 0x01, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, },
    { 0 }, 0
  },
#if 0
  {
    /*
     * ----------------------------------------------
     * another version of Kingston elite pro 50 x
     * testskript 25M: 75s
     * ----------------------------------------------
     */
    .producttype = "Kingston1G_2",
    .type = CARD_TYPE_SD,
    .ocr = 0x80ff8000,
    .usec_reset = 5000,
    .csd = {
      0x00,0x0e,0x01,0x32,0x13,0x59,0x83,0xb0,
      0xff,0xff,0xff,0xff,0x12,0x40,0x00,0x00,
    },
    .cid = {
      0x01, 0x50, 0x41, 0x53, 0x51, 0x30, 0x31, 0x47,
      0x63, 0x9a, 0x80, 0x58, 0xaf, 0x00, 0x61, 0x00,
    },
    .scr = {
      0x00, 0xa5, 0x00, 0x00, 0x06, 0x63, 0x00, 0x00,
    },
  },
  {
    /* testscript 25M 20 sec */
    .producttype = "Corsair1G",
    .type = CARD_TYPE_SD,
    .ocr = 0x80ff8000,
    .usec_reset = 5000,
    .csd = {
      0x00,0x5e,0x00,0x32,0x5f,0x59,0x83,0xd0,
      0x6d,0xb7,0xbf,0x9f,0x96,0x40,0x00,0x00,
    },
    .cid = {
      0x1d, 0x41, 0x44, 0x53, 0x44, 0x20, 0x20, 0x20,
      0x10, 0x00, 0x01, 0xb6, 0x1e, 0x00, 0x62, 0x00,
    },
    .scr = {
      0x01, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    }
  },
  {
    /* White Toshiba card for FS3 */
    .producttype = "Toshiba1G",
    .type = CARD_TYPE_SD,
    .ocr = 0x80ff8000,
    .usec_reset = 5000,
    .csd = {
      0x00,0x2d,0x00,0x32,0x5b,0x59,0x83,0xd6,
      0x7e,0xfb,0xff,0x80,0x12,0x40,0x00,0x00,
    },
    .cid = {
      0x02, 0x54, 0x4d, 0x53, 0x44, 0x30, 0x31, 0x47,
      0x25, 0x92, 0xe0, 0x7b, 0x2d, 0x00, 0x66, 0x00,
    },
    .scr = {
      0x01, 0xa5, 0x00, 0x00, 0x16, 0x01, 0x01, 0x02,
    },
  },
  {
    /* My card */
    .producttype = "TranscendMini1G",
    .type = CARD_TYPE_SD,
    .ocr = 0x80ff8000,
    .usec_reset = 5000,
    .rca = 0x0002,
    .csd = {
      0x00,0x2f,0x01,0x32,0x5f,0x59,0x83,0xbd,
      0xf6,0xdb,0xdf,0xff,0x9e,0x40,0x00,0x00,
    },
    .cid = {
      0x1b, 0x53, 0x4d, 0x53, 0x44, 0x4d, 0x20, 0x20,
      0x00, 0x00, 0x00, 0x85, 0xac, 0x00, 0x69, 0x00,
    },
    .scr = {
      0x01, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    },
    .ssr = {
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x34,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x34, 0x00, 0x34,
      0x00, 0x34, 0x00, 0x34, 0x00, 0x34, 0x00, 0x34,
      0x00, 0x34, 0x00, 0x34, 0x00, 0x34, 0x00, 0x34,
      0x00, 0x34, 0x00, 0x34, 0x00, 0x34, 0x00, 0x34,
      0x00, 0x34, 0x00, 0x34, 0x00, 0x34, 0x00, 0x34,
      0x00, 0x34, 0x00, 0x34, 0x00, 0x34, 0x00, 0x34,

    },
  },
  {
    .producttype = "SandiskMicro1G",
    .type = CARD_TYPE_SD,
    .ocr = 0x80ff8000,
    .usec_reset = 5000,
    .rca = 0xa785,
    .csd = {
      0x00,0x26,0x00,0x32,0x5f,0x59,0x83,0xc8,
      0xbe,0xfb,0xcf,0xff,0x92,0x40,0x40,0x00,
    },
    .cid = {
      0x03, 0x53, 0x44, 0x53, 0x55, 0x30, 0x31, 0x47,
      0x80, 0x00, 0x13, 0x8b, 0x03, 0x00, 0x6c, 0x00,
    },
    .scr = {
      0x02, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    },
    .ssr = {
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x28,
      0x01, 0x01, 0x90, 0x00, 0x0a, 0x05, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x28, 0x00, 0x28,
      0x00, 0x28, 0x00, 0x28, 0x00, 0x28, 0x00, 0x28,
      0x00, 0x28, 0x00, 0x28, 0x00, 0x28, 0x00, 0x28,
      0x00, 0x28, 0x00, 0x28, 0x00, 0x28, 0x00, 0x28,
      0x00, 0x28, 0x00, 0x28, 0x00, 0x28, 0x00, 0x28,
      0x00, 0x28, 0x00, 0x28, 0x00, 0x28, 0x00, 0x28,

    },
  },
  {
    .producttype = "SiliconPower1G",
    .type = CARD_TYPE_SD,
    .ocr = 0x80ff8000,
    .usec_reset = 5000,
    .rca = 0x0002,
    .csd = {
      0x00,0x2f,0x01,0x32,0x5f,0x59,0x83,0xb9,
      0xf6,0xdb,0xdf,0xff,0x9e,0x40,0x00,0x00,
    },
    .cid = {
      0x31, 0x53, 0x50, 0x20, 0x20, 0x20, 0x20, 0x20,
      0x00, 0x00, 0x68, 0x00, 0x0b, 0x00, 0x6a, 0x00,
    },
    .scr = {
      0x01, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    },
    .ssr = {
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0x00, 0x50,
      0x00, 0x50, 0x00, 0x50, 0x00, 0x50, 0x00, 0x50,
      0x00, 0x50, 0x00, 0x50, 0x00, 0x50, 0x00, 0x50,
      0x00, 0x50, 0x00, 0x50, 0x00, 0x50, 0x00, 0x50,
      0x00, 0x50, 0x00, 0x50, 0x00, 0x50, 0x00, 0x50,
      0x00, 0x50, 0x00, 0x50, 0x00, 0x50, 0x00, 0x50,

    },
  },
  {
    /* My card */
    .producttype = "X2G",
    .type = CARD_TYPE_SD,
    .ocr = 0x80ff8000,
    .usec_reset = 5000,
    .rca = 0xb368,
    .csd = {
      0x00,0x5e,0x00,0x32,0x5f,0x5a,0x83,0xbd,
      0x2d,0xb7,0xff,0xbf,0x96,0x80,0x00,0x00,
    },
    .cid = {
      0x1b, 0x53, 0x4d, 0x53, 0x44, 0x20, 0x20, 0x20,
      0x10, 0xca, 0x20, 0x74, 0x74, 0x00, 0x72, 0x00,
    },
    .scr = {
      0x01, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    },
    .ssr = {
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x28,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x28, 0x00, 0x28,
      0x00, 0x28, 0x00, 0x28, 0x00, 0x28, 0x00, 0x28,
      0x00, 0x28, 0x00, 0x28, 0x00, 0x28, 0x00, 0x28,
      0x00, 0x28, 0x00, 0x28, 0x00, 0x28, 0x00, 0x28,
      0x00, 0x28, 0x00, 0x28, 0x00, 0x28, 0x00, 0x28,
      0x00, 0x28, 0x00, 0x28, 0x00, 0x28, 0x00, 0x28,

    },
  },
  {
    /* My card */
    .producttype = "Kingston_uSD_2G",
    .type = CARD_TYPE_SD,
    .ocr = 0x80ff8000,
    .usec_reset = 10000,
    .rca = 0xb368,
    .csd = {
      0x00,0x2e,0x00,0x32,0x5b,0x5a,0x83,0xa9,
      0xff,0xff,0xff,0x80,0x16,0x80,0x00,0x91
    },
    .cid = {
      0x02,0x54,0x4d,0x53,0x44,0x30,0x32,0x47,
      0x38,0xa1,0xce,0x7b,0x4c,0x00,0x9b,0xf3,
    },
    .scr = {
      0x01, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    },
  },
  {
    /* MSW private card from Reichelt Vendor "PR" */
    .producttype = "Platinum4G_1",
    .type = CARD_TYPE_SD,
    .ocr = 0x80ff8000,
    .usec_reset = 5000,
    .csd = {
      0x00,0x2f,0x01,0x32,0x5f,0x5b,0x83,0xb0,
      0xf6,0xdb,0x9f,0xff,0x9e,0xc0,0x00,0x00,
    },
    .cid = {
      0x30, 0x50, 0x52, 0x20, 0x20, 0x20, 0x20, 0x20,
      0x00, 0x00, 0x20, 0x00, 0x66, 0x00, 0x71, 0x00,
    },
    .scr = {
      0x01, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    }
  },
  {
    /* MSW private card from Reichelt Vendor "DY" */
    .producttype = "Platinum4G_2",
    .type = CARD_TYPE_SD,
    .ocr = 0x80ff8000,
    .usec_reset = 5000,
    .csd = {
      0x00,0x2e,0x01,0x32,0x5f,0x5b,0x83,0xbb,
      0x36,0xdb,0xdf,0xff,0x9e,0xc0,0x00,0x00,
    },
    .cid = {
      0x19, 0x44, 0x59, 0x53, 0x44, 0x20, 0x20, 0x20,
      0x00, 0x00, 0x00, 0x18, 0x52, 0x00, 0x64, 0x00,
    },
    .scr = {
      0xd2, 0xc2, 0x3f, 0x09, 0x1b, 0xf0, 0x17, 0x21,
    }
  },
  {
    /*
     * ----------------------------------------------------------
     * This card has a unknown algorithm for changing the rca
     * when calling  SD_SEND_RELATIVE_ADDR
     * 0xb368 -> 0x66d0 ..
     * ----------------------------------------------------------
     */
    .producttype = "Transcend4GB",
    .type = CARD_TYPE_SD,
    .ocr = 0x80ff8000,
    .usec_reset = 5000,
    .rca = 0xb368,
    .csd = {
      0x00,0x5e,0x00,0x32,0x5f,0x5b,0x83,0xd5,
      0x6d,0xb7,0xff,0xff,0x96,0xc0,0x00,0x00,
    },
    .cid = {
      0x1c, 0x53, 0x56, 0x53, 0x44, 0x43, 0x20, 0x20,
      0x10, 0x00, 0x02, 0xb6, 0xb3, 0x00, 0x72, 0x00,
    },
    .scr = {
      0x01, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    },
    .ssr = {
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x28,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x28, 0x00, 0x28,
      0x00, 0x28, 0x00, 0x28, 0x00, 0x28, 0x00, 0x28,
      0x00, 0x28, 0x00, 0x28, 0x00, 0x28, 0x00, 0x28,
      0x00, 0x28, 0x00, 0x28, 0x00, 0x28, 0x00, 0x28,
      0x00, 0x28, 0x00, 0x28, 0x00, 0x28, 0x00, 0x28,
      0x00, 0x28, 0x00, 0x28, 0x00, 0x28, 0x00, 0x28,

    },
  },
  {
    /*
     * My card from Reichelt
     */
    .producttype = "CnMemory4GB",
    .type = CARD_TYPE_SD,
    .ocr = 0x80ff8000,
    .usec_reset = 5000,
    .rca = 0x1234,
    .csd = {
      0x00,0x4f,0x00,0x32,0x5f,0x5b,0x83,0xd4,
      0xf6,0xdb,0xff,0x8f,0x8a,0xc0,0x00,0x00,
    },
    .cid = {
      0x27, 0x50, 0x48, 0x53, 0x44, 0x30, 0x34, 0x47,
      0x11, 0xe5, 0xe0, 0x0e, 0x3b, 0x00, 0x72, 0x00,
    },
    .scr = {
      0x01, 0x25, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
    },
    .ssr = {
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x08,
      0x00, 0x08, 0x00, 0x08, 0x00, 0x08, 0x00, 0x08,
      0x00, 0x08, 0x00, 0x08, 0x00, 0x08, 0x00, 0x08,
      0x00, 0x08, 0x00, 0x08, 0x00, 0x08, 0x00, 0x08,
      0x00, 0x08, 0x00, 0x08, 0x00, 0x08, 0x00, 0x08,
      0x00, 0x08, 0x00, 0x08, 0x00, 0x08, 0x00, 0x08,

    },
  },
  {
    .producttype = "Sandisk4GB_HC",
    .type = CARD_TYPE_SD,
    .ocr = 0xc0ff8000,
    .usec_reset = 5000,
    .rca = 0xe624,
    .csd = {
      0x40,0x0e,0x00,0x32,0x5b,0x59,0x00,0x00,
      0x1e,0x5c,0x7f,0x80,0x0a,0x40,0x40,0x00,
    },
    .cid = {
      0x03, 0x53, 0x44, 0x53, 0x44, 0x30, 0x34, 0x47,
      0x80, 0x21, 0x09, 0x3b, 0x01, 0x00, 0x72, 0x00,
    },
    .scr = {
      0x02, 0x35, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    },
    .ssr = {
      0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
      0x01, 0x01, 0x90, 0x00, 0x14, 0x05, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    },

  },
  {
    .producttype = "Corsair4G_HC",
    .type = CARD_TYPE_SD,
    .ocr = 0xC0ff8000,
    .usec_reset = 5000,
    .csd = {
      0x40,0x7f,0x0f,0x32,0x5b,0x59,0x80,0x00,
      0x1e,0x44,0x7f,0x80,0x16,0x40,0x00,0x00,
    },
    .cid = {
      0x1d, 0x41, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x12, 0x00, 0x6c, 0x00,
    },
    .scr = {
      0x02, 0xa5, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    },
    .ssr = {
      0x00, 0x00, 0x00, 0x00, 0x02, 0x80, 0x00, 0x00,
      0x01, 0x01, 0x50, 0x00, 0x01, 0x05, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    },
  },
  {
    /* My 4GB SDHC card from Toshiba bought at Reichelt */
    .producttype = "Toshiba4G_HC",
    .type = CARD_TYPE_SD,
    .ocr = 0xC0ff8000,
    .usec_reset = 5000,
    .rca = 0x35a7,
    .csd = {
      0x40,0x0e,0x00,0x32,0x5b,0x59,0x00,0x00,
      0x1d,0xff,0x7f,0x80,0x0a,0x40,0x00,0x00,
    },
    .cid = {
      0x02, 0x54, 0x4d, 0x53, 0x44, 0x30, 0x34, 0x47,
      0x30, 0xb0, 0x75, 0xd5, 0x17, 0x00, 0x6b, 0x00,
    },
    .scr = {
      0x02, 0xb5, 0x00, 0x00, 0x18, 0x01, 0x11, 0x02,
    },
    .ssr = {
      0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
      0x02, 0x02, 0x90, 0x02, 0x00, 0xaa, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    }

  },
  {
    .producttype = "Transcend4G_HC",
    .type = CARD_TYPE_SD,
    .ocr = 0xC0ff8000,
    .usec_reset = 5000,
    .csd = {
      0x40,0x7f,0x0f,0x32,0x5b,0x59,0x80,0x00,
      0x1e,0x44,0x7f,0x80,0x16,0x40,0x00,0x00,
    },
    .cid = {
      0x1c, 0x53, 0x56, 0x53, 0x44, 0x48, 0x43, 0x00,
      0x0a, 0x00, 0x00, 0x23, 0xd5, 0x00, 0x6b, 0x00,
    },
    .scr = {
      0x02, 0xa5, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    },
    .ssr = {
      0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
      0x03, 0x01, 0x90, 0x00, 0x01, 0x05, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    },
  },
  {
    .producttype = "Kingston4G_HC",
    .type = CARD_TYPE_SD,
    .ocr = 0xC0ff8000,
    .usec_reset = 5000,
    .rca = 0x1234,
    .csd = {
      0x40,0x0e,0x00,0x32,0x5b,0x59,0x00,0x00,
      0x1e,0x77,0x7f,0x80,0x0a,0x40,0x00,0x00,
    },
    .cid = {
      0x41, 0x34, 0x32, 0x53, 0x44, 0x34, 0x47, 0x42,
      0x20, 0x5b, 0x00, 0x00, 0xa7, 0x00, 0x71, 0x00,
    },
    .scr = {
      0x02, 0x35, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
    },
    .ssr = {
      0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
      0x02, 0x02, 0x90, 0x00, 0x01, 0x06, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    },

  },
  {
    .producttype = "Transcend8G_HC",
    .type = CARD_TYPE_SD,
    .ocr = 0xC0ff8000,
    .usec_reset = 5000,
    .rca = 1,
    .csd = {
      0x40,0x0e,0x00,0x32,0x5b,0x59,0x00,0x00,
      0x3c,0x4a,0x7f,0x80,0x0a,0x40,0x00,0x00,
    },
    .cid = {
      0x1c, 0x53, 0x56, 0x53, 0x44, 0x48, 0x43, 0x00,
      0x10, 0x00, 0x00, 0x04, 0xc2, 0x00, 0x72, 0x00,
    },
    .scr = {
      0x02, 0xa5, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    },
    .ssr = {
      0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
      0x01, 0x01, 0x90, 0x00, 0x08, 0x05, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    },

  },
  {
    .producttype = "Hama8G_HC",
    .type = CARD_TYPE_SD,
    .ocr = 0xC0ff8000,
    .usec_reset = 5000,
    .rca = 0x1234,
    .csd = {
      0x40,0x0e,0x00,0x32,0x5b,0x59,0x00,0x00,
      0x3d,0x0f,0x7f,0x80,0x0a,0x40,0x00,0x00,
    },
    .cid = {
      0x27, 0x50, 0x48, 0x53, 0x44, 0x30, 0x38, 0x47,
      0x20, 0xeb, 0x60, 0x25, 0xd2, 0x00, 0x71, 0x00,
    },
    .scr = {
      0x02, 0x35, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
    },
    .ssr = {
      0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
      0x01, 0x01, 0x90, 0x00, 0x01, 0x06, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    },

  },
#endif
  {
    "", "Adata8G_HC", "", CARD_TYPE_SD, 5000, 0xC0ff8000,
    { 0x1c, 0x53, 0x56, 0x53, 0x44, 0x48, 0x43, 0x00,
      0x0a, 0x00, 0x00, 0x23, 0xd5, 0x00, 0x6b, 0x00, },
    /*{ 0x1d, 0x41, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0xc6, 0x00, 0x71, 0x00, },*/
    { 0x40,0x0e,0x00,0x32,0x5b,0x59,0x00,0x00,
      0x3c,0x4a,0x7f,0x80,0x0a,0x40,0x00,0x00, },
    { 0x02, 0xa5, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, },
    { 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
      0x01, 0x01, 0x90, 0x00, 0x08, 0x05, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, },
    0x0001,
  },
};


/*
 ********************************************************************+
 * Translation of status codes from Parallel to SPI mode
 ********************************************************************+
 */
uint8_t SdCard::translate_status_to_spir1(uint32_t card_status) {
  uint8_t spir1 = 0;
  if(card_status & STATUS_ERASE_RESET) {
    spir1 |= SPIR1_ERASE_RESET;
  }
  if(card_status & STATUS_COM_CRC_ERROR) {
    spir1 |= SPIR1_COM_CRC_ERR;
  }
  if(card_status & STATUS_ERASE_SEQ_ERROR) {
    spir1 |= SPIR1_ERASE_SEQ_ERR;
  }
  if(card_status & STATUS_ADDRESS_ERROR) {
    spir1 |= SPIR1_ADDR_ERR;
  }
  return spir1;
}

void SdCard::translate_status_to_spir2(uint32_t card_status,uint8_t *spir2) {
  spir2[0] = translate_status_to_spir1(card_status);
  spir2[1] = 0;
  if(card_status & STATUS_CARD_IS_LOCKED) {
    spir2[1] |= SPIR2_CARD_LCKD;
  }
  if(card_status & STATUS_WP_ERASE_SKIP) {
    spir2[1] |= SPIR2_WPERSKIPLCK_FAIL;
  }
  if(card_status & STATUS_ERROR) {
    spir2[1] |= SPIR2_ERROR;
  }
  if(card_status & STATUS_CC_ERROR) {
    spir2[1] |= SPIR2_CC_ERR;
  }
  if(card_status & STATUS_CARD_ECC_FAILED) {
    spir2[1] |= SPIR2_CARD_ECC_FAIL;
  }
  if(card_status & STATUS_WP_VIOLATION) {
    spir2[1] |= SPIR2_WP_VIOLATION;
  }
  if(card_status & STATUS_ERASE_PARAM) {
    spir2[1] |= SPIR2_ERASE_PARAM;
  }
  if(card_status & STATUS_CID_CSD_OVERWRITE) {
    spir2[1] |= SPIR2_OOR_CSD_OVER;
  }
}

/*
 ************************************************************
 * Bit access functions for card registers cid/csd/scr
 ************************************************************
 */
uint32_t SdCard::getbits(const uint8_t *arr, int arrlen, int from, int to) {
  int i;
  uint32_t val = 0;
  for(i = to; i >= from; i--) {
    int idx = arrlen - 1 - (i >> 3);
    int bit = i & 7;
    if(arr[idx] & (1 << bit)) {
      val = (val << 1) | 1;
    } else {
      val = (val << 1);
    }
  }
  return val;
}

void SdCard::copybits(uint32_t val, uint8_t *arr, int arrlen, int from, int to) {
  int i;
  for(i = from; i <= to; i++) {
    int idx = arrlen - 1 - (i >> 3);
    int bit = i & 7;
    if(val & 1) {
      arr[idx] |= (1 << bit);
    } else {
      arr[idx] &= ~(1 << bit);
    }
    val = (val >> 1);
  }
}
/*
 * ----------------------------------------------------------
 * csd_get_blocklen
 *  Get the blocklen from CSD structure
 * ----------------------------------------------------------
 */
uint32_t SdCard::csd_get_blocklen(const uint8_t *_csd, int cardtype) {
  int csd_structure;
  uint32_t _blocklen;
  int csdsize = 16;
  csd_structure = getbits(_csd, csdsize, 126, 127);
  if(cardtype == CARD_TYPE_SD) {
    switch(csd_structure) {
      case 0:
        _blocklen = 1 << getbits(_csd, csdsize, 80, 83);
        break;
      case 1:
        _blocklen = 512;
        break;
      default:
        _blocklen = 512;
        panic("Unknown CSD structure version\n");
    }
  } else if(cardtype == CARD_TYPE_MMC) {
    switch(csd_structure) {
      case 0:
      case 1:
        _blocklen = 1 << getbits(_csd, csdsize, 80, 83);
        break;
      default:
        _blocklen = 0;
        panic("Unknown CSD structure version\n");
    }
  } else {
    panic("MMCard: Unknown card type %d\n", cardtype);
  }
  return _blocklen;
}

/*
 * ------------------------------------------------------
 * csd_get_capacity
 *  Read the card capacity from the CSD structure.
 *  Depends on cardtype SD/MMC and on CSD structure
 *  version.
 * ------------------------------------------------------
 */
uint64_t SdCard::csd_get_capacity(const uint8_t *_csd, int cardtype)
{
  int csd_structure;
  uint32_t c_size_mult;
  uint64_t c_size;
  uint64_t _capacity;
  uint32_t _blocklen;
  int csdsize = 16;
  csd_structure = getbits(_csd, csdsize, 126, 127);
  _blocklen = csd_get_blocklen(_csd, cardtype);
  if(cardtype == CARD_TYPE_SD) {
    switch(csd_structure) {
      case 0:
        c_size = getbits(_csd, csdsize, 62, 73);
        c_size_mult = getbits(_csd, csdsize, 47, 49);
        _capacity = _blocklen * ((c_size + 1) << (c_size_mult + 2));
        break;
      case 1:
        c_size = getbits(_csd, csdsize, 48, 69);
        _capacity = (c_size + 1) * 512 * 1024;
        break;
      default:
        _capacity = 0;
        panic("Unknown CSD structure version\n");
    }
  } else if(cardtype == CARD_TYPE_MMC) {
    switch(csd_structure) {
      case 0:
      case 1:
        c_size = getbits(_csd, csdsize, 62, 73);
        c_size_mult = getbits(_csd, csdsize, 47, 49);
        _capacity = _blocklen * ((c_size + 1) << (c_size_mult + 2));
        break;
      default:
        _blocklen = 0;
        _capacity = 0;
        panic("Unknown CSD structure version\n");
    }
  } else {
    _capacity = 0;
    panic("Unknown cardtype %d\n", cardtype);
  }
  return _capacity;
}

/*
 * ----------------------------------------------------------------------------
 * dump_cardtypes
 *  Print a list of all implemented Cards with some technical information
 * ----------------------------------------------------------------------------
 */
const char *SdCard::curr_max[8] =  {
  "  1mA",
  "  5mA",
  " 10mA",
  " 25mA",
  " 35mA",
  " 45mA",
  " 80mA",
  "100mA",
};

void SdCard::dump_cardtypes() {
#ifdef TRACE_ON
  int nr_types = sizeof(cardspec)/sizeof(SdCardSpec);
  int i;
  const char *interface;
  char vendname[3] = {' ', ' ', 0};
  int manfact;
  const SdCardSpec *spec;
  DPRINTF(SdCard, "Possible MMC/SD Cards:\n");
  for(i = 0; i < nr_types; i++) {
    spec = &cardspec[i];
    int csdvers = getbits(spec->csd, 16, 126, 127);
    uint64_t size;
    int ccc;
    int speed_class;
    int blocklen = csd_get_blocklen(spec->csd, spec->type);
    //int erase_blk_en;
    int max_rd_current = GETBITS(spec->csd, 56, 58); /* only for csd-1.0 */
    int max_wr_current = GETBITS(spec->csd, 50, 52); /* only for csd-1.0 */
    //uint32_t psn;
    //psn = GETBITS(spec->cid, 24, 55);
    //erase_blk_en = GETBITS(spec->csd, 46, 46);
    ccc = GETBITS(spec->csd, 84, 95);
    manfact = spec->cid[0];
    if(isprint(spec->cid[1])) {
      vendname[0] = spec->cid[1];
    } else {
      vendname[0] = ' ';
    }
    if(isprint(spec->cid[2])) {
      vendname[1] = spec->cid[2];
    } else {
      vendname[1] = ' ';
    }
    if(spec->type == CARD_TYPE_SD) {
      if(csdvers == 0) {
        interface = "SD-Card  ";
      } else if (csdvers == 1) {
        interface = "SDHC-Card";
      } else {
        interface = "Unknown";
      }
    } else if (spec->type == CARD_TYPE_MMC) {
      interface = "MMC-Card ";
    } else {
      interface ="          ";
    }
    size = csd_get_capacity(spec->csd, spec->type);
    speed_class = GETBITS(spec->ssr,440,447) * 2;
    if(csdvers == 0) {
      DPRINTF(SdCard, "%s: 0x%02x \"%s\" \"%18s\" bl %4d sz %11lld Class(%d) "
              "CCC %03x Iw %s Ir %s\n", interface, manfact, vendname,
              spec->producttype, blocklen, (long long)size, speed_class, ccc,
              curr_max[max_wr_current], curr_max[max_rd_current]);
    } else {
      DPRINTF(SdCard, "%s: 0x%02x \"%s\" \"%18s\" bl %4d sz %11lld Class(%d) "
              "CCC %03x\n", interface, manfact, vendname, spec->producttype,
              blocklen, (long long)size, speed_class, ccc);
    }
  }
  DPRINTF(SdCard, "SD-Card  : \"auto_sd\"\n");
  DPRINTF(SdCard, "MMC-Card : \"auto_mmc\"\n");
  DPRINTF(SdCard, "SDHC-Card: \"auto_sdhc\"\n");
#endif
}

/*
 * ----------------------------------------------------------------
 * read:
 * retval: number of Bytes, -ERRCODE on error  0=eofdata
 * ----------------------------------------------------------------
 */
int SdCard::read(uint8_t *buf) {
  int i;
  if (state != STATE_DATA) {
    return 0;
  }
  if (cmd == (SD_APP_SEND_SCR | CMD_FLAG_IS_APPCMD)) {
    for (i = 0; transfer_count < 8; i++) {
      buf[i]= scr[transfer_count];
      transfer_count++;
    }
    if (transfer_count == 8) {
      state = STATE_TRANSFER;
    }
    return i;
  } else if (cmd == SD_SWITCH) {
    for (i = 0; transfer_count < 64; i++) {
      buf[i] = 0;
      transfer_count++;
    }
    if (transfer_count == 64) {
      state = STATE_TRANSFER;
    }
    return i;
  } else if (cmd == (SD_APP_SEND_NUM_WR_BLKS | CMD_FLAG_IS_APPCMD)) {
    for (i = 0; transfer_count < 4; i++) {
      buf[i] = (well_written_blocks >> ((3 - transfer_count) * 8)) & 0xff;
      transfer_count++;
    }
    if (transfer_count == 4) {
      state = STATE_TRANSFER;
    }
    //fprintf(stderr,"Send well written blocks %d state %d\n",card->well_written_blocks,card->state);
    return i;
  } else if (cmd == SD_READ_SINGLE_BLOCK) {
    uint64_t _address = address + transfer_count;
    if (_address >= capacity) {
      return 0;
    }
    if ((_address + blocklen) > capacity) {
      uint64_t count = capacity - _address;
      if (disk_image->read(buf, _address, count) < count) {
        DPRINTF(SdCard, "Error reading from diskimage\n");
      }
      transfer_count += count;
      if (transfer_count == blocklen) {
        state = STATE_TRANSFER;
      }
      return count;
    }
    if (disk_image->read(buf, _address, blocklen) < blocklen) {
      DPRINTF(SdCard, "Error reading from diskimage\n");
    }
    transfer_count += blocklen;
    if (transfer_count == blocklen) {
      state = STATE_TRANSFER;
    }
    return blocklen;
  } else if (cmd == SD_READ_MULTIPLE_BLOCK) {
    uint64_t _address = address + transfer_count;
    if ((_address + blocklen) > capacity) {
      uint64_t count = capacity - _address;
      if (disk_image->read(buf, _address, count) < count) {
        DPRINTF(SdCard, "Error reading from diskimage\n");
      }
      transfer_count += count;
      return count;
    }
    if (block_count) {
      if((_address & ~(blocklen - 1)) != ((_address + blocklen) & ~(blocklen - 1))) {
        block_count--;
        if(!block_count) {
          state = STATE_TRANSFER;
        }
      }
    }
    if (disk_image->read(buf, _address, blocklen) < blocklen) {
      DPRINTF(SdCard, "Error reading from diskimage\n");
    }
    transfer_count += blocklen;
    return blocklen;
  } else if (cmd == SD_READ_DAT_UNTIL_STOP) {
    uint64_t _address = address + transfer_count;
    if ((_address + blocklen) > capacity) {
      uint64_t count = capacity - _address;
      if (disk_image->read(buf, _address, count) < count) {
        DPRINTF(SdCard, "Error reading from diskimage\n");
      }
      transfer_count += count;
      return count;
    }
    if (disk_image->read(buf, _address, blocklen) < blocklen) {
      DPRINTF(SdCard, "Error reading from diskimage\n");
    }
    transfer_count += blocklen;
    return blocklen;
  } else if (cmd == (SD_APP_SEND_STATUS | CMD_FLAG_IS_APPCMD)) {
    for (i = 0; transfer_count < 64; i++) {
      buf[i]= ssr[transfer_count];
      transfer_count++;
    }
    DPRINTF(SdCard, "APP_SEND_STATUS count %d tc %d \n", blocklen, transfer_count);
    if (transfer_count == 64) {
      state = STATE_TRANSFER;
    }
    return i;
  } else {
    DPRINTF(SdCard, "SD card read with unknown command %d\n", cmd);
    return 0;
  }
}

/*
 * ----------------------------------------------------------------------
 * SdCard initiated transmission to host
 * The SdCard fires without checking if the host really has enough buffer
 * (Real cards have the same behaviour)
 * ----------------------------------------------------------------------
 */
void SdCard::StartTransmission() {
  /*
  if(CycleTimer_IsActive(&transmissionTimer)) {
    fprintf(stderr,"MMCard: Warning, transmission timer is already running !\n");
  } else {
    CycleTimer_Mod(&card->transmissionTimer,CycleTimerRate_Get() / 26600);
  }*/
  DoTransmission();
}

void SdCard::DoTransmission() {
  int result;
  //uint64_t cycles;
  //uint32_t freq;
/*
  if(CycleTimer_IsActive(&card->transmissionTimer)) {
    fprintf(stderr,"Error: Card Transmission timer is already running\n");
    return;
  }*/
  if(state != STATE_DATA) {
    return;
  }
  ///len = li->maxpkt < sizeof(li->buf) ? li->maxpkt : sizeof(li->buf);
  result = read(ctrl->buf);
  if(result <= 0) {
    return;
  }
//  fprintf(stderr,"MMCard: Do the transmission len %d, transfer cnt %d\n",result,card->transfer_count); // jk
  //MMC_CRC16Init(&dataBlock.crc,0);
  //MMC_CRC16(&dataBlock.crc,dataBlock.data,dataBlock.datalen);
  ctrl->receive_data(result);
  /*freq = Clock_Freq(card->clk);
  if(!freq) {
    freq = 1;
    fprintf(stderr,"Error: MMCard used with clock of 0 HZ\n");
  }
  if(card->type == CARD_TYPE_MMC) {
    cycles = NanosecondsToCycles(1000000000 / freq * 10 * result);
  } else {
    cycles = NanosecondsToCycles((1000000000/4) / freq * 10 * result);
  }
  if(CycleTimer_IsActive(&card->transmissionTimer)) {
    fprintf(stderr,"MMCard: Bug, transmission timer is already running !\n");
  } else {
    CycleTimer_Mod(&card->transmissionTimer,cycles);
  }*/
}


/*
 * ----------------------------------------------
 * CMD0: go_idle
 * No response
 * ----------------------------------------------
 */
int SdCard::sd_go_idle(uint32_t cmd, uint32_t arg, SdResponse *resp) {
  if(state == STATE_INACTIVE) {
    return SD_ERR_TIMEOUT;
  }
  state = STATE_IDLE;
  reset_state = RST_NOT_STARTED;
  rca=0;
  card_status = STATUS_READY_FOR_DATA;
  host_is_2_0 = 0;
  resp->len = 0;
  return SD_ERR_NONE;
}
/*
 * ----------------------------------------------
 * CMD0: SPI version
 * ----------------------------------------------
 */
int SdCard::spi_go_idle(uint32_t cmd, uint32_t arg, SdResponse *resp) {
  int result = sd_go_idle(cmd, arg, resp);
  resp->len = 1;
  resp->data[0] = SPIR1_IDLE;
  return result;
}

/*
 * -------------------------------------------------------------------------
 * MMC send op cond: CMD1 Response format R3
 * MMC only
 * -------------------------------------------------------------------------
 */
int SdCard::sd_send_op_cond(uint32_t cmd, uint32_t arg, SdResponse *resp) {
  if(state != STATE_IDLE) {
    DPRINTF(SdCard, "got SEND_OP_COND cmd in non idle state (%d)\n", state);
    resp->len = 0;
    return SD_ERR_TIMEOUT;
  }
  if(reset_state == RST_NOT_STARTED) {
    //reset_start_time = CycleCounter_Get();
    ocr &= ~OCR_NOTBUSY;
    reset_state = RST_STARTED;
  } else if(reset_state == RST_STARTED) {
    //int64_t usec = curTick();//CyclesToMicroseconds(CycleCounter_Get() - reset_start_time);
    //if(usec > usec_reset) {
      state = STATE_READY;
      ocr |= OCR_NOTBUSY;
      reset_state = RST_DONE;
      block_count = 0;
    //}
  } else if(reset_state != RST_DONE) {
    panic("Emulator bug: MMC-Card reset_state %d not valid\n", reset_state);
  }
  resp->len = 6;
  resp->data[0] = 0x3f;
  resp->data[1] = ocr >> 24;
  resp->data[2] = ocr >> 16;
  resp->data[3] = ocr >> 8;
  resp->data[4] = ocr;
  resp->data[5] = 0xff;
  DPRINTF(SdCard, "Send op cond arg 0x%08x\n",arg);
  return SD_ERR_NONE;
}

/*
 * -------------------------------------------------------------------------
 * CMD1 SPI version
 * -------------------------------------------------------------------------
 */
int SdCard::spi_send_op_cond(uint32_t cmd, uint32_t arg, SdResponse *resp) {
  int result = sd_send_op_cond(cmd, arg, resp);
  resp->len = 1;
  if(state == STATE_IDLE) {
    resp->data[0] = SPIR1_IDLE; /* in idle state */
  } else {
    resp->data[0] = 0;
  }
  DPRINTF(SdCard, "Send op cond arg 0x%08x\n",arg);
  return result;
}


/*
 * ---------------------------------------------------------------------
 * CMD2, no argument
 * Response format R2 (136 Bits)
 * doesn't respond when  rca is not 0.  (Toshiba docu says this)
 * ---------------------------------------------------------------------
 */
int SdCard::sd_all_send_cid(uint32_t cmd, uint32_t arg, SdResponse *resp) {
  int i;
  if(rca != 0) {
    return SD_ERR_TIMEOUT;
  }
  if(state != STATE_READY) {
    /* Real card seems to behave this way ? Or is it the controller ? */
    DPRINTF(SdCard, "ALL_SEND_CID: card not in ready state\n");
    resp->len = 0;
    return SD_ERR_TIMEOUT;
  }
  resp->len = 17;
  resp->data[0] = 0x3f;
  for(i = 0; i < 16; i++) {
    resp->data[1 + i] = cid[i];
  }
  state = STATE_IDENT;
  return SD_ERR_NONE;
}
/*
 * ---------------------------------------------------------------------------------
 * CMD3 for MMC cards: set relative address
 * for SD-Cards see sd_send relative address
 * Argument: Bits 16-31 RCA
 * Response format R1
 * --------------------------------------------------------------------------------
 */
int SdCard::sd_set_relative_addr(uint32_t cmd, uint32_t arg, SdResponse *resp) {
  uint32_t _card_status = GET_STATUS();
  if(state != STATE_IDENT) {
    DPRINTF(SdCard, "Got SET_RCA but not in IDENT state (%d)\n", state);
    resp->len = 0;
    return SD_ERR_TIMEOUT;
  }
  state = STATE_STBY;
  rca = (arg >> 16) & 0xffff;
  DPRINTF(SdCard, "New rca is %d\n", rca);
  card_status  &= ~(0xfd3fc020);
  resp->len = 6;
  resp->data[0] = cmd & 0x3f;
  resp->data[1] = (_card_status >> 24) & 0xff;
  resp->data[2] = (_card_status >> 16) & 0xff;
  resp->data[3] = (_card_status >> 8)  & 0xff;
  resp->data[4] = _card_status & 0xff;
  resp->data[5] = SD_RespCRCByte(resp);
  return SD_ERR_NONE;
}
/*
 * ---------------------------------------------------------------------------------
 * CMD3 for SD-Cards, send relative address
 * All cards seem to increment the RCA by one when CMD3 is called a second time
 * Argument: stuff bits
 * Response format R6
 * --------------------------------------------------------------------------------
 */
int SdCard::sd_send_relative_addr(uint32_t cmd, uint32_t arg, SdResponse *resp)
{
  uint32_t _card_status = GET_STATUS();
  if(state == STATE_IDENT) {
    rca = initial_rca;
  } else if (state == STATE_STBY) {
    rca++;
  } else {
    resp->len = 0;
    return SD_ERR_TIMEOUT;
  }
  state = STATE_STBY;
  if(!rca) rca++;
  //card->card_status  &= ~(0xfd3fc020);
  card_status  &= ~(0xc020);
  resp->len = 6;
  resp->data[0] = cmd & 0x3f;
  resp->data[1] = (rca >> 8) & 0xff;
  resp->data[2] = rca & 0xff;
  resp->data[3] = (_card_status >> 8)  & 0xff;
  resp->data[4] = _card_status & 0xff;
  resp->data[5] = SD_RespCRCByte(resp);
  return SD_ERR_NONE;
}

/*
 * -------------------------------------------------------------
 * CMD4 SET_DSR optional broadcast command
 * only present when bit 96 in CSD is set
 * STATE_STBY -> STATE_STBY (Sandisk state table)
 * -------------------------------------------------------------
 */
int SdCard::sd_set_dsr(uint32_t cmd, uint32_t arg, SdResponse *resp) {
  resp->len = 0;
  if(state != STATE_STBY) {
    DPRINTF(SdCard, "Got set dsr cmd in wrong state %d\n", state);
    return SD_ERR_TIMEOUT;
  }
  dsr = arg >> 16;
  return SD_ERR_NONE;
}

/*
 * ------------------------------------------------------------------------------
 * CMD6: SD_SWITCH
 * State: Transfer -> Data (involves data stage)
 * Valid in transfer state (Simplified Spec 2.0), Mandatory for SD >= 1.10
 * Response Format R1 and 512 Bit on the Data lines
 * ------------------------------------------------------------------------------
 */
int SdCard::sd_switch(uint32_t _cmd, uint32_t arg, SdResponse *resp) {
  uint32_t _card_status = GET_STATUS();
  if (state != STATE_TRANSFER) {
    DPRINTF(SdCard, "switch in wrong state %d\n", state);
    return SD_ERR_TIMEOUT;
  }
  state = STATE_DATA;
  card_status  &= ~(0xfd3fc020);
  cmd = _cmd;
  transfer_count = 0;
  resp->len = 6;
  resp->data[0] = _cmd & 0x3f;
  resp->data[1] = (_card_status >> 24) & 0xff;
  resp->data[2] = (_card_status >> 16) & 0xff;
  resp->data[3] = (_card_status >> 8)  & 0xff;
  resp->data[4] = _card_status & 0xff;
  resp->data[5] = SD_RespCRCByte(resp);
  StartTransmission();
  return SD_ERR_NONE;
}

/*
 * ------------------------------------------------------------------------------
 * CMD7 select Card
 * State: Stdby->Trans Dis->Prg when addressed
 *    STDBY|TRAN|DATA->STDBY , PRG->DIS when not addressed
 * Response format R1 arg argument RCA
 * ------------------------------------------------------------------------------
 */
int SdCard::sd_select_card(uint32_t cmd, uint32_t arg, SdResponse *resp) {
  uint16_t _rca = arg >> 16;
  uint32_t _card_status = GET_STATUS();
  if((rca != _rca) || (_rca == 0)) {
    if((state == STATE_STBY) || (state == STATE_TRANSFER) ||
       (state == STATE_DATA)) {
      state = STATE_STBY;
    } else if(state == STATE_PRG) {
      state = STATE_DIS;
    } else {
      DPRINTF(SdCard, "CMD7 unsel card not allowed in state %d\n", state);
      return SD_ERR_TIMEOUT;
    }
  } else {
    if((state == STATE_DIS)) {
      state = STATE_PRG;
    } else if (state == STATE_STBY) {
      state = STATE_TRANSFER;
    } else {
      DPRINTF(SdCard, "CMD7 sel card not allowed in state %d\n", state);
      return SD_ERR_TIMEOUT;
    }
  }
  card_status  &= ~(0xfd3fc020);
  resp->len = 6;
  resp->data[0] = cmd & 0x3f;
  resp->data[1] = (_card_status >> 24) & 0xff;
  resp->data[2] = (_card_status >> 16) & 0xff;
  resp->data[3] = (_card_status >> 8)  & 0xff;
  resp->data[4] = _card_status & 0xff;
  resp->data[5] = SD_RespCRCByte(resp);
  return SD_ERR_NONE;
}

/*
 * ----------------------------------------------------------------------------
 * CMD8
 *  SD_SEND_INTERFACE_COND
 *
 * Version: SD >= 2.0
 * Is mandatory before ACMD41 for high capacity cards !
 * State: Idle -> Idle
 * Response format R7
 * -----------------------------------------------------------------------------
 */
int SdCard::sd_send_interface_cond(uint32_t cmd, uint32_t arg, SdResponse *resp) {
  int vhs;
  int check_pattern;
  if(!spimode && (state != STATE_IDLE)) {
    DPRINTF(SdCard, "SD-CMD8 received in wrong state %d\n", state);
    return SD_ERR_TIMEOUT;
  }
  vhs = (arg >> 8) & 0xf;
  check_pattern = arg & 0xff;
  if(check_pattern != 0xaa) {
    DPRINTF(SdCard, "Received non recommended checkpattern\n");
  }
  host_is_2_0 = 1;
  resp->len = 6;
  resp->data[0] = cmd & 0x3f;
  resp->data[1] = 0;
  resp->data[2] = 0;
  resp->data[3] = vhs; /* always accept the voltage */
  resp->data[4] = check_pattern;
  resp->data[5] = SD_RespCRCByte(resp);
  return SD_ERR_NONE;
}

int SdCard::spi_send_interface_cond(uint32_t cmd, uint32_t arg, SdResponse *resp) {
  int result = sd_send_interface_cond(cmd, arg, resp);
  resp->len = 5;
  if(state == STATE_IDLE) {
    resp->data[0] = SPIR1_IDLE;
  } else {
    resp->data[0] = 0;
  }
  return result;
}

/*
 * -----------------------------------------------------------------------
 * CMD9 MMC_SEND_CSD
 *  read card specific data
 * arg: rca in high
 * response: 136 Bit R2
 * STATE_STBY -> STATE_STBY
 * -----------------------------------------------------------------------
 */
int SdCard::sd_send_csd(uint32_t cmd, uint32_t arg, SdResponse *resp) {
  int i;
  uint16_t _rca = arg >> 16;
  if(!spimode && (state != STATE_STBY)) {
    DPRINTF(SdCard, "SEND_CSD but not in standby state\n");
    return SD_ERR_TIMEOUT;
  }
  if(rca != _rca) {
    DPRINTF(SdCard, "SEND CSD: card not selected, rca %d\n", _rca);
    return SD_ERR_TIMEOUT;
  }
  resp->len = 17;
  resp->data[0] = 0x3f;
  for(i=0;i<16;i++) {
    resp->data[1+i] = csd[i];
  }
  return SD_ERR_NONE;
}

/*
 **************************************************************************
 * SPI version of CMD9
 * Is a little bit a hack, It includes the data frame in the response
 **************************************************************************
 */
int SdCard::spi_send_csd(uint32_t cmd, uint32_t arg, SdResponse *resp) {
  int i;
  uint16_t crc = 0;
  if(state == STATE_IDLE) {
    resp->data[0] = SPIR1_IDLE;
  } else {
    resp->data[0] = 0;
  }
  resp->data[1] = 0xfe; /* Start block marker */
  for(i=0;i<16;i++) {
    resp->data[2+i] = csd[i];
  }
  SD_CRC16(&crc, csd, 16);
  resp->data[18] = crc >> 8;
  resp->data[19] = crc & 0xff;
  resp->len = 20;
  return SD_ERR_NONE;
}

/*
 * -----------------------------------------------------------------
 * CMD10 SEND_CID
 * arg: rca
 * return R2 (128 + 8 Bits)
 * STATE_STBY -> STATE_STBY
 * -----------------------------------------------------------------
 */
int SdCard::sd_send_cid(uint32_t cmd, uint32_t arg, SdResponse *resp) {
  int i;
  uint16_t _rca = arg >> 16;
  if(state != STATE_STBY) {
    resp->len = 0;
    return SD_ERR_TIMEOUT;
  }
  if(rca != _rca) {
    DPRINTF(SdCard, "SEND CID: card not selected\n");
    return SD_ERR_TIMEOUT;
  }
  resp->len = 17;
  resp->data[0] = 0x3f;
  for(i=0;i<16;i++) {
    resp->data[1+i] = cid[i];
  }
  return SD_ERR_NONE;
}

/*
 *******************************************************
 * SPI version of send CID CMD10
 * Hack !, includes data frame in response
 *******************************************************
 */
int SdCard::spi_send_cid(uint32_t cmd, uint32_t arg, SdResponse *resp) {
  int i;
  if(state == STATE_IDLE) {
    resp->data[0] = SPIR1_IDLE;
  } else {
    resp->data[0] = 0;
  }
  resp->data[1] = 0xfe; /* Start block marker */
  for(i=0;i<16;i++) {
    resp->data[2+i] = cid[i];
  }
  resp->data[18] = 0xff;
  resp->data[19] = 0xff;
  resp->len = 20;
  return SD_ERR_NONE;
}

/*
 * ---------------------------------------------------------
 * CMD11 READ_DATA_UNTIL_STOP, Maybe only mmc ? not in SD-2.0
 * arg: none
 * resp R1
 * STATE ??? not found in SanDisk docu
 * ---------------------------------------------------------
 */
int SdCard::sd_read_dat_until_stop(uint32_t _cmd, uint32_t arg, SdResponse *resp) {
  uint32_t _card_status = GET_STATUS();
  if (state != STATE_TRANSFER) {
    DPRINTF(SdCard, "read data until stop in wrong state %d\n", state);
    return SD_ERR_TIMEOUT;
  }
  /* Store the attributes for the recognized data operation */
  cmd = SD_READ_DAT_UNTIL_STOP;
  address = arg;  /* Block alignement check required ? */
  transfer_count = 0;

  state = STATE_DATA;
  card_status  &= ~(0xfd3fc020);
  resp->len = 6;
  resp->data[0] = _cmd & 0x3f;
  resp->data[1] = (_card_status >> 24) & 0xff;
  resp->data[2] = (_card_status >> 16) & 0xff;
  resp->data[3] = (_card_status >> 8)  & 0xff;
  resp->data[4] = _card_status & 0xff;
  resp->data[5] = SD_RespCRCByte(resp);
  StartTransmission();
  return SD_ERR_NONE;
}

/*
 * ---------------------------------------------------------------------------------
 * CMD12 STOP_TRANSMISSION
 * arg: none
 * resp: R1b  (busy signal was never observed after CMD12 ????)
 * STATE: Read:  STATE_DATA -> STATE_TRANSFER
 *    Write: STATE_RCV -> STATE_PRG ... (delay) -> STATE_TRANSFER
 *
 * The data transfer stops after the end bit of the stop cmd according to
 * Simplified Spec V2.0
 * ---------------------------------------------------------------------------------
 */
int SdCard::sd_stop_transmission(uint32_t cmd, uint32_t arg, SdResponse *resp) {
  resp->len = 0;
  uint32_t _card_status = GET_STATUS();
  if((state != STATE_DATA) && (state != STATE_RCV)) {
    DPRINTF(SdCard, "STOP_TRANSMISSION but in wrong state: %d\n", state);
    return SD_ERR_TIMEOUT;
  }
  if(state == STATE_DATA) {
    state = STATE_TRANSFER;
  } else if(state == STATE_RCV) {
    /* This should go to STATE_PRG and after a delay go to STATE_TRANSFER ! */
    state = STATE_TRANSFER;
  } else {
    DPRINTF(SdCard, "Bug: stop tranmission in state %d\n", state);
  }
  card_status  &= ~(0xfd3fc020);
  resp->len = 6;
  resp->data[0] = cmd & 0x3f;
  resp->data[1] = (_card_status >> 24) & 0xff;
  resp->data[2] = (_card_status >> 16) & 0xff;
  resp->data[3] = (_card_status >> 8)  & 0xff;
  resp->data[4] = _card_status & 0xff;
  resp->data[5] = SD_RespCRCByte(resp);
  ctrl->setTDone();
  return SD_ERR_NONE;
}

int SdCard::spi_stop_transmission(uint32_t cmd, uint32_t arg, SdResponse *resp) {
  int result = sd_stop_transmission(cmd, arg, resp);
  resp->len = 1;
  if(state == STATE_IDLE) {
    resp->data[0] = SPIR1_IDLE; /* in idle state */
  } else {
    resp->data[0] = 0;
  }
  return result;
}

/*
 * -------------------------------------------------------------------
 * CMD13 MMC_SEND_STATUS
 * arg rca
 * response format R1
 * Keep STATE
 * -------------------------------------------------------------------
 */
int SdCard::sd_send_status(uint32_t cmd, uint32_t arg, SdResponse *resp) {
  uint32_t _card_status = GET_STATUS();
  uint16_t _rca = (arg >> 16) & 0xffff;
  if(rca != _rca) {
    DPRINTF(SdCard, "SEND STATUS: Card not selected\n");
    return SD_ERR_TIMEOUT;
  }
  if(!spimode && (state != STATE_STBY) && (state != STATE_TRANSFER) && (state != STATE_DATA)
    && (state != STATE_RCV) && (state != STATE_PRG) && (state != STATE_DIS)) {
    DPRINTF(SdCard, "SEND STATUS: in wrong state %d\n", state);
    return SD_ERR_TIMEOUT;
  }
  card_status  &= ~(0xfd3fc020);
  resp->len = 6;
  resp->data[0] = cmd & 0x3f;
  resp->data[1] = (_card_status >> 24) & 0xff;
  resp->data[2] = (_card_status >> 16) & 0xff;
  resp->data[3] = (_card_status >> 8)  & 0xff;
  resp->data[4] = _card_status & 0xff;
  resp->data[5] = SD_RespCRCByte(resp);
  DPRINTF(SdCard, "CMD13: Card status %08x\n", card_status);
  return SD_ERR_NONE;
}

/**
 ******************************************************************************************
 * \fn static int spi_send_status(MMCard *card,uint32_t cmd,uint32_t arg,MMCResponse *resp)
 ******************************************************************************************
 */
int SdCard::spi_send_status(uint32_t cmd, uint32_t arg, SdResponse *resp) {
  uint32_t _card_status = GET_STATUS();
  card_status  &= ~(0xfd3fc020);
  translate_status_to_spir2(_card_status, resp->data);
  resp->len = 2;
  if(state == STATE_IDLE) {
    resp->data[0] |= SPIR1_IDLE;
  }
  DPRINTF(SdCard, "CMD13: Card status %08x\n", card_status);
  return SD_ERR_NONE;
}

/*
 * -------------------------------------------------------------------------------
 * ACMD13
 * SD_APP_SEND_STATUS
 * State: Tran -> Data
 * Argument: ignored
 *
 * Response: SD-Card Spec says R2. I think this is wrong
 * Sandisk says R1 real card also, but maybe its R1b.
 * White unlabeled 256M card from Toshiba needs a pause of > 3msec after
 * this command or it will stop working
 * -------------------------------------------------------------------------------
 */
int SdCard::sd_app_send_status(uint32_t _cmd,uint32_t arg, SdResponse *resp) {
  uint32_t _card_status = GET_STATUS();
  if(state != STATE_TRANSFER) {
    DPRINTF(SdCard, "SD_SEND_STATUS in wrong state\n");
    return SD_ERR_TIMEOUT;
  }
  transfer_count = 0;
  state = STATE_DATA;
  cmd = _cmd | CMD_FLAG_IS_APPCMD;
  card_status  &= ~(0xfd3fc020);
  /** Missing here: set appcmd in status field !!!!!!!!!!!!!!!!!!!! */
  resp->len = 6;
  resp->data[0] = _cmd & 0x3f;
  resp->data[1] = (_card_status >> 24) & 0xff;
  resp->data[2] = (_card_status >> 16) & 0xff;
  resp->data[3] = (_card_status >> 8)  & 0xff;
  resp->data[4] = _card_status & 0xff;
  resp->data[5] = SD_RespCRCByte(resp);
  StartTransmission();
  return SD_ERR_NONE;
}

/*
 * ------------------------------------------------------------------
 * CMD15 GO_INACTIVE
 * arg RCA
 * response none
 * STATE_xxx -> STATE_INACTIVE (Sandisk manual)
 * ------------------------------------------------------------------
 */
int SdCard::sd_go_inactive_state(uint32_t cmd, uint32_t arg, SdResponse *resp) {
  uint16_t _rca = (arg >> 16) & 0xffff;
  resp->len = 0;
  if(rca != _rca) {
    DPRINTF(SdCard, "SEND STATUS: Card not selected\n");
    return SD_ERR_TIMEOUT;
  }
  if((state != STATE_STBY) && (state != STATE_TRANSFER) && (state != STATE_DATA)
    && (state != STATE_RCV) && (state != STATE_PRG) && (state != STATE_DIS)) {
    DPRINTF(SdCard, "SEND STATUS: in wrong state %d\n", state);
    return SD_ERR_TIMEOUT;
  }
  state = STATE_INACTIVE;
  return SD_ERR_NONE;
}

/*
 * -----------------------------------------------------------------------------------------------
 * CMD16 MMC_SET_BLOCKLEN
 * arg: Blocklen in bytes for all following block read write commands
 *  Response R1
 * STATE_TRANSFER -> STATE_TRANSFER
 * -----------------------------------------------------------------------------------------------
 */
int SdCard::sd_set_blocklen(uint32_t cmd, uint32_t arg, SdResponse *resp) {
  uint32_t _card_status = GET_STATUS();
  if(!spimode && (state != STATE_TRANSFER)) {
    DPRINTF(SdCard, "MMC SET BLOCKLEN: in wrong state %d\n", state);
    return SD_ERR_TIMEOUT;
  }
  if(ocr & OCR_CCS) {
    if(arg != 512) {
      DPRINTF(SdCard, "Set blocklen %d has no effect on High Capacity card\n",
              arg);
    }
  } else {
    blocklen = arg;
  }
  card_status  &= ~(0xfd3fc020);
  resp->len = 6;
  resp->data[0] = cmd & 0x3f;
  resp->data[1] = (_card_status >> 24) & 0xff;
  resp->data[2] = (_card_status >> 16) & 0xff;
  resp->data[3] = (_card_status >> 8)  & 0xff;
  resp->data[4] = _card_status & 0xff;
  resp->data[5] = SD_RespCRCByte(resp);
  return SD_ERR_NONE;
}

int SdCard::spi_set_blocklen(uint32_t cmd, uint32_t arg, SdResponse *resp) {
  int result = sd_set_blocklen(cmd, arg, resp);
  if(state == STATE_IDLE) {
    resp->data[0] = SPIR1_IDLE;
  } else {
    resp->data[0] = 0;
  }
  resp->len = 1;
  return result;
}

/*
 * ------------------------------------------------------------------------
 * CMD17 Read single block
 *  Argument: address
 *  Response Format R1
 * STATE_TRANSFER -> STATE_DATA
 * ------------------------------------------------------------------------
 */
int SdCard::sd_read_single_block(uint32_t _cmd, uint32_t arg, SdResponse *resp) {
  uint32_t _card_status = GET_STATUS();
  if(state != STATE_TRANSFER) {
    DPRINTF(SdCard, "READ single block: not in TRANSFER state but %d\n", state);
    return SD_ERR_TIMEOUT;
  }
  /* Store the attributes for the recognized data operation */
  cmd = SD_READ_SINGLE_BLOCK;
  if(ocr & OCR_CCS) {
    address = ((uint64_t) arg) << 9;
  } else {
    /* any address is accepted, Table 4.2 Simpl-2.0 */
    address = arg;
  }
  transfer_count = 0;

  state = STATE_DATA;
  card_status  &= ~(0xfd3fc020);
  resp->len = 6;
  resp->data[0] = _cmd & 0x3f;
  resp->data[1] = (_card_status >> 24) & 0xff;
  resp->data[2] = (_card_status >> 16) & 0xff;
  resp->data[3] = (_card_status >> 8)  & 0xff;
  resp->data[4] = _card_status & 0xff;
  resp->data[5] = SD_RespCRCByte(resp);
  DPRINTF(SdCard, "Read single block start transmission \n");
  StartTransmission();
  return SD_ERR_NONE;
}
/*
 * ------------------------------------------------------------------------
 * CMD17 SPI mode: Read single block
 *  Argument: address
 *  Response Format R1
 * State transition unclear: How can I go to TRANSFER state in SPI mode ?
 * Real card always returns resp[0] == 0 for successful read.
 * Tested with kingston + atp card
 * ------------------------------------------------------------------------
 */
int SdCard::spi_read_single_block(uint32_t _cmd, uint32_t arg, SdResponse *resp) {
  //uint32_t card_status = GET_STATUS(card);
  /* Store the attributes for the recognized data operation */
  DPRINTF(SdCard, "SPI version of read single block\n");
  cmd = SD_READ_SINGLE_BLOCK;
  if(ocr & OCR_CCS) {
    address = ((uint64_t) arg) << 9;
  } else {
    /* any address is accepted, Table 4.2 Simpl-2.0 */
    address = arg;
  }
  transfer_count = 0;
  state = STATE_DATA;
  card_status  &= ~(0xfd3fc020);
  resp->len = 1;
  if(state == STATE_IDLE) {
    resp->data[0] = SPIR1_IDLE;
  } else {
    resp->data[0] = 0;
  }
  DPRINTF(SdCard, "StartTransmission\n");
  StartTransmission();
  return SD_ERR_NONE;
}

/*
 * --------------------------------------------------------------------------
 * CMD18
 *  SdCard read multiple block
 *  Argument: address
 *  Response format R1
 * --------------------------------------------------------------------------
 */
int SdCard::sd_read_multiple_block(uint32_t _cmd, uint32_t arg, SdResponse *resp) {
  uint32_t _card_status = GET_STATUS();
  if(state != STATE_TRANSFER) {
    DPRINTF(SdCard, "READ multiple block: in wrong state %d\n", state);
    return SD_ERR_TIMEOUT;
  }

  /* Store the attributes for the recognized data operation */
  cmd = SD_READ_MULTIPLE_BLOCK;
  if(ocr & OCR_CCS) {
    address = ((uint64_t) arg) << 9;
  } else {
    address = arg & ~(blocklen - 1);
  }
  transfer_count=0;
  if((set_block_count_time + 1) != cmdcount) {
    block_count = 0; /* means infinite */
  }
  state = STATE_DATA;
  card_status  &= ~(0xfd3fc020);
  resp->len = 6;
  resp->data[0] = _cmd & 0x3f;
  resp->data[1] = (_card_status >> 24) & 0xff;
  resp->data[2] = (_card_status >> 16) & 0xff;
  resp->data[3] = (_card_status >> 8)  & 0xff;
  resp->data[4] = _card_status & 0xff;
  resp->data[5] = SD_RespCRCByte(resp);
  StartTransmission();
  return SD_ERR_NONE;
}

int SdCard::spi_read_multiple_block(uint32_t _cmd, uint32_t arg, SdResponse *resp) {
  /* Store the attributes for the recognized data operation */
  cmd = SD_READ_MULTIPLE_BLOCK;
  if(ocr & OCR_CCS) {
    address = ((uint64_t) arg) << 9;
  } else {
    address = arg & ~(blocklen - 1);
  }
  transfer_count = 0;
  if((set_block_count_time + 1) != cmdcount) {
    block_count = 0; /* means infinite */
  }
  state = STATE_DATA;
  card_status  &= ~(0xfd3fc020);
  resp->len = 1;
  if(state == STATE_IDLE) {
    resp->data[0] = SPIR1_IDLE;
  } else {
    resp->data[0] = 0;
  }
  StartTransmission();
  return SD_ERR_NONE;
}

/*
 * --------------------------------------------------------------------
 * CMD20 MMC_WRITE_DAT_UNTIL_STOP Maybe only MMC ?
 * arg: address
 * response R1
 * STATE: ???? not mentioned in SanDisk docu
 * --------------------------------------------------------------------
 */
int SdCard::sd_write_dat_until_stop(uint32_t _cmd, uint32_t arg, SdResponse *resp) {
  uint32_t _card_status = GET_STATUS();
  if(state != STATE_TRANSFER) {
    DPRINTF(SdCard, "write dat until stop in wrong state %d\n", state);
    return SD_ERR_TIMEOUT;
  }
  /* Store the attributes for the recognized data operation */
  cmd = SD_WRITE_DAT_UNTIL_STOP;
  address = arg;
  transfer_count=0;
  well_written_blocks = 0;

  state = STATE_DATA;
  card_status  &= ~(0xfd3fc020);
  resp->len = 6;
  resp->data[0] = _cmd & 0x3f;
  resp->data[1] = (_card_status >> 24) & 0xff;
  resp->data[2] = (_card_status >> 16) & 0xff;
  resp->data[3] = (_card_status >> 8)  & 0xff;
  resp->data[4] = _card_status & 0xff;
  resp->data[5] = SD_RespCRCByte(resp);
  return SD_ERR_NONE;
}

/*
 * ---------------------------------------------------------------------------------
 * This command is only valid in MMC >= 3.1. See Section B.5
 * in MMC-System-Spec-v3.31.pdf
 * Kingston1G and Kingston512M don't have it for example
 *
 * CMD23 MMC_SET_BLOCK_COUNT
 * arg: bits 0-15 block count
 *  16-31: 0
 * response: R1
 * STATE: suspect tranfer because before read/write mult
 * Set block count for immediately following CMD 18 (read mult.) (write mult ????)
 * ---------------------------------------------------------------------------------
 */
int SdCard::sd_set_block_count(uint32_t cmd,uint32_t arg, SdResponse *resp) {
  uint32_t _card_status = GET_STATUS();
  if(state != STATE_TRANSFER) {
    DPRINTF(SdCard, "set_block_count in wrong state %d\n", state);
    return SD_ERR_TIMEOUT;
  }
  if(spec_vers < CSD_SPEC_VER_3) {
    return SD_ERR_TIMEOUT;
  }
  warn("set block count %d is ignored\n", arg);
  card_status  &= ~(0xfd3fc020);
  block_count = arg & 0xffff; /* upper bits of arg should be 0 */
  set_block_count_time = cmdcount;
  resp->len = 6;
  resp->data[0] = cmd & 0x3f;
  resp->data[1] = (_card_status >> 24) & 0xff;
  resp->data[2] = (_card_status >> 16) & 0xff;
  resp->data[3] = (_card_status >> 8)  & 0xff;
  resp->data[4] = _card_status & 0xff;
  resp->data[5] = SD_RespCRCByte(resp);
  return SD_ERR_NONE;
}

/*
 * ------------------------------------------------------------------------
 * CMD24 MMC_WRITE_SINGLE_BLOCK
 *  arg: address
 *  resp: R1
 *  STATE_TRANSFER -> STATE_RCV
 * ------------------------------------------------------------------------
 */
int SdCard::sd_write_single_block(uint32_t _cmd, uint32_t arg, SdResponse *resp) {
  uint32_t _card_status = GET_STATUS();
  if(!spimode && (state != STATE_TRANSFER)) {
    fprintf(stderr,"MMC WRITE single block: in wrong state %d\n", state);
    return SD_ERR_TIMEOUT;
  }
  /* Store the attributes for the recognized data operation */
  cmd = SD_WRITE_SINGLE_BLOCK;
  if(ocr & OCR_CCS) {
    address = ((uint64_t) arg) << 9;
  } else {
    address = arg & ~(blocklen - 1);
  }
  transfer_count = 0;
  well_written_blocks = 0;

  state = STATE_RCV;
  card_status  &= ~(0xfd3fc020);
  resp->len = 6;
  resp->data[0] = _cmd & 0x3f;
  resp->data[1] = (_card_status >> 24) & 0xff;
  resp->data[2] = (_card_status >> 16) & 0xff;
  resp->data[3] = (_card_status >> 8)  & 0xff;
  resp->data[4] = _card_status & 0xff;
  resp->data[5] = SD_RespCRCByte(resp);
  return SD_ERR_NONE;
}

int SdCard::spi_write_single_block(uint32_t cmd, uint32_t arg, SdResponse *resp) {
  int result = sd_write_single_block(cmd, arg, resp);
  resp->len = 1;
  if(state == STATE_IDLE) {
    resp->data[0] = SPIR1_IDLE;
  } else {
    resp->data[0] = 0;
  }
  return result;
}

/*
 * ------------------------------------------------------------------------
 * CMD25 WRITE_MULTIPLE_BLOCK
 *  arg: address
 *  resp: R1
 *  STATE_TRANSFER -> STATE_RCV
 * ------------------------------------------------------------------------
 */
int SdCard::sd_write_multiple_block(uint32_t _cmd, uint32_t arg, SdResponse *resp) {
  uint32_t _card_status = GET_STATUS();
  if(!spimode && (state != STATE_TRANSFER)) {
    fprintf(stderr,"MMC write multiple block: in wrong state %d\n", state);
    return SD_ERR_TIMEOUT;
  }
  /* Store the attributes for the recognized data operation */
  cmd = SD_WRITE_MULTIPLE_BLOCK;
  if(ocr & OCR_CCS) {
    address = ((uint64_t) arg) << 9;
  } else {
    address = arg & ~(blocklen - 1);
  }
  transfer_count = 0;
  well_written_blocks = 0;
  if((set_block_count_time + 1) != cmdcount) {
    block_count = 0; /* means infinite */
  }
  state = STATE_RCV;
  card_status  &= ~(0xfd3fc020);
  resp->len = 6;
  resp->data[0] = _cmd & 0x3f;
  resp->data[1] = (_card_status >> 24) & 0xff;
  resp->data[2] = (_card_status >> 16) & 0xff;
  resp->data[3] = (_card_status >> 8)  & 0xff;
  resp->data[4] = _card_status & 0xff;
  resp->data[5] = SD_RespCRCByte(resp);
  return SD_ERR_NONE;
}

int SdCard::spi_write_multiple_block(uint32_t cmd, uint32_t arg, SdResponse *resp) {
  int result = sd_write_multiple_block(cmd, arg, resp);
  resp->len = 1;
  if(state == STATE_IDLE) {
    resp->data[0] = SPIR1_IDLE;
  } else {
    resp->data[0] = 0;
  }
  return result;
}

/*
 * ----------------------------------------------------------------------------
 * CMD26 PROGRAM_CID
 * The argument is don't care.
 * Type ADTC (Addressed Data Transfer Command)
 * Response R1
 * ----------------------------------------------------------------------------
 */
int SdCard::sd_program_cid(uint32_t _cmd, uint32_t arg, SdResponse *resp) {
  uint32_t _card_status = GET_STATUS();
  if(!spimode && (state != STATE_TRANSFER)) {
    DPRINTF(SdCard, "WRITE single block: in wrong state %d\n", state);
    return SD_ERR_TIMEOUT;
  }
  /* Store the attributes for the recognized data operation */
  cmd = SD_PROGRAM_CID;
  transfer_count=0;

  state = STATE_RCV;
  card_status  &= ~(0xfd3fc020);
  resp->len = 6;
  resp->data[0] = _cmd & 0x3f;
  resp->data[1] = (_card_status >> 24) & 0xff;
  resp->data[2] = (_card_status >> 16) & 0xff;
  resp->data[3] = (_card_status >> 8)  & 0xff;
  resp->data[4] = _card_status & 0xff;
  resp->data[5] = SD_RespCRCByte(resp);
  DPRINTF(SdCard, "Reserved command PROGRAM_CID\n");
  return SD_ERR_NONE;
}

/*
 * ----------------------------------------------------------------------------
 * CMD26 PROGRAM_CSD
 * reserved for manufacturer
 * ----------------------------------------------------------------------------
 */
int SdCard::sd_program_csd(uint32_t _cmd, uint32_t arg, SdResponse *resp) {
  uint32_t _card_status = GET_STATUS();
  if(!spimode && (state != STATE_TRANSFER)) {
    DPRINTF(SdCard, "WRITE single block: in wrong state %d\n",state);
    return SD_ERR_TIMEOUT;
  }
  /* Store the attributes for the recognized data operation */
  cmd = SD_PROGRAM_CSD;
  transfer_count=0;

  state = STATE_RCV;
  card_status  &= ~(0xfd3fc020);
  resp->len = 6;
  resp->data[0] = _cmd & 0x3f;
  resp->data[1] = (_card_status >> 24) & 0xff;
  resp->data[2] = (_card_status >> 16) & 0xff;
  resp->data[3] = (_card_status >> 8)  & 0xff;
  resp->data[4] = _card_status & 0xff;
  resp->data[5] = SD_RespCRCByte(resp);
  DPRINTF(SdCard, "Reserved command PROGRAMM_CSD\n");
  return SD_ERR_TIMEOUT;
}

/*
 * -------------------------------------------------------------------------------
 * CMD28 SET_WRITE_PROT
 * arg: address of sector group
 * Response R1b
 * Set writeprotect for sector group. Group and sector size is stored in CSD
 * -------------------------------------------------------------------------------
 */
int SdCard::sd_set_write_prot(uint32_t cmd, uint32_t arg, SdResponse *resp) {
  resp->len = 0;
  DPRINTF(SdCard, "SET WRITE PROT not implemented\n");
  return SD_ERR_TIMEOUT;
}

/*
 * ---------------------------------------------------------------------------
 * CMD29 CLR_WRITE_PROT
 * arg: address of sector group
 * Response R1b
 * ---------------------------------------------------------------------------
 */
int SdCard::sd_clr_write_prot(uint32_t cmd, uint32_t arg, SdResponse *resp) {
  resp->len = 0;
  DPRINTF(SdCard, "CLR_WRITE_PROT\n");
  return SD_ERR_TIMEOUT;
}

/*
 * ---------------------------------------------------------------------------
 * CMD30 SEND_WRITE_PROT
 * Response Format R1
 * Send the write protection of 32 groups on the data lines
 * ---------------------------------------------------------------------------
 */
int SdCard::sd_send_write_prot(uint32_t cmd, uint32_t arg, SdResponse *resp) {
  resp->len = 0;
  DPRINTF(SdCard, "cmd 0x%02x not implemented\n",cmd);
  return SD_ERR_TIMEOUT;
}

/*
 * -----------------------------------------------------------------------
 * CMD32: MMC_ERASE_WR_BLK_START
 * Response Format R1
 * STATE: Tran -> Tran
 * -----------------------------------------------------------------------
 */
int SdCard::sd_erase_wr_blk_start(uint32_t cmd,uint32_t arg, SdResponse *resp) {
  uint32_t _card_status = GET_STATUS();
  if(!spimode && (state != STATE_TRANSFER)) {
    DPRINTF(SdCard, "Erase WR blk start not in state transfer\n");
    return SD_ERR_TIMEOUT;
  }
  card_status  &= ~(0xfd3fc020);
  resp->len = 6;
  resp->data[0] = cmd & 0x3f;
  resp->data[1] = (_card_status >> 24) & 0xff;
  resp->data[2] = (_card_status >> 16) & 0xff;
  resp->data[3] = (_card_status >> 8)  & 0xff;
  resp->data[4] = _card_status & 0xff;
  resp->data[5] = SD_RespCRCByte(resp);
  if(ocr & OCR_CCS) {
    erase_start = ((uint64_t) arg) << 9;
  } else {
    erase_start = arg;
  }
  DPRINTF(SdCard, "erase start %lld\n", (long long)erase_start);
  return SD_ERR_NONE;
}

int SdCard::spi_erase_wr_blk_start(uint32_t cmd, uint32_t arg, SdResponse *resp) {
  int result = sd_erase_wr_blk_start(cmd, arg, resp);
  resp->len = 1;
  if(state == STATE_IDLE) {
    resp->data[0] = SPIR1_IDLE;
  } else {
    resp->data[0] = 0;
  }
  return result;
}

/*
 * -------------------------------------------------------------------------------
 * CMD33: MMC_ERASE_WR_BLK_END
 * Response format R1
 * State Transfer -> Transfer
 * -------------------------------------------------------------------------------
 */
int SdCard::sd_erase_wr_blk_end(uint32_t cmd, uint32_t arg, SdResponse *resp) {
  uint32_t _card_status = GET_STATUS();
  if(!spimode && (state != STATE_TRANSFER)) {
    DPRINTF(SdCard, "Erase WR blk end not in state transfer\n");
    return SD_ERR_TIMEOUT;
  }
  card_status  &= ~(0xfd3fc020);
  resp->len = 6;
  resp->data[0] = cmd & 0x3f;
  resp->data[1] = (_card_status >> 24) & 0xff;
  resp->data[2] = (_card_status >> 16) & 0xff;
  resp->data[3] = (_card_status >> 8)  & 0xff;
  resp->data[4] = _card_status & 0xff;
  resp->data[5] = SD_RespCRCByte(resp);
  if(ocr & OCR_CCS) {
    erase_end = ((uint64_t) arg) << 9;
  } else {
    erase_end = arg;
  }
  DPRINTF(SdCard, "erase end %lld\n",(long long)erase_end);
  return SD_ERR_NONE;
}

/**
 **************************************************************************************************
 * \fn static int spi_erase_wr_blk_end(MMCard *card,uint32_t cmd,uint32_t arg,MMCResponse *resp)
 **************************************************************************************************
 */
int SdCard::spi_erase_wr_blk_end(uint32_t cmd, uint32_t arg, SdResponse *resp) {
  int result = sd_erase_wr_blk_end(cmd, arg, resp);
  resp->len = 1;
  if(state == STATE_IDLE) {
    resp->data[0] = SPIR1_IDLE;
  } else {
    resp->data[0] = 0;
  }
  return result;
}

/*
 * ------------------------------------------------------------------------
 * CMD35 erase group start MMC only !
 * arg: address of first write block to be erased (erase group ???)
 * Response R1
 * State Transfer -> Transfer
 * ------------------------------------------------------------------------
 */
int SdCard::sd_erase_group_start(uint32_t cmd, uint32_t arg, SdResponse *resp) {
  uint32_t _card_status = GET_STATUS();
  if(state != STATE_TRANSFER) {
    DPRINTF(SdCard, "Erase not in state transfer\n");
    return SD_ERR_TIMEOUT;
  }
  card_status  &= ~(0xfd3fc020);
  resp->len = 6;
  resp->data[0] = cmd & 0x3f;
  resp->data[1] = (_card_status >> 24) & 0xff;
  resp->data[2] = (_card_status >> 16) & 0xff;
  resp->data[3] = (_card_status >> 8)  & 0xff;
  resp->data[4] = _card_status & 0xff;
  resp->data[5] = SD_RespCRCByte(resp);
  erase_start = arg;
  return SD_ERR_NONE;
}

/*
 * ----------------------------------------------------------------------
 * CMD36 ERASE_GROUP_END (MMC Only ?)
 * arg: address of last write block to be erased
 * State: Transfer -> Transfer
 * Response R1
 * ----------------------------------------------------------------------
 */
int SdCard::sd_erase_group_end(uint32_t cmd, uint32_t arg, SdResponse *resp) {
  uint32_t _card_status = GET_STATUS();
  card_status  &= ~(0xfd3fc020);
  if(state != STATE_TRANSFER) {
    return SD_ERR_TIMEOUT;
  }
  resp->len = 6;
  resp->data[0] = cmd & 0x3f;
  resp->data[1] = (_card_status >> 24) & 0xff;
  resp->data[2] = (_card_status >> 16) & 0xff;
  resp->data[3] = (_card_status >> 8)  & 0xff;
  resp->data[4] = _card_status & 0xff;
  resp->data[5] = SD_RespCRCByte(resp);
  erase_end = arg;
  return SD_ERR_NONE;
}

/*
 * ---------------------------------------------------------------------------
 * CMD38 ERASE
 * This command should be reimplemented to work in the Background !
 * erase previously selected blocks
 * arg: none
 * State Transfer -> PRG -> (some time) Transfer
 * Response R1b (R1 with busy on data line)
 * ---------------------------------------------------------------------------
 */
int SdCard::sd_erase(uint32_t cmd, uint32_t arg, SdResponse *resp) {
  resp->len = 0;
  uint32_t _card_status = GET_STATUS();
  uint64_t start = erase_start;
  uint64_t end = erase_end | (blocklen -1);
  uint8_t buf[256];
  if(start > capacity) {
    start = capacity;
  }
  if(end > capacity) {
    warn("erasing past end of card\n");
    end = capacity;
  }
  if((end - start) > 256 * 1024 * 1024) {
    warn("Erase (CMD%d) should be implemented in Background\n",cmd);
  }
  memset(buf, 0xff, sizeof(buf));
  while(start < end) {
    uint64_t count = end - start;
    if(count > sizeof(buf)) {
      count = sizeof(buf);
    }
    if(disk_image->write(buf, start, count) < count) {
      DPRINTF(SdCard, "Writing to diskimage failed\n");
      break;
    }
    start += count;
  }
  card_status  &= ~(0xfd3fc020);
  resp->len = 6;
  resp->data[0] = cmd & 0x3f;
  resp->data[1] = (_card_status >> 24) & 0xff;
  resp->data[2] = (_card_status >> 16) & 0xff;
  resp->data[3] = (_card_status >> 8)  & 0xff;
  resp->data[4] = _card_status & 0xff;
  resp->data[5] = SD_RespCRCByte(resp);
  DPRINTF(SdCard, "erase done\n");
  return SD_ERR_NONE;
}

int SdCard::spi_erase(uint32_t cmd, uint32_t arg, SdResponse *resp) {
  int result = sd_erase(cmd,arg,resp);
  resp->len = 1;
  if(state == STATE_IDLE) {
    resp->data[0] = SPIR1_IDLE;
  } else {
    resp->data[0] = 0;
  }
  return result;
}

int SdCard::sd_fast_io(uint32_t cmd, uint32_t arg, SdResponse *resp) {
  resp->len = 0;
  DPRINTF(SdCard, "cmd 0x%02x not implemented\n", cmd);
  return SD_ERR_TIMEOUT;
}

int SdCard::sd_go_irq_state(uint32_t cmd, uint32_t arg, SdResponse *resp) {
  resp->len = 0;
  DPRINTF(SdCard, "cmd 0x%02x not implemented\n",cmd);
  return SD_ERR_TIMEOUT;
}

/*
 * ----------------------------------------------------------------------
 * CMD42: MMC_LOCK_UNLOCK
 * Response format R1
 * Arg is 0.
 * State: Trans -> RCV (Has a data stage)
 * ----------------------------------------------------------------------
 */
int SdCard::sd_lock_unlock(uint32_t cmd, uint32_t arg, SdResponse *resp) {
  DPRINTF(SdCard, "lock/unlock not implemented\n");
  card_status  &= ~(0xfd3fc020);
  return SD_ERR_TIMEOUT;
}

/*
 * ----------------------------------------------------------------------
 * CMD52: SPIO_RW_DIRECT
 * Response format R5
 * Arg is R/W FN RAW Stuff RegAddr Stuff Data.
 * ----------------------------------------------------------------------
 */
int SdCard::sdio_rw_direct(uint32_t cmd, uint32_t arg, SdResponse *resp) {
  //uint8_t regaddr = (arg >> 9) & 0xFF;
  //bool direct = (arg & 0x80000000) != 0;
  uint8_t data = arg & 0xFF;
  resp->len = 0;
  resp->len = 6;
  resp->data[0] = cmd & 0x3f;
  resp->data[1] = 0;
  resp->data[2] = 0;
  resp->data[3] = 0;
  resp->data[4] = data;
  resp->data[5] = SD_RespCRCByte(resp);
  return SD_ERR_NONE;
}

/*
 * ----------------------------------------------------------
 * CMD55 app_cmd
 * Response format R1
 * ----------------------------------------------------------
 */
int SdCard::sd_app_cmd(uint32_t cmd, uint32_t arg, SdResponse *resp) {
  uint32_t _card_status = GET_STATUS() | STATUS_APP_CMD;
  uint16_t _rca = (arg >> 16) & 0xffff;
  /* behaviour for rca == 0 ???? */
  if(/*(rca != 0) &&*/(rca != _rca)) {
    DPRINTF(SdCard, "APP CMD: card not selected, rca %d instead of %d\n",
            _rca, rca);
    return SD_ERR_TIMEOUT;
  }
  card_status  &= ~(0xfd3fc020);
  is_app_cmd = 1;
  resp->len = 6;
  resp->data[0] = cmd & 0x3f;
  resp->data[1] = (_card_status >> 24) & 0xff;
  resp->data[2] = (_card_status >> 16) & 0xff;
  resp->data[3] = (_card_status >> 8)  & 0xff;
  resp->data[4] = _card_status & 0xff;
  resp->data[5] = SD_RespCRCByte(resp);
  return SD_ERR_NONE;
}

/**
 ****************************************************************************************
 * \fn static int spi_app_cmd(MMCard *card,uint32_t cmd,uint32_t arg,MMCResponse *resp)
 ****************************************************************************************
 */
int SdCard::spi_app_cmd(uint32_t cmd, uint32_t arg, SdResponse *resp) {
  int result = sd_app_cmd(cmd, arg, resp);
  resp->len = 1;
  if(state == STATE_IDLE) {
    resp->data[0] = SPIR1_IDLE;
  } else {
    resp->data[0] = 0;
  }
  return result;
}

/*
 * ------------------------------------------------------------------------
 * CMD56 GEN_CMD
 * ------------------------------------------------------------------------
 */
int SdCard::sd_gen_cmd(uint32_t cmd, uint32_t arg, SdResponse *resp) {
  resp->len = 0;
  fprintf(stderr,"GEN_CMD 0x%02x not implemented\n",cmd);
  return SD_ERR_TIMEOUT;
}

/*
 * ----------------------------------------------------------
 * CMD58 READ_OCR
 * Response format R3
 * State: ?????
 * Argument 0-31 stuff bits
 * ----------------------------------------------------------
 */
int SdCard::sd_read_ocr(uint32_t cmd, uint32_t arg, SdResponse *resp) {
  resp->data[0] = 0x3f;
  resp->data[1] = ocr >> 24;
  resp->data[2] = ocr >> 16;
  resp->data[3] = ocr >> 8;
  resp->data[4] = ocr;
  resp->data[5] = 0xff;
  resp->len = 6;
  return SD_ERR_NONE;
}

/*
 *******************************************
 * CMD58 SPI version
 * Response format SPI-R3
 *******************************************
 */
int SdCard::spi_read_ocr(uint32_t cmd, uint32_t arg, SdResponse *resp) {
  if(state == STATE_IDLE) {
    resp->data[0] = SPIR1_IDLE;
  } else {
    resp->data[0] = 0;
  }
  resp->data[1] = ocr >> 24;
  resp->data[2] = ocr >> 16;
  resp->data[3] = ocr >> 8;
  resp->data[4] = ocr;
  resp->len = 5;
  return SD_ERR_NONE;
}

/*
 *******************************************
 * CMD59 CRC on/off
 * Response format R1
 *******************************************
 */
int SdCard::sd_crc_on_off(uint32_t cmd, uint32_t arg, SdResponse *resp) {
  uint32_t _card_status = GET_STATUS();
  resp->len = 6;
  resp->data[0] = cmd & 0x3f;
  resp->data[1] = (_card_status >> 24) & 0xff;
  resp->data[2] = (_card_status >> 16) & 0xff;
  resp->data[3] = (_card_status >> 8)  & 0xff;
  resp->data[4] = _card_status & 0xff;
  resp->data[5] = SD_RespCRCByte(resp);
  crc_on = arg & 1;
  return SD_ERR_NONE;
}


int SdCard::spi_crc_on_off(uint32_t cmd, uint32_t arg, SdResponse *resp) {
  resp->len = 1;
  crc_on = arg & 1;
  if(state == STATE_IDLE) {
    resp->data[0] = SPIR1_IDLE;
  } else {
    resp->data[0] = 0;
  }
  return SD_ERR_NONE;
}
/*
 * ----------------------------------------------------------------------------
 * ACMD6 SD_APP_SET_BUS_WIDTH
 * arg: 00 = 1 Bit, 10 = 4Bits
 * ----------------------------------------------------------------------------
 */
int SdCard::sd_app_set_bus_width(uint32_t cmd, uint32_t arg, SdResponse *resp) {
  uint32_t _card_status = GET_STATUS() | STATUS_APP_CMD;
  if(type != CARD_TYPE_SD) {
    DPRINTF(SdCard, "SD_APP_SET_BUS_WIDTH: Not an SD-Card !\n");
    return SD_ERR_TIMEOUT;
  }
  if(state != STATE_TRANSFER) {
    DPRINTF(SdCard, "SD_APP_BUS_WIDTH in state %d\n", state);
    return SD_ERR_TIMEOUT;
  }
  card_status  &= ~(0xfd3fc020);
  COPYBITS(arg & 3, ssr, 510, 511);
  resp->len = 6;
  resp->data[0] = cmd & 0x3f;
  resp->data[1] = (_card_status >> 24) & 0xff;
  resp->data[2] = (_card_status >> 16) & 0xff;
  resp->data[3] = (_card_status >> 8)  & 0xff;
  resp->data[4] = _card_status & 0xff;
  resp->data[5] = SD_RespCRCByte(resp);
  return SD_ERR_NONE;
}

/*
 * ------------------------------------------------------------------------
 * ACMD18: SECURE_READ_MULTI_BLOCK
 * Argument: ignored
 * Response: R1
 * State: Transfer -> Data ?
 * ------------------------------------------------------------------------
 */

int SdCard::sd_app_secure_read_multi_block(uint32_t cmd, uint32_t arg, SdResponse *resp) {
  DPRINTF(SdCard, "ACMD18: SECURE_READ_MULTI_BLOCK is not implemented\n");
  return SD_ERR_TIMEOUT;
}

/*
 * -------------------------------------------------------------------------
 * ACMD22
 * SD_APP_SEND_NUM_WR_BLKS
 * Respond with the number of well written blocks
 * Response format R1 with 4 byte data phase
 * state: Not documented but suspect transfer -> data
 * -------------------------------------------------------------------------
 */
int SdCard::sd_app_send_num_wr_blks(uint32_t _cmd, uint32_t arg, SdResponse *resp) {
  uint32_t _card_status = GET_STATUS();
  if(type != CARD_TYPE_SD) {
    DPRINTF(SdCard, "SD_APP_SEND_SCR: Not an SD-Card !\n");
    return SD_ERR_TIMEOUT;
  }
#if 1
  if(state != STATE_TRANSFER) {
    DPRINTF(SdCard, "send_num_wr_blks in wrong state\n");
    return SD_ERR_TIMEOUT;
  }
#endif
  transfer_count = 0;
  state = STATE_DATA;
  card_status  &= ~(0xfd3fc020);

  /* Store the attributes for the recognized data operation */
  cmd = _cmd | CMD_FLAG_IS_APPCMD;

  resp->len = 6;
  resp->data[0] = _cmd & 0x3f;
  resp->data[1] = (_card_status >> 24) & 0xff;
  resp->data[2] = (_card_status >> 16) & 0xff;
  resp->data[3] = (_card_status >> 8)  & 0xff;
  resp->data[4] = _card_status & 0xff;
  resp->data[5] = SD_RespCRCByte(resp);
  /* resp->len = 0; */
  StartTransmission();
  return SD_ERR_NONE;
}

int SdCard::spi_app_send_num_wr_blks(uint32_t cmd, uint32_t arg, SdResponse *resp) {
  int result = sd_app_send_num_wr_blks(cmd, arg, resp);
  resp->len = 1;
  if(state == STATE_IDLE) {
    resp->data[0] = SPIR1_IDLE;
  } else {
    resp->data[0] = 0;
  }
  return result;
}

/*
 * -------------------------------------------------------------------------
 * ACMD23
 * SD_APP_SET_WR_BLK_ERASE_COUNT
 * Currently this command is ignored
 * state ????
 * -------------------------------------------------------------------------
 */
int SdCard::sd_app_set_wr_blk_erase_count(uint32_t cmd, uint32_t arg, SdResponse *resp) {
  uint32_t _card_status = GET_STATUS();
  if(type != CARD_TYPE_SD) {
    DPRINTF(SdCard, "SD_APP_SEND_SCR: Not an SD-Card !\n");
    return SD_ERR_TIMEOUT;
  }
#if 0
  if(card->state != STATE_TRANSFER) {
    fprintf(stderr,"send_num_wr_blks in wrong state\n");
    return MMC_ERR_TIMEOUT;
  }
#endif
  card_status  &= ~(0xfd3fc020);

  resp->len = 6;
  resp->data[0] = cmd & 0x3f;
  resp->data[1] = (_card_status >> 24) & 0xff;
  resp->data[2] = (_card_status >> 16) & 0xff;
  resp->data[3] = (_card_status >> 8)  & 0xff;
  resp->data[4] = _card_status & 0xff;
  resp->data[5] = SD_RespCRCByte(resp);
  resp->len = 0;
  return SD_ERR_NONE;

}

/*
 * ----------------------------------------------------------------------
 * ACMD25: SECURE_WRITE_MULT_BLOCK
 * Argument: ignored
 * Response Format: R1
 * State: Transfer -> RCV ?
 * ----------------------------------------------------------------------
 */
int SdCard::sd_app_secure_write_mult_block(uint32_t cmd, uint32_t arg, SdResponse *resp) {
  DPRINTF(SdCard, "ACMD25: SECURE_WRITE_MULT_BLOCK not implemented\n");
  return SD_ERR_TIMEOUT;
}

/*
 * ----------------------------------------------------------------------
 * ACMD26: SECURE_WRITE_MKB
 * Argument: ignored
 * Response Format: R1
 * State: Transfer -> RCV ?
 * ----------------------------------------------------------------------
 */
int SdCard::sd_app_secure_write_mkb(uint32_t cmd, uint32_t arg, SdResponse *resp) {
  fprintf(stderr,"ACMD26: SECURE_WRITE_MKB not implemented\n");
  return SD_ERR_TIMEOUT;
}

/*
 * ---------------------------------------------------------------------
 * ACMD38: SECURE_ERASE
 * ---------------------------------------------------------------------
 */
int SdCard::sd_app_secure_erase(uint32_t cmd, uint32_t arg, SdResponse *resp) {
  DPRINTF(SdCard, "ACMD38: SECURE_ERASE not implemented\n");
  return SD_ERR_TIMEOUT;
}

/*
 * ----------------------------------------------------------------------
 * SD app op condition ACMD41 (0x29)
 * Response format R3
 * STATE_IDLE -> STATE_READY when voltage good and card not busy else
 * go to inactive
 * ----------------------------------------------------------------------
 */
int SdCard::sd_app_op_cond(uint32_t cmd, uint32_t arg, SdResponse *resp) {
  int ccs = !!(ocr & OCR_CCS); /* Card Capacity status */
  int hcs = !!(arg & (1<<30));     /* Host capacity status */
  resp->len = 0;
  if(!(arg & ocr & OCR_VOLT_MSK)) {
    warn("Warning: OCR Voltage not matching, arg %08x ocr %08x\n", arg, ocr);
  }
  if(type != CARD_TYPE_SD) {
    DPRINTF(SdCard, "SD_AP_OP_COND for non SD-Card\n");
    return SD_ERR_TIMEOUT;
  }
  if(state != STATE_IDLE) {
    resp->len = 0;
    DPRINTF(SdCard, "SD_AP_OP_COND when not in IDLE state (%d)\n", state);
    return SD_ERR_TIMEOUT;
  }
  /* Do not start the reset if card is high capacity but host not 2.0 */
  if(ccs && !host_is_2_0) {
    DPRINTF(SdCard, "Host is not 2.0\n");
    ocr &= ~OCR_NOTBUSY;
  } else if (ccs && !hcs) {
    DPRINTF(SdCard, "Host did not enable High Capacity mode : 0x%08x\n", arg);
    ocr &= ~OCR_NOTBUSY;
  } else if (reset_state == RST_NOT_STARTED) {
    //reset_start_time = CycleCounter_Get();
    ocr &= ~OCR_NOTBUSY;
    reset_state = RST_STARTED;
  } else if (reset_state == RST_STARTED) {
    //int64_t usec = CyclesToMicroseconds(CycleCounter_Get()-card->reset_start_time);
    //if(usec > usec_reset) {
      state = STATE_READY;
      ocr |= OCR_NOTBUSY;
      reset_state = RST_DONE;
    //}
  } else if (reset_state != RST_DONE) {
    panic("Emulator bug: MMC-Card reset_state %d not valid\n", reset_state);
  }
  // init card is missing here, need some time, return busy the first few times
  resp->data[0] = 0x3f;
  resp->data[1] = ocr >> 24;
  resp->data[2] = ocr >> 16;
  resp->data[3] = ocr >> 8;
  resp->data[4] = ocr;
  resp->data[5] = 0xff;
  resp->len = 6;
  DPRINTF(SdCard, "SD app op cond arg 0x%08x, ocr %08x\n", arg, ocr);
  return SD_ERR_NONE;
}

int SdCard::spi_app_op_cond(uint32_t cmd, uint32_t arg, SdResponse *resp) {
  int result = sd_app_op_cond(cmd, arg, resp);
  resp->len = 1;
  if(state == STATE_IDLE) {
    resp->data[0] = SPIR1_IDLE;
  } else {
    resp->data[0] = 0;
  }
  return result;
}

/*
 * ---------------------------------------------------------------------------
 * ACMD42: SD_APP_SET_CLR_CARD_DETECT
 * State: Trans -> Trans
 * Response format R1
 * ---------------------------------------------------------------------------
 */
int SdCard::sd_app_set_clr_card_detect(uint32_t cmd, uint32_t arg, SdResponse *resp) {
  uint32_t _card_status = GET_STATUS();
  if(state != STATE_TRANSFER) {
    DPRINTF(SdCard, "SET_CLR_CARD_DETECT in wrong state\n");
    return SD_ERR_TIMEOUT;
  }
  if(arg & 1) {
    DPRINTF(SdCard, "Connect Pullup resistor\n");
  } else {
    DPRINTF(SdCard, "Disconnect Pullup resistor\n");
  }
  resp->len = 6;
  resp->data[0] = cmd & 0x3f;
  resp->data[1] = (_card_status >> 24) & 0xff;
  resp->data[2] = (_card_status >> 16) & 0xff;
  resp->data[3] = (_card_status >> 8)  & 0xff;
  resp->data[4] = _card_status & 0xff;
  resp->data[5] = 1; /* CRC missing here */
  return SD_ERR_NONE;
}
/*
 * ------------------------------------------------------------------
 * ACMD43: GET_MKB
 * Response Format R1
 * Argument [0:15] UNIT OFFSET
 *      [16:23] MKB ID
 *      [24:31] UNIT COUNT
 * State: transfer -> data ?
 * ------------------------------------------------------------------
 */
int SdCard::sd_app_get_mkb(uint32_t cmd, uint32_t arg, SdResponse *resp) {
  DPRINTF(SdCard, "ACMD43 GET_MKB: not implemented\n");
  return SD_ERR_TIMEOUT;
}

/*
 * -------------------------------------------------------------------
 * ACMD44: GET_MID
 * Response Format R1
 * Argument: ignored
 * State: transfer -> data ?
 * -------------------------------------------------------------------
 */
int SdCard::sd_app_get_mid(uint32_t cmd, uint32_t arg, SdResponse *resp) {
  DPRINTF(SdCard, "ACMD44 GET_MID: not implemented\n");
  return SD_ERR_TIMEOUT;
}

/*
 * ------------------------------------------------------------------
 * ACMD45: SET_CER_RN1
 * Response format R1
 * Argument: ignored
 * State: Transfer -> RCV ?
 * ------------------------------------------------------------------
 */
int SdCard::sd_app_set_cer_rn1(uint32_t cmd, uint32_t arg, SdResponse *resp) {
  DPRINTF(SdCard, "ACMD45 SET_CER_RN1: not implemented\n");
  return SD_ERR_TIMEOUT;
}

/*
 * -------------------------------------------------------------------
 * ACMD46: GET_CER_RN2
 * Response format R1
 * Argument: ignored
 * State: Transfer -> Data ?
 * -------------------------------------------------------------------
 */
int SdCard::sd_app_get_cer_rn2(uint32_t cmd, uint32_t arg, SdResponse *resp) {
  DPRINTF(SdCard, "ACMD46 GET_CER_RN2: not implemented\n");
  return SD_ERR_TIMEOUT;
}

/*
 * ------------------------------------------------------------------
 * ACMD47: SET_CER_RES2
 * Response Format R1
 * Argument: ignored
 * State: Transfer -> RCV ?
 * ------------------------------------------------------------------
 */
int SdCard::sd_app_set_cer_res2(uint32_t cmd, uint32_t arg, SdResponse *resp) {
  DPRINTF(SdCard, "ACMD47: SET_CER_RES2 not implemented\n");
  return SD_ERR_TIMEOUT;
}
/*
 * ------------------------------------------------------------------
 * ACMD48: GET_CER_RES1
 * Response Format R1
 * Argument: ignored
 * State: Transfer -> Data
 * ------------------------------------------------------------------
 */
int SdCard::sd_app_get_cer_res1(uint32_t cmd, uint32_t arg, SdResponse *resp) {
  DPRINTF(SdCard, "ACMD47: GET_CER_RES1 not implemented\n");
  return SD_ERR_TIMEOUT;

}

/*
 * ------------------------------------------------------------------
 * ACMD49: Change Secure Area
 * Response Format R1b
 * Argument: ignored
 * State: ??
 * ------------------------------------------------------------------
 */
int SdCard::sd_app_change_secure_area(uint32_t cmd, uint32_t arg, SdResponse *resp) {
  DPRINTF(SdCard, "ACMD49: Change Secure Area not implemented\n");
  return SD_ERR_TIMEOUT;
}

/*
 * ------------------------------------------------------------------
 * ACMD51: SEND SCR
 * Response format R1, dummy argument, goes to data state
 * State: Transfer -> Data
 * ------------------------------------------------------------------
 */
int SdCard::sd_app_send_scr(uint32_t _cmd, uint32_t arg, SdResponse *resp) {
  uint32_t _card_status = GET_STATUS();
  if(type != CARD_TYPE_SD) {
    DPRINTF(SdCard, "SD_APP_SEND_SCR: Not an SD-Card !\n");
    return SD_ERR_TIMEOUT;
  }
  if(!spimode && (state != STATE_TRANSFER)) {
    DPRINTF(SdCard, "Send scr in wrong state\n");
    return SD_ERR_TIMEOUT;
  }
  transfer_count = 0;
  state = STATE_DATA;
  card_status  &= ~(0xfd3fc020);

  /* Store the attributes for the recognized data operation */
  cmd = _cmd | CMD_FLAG_IS_APPCMD;

  resp->len = 6;
  resp->data[0] = _cmd & 0x3f;
  resp->data[1] = (_card_status >> 24) & 0xff;
  resp->data[2] = (_card_status >> 16) & 0xff;
  resp->data[3] = (_card_status >> 8)  & 0xff;
  resp->data[4] = _card_status & 0xff;
  resp->data[5] = SD_RespCRCByte(resp);
  /* put data to somewhere */
  StartTransmission();
  return SD_ERR_NONE;
}

/**
 *************************************************************************
 * \fn static int spi_app_send_scr(MMCard *card,uint32_t cmd,uint32_t arg,MMCResponse *resp)
 *************************************************************************
 */
int SdCard::spi_app_send_scr(uint32_t cmd,uint32_t arg, SdResponse *resp) {
  int result = sd_app_send_scr(cmd, arg, resp);
  resp->len = 1;
  if(state == STATE_IDLE) {
    resp->data[0] = SPIR1_IDLE;
  } else {
    resp->data[0] = 0;
  }
  return result;
}

#if 0
static void
MMC_VerifyDataCrc(MMCDev *dev,const uint8_t buf,int count)
{
  MMCard *card = container_of(dev,MMCard,mmcdev);
  if(!card) {
    return 0;
  }
  return
}
#endif

/*
 * ------------------------------------------------------------------
 * MMCard_Write
 * The Data Phase of a command
 * retval: number of Bytes, -errcode on error
 * Busy polling implenentation is missing
 * ------------------------------------------------------------------
 */
int SdCard::write(const uint8_t *buf, int count) {
  if(state != STATE_RCV) {
    DPRINTF(SdCard, "Write: Card not in RCV: %d\n", state);
    return 0;
  }
  if(cmd ==  SD_WRITE_SINGLE_BLOCK) {
    uint32_t _address  = address + transfer_count;
    if(transfer_count + count > blocklen) {
      count = blocklen - transfer_count;
      if(count<0) {
        DPRINTF(SdCard, "transfer count < 0 should never happen\n");
        return 0;
      }
    }
    if((_address + count) > capacity) {
      count = capacity - _address;
    }
    if(disk_image->write(buf, _address, count) < count) {
      DPRINTF(SdCard, "Error writing to diskimage\n");
    }
    transfer_count += count;
    if(transfer_count == blocklen) {
      state = STATE_TRANSFER;
      well_written_blocks = 1;
    }
    return count;
  } else if (cmd == SD_WRITE_MULTIPLE_BLOCK) {
    uint32_t _address  = address + transfer_count;
    if((_address + count) > capacity) {
      count = capacity - _address;
    }
    if(disk_image->write(buf, _address, count) < count) {
      DPRINTF(SdCard, "Error writing to diskimage\n");
    }
    transfer_count += count;
    if((_address & ~(blocklen - 1)) != ((_address + count) & ~(blocklen - 1))) {
      if(block_count) {
        block_count--;
        if(!block_count) {
          state = STATE_TRANSFER;
        }
      }
      if(blocklen) {
        well_written_blocks = transfer_count / blocklen;
      }
    }
    return count;
  } else if(cmd == SD_WRITE_DAT_UNTIL_STOP) {
    uint32_t _address  = address + transfer_count;
    if((_address+count) > capacity) {
      count = capacity - _address;
    }
    if(disk_image->write(buf, _address, count) < count) {
      DPRINTF(SdCard, "Error writing to diskimage\n");
    }
    transfer_count += count;
    if (blocklen) {
      well_written_blocks = transfer_count / blocklen;
    }
    return count;
  } else if(cmd == SD_PROGRAM_CSD) {
    int i;
    for(i = 0; (i < count) && (transfer_count < 16); i++) {
      dataAssBuf[transfer_count] = buf[i];
      ++transfer_count;
    }
    if(transfer_count == 16) {
      //card->data_crc = bla;
      ++transfer_count;
      ++i;
    } else if(transfer_count == 17) {
      //card->data_crc |= blub;
      ++transfer_count;
      ++i;
      state = STATE_TRANSFER;
    }
    return i;
  } else if(cmd == SD_PROGRAM_CID) {
    int i;
    transfer_count += count;
    for(i = 0; (i < count) && (transfer_count < 16); i++) {
      dataAssBuf[transfer_count] = buf[i];
      ++transfer_count;
    }
    /* Warning, CRC transmission by master not yet active !
     * So this will not work currently
     */
    if((transfer_count == 16) && (i < count)) {
      //card->data_crc = bla;
      ++transfer_count;
      ++i;
    }
    if((transfer_count == 17) && (i < count)) {
      //card->data_crc |= blub;
      ++transfer_count;
      ++i;
      state = STATE_TRANSFER;
    }
    return i;
  } else {
    DPRINTF(SdCard, "Write cmd %d not known\n", cmd);
    return 0;
  }
}

void SdCard::InitCmds() {
  int ccc = GETBITS(csd, 84, 95);
  if(ccc & CCC_BASIC) {
    cmdProc[SD_GO_IDLE_STATE] = &SdCard::sd_go_idle;     /* CMD0 */
    cmdProc[SD_ALL_SEND_CID] = &SdCard::sd_all_send_cid;   /* CMD2 */
    cmdProc[SD_SEND_RELATIVE_ADDR] = &SdCard::sd_send_relative_addr;
    cmdProc[SD_SET_DSR] = &SdCard::sd_set_dsr;
    cmdProc[SD_SEND_OP_COND] = &SdCard::sd_send_op_cond; /* CMD1 really available ? */
    cmdProc[SDIO_RW_DIRECT] = &SdCard::sdio_rw_direct;
    cmdProc[SPIO_] = &SdCard::spi_send_op_cond; /* CMD1 really available ? */
  }
  if(ccc & CCC_SWITCH) {
    cmdProc[SD_SWITCH] = &SdCard::sd_switch;
  }
  if(ccc & CCC_BASIC) {
    cmdProc[SD_SELECT_CARD] = &SdCard::sd_select_card;
    cmdProc[SD_SEND_INTERFACE_COND] = &SdCard::sd_send_interface_cond;
    cmdProc[SD_SEND_CSD] = &SdCard::sd_send_csd;
    cmdProc[SD_SEND_CID] = &SdCard::sd_send_cid;
    cmdProc[SD_STOP_TRANSMISSION] = &SdCard::sd_stop_transmission;
    cmdProc[SD_SEND_STATUS] = &SdCard::sd_send_status;
    cmdProc[SD_GO_INACTIVE_STATE] = &SdCard::sd_go_inactive_state;
    cmdProc[SD_CRC_ON_OFF] = &SdCard::sd_crc_on_off;
  }
  if(ccc & (CCC_BLOCK_READ | CCC_BLOCK_WRITE | CCC_LOCK_CARD)) {
    cmdProc[SD_SET_BLOCKLEN] = &SdCard::sd_set_blocklen;
  }
  if(ccc & CCC_BLOCK_READ) {
    cmdProc[SD_READ_SINGLE_BLOCK] = &SdCard::sd_read_single_block;
    cmdProc[SD_READ_MULTIPLE_BLOCK] = &SdCard::sd_read_multiple_block;
  }
  if(ccc & CCC_BLOCK_WRITE) {
    cmdProc[SD_WRITE_SINGLE_BLOCK] = &SdCard::sd_write_single_block;
    cmdProc[SD_WRITE_MULTIPLE_BLOCK] = &SdCard::sd_write_multiple_block;
    cmdProc[SD_PROGRAM_CID] = &SdCard::sd_program_cid; /* reserved for manufacturer */
    cmdProc[SD_PROGRAM_CSD] = &SdCard::sd_program_csd;
  }
  if(ccc & CCC_WRITE_PROT) {
    cmdProc[SD_SET_WRITE_PROT] = &SdCard::sd_set_write_prot;
    cmdProc[SD_CLR_WRITE_PROT] = &SdCard::sd_clr_write_prot;
    cmdProc[SD_SEND_WRITE_PROT] = &SdCard::sd_send_write_prot;
  }
  if(ccc & CCC_ERASE) {
    cmdProc[SD_ERASE_WR_BLK_START] = &SdCard::sd_erase_wr_blk_start;
    cmdProc[SD_ERASE_WR_BLK_END] = &SdCard::sd_erase_wr_blk_end;
    cmdProc[SD_ERASE] = &SdCard::sd_erase;
  }
  if(ccc & CCC_SWITCH) {
    cmdProc[SD_ERASE_GROUP_START] = &SdCard::sd_erase_group_start;
    cmdProc[SD_ERASE_GROUP_END] = &SdCard::sd_erase_group_end;
  }
  if(ccc & CCC_LOCK_CARD) {
    cmdProc[SD_LOCK_UNLOCK] = &SdCard::sd_lock_unlock;
  }
  if(ccc & CCC_APP_SPEC) {
    cmdProc[SD_APP_CMD] = &SdCard::sd_app_cmd;
    cmdProc[SD_GEN_CMD] = &SdCard::sd_gen_cmd;
  }
  /* CMD58 is not mentioned in the CCC specification */
  cmdProc[SD_READ_OCR] = &SdCard::sd_read_ocr;
  /* The APP cmds */
  if(ccc & CCC_APP_SPEC) {
    appCmdProc[SD_APP_SET_BUS_WIDTH] = &SdCard::sd_app_set_bus_width;
    appCmdProc[SD_APP_SEND_STATUS] = &SdCard::sd_app_send_status;
    appCmdProc[SD_APP_OP_COND] = &SdCard::sd_app_op_cond;
    appCmdProc[SD_APP_SEND_NUM_WR_BLKS] = &SdCard::sd_app_send_num_wr_blks;
    appCmdProc[SD_APP_SET_WR_BLK_ERASE_COUNT] = &SdCard::sd_app_set_wr_blk_erase_count;
    appCmdProc[SD_APP_SET_CLR_CARD_DETECT] = &SdCard::sd_app_set_clr_card_detect;
    appCmdProc[SD_APP_SEND_SCR] = &SdCard::sd_app_send_scr;
    /*
     * currently I have no hint about the condition under which the secure commands are
     * available
     */
    appCmdProc[SD_APP_SECURE_READ_MULTI_BLOCK] = &SdCard::sd_app_secure_read_multi_block;
    appCmdProc[SD_APP_SECURE_WRITE_MULT_BLOCK] = &SdCard::sd_app_secure_write_mult_block;
    appCmdProc[SD_APP_SECURE_WRITE_MKB] = &SdCard::sd_app_secure_write_mkb;
    appCmdProc[SD_APP_SECURE_ERASE] = &SdCard::sd_app_secure_erase;
    appCmdProc[SD_APP_GET_MKB] = &SdCard::sd_app_get_mkb;
    appCmdProc[SD_APP_GET_MID] = &SdCard::sd_app_get_mid;
    appCmdProc[SD_APP_SET_CER_RN1] = &SdCard::sd_app_set_cer_rn1;
    appCmdProc[SD_APP_GET_CER_RN2] = &SdCard::sd_app_get_cer_rn2;
    appCmdProc[SD_APP_SET_CER_RES2] = &SdCard::sd_app_set_cer_res2;
    appCmdProc[SD_APP_GET_CER_RES1] = &SdCard::sd_app_get_cer_res1;
    appCmdProc[SD_APP_CHANGE_SECURE_AREA] = &SdCard::sd_app_change_secure_area;
  }
}

void SdCard::GotoSpi() {
  int ccc = GETBITS(csd, 84, 95);
  int i;
  spimode = 1;
  crc_on = 0;
  for(i = 0; i < 64; i++) {
    cmdProc[i] = 0;
    appCmdProc[i] = 0;
  }
  if(ccc & CCC_BASIC) {
    cmdProc[SD_SEND_INTERFACE_COND] = &SdCard::spi_send_interface_cond;
    cmdProc[SD_SEND_OP_COND] = &SdCard::spi_send_op_cond; /* CMD1 really available ? */
    cmdProc[SD_GO_IDLE_STATE] = &SdCard::spi_go_idle;
    cmdProc[SD_SEND_STATUS] = &SdCard::spi_send_status;
    cmdProc[SD_SEND_CSD] = &SdCard::spi_send_csd;
    cmdProc[SD_SEND_CID] = &SdCard::spi_send_cid;
    /* Don't know if this is in the right group */
    cmdProc[SD_CRC_ON_OFF] = &SdCard::spi_crc_on_off;
  }
  if(ccc & CCC_SWITCH) {
  }
  if(ccc & CCC_BASIC) {
    cmdProc[SD_STOP_TRANSMISSION] = &SdCard::spi_stop_transmission;
  }
  if(ccc & (CCC_BLOCK_READ | CCC_BLOCK_WRITE | CCC_LOCK_CARD)) {
    cmdProc[SD_SET_BLOCKLEN] = &SdCard::spi_set_blocklen;
  }
  if(ccc & CCC_BLOCK_READ) {
    cmdProc[SD_READ_SINGLE_BLOCK] = &SdCard::spi_read_single_block;
    cmdProc[SD_READ_MULTIPLE_BLOCK] = &SdCard::spi_read_multiple_block;
  }
  if(ccc & CCC_BLOCK_WRITE) {
    cmdProc[SD_WRITE_SINGLE_BLOCK] = &SdCard::spi_write_single_block;
    cmdProc[SD_WRITE_MULTIPLE_BLOCK] = &SdCard::spi_write_multiple_block;
  }
  if(ccc & CCC_WRITE_PROT) {
  }
  if(ccc & CCC_ERASE) {
    cmdProc[SD_ERASE_WR_BLK_START] = &SdCard::spi_erase_wr_blk_start;
    cmdProc[SD_ERASE_WR_BLK_END] = &SdCard::spi_erase_wr_blk_end;
    cmdProc[SD_ERASE] = &SdCard::spi_erase;
  }
  if(ccc & CCC_SWITCH) {
  }
  if(ccc & CCC_LOCK_CARD) {
  }
  if(ccc & CCC_APP_SPEC) {
    cmdProc[SD_APP_CMD] = &SdCard::spi_app_cmd;
    appCmdProc[SD_APP_OP_COND] = &SdCard::spi_app_op_cond;
    appCmdProc[SD_APP_SEND_NUM_WR_BLKS] = &SdCard::spi_app_send_num_wr_blks;
    appCmdProc[SD_APP_SEND_SCR] = &SdCard::spi_app_send_scr;
    /*
     * spi_app_send_status
     * Seems not to exist and fall back to the non APP version
     * Test with sandisk, Toshiba and samsung card
     * card->appCmdProc[SD_APP_SEND_STATUS] = spi_app_send_status;
     */
  }
  /* CMD58 is not mentioned in the CCC specification */
  cmdProc[SD_READ_OCR] = &SdCard::spi_read_ocr;
  /* The APP cmds */
  if(ccc & CCC_APP_SPEC) {
  }
}
/*
 * ----------------------------------------------------------------------------------
 * MMCard_DoCmd
 *  Search a command Proc in the cmd/appCmd Table and execute it
 * ----------------------------------------------------------------------------------
 */
int SdCard::DoCmd(uint32_t cmd, uint32_t arg, SdResponse *resp) {
  int result = SD_ERR_TIMEOUT;
  SdCmdProc SdCard::*_cmdProc = NULL;
  if(cmd > 63) {
    DPRINTF(SdCard, "Illegal cmd opcode 0x%02x\n", cmd);
    return SD_ERR_TIMEOUT;
  }
  DPRINTF(SdCard, "CMD%d, arg 0x%08x app %d\n",cmd, arg, is_app_cmd);
  resp->len = 0;
  memset(resp->data, 0xff, SDCARD_MAX_RESPLEN);
  if (is_app_cmd) {
    _cmdProc = appCmdProc[cmd];
    is_app_cmd = 0;
  }
  /* If app cmd does not exist the regular command is used */
  if(_cmdProc == NULL) {
    _cmdProc = cmdProc[cmd];
  }
  if(_cmdProc) {
    cmdcount++;
    result = (this->*_cmdProc)(cmd, arg, resp);
  } else {
    DPRINTF(SdCard, "CMD%d not implemented\n", cmd);
  }
  if(resp->len > (sizeof(resp->data) / sizeof(resp->data[0])))  {
    DPRINTF(SdCard, "Emulator: invalid response len %d\n", resp->len);
  }
  return result;
}


static uint32_t hash_string(const char *s) {
  uint32_t hash=0;
  while(*s) {
    hash = *s + (hash << 6) + (hash << 16) - hash;
    s++;
  }
  return hash;
}

/*
 * -----------------------------------------------------------------
 * Fill the card registers with values from a template
 * identified by cardtype
 * -----------------------------------------------------------------
 */
int SdCard::init_from_template(const char *cardtype) {
  uint8_t default_ssr[] = {
    0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
    0x02, 0x02, 0x90, 0x02, 0x00, 0xaa, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  };
  int nr_types = sizeof(cardspec)/sizeof(SdCardSpec);
  int i;
  int ssr_ok = 0;
  const SdCardSpec *spec;
  for(i = 0;i < nr_types; i++) {
    spec = &cardspec[i];
    if(strcmp(cardtype, spec->producttype) ==  0) {
      break;
    }
  }
  if(i == nr_types) {
    return -1;
  }
  type = spec->type;
  usec_reset = spec->usec_reset;
  ocr = spec->ocr & ~OCR_NOTBUSY;
  for(i = 0; i < 8; i++) {
    scr[i] = spec->scr[i];
  }
  for(i = 0; i < sizeof(cid); i++) {
    cid[i] = spec->cid[i];
    csd[i] = spec->csd[i];
  }
  for(i = 0; i < sizeof(ssr); i++) {
    ssr[i] = spec->ssr[i];
    if(ssr[i]) {
      ssr_ok = 1;
    }
  }
  /* For cards where I don't have the ssr contents */
  if(!ssr_ok) {
    memcpy(ssr, default_ssr, sizeof(ssr));
  }
  initial_rca = spec->rca;
  return 0;
}

/*
 * ---------------------------------------------
 * Create a card name (from capacity)
 * ---------------------------------------------
 */
void SdCard::create_cardname(const char *prefix) {
  uint64_t _capacity = capacity;
  if(strlen(prefix) > 1) {
    cid[3] = prefix[0];
    cid[4] = prefix[1];
  }
  if(capacity >= 1<<30) {
    int giga = _capacity >> 30;
    cid[5]='0' + (giga / 10);
    cid[6]='0' + (giga % 10);
    cid[7]='G';
  } else if(_capacity >= 1<<20) {
    int mega = _capacity >> 20;
    cid[5]='0' + (mega / 100);
    cid[6]='0' + ((mega / 10) % 10);
    cid[7]='0' + (mega % 10);
  }
}

/*
 * ---------------------------------------------------------------------
 * csd2_0_set_capacity
 *  Set the Card Capacity of a SD card with CSD format 2.0
 *  (SD High Capacity)
 * ---------------------------------------------------------------------
 */

int SdCard::csd2_0_set_capacity(uint64_t _capacity) {
  uint32_t c_size = (capacity >> 19) - 1;
  COPYBITS(c_size, csd, 48, 69);
  COPYBITS(9, csd, 80, 83);
  capacity = _capacity;
  blocklen = 512;
  if(_capacity & 0x7ffff) {
    DPRINTF(SdCard, "Capacity must be multiple of 512k\n");
    return -1;
  }
  create_cardname("HC");
  return 0;
}

/*
 * -----------------------------------------------------------------------
 * csd1_0_set_capacity
 *  Set the Card Capacity of a SD card with CSD format 1.0
 *  Capacity is < 4GB with 2048byte sectors and < 1GB with
 *  512 Byte sectors. The algorithm chooses 512 Byte
 *  sector size if possible.
 * -----------------------------------------------------------------------
 */
int SdCard::csd1_0_set_capacity(uint64_t _capacity) {
  int nr_sectors;
  int shift = 0;
  int c_size_mult, c_size;
  int blkbits = 9;
  nr_sectors = _capacity >> blkbits;
  if(nr_sectors > 4194304) {
    nr_sectors >>= 2;
    blkbits+=2;
  } else if(nr_sectors > 2097152) {
    nr_sectors >>= 1;
    blkbits++;
  }
  while((nr_sectors == ((nr_sectors >> 1) << 1)) && (shift != 9)) {
    shift++;
    nr_sectors >>= 1;
  }
  if(shift < 2) {
    DPRINTF(SdCard, "Illegal MM/SD-Card Size\n");
    return -1;
  }
  if(nr_sectors > 4096) {
    DPRINTF(SdCard, "MM/SD-Card size can not be matched in CSD\n");
    return -1;
  }
  c_size = nr_sectors - 1;
  c_size_mult = shift - 2;
  COPYBITS(c_size, csd, 62, 73);
  COPYBITS(blkbits & 0xf, csd, 80, 83);
  COPYBITS(c_size_mult & 0x7, csd, 47, 49);

  blocklen = 1 << blkbits;
  capacity = blocklen * ((c_size + 1) << (c_size_mult + 2));
  if(type == CARD_TYPE_MMC) {
    create_cardname("MM");
  } else if (type == CARD_TYPE_SD) {
    create_cardname("SD");
  }
  //fprintf(stderr,"blkbits %02x,csd[5]: %02x\n",blkbits,card->csd[5]);
  //exit(1);
  return 0;
}

/*
 * ------------------------------------------------------------
 * csd_set_capacity
 *  Set the card capacity into the CSD register.
 *  Calls CSD structure version dependent set function
 * ------------------------------------------------------------
 */
int SdCard::csd_set_capacity(uint64_t _capacity) {
  int csd_structure;
  csd_structure = getbits(csd, 16, 126, 127);
  switch(csd_structure) {
    case 0:
      return csd1_0_set_capacity(_capacity);
    case 1:
      return csd2_0_set_capacity(_capacity);
    default:
      panic("Unknown CSD structure version\n");
  }
}

SdCard::SdCard(const Params *p) :
  SimObject(p), disk_image(p->image), ctrl(NULL) {
  //int autotype = 0;
  uint32_t psn = hash_string("sdcard0");

  if(p->producttype == "auto_sdhc") {
    if(init_from_template("Adata8G_HC") < 0) {
      panic("Emulator bug, SDHC-Card template not found\n");
    }
    //autotype = 1;
  } else if (p->producttype == "auto_sd") {
    if(init_from_template("Toshiba32M") < 0) {
      panic("Emulator bug, SD-Card template not found\n");
    }
    //autotype = 1;
  } else {
    if(init_from_template(p->producttype.c_str()) < 0) {
      dump_cardtypes();
      panic("SdCard Product \"%s\" not found. Please fix configfile !\n",
            p->producttype);
    }
  }
  capacity = csd_get_capacity(csd, type);
  blocklen = csd_get_blocklen(csd, type);
  COPYBITS(psn, cid, 24, 55);
  card_status = STATUS_READY_FOR_DATA;
  reset_state = RST_NOT_STARTED;
  //clk = Clock_New("%s.clk",name);
  //Clock_SetFreq(card->clk,16*1000*1000); /* Bad, the clock should come from controller */
  //CycleTimer_Init(&card->transmissionTimer,MMCard_DoTransmission,&card->mmcdev);
  if(type == CARD_TYPE_SD) {
    InitCmds();
  }
  crc_on = 1;
  DPRINTF(SdCard, "MM/SD-card type \"%s\" cap. %llub blksize %d\n",
          p->producttype, (unsigned long long)capacity, blocklen);
}

void SdCard::serialize(std::ostream &os) {

}

void SdCard::unserialize(Checkpoint *cp, const std::string &section) {

}

static uint16_t crctab[256];
static uint8_t crc7tab[256];
static int crctab_initialized = 0;

static void CRC16_CreateTab(void);
static void CRC7_CreateTab(void);

/*
 * ------------------------------------------
 * Slow Bitwise CRC
 * ------------------------------------------
 */
static void CRC7_Bitwise(uint8_t *crc, uint8_t val) {
  int i;
  for (i = 7; i >= 0; i--) {
    int carry = *crc & 0x40;
    int inbit = !!(val & (1 << i));
    *crc = (*crc << 1) & 0x7f;
    if (carry) inbit = !inbit;
    if (inbit) {
      *crc = *crc ^ 0x9;
    }
  }
}

/*
 * -------------------------------------------------------
 * Create the CRC table for the fast CRC calculation
 * using the slow bitwise crc
 * -------------------------------------------------------
 */
static void CRC7_CreateTab() {
  uint8_t crc;
  int i;
  for (i = 0; i < 256; i++) {
    crc = 0;
    CRC7_Bitwise(&crc, i);
    crc7tab[i] = crc;
  }
}

void SdCard::SD_CRC7(uint8_t *crc, const uint8_t *vals, int len) {
  uint8_t index;
  int i;
  for (i = 0; i < len; i++) {
    index = (*crc << 1) ^ vals[i];
    *crc = ((*crc << 7) ^ crc7tab[index & 0xff]) & 0x7f;
  }
}

void SdCard::SD_CRC7Init(uint8_t *crc, uint16_t initval) {
  if (!crctab_initialized) {
    CRC16_CreateTab();
    CRC7_CreateTab();
    crctab_initialized = 1;
  }
  *crc = initval;
}
;

/*
 * ------------------------------------------
 * Slow Bitwise CRC
 * ------------------------------------------
 */
static void CRC16_Bitwise(uint8_t val, uint16_t *crc) {
  int i;
  for (i = 7; i >= 0; i--) {
    int carry = *crc & 0x8000;
    int inbit = !!(val & (1 << i));
    *crc = *crc << 1;
    if (carry) inbit = !inbit;
    if (inbit) {
      *crc = *crc ^ 0x1021;
    }
  }
}

/*
 * -------------------------------------------------------
 * Create the CRC table for the fast CRC calculation
 * using the slow bitwise crc
 * -------------------------------------------------------
 */
static void CRC16_CreateTab() {
  uint16_t crc;
  int i;
  for (i = 0; i < 256; i++) {
    crc = 0;
    CRC16_Bitwise(i, &crc);
    crctab[i] = crc;
  }
}

/*
 * ----------------------------------------------------------------
 * CRC16
 *      Set Start value for the CRC (here 0x0000)
 * ----------------------------------------------------------------
 */
void SdCard::SD_CRC16Init(uint16_t *crc, uint16_t initval) {
  if (!crctab_initialized) {
    CRC16_CreateTab();
    CRC7_CreateTab();
    crctab_initialized = 1;
  }
  *crc = initval;
}

void SdCard::SD_CRC16(uint16_t *crc, const uint8_t *vals, int len) {
  uint8_t index;
  int i;
  for (i = 0; i < len; i++) {
    index = (*crc >> 8) ^ vals[i];
    *crc = (*crc << 8) ^ crctab[index];
  }
}

inline uint8_t SdCard::SD_RespCRCByte(SdResponse *resp) {
  uint8_t crc;
  SD_CRC7Init(&crc, 0);
  SD_CRC7(&crc, resp->data, resp->len - 1);
  return (crc << 1) | 1;
}

SdCard *SdCardParams::create() {
  return new SdCard(this);
}
