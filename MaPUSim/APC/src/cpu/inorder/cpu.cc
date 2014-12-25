/*
 * Copyright (c) 2012 ARM Limited
 * All rights reserved
 *
 * The license below extends only to copyright in the software and shall
 * not be construed as granting a license to any other intellectual
 * property including but not limited to intellectual property relating
 * to a hardware implementation of the functionality of the software
 * licensed hereunder.  You may use the software subject to the license
 * terms below provided that you ensure that this notice is replicated
 * unmodified and in its entirety in all distributions of the software,
 * modified or unmodified, in source code or in binary form.
 *
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

#include <algorithm>

#include "arch/utility.hh"
#include "base/bigint.hh"
#include "config/the_isa.hh"
#include "cpu/inorder/resources/cache_unit.hh"
#include "cpu/inorder/resources/resource_list.hh"
#include "cpu/inorder/cpu.hh"
#include "cpu/inorder/first_stage.hh"
#include "cpu/inorder/vliw_pipeline_stage.hh"
#include "cpu/inorder/inorder_dyn_inst.hh"
#include "cpu/inorder/pipeline_traits.hh"
#include "cpu/inorder/resource_pool.hh"
#include "cpu/inorder/thread_context.hh"
#include "cpu/inorder/thread_state.hh"
#include "cpu/activity.hh"
#include "cpu/base.hh"
#include "cpu/exetrace.hh"
#include "cpu/quiesce_event.hh"
#include "cpu/simple_thread.hh"
#include "cpu/thread_context.hh"
#include "debug/Activity.hh"
#include "debug/InOrderCPU.hh"
#include "debug/InOrderMPUStall.hh"
#include "debug/InOrderCachePort.hh"
#include "debug/Interrupt.hh"
#include "debug/Quiesce.hh"
#include "debug/RefCount.hh"
#include "debug/SkedCache.hh"
#include "debug/MapuCSU.hh"
#include "debug/MaPUPipeView.hh"
#include "params/InOrderCPU.hh"
#include "sim/full_system.hh"
#include "sim/process.hh"
#include "sim/stat_control.hh"
#include "sim/system.hh"
#include "base/loader/csu_object.hh"

#if THE_ISA == ALPHA_ISA
#include "arch/alpha/osfpal.hh"
#endif

using namespace std;
using namespace TheISA;
using namespace ThePipeline;

InOrderCPU::CachePort::CachePort(CacheUnit *_cacheUnit)
  : CpuPort(_cacheUnit->name() + "-cache-port", _cacheUnit->cpu),
    cacheUnit(_cacheUnit) {}

bool InOrderCPU::CachePort::recvTimingResp(Packet *pkt) {
  if (pkt->isError())
    DPRINTF(InOrderCachePort, "Got error packet back for address: %x\n",
            pkt->getAddr());
  else
    cacheUnit->processCacheCompletion(pkt);

  return true;
}

void InOrderCPU::CachePort::recvRetry() {
    cacheUnit->recvRetry();
}

InOrderCPU::LocalMemPort::LocalMemPort(LocalMemUnit *_localmemUnit)
  : CpuPort(_localmemUnit->name() + "-localmem-port", _localmemUnit->cpu),
    localmemUnit(_localmemUnit) {}

bool InOrderCPU::LocalMemPort::recvTimingResp(Packet *pkt) {
  if (pkt->isError())
    DPRINTF(InOrderCachePort, "Got error packet back for address: %x\n",
            pkt->getAddr());
  else
    localmemUnit->processLocalMemCompletion(pkt);

  return true;
}

void InOrderCPU::LocalMemPort::recvRetry() {
  localmemUnit->recvRetry();
}

InOrderCPU::MTickEvent::MTickEvent(InOrderCPU *c)
  : Event(CPU_Tick_Pri), cpu(c) {
}

void InOrderCPU::MTickEvent::process() {
  cpu->mtick();
}

const char *
InOrderCPU::MTickEvent::description() const {
  return "InOrderCPU mtick event";
}

InOrderCPU::TickEvent::TickEvent(InOrderCPU *c)
  : Event(CPU_Tick_Pri), cpu(c) {
}

void InOrderCPU::TickEvent::process() {
  cpu->tick();
}

const char *
InOrderCPU::TickEvent::description() const {
  return "InOrderCPU tick event";
}

InOrderCPU::CPUEvent::CPUEvent(InOrderCPU *_cpu,
                               CPUEventType e_type,
                               Fault fault,
                               ThreadID _tid,
                               DynInstPtr inst,
                               CPUEventPri event_pri)
  : Event(event_pri), cpu(_cpu) {
  setEvent(e_type, fault, _tid, inst);
}

std::string InOrderCPU::eventNames[NumCPUEvents] = { "ActivateThread",
                                                     "ActivateNextReadyThread",
                                                     "DeactivateThread",
                                                     "HaltThread",
                                                     "SuspendThread", "Trap",
                                                     "Syscall",
                                                     "SquashFromMemStall",
                                                     "UpdatePCs" };

void InOrderCPU::CPUEvent::process() {
  switch (cpuEventType) {
  case ActivateThread:
    cpu->activateThread(tid);
    cpu->resPool->activateThread(tid);
    break;

  case ActivateNextReadyThread:
    cpu->activateNextReadyThread();
    break;

  case DeactivateThread:
    cpu->deactivateThread(tid);
    cpu->resPool->deactivateThread(tid);
    break;

  case HaltThread:
    cpu->haltThread(tid);
    cpu->resPool->deactivateThread(tid);
    break;

  case SuspendThread:
    cpu->suspendThread(tid);
    cpu->resPool->suspendThread(tid);
    break;

  case SquashFromMemStall:
    cpu->squashDueToMemStall(inst->squashingStage, inst->seqNum, tid);
    cpu->resPool->squashDueToMemStall(inst, inst->squashingStage, inst->seqNum,
                                      tid);
    break;

  case Trap:
    DPRINTF(InOrderCPU, "Trapping CPU\n");
    cpu->trap(fault, tid, inst);
    cpu->resPool->trap(fault, tid, inst);
    cpu->trapPending[tid] = false;
    break;

  case Syscall:
    cpu->syscall(inst->syscallNum, tid);
    cpu->resPool->trap(fault, tid, inst);
    break;

  default:
    fatal("Unrecognized Event Type %s", eventNames[cpuEventType]);
  }

  cpu->cpuEventRemoveList.push(this);
}

const char *
InOrderCPU::CPUEvent::description() const {
  return "InOrderCPU event";
}

void InOrderCPU::CPUEvent::scheduleEvent(int delay) {
  assert(!scheduled() || squashed());
  cpu->reschedule(this, cpu->nextCycle(curTick() + cpu->ticks(delay)), true);
}

void InOrderCPU::CPUEvent::unscheduleEvent() {
  if (scheduled())
    squash();
}

InOrderCPU::InOrderCPU(Params *params)
  : BaseCPU(params), cpu_id(params->cpu_id), coreType("default"),
    _status(Idle), tickEvent(this), mtickEvent(this),
    stageWidth(params->stageWidth), mstageWidth(params->mstageWidth),
    resPool(new ResourcePool(this, params)), stimeBuffer(2, 2),
    mtimeBuffer(2, 2), dataPort(resPool->getDataUnit()),
    instPort(resPool->getInstUnit()), minstPort(resPool->getMInstUnit()),
    removeInstsThisCycle(false),
    sactivityRec(params->name + "_spu", NumSStages, 20, params->activity),
    mactivityRec(params->name + "_mpu", NumMStages, 20, params->activity),
    system(params->system),
#ifdef DEBUG
    cpuEventNum(0),
    resReqCount(0),
#endif // DEBUG
    drainCount(0), deferRegistration(false/*params->deferRegistration*/),
    stageTracing(params->stageTracing), lastRunningCycle(0), instsPerSwitch(0) {
  cpu_params = params;

  // Resize for Multithreading CPUs
  thread.resize(numThreads);

  ThreadID active_threads = params->workload.size();
  if (FullSystem) {
    active_threads = 1;
  } else {
    active_threads = params->workload.size();

    if (active_threads > MaxThreads) {
      panic("Workload Size too large. Increase the 'MaxThreads'"
            "in your InOrder implementation or "
            "edit your workload size.");
    }

    if (active_threads > 1) {
      threadModel = (InOrderCPU::ThreadModel) params->threadModel;

      if (threadModel == SMT) {
        DPRINTF(InOrderCPU, "Setting Thread Model to SMT.\n");
      } else if (threadModel == SwitchOnCacheMiss) {
        DPRINTF(InOrderCPU,
                "Setting Thread Model to " "Switch On Cache Miss\n");
      } else if (threadModel == HeteroDT) {
        if (active_threads == 2)
          DPRINTF(
            InOrderCPU,
            "Setting Thread Model to " "parallel hetero-double-threading mode.\n");
        else
          panic("More than 2 threads under parallel "
                "hetero-double-threading mode is not allowed.\n");
      }

    } else {
      threadModel = (InOrderCPU::ThreadModel) params->threadModel;
      if (threadModel == HeteroDT)
        DPRINTF(
          InOrderCPU,
          "Setting parallel hetero-double-threading Model to " "Single mode.\n");

      threadModel = Single;
    }
  }

  for (ThreadID tid = 0; tid < numThreads; ++tid) {
    if (tid < MaxSThreads) {
      pc[tid].set(0);
      lastCommittedPC[tid].set(0);
    } else {
      pc[tid].mset(0);
      lastCommittedPC[tid].mset(0);
    }

    if (FullSystem) {
      // SMT is not supported in FS mode yet.
      assert(numThreads == 1);
      thread[tid] = new Thread(this, 0, NULL);
    } else {
      if (tid < (ThreadID) params->workload.size()) {
        DPRINTF(InOrderCPU, "Workload[%i] process is %#x\n", tid,
                params->workload[tid]->prog_fname);
        thread[tid] = new Thread(this, tid, params->workload[tid]);
      } else {
        //Allocate Empty thread so M5 can use later
        //when scheduling threads to CPU
        Process* dummy_proc = params->workload[0];
        thread[tid] = new Thread(this, tid, dummy_proc);
      }

      // Eventually set this with parameters...
      asid[tid] = tid;
    }

    // Setup the TC that will serve as the interface to the threads/CPU.
    InOrderThreadContext *tc = new InOrderThreadContext;
    tc->cpu = this;
    tc->thread = thread[tid];

    // Setup quiesce event.
    this->thread[tid]->quiesceEvent = new EndQuiesceEvent(tc);

    // Give the thread the TC.
    thread[tid]->tc = tc;
    thread[tid]->setFuncExeInst(0);
    globalSeqNum[tid] = 1;

    globalLineSeqNum[tid] = 1;

    // Add the TC to the CPU's list of TC's.
    this->threadContexts.push_back(tc);
  }

  // Initialize TimeBuffer Stage Queues
  for (int stNum = 0; stNum < NumSStages - 1; stNum++) {
    stageQueue[stNum] = new StageQueue(NumSStages, NumSStages);
    stageQueue[stNum]->id(stNum);
  }

  // Set Up Pipeline Stages
  for (int stNum = 0; stNum < NumSStages; stNum++) {
    if (stNum == 0)
      pipelineStage[stNum] = new FirstStage(params, stNum, false);
    else if (stNum >= ThePipeline::BackEndStartStage)
      pipelineStage[stNum] = new VliwPipelineStage(params, stNum, false);
    else
      pipelineStage[stNum] = new PipelineStage(params, stNum, false);

    pipelineStage[stNum]->setCPU(this);
    pipelineStage[stNum]->setActiveThreads(&activeSThreads);
    pipelineStage[stNum]->setTimeBuffer(&stimeBuffer);

    // Take Care of 1st/Nth stages
    if (stNum > 0)
      pipelineStage[stNum]->setPrevStageQueue(stageQueue[stNum - 1]);
    if (stNum < NumSStages - 1)
      pipelineStage[stNum]->setNextStageQueue(stageQueue[stNum]);
  }

  // Initialize TimeBuffer Stage Queues
  for (int stNum = 0; stNum < NumMStages - 1; stNum++) {
    mstageQueue[stNum] = new StageQueue(NumMStages, NumMStages);
    mstageQueue[stNum]->id(stNum);
  }
  // Set Up Pipeline Stages
  for (int stNum = 0; stNum < NumMStages; stNum++) {
    if (stNum == 0)
      mpipelineStage[stNum] = new FirstStage(params, stNum, true);
    else
      mpipelineStage[stNum] = new VliwPipelineStage(params, stNum, true);

    mpipelineStage[stNum]->setCPU(this);
    mpipelineStage[stNum]->setActiveThreads(&activeMThreads);
    mpipelineStage[stNum]->setTimeBuffer(&mtimeBuffer);

    // Take Care of 1st/Nth stages
    if (stNum > 0)
      mpipelineStage[stNum]->setPrevStageQueue(mstageQueue[stNum - 1]);
    if (stNum < NumMStages - 1)
      mpipelineStage[stNum]->setNextStageQueue(mstageQueue[stNum]);
  }

  // Initialize thread specific variables
  for (ThreadID tid = 0; tid < numThreads; tid++) {
    archRegDepMap[tid].setCPU(this);

    nonSpecInstActive[tid] = false;
    nonSpecSeqNum[tid] = 0;

    squashSeqNum[tid] = MaxAddr;
    lastSquashCycle[tid] = 0;

#if THE_ISA == MAPU_ISA
    squashLineSeqNum[tid] = MaxAddr;
    if (threadModel != HeteroDT || tid == 0) {
#endif
      memset(intRegs[tid], 0, sizeof(intRegs[tid]));
      memset(floatRegs.i[tid], 0, sizeof(floatRegs.i[tid]));

#if THE_ISA == MAPU_ISA
      memset(intJRegs[tid], 0, sizeof(intJRegs[tid]));
      memset(doubleRegs[tid], 0, sizeof(doubleRegs[tid]));
    }
#endif
    isa[tid].clear();

    // Define dummy instructions and resource requests to be used.
    dummyInst[tid] = new InOrderDynInst(this, thread[tid], 0, tid, asid[tid]);

    dummyReq[tid] = new ResourceRequest(resPool->getResource(0));

    if (FullSystem) {
      // Use this dummy inst to force squashing behind every instruction
      // in pipeline
      dummyTrapInst[tid] = new InOrderDynInst(this, NULL, 0, 0, 0);
      dummyTrapInst[tid]->seqNum = 0;
#if THE_ISA == MAPU_ISA
      dummyTrapInst[tid]->squashLineSeqNum = 0;
#endif
      dummyTrapInst[tid]->squashSeqNum = 0;
      dummyTrapInst[tid]->setTid(tid);
    }

    trapPending[tid] = false;

  }

  // InOrderCPU always requires an interrupt controller.
  if (!params->defer_registration && !interrupts) {
    fatal("InOrderCPU %s has no interrupt controller.\n"
          "Ensure createInterruptController() is called.\n",
          name());
  }

  dummyReqInst = new InOrderDynInst(this, NULL, 0, 0, 0);
  dummyReqInst->setSquashed();
  dummyReqInst->resetInstCount();

  dummyBufferInst = new InOrderDynInst(this, NULL, 0, 0, 0);
  dummyBufferInst->setSquashed();
  dummyBufferInst->resetInstCount();

  frontEndSked = createFrontEndSked();
  mfrontEndSked = createFrontEndSked(true);
  //lfrontEndSked = createLoopFrontEndSked();
  faultSked = createFaultSked();

  lastRunningCycle = curTick();

  lockAddr = 0;
  lockFlag = false;

  // Schedule First Tick Event, CPU will reschedule itself from here on out.
  if (!MaPUSystem)
    scheduleTickEvent(0);
}

