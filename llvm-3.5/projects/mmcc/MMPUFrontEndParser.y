%{
#include <string>
#include <bitset>
%}

//%output    "MMPULiteGenInstrParser.bison"
%defines   "MMPUGenFrontEndParser.h"
%define api.pure
%name-prefix="mmpucompiler"

%parse-param { MMPUCompiler *mc }
%code {
#define YYLEX_PARAM mc
}
/*
%union {
  std::string *string;
  uint64_t val;
  int token;
  double fval;
  ExprAST* AST;
  //FunctionAST* FAST;
  //PrototypeAST* PAST;
  std::vector<ExprAST *> * ASTVec;
  std::pair<std::string, ExprAST*> * Var;
  std::vector<std::pair<std::string, ExprAST*> > * VarVec;
  std::vector<std::string> * StrVec;
}
*/
%token  IDENTIFIER I_CONSTANT F_CONSTANT STRING_LITERAL FUNC_NAME SIZEOF
%token  PTR_OP INC_OP DEC_OP LEFT_OP RIGHT_OP LE_OP GE_OP EQ_OP NE_OP
%token  AND_OP OR_OP MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN ADD_ASSIGN
%token  SUB_ASSIGN LEFT_ASSIGN RIGHT_ASSIGN AND_ASSIGN
%token  XOR_ASSIGN OR_ASSIGN
%token  TYPEDEF_NAME ENUMERATION_CONSTANT

%token  TYPEDEF EXTERN STATIC AUTO REGISTER INLINE
%token  CONST RESTRICT VOLATILE
%token  BOOL CHAR SHORT INT LONG SIGNED UNSIGNED FLOAT DOUBLE VOID
%token  COMPLEX IMAGINARY 
%token  STRUCT UNION ENUM ELLIPSIS

%token  CASE DEFAULT IF ELSE SWITCH WHILE DO FOR GOTO CONTINUE BREAK RETURN

%token  ALIGNAS ALIGNOF ATOMIC GENERIC NORETURN STATIC_ASSERT THREAD_LOCAL

%token  APDL DM MREG TREG KREG MACREG
%token  SADD SSUB
%start translation_unit
%%

primary_expression
  : IDENTIFIER { $$ = ExprAST::CreateIdent(dyn_cast<VariableExprAST>($1)->getName(), mc); delete $1; }
  | constant { $$ = $1; }
/*  | string */
  | '(' expression ')' { $$ = $2; }
/*  | generic_selection */
  ;

constant
  : I_CONSTANT  { $$ = new ExprExprAST(new TagExprAST(TagExprAST::TYPE_INT, NULL), new TagExprAST(TagExprAST::EXPR_INT, $1)); }  /* includes character_constant */
  | F_CONSTANT
/*  | ENUMERATION_CONSTANT  /* after it has been defined as such */
  ;

enumeration_constant    /* before it has been defined as such */
  : IDENTIFIER 
  ;

string
  : STRING_LITERAL
  | FUNC_NAME
  ;

generic_selection
  : GENERIC '(' assignment_expression ',' generic_assoc_list ')'
  ;

generic_assoc_list
  : generic_association
  | generic_assoc_list ',' generic_association
  ;

generic_association
  : type_name ':' assignment_expression
  | DEFAULT ':' assignment_expression
  ;

postfix_expression
  : primary_expression { $$ = $1; }
  | postfix_expression '[' expression ']' {
    if ($1 && $1->getNext() && isa<TagExprAST>($1->getNext()) &&
        dyn_cast<TagExprAST>($1->getNext())->getTag() == TagExprAST::EXPR_IDENT &&
        $1->getNext()->getNext() && isa<SymExprAST>($1->getNext()->getNext()) &&
        dyn_cast<SymExprAST>($1->getNext()->getNext())->getSymbol()->getStorage() == TagExprAST::STORAGE_TREG) {
      $$ = ExprAST::append($1, $3);
    } else
      $$ = ExprAST::CreateIndex($1, $3);
  }
  | postfix_expression '(' ')' { $$ = ExprAST::CreateCall($1, NULL); }
  | postfix_expression '(' argument_expression_list ')' { $$ = ExprAST::CreateCall($1, $3); }
  | postfix_expression '.' IDENTIFIER { $$ = ExprAST::CreateField($1, dyn_cast<VariableExprAST>($3)->getName(), mc); delete $3; }
  | postfix_expression PTR_OP IDENTIFIER { $$ = ExprAST::CreateField(ExprAST::CreateIndir($1), dyn_cast<VariableExprAST>($3)->getName(), mc); delete $3; }
  | postfix_expression INC_OP {
    if ($1 && $1->getNext() && isa<TagExprAST>($1->getNext()) &&
        dyn_cast<TagExprAST>($1->getNext())->getTag() == TagExprAST::EXPR_IDENT &&
        $1->getNext()->getNext() && isa<SymExprAST>($1->getNext()->getNext()) &&
        (dyn_cast<SymExprAST>($1->getNext()->getNext())->getSymbol()->getStorage() == TagExprAST::STORAGE_DM ||
         dyn_cast<SymExprAST>($1->getNext()->getNext())->getSymbol()->getStorage() == TagExprAST::STORAGE_MREG)) {
      dyn_cast<TagExprAST>($1->getNext())->setTag(TagExprAST::EXPR_APDLINC);
      $$ = $1;
    } else if ($1 && $1->getNext() && isa<TagExprAST>($1->getNext()) &&
               dyn_cast<TagExprAST>($1->getNext())->getTag() == TagExprAST::EXPR_ASSIGN) {
      ExprAST *Ident = NULL;
      if ($1->getNext()->getNext() && isa<ExprExprAST>($1->getNext()->getNext()))
        Ident = dyn_cast<ExprExprAST>($1->getNext()->getNext())->getExpr();
      dyn_cast<TagExprAST>(Ident->getNext())->setTag(TagExprAST::EXPR_APDLINC);
    } else
      $$ = ExprAST::CreateBinary(TagExprAST::EXPR_SUB,
                                 ExprAST::CreateAssignOp(TagExprAST::EXPR_ADD, $1,
                                                         ExprAST::CreateConstInt(1), mc),
                                 ExprAST::CreateConstInt(1));
  }
  | postfix_expression DEC_OP
  | '(' type_name ')' '{' initializer_list '}'
  | '(' type_name ')' '{' initializer_list ',' '}'
  ;

