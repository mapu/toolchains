//===-------------- apdlgen/APDLLegalize.cpp - APDL Legalize-------------===//
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

void APDLGen::Legalize(void) {
  SharedMMPUOprd O;
  MCLoopBlock *top;
  unsigned dest;
  MCLoopBlock *Org;
  MCLoopBlock *Back;
  std::vector<MCParsedInst *> LocalTerminals;
  Graph<MCParsedInst *> ForwardDAG, BackwardDAG;
  std::map<MCParsedInst *, uint64_t> SrcBuffed;

  if (APDLFunc) {
    APDLFunc->EliminateEmptyBlock();
    if (APDLFunc->getBlocks().size()) {
      for (unsigned i = 0; i < APDLFunc->getBlocks().size(); i++) {
        top = APDLFunc;
        APDLFunc = APDLFunc->getBlocks()[i];
        Legalize();
        APDLFunc = top;
      }
    } else if (APDLFunc->size()) {
      // Add a psuedo start vextor into DAG
      ForwardDAG.AddVextor(NULL);
      BackwardDAG.AddVextor(NULL);
      /* Add psuedo arcs between the psuedo start node and all other vextors.
         The weight is non-zero if there is any connection between vextors and
         global terminals. */
      LeafBlockInfo[APDLFunc].Block = APDLFunc;
      for (unsigned i = 0; i < APDLFunc->getInsts().size(); i++) {
        ForwardDAG.AddVextor(APDLFunc->getInsts()[i]);
        BackwardDAG.AddVextor(APDLFunc->getInsts()[i]);
        if (SrcTable.count(APDLFunc->getInsts()[i])) {
          uint64_t weight = 0;
          for (unsigned j = 0;
               j < SrcTable[APDLFunc->getInsts()[i]].size();
               j++) {
            uint64_t src = SrcTable[APDLFunc->getInsts()[i]][j];
            if (GlobalTerminals.count(VRegTable[src].producer)) {
              LeafBlockInfo[APDLFunc].Entry.push_back(src);
              if (weight < GlobalTerminals[VRegTable[src].producer])
                weight = GlobalTerminals[VRegTable[src].producer];
            }
            if (VRegTable[src].type != TagExprAST::STORAGE_TREG)
              SrcBuffed[APDLFunc->getInsts()[i]] = weight;
          }
          BackwardDAG.AddArc(APDLFunc->getInsts()[i], NULL, weight);
        }
      }

      /* In the first round, collect all terminal insts, build the DAG */
      for (unsigned i = 0; i < APDLFunc->getInsts().size(); i++) {
        O = CAST_TO_MMPU_OPRD(APDLFunc->getInsts()[i]->getOperand(0));
        dest = O->getOpc();

        if (hasDest(O->getOpc())) {
          uint64_t vr;
          if (hasTDest(O->getOpc())) {
            O = CAST_TO_MMPU_OPRD(APDLFunc->getInsts()[i]->getOperand(3));
            vr = O->getImm();
          } else if (hasMDest(O->getOpc())) {
            O = CAST_TO_MMPU_OPRD(APDLFunc->getInsts()[i]->getOperand(2));
            vr = O->getImm();
          } else {
            O = CAST_TO_MMPU_OPRD(APDLFunc->getInsts()[i]->getOperand(1));
            vr = O->getReg() - MMPULiteReg::BIU0;
          }

          if (VRegTable.count(vr)) {
            for (unsigned j = 0;
                 j < VRegTable[vr].consumers.size();
                 j++) {
              uint64_t Pos;
              if (APDLFunc->findInstPos(VRegTable[vr].consumers[j], Pos)) {
                if (VRegTable[vr].type != TagExprAST::STORAGE_MACREG) {
                  BackwardDAG.AddArc(VRegTable[vr].consumers[j],
                                     APDLFunc->getInsts()[i],
                                     MMPULiteScheduler::getLatency(dest));
                } else {
                  /* For MAC variable, the register accessing time is several
                     cycles later than the inst issuing time */
                  uint64_t Postpone = 0;
                  unsigned consumerdest =
                    CAST_TO_MMPU_OPRD(VRegTable[vr].consumers[j]->getOperand(0))
                      ->getOpc();
                  if (consumerdest == IMA || (consumerdest >= IMAToBIU &&
                                              consumerdest <= IMAToSHU))
                    Postpone = 5;
                  else if (consumerdest >= IMAcc && consumerdest <= IMAccToSHU)
                    Postpone = 1;
                  else if (consumerdest >= IMaC && consumerdest <= IMaCToSHU)
                    Postpone = 5;
                  BackwardDAG.AddArc(VRegTable[vr].consumers[j],
                                     APDLFunc->getInsts()[i],
                                     MMPULiteScheduler::getLatency(dest) - Postpone);
                }
              } else {
                LocalTerminals.push_back(APDLFunc->getInsts()[i]);
                break;
              }
            }
          } else errs() << "Legalize an unused VReg.\n";
        } else {
          LocalTerminals.push_back(APDLFunc->getInsts()[i]);
          continue;
        }

        // If this is a TReg assignment, it needs to fix the dest

        if (O->isImm() && VRegTable.count(O->getImm()) &&
            !VRegTable[O->getImm()].consumers.empty() &&
            VRegTable[O->getImm()].type == TagExprAST::STORAGE_TREG) {
          if (VRegTable[O->getImm()].consumers.size() > 1)
            errs() << "Warning: TReg is referred more than once.\n";

          O = CAST_TO_MMPU_OPRD(VRegTable[O->getImm()].consumers[0]->getOperand(0));
          if (O->getOpc() >= SHU0CombToBIU) {
            //delete APDLFunc->getInsts()[i]->getOperand(0);
            (*APDLFunc->getInsts()[i]->getOperands())[0] =
              SHARED_OPRD(Opc, dest + 1, SMLoc(), SMLoc());
          }
          if (O->getOpc() >= IALUBinToBIU && O->getOpc() <= IALUUryToSHU) ;
          else if (O->getOpc() >= FALUBinToBIU && O->getOpc() <= FALUUryToSHU) {
            //delete APDLFunc->getInsts()[i]->getOperand(1);
            (*APDLFunc->getInsts()[i]->getOperands())[1] =
              SHARED_OPRD(Reg, MMPULiteReg::FALU, SMLoc(), SMLoc());
          } else if (O->getOpc() >= FMA && O->getOpc() <= FMulToSHU) {
            //delete APDLFunc->getInsts()[i]->getOperand(1);
            (*APDLFunc->getInsts()[i]->getOperands())[1] =
              SHARED_OPRD(Reg, MMPULiteReg::FMAC, SMLoc(), SMLoc());
          } else if (O->getOpc() >= IMA && O->getOpc() <= IMulToSHU) {
            //delete APDLFunc->getInsts()[i]->getOperand(1);
            (*APDLFunc->getInsts()[i]->getOperands())[1] =
              SHARED_OPRD(Reg, MMPULiteReg::IMAC, SMLoc(), SMLoc());
          } else if (O->getOpc() >= SHU0CombToBIU && O->getOpc() <= SHU0Stop) {
            //delete APDLFunc->getInsts()[i]->getOperand(1);
            (*APDLFunc->getInsts()[i]->getOperands())[1] =
              SHARED_OPRD(Reg, MMPULiteReg::SHU0, SMLoc(), SMLoc());
            APDLFunc->getInsts()[i]->delOperand(4);  // delete IPath
          } else if (O->getOpc() >= SHU1CombToBIU && O->getOpc() <= SHU1Stop) {
            //delete APDLFunc->getInsts()[i]->getOperand(1);
            (*APDLFunc->getInsts()[i]->getOperands())[1] =
              SHARED_OPRD(Reg, MMPULiteReg::SHU1, SMLoc(), SMLoc());
            APDLFunc->getInsts()[i]->delOperand(4);  // delete IPath
          }
        }
      }

      // NOP Insertion

      Back = APDLFunc->DupThisBlock(0);
      Org = APDLFunc->DupThisBlock(0);
      APDLFunc->getInsts().clear();

      /* Now DAG are built for current block, and all terminals
         are collected in LocalTerminals, while all possible entries are stored
         in GlobalTerminals. To insert NOPs correctly, the critical path in DAG
         should be emitted first. */
      MCParsedInst *p = NULL;

      std::stack<unsigned> sre;
      unsigned maxroutine;
      //uint64_t *order = ForwardDAG.TopologicalOrder(sre, maxroutine);
      int64_t *revorder = BackwardDAG.TopologicalOrder(sre, maxroutine);
      uint64_t length = revorder[maxroutine];
      while (APDLFunc->getInsts().size() <= length)
        APDLFunc->addParsedInst(NULL);
      for (unsigned i = 0; i <= Org->getInsts().size(); i++)
        revorder[i] = length - revorder[i];
      for (unsigned i = 0; i < Org->getInsts().size(); i++) {
        ForwardDAG.AddArc(NULL, Org->getInsts()[i], revorder[i + 1]);
        if (APDLFunc->getInsts()[revorder[i + 1]])
          APDLFunc->getInsts()[revorder[i + 1]]->append(Org->getInsts()[i]);
        else
          APDLFunc->addParsedInst(revorder[i + 1] + APDLFunc->getStart(),
                                  Org->getInsts()[i]);
      }
#if 0
      std::map<MCParsedInst *, MCParsedInst *> critical;
      if (DAG.Criticalpath(critical)) {
        MCParsedInst *Next = NULL;
        while (critical.count(Next)) {
          Org->delInst(critical[Next]);
          unsigned i = 0;
          while (++i < DAG.getArcWeight(Next, critical[Next]))
            APDLFunc->addParsedInst(NULL);
          APDLFunc->addParsedInst(critical[Next]);
          errs() << "C:" << Next << "->" << critical[Next] << "\n";
          Next = critical[Next];
        }
      } else errs() << "No critical path!\n";

      // Add non-critical vextors
      while (!Org->getInsts().empty()) {
        if (!p) p = Org->getInsts().front();
        uint64_t Pos = APDLFunc->size() - 1;//DAG.getArcWeight(NULL, p);
        uint64_t Base = 0;
        bool deterministic = true;
        /*if (SrcTable.count(p) && SrcTable[p].size()) {
          for (unsigned i = 0;
               i < SrcTable[p].size();
               i++) {
            if (VRegTable.count(SrcTable[p][i]) &&
                VRegTable[SrcTable[p][i]].producer) {
              if (!APDLFunc->findInstPos(VRegTable[SrcTable[p][i]].producer,
                                         Base)) {
                p = VRegTable[SrcTable[p][i]].producer;
                continue;
              } else {
                uint64_t Latency = MMPULiteScheduler::
                  getLatency(CAST_TO_MMPU_OPRD(
                               VRegTable[SrcTable[p][i]].producer
                                                         ->getOperand(0))
                                                         ->getOpc());
                if (Pos < Base + Latency)
                  Pos = Base + Latency;
              }
            } else errs() << "source operand is not found or has no producer\n";
          }
        } else Pos = APDLFunc->getInsts().size() - 1;*/
        if (DestTable.count(p) && DestTable[p].size()) {
          /* non-critical node doesn't have an exact location, so let it
             depend on the location of consumers of it's dest here. */
          for (unsigned i = 0; i < DestTable[p].size(); i++) {
            if (VRegTable.count(DestTable[p][i]) &&
                VRegTable[DestTable[p][i]].consumers.size()) {
              for (unsigned j = 0;
                   j < VRegTable[DestTable[p][i]].consumers.size();
                   j++) {
                MCParsedInst *consumer =
                  VRegTable[DestTable[p][i]].consumers[j];
                if (!APDLFunc->findInstPos(consumer, Base) &&
                    Org->findInstPos(consumer, Base)) {
                  p = consumer;
                  deterministic = false;
                  break;
                }
                uint64_t Latency = MMPULiteScheduler::
                getLatency(CAST_TO_MMPU_OPRD(p->getOperand(0))
                           ->getOpc());
                if (VRegTable[DestTable[p][i]].type !=
                    TagExprAST::STORAGE_MACREG) {
                  if (Pos > Base - Latency) Pos = Base - Latency;
                } else {
                  uint64_t Postpone = 0;
                  unsigned consumerdest = CAST_TO_MMPU_OPRD
                                          (consumer->getOperand(0))->getOpc();
                  if (consumerdest == IMA || (consumerdest >= IMAToBIU &&
                                              consumerdest <= IMAToSHU))
                    Postpone = 5;
                  else if (consumerdest >= IMAcc && consumerdest <= IMAccToSHU)
                    Postpone = 1;
                  else if (consumerdest >= IMaC && consumerdest <= IMaCToSHU)
                    Postpone = 5;
                  if (Pos > Base + Postpone - Latency)
                    Pos = Base + Postpone - Latency;
                }
              }
            } else errs() << "dest operand is not found or has no consumer\n";
            if (!deterministic) break;
          }
        }
        if (deterministic) {
          if (APDLFunc->getInsts()[Pos]) APDLFunc->getInsts()[Pos]->append(p);
          else APDLFunc->addParsedInst(Pos + APDLFunc->getStart(), p);
          Org->delInst(p);
          p = NULL;
        }
      }
      delete Org;
#endif
      /* After the first round of legalization, there might be some slot
         collisions which reside in MCParsedInst chains. */
      errs() << "Solve collision\n";
      for (unsigned i = 0; i < APDLFunc->size(); i++) {
        MCParsedInst *Head = APDLFunc->getInsts()[i];
        MCParsedInst *q = Head, *LastCol;
        if (Head) p = Head->getNext();
        else p = NULL;
        while (p) {
          int64_t weight = 0;
          while (p != q) {
            if (CheckCollision(q, p)) {
              LastCol = q;
              weight++;
            }
            q = q->getNext();
          }
          if (weight != 0) {
            SmallVector<SharedOperand, 8> n;
            n.push_back(SHARED_OPRD(Opc, NOP, SMLoc(), SMLoc()));
            MCParsedInst *MI = new MCParsedInst(n);
            BackwardDAG.InsertVextor(MI, p, 1);
            BackwardDAG.AddArc(MI, LastCol, 0);
          }
          p = p->getNext();
          q = Head;
        }
      }

      Org = Back->DupThisBlock(0);
      for (unsigned i = 0; i < Org->size(); i++)
        Org->getInsts()[i]->setNext(NULL);

      APDLFunc->getInsts().clear();
      //uint64_t *order = ForwardDAG.TopologicalOrder(sre, maxroutine);
      revorder = BackwardDAG.TopologicalOrder(sre, maxroutine);
      length = revorder[maxroutine];
      while (APDLFunc->getInsts().size() <= length)
        APDLFunc->addParsedInst(NULL);
      for (unsigned i = 0; i <= Org->getInsts().size(); i++)
        revorder[i] = length - revorder[i];
      for (unsigned i = 0; i < Org->getInsts().size(); i++) {
        if (APDLFunc->getInsts()[revorder[i + 1]])
          APDLFunc->getInsts()[revorder[i + 1]]->append(Org->getInsts()[i]);
        else
          APDLFunc->addParsedInst(revorder[i + 1] + APDLFunc->getStart(),
                                  Org->getInsts()[i]);
      }
      /*
      int64_t *order = ForwardDAG.TopologicalOrder(sre, maxroutine);
      while (APDLFunc->getInsts().size() <= uint64_t(order[maxroutine]))
        APDLFunc->addParsedInst(NULL);
      for (unsigned i = 0; i < Org->getInsts().size(); i++) {
        if (APDLFunc->getInsts()[order[i + 1]])
          APDLFunc->getInsts()[order[i + 1]]->append(Org->getInsts()[i]);
        else
          APDLFunc->addParsedInst(order[i + 1] + APDLFunc->getStart(),
                                  Org->getInsts()[i]);
      }
       */
#if 0
      critical.clear();
      if (DAG.Criticalpath(critical)) {
        MCParsedInst *Next = NULL;
        while (critical.count(Next)) {
          Org->delInst(critical[Next]);
          unsigned i = 0;
          while (++i < DAG.getArcWeight(Next, critical[Next]))
            APDLFunc->addParsedInst(NULL);
          APDLFunc->addParsedInst(critical[Next]);
          Next = critical[Next];
        }
      } else errs() << "No critical path!\n";

      // Add non-critical vextors
      while (!Org->getInsts().empty()) {
        if (!p) p = Org->getInsts().front();
        uint64_t Pos = APDLFunc->size() - 1;//DAG.getArcWeight(NULL, p);
        uint64_t Base = 0;
        bool deterministic = true;
        if (DestTable.count(p) && DestTable[p].size()) {
          for (unsigned i = 0; i < DestTable[p].size(); i++) {
            if (VRegTable.count(DestTable[p][i]) &&
                VRegTable[DestTable[p][i]].consumers.size()) {
              for (unsigned j = 0;
                   j < VRegTable[DestTable[p][i]].consumers.size();
                   j++) {
                MCParsedInst *consumer =
                  VRegTable[DestTable[p][i]].consumers[j];
                if (!APDLFunc->findInstPos(consumer, Base) &&
                    Org->findInstPos(consumer, Base)) {
                  p = VRegTable[DestTable[p][i]].consumers[j];
                  deterministic = false;
                  break;
                }
                uint64_t Latency = MMPULiteScheduler::
                getLatency(CAST_TO_MMPU_OPRD(p->getOperand(0))
                           ->getOpc());
                if (Pos > Base - Latency) Pos = Base - Latency;
              }
            } else errs() << "dest operand is not found or has no consumer\n";
            if (!deterministic) break;
          }
        }
        if (deterministic) {
          if (APDLFunc->getInsts()[Pos]) APDLFunc->getInsts()[Pos]->append(p);
          else APDLFunc->addParsedInst(Pos + APDLFunc->getStart(), p);
          Org->delInst(p);
          p = NULL;
        }
      }
      delete Org;
      delete Back;
#endif

      // All Insts have been legalized, update the GlobalTerminals
      uint64_t Latency = APDLFunc->getInsts().size();
      std::map<MCParsedInst *, uint64_t>::iterator iter;
      for (iter = GlobalTerminals.begin();
           iter != GlobalTerminals.end();
           iter++)
        if (iter->second < Latency) iter->second = 0;
        else iter->second -= Latency;
      for (unsigned i = 0; i < LocalTerminals.size(); i++) {
        O = CAST_TO_MMPU_OPRD(LocalTerminals[i]->getOperand(0));
        dest = O->getOpc();

        if (hasDest(O->getOpc())) {
          uint64_t Pos = 0;
          APDLFunc->findInstPos(LocalTerminals[i], Pos);
          if (MMPULiteScheduler::getLatency(dest) >= (Latency - 1 - Pos))
            GlobalTerminals[LocalTerminals[i]] =
              MMPULiteScheduler::getLatency(dest) - (Latency - 1 - Pos);
          else
            GlobalTerminals[LocalTerminals[i]] = 0;
          uint64_t vr;
          if (hasTDest(O->getOpc())) {
            O = CAST_TO_MMPU_OPRD
                (LocalTerminals[i]->getOperand(3));
            vr = O->getImm();
          } else if (hasMDest(O->getOpc())) {
            O = CAST_TO_MMPU_OPRD
                (LocalTerminals[i]->getOperand(2));
            vr = O->getImm();
          } else {
            O = CAST_TO_MMPU_OPRD
                (LocalTerminals[i]->getOperand(1));
            vr = O->getReg() - MMPULiteReg::BIU0;
          }
          LeafBlockInfo[APDLFunc].Terminal.push_back(vr);
        }
      }
    }
    /* FIXME: Loop is not properly dealed with yet. If there is any variable
       iterated between loops, it shall be announced as a PHY node */
    APDLFunc->RecalStart(0);
  } // end of APDLFunc
}