InOrderCPU::~InOrderCPU() {
  delete resPool;

  SkedCacheIt sked_it = skedCache.begin();
  SkedCacheIt sked_end = skedCache.end();

  while (sked_it != sked_end) {
    delete (*sked_it).second;
    sked_it++;
  }
  skedCache.clear();

  sked_it = skedCacheM.begin();
  sked_end = skedCacheM.end();

  while (sked_it != sked_end) {
    delete (*sked_it).second;
    sked_it++;
  }
  skedCacheM.clear();
}

m5::hash_map<InOrderCPU::SkedID, ThePipeline::RSkedPtr> InOrderCPU::skedCache;
m5::hash_map<InOrderCPU::SkedID, ThePipeline::RSkedPtr> InOrderCPU::skedCacheM;

RSkedPtr InOrderCPU::createLoopFrontEndSked(DynInstPtr inst) {
  RSkedPtr res_sked = lookupSked(inst);
  if (res_sked != NULL) {
    DPRINTF(SkedCache, "Found %s in sked cache.\n", inst->instName());
    return res_sked;
  } else {
    res_sked = new ResourceSked(NumMStages);
  }
  //RSkedPtr res_sked = new ResourceSked(NumMStages);
  int stage_num = 0;
  StageScheduler FG(res_sked, stage_num++);
  StageScheduler FS(res_sked, stage_num++);
  StageScheduler FW(res_sked, stage_num++);
  StageScheduler FR(res_sked, stage_num++);
  StageScheduler DP(res_sked, stage_num++);

  // DECODE
  FW.needs(MICache, MFetchUnit::CompleteMFetch);
  for (int idx = 0; idx < inst->numSrcRegs(); idx++) {
    FR.needs(RegManager, UseDefUnit::ReadSrcReg, idx);
  }
  FR.needs(ExecUnit, ExecutionUnit::ExecuteInst);
  for (int idx = 0; idx < inst->numDestRegs(); idx++) {
    FR.needs(RegManager, UseDefUnit::WriteDestReg, idx);
  }
  FR.needs(Grad, GraduationUnit::CheckFault);
  FR.needs(Grad, GraduationUnit::GraduateInst);

  // Insert Back Schedule into our cache of
  // resource schedules
  addToSkedCache(inst, res_sked);

  DPRINTF(SkedCache, "Front End Sked Created for instruction: %s (%08p)\n",
          inst->instName(), inst->getMachInst());
  res_sked->print();

  return res_sked;
}

RSkedPtr InOrderCPU::createFrontEndSked(bool SorM) {
  RSkedPtr res_sked =
    SorM ? new ResourceSked(NumMStages) : new ResourceSked(NumSStages);
  int stage_num = 0;

#if THE_ISA != MAPU_ISA
  StageScheduler F(res_sked, stage_num++);
  StageScheduler D(res_sked, stage_num++);

  // FETCH
  F.needs(FetchSeq, FetchSeqUnit::AssignNextPC);
  F.needs(ICache, FetchUnit::InitiateFetch);

  // DECODE
  D.needs(ICache, FetchUnit::CompleteFetch);
  D.needs(Decode, DecodeUnit::DecodeInst);
  D.needs(BPred, BranchPredictor::PredictBranch);
  D.needs(FetchSeq, FetchSeqUnit::UpdateTargetPC);

#else
  if (!SorM) {
    StageScheduler FG(res_sked, stage_num++);
    StageScheduler FS(res_sked, stage_num++);
    StageScheduler FW(res_sked, stage_num++);
    StageScheduler FR(res_sked, stage_num++);
    //StageScheduler DP(res_sked, stage_num++);

    // FETCH
    FG.needs(FetchSeq, FetchSeqUnit::AssignNextPC);
    FS.needs(ICache, FetchUnit::InitiateFetch);

    // DECODE
    FW.needs(ICache, FetchUnit::CompleteFetch);
    FR.needs(Decode, DecodeUnit::DecodeInst);
    FR.needs(BPred, BranchPredictor::PredictBranch);
    FR.needs(FetchSeq, FetchSeqUnit::UpdateTargetPC);
  } else {
    StageScheduler FG(res_sked, stage_num++);
    StageScheduler FS(res_sked, stage_num++);
    StageScheduler FW(res_sked, stage_num++);
    StageScheduler FR(res_sked, stage_num++);
    StageScheduler DP(res_sked, stage_num++);

    // FETCH
    FG.needs(FetchSeq, FetchSeqUnit::AssignNextMPC);
    FS.needs(MICache, MFetchUnit::InitiateMFetch);

    // DECODE
    FR.needs(MICache, MFetchUnit::CompleteMFetch);
    DP.needs(Decode, DecodeUnit::DecodeInst);
  }
#endif

  DPRINTF(SkedCache, "Resource Sked created for instruction Front End\n");

  return res_sked;
}

RSkedPtr InOrderCPU::createFaultSked() {
  RSkedPtr res_sked = new ResourceSked(NumSStages);
  StageScheduler W(res_sked, NumSStages - 1);
  W.needs(Grad, GraduationUnit::CheckFault);
  DPRINTF(SkedCache, "Resource Sked created for instruction Faults\n");
  return res_sked;
}