argument_expression_list
  : assignment_expression { $$ = new ExprExprAST($1, NULL); }
  | argument_expression_list ',' assignment_expression { $$ = ExprAST::append($1, new ExprExprAST($3, NULL)); }
  ;

unary_expression
  : postfix_expression { $$ = $1; }
  | INC_OP unary_expression { $$ = ExprAST::CreateAssignOp(TagExprAST::EXPR_ADD, $2, ExprAST::CreateConstInt(1), mc); }
  | DEC_OP unary_expression { $$ = ExprAST::CreateAssignOp(TagExprAST::EXPR_SUB, $2, ExprAST::CreateConstInt(1), mc); }
  | unary_operator cast_expression {
  TagExprAST::TagTy tag;
  if (isa<TagExprAST>($1)) {
    tag = dyn_cast<TagExprAST>($1)->getTag();
    switch (tag) {
    case TagExprAST::EXPR_AND: $$ = ExprAST::CreateAddr($2); break;
    case TagExprAST::EXPR_MUL: $$ = ExprAST::CreateIndir($2); break;
    default: $$ = ExprAST::CreateUnary(tag, $2); break;
    }
  }
  delete $1;
}
  | SIZEOF unary_expression { $$ = ExprAST::CreateSizeof($2, 1); }
  | SIZEOF '(' type_name ')' { $$ = ExprAST::CreateSizeof($3, 0); }
  | ALIGNOF '(' type_name ')'
  ;

unary_operator
  : '&' { $$ = new TagExprAST(TagExprAST::EXPR_AND, NULL); }
  | '*' { $$ = new TagExprAST(TagExprAST::EXPR_MUL, NULL); }
  | '+' { $$ = new TagExprAST(TagExprAST::EXPR_ADD, NULL); }
  | '-' { $$ = new TagExprAST(TagExprAST::EXPR_SUB, NULL); }
  | '~' { $$ = new TagExprAST(TagExprAST::EXPR_NOT, NULL); }
  | '!' { $$ = new TagExprAST(TagExprAST::EXPR_LNOT, NULL); }
  ;

cast_expression
  : unary_expression { $$ = $1; }
  | '(' type_name ')' cast_expression { $$ = ExprAST::CreateCast($2, $4); }
  ;

multiplicative_expression
  : cast_expression { $$ = $1; }
  | multiplicative_expression '*' cast_expression { $$ = ExprAST::CreateBinary(TagExprAST::EXPR_MUL, $1, $3); }
  | multiplicative_expression '/' cast_expression { $$ = ExprAST::CreateBinary(TagExprAST::EXPR_DIV, $1, $3); }
  | multiplicative_expression '%' cast_expression { $$ = ExprAST::CreateBinary(TagExprAST::EXPR_MOD, $1, $3); }
  ;

additive_expression
  : multiplicative_expression { $$ = $1; }
  | additive_expression SADD multiplicative_expression { $$ = ExprAST::CreateBinary(TagExprAST::EXPR_SADD, $1, $3); }
  | additive_expression '+' multiplicative_expression { $$ = ExprAST::CreateBinary(TagExprAST::EXPR_ADD, $1, $3); }
  | additive_expression SSUB multiplicative_expression { $$ = ExprAST::CreateBinary(TagExprAST::EXPR_SSUB, $1, $3); }
  | additive_expression '-' multiplicative_expression { $$ = ExprAST::CreateBinary(TagExprAST::EXPR_SUB, $1, $3); }
  ;

