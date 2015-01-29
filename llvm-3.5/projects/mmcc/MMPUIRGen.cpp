//===-- MMPUIRGen.cpp - MMPU IR Generator------------------------===//
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

#include "MMPUIRGen.h"
#include "MMPUAST.h"
#include "MMPUBlock.h"
#include "MMPUSymbol.h"
#include <sstream>

using namespace MMPU;

namespace MMPU {
const char *vm_instr_str[] = {
  "zero",

  "add",
  "sub",
  "mul",
  "div",
  "div",
  "mod",
  "mod",
  "neg",

  "icmp slt",
  "icmp sle",
  "icmp sge",
  "icmp sgt",
  "icmp eq",
  "icmp ne",

  "icmp ult",
  "icmp ule",
  "icmp uge",
  "icmp ugt",
    
  "and_i",
  "or_i",
  "xor_i",
  "not_i",
  "shl_i",
  "shr_i",
  "shr_ui",

  NULL,
};
  
void IRGen::GenIR(Block *BAst) {
  unsigned i = 0;
  if (BAst) {
    switch (BAst->getType()) {
    case Block::BLOCK_AST:
      if (BAst->getParent() &&
          BAst->getParent()->getType() == Block::BLOCK_WHILE)
        Gen_InstrFor2(BAst->getAST(), BAst->getParent());
      Gen_InstrExpr(BAst->getAST());
      break;
    case Block::BLOCK_RET:
      Gen_InstrReturn(BAst->getAST());
      break;
    case Block::BLOCK_WHILE:
      while (BAst->getParent()->getChild(i) != BAst) i++;
      Gen_InstrFor1(BAst->getParent()->getChild(i - 2)->getAST(),
                    BAst->getParent()->getChild(i - 1)->getAST(),
                    BAst);
    default:
      break;
    }
  }
  Dump();
}

void IRGen::Gen_LI(InstTy c, int64_t data, Symbol *s) {/*
  if (s == NULL) {
    *asmout << " " << vm_instr_str[c] << " " << data << "\n";
  } else if (data == 0) {
    *asmout << " " << vm_instr_str[c] << " " << s->getName() << "\n";
  } else {
    *asmout << " " << vm_instr_str[c] << " " << s->getName()
    << "+" << data << "\n";
  }*/
}

void IRGen::Gen_Code(InstTy c) {/*
  *asmout << " " << vm_instr_str[c] << "\n";*/
}
/*
 void IRGen::Gen_Int(int64_t c) {
 printf(" =%d\n",c);
 }
 */
void IRGen::Gen_Label(uint64_t c) {
  std::stringstream ss;
  ss << "L." << c << ":\n";
  IRStack.push_back(new std::string(ss.str()));
}

void IRGen::Gen_Jmp(ExprAST *Cond, uint64_t LTrue, uint64_t LFalse) {
  std::stringstream ss;
  if (Cond)
    ss << "br i1 %T." << TempID << ", label %L." << LTrue << ", label %L." << LFalse << "\n";
  else
    ss << "br label %L." << LTrue << "\n";
  IRStack.push_back(new std::string(ss.str()));
  if (Cond) Gen_Expr(Cond);
}

void IRGen::Gen_Cast(ExprAST *Expr) {
  ExprAST *e1;
  TagExprAST::TagTy t1, t2;
  std::stringstream ss;
  unsigned size = 0;
  
  if (Expr && Expr->getNext() && Expr->getNext()->getNext() &&
      isa<ExprExprAST>(Expr->getNext()->getNext())) {
    e1 = dyn_cast<ExprExprAST>(Expr->getNext()->getNext())->getExpr();
    if (isa<ExprExprAST>(Expr) && isa<TagExprAST>(dyn_cast<ExprExprAST>(Expr)))
      t1 = dyn_cast<TagExprAST>(dyn_cast<ExprExprAST>(Expr))->getTag();
    if (e1 && isa<ExprExprAST>(e1) &&
        isa<TagExprAST>(dyn_cast<ExprExprAST>(e1)->getExpr()))
      t2 = dyn_cast<TagExprAST>(dyn_cast<ExprExprAST>(e1)->getExpr())->getTag();
  }
  ss << "%T." << TempID++ << " = ";
  switch (t1) {
  case TagExprAST::TYPE_CHAR:
  case TagExprAST::TYPE_UCHAR:
    switch(t2) {
    case TagExprAST::TYPE_UINT:
      size += 16;
    case TagExprAST::TYPE_USHORT:
      size += 8;
    case TagExprAST::TYPE_UCHAR:
      size += 8;
      ss << "zext ";
      break;
    case TagExprAST::TYPE_INT:
      size += 16;
    case TagExprAST::TYPE_SHORT:
      size += 8;
    case TagExprAST::TYPE_CHAR:
      size += 8;
      ss << "sext ";
    default:
      break;
    }
    break;
      
  case TagExprAST::TYPE_SHORT:
  case TagExprAST::TYPE_USHORT:
    switch(t2) {
    case TagExprAST::TYPE_CHAR:
    case TagExprAST::TYPE_UCHAR:
      size = 8;
      ss << "trunc ";
      break;
    case TagExprAST::TYPE_UINT:
      size += 16;
    case TagExprAST::TYPE_USHORT:
      size += 16;
      ss << "zext ";
      break;
    case TagExprAST::TYPE_INT:
      size += 16;
    case TagExprAST::TYPE_SHORT:
      size += 16;
      ss << "sext ";
    default:
      break;
    }
    break;
      
  case TagExprAST::TYPE_INT:
  case TagExprAST::TYPE_UINT:
  case TagExprAST::TYPE_ENUM:
  case TagExprAST::TYPE_POINTER:
  case TagExprAST::TYPE_ARRAY:
  case TagExprAST::TYPE_FUNC:
    switch(t2) {
    case TagExprAST::TYPE_INT:
    case TagExprAST::TYPE_UINT:
      size += 16;
    case TagExprAST::TYPE_SHORT:
    case TagExprAST::TYPE_USHORT:
      size += 8;
    case TagExprAST::TYPE_CHAR:
    case TagExprAST::TYPE_UCHAR:
      size += 8;
      ss << "trunc ";
      break;
    default:
      break;
    }
    break;
      
  case TagExprAST::TYPE_VOID:
    break;
      
  default:
    //Error("Conversion to a non-type implements");
    break;
  }
  ss << "%T." << TempID << " to i" << size << "\n";
  IRStack.push_back(new std::string(ss.str()));
  Gen_RVal(e1);
}

void IRGen::Gen_Binary_Op(ExprAST *Expr,int op_i, int op_ui) {
  TagExprAST::TagTy t = TagExprAST::INVALIDTAG;
  ExprAST *e1 = NULL, *e2 = NULL;
  std::string *bop = new std::string();
  std::stringstream ss;
  uint64_t src1, src2;
  
  if (Expr && Expr->getNext()->getNext() &&
      isa<ExprExprAST>(Expr->getNext()->getNext())) {
    e1 = dyn_cast<ExprExprAST>(Expr->getNext()->getNext())->getExpr();
    if (Expr->getNext()->getNext()->getNext() &&
        isa<ExprExprAST>(Expr->getNext()->getNext()->getNext()))
      e2 = dyn_cast<ExprExprAST>(Expr->getNext()->getNext()->getNext())->getExpr();
  }
  if (e1 && isa<ExprExprAST>(e1) &&
      isa<TagExprAST>(dyn_cast<ExprExprAST>(e1)->getExpr()))
    t = dyn_cast<TagExprAST>(dyn_cast<ExprExprAST>(e1)->getExpr())->getTag();
  
  IRStack.push_back(bop);
  ss << "%T." << TempID++ << " = ";
  src1 = TempID;
  Gen_RVal(e2);
  src2 = TempID;
  Gen_RVal(e1);
  switch (t) {
    case TagExprAST::TYPE_UINT:
      ss << vm_instr_str[op_ui] << " i32 %T.";
      break;
    default:
      ss << vm_instr_str[op_i] << " i32 %T.";
      break;
  }
  ss << src1 << ", %T." << src2 << "\n";
  *bop = ss.str();
}

void IRGen::Gen_ExprUnary(ExprAST *Expr, int op_i, int op_ui) {
  TagExprAST::TagTy t;
  ExprAST *e1;
  
  if (Expr && Expr->getNext()->getNext() &&
      isa<ExprExprAST>(Expr->getNext()->getNext()))
    e1 = dyn_cast<ExprExprAST>(Expr->getNext()->getNext())->getExpr();
  if (e1 && isa<ExprExprAST>(e1) &&
      isa<TagExprAST>(dyn_cast<ExprExprAST>(e1)->getExpr()))
    t = dyn_cast<TagExprAST>(dyn_cast<ExprExprAST>(e1)->getExpr())->getTag();
  
  Gen_RVal(e1);
  switch (t) {
  case TagExprAST::TYPE_UINT:
      //*asmout << op_ui;
    break;
  default:
      //*asmout << op_i;
    break;
  }
}

void IRGen::Gen_Assign(ExprAST *Expr) {
  ExprAST *e1 = NULL, *e2 = NULL;
  TagExprAST::TagTy t;
  int64_t size = -1;
  std::stringstream ss;
  std::string *sentence = new std::string();
  
  if (Expr && isa<ExprExprAST>(Expr) &&
      isa<TagExprAST>(dyn_cast<ExprExprAST>(Expr)->getExpr()))
    t = dyn_cast<TagExprAST>(dyn_cast<ExprExprAST>(Expr)->getExpr())->getTag();
  
  if (Expr && Expr->getNext()->getNext() &&
      isa<ExprExprAST>(Expr->getNext()->getNext())) {
    e1 = dyn_cast<ExprExprAST>(Expr->getNext()->getNext())->getExpr();
    if (Expr->getNext()->getNext()->getNext() &&
        isa<ExprExprAST>(Expr->getNext()->getNext()->getNext()))
      e2 = dyn_cast<ExprExprAST>(Expr->getNext()->getNext()->getNext())->getExpr();
  }
  IRStack.push_back(new std::string("\n"));
  IRStack.push_back(sentence);
  Gen_LVal(e1);
  switch (t) {
  case TagExprAST::TYPE_CHAR:
  case TagExprAST::TYPE_UCHAR:
    ss << "store i8 %T." << TempID << ", i8* " << *IRStack.back();
    break;
  case TagExprAST::TYPE_SHORT:
  case TagExprAST::TYPE_USHORT:
    ss << "store i16 %T." << TempID << ", i16* " << *IRStack.back();
    break;
  case TagExprAST::TYPE_INT:
  case TagExprAST::TYPE_UINT:
  case TagExprAST::TYPE_ENUM:
  case TagExprAST::TYPE_POINTER:
  case TagExprAST::TYPE_ARRAY:
  case TagExprAST::TYPE_FUNC:
    ss << "store i32 %T." << TempID << ", i32* " << *IRStack.back();
    break;
  /* structures & unions */
  /*case TYPE_STRUCT:
  case TYPE_UNION:
    size=Type_Size(hd_list(e2));
    switch(size) {
    case 4:
      Gen_RVal(e2);
      Gen_Code(ld_i);
      Gen_LVal(e1);
      Gen_Code(st_i);
      break;
    default:
      Error_Internal("Gen_Assign: Taille de struct non implÈmentÈe");
    }
    break;*/
  default:
    Error("Gen_Assign: type not implemented");
  }
  IRStack.pop_back();
  *sentence = ss.str();
  Gen_RVal(e2);
}

void IRGen::Gen_ExprBinaryLogical(int op, ExprAST *Expr) {
  ExprAST *e1, *e2;
  uint64_t l1, l2;
  
  if (Expr && Expr->getNext()->getNext() &&
      isa<ExprExprAST>(Expr->getNext()->getNext())) {
    e1 = dyn_cast<ExprExprAST>(Expr->getNext()->getNext())->getExpr();
    if (Expr->getNext()->getNext()->getNext() &&
        isa<ExprExprAST>(Expr->getNext()->getNext()->getNext()))
      e2 = dyn_cast<ExprExprAST>(Expr->getNext()->getNext()->getNext())->getExpr();
  }
  l1 = CreateNewLabel();
  l2 = CreateNewLabel();
  
  Gen_RVal(e1);
  //if (op == TagExprAST::EXPR_LAND) Gen_Jmp(jeq_i, l1); else Gen_Jmp(jne_i, l1);
  
  Gen_RVal(e2);
  //if (op == TagExprAST::EXPR_LAND) Gen_Jmp(jeq_i, l1); else Gen_Jmp(jne_i, l1);
  
  //if (op == TagExprAST::EXPR_LAND) Gen_LI(li_i, 1, NULL); else Gen_LI(li_i, 0, NULL);
  //Gen_Jmp(jmp, l2);
  
  Gen_Label(l1);
  //if (op == TagExprAST::EXPR_LAND) Gen_LI(li_i, 0, NULL); else Gen_LI(li_i, 1, NULL);
  
  Gen_Label(l2);
}

void IRGen::Gen_ExprCond(ExprAST *Expr) {
  ExprAST *e, *e1, *e2;
  int l1,l2;
  
  if (Expr && Expr->getNext()->getNext() &&
      isa<ExprExprAST>(Expr->getNext()->getNext())) {
    e = dyn_cast<ExprExprAST>(Expr->getNext()->getNext())->getExpr();
    if (Expr->getNext()->getNext()->getNext() &&
        isa<ExprExprAST>(Expr->getNext()->getNext()->getNext())) {
      e1 = dyn_cast<ExprExprAST>(Expr->getNext()->getNext()->getNext())->getExpr();
      if (Expr->getNext()->getNext()->getNext()->getNext() &&
          isa<ExprExprAST>(Expr->getNext()->getNext()->getNext()->getNext()))
        e2 = dyn_cast<ExprExprAST>(Expr->getNext()->getNext()
                                   ->getNext()->getNext())->getExpr();
    }
  }
  
  l1 = CreateNewLabel();
  l2 = CreateNewLabel();
  
  Gen_RVal(e);
  //Gen_Jmp(jeq_i, l1);
  
  Gen_RVal(e1);
  //Gen_Jmp(jmp, l2);
  
  Gen_Label(l1);
  Gen_RVal(e2);
  
  Gen_Label(l2);
}

void IRGen::Gen_LIdent(ExprAST *Expr) {
  Symbol *s = NULL;
  ExprAST *var_adr = NULL;
  TagExprAST *tag;
  //IntExprAST *i;
  std::stringstream ss;
  
  if (Expr && Expr->getNext() && Expr->getNext()->getNext() &&
      isa<SymExprAST>(Expr->getNext()->getNext())) {
    s = dyn_cast<SymExprAST>(Expr->getNext()->getNext())->getSymbol();
    if (s && s->getExpr() && s->getExpr()->getNext() &&
        s->getExpr()->getNext()->getNext() &&
        s->getExpr()->getNext()->getNext()->getNext() &&
        isa<ExprExprAST>(s->getExpr()->getNext()->getNext()->getNext()))
      var_adr = dyn_cast<ExprExprAST>(s->getExpr()->getNext()
                                       ->getNext()->getNext())->getExpr();
  }
  tag = dyn_cast_or_null<TagExprAST>(var_adr);
  switch(tag->getTag()) {
    case TagExprAST::VAR_STACK:
      //i = dyn_cast_or_null<IntExprAST>(var_adr->getNext());
      ss << "%" << s->getName();
      IRStack.push_back(new std::string(ss.str()));
      //Gen_LI(libp_i, i->getVal(), NULL);
      break;
    default:
      ss << "%" << s->getName();
      IRStack.push_back(new std::string(ss.str()));
      break;
  }
}

void IRGen::Gen_RIdent(ExprAST *Expr) {
  TagExprAST::TagTy t = TagExprAST::INVALIDTAG;
  std::stringstream ss;
  std::string *sentence = new std::string();
  
  IRStack.push_back(sentence);
  ss << "%T." << TempID++ << " = load ";
  if (Expr && Expr->getNext() && isa<TagExprAST>(Expr->getNext())&&
      dyn_cast<TagExprAST>(Expr->getNext())->getTag() == TagExprAST::EXPR_INDEX)
    Gen_LVal(Expr);
  else
    Gen_LIdent(Expr);
  if (Expr && isa<ExprExprAST>(Expr) &&
      dyn_cast<ExprExprAST>(Expr)->getExpr() &&
      isa<TagExprAST>(dyn_cast<ExprExprAST>(Expr)->getExpr()))
    t = dyn_cast<TagExprAST>(dyn_cast<ExprExprAST>(Expr)->getExpr())->getTag();
  switch(t) {
    case TagExprAST::TYPE_CHAR:
      ss << "i8* ";
      break;
    case TagExprAST::TYPE_UCHAR:
      ss << "i8* ";
      break;
    case TagExprAST::TYPE_SHORT:
      ss << "i16* ";
      break;
    case TagExprAST::TYPE_USHORT:
      ss << "i16* ";
      break;
    case TagExprAST::TYPE_INT:
    case TagExprAST::TYPE_UINT:
    case TagExprAST::TYPE_POINTER:
    case TagExprAST::TYPE_ENUM:
      ss << "i32* ";
      break;
    case TagExprAST::TYPE_FUNC:
    case TagExprAST::TYPE_STRUCT:
    case TagExprAST::TYPE_UNION:
    case TagExprAST::TYPE_ARRAY:
      break;
    default:
      Error("Gen_RIdent: unmanaged types");
      break;
  }
  ss << *IRStack.back() << "\n";
  IRStack.pop_back();
  *sentence = ss.str();
}

void IRGen::Gen_Indir(ExprAST *Expr) {
  ExprAST *e1 = NULL;
  TagExprAST::TagTy t = TagExprAST::INVALIDTAG;
  
  if (Expr && Expr->getNext() && Expr->getNext()->getNext() &&
      isa<ExprExprAST>(Expr->getNext()->getNext())) {
    e1 = dyn_cast<ExprExprAST>(Expr->getNext()->getNext())->getExpr();
    Gen_RVal(e1);
    if (isa<ExprExprAST>(Expr) && dyn_cast<ExprExprAST>(Expr)->getExpr() &&
        isa<TagExprAST>(dyn_cast<ExprExprAST>(Expr)->getExpr()))
      t = dyn_cast<TagExprAST>(dyn_cast<ExprExprAST>(Expr)->getExpr())->getTag();
  }
  switch(t) {
    case TagExprAST::TYPE_CHAR:
      //Gen_Code(ld_b);
      break;
    case TagExprAST::TYPE_UCHAR:
      //Gen_Code(ld_ub);
      break;
    case TagExprAST::TYPE_SHORT:
      //Gen_Code(ld_w);
      break;
    case TagExprAST::TYPE_USHORT:
      //Gen_Code(ld_uw);
      break;
    case TagExprAST::TYPE_INT:
    case TagExprAST::TYPE_UINT:
    case TagExprAST::TYPE_POINTER:
      //Gen_Code(ld_i);
      break;
    case TagExprAST::TYPE_FUNC:
    case TagExprAST::TYPE_STRUCT:
    case TagExprAST::TYPE_UNION:
    case TagExprAST::TYPE_ENUM:
    case TagExprAST::TYPE_ARRAY:
      break;
    default:
      Error("Gen_Indir: unmanaged type");
      break;
  }
}

void IRGen::Gen_Const(ExprAST *Expr) {
  int64_t c = 0;
  ExprAST *type;
  //Symbol *s;
  ExprAST *buf;
  //int buf_size, i;
  TagExprAST::TagTy tag = TagExprAST::INVALIDTAG;
  std::stringstream ss;
  
  if (Expr && isa<ExprExprAST>(Expr)) {
    type = dyn_cast<ExprExprAST>(Expr)->getExpr();
    if (Expr->getNext() && isa<TagExprAST>(Expr->getNext()))
      tag = dyn_cast<TagExprAST>(Expr->getNext())->getTag();
  }
  if (tag == TagExprAST::EXPR_STR) {
    //AddSymbol(s = new Symbol(Symbol::TABLE_VAR, DeclBlock, NULL));
    //printf(".data\n");
    //printf("%s:\n",s->str);
    if (Expr && Expr->getNext()) buf = Expr->getNext()->getNext();
    /*while (buf != NULL) {
     buf_size = buf->data.buf_size;
     for(i=0;i<buf_size;i++) {
     printf(" .byte %d\n",buf->str[i]);
     }
     buf=tl(buf);
     }*/
    //printf(" .byte 0\n");
    //printf(".text\n");
    //Gen_LI(li_i, 0, s);
  } else {
    if (Expr && Expr->getNext() && Expr->getNext()->getNext() &&
        isa<IntExprAST>(Expr->getNext()->getNext()))
      c = dyn_cast<IntExprAST>(Expr->getNext()->getNext())->getVal();
    ss << "%T." << TempID++ << " = add i32 0, " << c << "\n";
    //Gen_LI(li_i, c, NULL);
  }
  IRStack.push_back(new std::string(ss.str()));
}

void IRGen::Gen_ExprCall(ExprAST *Expr) {
  uint64_t param_size;
  int64_t param_nb, i, j;
  ExprAST *params, *param, *func_expr;
  
  if (Expr && Expr->getNext() && Expr->getNext()->getNext() &&
      isa<ExprExprAST>(Expr->getNext()->getNext())) {
    func_expr = dyn_cast<ExprExprAST>(Expr->getNext()->getNext())->getExpr();
    if (Expr->getNext()->getNext()->getNext() &&
        isa<IntExprAST>(Expr->getNext()->getNext()->getNext())) {
      param_nb = dyn_cast<IntExprAST>(Expr->getNext()->getNext()
                                      ->getNext())->getVal();
      params = Expr->getNext()->getNext()->getNext()->getNext();
    }
  }
  param_size = 0;
  for (i = param_nb - 1; i >= 0; i--) {
    param = params;
    for(j=0; j<i; j++) param = param->getNext();
    Gen_RVal(dyn_cast<ExprExprAST>(param)->getExpr());
    //param_size += VM_STACK_ALIGN;
  }
  Gen_RVal(func_expr);
  //Gen_LI(jsr, param_size, NULL);
}

void IRGen::Gen_ExprList(ExprAST *Expr) {
  ExprAST *e;
  if (Expr && Expr->getNext())
    Expr = Expr->getNext()->getNext();
  do {
    if (Expr && isa<ExprExprAST>(Expr))
      e = dyn_cast<ExprExprAST>(Expr)->getExpr();
    Gen_RVal(e);
    if (Expr) Expr = Expr->getNext();
    //if (Expr != NULL) Gen_LI(addsp, -4, NULL);
  } while (Expr != NULL);
}

void IRGen::Gen_LVal(ExprAST *E) {
  TagExprAST::TagTy tag = TagExprAST::INVALIDTAG;
  
  if (E && E->getNext() &&
      isa<TagExprAST>(E->getNext()))
    tag = dyn_cast<TagExprAST>(E->getNext())->getTag();
  switch(tag) {
  case TagExprAST::EXPR_IDENT:
    Gen_LIdent(E);
    break;
  case TagExprAST::EXPR_APDLINC:
    Gen_LIdent(E);
    break;
  case TagExprAST::EXPR_INDIR:
    if (E && E->getNext() && E->getNext()->getNext() &&
        isa<ExprExprAST>(E->getNext()->getNext()))
      Gen_RVal(dyn_cast<ExprExprAST>(E->getNext()->getNext())->getExpr());
    break;
  case TagExprAST::EXPR_INDEX:
      Gen_Index(E);
    break;
  default:
    Error("Gen_LVal: operation %d is not implemented", tag);
    break;
  }
}

void IRGen::Gen_Index(ExprAST *E) {
  TagExprAST::TagTy tag = TagExprAST::INVALIDTAG;
  ExprAST *var = NULL, *var_type = NULL;
  Symbol *sym = NULL;
  ExprExprAST *ee = NULL;
  TagExprAST *dim = NULL;
  std::stringstream ss;
  std::string *sentence = new std::string();
  uint64_t len = 0, level = 0, curlevel = 0, lastptr = 0;
  
  IRStack.push_back(sentence);
  
  if (E && E->getNext() && E->getNext()->getNext() &&
      isa<ExprExprAST>(E->getNext()->getNext()))
    var = dyn_cast<ExprExprAST>(E->getNext()->getNext())->getExpr();
  
  if (var) var = var->getNext();
  else var = NULL;
  
  if (isa<SymExprAST>(var))
    sym = dyn_cast<SymExprAST>(var)->getSymbol();
  
  if (E && E->getNext() && E->getNext()->getNext() &&
      E->getNext()->getNext()->getNext())
    ee = dyn_cast_or_null<ExprExprAST>(E->getNext()->getNext()->getNext());
  
  if (sym && sym->getExpr() && sym->getExpr()->getNext() &&
      sym->getExpr()->getNext()->getNext() &&
      isa<ExprExprAST>(sym->getExpr()->getNext()->getNext()))
    var_type = dyn_cast<ExprExprAST>(sym->getExpr()->getNext()
                                        ->getNext())->getExpr();
  do {
    ss << "%T." << TempID << " = getelementptr ";
    dim = dyn_cast_or_null<TagExprAST>(var_type);
    for (unsigned i = 0; i < curlevel; i++) {
      if (dim && dim->getNext())
        dim = dyn_cast_or_null<TagExprAST>(dim->getNext()->getNext());
    }
    level = 0;
    do {
      if (dim->getNext() && isa<IntExprAST>(dim->getNext())) {
        len = dyn_cast<IntExprAST>(dim->getNext())->getVal();
        dim = dyn_cast_or_null<TagExprAST>(dim->getNext()->getNext());
        ss << "[" << len << " x ";
        level++;
      }
    } while (dim && dim->getTag() == TagExprAST::TYPE_ARRAY);
    if (dim) ss << "i" << dim->TypeSize() * 8;
    while (level--) ss << "]";
    ss << "* %";
    if (curlevel)
      ss << "T." << lastptr;
    else
      ss << sym->getName();
    lastptr = TempID;
    ss << ", i32 0, i32 %T." << ++TempID << "\n";
    if (ee) Gen_RVal(ee->getExpr());
    if (ee->getNext() && isa<TagExprAST>(ee->getNext())) {
      tag = dyn_cast<TagExprAST>(ee->getNext())->getTag();
      ee = dyn_cast_or_null<ExprExprAST>(ee->getNext()->getNext());
    } else tag = TagExprAST::INVALIDTAG;
    ++curlevel;
  } while (tag != TagExprAST::INVALIDTAG);
  *sentence = ss.str();
  ss.str("");
  ss << "%T." << lastptr;
  IRStack.push_back(new std::string(ss.str()));
}
  
void IRGen::Gen_RVal(ExprAST *E) {
  TagExprAST::TagTy tag = TagExprAST::INVALIDTAG;
  
  if (E && E->getNext() &&
      isa<TagExprAST>(E->getNext()))
    tag = dyn_cast<TagExprAST>(E->getNext())->getTag();
  
  switch (tag) {
    case TagExprAST::EXPR_INT:
    case TagExprAST::EXPR_STR:
      Gen_Const(E);
      break;
    case TagExprAST::EXPR_ASSIGN:
      Gen_Assign(E);
      break;
    case TagExprAST::EXPR_IDENT:
      Gen_RIdent(E);
      break;
    case TagExprAST::EXPR_INDIR:
      Gen_Indir(E);
      break;
    case TagExprAST::EXPR_INDEX:
      Gen_RIdent(E);
      break;
    case TagExprAST::EXPR_APDLINC:
      Gen_RIdent(E);
      break;
    case TagExprAST::EXPR_ADDR:
      if (E && E->getNext() && E->getNext()->getNext() &&
          isa<ExprExprAST>(E->getNext()->getNext()))
        Gen_LVal(dyn_cast<ExprExprAST>(E->getNext()->getNext())->getExpr());
      break;
    case TagExprAST::EXPR_CALL:
      Gen_ExprCall(E);
      break;
    case TagExprAST::EXPR_CAST:
      Gen_Cast(E);
      break;
    case TagExprAST::EXPR_LIST:
      Gen_ExprList(E);
      break;
      
    case TagExprAST::EXPR_ADD:
    case TagExprAST::EXPR_SADD:
      Gen_Binary_Op(E, add_i, add_i);
      break;
    case TagExprAST::EXPR_SUB:
    case TagExprAST::EXPR_SSUB:
      Gen_Binary_Op(E, sub_i, sub_i);
      break;
    case TagExprAST::EXPR_MUL:
      Gen_Binary_Op(E, mul_i, mul_i);
      break;
    case TagExprAST::EXPR_DIV:
      Gen_Binary_Op(E, div_i, div_ui);
      break;
    case TagExprAST::EXPR_MOD:
      Gen_Binary_Op(E, mod_i, mod_ui);
      break;
    case TagExprAST::EXPR_AND:
      Gen_Binary_Op(E, and_i, and_i);
      break;
    case TagExprAST::EXPR_OR:
      Gen_Binary_Op(E, or_i, or_i);
      break;
    case TagExprAST::EXPR_XOR:
      Gen_Binary_Op(E, xor_i, xor_i);
      break;
    case TagExprAST::EXPR_SHR:
      Gen_Binary_Op(E, shr_i, shr_ui);
      break;
    case TagExprAST::EXPR_SHL:
      Gen_Binary_Op(E, shl_i, shl_i);
      break;
      
    case TagExprAST::EXPR_NOT:
      Gen_ExprUnary(E, not_i, not_i);
      break;
    case TagExprAST::EXPR_NEG:
      Gen_ExprUnary(E, neg_i, neg_i);
      break;
      
    case TagExprAST::EXPR_LT:
      Gen_Binary_Op(E, cmplt_i, cmplt_ui);
      break;
    case TagExprAST::EXPR_LE:
      Gen_Binary_Op(E, cmple_i, cmple_ui);
      break;
    case TagExprAST::EXPR_GT:
      Gen_Binary_Op(E, cmpgt_i, cmpgt_ui);
      break;
    case TagExprAST::EXPR_GE:
      Gen_Binary_Op(E, cmpge_i, cmpge_ui);
      break;
    case TagExprAST::EXPR_EQ:
      Gen_Binary_Op(E, cmpeq_i, cmpeq_i);
      break;
    case TagExprAST::EXPR_NE:
      Gen_Binary_Op(E, cmpne_i, cmpne_i);
      break;
      
    case TagExprAST::EXPR_LAND:
    case TagExprAST::EXPR_LOR:
      Gen_ExprBinaryLogical(tag, E);
      break;
      
    case TagExprAST::EXPR_COND:
      Gen_ExprCond(E);
      break;
      
    default:
      Error("Gen_RVal: operation %d is unimplemented", tag);
      break;
  }
}

void IRGen::Gen_Expr(ExprAST *E) {
  Gen_RVal(E);
}

void IRGen::Gen_InstrIf1(ExprAST *Expr) {
  int64_t l1, l2;
  
  l1 = CreateNewLabel();
  l2 = CreateNewLabel();
  
  Gen_Expr(Expr);
  //Gen_Jmp(jeq_i, l1);
  
  //Block_Enter(Block::BLOCK_IF);
  //CurBlock->setControlLabels(l1, l2);
  
  delete Expr;
}

void IRGen::Gen_InstrIf2(void) {
  //Gen_Label(CurBlock->getBreak());
  //Block_Leave();
}

void IRGen::Gen_InstrIfElse2(void) {
  //Gen_Jmp(jmp, CurBlock->getContinue());
  //Gen_Label(CurBlock->getBreak());
}

void IRGen::Gen_InstrIfElse3(void) {
  //Gen_Label(CurBlock->getContinue());
  //Block_Leave();
}

void IRGen::Gen_InstrWhile1(ExprAST *Expr) {
  int64_t l1,l2;
  
  l1 = CreateNewLabel();
  l2 = CreateNewLabel();
  
  Gen_Label(l1);
  Gen_Expr(Expr);
  //Gen_Jmp(jeq_i, l2);
  
  //Block_Enter(Block::BLOCK_WHILE);
  //CurBlock->setControlLabels(l2, l1);
  
  delete Expr;
}

void IRGen::Gen_InstrWhile2(void) {
  //Gen_Jmp(jmp, CurBlock->getContinue());
  //Gen_Label(CurBlock->getBreak());
  //Block_Leave();
}

void IRGen::Gen_InstrDo1(void) {
  int64_t l1, l2, l3;
  
  l1 = CreateNewLabel();
  l2 = CreateNewLabel();
  l3 = CreateNewLabel();
  
  Gen_Label(l1);
  
  //Block_Enter(Block::BLOCK_WHILE);
  //CurBlock->setControlLabels(l2, l3, l1);
}

void IRGen::Gen_InstrDo2(ExprAST *Expr) {
  //Gen_Label(CurBlock->getContinue());
  
  Gen_Expr(Expr);
  //Gen_Jmp(jne_i, CurBlock->getRestart());
  
  //Gen_Label(CurBlock->getBreak());
  
  //Block_Leave();
  
  delete Expr;
}

void IRGen::Gen_InstrFor1(ExprAST *Expr1, ExprAST *Expr2, Block *BFor) {
  uint64_t l = CreateNewLabel();
  int64_t lb, lc, lr;
  lb = CreateNewLabel();
  lc = CreateNewLabel();
  lr = CreateNewLabel();
  BFor->setControlLabels(lb, lc, lr);
  
  Gen_Label(lr);
  Gen_Jmp(NULL, lr);
  if (Expr1 != NULL) Gen_InstrExpr(Expr1);
  Dump();
  
  Gen_Label(l = CreateNewLabel());
  Gen_Jmp(Expr2, l, lb);
}

void IRGen::Gen_InstrFor2(ExprAST *Expr3, Block *BFor) {
  Gen_Label(BFor->getBreak());
  Gen_Jmp(NULL, BFor->getRestart());
  if (Expr3 != NULL) Gen_InstrExpr(Expr3);
  Gen_Label(BFor->getContinue());
  Gen_Jmp(NULL, BFor->getContinue());
}

void IRGen::Gen_InstrBreak(void) {
  Block *b;
  //b = CurBlock;
  while (1) {
    if (b->getType() == Block::BLOCK_GLOBAL)
      Error("Command 'break' without loop or 'switch'");
    if (b->getType() == Block::BLOCK_WHILE ||
        b->getType() == Block::BLOCK_SWITCH)
      break;
    b = b->getParent();
  }
  //Gen_Jmp(jmp, b->getBreak());
}

void IRGen::Gen_InstrContinue(void) {
  Block *b;
  //b = CurBlock;
  while (1) {
    if (b->getType() == Block::BLOCK_GLOBAL)
      Error("Instruction 'continue' without loop");
    if (b->getType() == Block::BLOCK_WHILE) break;
    b = b->getParent();
  }
  //Gen_Jmp(jmp, b->getContinue());
}

void IRGen::Gen_InstrGoto(VariableExprAST *Ident) {
  Symbol *s;
  int64_t l1;
  std::string label;
  
  if (Ident) label = Ident->getName();
  
  //s = LookupSym(label, Symbol::TABLE_LABEL);
  if (s == NULL) {
    l1 = CreateNewLabel();
    //  AddSymbol(new Symbol(label, Symbol::TABLE_LABEL, FuncBlock,
    //                     new IntExprAST(-1, new IntExprAST(l1, NULL))));
  } else {
    if (s->getExpr() && s->getExpr()->getNext() &&
        isa<IntExprAST>(s->getExpr()->getNext()))
      l1 = dyn_cast<IntExprAST>(s->getExpr()->getNext())->getVal();
  }
  //Gen_Jmp(jmp, l1);
  
  delete Ident;
}

void IRGen::Gen_InstrReturn(ExprAST *Expr) {
  if (Expr != NULL) {
    Gen_Expr(Expr);
  } else {
    IRStack.push_back(new std::string("ret void\n"));
  }
}

void IRGen::Gen_InstrLabel(VariableExprAST *Ident) {
  Symbol *s;
  int64_t l1, flag;
  std::string label;
  
  if (Ident) label = Ident->getName();
  
  //s = LookupSym(label, Symbol::TABLE_LABEL);
  if (s != NULL) {
    if (s->getExpr() && isa<IntExprAST>(s->getExpr())) {
      flag = dyn_cast<IntExprAST>(s->getExpr())->getVal();
      if (flag != -1) Error("Label '%s' redefined", label.c_str());
      if (s->getExpr()->getNext() && isa<IntExprAST>(s->getExpr()->getNext()))
			  l1 = dyn_cast<IntExprAST>(s->getExpr()->getNext())->getVal();
			delete s->getExpr();
			s->setExpr(new IntExprAST(0, new IntExprAST(l1, NULL)));
    }
  } else {
    l1 = CreateNewLabel();
    // AddSymbol(new Symbol(label, Symbol::TABLE_LABEL, FuncBlock,
    //                     new IntExprAST(0, new IntExprAST(l1, NULL))));
  }
  Gen_Label(l1);
  
  delete Ident;
}

void IRGen::Gen_InstrExpr(ExprAST *Expr) {
  Gen_Expr(Expr);
  //Gen_Code(pop);
  //delete Expr;
}

void IRGen::Gen_InstrSwitch1(ExprAST *Expr) {
  int64_t l1;
  Symbol *s;
  
  l1 = CreateNewLabel();
  //AddSymbol(s = new Symbol(Symbol::TABLE_VAR, DeclBlock, NULL));
  
  Gen_Expr(Expr);
  //Gen_LI(li_i, 0, s);
  //Gen_Code(switch_i);
  
  //Block_Enter(Block::BLOCK_SWITCH);
  //setControlLabels(l1, -1, -1);
  //CurBlock->se = new SymExprAST(s, NULL);
  
  delete Expr;
}

#if 0
void IRGen::Gen_InstrSwitch2(void) {
  LIST *sv,*sv1;
  SYM *s;
  int count;
  
  sv=block_current->switch_values;
  s=hd_sym(sv);
  sv=tl(sv);
  
  sv1=sv;
  count=1;
  while (sv1!=NULL) {
    count++;
    sv1=tl(tl(sv1));
  }
  
  printf(".data\n");
  printf(" .align 4\n");
  printf("%s:\n",s->str);
  
  /* gÈnÈration de la table de valeurs */
  printf(" .int %d\n",count);
  sv1=sv;
  while (sv1!=NULL) {
    printf(" .int %d\n",hd_int(sv1));
    sv1=tl(tl(sv1));
  }
  
  /* table de pointeurs */
  if (block_current->label_restart != -1) {
    printf(" .int @L%d\n",block_current->label_restart);
  } else {
    printf(" .int @L%d\n",block_current->label_break);
  }
  sv1=sv;
  while (sv1!=NULL) {
    printf(" .int @L%d\n",hd_int(tl(sv1)));
    sv1=tl(tl(sv1));
  }
  
  printf(".text\n");
  list_free(block_current->switch_values);
  
  Gen_Label(block_current->label_break);
  Block_Leave();
}

/*
 * On ne dÈtecte pas ici s'il y a des Ètiquettes case redondantes.
 * A faire !
 */

void Gen_InstrCase(LIST *expr)
{
  BLOCK *b;
  int l1;
  
  b=block_current;
  while (1) {
    if (b->type == BLOCK_GLOBAL) Error("Instruction 'case' ou 'default' sans switch");
    if (b->type == BLOCK_SWITCH) break;
    b=b->dad;
  }
  
  l1=Gen_NewLabel();
  Gen_Label(l1);
  
  if (expr==NULL) {
    /* default */
    if (b->label_restart != -1) Error("Un seul 'default:' autorisÈ");
    b->label_restart=l1;
  } else {
    /* case */
    /* test de type non fait */
    if (hd_tag(tl(expr)) != EXPR_INT ) 
      Error("Expression constante entiËre attendue aprËs un case");
    
    b->switch_values=append(b->switch_values,
                            mk_int(hd_int(tl(tl(expr))),mk_int(l1,NULL)));
  }
  
  list_free(expr);
}
#endif
}
