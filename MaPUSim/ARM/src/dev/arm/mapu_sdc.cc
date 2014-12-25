/*
 * mapu_sdc.cc
 *
 *  Created on: Mar 22, 2014
 *      Author: wangl
 */

#include "dev/arm/amba_device.hh"
#include "dev/arm/base_gic.hh"
#include "dev/arm/mapu_sdc.hh"
#include "debug/SdCtrl.hh"

SdController::SdController(const Params *p) :
  AmbaDmaDevice(p), magic(0x62381233), sdclk(p->sdclk),
  outbuf_wp(0), outbuf_rp(0), inbuf_wp(0), inbuf_rp(0),
  regSTATE(0xF0400), regCLK(2), regCAP(0x670430b0),
  card(p->card), dmaDoneEvent(this) {
  pioSize = 0xFFFF;
  // update_clock();
  if (card) card->setController(this);
}


void SdController::tmode_write(uint32_t value) {
  DPRINTF(SdCtrl, "Tmode 0x%08x\n", value);
  regTMDCMD = (regTMDCMD & 0xffff0000) | (value & 0xffff);
}

uint32_t SdController::cmd_read() {
  return regTMDCMD >> 16;
}

void SdController::cmd_write(uint32_t value) {
  regTMDCMD = (regTMDCMD & 0xffff) |  (value << 16);
  DPRINTF(SdCtrl, "Command 0x%08x\n", value);
  do_cmd_delayed();
}

uint32_t SdController::datal_read(int32_t rqlen) {
  int i;
  int rcnt = 0;
  uint32_t value = 0;
  for(i = 0; i < rqlen; i++) {
    if(IBUF_CNT > 0) {
      ((uint8_t *)&value)[i] = inbuf[IBUF_RP];
      inbuf_rp++;
      rcnt++;
    }
  }
  if (IBUF_CNT == 0)
    regSTATE &= ~STATE_RDEN;
  DPRINTF(SdCtrl, "%s: data L read(%d/%d) %04x\n", __func__, rcnt, IBUF_CNT, value);
  if ((regTMDCMD & TMODE_BCNTEN) && regBCNT && IBUF_CNT == 0)
    card->StartTransmission();
  return value;
}

void SdController::datal_write(uint32_t value, int32_t rqlen) {
  int i;
  DPRINTF(SdCtrl, "%s: data L write(%d/%d) %04x\n", __func__, rqlen, OBUF_CNT, value);
  for(i = 0; i < rqlen; i++) {
    uint8_t data = value & 0xff;
    if(OBUF_SPACE > 0) {
      outbuf[OBUF_WP] = data;
      outbuf_wp++;
    }
    value >>= 8;
  }
  if (OBUF_CNT >= dma_expected_bytes)
    do_transfer_write();
}

uint32_t SdController::datah_read(int32_t rqlen) {
  int i;
  int rcnt = 0;
  uint32_t value = 0;
  for(i = 0; i < rqlen;i++) {
    value <<= 8;
    if (IBUF_CNT > 0) {
      value |= inbuf[IBUF_RP];
      inbuf_rp++;
      rcnt++;
    }
  }
  if (IBUF_CNT == 0)
    regSTATE &= ~STATE_RDEN;
  DPRINTF(SdCtrl, "%s: data H read(%d/%d) %04x\n", __func__, rcnt, IBUF_CNT, value);
  return value;
}

void SdController::datah_write(uint32_t value, int32_t rqlen) {
  int i;
  for(i = 0; i < rqlen;i++) {
    uint8_t data = value & 0xff;
    outbuf[OBUF_WP] = data;
    if(OBUF_SPACE > 0) {
      outbuf_wp++;
    }
    value >>= 8;
    if (OBUF_CNT == regBSIZE) {
      regSTATE &= ~STATE_WREN;
    }
  }
  // Check_start_transfer
  DPRINTF(SdCtrl, "%s: Register not implemented\n", __func__);
}

void SdController::clk_write(uint32_t value) {
  regCLK  = value | 0x2;
  DPRINTF(SdCtrl, "Clock register written with 0x%08x\n", value);
  // update_clock();
}

