//===-- MMPUIRGen.h - MMPU IRGen class -------------------------*- C++ -*-===//
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

#ifndef MMPUIRGEN_H
#define MMPUIRGEN_H

#include <cstdarg>
#include <vector>
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

//===----------------------------------------------------------------------===//
// Block
//===----------------------------------------------------------------------===//

namespace MMPU {
class Symbol;
class ExprAST;
class VariableExprAST;
class Block;

enum InstTy {
  add_i = 1,
  sub_i,
  mul_i,
  div_i,
  div_ui,
  mod_i,
  mod_ui,
  neg_i,
    
  cmplt_i,
  cmple_i,
  cmpge_i,
  cmpgt_i,
  cmpeq_i,
  cmpne_i,
    
  cmplt_ui,
  cmple_ui,
  cmpge_ui,
  cmpgt_ui,
    
  and_i,
  or_i,
  xor_i,
  not_i,
  shl_i,
  shr_i,
  shr_ui
};
  
class IRGen {
  std::vector<const std::string *> IRStack;
  uint64_t TempID;
  uint64_t LabelNum;
  raw_ostream *irout;
private:
  void Gen_LI(InstTy c,int64_t data, Symbol *s);
  
  void Gen_Code(InstTy c);
  
  /*
   void Gen_Int(int64_t c);
   */
  void Gen_Label(uint64_t c);
  
  void Gen_Jmp(ExprAST *Cond, uint64_t LTrue, uint64_t LFalse = 0);
  
  void Gen_Cast(ExprAST *Expr);
  
  void Gen_Binary_Op(ExprAST *Expr,int op_i, int op_ui);
  
  void Gen_ExprUnary(ExprAST *Expr, int op_i, int op_ui);
  
  void Gen_Assign(ExprAST *Expr);
  
  void Gen_ExprBinaryLogical(int op, ExprAST *Expr);
  
  void Gen_ExprCond(ExprAST *Expr);
  
  void Gen_LIdent(ExprAST *Expr);
  
  void Gen_RIdent(ExprAST *Expr);
  
  void Gen_Indir(ExprAST *Expr);
  
  void Gen_Index(ExprAST *Expr);
  
  void Gen_Const(ExprAST *Expr);
  
  void Gen_ExprCall(ExprAST *Expr);
  
  void Gen_ExprList(ExprAST *Expr);
  
  void Gen_LVal(ExprAST *E);
  
  void Gen_RVal(ExprAST *E);
  
  void Gen_Expr(ExprAST *E);
  
  void Gen_InstrIf1(ExprAST *Expr);
  
  void Gen_InstrIf2(void);
  
  void Gen_InstrIfElse2(void);
  
  void Gen_InstrIfElse3(void);
  
  void Gen_InstrWhile1(ExprAST *Expr);
  
  void Gen_InstrWhile2(void);
  
  void Gen_InstrDo1(void);
  
  void Gen_InstrDo2(ExprAST *Expr);
  
  void Gen_InstrFor1(ExprAST *Expr1, ExprAST *Expr2, Block* BFor);
  
  void Gen_InstrFor2(ExprAST *Expr3, Block *BFor);
  
  void Gen_InstrBreak(void);
  
  void Gen_InstrContinue(void);
  
  void Gen_InstrGoto(VariableExprAST *Ident);
  
  void Gen_InstrReturn(ExprAST *Expr);
  
  void Gen_InstrLabel(VariableExprAST *Ident);
  
  void Gen_InstrExpr(ExprAST *Expr);
  
  void Gen_InstrSwitch1(ExprAST *Expr);
  
  int64_t CreateNewLabel(void) { return ++LabelNum; }
  
  void Error(const char *format,...) {
    va_list ap;
    
    va_start(ap,format);
    vfprintf(stderr,format,ap);
    va_end(ap);
    
    fprintf(stderr,"\n");
    exit(1);
  }
public:
  IRGen(raw_ostream *o) : TempID(1), LabelNum(0), irout(o) { }
  void GenIR(Block *BAst);
  void Dump() {
    while (!IRStack.empty()) {
      *irout << *IRStack.back();
      delete IRStack.back();
      IRStack.pop_back();
    }
  }
};
}
#endif
