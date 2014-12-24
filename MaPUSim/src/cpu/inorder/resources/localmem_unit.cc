/*
 * Copyright (c) 2007 MIPS Technologies, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer;
 * redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution;
 * neither the name of the copyright holders nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Korey Sewell
 *
 */

#include <list>
#include <vector>

#include "arch/isa_traits.hh"
#include "arch/mapu/cpu.hh"
#include "arch/locked_mem.hh"
#include "arch/utility.hh"
#include "config/the_isa.hh"
#include "cpu/inorder/resources/localmem_unit.hh"
#include "cpu/inorder/cpu.hh"
#include "cpu/inorder/pipeline_traits.hh"
#include "cpu/inorder/resource_pool.hh"
#include "debug/Activity.hh"
#include "debug/AddrDep.hh"
#include "debug/InOrderLocalMemPort.hh"
#include "debug/InOrderStall.hh"
#include "debug/InOrderTLB.hh"
#include "debug/LLSC.hh"
#include "debug/RefCount.hh"
#include "debug/ThreadModel.hh"
#include "mem/request.hh"

using namespace std;
using namespace TheISA;
using namespace ThePipeline;

#if TRACING_ON
static std::string
printMemData(uint8_t *data, unsigned size)
{
  std::stringstream dataStr;
  for (unsigned pos = 0; pos < size; pos++) {
    ccprintf(dataStr, "%02x", data[pos]);
  }
  return dataStr.str();
}
#endif

LocalMemUnit::LocalMemUnit(string res_name, int res_id, int res_width,
                           int res_latency, InOrderCPU *_cpu,
                           ThePipeline::Params *params)
    : Resource(res_name, res_id, res_width, res_latency, _cpu),
      localmemPort(NULL), localmemPortBlocked(false), causedbySorM(false) {
  // Hard-Code Selection For Now
  if (res_id == ICache) _tlb = params->itb;
  else if (res_id == MICache) _tlb = params->itb;
  else if (res_id == DCache) _tlb = params->dtb;
  else _tlb = params->dtb;

  memset(accessTick, 0, sizeof(accessTick));
  //fatal("Unrecognized TLB name passed by user");

  // Note that the CPU port is not yet instantiated (as it is done
  // after the resource pool), we delay setting the localmemPort
  // pointer until init().
}

TheISA::TLB*
LocalMemUnit::tlb() {
  return _tlb;

}

void LocalMemUnit::init() {
  // Get the appropriate port from the CPU based on the resource name.
  if (id == ICache) {
    localmemPort = &cpu->getInstPort();
  } else if (id == MICache) {
    localmemPort = &cpu->getMInstPort();
  } else if (id == DCache) {
    localmemPort = &cpu->getDataPort();
  } else if (id == BiuUnit) {
    localmemPort = &dynamic_cast<MapuCPU *>(cpu)->getBiuPort();
  }
  assert(localmemPort != NULL);

  for (int i = 0; i < width; i++) {
    reqs[i] = new LocalMemRequest(this);
  }

  resourceEvent = new ResourceEvent[width];

  //localmemBlkSize = localmemPort->peerBlockSize();
  //localmemBlkMask = localmemBlkSize - 1;

  initSlots();
}

int LocalMemUnit::getSlot(DynInstPtr inst) {
  if (!inst->validMemAddr()) {
    panic("[tid:%i][sn:%i] Mem. Addr. must be set before requesting "
          "localmem access\n",
          inst->readTid(), inst->seqNum);
  }

  int new_slot = Resource::getSlot(inst);
  inst->memTime = curTick();
  //@note: add back in if you want speculative loads/store capability
  return new_slot;
}

ResReqPtr LocalMemUnit::findRequest(DynInstPtr inst) {
  for (int i = lastReqIdx; i < width; i++) {
    LocalMemRequest* localmem_req = dynamic_cast<LocalMemRequest*>(reqs[i]);
    assert(localmem_req);

    if (localmem_req->valid && localmem_req->getInst() == inst
        && localmem_req->instIdx == inst->curSkedEntry->idx) {
      lastReqIdx = i;
      return localmem_req;
    }
  }
  for (int i = 0; i < lastReqIdx; i++) {
    LocalMemRequest* localmem_req = dynamic_cast<LocalMemRequest*>(reqs[i]);
    assert(localmem_req);

    if (localmem_req->valid && localmem_req->getInst() == inst
        && localmem_req->instIdx == inst->curSkedEntry->idx) {
      lastReqIdx = i;
      return localmem_req;
    }
  }

  return NULL;
}

