/*
 * dw_apb_uart.cc
 *
 *  Created on: Mar 7, 2014
 *      Author: wangl
 */

#include "base/trace.hh"
#include "debug/Checkpoint.hh"
#include "debug/Uart.hh"
#include "dev/arm/amba_device.hh"
#include "dev/arm/base_gic.hh"
#include "dev/arm/dw_apb_uart.hh"
#include "dev/terminal.hh"
#include "mem/packet.hh"
#include "mem/packet_access.hh"
#include "sim/sim_exit.hh"

DW_Uart::DW_Uart(const Params *p)
    : Uart(p, 0xfff), control(0x300), fbrd(0), ibrd(0), lcr(0), lsr(0x60),
      ifls(0x12), iir(1), msr(0), scr(0),
      imsc(0), rawInt(0), intNum(p->int_num), gic(p->gic),
      endOnEOT(p->end_on_eot), intDelay(p->int_delay), intEvent(this) {
}

Tick DW_Uart::read(PacketPtr pkt) {
  assert(pkt->getAddr() >= pioAddr && pkt->getAddr() < pioAddr + pioSize);

  Addr daddr = pkt->getAddr() - pioAddr;
  pkt->allocate();

  //DPRINTF(Uart, " read register %#x size=%d\n", daddr, pkt->getSize());

  // use a temporary data since the uart registers are read/written with
  // different size operations
  //
  uint32_t data = 0;

  switch (daddr) {
  case UART_R_MMAP::RBR:
    data = 0;
    if (lcr & 0x80) data = fbrd;
    else if (term->dataAvailable()) {
      data = term->in();
      if (!term->dataAvailable()) {
        rawInt.rxim = 0;
        if (rawInt.txim && imsc.txim) {
          iir = 0x2;
        } else {
          iir = 0x1;
          gic->clearInt(intNum);
        }
        lsr &= ~1;
      } else {
        if (iir == 0x4)
          gic->sendInt(intNum);
      }
    }
    break;
  case UART_R_MMAP::IER_R:
    if (lcr & 0x80) data = ibrd;
    else data = imsc;
    break;
  case UART_R_MMAP::IIR:
    data = iir;
    if (iir == 0x2) {
      rawInt.txim = 0;
      if (rawInt.rxim && imsc.rxim) {
        iir = 0x4;
        gic->sendInt(intNum);
      } else {
        iir = 0x1;
        gic->clearInt(intNum);
      }
    }
    break;
  case UART_R_MMAP::LCR_R:
    data = lcr;
    break;
  case UART_R_MMAP::MCR_R:
    data = control;
    break;
  case UART_R_MMAP::LSR:
    data = lsr;
    break;
  case UART_R_MMAP::MSR:
    data = msr;
    break;
  case UART_R_MMAP::SCR_R:
    data = scr;
    break;
  default:
    if (readId(pkt, AMBA_ID, pioAddr)) {
      // Hack for variable size accesses
      data = pkt->get<uint32_t>();
      break;
    }

    panic("Tried to read DW_Uart at offset %#x that doesn't exist\n", daddr);
    break;
  }

  DPRINTF(Uart, " read register %#x size=%d val=%x\n", daddr, pkt->getSize(), data);

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
    panic("Uart read size too big?\n");
    break;
  }

  pkt->makeAtomicResponse();
  return pioDelay;
}

