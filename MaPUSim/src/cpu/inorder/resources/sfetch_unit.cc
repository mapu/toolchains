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
#include "cpu/inorder/resources/sfetch_unit.hh"
#include "cpu/inorder/cpu.hh"
#include "cpu/inorder/pipeline_traits.hh"
#include "cpu/inorder/resource_pool.hh"
#include "debug/Activity.hh"
#include "debug/InOrderLocalMemPort.hh"
#include "debug/InOrderStall.hh"
#include "debug/RefCount.hh"
#include "debug/ThreadModel.hh"
#include "mem/request.hh"

using namespace std;
using namespace TheISA;
using namespace ThePipeline;

SFetchUnit::SFetchUnit(string res_name, int res_id, int res_width,
                       int res_latency, InOrderCPU *_cpu,
                       ThePipeline::Params *params) :
    LocalMemUnit(res_name, res_id, res_width, res_latency, _cpu, params),
    instSize(sizeof(TheISA::MachInst)), lastFetchTick(-1),
    fetchBuffSize(params->fetchBuffSize) {
  for (int tid = 0; tid < MaxSThreads; tid++)
    decoder[tid] = new Decoder(NULL);
}

SFetchUnit::~SFetchUnit() {
  std::list<FetchBlock*>::iterator fetch_it = fetchBuffer.begin();
  std::list<FetchBlock*>::iterator end_it = fetchBuffer.end();
  while (fetch_it != end_it) {
    delete (*fetch_it)->block;
    delete *fetch_it;
    fetch_it++;
  }
  fetchBuffer.clear();

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

void SFetchUnit::createMachInst(std::list<FetchBlock*>::iterator fetch_it,
                                int pkglen,
                                int pkgbreak,
                                DynInstPtr inst) {
  Addr block_addr = sfetchBlockAlign(inst->getMemAddr());
  Addr fetch_addr = inst->getMemAddr();
  unsigned fetch_offset = (fetch_addr - block_addr) >> 2;
  ThreadID tid = inst->readTid();
  TheISA::PCState instPC = inst->pcState();

  assert(tid < MaxSThreads);

  DPRINTF(InOrderLocalMemPort, "Creating instruction [sn:%i] w/fetch data @"
          "addr:%08p block:%08p pkglen:%d, pkgbreak:%d\n",
          inst->seqNum, fetch_addr, block_addr, pkglen, pkgbreak);

  assert((*fetch_it)->valid);

  TheISA::MachInst *fetchInsts =
    reinterpret_cast<TheISA::MachInst *>((*fetch_it)->block);

  decoder[tid]->setTC(cpu->thread[tid]->getTC());

  for (unsigned i = 0; i < pkglen; i++) {
    if (i >= pkgbreak) {
      fetch_it++;
      assert(fetch_it != fetchBuffer.end());
      fetchInsts =
        reinterpret_cast<TheISA::MachInst *>((*fetch_it)->block);
      fetch_offset = 0;
      pkgbreak = pkglen;
    }
    TheISA::MachInst mach_inst = TheISA::gtoh(fetchInsts[fetch_offset++]);
    decoder[tid]->moreBytes(instPC, inst->instAddr(), mach_inst);
  }

  instPC.nlpc(instPC.pc() + pkglen * instSize);

  assert(decoder[tid]->instReady());
  inst->setStaticInst(decoder[tid]->decode(instPC));
  inst->pcState(instPC);
  if (inst->staticInst->isPackageEnd()) {
    DPRINTF(InOrderLocalMemPort, "Package end is found at %#x.\n",
            inst->pcState());
    inst->setPackageEnd();
    inst->setSeqLineNum(cpu->getAndIncrementInstSeqLine(inst->readTid()));
  } else
    inst->setSeqLineNum(cpu->nextInstSeqLineNum(inst->readTid()));
  DPRINTF(InOrderLocalMemPort, "Inst pc is set to %s\n", inst->pcState());
}

ResReqPtr SFetchUnit::getRequest(DynInstPtr inst, int stage_num, int res_idx,
                                 int slot_num, unsigned cmd) {
  ScheduleEntry* sched_entry = *inst->curSkedEntry;
  LocalMemRequest* localmem_req = dynamic_cast<LocalMemRequest*>(reqs[slot_num]);

  if (!inst->validMemAddr()) {
    panic("Mem. Addr. must be set before requesting localmem access\n");
  }

  assert(sched_entry->cmd == InitiateFetch);

  DPRINTF(InOrderLocalMemPort,
          "[tid:%i]: Fetch request from [sn:%i] for addr %08p\n",
          inst->readTid(), inst->seqNum, inst->getMemAddr());

  localmem_req->setRequest(inst, stage_num, id, slot_num, sched_entry->cmd,
                           MemCmd::ReadReq, inst->curSkedEntry->idx);

  return localmem_req;
}

void SFetchUnit::setupMemRequest(DynInstPtr inst, LocalMemReqPtr localmem_req,
                                 int acc_size, int flags) {
  ThreadID tid = inst->readTid();
  Addr aligned_addr = sfetchBlockAlign(inst->getMemAddr());
  if (localmem_req->memReq == NULL) {
    localmem_req->memReq = new Request(tid, aligned_addr, acc_size, flags,
                                       cpu->instMasterId(), inst->instAddr(),
                                       cpu->readCpuId(), tid);
    DPRINTF(InOrderLocalMemPort, "[sn:%i] Created memReq @%x, ->%x\n",
            inst->seqNum, aligned_addr, aligned_addr + acc_size);
  }
}

void SFetchUnit::clearFetchBuffer() {
  std::list<FetchBlock*>::iterator fetch_it = fetchBuffer.begin();
  std::list<FetchBlock*>::iterator end_it = fetchBuffer.end();

  while (fetch_it != end_it) {
    if ((*fetch_it)->block) {
      delete[] (*fetch_it)->block;
    }
    delete *fetch_it;
    fetch_it++;
  }
  fetchBuffer.clear();
}

bool SFetchUnit::searchPackage(DynInstPtr inst, int &pkglen, int &pkgbreak) {
  Addr block_addr = sfetchBlockAlign(inst->getMemAddr());
  Addr fetch_addr = inst->getMemAddr();
  unsigned fetch_offset = fetch_addr - block_addr;
  bool found = false;
  for (unsigned i = fetch_offset + instSize - 1;
       i < SCodeLineSize; i += instSize) {
    pkglen++;
    if (fetchBuffer.front()->block[i] >> 7) {
      found = true;
      break;
    }
  }
  pkgbreak = pkglen;
  if (!found && fetchBuffer.size() > 1) {
    std::list<FetchBlock*>::iterator fetch_it = fetchBuffer.begin();
    fetch_it++;
    for (unsigned i = instSize - 1;
         i < SCodeLineSize; i += instSize) {
      pkglen++;
      if ((*fetch_it)->block[i] >> 7) {
        found = true;
        break;
      }
    }
  }
  if (pkglen > SCodeLineSize / instSize) return false;
  return found;
}

void SFetchUnit::execute(int slot_num) {
  LocalMemReqPtr localmem_req = dynamic_cast<LocalMemReqPtr>(reqs[slot_num]);
  assert(localmem_req);

  if (localmemPortBlocked && localmem_req->cmd == InitiateFetch) {
    DPRINTF(InOrderLocalMemPort, "LocalMem Port Blocked. Cannot Access\n");
    localmem_req->done(false);
    return;
  }

  DynInstPtr inst = localmem_req->inst;
  Addr block_addr = sfetchBlockAlign(inst->getMemAddr());

  if (inst->fault != NoFault) {
    DPRINTF(InOrderLocalMemPort,
            "[tid:%i]: [sn:%i]: Detected %s fault @ %x. Forwarding to "
            "next stage.\n", inst->readTid(), inst->seqNum, inst->fault->name(),
            sfetchBlockAlign(inst->getMemAddr()));
    finishLocalMemUnitReq(inst, localmem_req);
    return;
  }

  switch (localmem_req->cmd) {
  case InitiateFetch: {
    // Check to see if we've already got this request buffered
    // or pending to be buffered
    bool do_fetch = true;
    int total_pending = pendingFetch.size() + fetchBuffer.size();

    if (!pendingFetch.empty()) {
      FetchBlock* pending_it = pendingFetch.back();
      if (pending_it->addr == block_addr) {
        do_fetch = false;

        DPRINTF(InOrderLocalMemPort, "%08p is a pending fetch block.\n",
                block_addr);
      }
    } else if (!fetchBuffer.empty()) {
      // FIXME: A fetch block may be split into two cycles due to
      // insufficient stage width, whereas this is not safe to check
      // if a preceding fetch block is the same fetch op with this one
      FetchBlock* fetch_it = fetchBuffer.back();
      if (fetch_it->addr == block_addr)
        do_fetch = false;

        DPRINTF(InOrderLocalMemPort, "%08p is a pending fetch block.\n",
                block_addr);
    }
    if (!do_fetch) {
      DPRINTF(InOrderLocalMemPort, "Inst. [sn:%i] marked to be filled "
              "through fetch buffer.\n", inst->seqNum);
      localmem_req->fetchBufferFill = true;
      localmem_req->setCompleted(true);
      return;
    }

    // FIXME: this will be removed when local memory is a real timing model
    if (lastFetchTick == curTick()) {
      DPRINTF(InOrderLocalMemPort, "Fetching port is already used.\n");
      localmem_req->done(false);
      return;
    }

    // Check to see if there is room in the fetchbuffer for this instruction.
    // If not, block this request.
    if (total_pending >= fetchBuffSize) {
      DPRINTF(InOrderLocalMemPort, "No room available in fetch buffer.\n");
      localmem_req->done(false);
      return;
    }

    doTLBAccess(inst, localmem_req, SCodeLineSize, Request::INST_FETCH,
                TheISA::TLB::Execute);

    if (inst->fault == NoFault) {
      DPRINTF(InOrderLocalMemPort, "[tid:%u]: Initiating fetch access to %s for"
              " addr:%#x (block:%#x)\n", inst->readTid(), name(),
              localmem_req->inst->getMemAddr(), block_addr);

      localmem_req->reqData = new uint8_t[SCodeLineSize];

      inst->setCurResSlot(slot_num);

      doLocalMemAccess(inst);

      if (localmem_req->isMemAccPending()) {
        pendingFetch.push_back(new FetchBlock(block_addr));
        // FIXME: this will be removed
        lastFetchTick = curTick();
        // mark replacement block
      }
    }

    break;
  }

  case CompleteFetch:
    if (inst->fault != NoFault) {
      DPRINTF(InOrderLocalMemPort,
              "[tid:%i]: [sn:%i]: Detected %s fault @ %x. Forwarding to "
              "next stage.\n",  inst->readTid(), inst->seqNum,
              inst->fault->name(), inst->getMemAddr());
      finishLocalMemUnitReq(inst, localmem_req);
      return;
    }

    if (!localmem_req->fetchBufferFill && localmem_req->isMemAccComplete()) {
      localmem_req->fetchBufferFill = true;
    }

    if (localmem_req->fetchBufferFill) {
      // Block request if it's depending on a previous fetch, but it hasnt made it yet
      std::list<FetchBlock*>::iterator fetch_it = fetchBuffer.begin();
      if (fetch_it == fetchBuffer.end() ||
          (*fetch_it)->addr != block_addr) {
        DPRINTF(InOrderLocalMemPort, "Block %#x is not available yet\n", block_addr);
        localmem_req->setCompleted(false);
        return;
      }

      int pkglen = 0;
      int pkgbreak = 0;
      if (!searchPackage(inst, pkglen, pkgbreak)) {
        DPRINTF(InOrderLocalMemPort,
                "Inst line @%#x is not available yet, waiting for fetching\n",
                inst->getMemAddr());
        localmem_req->setCompleted(false);
        return;
      }
      assert(pkglen <= SCodeLineSize / instSize);

      // Make New Instruction
      createMachInst(fetch_it, pkglen, pkgbreak, inst);
      if (inst->traceData) {
        inst->traceData->setStaticInst(inst->staticInst);
        inst->traceData->setPC(inst->pcState());
      }

      if (sfetchBlockAlign(inst->pcState().npc()) !=
          fetchBuffer.front()->addr) {
        DPRINTF(InOrderLocalMemPort, "%#x released.\n", fetchBuffer.front()->addr);
        delete[] fetchBuffer.front()->block;
        delete fetchBuffer.front();
        fetchBuffer.erase(fetchBuffer.begin());
      }

      DPRINTF(InOrderLocalMemPort, "[tid:%i]: Instruction [sn:%i] is: %s\n",
              inst->readTid(), inst->seqNum,
              inst->staticInst->disassemble(inst->instAddr()));

      inst->unsetMemAddr();

      localmem_req->done();
      return;
    } else {
      DPRINTF(InOrderLocalMemPort,
              "[tid:%i]: [sn:%i]: Unable to Complete Fetch Access\n",
              inst->readTid(), inst->seqNum);
      DPRINTF(InOrderStall, "STALL: [tid:%i]: Fetch miss from %08p\n",
              inst->readTid(), localmem_req->inst->instAddr());
      localmem_req->setCompleted(false);
    }
    break;

  default:
    fatal("Unrecognized command to %s", resName);
  }
}

void SFetchUnit::processLocalMemCompletion(PacketPtr pkt) {
  // Cast to correct packet type
  // @todo: use pkt Sender state here to be consistent with other
  // cpu models
  LocalMemReqPacket* localmem_pkt = dynamic_cast<LocalMemReqPacket*>(pkt);
  assert(localmem_pkt);

  DPRINTF(InOrderLocalMemPort, "Finished request for %x\n", localmem_pkt->getAddr());

  if (processSquash(localmem_pkt, false)) return;

  Addr block_addr =
    sfetchBlockAlign(localmem_pkt->localmemReq->getInst()->getMemAddr());

  DPRINTF(InOrderLocalMemPort,
          "[tid:%u]: [sn:%i]: Waking from fetch access to addr:%#x(phys:%#x), size:%i\n",
          localmem_pkt->localmemReq->getInst()->readTid(),
          localmem_pkt->localmemReq->getInst()->seqNum,
          block_addr, localmem_pkt->getAddr(),
          localmem_pkt->getSize());

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

  assert(!localmem_req->isSquashed());
  assert(inst->curSkedEntry->cmd == CompleteFetch);

  DPRINTF(InOrderLocalMemPort,
          "[tid:%u]: [sn:%i]: Processing fetch access for block %#x\n",
          localmem_req->inst->readTid(), inst->seqNum, block_addr);

  std::list<FetchBlock*>::iterator pend_it = pendingFetch.begin();
  while (pend_it != pendingFetch.end() && (*pend_it)->addr != block_addr)
    pend_it++;

  assert(pend_it != pendingFetch.end());

  // Copy Data to pendingFetch queue...
  (*pend_it)->block = new uint8_t[SCodeLineSize];
  memcpy((*pend_it)->block, localmem_pkt->getPtr<uint8_t>(), SCodeLineSize);
  (*pend_it)->valid = true;

  FetchBlock *new_block = (*pend_it);
  pendingFetch.erase(pend_it);
  fetchBuffer.push_back(new_block);

  localmem_req->setMemAccPending(false);
  localmem_req->setMemAccCompleted();

  // Wake up the CPU (if it went to sleep and was waiting on this
  // completion event).
  cpu->wakeCPU();

  DPRINTF(Activity, "[tid:%u] Activating %s due to localmem completion\n",
          inst->readTid(), cpu->pipelineStage[stage_num]->name());

  cpu->switchToActive(stage_num);
}

void SFetchUnit::squashLocalMemRequest(LocalMemReqPtr req_ptr) {
  DynInstPtr inst = req_ptr->getInst();
  Addr block_addr = sfetchBlockAlign(inst->getMemAddr());

  // Check Fetch Buffer (or pending fetch) for this block and
  // update pending counts
  std::list<FetchBlock*>::iterator buff_it = fetchBuffer.begin();
  while (buff_it != fetchBuffer.end() && (*buff_it)->addr != block_addr)
    buff_it++;
  if (buff_it != fetchBuffer.end()) {
    DPRINTF(InOrderLocalMemPort, "[sn:%i] Removing Pending Access "
            "for Fetch Buffer block %08p, %d left.\n", inst->seqNum, block_addr, fetchBuffer.size());
    delete[] (*buff_it)->block;
    delete *buff_it;
    fetchBuffer.erase(buff_it);
  }
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

void SFetchUnit::trap(Fault fault, ThreadID tid, DynInstPtr inst) {
  if (inst->isMCode()) return;
  DPRINTF(InOrderLocalMemPort, "%s trapped\n", name());
  //@todo: per thread?
  decoder[tid]->reset();

  //@todo: squash using dummy inst seq num
  squash(NULL, NumSStages - 1, 0, tid);

  //@todo: make sure no blocks are in use
  assert(fetchBuffer.size() == 0);
  assert(pendingFetch.size() == 0);

  //@todo: clear pendingFetch and fetchBuffer
  clearFetchBuffer();
}
