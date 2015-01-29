//===------ MMPUCompiler.cpp - MMPU Compiler ----------------------------===//
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
  
void mmpucompilererror(MMPUCompiler *mc, const char * p) {std::cerr << p << std::endl;}
void Error(const char *format,...) {
  va_list ap;
    
  va_start(ap,format);
  vfprintf(stderr,format,ap);
  va_end(ap);
    
  fprintf(stderr,"\n");
  exit(1);
}
#include "MMPUGenFrontEndLexer.flex"
#include "MMPUGenFrontEndParser.bison"

Module *MMPUCompiler::parse() {
  
  YY_BUFFER_STATE buf = mmpucompiler_create_buffer(0, YY_BUF_SIZE);
  mmpucompiler_switch_to_buffer(buf);
  mmpucompiler_flush_buffer(buf);
  
  BEGIN INITIAL;
  mmpucompilerparse(this);
  
  mmpucompiler_delete_buffer(buf);
  return Mod;
}

Symbol *MMPUCompiler::LookupSym(const std::string &Name,
                                Symbol::SymbolTy Type) const {
  if (Sym_Table.count(Name) &&
      Sym_Table.find(Name)->second->getType() == Type)
    return Sym_Table.find(Name)->second;
  else return NULL;
}
  
Symbol *MMPUCompiler::CreateSymbol(const std::string *Name,
                                   Symbol::SymbolTy Type,
                                   MMPU::ExprAST *E,
                                   Block *B) {
  if (Name == NULL)
    return new Symbol(Type, B, E);
  Symbol *s = LookupSym(*Name, Type);
  if (s && (s->getBlock() == CurBlock))
    Error("The identifier '%s' is already defined within this block",
          Name->c_str());
  return new Symbol(*Name, Type, B, E);
}
  
/*
 * Static local variable declaration
 *
 * 'VarType' is not duplicated
 */
void MMPUCompiler::Local_Static(const std::string &VarName,
                                TagExprAST::TagTy VarStorage,
                                ExprAST *VarType,
                                ExprAST *VarInit) {
  AddSymbol(CreateSymbol(&VarName, Symbol::TABLE_VAR,
                         new TagExprAST(TagExprAST::SYM_VAR,
                             new TagExprAST(VarStorage,
                                 new ExprExprAST(VarType,
                                     new ExprExprAST(
                                          new TagExprAST(TagExprAST::VAR_DATA,
                                                         NULL), NULL))))));
    
  //Gen_VarInit(var_name, var_type, var_init);
  //printf(" .text\n");
}
  
  
/*
 * Local variable declaration
 *
 * 'VarType' is not duplicated
 */
void MMPUCompiler::Local_Declare(std::string &VarName,
                                 TagExprAST::TagTy Storage,
                                 ExprAST *VarType,
                                 ExprAST *VarInit) {
  int64_t var_offset, var_size, var_align;
  Block *expr;
  if (VarType && isa<TagExprAST>(VarType))
    var_size = dyn_cast<TagExprAST>(VarType)->TypeSize();
  //var_align = Type_Align(var_type);
  if (var_size == -1)
    Error("Size of the local variable '%s' unknown", VarName.c_str());
  
#if 0
  /* alignement */
  block_decl->local_var_offset = (block_decl->local_var_offset + (var_align - 1)) & ~(var_align - 1);
  /* allocation */
  var_offset = block_decl->local_var_offset;
  block_decl->local_var_offset = block_decl->local_var_offset+var_size;
  if (block_decl->local_var_offset > local_var_size)
    local_var_size = block_decl->local_var_offset;
    
  printf("/* local variable '%s' stack offset = %d */\n", var_name, var_offset);
#endif
  TagExprAST *dim;
  uint64_t len, level = 0;
  if (dyn_cast<TagExprAST>(VarType)->getTag() == TagExprAST::TYPE_ARRAY) {
    dim = dyn_cast<TagExprAST>(VarType);
    *asmout << "%" << VarName << " = alloca ";
    do {
      if (dim->getNext() && isa<IntExprAST>(dim->getNext())) {
        len = dyn_cast<IntExprAST>(dim->getNext())->getVal();
        dim = dyn_cast_or_null<TagExprAST>(dim->getNext()->getNext());
        *asmout << "[" << len << " x ";
        level++;
      }
    } while (dim && dim->getTag() == TagExprAST::TYPE_ARRAY);
    if (dim)
      *asmout << "i" << dim->TypeSize() * 8;
    while (level--) *asmout << "]";
    *asmout << "\n";
  } else
    *asmout << "%" << VarName << " = alloca i" << var_size * 8 << "\n";
  AddSymbol(CreateSymbol(&VarName, Symbol::TABLE_VAR,
                         new TagExprAST(TagExprAST::SYM_VAR,
                             new TagExprAST(Storage,
                                 new ExprExprAST(VarType,
                                     new ExprExprAST(
                                         new TagExprAST(TagExprAST::VAR_STACK,
                                             new IntExprAST(var_offset,
                                                            NULL)), NULL))))));
    
  /* potential initialization */
  if (VarInit != NULL) {
    if (!isa<TagExprAST>(VarInit) ||
        dyn_cast<TagExprAST>(VarInit)->getTag() != TagExprAST::INIT_EXPR)
      Error("Unhandled static initialization for local variables");
    CurBlock->addChild(expr = new Block(Block::BLOCK_AST, CurBlock));
    expr->setAST(ExprAST::CreateAssign(ExprAST::CreateIdent(VarName, this),
                                       VarInit->getNext()->Dup()));
    getIRGen()->GenIR(expr);
  }
}

