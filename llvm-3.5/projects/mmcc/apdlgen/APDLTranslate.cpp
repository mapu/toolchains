//===---------- apdlgen/APDLTranslate.cpp - APDL Translation-----------===//
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
MCInst *APDLGen::Translate(MCParsedInst *PInst) {
  MCInst *Inst = new MCInst;
  MCInst *CurInst = Inst, *NewInst;
  SharedMMPUOprd op;
  MCOperand MO;

  while (PInst) {
    SharedOperandVector *ops = PInst->getOperands();
    for(unsigned i=0; ops && i < ops->size(); i++) {
      op = CAST_TO_MMPU_OPRD((*ops)[i]);
      switch(op->getKind()) {
      default:
        return NULL;

      case AsmOpcode:
        if (op->getOpc() == MMPULite::NOP) break;
        NewInst = new MCInst;
        MO = MCOperand::CreateInst(NewInst);
        CurInst->addOperand(MO);
        CurInst = NewInst;
        CurInst->setOpcode(op->getOpc());
        CurInst->setLoc(op->getStartLoc());
        Inst->addOperand(MO); // There is a redundant MCOperand for the first MCInst.
        break;

      case AsmRegister:
        MO = MCOperand::CreateReg(op->getReg());
        CurInst->addOperand(MO);
        Inst->addOperand(MO);
        break;

      case AsmImmediate:
        MO = MCOperand::CreateImm(op->getImm());
        CurInst->addOperand(MO);
        Inst->addOperand(MO);
        break;

      case AsmExpression:
        MO = MCOperand::CreateExpr(op->getExpr());
        CurInst->addOperand(MO);
        Inst->addOperand(MO);
        break;

      case AsmHMacro:
        break;

      case AsmSlot:
        break;
      }
    }
    PInst = PInst->getNext();
  }
  if (Inst->getNumOperands() == 0) {
    NewInst = new MCInst();
    NewInst->setOpcode(MMPULite::NOP);
    Inst->addOperand(MCOperand::CreateInst(NewInst));
    CurInst = NewInst;
  }
  return Inst;
}
}
