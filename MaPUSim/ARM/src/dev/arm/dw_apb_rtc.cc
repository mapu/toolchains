/*
 * dw_apb_rtc.cc
 *
 *  Created on: Mar 20, 2014
 *      Author: wangl
 */

#include "base/intmath.hh"
#include "base/time.hh"
#include "base/trace.hh"
#include "debug/Checkpoint.hh"
#include "debug/Timer.hh"
#include "dev/arm/amba_device.hh"
#include "dev/arm/dw_apb_rtc.hh"
#include "dev/mc146818.hh"
#include "mem/packet.hh"
#include "mem/packet_access.hh"

DWrtc::DWrtc(Params *p) :
  AmbaIntDevice(p, 0xfff), timeVal(mkutctime(&p->time)),
  lastWrittenTick(0), loadVal(0), matchVal(0),
  rawInt(false), maskInt(false), pendingInt(false), matchEvent(this) {}

Tick DWrtc::read(PacketPtr pkt) {
  assert(pkt->getAddr() >= pioAddr && pkt->getAddr() < pioAddr + pioSize);
  assert(pkt->getSize() == 4);
  Addr daddr = pkt->getAddr() - pioAddr;
  pkt->allocate();
  uint32_t data;

  DPRINTF(Timer, "Reading from RTC at offset: %#x\n", daddr);

  switch (daddr) {
  case DataReg:
    data = timeVal + ((curTick() - lastWrittenTick) / SimClock::Int::s);
    break;
  case MatchReg:
    data = matchVal;
    break;
  case LoadReg:
    data = loadVal;
    break;
  case ControlReg:
    data = uint32_t(maskInt) << 1 | enableInt; // Always enabled otherwise there is no point
    break;
  case RawISR:
    data = rawInt;
    break;
  case MaskedISR:
    data = pendingInt;
    break;
  case IntClear:
    rawInt = false;
    pendingInt = false;
    data = 0;
    gic->clearInt(intNum);
    break;
  default:
    if (readId(pkt, ambaId, pioAddr)) {
      // Hack for variable sized access
      data = pkt->get<uint32_t>();
      break;
    }
    panic("Tried to read DWrtc at offset %#x that doesn't exist\n", daddr);
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
    panic("Uart read size too big?\n");
    break;
  }

  pkt->makeAtomicResponse();
  return pioDelay;
}

Tick DWrtc::write(PacketPtr pkt) {
  assert(pkt->getAddr() >= pioAddr && pkt->getAddr() < pioAddr + pioSize);
  assert(pkt->getSize() == 4);
  Addr daddr = pkt->getAddr() - pioAddr;
  pkt->allocate();
  DPRINTF(Timer, "Writing to RTC at offset: %#x\n", daddr);

  switch (daddr) {
  case MatchReg:
    matchVal = pkt->get<uint32_t>();
    resyncMatch();
    break;
  case LoadReg:
    lastWrittenTick = curTick();
    timeVal = pkt->get<uint32_t>();
    loadVal = timeVal;
    resyncMatch();
    break;
  case ControlReg:
    maskInt = pkt->get<uint32_t>() & 0x2;
    enableInt = pkt->get<uint32_t>() & 0x1;
    rawInt = rawInt & enableInt;
    pendingInt = ~maskInt & rawInt;
    if (pendingInt) gic->sendInt(intNum);
    else gic->clearInt(intNum);
    break; // Can't stop when started
  default:
    if (readId(pkt, ambaId, pioAddr)) break;
    panic("Tried to read DWrtc at offset %#x that doesn't exist\n", daddr);
    break;
  }

  pkt->makeAtomicResponse();
  return pioDelay;
}

void DWrtc::resyncMatch() {
  DPRINTF(Timer, "Setting up new match event match=%d time=%d\n", matchVal,
          timeVal);

  uint32_t seconds_until = matchVal - timeVal;
  Tick ticks_until = SimClock::Int::s * seconds_until;

  if (matchEvent.scheduled()) {
    DPRINTF(Timer, "-- Event was already schedule, de-scheduling\n");
    deschedule(matchEvent);
  }
  schedule(matchEvent, curTick() + ticks_until);
  DPRINTF(Timer, "-- Scheduling new event for: %d\n", curTick() + ticks_until);
}

void DWrtc::counterMatch() {
  DPRINTF(Timer, "Counter reached zero\n");

  rawInt = true & enableInt;
  pendingInt = ~maskInt & rawInt;
  if (pendingInt) {
    DPRINTF(Timer, "-- Causing interrupt\n");
    gic->sendInt(intNum);
  }
}

void DWrtc::serialize(std::ostream &os) {
  DPRINTF(Checkpoint, "Serializing Arm DWrtc\n");
  SERIALIZE_SCALAR(timeVal);
  SERIALIZE_SCALAR(lastWrittenTick);
  SERIALIZE_SCALAR(loadVal);
  SERIALIZE_SCALAR(matchVal);
  SERIALIZE_SCALAR(rawInt);
  SERIALIZE_SCALAR(maskInt);
  SERIALIZE_SCALAR(pendingInt);

  bool is_in_event = matchEvent.scheduled();
  SERIALIZE_SCALAR(is_in_event);

  Tick event_time;
  if (is_in_event) {
    event_time = matchEvent.when();
    SERIALIZE_SCALAR(event_time);
  }
}

void DWrtc::unserialize(Checkpoint *cp, const std::string &section) {
  DPRINTF(Checkpoint, "Unserializing Arm DWrtc\n");

  UNSERIALIZE_SCALAR(timeVal);
  UNSERIALIZE_SCALAR(lastWrittenTick);
  UNSERIALIZE_SCALAR(loadVal);
  UNSERIALIZE_SCALAR(matchVal);
  UNSERIALIZE_SCALAR(rawInt);
  UNSERIALIZE_SCALAR(maskInt);
  UNSERIALIZE_SCALAR(pendingInt);

  bool is_in_event;
  UNSERIALIZE_SCALAR(is_in_event);

  Tick event_time;
  if (is_in_event) {
    UNSERIALIZE_SCALAR(event_time);
    schedule(matchEvent, event_time);
  }
}

DWrtc *DWrtcParams::create() {
  return new DWrtc(this);
}

