//===-- MMPUCompiler.h - MMPUCompiler class ----------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===--------------------------------------------------------------------===//
//
// This class stores the data for the MMPUCompiler compiler so it doesn't have
// to pass all of it around.  The main method is parse.
//
//===--------------------------------------------------------------------===//

#ifndef MMPUCOMPILER_H
#define MMPUCOMPILER_H

#include "MMPUSymbol.h"
#include "MMPUAST.h"
#include "VMInfo.h"
#include "MMPUIRGen.h"
#include "MMPUAPDLGen.h"
#include "MMPUBlock.h"

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"

/// This class provides a parser for the MMPUCompiler language.
/// The class itself is made to store values during
/// parsing so they don't have to be passed around
/// as much.

namespace MMPU {
void Error(const char *format, ...);
  
 
class MMPUCompiler {
  std::map<const std::string, Symbol *> Sym_Table;
  Block *CurBlock, *DeclBlock, *FuncBlock, *GlobalBlock;
  Symbol *LocalVarSym;
  uint64_t LocalVarSize;
  raw_ostream *apdlout, *asmout;
  IRGen *IRgen;
  
  /* APDL Generation */
  APDLGen *APDLgen;
  bool isDefiningAPDL;
  Block *APDLBlock;
  unsigned OptLevel;
  
public:
  Module *Mod;
  LLVMContext &Ctx;
  std::istream *In;
  IRBuilder<> &Builder;
  std::map<std::string, AllocaInst*> NamedValues;
  
  MMPUCompiler(std::istream *in, raw_ostream *out1,
               raw_ostream  *out2, LLVMContext& C)
    : CurBlock(NULL), DeclBlock(NULL), FuncBlock(NULL), GlobalBlock(NULL),
      LocalVarSym(NULL), apdlout(out1), asmout(out2), IRgen(new IRGen(out2)),
      APDLgen(new APDLGen(out1)), isDefiningAPDL(false), APDLBlock(NULL),
      OptLevel(0),
      Ctx(C), In(in), Builder(*(new IRBuilder<>(getGlobalContext()))) {
    Block_Enter(Block::BLOCK_GLOBAL);
    GlobalBlock = CurBlock;
    DeclBlock = CurBlock;
    Mod = new Module("Algebra Instruction", Ctx);
  }
  
  Module *parse();
  
  void Block_Enter(Block::BlockTy Type);
  
  void Block_Leave(void);
  
  void addChild(Block *B) { B->setParent(CurBlock); CurBlock->addChild(B); }
  
  void setControlLabels(int64_t Break, int64_t Continue, int64_t Restart) {
    CurBlock->setControlLabels(Break, Continue, Restart);
  }
  
  Symbol *LookupSym(const std::string &Name, Symbol::SymbolTy Type) const;
  
  void AddSymbol(Symbol *S) { Sym_Table[S->getName()] = S; };
  
  Symbol *CreateSymbol(const std::string *Name,
                       Symbol::SymbolTy Type, ExprAST *E, Block *B);
  
  Symbol *CreateSymbol(const std::string *Name,
                       Symbol::SymbolTy Type, MMPU::ExprAST *E) {
    return CreateSymbol(Name, Type, E, DeclBlock);
  }
  
  void Func_Declare(ExprAST *Spec, ExprAST *Decl, ExprAST *VarList);
  
  void Func_End(void);
  
  ExprAST *Type_VarList(ExprAST *Spec, ExprAST *Decls);
  
  TagExprAST::TagTy Spec_Parse(TagExprAST::TagTy &Storage,
                               Symbol* &Sym, ExprAST *Spec);
  
  void Global_Declare(std::string &VarName,
                      TagExprAST::TagTy VarStorage,
                      ExprAST *VarType);
  
  void Var_Declare(ExprAST *VarList);
  
  void Local_Static(const std::string &VarName, TagExprAST::TagTy VarStorage,
                    ExprAST *VarType, ExprAST *VarInit);
  
  void Local_Declare(std::string &VarName, TagExprAST::TagTy Storage,
                     ExprAST *VarType, ExprAST *VarInit);

  ExprAST *Type_ArrayDeclare(ExprAST *Type, ExprAST *Expr);
  
  ExprAST *Type_FuncDeclare(TagExprAST::TagTy FuncType, ExprAST *VarList);
  
  ExprAST *ConstEval(ExprAST *Expr);
  
  void BeginCompoundStmts(void);
  
  void EndCompoundStmts(void);
  
  IRGen *getIRGen(void) const { return IRgen; }
  
  void GenCurBlock(void) { IRgen->GenIR(CurBlock); }
  
  void setDefineAPDL(void) { isDefiningAPDL = true; }

  void clearDefineAPDL(void) { isDefiningAPDL = false; }
  
  void setOptLevel(unsigned Level) { OptLevel = Level; }
  
  void GenAPDL() {
    if (APDLBlock) {
      APDLgen->setOptLevel(OptLevel);
      APDLgen->GenAPDLInst(APDLBlock);
      APDLgen->Legalize();
      APDLgen->GlobalSchedule();
      APDLgen->SoftPipe();
      APDLgen->GenAPDLTree();
      APDLgen->RegAllocation();
      APDLgen->ResourceAllocation();
      APDLgen->FixupFragmentInst();
      APDLgen->Emit();
    }
  }
  
  void GenMacroCode();
};
}
#endif