RSkedPtr InOrderCPU::createBackEndSked(DynInstPtr inst) {
  RSkedPtr res_sked = lookupSked(inst);
  if (res_sked != NULL) {
    DPRINTF(SkedCache, "Found %s in sked cache.\n", inst->instName());
    return res_sked;
  } else {
    res_sked = new ResourceSked(NumSStages);
  }

#if THE_ISA != MAPU_ISA
  int stage_num = ThePipeline::BackEndStartStage;
  StageScheduler X(res_sked, stage_num++);
  StageScheduler M(res_sked, stage_num++);
  StageScheduler W(res_sked, stage_num++);

  if (!inst->staticInst) {
    warn_once("Static Instruction Object Not Set. Can't Create"
      " Back End Schedule");
    return NULL;
  }

  // EXECUTE
  X.needs(RegManager, UseDefUnit::MarkDestRegs);
  for (int idx=0; idx < inst->numSrcRegs(); idx++) {
    if (!idx || !inst->isStore()) {
      X.needs(RegManager, UseDefUnit::ReadSrcReg, idx);
    }
  }

  //@todo: schedule non-spec insts to operate on this cycle
  // as long as all previous insts are done
  if ( inst->isNonSpeculative() ) {
    // skip execution of non speculative insts until later
  } else if ( inst->isMemRef() ) {
    if ( inst->isLoad() ) {
      X.needs(AGEN, AGENUnit::GenerateAddr);
    }
  } else if (inst->opClass() == IntMultOp || inst->opClass() == IntDivOp) {
    X.needs(MDU, MultDivUnit::StartMultDiv);
  } else {
    X.needs(ExecUnit, ExecutionUnit::ExecuteInst);
  }

  // MEMORY
  if (!inst->isNonSpeculative()) {
    if (inst->opClass() == IntMultOp || inst->opClass() == IntDivOp) {
      M.needs(MDU, MultDivUnit::EndMultDiv);
    }

    if ( inst->isLoad() ) {
      M.needs(DCache, CacheUnit::InitiateReadData);
      if (inst->splitInst)
      M.needs(DCache, CacheUnit::InitSecondSplitRead);
    } else if ( inst->isStore() ) {
      for (int i = 1; i < inst->numSrcRegs(); i++ ) {
        M.needs(RegManager, UseDefUnit::ReadSrcReg, i);
      }
      M.needs(AGEN, AGENUnit::GenerateAddr);
      M.needs(DCache, CacheUnit::InitiateWriteData);
      if (inst->splitInst)
      M.needs(DCache, CacheUnit::InitSecondSplitWrite);
    }
  }

  // WRITEBACK
  if (!inst->isNonSpeculative()) {
    if ( inst->isLoad() ) {
      W.needs(DCache, CacheUnit::CompleteReadData);
      if (inst->splitInst)
      W.needs(DCache, CacheUnit::CompleteSecondSplitRead);
    } else if ( inst->isStore() ) {
      W.needs(DCache, CacheUnit::CompleteWriteData);
      if (inst->splitInst)
      W.needs(DCache, CacheUnit::CompleteSecondSplitWrite);
    }
  } else {
    // Finally, Execute Speculative Data
    if (inst->isMemRef()) {
      if (inst->isLoad()) {
        W.needs(AGEN, AGENUnit::GenerateAddr);
        W.needs(DCache, CacheUnit::InitiateReadData);
        if (inst->splitInst)
        W.needs(DCache, CacheUnit::InitSecondSplitRead);
        W.needs(DCache, CacheUnit::CompleteReadData);
        if (inst->splitInst)
        W.needs(DCache, CacheUnit::CompleteSecondSplitRead);
      } else if (inst->isStore()) {
        if ( inst->numSrcRegs() >= 2 ) {
          W.needs(RegManager, UseDefUnit::ReadSrcReg, 1);
        }
        W.needs(AGEN, AGENUnit::GenerateAddr);
        W.needs(DCache, CacheUnit::InitiateWriteData);
        if (inst->splitInst)
        W.needs(DCache, CacheUnit::InitSecondSplitWrite);
        W.needs(DCache, CacheUnit::CompleteWriteData);
        if (inst->splitInst)
        W.needs(DCache, CacheUnit::CompleteSecondSplitWrite);
      }
    } else {
      W.needs(ExecUnit, ExecutionUnit::ExecuteInst);
    }
  }

  W.needs(Grad, GraduationUnit::CheckFault);

  for (int idx=0; idx < inst->numDestRegs(); idx++) {
    W.needs(RegManager, UseDefUnit::WriteDestReg, idx);
  }

  if (inst->isControl())
  W.needs(BPred, BranchPredictor::UpdatePredictor);

  W.needs(Grad, GraduationUnit::GraduateInst);

#else  //MAPU
  int stage_num = ThePipeline::BackEndStartStage;
  StageScheduler RR(res_sked, stage_num++);
  StageScheduler EX1(res_sked, stage_num++);
  StageScheduler EX2(res_sked, stage_num++);
  StageScheduler EX3(res_sked, stage_num++);
  StageScheduler EX4(res_sked, stage_num++);
  StageScheduler EX5(res_sked, stage_num++);
  StageScheduler EX6(res_sked, stage_num++);
  StageScheduler EX7(res_sked, stage_num++);

  if (!inst->staticInst) {
    warn_once("Static Instruction Object Not Set. Can't Create"
              " Back End Schedule");
    return NULL;
  }

  // EXECUTE
  RR.needs(RegManager, UseDefUnit::MarkDestRegs);
  for (int idx = 0; idx < inst->numSrcRegs(); idx++) {
    if (inst->is4cycle())
      EX3.needs(RegManager, UseDefUnit::ReadSrcReg, idx);
    else
      RR.needs(RegManager, UseDefUnit::ReadSrcReg, idx);
  }

  //@todo: schedule non-spec insts to operate on this cycle
  // as long as all previous insts are done
  if (inst->isNonSpeculative()) {
    // skip execution of non speculative insts until later
  } else if (inst->isMemRef()) {
    EX1.needs(AGEN, AGENUnit::GenerateAddr);
  } else if (inst->is4cycle()) {
    EX4.needs(ExecUnit, ExecutionUnit::ExecuteInst);
  } else {
    EX1.needs(ExecUnit, ExecutionUnit::ExecuteInst);
  }

  if (inst->isCSUOp()) {
    if(inst->is3cycle())
      EX2.needs(CsuUnit, ComSynUnit::CsuRead);
    else
      EX2.needs(CsuUnit, ComSynUnit::CsuWrite);
  }


  // MEMORY
  if (!inst->isNonSpeculative()) {
    if (inst->isLoad()) {
      EX3.needs(DCache, CacheUnit::InitiateReadData);
    } else if (inst->isStore()) {
      EX3.needs(DCache, CacheUnit::InitiateWriteData);
    }
  }

  // WRITEBACK
  if (!inst->isNonSpeculative()) {
    if (inst->isLoad()) {
      EX5.needs(DCache, CacheUnit::CompleteReadData);
    } else if (inst->isStore()) {
      EX5.needs(DCache, CacheUnit::CompleteWriteData);
    }
  } else {
    EX1.needs(ExecUnit, ExecutionUnit::ExecuteInst);
  }
  StageScheduler *pW = NULL;
  if (inst->numDestRegs()) {
    if (inst->is1cycle())
      pW = &EX2;
    else if (inst->is2cycle() && !inst->isMemRef())
      pW = &EX3;
    else if (inst->is3cycle())
      pW = &EX4;
    else if (inst->is4cycle())
      pW = &EX5;
    else if (!inst->isLoad())
      pW = &EX6;
    else
      pW = &EX7;
  } else {
    if (inst->is1cycle())
      pW = &EX1;
    else if (inst->is2cycle() && !inst->isMemRef())
      pW = &EX2;
    else if (inst->is3cycle())
      pW = &EX3;
    else if (inst->is4cycle())
      pW = &EX4;
    else
      pW = &EX5;
  }

  pW->needs(Grad, GraduationUnit::CheckFault);

  for (int idx = 0; idx < inst->numDestRegs(); idx++) {
    if (inst->isMemRef() && inst->is2cycle() && idx == 0)
      EX3.needs(RegManager, UseDefUnit::WriteDestReg, idx);
    else
      pW->needs(RegManager, UseDefUnit::WriteDestReg, idx);
  }

  pW->needs(Grad, GraduationUnit::GraduateInst);
#endif

  // Insert Back Schedule into our cache of
  // resource schedules
  addToSkedCache(inst, res_sked);

  DPRINTF(SkedCache, "Back End Sked Created for instruction: %s (%08p)\n",
          inst->instName(), inst->getMachInst());
  res_sked->print();

  return res_sked;
}

RSkedPtr InOrderCPU::createMBackEndSked(DynInstPtr inst) {
  RSkedPtr res_sked = lookupSked(inst);
  if (res_sked != NULL) {
    DPRINTF(SkedCache, "Found %s in sked cache.\n", inst->instName());
    return res_sked;
  } else {
    res_sked = new ResourceSked(NumMStages);
  }
  DPRINTF(SkedCache, "Current next stage: %i.\n", inst->getNextStage());

  int stage_num = ThePipeline::BackEndStartStage + 1;
  //StageScheduler DP(res_sked, stage_num++);
  //StageScheduler IB(res_sked, stage_num++);
  StageScheduler RR(res_sked, stage_num++);
  StageScheduler EX1(res_sked, stage_num++);
  StageScheduler EX2(res_sked, stage_num++);
  StageScheduler EX3(res_sked, stage_num++);
  StageScheduler EX4(res_sked, stage_num++);
  StageScheduler EX5(res_sked, stage_num++);
  StageScheduler EX6(res_sked, stage_num++);
  StageScheduler EX7(res_sked, stage_num++);
  StageScheduler EX8(res_sked, stage_num++);
  StageScheduler EX9(res_sked, stage_num++);
  StageScheduler EX10(res_sked, stage_num++);
  StageScheduler EX11(res_sked, stage_num++);
  StageScheduler EX12(res_sked, stage_num++);

  if (!inst->staticInst) {
    warn_once("Static Instruction Object Not Set. Can't Create"
              " Back End Schedule");
    return NULL;
  }

  // EXECUTE
  for (int idx = 0; idx < inst->numSrcRegs(); idx++) {
    switch (inst->mopClass()) {
    case Biu0MemOp:
    case Biu1MemOp:
    case Biu2MemOp:
      if ((inst->isStore() && idx == 0))
        RR.needs(MPURegManager, MpuRfsUnit::ReadSrcReg, idx);
      else
        RR.needs(RegManager, UseDefUnit::ReadSrcReg, idx);
      break;
    case ImacAccOp:
    case ImacAccMovOp:
      if (inst->srcRegIdx(idx) > 4)
        EX1.needs(MPURegManager, MpuRfsUnit::ReadMR, idx);
      else
        RR.needs(MPURegManager, MpuRfsUnit::ReadSrcReg, idx);
      break;
    case ImacMacOp:
    case ImacMacMovOp:
      if (inst->srcRegIdx(idx) > 4)
        EX6.needs(MPURegManager, MpuRfsUnit::ReadMR, idx);
      else
        RR.needs(MPURegManager, MpuRfsUnit::ReadSrcReg, idx);
      break;
    case FmacMacOp:
    case FmacMacMovOp:
      if (inst->srcRegIdx(idx) > 4)
        RR.needs(MPURegManager, MpuRfsUnit::ReadMR, idx);
      else
        RR.needs(MPURegManager, MpuRfsUnit::ReadSrcReg, idx);
      break;
    case IaluDivOp:
    case IaluDivMovOp:
      if (inst->numSrcRegs() == 2)
        RR.needs(MPURegManager, MpuRfsUnit::ReadSrcReg, idx);
      else
        RR.needs(MPURegManager, MpuRfsUnit::ReadMR, idx);
      break;
    default:
        RR.needs(MPURegManager, MpuRfsUnit::ReadSrcReg, idx);
      break;
    }
  }
#if 0
    if (inst->mopClass() == BiuMemOp
        /*inst->isMemRef() || inst->numSrcRegs() >= 23*/) {
      if ((inst->isStore() && idx == 0))
        RR.needs(MPURegManager, MpuRfsUnit::ReadSrcReg, idx);
      else
        RR.needs(RegManager, UseDefUnit::ReadSrcReg, idx);
    } else if (inst->mopClass() == ImacAccOp ||
               inst->mopClass() == ImacAccMovOp
               /*inst->opClass() == IntMultOp &&
               inst->pcState().sn() == 7 &&
               inst->srcRegIdx(idx) > 4*/) {
      //if(inst->is2cycle() || inst->is4cycle())
        RR.needs(MPURegManager, MpuRfsUnit::ReadMR, idx);
      //else
    } else if (inst->mopClass() == ImacMacOp ||
               inst->mopClass() == ImacAccMacOp) {
      EX4.needs(MPURegManager, MpuRfsUnit::ReadMR, idx);
    } else if (inst->opClass() == IntMultOp &&
               inst->pcState().sn() == 9 && inst->srcRegIdx(idx) > 4) {
      RR.needs(MPURegManager, MpuRfsUnit::ReadMR, idx);
    } else if (inst->opClass() == IntMultOp &&
               inst->pcState().sn() == 6  ) {   //ialudiv
        if(inst->numSrcRegs()==2)
          RR.needs(MPURegManager, MpuRfsUnit::ReadSrcReg, idx);
        else
          RR.needs(MPURegManager, MpuRfsUnit::ReadMR, idx);
    } else
        RR.needs(MPURegManager, MpuRfsUnit::ReadSrcReg, idx);
  }