/*
 * Declaring a variable list with any initialization
 */
void MMPUCompiler::Var_Declare(ExprAST *VarList) {
  std::string var_name;
  ExprAST *vars, *var_type, *var_init;
  TagExprAST::TagTy var_storage;
  Symbol *s;
    
  vars = VarList;
  while (vars != NULL) {
    if (isa<ExprExprAST>(vars) &&
        isa<VariableExprAST>(dyn_cast<ExprExprAST>(vars)->getExpr()))
      var_name = dyn_cast<VariableExprAST>(
                   dyn_cast<ExprExprAST>(vars)->getExpr())->getName();
    if (vars->getNext() && isa<TagExprAST>(vars->getNext())) {
      var_storage = dyn_cast<TagExprAST>(vars->getNext())->getTag();
      if (vars->getNext()->getNext() &&
          isa<ExprExprAST>(vars->getNext()->getNext())) {
        var_type = dyn_cast<ExprExprAST>(vars->getNext()->getNext())
                     ->getExpr()->Dup(); /* we duplicate the type here! */
        if (vars->getNext()->getNext()->getNext() &&
            isa<ExprExprAST>(vars->getNext()->getNext()->getNext()))
			    var_init = dyn_cast<ExprExprAST>(
                       vars->getNext()->getNext()->getNext())->getExpr();
      }
    }
/*
    printf("DÈclaration: var=%s \n",var_name);
    printf("type="); List_Print(var_type);
    printf("\ninit="); List_Print(var_init);
    printf("\n");
*/
    if (var_storage == TagExprAST::STORAGE_TYPEDEF) {
      /* If of the typedef */
      if (var_init) Error("A 'typedef' one shall be initialized");
      CreateSymbol(&var_name, Symbol::TABLE_VAR,
                   new TagExprAST(TagExprAST::TYPE_TYPEDEF_IDENT, var_type));
    } else if (CurBlock == GlobalBlock) {
      /* global declaration */
      if (var_storage == TagExprAST::STORAGE_REGISTER ||
          var_storage == TagExprAST::STORAGE_AUTO ||
          /* APDL specified storage types should not be used for
             global declarations */
          (var_storage >= TagExprAST::STORAGE_DM &&
           var_storage <= TagExprAST::STORAGE_MACREG))
        Error("Wrong storage specifier for global variable");
      
      /* if it is a function, it is by default in external */
      if (var_type && isa<TagExprAST>(var_type) &&
          dyn_cast<TagExprAST>(var_type)->getTag() == TagExprAST::TYPE_FUNC)
        var_storage = TagExprAST::STORAGE_EXTERN;
      
      Global_Declare(var_name, var_storage, var_type);
#if 0
      if (var_storage != TagExprAST::STORAGE_EXTERN) {
        Gen_VarInit(var_name, var_type, var_init);
        if (var_storage != TagExprAST::STORAGE_STATIC) {
          printf(" .globl %s\n", var_name);
        }
      }
#endif
    } else {
      /* local declaration */
      s = LookupSym(var_name, Symbol::TABLE_VAR);
      if (s) {
        if (s->getBlock() == DeclBlock)
          Error("Redefinition of '%s' in the current block", var_name.c_str());
      }
      if (var_storage == TagExprAST::STORAGE_STATIC) {
        Local_Static(var_name, var_storage, var_type, var_init);
      } else {
        Local_Declare(var_name, var_storage, var_type, var_init);
      }
    }
    if (vars->getNext() && vars->getNext()->getNext() &&
        vars->getNext()->getNext()->getNext())
      vars = vars->getNext()->getNext()->getNext()->getNext();
  }
}
  
