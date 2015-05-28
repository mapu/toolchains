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
#include "config/the_isa.hh"
#include "cpu/inorder/resources/mpu_rfs.hh"
#include "cpu/inorder/cpu.hh"
#include "cpu/inorder/pipeline_traits.hh"
#include "debug/InOrderStall.hh"
#include "debug/InOrderMpuRfs.hh"
#include "debug/MapuReg.hh"

using namespace std;
using namespace TheISA;
using namespace ThePipeline;

MpuRfsUnit::MpuRfsUnit(string res_name,
                       int res_id,
                       int res_width,
                       int res_latency,
                       InOrderCPU *_cpu,
                       ThePipeline::Params *params)
  : Resource(res_name, res_id, res_width, res_latency, _cpu) {
  for (ThreadID tid = 0; tid < ThePipeline::MaxMThreads; tid++) {
    nonSpecInstActive[tid] = &cpu->nonSpecInstActive[tid];
    nonSpecSeqNum[tid] = &cpu->nonSpecSeqNum[tid];
    serializeOnNextInst[tid] = false;
    serializeAfterSeqNum[tid] = 0;
    regDepMap[tid] = &cpu->archRegDepMap[tid];
  }

}

void MpuRfsUnit::regStats() {
  MRegFileReads.name(name() + ".MRegFileReads").desc(
    "Number of Reads from M Register File");

  MRegFileWrites.name(name() + ".MRegFileWrites").desc(
    "Number of Writes to M Register File");

  MRegFileAccs.name(name() + ".MRegFileAccs").desc(
    "Total Accesses (Read+Write) to M Register File");
  MRegFileAccs = MRegFileReads + MRegFileWrites;

  Shu0TRegFileReads.name(name() + ".Shu0TRegFileReads").desc(
    "Number of Reads from SHU0 T Register File");

  Shu0TRegFileWrites.name(name() + ".Shu0TRegFileWrites").desc(
    "Number of Writes to SHU0 T Register File");

  Shu0TRegFileAccs.name(name() + ".Shu0TRegFileAccs").desc(
    "Total Accesses (Read+Write) to SHU0 T Register File");
  Shu0TRegFileAccs = Shu0TRegFileReads + Shu0TRegFileWrites;

  Shu1TRegFileReads.name(name() + ".Shu1TRegFileReads").desc(
    "Number of Reads from FALU T Register File");

  Shu1TRegFileWrites.name(name() + ".Shu1TRegFileWrites").desc(
    "Number of Writes to FALU T Register File");

  Shu1TRegFileAccs.name(name() + ".Shu1TRegFileAccs").desc(
    "Total Accesses (Read+Write) to FALU T Register File");
  Shu1TRegFileAccs = Shu1TRegFileReads + Shu1TRegFileWrites;

  IaluTRegFileReads.name(name() + ".IaluTRegFileReads").desc(
    "Number of Reads from IALU T Register File");

  IaluTRegFileWrites.name(name() + ".IaluTRegFileWrites").desc(
    "Number of Writes to IALU T Register File");

  IaluTRegFileAccs.name(name() + ".IaluTRegFileAccs").desc(
    "Total Accesses (Read+Write) to IALU T Register File");
  IaluTRegFileAccs = IaluTRegFileReads + IaluTRegFileWrites;

  FaluTRegFileReads.name(name() + ".FaluTRegFileReads").desc(
    "Number of Reads from FALU T Register File");

  FaluTRegFileWrites.name(name() + ".FaluTRegFileWrites").desc(
    "Number of Writes to FALU T Register File");

  FaluTRegFileAccs.name(name() + ".FaluTRegFileAccs").desc(
    "Total Accesses (Read+Write) to FALU T Register File");
  FaluTRegFileAccs = FaluTRegFileReads + FaluTRegFileWrites;

  ImacTRegFileReads.name(name() + ".ImacTRegFileReads").desc(
    "Number of Reads from IMAC T Register File");

  ImacTRegFileWrites.name(name() + ".ImacTRegFileWrites").desc(
    "Number of Writes to IMAC T Register File");

  ImacTRegFileAccs.name(name() + ".ImacTRegFileAccs").desc(
    "Total Accesses (Read+Write) to IMAC T Register File");
  ImacTRegFileAccs = ImacTRegFileReads + ImacTRegFileWrites;

  FmacTRegFileReads.name(name() + ".FmacTRegFileReads").desc(
    "Number of Reads from FMAC T Register File");

  FmacTRegFileWrites.name(name() + ".FmacTRegFileWrites").desc(
    "Number of Writes to FMAC T Register File");

  FmacTRegFileAccs.name(name() + ".FmacTRegFileAccs").desc(
    "Total Accesses (Read+Write) to FMAC T Register File");
  FmacTRegFileAccs = FmacTRegFileReads + FmacTRegFileWrites;

  Resource::regStats();
}