ResReqPtr LocalMemUnit::findRequest(DynInstPtr inst, int idx) {
  for (int i = lastReqIdx; i < width; i++) {
    LocalMemRequest* localmem_req = dynamic_cast<LocalMemRequest*>(reqs[i]);
    assert(localmem_req);

    if (localmem_req->valid && localmem_req->getInst() == inst
        && localmem_req->instIdx == idx) {
      lastReqIdx = i;
      return localmem_req;
    }
  }
  for (int i = 0; i < lastReqIdx; i++) {
    LocalMemRequest* localmem_req = dynamic_cast<LocalMemRequest*>(reqs[i]);
    assert(localmem_req);

    if (localmem_req->valid && localmem_req->getInst() == inst
        && localmem_req->instIdx == idx) {
      lastReqIdx = i;
      return localmem_req;
    }
  }

  return NULL;
}

ResReqPtr LocalMemUnit::getRequest(DynInstPtr inst, int stage_num, int res_idx,
                                   int slot_num, unsigned cmd) {
  ScheduleEntry* sched_entry = *inst->curSkedEntry;
  LocalMemRequest* localmem_req = dynamic_cast<LocalMemRequest*>(reqs[slot_num]);

  if (!inst->validMemAddr()) {
    panic("Mem. Addr. must be set before requesting localmem access\n");
  }

  MemCmd::Command pkt_cmd;

  switch (sched_entry->cmd) {
  case InitiateReadData:
    if (inst->isMCode()) pkt_cmd = MemCmd::GatherReq;
    else pkt_cmd = MemCmd::ReadReq;

    DPRINTF(InOrderLocalMemPort,
            "[tid:%i]: Read request from [sn:%i] for addr %08p\n",
            inst->readTid(), inst->seqNum, inst->getMemAddr());
    break;

  case InitiateWriteData:
    if (inst->isMCode()) pkt_cmd = MemCmd::ScatterReq;
    else pkt_cmd = MemCmd::WriteReq;

    DPRINTF(InOrderLocalMemPort,
            "[tid:%i]: Write request from [sn:%i] for addr %08p\n",
            inst->readTid(), inst->seqNum, inst->getMemAddr());
    break;

  default:
    panic("%i: Unexpected request type (%i) to %s", curTick(), sched_entry->cmd,
          name());
  }

  localmem_req->setRequest(inst, stage_num, id, slot_num, sched_entry->cmd,
                           pkt_cmd, inst->curSkedEntry->idx);
  return localmem_req;
}

void LocalMemUnit::requestAgain(DynInstPtr inst, bool &service_request) {
  LocalMemReqPtr localmem_req = dynamic_cast<LocalMemReqPtr>(findRequest(inst));
  assert(localmem_req);

  // Check to see if this instruction is requesting the same command
  // or a different one
  if (localmem_req->cmd != inst->curSkedEntry->cmd && localmem_req->instIdx
      == inst->curSkedEntry->idx) {
    // If different, then update command in the request
    localmem_req->cmd = inst->curSkedEntry->cmd;
    DPRINTF(InOrderLocalMemPort,
            "[tid:%i]: [sn:%i]: Updating the command for this "
            "instruction\n", inst->readTid(), inst->seqNum);

    service_request = true;
  } else {
    // If same command, just check to see if memory access was completed
    // but dont try to re-execute
    DPRINTF(InOrderLocalMemPort,
            "[tid:%i]: [sn:%i]: requesting this resource again\n",
            inst->readTid(), inst->seqNum);

    service_request = true;
  }
}

void LocalMemUnit::setupMemRequest(DynInstPtr inst, LocalMemReqPtr localmem_req,
                                   int acc_size, int flags) {
  ThreadID tid = inst->readTid();
  Addr aligned_addr = inst->getMemAddr();
  if (localmem_req->memReq == NULL) {
    localmem_req->memReq = new Request(cpu->asid[tid], aligned_addr, acc_size,
                                       flags, cpu->dataMasterId(),
                                       inst->instAddr(), cpu->readCpuId(), //@todo: use context id
                                       tid);
  }
}

