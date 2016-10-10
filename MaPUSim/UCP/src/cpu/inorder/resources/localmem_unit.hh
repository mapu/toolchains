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

#ifndef __CPU_INORDER_LOCALMEM_UNIT_HH__
#define __CPU_INORDER_LOCALMEM_UNIT_HH__

#include <list>
#include <string>
#include <vector>

#include "arch/tlb.hh"
#include "base/hashmap.hh"
#include "config/the_isa.hh"
#include "cpu/inorder/inorder_dyn_inst.hh"
#include "cpu/inorder/pipeline_traits.hh"
#include "cpu/inorder/resource.hh"
#include "mem/packet.hh"
#include "mem/packet_access.hh"
#include "mem/port.hh"
#include "params/InOrderCPU.hh"
#include "sim/sim_object.hh"

class LocalMemReqPacket;
typedef LocalMemReqPacket* LocalMemReqPktPtr;

class LocalMemRequest;
typedef LocalMemRequest* LocalMemReqPtr;

class LocalMemUnit: public Resource {
public:
  typedef ThePipeline::DynInstPtr DynInstPtr;

public:
  LocalMemUnit(std::string res_name, int res_id, int res_width, int res_latency,
               InOrderCPU *_cpu, ThePipeline::Params *params);

  enum Command {
    InitiateReadData, CompleteReadData, InitiateWriteData, CompleteWriteData
  };

public:

  void init();

  ResourceRequest* getRequest(DynInstPtr _inst, int stage_num, int res_idx,
                              int slot_num, unsigned cmd);

  ResReqPtr findRequest(DynInstPtr inst);
  ResReqPtr findRequest(DynInstPtr inst, int idx);

  void requestAgain(DynInstPtr inst, bool &try_request);

  virtual int getSlot(DynInstPtr inst);

  /** Executes one of the commands from the "Command" enum */
  virtual void execute(int slot_num);

  virtual void squash(DynInstPtr inst, int stage_num, InstSeqNum squash_seq_num,
                      ThreadID tid);

  virtual void squashLocalMemRequest(LocalMemReqPtr req_ptr);

  /** After memory request is completed in the local memory, then do final
   processing to complete the request in the CPU.
   */
  virtual void processLocalMemCompletion(PacketPtr pkt);

  /** Create request that will interface w/TLB and Memory objects */
  virtual void setupMemRequest(DynInstPtr inst, LocalMemReqPtr localmem_req,
                               int acc_size, int flags);

  void finishLocalMemUnitReq(DynInstPtr inst, LocalMemRequest *localmem_req);

  void buildDataPacket(LocalMemRequest *localmem_req);

  bool processSquash(LocalMemReqPacket *localmem_pkt) {
    return processSquash(localmem_pkt, false);
  }

  bool processSquash(LocalMemReqPacket *localmem_pkt, bool SorM);

  void recvRetry();

  Fault read(DynInstPtr inst, Addr addr, uint8_t *data, unsigned size,
             unsigned flags);

  Fault write(DynInstPtr inst, uint8_t *data, unsigned size, Addr addr,
              unsigned flags, uint64_t *res);

  void doTLBAccess(DynInstPtr inst, LocalMemReqPtr localmem_req, int acc_size,
                   int flags, TheISA::TLB::Mode tlb_mode);

  /** Read/Write on behalf of an instruction.
   *  curResSlot needs to be a valid value in instruction.
   */
  void doLocalMemAccess(DynInstPtr inst, uint64_t *write_result = NULL);

  void stallPipelineStages(LocalMemReqPtr req, int start, int end,
                           int tid, bool SorM);

  void unstallPipelineStages(LocalMemReqPtr req, int start, int end, int src,
                             int tid, bool SorM);

  class LocalMemEvent: public ResourceEvent {
  public:
    LocalMemEvent()
      : ResourceEvent() {}
    ~LocalMemEvent() {}

    void process();

  };

protected:
  /** LocalMem interface. */
  MasterPort *localmemPort;

  bool localmemPortBlocked;

  bool causedbySorM;

  Tick accessTick[6];

public:
  TheISA::TLB* tlb();
  TheISA::TLB *_tlb;
};

//@todo: Move into LocalMemUnit Class for private access to "valid" field
class LocalMemRequest: public ResourceRequest {
public:
  LocalMemRequest(LocalMemUnit *cres)
      : ResourceRequest(cres), pktCmd(MemCmd::InvalidCmd), memReq(NULL), reqData(NULL),
        dataPkt(NULL), memAccComplete(false), memAccPending(false),
        instIdx(0), fetchBufferFill(false), needBankSync(false) {}

  virtual ~LocalMemRequest() {
    if (reqData) delete[] reqData;
  }

  void setRequest(DynInstPtr _inst, int stage_num, int res_idx, int slot_num,
                  unsigned _cmd, MemCmd::Command pkt_cmd, int idx) {
    pktCmd = pkt_cmd;
    instIdx = idx;

    ResourceRequest::setRequest(_inst, stage_num, res_idx, slot_num, _cmd);
  }

  void clearRequest();

  virtual PacketDataPtr getData() {
    return reqData;
  }

  void setMemAccCompleted(bool completed = true) {
    memAccComplete = completed;
  }

  bool isMemAccComplete() {
    return memAccComplete;
  }

  void setMemAccPending(bool pending = true) {
    memAccPending = pending;
  }

  bool isMemAccPending() {
    return memAccPending;
  }

  //Make this data private/protected!
  MemCmd::Command pktCmd;
  RequestPtr memReq;
  PacketDataPtr reqData;
  LocalMemReqPacket *dataPkt;

  bool memAccComplete;
  bool memAccPending;
  int instIdx;

  /** Should we expect block from localmem access or fetch buffer? */
  bool fetchBufferFill;
  bool needBankSync;
};

class LocalMemReqPacket: public Packet {
public:
  LocalMemReqPacket(LocalMemRequest *_req, Command _cmd, int _idx = 0)
      : Packet(&(*_req->memReq), _cmd), localmemReq(_req), instIdx(_idx),
        hasSlot(false), reqData(NULL), memReq(NULL) {}

  LocalMemRequest *localmemReq;
  int instIdx;
  bool hasSlot;
  PacketDataPtr reqData;
  RequestPtr memReq;
};

#endif //__CPU_LOCALMEM_UNIT_HH__
