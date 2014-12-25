/*
 * dw_aph_ictl.cc
 *
 *  Created on: Mar 6, 2014
 *      Author: wangl
 */

#include "base/trace.hh"
#include "debug/Checkpoint.hh"
#include "debug/GIC.hh"
#include "debug/IPI.hh"
#include "debug/Interrupt.hh"
#include "dev/arm/dw_apb_ictl.hh"
#include "dev/arm/realview.hh"
#include "dev/terminal.hh"
#include "mem/packet.hh"
#include "mem/packet_access.hh"

DW_Ictl::DW_Ictl(const Params *p) :
  BaseGic(p), reg_mmap(), pioAddr(p->pio_addr), pioDelay(p->pio_delay),
  intLatency(p->int_latency), postIntEvent(p->platform) {
  pioSize = 0xFFFF;
  RealView *rv = dynamic_cast<RealView*>(p->platform);
  assert(rv);
  rv->setGic(this);
}

Tick DW_Ictl::read(PacketPtr pkt) {
  Addr daddr = pkt->getAddr() - pioAddr;
  pkt->allocate();
  DPRINTF(GIC, "gic cpu read register %#x\n", daddr);
  if (pkt->getSize() == 8) {
    switch (daddr) {
    case IRQ_INTEN_L:
      pkt->set<uint64_t>(reg_mmap.irq_en);
      break;
    case IRQ_INTMASK_L:
      pkt->set<uint64_t>(reg_mmap.irq_mask);
      break;
    case IRQ_FORCE_L:
      pkt->set<uint64_t>(reg_mmap.irq_force);
      break;
    case IRQ_RAWSTATUS_L:
      pkt->set<uint64_t>(reg_mmap.irq_raw);
      break;
    case IRQ_STATUS_L:
      pkt->set<uint64_t>(reg_mmap.irq_status);
      break;
    case IRQ_MASKSTATUS_L:
      pkt->set<uint64_t>(reg_mmap.irq_mask_status);
      break;
    case IRQ_FINALSTATUS_L:
      pkt->set<uint64_t>(reg_mmap.irq_final_status);
      break;
    default:
      if (daddr <= IRQ_VECTOR_15) {
        pkt->set<uint64_t>(reg_mmap.irq_vectors[(daddr - IRQ_VECTOR_0) / 8]);
      } else if (daddr >= IRQ_PR_START) {
        pkt->set<uint64_t>(reg_mmap.irq_pri[(daddr - IRQ_PR_START) / 4]);
      } else {
        panic("Unknown register at 0x%x in DW_Ictrl.", daddr);
      }
      break;
    }
  } else {
    switch (daddr) {
    case IRQ_INTEN_L:
      pkt->set<uint32_t>(reg_mmap.irq_en);
      break;
    case IRQ_INTEN_H:
      pkt->set<uint32_t>(reg_mmap.irq_en >> 32);
      break;
    case IRQ_INTMASK_L:
      pkt->set<uint32_t>(reg_mmap.irq_mask);
      break;
    case IRQ_INTMASK_H:
      pkt->set<uint32_t>(reg_mmap.irq_mask >> 32);
      break;
    case IRQ_FORCE_L:
      pkt->set<uint32_t>(reg_mmap.irq_force);
      break;
    case IRQ_FORCE_H:
      pkt->set<uint32_t>(reg_mmap.irq_force >> 32);
      break;
    case IRQ_RAWSTATUS_L:
      pkt->set<uint32_t>(reg_mmap.irq_raw);
      break;
    case IRQ_RAWSTATUS_H:
      pkt->set<uint32_t>(reg_mmap.irq_raw >> 32);
      break;
    case IRQ_STATUS_L:
      pkt->set<uint32_t>(reg_mmap.irq_status);
      break;
    case IRQ_STATUS_H:
      pkt->set<uint32_t>(reg_mmap.irq_status >> 32);
      break;
    case IRQ_MASKSTATUS_L:
      pkt->set<uint32_t>(reg_mmap.irq_mask_status);
      break;
    case IRQ_MASKSTATUS_H:
      pkt->set<uint32_t>(reg_mmap.irq_mask_status >> 32);
      break;
    case IRQ_FINALSTATUS_L:
      pkt->set<uint32_t>(reg_mmap.irq_final_status);
      break;
    case IRQ_FINALSTATUS_H:
      pkt->set<uint32_t>(reg_mmap.irq_final_status >> 32);
      break;
    case IRQ_VECTOR:
      pkt->set<uint32_t>(reg_mmap.irq_vector);
      break;
    case IRQ_PLEVEL:
      pkt->set<uint32_t>(reg_mmap.irq_pri_level);
      break;
    case IRQ_VECTOR_DEFAULT:
      pkt->set<uint32_t>(reg_mmap.irq_vector_default);
      break;
    default:
      if (daddr <= IRQ_VECTOR_15) {
        pkt->set<uint32_t>(reg_mmap.irq_vectors[(daddr - IRQ_VECTOR_0) / 8]);
      } else if (daddr >= IRQ_PR_START) {
        pkt->set<uint32_t>(reg_mmap.irq_pri[(daddr - IRQ_PR_START) / 4]);
      } else {
        panic("Unknown register at 0x%x in DW_Ictrl.", daddr);
      }
      break;
    }
  }
  pkt->makeAtomicResponse();
  return pioDelay;
}

