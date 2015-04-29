//===---------- apdlgen/APDLASTGen.cpp - APDL AST generator--------------===//
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
void APDLGen::genAST(ExprAST *E) {
  /* most probably this is an expression statement */
  TagExprAST::TagTy tag = TagExprAST::INVALIDTAG;
  
  if (E && E->getNext() && isa<TagExprAST>(E->getNext()))
    tag = dyn_cast<TagExprAST>(E->getNext())->getTag();
  
  switch (tag) {
  case TagExprAST::EXPR_ASSIGN:
    Gen_Assign(E);
    break;
      
  default:
    break;
  }
}
}