void MMPUCompiler::Func_Declare(ExprAST *Spec,
                                ExprAST *Decl,
                                ExprAST *VarList) {
  ExprAST *func, *func_type, *params, *param_type;
  VariableExprAST *l;
  std::string func_name, param_name;
  TagExprAST::TagTy func_storage, func_style;
  TagExprAST::TagTy param_storage;
  int64_t param_size, param_offset;
  int64_t stack_offset;
  Symbol *s;
  std::string symname;
  
  func = Type_VarList(Spec, new ExprExprAST(Decl,
                                            new ExprExprAST(NULL, NULL)));
    
  if (isa<ExprExprAST>(func) &&
      isa<VariableExprAST>(dyn_cast<ExprExprAST>(func)->getExpr()))
    func_name = dyn_cast<VariableExprAST>(
                dyn_cast<ExprExprAST>(func)->getExpr())->getName();
  if (isa<TagExprAST>(func->getNext()))
    func_storage = dyn_cast<TagExprAST>(func->getNext())->getTag();
  if (isa<ExprExprAST>(func->getNext()->getNext()))
    func_type =
      dyn_cast<ExprExprAST>(func->getNext()->getNext())->getExpr()->Dup();
  
  if (isa<TagExprAST>(func_type) &&
      dyn_cast<TagExprAST>(func_type)->getTag() != TagExprAST::TYPE_FUNC)
    Error("Function declaration expected");
  if (func_storage != TagExprAST::STORAGE_STATIC &&
      func_storage != TagExprAST::STORAGE_DEFAULT)
    Error("Invalid storage class for function '%s'", func_name.c_str());
  
  if (isa<TagExprAST>(func_type->getNext()))
    func_style = dyn_cast<TagExprAST>(func_type->getNext())->getTag();
    
  if (VarList != NULL && func_style != TagExprAST::FUNC_OLD)
    Error("Mixing old form and new form");
    
  /* Creation of the new definition entry function */
    
  Global_Declare(func_name, func_storage, func_type);
    
  /* New block */
  Block_Enter(Block::BLOCK_DECL);
  FuncBlock = DeclBlock = CurBlock;
  
  /* Create new APDL block*/
  if (isDefiningAPDL) {
    APDLBlock = CurBlock;//new Block(Block::BLOCK_DECL, NULL);
                         //GlobalBlock->addChild(APDLBlock);
    APDLBlock->setAPDL(true);
  }
  
  /* introduction of variables defined in the old form */
  params = VarList;
  while (params != NULL) {
    if (isa<ExprExprAST>(params))
      l = dyn_cast<VariableExprAST>(dyn_cast<ExprExprAST>(params)->getExpr());
    if (l) {
      param_name = l->getName();
      params = params->getNext();
    }
    if (params && isa<TagExprAST>(params)) {
      param_storage = dyn_cast<TagExprAST>(params)->getTag();
      params = params->getNext();
    }
    if (params && isa<ExprExprAST>(params)) {
      param_type = dyn_cast<ExprExprAST>(params)->getExpr()->Dup();
      params = params->getNext();
    }
    if (params) params = params->getNext();
    AddSymbol(CreateSymbol(&param_name, Symbol::TABLE_VAR,
                          new TagExprAST(TagExprAST::SYM_VAR,
                              new TagExprAST(param_storage,
                                  new ExprExprAST(param_type,
                                      new ExprExprAST(NULL, NULL))))));
  }
  delete VarList;
    
  /* header generation function */
  // *asmout << ".text\n"
  *asmout << " ; definition of function " << func_name << " \n";
  std::string rettype;
  switch (dyn_cast<TagExprAST>(func_type->getNext()->getNext()->getNext())->getTag()) {
  case TagExprAST::TYPE_INT:
    rettype = "i32";
    break;
  case TagExprAST::TYPE_SHORT:
    rettype = "i16";
    break;
  case TagExprAST::TYPE_CHAR:
    rettype = "i8";
    break;
  case TagExprAST::TYPE_VOID:
    rettype = "void";
    break;
  default:
    break;
  }
  *asmout << "define " << rettype << " @"<< func_name << "() {\n";
  //if (func_storage != TagExprAST::STORAGE_STATIC ||
  //    func_storage != TagExprAST::STORAGE_APDL)
    //  *asmout << " .globl " << func_name << "\n";

  LocalVarSym = CreateSymbol(NULL, Symbol::TABLE_VAR, NULL);
  AddSymbol(LocalVarSym);

  //Gen_LI(addsp, 0, LocalVarSym);
    
  /* Preparation for declaration of local variables */
  DeclBlock->local_var_offset = VM_LOCAL_START;
  LocalVarSize = VM_LOCAL_START;
    
  /* Environmental parameters in the block and
     calculating their address */
  if (func_type && func_type->getNext() && func_type->getNext()->getNext() &&
      isa<ExprExprAST>(func_type->getNext()->getNext()))
    params = dyn_cast<ExprExprAST>(func_type->getNext()->getNext())->getExpr();
    
  stack_offset = VM_LOCAL_PARAM_END;
    
  while (params != NULL) {
    if (!isa<ExprExprAST>(params) ||
        !dyn_cast<ExprExprAST>(params)->getExpr())
      Error("Function parameter without a name");
    l = dyn_cast<VariableExprAST>(dyn_cast<ExprExprAST>(params)->getExpr());
    if (l) param_name = l->getName();
    params = params->getNext();
    if (params && isa<TagExprAST>(params)) {
      param_storage = dyn_cast<TagExprAST>(params)->getTag();
			params = params->getNext();
    }
    if (params && isa<ExprExprAST>(params)) {
      param_type = dyn_cast<ExprExprAST>(params)->getExpr()->Dup();
      if ((params = params->getNext())) params = params->getNext();
    }
			
    /* If old-style parameter is of type 'int', unless
     * is defined within VarList. Implicit conversions are
     * not supported.
     */
    ExprAST *temp;
    s = NULL;
    if (param_type == NULL) {
      s = LookupSym(param_name, Symbol::TABLE_VAR);
      if (s != NULL && s->getBlock() == CurBlock) {
        if (s->getExpr() && s->getExpr()->getNext() &&
            s->getExpr()->getNext()->getNext()) {
          temp = s->getExpr()->getNext()->getNext()->getNext();
          param_type = dyn_cast<ExprExprAST>(s->getExpr()->getNext()
                                               ->getNext())->getExpr();
        }
        else param_type = temp = NULL;
      } else {
        s = NULL;
      }
      if (param_type == NULL)
        param_type = new TagExprAST(TagExprAST::TYPE_INT, NULL);
    }

    /* the type of the parameter was already under control so it can be used */
    param_size = dyn_cast<TagExprAST>(param_type)->TypeSize();
    if (param_size > VM_STACK_ALIGN)
      Error("Unmanaged type for a parameter to a function");
      
#if VM_LITTLE_ENDIAN==0
    param_offset = stack_offset - param_size;
#else
    param_offset = stack_offset - VM_STACK_ALIGN;
#endif
    stack_offset = stack_offset - VM_STACK_ALIGN;

    /* creation or modification of the variable */
    if (s == NULL) {
      AddSymbol(CreateSymbol(&param_name, Symbol::TABLE_VAR,
        new TagExprAST(TagExprAST::SYM_VAR,
            new TagExprAST(param_storage,
                new ExprExprAST(param_type,
                    new ExprExprAST(new TagExprAST(TagExprAST::VAR_STACK,
                        new IntExprAST(param_offset, NULL)), NULL))))));
    } else {
      if (s->getExpr() && s->getExpr()->getNext() &&
          s->getExpr()->getNext()->getNext())
       temp = s->getExpr()->getNext()->getNext();
      else temp = NULL;
      if (temp && isa<ExprExprAST>(temp))
        dyn_cast<ExprExprAST>(temp)
          ->setExpr(new TagExprAST(TagExprAST::VAR_STACK,
                        new IntExprAST(param_offset, NULL)));
      else Error("'put_list': type incorrect");
    }
  }
    
  /* looking whether not remaining are not defined in the entity
   * of the function when it is a statement in the old style
   */
  s = CurBlock->getFirstSym();
  while (s != NULL) {
    ExprAST *e;
    if (s->getExpr() && isa<TagExprAST>(s->getExpr()) &&
        dyn_cast<TagExprAST>(s->getExpr())->getTag() == TagExprAST::SYM_VAR) {
      if (s->getExpr()->getNext() && s->getExpr()->getNext()->getNext() &&
          isa<ExprExprAST>(s->getExpr()->getNext()->getNext()->getNext()))
        e = dyn_cast<ExprExprAST>(s->getExpr()->getNext()
                                   ->getNext()->getNext())->getExpr();
      if (l == NULL)
        Error("No corresponding parameter as such declaration of '%s'",
              s->getName().c_str());
    }
    s = s->getNext();
  }
    
  delete func;
}
  
