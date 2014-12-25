/*
 * dw_apb_timers.cc
 *
 *  Created on: Mar 20, 2014
 *      Author: wangl
 */

#include "base/intmath.hh"
#include "base/trace.hh"
#include "debug/Checkpoint.hh"
#include "debug/Timer.hh"
#include "dev/arm/base_gic.hh"
#include "dev/arm/dw_apb_timers.hh"
#include "mem/packet.hh"
#include "mem/packet_access.hh"

DWTimers::DWTimers(Params *p) :
    AmbaPioDevice(p, 0xfff), gic(p->gic) {
  timer[0] = new Timer(name() + ".timer0", this, p->int_num0, p->clock0);
  timer[1] = new Timer(name() + ".timer1", this, p->int_num1, p->clock1);
  timer[2] = new Timer(name() + ".timer2", this, p->int_num2, p->clock2);
  timer[3] = new Timer(name() + ".timer3", this, p->int_num3, p->clock3);
  timer[4] = new Timer(name() + ".timer4", this, p->int_num4, p->clock4);
  timer[5] = new Timer(name() + ".timer5", this, p->int_num5, p->clock5);
  timer[6] = new Timer(name() + ".timer6", this, p->int_num6, p->clock6);
  timer[7] = new Timer(name() + ".timer7", this, p->int_num7, p->clock7);
}

DWTimers::~DWTimers() {
  for (unsigned i = 0; i < 8; i++) {
    if (timer[i]) delete timer[i];
  }
}

DWTimers::Timer::Timer(std::string __name, DWTimers *_parent, int int_num,
                       Tick _clock) :
    _name(__name), parent(_parent), intNum(int_num), clock(_clock),
    control(0x0), rawInt(false), pendingInt(false), loadValue(0),
    zeroEvent(this) {}

Tick DWTimers::read(PacketPtr pkt) {
  assert(pkt->getAddr() >= pioAddr && pkt->getAddr() < pioAddr + pioSize);
  assert(pkt->getSize() == 4);
  Addr daddr = pkt->getAddr() - pioAddr;
  pkt->allocate();
  DPRINTF(Timer, "Reading from DWTimer at offset: %#x\n", daddr);

  uint32_t data = 0;
  if (daddr < Timer::Size * 8)
    timer[daddr / Timer::Size]->read(pkt, daddr % Timer::Size);
  else if (daddr <= 0xAC) {
    switch (daddr) {
    case 0xA0:
      for (unsigned i = 0; i < 8; i++) {
        data |= timer[i]->getMaskedInt() << i;
      }
      break;

    case 0xA4:
      for (unsigned i = 0; i < 8; i++) {
        timer[i]->clearInt();
      }
      break;

    case 0xA8:
      for (unsigned i = 0; i < 8; i++) {
        data |= timer[i]->getRawInt() << i;
      }
      break;

    default:
      panic("Tried to read DWTimers at offset %#x that doesn't exist\n", daddr);
      break;
    }
    pkt->set<uint32_t>(data);
  } else if (!readId(pkt, ambaId, pioAddr))
    panic("Tried to read DWTimers at offset %#x that doesn't exist\n", daddr);

  pkt->makeAtomicResponse();
  return pioDelay;
}

void DWTimers::Timer::read(PacketPtr pkt, Addr daddr) {
  switch (daddr) {
  case LoadReg:
    pkt->set<uint32_t>(loadValue);
    break;
  case CurrentReg:
    DPRINTF(Timer, "Event schedule for %d, clock=%d\n",
            zeroEvent.when(), clock);
    Tick time;
    time = zeroEvent.when() - curTick();
    time = time / clock;
    DPRINTF(Timer, "-- returning counter at %d\n", time);
    pkt->set<uint32_t>(time);
    break;
  case ControlReg:
    pkt->set<uint32_t>(control);
    break;
  case IntClear:
    pkt->set<uint32_t>(0);
    clearInt();
    parent->gic->clearInt(intNum);
    //parent->updateInt();
    break;
  case MaskedISR:
    pkt->set<uint32_t>(pendingInt);
    break;
  default:
    panic("Tried to read DW timer at offset %#x\n", daddr);
    break;
  }
  DPRINTF(Timer, "Reading %#x from Timer at offset: %#x\n",
          pkt->get<uint32_t>(), daddr);
}

Tick DWTimers::write(PacketPtr pkt) {
  assert(pkt->getAddr() >= pioAddr && pkt->getAddr() < pioAddr + pioSize);
  assert(pkt->getSize() == 4);
  Addr daddr = pkt->getAddr() - pioAddr;
  pkt->allocate();
  DPRINTF(Timer, "Writing to DWTimer at offset: %#x\n", daddr);

  if (daddr < Timer::Size * 8)
    timer[daddr / Timer::Size]->write(pkt, daddr % Timer::Size);
  else if (!readId(pkt, ambaId, pioAddr))
    panic("Tried to write DWTimers at offset %#x that doesn't exist\n", daddr);
  pkt->makeAtomicResponse();
  return pioDelay;
}