void LocalMemUnit::doTLBAccess(DynInstPtr inst, LocalMemReqPtr localmem_req,
                               int acc_size, int flags,
                               TheISA::TLB::Mode tlb_mode) {
  setupMemRequest(inst, localmem_req, acc_size, flags);

  if (tlb_mode != TheISA::TLB::MExecute && inst->isMCode())
    tlb_mode = TheISA::TLB::MAccess;
  inst->fault = _tlb->translateAtomic(localmem_req->memReq, NULL, tlb_mode);

  DPRINTF(InOrderTLB, "[tid:%i]: [sn:%i] virt. addr %08p translated "
          "to phys. addr:%08p.\n", inst->readTid(), inst->seqNum,
          localmem_req->memReq->getVaddr(),
          localmem_req->memReq->getPaddr());
}

Fault LocalMemUnit::read(DynInstPtr inst, Addr addr, uint8_t *data,
                         unsigned size, unsigned flags) {
  LocalMemReqPtr localmem_req = dynamic_cast<LocalMemReqPtr>(findRequest(inst));
  assert(localmem_req && "Can't Find Instruction for Read!");

  //The size of the data we're trying to read.
  inst->totalSize = size;

  if (inst->traceData && !inst->traceData->getAddrValid()) {
    inst->traceData->setAddr(addr);
  }

  doTLBAccess(inst, localmem_req, size, flags, TheISA::TLB::Read);

  if (inst->fault == NoFault) {
    localmem_req->reqData = new uint8_t[size];
    doLocalMemAccess(inst, NULL);
  }

  return inst->fault;
}

Fault LocalMemUnit::write(DynInstPtr inst, uint8_t *data, unsigned size,
                          Addr addr, unsigned flags, uint64_t *write_res) {
  LocalMemReqPtr localmem_req = dynamic_cast<LocalMemReqPtr>(findRequest(inst));
  assert(localmem_req && "Can't Find Instruction for Write!");

  //The size of the data we're trying to write.
  inst->totalSize = size;

  if (inst->traceData && !inst->traceData->getAddrValid()) {
    inst->traceData->setAddr(addr);
  }

  doTLBAccess(inst, localmem_req, size, flags, TheISA::TLB::Write);

  if (inst->fault == NoFault) {
    localmem_req->reqData = new uint8_t[size];
    memcpy(localmem_req->reqData, data, size);
    doLocalMemAccess(inst, write_res);
  }

  return inst->fault;
}