ExprAST *MMPUCompiler::Type_VarList(ExprAST *Spec, ExprAST *Decls) {
  ExprAST *name, *decl_list, *decl, *vardef, *vardef_list, *type, *init;
  TagExprAST::TagTy spec_storage, spec_type;
  Symbol *spec_sym;
    
  spec_type = Spec_Parse(spec_storage, spec_sym, Spec);
  vardef_list = NULL;
  decl_list = Decls;
    
  while (decl_list != NULL) {
    if (isa<ExprExprAST>(decl_list))
      decl = dyn_cast<ExprExprAST>(decl_list)->getExpr();
    if (decl && isa<ExprExprAST>(decl)) {
      name = dyn_cast<ExprExprAST>(decl)->getExpr()->Dup();
      decl = decl->getNext();
    }
    decl_list = decl_list->getNext();
    if (decl_list && isa<ExprExprAST>(decl_list)) {
      init = dyn_cast<ExprExprAST>(decl_list)->getExpr();
      if (init) init = init->Dup();
      decl_list = decl_list->getNext();
    }
    switch (spec_type) {
    case TagExprAST::TYPE_STRUCT:
    case TagExprAST::TYPE_UNION:
    case TagExprAST::TYPE_ENUM:
      type = ExprAST::append(decl ? decl->Dup() : NULL,
                             new TagExprAST(spec_type,
                                            new SymExprAST(spec_sym, NULL)));
    break;

    case TagExprAST::TYPE_TYPEDEF_IDENT:
      type = ExprAST::append(decl ? decl->Dup() : NULL,
                             spec_sym->getExpr()->getNext()->Dup());
      break;
    default:
      type = ExprAST::append(decl ? decl->Dup() : NULL,
                             new TagExprAST(spec_type, NULL));
    }
			
    vardef =new ExprExprAST(name,
                new TagExprAST(spec_storage,
                    new ExprExprAST(type,
                        new ExprExprAST(init, NULL))));
			
    vardef_list = ExprAST::append(vardef_list, vardef);
  }
  delete Spec;
  delete Decls;
  return vardef_list;
}
  
