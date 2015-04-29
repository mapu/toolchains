//===-- apdlgen/APDLGenMisc.cpp - Miscellaneous Generate Func----------===//
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
// getLValType: get the storage type of the left hand side variable
TagExprAST::TagTy APDLGen::getLValType(ExprAST *E) {
  TagExprAST::TagTy tag = TagExprAST::INVALIDTAG;
  Symbol *s = NULL;
  TagExprAST *storage = NULL;

  if (E && E->getNext() && isa<TagExprAST>(E->getNext()))
    tag = dyn_cast<TagExprAST>(E->getNext())->getTag();

  if (tag != TagExprAST::EXPR_IDENT && tag != TagExprAST::EXPR_APDLINC)
    return tag;

  if (E->getNext()->getNext() && isa<SymExprAST>(E->getNext()->getNext()))
    s = dyn_cast<SymExprAST>(E->getNext()->getNext())->getSymbol();

  if (s && s->getExpr())
    storage = dyn_cast_or_null<TagExprAST>(s->getExpr()->getNext());

  if (storage) return storage->getTag();
  else return TagExprAST::INVALIDTAG;
}
bool APDLGen::hasDest(unsigned Opc) const {
  unsigned index = 0;
  if (Opc >= BIU0KG) {
    index = Opc - BIU0KG;
    if (MMPULiteScheduler::ResUsageMap[index] &
        (MMPULiteScheduler::R_SHUT | MMPULiteScheduler::R_MACCT |
         MMPULiteScheduler::R_MW | MMPULiteScheduler::R_BIUW |
         (1 << MMPULiteScheduler::IMRW)))
      return true;
    else return false;
  }
  return false;
}

bool APDLGen::hasTDest(unsigned Opc) const {
  unsigned index = 0;
  if (Opc >= BIU0KG) {
    index = Opc - BIU0KG;
    if (MMPULiteScheduler::ResUsageMap[index] &
        (MMPULiteScheduler::R_SHUT | MMPULiteScheduler::R_MACCT))
      return true;
    else return false;
  }
  return false;
}

bool APDLGen::hasMDest(unsigned Opc) const {
  unsigned index = 0;
  if (Opc >= BIU0KG) {
    index = Opc - BIU0KG;
    if (MMPULiteScheduler::ResUsageMap[index] &
        (MMPULiteScheduler::R_MW | (1 << MMPULiteScheduler::IMRW)))
      return true;
    else return false;
  }
  return false;
}

bool APDLGen::UseIPath(unsigned Opc) const {
  unsigned index = 0;
  if (Opc >= BIU0KG) {
    index = Opc - BIU0KG;
    if (MMPULiteScheduler::ResUsageMap[index] & MMPULiteScheduler::R_IPATH)
      return true;
    else return false;
  }
  return false;
}

bool APDLGen::CheckCollision(MCParsedInst *I1, MCParsedInst *I2) const {
  if (!I1 || !I2) return false;
  unsigned Slot1 = CAST_TO_MMPU_OPRD
  (I1->getOperand(I1->size() - 1))->getSlot();
  unsigned Slot2 = CAST_TO_MMPU_OPRD
  (I2->getOperand(I2->size() - 1))->getSlot();
  return Slot1 == Slot2;
}
}
