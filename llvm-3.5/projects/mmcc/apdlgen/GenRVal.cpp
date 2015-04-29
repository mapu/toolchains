//===------- apdlgen/GenRVal.cpp - Generate Right Side Value-----------===//
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
void APDLGen::Gen_RVal(ExprAST *E, SharedOperandVector &O) {

  TagExprAST::TagTy tag = TagExprAST::INVALIDTAG;
  SmallVector<SharedOperand, 8> BackO;
  uint64_t tempT;

  if (E && E->getNext() && isa<TagExprAST>(E->getNext()))
    tag = dyn_cast<TagExprAST>(E->getNext())->getTag();

  switch (tag) {

  case TagExprAST::EXPR_INT:
    Gen_Const(E, O);
    break;

  case TagExprAST::EXPR_IDENT:
  case TagExprAST::EXPR_APDLINC:
    Gen_RIdent(E, O);
    break;

  case TagExprAST::EXPR_ASSIGN:
    /* adjacent two assignments are not supported */
    break;

  case TagExprAST::EXPR_ADD:
  case TagExprAST::EXPR_SUB:
  case TagExprAST::EXPR_SADD:
  case TagExprAST::EXPR_SSUB:
  case TagExprAST::EXPR_MOD:
  case TagExprAST::EXPR_AND:
  case TagExprAST::EXPR_OR:
  case TagExprAST::EXPR_XOR:
  case TagExprAST::EXPR_SHR:
  case TagExprAST::EXPR_SHL:

  case TagExprAST::EXPR_LT:
  case TagExprAST::EXPR_LE:
  case TagExprAST::EXPR_GT:
  case TagExprAST::EXPR_GE:
  case TagExprAST::EXPR_EQ:
  case TagExprAST::EXPR_NE:
    if (needTemp) {
      tempT = AllocateVReg(TagExprAST::STORAGE_TREG);
      O.push_back(SHARED_OPRD(Reg, MMPULiteReg::VT, SMLoc(), SMLoc()));
      O.push_back(SHARED_OPRD(Imm, tempT, SMLoc(), SMLoc()));
      for (unsigned i = 0; i < O.size(); i++)
        BackO.push_back(O[i]);
      O.clear();
      O.push_back(SHARED_OPRD(Opc, MMPULite::IALUBinToMACC, SMLoc(), SMLoc())); // Opcode
      O.push_back(SHARED_OPRD(Reg, MMPULiteReg::IALU, SMLoc(), SMLoc()));
      O.push_back(SHARED_OPRD(Reg, MMPULiteReg::VT, SMLoc(), SMLoc())); // T
      O.push_back(SHARED_OPRD(Imm, tempT, SMLoc(), SMLoc())); // VReg
      O.push_back(SHARED_OPRD(Imm, 0, SMLoc(), SMLoc())); // I
      O.push_back(SHARED_OPRD(Reg, MMPULiteReg::NOC, SMLoc(), SMLoc()));
    } else
      O[0] = SHARED_OPRD(Opc, MMPULite::IALUBinToBIU, SMLoc(), SMLoc());
    Gen_IALU_Binary(E, tag, O);
    if (needTemp) {
      DestStack.push_back(tempT);
      addParsedInst(new MCParsedInst(O));
      O.clear();
      for (unsigned i = 0; i < BackO.size(); i++)
        O.push_back(BackO[i]);
      BackO.clear();
    }
    break;

  case TagExprAST::EXPR_MUL:
    if (needTemp) {
      tempT = AllocateVReg(TagExprAST::STORAGE_TREG);
      O.push_back(SHARED_OPRD(Reg, MMPULiteReg::VT, SMLoc(), SMLoc()));
      O.push_back(SHARED_OPRD(Imm, tempT, SMLoc(), SMLoc()));
      for (unsigned i = 0; i < O.size(); i++)
        BackO.push_back(O[i]);
      O.clear();
      O.push_back(SHARED_OPRD(Opc, MMPULite::IMulToMACC, SMLoc(), SMLoc())); // Opcode
      O.push_back(SHARED_OPRD(Reg, MMPULiteReg::IALU, SMLoc(), SMLoc()));
      O.push_back(SHARED_OPRD(Reg, MMPULiteReg::VT, SMLoc(), SMLoc())); // T
      O.push_back(SHARED_OPRD(Imm, tempT, SMLoc(), SMLoc())); // VReg
      O.push_back(SHARED_OPRD(Imm, 0, SMLoc(), SMLoc())); // I
      O.push_back(SHARED_OPRD(Reg, MMPULiteReg::NOC, SMLoc(), SMLoc()));
    } else
      O[0] = SHARED_OPRD(Opc, MMPULite::IMulToBIU, SMLoc(), SMLoc());
    Gen_IMAC_Binary(E, tag, O);
    if (needTemp) {
      DestStack.push_back(tempT);
      addParsedInst(new MCParsedInst(O));
      O.clear();
      for (unsigned i = 0; i < BackO.size(); i++)
        O.push_back(BackO[i]);
      BackO.clear();
    }
    break;

  case TagExprAST::EXPR_DIV:

    break;

  case TagExprAST::EXPR_NOT:
      //Gen_ExprUnary(E, not_i, not_i);
    break;
  case TagExprAST::EXPR_NEG:
      //Gen_ExprUnary(E, neg_i, neg_i);
    break;

  case TagExprAST::EXPR_LAND:
  case TagExprAST::EXPR_LOR:
    /* unsupported */
    break;

  case TagExprAST::EXPR_COND:
      //Gen_ExprCond(E);
    break;
  default:
    break;
  }
}
}