TagExprAST::TagTy MMPUCompiler::Spec_Parse(TagExprAST::TagTy &Storage,
                                           Symbol* &Sym, ExprAST *Spec) {
  TagExprAST::TagTy a, spec_storage;
  TagExprAST::TagTy spec_type, spec_sign;
  
  spec_sign = TagExprAST::INVALIDTAG;
  spec_type = TagExprAST::INVALIDTAG;
  spec_storage = TagExprAST::STORAGE_DEFAULT;
  
  while (Spec != NULL) {
    TagExprAST *tag = dyn_cast<TagExprAST>(Spec);
    if (tag) a = tag->getTag();
    switch(a) {
    case TagExprAST::QUALIF_CONST:
    case TagExprAST::QUALIF_VOLATILE:
      break;
        
    case TagExprAST::TYPE_SIGNED:
    case TagExprAST::TYPE_UNSIGNED:
      if (spec_sign == TagExprAST::INVALIDTAG) {
        spec_sign = a;
      } else if (spec_sign != a)
        Error("incompatible between 'unsigned' and 'signed'");
      break;
        
    case TagExprAST::TYPE_VOID:
    case TagExprAST::TYPE_CHAR:
    case TagExprAST::TYPE_SHORT:
    case TagExprAST::TYPE_INT:
      if ((spec_type == TagExprAST::TYPE_INT &&
           a == TagExprAST::TYPE_SHORT) ||
          (spec_type == TagExprAST::TYPE_SHORT &&
           a == TagExprAST::TYPE_INT)) {
        spec_type = TagExprAST::TYPE_SHORT;
      } else if (spec_type != TagExprAST::INVALIDTAG) {
        Error("Too many type specifiers");
      } else {
        spec_type = a;
      }
      break;
        
    case TagExprAST::TYPE_TYPEDEF_IDENT:
      Spec = Spec->getNext();
      if (spec_type != TagExprAST::INVALIDTAG)
        Error("Too many type specifiers");
      spec_type = TagExprAST::TYPE_TYPEDEF_IDENT;
      if (isa<SymExprAST>(Spec))
        Sym = dyn_cast<SymExprAST>(Spec)->getSymbol();
      break;
        
    case TagExprAST::TYPE_STRUCT:
    case TagExprAST::TYPE_UNION:
    case TagExprAST::TYPE_ENUM:
      Spec = Spec->getNext();
      if (Spec && isa<SymExprAST>(Spec))
        Sym = dyn_cast<SymExprAST>(Spec)->getSymbol();
      if (spec_type != TagExprAST::INVALIDTAG)
        Error("Too many type specifiers");
      spec_type = a;
      break;

    case TagExprAST::STORAGE_AUTO:
    case TagExprAST::STORAGE_REGISTER:
    case TagExprAST::STORAGE_STATIC:
    case TagExprAST::STORAGE_EXTERN:
    case TagExprAST::STORAGE_TYPEDEF:

    case TagExprAST::STORAGE_DM:
    case TagExprAST::STORAGE_MREG:
    case TagExprAST::STORAGE_TREG:
    case TagExprAST::STORAGE_KREG:
    case TagExprAST::STORAGE_MACREG:
      if (spec_storage != TagExprAST::STORAGE_DEFAULT)
        Error("More than one type specifier defined storage");
      spec_storage = a;
      break;
        
    default:
      Error("Unimplemented specifier");
    }
    if (Spec) Spec = Spec->getNext();
  }
  switch(spec_type) {
  case TagExprAST::TYPE_CHAR:
    if (spec_sign == TagExprAST::TYPE_UNSIGNED)
      spec_type = TagExprAST::TYPE_UCHAR;
    break;
  case TagExprAST::TYPE_SHORT:
    if (spec_sign == TagExprAST::TYPE_UNSIGNED)
      spec_type = TagExprAST::TYPE_USHORT;
    break;
  case TagExprAST::INVALIDTAG:
  case TagExprAST::TYPE_INT:
    if (spec_sign == TagExprAST::TYPE_UNSIGNED)
      spec_type = TagExprAST::TYPE_UINT;
    else spec_type = TagExprAST::TYPE_INT;
    break;
		
  default:
    if (spec_sign != TagExprAST::INVALIDTAG)
      Error("Ineffective 'unsigned' or 'signed' here");
    break;
  }
  
  Storage = spec_storage;
  return spec_type;
}
  
  
/*
 * Global declaration of a variable or a function
 *
 * 'VarType' is not duplicated
 */
