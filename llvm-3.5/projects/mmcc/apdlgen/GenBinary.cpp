//===---------- apdlgen/GenBinary.cpp - Generate Binary Expr-------------===//
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

void APDLGen::Gen_IALU_Binary(ExprAST *E, TagExprAST::TagTy Op,
                              SharedOperandVector &O) {
  ExprAST *e1 = NULL, *e2 = NULL;
  uint64_t src = 0;

  switch (Op) {
  case TagExprAST::EXPR_SADD:
  case TagExprAST::EXPR_ADD:
    O.push_back(SHARED_OPRD(Reg, MMPULiteReg::IADD, SMLoc(), SMLoc()));
    break;

  case TagExprAST::EXPR_SSUB:
  case TagExprAST::EXPR_SUB:
    O.push_back(SHARED_OPRD(Reg, MMPULiteReg::ISUB, SMLoc(), SMLoc()));
    break;

      //case TagExprAST::EXPR_MUL:
      //case TagExprAST::EXPR_DIV:
      //case TagExprAST::EXPR_MOD:
  case TagExprAST::EXPR_AND:
    O.push_back(SHARED_OPRD(Reg, MMPULiteReg::AND, SMLoc(), SMLoc()));
    break;
  case TagExprAST::EXPR_OR:
    O.push_back(SHARED_OPRD(Reg, MMPULiteReg::OR, SMLoc(), SMLoc()));
    break;
  case TagExprAST::EXPR_XOR:
    O.push_back(SHARED_OPRD(Reg, MMPULiteReg::XOR, SMLoc(), SMLoc()));
    break;
  case TagExprAST::EXPR_SHR:
    O.push_back(SHARED_OPRD(Reg, MMPULiteReg::RSHT, SMLoc(), SMLoc()));
    break;
  case TagExprAST::EXPR_SHL:
    O.push_back(SHARED_OPRD(Reg, MMPULiteReg::LSHT, SMLoc(), SMLoc()));
    break;

  case TagExprAST::EXPR_LT:
    O.push_back(SHARED_OPRD(Reg, MMPULiteReg::IST, SMLoc(), SMLoc()));
    break;
  case TagExprAST::EXPR_LE:
    O.push_back(SHARED_OPRD(Reg, MMPULiteReg::INLT, SMLoc(), SMLoc()));
    break;
  case TagExprAST::EXPR_GT:
    O.push_back(SHARED_OPRD(Reg, MMPULiteReg::ILT, SMLoc(), SMLoc()));
    break;
  case TagExprAST::EXPR_GE:
    O.push_back(SHARED_OPRD(Reg, MMPULiteReg::INST, SMLoc(), SMLoc()));
    break;
  case TagExprAST::EXPR_EQ:
    O.push_back(SHARED_OPRD(Reg, MMPULiteReg::IEQU, SMLoc(), SMLoc()));
    break;
  case TagExprAST::EXPR_NE:
    O.push_back(SHARED_OPRD(Reg, MMPULiteReg::INEQ, SMLoc(), SMLoc()));
    break;

  default:
    break;
  }

  if (E && E->getNext() && E->getNext()->getNext() &&
      isa<ExprExprAST>(E->getNext()->getNext())) {
    e1 = dyn_cast<ExprExprAST>(E->getNext()->getNext())->getExpr();
    if (E->getNext()->getNext()->getNext() &&
        isa<ExprExprAST>(E->getNext()->getNext()->getNext()))
      e2 = dyn_cast<ExprExprAST>(E->getNext()->getNext()->getNext())->getExpr();
  }

  bool BackupNT = needTemp;
  needTemp = true;
  Gen_RVal(e1, O);
  src = CAST_TO_MMPU_OPRD(O.back())->getImm();
  Gen_RVal(e2, O);
  needTemp = BackupNT;

  SrcStack.push_back(src);
  src = CAST_TO_MMPU_OPRD(O.back())->getImm();
  SrcStack.push_back(src);

  // The third operand of binary ops is not used, but is required by printer
  O.push_back(SHARED_OPRD(Reg, MMPULiteReg::T0, SMLoc(), SMLoc()));

  /* generate flags */
  switch (Op) {
  case TagExprAST::EXPR_ADD:
  case TagExprAST::EXPR_SUB:
    Flags.set(TF);
    break;

  default:
    break;
  }
  O.push_back(SHARED_OPRD(Imm, Flags.to_ulong(), SMLoc(), SMLoc()));
  Flags.reset();

  O.push_back(SHARED_OPRD(Slot, 6, SMLoc(), SMLoc()));
}

void APDLGen::Gen_IMAC_Binary(ExprAST *E, TagExprAST::TagTy Op,
                              SharedOperandVector &O) {
  ExprAST *e1 = NULL, *e2 = NULL;
  uint64_t src = 0;

  switch (Op) {
  case TagExprAST::EXPR_MUL:
  //case TagExprAST::EXPR_DIV:
  //case TagExprAST::EXPR_MOD:
    O.push_back(SHARED_OPRD(Reg, MMPULiteReg::IMUL, SMLoc(), SMLoc()));
    break;

  default:
    break;
  }

  if (E && E->getNext()->getNext() &&
      isa<ExprExprAST>(E->getNext()->getNext())) {
    e1 = dyn_cast<ExprExprAST>(E->getNext()->getNext())->getExpr();
    if (E->getNext()->getNext()->getNext() &&
        isa<ExprExprAST>(E->getNext()->getNext()->getNext()))
      e2 = dyn_cast<ExprExprAST>(E->getNext()->getNext()->getNext())->getExpr();
  }

  bool BackofNT = needTemp;
  needTemp = true;
  Gen_RVal(e1, O);
  src = CAST_TO_MMPU_OPRD(O.back())->getImm();
  Gen_RVal(e2, O);
  needTemp = BackofNT;

  SrcStack.push_back(src);
  src = CAST_TO_MMPU_OPRD(O.back())->getImm();
  SrcStack.push_back(src);

  /* generate flags */
  switch (Op) {
  case TagExprAST::EXPR_MUL:
    Flags.set(TF);
    break;

  default:
    break;
  }
  O.push_back(SHARED_OPRD(Imm, Flags.to_ulong(), SMLoc(), SMLoc()));
  Flags.reset();

  O.push_back(SHARED_OPRD(Slot, 7, SMLoc(), SMLoc()));
}
