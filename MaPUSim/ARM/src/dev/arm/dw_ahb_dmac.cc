/*
 * dw_ahb_dmac.cc
 *
 *  Created on: Mar 5, 2014
 *      Author: wangl
 */

#include "base/vnc/vncinput.hh"
#include "base/bitmap.hh"
#include "base/output.hh"
#include "base/trace.hh"
#include "debug/DmaController.hh"
#include "debug/Uart.hh"
#include "dev/arm/amba_device.hh"
#include "dev/arm/base_gic.hh"
#include "dev/arm/dw_ahb_dmac.hh"
#include "mem/packet.hh"
#include "mem/packet_access.hh"
#include "sim/system.hh"

DW_Dma_Controller::DW_Dma_Controller(const Params *p) :
  AmbaDmaDevice(p), status(0), dma_cfg(0), ch_en(0),
  dmaDoneEventAll(maxOutstandingDma, this),
  dmaDoneEventFree(maxOutstandingDma), intEvent(this), gic(p->gic) {
  pioSize = 0xFFFF;
  for (int i = 0; i < maxOutstandingDma; ++i)
    dmaDoneEventFree[i] = &dmaDoneEventAll[i];
}

// read registers
Tick DW_Dma_Controller::read(PacketPtr pkt) {
  uint64_t data = 0;

  assert(pkt->getAddr() >= pioAddr &&
         pkt->getAddr() < pioAddr + pioSize);

  Addr daddr = pkt->getAddr() - pioAddr;
  pkt->allocate();

  DPRINTF(DmaController, " read register %#x size=%d\n", daddr, pkt->getSize());

  if (daddr < RAW_START) {
    int chid = daddr / 0x58;
    int choff = (daddr % 0x58) >> 3;
    data = ((uint64_t *)(&(ch_conf[chid])))[choff];
  } else if (daddr >= STATUS_INT) {
    switch (daddr) {
    case STATUS_INT:
      data = status;
      break;
    case DMA_CFG_REG:
      data = dma_cfg;
      break;
    case CH_EN_REG:
      data = ch_en;
      break;
    default:
      panic("Tried to read DmaController at offset %#x that doesn't exist\n",
            daddr);
      break;
    }
  } else {
    uint64_t type = (daddr - RAW_START) / 40;
    uint64_t offset = ((daddr - RAW_START) % 40) >> 3;
    switch (type) {
    case 0:
      data = ((uint64_t *)&raw_int)[offset];
      break;
    case 1:
      data = ((uint64_t *)&status_int)[offset];
      break;
    case 2:
      data = ((uint64_t *)&mask_int)[offset];
      break;
    case 3:
      data = ((uint64_t *)&clear_int)[offset];
      break;
    }
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
  case 8:
    pkt->set<uint64_t>(data);
    break;
  default:
    panic("DmaController read size too big?\n");
    break;
  }

  pkt->makeAtomicResponse();
  return pioDelay;
}

// write registers
Tick DW_Dma_Controller::write(PacketPtr pkt) {
  uint64_t data = 0;

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
  case 8:
    data = pkt->get<uint64_t>();
    break;
  default:
    panic("DmaController write size too big?\n");
    break;
  }

  assert(pkt->getAddr() >= pioAddr &&
         pkt->getAddr() < pioAddr + pioSize);

  Addr daddr = pkt->getAddr() - pioAddr;

  DPRINTF(DmaController, " write register %#x value %#x size=%d\n", daddr,
          pkt->get<uint32_t>(), pkt->getSize());

  if (daddr < RAW_START) {
    int chid = daddr / 0x58;
    int choff = daddr % 0x58;
    if (pkt->getSize() == 4) {
      choff >>= 2;
      ((uint32_t *)(&(ch_conf[chid])))[choff] = data;
    } else if (pkt->getSize() == 8) {
      choff >>= 3;
      ((uint64_t *)(&(ch_conf[chid])))[choff] = data;
    }
  } else if (daddr >= STATUS_INT) {
    switch (daddr) {
    case DMA_CFG_REG:
      dma_cfg = data;
      if (ch_en) {
        for (unsigned i = 0; i < 8; i++) {
          if (ch_en & (1 << i))
            startDma(i);
        }
      }
      break;
    case CH_EN_REG:
      ch_en = ((~(data >> 8) & ch_en) | (((data >> 8) & data))) & 0xFF;
      if (dma_cfg) {
        for (unsigned i = 0; i < 8; i++) {
          if (((data >> 8) & data) & (1 << i))
            startDma(i);
        }
      }
      break;
    default:
      panic("Tried to read DmaController at offset %#x that doesn't exist\n",
            daddr);
      break;
    }
  } else {
    uint64_t type = (daddr - RAW_START) / 40;
    uint64_t offset = ((daddr - RAW_START) % 40) >> 3;
    switch (type) {
    case 2:
      ((uint64_t *)&mask_int)[offset] =
        ((~(data >> 8) & ((uint64_t *)&mask_int)[offset]) |
        (((data >> 8) & data))) & 0xFF;;
      updateStatus();
      break;
    case 3:
      ((uint64_t *)&clear_int)[offset] = data;
      updateStatus();
      break;
    }
  }

  pkt->makeAtomicResponse();
  return pioDelay;
}

