/*
 * Copyright (c) 2011 The Regents of The University of Michigan
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
#include "arch/locked_mem.hh"
#include "arch/utility.hh"
#include "config/the_isa.hh"
#include "cpu/inorder/resources/localmem_unit.hh"
#include "cpu/inorder/resources/mfetch_unit.hh"
#include "cpu/inorder/resources/use_def.hh"
#include "cpu/inorder/resources/execution_unit.hh"
#include "cpu/inorder/cpu.hh"
#include "cpu/inorder/pipeline_traits.hh"
#include "cpu/inorder/resource_pool.hh"
#include "debug/Activity.hh"
#include "debug/InOrderLocalMemPort.hh"
#include "debug/InOrderRepeat.hh"
#include "debug/InOrderStall.hh"
#include "debug/RefCount.hh"
#include "debug/ThreadModel.hh"
#include "debug/MapuDisasm.hh"
#include "mem/request.hh"
#include "base/bitfield.hh"

using namespace std;
using namespace TheISA;
using namespace ThePipeline;

MFetchUnit::MFetchUnit(string res_name, int res_id, int res_width,
                       int res_latency, InOrderCPU *_cpu,
                       ThePipeline::Params *params) :
    LocalMemUnit(res_name, res_id, res_width, res_latency, _cpu, params),
    instSize(sizeof(TheISA::MachInst)), fetchBuffSize(params->mfetchBuffSize),
    fetchBuffer(0), pendingFetch(0) {
  for (int tid = 0; tid < MaxMThreads; tid++) {
    decoder[tid] = new Decoder(NULL);
    isRepeating[tid] = false;
    ReCount[tid] = 0;
    ReBlkAddr[tid] = 0;
    cached_inst = NULL;
  }
  nextStage = 0;
  nextResStage = 0;
}

MFetchUnit::~MFetchUnit() {
  if (fetchBuffer.block)
    delete [] fetchBuffer.block;

  std::list<FetchBlock*>::iterator pend_it = pendingFetch.begin();
  std::list<FetchBlock*>::iterator pend_end = pendingFetch.end();
  while (pend_it != pend_end) {
    if ((*pend_it)->block) {
      delete (*pend_it)->block;
    }

    delete *pend_it;
    pend_it++;
  }
  pendingFetch.clear();
}

void MFetchUnit::init() {
  // Get the appropriate port from the CPU based on the resource name.
  localmemPort = &cpu->getMInstPort();
  assert(localmemPort != NULL);

  for (int i = 0; i < width; i++) {
    reqs[i] = new LocalMemRequest(this);
  }

  initSlots();
}

void MFetchUnit::createMachInst(FetchBlock* fetch_it, DynInstPtr inst) {
  Addr block_addr = inst->pcState().nlpc() - MCodeLineSize;
  Addr fetch_addr = inst->getMemAddr();
  unsigned fetch_offset = fetch_addr - block_addr;
  ThreadID tid = inst->readTid();
  TheISA::PCState instPC = inst->pcState();

  assert(tid >= MaxSThreads && tid < MaxThreads);

  DPRINTF(InOrderLocalMemPort, "Creating instruction [sn:%i] w/fetch data @"
          "addr:%08p block:%08p\n", inst->seqNum, fetch_addr, block_addr);

  assert(fetch_it->valid);

  ExtMachInst *fetchInsts =
      reinterpret_cast<TheISA::ExtMachInst *>(&fetch_it->block[fetch_offset]);

  ExtMachInst mach_inst = TheISA::gtoh(fetchInsts[0]);

  mach_inst = mach_inst >> (instPC.pcb() & 0x7);
  mach_inst &= ~((-1ULL) << (instPC.npcb() - instPC.pcb()));
  mach_inst |= ExtMachInst(instPC.sn()) << 32ULL;
  mach_inst |= 1ULL << 63; // S or M flag

  decoder[tid - MaxSThreads]->setTC(cpu->thread[tid]->getTC());
  decoder[tid - MaxSThreads]->moreBytes(mach_inst);

  assert(decoder[tid - MaxSThreads]->instReady());
  inst->setStaticInst(decoder[tid - MaxSThreads]->decode(instPC));
  if (inst->staticInst->isLoop()) {
    inst->setFrontSked(cpu->createLoopFrontEndSked(inst));
  }

  DPRINTF(InOrderLocalMemPort, "Inst pc is set to %s(%i)(slot %i)\n",
          inst->pcState(), instPC.pcb(), instPC.sn());

  if (isRepeating[tid - MaxSThreads]) return;
  // Check if it is Repeat
  if (instPC.sn() == (NumSlots - 1) &&
      (bits<ExtMachInst>(mach_inst, 22, 19) == 0x1 ||
       bits<ExtMachInst>(mach_inst, 22, 19) == 0x2)) {
    isRepeating[tid - MaxSThreads] = true;
    ReBlkAddr[tid - MaxSThreads] = block_addr;
    if (bits<ExtMachInst>(mach_inst, 22, 19) == 0x2) {
      ReCount[tid - MaxSThreads] = cpu->readMiscReg(
      TheISA::MISCREG_KI_BASE + bits<ExtMachInst>(mach_inst, 3, 0),
                                                    tid - MaxSThreads)
                                   - bits<ExtMachInst>(mach_inst, 7, 4);
      ReCount[tid - MaxSThreads] = (ReCount[tid - MaxSThreads] - 1) % (1 << 18);
      DPRINTF(InOrderRepeat,
              "Begin \"repeatki\"[sn:%i] inst at %#08x for %i times\n",
              inst->seqNum, block_addr, ReCount[tid - MaxSThreads]);
    } else {
      ReCount[tid - MaxSThreads] = bits<ExtMachInst>(mach_inst, 18, 8);
      ReCount[tid - MaxSThreads] = (ReCount[tid - MaxSThreads] - 1) % (1 << 11);
      DPRINTF(InOrderRepeat,
              "Begin \"repeatimm\"[sn:%i] inst at %#08x for %i times\n",
              inst->seqNum, block_addr, ReCount[tid - MaxSThreads]);
    }
    //if (ReCount[tid - MaxSThreads] == 1)
    //  cpu->mstallTicks = ticks(1);
    cached_inst = inst;
    nextStage = inst->nextStage;
    nextResStage = inst->nextResStage();
  }

}

ResReqPtr MFetchUnit::getRequest(DynInstPtr inst, int stage_num, int res_idx,
                                 int slot_num, unsigned cmd) {
  ScheduleEntry* sched_entry = *inst->curSkedEntry;
  LocalMemRequest* localmem_req = dynamic_cast<LocalMemRequest*>(reqs[slot_num]);

  if (!inst->validMemAddr()) {
    panic("Mem. Addr. must be set before requesting localmem access\n");
  }

  assert(sched_entry->cmd == InitiateMFetch);

  DPRINTF(InOrderLocalMemPort,
          "[tid:%i]: Fetch request from [sn:%i] for addr %08p\n",
          inst->readTid(), inst->seqNum, inst->getMemAddr());

  localmem_req->setRequest(inst, stage_num, id, slot_num, sched_entry->cmd,
                           MemCmd::ReadReq, inst->curSkedEntry->idx);

  return localmem_req;
}

void MFetchUnit::setupMemRequest(DynInstPtr inst, LocalMemReqPtr localmem_req,
                                 int acc_size, int flags) {
  ThreadID tid = inst->readTid();
  Addr aligned_addr = inst->pcState().nlpc() - MCodeLineSize;
  if (localmem_req->memReq == NULL) {
    localmem_req->memReq = new Request(
        tid, aligned_addr, acc_size, flags, cpu->instMasterId(),
        inst->instAddr(), cpu->readCpuId(), tid);
    DPRINTF(InOrderLocalMemPort, "[sn:%i] Created memReq @%x, ->%x\n",
            inst->seqNum, &localmem_req->memReq, localmem_req->memReq);
  }
}

void MFetchUnit::execute(int slot_num) {
  LocalMemReqPtr localmem_req = dynamic_cast<LocalMemReqPtr>(reqs[slot_num]);
  assert(localmem_req);

  if (localmemPortBlocked && localmem_req->cmd == InitiateMFetch) {
    DPRINTF(InOrderLocalMemPort, "LocalMem Port Blocked. Cannot Access\n");
    localmem_req->done(false);
    return;
  }

  DynInstPtr inst = localmem_req->inst;
  ThreadID tid = inst->readTid() - MaxSThreads;
  Addr block_addr = inst->pcState().nlpc() - MCodeLineSize;

  if (inst->fault != NoFault) {
    DPRINTF(InOrderLocalMemPort,
            "[tid:%i]: [sn:%i]: Detected %s fault @ %x. Forwarding to "
            "next stage.\n", inst->readTid(), inst->seqNum, inst->fault->name(),
            inst->pcState().nlpc() - MCodeLineSize);
    finishLocalMemUnitReq(inst, localmem_req);
    return;
  }

  switch (localmem_req->cmd) {
  case InitiateMFetch: {
    // Check to see if we've already got this request buffered
    // or pending to be buffered
    bool do_fetch = true;

    int total_pending = pendingFetch.size() + fetchBuffer.valid;
    if (!pendingFetch.empty()) {
      FetchBlock* pending_it = pendingFetch.back();
      if (pending_it->addr == block_addr) {
        do_fetch = false;

        DPRINTF(InOrderLocalMemPort, "%08p is a pending fetch block.\n",
                block_addr);
      }
    }

    if (!do_fetch) {
      DPRINTF(InOrderLocalMemPort, "Inst. [sn:%i] marked to be filled "
              "through fetch buffer.\n", inst->seqNum);
      localmem_req->fetchBufferFill = true;
      localmem_req->setCompleted(true);
      return;
    }

    // Check to see if there is room in the fetchbuffer for this instruction.
    // If not, block this request.
    if (total_pending >= fetchBuffSize) {
      DPRINTF(InOrderLocalMemPort, "No room available in fetch buffer.\n");
      localmem_req->setCompleted(false);
      return;
    }

    doTLBAccess(inst, localmem_req, MCodeLineSize, Request::INST_FETCH,
                TheISA::TLB::MExecute);

    if (inst->fault == NoFault) {
      DPRINTF(InOrderLocalMemPort, "[tid:%u]: Initiating fetch access to %s for"
              " addr:%#x (block:%#x)\n", inst->readTid(), name(),
              localmem_req->inst->getMemAddr(), block_addr);

      localmem_req->reqData = new uint8_t[MCodeLineSize];

      inst->setCurResSlot(slot_num);

      doLocalMemAccess(inst);

      if (localmem_req->isMemAccPending()) {
        pendingFetch.push_back(new FetchBlock(block_addr));
        // mark replacement block
      }
    }

    break;
  }

  case CompleteMFetch:
    if (isRepeating[tid]) {
      // It is in repeating period now, so the next inst is allocated
      // and initialized here
      // The seqNum and seqLineNum are not continuous with the first
      // "repeat" inst, due to the stages before CompleteMFetch stage.
      DynInstPtr reinst =
        new InOrderDynInst(cpu,
                           cpu->thread[cached_inst->readTid()],
                           cpu->getAndIncrementInstSeq(cached_inst->readTid()),
                           cached_inst->readTid(), cached_inst->readTid());

#if TRACING_ON
      reinst->traceData =
        dynamic_cast<Trace::InOrderTrace *>(cpu->getTracer())->
          getInstRecord(ThePipeline::NumMStages,
                        cpu->stageTracing,
                        cpu->thread[cached_inst->readTid()]->getTC());
      // Copy note of trace data for this inst & stage

#else
      reinst->traceData = NULL;
#endif      // TRACING_ON

      if (reinst->traceData) {
          //@todo: exec traces are broke. fix them
        for (unsigned i = 0; i < nextStage; i++)
          reinst->traceData->setStageCycle(i, curTick());
      }
      // Add instruction to the CPU's list of instructions.
      reinst->setInstListIt(cpu->addInst(reinst));
      reinst->setFrontSked(cpu->mfrontEndSked);
      reinst->setMCode();

      // Pretend the new repeated inst has passed the stages before
      // CompleteMFetch stage.
      while (reinst->nextResStage() != nextResStage)
        reinst->finishSkedEntry();
      reinst->setNextStage(nextStage);
      reinst->finishSkedEntry();

      // Calulate the pc address
      PCState pc;
      if (cached_inst->pcState().sn() == TheISA::NumSlots - 1) {
        pc.mset(ReBlkAddr[tid]);
        reinst->setSeqLineNum(
            cpu->getAndIncrementInstSeqLine(reinst->readTid()));
      } else {
        pc = cached_inst->pcState();
        pc.madvance();
        reinst->setSeqLineNum(cached_inst->seqLineNum);
      }
      reinst->pcState(pc);
      reinst->setMemAddr(pc.instAddr());

      if (reinst->pcState().sn() == TheISA::NumSlots - 1) reinst->setPackageEnd();

      // Initiate staticinst for the new inst from fetch buffer
      // and decode it
      FetchBlock* fetch_it = (fetchBuffer.addr == ReBlkAddr[tid] &&
                              fetchBuffer.valid) ? &fetchBuffer : NULL;
      if (fetch_it == NULL) {
        panic("\"Repeat\" cannot find the inst in the fetchBuffer.\n");
      }
      createMachInst(fetch_it, reinst);
#if TRACING_ON
      if (inst->if_tick == MaxTick)
        inst->if_tick = curTick() - 1000;
#endif
      if (reinst->traceData) {
        reinst->traceData->setStaticInst(reinst->staticInst);
        reinst->traceData->setPC(reinst->pcState());
      };

      // Now it is ready to send to the next stage
      if (!cpu->mpipelineStage[nextStage]->sendRepeatedInstToNextStage(reinst))
        panic("\"Repeat\" cannot send the inst to the next"
              " stage from stage %i.\n", nextStage);

      DPRINTF(InOrderRepeat, "Repeat inst[sn:%i] inst at %s.\n",
              reinst->seqNum, reinst->pcState());

      // Update the repeat counter
      if (reinst->pcState().sn() == TheISA::NumSlots - 1)
        ReCount[tid] = (ReCount[tid] - 1) & 0xFFFFF;
      if (ReCount[tid] == 0) isRepeating[tid] = false;
      cached_inst = reinst;

      // Block the inst after the unfinished "repeat"
      localmem_req->setCompleted(false);
      return;
    }

    if (inst->fault != NoFault) {
      DPRINTF(InOrderLocalMemPort,
              "[tid:%i]: [sn:%i]: Detected %s fault @ %x. Forwarding to "
              "next stage.\n", inst->readTid(), inst->seqNum,
              inst->fault->name(), inst->getMemAddr());
      finishLocalMemUnitReq(inst, localmem_req);
      return;
    }

    if (localmem_req->fetchBufferFill) {
      // Block request if it's depending on a previous fetch, but it hasnt made it yet
      FetchBlock* fetch_it = (fetchBuffer.addr == block_addr &&
                              fetchBuffer.valid) ? &fetchBuffer : NULL;
      if (fetch_it == NULL) {
        DPRINTF(InOrderLocalMemPort, "%#x not available yet\n", block_addr);
        localmem_req->setCompleted(false);
        return;
      }

      // Make New Instruction
      createMachInst(fetch_it, inst);
      if (inst->traceData) {
        inst->traceData->setStaticInst(inst->staticInst);
        inst->traceData->setPC(inst->pcState());
      }
#if TRACING_ON
      if (inst->id_tick == MaxTick)
        inst->id_tick = curTick();
#endif
      // For MaPU GUI trace
      if (!inst->isNop())
        DPRINTF(MapuDisasm, "[sn:%lli] : [sln:%u] : %llx : %s\n",
                inst->seqNum, inst->seqLineNum, inst->instAddr(),
                inst->staticInst->disassemble(inst->instAddr()));
      localmem_req->done();
      return;
    }

    if (localmem_req->isMemAccComplete()) {
      // If there is no replacement block, then we'll just have
      // to wait till that gets cleared before satisfying the fetch
      // for this instruction
      delete [] fetchBuffer.block;
      fetchBuffer.valid = false;

      DPRINTF(InOrderLocalMemPort,
              "[tid:%i]: Completing Fetch Access for [sn:%i]\n",
              inst->readTid(), inst->seqNum);

      // Make New Instruction
      assert(!pendingFetch.empty());;
      FetchBlock* fetch_it = pendingFetch.front();

      assert(fetch_it);
      assert(fetch_it->block);

      createMachInst(fetch_it, inst);
      if (inst->traceData) {
        inst->traceData->setStaticInst(inst->staticInst);
        inst->traceData->setPC(inst->pcState());
      }
#if TRACING_ON
      if (inst->id_tick == MaxTick)
        inst->id_tick = curTick();
#endif

      // Finally, update FetchBuffer w/Pending Block into the
      // MRU location
      fetchBuffer = *fetch_it;
      pendingFetch.pop_front();
      delete fetch_it;

      DPRINTF(InOrderLocalMemPort, "[tid:%i]: Instruction [sn:%i] is: %s\n",
              inst->readTid(), inst->seqNum,
              inst->staticInst->disassemble(inst->instAddr()));
      // For MaPU GUI trace
      if (!inst->isNop())
        DPRINTF(MapuDisasm, "[sn:%lli] : [sln:%u] : %llx : %s\n",
                inst->seqNum, inst->seqLineNum, inst->instAddr(),
                inst->staticInst->disassemble(inst->instAddr()));

      inst->unsetMemAddr();

      localmem_req->done();
    } else {
      DPRINTF(InOrderLocalMemPort,
              "[tid:%i]: [sn:%i]: Unable to Complete Fetch Access\n",
              inst->readTid(), inst->seqNum);
      DPRINTF(InOrderStall, "STALL: [tid:%i]: Fetch miss from %08p\n",
              inst->readTid(), localmem_req->inst->instAddr());
      localmem_req->setCompleted(false);
      localmem_req->setMemStall(true);
      stallPipelineStages(localmem_req, 0, NumMStages, tid, true);
    }
    break;

  default:
    fatal("Unrecognized command to %s", resName);
  }
}

void MFetchUnit::processLocalMemCompletion(PacketPtr pkt) {
  // Cast to correct packet type
  // @todo: use pkt Sender state here to be consistent with other
  // cpu models
  LocalMemReqPacket* localmem_pkt = dynamic_cast<LocalMemReqPacket*>(pkt);
  assert(localmem_pkt);

  DPRINTF(InOrderLocalMemPort, "Finished request for %x\n",
          localmem_pkt->getAddr());

  if (processSquash(localmem_pkt, true)) return;

  Addr block_addr = localmem_pkt->localmemReq->getInst()->pcState().nlpc() -
                    MCodeLineSize;

  DPRINTF(InOrderLocalMemPort, "[tid:%u]: [sn:%i]: Waking from fetch access to "
          "addr:%#x(phys:%#x), size:%i\n",
          localmem_pkt->localmemReq->getInst()->readTid(),
          localmem_pkt->localmemReq->getInst()->seqNum, block_addr,
          localmem_pkt->getAddr(), localmem_pkt->getSize());

  // Cast to correct request type
  LocalMemRequest *localmem_req =
    dynamic_cast<LocalMemReqPtr>(findRequest(localmem_pkt->localmemReq->getInst(),
                                             localmem_pkt->instIdx));

  if (!localmem_req) {
    panic("[tid:%u]: [sn:%i]: Can't find slot for fetch access to "
          "addr. %08p\n", localmem_pkt->localmemReq->getInst()->readTid(),
          localmem_pkt->localmemReq->getInst()->seqNum, block_addr);
  }

  // Get resource request info
  unsigned stage_num = localmem_req->getStageNum();
  DynInstPtr inst = localmem_req->inst;
  ThreadID tid = inst->readTid() - MaxSThreads;

  assert(!localmem_req->isSquashed());
  assert(inst->curSkedEntry->cmd == CompleteMFetch);

  DPRINTF(InOrderLocalMemPort,
          "[tid:%u]: [sn:%i]: Processing fetch access for block %#x\n",
          localmem_req->inst->readTid(), inst->seqNum, block_addr);

  std::list<FetchBlock*>::
  iterator pend_it = pendingFetch.begin();
  while(pend_it != pendingFetch.end()) {
    if ((*pend_it)->addr == block_addr &&
        !(*pend_it)->block) break;
    pend_it++;
  }
  assert(pend_it != pendingFetch.end());

  // Copy Data to pendingFetch queue...
  (*pend_it)->block = new uint8_t[MCodeLineSize];
  memcpy((*pend_it)->block, localmem_pkt->getPtr<uint8_t>(), MCodeLineSize);
  (*pend_it)->valid = true;

  localmem_req->setMemAccPending(false);
  localmem_req->setMemAccCompleted();

  if (localmem_req->isMemStall()) {
    unstallPipelineStages(localmem_req, 0, NumMStages, inst->nextStage,
                          tid, true);
  }
  localmem_req->setMemStall(false);

  // Wake up the CPU (if it went to sleep and was waiting on this
  // completion event).
  cpu->wakeMPU();

  DPRINTF(Activity, "[tid:%u] Activating %s due to localmem completion\n",
          inst->readTid(), cpu->mpipelineStage[stage_num]->name());

  cpu->switchToActive(stage_num);
}

void MFetchUnit::squashLocalMemRequest(LocalMemReqPtr req_ptr) {
  DynInstPtr inst = req_ptr->getInst();
  Addr block_addr = inst->pcState().nlpc() - MCodeLineSize;

  // Check Fetch Buffer (or pending fetch) for this block and
  // update pending counts
  FetchBlock* buff_it = (fetchBuffer.addr == block_addr &&
                         fetchBuffer.valid) ? &fetchBuffer : NULL;
  if (buff_it) {
    buff_it->valid = false;
    delete [] buff_it->block;
    buff_it->block = NULL;
    DPRINTF(InOrderLocalMemPort, "[sn:%i] Removing Pending Access "
            "for Fetch Buffer block %08p.\n", inst->seqNum, block_addr);
  }
  // MaPU impl.: pendingFetch may have the same block addr with fetchBuffer
  std::list<FetchBlock*>::iterator block_it = pendingFetch.begin();
  while (block_it != pendingFetch.end() && (*block_it)->addr != block_addr)
    block_it++;
  if (block_it != pendingFetch.end()) {
    DPRINTF(InOrderLocalMemPort, "[sn:%i] Removing Pending Access "
            "for Pending Buffer Block %08p\n", inst->seqNum, block_addr);
    if ((*block_it)->block) {
      delete[] (*block_it)->block;
    }
    delete *block_it;
    pendingFetch.erase(block_it);
  }
  LocalMemUnit::squashLocalMemRequest(req_ptr);
}

void MFetchUnit::trap(Fault fault, ThreadID tid, DynInstPtr inst) {
  if (!inst->isMCode()) return;
  DPRINTF(InOrderLocalMemPort, "%s trapped\n", name());
  //@todo: per thread?
  decoder[tid - MaxSThreads]->reset();

  //@todo: squash using dummy inst seq num
  squash(NULL, NumMStages - 1, 0, tid);

  //@todo: make sure no blocks are in use
  assert(!fetchBuffer.valid);
  assert(pendingFetch.empty());
}

void MFetchUnit::squash(DynInstPtr inst, int stage_num,
                        InstSeqNum squash_seq_num, ThreadID tid) {
  //@todo: clear repeating status
  if (inst->isMCode() && cached_inst && cached_inst->seqNum >= squash_seq_num) {
    for (int i = 0; i < MaxMThreads; i++) {
      isRepeating[i] = false;
      ReCount[i] = 0;
      ReBlkAddr[i] = 0;
      cached_inst = NULL;
    }
    nextStage = 0;
    nextResStage = 0;
  }

  LocalMemUnit::squash(inst, stage_num, squash_seq_num, tid);
}

Tick MFetchUnit::ticks(int num_cycles) {
  return cpu->ticks(num_cycles) / 2;
}
