/*
 * cop.cc
 *
 *  Created on: Apr 17, 2014
 *      Author: wangl
 */


#include "arch/mapu/process.hh"
#include "base/trace.hh"
#include "debug/BusAddrRanges.hh"

#include "cpu.hh"
#include "cop.hh"

CoPPort::CoPPort(CoProcessor *cp) :
  SimpleTimingPort(cp->name() + "-cop", cp), cop(cp) {
}

Tick CoPPort::recvAtomic(PacketPtr pkt) {
  return pkt->isRead() ? cop->read(pkt) : cop->write(pkt);
}

AddrRangeList CoPPort::getAddrRanges() {
  return cop->getAddrRanges();
}

CoProcessor::CoProcessor(const Params *p) :
  MemObject(p), cpu(p->cpu), copPort(this), copAddr(p->cop_addr),
  copSize(p->cop_size), copDelay(p->cop_latency) {}

CoProcessor::~CoProcessor() {}

void CoProcessor::init() {
  if (!copPort.isConnected())
    panic("CoP port of %s not connected to anything!", name());
  copPort.sendRangeChange();
}

SlavePort &CoProcessor::getSlavePort(const std::string &if_name, int idx) {
  if (if_name == "cop") {
    return copPort;
  }
  return MemObject::getSlavePort(if_name, idx);
}

unsigned int CoProcessor::drain(Event *de) {
  unsigned int count;
  count = copPort.drain(de);
  if (count)
    changeState(Draining);
  else
    changeState(Drained);
  return count;
}

AddrRangeList CoProcessor::getAddrRanges() {
  assert(copSize != 0);
  AddrRangeList ranges;
  DPRINTF(BusAddrRanges, "registering range: %#x-%#x\n", copAddr, copSize);
  ranges.push_back(RangeSize(copAddr, copSize));
  return ranges;
}

void CoProcessor::sendEvent(enum CsuObject::CSU_EVENT e) {
  MapuLiveProcess *p =
    dynamic_cast<MapuLiveProcess *>(
      cpu->getContext(1 - ThePipeline::MaxSThreads)->getProcessPtr());
  p->getCsuPtr()->sendEvent(e);
}

void CoProcessor::clearEvent(enum CsuObject::CSU_EVENT e) {
  MapuLiveProcess *p =
    dynamic_cast<MapuLiveProcess *>(
      cpu->getContext(1 - ThePipeline::MaxSThreads)->getProcessPtr());
  p->getCsuPtr()->clearEvent(e);
}