void LocalMemUnit::execute(int slot_num) {
  LocalMemReqPtr localmem_req = dynamic_cast<LocalMemReqPtr>(reqs[slot_num]);
  //int index = 0;
  assert(localmem_req);

  if (localmemPortBlocked && (localmem_req->cmd == InitiateReadData
      || localmem_req->cmd == InitiateWriteData)) {
    DPRINTF(InOrderLocalMemPort, "LocalMem Port Blocked. Cannot Access\n");
    localmem_req->done(false);
    return;
  }

  DynInstPtr inst = localmem_req->inst;
  if (inst->fault != NoFault) {
    DPRINTF(InOrderLocalMemPort,
            "[tid:%i]: [sn:%i]: Detected %s fault @ %x. Forwarding to "
            "next stage.\n", inst->readTid(), inst->seqNum, inst->fault->name(),
            inst->getMemAddr());
    finishLocalMemUnitReq(inst, localmem_req);
    return;
  }

  if (inst->isSquashed()) {
    DPRINTF(InOrderLocalMemPort, "[tid:%i]: [sn:%i]: Detected squashed instruction "
            "next stage.\n", inst->readTid(), inst->seqNum);
    finishLocalMemUnitReq(inst, localmem_req);
    return;
  }

  ThreadID tid = inst->readTid();
#if TRACING_ON
  std::string acc_type = "write";
#endif

  switch (localmem_req->cmd) {
  case InitiateReadData:
#if TRACING_ON
    acc_type = "read";
#endif
  case InitiateWriteData:
    if (localmemPortBlocked) {
      DPRINTF(InOrderLocalMemPort, "LocalMem Port Blocked. Cannot Access\n");
      localmem_req->done(false);
      return;
    }
    DPRINTF(InOrderLocalMemPort, "[tid:%u]: [sn:%i] Initiating data %s access to %s for "
            "addr. %08p\n", tid, inst->seqNum, acc_type, name(),
            localmem_req->inst->getMemAddr());
    inst->setCurResSlot(slot_num);
    /*if (inst->isMCode()) {
      Addr *EA = (Addr *)inst->getEA();
      for (unsigned i = 0; i < 64; i++)
        if (EA[i] != DM5SDA2SegEnd + 1) {
          index = (EA[i] >> 22) - 1;
          break;
        }
      if (!inst->initiated) {
        if (accessTick[index] <= curTick()) {
          accessTick[index] = curTick() + 2000;
        } else {
          cpu->mstallTicks = 1000;
          accessTick[index] = curTick() + 3000;
        }
      }
    }*/
    inst->initiateAcc();
#if TRACING_ON
                if (inst->ex_tick == MaxTick)
                  inst->ex_tick = curTick();
#endif
    break;

  case CompleteReadData:
    DPRINTF(InOrderLocalMemPort, "[tid:%i]: [sn:%i]: Trying to Complete Data Read Access\n",
            tid, inst->seqNum);

    //@todo: timing translations need to check here...
    assert(!inst->isInstPrefetch() && "Can't Handle Inst. Prefecthes");
    if (localmem_req->isMemAccComplete()) {
      finishLocalMemUnitReq(inst, localmem_req);
    } else {
      DPRINTF(InOrderStall, "STALL: [tid:%i]: [sn:%i] Data miss from %08p, slot num is %i\n", tid, localmem_req->inst->seqNum,
              localmem_req->inst->getMemAddr(), slot_num);
      localmem_req->setCompleted(false);
      if (inst->isMCode()) {
        stallPipelineStages(localmem_req, BackEndStartStage, NumMStages,
                           tid - MaxSThreads, true);
      } else {
        stallPipelineStages(localmem_req, BackEndStartStage, NumSStages, tid,
                           false);
      }
      localmem_req->setMemStall(true);
    }
    break;

  case CompleteWriteData: {
    DPRINTF(InOrderLocalMemPort,
            "[tid:%i]: [sn:%i]: Trying to Complete Data Write Access\n",
            tid, inst->seqNum);

    //@todo: check that timing translation is finished here
    RequestPtr mem_req = localmem_req->memReq;
    if (mem_req->isLLSC()) {
      DPRINTF(InOrderLocalMemPort, "Detected Conditional Store Inst.\n");

      if (!localmem_req->isMemAccComplete()) {
        DPRINTF(InOrderStall, "STALL: [tid:%i]: Data miss from %08p\n", tid,
                localmem_req->inst->getMemAddr());
        localmem_req->setCompleted(false);
        localmem_req->setMemStall(true);
        return;
      } else {
        DPRINTF(InOrderStall, "Mem Acc Completed\n");
      }
    }

    if (localmem_req->isMemAccPending()) {
      DPRINTF(InOrderLocalMemPort, "Store Instruction Pending Completion.\n");
      //localmem_req->dataPkt->reqData = localmem_req->reqData;
      //localmem_req->dataPkt->memReq = localmem_req->memReq;
      localmem_req->setCompleted(false);
      if (inst->isMCode()) {
        stallPipelineStages(localmem_req, BackEndStartStage, NumMStages,
                            tid - MaxSThreads, true);
      } else {
        stallPipelineStages(localmem_req, BackEndStartStage, NumSStages, tid,
                            false);
      }
      localmem_req->setMemStall(true);
    } else {
      DPRINTF(InOrderLocalMemPort, "Store Instruction Finished Completion.\n");
      finishLocalMemUnitReq(inst, localmem_req);
    }
  }
    break;

  default:
    fatal("Unrecognized command to %s", resName);
  }
}

void LocalMemUnit::finishLocalMemUnitReq(DynInstPtr inst,
                                         LocalMemRequest *localmem_req) {
  localmem_req->setMemStall(false);
  localmem_req->done();
}

void LocalMemUnit::buildDataPacket(LocalMemRequest *localmem_req) {
  // Check for LL/SC and if so change command
  if (localmem_req->memReq->isLLSC() &&
      localmem_req->pktCmd == MemCmd::ReadReq) {
    localmem_req->pktCmd = MemCmd::LoadLockedReq;
  } else if (localmem_req->pktCmd == MemCmd::WriteReq) {
    localmem_req->pktCmd = (localmem_req->memReq->isLLSC() ?
                           MemCmd::StoreCondReq : MemCmd::WriteReq);
  }

  localmem_req->dataPkt = new LocalMemReqPacket(localmem_req,
                                                localmem_req->pktCmd,
                                                localmem_req->instIdx);
  DPRINTF(InOrderLocalMemPort, "[slot:%i]: Slot marked for %x\n",
          localmem_req->getSlot(), localmem_req->dataPkt->getAddr());

  localmem_req->dataPkt->hasSlot = true;
  localmem_req->dataPkt->dataStatic(localmem_req->reqData);
  localmem_req->dataPkt->reqData = localmem_req->reqData;
}

