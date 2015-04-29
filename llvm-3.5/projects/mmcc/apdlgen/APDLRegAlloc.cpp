//===-------- apdlgen/APDLRegAlloc.cpp - APDL Register Allocation--------===//
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
void APDLGen::RegAllocation(void) {
  // Initialize all register pools

  typedef std::vector<struct lifespan> history;
  history IALUTRF[100];
  history FALUTRF[100];
  history IMACTRF[100];
  history FMACTRF[100];
  history SHU0TRF[100];
  history SHU1TRF[100];
  history MRF[128];

  history *RegFiles[7] = {IALUTRF, FALUTRF, IMACTRF, FMACTRF,
                          SHU0TRF, SHU1TRF, MRF};

  int32_t regclass = -1;
  uint64_t birth = 0, retire = 0, loop = 1, loopsize = 0;
  uint64_t latency = 0, pos = 0;
  unsigned poolsize = 0;

  for (uint64_t i = 3; i < VRegTable.size(); i++) {
    // Calculate the lifespan of this virtual register
    if (VRegTable[i].type == TagExprAST::STORAGE_MACREG) {
      for (unsigned k = 0; k < VRegTable[i].consumers.size(); k++)
        for (unsigned j = 0; j < VRegTable[i].consumers[k]->getOperands()->size(); j++) {
          if (CAST_TO_MMPU_OPRD
              ((*VRegTable[i].consumers[k]->getOperands())[j])->isReg() &&
              CAST_TO_MMPU_OPRD
              ((*VRegTable[i].consumers[k]->getOperands())[j])->getReg() ==
              MMPULiteReg::MR) {
            VRegTable[i].consumers[k]->delOperand(j);
            VRegTable[i].consumers[k]->delOperand(j);
          }
        }
      if (VRegTable[i].producer) {
        for (unsigned j = 0; j < VRegTable[i].producer->getOperands()->size(); j++) {
          if (CAST_TO_MMPU_OPRD
              ((*VRegTable[i].producer->getOperands())[j])->isReg() &&
              CAST_TO_MMPU_OPRD
              ((*VRegTable[i].producer->getOperands())[j])->getReg() ==
              MMPULiteReg::MR) {
            VRegTable[i].producer->delOperand(j);
            VRegTable[i].producer->delOperand(j);
          }
        }
      }
      continue;
    }
    if (VRegTable[i].producer &&
        !VRegTable[i].consumers.empty()) {
      VRegTable[i].producer->getParent()->findInstPos(VRegTable[i].producer,
                                                      pos);
      latency = MMPULiteScheduler::
                getLatency(CAST_TO_MMPU_OPRD
                           (VRegTable[i].producer->getOperand(0))->getOpc());
      birth = VRegTable[i].producer->getParent()->getStart() + pos + latency;
      retire = 0;
      for (unsigned j = 0; j < VRegTable[i].consumers.size(); j++) {
        MCParsedInst *consumer = VRegTable[i].consumers[j];
        consumer->getParent()->findInstPos(VRegTable[i].consumers[j], pos);
        if (retire < consumer->getParent()->getStart() + pos)
          retire = consumer->getParent()->getStart() + pos;
        /* Calculate the retire time is very complex, it must meet the following
           requirements:
           1. the top Loop;
           2. block that doesn't include the producer;
          */
        MCLoopBlock *Ref = consumer->getParent();
        MCLoopBlock *Res = NULL;
        while (Ref) {
          if (Ref == VRegTable[i].producer->getParent()) break;
          if (Ref->getType() == Loop) {
            if (Ref->getKI() == VRegTable[i].producer->getParent()->getKI())
              // they are sub loops splitted from the same loop
              break;
            Res = Ref;
          }
          Ref = Ref->getParent();
        }
        if (Res) retire = Res->getStart() + Res->size(); // FIXME: reserved too much
      }
      if (VRegTable[i].producer->getParent()->getType() == Loop) {
        loop = LoopInfoMap[VRegTable[i].producer->getParent()->getKI()].OrigSize /
               LoopInfoMap[VRegTable[i].producer->getParent()->getKI()].SplitSize;
        loopsize = LoopInfoMap[VRegTable[i].producer->getParent()->getKI()].SplitSize;
      }
      if (retire == 0) errs() << "Consumer is not found.\n";
    } else errs() << "VReg was allocated but is never used.\n";

    // Get the register class of this virtual register
    if (VRegTable[i].type == TagExprAST::STORAGE_MREG) {
      regclass = 6;
    } else if (VRegTable[i].type == TagExprAST::STORAGE_TREG &&
               VRegTable[i].producer) {
      switch (CAST_TO_MMPU_OPRD(VRegTable[i].producer->getOperand(1))
                ->getReg()) {
      case MMPULiteReg::IALU:
        regclass = 0;
        break;
      case MMPULiteReg::FALU:
        regclass = 1;
        break;
      case MMPULiteReg::IMAC:
        regclass = 2;
        break;
      case MMPULiteReg::FMAC:
        regclass = 3;
        break;
      case MMPULiteReg::SHU0:
        regclass = 4;
        break;
      case MMPULiteReg::SHU1:
        regclass = 5;
        break;
      default:
        regclass = -1;
        break;
      }
    }
    if (regclass == -1) errs() << "Unknown register class.\n";
    if (regclass == 6) poolsize = 128;
    else poolsize = 4;
    unsigned j;
    for (j = 0; j < poolsize; j++) {
      bool allocatable = true;
      for (unsigned k = 0; k < RegFiles[regclass][j].size(); k++)
        for (unsigned newloop = 0; newloop < loop; newloop++) {
          for (unsigned testloop = 0;
               testloop < RegFiles[regclass][j][k].loop;
               testloop++) {
            if (((birth + newloop * loopsize >=
                  RegFiles[regclass][j][k].birth +
                  testloop * RegFiles[regclass][j][k].loopsize) &&
                 (birth + newloop * loopsize <=
                  RegFiles[regclass][j][k].retire +
                  testloop * RegFiles[regclass][j][k].loopsize)) ||
                ((retire + newloop * loopsize >=
                  RegFiles[regclass][j][k].birth +
                  testloop * RegFiles[regclass][j][k].loopsize) &&
                 (retire + newloop * loopsize <=
                  RegFiles[regclass][j][k].retire +
                  testloop * RegFiles[regclass][j][k].loopsize))) {
              allocatable = false;
              break;
            }
          }
          if (!allocatable) break;
        }
      if (!allocatable) continue;
      RegFiles[regclass][j].push_back(*new struct lifespan);
      RegFiles[regclass][j].back().birth = birth;
      RegFiles[regclass][j].back().retire = retire;
      RegFiles[regclass][j].back().loop = loop;
      RegFiles[regclass][j].back().loopsize = loopsize;
      if (regclass == 6) {
        //delete VRegTable[i].producer->getOperand(1);
        (*VRegTable[i].producer->getOperands())[1] =
          SHARED_OPRD(Reg, MMPULiteReg::M0 + j, SMLoc(), SMLoc());
        VRegTable[i].producer->delOperand(2);
        for (unsigned k = 0; k < VRegTable[i].consumers.size(); k++) {
          for (unsigned l = 0;
               l < VRegTable[i].consumers[k]->getOperands()->size();
               l++) {
            if ((*VRegTable[i].consumers[k]->getOperands())[l]->isReg() &&
                (*VRegTable[i].consumers[k]->getOperands())[l]->getReg()
                 == MMPULiteReg::M0) {
              if (l + 1 < VRegTable[i].consumers[k]->getOperands()->size() &&
                  (*VRegTable[i].consumers[k]->getOperands())[l + 1]->isImm() &&
                  CAST_TO_MMPU_OPRD
                  ((*VRegTable[i].consumers[k]->getOperands())[l + 1])->getImm() ==
                  int64_t(i)) {
                //delete (*VRegTable[i].consumers[k]->getOperands())[l];
                (*VRegTable[i].consumers[k]->getOperands())[l] =
                  SHARED_OPRD(Reg, MMPULiteReg::M0 + j, SMLoc(), SMLoc());
                VRegTable[i].consumers[k]->delOperand(l + 1);
              }
            }
          }
        }
      } else {
        //delete VRegTable[i].producer->getOperand(2);
        (*VRegTable[i].producer->getOperands())[2] =
          SHARED_OPRD(Reg, MMPULiteReg::T0 + j, SMLoc(), SMLoc());
        VRegTable[i].producer->delOperand(3);
        for (unsigned k = 0; k < VRegTable[i].consumers.size(); k++) {
          for (unsigned l = 0;
               l < VRegTable[i].consumers[k]->getOperands()->size();
               l++) {
            if ((*VRegTable[i].consumers[k]->getOperands())[l]->isReg() &&
                (*VRegTable[i].consumers[k]->getOperands())[l]->getReg()
                == MMPULiteReg::VT) {
              if (l + 1 < VRegTable[i].consumers[k]->getOperands()->size() &&
                  (*VRegTable[i].consumers[k]->getOperands())[l + 1]->isImm() &&
                  CAST_TO_MMPU_OPRD
                  ((*VRegTable[i].consumers[k]->getOperands())[l + 1])->getImm() ==
                  int64_t(i)) {
                //delete (*VRegTable[i].consumers[k]->getOperands())[l];
                (*VRegTable[i].consumers[k]->getOperands())[l] =
                  SHARED_OPRD(Reg, MMPULiteReg::T0 + j, SMLoc(), SMLoc());
                VRegTable[i].consumers[k]->delOperand(l + 1);
              }
            }
          }
        }
      }

      break; // successfully allocated, stop searching physical registers
    }
    if (j == poolsize) errs() << "Regs " << regclass << " out of use.\n";
    errs() << regclass << " RA " << i << ": " << birth << ":" << retire << ":"
           << loop << ":" << loopsize << "\n";
    // FIXME: whatif TReg are out of use?
  }
}