shift_expression
  : additive_expression { $$ = $1; }
  | shift_expression LEFT_OP additive_expression { $$ = ExprAST::CreateBinary(TagExprAST::EXPR_SHL, $1, $3); }
  | shift_expression RIGHT_OP additive_expression { $$ = ExprAST::CreateBinary(TagExprAST::EXPR_SHR, $1, $3); }
  ;

relational_expression
  : shift_expression { $$ = $1; }
  | relational_expression '<' shift_expression { $$ = ExprAST::CreateBinary(TagExprAST::EXPR_LT, $1, $3); }
  | relational_expression '>' shift_expression { $$ = ExprAST::CreateBinary(TagExprAST::EXPR_GT, $1, $3); }
  | relational_expression LE_OP shift_expression { $$ = ExprAST::CreateBinary(TagExprAST::EXPR_LE, $1, $3); }
  | relational_expression GE_OP shift_expression { $$ = ExprAST::CreateBinary(TagExprAST::EXPR_GE, $1, $3); }
  ;

equality_expression
  : relational_expression { $$ = $1; }
  | equality_expression EQ_OP relational_expression { $$ = ExprAST::CreateBinary(TagExprAST::EXPR_EQ, $1, $3); }
  | equality_expression NE_OP relational_expression { $$ = ExprAST::CreateBinary(TagExprAST::EXPR_NE, $1, $3); }
  ;

and_expression
  : equality_expression { $$ = $1; }
  | and_expression '&' equality_expression { $$ = ExprAST::CreateBinary(TagExprAST::EXPR_AND, $1, $3); }
  ;

exclusive_or_expression
  : and_expression { $$ = $1; }
  | exclusive_or_expression '^' and_expression { $$ = ExprAST::CreateBinary(TagExprAST::EXPR_XOR, $1, $3); }
  ;

inclusive_or_expression
  : exclusive_or_expression { $$ = $1; }
  | inclusive_or_expression '|' exclusive_or_expression { $$ = ExprAST::CreateBinary(TagExprAST::EXPR_OR, $1, $3); }
  ;

logical_and_expression
  : inclusive_or_expression { $$ = $1; }
  | logical_and_expression AND_OP inclusive_or_expression { $$ = ExprAST::CreateBinary(TagExprAST::EXPR_LAND, $1, $3); }
  ;

logical_or_expression
  : logical_and_expression { $$ = $1; }
  | logical_or_expression OR_OP logical_and_expression { $$ = ExprAST::CreateBinary(TagExprAST::EXPR_LOR, $1, $3); }
  ;

conditional_expression
  : logical_or_expression { $$ = $1; }
  | logical_or_expression '?' expression ':' conditional_expression { $$ = ExprAST::CreateCond($1, $3, $5); }
  ;

assignment_expression
  : conditional_expression { $$ = $1; }
  | unary_expression assignment_operator assignment_expression {
  if ($2 == NULL)
    $$ = ExprAST::CreateAssign($1, $3);
  else {
    $$ = ExprAST::CreateAssignOp(dyn_cast<TagExprAST>($2)->getTag(), $1, $3, mc);
    delete $2;
  }
}
  ;

assignment_operator
  : '=' { $$ = NULL; }
  | MUL_ASSIGN { $$ = new TagExprAST(TagExprAST::EXPR_MUL, NULL); }
  | DIV_ASSIGN { $$ = new TagExprAST(TagExprAST::EXPR_DIV, NULL); }
  | MOD_ASSIGN { $$ = new TagExprAST(TagExprAST::EXPR_MOD, NULL); }
  | ADD_ASSIGN { $$ = new TagExprAST(TagExprAST::EXPR_ADD, NULL); }
  | SUB_ASSIGN { $$ = new TagExprAST(TagExprAST::EXPR_SUB, NULL); }
  | LEFT_ASSIGN { $$ = new TagExprAST(TagExprAST::EXPR_SHL, NULL); }
  | RIGHT_ASSIGN { $$ = new TagExprAST(TagExprAST::EXPR_SHR, NULL); }
  | AND_ASSIGN { $$ = new TagExprAST(TagExprAST::EXPR_AND, NULL); }
  | XOR_ASSIGN { $$ = new TagExprAST(TagExprAST::EXPR_XOR, NULL); }
  | OR_ASSIGN { $$ = new TagExprAST(TagExprAST::EXPR_OR, NULL); }
  ;

expression
  : expression_ref { $$ = ExprAST::CreateList($1); }
  ;

expression_ref
  : assignment_expression { $$ = new ExprExprAST($1, NULL); }
  | expression ',' assignment_expression { $$ = ExprAST::append($1, new ExprExprAST($3, NULL)); }
  ;

constant_expression
  : conditional_expression { $$ = mc->ConstEval($1); } /* with constraints */
  ;

declaration
  : declaration_specifiers ';' { delete $1; }
  | declaration_specifiers init_declarator_list ';' {
  ExprAST *vars;
  vars = mc->Type_VarList($1, $2);
  mc->Var_Declare(vars);
  delete vars;
}
/*  | static_assert_declaration*/
  ;