void LocalMemUnit::doLocalMemAccess(DynInstPtr inst, uint64_t *write_res) {
  Fault fault = NoFault;
#if TRACING_ON
  ThreadID tid = inst->readTid();
#endif
  bool do_access = true;  // flag to suppress localmem access

  // Special Handling if this is a split request
  LocalMemReqPtr localmem_req =
    dynamic_cast<LocalMemReqPtr>(reqs[inst->getCurResSlot()]);

  // Make a new packet inside the LocalMemRequest object
  assert(localmem_req);
  buildDataPacket(localmem_req);

  // Special Handling for LL/SC or Compare/Swap
  bool is_write = localmem_req->dataPkt->isWrite();
  RequestPtr mem_req = localmem_req->dataPkt->req;
  if (is_write) {
    DPRINTF(InOrderLocalMemPort, "[tid:%u]: [sn:%i]: Storing data: %s\n",
            tid, inst->seqNum,
            printMemData(localmem_req->dataPkt->getPtr<uint8_t>(),
                         localmem_req->dataPkt->getSize()));

    if (mem_req->isLLSC()) {
      assert(localmem_req->inst->isStoreConditional());
      DPRINTF(InOrderLocalMemPort, "Evaluating Store Conditional access\n");
      do_access = TheISA::handleLockedWrite(inst.get(), mem_req);
    }
  }

  // Finally, go ahead and make the access if we can...
  DPRINTF(InOrderLocalMemPort, "[tid:%i] [sn:%i] attempting to access"
          " localmem for addr %08p\n", tid, inst->seqNum,
          localmem_req->dataPkt->getAddr());

  if (do_access) {
    if (!localmemPort->sendTimingReq(localmem_req->dataPkt)) {
      DPRINTF(InOrderLocalMemPort, "[tid:%i] [sn:%i] cannot access localmem, because port "
              "is blocked. now waiting to retry request\n", tid, inst->seqNum);
      delete localmem_req->dataPkt;
      localmem_req->dataPkt = NULL;

      delete localmem_req->memReq;
      localmem_req->memReq = NULL;

      localmem_req->done(false);
      localmemPortBlocked = true;
      causedbySorM = inst->isMCode();
    } else {
      DPRINTF(InOrderLocalMemPort, "[tid:%i] [sn:%i] is now waiting for localmem response\n",
              tid, inst->seqNum);
      localmem_req->setCompleted();
      localmem_req->setMemAccPending();
      localmemPortBlocked = false;
    }
  } else if (mem_req->isLLSC()) {
    // Store-Conditional instructions complete even if they "failed"
    assert(localmem_req->inst->isStoreConditional());
    localmem_req->setCompleted(true);

    DPRINTF(LLSC, "[tid:%i]: T%i Ignoring Failed Store Conditional Access\n",
            tid, tid);

    processLocalMemCompletion(localmem_req->dataPkt);
  } else {
    delete localmem_req->dataPkt;
    localmem_req->dataPkt = NULL;

    delete localmem_req->memReq;
    localmem_req->memReq = NULL;

    // Make localmem request again since access due to
    // inability to access
    DPRINTF(InOrderStall, "STALL: \n");
    localmem_req->done(false);
  }

}