void APDLGen::ResourceAllocation(void) {
  std::vector<lifespan> IPath[3];
  uint64_t latency, ocuppy, loop, loopsize; // Only is birth(ocuppy) used for IPath
  MCParsedInst *MPI;
  for (unsigned i = 0; i < HMacros.size(); i++) {
    if (!HMacros[i]->isNested()) {
      for (unsigned j = 0; j < HMacros[i]->size(); j++) {
        HMacros[i]->getParsedInst(HMacros[i]->getStart() + j, MPI);
        while (MPI) {
          unsigned op =
            CAST_TO_MMPU_OPRD(MPI->getOperand(0))->getOpc();
          if (UseIPath(op)) {
            latency = MMPULiteScheduler::getLatency(op, MMPULiteScheduler::I0);
            ocuppy = HMacros[i]->getStart() + j + latency;
            if (HMacros[i]->getType() == Loop) {
              loop = LoopInfoMap[HMacros[i]->getKI()].OrigSize /
                     LoopInfoMap[HMacros[i]->getKI()].SplitSize;
              loopsize = loop = LoopInfoMap[HMacros[i]->getKI()].SplitSize;
            } else {
              loop = 1;
              loopsize = 0;
            }
            unsigned k = 0;
            for (; k < 3; k++) {
              bool allocatable = true;
              for (unsigned l = 0; l < IPath[k].size(); l++) {
                for (unsigned newloop = 0; newloop < loop; newloop++) {
                  for (unsigned testloop = 0;
                       testloop < IPath[k][l].loop;
                       testloop++) {
                    if (ocuppy + newloop * loopsize ==
                        IPath[k][l].birth + testloop * IPath[k][l].loopsize) {
                      allocatable = false;
                      break;
                    }
                  }
                  if (!allocatable) break;
                }
              }
              if (!allocatable) continue;
              //delete MPI->getOperand(3);
              (*MPI->getOperands())[3] = SHARED_OPRD(Imm, k + 1, SMLoc(), SMLoc());
              IPath[k].push_back(*new struct lifespan);
              IPath[k].back().birth = ocuppy;
              IPath[k].back().loop = loop;
              IPath[k].back().loopsize = loopsize;
              break;
            }
            if (k == 3) errs() << "IPaths are out of use.\n";
          }
          MPI = MPI->getNext();
        }
      }
    }
  }
}
}