declaration_specifiers
  : storage_class_specifier declaration_specifiers { $$ = ExprAST::append($1, $2); }
  | storage_class_specifier { $$ = $1; }
  | type_specifier declaration_specifiers { $$ = ExprAST::append($1, $2); }
  | type_specifier { $$ = $1; }
  | type_qualifier declaration_specifiers { $$ = ExprAST::append($1, $2); }
  | type_qualifier { $$ = $1; }
  | function_specifier declaration_specifiers { $$ = ExprAST::append($1, $2); }
  | function_specifier { $$ = $1; }
/*  | alignment_specifier declaration_specifiers
  | alignment_specifier*/
  ;

init_declarator_list
  : init_declarator { $$ = $1; }
  | init_declarator_list ',' init_declarator { $$ = ExprAST::append($1, $3); }
  ;

init_declarator
  : declarator '=' initializer { $$ = new ExprExprAST($1, new ExprExprAST($3, NULL)); }
  | declarator { $$ = new ExprExprAST($1, new ExprExprAST(NULL, NULL)); }
  ;

storage_class_specifier
  : TYPEDEF { $$ = new TagExprAST(TagExprAST::STORAGE_TYPEDEF, NULL); }  /* identifiers must be flagged as TYPEDEF_NAME */
  | EXTERN { $$ = new TagExprAST(TagExprAST::STORAGE_EXTERN, NULL); }
  | STATIC { $$ = new TagExprAST(TagExprAST::STORAGE_STATIC, NULL); }
/*  | THREAD_LOCAL { $$ = new TagExprAST(TagExprAST::STORAGE_AUTO, NULL); }*/
  | AUTO { $$ = new TagExprAST(TagExprAST::STORAGE_AUTO, NULL); }
  | REGISTER { $$ = new TagExprAST(TagExprAST::STORAGE_REGISTER, NULL); }
  ;

type_specifier
  : VOID { $$ = new TagExprAST(TagExprAST::TYPE_VOID, NULL); }
  | CHAR { $$ = new TagExprAST(TagExprAST::TYPE_CHAR, NULL); }
  | SHORT { $$ = new TagExprAST(TagExprAST::TYPE_SHORT, NULL); }
  | INT { $$ = new TagExprAST(TagExprAST::TYPE_INT, NULL); }
  | SIGNED { $$ = new TagExprAST(TagExprAST::TYPE_SIGNED, NULL); }
  | UNSIGNED { $$ = new TagExprAST(TagExprAST::TYPE_UNSIGNED, NULL); }
/*  | LONG { $$ = new TagExprAST(TagExprAST::TYPE_SIGNED, NULL); }
  | FLOAT { $$ = new TagExprAST(TagExprAST::TYPE_VOID, NULL); }
  | DOUBLE { $$ = new TagExprAST(TagExprAST::TYPE_VOID, NULL); }
  | BOOL { $$ = new TagExprAST(TagExprAST::TYPE_VOID, NULL); }
  | COMPLEX
  | IMAGINARY    
  | atomic_type_specifier */
  | struct_or_union_specifier { $$ = $1; }
  | enum_specifier { $$ = $1; }
  | TYPEDEF_NAME { $$ = new TagExprAST(TagExprAST::TYPE_TYPEDEF_IDENT, $1); }    /* after it has been defined as such */
  ;

struct_or_union_specifier
  : struct_or_union '{' struct_declaration_list '}'
  | struct_or_union IDENTIFIER '{' struct_declaration_list '}'
  | struct_or_union IDENTIFIER
  ;

struct_or_union
  : STRUCT { $$ = new TagExprAST(TagExprAST::TYPE_STRUCT, NULL); }
  | UNION { $$ = new TagExprAST(TagExprAST::TYPE_UNION, NULL); }
  ;

struct_declaration_list
  : struct_declaration { $$ = $1; }
  | struct_declaration_list struct_declaration { $$ = ExprAST::append($1, $2); }
  ;

struct_declaration
  : /*specifier_qualifier_list ';'*/  /* for anonymous struct/union */
/*  |*/ specifier_qualifier_list struct_declarator_list ';' { $$ = mc->Type_VarList($1, $2); }
/*  | static_assert_declaration*/
  ;

specifier_qualifier_list
  : type_specifier specifier_qualifier_list { $$ = ExprAST::append($1, $2); }
  | type_specifier { $$ = $1; }
  | type_qualifier specifier_qualifier_list { $$ = ExprAST::append($1, $2); }
  | type_qualifier { $$ = $1; }
  ;

struct_declarator_list
  : struct_declarator { $$ = new ExprExprAST($1, new ExprExprAST(NULL, NULL)); }
  | struct_declarator_list ',' struct_declarator { $$ = ExprAST::append($1, new ExprExprAST($3, new ExprExprAST(NULL, NULL))); }
  ;

