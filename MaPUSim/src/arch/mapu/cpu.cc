#include "arch/mapu/cpu.hh"
#include "arch/mapu/system.hh"
#include "base/trace.hh"
#include "cpu/inorder/cpu.hh"
#include "cpu/inorder/resource_pool.hh"
#include "cpu/inorder/resources/resource_list.hh"
#include "debug/MapuCSU.hh"
#include "params/MapuCPU.hh"
#include "params/InOrderCPU.hh"

MapuCPU::MapuCPU(Params *p)
  : InOrderCPU((InOrderCPUParams *)p), sfetchPort(resPool->getInstUnit()),
    mfetchPort(resPool->getMInstUnit()), aguPort(resPool->getDataUnit()),
    biuPort(resPool->getBiuUnit()), csuExtPort("csuext-port", this),
    csuIntPort("csuint-port", this), turboDecPort("turbo-port", this),
    ctickEvent(this) {
}

MapuCPU::~MapuCPU() {
}

Fault MapuCPU::read(DynInstPtr inst,
                       Addr addr,
                       uint8_t *data,
                       unsigned size,
                       unsigned flags) {
  if (inst->isMCode())
    return resPool->getBiuUnit()->read(inst, addr, data, size, flags);
  else
    return resPool->getDataUnit()->read(inst, addr, data, size, flags);
}

Fault MapuCPU::write(DynInstPtr inst,
                        uint8_t *data,
                        unsigned size,
                        Addr addr,
                        unsigned flags,
                        uint64_t *write_res) {
  if (inst->isMCode())
    return resPool->getBiuUnit()->write(inst, data, size, addr, flags, write_res);
  else
    return resPool->getDataUnit()->write(inst, data, size, addr, flags, write_res);
}

MasterPort &
MapuCPU::getMasterPort(const std::string &if_name, int idx) {
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
  else if (if_name == "biu_port")
    return getBiuPort();
  else if (if_name == "turbo_port")
    return getTurboDecPort();
  else
    return MemObject::getMasterPort(if_name, idx);
}

MapuCPU::CsuTickEvent::CsuTickEvent(MapuCPU *_cpu)
  : Event(Sim_Exit_Pri + 1), cpu(_cpu) {
}

const char *
MapuCPU::CsuTickEvent::description() const {
  return "InOrderCSU tick event";
}

void MapuCPU::CsuTickEvent::process() {
  cpu->ctick();
}

void MapuCPU::ctick() {
  // ctick should be only used in MapuSystem
  assert(MaPUSystem);
  LiveProcess *p = dynamic_cast<LiveProcess *>(threadContexts[0]->getProcessPtr());
  assert(p);
  CsuObject *csuObj = dynamic_cast<CsuObject *>(p->getObjFilePtr());
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
  if (!dynamic_cast<MapuSystem *>(system)->csuSkedEvent.scheduled())
    schedule(dynamic_cast<MapuSystem *>(system)->csuSkedEvent, nextsched);

  csuObj->update(cpu_id);
}

MapuCPU *
MapuCPUParams::create() {
  return new MapuCPU(this);
}
