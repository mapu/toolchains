//===-- MMPUAST.h - MMPU AST classes --------------------------*- C++ -*-===//
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

#ifndef MMPUAST_H
#define MMPUAST_H

#include <vector>
#include <string>
#include "llvm/Support/Casting.h"

using namespace llvm;

namespace MMPU {
class Block;
class Symbol;
class MMPUCompiler;
}

using namespace MMPU;

//===----------------------------------------------------------------------===//
// Abstract Syntax Tree (aka Parse Tree)
//===----------------------------------------------------------------------===//

namespace MMPU {
/// ExprAST - Base class for all expression nodes.
class ExprAST {
public:
  enum ASTTy {
    Invalid = 0,
    Expr = 1,
    Integer,
    Float,
    String,
    Tag,
    Sym
  };
  
protected:
  const ASTTy type;
  ExprAST *Next;
  
  ExprAST(ASTTy _type) : type(_type), Next(NULL) {}
  
  ExprAST(ASTTy _type, ExprAST *_E) : type(_type) , Next(_E) {}

public:
  virtual ~ExprAST() { delete Next; }
  
  //virtual Value *Codegen(MMPUCompiler *mc) = 0;
  
  static ExprAST *append(ExprAST *Head, ExprAST *NewTail) {
    if (Head == NULL) return NewTail;
    ExprAST *Temp = Head;
    while (Temp->Next != NULL) Temp = Temp->Next;
    Temp->Next = NewTail;
    return Head;
  }
  
  ExprAST *getNext() const { return Next; }
  ASTTy getType() const { return type; }
  
  virtual ExprAST *Dup() const { return new ExprAST(Expr, Next); };
  
  ExprAST *CEval();
  ExprAST *CCast();
  ExprAST *CBinary();
  ExprAST *CUnary();
  int64_t ConstVal();
  bool isConst();
  void CheckLValue(void);
  void BreakList() { Next = NULL; }
  