#endif

  //@todo: schedule non-spec insts to operate on this cycle
  // as long as all previous insts are done
  if (inst->isMemRef()) {
    RR.needs(AGEN, AGENUnit::GenerateAddr);
  } else if (inst->mopClass() == ImacMacOp ||
             inst->mopClass() == ImacMacMovOp
      /*inst->opClass() == IntMultOp && inst->pcState().sn() == 7 &&
             !inst->is2cycle() && !inst->is4cycle()*/) {
    EX6.needs(ExecUnit, ExecutionUnit::ExecuteInst);
  } else if (inst->mopClass() == ImacAccOp ||
             inst->mopClass() == ImacAccMovOp) {
    EX1.needs(ExecUnit, ExecutionUnit::ExecuteInst);
  } else if (!inst->isNop()){
    RR.needs(ExecUnit, ExecutionUnit::ExecuteInst);
  }

  // MEMORY
  //for (unsigned i = 0; i < 64; i++)
  if (inst->isLoad()) {
    EX1.needs(Biu0Unit + (inst->mopClass() - Biu0MemOp),
              CacheUnit::InitiateReadData);
  } else if (inst->isStore()) {
    EX1.needs(Biu0Unit + (inst->mopClass() - Biu0MemOp),
              CacheUnit::InitiateWriteData);
  }

  // WRITEBACK
  if (inst->isLoad()) {
    EX3.needs(Biu0Unit + (inst->mopClass() - Biu0MemOp),
              CacheUnit::CompleteReadData);
  } else if (inst->isStore()) {
    EX3.needs(Biu0Unit + (inst->mopClass() - Biu0MemOp),
              CacheUnit::CompleteWriteData);
  }

  uint32_t wb_stage = 0;
  switch (inst->mopClass()) {
  case Mr0123Op: wb_stage = 2; break;
  case Mr2WOp: wb_stage = 3; break;
  case ShuIndCombOp: wb_stage = 2; break;
  case IaluArithOp: wb_stage = 3; break;
  case IaluDivOp: wb_stage = 1; break;
  case IaluDivMovOp: wb_stage = 2; break;
  case IaluCondOp: wb_stage = 3; break;
  case ImacMulOp: wb_stage = 5; break;
  case ImacMASOp: wb_stage = 7; break;
  case ImacMAOp: wb_stage = 6; break;
  case ImacMacOp: wb_stage = 6; break;
  case ImacMacMovOp: wb_stage = 8; break;
  case ImacAccOp: wb_stage = 1; break;
  case ImacAccMovOp: wb_stage = 4; break;
  case FaluArithOp: wb_stage = 5; break;
  case FmacMulOp: wb_stage = 6; break;
  case FmacCmulOp: wb_stage = 10; break;
  case FmacMAOp: wb_stage = 8; break;
  case FmacMacOp: wb_stage = 8; break;
  case FmacMacMovOp: wb_stage = 9; break;
  case Biu0MemOp:
  case Biu1MemOp:
  case Biu2MemOp:
    if (inst->isStore())
      wb_stage = 8;
    else if (inst->isLoad())
      wb_stage = 10;
    else
      wb_stage = 3;
    break;
  default: wb_stage = 0; break;
  }
  // Fix WB stage according to destination
  if (!(inst->mopClass() == ImacMAOp ||
        inst->mopClass() == ImacMacOp ||
        inst->mopClass() == ImacAccOp ||
        inst->mopClass() == IaluDivOp ||
        inst->mopClass() == FmacMAOp ||
        inst->mopClass() == FmacMacOp ||
        inst->mopClass() == IaluCondOp ||
        ((inst->mopClass() == Biu0MemOp ||
          inst->mopClass() == Biu1MemOp ||
          inst->mopClass() == Biu2MemOp) && !inst->isLoad()))) {
  switch (inst->staticInst->dest()) {
  case 0:
    wb_stage++;
  case 3:
    switch (inst->mopClass()) {
    case Biu0MemOp:
    case Biu1MemOp:
    case Biu2MemOp:
    case ShuIndCombOp:
      wb_stage++;
      break;
    default: break;
    }
    break;

  case 1:
  case 2:
    switch (inst->mopClass()) {
    /*
    case Mr0123Op:
    case Mr2WOp:
    case IaluArithOp:
    case IaluDivOp:
    case IaluDivMovOp:
    case IaluCondOp:
    case ImacMulOp:
    case ImacMASOp:
    case ImacMAOp:
    case ImacMacOp:
    case ImacMacMovOp:
    case ImacAccOp:
    case ImacAccMovOp:
    case FaluArithOp:
    case FmacMulOp:
    case FmacCmulOp:
    case FmacMAOp:
    case FmacMacOp:
    case FmacMacMovOp:*/
    case No_MOpClass:
    case MseqCtrlOp:
      break;
    case ShuIndCombOp:
      if (inst->staticInst->dest() == 2)
        wb_stage++;
      break;
    case Biu0MemOp:
    case Biu1MemOp:
    case Biu2MemOp:
      if (inst->staticInst->dest() == 1)
        wb_stage++;
      break;
    default:
      wb_stage++;
      break;
    }
    break;
  default: break;
  }
  }

  StageScheduler *pW = NULL;
  switch (wb_stage) {
  case 0: pW = &RR; break;
  case 1: pW = &EX1; break;
  case 2: pW = &EX2; break;
  case 3: pW = &EX3; break;
  case 4: pW = &EX4; break;
  case 5: pW = &EX5; break;
  case 6: pW = &EX6; break;
  case 7: pW = &EX7; break;
  case 8: pW = &EX8; break;
  case 9: pW = &EX9; break;
  case 10: pW = &EX10; break;
  case 11: pW = &EX11; break;
  case 12: pW = &EX12; break;
  default: pW = &RR; break;
  }
#if 0
  if (inst->is1cycle())
    pW = &EX1;
  else if (inst->is2cycle())
    pW = &EX2;
  else if (inst->is3cycle())
    pW = &EX3;
  else if (inst->is4cycle())
    pW = &EX4;
  else if (inst->is5cycle())
    pW = &EX5;
  else if (inst->is6cycle())
    pW = &EX6;
  else if (inst->is7cycle() || inst->isStore())
    pW = &EX7;
  else if (inst->is8cycle() || inst->isLoad())
    pW = &EX8;
  else if (inst->is9cycle())
    pW = &EX9;
  else if (inst->is10cycle())
    pW = &EX10;
  else
    pW = &RR;
#endif

  pW->needs(Grad, GraduationUnit::CheckFault);
  for (int idx = 0; idx < inst->numDestRegs(); idx++) {
    switch (inst->mopClass()) {
    case Biu0MemOp:
    case Biu1MemOp:
    case Biu2MemOp:
      if ((inst->isLoad() && idx == 0))
        pW->needs(MPURegManager, MpuRfsUnit::WriteDestReg, idx);
      else if (inst->isMemRef())
        RR.needs(MPURegManager, MpuRfsUnit::WriteDestReg, idx);
      else
        pW->needs(MPURegManager, MpuRfsUnit::WriteDestReg, idx);
      break;
    case ImacAccOp:
    case ImacAccMovOp:
      if (idx < 2)
        EX1.needs(MPURegManager, MpuRfsUnit::WriteMR, idx);
      else
        pW->needs(MPURegManager, MpuRfsUnit::WriteDestReg, idx);
      break;
    case ImacMAOp:
    case ImacMacOp:
    case ImacMacMovOp:
      if (idx < 2)
        EX6.needs(MPURegManager, MpuRfsUnit::WriteMR, idx);
      else
        pW->needs(MPURegManager, MpuRfsUnit::WriteDestReg, idx);
      break;
    case FmacMAOp:
    case FmacMacOp:
    case FmacMacMovOp:
      if (idx == 0)
        EX8.needs(MPURegManager, MpuRfsUnit::WriteMR, idx);
      else
        pW->needs(MPURegManager, MpuRfsUnit::WriteDestReg, idx);
      break;
    case IaluDivOp:
      RR.needs(MPURegManager, MpuRfsUnit::WriteMR, idx);
      break;
    case ShuIndCombOp:
      if (idx != 0)
        RR.needs(MPURegManager, MpuRfsUnit::WriteDestReg, idx);
      else
        pW->needs(MPURegManager, MpuRfsUnit::WriteDestReg, idx);
      break;
    default:
      pW->needs(MPURegManager, MpuRfsUnit::WriteDestReg, idx);
      break;
    }
  }
#if 0
  for (int idx = 0; idx < inst->numDestRegs(); idx++) {
    if (inst->isMemRef()) {
      if ((inst->isLoad() && idx == 0))
        pW->needs(MPURegManager, MpuRfsUnit::WriteDestReg, idx);
      else
        RR.needs(MPURegManager, MpuRfsUnit::WriteDestReg, idx);
    } else if (inst->opClass() == IntMultOp &&
               inst->pcState().sn() == 7 && idx < 2) {
      if (inst->is2cycle() || inst->is4cycle())
        RR.needs(MPURegManager, MpuRfsUnit::WriteMR, idx);
      else
        EX5.needs(MPURegManager, MpuRfsUnit::WriteMR, idx);
    } else if (inst->opClass() == IntMultOp &&
               inst->pcState().sn() == 9 && idx == 0){
        EX8.needs(MPURegManager, MpuRfsUnit::WriteMR, idx);
    } else if (inst->opClass() == IntMultOp &&
               inst->pcState().sn() == 6 && inst->is1cycle() ){  //ialudiv
          RR.needs(MPURegManager, MpuRfsUnit::WriteMR, idx);
    } else if ((inst->pcState().sn() == 4 ||
              inst->pcState().sn() == 5) &&
              idx != 0)
      RR.needs(MPURegManager, MpuRfsUnit::WriteDestReg, idx);
    else
      pW->needs(MPURegManager, MpuRfsUnit::WriteDestReg, idx);
  }
#endif
  pW->needs(Grad, GraduationUnit::GraduateInst);

  // Insert Back Schedule into our cache of
  // resource schedules
  addToSkedCache(inst, res_sked);

  DPRINTF(SkedCache, "Back End Sked Created for instruction: %s (%08p)\n",
          inst->instName(), inst->getMachInst());
  res_sked->print();

  return res_sked;
}