void MpuRfsUnit::init() {
  // Set Up Resource Events to Appropriate Resource BandWidth
  resourceEvent = new MpuRfsEvent[width];

  for (int i = 0; i < width; i++) {
    reqs[i] = new MpuRfsRequest(this);
  }

  initSlots();
}

ResReqPtr MpuRfsUnit::getRequest(DynInstPtr inst,
                                 int stage_num,
                                 int res_idx,
                                 int slot_num,
                                 unsigned cmd) {
  MpuRfsRequest *mr_req = dynamic_cast<MpuRfsRequest*>(reqs[slot_num]);
  mr_req->setRequest(inst, stage_num, id, slot_num, cmd,
                     inst->curSkedEntry->idx);
  return mr_req;
}

ResReqPtr MpuRfsUnit::findRequest(DynInstPtr inst) {
  for (int i = lastReqIdx; i < width; i++) {
    MpuRfsRequest* mr_req = dynamic_cast<MpuRfsRequest*>(reqs[i]);
    assert(mr_req);

    if (mr_req->valid && mr_req->getInst() == inst
        && mr_req->cmd == inst->curSkedEntry->cmd
        && mr_req->mpuRfsIdx == inst->curSkedEntry->idx) {
      lastReqIdx = i;
      return mr_req;
    }
  }
  for (int i = 0; i < lastReqIdx; i++) {
    MpuRfsRequest* mr_req = dynamic_cast<MpuRfsRequest*>(reqs[i]);
    assert(mr_req);

    if (mr_req->valid && mr_req->getInst() == inst
        && mr_req->cmd == inst->curSkedEntry->cmd
        && mr_req->mpuRfsIdx == inst->curSkedEntry->idx) {
      lastReqIdx = i;
      return mr_req;
    }
  }

  return NULL;
}

void MpuRfsUnit::exeMRegreq(RegIndex dest_idx, int slot_idx) {
  MpuRfsRequest* mr_req = dynamic_cast<MpuRfsRequest*>(reqs[slot_idx]);
  DynInstPtr inst = mr_req->inst;
  int mr_idx = mr_req->mpuRfsIdx;
  DPRINTF(InOrderMpuRfs, "[tid:%i]: [sn:%i]: Scheduled Writing Vector Reg %i"
          " to Register File.\n", inst->readTid(), inst->seqNum,
          dest_idx);
  cpu->setMPUReg(dest_idx, inst->readMPURegResult(mr_idx), inst->readTid());
#if TRACING_ON
  if (inst->wb_tick == MaxTick)
    inst->wb_tick = curTick();
#endif
  inst->setExecuted();
  mr_req->done();
  mr_req->setProcessing(false);
  mr_req->freeSlot();
}