  /*
   * Generating a 'cast'
   * No test is performed to check the validity of the conversion
   * Type.
   */
  static ExprAST *CreateCast(ExprAST *NewType, ExprAST *E1);
  /*
   * Generate an expression of type 'type1' from the expression 'e2'.
   * if possible using the rules of the assignment.
   */
  static ExprAST *CreateCastAssign(ExprAST *Type1, ExprAST *E2);
  /*
   * Promotion Integer
   */
  static ExprAST *CreatePromoteInteger(ExprAST *E);
  /*
   * Application of common conversions on two operands of a
   * Binary
   */
  static void BinaryCast(ExprAST *&Res1, ExprAST *&Res2,
                         ExprAST *E1, ExprAST *E2);
  /*
   * Type checking an integer binary operation
   */
  static ExprAST *CreateBinaryInteger(const unsigned Op,
                                      ExprAST *E1, ExprAST *E2);
  /*
   * Type a binary arithmetic operation
   * We do not floating so it's easier :)
   */
  static ExprAST *CreateBinaryArith(const unsigned Op,
                                    ExprAST *E1, ExprAST *E2);
  /*
   * Generating a constant of type 'int'
   */
  static ExprAST *CreateConstInt(const uint64_t C);
  /*
   * Generating a list of expressions. If there is only one
   * expression, it returns the only expression.
   *
   * The type is the one of the last expression
   */
  static ExprAST *CreateList(ExprAST *ExprList);
  /*
   * Generation and control of the type for the addition or subtraction
   */
  static ExprAST *CreateAddSub(unsigned Op, ExprAST *E1, ExprAST *E2);
  /*
   * Type checking a shift expression
   */
  static ExprAST *CreateBinaryShift(unsigned Op, ExprAST *E1, ExprAST *E2);
  /*
   * Type checking a comparison expression
   * It is not the comparison of pointer with 0
   */
  static ExprAST *CreateBinaryCompare(unsigned Op, ExprAST *E1, ExprAST *E2);
  /*
   * Generation and control of the type for a member of a logical expression
   * EXPR_LAND, EXPR_LOR, EXPR_LNOT
   */
  static ExprAST *CreateLogicalInt(ExprAST *Expr);
  /*
   * Type of a logical expression EXPR_LAND, EXPR_LOR
   */
  static ExprAST *CreateBinaryLogical(unsigned Op, ExprAST *E1, ExprAST *E2);
  static ExprAST *CreateBinary(unsigned Op, ExprAST *E1, ExprAST *E2);
  static ExprAST *CreateUnary(unsigned Op, ExprAST *E1);
  static ExprAST *CreateIndir(ExprAST *E1);
  /*
   * operator '[]'
   */
  static ExprAST *CreateIndex(ExprAST *E1, ExprAST *E2);
  /*
   * Type test for the operator '&'
   * We do not test if it is a record
   */
  static ExprAST *CreateAddr(ExprAST *E);
  /*
   * Taking a field of a structure
   */
  static ExprAST *CreateField(ExprAST *E1, const std::string &Name,
                              MMPUCompiler *mc);
  static ExprAST *CreateAssign(ExprAST *E1, ExprAST *E2);
  /*
   * Generating an expression for the assignment with operation
   * If the expression on the left is too complicated, it generates a variable
   * for temporarily storing the address
   */
  static ExprAST *CreateAssignOp(unsigned Op, ExprAST *E1, ExprAST *E2,
                                 MMPUCompiler *mc);
  /*
   * Typing the phrase '? :'
   * Standard Ansi not strictly implemented for pointers
   */
  static ExprAST *CreateCond(ExprAST *E, ExprAST *E1, ExprAST *E2);
  /*
   * Calling a function
   */
  static ExprAST *CreateCall(ExprAST *E1, ExprAST *Params1);
  /*
   * Creating an expression containing an identifier
   */
  static ExprAST *CreateIdent(const std::string &Name, MMPUCompiler *mc);
  /*
   * Management of the operator 'sizeof'
   */
  static ExprAST *CreateSizeof(ExprAST *E, bool isExpr);
};
  
/// IntExprAST - Expression class for integer.
class InvalidExprAST : public ExprAST {
  
public:
  InvalidExprAST() : ExprAST(Invalid) {}
  static bool classof(const ExprAST *E) { return false; }
};

/// IntExprAST - Expression class for integer.
class IntExprAST : public ExprAST {
  int64_t Val;

public:
  IntExprAST(int64_t val) : ExprAST(Integer), Val(val) {}
  IntExprAST(int64_t val, ExprAST *expr) : ExprAST(Integer, expr), Val(val) {}
  virtual ExprAST *Dup() const {
    return new IntExprAST(Val, getNext() ? getNext()->Dup() : NULL);
  }
  int64_t getVal() const { return Val; }
  //virtual Value *Codegen(MMPUCompiler *mc);
  static bool classof(const ExprAST *E) { return E->getType() == Integer; }
};

/// FPExprAST - Expression class for float point.
class FPExprAST : public ExprAST {
  double Val;
  
public:
  FPExprAST(double val) : ExprAST(Float), Val(val) {}
  FPExprAST(double val, ExprAST *expr) : ExprAST(Float, expr), Val(val) {}
  virtual ExprAST *Dup() const {
    return new FPExprAST(Val, getNext() ? getNext()->Dup() : NULL);
  }
  //virtual Value *Codegen(MMPUCompiler *mc);
  static bool classof(const ExprAST *E) { return E->getType() == Float; }
};

/// TagExprAST - Expression class for tag.
class TagExprAST : public ExprAST {
  
public:
enum TagTy {
  INVALIDTAG = 0,
  TYPE_VOID = 1,
  TYPE_CHAR,
  TYPE_UCHAR,
  TYPE_SHORT,
  TYPE_USHORT,
  TYPE_INT,
  TYPE_UINT,
  TYPE_POINTER,
  
  TYPE_FUNC,
  TYPE_ARRAY,
  TYPE_STRUCT,
  TYPE_UNION,
  TYPE_ENUM,
  TYPE_TYPEDEF,
  
  QUALIF_CONST,
  QUALIF_VOLATILE,
  
  TYPE_VAR_IDENT,
  TYPE_TYPEDEF_IDENT,
  TYPE_UNSIGNED,
  TYPE_SIGNED,
  
  STORAGE_AUTO,
  STORAGE_REGISTER,
  STORAGE_STATIC,
  STORAGE_EXTERN,
  STORAGE_TYPEDEF,
  STORAGE_DEFAULT,
  
  SYM_VAR,
  SYM_TYPEDEF,
  SYM_STRUCT,
  SYM_UNION,
  SYM_ENUM,
  SYM_ENUM_CONST,
    
  VAR_STACK,
  VAR_DATA,
  VAR_CODE,
  
  FUNC_NEW,
  FUNC_ELLIPSIS,
  FUNC_OLD,
  
  EXPR_INT,
  EXPR_STR,
    
  EXPR_IDENT,
  EXPR_CAST,
  
