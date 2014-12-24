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

#include "base/str.hh"
#include "config/the_isa.hh"
#include "cpu/inorder/cpu.hh"
#include "cpu/inorder/vliw_pipeline_stage.hh"
#include "cpu/inorder/resource_pool.hh"
#include "debug/Activity.hh"
#include "debug/InOrderDynInst.hh"
#include "debug/InOrderStage.hh"
#include "debug/InOrderStall.hh"
#include "debug/Resource.hh"
#include "debug/ThreadModel.hh"

using namespace std;
using namespace ThePipeline;

VliwPipelineStage::VliwPipelineStage(Params *params, unsigned stage_num,
                                     bool SorM) :
    PipelineStage(params, stage_num, SorM) {
  init(params);
}

VliwPipelineStage::~VliwPipelineStage() {
  for (ThreadID tid = 0; tid < numThreads; tid++) {
    skidBuffer[tid].clear();
    stalls[tid].resources.clear();
  }
}

bool VliwPipelineStage::canSendInstToStage(unsigned stage_num) {
  bool buffer_avail = false;

  if (pipetype) {
    if (cpu->mpipelineStage[stage_num]->prevStageValid) {
      buffer_avail =
        cpu->mpipelineStage[stage_num]->stageBufferAvail() == stageBufferMax;
    }
  } else {
    if (cpu->pipelineStage[stage_num]->prevStageValid) {
      buffer_avail =
        cpu->pipelineStage[stage_num]->stageBufferAvail() == stageBufferMax;
    }
  }

  if (!buffer_avail && nextStageQueueValid(stage_num)) {
    DPRINTF(InOrderStall, "STALL: No room in stage %i buffer.\n", stageNum + 1);
  }

  return buffer_avail;
}

void VliwPipelineStage::sortInsts() {
  if (prevStageValid) {
    assert(prevStage->insts.size() <= stageWidth);

    int insts_from_prev_stage = prevStage->insts.size();
    if (!insts_from_prev_stage) return;

    ThreadID tid = prevStage->insts[0]->threadNumber;
    tid = pipetype ? tid - MaxSThreads : tid;

#if TRACING_ON
    int insts_from_cur_stage = skidSize();
#endif
    DPRINTF(InOrderStage, "%i insts available from stage buffer %i. Stage "
            "currently has %i insts from last cycle.\n",
            insts_from_prev_stage, prevStageQueue->id(), insts_from_cur_stage);

    //if (!insts_from_cur_stage && !checkStall(tid)) {
      DPRINTF(InOrderStage, "[tid:%i]: Inserting %d insts begin from [sn:%i] "
              "into stage buffer.\n", prevStage->insts[0]->readTid(),
              prevStage->insts.size(), prevStage->insts[0]->seqNum);
      skidBuffer[tid].insert(skidBuffer[tid].end(), prevStage->insts.begin(), prevStage->insts.end());
      prevStage->insts.clear();
      return;
    //}
/*
    // Current VLIW stage is not empty and should not accept new inst lines.
    PipelineStage **cpu_pipeline = pipetype ? cpu->mpipelineStage
                                            : cpu->pipelineStage;

    int prev_stage = stageNum - 1;
    DPRINTF(InOrderStage, "VLIW Stage %i has accepted an inst line "
            "possible for this tick. Placing %d insts from [sn:%i] "
            "in stage %i skidBuffer\n", stageNum, prevStage->insts.size(),
            prevStage->insts[0]->seqNum, stageNum - 1);
    cpu_pipeline[prev_stage]->
      skidBuffer[tid].insert(cpu_pipeline[prev_stage]->skidBuffer[tid].begin(),
                             prevStage->insts.begin(),
                             prevStage->insts.end());
    prevStage->insts.clear();

    if (cpu_pipeline[prev_stage]->stageStatus[tid] == Running ||
        cpu_pipeline[prev_stage]->stageStatus[tid] == Idle) {
      cpu_pipeline[prev_stage]->stageStatus[tid] = Unblocking;
    }*/
  }
}