void MpuRfsUnit::execute(int slot_idx) {
  MpuRfsRequest* mr_req = dynamic_cast<MpuRfsRequest*>(reqs[slot_idx]);
  DynInstPtr inst = mr_req->inst;
  ThreadID tid = inst->readTid();
  int mr_idx = mr_req->mpuRfsIdx;
#if TRACING_ON
  InstSeqNum seq_num = inst->seqNum;
#endif

  if (inst->fault != NoFault) {
    DPRINTF(InOrderMpuRfs, "[tid:%i]: [sn:%i]: Detected %s fault @ %x. Forwarding to "
            "next stage.\n", tid, inst->seqNum, inst->fault->name(),
            inst->pcState());
    mr_req->done();
    return;
  }

  switch (mr_req->cmd) {
  case ReadSrcReg: {
    InOrderCPU::RegType reg_type;
    RegIndex reg_idx = inst->_srcRegIdx[mr_idx];
    // Uniform the register index format into {index, unitid, groupid}.
    // This will help isa flatten the index
    if (reg_idx >= 128)
      reg_idx = (reg_idx >> 5) | (inst->pcState().sn() << 4);
      // This is M[I++] or M[S++], so record the slot num at <7:5>
    else
      reg_idx = (reg_idx << 4) | sid2uid(inst);
    RegIndex flat_idx = cpu->flattenRegIdx(reg_idx, reg_type, tid);
    //inst->flattenSrcReg(mr_idx, flat_idx);

    if (flat_idx == -1) {
      // IMAC extended result write-back may cause writing to high reg
      // BIU, ignore it
      fatal("reading BIU high register.\n");
      mr_req->done();
      return;
    }

    switch (reg_type) {
    case InOrderCPU::VectorType: {
      DPRINTF(InOrderMpuRfs, "[tid:%i]: [sn:%i]: Reading Vector Reg %i"
              " (%i) from Register File.\n", tid, seq_num,
              reg_idx, flat_idx);
      inst->setMPUSrc(mr_idx, cpu->readMPUReg(flat_idx, tid));

      // For MaPU GUI trace
      DPRINTF(MapuReg, "[sn:%lli] : R MPU Reg %i : ", inst->seqNum, flat_idx);
      for (int i = 0; i < 64; i++)
        DPRINTFR(MapuReg, "%#02x ", cpu->readMPUReg(flat_idx, tid)[i]);
      DPRINTFR(MapuReg, "\n");
#if TRACING_ON
      if (inst->rr_tick == MaxTick)
        inst->rr_tick = curTick();
#endif
      MRegFileReads++;
    }
      break;

    default:
      panic("Invalid MPU Register Type: %i", reg_type);
    }

    mr_req->done();
  }
    break;

  case ReadMR: {
    RegIndex reg_idx = inst->_srcRegIdx[mr_idx];
    DPRINTF(InOrderMpuRfs, "[tid:%i]: [sn:%i]: Reading Vector Reg %i"
            " (%i) from Register File.\n", tid, seq_num, reg_idx, reg_idx);
    inst->setMPUSrc(mr_idx, cpu->readMPUReg(reg_idx, tid));

    // For MaPU GUI trace
    DPRINTF(MapuReg, "[sn:%lli] : R MPU Reg %i : ", inst->seqNum, reg_idx);
    for (int i = 0; i < 64; i++)
      DPRINTFR(MapuReg, "%#02x ", cpu->readMPUReg(reg_idx, tid)[i]);
    DPRINTFR(MapuReg, "\n");
    mr_req->done();
  }
    break;

  case WriteDestReg: {
    InOrderCPU::RegType reg_type;
    RegIndex reg_idx = inst->_destRegIdx[mr_idx];
    if (reg_idx == 0x4) reg_idx |= inst->pcState().sn() << 4; // M[I++], record the slot
    RegIndex flat_idx;
    if (inst->resultType(mr_idx) == InOrderDynInst::Vector)
      flat_idx = cpu->flattenRegIdx(reg_idx, reg_type, tid);
    else
      flat_idx = cpu->flattenRegIdx(reg_idx, reg_type, tid - MaxSThreads);

    if (flat_idx == 0xFFFF) {
      DPRINTF(InOrderMpuRfs, "[tid:%i]: [sn:%i]: Writing BIU high reg, bypassed.\n",
              tid, seq_num);
      mr_req->done();
      return;
    }

    switch (reg_type) {
    case InOrderCPU::VectorType: {
      DPRINTF(InOrderMpuRfs, "[tid:%i]: [sn:%i]: Writing Vector Result "
              "to register idx %i (%i).\n", tid, seq_num,
              reg_idx, flat_idx);

      // For MaPU GUI trace
      DPRINTF(MapuReg, "[sn:%lli] : W MPU Reg %i : ", inst->seqNum, flat_idx);
      for (int i = 0; i < 64; i++)
        DPRINTFR(MapuReg, "%#02x ", inst->readMPURegResult(mr_idx)[i]);
      DPRINTFR(MapuReg, "\n");

      /*if ((reg_type == InOrderCPU::VectorType && flat_idx < NumMRegs) ||
          (inst->opClass() == IntMultOp && mr_idx == 3)) {
        DPRINTF(InOrderMpuRfs,
                "[tid:%i]: [sn:%i]: Delayed writing to M Regfile.\n",
                tid, seq_num);

        if ((inst->opClass() == IntMultOp && mr_idx == 3) && flat_idx < NumMRegs)
          mr_req->delay = ticks(1);
        else
          mr_req->delay = 0;
        mr_req->destIdx = flat_idx;
        scheduleEvent(slot_idx, 1);
        mr_req->setProcessing();
        mr_req->setCompleted();
        MRegFileWrites++;
        return;
      }*/
#if TRACING_ON
      if (inst->wb_tick == MaxTick)
        inst->wb_tick = curTick();
#endif
      cpu->setMPUReg(flat_idx, inst->readMPURegResult(mr_idx), tid);
    }
      break;

    case InOrderCPU::MiscType: {
      DPRINTF(InOrderMpuRfs, "[tid:%i]: Writing Misc. 0x%x "
              "to register idx %i.\n",
              tid, inst->readIntResult(mr_idx), reg_idx - Ctrl_Base_DepTag);

      cpu->setMiscReg(flat_idx,
                      inst->readIntResult(mr_idx),
                      tid);

      DPRINTF(MapuReg, "[sn:%lli] : W Misc Reg %i : %#llx",
              inst->seqNum, reg_idx - Ctrl_Base_DepTag, inst->readIntResult(mr_idx));
      break;

    }

    default:
      panic("Invalid MPU Register Type: %i", reg_type);
    }
    mr_req->done();
  }
    break;

  case WriteMR: {
    RegIndex reg_idx = inst->_destRegIdx[mr_idx];

    DPRINTF(InOrderMpuRfs, "[tid:%i]: [sn:%i]: Writing Vector Result "
            "to register idx %i (%i).\n", tid, seq_num, /*inst->readIntResult(ud_idx),*/
            reg_idx, reg_idx);

    // For MaPU GUI trace
    DPRINTF(MapuReg, "[sn:%lli] : W MPU Reg %i : ", inst->seqNum, reg_idx);
    for (int i = 0; i < 64; i++)
      DPRINTFR(MapuReg, "%#02x ", inst->readMPURegResult(mr_idx)[i]);
    DPRINTFR(MapuReg, "\n");

    cpu->setMPUReg(reg_idx, inst->readMPURegResult(mr_idx), tid);
    mr_req->done();
  }
    break;

  default:
    fatal("Unrecognized command to %s", resName);
  }
}

