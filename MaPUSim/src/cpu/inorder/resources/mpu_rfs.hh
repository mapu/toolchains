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

#ifndef __CPU_INORDER_MPU_RFS_UNIT_HH__
#define __CPU_INORDER_MPU_RFS_UNIT_HH__

#include <list>
#include <string>
#include <vector>

#include "cpu/inorder/first_stage.hh"
#include "cpu/inorder/inorder_dyn_inst.hh"
#include "cpu/inorder/pipeline_traits.hh"
#include "cpu/inorder/reg_dep_map.hh"
#include "cpu/inorder/resource.hh"
#include "cpu/func_unit.hh"

class MpuRfsUnit: public Resource {
public:
  typedef ThePipeline::DynInstPtr DynInstPtr;
  typedef TheISA::RegIndex RegIndex;

  enum Command {
    ReadSrcReg, ReadMR, WriteDestReg, WriteMR
  };

public:
  MpuRfsUnit(std::string res_name,
             int res_id,
             int res_width,
             int res_latency,
             InOrderCPU *_cpu,
             ThePipeline::Params *params);

  void init();

  ResourceRequest* getRequest(DynInstPtr _inst,
                              int stage_num,
                              int res_idx,
                              int slot_num,
                              unsigned cmd);

  ResReqPtr findRequest(DynInstPtr inst);

  void execute(int slot_num);

  void exeMRegreq(RegIndex destIdx, int slot_num);

  void regStats();

  Tick ticks(int num_cycles);

  void requestAgain(DynInstPtr inst, bool &try_request);

protected:
  RegDepMap *regDepMap[ThePipeline::MaxThreads];
  // Translate the slot No. to unit's groupid_unitid
  RegIndex sid2uid(DynInstPtr _inst) const;
  bool *nonSpecInstActive[ThePipeline::MaxMThreads];
  InstSeqNum *nonSpecSeqNum[ThePipeline::MaxMThreads];

  bool serializeOnNextInst[ThePipeline::MaxMThreads];
  InstSeqNum serializeAfterSeqNum[ThePipeline::MaxMThreads];

public:
  class MpuRfsRequest: public ResourceRequest {
  public:
    typedef ThePipeline::DynInstPtr DynInstPtr;

  public:
    MpuRfsRequest(MpuRfsUnit *res)
      : ResourceRequest(res), mpuRfsIdx(0), destIdx(0), delay(0) {
    }

    TheISA::RegIndex mpuRfsIdx;

    void setRequest(DynInstPtr _inst,
                    int stage_num,
                    int res_idx,
                    int slot_num,
                    unsigned _cmd,
                    TheISA::RegIndex idx) {
      mpuRfsIdx = idx;

      ResourceRequest::setRequest(_inst, stage_num, res_idx, slot_num, _cmd);
    }

    TheISA::RegIndex destIdx;
    Tick delay;
  };

protected:
  /** M Register File Reads */
  Stats::Scalar MRegFileReads;

  /** M Register File Writes */
  Stats::Scalar MRegFileWrites;

  /** M Register File Total Accesses (Read+Write) */
  Stats::Formula MRegFileAccs;

  /** SHU0 T Register File Reads */
  Stats::Scalar Shu0TRegFileReads;

  /** SHU0 T Register File Writes */
  Stats::Scalar Shu0TRegFileWrites;

  /** SHU0 T Register File Total Accesses (Read+Write) */
  Stats::Formula Shu0TRegFileAccs;

  /** SHU1 T Register File Reads */
  Stats::Scalar Shu1TRegFileReads;

  /** SHU1 T Register File Writes */
  Stats::Scalar Shu1TRegFileWrites;

  /** SHU1 T Register File Total Accesses (Read+Write) */
  Stats::Formula Shu1TRegFileAccs;

  /** IALU T Register File Reads */
  Stats::Scalar IaluTRegFileReads;

  /** IALU T Register File Writes */
  Stats::Scalar IaluTRegFileWrites;

  /** IALU T Register File Total Accesses (Read+Write) */
  Stats::Formula IaluTRegFileAccs;

  /** FALU T Register File Reads */
  Stats::Scalar FaluTRegFileReads;

  /** FALU T Register File Writes */
  Stats::Scalar FaluTRegFileWrites;

  /** FALU T Register File Total Accesses (Read+Write) */
  Stats::Formula FaluTRegFileAccs;

  /** IMAC T Register File Reads */
  Stats::Scalar ImacTRegFileReads;

  /** IMAC T Register File Writes */
  Stats::Scalar ImacTRegFileWrites;

  /** IMAC T Register File Total Accesses (Read+Write) */
  Stats::Formula ImacTRegFileAccs;

  /** FMAC T Register File Reads */
  Stats::Scalar FmacTRegFileReads;

  /** FMAC T Register File Writes */
  Stats::Scalar FmacTRegFileWrites;

  /** FMAC T Register File Total Accesses (Read+Write) */
  Stats::Formula FmacTRegFileAccs;

  class MpuRfsEvent: public ResourceEvent {
  public:
    MpuRfsEvent()
      : ResourceEvent() {}
    ~MpuRfsEvent() {}

    void process();

  };
};


#endif //__CPU_INORDER_USE_DEF_UNIT_HH__
