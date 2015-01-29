//===-- MMPUSymbol.h - MMPU Symbol class --------------------------*- C++ -*-===//
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

#ifndef MMPUSYMBOL_H
#define MMPUSYMBOL_H

#include "MMPUBlock.h"
#include "MMPUAST.h"

//===----------------------------------------------------------------------===//
// Symbol
//===----------------------------------------------------------------------===//

namespace MMPU {
  
class Block;
  
class Symbol {
public:
enum SymbolTy {
  TABLE_VAR = 1,    /* symbol table for variable */
  TABLE_LABEL,    /* labels for goto */
  TABLE_STRUCT   /* structures/unions/enums */
};
  
enum APDLStorageTy {
  UNKNOWN = 0,
  DM = 1,
  MREG,
  TREG,
  KREG
};
  
private:
  std::string name;
  SymbolTy type;
  Block *block;
  ExprAST *expr;
  Symbol *block_next;
  static uint64_t sym_noname_id;
  
  int64_t allocaid;

public:
  Symbol(const std::string &Name, SymbolTy Type, Block *B, ExprAST *E)
    : name(Name), type(Type),
      block(B), expr(E), block_next(B->getFirstSym()),
      allocaid(-1) {
    B->setFirstSym(this);
  }
  Symbol(SymbolTy Type, Block *B, ExprAST *E)
    : name("@S"), type(Type),
      block(B), expr(E), block_next(B->getFirstSym()),
      allocaid(-1) {
    B->setFirstSym(this);
  }
  ExprAST *getExpr() const { return expr; }
  void setExpr(ExprAST *E) { expr = E; }
  SymbolTy getType() const { return type; }
  const std::string &getName() const { return name; }
  Block *getBlock() const { return block; }
  Symbol *getNext() const { return block_next; }
  void Allocate(int64_t ID) { allocaid = ID; }
  int64_t getID() const { return allocaid; }
  TagExprAST::TagTy getStorage() const;
};
}
#endif