Tick DW_Uart::write(PacketPtr pkt) {
  assert(pkt->getAddr() >= pioAddr && pkt->getAddr() < pioAddr + pioSize);

  Addr daddr = pkt->getAddr() - pioAddr;

  DPRINTF(Uart, " write register %#x value %#x size=%d\n", daddr,
          pkt->get<uint8_t>(), pkt->getSize());

  // use a temporary data since the uart registers are read/written with
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
    panic("Uart write size too big?\n");
    break;
  }

  switch (daddr) {
  case UART_W_MMAP::THR:
    if (lcr & 0x80) fbrd = data;
    else {
      if ((data & 0xFF) == 0x04 && endOnEOT)
        exitSimLoop("UART received EOT", 0);

      term->out(data & 0xFF);

      //raw interrupt is set regardless of imsc.txim
      rawInt.txim = 1;
      if (imsc.txim) {
        DPRINTF(Uart, "TX int enabled, scheduling interrupt\n");
        if (!intEvent.scheduled()) schedule(intEvent, curTick() + intDelay);
      }
    }
    break;
  case UART_W_MMAP::IER_W:
    if (lcr & 0x80) ibrd = data;
    else imsc = data;

    if (imsc.rxim && rawInt.rxim) {
      DPRINTF(Uart, "Writing to IMSC: RX int enabled, scheduling interrupt\n");
      if (!intEvent.scheduled()) schedule(intEvent, curTick() + intDelay);
    } else if (imsc.txim && rawInt.txim) {
      DPRINTF(Uart, "Writing to IMSC: TX int enabled, scheduling interrupt\n");
      if (!intEvent.scheduled()) schedule(intEvent, curTick() + intDelay);
    } else {
      iir = 0x1;
      gic->clearInt(intNum);
    }
    break;
  case UART_W_MMAP::FCR:
    break;
  case UART_W_MMAP::LCR_W:
    lcr = data;
    break;
  case UART_W_MMAP::MCR_W:
    control = data;
    break;
  case UART_W_MMAP::SCR_W:
    scr = data;
    break;
  default:
    panic("Tried to write DW_Uart at offset %#x that doesn't exist\n", daddr);
    break;
  }
  pkt->makeAtomicResponse();
  return pioDelay;
}

void DW_Uart::dataAvailable() {
  /*@todo ignore the fifo, just say we have data now
   * We might want to fix this, or we might not care */
  lsr |= 0x1;
  rawInt.rxim = 1;
  if (imsc.rxim) {
    DPRINTF(Uart, "Data available, scheduling interrupt\n");

    if (!intEvent.scheduled())
      schedule(intEvent, curTick() + intDelay);
  }
}

void DW_Uart::generateInterrupt() {
  DPRINTF(Uart, "Generate Interrupt: imsc=0x%x rawInt=0x%x iir=0x%x\n",
          imsc, rawInt, iir);

  if (rawInt.rxim && imsc.rxim) iir = 0x4;
  else if (rawInt.txim && imsc.txim) iir =0x2;

  gic->sendInt(intNum);
  DPRINTF(Uart, " -- Generated\n");
}

void DW_Uart::serialize(std::ostream &os) {
  DPRINTF(Checkpoint, "Serializing DesignWare Uart\n");
  SERIALIZE_SCALAR(control);
  SERIALIZE_SCALAR(fbrd);
  SERIALIZE_SCALAR(ibrd);
  SERIALIZE_SCALAR(lcr);
  SERIALIZE_SCALAR(iir);
  uint16_t imsc_serial = imsc;
  SERIALIZE_SCALAR(imsc_serial);

  uint16_t rawInt_serial = rawInt;
  SERIALIZE_SCALAR(rawInt_serial);

  SERIALIZE_SCALAR(endOnEOT);
  SERIALIZE_SCALAR(intDelay);
}

void DW_Uart::unserialize(Checkpoint *cp, const std::string &section) {
  DPRINTF(Checkpoint, "Unserializing DesignWare Uart\n");

  UNSERIALIZE_SCALAR(control);
  UNSERIALIZE_SCALAR(fbrd);
  UNSERIALIZE_SCALAR(ibrd);
  UNSERIALIZE_SCALAR(lcr);
  UNSERIALIZE_SCALAR(iir);

  uint16_t imsc_serial;
  UNSERIALIZE_SCALAR(imsc_serial);
  imsc = imsc_serial;

  uint16_t rawInt_serial;
  UNSERIALIZE_SCALAR(rawInt_serial);
  rawInt = rawInt_serial;

  UNSERIALIZE_SCALAR(endOnEOT);
  UNSERIALIZE_SCALAR(intDelay);
}

DW_Uart *DW_UartParams::create()
{
    return new DW_Uart(this);
}