bool LocalMemUnit::processSquash(LocalMemReqPacket *localmem_pkt, bool SorM) {
  // The resource may no longer be actively servicing this
  // packet. Scenarios like a store that has been sent to the
  // memory system or access that's been squashed. If that's
  // the case, we can't access the request slot because it
  // will be either invalid or servicing another request.
  if (localmem_pkt->localmemReq->getInst())
    SorM = localmem_pkt->localmemReq->getInst()->isMCode();
  if (!localmem_pkt->hasSlot) {
    DPRINTF(InOrderLocalMemPort, "%x does not have a slot in unit, ignoring.\n",
            localmem_pkt->getAddr());

    if (localmem_pkt->reqData) {
      delete[] localmem_pkt->reqData;
      localmem_pkt->reqData = NULL;
    }

    if (localmem_pkt->memReq) {
      delete localmem_pkt->memReq;
      localmem_pkt->memReq = NULL;
    }

    delete localmem_pkt;
    localmem_pkt = NULL;
    if (SorM) cpu->wakeMPU();
    else cpu->wakeCPU();
    return true;
  } else {
    DPRINTF(InOrderLocalMemPort, "%x has slot %i\n", localmem_pkt->getAddr(),
            localmem_pkt->localmemReq->getSlot());
  }

  // It's possible that the request is squashed but the
  // packet is still acknowledged by the resource. Squashes
  // should happen at the end of the cycles and trigger the
  // code above, but if not, this would handle any timing
  // variations due to diff. user parameters.
  if (localmem_pkt->localmemReq->isSquashed()) {
    DPRINTF(InOrderLocalMemPort,
            "Ignoring completion of squashed access, [tid:%i] [sn:%i]\n",
            localmem_pkt->localmemReq->getInst()->readTid(),
            localmem_pkt->localmemReq->getInst()->seqNum);

    localmem_pkt->localmemReq->setMemAccPending(false);
    localmem_pkt->localmemReq->freeSlot();
    delete localmem_pkt;
    localmem_pkt = NULL;
    if (SorM) cpu->wakeMPU();
    else cpu->wakeCPU();
    return true;
  }

  return false;
}

void LocalMemUnit::processLocalMemCompletion(PacketPtr pkt) {
  //@todo: use packet sender state instead of deriving from packet class to
  //  get special state
  LocalMemReqPacket* localmem_pkt = dynamic_cast<LocalMemReqPacket*>(pkt);
  assert(localmem_pkt);

  DPRINTF(InOrderLocalMemPort, "Finished request for %x\n", pkt->getAddr());

  if (processSquash(localmem_pkt)) return;

  LocalMemRequest *localmem_req = dynamic_cast<LocalMemReqPtr>(findRequest(
      localmem_pkt->localmemReq->getInst(), localmem_pkt->instIdx));

  if (!localmem_req) {
    panic("[tid:%u]: [sn:%i]: Can't find slot for localmem access to "
          "addr. %08p\n",
          localmem_pkt->localmemReq->getInst()->readTid(),
          localmem_pkt->localmemReq->getInst()->seqNum,
          localmem_pkt->localmemReq->getInst()->getMemAddr());
  }

  assert(localmem_req);
  assert(localmem_req == localmem_pkt->localmemReq);

  DPRINTF(InOrderLocalMemPort, "[tid:%u]: [sn:%i]: [slot:%i] Waking from localmem"
          " access (vaddr.%08p, paddr:%08p)\n",
          localmem_pkt->localmemReq->getInst()->readTid(),
          localmem_pkt->localmemReq->getInst()->seqNum, localmem_req->getSlot(),
          localmem_pkt->req->getVaddr(), localmem_pkt->req->getPaddr());

  // Get resource request info
  unsigned stage_num = localmem_req->getStageNum();
  DynInstPtr inst = localmem_req->inst;

  assert(!localmem_req->isSquashed());
  assert(inst->staticInst && inst->isMemRef());

  DPRINTF(InOrderLocalMemPort, "[tid:%u]: [sn:%i]: Processing localmem access\n",
          inst->readTid(), inst->seqNum);

  inst->completeAcc(localmem_pkt);

  if (inst->isMCode()) {
    Addr *EA = (Addr *) (inst->getEA());
    assert(EA);
    delete[] EA;
  }
  localmem_req->setMemAccPending(false);
  localmem_req->setMemAccCompleted();

  inst->setExecuted();

  if (localmem_req->isMemStall()) {
    if (inst->isMCode()) {
      unstallPipelineStages(localmem_req, BackEndStartStage, NumMStages,
                            inst->getNextStage(), inst->readTid() - MaxSThreads,
                            true);
    } else {
      unstallPipelineStages(localmem_req, BackEndStartStage, NumSStages,
                            inst->getNextStage(), inst->readTid(), false);
    }
  }
  localmem_req->setMemStall(false);

  if (inst->isLoad()) {
    assert(localmem_pkt->isRead());

    if (localmem_pkt->req->isLLSC()) {
      DPRINTF(InOrderLocalMemPort, "[tid:%u]: Handling Load-Linked for [sn:%u]\n",
              inst->readTid(), inst->seqNum);
      TheISA::handleLockedRead(inst.get(), localmem_pkt->req);
    }

    DPRINTF(InOrderLocalMemPort, "[tid:%u]: [sn:%i]: Bytes loaded were: %s\n",
            inst->readTid(), inst->seqNum,
            printMemData(localmem_pkt->getPtr<uint8_t>(),
                         localmem_pkt->getSize()));
  } else if (inst->isStore()) {
    assert(localmem_pkt->isWrite());

    DPRINTF(InOrderLocalMemPort, "[tid:%u]: [sn:%i]: Bytes stored were: %s\n",
            inst->readTid(), inst->seqNum,
            printMemData(localmem_pkt->getPtr<uint8_t>(),
                         localmem_pkt->getSize()));
  }

  // Wake up the CPU (if it went to sleep and was waiting on this
  // completion event).
  if (inst->isMCode()) cpu->wakeMPU();
  else cpu->wakeCPU();

  DPRINTF(Activity, "[tid:%u] Activating %s due to localmem completion\n",
          inst->readTid(), cpu->pipelineStage[stage_num]->name());

  cpu->switchToActive(stage_num);
}