void MMPUCompiler::Global_Declare(std::string &VarName,
                                  TagExprAST::TagTy VarStorage,
                                  ExprAST *VarType) {
  TagExprAST::TagTy tag = TagExprAST::INVALIDTAG;
  TagExprAST::TagTy var1_storage = TagExprAST::INVALIDTAG;
  ExprAST *var1_type = NULL;
  Symbol *s = NULL;
  int64_t type_size = -1;
  
  if (VarType && isa<TagExprAST>(VarType)) {
    type_size = dyn_cast<TagExprAST>(VarType)->TypeSize();
  }
    
  s = LookupSym(VarName, Symbol::TABLE_VAR);
    
  if (s != NULL) {
    /* check if the variable is already defined */
    if (isa<TagExprAST>(s->getExpr()))
      tag = dyn_cast<TagExprAST>(s->getExpr())->getTag();
    if (tag != TagExprAST::SYM_VAR)
      Error("Identifier '%s' redefined", VarName.c_str());
    
    if (isa<TagExprAST>(s->getExpr()->getNext()))
      var1_storage = dyn_cast<TagExprAST>(s->getExpr()->getNext())->getTag();
		if (isa<ExprExprAST>(s->getExpr()->getNext()->getNext()))
      var1_type =
        dyn_cast<ExprExprAST>(s->getExpr()->getNext()->getNext())->getExpr();
    
    if (var1_storage != TagExprAST::STORAGE_EXTERN &&
        VarStorage != TagExprAST::STORAGE_EXTERN)
      Error("Redefinition of the variable or function '%s'", VarName.c_str());
    if (*dyn_cast<TagExprAST>(VarType) == *dyn_cast<TagExprAST>(var1_type))
      Error("Redefinition of the variable or function \
            '% s' with the wrong type", VarName.c_str());
    if (var1_storage == TagExprAST::STORAGE_EXTERN &&
        VarStorage != TagExprAST::STORAGE_EXTERN)
      delete s->getExpr();
  } else {
			/* variable definition */
    s = CreateSymbol(&VarName, Symbol::TABLE_VAR, NULL);
    AddSymbol(s);
    if (type_size != -1)
      *asmout << "@" << VarName << " = alloca i" << type_size * 8 <<"\n";
  }
  
  s->setExpr(new TagExprAST(TagExprAST::SYM_VAR,
                 new TagExprAST(VarStorage,
                     new ExprExprAST(VarType,
                         new ExprExprAST(new TagExprAST(TagExprAST::VAR_DATA,
                                                        NULL), NULL)))));
}
  
