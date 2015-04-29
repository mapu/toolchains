//===-- apdlgen/GenConst.cpp - Generate constant-----------------------===//
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
void APDLGen::Gen_Const(ExprAST *E, SharedOperandVector &O) {
  int64_t c = 0;
  if (E && E->getNext() && E->getNext()->getNext() &&
        isa<IntExprAST>(E->getNext()->getNext()))
    c = dyn_cast<IntExprAST>(E->getNext()->getNext())->getVal();
  O.push_back(SHARED_OPRD(Imm, c, SMLoc(), SMLoc()));
}
}