void InOrderCPU::regStats() {
  /* Register the Resource Pool's stats here.*/
  resPool->regStats();

  /* Register for each Pipeline Stage */
  for (int stage_num = 0; stage_num < ThePipeline::NumSStages; stage_num++) {
    pipelineStage[stage_num]->regStats();
  }

  /* Register for each MPU Pipeline Stage */
  for (int stage_num = 0; stage_num < ThePipeline::NumMStages; stage_num++) {
    mpipelineStage[stage_num]->regStats();
  }

  /* Register any of the InOrderCPU's stats here.*/
  instsPerCtxtSwitch.name(name() + ".instsPerContextSwitch").desc(
    "Instructions Committed Per Context Switch").prereq(instsPerCtxtSwitch);

  numCtxtSwitches.name(name() + ".contextSwitches").desc(
    "Number of context switches");

  comLoads.name(name() + ".comLoads").desc(
    "Number of Load instructions committed");

  comStores.name(name() + ".comStores").desc(
    "Number of Store instructions committed");

  comBranches.name(name() + ".comBranches").desc(
    "Number of Branches instructions committed");

  comNops.name(name() + ".comNops").desc(
    "Number of Nop instructions committed");

  comNonSpec.name(name() + ".comNonSpec").desc(
    "Number of Non-Speculative instructions committed");

  comInts.name(name() + ".comInts").desc(
    "Number of Integer instructions committed");

  comFloats.name(name() + ".comFloats").desc(
    "Number of Floating Point instructions committed");

  timesIdled.name(name() + ".timesIdled").desc(
    "Number of times that the entire CPU went into an idle state and"
    " unscheduled itself").prereq(timesIdled);

  idleCycles.name(name() + ".idleCycles").desc(
    "Number of cycles cpu's stages were not processed");

  runCycles.name(name() + ".runCycles").desc(
    "Number of cycles cpu stages are processed.");

  activity.name(name() + ".activity").desc("Percentage of cycles cpu is active").precision(
    6);
  activity = (runCycles / numCycles) * 100;

  threadCycles.init(numThreads).name(name() + ".threadCycles").desc(
    "Total Number of Cycles A Thread Was Active in CPU (Per-Thread)");

  smtCycles.name(name() + ".smtCycles").desc(
    "Total number of cycles that the CPU was in SMT-mode");

  committedInsts.init(numThreads).name(name() + ".committedInsts").desc(
    "Number of Instructions committed (Per-Thread)");

  committedOps.init(numThreads).name(name() + ".committedOps").desc(
    "Number of Ops committed (Per-Thread)");

  smtCommittedInsts.init(numThreads).name(name() + ".smtCommittedInsts").desc(
    "Number of SMT Instructions committed (Per-Thread)");

  totalCommittedInsts.name(name() + ".committedInsts_total").desc(
    "Number of Instructions committed (Total)");

  cpi.name(name() + ".cpi").desc("CPI: Cycles Per Instruction (Per-Thread)").precision(
    6);
  cpi = numCycles / committedInsts;

  smtCpi.name(name() + ".smt_cpi").desc("CPI: Total SMT-CPI").precision(6);
  smtCpi = smtCycles / smtCommittedInsts;

  totalCpi.name(name() + ".cpi_total").desc("CPI: Total CPI of All Threads").precision(
    6);
  totalCpi = numCycles / totalCommittedInsts;

  ipc.name(name() + ".ipc").desc("IPC: Instructions Per Cycle (Per-Thread)").precision(
    6);
  ipc = committedInsts / numCycles;

  smtIpc.name(name() + ".smt_ipc").desc("IPC: Total SMT-IPC").precision(6);
  smtIpc = smtCommittedInsts / smtCycles;

  totalIpc.name(name() + ".ipc_total").desc("IPC: Total IPC of All Threads").precision(
    6);
  totalIpc = totalCommittedInsts / numCycles;

  BaseCPU::regStats();
}

void InOrderCPU::mtick() {
  DPRINTF(InOrderCPU, "\n\nInOrderCPU: MTicking main, InOrderCPU.\n");

  ++numCycles;

  bool pipes_idle = true;
  //Tick each of the stages
  for (int stNum = NumMStages - 1; stNum >= 0; stNum--) {
    mpipelineStage[stNum]->tick();

    pipes_idle = pipes_idle && mpipelineStage[stNum]->idle;
  }

  if (pipes_idle)
    idleCycles++;
  else
    runCycles++;

  // Now advance the time buffers one tick
  mtimeBuffer.advance();
  for (int sqNum = 0; sqNum < NumMStages - 1; sqNum++) {
    mstageQueue[sqNum]->advance();
  }
  mactivityRec.advance();

  // Any squashed events, or insts then remove them now
  cleanUpRemovedEvents();
  cleanUpRemovedInsts();

  // Re-schedule CPU for this cycle
  if (!mtickEvent.scheduled()) {
    if (_status == SwitchedOut) {
      // increment stat
      lastRunningCycle = curTick();
    } else if (!mactivityRec.active()) {
      DPRINTF(InOrderCPU, "sleeping CPU.\n");
      lastRunningCycle = curTick();
      timesIdled++;
    } else {
      //Tick next_tick = curTick() + cycles(1);
      //tickEvent.schedule(next_tick);
      schedule(&mtickEvent, nextMCycle(curTick() + 1));
      DPRINTF(InOrderCPU, "Scheduled MPU for next tick @ %i.\n",
              nextMCycle(curTick() + 1));
    }
  }

  tickThreadStats();
  updateThreadPriority();
}

void InOrderCPU::tick() {
  DPRINTF(InOrderCPU, "\n\nInOrderCPU: Ticking main, InOrderCPU.\n");

  ++numCycles;

  checkForInterrupts();

  bool pipes_idle = true;
  //Tick each of the stages
  for (int stNum = NumSStages - 1; stNum >= 0; stNum--) {
    pipelineStage[stNum]->tick();

    pipes_idle = pipes_idle && pipelineStage[stNum]->idle;
  }

  if (pipes_idle)
    idleCycles++;
  else
    runCycles++;

  // Now advance the time buffers one tick
  stimeBuffer.advance();
  for (int sqNum = 0; sqNum < NumSStages - 1; sqNum++) {
    stageQueue[sqNum]->advance();
  }
  sactivityRec.advance();

  // Any squashed events, or insts then remove them now
  cleanUpRemovedEvents();
  cleanUpRemovedInsts();

  // Re-schedule CPU for this cycle
  if (!tickEvent.scheduled()) {
    if (_status == SwitchedOut) {
      // increment stat
      lastRunningCycle = curTick();
    } else if (!sactivityRec.active()) {
      DPRINTF(InOrderCPU, "sleeping CPU.\n");
      lastRunningCycle = curTick();
      timesIdled++;
    } else if (!isThreadSuspended(0)) {
      //Tick next_tick = curTick() + cycles(1);
      //tickEvent.schedule(next_tick);
      schedule(&tickEvent, nextCycle(curTick() + 1));
      DPRINTF(InOrderCPU, "Scheduled CPU for next tick @ %i.\n",
              nextCycle(curTick() + 1));
    }
  }

  tickThreadStats();
  updateThreadPriority();
}

void InOrderCPU::init() {
  BaseCPU::init();

  for (ThreadID tid = 0; tid < numThreads; ++tid) {
    // Set inSyscall so that the CPU doesn't squash when initially
    // setting up registers.
    thread[tid]->inSyscall = true;
    // Initialise the ThreadContext's memory proxies
    thread[tid]->initMemProxies(thread[tid]->getTC());
  }

  if (FullSystem && !params()->defer_registration) {
    for (ThreadID tid = 0; tid < numThreads; tid++) {
      ThreadContext *src_tc = threadContexts[tid];
      TheISA::initCPU(src_tc, src_tc->contextId());
    }
  }

  // Clear inSyscall.
  for (ThreadID tid = 0; tid < numThreads; ++tid)
    thread[tid]->inSyscall = false;

  // Call Initializiation Routine for Resource Pool
  resPool->init();
}

Fault InOrderCPU::hwrei(ThreadID tid) {
#if THE_ISA == ALPHA_ISA
  // Need to clear the lock flag upon returning from an interrupt.
  setMiscRegNoEffect(AlphaISA::MISCREG_LOCKFLAG, false, tid);

  thread[tid]->kernelStats->hwrei();
  // FIXME: XXX check for interrupts? XXX
#endif

  return NoFault;
}

bool InOrderCPU::simPalCheck(int palFunc, ThreadID tid) {
#if THE_ISA == ALPHA_ISA
  if (this->thread[tid]->kernelStats)
    this->thread[tid]->kernelStats->callpal(palFunc, this->threadContexts[tid]);

  switch (palFunc) {
  case PAL::halt:
    halt();
    if (--System::numSystemsRunning == 0)
      exitSimLoop("all cpus halted");
    break;

  case PAL::bpt:
  case PAL::bugchk:
    if (this->system->breakpoint())
      return false;
    break;
  }
#endif
  return true;
}

void InOrderCPU::checkForInterrupts() {
  for (int i = 0; i < threadContexts.size(); i++) {
    ThreadContext *tc = threadContexts[i];

    if (interrupts->checkInterrupts(tc)) {
      Fault interrupt = interrupts->getInterrupt(tc);

      if (interrupt != NoFault) {
        DPRINTF(Interrupt, "Processing Intterupt for [tid:%i].\n",
                tc->threadId());

        ThreadID tid = tc->threadId();
        interrupts->updateIntrInfo(tc);

        // Squash from Last Stage in Pipeline
        unsigned last_stage = NumSStages - 1;
        dummyTrapInst[tid]->squashingStage = last_stage;
        pipelineStage[last_stage]->setupSquash(dummyTrapInst[tid], tid);

        // By default, setupSquash will always squash from stage + 1
        pipelineStage[BackEndStartStage - 1]->setupSquash(dummyTrapInst[tid],
                                                          tid);

        // Schedule Squash Through-out Resource Pool
        resPool->scheduleEvent(
          (InOrderCPU::CPUEventType) ResourcePool::SquashAll,
          dummyTrapInst[tid], 0);

        // Finally, Setup Trap to happen at end of cycle
        trapContext(interrupt, tid, dummyTrapInst[tid]);
      }
    }
  }
}

Fault InOrderCPU::getInterrupts() {
  // Check if there are any outstanding interrupts
  return interrupts->getInterrupt(threadContexts[0]);
}

void InOrderCPU::processInterrupts(Fault interrupt) {
  // Check for interrupts here.  For now can copy the code that
  // exists within isa_fullsys_traits.hh.  Also assume that thread 0
  // is the one that handles the interrupts.
  // @todo: Possibly consolidate the interrupt checking code.
  // @todo: Allow other threads to handle interrupts.

  assert(interrupt != NoFault);
  interrupts->updateIntrInfo(threadContexts[0]);

  DPRINTF(InOrderCPU, "Interrupt %s being handled\n", interrupt->name());

  // Note: Context ID ok here? Impl. of FS mode needs to revisit this
  trap(interrupt, threadContexts[0]->contextId(), dummyBufferInst);
}

void InOrderCPU::trapContext(Fault fault,
                             ThreadID tid,
                             DynInstPtr inst,
                             int delay) {
  scheduleCpuEvent(Trap, fault, tid, inst, delay);
  trapPending[tid] = true;
}

void InOrderCPU::trap(Fault fault, ThreadID tid, DynInstPtr inst) {
  fault->invoke(tcBase(tid), inst->staticInst);
  removePipelineStalls(tid);
}

void InOrderCPU::squashFromMemStall(DynInstPtr inst, ThreadID tid, int delay) {
  scheduleCpuEvent(SquashFromMemStall, NoFault, tid, inst, delay);
}

void InOrderCPU::squashDueToMemStall(int stage_num,
                                     InstSeqNum seq_num,
                                     ThreadID tid) {
  DPRINTF(InOrderCPU, "Squashing Pipeline Stages Due to Memory Stall...\n");

  // Squash all instructions in each stage including
  // instruction that caused the squash (seq_num - 1)
  // NOTE: The stage bandwidth needs to be cleared so thats why
  //       the stalling instruction is squashed as well. The stalled
  //       instruction is previously placed in another intermediate buffer
  //       while it's stall is being handled.
  InstSeqNum squash_seq_num = seq_num - 1;
  if (tid < MaxSThreads) {
    for (int stNum = stage_num; stNum >= 0; stNum--) {
      pipelineStage[stNum]->squashDueToMemStall(squash_seq_num, tid);
    }
  } else {
    for (int stNum = stage_num; stNum >= 0; stNum--) {
      mpipelineStage[stNum]->squashDueToMemStall(squash_seq_num,
                                                 tid - MaxSThreads);
    }
  }
}

void InOrderCPU::scheduleCpuEvent(CPUEventType c_event,
                                  Fault fault,
                                  ThreadID tid,
                                  DynInstPtr inst,
                                  unsigned delay,
                                  CPUEventPri event_pri) {
  CPUEvent *cpu_event = new CPUEvent(this, c_event, fault, tid, inst,
                                     event_pri);

  Tick sked_tick = nextCycle(curTick() + ticks(delay));
  if (delay >= 0) {
    DPRINTF(InOrderCPU, "Scheduling CPU Event (%s) for cycle %i, [tid:%i].\n",
            eventNames[c_event], curTick() + delay, tid);
    schedule(cpu_event, sked_tick);
  } else {
    cpu_event->process();
    cpuEventRemoveList.push(cpu_event);
  }

  // Broadcast event to the Resource Pool
  // Need to reset tid just in case this is a dummy instruction
  inst->setTid(tid);
  resPool->scheduleEvent(c_event, inst, 0, 0, tid);
}

