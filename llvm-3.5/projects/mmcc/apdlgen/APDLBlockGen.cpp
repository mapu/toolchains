//===---------- apdlgen/APDLBlockGen.cpp - APDL Block Generator----------===//
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
void APDLGen::genBlock(MMPU::Block *B) {
  Block *child;
  MCLoopBlock *NewLoop;
  for (unsigned i = 0; i < B->Size(); i++) {
    child = B->getChild(i);
    switch (child->getType()) {
    case Block::BLOCK_AST:
      genAST(child->getAST());
      break;

    case Block::BLOCK_WHILE:
      /* Reorganize the structure of MCLoopBlock in order to insert
       * a loop block. */
      child->setIndex(PrevSym);
      if (APDLFunc->getType() == Loop)
        APDLFunc = APDLFunc->addSubBlock(
          new MCLoopBlock(Loop, 0, 0, child->getIndex()->getID()));
      else if (APDLFunc->size() == 0)
        APDLFunc->setType(Loop, 0, child->getIndex()->getID());
      else if (APDLFunc->getParent())
        APDLFunc = APDLFunc->addSiblingBlock(
          new MCLoopBlock(Loop, 0, 0, child->getIndex()->getID()));
      NewLoop = APDLFunc;
      genBlock(child);
      LoopInfoMap[NewLoop->getKI()].KReg = NewLoop->getKI();
      if (!NewLoop->isNested()) {
        LoopInfoMap[NewLoop->getKI()].OrigSize = NewLoop->size();
        LoopInfoMap[NewLoop->getKI()].SplitSize = NewLoop->size();
      }
      APDLFunc = APDLFunc->addSubBlock(new MCLoopBlock(Sequential, 0));
      break;

    case Block::BLOCK_DECL:
      genBlock(child);
      break;

    default:
      break;
    }
  }
  if (B->getType() == Block::BLOCK_WHILE)
    APDLFunc = APDLFunc->getParent();
}
}
