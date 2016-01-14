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

#include "cpu/inorder/resources/graduation_unit.hh"
#include "debug/InOrderGraduation.hh"
#include "debug/MapuPipeStages.hh"
#include "debug/MapuMem.hh"

using namespace ThePipeline;

GraduationUnit::GraduationUnit(std::string res_name, int res_id, int res_width,
                               int res_latency, InOrderCPU *_cpu,
                               ThePipeline::Params *params)
    : Resource(res_name, res_id, res_width, res_latency, _cpu)
{
    for (ThreadID tid = 0; tid < ThePipeline::MaxThreads; tid++) {
        nonSpecInstActive[tid] = &cpu->nonSpecInstActive[tid];
        nonSpecSeqNum[tid] = &cpu->nonSpecSeqNum[tid];
        lastNonSpecTick[tid] = 0;
        lastFaultTick[tid] = 0;
    }
}

void
GraduationUnit::execute(int slot_num)
{
    ResourceRequest* grad_req = reqs[slot_num];
    DynInstPtr inst = reqs[slot_num]->inst;
    ThreadID tid = inst->readTid();
    int stage_num = inst->curSkedEntry->stageNum;
    Tick cur_tick = curTick();

    //@todo: not the common case, anyway we can move this
    //       check to the stage and just ignore instructions
    //       after?
    if (lastNonSpecTick[tid] == cur_tick) {
        DPRINTF(InOrderGraduation, "Unable to graduate [sn:%i]. "
                "Only 1 nonspec inst. per cycle can graduate.\n");
        grad_req->done(false);
        return;
    }

    //@todo: use trap Pending
    if (cpu->trapPending[tid]) {
        //if (lastFaultTick[tid] == cur_tick) {
        DPRINTF(InOrderGraduation, "Unable to graduate [sn:%i]. "
                "Only 1 fault can be handled per tick.\n");
        grad_req->done(false);
        return;
    }


    switch (grad_req->cmd)
    {
      case CheckFault:
        {
            // Handle Any Faults Before Graduating Instruction
            if (inst->fault != NoFault) {
                DPRINTF(InOrderGraduation, "[tid:%i]: [sn:%i]: fault %s found for %s\n",
                        tid, inst->seqNum, inst->fault->name(),
                        inst->instName());
                squashThenTrap(stage_num, inst);
                lastFaultTick[tid] = cur_tick;
                grad_req->done(false);
                return;
            }

            DPRINTF(InOrderGraduation, "[tid:%i] [sn:%i]: No fault found for %s\n",
                    tid, inst->seqNum, inst->instName());
            grad_req->done();
        }
        break;

      case GraduateInst:
        {
            DPRINTF(InOrderGraduation,
                    "[tid:%i]:[sn:%i]: Graduating instruction %s.\n",
                    tid, inst->seqNum, inst->staticInst->disassemble(inst->instAddr()));

            // Release Non-Speculative "Block" on instructions that could not
            // execute because there was a non-speculative inst. active.
            // @TODO: Fix this functionality. Probably too conservative.
            //        Maybe it should be, non-spec. insts should block other
            //        non-spec insts because they can potentially be reading
            //        system state that will be changed by the 1st non-spec inst.
            if (inst->isNonSpeculative()) {
                *nonSpecInstActive[tid] = false;
                DPRINTF(InOrderGraduation,
                        "[tid:%i] Non-speculative inst [sn:%i] graduated\n",
                        tid, inst->seqNum);
                lastNonSpecTick[tid] = cur_tick;
            }

            if (inst->traceData) {
                inst->traceData->setStageCycle(stage_num, cur_tick);
            }

            // Tell CPU that instruction is finished processing
#if TRACING_ON
            if (inst->grad_tick == MaxTick)
              inst->grad_tick = curTick();
            if (inst->ex_tick > inst->grad_tick)
              inst->ex_tick = inst->grad_tick;
            if (inst->traceData && !inst->isNop()) {
            DPRINTF(MapuPipeStages, "[tid:%u]: [sn:%i]: [sln:%i]: 0x%08x: %s :", tid,
                    inst->seqNum, inst->seqLineNum, inst->pcState().instAddr(),
                    inst->staticInst->disassemble(inst->instAddr()));
            for (int i = 0; i < inst->traceData->stageCycle.size(); i++)
              DPRINTFR(MapuPipeStages, " %lld", inst->traceData->stageCycle[i]);
            DPRINTFR(MapuPipeStages, "\n");
            }
            if (inst->traceData->getAddrValid()) {
              if (inst->traceData->getDataStatus() != 9/*custom*/)
                DPRINTF(MapuMem, "[tid:%i]: [sn:%i]: %c Mem : 0x%x @A=0x%08x\n", tid,
                        inst->seqNum, inst->isStore() ? 'W' : 'R',
                        inst->traceData->getIntData(),
                        inst->traceData->getAddr());
              else {
                DPRINTF(MapuMem, "[tid:%i]: [sn:%i]: %c Mem : ", tid,
                        inst->seqNum, inst->isStore() ? 'W' : 'R');
                TheISA::MPUReg *mr = (TheISA::MPUReg *)inst->traceData->getCustomData();
                for (int i = 0; i < 64; i++)
                  DPRINTFR(MapuMem, "%#02x ", ((uint8_t *)(*mr))[i]);
                DPRINTFR(MapuPipeStages, "@A=0x%08x\n", inst->traceData->getAddr());
              }
            }
#endif
            cpu->instDone(inst, tid);

            grad_req->done();
        }
        break;

      default:
        fatal("Unrecognized command to %s", resName);
    }

}
