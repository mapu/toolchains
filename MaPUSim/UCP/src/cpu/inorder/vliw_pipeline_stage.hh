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

#ifndef __CPU_INORDER_VLIW_PIPELINE_STAGE_HH__
#define __CPU_INORDER_VLIW_PIPELINE_STAGE_HH__

#include <queue>
#include <vector>

#include "base/statistics.hh"
#include "cpu/inorder/comm.hh"
#include "cpu/inorder/inorder_dyn_inst.hh"
#include "cpu/inorder/pipeline_stage.hh"
#include "cpu/inorder/pipeline_traits.hh"
#include "cpu/timebuf.hh"
#include "params/InOrderCPU.hh"

class InOrderCPU;

class VliwPipelineStage: public PipelineStage {
public:
  VliwPipelineStage(Params *params, unsigned stage_num, bool SorM = false);

  virtual ~VliwPipelineStage();

protected:
  virtual void processInsts(ThreadID tid);

  /** Is there room in the next stage buffer for this instruction? */
  bool canSendInstToStage(unsigned stage_num);

  /** Separates instructions from fetch into individual lists of instructions
   * sorted by thread.
   */
  void sortInsts();

  bool sendInstToNextStage(DynInstPtr inst);
};

#endif
