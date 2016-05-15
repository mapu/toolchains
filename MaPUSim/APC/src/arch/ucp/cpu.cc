/*
 * cpu.cc
 *
 *  Created on: Jun 27, 2013
 *      Author: wangl
 */

#include "base/trace.hh"
#include "cpu/inorder/cpu.hh"
#include "cpu/inorder/resource_pool.hh"
#include "cpu/inorder/resources/resource_list.hh"
#include "debug/MapuCSU.hh"
#include "params/TheCPU.hh"
#include "params/InOrderCPU.hh"

#include "cpu.hh"
#include "system.hh"

TheCPU::TheCPU(Params *p)
  : InOrderCPU((InOrderCPUParams *)p), sfetchPort(resPool->getInstUnit()),
    mfetchPort(resPool->getMInstUnit()), aguPort(resPool->getDataUnit()),
    biu0Port(resPool->getBiuUnit(0)), biu1Port(resPool->getBiuUnit(1)),
    biu2Port(resPool->getBiuUnit(2)), csuExtPort("csuext-port", this),
    csuIntPort("csuint-port", this), ctickEvent(this) {
}

TheCPU::~TheCPU() {
}

Fault TheCPU::read(DynInstPtr inst,
                       Addr addr,
                       uint8_t *data,
                       unsigned size,
                       unsigned flags) {
  if (inst->isMCode())
    return resPool->getBiuUnit(
        inst->mopClass() - Biu0MemOp)->read(inst, addr, data, size, flags);
  else
    return resPool->getDataUnit()->read(inst, addr, data, size, flags);
}

Fault TheCPU::write(DynInstPtr inst,
                        uint8_t *data,
                        unsigned size,
                        Addr addr,
                        unsigned flags,
                        uint64_t *write_res) {
  if (inst->isMCode())
    return resPool->getBiuUnit(
      inst->mopClass() - Biu0MemOp)->write(inst, data, size, addr, flags, write_res);
  else
    return resPool->getDataUnit()->write(inst, data, size, addr, flags, write_res);
}

MasterPort &
TheCPU::getMasterPort(const std::string &if_name, int idx) {
  // Get the right port based on name. This applies to all the
  // subclasses of the base CPU and relies on their implementation
  // of getDataPort and getInstPort. In all cases there methods
  // return a CpuPort pointer.
  if (if_name == "agu_port")
    return getDataPort();
  else if (if_name == "mfetch_port")
    return getMInstPort();
  else if (if_name == "sfetch_port")
    return getInstPort();
  else if (if_name == "csuext_port")
    return getCsuExtPort();
  else if (if_name == "csuint_port")
    return getCsuIntPort();
  else if (if_name == "biu0_port")
    return getBiuPort(0);
  else if (if_name == "biu1_port")
    return getBiuPort(1);
  else if (if_name == "biu2_port")
    return getBiuPort(2);
  else
    return MemObject::getMasterPort(if_name, idx);
}

TheCPU::CsuTickEvent::CsuTickEvent(TheCPU *_cpu)
  : Event(Sim_Exit_Pri + 1), cpu(_cpu) {
}

const char *
TheCPU::CsuTickEvent::description() const {
  return "InOrderCSU tick event";
}

void TheCPU::CsuTickEvent::process() {
  cpu->ctick();
}

void TheCPU::ctick() {
  // ctick should be only used in UcpSystem
  assert(MaPUSystem);
  UcpLiveProcess *p = dynamic_cast<UcpLiveProcess *>(threadContexts[0]->getProcessPtr());
  assert(p);
  CsuObject *csuObj = p->getCsuPtr();
  assert(csuObj);

  Tick nextsched = nextCycle(curTick() + 1);

  if (!csuObj->isAPEOn()) {
    if (system->numRunningContexts() == 0) {
      nextsched = curTick();
    } else if (numRunningContexts() != 0) {
      DPRINTF(MapuCSU, "APE is off. Turning SPU off if it's active...\n");
      if (threadContexts[0]->status() != ThreadContext::Halted)
        threadContexts[0]->halt(0);
    }
  }

  // check if need to start APE
  if (csuObj->isAPEOn()) {
    if (threadContexts[0]->status() == ThreadContext::Halted) {
      DPRINTF(MapuCSU, "APE is on. Turning SPU on if it's halted...\n");
      threadContexts[0]->pcState(0);
      //cleanUpRemovedInsts();
      threadContexts[0]->activate(0);
    }
  }

  if (csuObj->isCSUBusy()) {
    DPRINTF(MapuCSU, "CSU received a transmission request from ARM.\n");
    PortProxy localProxy(getCsuIntPort());
    PortProxy globalProxy(getCsuExtPort());
    csuObj->processFrontDMA(cpu_id, globalProxy, localProxy, true);
  }

  // Re-schedule CSU scheduler for this cycle
  if (!dynamic_cast<UcpSystem *>(system)->csuSkedEvent.scheduled())
    schedule(dynamic_cast<UcpSystem *>(system)->csuSkedEvent, nextsched);

  csuObj->update(cpu_id);
}

TheCPU *
TheCPUParams::create() {
  return new TheCPU(this);
}
