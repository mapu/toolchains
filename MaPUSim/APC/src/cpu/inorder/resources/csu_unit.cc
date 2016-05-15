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

#include "config/the_isa.hh"
#include "arch/process.hh"
#include "cpu/inorder/resources/csu_unit.hh"
#include "cpu/inorder/resource_pool.hh"
#include "debug/Fault.hh"
#include "debug/InOrderCSU.hh"
#include "debug/InOrderStall.hh"
#include "sim/full_system.hh"

using namespace std;
using namespace ThePipeline;

ComSynUnit::ComSynUnit(string res_name,
                       int res_id,
                       int res_width,
                       int res_latency,
                       InOrderCPU *_cpu,
                       ThePipeline::Params *params)
  : Resource(res_name, res_id, res_width, res_latency, _cpu) {
}

void ComSynUnit::regStats() {
  csu_operation
  .name(name() + ".csu_unit").desc(
    "Number of CSU operations Executed.");

  Resource::regStats();
}

void ComSynUnit::init() {
  // Set Up Resource Events to Appropriate Resource BandWidth
  resourceEvent = new CsuAccessEvent[width];

  for (int i = 0; i < width; i++) {
    reqs[i] = new ResourceRequest(this);
  }

  initSlots();
}

void ComSynUnit::execute(int slot_num) {
  ResourceRequest* csu_req = reqs[slot_num];
  DynInstPtr inst = reqs[slot_num]->inst;

  TheLiveProcess *proc = dynamic_cast<TheLiveProcess *>(inst->tcBase()->getProcessPtr());

  if (!proc)
    fatal("Unknown process is trying to use CSU.\n");

  CsuObject *csu = proc->getCsuPtr();

  if (!csu)
    fatal("Cannot get CSU object from current thread.\n");

  Fault fault = NoFault;
  //Tick cur_tick = curTick();
  Addr chid = inst->getEA();
  TheISA::IntReg final_val = 0;

#if TRACING_ON
  InstSeqNum seq_num = inst->seqNum;
  ThreadID tid = inst->readTid();
#endif

  if (csu_req->isCompleted()) return;

  switch (csu_req->cmd) {
  case CsuRead: {
    if (inst->isCSUNumOp()) {
      DPRINTF(InOrderCSU, "[tid:%i]: [sn:%i] Reading volume of CSU Register %u completed.\n",
              tid, seq_num, chid);
      final_val = csu->readCSUNum(chid);
      inst->setIntRegOperand(inst->staticInst.get(), 0, final_val);
      if (inst->traceData) { inst->traceData->setData(final_val); }
      csu_req->done();
      return;
    }
    if (chid == 0x1B) {
      final_val = cpu->cpuId();
      inst->setIntRegOperand(inst->staticInst.get(), 0, final_val);
      if (inst->traceData) { inst->traceData->setData(final_val); }
      csu_req->done();
      return;
    }
    if (csu->readByAPE(cpu->cpuId(), chid, final_val)) {
      DPRINTF(InOrderCSU, "[tid:%i]: [sn:%i] Reading CSU Register %u completed.\n",
              tid, seq_num, chid);
      inst->setIntRegOperand(inst->staticInst.get(), 0, final_val);
      if (inst->traceData) { inst->traceData->setData(final_val); }
      if (inst->tcBase()->status() == ThreadContext::Suspended) {
        inst->tcBase()->activate(1);
      }
      csu_req->done();
    } else {
      DPRINTF(InOrderCSU, "[tid:%i]: [sn:%i] Blocked due to reading CSU Register %u.\n",
              tid, seq_num, chid);
      scheduleEvent(slot_num, 1);
      csu_req->setProcessing();
      if (inst->tcBase()->status() == ThreadContext::Active)
        inst->tcBase()->suspend(0);
    }
    break;
  }

  case CsuWrite: {
    final_val = inst->readIntSrc(0);
    if (csu->setByAPE(cpu->cpuId(), chid, final_val)) {
      if (chid == 0x11) {
        // DMA cmd was issued, a DMA Event must be scheduled
        DPRINTF(InOrderCSU, "[tid:%i]: [sn:%i] CSU DMA request is issued.\n",
                tid, seq_num);
        cpu->schedule(new DMAEvent(dynamic_cast<TheCPU *>(cpu), csu), curTick() + 2000);
      }
      DPRINTF(InOrderCSU, "[tid:%i]: [sn:%i] Writing CSU Register %u completed.\n",
              tid, seq_num, chid);
      if (inst->traceData) { inst->traceData->setData(final_val); }
      if (inst->tcBase()->status() == ThreadContext::Suspended) {
        inst->tcBase()->activate(1);
      }
      csu_req->done();
    } else {
      DPRINTF(InOrderCSU, "[tid:%i]: [sn:%i] Blocked due to writing CSU Register %u.\n",
              tid, seq_num, chid);
      scheduleEvent(slot_num, 1);
      csu_req->setProcessing();
      if (inst->tcBase()->status() == ThreadContext::Active)
        inst->tcBase()->suspend(0);
    }
    break;
  }

  default:
    fatal("Unrecognized command to %s\n", resName);
  }
}