  EXPR_CALL,
  EXPR_INDIR,
  EXPR_INDEX,
  EXPR_ADDR,
  EXPR_ASSIGN,
  
  EXPR_ADD,
  EXPR_SUB,
  EXPR_MUL,
  EXPR_DIV,
  EXPR_MOD,
  EXPR_NEG,
  EXPR_PLUS,
  
  EXPR_AND,
  EXPR_OR,
  EXPR_XOR,
  EXPR_NOT,
  EXPR_SHR,
  EXPR_SHL,
  
  EXPR_LT,
  EXPR_LE,
  EXPR_EQ,
  EXPR_GE,
  EXPR_GT,
  EXPR_NE,
  
  EXPR_LAND,
  EXPR_LOR,
  EXPR_LNOT,
  
  EXPR_LIST,
  EXPR_COND,
  
  INIT_EXPR,
  INIT_LIST,
  
  /* APDL specified tags */
  EXPR_APDLINC,
  
  EXPR_SADD,
  EXPR_SSUB,
  
  STORAGE_DM,
  STORAGE_MREG,
  STORAGE_TREG,
  STORAGE_KREG,
  STORAGE_MACREG,
  EXPR_PAIR
  /*
  STORAGE_APDL,*/
};

private:
  TagTy Val;
  
public:
  TagExprAST(TagTy val) : ExprAST(Tag), Val(val) {}
  TagExprAST(TagTy val, ExprAST *expr) : ExprAST(Tag, expr), Val(val) {}
  TagExprAST(unsigned val) : ExprAST(Tag), Val(TagTy(val)) {}
  TagExprAST(unsigned val, ExprAST *expr)
    : ExprAST(Tag, expr), Val(TagTy(val)) {}
  TagTy getTag() const { return Val; }
  void setTag(TagExprAST::TagTy T) { Val = T; }
  virtual ExprAST *Dup() const {
    return new TagExprAST(Val, getNext() ? getNext()->Dup() : NULL);
  }
  //virtual Value *Codegen(MMPUCompiler *mc);
  bool isInteger() {
    return (Val >= TYPE_CHAR && Val <= TYPE_UINT) ||
    Val == TYPE_ENUM;
  }
  bool isArith() { return isInteger(); }
  ExprAST *isPointer() {
    if (Val == TYPE_POINTER) return getNext();
    else if ((Val == TYPE_ARRAY) && getNext()) return getNext()->getNext();
    else return new InvalidExprAST();
  }
  int64_t TypeSize();
  bool operator ==(TagExprAST &RHS) const;
  bool operator !=(TagExprAST &RHS) const {return !(*this == RHS); };
  static bool classof(const ExprAST *E) { return E->getType() == Tag; }
};

/// VariableExprAST - Expression class for referencing a variable, like "a".
class VariableExprAST : public ExprAST {
  std::string Name;
public:
  VariableExprAST(const std::string &name) : ExprAST(String), Name(name) {}
  VariableExprAST(const std::string &name, ExprAST *expr)
    : ExprAST(String, expr), Name(name) {}
  const std::string &getName() const { return Name; }
  virtual ExprAST *Dup() const {
    std::string name(Name);
    return new VariableExprAST(name, getNext() ? getNext()->Dup() : NULL);
  }
  static bool classof(const ExprAST *E) { return E->getType() == String; }