struct_declarator
  : ':' constant_expression
  | declarator ':' constant_expression
  | declarator
  ;

enum_specifier
  : ENUM '{' enumerator_list '}'
  | ENUM '{' enumerator_list ',' '}'
  | ENUM IDENTIFIER '{' enumerator_list '}'
  | ENUM IDENTIFIER '{' enumerator_list ',' '}'
  | ENUM IDENTIFIER
  ;

enumerator_list
  : enumerator
  | enumerator_list ',' enumerator
  ;

enumerator  /* identifiers must be flagged as ENUMERATION_CONSTANT */
  : enumeration_constant '=' constant_expression
  | enumeration_constant
  ;

atomic_type_specifier
  : ATOMIC '(' type_name ')'
  ;

type_qualifier
  : CONST { $$ = new TagExprAST(TagExprAST::QUALIF_CONST, NULL); }
  | VOLATILE { $$ = new TagExprAST(TagExprAST::QUALIF_VOLATILE, NULL); }
/*  | RESTRICT
  | ATOMIC*/
  ;

function_specifier
  : INLINE
  | NORETURN
  ;

alignment_specifier
  : ALIGNAS '(' type_name ')'
  | ALIGNAS '(' constant_expression ')'
  ;

declarator
  : pointer direct_declarator { $$ = ExprAST::append($2, $1); }
  | direct_declarator
  ;

direct_declarator
  : IDENTIFIER { $$ = new ExprExprAST($1, NULL); }
  | '(' declarator ')' { $$ = $2; }
  | direct_declarator '[' ']' { $$ = ExprAST::append($1, mc->Type_ArrayDeclare(NULL, NULL)); }
  | direct_declarator '[' '*' ']'
  | direct_declarator '[' STATIC type_qualifier_list assignment_expression ']'
  | direct_declarator '[' STATIC assignment_expression ']'
  | direct_declarator '[' type_qualifier_list '*' ']'
  | direct_declarator '[' type_qualifier_list STATIC assignment_expression ']'
  | direct_declarator '[' type_qualifier_list assignment_expression ']'
  | direct_declarator '[' type_qualifier_list ']'
  | direct_declarator '[' assignment_expression ']' { $$ = ExprAST::append($1, mc->Type_ArrayDeclare(NULL, $3)); }
  | direct_declarator '(' parameter_type_list ')' { $$ = ExprAST::append($1, mc->Type_FuncDeclare(TagExprAST::FUNC_NEW, $3)); }
  | direct_declarator '(' ')' { $$ = ExprAST::append($1, mc->Type_FuncDeclare(TagExprAST::FUNC_OLD, NULL)); }
  | direct_declarator '(' identifier_list ')' { $$ = ExprAST::append($1, mc->Type_FuncDeclare(TagExprAST::FUNC_OLD, $3));}
  ;

pointer
  : '*' type_qualifier_list pointer { delete $2; $$ = new TagExprAST(TagExprAST::TYPE_POINTER, $3); }
  | '*' type_qualifier_list { delete $2; $$ = new TagExprAST(TagExprAST::TYPE_POINTER, NULL); }
  | '*' pointer { $$ = new TagExprAST(TagExprAST::TYPE_POINTER, $2); }
  | '*' { $$ = new TagExprAST(TagExprAST::TYPE_POINTER, NULL); }
  ;

type_qualifier_list
  : type_qualifier { $$ = $1; }
  | type_qualifier_list type_qualifier { $$ = ExprAST::append($1, $2); }
  ;


parameter_type_list
  : parameter_list ',' ELLIPSIS
  | parameter_list
  ;

parameter_list
  : parameter_declaration { $$ = $1; }
  | parameter_list ',' parameter_declaration { $$ = ExprAST::append($1, $3); }
  ;

parameter_declaration
  : declaration_specifiers declarator { $$ = mc->Type_VarList($1, new ExprExprAST($2, new ExprExprAST(NULL, NULL))); }
  | declaration_specifiers abstract_declarator { $$ = mc->Type_VarList($1, new ExprExprAST($2, new ExprExprAST(NULL, NULL))); }
  | declaration_specifiers { $$ = mc->Type_VarList($1, new ExprExprAST(new ExprExprAST(NULL, NULL), new ExprExprAST(NULL, NULL))); }
  ;

identifier_list
  : IDENTIFIER { $$ = new ExprExprAST($1, 
                                new TagExprAST(TagExprAST::STORAGE_DEFAULT, 
                                    new ExprExprAST(NULL, 
                                        new ExprExprAST(NULL, NULL)))); }
  | identifier_list ',' IDENTIFIER { $$ = ExprAST::append(new ExprExprAST($1, 
                                                                    new TagExprAST(TagExprAST::STORAGE_DEFAULT, 
                                                                        new ExprExprAST(NULL, 
                                                                            new ExprExprAST(NULL, NULL)))), $3); }
  ;