Tick DW_Ictl::write(PacketPtr pkt) {
  Addr daddr = pkt->getAddr() - pioAddr;
  pkt->allocate();
  DPRINTF(GIC, "gic distributor write register %#x size %#x value %#x \n",
          daddr, pkt->getSize(), pkt->get<uint32_t>());
  if (pkt->getSize() == 8) {
    switch (daddr) {
    case IRQ_INTEN_L:
      reg_mmap.irq_en = pkt->get<uint64_t>();
      break;
    case IRQ_INTMASK_L:
      reg_mmap.irq_mask = pkt->get<uint64_t>();
      break;
    case IRQ_FORCE_L:
      reg_mmap.irq_force = pkt->get<uint64_t>();
      break;
    case IRQ_PLEVEL:
      reg_mmap.irq_pri_level = pkt->get<uint32_t>();
      break;
    case IRQ_VECTOR_DEFAULT:
      reg_mmap.irq_vector_default = pkt->get<uint32_t>();
      break;
    }
  } else {
    switch (daddr) {
    case IRQ_INTEN_L:
      *(uint32_t *)&(reg_mmap.irq_en) = pkt->get<uint32_t>();
      break;
    case IRQ_INTEN_H:
      *((uint32_t *)&(reg_mmap.irq_en) + 1) = pkt->get<uint32_t>();
      break;
    case IRQ_INTMASK_L:
      *(uint32_t *)&(reg_mmap.irq_mask) = pkt->get<uint32_t>();
      break;
    case IRQ_INTMASK_H:
      *((uint32_t *)&(reg_mmap.irq_mask) + 1) = pkt->get<uint32_t>();
      break;
    case IRQ_FORCE_L:
      *(uint32_t *)&(reg_mmap.irq_force) = pkt->get<uint32_t>();
      break;
    case IRQ_FORCE_H:
      *((uint32_t *)&(reg_mmap.irq_force) + 1) = pkt->get<uint32_t>();
      break;
    case IRQ_PLEVEL:
      reg_mmap.irq_pri_level = pkt->get<uint32_t>();
      break;
    case IRQ_VECTOR_DEFAULT:
      reg_mmap.irq_vector_default = pkt->get<uint32_t>();
      break;
    }
  }
  updateIntState();
  pkt->makeAtomicResponse();
  return pioDelay;
}

void DW_Ictl::updateIntState() {
  reg_mmap.irq_status = (reg_mmap.irq_raw | reg_mmap.irq_force) &
                        reg_mmap.irq_en;
  reg_mmap.irq_mask_status = reg_mmap.irq_status & ~reg_mmap.irq_mask;
  uint64_t pri_mask = 0;
  uint64_t vector_pri = 0;
  for (unsigned i = 0; i < 64; i++) {
    if (reg_mmap.irq_pri[i] >= reg_mmap.irq_pri_level) {
      pri_mask |= 1 << i;
      if (reg_mmap.irq_pri[i] > vector_pri)
        vector_pri = reg_mmap.irq_pri[i];
    }
  }
  reg_mmap.irq_final_status = reg_mmap.irq_mask_status & pri_mask;
  if (reg_mmap.irq_final_status) {
    reg_mmap.irq_vector = reg_mmap.irq_vectors[vector_pri];
    postInt(curTick() + intLatency);
  } else {
    platform->intrctrl->clear(ArmISA::INT_IRQ, 0);
    reg_mmap.irq_vector = reg_mmap.irq_vector_default;
  }
}

void DW_Ictl::postInt(Tick when) {
  DPRINTF(GIC, "Assert interrupt %#016llx to ARM at %d\n", reg_mmap.irq_final_status, when);
  if (!(postIntEvent.scheduled()))
    eventq->schedule(&postIntEvent, when);
}

void DW_Ictl::sendInt(uint32_t num) {
  DPRINTF(Interrupt, "Received Int %d\n", num);
  reg_mmap.irq_raw |= 1 << num;
  updateIntState();
}

void DW_Ictl::clearInt(uint32_t num) {
  DPRINTF(Interrupt, "Received Int clear %d\n", num);
  reg_mmap.irq_raw &= ~(1 << num);
  updateIntState();
}

void DW_Ictl::sendPPInt(uint32_t num, uint32_t cpu) {
  DPRINTF(Interrupt, "Received PPI %d\n", num);
  reg_mmap.irq_raw |= 1 << num;
  updateIntState();
}

void DW_Ictl::clearPPInt(uint32_t num, uint32_t cpu) {
  DPRINTF(Interrupt, "Received PPI clear %d\n", num);
  reg_mmap.irq_raw &= ~(1 << num);
  updateIntState();
}

void DW_Ictl::serialize(std::ostream &os) {
  DPRINTF(Checkpoint, "Serializing Arm GIC\n");
}

void DW_Ictl::unserialize(Checkpoint *cp, const std::string &section) {
  DPRINTF(Checkpoint, "Unserializing Arm GIC\n");
}

AddrRangeList DW_Ictl::getAddrRanges() const {
  AddrRangeList ranges;
  ranges.push_back(RangeSize(pioAddr, pioSize));
  return ranges;
}

DW_Ictl *DW_IctlParams::create() {
  return new DW_Ictl(this);
}
