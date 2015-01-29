//===-- MMPUSymbol.cpp - MMPU Symbol  ------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===--------------------------------------------------------------------===//
//
// This class compiles the serial Microcode program language into LLVM assembly.
//
//
//===--------------------------------------------------------------------===//

#include "MMPUSymbol.h"
#include <iostream>

namespace MMPU {

uint64_t Symbol::sym_noname_id = 0;
  
TagExprAST::TagTy Symbol::getStorage() const {
  TagExprAST *storage = dyn_cast_or_null<TagExprAST>(expr->getNext());
  if (storage) return storage->getTag();
  else return TagExprAST::INVALIDTAG;
}
}