type_name
  : specifier_qualifier_list abstract_declarator {
  ExprAST *var;
  var = mc->Type_VarList($1, new ExprExprAST($2, new ExprExprAST(NULL, NULL)));
  if (var && var->getNext() && var->getNext()->getNext() &&
      isa<ExprExprAST>(var->getNext()->getNext())) {
    $$ = dyn_cast<ExprExprAST>(var->getNext()->getNext())->getExpr()->Dup();
    delete var;
  }
}
  | specifier_qualifier_list{
  ExprAST *var;
  var = mc->Type_VarList($1, new ExprExprAST(new ExprExprAST(NULL, NULL), new ExprExprAST(NULL, NULL)));
  if (var && var->getNext() && var->getNext()->getNext() &&
      isa<ExprExprAST>(var->getNext()->getNext())) {
    $$ = dyn_cast<ExprExprAST>(var->getNext()->getNext())->getExpr()->Dup();
    delete var;
  }
}  ;

abstract_declarator
  : pointer direct_abstract_declarator { $$ = ExprAST::append($2, $1); }
  | pointer { $$ = new ExprExprAST(NULL, $1); }
  | direct_abstract_declarator { $$ = $1; }
  ;

direct_abstract_declarator
  : '(' abstract_declarator ')' { $$ = $2; }
  | '[' ']' { $$ = new ExprExprAST(NULL, mc->Type_ArrayDeclare(NULL, NULL)); }
/*  | '[' '*' ']'
  | '[' STATIC type_qualifier_list assignment_expression ']'
  | '[' STATIC assignment_expression ']'
  | '[' type_qualifier_list STATIC assignment_expression ']'
  | '[' type_qualifier_list assignment_expression ']'
  | '[' type_qualifier_list ']'*/
  | '[' assignment_expression ']' { $$ = new ExprExprAST(NULL, mc->Type_ArrayDeclare(NULL, $2)); }
  | direct_abstract_declarator '[' ']' { $$ = ExprAST::append($1, mc->Type_ArrayDeclare(NULL, NULL)); }
/*  | direct_abstract_declarator '[' '*' ']'
  | direct_abstract_declarator '[' STATIC type_qualifier_list assignment_expression ']'
  | direct_abstract_declarator '[' STATIC assignment_expression ']'
  | direct_abstract_declarator '[' type_qualifier_list assignment_expression ']'
  | direct_abstract_declarator '[' type_qualifier_list STATIC assignment_expression ']'
  | direct_abstract_declarator '[' type_qualifier_list ']'*/
  | direct_abstract_declarator '[' assignment_expression ']' { $$ = ExprAST::append($1, mc->Type_ArrayDeclare(NULL, $3)); }
  | '(' ')' { $$ = new ExprExprAST(NULL, mc->Type_FuncDeclare(TagExprAST::FUNC_OLD, NULL)); }
  | '(' parameter_type_list ')' { $$ = new ExprExprAST(NULL, mc->Type_FuncDeclare(TagExprAST::FUNC_NEW, $2)); }
  | direct_abstract_declarator '(' ')' { $$ = ExprAST::append($1, mc->Type_FuncDeclare(TagExprAST::FUNC_OLD, NULL)); }
  | direct_abstract_declarator '(' parameter_type_list ')' { $$ = ExprAST::append($1, mc->Type_FuncDeclare(TagExprAST::FUNC_NEW, $3)); }
  ;

initializer
  : '{' initializer_list '}' { $$ = new TagExprAST(TagExprAST::INIT_LIST, $2); }
  | '{' initializer_list ',' '}' { $$ = new TagExprAST(TagExprAST::INIT_LIST, $2); }
  | assignment_expression { $$ = new TagExprAST(TagExprAST::INIT_EXPR, mc->ConstEval($1)); }
  ;

initializer_list
  : /* designation initializer
  | initializer_list ',' designation initializer
  |*/ initializer { $$ = new ExprExprAST($1, NULL); }
  | initializer_list ',' initializer { $$ = ExprAST::append($1, new ExprExprAST($3, NULL)); }
  ;

designation
  : designator_list '='
  ;

designator_list
  : designator
  | designator_list designator
  ;

designator
  : '[' constant_expression ']'
  | '.' IDENTIFIER
  ;

static_assert_declaration
  : STATIC_ASSERT '(' constant_expression ',' STRING_LITERAL ')' ';'
  ;

statement
  : labeled_statement
  | compound_statement
  | expression_statement {
  Block *BAst = new Block(Block::BLOCK_AST, NULL);
  BAst->setAST($1);
  mc->addChild(BAst);
  mc->getIRGen()->GenIR(BAst);
}
  | selection_statement
  | iteration_statement
  | jump_statement
  ;

labeled_statement
  : IDENTIFIER ':' statement
  | CASE constant_expression ':' statement
  | DEFAULT ':' statement
  ;

compound_statement
  : '{' '}'
  | '{' { mc->BeginCompoundStmts(); } block_item_list '}' { mc->EndCompoundStmts(); }
  ;