void LocalMemUnit::recvRetry() {
  DPRINTF(InOrderLocalMemPort, "Unblocking LocalMem Port. \n");

  assert(localmemPortBlocked);

  // Clear the localmem port for use again
  localmemPortBlocked = false;

  if (causedbySorM) cpu->wakeMPU();
  else cpu->wakeCPU();
}

void LocalMemUnit::squashLocalMemRequest(LocalMemReqPtr req_ptr) {
  DynInstPtr inst = req_ptr->getInst();
  req_ptr->setSquashed();
  inst->setSquashed();
}

void LocalMemUnit::squash(DynInstPtr inst, int stage_num,
                          InstSeqNum squash_seq_num, ThreadID tid) {
  for (int i = 0; i < width; i++) {
    ResReqPtr req_ptr = reqs[i];

    if (req_ptr->valid && req_ptr->getInst()->readTid() == tid
        && req_ptr->getInst()->seqNum > squash_seq_num) {

      DPRINTF(InOrderLocalMemPort, "[tid:%i] Squashing request from [sn:%i]\n",
              req_ptr->getInst()->readTid(), req_ptr->getInst()->seqNum);

      if (req_ptr->isSquashed()) {
        DPRINTF(AddrDep, "Request for [tid:%i] [sn:%i] already "
                "squashed, ignoring squash process.\n",
                req_ptr->getInst()->readTid(), req_ptr->getInst()->seqNum);
        continue;
      }

      LocalMemReqPtr localmem_req = dynamic_cast<LocalMemReqPtr>(req_ptr);
      assert(localmem_req);

      squashLocalMemRequest(localmem_req);

      int req_slot_num = req_ptr->getSlot();

      if (localmem_req->isMemAccPending()) {
        localmem_req->dataPkt->reqData = localmem_req->reqData;
        localmem_req->dataPkt->memReq = localmem_req->memReq;
      }
      freeSlot(req_slot_num);
    }
  }
}

void LocalMemRequest::clearRequest() {
  if (!memAccPending) {
    if (reqData) delete[] reqData;

    if (memReq) delete memReq;

    if (dataPkt) delete dataPkt;
  } else {
    if (dataPkt) dataPkt->hasSlot = false;
  }

  memReq = NULL;
  reqData = NULL;
  dataPkt = NULL;
  memAccComplete = false;
  memAccPending = false;
  instIdx = 0;
  fetchBufferFill = false;

  ResourceRequest::clearRequest();
}

void LocalMemUnit::
stallPipelineStages(LocalMemReqPtr req, int start, int end, int tid, bool SorM) {
  if (SorM)
    for (unsigned i = start; i < end; i++)
      cpu->mpipelineStage[i]->setResStall(req, tid);
  else
    for (unsigned i = start; i < end; i++)
      cpu->mpipelineStage[i]->setResStall(req, tid);
}


void LocalMemUnit::
unstallPipelineStages(LocalMemReqPtr req, int start, int end, int src,
                      int tid, bool SorM) {
  if (SorM) {
    for (unsigned i = start; i < end; i++) {
      cpu->mpipelineStage[i]->unsetResStall(req, tid);
    if (i > src)
      cpu->mpipelineStage[i]->stageStatus[tid] = PipelineStage::Resuming;
    }
  } else
    for (unsigned i = start; i < end; i++)
      cpu->mpipelineStage[i]->unsetResStall(req, tid);
}
