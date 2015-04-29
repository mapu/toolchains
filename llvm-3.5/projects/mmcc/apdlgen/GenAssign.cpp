//===---------- apdlgen/GenAssign.cpp - Generate Assignment--------------===//
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
void APDLGen::Gen_Assign(ExprAST *Expr) {
  ExprAST *e1 = NULL, *e2 = NULL;
  TagExprAST::TagTy dest;
  unsigned Fixup = 0;
  unsigned Fixedopc = 0;
  Symbol *s = NULL;
  SmallVector<SharedOperand, 8> Operands;

  if (Expr && Expr->getNext()->getNext() &&
      isa<ExprExprAST>(Expr->getNext()->getNext())) {
    e1 = dyn_cast<ExprExprAST>(Expr->getNext()->getNext())->getExpr();
    if (Expr->getNext()->getNext()->getNext() &&
        isa<ExprExprAST>(Expr->getNext()->getNext()->getNext()))
      e2 = dyn_cast<ExprExprAST>(Expr->getNext()->getNext()
                                     ->getNext())->getExpr();
  }

  dest = getLValType(e1);

  if (dest == TagExprAST::STORAGE_MACREG) {
    Gen_AssignOp(e1, e2);
    return;
  }

  if (dest != TagExprAST::STORAGE_KREG) {
    Operands.push_back(NULL); // Opcode
    Operands.push_back(NULL); // Dest
    if (dest == TagExprAST::STORAGE_TREG) {
      Operands.push_back(NULL); // T
      Operands.push_back(NULL); // I
    }
    if (dest != TagExprAST::STORAGE_DM) Operands.push_back(NULL); // VReg
    Operands.push_back(SHARED_OPRD(Reg, MMPULiteReg::NOC, SMLoc(), SMLoc()));

    Gen_RVal(e2, Operands);
  }
  switch (dest) {
  case TagExprAST::STORAGE_DM:
    if (e1 && e1->getNext() && e1->getNext()->getNext() &&
        isa<SymExprAST>(e1->getNext()->getNext())) {
      s = dyn_cast<SymExprAST>(e1->getNext()->getNext())->getSymbol();
      if (s) {
        if (s->getID() == -1) {
          s->Allocate(BIUID++);
        }
        DestStack.push_back(s->getID());
      }
    }
    Operands[1] = SHARED_OPRD(Reg, MMPULiteReg::BIU0 + s->getID(), SMLoc(),
                              SMLoc());
    Fixup = 0;
    /* it's not completed for BIU store.
     * A BIU store operation consists of two parts:
     * 1. ->BIUx, this is done by here
     * 2. BIUx->DM, this is emitted after 1.
     * the later one is a terminal inst */
    break;

  case TagExprAST::STORAGE_MREG:
    if (e1 && e1->getNext() && e1->getNext()->getNext() &&
        isa<SymExprAST>(e1->getNext()->getNext())) {
      s = dyn_cast<SymExprAST>(e1->getNext()->getNext())->getSymbol();
      if (s) {
        // if (s->getID() == -1)
        /* an MReg variable may be allocated several times, they are
         * different versions of the variable, hence to approach the
         * SSA purpose */
        s->Allocate(AllocateVReg(TagExprAST::STORAGE_MREG));
        DestStack.push_back(s->getID());
      }
    }
    Operands[1] = SHARED_OPRD(Reg, MMPULiteReg::M0, SMLoc(), SMLoc());
    Operands[2] = SHARED_OPRD(Imm, s->getID(), SMLoc(), SMLoc());
    Fixup = 1;
    break;

  case TagExprAST::STORAGE_TREG:
    /* use IALU.T0 to represent any unallocated TReg, and deal with TReg
       allocation later */
    Operands[1] = SHARED_OPRD(Reg, MMPULiteReg::IALU, SMLoc(), SMLoc());
    Operands[2] = SHARED_OPRD(Reg, MMPULiteReg::VT, SMLoc(), SMLoc());

    /* allocate a virtual TReg */
    if (e1 && e1->getNext() && e1->getNext()->getNext() &&
        isa<SymExprAST>(e1->getNext()->getNext())) {
      s = dyn_cast<SymExprAST>(e1->getNext()->getNext())->getSymbol();
      if (s) {
        //if (s->getID() == -1)
        s->Allocate(AllocateVReg(TagExprAST::STORAGE_TREG));
        DestStack.push_back(s->getID());
      }
    }
    Operands[3] = SHARED_OPRD(Imm, s->getID(), SMLoc(), SMLoc());
    /* here is an optional operand for IPath which depends on the
       dest of the next inst, determine it later */
    Operands[4] = SHARED_OPRD(Imm, 0, SMLoc(), SMLoc()); // IPath
    Fixup = 2;
    break;

  case TagExprAST::STORAGE_KREG:
    /* A KReg assignment is found, the only legal situation is that a loop
       block is following */
    if (e1 && e1->getNext() && e1->getNext()->getNext() &&
        isa<SymExprAST>(e1->getNext()->getNext())) {
      s = dyn_cast<SymExprAST>(e1->getNext()->getNext())->getSymbol();
      if (s) {
        s->Allocate(AllocateKReg());
      }
      PrevSym = s;
    }
    return;

  default:
    break;
  }

  unsigned tag = TagExprAST::INVALIDTAG;
  if (e2 && e2->getNext() && isa<TagExprAST>(e2->getNext()))
    tag = dyn_cast<TagExprAST>(e2->getNext())->getTag();
  if (tag != TagExprAST::EXPR_IDENT && tag!= TagExprAST::EXPR_APDLINC) {
    Fixedopc = CAST_TO_MMPU_OPRD(Operands[0])->getOpc() + Fixup;
    //delete Operands[0];

    Operands[0] = SHARED_OPRD(Opc, Fixedopc, SMLoc(), SMLoc());
  }

  addParsedInst(new MCParsedInst(Operands));
  if (CAST_TO_MMPU_OPRD(Operands[0])->getOpc() >= IMRToBIU &&
      CAST_TO_MMPU_OPRD(Operands[0])->getOpc() <= IMRToSHU)
    FragmentInsts.push_back(APDLFunc->getInsts().back());

  if (dest == TagExprAST::STORAGE_DM) {
    Operands.clear();
    Operands.push_back(SHARED_OPRD(Opc, (BIU0St + s->getID() * (BIU1St - BIU0St)),
                               SMLoc(), SMLoc()));
    Operands.push_back(SHARED_OPRD(Reg, MMPULiteReg::NOC, SMLoc(), SMLoc()));
    /* Flags should be checked if here is BIU or BIU++ to determine if (A++, K++)
       is required, but in practical, flags are always needed by DM store. */
    Flags.set(APPF);
    Flags.set(KPPF);
    Operands.push_back(SHARED_OPRD(Imm, Flags.to_ulong(), SMLoc(), SMLoc()));
    Flags.reset();
    Operands.push_back(SHARED_OPRD(Slot, 10 + s->getID(), SMLoc(), SMLoc()));
    SrcStack.push_back(s->getID());
    addParsedInst(new MCParsedInst(Operands));
  }
}
}