void SdController::swreset_write(uint32_t value) {
  DPRINTF(SdCtrl, "SW Reset register written with 0x%08x\n", value);
  if ((value & 0x4) || (value & 0x1)) {

  }
  // update_clock();
}

void SdController::stsl_write(uint32_t value) {
  regSTS &= ~(value & 0xffff00ff);
  if (!(regSTS & 0xffff0000)) regSTS &= ~STS_ERRINT;
  //regSTS &= ~((~value) & 0xff00);
  update_interrupt();
}

void SdController::stsh_write(uint32_t value) {
  DPRINTF(SdCtrl, "%s: clear %04x\n", __func__, value);
  regSTS &= ~(value << 16);
  update_interrupt();
}

void SdController::stsenl_write(uint32_t value) {
  regSTSEN = (regSTSEN & 0xffff0000) | (value & 0xffff);
  DPRINTF(SdCtrl, "%s: Register not tested\n", __func__);
  update_interrupt();
}

void SdController::stsenh_write(uint32_t value) {
  regSTSEN = (regSTSEN & 0xffff) | ((value  & 0xffff) << 16);
  DPRINTF(SdCtrl, "%s: Register not implemented\n", __func__);
  update_interrupt();
}

void SdController::intenl_write(uint32_t value) {
  regINTEN = (regINTEN & 0xffff0000) | value;
  DPRINTF(SdCtrl, "INTEN write\n");
  update_interrupt();
}

void SdController::intenh_write(uint32_t value) {
  regINTEN = (regINTEN & 0xffff) | (value << 16);
  update_interrupt();
}

int SdController::receive_data(int len) {
  int i;
  DPRINTF(SdCtrl, "GOT %d bytes of data, expected %d, addr SDMA 0x%08x\n",
          len, dma_expected_bytes, regSDMA);

  if (regTMDCMD & TMODE_DMAEN) {
    startDma();
  } else {
    for (i = 0; i < len; i++) {
      inbuf[IBUF_WP] = buf[i];
      inbuf_wp += 1;
    }
    regSTATE &= ~STATE_DATACT;
    //regSTS |= STS_TDONE;
    regSTATE |= STATE_RDEN;
    regSTS |= STS_RDRDY;
    if ((regTMDCMD & TMODE_BCNTEN) && regBCNT) {
      regBCNT--;
      if (regBCNT == 0)
        regSTS |= STS_TDONE;
    } else {
      regSTS |= STS_TDONE;
    }
    //dbgprintf(stderr,"STS_DATADONE***\n");
    update_interrupt();
  }
  return len;
}

void SdController::do_transfer_write() {
  if((regTMDCMD & TMODE_DIR_R) || !(regSTATE & STATE_DATACT)) {
    DPRINTF(SdCtrl, "Unexpected data write\n");
    return;
  }
  if (regTMDCMD & TMODE_DMAEN)
    startDma();
  else {
    // fetch from fifo
    card->write(&outbuf[OBUF_RP], dma_expected_bytes);
    regSTATE |= STATE_WREN;
    regSTS |= STS_WRRDY;
    outbuf_rp += dma_expected_bytes;
    dma_expected_bytes = 0;
    if ((regTMDCMD & TMODE_BCNTEN) && regBCNT) {
      regBCNT--;
      if (regBCNT == 0)
        regSTS |= STS_TDONE;
      else
        dma_expected_bytes = (regBSIZE & 0xfff) | ((regBSIZE >> 3) & 0x1000);
    } else {
      regSTS |= STS_TDONE;
    }
    update_interrupt();
  }
  return;
}

