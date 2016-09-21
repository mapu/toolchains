%{
#include "llvm/MC/MCParser/MCParsedAsmOperand.h"
#include "llvm/ADT/SmallVector.h"
using namespace llvm;
%}

/* %verbose */
//%expect 1
//%output  	"UCPSGenInstrParser.bison"
%defines 	"UCPSGenInstrParser.h"
%define 	api.pure
//%language "C++"

//%define 	api.prefix  ucps
%name-prefix="ucpsinst"

// flex interface, yyparse(Operands) -> yylex(...)
//%parse-param { OperandVector &Operands }

// ragel interface, yyparse(Operands, Ref) -> yylex(..., Ref)
// bison cares only how to invoke yylex rather than its signature.
// note: bison 2.4 can only handle the last arg designated by xxx-param
%parse-param { UCPS::InstLineToParse &InstLine }
%lex-param   { UCPS::InstLineToParse &InstLine }

//%initial-action { }

%token <op>    _RReg _Symb _Imm _Expr _Err

%union {

	UCPS::UCPSAsmOperand* op;
	unsigned  opc;
	unsigned  flags;

	struct __ucps_inst_line {
			unsigned bits;
			unsigned cnt;
	} line;

}

%%

XYFlag : { InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createImm(0))); };

%%
