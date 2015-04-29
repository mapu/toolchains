//===-------------- apdlgen/APDLOptimize.cpp - APDL Optimize-------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===--------------------------------------------------------------------===//
//
//
//===--------------------------------------------------------------------===//

#include "MMPUAPDLGen.h"
#include "MMPUAST.h"
#include "MMPUSymbol.h"
#include "MMPUBlock.h"
#include "AsmParser/MMPULiteScheduler.h"
#include <list>

using namespace llvm;
using namespace Lite;
using namespace MMPULite;

namespace MMPU {
void APDLGen::GlobalSchedule(void) {
  std::map<MCLoopBlock *, struct BlockInfo>::iterator iter;
  std::vector<MCLoopBlock *> GlobalEntry;

  // FIXME: fixup Loop and MAC inst

  for (iter = LeafBlockInfo.begin(); iter != LeafBlockInfo.end(); iter++)
    HMacros.push_back(iter->second.Block);

  if (OptLevel == 0) return;
  // Add a psuedo start vextor into DAG
  GlobalDAG.AddVextor(NULL);

  for (iter = LeafBlockInfo.begin(); iter != LeafBlockInfo.end(); iter++) {
    GlobalDAG.AddVextor(iter->second.Block);
    if (iter->second.Entry.empty())
      GlobalEntry.push_back(iter->first);
  }

  /* Add psuedo arcs between the psuedo start node and all other vextors.
   The weight is non-zero if there is any connection between vextors and
   global terminals. */
  for (unsigned i = 0; i < GlobalEntry.size(); i++)
    GlobalDAG.AddArc(NULL, GlobalEntry[i], 0);

  for (unsigned i = 0; i < GlobalEntry.size(); i++) {
    struct BlockInfo &p = LeafBlockInfo[GlobalEntry[i]];
    for (unsigned j = 0; j < p.Terminal.size(); j++) {
      if (VRegTable.count(p.Terminal[j])) {
        MCParsedInst *producer = VRegTable[p.Terminal[j]].producer;
        uint64_t Latency = MMPULiteScheduler::getLatency
        (CAST_TO_MMPU_OPRD(producer->getOperand(0))->getOpc());
        uint64_t PosT = 0, PosE = 0;
        producer->getParent()->findInstPos(producer, PosT);
        for (unsigned k = 0;
             k < VRegTable[p.Terminal[j]].consumers.size();
             k++) {
          MCParsedInst *consumer = VRegTable[p.Terminal[j]].consumers[k];
          if (consumer && consumer->getParent() &&
              consumer->getParent() != producer->getParent() && // This an MReg variable
              LeafBlockInfo.count(consumer->getParent()) &&
              consumer->getParent()->findInstPos(consumer, PosE)) {
            // find an Arc, calculate its weight
            errs() << PosT << ":" << Latency << ":" << PosE << "\n";
            if (PosT + Latency >= PosE) {
              if (GlobalDAG.getArcWeight(consumer->getParent(),
                                         producer->getParent()) ||
                  GlobalDAG.getArcWeight(producer->getParent(),
                                         consumer->getParent()) + PosE <
                  PosT + Latency)
                GlobalDAG.DelArc(consumer->getParent(), producer->getParent());
                GlobalDAG.AddArc(producer->getParent(), consumer->getParent(),
                                 PosT + Latency - PosE);
            } else {
              GlobalDAG.AddArc(consumer->getParent(), producer->getParent(),
                               PosE - PosT - Latency);
            }
            // delete the entry
            std::vector<uint64_t>::iterator deliter =
              LeafBlockInfo[consumer->getParent()].Entry.begin();
            while (deliter != LeafBlockInfo[consumer->getParent()].Entry.end() &&
                   *deliter != p.Terminal[j]) deliter++;
            if (deliter == LeafBlockInfo[consumer->getParent()].Entry.end())
              errs() << "cannot find an entry for this terminal: "
                     << p.Terminal[j] << "\n";
            else
              LeafBlockInfo[consumer->getParent()].Entry.erase(deliter);
            if (LeafBlockInfo[consumer->getParent()].Entry.empty())
              GlobalEntry.push_back(consumer->getParent());
          } else if (consumer && consumer->getParent() &&
                     consumer->getParent() != producer->getParent())
            errs() << "Spurious entry of the block, consumer has no parent.\n";
        }
      } else errs() << "Spurious entry of the block, "
                       "terminal cannot be find in VReg table.\n";
    }
  }

  std::map<MCLoopBlock *, MCLoopBlock *> critical;

  std::vector<MCLoopBlock *> UnitHMacros;

  if (GlobalDAG.Criticalpath(critical)) {
    MCLoopBlock *Next = NULL;
    uint64_t start = 0;
    while (critical.count(Next)) {
      LeafBlockInfo[critical[Next]].Scheduled = true;
      start += GlobalDAG.getArcWeight(Next, critical[Next]);
      critical[Next]->setStart(start);
      UnitHMacros.push_back(critical[Next]);
      Next = critical[Next];
    }
  } else errs() << "No critical path!\n";

  while (1) {
    for (iter = LeafBlockInfo.begin(); iter != LeafBlockInfo.end(); iter++)
      if (!iter->second.Scheduled) break;
    if (iter == LeafBlockInfo.end()) break;
    else do {
      uint64_t start = 0;
      unsigned i;
      for (i = 0; i < iter->second.Entry.size(); i++) {
        MCLoopBlock *pp;
        if (VRegTable.count(iter->second.Entry[i]) &&
            VRegTable[iter->second.Entry[i]].producer &&
            (pp = VRegTable[iter->second.Entry[i]].producer->getParent())) {
          unsigned j;
          for (j = 0; j < UnitHMacros.size(); j++) {
            if (UnitHMacros[j] == pp) {
              if (start < pp->getStart() +
                          GlobalDAG.getArcWeight(pp, iter->first))
                start = pp->getStart() +
                        GlobalDAG.getArcWeight(pp, iter->first);
              break;
            }
          }
          if (j == UnitHMacros.size()) break;
        }
      }
      if (i == iter->second.Entry.size()) {
        iter->second.Scheduled = true;
        iter->first->setStart(start);
        UnitHMacros.push_back(iter->first);
      }
    } while (++iter != LeafBlockInfo.end());
  }

  HMacros = UnitHMacros;
}

void APDLGen::SoftPipe(void) {
  if (OptLevel <= 1) return;
  /* Unrolling the inner loops */
  uint64_t relax = OptLevel - 2;
  std::vector<MCLoopBlock *> AfterUnroll;
  for (unsigned i = 0; i < HMacros.size(); i++) {
    if (HMacros[i]->getType() == Loop) {
      uint64_t nonbubbles;
      EstimatePipelineUsage(HMacros[i], nonbubbles);
      if (nonbubbles != 0) {
        MCLoopBlock *P = HMacros[i]->getParent();
        // Pad the loop with Nops to match times of non-bubbles
        if (nonbubbles == HMacros[i]->size()) {
          AfterUnroll.push_back(HMacros[i]);
          continue;
        }
        nonbubbles = nonbubbles + relax;
        while (HMacros[i]->size() % nonbubbles != 0) {
          HMacros[i]->addParsedInst(NULL);
          EstimatePipelineUsage(HMacros[i], nonbubbles);
          nonbubbles = nonbubbles + relax;
        }
        /* Split the loop into (HMacros[i]->size() % nonbubbles) of sub loops,
         each of which is in the size of non-bubbles. */
        LoopInfoMap[HMacros[i]->getKI()].KReg = HMacros[i]->getKI();
        LoopInfoMap[HMacros[i]->getKI()].OrigSize = HMacros[i]->size();
        LoopInfoMap[HMacros[i]->getKI()].SplitSize = nonbubbles;
        unsigned numofsubs = HMacros[i]->size() / nonbubbles;
        errs() << "Info: Loop KI" << HMacros[i]->getKI() << " (size is "
               << HMacros[i]->size() << ") is splitted into size of "
               << nonbubbles << "\n";
        for (unsigned j = 0; j < numofsubs; j++) {
          MCLoopBlock *NewSubLoop =
          HMacros[i]->DupThisBlock(HMacros[i]->getStart() + j * nonbubbles);
          errs() << "Debug: " << HMacros[i]->getStart() + j * nonbubbles << "\n";
          NewSubLoop->setParent(P);
          for (unsigned k = 0; k < nonbubbles; k++)
            HMacros[i]->getInsts().erase(HMacros[i]->getInsts().begin());
          bool AllNopLoop = true;
          for (unsigned k = 0; k < nonbubbles; k++)
            if (NewSubLoop->getInsts()[k]) {
              AllNopLoop = false;
              break;
            }
          for (unsigned k = 0; k < HMacros[i]->size(); k++)
            NewSubLoop->getInsts().pop_back();
          if (!AllNopLoop) {
            AfterUnroll.push_back(NewSubLoop);
            P->addSubBlock(NewSubLoop);
          } else delete NewSubLoop;
        }
        std::vector<MCLoopBlock *> Reorder = P->getBlocks();
        P->getBlocks().clear();
        uint64_t index = P->getStart();
        while (!Reorder.empty()) {
          uint64_t nextindex = 0;
          std::vector<MCLoopBlock *>::iterator iter;
          for (iter = Reorder.begin(); iter != Reorder.end(); iter++) {
            if ((*iter) == HMacros[i]) {
              delete *iter;
              Reorder.erase(iter);
              break;
            }
          }
          for (iter = Reorder.begin(); iter != Reorder.end(); ) {
            if (nextindex == 0) nextindex = (*iter)->getStart();
            else if ((*iter)->getStart() != index &&
                     nextindex > (*iter)->getStart())
              nextindex = (*iter)->getStart();
            if ((*iter)->getStart() == index) {
              P->addSubBlock(*iter);
              Reorder.erase(iter);
            } else iter++;
          }
          index = nextindex;
        }
      }
    } else AfterUnroll.push_back(HMacros[i]);
  }

  HMacros = AfterUnroll;
}

void APDLGen::EstimatePipelineUsage(MCLoopBlock *Pipeline,
                                    uint64_t &Bubbles) const {
  Bubbles = 0;
  uint64_t Units[14] = {0};
  for (unsigned i = 0; i < Pipeline->size(); i++) {
    MCParsedInst *PInst = Pipeline->getInsts()[i];
    if (!PInst) {
      Units[13]++;
      continue;
    }
    while (PInst) {
      unsigned category = CAST_TO_MMPU_OPRD
                          (PInst->getOperand(PInst->size() - 1))->isSlot() ?
                          CAST_TO_MMPU_OPRD
                          (PInst->getOperand(PInst->size() - 1))->getSlot() : 13;
      Units[category]++;
      PInst = PInst->getNext();
    }
  }
  //unsigned critical_unit_id;
  for (unsigned i = 0; i < 13; i++) {
    if (Bubbles < Units[i]) {
      //critical_unit_id = i;
      Bubbles = Units[i];
    }
  }
}
}