bool InOrderCPU::isThreadActive(ThreadID tid) {
  list<ThreadID>::iterator isActive = std::find(activeThreads.begin(),
                                                activeThreads.end(), tid);

  return (isActive != activeThreads.end());
}

bool InOrderCPU::isThreadReady(ThreadID tid) {
  list<ThreadID>::iterator isReady = std::find(readyThreads.begin(),
                                               readyThreads.end(), tid);

  return (isReady != readyThreads.end());
}

bool InOrderCPU::isThreadSuspended(ThreadID tid) {
  list<ThreadID>::iterator isSuspended = std::find(suspendedThreads.begin(),
                                                   suspendedThreads.end(), tid);

  return (isSuspended != suspendedThreads.end());
}

void InOrderCPU::activateNextReadyThread() {
  if (readyThreads.size() >= 1) {
    ThreadID ready_tid = readyThreads.front();

    // Activate in Pipeline
    activateThread(ready_tid);

    // Activate in Resource Pool
    resPool->activateThread(ready_tid);

    list<ThreadID>::iterator ready_it = std::find(readyThreads.begin(),
                                                  readyThreads.end(),
                                                  ready_tid);
    readyThreads.erase(ready_it);
  } else {
    DPRINTF(
      InOrderCPU,
      "Attempting to activate new thread, but No Ready Threads to" "activate.\n");
    DPRINTF(InOrderCPU, "Unable to switch to next active thread.\n");
  }
}

void InOrderCPU::activateThread(ThreadID tid) {
  if (isThreadSuspended(tid)) {
    DPRINTF(InOrderCPU, "Removing [tid:%i] from suspended threads list.\n",
            tid);

    list<ThreadID>::iterator susp_it = std::find(suspendedThreads.begin(),
                                                 suspendedThreads.end(), tid);
    suspendedThreads.erase(susp_it);
  }

  if (threadModel == SwitchOnCacheMiss && numActiveThreads() == 1) {
    DPRINTF(
      InOrderCPU,
      "Ignoring activation of [tid:%i], since [tid:%i] is " "already running.\n",
      tid, activeThreadId());

    DPRINTF(InOrderCPU, "Placing [tid:%i] on ready threads list\n", tid);

    readyThreads.push_back(tid);

  } else if (!isThreadActive(tid)) {
    DPRINTF(InOrderCPU, "Adding [tid:%i] to active threads list.\n", tid);
    activeThreads.push_back(tid);
    if (tid < MaxSThreads) {
      DPRINTF(InOrderCPU, "Adding [tid:%i] to SPU active threads list.\n", tid);
      activeSThreads.push_back(tid);
    } else {
      DPRINTF(InOrderCPU, "Adding [tid:%i] to MPU active threads list.\n", tid);
      activeMThreads.push_back(tid);
    }

    activateThreadInPipeline(tid);

    thread[tid]->lastActivate = curTick();

    tcBase(tid)->setStatus(ThreadContext::Active);

    if (tid < MaxSThreads)
      wakeCPU();
    else
      wakeMPU();

    numCtxtSwitches++;
  }
}

void InOrderCPU::activateThreadInPipeline(ThreadID tid) {
  if (tid < MaxSThreads) {
    for (int stNum = 0; stNum < NumSStages; stNum++) {
      pipelineStage[stNum]->activateThread(tid);
    }
  } else {
    for (int stNum = 0; stNum < NumMStages; stNum++) {
      mpipelineStage[stNum]->activateThread(tid - MaxSThreads);
    }
  }
}

void InOrderCPU::deactivateContext(ThreadID tid, int delay) {
  DPRINTF(InOrderCPU, "[tid:%i]: Deactivating ...\n", tid);

  scheduleCpuEvent(DeactivateThread, NoFault, tid, dummyInst[tid], delay);

  // Be sure to signal that there's some activity so the CPU doesn't
  // deschedule itself.
  if (tid < MaxSThreads) sactivityRec.activity();
  else  mactivityRec.activity();

  _status = Running;
}

void InOrderCPU::deactivateThread(ThreadID tid) {
  DPRINTF(InOrderCPU, "[tid:%i]: Calling deactivate thread.\n", tid);

  if (isThreadActive(tid)) {
    DPRINTF(InOrderCPU, "[tid:%i]: Removing from active threads list\n", tid);
    list<ThreadID>::iterator thread_it = std::find(activeThreads.begin(),
                                                   activeThreads.end(), tid);

    removePipelineStalls(*thread_it);

    activeThreads.erase(thread_it);
    if (tid < MaxSThreads) {
      thread_it = std::find(activeSThreads.begin(), activeSThreads.end(), tid);
      activeSThreads.erase(thread_it);
    } else {
      thread_it = std::find(activeMThreads.begin(), activeMThreads.end(), tid);
      activeMThreads.erase(thread_it);
    }

    // Ideally, this should be triggered from the
    // suspendContext/Thread functions
    tcBase(tid)->setStatus(ThreadContext::Suspended);
  }

  assert(!isThreadActive(tid));
}

void InOrderCPU::removePipelineStalls(ThreadID tid) {
  DPRINTF(InOrderCPU, "[tid:%i]: Removing all pipeline stalls\n", tid);
  if (tid < MaxSThreads)
    for (int stNum = 0; stNum < NumSStages; stNum++) {
      pipelineStage[stNum]->removeStalls(tid);
    }
  else
    for (int stNum = 0; stNum < NumMStages; stNum++) {
      mpipelineStage[stNum]->removeStalls(tid - MaxSThreads);
    }

}

void InOrderCPU::updateThreadPriority() {
  if (activeThreads.size() > 1) {
    //DEFAULT TO ROUND ROBIN SCHEME
    //e.g. Move highest priority to end of thread list
    list<ThreadID>::iterator list_begin = activeThreads.begin();

    unsigned high_thread = *list_begin;

    activeThreads.erase(list_begin);

    activeThreads.push_back(high_thread);
  }
}

inline void InOrderCPU::tickThreadStats() {
  /** Keep track of cycles that each thread is active */
  list<ThreadID>::iterator thread_it = activeThreads.begin();
  while (thread_it != activeThreads.end()) {
    threadCycles[*thread_it]++;
    thread_it++;
  }

  // Keep track of cycles where SMT is active
  if (activeThreads.size() > 1) {
    smtCycles++;
  }
}

void InOrderCPU::activateContext(ThreadID tid, int delay) {
  DPRINTF(InOrderCPU, "[tid:%i]: Activating ...\n", tid);

  scheduleCpuEvent(ActivateThread, NoFault, tid, dummyInst[tid], delay);

  // Be sure to signal that there's some activity so the CPU doesn't
  // deschedule itself.
  if (tid < MaxSThreads) sactivityRec.activity();
  else  mactivityRec.activity();

  _status = Running;
}

void InOrderCPU::activateNextReadyContext(int delay) {
  DPRINTF(InOrderCPU, "Activating next ready thread\n");

  scheduleCpuEvent(ActivateNextReadyThread, NoFault, 0/*tid*/, dummyInst[0],
                   delay, ActivateNextReadyThread_Pri);

  // Be sure to signal that there's some activity so the CPU doesn't
  // deschedule itself.
  sactivityRec.activity();
  mactivityRec.activity();

  _status = Running;
}

void InOrderCPU::haltContext(ThreadID tid) {
  DPRINTF(InOrderCPU, "[tid:%i]: Calling Halt Context...\n", tid);

  scheduleCpuEvent(HaltThread, NoFault, tid, dummyInst[tid]);

  if (tid < MaxSThreads) sactivityRec.activity();
  else  mactivityRec.activity();
}

void InOrderCPU::haltThread(ThreadID tid) {
  DPRINTF(InOrderCPU, "[tid:%i]: Placing on Halted Threads List...\n", tid);
  deactivateThread(tid);
  squashThreadInPipeline(tid);
  haltedThreads.push_back(tid);

  tcBase(tid)->setStatus(ThreadContext::Halted);

  if (threadModel == SwitchOnCacheMiss) {
    activateNextReadyContext();
  }
}

void InOrderCPU::suspendContext(ThreadID tid, int delay) {
  scheduleCpuEvent(SuspendThread, NoFault, tid, dummyInst[tid], delay);
}

void InOrderCPU::suspendThread(ThreadID tid) {
  DPRINTF(InOrderCPU, "[tid:%i]: Placing on Suspended Threads List...\n", tid);
  deactivateThread(tid);
  suspendedThreads.push_back(tid);
  thread[tid]->lastSuspend = curTick();

  tcBase(tid)->setStatus(ThreadContext::Suspended);
}

void InOrderCPU::squashThreadInPipeline(ThreadID tid) {
  //Squash all instructions in each stage
  if (tid < MaxSThreads) {
    stimeBuffer.advance();
    pipelineStage[NumSStages - 1]->squash(0, tid);
    for (int stNum = NumSStages - 2; stNum >= 0; stNum--) {
      pipelineStage[stNum]->squash(0, tid);
    }
  } else {
    mtimeBuffer.advance();
    mpipelineStage[NumMStages - 1]->squash(0, tid - MaxSThreads);
    for (int stNum = NumMStages - 2; stNum >= 0; stNum--) {
      mpipelineStage[stNum]->squash(0, tid - MaxSThreads);
    }
  }
}

PipelineStage*
InOrderCPU::getPipeStage(int stage_num) {
  return pipelineStage[stage_num];
}

#if THE_ISA != MAPU_ISA
RegIndex
InOrderCPU::flattenRegIdx(RegIndex reg_idx, RegType &reg_type, ThreadID tid)
{
  if (reg_idx < FP_Base_DepTag) {
    reg_type = IntType;
    return isa[tid].flattenIntIndex(reg_idx);
  } else if (reg_idx < Ctrl_Base_DepTag) {
    reg_type = FloatType;
    reg_idx -= FP_Base_DepTag;
    return isa[tid].flattenFloatIndex(reg_idx);
  } else {
    reg_type = MiscType;
    return reg_idx - TheISA::Ctrl_Base_DepTag;
  }
}
#else // MAPU
RegIndex InOrderCPU::flattenRegIdx(RegIndex reg_idx,
                                   RegType &reg_type,
                                   ThreadID tid) {
  if (tid < MaxSThreads) {
    if (reg_idx < J_Base_DepTag) {
      reg_type = IntType;
      return isa[tid].flattenIntIndex(reg_idx);
    } else if (reg_idx < FP_Base_DepTag) {
      reg_type = IntJType;
      reg_idx -= J_Base_DepTag;
      return isa[tid].flattenIntIndex(reg_idx);
    } else if (reg_idx < DR_Base_DepTag) {
      reg_type = FloatType;
      reg_idx -= FP_Base_DepTag;
      return isa[tid].flattenFloatIndex(reg_idx);
    } else if (reg_idx < Ctrl_Base_DepTag) {
      reg_type = DoubleType;
      reg_idx -= DR_Base_DepTag;
      return isa[tid].flattenFloatIndex(reg_idx);
    } else if (reg_idx < MPU_Base_DepTag){
      reg_type = MiscType;
      return reg_idx - TheISA::Ctrl_Base_DepTag;
    } else {
      reg_type = VectorType;
      return reg_idx - MPU_Base_DepTag;
    }

  } else {
    reg_type = VectorType;
    return isa[0].flattenMPURegIndex(reg_idx);
  }
}
#endif

uint64_t InOrderCPU::readIntReg(RegIndex reg_idx, ThreadID tid) {
  DPRINTF(IntRegs, "[tid:%i]: Reading Int. Reg %i as %x\n", tid, reg_idx,
          intRegs[tid][reg_idx]);

  return intRegs[tid][reg_idx];
}