void DWTimers::Timer::write(PacketPtr pkt, Addr daddr) {
  DPRINTF(Timer, "Writing %#x to Timer at offset: %#x\n", pkt->get<uint32_t>(),
          daddr);
  switch (daddr) {
  case LoadReg:
    loadValue = pkt->get<uint32_t>();
    restartCounter(loadValue);
    break;
  case ControlReg:
    bool old_enable;
    old_enable = control.timerEnable;
    control = pkt->get<uint32_t>();
    if ((old_enable == 0) && control.timerEnable)
      restartCounter(loadValue);
    rawInt &= control.timerEnable;
    pendingInt &= control.intEnable;
    if (pendingInt) parent->gic->sendInt(intNum);
    else parent->gic->clearInt(intNum);
    break;
  default:
    panic("Tried to write DW timer at offset %#x\n", daddr);
    break;
  }
}
/*
void DWTimers::updateInt() {
  uint32_t data = 0;
  for (unsigned i = 0; i < 8; i++)
    data |= timer[i]->getMaskedInt() << i;
  if (data == 0) gic->clearInt(timer[0].intNum);
}
*/
void DWTimers::Timer::restartCounter(uint32_t val) {
  DPRINTF(Timer, "Resetting counter with value %#x\n", val);
  if (!control.timerEnable) return;

  Tick time = clock;
  time *= val;

  if (zeroEvent.scheduled()) {
    DPRINTF(Timer, "-- Event was already schedule, de-scheduling\n");
    parent->deschedule(zeroEvent);
  }
  DPRINTF(Timer, "-- Scheduling new event for: %d\n", curTick() + time);
  parent->schedule(zeroEvent, curTick() + time);
}

void DWTimers::Timer::counterAtZero() {
  if (!control.timerEnable) return;

  DPRINTF(Timer, "Counter reached zero\n");

  rawInt = true;
  if (!control.intEnable) pendingInt = true;
  if (pendingInt) {
    DPRINTF(Timer, "-- Causing interrupt\n");
    parent->gic->sendInt(intNum);
  }

  // Free-running
  if (control.timerMode == 0) restartCounter(0xffffffff);
  else restartCounter(loadValue);
}

void DWTimers::Timer::serialize(std::ostream &os) {
  DPRINTF(Checkpoint, "Serializing Arm DW Timer\n");

  uint32_t control_serial = control;
  SERIALIZE_SCALAR(control_serial);

  SERIALIZE_SCALAR(rawInt);
  SERIALIZE_SCALAR(pendingInt);
  SERIALIZE_SCALAR(loadValue);

  bool is_in_event = zeroEvent.scheduled();
  SERIALIZE_SCALAR(is_in_event);

  Tick event_time;
  if (is_in_event) {
    event_time = zeroEvent.when();
    SERIALIZE_SCALAR(event_time);
  }
}

void DWTimers::Timer::unserialize(Checkpoint *cp, const std::string &section) {
  DPRINTF(Checkpoint, "Unserializing Arm DW Timer\n");

  uint32_t control_serial;
  UNSERIALIZE_SCALAR(control_serial);
  control = control_serial;

  UNSERIALIZE_SCALAR(rawInt);
  UNSERIALIZE_SCALAR(pendingInt);
  UNSERIALIZE_SCALAR(loadValue);

  bool is_in_event;
  UNSERIALIZE_SCALAR(is_in_event);

  Tick event_time;
  if (is_in_event) {
    UNSERIALIZE_SCALAR(event_time);
    parent->schedule(zeroEvent, event_time);
  }
}

void DWTimers::serialize(std::ostream &os) {
  nameOut(os, csprintf("%s.timer0", name()));
  timer[0]->serialize(os);
  nameOut(os, csprintf("%s.timer1", name()));
  timer[1]->serialize(os);
  nameOut(os, csprintf("%s.timer2", name()));
  timer[2]->serialize(os);
  nameOut(os, csprintf("%s.timer3", name()));
  timer[3]->serialize(os);
  nameOut(os, csprintf("%s.timer4", name()));
  timer[4]->serialize(os);
  nameOut(os, csprintf("%s.timer5", name()));
  timer[5]->serialize(os);
  nameOut(os, csprintf("%s.timer6", name()));
  timer[6]->serialize(os);
  nameOut(os, csprintf("%s.timer7", name()));
  timer[7]->serialize(os);
}

void DWTimers::unserialize(Checkpoint *cp, const std::string &section) {
  timer[0]->unserialize(cp, csprintf("%s.timer0", section));
  timer[1]->unserialize(cp, csprintf("%s.timer1", section));
  timer[2]->unserialize(cp, csprintf("%s.timer2", section));
  timer[3]->unserialize(cp, csprintf("%s.timer3", section));
  timer[4]->unserialize(cp, csprintf("%s.timer4", section));
  timer[5]->unserialize(cp, csprintf("%s.timer5", section));
  timer[6]->unserialize(cp, csprintf("%s.timer6", section));
  timer[7]->unserialize(cp, csprintf("%s.timer7", section));
}

DWTimers *DWTimersParams::create() {
  return new DWTimers(this);
}