void VliwPipelineStage::processInsts(ThreadID tid) {
  // Instructions can come either from the skid buffer or the list of
  // instructions coming from fetch, depending on stage's status.
  int insts_available = skidBuffer[tid].size();

  std::list<DynInstPtr> &insts_to_stage = skidBuffer[tid];

  if (insts_available == 0 ||
      (nextStageValid && !canSendInstToStage(stageNum + 1))) {
    DPRINTF(InOrderStage, "[tid:%u]: Nothing to do, breaking out"
            " early.\n", cpu_tid(tid));
    return;
  }

  DynInstPtr inst;
  bool last_req_completed = true;

  bool pkg_completed = false;
  std::list<DynInstPtr> insts_processed;

  while ((instsRepeated || !pkg_completed) && instsProcessed < stageWidth &&
         last_req_completed) {
    if (insts_to_stage.empty()) break;

    inst = insts_to_stage.front();
    if (inst->isPackageEnd()) pkg_completed = true;

    if (inst->reqCompleted) {
      insts_processed.push_back(inst);
      insts_to_stage.pop_front();
      DPRINTF(InOrderStage, "[tid:%i] [sn:%i] was processed in stage %i.\n",
              cpu_tid(tid), insts_processed.back()->seqNum, stageNum);
      ++instsProcessed;
      continue;
    }

    DPRINTF(InOrderStage, "[tid:%u]: Processing instruction [sn:%lli] "
            "%s with PC %s\n", cpu_tid(tid), inst->seqNum, inst->instName(),
            inst->pcState());

    if (inst->isSquashed()) {
      DPRINTF(InOrderStage, "[tid:%u]: Instruction %i with PC %s is "
              "squashed, skipping.\n", cpu_tid(tid), inst->seqNum,
              inst->pcState());

      insts_to_stage.pop_front();
      if (instsProcessed && inst->isPackageEnd()) {
        insts_processed.back()->setPackageEnd();
        DPRINTF(InOrderStage, "[tid:%u]: Pass package end from %i to %i.\n",
                cpu_tid(tid), inst->seqNum, insts_processed.back()->seqNum);
      }
      continue;
    }

    int reqs_processed = 0;
    last_req_completed = processInstSchedule(inst, reqs_processed);

    // If the instruction isnt squashed & we've completed one request
    // Then we can officially count this instruction toward the stage's
    // bandwidth count
    if (reqs_processed > 0 || instsRepeated) instsProcessed++;

    // Don't let instruction pass to next stage if it hasnt completed
    // all of it's requests for this stage.
    if (!last_req_completed) {
      if (instsRepeated) {
        last_req_completed = true;
      }
      continue;
    }

    insts_processed.push_back(inst);
    inst->reqCompleted = true;

    insts_to_stage.pop_front();
  }

  DynInstPtr pre = NULL;
  if (last_req_completed && pkg_completed) {
    while (!insts_processed.empty()) {
      // Send to Next Stage or Break Loop
      if (insts_processed.front()->isFinished) {
        DPRINTF(InOrderDynInst, "[tid:%i] [sn:%i] done in stage %i.\n",
                cpu_tid(tid), insts_processed.front()->seqNum, stageNum);
        if (insts_processed.front()->isPackageEnd() && pre) {
          pre->setPackageEnd();
          DPRINTF(InOrderStage, "[tid:%u]: Pass package end from %i to %i.\n",
                  cpu_tid(tid), insts_processed.front()->seqNum, pre->seqNum);
        }
      } else {
        pre = insts_processed.front();
        if (nextStageValid &&
            !sendInstToNextStage(insts_processed.front())) {
          last_req_completed = false;
          DPRINTF(InOrderStage, "[tid:%i] [sn:%i] unable to proceed to stage"
                  " %i.\n", cpu_tid(tid), insts_processed.front()->seqNum,
                  insts_processed.front()->nextStage);
          break;
        }
      }
      insts_processed.front()->reqCompleted = false;
      insts_processed.pop_front();
    }
  }
  if (!insts_processed.empty()) {
    insts_to_stage.insert(insts_to_stage.begin(), insts_processed.begin(),
                          insts_processed.end());
    insts_processed.clear();
  }

  // If we didn't process all instructions, then we will need to block
  // and put all those instructions into the skid buffer.
  if (!insts_to_stage.empty() && !instsRepeated) {
    DPRINTF(InOrderStage, "[tid:%i] [sn:%i] %d of %d, vi: %d of %d\n",
            cpu_tid(tid), inst->seqNum, instsProcessed, stageWidth,
            insts_available, skidBuffer[tid].size());
    blockDueToBuffer(tid);
  }

  // Record that stage has written to the time buffer for activity
  // tracking.
  if (instsProcessed) {
    wroteToTimeBuffer = true;
  }
}