#if THE_ISA == MAPU_ISA
uint64_t InOrderCPU::readIntJReg(RegIndex reg_idx, ThreadID tid) {
  DPRINTF(IntJRegs, "[tid:%i]: Reading Int. JReg %i as %x\n", tid, reg_idx,
          intJRegs[tid][reg_idx]);

  return intJRegs[tid][reg_idx];
}

DoubleReg InOrderCPU::readDoubleReg(RegIndex reg_idx, ThreadID tid) {
  uint32_t hi, lo;
  uint64_t doublebits;
  hi = intRegs[tid][reg_idx * 2 + 1];
  lo = intRegs[tid][reg_idx * 2];
  doublebits = (((uint64_t) hi) << 32ULL) | lo;
  return *(double *) (&doublebits);
}
#endif

FloatReg InOrderCPU::readFloatReg(RegIndex reg_idx, ThreadID tid) {
  DPRINTF(FloatRegs, "[tid:%i]: Reading Float Reg %i as %x, %08f\n", tid,
          reg_idx, intRegs[tid][reg_idx], *(float *) (&(intRegs[tid][reg_idx])));

  return *(float *) (&(intRegs[tid][reg_idx]));    //floatRegs.f[tid][reg_idx];
}

FloatRegBits InOrderCPU::readFloatRegBits(RegIndex reg_idx, ThreadID tid) {
  DPRINTF(FloatRegs, "[tid:%i]: Reading Float Reg %i as %x, %08f\n", tid,
          reg_idx, intRegs[tid][reg_idx], *(float *) (&(intRegs[tid][reg_idx])));

  return intRegs[tid][reg_idx];    //floatRegs.i[tid][reg_idx];
}

DoubleRegBits InOrderCPU::readDoubleRegBits(RegIndex reg_idx, ThreadID tid) {
  DPRINTF(FloatRegs, "[tid:%i]: Reading Float Reg %i as %x, %08f\n", tid,
          reg_idx, floatRegs.i[tid][reg_idx], floatRegs.f[tid][reg_idx]);
  uint64_t bits = intRegs[tid][reg_idx * 2];
  bits |= ((uint64_t) (intRegs[tid][reg_idx * 2 + 1])) << 32ULL;
  return bits;
}

void InOrderCPU::setIntReg(RegIndex reg_idx, uint64_t val, ThreadID tid) {
  if (reg_idx == TheISA::ZeroReg) {
    DPRINTF(
      IntRegs,
      "[tid:%i]: Ignoring Setting of ISA-ZeroReg " "(Int. Reg %i) to %x\n", tid,
      reg_idx, val);
    return;
  } else {
    DPRINTF(IntRegs, "[tid:%i]: Setting Int. Reg %i to %x\n", tid, reg_idx,
            val);

    intRegs[tid][reg_idx] = val;
  }
}

#if THE_ISA == MAPU_ISA
void InOrderCPU::setIntJReg(RegIndex reg_idx, uint64_t val, ThreadID tid) {
  if (reg_idx == TheISA::ZeroReg) {
    DPRINTF(
      IntJRegs,
      "[tid:%i]: Ignoring Setting of ISA-ZeroReg " "(Int. Reg %i) to %x\n", tid,
      reg_idx, val);
    return;
  } else {
    DPRINTF(IntJRegs, "[tid:%i]: Setting Int. JReg %i to %x\n", tid, reg_idx,
            val);

    intJRegs[tid][reg_idx] = val;
  }
}

void InOrderCPU::setDoubleReg(RegIndex reg_idx, DoubleReg val, ThreadID tid) {
  uint64_t doublebits = *(uint64_t *) (&val);
  intRegs[tid][reg_idx * 2] = doublebits & 0xFFFFFFFF;
  intRegs[tid][reg_idx * 2 + 1] = doublebits >> 32;
}

void InOrderCPU::setDoubleRegBits(RegIndex reg_idx,
                                  DoubleRegBits val,
                                  ThreadID tid) {
  intRegs[tid][reg_idx * 2] = val & 0xFFFFFFFF;
  intRegs[tid][reg_idx * 2 + 1] = val >> 32;
}
#endif

void InOrderCPU::setFloatReg(RegIndex reg_idx, FloatReg val, ThreadID tid) {
  intRegs[tid][reg_idx] = *(uint32_t *) (&val); //floatRegs.f[tid][reg_idx] = val;
  DPRINTF(FloatRegs, "[tid:%i]: Setting Float. Reg %i bits to " "%x, %08f\n",
          tid, reg_idx, floatRegs.i[tid][reg_idx], floatRegs.f[tid][reg_idx]);
}

void InOrderCPU::setFloatRegBits(RegIndex reg_idx,
                                 FloatRegBits val,
                                 ThreadID tid) {
  intRegs[tid][reg_idx] = val;    //floatRegs.i[tid][reg_idx] = val;
  DPRINTF(FloatRegs, "[tid:%i]: Setting Float. Reg %i bits to " "%x, %08f\n",
          tid, reg_idx, floatRegs.i[tid][reg_idx], floatRegs.f[tid][reg_idx]);
}

uint64_t InOrderCPU::readRegOtherThread(unsigned reg_idx, ThreadID tid) {
  // If Default value is set, then retrieve target thread
  if (tid == InvalidThreadID) {
    //tid = TheISA::getTargetThread(tcBase(tid));
  }

  if (reg_idx < FP_Base_DepTag) {
    // Integer Register File
    return readIntReg(reg_idx, tid);
  } else if (reg_idx < Ctrl_Base_DepTag) {
    // Float Register File
    reg_idx -= FP_Base_DepTag;
    return readFloatRegBits(reg_idx, tid);
  } else {
    reg_idx -= Ctrl_Base_DepTag;
    return readMiscReg(reg_idx, tid);  // Misc. Register File
  }
}
void InOrderCPU::setRegOtherThread(unsigned reg_idx,
                                   const MiscReg &val,
                                   ThreadID tid) {
  // If Default value is set, then retrieve target thread
  if (tid == InvalidThreadID) {
    //tid = TheISA::getTargetThread(tcBase(tid));
  }

  if (reg_idx < FP_Base_DepTag) {            // Integer Register File
    setIntReg(reg_idx, val, tid);
  } else if (reg_idx < Ctrl_Base_DepTag) {   // Float Register File
    reg_idx -= FP_Base_DepTag;
    setFloatRegBits(reg_idx, val, tid);
  } else {
    reg_idx -= Ctrl_Base_DepTag;
    setMiscReg(reg_idx, val, tid); // Misc. Register File
  }
}

MiscReg InOrderCPU::readMiscRegNoEffect(int misc_reg, ThreadID tid) {
  return isa[0].readMiscRegNoEffect(misc_reg);
}

MiscReg InOrderCPU::readMiscReg(int misc_reg, ThreadID tid) {
  return isa[0].readMiscReg(misc_reg, tcBase(tid));
}

MPUReg &
InOrderCPU::readMPUReg(RegIndex mpu_reg, ThreadID tid) {
  return isa[0].readMPUReg(mpu_reg, tid);
}

void InOrderCPU::setMiscRegNoEffect(int misc_reg,
                                    const MiscReg &val,
                                    ThreadID tid) {
  isa[0].setMiscRegNoEffect(misc_reg, val);
}

void InOrderCPU::setMiscReg(int misc_reg, const MiscReg &val, ThreadID tid) {
  isa[0].setMiscReg(misc_reg, val, tcBase(tid));
}

void InOrderCPU::setMPUReg(RegIndex mpu_reg, const MPUReg &val, ThreadID tid) {
  isa[0].setMPUReg(mpu_reg, val, tid);
}

void InOrderCPU::setPartialMPUReg(RegIndex mpu_reg, int size, int idx,
                                  IntReg val, ThreadID tid) {
  isa[0].setPartialMPUReg(mpu_reg, size, idx, val, tid);
}

InOrderCPU::ListIt InOrderCPU::addInst(DynInstPtr inst) {
  ThreadID tid = inst->readTid();
#if THE_ISA == MAPU_ISA
  DPRINTF(
    InOrderCPU,
    "Pushing instruction [tid:%i] PC %s " "[sn:%lli] [lsn:%lli] to inst list\n",
    inst->threadNumber, inst->pcState(), inst->seqNum, inst->seqLineNum);
#endif
  instList[tid].push_back(inst);
  ListIt end = instList[tid].end();
  end--;
  return end;
}

InOrderCPU::ListIt InOrderCPU::findInst(InstSeqNum seq_num, ThreadID tid) {
  ListIt it = instList[tid].begin();
  ListIt end = instList[tid].end();

  while (it != end) {
    if ((*it)->seqNum == seq_num)
      return it;
    else if ((*it)->seqNum > seq_num)
      break;

    it++;
  }

  return instList[tid].end();
}

void InOrderCPU::updateContextSwitchStats() {
  // Set Average Stat Here, then reset to 0
  instsPerCtxtSwitch = instsPerSwitch;
  instsPerSwitch = 0;
}

void InOrderCPU::instDone(DynInstPtr inst, ThreadID tid) {
  // Set the nextPC to be fetched if this is the last instruction
  // committed
  // ========
  // This contributes to the precise state of the CPU
  // which can be used when restoring a thread to the CPU after after any
  // type of context switching activity (fork, exception, etc.)
  TheISA::PCState comm_pc = inst->pcState();
  lastCommittedPC[tid] = comm_pc;
  TheISA::advancePC(comm_pc, inst->staticInst);
  pcState(comm_pc, tid);

  //@todo: may be unnecessary with new-ISA-specific branch handling code
  if (inst->isControl()) {
    thread[tid]->lastGradIsBranch = true;
    thread[tid]->lastBranchPC = inst->pcState();
    TheISA::advancePC(thread[tid]->lastBranchPC, inst->staticInst);
  } else {
    thread[tid]->lastGradIsBranch = false;
  }

  // Finalize Trace Data For Instruction
  if (inst->traceData) {
    //inst->traceData->setCycle(curTick());
    inst->traceData->setFetchSeq(inst->seqNum);
    //inst->traceData->setCPSeq(cpu->tcBase(tid)->numInst);
    if (!inst->isNop())
      inst->traceData->dump();
    delete inst->traceData;
    inst->traceData = NULL;
  }

#if TRACING_ON
  if (inst->isMCode() && !inst->isNop()) {
    if (inst->if_tick != MaxTick)
  DPRINTFR(MaPUPipeView, "MaPUPipeView:fetch:%llu:0x%08llx:%d:%llu:%s\n",
           inst->if_tick,
           inst->pcState().pc(),
           inst->pcState().npc(),
           inst->seqNum,
           inst->staticInst->disassemble(inst->instAddr()));
    if (inst->id_tick != MaxTick)
  DPRINTFR(MaPUPipeView, "MaPUPipeView:decode:%llu\n", inst->id_tick);
  //DPRINTFR(MaPUPipeView, "MaPUPipeView:rename:%llu\n", inst->if_tick);
    if (inst->rr_tick != MaxTick)
  DPRINTFR(MaPUPipeView, "MaPUPipeView:dispatch:%llu\n", inst->rr_tick);
    if (inst->ex_tick != MaxTick)
  DPRINTFR(MaPUPipeView, "MaPUPipeView:issue:%llu\n", inst->ex_tick);
    //if (inst->wb_tick != MaxTick)
  //DPRINTFR(MaPUPipeView, "MaPUPipeView:complete:%llu\n", inst->wb_tick);
    if (inst->grad_tick != MaxTick)
  DPRINTFR(MaPUPipeView, "MaPUPipeView:retire:%llu\n", inst->grad_tick);
  }
#endif

  // Increment active thread's instruction count
  instsPerSwitch++;

  // Increment thread-state's instruction count
  thread[tid]->numInst++;
  thread[tid]->numOp++;

  // Increment thread-state's instruction stats
  thread[tid]->numInsts++;
  thread[tid]->numOps++;

  // Count committed insts per thread stats
  if (!inst->isMicroop() || inst->isLastMicroop()) {
    committedInsts[tid]++;

    // Count total insts committed stat
    totalCommittedInsts++;
  }

  committedOps[tid]++;

  // Count SMT-committed insts per thread stat
  if (numActiveThreads() > 1) {
    if (!inst->isMicroop() || inst->isLastMicroop())
      smtCommittedInsts[tid]++;
  }

  // Instruction-Mix Stats
  if (inst->isLoad()) {
    comLoads++;
  } else if (inst->isStore()) {
    comStores++;
  } else if (inst->isControl()) {
    comBranches++;
  } else if (inst->isNop()) {
    comNops++;
  } else if (inst->isNonSpeculative()) {
    comNonSpec++;
  } else if (inst->isInteger()) {
    comInts++;
  } else if (inst->isFloating()) {
    comFloats++;
  }

  // Check for instruction-count-based events.
  comInstEventQueue[tid]->serviceEvents(thread[tid]->numOp);

  // Finally, remove instruction from CPU
  removeInst(inst);
}