void SdController::do_cmd_delayed() {
  SdResponse resp;
  int rtype = regTMDCMD & TMODE_RTYPE_MSK;
  int result;
  uint8_t cmd = (regTMDCMD & TMODE_CMDINDEX_MSK) >> TMODE_CMDINDEX_SHIFT;

  if (!card) {
    regSTATE &= ~(STATE_NOCMD | STATE_NODAT | STATE_DATACT | STATE_WRACT |
                  STATE_RDACT | STATE_RDEN);
    regSTS |= STS_ERR_CMDTIME;
    update_interrupt();
    //update_interrupt(sdc);
    return;
  }

  dma_expected_bytes = regBSIZE & 0xfff;
  result = card->DoCmd(cmd, regARG, &resp);
  DPRINTF(SdCtrl, "Done cmd %d with result %d\n", cmd, result);
  //sleep(1);
  if (result != SD_ERR_NONE) {
    /* Hack, I don't know real response */
    regSTATE &= ~(STATE_NOCMD | STATE_NODAT | STATE_DATACT | STATE_WRACT |
                  STATE_RDACT | STATE_RDEN);
    regSTS |= STS_CDONE;
    regSTS |= STS_ERRINT;
#if 0
    if(result == MMC_ERR_BADCRC) {
      sdc->regSTS |= STS_ERR_CMDCRC;
    } else {
#endif
    regSTS |= STS_ERR_CMDTIME;
    update_interrupt();
    return;
  }
  if (resp.len > 17) {
    warn("Emulator bug: SD/MMC card response longer than 17 Bytes\n");
    resp.len = 16;
  }
  /* Handle the response from SD/MMC card */
  DPRINTF(SdCtrl, "RTYPE %08x\n", rtype);
  if (rtype == RTYPE_NO_RESP) {
  } else if (rtype == RTYPE_R2) {
    DPRINTF(SdCtrl, "RESPONSE_TYPE R2\n");
    regRESP[0] = resp.data[15] | (resp.data[14] << 8);
    regRESP[1] = resp.data[13] | (resp.data[12] << 8);
    regRESP[2] = resp.data[11] | (resp.data[10] << 8);
    regRESP[3] = resp.data[9] | (resp.data[8] << 8);
    regRESP[4] = resp.data[7] | (resp.data[6] << 8);
    regRESP[5] = resp.data[5] | (resp.data[4] << 8);
    regRESP[6] = resp.data[3] | (resp.data[2] << 8);
    regRESP[7] = resp.data[1] | (resp.data[0] << 8);
  } else if (rtype == RTYPE_134567) {
    regRESP[0] = (resp.data[3] << 8) | resp.data[4];
    regRESP[1] = (resp.data[1] << 8) | resp.data[2];
  } else if (rtype == RTYPE_R1BR5B) {
    regRESP[0] = (resp.data[3] << 8) | resp.data[4];
    regRESP[1] = (resp.data[1] << 8) | resp.data[2];
  } else {
    panic("nonexisting RTYPE %d\n", rtype);
  }
  regSTATE &= ~(STATE_NOCMD);
  regSTS |= STS_CDONE;
  update_interrupt();
  if (regTMDCMD & TMODE_DATSEL) {
    if ((regTMDCMD & TMODE_MS) && (regTMDCMD & TMODE_BCNTEN)) {
      if (regBCNT == 0) {
        return;
      }
    }
    sdma_bufsize = 4096 << ((regBSIZE >> 12) & 7);
    regSTATE |= STATE_DATACT;
    if (regTMDCMD & TMODE_DIR_R) {
//CycleTimer_Mod(&sdc->data_delay_timer,MicrosecondsToCycles(UDELAY_DATA));
    } else {
      /* Direction is write */
      /* do data_write */
      if (OBUF_CNT >= dma_expected_bytes)
        do_transfer_write();
      else {
        regSTATE |= STATE_WREN;
        regSTS |= STS_WRRDY;
        update_interrupt();
      }
    }
  } else {
    regSTATE &= ~(STATE_NODAT); /* ? */
  }
}

void SdController::update_interrupt() {
  uint32_t sts = regSTS;
  uint32_t stsen = regSTSEN;
  uint32_t inten = regINTEN;
  if (sts & stsen & inten) {
    DPRINTF(SdCtrl, "Post SD-Interrupt %08x %08x %08x\n", sts, stsen, inten);
    gic->sendInt(intNum);
  } else {
    DPRINTF(SdCtrl, "Unpost SD-Interrupt %08x %08x %08x\n", sts, stsen, inten);
    gic->clearInt(intNum);
  }
}

// read registers and frame buffer
Tick SdController::read(PacketPtr pkt) {
  // use a temporary data since the LCD registers are read/written with
  // different size operations

  uint32_t data = 0;

  assert(pkt->getAddr() >= pioAddr && pkt->getAddr() < pioAddr + pioSize);

  Addr daddr = pkt->getAddr() - pioAddr;
  pkt->allocate();

  DPRINTF(SdCtrl, " read register %#x size=%d\n", daddr, pkt->getSize());

  const uint32_t base = 0;
  switch (daddr) {
  case SD_SDMA(base): data = regSDMA; break;
  case SD_BSIZE(base): data = regBSIZE; break;
  case SD_BCNT(base): data = regBCNT; break;
  case SD_ARG(base): data = regARG; break;
  case SD_TMODE(base): data = regTMDCMD; break;
  case SD_CMD(base): data = cmd_read(); break;
  /*case SD_RESP0(base): data = regRESP[1]; data = (data << 16) | regRESP[0]; break;
  case SD_RESP1(base): data = regRESP[1]; break;
  case SD_RESP2(base): data = regRESP[3]; data = (data << 16) | regRESP[2]; break;
  case SD_RESP3(base): data = regRESP[3]; break;
  case SD_RESP4(base): data = regRESP[5]; data = (data << 16) | regRESP[4]; break;
  case SD_RESP5(base): data = regRESP[5]; break;
  case SD_RESP6(base): data = regRESP[7]; data = (data << 16) | regRESP[6]; break;
  case SD_RESP7(base): data = regRESP[7]; break;*/
  case SD_DATAL(base): data = datal_read(pkt->getSize()); break;
  case SD_DATAH(base): data = datah_read(pkt->getSize()); break;
  case SD_STATE(base): data = regSTATE; break;
  case SD_CONT1(base): data = regCONT & 0xff; break;
  case SD_POWER(base): data = (regCONT >> 8) & 0xff; break;
  case SD_BLOCKGAP(base): data = (regCONT >> 16) & 0xff; break;
  case SD_WAKEUP(base): data = regCONT >> 24; break;
  case SD_CLK(base): data = regCLK; break;
  case SD_TIME(base): data = regTIME; break;
  case SD_SWRESET(base): data = regSWRESET; break;
  case SD_STSL(base): data = regSTS; break;
  case SD_STSH(base): data = regSTS >> 16; break;
  case SD_STSENL(base): data = regSTSEN; break;
  case SD_STSENH(base): data = regSTSEN >> 16; break;
  case SD_INTENL(base): data = regINTEN; break;
  case SD_INTENH(base): data = regSTSEN >> 16; break;
  case SD_CMD12ERR(base): data = 0; break;
  case SD_CTRL2(base): data = 0; break;
  case SD_CAPL(base): data = regCAP; break;
  case SD_CAPH(base): data = 0; break;
  case SD_CURL(base): data = 0x00FFFFFF; break;
  case SD_CURH(base): data = 0; break;
  case SD_FORCEL(base): data = 0; break;
  case SD_FORCEH(base): data = 0; break;
  case SD_ADMAERR(base): data = 0; break;
  case SD_ADDR0(base): data = 0; break;
  case SD_ADDR1(base): data = 0; break;
  case SD_ADDR2(base): data = 0; break;
  case SD_ADDR3(base): data = 0; break;
  case SD_SLOT(base): data = 0; break;
  case SD_VERSION(base): data = 0; break;

  default:
    if (daddr >= SD_RESP0(base) && daddr < SD_DATAL(base)) {
      data = *(uint32_t *)&(((uint8_t *)regRESP)[daddr - SD_RESP0(base)]);
    } else {
      panic("Tried to read SD controller register at offset %#x that \
             doesn't exist\n", daddr);
    }
    break;
  }

  switch (pkt->getSize()) {
  case 1:
    pkt->set<uint8_t>(data);
    break;
  case 2:
    pkt->set<uint16_t>(data);
    break;
  case 4:
    pkt->set<uint32_t>(data);
    break;
  default:
    panic("SD controller read size too big?\n");
    break;
  }

  pkt->makeAtomicResponse();
  return pioDelay;
}

void SdController::setTDone() {
  regSTS |= STS_TDONE;
  regBCNT = 0;
  update_interrupt();
}

// write registers and frame buffer
Tick SdController::write(PacketPtr pkt) {
  // use a temporary data since the LCD registers are read/written with
  // different size operations
  //
  uint32_t data = 0;

  switch (pkt->getSize()) {
  case 1:
    data = pkt->get<uint8_t>();
    break;
  case 2:
    data = pkt->get<uint16_t>();
    break;
  case 4:
    data = pkt->get<uint32_t>();
    break;
  default:
    panic("SD controller write size too big?\n");
    break;
  }

  assert(pkt->getAddr() >= pioAddr && pkt->getAddr() < pioAddr + pioSize);

  Addr daddr = pkt->getAddr() - pioAddr;

  DPRINTF(SdCtrl, " write register %#x value %#x size=%d\n", daddr,
          data, pkt->getSize());

  const uint32_t base = 0;

  switch (daddr) {
  case SD_SDMA(base): regSDMA = data; break;
  case SD_BSIZE(base): regBSIZE = data; break;
  case SD_BCNT(base): regBCNT = data; break;
  case SD_ARG(base): regARG = data; break;
  case SD_TMODE(base): tmode_write(data); break;
  case SD_CMD(base): cmd_write(data); break;
  case SD_RESP0(base): regRESP[0] = data; break;
  case SD_RESP1(base): regRESP[1] = data; break;
  case SD_RESP2(base): regRESP[2] = data; break;
  case SD_RESP3(base): regRESP[3] = data; break;
  case SD_RESP4(base): regRESP[4] = data; break;
  case SD_RESP5(base): regRESP[5] = data; break;
  case SD_RESP6(base): regRESP[6] = data; break;
  case SD_RESP7(base): regRESP[7] = data; break;
  case SD_DATAL(base): datal_write(data, pkt->getSize()); break;
  case SD_DATAH(base): datah_write(data, pkt->getSize()); break;
  case SD_STATE(base): panic("SdCtrl: Write to read-only register."); break;
  case SD_CONT1(base): regCONT = (regCONT & 0xffffff00) | (data & 0xff); break;
  case SD_POWER(base): regCONT = (regCONT & 0xffff00ff) | ((data & 0xff) << 8); break;
  case SD_BLOCKGAP(base): regCONT = (regCONT & 0xff00ffff) | ((data & 0xff) << 16); break;
  case SD_WAKEUP(base): regCONT = (regCONT & 0xffffff) | ((data & 0xff) << 24); break;
  case SD_CLK(base): clk_write(data); break;
  case SD_TIME(base): regTIME = data; break;
  case SD_SWRESET(base): swreset_write(data); break;
  case SD_STSL(base): stsl_write(data); break;
  case SD_STSH(base): stsh_write(data); break;
  case SD_STSENL(base): stsenl_write(data); break;
  case SD_STSENH(base): stsenh_write(data); break;
  case SD_INTENL(base): intenl_write(data); break;
  case SD_INTENH(base): intenh_write(data); break;
  case SD_CMD12ERR(base): warn("SdCtrl: unimplemented register %d.", SD_CMD12ERR(base)); break;
  case SD_CAPL(base): panic("SdCtrl: Write to read-only register."); break;
  case SD_CAPH(base): panic("SdCtrl: Write to read-only register."); break;
  case SD_CURL(base): warn("SdCtrl: unimplemented register %d.", SD_CURL(base)); break;
  case SD_CURH(base): warn("SdCtrl: unimplemented register %d.", SD_CURH(base)); break;
  case SD_FORCEL(base): warn("SdCtrl: unimplemented register %d.", SD_FORCEL(base)); break;
  case SD_FORCEH(base): warn("SdCtrl: unimplemented register %d.", SD_FORCEH(base)); break;
  case SD_ADMAERR(base): warn("SdCtrl: unimplemented register %d.", SD_ADMAERR(base)); break;
  case SD_ADDR0(base): warn("SdCtrl: unimplemented register %d.", SD_ADDR0(base)); break;
  case SD_ADDR1(base): warn("SdCtrl: unimplemented register %d.", SD_ADDR1(base)); break;
  case SD_ADDR2(base): warn("SdCtrl: unimplemented register %d.", SD_ADDR2(base)); break;
  case SD_ADDR3(base): warn("SdCtrl: unimplemented register %d.", SD_ADDR3(base)); break;
  case SD_SLOT(base): warn("SdCtrl: unimplemented register %d.", SD_SLOT(base)); break;
  case SD_VERSION(base): warn("SdCtrl: unimplemented register %d.", SD_VERSION(base)); break;

  default:
    panic("Tried to write SD Controller register at offset %#x that \
           doesn't exist\n", daddr);
    break;
  }

  pkt->makeAtomicResponse();
  return pioDelay;
}

void SdController::serialize(std::ostream &os) {

}

void SdController::unserialize(Checkpoint *cp, const std::string &section) {

}

AddrRangeList SdController::getAddrRanges() const {
  AddrRangeList ranges;
  ranges.push_back(RangeSize(pioAddr, pioSize));
  return ranges;
}

void SdController::startDma() {
  if (dmaDoneEvent.scheduled()) {
    warn("Unexpected DMA command for SD Controller.");
    return;
  }
  if (regTMDCMD & TMODE_DIR_R) {
    if (regTMDCMD & TMODE_DMAEN)
      dmaPort.dmaAction(MemCmd::WriteReq, regSDMA, dma_expected_bytes,
                        &dmaDoneEvent, buf, 0, Request::UNCACHEABLE);
  } else {
      dmaPort.dmaAction(MemCmd::ReadReq, regSDMA, dma_expected_bytes,
                        &dmaDoneEvent, buf, 0, Request::UNCACHEABLE);
  }
}

void SdController::dmaDone() {
  int result;
  if (regTMDCMD & TMODE_DIR_R) {
    DPRINTF(SdCtrl, "Read %d bytes\n", dma_expected_bytes);
    dma_expected_bytes = 0;
    if (regTMDCMD & TMODE_MS) {
      if ((regTMDCMD & TMODE_BCNTEN) && regBCNT) {
        regBCNT--;
        if (regBCNT) {
          DPRINTF(SdCtrl, "BCNT now %d\n", regBCNT);
          dma_expected_bytes = (regBSIZE & 0xfff) | ((regBSIZE >> 3)  & 0x1000);
          card->StartTransmission();
          return;
        }
      }
    }
    regSTATE &= ~STATE_DATACT;
    regSTS |= STS_TDONE;
  } else {
    DPRINTF(SdCtrl, "Written %d bytes\n", dma_expected_bytes);
    if (dma_expected_bytes) {
      if (card) {
        result = card->write(buf, dma_expected_bytes);
        if(result) {
          /* do something */
        }
      }
    }
    dma_expected_bytes = 0;
    if (regTMDCMD & TMODE_MS) {
      if ((regTMDCMD & TMODE_BCNTEN) && regBCNT) {
        regBCNT--;
        if (regBCNT) {
          DPRINTF(SdCtrl, "BCNT now %d\n", regBCNT);
          dma_expected_bytes = (regBSIZE & 0xfff) | ((regBSIZE >> 3)  & 0x1000);
          do_transfer_write();
          return;
        } else if (regTMDCMD & TMODE_ACMD12) {
          SdResponse resp ;
          int result;
          if (card) {
            result = card->DoCmd(12, 0, &resp);
            if (result == SD_ERR_NONE) {
              regRESP[6] = (resp.data[3] << 8) | resp.data[4];
              regRESP[7] = (resp.data[1] << 8) | resp.data[2];
            }
          }
        }
      } else {
        dma_expected_bytes = (regBSIZE & 0xfff) | ((regBSIZE >> 3)  & 0x1000);
      }
    }
    regSTATE &= ~STATE_DATACT;
    regSTS |= STS_TDONE;
  }
  //fprintf(stderr,"STS_DATADONE WRITE*****************\n");
  update_interrupt();
}

SdController *SdControllerParams::create() {
  return new SdController(this);
}