bool VliwPipelineStage::sendInstToNextStage(DynInstPtr inst) {
  // Update Next Stage Variable in Instruction
  // NOTE: Some Resources will update this nextStage var. to
  // for bypassing, so can't always assume nextStage=stageNum+1
  if (inst->nextStage == stageNum) inst->nextStage++;

  bool success = false;
  ThreadID tid = inst->readTid();
  int next_stage = inst->nextStage;
  int prev_stage = next_stage - 1;

  assert(next_stage >= 1);
  assert(prev_stage >= 0);

  DPRINTF(InOrderStage, "[tid:%u]: Attempting to send instructions to "
          "stage %u.\n", tid, stageNum+1);

  if (nextStageQueueValid(inst->nextStage - 1)) {
#if THE_ISA == MAPU_ISA
    if (inst->seqLineNum > cpu->squashLineSeqNum[tid] &&
#else
    if (inst->seqNum > cpu->squashSeqNum[tid] &&
#endif
        curTick() == cpu->lastSquashCycle[tid]) {
      DPRINTF(InOrderStage, "[tid:%u]: [sn:%i]: squashed, skipping "
              "insertion into stage %i queue.\n", tid, inst->seqNum,
              inst->nextStage);
    } else {
      if (nextStageValid) {
        DPRINTF(InOrderStage, "[tid:%u] %i slots available in next "
                "stage buffer.\n", tid,
                pipetype ? cpu->mpipelineStage[next_stage]->stageBufferAvail()
                         : cpu->pipelineStage[next_stage]->stageBufferAvail());
      }

      DPRINTF(InOrderStage, "[tid:%u]: [sn:%i]: being placed into "
              "index %i of stage buffer %i queue.\n", tid, inst->seqNum,
          pipetype ? cpu->mpipelineStage[prev_stage]->nextStage->insts.size()
                   : cpu->pipelineStage[prev_stage]->nextStage->insts.size(),
          pipetype ? cpu->mpipelineStage[prev_stage]->nextStageQueue->id()
                   : cpu->pipelineStage[prev_stage]->nextStageQueue->id());

      // Place instructions in inter-stage communication struct for next
      // pipeline stage to read next cycle
      if (pipetype)
        cpu->mpipelineStage[prev_stage]->nextStage->insts.push_back(inst);
      else
        cpu->pipelineStage[prev_stage]->nextStage->insts.push_back(inst);

      success = true;

      // Take note of trace data for this inst & stage
      if (inst->traceData) {
        //@todo: exec traces are broke. fix them
        inst->traceData->setStageCycle(stageNum, curTick());
      }
#if TRACING_ON
      if (inst->nextStage == BackEndStartStage)
        inst->id_tick = curTick() + 1000;
      if (inst->nextStage == BackEndStartStage + 1) {
        inst->rr_tick = curTick() + 1000;
        inst->ex_tick = curTick() + 2000;
      }
#endif
    }
  }

  return success;
}