inline RegIndex MpuRfsUnit::sid2uid(DynInstPtr _inst) const {
  const RegIndex UnitID[NumSlots] = { 0x0, 0x0, 0x0, 0x0, 0x1, 0x5, 0x3, 0x7,
                                      0xB, 0xF, 0x2, 0x6, 0xA, 0x0 };
  return UnitID[_inst->pcState().sn()];
}

Tick MpuRfsUnit::ticks(int num_cycles) {
  return cpu->ticks(num_cycles) / 2;
}

void MpuRfsUnit::requestAgain(DynInstPtr inst, bool &service_request) {
  ResReqPtr mr_req = findRequest(inst);
  assert(mr_req);

  service_request = true;

  // Check to see if this instruction is requesting the same command
  // or a different one
  if (mr_req->cmd != inst->curSkedEntry->cmd) {
    // If different, then update command in the request
    mr_req->cmd = inst->curSkedEntry->cmd;
    DPRINTF(InOrderMpuRfs,
            "[tid:%i]: [sn:%i]: Updating the command for this " "instruction\n",
            inst->readTid(), inst->seqNum);
  } else {
    // If same command, just check to see if access was completed
    // but dont try to re-execute
    DPRINTF(InOrderMpuRfs,
            "[tid:%i]: [sn:%i]: requesting this resource again\n",
            inst->readTid(), inst->seqNum);
  }
}

void MpuRfsUnit::MpuRfsEvent::process() {

  MpuRfsRequest* mr_req = reinterpret_cast<MpuRfsRequest *>(resource->reqs[slotIdx]);

  MpuRfsUnit* mr_res = reinterpret_cast<MpuRfsUnit*>(resource);

  if (mr_res->cpu->mpipelineStage[mr_req->getStageNum()]->isBlocked(mr_req->getTid())) {
    scheduleEvent(1);
    return;
  }

  if (mr_req->delay != 0) {
    mr_req->delay -= mr_res->ticks(1);
    scheduleEvent(1);
    return;
  }

  if (mr_req->isSquashed()) {
    mr_req->freeSlot();
    return;
  }

  mr_res->exeMRegreq(mr_req->destIdx, slotIdx);
}