block_item_list
  : block_item { $$ = new ExprExprAST($1, NULL); }
  | block_item_list block_item { $$ = ExprAST::append($1, new ExprExprAST($2, NULL));}
  ;

block_item
  : declaration
  | statement
  ;

expression_statement
  : ';'
  | expression ';' { $$ = mc->ConstEval($1); }
  ;

selection_statement
  : IF '(' expression ')' {
  int64_t l1, l2;
  /*l1 = mc->CreateNewLabel();
  l2 = mc->CreateNewLabel();*/
  Block *BAst;
  mc->addChild(BAst = new Block(Block::BLOCK_AST, NULL));
  BAst->setAST($3);
  mc->Block_Enter(Block::BLOCK_IF);
  mc->setControlLabels(l1, l2, 0);
}   statement ELSE {} statement { mc->Block_Leave(); }
  /*| IF '(' expression ')' {
  Block *BAst;
  mc->addChild(BAst = new Block(Block::BLOCK_AST, NULL));
  BAst->setAST($3);
  mc->Block_Enter(Block::BLOCK_IF);
}   statement { mc->Block_Leave(); }*/
  | SWITCH '(' expression ')' statement
  ;

iteration_statement
  : WHILE '(' expression ')' statement
  | DO statement WHILE '(' expression ')' ';'
  | FOR '(' expression_statement expression_statement ')' statement
  | FOR '(' expression_statement expression_statement expression ')' {
    Block *BAst = new Block(Block::BLOCK_AST, NULL);
    BAst->setAST($3);
    mc->addChild(BAst);
    BAst = new Block(Block::BLOCK_AST, NULL);
    BAst->setAST($4);
    mc->addChild(BAst);
    mc->Block_Enter(Block::BLOCK_WHILE);
    mc->GenCurBlock();
  } statement {
    Block *BAst = new Block(Block::BLOCK_AST, NULL);
    BAst->setAST($5);
    mc->addChild(BAst);
    mc->Block_Leave();
    mc->getIRGen()->GenIR(BAst);
  }
  | FOR '(' declaration expression_statement ')' statement
  | FOR '(' declaration expression_statement expression ')' {
    Block *BAst = new Block(Block::BLOCK_AST, NULL);
    BAst->setAST($4);
    mc->addChild(BAst);
    mc->Block_Enter(Block::BLOCK_WHILE);
    mc->GenCurBlock();
  } statement {
    Block *BAst = new Block(Block::BLOCK_AST, NULL);
    BAst->setAST($5);
    mc->addChild(BAst);
    mc->Block_Leave();
    mc->getIRGen()->GenIR(BAst);
  }
  ;

jump_statement
  : GOTO IDENTIFIER ';'
  | CONTINUE ';'
  | BREAK ';'
  | RETURN ';' {
  Block *BAst;
  mc->addChild(BAst = new Block(Block::BLOCK_RET, NULL));
  mc->getIRGen()->GenIR(BAst);
}
  | RETURN expression ';' {
  Block *BAst;
  mc->addChild(BAst = new Block(Block::BLOCK_RET, NULL));
  BAst->setAST($2);
  mc->getIRGen()->GenIR(BAst);
}
  ;

translation_unit
  : external_declaration
  | translation_unit external_declaration
  ;

external_declaration
  : function_definition
  | declaration
  ;

function_definition
  : declaration_specifiers declarator declaration_list {
      mc->Func_Declare($1, $2, $3);
    }
    compound_statement {
      mc->Func_End();
    }
  | declaration_specifiers declarator {
      mc->Func_Declare($1, $2, NULL);
    }
    compound_statement {
      mc->Func_End();
    }

declaration_list
  : declaration { $$ = $1; }
  | declaration_list declaration { $$ = ExprAST::append($1, $2); }
  ;

/* rules for APDL */
function_definition
  : apdl_function_specifier declaration_specifiers declarator {
    mc->setDefineAPDL();
    mc->Func_Declare($2, $3, NULL);
  }
  compound_statement {
    mc->Func_End();
    mc->clearDefineAPDL();
  }
  ;

apdl_function_specifier
  : APDL 
  ;

dm_specifier
  : DM { $$ = new TagExprAST(TagExprAST::STORAGE_DM, NULL); }
  ;

treg_specifier
  : TREG { $$ = new TagExprAST(TagExprAST::STORAGE_TREG, NULL); }
  ;

mreg_specifier
  : MREG { $$ = new TagExprAST(TagExprAST::STORAGE_MREG, NULL); }
  ;

kreg_specifier
  : KREG { $$ = new TagExprAST(TagExprAST::STORAGE_KREG, NULL); }
;

mac_specifier
  : MACREG { $$ = new TagExprAST(TagExprAST::STORAGE_MACREG, NULL); }
  ;

config_pair
  : '(' I_CONSTANT ',' I_CONSTANT ')' { $$ = new TagExprAST(TagExprAST::EXPR_PAIR, ExprAST::append($2, $4)); }
  | '(' I_CONSTANT ')' { $$ = new TagExprAST(TagExprAST::EXPR_PAIR, ExprAST::append($2, new IntExprAST(0, NULL))); }
  ;

