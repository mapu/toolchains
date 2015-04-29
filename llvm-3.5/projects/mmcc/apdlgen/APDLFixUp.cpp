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
void APDLGen::FixupFragmentInst(void) {
  /* Try to merge dmem/treg/mreg = mac and mac += ... into
   dmem/treg/mreg = mr += ...
   */
  uint64_t vr = 0;
  unsigned fixup = 0;
  while (!FragmentInsts.empty()) {
    MCParsedInst *FI = FragmentInsts.front();
    errs() << "fix FI:" << FI << ":" << FI->getParent() << "\n";
    FragmentInsts.erase(FragmentInsts.begin());
    unsigned opc = CAST_TO_MMPU_OPRD(FI->getOperand(0))->getOpc();
    FI->getParent()->delInst(FI);
    switch (opc) {
    case IMRToBIU:
    case IMRToM:
    case IMRToMACC:
    case IMRToSHU:
      fixup = opc - IMRToBIU + 1;
      vr = SrcTable[FI][0];
        if (VRegTable[vr].producer) {
          opc = CAST_TO_MMPU_OPRD
                (VRegTable[vr].producer->getOperand(0))->getOpc();
          if (VRegTable[vr].producer->getParent() != FI->getParent()) {
            MCLoopBlock *parent = VRegTable[vr].producer->getParent();
            if (FI->getParent()->getType() == Loop)
              errs() << "Fragment insts reside in different loops";
            else {
              while (FI->getParent()->getParent() != parent) {
                if (parent) parent = parent->getParent();
                else break;
              }
              if (FI->getParent()->getParent() != parent)
                errs() << "Failed to merge Fragment inst\n";
              else {

              }
            }
          }
          switch (opc) {
            case IMA:
              errs() << "mr = a + b * c -> dmem/mreg/treg is not supported\n";
              break;
            case IMaC:
              //delete (*VRegTable[vr].producer->getOperands())[0];
              VRegTable[vr].producer->getOperands()
                ->erase(VRegTable[vr].producer->getOperands()->begin());
              for (unsigned i = 1; i < FI->getOperands()->size() - 3; i++) {
                VRegTable[vr].producer->getOperands()
                  ->insert(VRegTable[vr].producer->getOperands()->begin(),
                           (*FI->getOperands())[i]);
              }
              VRegTable[vr].producer->getOperands()
                ->insert(VRegTable[vr].producer->getOperands()->begin(),
                         SHARED_OPRD(Opc, opc + fixup, SMLoc(), SMLoc()));

              break;
            case IMAcc:
              break;
            default:
              break;
          }
        }

      break;

    default:
      break;
    }
  }
}
}