void MMPUCompiler::Func_End(void) {
  //Gen_LI(li_i, 0, NULL);
  //Gen_Code(rts);
  *asmout << "}\n";
  LocalVarSize = (LocalVarSize + VM_STACK_ALIGN - 1) & ~(VM_STACK_ALIGN - 1);
  //*asmout << " .equ " << LocalVarSym->getName() << ","
  //        << LocalVarSize - VM_LOCAL_START << "\n";
  *asmout << "; finish function definition \n";

  Block_Leave();
  DeclBlock = CurBlock;
}
  
/*
 * Declaring an array of 'type' size 'e'. If the e == NULL
 * Size is undefined.
 */
  
ExprAST *MMPUCompiler::Type_ArrayDeclare(ExprAST *Type, ExprAST *Expr) {
  TagExprAST::TagTy tag = TagExprAST::INVALIDTAG, t = TagExprAST::INVALIDTAG;
  uint64_t size = 0;
    
  if (Expr == NULL) {
    size = -1ULL;
  } else {
    if (isa<ExprExprAST>(Expr) &&
        isa<TagExprAST>((dyn_cast<ExprExprAST>(Expr)->getExpr())))
      t = dyn_cast<TagExprAST>((dyn_cast<ExprExprAST>(Expr)->getExpr()))->getTag();
    if (Expr->getNext() && isa<TagExprAST>(Expr->getNext()))
			tag = dyn_cast<TagExprAST>(Expr->getNext())->getTag();
    if (tag != TagExprAST::EXPR_INT)
      Error("The size of an array must be an integer constant");
    if (Expr->getNext() && isa<IntExprAST>(Expr->getNext()->getNext()))
      size = dyn_cast<IntExprAST>(Expr->getNext()->getNext())->getVal();
    if (size <= 0)
      Error("The size of a table should lie strictly positive");
  }
  return new TagExprAST(TagExprAST::TYPE_ARRAY, new IntExprAST(size, Type));
}
  
/*
 * Declaring a function to format 'FuncType' with parameters
 * 'VarList'.
 * The array arguments and function type are converted to pointers,
 */