void DW_Dma_Controller::generateInterrupt() {
  DPRINTF(DmaController, "Generate Interrupt: Status=0x%llx DMA_Cfg=0x%llx "
          "Channel=0x%llx RawInt=0x%llx\n",
          status, dma_cfg, ch_en, raw_int.transfer);
  if (status) gic->sendInt(intNum);
  else gic->clearInt(intNum);
}

void DW_Dma_Controller::updateStatus() {
  raw_int.block = raw_int.block & ~clear_int.block;
  clear_int.block = 0;
  raw_int.dest_tran = raw_int.dest_tran & ~clear_int.dest_tran;
  clear_int.dest_tran = 0;
  raw_int.error = raw_int.error & ~clear_int.error;
  clear_int.error = 0;
  raw_int.src_tran = raw_int.src_tran & ~clear_int.src_tran;
  clear_int.src_tran = 0;
  raw_int.transfer = raw_int.transfer & ~clear_int.transfer;
  clear_int.transfer = 0;

  status_int.block = raw_int.block & mask_int.block;
  status_int.dest_tran = raw_int.dest_tran & mask_int.dest_tran;
  status_int.error = raw_int.error & mask_int.error;
  status_int.src_tran = raw_int.src_tran & mask_int.src_tran;
  status_int.transfer = raw_int.transfer & mask_int.transfer;

  uint64_t oldstatus = status;
  status = (((status_int.error & 0xFF) != 0) << 4) |
           (((status_int.dest_tran & 0xFF) != 0) << 3) |
           (((status_int.src_tran & 0xFF) != 0) << 2) |
           (((status_int.block & 0xFF) != 0) << 1) |
           ((status_int.transfer & 0xFF) != 0);

  if ((oldstatus != status)) {
    DPRINTF(DmaController, "int Event sched at %d\n", clockEdge());

    schedule(intEvent, clockEdge());
  }
}

AddrRangeList DW_Dma_Controller::getAddrRanges() const {
  AddrRangeList ranges;
  ranges.push_back(RangeSize(pioAddr, pioSize));
  return ranges;
}

void DW_Dma_Controller::dmaDone(uint8_t ch_id) {
  DPRINTF(DmaController, "DMA Done.\n");
  if (ch_conf[ch_id].ctrl_reg & 1) {
    raw_int.transfer |= 1 << ch_id;
    updateStatus();
  }
}

void DW_Dma_Controller::startDma(uint8_t ch_id) {
  assert(!dmaDoneEventFree.empty());
  DmaDoneEvent *event = dmaDoneEventFree.back();
  dmaDoneEventFree.pop_back();
  assert(!event->scheduled());
  event->setCh(ch_id);

  Addr startAddr = ch_conf[ch_id].src_addr_reg;
  uint64_t dmaSize = (ch_conf[ch_id].ctrl_reg >> 32) <<
                     ((ch_conf[ch_id].ctrl_reg >> 1) & 0x7);
  uint8_t *dmaBuffer = new uint8_t[dmaSize];
  event->setBuffer(dmaBuffer);
  dmaPort.dmaAction(MemCmd::ReadReq, startAddr, dmaSize,
                    event, dmaBuffer, 0, Request::UNCACHEABLE);
}

void DW_Dma_Controller::serialize(std::ostream &os) {
  DPRINTF(DmaController, "Serializing ARM DW_Dma_Controller\n");
}

void DW_Dma_Controller::unserialize(Checkpoint *cp,
                                    const std::string &section) {
  DPRINTF(DmaController, "Unserializing ARM DW_Dma_Controller\n");
}

DW_Dma_Controller *DW_Dma_ControllerParams::create() {
  return new DW_Dma_Controller(this);
}
