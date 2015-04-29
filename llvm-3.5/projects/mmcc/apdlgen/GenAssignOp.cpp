//===-- apdlgen/GenAssignOp.cpp - Generate Assignment Operation----------===//
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
void APDLGen::Gen_AssignOp(ExprAST *D, ExprAST *E) {
  SmallVector<SharedOperand, 8> Operands;

  TagExprAST::TagTy tag = TagExprAST::INVALIDTAG,
                    tag1 = TagExprAST::INVALIDTAG,
                    tag2 = TagExprAST::INVALIDTAG;
  Symbol *s = NULL;
  ExprAST *e1 = NULL, *e2 = NULL;

  if (E && E->getNext() && isa<TagExprAST>(E->getNext()))
    tag = dyn_cast<TagExprAST>(E->getNext())->getTag();

  if (E && E->getNext()->getNext() &&
      isa<ExprExprAST>(E->getNext()->getNext())) {
    e1 = dyn_cast<ExprExprAST>(E->getNext()->getNext())->getExpr();
    if (E->getNext()->getNext()->getNext() &&
        isa<ExprExprAST>(E->getNext()->getNext()->getNext()))
      e2 = dyn_cast<ExprExprAST>(E->getNext()->getNext()->getNext())->getExpr();
  }

  enum macinst {
    NA = 0,
    MAC,
    MAC_CR,
    MA,
    MA_CR,
    ACC,
    ACC_CR
  } insttype = NA;

  std::vector<ExprAST *> SubExpr;
  switch (tag) {
  case TagExprAST::EXPR_INT:
    // mr = 0
    Gen_Const(E, Operands);
    if (CAST_TO_MMPU_OPRD(Operands.back())->getImm())
      errs() << "Initializing MR with non-zero immediate number.\n";
    else {
      /* Clearing mr register is found isolated from any mac operation.
         Save this event until it can be determined that with which mac
         operation it works. */
      if (D && D->getNext() && D->getNext()->getNext() &&
          isa<SymExprAST>(D->getNext()->getNext())) {
        s = dyn_cast<SymExprAST>(D->getNext()->getNext())->getSymbol();
        // Allocate a VReg for a mac variable with NO producer.
        if (s) s->Allocate(AllocateVReg(TagExprAST::STORAGE_MACREG));
      }
    }
    return;

  case TagExprAST::EXPR_IDENT:
    // mr += ident (CR)
    if (getLValType(E) != TagExprAST::STORAGE_MACREG) {
      SubExpr.push_back(E);
      insttype = ACC_CR;
    }
    else errs() << "Found illegal expression \"mr = mr\" ?\n";
    break;

  case TagExprAST::EXPR_SADD:
  case TagExprAST::EXPR_ADD:
    // mr += ...
    if (e1 && e1->getNext() && isa<TagExprAST>(e1->getNext()))
      tag1 = dyn_cast<TagExprAST>(e1->getNext())->getTag();
    if (e2 && e2->getNext() && isa<TagExprAST>(e2->getNext()))
      tag2 = dyn_cast<TagExprAST>(e2->getNext())->getTag();
    if (tag1 == TagExprAST::EXPR_IDENT && tag2 == TagExprAST::EXPR_IDENT) {
      if ((getLValType(e1) == TagExprAST::STORAGE_MACREG) ^
          (getLValType(e2) == TagExprAST::STORAGE_MACREG)) {
        // mr += ident
        insttype = ACC;
        if (getLValType(e1) == TagExprAST::STORAGE_MACREG) {
          SubExpr.push_back(e2);
          E = e2;
        } else {
          SubExpr.push_back(e1);
          E = e1;
        }
      } if ((getLValType(e1) != TagExprAST::STORAGE_MACREG) &&
            (getLValType(e2) != TagExprAST::STORAGE_MACREG)) {
        // expr = ident + ident
        // mr += expr (CR)
        insttype = ACC_CR;
        SubExpr.push_back(E);
      } else errs() << "Found illegal expression \"mr += mr\" ?\n";
    } else if ((tag1 == TagExprAST::EXPR_IDENT) ^
               (tag2 == TagExprAST::EXPR_IDENT)) {
      if ((getLValType(e1) == TagExprAST::STORAGE_MACREG) ||
          (getLValType(e2) == TagExprAST::STORAGE_MACREG)) {
        if ((tag1 == TagExprAST::EXPR_MUL) || (tag2 == TagExprAST::EXPR_MUL)) {
          // mr += expr * expr
          insttype = MAC;
          ExprAST *Mul;
          if (tag1 == TagExprAST::EXPR_MUL) {
            E = e2;
            Mul = e1;
          } else {
            Mul = e2;
            E = e1;
          }
          if (Mul && Mul->getNext()->getNext() &&
              isa<ExprExprAST>(Mul->getNext()->getNext())) {
            e1 = dyn_cast<ExprExprAST>(Mul->getNext()->getNext())->getExpr();
            if (Mul->getNext()->getNext()->getNext() &&
                isa<ExprExprAST>(Mul->getNext()->getNext()->getNext()))
              e2 = dyn_cast<ExprExprAST>(Mul->getNext()->getNext()->getNext())->getExpr();
          }
          SubExpr.push_back(e1);
          SubExpr.push_back(e2);
        } else {
          // mr += expr
          insttype = ACC;
          if (getLValType(e1) == TagExprAST::STORAGE_MACREG) {
            SubExpr.push_back(e2);
            E = e1;
          } else {
            SubExpr.push_back(e1);
            E = e2;
          }
        }
      } else {
        // mr += expr (CR)
        insttype = ACC_CR;
        SubExpr.push_back(E);
      }
    } else {
      // mr += expr + expr (CR)
      insttype = ACC_CR;
      SubExpr.push_back(E);
    }

    break;

  case TagExprAST::EXPR_MUL:
    // mr += expr * expr (CR)
    insttype = MAC_CR;
    SubExpr.push_back(e1);
    SubExpr.push_back(e2);
    break;

  default:
    // mr += expr (CR)
    SubExpr.push_back(E);
    insttype = ACC_CR;
    break;
  }

  switch (insttype) {
  case ACC:
  case ACC_CR:
    Operands.push_back(SHARED_OPRD(Opc, MMPULite::IMAcc, SMLoc(), SMLoc()));
    break;

  case MA:
  case MA_CR:
    errs() << "MA\n";
    Operands.push_back(SHARED_OPRD(Opc, MMPULite::IMA, SMLoc(), SMLoc()));
    break;

  case MAC:
  case MAC_CR:
    Operands.push_back(SHARED_OPRD(Opc, MMPULite::IMaC, SMLoc(), SMLoc()));
    break;

  default:
    break;
  }

  Operands.push_back(SHARED_OPRD(Reg, MMPULiteReg::MR, SMLoc(), SMLoc()));
  Operands.push_back(NULL); // VReg ID
  Operands.push_back(SHARED_OPRD(Reg, MMPULiteReg::NOC, SMLoc(), SMLoc()));
  std::vector<uint64_t> src;
  bool BackofNT = needTemp;
  needTemp = true;
  for (unsigned i = 0; i < SubExpr.size(); i++) {
    Gen_RVal(SubExpr[i], Operands);
    src.push_back(CAST_TO_MMPU_OPRD(Operands.back())->getImm());
  }
  needTemp = BackofNT;
  SrcStack = src;

  /* generate flags */
  switch (insttype) {
  case ACC_CR:
  case MA_CR:
  case MAC_CR:
    Flags.set(CRF);
    break;

  case ACC:
  case MA:
  case MAC:
    if (E && E->getNext() && E->getNext()->getNext() &&
        isa<SymExprAST>(E->getNext()->getNext())) {
      s = dyn_cast<SymExprAST>(E->getNext()->getNext())->getSymbol();
      SrcStack.push_back(s->getID());
    }
    break;

  default:
    break;
  }

  Operands.push_back(SHARED_OPRD(Imm, Flags.to_ulong(), SMLoc(), SMLoc()));
  Flags.reset();

  Operands.push_back(SHARED_OPRD(Slot, 7, SMLoc(), SMLoc()));

  if (D && D->getNext() && D->getNext()->getNext() &&
      isa<SymExprAST>(D->getNext()->getNext())) {
    s = dyn_cast<SymExprAST>(D->getNext()->getNext())->getSymbol();
    s->Allocate(AllocateVReg(TagExprAST::STORAGE_MACREG));
    DestStack.push_back(s->getID());
  }
  Operands[2] = SHARED_OPRD(Imm, s->getID(), SMLoc(), SMLoc());
  addParsedInst(new MCParsedInst(Operands));
}
}