// currently unused function, but substitute repetitive code w/this function
// call
void InOrderCPU::addToRemoveList(DynInstPtr inst) {
  removeInstsThisCycle = true;
  if (!inst->isRemoveList()) {
    DPRINTF(InOrderCPU,
            "Pushing instruction [tid:%i] PC %s " "[sn:%lli] to remove list\n",
            inst->threadNumber, inst->pcState(), inst->seqNum);
    inst->setRemoveList();
    //removeList.push(inst->getInstListIt());
    removeListPtr.push(inst);
  } else {
    DPRINTF(
      InOrderCPU,
      "Ignoring instruction removal for [tid:%i] PC %s " "[sn:%lli], already remove list\n",
      inst->threadNumber, inst->pcState(), inst->seqNum);
  }

}

void InOrderCPU::removeInst(DynInstPtr inst) {
  DPRINTF(InOrderCPU,
          "Removing graduated instruction [tid:%i] PC %s " "[sn:%lli]\n",
          inst->threadNumber, inst->pcState(), inst->seqNum);

  removeInstsThisCycle = true;

  // Remove the instruction.
  if (!inst->isRemoveList()) {
    DPRINTF(InOrderCPU,
            "Pushing instruction [tid:%i] PC %s " "[sn:%lli] to remove list\n",
            inst->threadNumber, inst->pcState(), inst->seqNum);
    inst->setRemoveList();
    //removeList.push(inst->getInstListIt());
    removeListPtr.push(inst);
  } else {
    DPRINTF(
      InOrderCPU,
      "Ignoring instruction removal for [tid:%i] PC %s " "[sn:%lli], already on remove list\n",
      inst->threadNumber, inst->pcState(), inst->seqNum);
  }

}

void InOrderCPU::removeInstsUntil(const InstSeqNum &seq_num, ThreadID tid) {
  assert(!instList[tid].empty());

  removeInstsThisCycle = true;

  ListIt inst_iter = instList[tid].end();

  inst_iter--;

  DPRINTF(
    InOrderCPU,
    "Squashing instructions from CPU instruction " "list that are from [tid:%i] and above [sn:%lli] (end=%lli).\n",
    tid, seq_num, (*inst_iter)->seqNum);
#if THE_ISA != MAPU_ISA
  while ((*inst_iter)->seqNum > seq_num) {
#else
  while ((*inst_iter)->seqLineNum > seq_num) {
#endif
    bool break_loop = (inst_iter == instList[tid].begin());

    squashInstIt(inst_iter, tid);

    inst_iter--;

    if (break_loop)
      break;
  }
}

inline void InOrderCPU::squashInstIt(const ListIt inst_it, ThreadID tid) {
  DynInstPtr inst = (*inst_it);

  if (inst->threadNumber == tid) {
    DPRINTF(InOrderCPU, "Squashing instruction, " "[tid:%i] [sn:%lli] PC %s\n",
            inst->threadNumber, inst->seqNum, inst->pcState());

    inst->setSquashed();
    archRegDepMap[tid].remove(inst);

    if (!inst->isRemoveList()) {
      DPRINTF(
        InOrderCPU,
        "Pushing instruction [tid:%i] PC %s " "[sn:%lli] to remove list\n",
        inst->threadNumber, inst->pcState(), inst->seqNum);
      inst->setRemoveList();
      //removeList.push(inst_it);
      removeListPtr.push(inst);
    } else {
      DPRINTF(
        InOrderCPU,
        "Ignoring instruction removal for [tid:%i]" " PC %s [sn:%lli], already on remove list\n",
        inst->threadNumber, inst->pcState(), inst->seqNum);
    }

  }

}

void InOrderCPU::cleanUpRemovedInsts() {
  while (!removeListPtr.empty()) {
    DPRINTF(InOrderCPU, "Removing instruction, " "[tid:%i] [sn:%lli] PC %s\n",
            (removeListPtr.front())->threadNumber,
            (removeListPtr.front())->seqNum,
            (removeListPtr.front())->pcState());

    DynInstPtr inst = removeListPtr.front();    //*removeList.front();
    ThreadID tid = inst->threadNumber;

    // Remove From Register Dependency Map, If Necessary
    // archRegDepMap[tid].remove(inst);

    // Clear if Non-Speculative
    if (inst->staticInst && inst->seqNum == nonSpecSeqNum[tid]
        && nonSpecInstActive[tid] == true) {
      nonSpecInstActive[tid] = false;
    }

    inst->onInstList = false;

    ListIt inst_iter = findInst(inst->seqNum, tid);
    assert(inst_iter != instList[tid].end());
    instList[tid].erase(inst_iter);

    removeListPtr.pop();
  }

  removeInstsThisCycle = false;
}

void InOrderCPU::cleanUpRemovedEvents() {
  while (!cpuEventRemoveList.empty()) {
    Event *cpu_event = cpuEventRemoveList.front();
    cpuEventRemoveList.pop();
    delete cpu_event;
  }
}

void InOrderCPU::dumpInsts() {
  int num = 0;

  ListIt inst_list_it = instList[0].begin();

  cprintf("Dumping Instruction List\n");

  while (inst_list_it != instList[0].end()) {
    cprintf("Instruction:%i\nPC:%s\n[tid:%i]\n[sn:%lli]\nIssued:%i\n"
            "Squashed:%i\n\n",
            num, (*inst_list_it)->pcState(), (*inst_list_it)->threadNumber,
            (*inst_list_it)->seqNum, (*inst_list_it)->isIssued(),
            (*inst_list_it)->isSquashed());
    inst_list_it++;
    ++num;
  }
}

void InOrderCPU::wakeCPU() {
  if (/*activityRec.active() || */tickEvent.scheduled()) {
    DPRINTF(Activity, "SPU already running.\n");
    return;
  }

  DPRINTF(Activity, "Waking up SPU\n");

  Tick extra_cycles = tickToCycles((curTick() - 1) - lastRunningCycle);

  idleCycles += extra_cycles;
  for (int stage_num = 0; stage_num < NumSStages; stage_num++) {
    pipelineStage[stage_num]->idleCycles += extra_cycles;
  }

  numCycles += extra_cycles;

  schedule(&tickEvent, nextCycle(curTick()));
}

void InOrderCPU::wakeMPU() {
  if (mtickEvent.scheduled()) {
    DPRINTF(Activity, "MPU already running.\n");
    return;
  }

  DPRINTF(Activity, "Waking up MPU\n");

  Tick extra_cycles = tickToCycles((curTick() - 1) - lastRunningCycle);

  idleCycles += extra_cycles;
  for (int stage_num = 0; stage_num < NumMStages; stage_num++) {
    mpipelineStage[stage_num]->idleCycles += extra_cycles;
  }

  numCycles += extra_cycles;

  schedule(&mtickEvent, nextCycle(curTick()));
}

// Lots of copied full system code...place into BaseCPU class?
void InOrderCPU::wakeup() {
  if (thread[0]->status() != ThreadContext::Suspended)
    return;

  wakeCPU();

  DPRINTF(Quiesce, "Suspended Processor woken\n");
  threadContexts[0]->activate();
}

void InOrderCPU::syscallContext(Fault fault,
                                ThreadID tid,
                                DynInstPtr inst,
                                int delay) {
  // Syscall must be non-speculative, so squash from last stage
  unsigned squash_stage = BackEndStartStage + 1;
  inst->setSquashInfo(squash_stage);

  // Squash In Pipeline Stage
  if (inst->isMCode())
    mpipelineStage[squash_stage]->setupSquash(inst, tid - MaxSThreads);
  else
    pipelineStage[squash_stage]->setupSquash(inst, tid);

  // Schedule Squash Through-out Resource Pool
  resPool->scheduleEvent((InOrderCPU::CPUEventType) ResourcePool::SquashAll,
                         inst, 0);
  scheduleCpuEvent(Syscall, fault, tid, inst, delay, Syscall_Pri);
}

void InOrderCPU::syscall(int64_t callnum, ThreadID tid) {
  DPRINTF(InOrderCPU, "[tid:%i] Executing syscall().\n\n", tid);

  DPRINTF(Activity, "Activity: syscall() called.\n");

  // Temporarily increase this by one to account for the syscall
  // instruction.
  ++(this->thread[tid]->funcExeInst);

  // Execute the actual syscall.
  this->thread[tid]->syscall(callnum);

  // Decrease funcExeInst by one as the normal commit will handle
  // incrementing it.
  --(this->thread[tid]->funcExeInst);

  // Clear Non-Speculative Block Variable
  nonSpecInstActive[tid] = false;
}

TheISA::TLB*
InOrderCPU::getITBPtr() {
  return resPool->getInstUnit()->tlb();
}

TheISA::TLB*
InOrderCPU::getDTBPtr() {
  return resPool->getDataUnit()->tlb();
}

TheISA::Decoder *
InOrderCPU::getDecoderPtr(unsigned tid) {
  return resPool->getInstUnit()->decoder[tid];
}

Fault InOrderCPU::read(DynInstPtr inst,
                       Addr addr,
                       uint8_t *data,
                       unsigned size,
                       unsigned flags) {
  return resPool->getDataUnit()->read(inst, addr, data, size, flags);
}

Fault InOrderCPU::write(DynInstPtr inst,
                        uint8_t *data,
                        unsigned size,
                        Addr addr,
                        unsigned flags,
                        uint64_t *write_res) {
  return resPool->getDataUnit()->write(inst, data, size, addr, flags, write_res);
}

MasterPort &
InOrderCPU::getMasterPort(const string &if_name, int idx) {
  // Get the right port based on name. This applies to all the
  // subclasses of the base CPU and relies on their implementation
  // of getDataPort and getInstPort. In all cases there methods
  // return a CpuPort pointer.
  if (if_name == "dcache_port")
    return getDataPort();
  else if (if_name == "micache_port")
    return getMInstPort();
  else if (if_name == "icache_port")
    return getInstPort();
  else
    return MemObject::getMasterPort(if_name, idx);
}

int
InOrderCPU::numRunningContexts()
{
    int running = 0;
    for (int i = 0; i < threadContexts.size(); ++i) {
        if (!threadContexts[i]) continue;
        if (threadContexts[i]->status() != ThreadContext::Halted)
            ++running;
    }
    return running;
}