config_pair_list
  : config_pair_list ',' config_pair { $$ = ExprAST::append($1, new ExprExprAST($3, NULL)); }
  | config_pair { $$ = new ExprExprAST($1, NULL); }
  ;

declaration
  : apdl_declaration
  ;

apdl_declaration
  : dm_specifier declaration_specifiers ';' { delete $1; delete $2; }
  | dm_specifier declaration_specifiers dm_init_declarator_list ';' {
    ExprAST *vars;
    ExprAST::append($1, $2);
    vars = mc->Type_VarList($1, $3);
    mc->Var_Declare(vars);
    delete vars;
  }
  | mreg_specifier declaration_specifiers ';' { delete $1; delete $2; }
  | mreg_specifier declaration_specifiers mreg_init_declarator_list ';' {
    ExprAST *vars;
    ExprAST::append($1, $2);
    vars = mc->Type_VarList($1, $3);
    mc->Var_Declare(vars);
    delete vars;
  }
  | treg_specifier declaration_specifiers ';' { delete $1; delete $2; }
  | treg_specifier declaration_specifiers treg_init_declarator_list ';' {
    ExprAST *vars;
    ExprAST::append($1, $2);
    vars = mc->Type_VarList($1, $3);
    mc->Var_Declare(vars);
    delete vars;
  }
  | kreg_specifier declaration_specifiers ';' { delete $1; delete $2; }
  | kreg_specifier declaration_specifiers kreg_init_declarator_list ';' {
    ExprAST *vars;
    ExprAST::append($1, $2);
    vars = mc->Type_VarList($1, $3);
    mc->Var_Declare(vars);
    delete vars;
  }
  | mac_specifier declaration_specifiers ';' { delete $1; delete $2; }
  | mac_specifier declaration_specifiers mac_init_declarator_list ';' {
    ExprAST *vars;
    ExprAST::append($1, $2);
    vars = mc->Type_VarList($1, $3);
    mc->Var_Declare(vars);
    delete vars;
  }
  ;

dm_init_declarator_list
  : dm_init_declarator { $$ = $1; }
  | dm_init_declarator_list ',' dm_init_declarator { $$ = ExprAST::append($1, $3); }
  ;

dm_init_declarator
  : dm_declarator '=' initializer { $$ = new ExprExprAST($1, new ExprExprAST($3, NULL)); }
  | dm_declarator { $$ = new ExprExprAST($1, new ExprExprAST(NULL, NULL)); }
  ;

dm_declarator
  : IDENTIFIER '[' config_pair_list ']' { $$ = new ExprExprAST($1, NULL); }
  ;

mreg_init_declarator_list
  : mreg_init_declarator { $$ = $1; }
  | mreg_init_declarator_list ',' mreg_init_declarator { $$ = ExprAST::append($1, $3); }
  ;

mreg_init_declarator
  : mreg_declarator { $$ = new ExprExprAST($1, new ExprExprAST(NULL, NULL)); }
  ;

mreg_declarator
  : IDENTIFIER '[' config_pair_list ']' { $$ = new ExprExprAST($1, NULL); }
  | IDENTIFIER { $$ = new ExprExprAST($1, NULL); }
  ;

treg_init_declarator_list
  : treg_init_declarator { $$ = $1; }
  | treg_init_declarator_list ',' treg_init_declarator { $$ = ExprAST::append($1, $3); }
  ;

treg_init_declarator
  : treg_declarator '=' initializer { $$ = new ExprExprAST($1, new ExprExprAST($3, NULL)); }
  | treg_declarator { $$ = new ExprExprAST($1, new ExprExprAST(NULL, NULL)); }
  ;

treg_declarator
  : IDENTIFIER { $$ = new ExprExprAST($1, NULL); }
  ;

kreg_init_declarator_list
  : kreg_init_declarator { $$ = $1; }
  | kreg_init_declarator_list ',' kreg_init_declarator { $$ = ExprAST::append($1, $3); }
  ;

kreg_init_declarator
  : kreg_declarator '=' initializer { $$ = new ExprExprAST($1, new ExprExprAST($3, NULL)); }
  | kreg_declarator { $$ = new ExprExprAST($1, new ExprExprAST(NULL, NULL)); }
  ;

kreg_declarator
  : IDENTIFIER { $$ = new ExprExprAST($1, NULL); }
  ;

mac_init_declarator_list
  : mac_init_declarator { $$ = $1; }
  | mac_init_declarator_list ',' kreg_init_declarator { $$ = ExprAST::append($1, $3); }
  ;

mac_init_declarator
  : mac_declarator { $$ = new ExprExprAST($1, new ExprExprAST(NULL, NULL)); }
  ;

mac_declarator
  : IDENTIFIER { $$ = new ExprExprAST($1, NULL); }
  ;
%%

