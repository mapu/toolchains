//===-- MMPUBlock.h - MMPU Block class --------------------------*- C++ -*-===//
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

#ifndef MMPUBLOCK_H
#define MMPUBLOCK_H

#include <vector>
#include <string>

//===----------------------------------------------------------------------===//
// Block
//===----------------------------------------------------------------------===//

namespace MMPU {
class Symbol;
class ExprAST;
class SymExprAST;
  
class Block {

public:
  enum BlockTy{
    BLOCK_WHILE = 1,
    BLOCK_SWITCH,
    BLOCK_IF,
    BLOCK_DECL,
    BLOCK_GLOBAL,
    BLOCK_AST,
    BLOCK_RET
  };
  
private:
  BlockTy type;
  Block *parent;
  Symbol *sym_first;
  std::vector<Block *> Children;
  ExprAST *ast;
  int64_t label_break, label_continue, label_restart;
  bool isAPDL;
  Symbol *index;
public:
  int64_t local_var_offset;
  SymExprAST *se;
public:
  Block(BlockTy _T, Block *_P)
    : type(_T), parent(_P), sym_first(NULL), ast(NULL),
      isAPDL(false), index(NULL) {
    if (parent) isAPDL = parent->isAPDL;
  }
  Symbol *getFirstSym() const { return sym_first; }
  void setFirstSym(Symbol *S) { sym_first = S; }
  Block *getParent() const { return parent; }
  void setParent(Block *B) { parent = B; isAPDL = B->isAPDL; }
  BlockTy getType() const { return type; }
  void setType(const BlockTy _T) { type = _T; }
  bool isAPDLBlock() const { return isAPDL; }
  void setAPDL(bool Is) { isAPDL = Is; }
  Symbol *getIndex() const { return index; }
  void setIndex(Symbol *I) { index = I; }
  void addChild(Block *_B) {
    Children.push_back(_B);
    _B->setParent(this);
    _B->isAPDL = isAPDL; }
  Block *getChild(const unsigned Index) const {
    if (Index < Children.size()) return Children[Index];
    else return NULL;
  }
  unsigned Size() const { return Children.size(); }
  void setAST(ExprAST* AST) { ast = AST; }
  ExprAST *getAST() const { return ast; }
  void setControlLabels(int64_t Break, int64_t Continue, int64_t Restart = 0) {
    label_break = Break;
    label_continue = Continue;
    label_restart = Restart;
  }
  int64_t getBreak() const { return label_break; }
  int64_t getContinue() const { return label_continue; }
  int64_t getRestart() const { return label_restart; }
};
}
#endif
