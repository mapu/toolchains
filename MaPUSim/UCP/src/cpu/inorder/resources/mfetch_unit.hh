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

#ifndef __CPU_INORDER_MFETCH_UNIT_HH__
#define __CPU_INORDER_MFETCH_UNIT_HH__

#include <list>
#include <string>
#include <vector>

#include "arch/decoder.hh"
#include "arch/tlb.hh"
#include "config/the_isa.hh"
#include "cpu/inorder/resources/localmem_unit.hh"
#include "cpu/inorder/inorder_dyn_inst.hh"
#include "cpu/inorder/pipeline_traits.hh"
#include "cpu/inorder/resource.hh"
#include "mem/packet.hh"
#include "mem/packet_access.hh"
#include "mem/port.hh"
#include "params/InOrderCPU.hh"
#include "sim/sim_object.hh"

class MFetchUnit: public LocalMemUnit {
  static const int32_t MCodeLineSize = 41;
public:
  MFetchUnit(std::string res_name, int res_id, int res_width, int res_latency,
             InOrderCPU *_cpu, ThePipeline::Params *params);

  virtual ~MFetchUnit();

  typedef ThePipeline::DynInstPtr DynInstPtr;

  struct FetchBlock {
    Addr addr;
    uint8_t *block;
    bool valid;

    FetchBlock(Addr _addr) : addr(_addr), block(NULL), valid(false) {}
  };

  /** Actions that this resource can take on an instruction */
  enum Command {
    InitiateMFetch, CompleteMFetch
  };

  ResourceRequest* getRequest(DynInstPtr _inst, int stage_num, int res_idx,
                              int slot_num, unsigned cmd);

  /** Executes one of the commands from the "Command" enum */
  void execute(int slot_num);

  virtual void squash(DynInstPtr inst, int stage_num, InstSeqNum squash_seq_num,
                      ThreadID tid);

  void trap(Fault fault, ThreadID tid, DynInstPtr inst);

  void init();

  TheISA::Decoder *decoder[ThePipeline::MaxMThreads];

private:
  void squashLocalMemRequest(LocalMemReqPtr req_ptr);

  void createMachInst(FetchBlock* fetch_it, DynInstPtr inst);

  /** After memory request is completed, then turn the fetched data
   into an instruction.
   */
  void processLocalMemCompletion(PacketPtr pkt);

  /** Create request that will interface w/TLB and Memory objects */
  virtual void setupMemRequest(DynInstPtr inst, LocalMemReqPtr localmem_req,
                               int acc_size, int flags);

  Tick ticks(int num_cycles);

  int instSize;

  int fetchBuffSize;

  /** Valid Buffer Blocks*/
  FetchBlock fetchBuffer;

  /** Buffer lines that are pending */
  std::list<FetchBlock*> pendingFetch;

  /* Check if it is in repeat mode now */
  bool isRepeating[ThePipeline::MaxMThreads];
  uint32_t ReCount[ThePipeline::MaxMThreads];
  Addr ReBlkAddr[ThePipeline::MaxMThreads];
  DynInstPtr cached_inst;
  int nextStage;
  int nextResStage;
};

#endif //__CPU_MFETCH_UNIT_HH__
