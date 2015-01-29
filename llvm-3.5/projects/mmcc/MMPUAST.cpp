//===-- MMPUAST.cpp - MMPU Abstract Syntax Tree  ------------------------===//
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

#include "MMPUAST.h"
#include "MMPUCompiler.h"
#include <iostream>

using namespace MMPU;

namespace MMPU {
/// BinopPrecedence - This holds the precedence for each binary operator that is
/// defined.
static std::map<char, int> BinopPrecedence;

/// Error* - These are little helper functions for error handling.
//ExprAST *Error(const char *Str) { fprintf(stderr, "Error: %s\n", Str);return 0;}
//PrototypeAST *ErrorP(const char *Str) { Error(Str); return 0; }
//FunctionAST *ErrorF(const char *Str) { Error(Str); return 0; }


//===----------------------------------------------------------------------===//
// Code Generation
//===----------------------------------------------------------------------===//

//static FunctionPassManager *TheFPM;

  //Value *ErrorV(const char *Str) { Error(Str); return 0; }

/// CreateEntryBlockAlloca - Create an alloca instruction in the entry block of
/// the function.  This is used for mutable variables etc.
static AllocaInst *CreateEntryBlockAlloca(Function *TheFunction,
                                          const std::string &VarName) {
  IRBuilder<> TmpB(&TheFunction->getEntryBlock(),
                   TheFunction->getEntryBlock().begin());
  return TmpB.CreateAlloca(Type::getDoubleTy(getGlobalContext()), 0,
                           VarName.c_str());
}

ExprAST *ExprAST::CEval() {
  TagExprAST::TagTy op;
  if (getNext() && isa<TagExprAST>(getNext()))
    op = dyn_cast<TagExprAST>(getNext())->getTag();
  
  switch(op) {
  case TagExprAST::EXPR_CAST:
    return CCast();
  case TagExprAST::EXPR_NEG:
  case TagExprAST::EXPR_INDIR:
    return CUnary();
  case TagExprAST::EXPR_ADD:
  case TagExprAST::EXPR_MUL:
  case TagExprAST::EXPR_SUB:
  case TagExprAST::EXPR_DIV:
  case TagExprAST::EXPR_ASSIGN:
    return CBinary();
  default:
    return Dup();
  }
}
  
ExprAST *ExprAST::CCast() {
  int64_t val;
  ExprAST *ec1, *type;
  if (getNext() && getNext()->getNext() &&
      isa<ExprExprAST>(getNext()->getNext()))
    ec1 = dyn_cast<ExprExprAST>(getNext()->getNext())->getExpr();
  if (isa<ExprExprAST>(this)) {
    type = dyn_cast<ExprExprAST>(this)->getExpr();
    if (type) type = type->Dup();
  }
  ec1 = ec1->CEval();
  
  if (ec1->isConst()) {
    val = ec1->ConstVal();
    if (type && isa<TagExprAST>(type)) {
      switch(dyn_cast<TagExprAST>(type)->getTag()) {
      case TagExprAST::TYPE_CHAR:
        val=(char)val;
        break;
      case TagExprAST::TYPE_UCHAR:
        val=(unsigned char)val;
        break;
      case TagExprAST::TYPE_SHORT:
        val=(short)val;
        break;
      case TagExprAST::TYPE_USHORT:
        val=(unsigned short)val;
        break;
      default:
        break;
      }
    }
    delete ec1;
    return new ExprExprAST(type, new TagExprAST(TagExprAST::EXPR_INT,
                                                new IntExprAST(val, NULL)));
  } else {
    return new ExprExprAST(type, new TagExprAST(TagExprAST::EXPR_CAST,
                                                new ExprExprAST(ec1, NULL)));
  }
}
  
ExprAST *ExprAST::CBinary() {
  TagExprAST::TagTy op;
  int64_t val1, val2, val;
  ExprAST *ec1, *ec2, *type;
    
  if (getNext() && getNext()->getNext() &&
      isa<ExprExprAST>(getNext()->getNext())) {
    ec1 = dyn_cast<ExprExprAST>(getNext()->getNext())->getExpr();
    if (getNext()->getNext()->getNext() &&
        isa<ExprExprAST>(getNext()->getNext()->getNext()))
      ec2 = dyn_cast<ExprExprAST>(getNext()->getNext()->getNext())->getExpr();
    if (isa<TagExprAST>(getNext()))
      op = dyn_cast<TagExprAST>(getNext())->getTag();
    if (isa<ExprExprAST>(this) &&
        (type = dyn_cast<ExprExprAST>(this)->getExpr()))
      type = type->Dup();
  }
  ec1 = ec1->CEval();
  ec2 = ec2->CEval();
    
  if (op != TagExprAST::EXPR_ASSIGN && ec1->isConst() && ec2->isConst() ) {
    val1 = ec1->ConstVal();
    val2 = ec2->ConstVal();
      
    switch(op) {
    case TagExprAST::EXPR_ADD:
      val = val1 + val2;
      break;
    case TagExprAST::EXPR_SUB:
      val = val1 - val2;
      break;
    case TagExprAST::EXPR_MUL:
      if (type && isa<TagExprAST>(type) &&
          dyn_cast<TagExprAST>(type)->getTag() == TagExprAST::TYPE_UINT)
        val = (unsigned int)val1 * (unsigned int)val2;
      else val = val1 * val2;
      break;
    case TagExprAST::EXPR_DIV:
      if (val2 == 0) Error("Divison by 0");
      if (type && isa<TagExprAST>(type) &&
          dyn_cast<TagExprAST>(type)->getTag() == TagExprAST::TYPE_UINT)
        val = (unsigned int)val1 / (unsigned int)val2;
      else val = val1 / val2;
      break;
    /* never reached !*/
    default:
      val = 0;
    }
    delete ec1;
    delete ec2;
    return new ExprExprAST(type, new TagExprAST(TagExprAST::EXPR_INT,
                                                new IntExprAST(val, NULL)));
  } else {
    return new ExprExprAST(type, new TagExprAST(op, new ExprExprAST(ec1,
                                     new ExprExprAST(ec2, NULL))));
  }
}

ExprAST *ExprAST::CUnary() {
  TagExprAST::TagTy op;
  int64_t val1, val;
  ExprAST *ec1, *type;
    
  if (getNext() && getNext()->getNext() &&
      isa<ExprExprAST>(getNext()->getNext())) {
    ec1 = dyn_cast<ExprExprAST>(getNext()->getNext())->getExpr();
    if (isa<TagExprAST>(getNext()))
      op = dyn_cast<TagExprAST>(getNext())->getTag();
    if (isa<ExprExprAST>(this) &&
        (type = dyn_cast<ExprExprAST>(this)->getExpr()))
      type = type->Dup();
  }
  ec1 = ec1->CEval();
    
  if (op != TagExprAST::EXPR_INDIR && ec1->isConst()) {
    val1 = ec1->ConstVal();
    switch (op) {
    case TagExprAST::EXPR_NEG:
      val = -val1;
      break;
    default:
      /* jamais atteint !*/
      val = 0;
    }
    delete ec1;
    return new ExprExprAST(type, new TagExprAST(TagExprAST::EXPR_INT,
                                                new IntExprAST(val, NULL)));
  } else {
    return new ExprExprAST(type, new TagExprAST(op,
                                                new ExprExprAST(ec1, NULL)));
  }
}
  
bool ExprAST::isConst() {
  if (getNext() && isa<TagExprAST>(getNext()))
    return dyn_cast<TagExprAST>(getNext())->getTag() == TagExprAST::EXPR_INT;
  else return false;
}
  
int64_t ExprAST::ConstVal() {
  if (getNext() && getNext()->getNext() &&
      isa<IntExprAST>(getNext()->getNext()))
    return dyn_cast<IntExprAST>(getNext()->getNext())->getVal();
  else return 0;
}

/*
 * Test lvalue
 */
void ExprAST::CheckLValue() {
  TagExprAST::TagTy tag = TagExprAST::INVALIDTAG, t = TagExprAST::INVALIDTAG;
  
  ExprAST *e;
  
  if (isa<TagExprAST>(getNext())) {
    tag = dyn_cast<TagExprAST>(getNext())->getTag();
    if (isa<ExprExprAST>(this) &&
        isa<TagExprAST>(dyn_cast<ExprExprAST>(this)->getExpr()))
      t = dyn_cast<TagExprAST>(dyn_cast<ExprExprAST>(this)->getExpr())->getTag();
  }
  switch (tag) {
    case TagExprAST::EXPR_IDENT:
    case TagExprAST::EXPR_INDIR:
    case TagExprAST::EXPR_INDEX:
    case TagExprAST::EXPR_APDLINC:
      /*if (t == TagExprAST::TYPE_ARRAY)
        Error("An array one shall be a Value");*/
      break;
      
    default:
      Error("Value expected");
  }
}

ExprAST *ExprAST::CreateCast(ExprAST *NewType,ExprAST *E1) {
  if (isa<TagExprAST>(NewType) && isa<ExprExprAST>(E1) &&
      isa<TagExprAST>(dyn_cast<ExprExprAST>(E1)->getExpr())) {
    if (*dyn_cast<TagExprAST>(NewType) ==
        *dyn_cast<TagExprAST>(dyn_cast<ExprExprAST>(E1)->getExpr())) {
      delete NewType;
      return E1;
    } else {
      return new ExprExprAST(NewType, new TagExprAST(TagExprAST::EXPR_CAST,
                                          new ExprExprAST(E1, NULL)));
    }
  }
  return NULL;
}

ExprAST *ExprAST::CreateCastAssign(ExprAST *Type1, ExprAST *E2) {
  TagExprAST *type1, *type2;
  TagExprAST *tp1, *tp2;
  type1 = dyn_cast<TagExprAST>(Type1);
  if (E2 && isa<ExprExprAST>(E2))
    type2 = dyn_cast<TagExprAST>(dyn_cast<ExprExprAST>(E2)->getExpr());
  
  if (type1 && type1->isArith() && type2 && type2->isArith()) {
    /* arithmetic types */
  } else if (type1 && (tp1 = dyn_cast<TagExprAST>(type1->isPointer())) &&
             type2 && (tp2 = dyn_cast<TagExprAST>(type2->isPointer()))) {
    /* 2 pointers */
    if ((tp1 && (tp1->getTag() == TagExprAST::TYPE_VOID)) ||
        (tp2 && (tp2->getTag() == TagExprAST::TYPE_VOID))) {
      /* void pointers */
    } else {
      if ( *tp1 != *tp2 )
        Error("Types of incorrect pointers in the allocation");
    }
  } else if (type1 && type1->isPointer() &&
             E2 && isa<TagExprAST>(E2->getNext()) &&
             (dyn_cast<TagExprAST>(E2->getNext())->getTag() ==
              TagExprAST::EXPR_INT)) {
    if (isa<ExprExprAST>(E2) &&
        isa<TagExprAST>(dyn_cast<ExprExprAST>(E2)->getExpr()) &&
        dyn_cast<TagExprAST>(dyn_cast<ExprExprAST>(E2)->getExpr())->isInteger() &&
        E2->getNext() && isa<IntExprAST>(E2->getNext()->getNext()) &&
        (dyn_cast<IntExprAST>(E2->getNext()->getNext())->getVal() == 0)) {
      /* assignment of 0 in a pointer */
    } else {
      Error("Only the constant 0 can be assigned to a pointer");
    }
  } else {
    if (*type1 != *type2)
      Error("Incompatible types for assignment");
    /* we should test here if structs & unions can be used
     * for an assignment, i.e. they are defined
     */
    return E2;
  }
  return CreateCast(type1->Dup(), E2);
}
  
ExprAST *ExprAST::CreatePromoteInteger(ExprAST *E) {
  TagExprAST::TagTy tag;
  if (isa<ExprExprAST>(E) &&
      isa<TagExprAST>(dyn_cast<ExprExprAST>(E)->getExpr()))
    tag = dyn_cast<TagExprAST>(dyn_cast<ExprExprAST>(E)->getExpr())->getTag();
  switch (tag) {
  case TagExprAST::TYPE_CHAR:
  case TagExprAST::TYPE_UCHAR:
  case TagExprAST::TYPE_SHORT:
  case TagExprAST::TYPE_USHORT:
  case TagExprAST::TYPE_ENUM:
    return CreateCast(new TagExprAST(TagExprAST::TYPE_INT, NULL), E);
  default:
    return E;
  }
}

void ExprAST::BinaryCast(ExprAST* &Res1, ExprAST* &Res2,
                         ExprAST *E1, ExprAST *E2) {
  TagExprAST *t1, *t2, *t;
  E1 = CreatePromoteInteger(E1);
  E2 = CreatePromoteInteger(E2);
    
  t1 = isa<ExprExprAST>(E1) ? dyn_cast<TagExprAST>(
                                dyn_cast<ExprExprAST>(E1)->getExpr()) : NULL;
  t2 = isa<ExprExprAST>(E2) ? dyn_cast<TagExprAST>(
                                dyn_cast<ExprExprAST>(E2)->getExpr()) : NULL;
    
  if ((t1 && (t1->getTag() == TagExprAST::TYPE_UINT)) ||
      (t2 && (t2->getTag() == TagExprAST::TYPE_UINT))) {
    t = new TagExprAST(TagExprAST::TYPE_UINT, NULL);
  } else {
    t = new TagExprAST(TagExprAST::TYPE_INT, NULL);
  }
    
  Res1 = CreateCast(t->Dup(), E1);
  Res2 = CreateCast(t->Dup(), E2);
  delete t;
}

ExprAST *ExprAST::CreateBinaryInteger(const unsigned Op,
                                      ExprAST *E1, ExprAST *E2) {
  ExprAST *res1, *res2;
  TagExprAST *t1, *t2;
  t1 = isa<ExprExprAST>(E1) ? dyn_cast<TagExprAST>(
                                dyn_cast<ExprExprAST>(E2)->getExpr()) : NULL;
  t2 = isa<ExprExprAST>(E2) ? dyn_cast<TagExprAST>(
                                dyn_cast<ExprExprAST>(E2)->getExpr()) : NULL;
  if (!t1 || !t1->isInteger() || !t2 || !t2->isInteger())
    Error("Integer operands expected");
    
  BinaryCast(res1, res2, E1, E2);
  
  return new ExprExprAST(dyn_cast<ExprExprAST>(res1)->getExpr()->Dup(),
                         new TagExprAST(Op,
                             new ExprExprAST(res1,
                                 new ExprExprAST(res2, NULL))));
}

ExprAST *ExprAST::CreateBinaryArith(const unsigned Op,
                                    ExprAST *E1, ExprAST *E2) {
  ExprAST *res1, *res2;
  TagExprAST *t1, *t2;
  
  t1 = isa<ExprExprAST>(E1) ? dyn_cast<TagExprAST>(
                                dyn_cast<ExprExprAST>(E2)->getExpr()) : NULL;
  t2 = isa<ExprExprAST>(E2) ? dyn_cast<TagExprAST>(
                                dyn_cast<ExprExprAST>(E2)->getExpr()) : NULL;
  
  if (!t1 || !t1->isArith() || !t2 || !t2->isArith())
    Error("Operands of arithmetic type expected");
    
  BinaryCast(res1, res2, E1, E2);
    
  return new ExprExprAST(dyn_cast<ExprExprAST>(res1)->getExpr()->Dup(),
                         new TagExprAST(Op,
                             new ExprExprAST(res1,
                                 new ExprExprAST(res2, NULL))));
}

ExprAST *ExprAST::CreateConstInt(const uint64_t C) {
  return new ExprExprAST(new TagExprAST(TagExprAST::TYPE_INT, NULL),
                         new TagExprAST(TagExprAST::EXPR_INT,
                                        new IntExprAST(C, NULL)));
}

ExprAST *ExprAST::CreateList(ExprAST *ExprList) {
  ExprAST *expr;
  if (ExprList && (ExprList->getNext() == NULL)) {
    expr = isa<ExprExprAST>(ExprList) ?
           dyn_cast<ExprExprAST>(ExprList)->getExpr() : NULL;
    dyn_cast<ExprExprAST>(ExprList)->setExpr(NULL);
    delete ExprList;
    return expr;
  } else {
    expr = ExprList;
    while (expr && expr->getNext()) expr = expr->getNext();
    return new ExprExprAST(dyn_cast<ExprExprAST>(expr)->getExpr()->Dup(),
                           new TagExprAST(TagExprAST::EXPR_LIST, ExprList));
  }
}

ExprAST *ExprAST::CreateAddSub(unsigned Op, ExprAST *E1, ExprAST *E2) {
  TagExprAST *type1, *type2, *tp1, *tp2;
  ExprAST *expr, *t;
  int64_t size;
    
  type1 = isa<ExprExprAST>(E1) ? dyn_cast<TagExprAST>(
                                   dyn_cast<ExprExprAST>(E1)->getExpr()) : NULL;
  type2 = isa<ExprExprAST>(E2) ? dyn_cast<TagExprAST>(
                                   dyn_cast<ExprExprAST>(E2)->getExpr()) : NULL;
    
  tp1 = dyn_cast<TagExprAST>(type1->isPointer());
  tp2 = dyn_cast<TagExprAST>(type2->isPointer());
    
  if (tp1 || tp2) {
    if (tp1 && tp2) {
      /* 2 pointers */
      if (Op == TagExprAST::EXPR_ADD)
        Error("Addition of two pointers prohibited");
      if (*tp1 != *tp2)
        Error("Subtraction of pointers to objects of different types");
      size = tp1->TypeSize();
      if (size == -1) Error("Element size unknown");
      expr = new ExprExprAST(type1->Dup(),
                             new TagExprAST(Op,
                                 new ExprExprAST(E1,
                                     new ExprExprAST(E2, NULL))));
      expr = CreateCast(new TagExprAST(TagExprAST::TYPE_INT, NULL), expr);
      if (size != 1) {
        expr = CreateBinary(TagExprAST::EXPR_DIV, expr, CreateConstInt(size));
      }
      return expr;
    } else {
      /* pointer + integer */
      if (tp2) {
        t = E1; E1 = E2; E2 = t;
        t = tp1; tp1 = tp2; tp2 = dyn_cast<TagExprAST>(t);
        t = type1; type1 = type2; type2 = dyn_cast<TagExprAST>(t);
      }
      if (!type2->isInteger())
        Error("Addition of a pointer with a non-integer");
        /* it was reduced to the case: pointer + num */
      size = tp1->TypeSize();
      if (size == -1) Error("Element size unknown");
      if (size != 1) {
        E2 = CreateBinary(TagExprAST::EXPR_MUL, E2, CreateConstInt(size));
      }
      E2 = CreateCast(new TagExprAST(TagExprAST::TYPE_POINTER,
                                     new TagExprAST(TagExprAST::TYPE_VOID,
                                                    NULL)), E2);
      return new ExprExprAST(type1->Dup(),
                 new TagExprAST(Op,
                     new ExprExprAST(E1,
                         new ExprExprAST(E2, NULL))));
    }
  } else {
    return CreateBinaryArith(Op, E1, E2);
  }
}

ExprAST *ExprAST::CreateBinaryShift(unsigned Op, ExprAST *E1, ExprAST *E2) {
  TagExprAST *type1, *type2;
  TagExprAST *tp1, *tp2;
  ExprAST *expr;
  
  type1 = isa<ExprExprAST>(E1) ? dyn_cast<TagExprAST>(
                                   dyn_cast<ExprExprAST>(E1)->getExpr()) : NULL;
  type2 = isa<ExprExprAST>(E2) ? dyn_cast<TagExprAST>(
                                   dyn_cast<ExprExprAST>(E2)->getExpr()) : NULL;

  if (!type1 || !type1->isInteger() || !type2 || !type2->isInteger())
    Error("Integer operands expected");
    
  E1 = CreatePromoteInteger(E1);
  expr = isa<ExprExprAST>(E1) ? dyn_cast<ExprExprAST>(E1)->getExpr() : NULL;
  E2 = CreateCast(new TagExprAST(TagExprAST::TYPE_INT, NULL),
                  CreatePromoteInteger(E2));
    
  return new ExprExprAST(type1->Dup(),
             new TagExprAST(Op, new ExprExprAST(E1,
                                    new ExprExprAST(E2, NULL))));
}

ExprAST *ExprAST::CreateBinaryCompare(unsigned Op, ExprAST *E1, ExprAST *E2) {
  ExprAST *res1, *res2, *t;
  TagExprAST *tp1, *tp2, *t1, *t2;
    
  t1 = isa<ExprExprAST>(E1) ? dyn_cast<TagExprAST>(
                                dyn_cast<ExprExprAST>(E1)->getExpr()) : NULL;
  t2 = isa<ExprExprAST>(E2) ? dyn_cast<TagExprAST>(
                                dyn_cast<ExprExprAST>(E2)->getExpr()) : NULL;
  
  tp1 = t1 ? dyn_cast<TagExprAST>(t1->isPointer()) : NULL;
  tp2 = t2 ? dyn_cast<TagExprAST>(t2->isPointer()) : NULL;
    
  if (tp1 && tp2) {
			/* comparison of pointers: one extends slightly ansi */
    if ((tp1->getTag() == TagExprAST::TYPE_VOID) ||
        (tp2->getTag() == TagExprAST::TYPE_VOID)) {
      /* void pointers: No tests */
    } else if (*tp1 != *tp2) {
      /* normal pointer */
      Error("Comparison of pointers to objects of different types");
    }
    res1 = E1;
    res2 = E2;
  } else if ( tp1 || tp2 ) {
    /* comparison with a pointer 0 */
    if (tp2) {
      t = tp1; tp1 = tp2; tp2 = dyn_cast<TagExprAST>(t);
      t = E1; E1 = E2; E2 = t;
      t = t1; t1 = t2; t2 = dyn_cast<TagExprAST>(t);
    }
    if (isa<TagExprAST>(E2->getNext()) &&
        (dyn_cast<TagExprAST>(E2->getNext())->getTag() == TagExprAST::EXPR_INT) &&
        isa<ExprExprAST>(E2) &&
        isa<TagExprAST>(dyn_cast<ExprExprAST>(E2)->getExpr()) &&
        dyn_cast<TagExprAST>(dyn_cast<ExprExprAST>(E2)->getExpr())->isInteger() &&
        isa<IntExprAST>(E2->getNext()->getNext()) &&
        (dyn_cast<IntExprAST>(E2->getNext()->getNext()) == 0)) {
      E2 = CreateCast(new TagExprAST(TagExprAST::TYPE_POINTER,
                          new TagExprAST(TagExprAST::TYPE_VOID, NULL)), E2);
      res1 = E1;
      res2 = E2;
    } else {
      Error("A pointer one shall be compared with a null integer");
    }
  } else if (t1->isArith() && t2->isArith()) {
    /* comparing numbers */
    BinaryCast(res1, res2, E1, E2);
  } else {
    Error("Comparing objects of incompatible types");
  }
  return new ExprExprAST(new TagExprAST(TagExprAST::TYPE_INT, NULL),
                         new TagExprAST(Op,
                             new ExprExprAST(res1,
                                 new ExprExprAST(res2, NULL))));
}

ExprAST *ExprAST::CreateLogicalInt(ExprAST *Expr) {
  TagExprAST *type;
    
  Expr = CreatePromoteInteger(Expr);
  type = isa<ExprExprAST>(Expr) ? dyn_cast<TagExprAST>(
                                    dyn_cast<ExprExprAST>(Expr)->getExpr()) : NULL;
  if (!type || (!type->isInteger() && !type->isPointer()))
    Error("Arithmetic or pointer type expected in the logical expression");
  return Expr;
}

ExprAST *ExprAST::CreateBinaryLogical(unsigned Op,
                                      ExprAST *E1, ExprAST *E2) {
  E1 = CreateLogicalInt(E1);
  E2 = CreateLogicalInt(E2);
  return new ExprExprAST(new TagExprAST(TagExprAST::TYPE_INT, NULL),
                         new TagExprAST(Op,
                             new ExprExprAST(E1, new ExprExprAST(E2, NULL))));
}

ExprAST *ExprAST::CreateBinary(unsigned Op, ExprAST *E1, ExprAST *E2) {
  switch (TagExprAST::TagTy(Op)) {
  case TagExprAST::EXPR_ADD:
  case TagExprAST::EXPR_SUB:
  case TagExprAST::EXPR_SADD:
  case TagExprAST::EXPR_SSUB:
    return CreateAddSub(Op, E1, E2);
        
  case TagExprAST::EXPR_MUL:
  case TagExprAST::EXPR_DIV:
    return CreateBinaryArith(Op, E1, E2);
        
  case TagExprAST::EXPR_MOD:
  case TagExprAST::EXPR_AND:
  case TagExprAST::EXPR_OR:
  case TagExprAST::EXPR_XOR:
    return CreateBinaryInteger(Op, E1, E2);
        
  case TagExprAST::EXPR_SHR:
  case TagExprAST::EXPR_SHL:
    return CreateBinaryShift(Op, E1, E2);
        
  case TagExprAST::EXPR_LT:
  case TagExprAST::EXPR_LE:
  case TagExprAST::EXPR_GT:
  case TagExprAST::EXPR_GE:
  case TagExprAST::EXPR_EQ:
  case TagExprAST::EXPR_NE:
    return CreateBinaryCompare(Op, E1, E2);
        
  case TagExprAST::EXPR_LOR:
  case TagExprAST::EXPR_LAND:
    return CreateBinaryLogical(Op, E1, E2);
        
  default:
    Error("Typing the operation %d not implemented", Op);
    return NULL;
  }
}

ExprAST *ExprAST::CreateUnary(unsigned Op, ExprAST *E1) {
  TagExprAST *type;
    
  type = isa<ExprExprAST>(E1) ? dyn_cast<TagExprAST>(
                                  dyn_cast<ExprExprAST>(E1)->getExpr()) : NULL;
  switch (TagExprAST::TagTy(Op)) {
  case TagExprAST::EXPR_NEG:
  case TagExprAST::EXPR_PLUS:
    if (!type || !type->isInteger())
      Error("Expected arithmetic type for '+' or '-'");
    E1 = CreatePromoteInteger(E1);
    if (TagExprAST::TagTy(Op) == TagExprAST::EXPR_NEG && E1 &&
        isa<ExprExprAST>(E1)) {
      return new ExprExprAST(dyn_cast<ExprExprAST>(E1)->getExpr()->Dup(),
                             new TagExprAST(Op, new ExprExprAST(E1, NULL)));
    } else {
      return E1;
    }
        
  case TagExprAST::EXPR_NOT:
    if (!type || !type->isInteger()) Error("Integer expected for '~'");
      E1 = CreatePromoteInteger(E1);
    if (E1 && isa<ExprExprAST>(E1))
      return new ExprExprAST(dyn_cast<ExprExprAST>(E1)->getExpr()->Dup(),
                             new TagExprAST(Op, new ExprExprAST(E1, NULL)));
        
  case TagExprAST::EXPR_LNOT:
    return CreateBinary(TagExprAST::EXPR_EQ, E1, CreateConstInt(0));
        
  default:
    Error("Type the unary operator %d implements", Op);
    return NULL;
  }
}
  
ExprAST *ExprAST::CreateIndir(ExprAST *E1) {
  TagExprAST *type1;
  ExprAST *tp;
    
  type1 = isa<ExprExprAST>(E1) ? dyn_cast<TagExprAST>(
                                   dyn_cast<ExprExprAST>(E1)->getExpr()) : NULL;
  tp = type1 ? type1->isPointer() : NULL;
  if (!tp) Error("'*' Can only be used a pointer or array");
  return new ExprExprAST(tp->Dup(), new TagExprAST(TagExprAST::EXPR_INDIR,
                                                   new ExprExprAST(E1, NULL)));
}

ExprAST *ExprAST::CreateIndex(ExprAST *E1, ExprAST *E2) {
  TagExprAST *type1, *type2;
  ExprAST *tp;

  type1 = isa<ExprExprAST>(E1) ? dyn_cast<TagExprAST>(
                                   dyn_cast<ExprExprAST>(E1)->getExpr()) : NULL;
  
  type2 = isa<ExprExprAST>(E2) ? dyn_cast<TagExprAST>(
                                   dyn_cast<ExprExprAST>(E2)->getExpr()) : NULL;
  tp = type1 ? type1->isPointer() : NULL;
  if (!tp) Error("'[]' Can only be used by an array or a pointer");
  if (!type2 || !type2->isInteger())
    Error("An array must be indexed by an integer");
  
  if (E1 && E1->getNext() && isa<TagExprAST>(E1->getNext()) &&
      dyn_cast<TagExprAST>(E1->getNext())->getTag() == TagExprAST::EXPR_INDEX) {
    if (isa<ExprExprAST>(E1)) {
      //delete dyn_cast<ExprExprAST>(E1)->getExpr();
      dyn_cast<ExprExprAST>(E1)->setExpr(tp);
    }
    return append(E1, new TagExprAST(TagExprAST::EXPR_INDEX,
                                     new ExprExprAST(E2, NULL)));
  }
  else
    return new ExprExprAST(tp->Dup(),
               new TagExprAST(TagExprAST::EXPR_INDEX,
                   new ExprExprAST(E1->getNext(),
                       new ExprExprAST(E2, NULL))));
  //return CreateIndir(CreateBinary(TagExprAST::EXPR_ADD, E1, E2));
}

ExprAST *ExprAST::CreateAddr(ExprAST *E){
  TagExprAST *t;
  E->CheckLValue();
  ExprExprAST *ee = isa<ExprExprAST>(E) ? dyn_cast<ExprExprAST>(E) : NULL;
  if (ee && ee->getExpr())
    t = new TagExprAST(TagExprAST::TYPE_POINTER, ee->getExpr()->Dup());
  return new ExprExprAST(t, new TagExprAST(TagExprAST::EXPR_ADDR,
                                           new ExprExprAST(E, NULL)));
}

ExprAST *ExprAST::CreateField(ExprAST *E1, const std::string &Name,
                              MMPUCompiler *mc) {
  TagExprAST *type1, *type2, *type3;
  TagExprAST::TagTy t1;
  Symbol *s;
  Symbol::SymbolTy sym_table;
  ExprExprAST *ee;
  int64_t offset;
    
  type1 = isa<ExprExprAST>(E1) ? dyn_cast<TagExprAST>(
                                   dyn_cast<ExprExprAST>(E1)->getExpr()) : NULL;
  t1 = type1 ? type1->getTag() : TagExprAST::INVALIDTAG;
  
  if (t1 != TagExprAST::TYPE_STRUCT && t1 != TagExprAST::TYPE_UNION)
    Error("Type 'struct' or 'union' expected");
    
  if (type1 && isa<SymExprAST>(type1->getNext()))
    s = dyn_cast<SymExprAST>(type1->getNext())->getSymbol();
  if (!s->getExpr() || !isa<IntExprAST>(s->getExpr()->getNext()) ||
      dyn_cast<IntExprAST>(s->getExpr()->getNext())->getVal() == -1)
    Error("'struct' or 'union' undefined");
  else
    sym_table = Symbol::SymbolTy(dyn_cast<IntExprAST>(s->getExpr()
                                                      ->getNext())->getVal());
    
  s = mc->LookupSym(Name, sym_table);
  if (s == NULL) Error("Field of 'struct' or 'union' undefined");
  offset = dyn_cast<IntExprAST>(s->getExpr()->getNext())->getVal();;
  ee = dyn_cast<ExprExprAST>(s->getExpr());
  type3 = ee ? new TagExprAST(TagExprAST::TYPE_POINTER, ee->getExpr()) : NULL;
  type2 = new TagExprAST(TagExprAST::TYPE_POINTER,
                         new TagExprAST(TagExprAST::TYPE_CHAR, NULL));
    
  return CreateIndir(CreateCast(type3,
                                CreateBinary(TagExprAST::EXPR_ADD,
                                             CreateCast(type2, E1),
                                             CreateConstInt(offset))));
}

ExprAST *ExprAST::CreateAssign(ExprAST *E1, ExprAST *E2) {
  E1->CheckLValue();
  if (isa<ExprExprAST>(E1))
    E2 = CreateCastAssign(dyn_cast<ExprExprAST>(E1)->getExpr(), E2);
  return new ExprExprAST(dyn_cast<ExprExprAST>(E1)->getExpr()->Dup(),
             new TagExprAST(TagExprAST::EXPR_ASSIGN,
                 new ExprExprAST(E1, new ExprExprAST(E2, NULL))));
}

ExprAST *ExprAST::CreateAssignOp(unsigned Op, ExprAST *E1, ExprAST *E2,
                                 MMPUCompiler *mc) {
  TagExprAST::TagTy tag1 = TagExprAST::INVALIDTAG;
  ExprAST *expr_list, *expr;
  ExprExprAST *e3;
  std::string var_name;
  
  if (E1 && isa<TagExprAST>(E1->getNext()))
    tag1 = dyn_cast<TagExprAST>(E1->getNext())->getTag();
  expr_list = NULL;
    
  if (tag1 != TagExprAST::EXPR_IDENT) {
    var_name = (mc->CreateSymbol(NULL, Symbol::TABLE_VAR, NULL, NULL))->getName();
    e3 = dyn_cast<ExprExprAST>(CreateAddr(E1));
    mc->Local_Declare(var_name, TagExprAST::STORAGE_DEFAULT,
										  e3->getExpr()->Dup(),
                      new TagExprAST(TagExprAST::INIT_EXPR, e3));
    expr = CreateAssign(CreateIndir(CreateIdent(var_name, mc)),
											  CreateBinary(Op,
                                     CreateIndir(CreateIdent(var_name, mc)),
                                     E2));
  } else {
    if (E1 && E1->getNext() && isa<SymExprAST>(E1->getNext()->getNext()))
      var_name = dyn_cast<SymExprAST>(E1->getNext()->getNext())->getSymbol()
                                                                 ->getName();
    expr = CreateAssign(CreateIdent(var_name, mc),
                        CreateBinary(Op, CreateIdent(var_name, mc), E2));
  }
  delete E1;
  return expr;
}

ExprAST *ExprAST::CreateCond(ExprAST *E, ExprAST *E1, ExprAST *E2) {
  ExprAST *res1, *res2;
  TagExprAST *t1, *t2;
  ExprAST *ee;
    
  E = CreateLogicalInt(E);
  t1 = isa<ExprExprAST>(E1) ? dyn_cast<TagExprAST>(
                                dyn_cast<ExprExprAST>(E1)->getExpr()) : NULL;
  t2 = isa<ExprExprAST>(E2) ? dyn_cast<TagExprAST>(
                                dyn_cast<ExprExprAST>(E2)->getExpr()) : NULL;
    
  if (t1->isArith() && t2->isArith()) {
    BinaryCast(res1, res2, E1, E2);
  } else if (*t1 != *t2) {
    Error("Types incompatibles pour l'opÈrateur conditionnel");
  } else {
    res1 = E1;
    res2 = E2;
  }
  ee = isa<ExprExprAST>(res1) ? dyn_cast<ExprExprAST>(res1)->getExpr() : NULL;
  if (!ee)
    return new ExprExprAST(ee->Dup(),
               new TagExprAST(TagExprAST::EXPR_COND,
                   new ExprExprAST(E,
                       new ExprExprAST(res1,
                           new ExprExprAST(res2, NULL)))));
  else return NULL;
}

ExprAST *ExprAST::CreateCall(ExprAST *E1, ExprAST *Params1){
  TagExprAST *type1, *type2, *param_type;
  ExprAST *func_params, *params, *func_rettype, *func_param_type;
  ExprAST *param_expr, *expr, *param_list;
  TagExprAST::TagTy t1, t2, func_type;
  int64_t params_nb;
    
  type1 = isa<ExprExprAST>(E1) ? dyn_cast<TagExprAST>(
                                   dyn_cast<ExprExprAST>(E1)->getExpr()) : NULL;
  type2 = E1 ? dyn_cast<TagExprAST>(E1->getNext()) : NULL;
  t1 = type1 ? type1->getTag() : TagExprAST::INVALIDTAG;
  t2 = type2 ? type2->getTag() : TagExprAST::INVALIDTAG;
  /* on accepte une fonction ou un pointeur sur une fonction */
    
  if (!(t1 == TagExprAST::TYPE_FUNC ||
       (t1 == TagExprAST::TYPE_POINTER &&
        t2 == TagExprAST::TYPE_FUNC)))
    Error("L'expression ‡ gauche de '(...)' n'est pas une fonction");
    
  if (type1 && (t1 == TagExprAST::TYPE_POINTER))
    type1 = dyn_cast<TagExprAST>(type1->getNext());
    
  /* on compare le type des paramËtres avec ceux de la fonction */
  if (type1 && isa<TagExprAST>(type1->getNext())) {
    func_type = dyn_cast<TagExprAST>(type1->getNext())->getTag();
    if (isa<ExprExprAST>(type1->getNext()->getNext())) {
      func_params = dyn_cast<ExprExprAST>(type1->getNext()->getNext())
                                            ->getExpr();
      func_rettype = type1->getNext()->getNext()->getNext() ?
                     type1->getNext()->getNext()->getNext()->Dup() : NULL;
    }
  }
  
  param_list = NULL;
  params = Params1;
  params_nb = 0;
    
  while (params != NULL) {
    if (isa<ExprExprAST>(params) && dyn_cast<ExprExprAST>(params)->getExpr())
			param_expr = dyn_cast<ExprExprAST>(params)->getExpr()->Dup();
    params = params->getNext();
    params_nb++;
			
    if (func_type == TagExprAST::FUNC_NEW && func_params == NULL)
      Error("Trop de paramËtres");
			
    if ((func_type == TagExprAST::FUNC_ELLIPSIS && func_params == NULL) ||
        (func_type == TagExprAST::FUNC_OLD)) {
      /* appel selon l'ancienne norme */
      param_type = isa<ExprExprAST>(params) ?
                   dyn_cast<TagExprAST>(
                     dyn_cast<ExprExprAST>(param_expr)->getExpr()) : NULL;
      if (param_type && param_type->isInteger()) {
        expr = CreatePromoteInteger(param_expr);
      } else if (param_type &&
                 (param_type->getTag() == TagExprAST::TYPE_ARRAY) &&
                 param_type->getNext()) {
        func_param_type = new TagExprAST(TagExprAST::TYPE_POINTER,
                              param_type->Dup()->getNext()->getNext());
        expr = CreateCastAssign(func_param_type, param_expr);
        delete func_param_type;
      } else {
        expr = param_expr;
      }
    } else {
      /* nouvelle norme */
      if (func_params && func_params->getNext() &&
          isa<ExprExprAST>(func_params->getNext()->getNext())) {
        func_param_type = dyn_cast<ExprExprAST>(func_params->getNext()
                                                  ->getNext())->getExpr();
        if (func_params->getNext()->getNext()->getNext())
          func_params = func_params->getNext()->getNext()->getNext()->getNext();
      }
      expr = CreateCastAssign(func_param_type, param_expr);
    }
    param_list = append(param_list, new ExprExprAST(expr, NULL));
  }
  if (func_type != TagExprAST::FUNC_OLD && func_params != NULL)
    Error("Too few parameters");
    
  delete Params1;
    
  return new ExprExprAST(func_rettype,
             new TagExprAST(TagExprAST::EXPR_CALL,
                 new ExprExprAST(E1,
                     new IntExprAST(params_nb, param_list))));
}

ExprAST *ExprAST::CreateIdent(const std::string &Name, MMPUCompiler *mc) {
  Symbol *s;
  TagExprAST *var;
  ExprAST *type = NULL;
  TagExprAST::TagTy tag;
    
  s = mc->LookupSym(Name, Symbol::TABLE_VAR);
  if (s == NULL) Error("Variable '%s' UNDECLARED", Name.c_str());
    
  var = dyn_cast_or_null<TagExprAST>(s->getExpr());
  tag = var ? var->getTag() : TagExprAST::INVALIDTAG;
  switch (tag) {
  case TagExprAST::SYM_VAR:
    if (var && var->getNext() && isa<ExprExprAST>(var->getNext()->getNext()))
      type = dyn_cast<ExprExprAST>(var->getNext()->getNext())->getExpr();
    type = type ? type->Dup() : type;
    return new ExprExprAST(type, new TagExprAST(TagExprAST::EXPR_IDENT,
                                                new SymExprAST(s, NULL)));
  case TagExprAST::SYM_ENUM_CONST:
    if (s->getExpr() && isa<IntExprAST>(s->getExpr()->getNext()))
      return CreateConstInt(dyn_cast<IntExprAST>(s->getExpr()->getNext())
                                                    ->getVal());
    else return NULL;
  default:
    Error("'%s': expected variable", Name.c_str());
    return NULL;
  }
}

ExprAST *ExprAST::CreateSizeof(ExprAST *E, bool isExpr) {
  TagExprAST *type;
  int64_t size;
    
  if (isExpr) {
    type = isa<ExprExprAST>(E) ? dyn_cast<TagExprAST>(
                                   dyn_cast<ExprExprAST>(E)->getExpr()) : NULL;
  } else {
    type = dyn_cast<TagExprAST>(E);
  }
    
  size = type->TypeSize();
  if (size == -1) Error("sizeof: incomplete type");
    
  delete E;
    
  return CreateConstInt(size);
}
  
int64_t TagExprAST::TypeSize() {
  int64_t dim, size;
  Symbol *s;
  switch(Val) {
  case TYPE_VOID:
    return -1;
  case TYPE_CHAR:
  case TYPE_UCHAR:
    return VM_CHAR_SIZE;
  case TYPE_SHORT:
  case TYPE_USHORT:
    return VM_SHORT_SIZE;
  case TYPE_INT:
  case TYPE_UINT:
    return VM_INT_SIZE;
  case TYPE_POINTER:
    return VM_POINTER_SIZE;
  case TYPE_ARRAY:
    if (isa<IntExprAST>(getNext())) {
      dim = dyn_cast<IntExprAST>(getNext())->getVal();
      if (dim == -1) return -1;
      if (isa<TagExprAST>(getNext()->getNext())) {
        size = dyn_cast<TagExprAST>(getNext()->getNext())->TypeSize();
        if (size == -1) return -1;
        else return dim * size;
      }
    }
    return -1;
  case TYPE_ENUM:
    return VM_INT_SIZE;
  case TYPE_UNION:
  case TYPE_STRUCT:
    if (isa<SymExprAST>(getNext())) {
      s = dyn_cast<SymExprAST>(getNext())->getSymbol();
      if (s->getExpr() && isa<IntExprAST>(s->getExpr()->getNext())) {
        if (dyn_cast<IntExprAST>(s->getExpr()->getNext())->getVal() != -1 &&
            isa<IntExprAST>(s->getExpr()->getNext()->getNext()))
          return dyn_cast<IntExprAST>(s->getExpr()->getNext()
                                      ->getNext())->getVal();
      }
    }
    return -1;
          
  case TYPE_FUNC:
    return -1;
  default:
    Error("Size of the type %d not defined", Val);
    return -1;
  }
}
  
bool TagExprAST::operator==(TagExprAST &RHS) const {
  TagExprAST *lht, *rht;
  SymExprAST *lhs, *rhs;
  if (getTag() != RHS.getTag()) {
    return false;
  } else {
    switch (getTag()) {
      case TYPE_POINTER:
        if ((lht = dyn_cast<TagExprAST>(getNext())) &&
            (rht = dyn_cast<TagExprAST>(RHS.getNext())))
        return *lht == *rht;
      case TYPE_ARRAY:
      case TYPE_FUNC:
        /* non fait */
        return true;
      case TYPE_ENUM:
      case TYPE_UNION:
      case TYPE_STRUCT:
        if ((lhs = dyn_cast<SymExprAST>(getNext())) &&
            (rhs = dyn_cast<SymExprAST>(RHS.getNext())))
          return lhs->getSymbol() == rhs->getSymbol();
      default:
			return true;
    }
  }
}
#if 0
Value *IntExprAST::Codegen(MMPUCompiler *mc) {
  return ConstantInt::get(getGlobalContext(), APInt(64, Val));
}

Value *VariableExprAST::Codegen(MMPUCompiler *mc) {
  // Look this variable up in the function.
  Value *V = mc->NamedValues[Name];
  if (V == 0) return ErrorV("Unknown variable name");
  
  // Load the value.
  return mc->Builder.CreateLoad(V, Name.c_str());
}

Value *UnaryExprAST::Codegen(MMPUCompiler *mc) {
  Value *OperandV = Operand->Codegen(mc);
  if (OperandV == 0) return 0;
  
  Function *F = mc->Mod->getFunction(std::string("unary")+Opcode);
  if (F == 0)
    return ErrorV("Unknown unary operator");
  
  return mc->Builder.CreateCall(F, OperandV, "unop");
}

Value *BinaryExprAST::Codegen(MMPUCompiler *mc) {
  // Special case '=' because we don't want to emit the LHS as an expression.
  if (Op == '=') {
    // Assignment requires the LHS to be an identifier.
    VariableExprAST *LHSE = dynamic_cast<VariableExprAST*>(LHS);
    if (!LHSE)
      return ErrorV("destination of '=' must be a variable");
    // Codegen the RHS.
    Value *Val = RHS->Codegen(mc);
    if (Val == 0) return 0;
    
    // Look up the name.
    Value *Variable = mc->NamedValues[LHSE->getName()];
    if (Variable == 0) return ErrorV("Unknown variable name");
    
    mc->Builder.CreateStore(Val, Variable);
    return Val;
  }
  
  Value *L = LHS->Codegen(mc);
  Value *R = RHS->Codegen(mc);
  if (L == 0 || R == 0) return 0;
  
  switch (Op) {
    case '+': return mc->Builder.CreateFAdd(L, R, "addtmp");
    case '-': return mc->Builder.CreateFSub(L, R, "subtmp");
    case '*': return mc->Builder.CreateFMul(L, R, "multmp");
    case '<':
      L = mc->Builder.CreateFCmpULT(L, R, "cmptmp");
      // Convert bool 0/1 to double 0.0 or 1.0
      return mc->Builder.CreateUIToFP(L, Type::getDoubleTy(getGlobalContext()),
                                  "booltmp");
    default: break;
  }
  
  // If it wasn't a builtin binary operator, it must be a user defined one. Emit
  // a call to it.
  Function *F = mc->Mod->getFunction(std::string("binary")+Op);
  assert(F && "binary operator not found!");
  
  Value *Ops[] = { L, R };
  return mc->Builder.CreateCall(F, Ops, "binop");
}

Value *CallExprAST::Codegen(MMPUCompiler *mc) {
  // Look up the name in the global module table.
  Function *CalleeF = mc->Mod->getFunction(Callee);
  if (CalleeF == 0)
    return ErrorV("Unknown function referenced");
  
  // If argument mismatch error.
  if (CalleeF->arg_size() != Args.size())
    return ErrorV("Incorrect # arguments passed");
  
  std::vector<Value*> ArgsV;
  for (unsigned i = 0, e = Args.size(); i != e; ++i) {
    ArgsV.push_back(Args[i]->Codegen(mc));
    if (ArgsV.back() == 0) return 0;
  }
  
  return mc->Builder.CreateCall(CalleeF, ArgsV, "calltmp");
}

Value *IfExprAST::Codegen(MMPUCompiler *mc) {
  Value *CondV = Cond->Codegen(mc);
  if (CondV == 0) return 0;
  
  // Convert condition to a bool by comparing equal to 0.0.
  CondV = mc->Builder.CreateFCmpONE(CondV,
                                ConstantFP::get(getGlobalContext(), APFloat(0.0)),
                                "ifcond");
  
  Function *TheFunction = mc->Builder.GetInsertBlock()->getParent();
  
  // Create blocks for the then and else cases.  Insert the 'then' block at the
  // end of the function.
  BasicBlock *ThenBB = BasicBlock::Create(getGlobalContext(), "then", TheFunction);
  BasicBlock *ElseBB = BasicBlock::Create(getGlobalContext(), "else");
  BasicBlock *MergeBB = BasicBlock::Create(getGlobalContext(), "ifcont");
  
  mc->Builder.CreateCondBr(CondV, ThenBB, ElseBB);
  
  // Emit then value.
  mc->Builder.SetInsertPoint(ThenBB);
  
  Value *ThenV = Then->Codegen(mc);
  if (ThenV == 0) return 0;
  
  mc->Builder.CreateBr(MergeBB);
  // Codegen of 'Then' can change the current block, update ThenBB for the PHI.
  ThenBB = mc->Builder.GetInsertBlock();
  
  // Emit else block.
  TheFunction->getBasicBlockList().push_back(ElseBB);
  mc->Builder.SetInsertPoint(ElseBB);
  
  Value *ElseV = Else->Codegen(mc);
  if (ElseV == 0) return 0;
  
  mc->Builder.CreateBr(MergeBB);
  // Codegen of 'Else' can change the current block, update ElseBB for the PHI.
  ElseBB = mc->Builder.GetInsertBlock();
  
  // Emit merge block.
  TheFunction->getBasicBlockList().push_back(MergeBB);
  mc->Builder.SetInsertPoint(MergeBB);
  PHINode *PN = mc->Builder.CreatePHI(Type::getDoubleTy(getGlobalContext()), 2,
                                  "iftmp");
  
  PN->addIncoming(ThenV, ThenBB);
  PN->addIncoming(ElseV, ElseBB);
  return PN;
}

Value *ForExprAST::Codegen(MMPUCompiler *mc) {
  // Output this as:
  //   var = alloca double
  //   ...
  //   start = startexpr
  //   store start -> var
  //   goto loop
  // loop:
  //   ...
  //   bodyexpr
  //   ...
  // loopend:
  //   step = stepexpr
  //   endcond = endexpr
  //
  //   curvar = load var
  //   nextvar = curvar + step
  //   store nextvar -> var
  //   br endcond, loop, endloop
  // outloop:
  
  Function *TheFunction = mc->Builder.GetInsertBlock()->getParent();
  
  // Create an alloca for the variable in the entry block.
  AllocaInst *Alloca = CreateEntryBlockAlloca(TheFunction, VarName);
  
  // Emit the start code first, without 'variable' in scope.
  Value *StartVal = Start->Codegen(mc);
  if (StartVal == 0) return 0;
  
  // Store the value into the alloca.
  mc->Builder.CreateStore(StartVal, Alloca);
  
  // Make the new basic block for the loop header, inserting after current
  // block.
  BasicBlock *LoopBB = BasicBlock::Create(getGlobalContext(), "loop", TheFunction);
  
  // Insert an explicit fall through from the current block to the LoopBB.
  mc->Builder.CreateBr(LoopBB);
  
  // Start insertion in LoopBB.
  mc->Builder.SetInsertPoint(LoopBB);
  
  // Within the loop, the variable is defined equal to the PHI node.  If it
  // shadows an existing variable, we have to restore it, so save it now.
  AllocaInst *OldVal = mc->NamedValues[VarName];
  mc->NamedValues[VarName] = Alloca;
  
  // Emit the body of the loop.  This, like any other expr, can change the
  // current BB.  Note that we ignore the value computed by the body, but don't
  // allow an error.
  if (Body->Codegen(mc) == 0)
    return 0;
  
  // Emit the step value.
  Value *StepVal;
  if (Step) {
    StepVal = Step->Codegen(mc);
    if (StepVal == 0) return 0;
  } else {
    // If not specified, use 1.0.
    StepVal = ConstantFP::get(getGlobalContext(), APFloat(1.0));
  }
  
  // Compute the end condition.
  Value *EndCond = End->Codegen(mc);
  if (EndCond == 0) return EndCond;
  
  // Reload, increment, and restore the alloca.  This handles the case where
  // the body of the loop mutates the variable.
  Value *CurVar = mc->Builder.CreateLoad(Alloca, VarName.c_str());
  Value *NextVar = mc->Builder.CreateFAdd(CurVar, StepVal, "nextvar");
  mc->Builder.CreateStore(NextVar, Alloca);
  
  // Convert condition to a bool by comparing equal to 0.0.
  EndCond = mc->Builder.CreateFCmpONE(EndCond,
                                  ConstantFP::get(getGlobalContext(), APFloat(0.0)),
                                  "loopcond");
  
  // Create the "after loop" block and insert it.
  BasicBlock *AfterBB = BasicBlock::Create(getGlobalContext(), "afterloop", TheFunction);
  
  // Insert the conditional branch into the end of LoopEndBB.
  mc->Builder.CreateCondBr(EndCond, LoopBB, AfterBB);
  
  // Any new code will be inserted in AfterBB.
  mc->Builder.SetInsertPoint(AfterBB);
  
  // Restore the unshadowed variable.
  if (OldVal)
    mc->NamedValues[VarName] = OldVal;
  else
    mc->NamedValues.erase(VarName);
  
  
  // for expr always returns 0.0.
  return Constant::getNullValue(Type::getDoubleTy(getGlobalContext()));
}

Value *VarExprAST::Codegen(MMPUCompiler *mc) {
  std::vector<AllocaInst *> OldBindings;
  
  Function *TheFunction = mc->Builder.GetInsertBlock()->getParent();
  
  // Register all variables and emit their initializer.
  for (unsigned i = 0, e = VarNames.size(); i != e; ++i) {
    const std::string &VarName = VarNames[i].first;
    ExprAST *Init = VarNames[i].second;
    
    // Emit the initializer before adding the variable to scope, this prevents
    // the initializer from referencing the variable itself, and permits stuff
    // like this:
    //  var a = 1 in
    //    var a = a in ...   # refers to outer 'a'.
    Value *InitVal;
    if (Init) {
      InitVal = Init->Codegen(mc);
      if (InitVal == 0) return 0;
    } else { // If not specified, use 0.0.
      InitVal = ConstantFP::get(getGlobalContext(), APFloat(0.0));
    }
    
    AllocaInst *Alloca = CreateEntryBlockAlloca(TheFunction, VarName);
    mc->Builder.CreateStore(InitVal, Alloca);
    
    // Remember the old variable binding so that we can restore the binding when
    // we unrecurse.
    OldBindings.push_back(mc->NamedValues[VarName]);
    
    // Remember this binding.
    mc->NamedValues[VarName] = Alloca;
  }
  
  // Codegen the body, now that all vars are in scope.
  Value *BodyVal = Body->Codegen(mc);
  if (BodyVal == 0) return 0;
  
  // Pop all our variables from scope.
  for (unsigned i = 0, e = VarNames.size(); i != e; ++i)
    mc->NamedValues[VarNames[i].first] = OldBindings[i];
  
  // Return the body computation.
  return BodyVal;
}

Function *PrototypeAST::Codegen(MMPUCompiler *mc) {
  // Make the function type:  double(double,double) etc.
  std::vector<Type*> Doubles(Args.size(),
                             Type::getDoubleTy(getGlobalContext()));
  FunctionType *FT = FunctionType::get(Type::getDoubleTy(getGlobalContext()),
                                       Doubles, false);
  
  Function *F = Function::Create(FT, Function::ExternalLinkage, Name, mc->Mod);
  
  // If F conflicted, there was already something named 'Name'.  If it has a
  // body, don't allow redefinition or reextern.
  if (F->getName() != Name) {
    // Delete the one we just made and get the existing one.
    F->eraseFromParent();
    F = mc->Mod->getFunction(Name);
    
    // If F already has a body, reject this.
    if (!F->empty()) {
      ErrorF("redefinition of function");
      return 0;
    }
    
    // If F took a different number of args, reject.
    if (F->arg_size() != Args.size()) {
      ErrorF("redefinition of function with different # args");
      return 0;
    }
  }
  
  // Set names for all arguments.
  unsigned Idx = 0;
  for (Function::arg_iterator AI = F->arg_begin(); Idx != Args.size();
       ++AI, ++Idx)
    AI->setName(Args[Idx]);
  
  return F;
}

/// CreateArgumentAllocas - Create an alloca for each argument and register the
/// argument in the symbol table so that references to it will succeed.
void PrototypeAST::CreateArgumentAllocas(Function *F, MMPUCompiler *mc) {
  Function::arg_iterator AI = F->arg_begin();
  for (unsigned Idx = 0, e = Args.size(); Idx != e; ++Idx, ++AI) {
    // Create an alloca for this variable.
    AllocaInst *Alloca = CreateEntryBlockAlloca(F, Args[Idx]);
    
    // Store the initial value into the alloca.
    mc->Builder.CreateStore(AI, Alloca);
    
    // Add arguments to variable symbol table.
    mc->NamedValues[Args[Idx]] = Alloca;
  }
}

Function *FunctionAST::Codegen(MMPUCompiler *mc) {
  mc->NamedValues.clear();
  
  Function *TheFunction = Proto->Codegen(mc);
  if (TheFunction == 0)
    return 0;
  
  // If this is an operator, install it.
  if (Proto->isBinaryOp())
    BinopPrecedence[Proto->getOperatorName()] = Proto->getBinaryPrecedence();
  
  // Create a new basic block to start insertion into.
  BasicBlock *BB = BasicBlock::Create(getGlobalContext(), "entry", TheFunction);
  mc->Builder.SetInsertPoint(BB);
  
  // Add all arguments to the symbol table and create their allocas.
  Proto->CreateArgumentAllocas(TheFunction, mc);
  
  if (Value *RetVal = Body->Codegen(mc)) {
    // Finish off the function.
    mc->Builder.CreateRet(RetVal);
    
    // Validate the generated code, checking for consistency.
    verifyFunction(*TheFunction);
    
    // Optimize the function.
    //TheFPM->run(*TheFunction);
    
    return TheFunction;
  }
  
  // Error reading body, remove function.
  TheFunction->eraseFromParent();
  
  if (Proto->isBinaryOp())
    BinopPrecedence.erase(Proto->getOperatorName());
  return 0;
}
#endif
}