  //virtual Value *Codegen(MMPUCompiler *mc);
};

/// ExprExprAST - Expression class for referencing an ExprAST.
class ExprExprAST : public ExprAST {
  ExprAST *E;
public:
  ExprExprAST(ExprAST *expr) : ExprAST(Expr), E(expr) {}
  ExprExprAST(ExprAST *expr1, ExprAST *expr2)
    : ExprAST(Expr, expr2), E(expr1) {}
  virtual ~ExprExprAST() { delete E; }
  ExprAST *getExpr() const { return E; }
  void setExpr(ExprAST *expr) { E = expr; }
  virtual ExprAST *Dup() const {
    return new ExprExprAST(E ? E->Dup() : NULL,
                           getNext() ? getNext()->Dup() : NULL);
  }
  static bool classof(const ExprAST *E) { return E->getType() == Expr; }
  //virtual Value *Codegen(MMPUCompiler *mc);
};
  
/// SymExprAST - Expression class for referencing an ExprAST.
class SymExprAST : public ExprAST {
  Symbol *S;
public:
  SymExprAST(Symbol *_S) : ExprAST(Sym), S(_S) {}
  SymExprAST(Symbol *_S, ExprAST *expr)
    : ExprAST(Sym, expr), S(_S) {}
  Symbol *getSymbol() const { return S; }
  virtual ExprAST *Dup() const {
    return new SymExprAST(S, getNext() ? getNext()->Dup() : NULL);
  }
  static bool classof(const ExprAST *E) { return E->getType() == Sym; }
  //virtual Value *Codegen(MMPUCompiler *mc);
};

#if 0
/// BinaryExprAST - Expression class for a binary operator.
class BinaryExprAST : public ExprExprAST {
  char Op;
  ExprAST *LHS, *RHS;
public:
  BinaryExprAST(char op, ExprAST *lhs, ExprAST *rhs)
    : ExprExprAST(lhs), Op(op), LHS(lhs), RHS(rhs) {}
  virtual Value *Codegen(MMPUCompiler *mc);
};
  
/// UnaryExprAST - Expression class for a unary operator.
class UnaryExprAST : public ExprExprAST {
  char Opcode;
  ExprAST *Operand;
public:
  UnaryExprAST(char opcode, ExprAST *operand)
    : ExprExprAST(operand), Opcode(opcode), Operand(operand) {}
  virtual Value *Codegen(MMPUCompiler *mc);
};
  
class AssignExprAST : public ExprExprAST {
public:
  AssignExprAST(ExprAST *lhs, ExprAST *rhs)
    : ExprExprAST(lhs,
                  new TagExprAST(TagExprAST::EXPR_ASSIGN,
                                 new ExprExprAST(lhs,
                                                 new ExprExprAST(rhs)))) {}
};


/// CallExprAST - Expression class for function calls.
class CallExprAST : public ExprAST {
  std::string Callee;
  std::vector<ExprAST*> Args;
public:
  CallExprAST(const std::string &callee, std::vector<ExprAST*> &args)
  : Callee(callee), Args(args) {}
  virtual Value *Codegen(MMPUCompiler *mc);
};

/// IfExprAST - Expression class for if/then/else.
class IfExprAST : public ExprAST {
  ExprAST *Cond, *Then, *Else;
public:
  IfExprAST(ExprAST *cond, ExprAST *then, ExprAST *_else)
  : Cond(cond), Then(then), Else(_else) {}
  virtual Value *Codegen(MMPUCompiler *mc);
};

/// ForExprAST - Expression class for for/in.
class ForExprAST : public ExprAST {
  std::string VarName;
  ExprAST *Start, *End, *Step, *Body;
public:
  ForExprAST(const std::string &varname, ExprAST *start, ExprAST *end,
             ExprAST *step, ExprAST *body)
  : VarName(varname), Start(start), End(end), Step(step), Body(body) {}
  virtual Value *Codegen(MMPUCompiler *mc);
};

/// VarExprAST - Expression class for var/in
class VarExprAST : public ExprAST {
  std::vector<std::pair<std::string, ExprAST*> > VarNames;
  ExprAST *Body;
public:
  VarExprAST(const std::vector<std::pair<std::string, ExprAST*> > &varnames,
             ExprAST *body)
  : VarNames(varnames), Body(body) {}
  
  virtual Value *Codegen(MMPUCompiler *mc);
};

/// PrototypeAST - This class represents the "prototype" for a function,
/// which captures its argument names as well as if it is an operator.
class PrototypeAST {
  std::string Name;
  std::vector<std::string> Args;
  bool isOperator;
  unsigned Precedence;  // Precedence if a binary op.
public:
  PrototypeAST(const std::string &name, const std::vector<std::string> &args,
               bool isoperator = false, unsigned prec = 0)
  : Name(name), Args(args), isOperator(isoperator), Precedence(prec) {}
  
  bool isUnaryOp() const { return isOperator && Args.size() == 1; }
  bool isBinaryOp() const { return isOperator && Args.size() == 2; }
  
  char getOperatorName() const {
    assert(isUnaryOp() || isBinaryOp());
    return Name[Name.size()-1];
  }
  
  unsigned getBinaryPrecedence() const { return Precedence; }
  
  Function *Codegen(MMPUCompiler *mc);
  
  void CreateArgumentAllocas(Function *F, MMPUCompiler *mc);
};

/// FunctionAST - This class represents a function definition itself.
class FunctionAST {
  PrototypeAST *Proto;
  ExprAST *Body;
public:
  FunctionAST(PrototypeAST *proto, ExprAST *body)
  : Proto(proto), Body(body) {}
  
  Function *Codegen(MMPUCompiler *mc);
};
#endif
}
#endif