ExprAST *MMPUCompiler::Type_FuncDeclare(TagExprAST::TagTy FuncType,
                                        ExprAST *VarList) {
  ExprAST *var_name, *var_type, *vars, *var_list1;
  TagExprAST::TagTy var_storage;
    
  if (FuncType == TagExprAST::FUNC_OLD) {
    return new TagExprAST(TagExprAST::TYPE_FUNC,
                          new TagExprAST(TagExprAST::FUNC_OLD,
                                         new ExprExprAST(VarList, NULL)));
  }
    
  /* test function without parameters */
  if (VarList && VarList->getNext() &&
      isa<ExprExprAST>(VarList->getNext()->getNext()))
    var_type = dyn_cast<ExprExprAST>(VarList->getNext()->getNext())->getExpr();
  if (var_type && isa<TagExprAST>(var_type) &&
      dyn_cast<TagExprAST>(var_type)->getTag() == TagExprAST::TYPE_VOID) {
    if (FuncType == TagExprAST::FUNC_ELLIPSIS)
      Error("A parameter must be causing this before '...'");
    if (VarList && VarList->getNext() && VarList->getNext()->getNext() &&
        VarList->getNext()->getNext()->getNext() &&
        VarList->getNext()->getNext()->getNext()->getNext())
      Error("Function definition without correct parameter");
    return new TagExprAST(TagExprAST::TYPE_FUNC,
                          new TagExprAST(TagExprAST::FUNC_NEW,
                                         new ExprExprAST(NULL, NULL)));
  }
    
  vars = VarList;
  var_list1 = NULL;
  while (vars != NULL) {
    if (isa<ExprExprAST>(vars))
      var_name = dyn_cast<ExprExprAST>(vars)->getExpr()->Dup();
    if ((vars = vars->getNext()) && isa<TagExprAST>(vars))
      var_storage = dyn_cast<TagExprAST>(vars)->getTag();
    if (vars && (vars = vars->getNext()) && isa<ExprExprAST>(vars))
      var_type = dyn_cast<ExprExprAST>(vars)->getExpr();
    if (vars && (vars = vars->getNext())) vars = vars->getNext();
			
    if (var_storage != TagExprAST::STORAGE_DEFAULT &&
        var_storage != TagExprAST::STORAGE_REGISTER)
      Error("Storage class is prohibited on a function parameter");
			
    /* convert arrays to pointers and functions
       pointers to functions */
      
    if (var_type && isa<TagExprAST>(var_type) &&
        dyn_cast<TagExprAST>(var_type)->getTag() == TagExprAST::TYPE_ARRAY) {
      if (var_type->getNext() && var_type->getNext()->getNext())
        var_type = new TagExprAST(TagExprAST::TYPE_POINTER,
                                  var_type->getNext()->getNext()->Dup());
    } else if (var_type && isa<TagExprAST>(var_type) &&
               dyn_cast<TagExprAST>(var_type)->getTag() ==
               TagExprAST::TYPE_FUNC) {
        var_type = new TagExprAST(TagExprAST::TYPE_POINTER, var_type->Dup());
    } else {
      var_type = var_type->Dup();
    }
#if 0
    if (Type_Size(var_type) == -1)
      Error("Parameter of unknown size");
#endif
    var_list1 = ExprAST::append(var_list1,
                  new ExprExprAST(var_name,
                      new TagExprAST(var_storage,
                          new ExprExprAST(var_type,
                              new ExprExprAST(NULL, NULL)))));
  }
  delete VarList;
  return new TagExprAST(TagExprAST::TYPE_FUNC,
                        new TagExprAST(FuncType,
                                       new ExprExprAST(var_list1, NULL)));
}
  
/*
 * It propagates constants and deletes the original expression
 */
  
ExprAST *MMPUCompiler::ConstEval(ExprAST *Expr) {
  ExprAST *res;
  /*	 printf("avant=");  List_Print(expr); printf("\n");  */
  res = Expr->CEval();
  delete Expr;
  /*   printf("aprËs=");  List_Print(res); printf("\n"); */
    
  return res;
}
  
/*
 * Allocate and enter a new Block object
 */
void MMPUCompiler::Block_Enter(Block::BlockTy Type) {
  Block *b;
  b = new Block(Type, CurBlock);
  if (CurBlock)
    CurBlock->addChild(b);
  CurBlock = b;
}
  
/*
 * Leave a Block, clear and release any symbols associated
 * with it.
 */
void MMPUCompiler::Block_Leave(void) {
  Symbol *s;
    
  s = CurBlock->getFirstSym();
  while (s != NULL) {
    Sym_Table.erase(s->getName());
    s = s->getNext();
  }
  CurBlock = CurBlock->getParent();
}

void MMPUCompiler::BeginCompoundStmts(void) {
  Block *b;
  Block_Enter(Block::BLOCK_DECL);
  DeclBlock = CurBlock;
  b = CurBlock;
  do {
    b = b->getParent();
    if (b == NULL) Error("Superior declaration block not found");
  } while (b->getType() != Block::BLOCK_DECL);
  DeclBlock->local_var_offset = b->local_var_offset;
}

void MMPUCompiler::EndCompoundStmts() {
  Block *b;
  Block_Leave();
  b = CurBlock;
  while (b->getType() != Block::BLOCK_DECL) {
    b = b->getParent();
    if (b == NULL) Error("Superior declaration block not found");
  }
  DeclBlock = b;
}

} // namespace MMPU
