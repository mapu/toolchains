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

%token <op>    _RReg _JReg _SVRReg _Imm  _Symb  _Expr  _FPImm


%type <op>     RReg JReg SVRReg Expr FPImm

%token _CCtrl _CStat _MC _Vect _CH  _LPCtr _Stat
%token _EOS  _Err

%token _EQ _NE _GT _GE _LT _LE _PlusE _Recip _RSqRt _If _NOP _SysCall
%token _Abs _Single _Double _Int _UInt _Jump _Call _CallM _LpTo _By _Stop

%token _T _B _H _L _N _AT _L0 _L1 _I _Flag _Column _Step
%token _U _S _D _X _Y _XY _CI
%token _SL _SR
%token _Pl


%type <flags>  CallMFlags KMFlags SCUFlags Sflag AGUFlags BHFlags

%type <flags>  SCUFlags_ AGUFlags_ BHFlags_


%type <line>  InstLine

%start UCPSInstLine

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

UCPSInstLine : InstLine _EOS { assert($1.cnt < 5 && "at most 4 instructions allowed"); YYACCEPT; }

InstLine:  /*NOPInst  {	$$.cnt = 1;	}*/
//| SeqInst  {	$$.bits = (1<<0);		$$.cnt = 1;	}
 SCUInst  {	$$.bits = (1<<1);		$$.cnt = 1;	}
| AGUInst  {	$$.bits = (1<<2);		$$.cnt = 1;	}
| SynInst  {	$$.bits = (1<<3);		$$.cnt = 1;	}

/*| InstLine  _Pl NOPInst  {
		$$.cnt = $1.cnt + 1;
	}
| InstLine  _Pl SeqInst  {
		assert(($1.bits & (1<<0)) != (1<<0) && "more than one Seq instructions exist");
		$$.bits = $1.bits | (1<<0);
		$$.cnt = $1.cnt + 1;
	}*/
| InstLine  _Pl SCUInst  {
		assert(($1.bits & (1<<1)) != (1<<1) && "more than one SCU instructions exist");
		$$.bits = $1.bits | (1<<1);
		$$.cnt = $1.cnt + 1;
	}
| InstLine  _Pl AGUInst  {
		assert(($1.bits & (1<<2)) != (1<<2) && "more than one AGU instructions exist");
		$$.bits = $1.bits | (1<<2);
		$$.cnt = $1.cnt + 1;
	}
| InstLine  _Pl SynInst  {
		assert(($1.bits & (1<<3)) != (1<<3) && "more than one Syn instructions exist");
		$$.bits = $1.bits | (1<<3);
		$$.cnt = $1.cnt + 1;
	};


/*NOPInst: _NOP {
		InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::NOP)));
	};*/
	
RReg : _RReg      { $$ = $1 ; InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>($1)); }
JReg : _JReg      { $$ = $1 ; InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>($1)); }
/*DReg : _DReg      { $$ = $1 ; InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>($1)); }
KReg : _KReg      { $$ = $1 ; InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>($1)); }
KMReg : _KMReg    { $$ = $1 ; InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>($1)); }*/
SVRReg : _SVRReg  { $$ = $1 ; InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>($1)); }
/*SHUReg : _SHUReg  { $$ = $1 ; InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>($1)); }
TReg : _TReg      { $$ = $1 ; InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>($1)); }
MCReg : _MCReg      { $$ = $1 ; InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>($1)); }*/

Expr : _Imm   { $$ = $1; InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>($1)); }
|      _Symb  { $$ = $1; InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>($1)); }
|      _Expr  { $$ = $1; InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>($1)); }

FPImm: _FPImm { $$ = $1; InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>($1)); }




/************************* Syn Instructions ***************************/

CallMFlags :  { $$ = 0; InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createImm(0))); }
| _B 				  {$$ = 1; InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createImm(1))); }

KMFlags :   {$$ = (1<<0)     ; InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createImm(1<<0))); }
| _H 				{$$ = (1<<1); InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createImm(1<<1))); }

SynInst: RReg '=' _CallM JReg CallMFlags {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::CallMJReg)));
	}

| RReg '='_CallM Expr CallMFlags {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::CallMImm)));
	}
	
	
/*************************** SCU Instructions*****************************/


SCUFlags_ :   {$$ = 0;}
| SCUFlags_  _CI {
		$$ = $1 | (1<<0);
	}
| SCUFlags_  _Flag {
		$$ = $1 | (1<<1);		
	}
| SCUFlags_  _U {
		$$ = $1 | (1<<2);
	}
| SCUFlags_  _T {
		$$ = $1 | (1<<3);
	}
| SCUFlags_  _I {
		$$ = $1 | (1<<0);
	}
	
SCUFlags : SCUFlags_ { $$ = $1; InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createImm($1))); }
	
Sflag: _S 

SCUInst : RReg '=' RReg '+' RReg SCUFlags {
		InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::AddR)));
	}

| RReg '=' RReg '-' RReg SCUFlags {
		InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::SubR)));
	}

| RReg '=' RReg '*' RReg SCUFlags {
		InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::MulR)));
	}

| RReg '=' RReg '+' RReg Sflag SCUFlags {
		InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::AddS)));
	}

| RReg '=' RReg '-' RReg Sflag SCUFlags {
		InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::SubS)));
	}

| RReg '=' RReg '*' RReg Sflag SCUFlags {
		InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::MulS)));
	}

| RReg '=' RReg '&' RReg  {
		InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::And)));
	}

| RReg '=' RReg '|' RReg  {
		InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::Or)));
	}

| RReg '=' RReg '^' RReg  {
		InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::XOr)));
	}

	
/********************************* AGU ********************************/

AGUFlags_ :   {$$ = 0;}
| AGUFlags_  _Step {
		$$ = $1 | (1<<0);
	}
| AGUFlags_  _Column {
		$$ = $1 | (1<<1);		
	}
| AGUFlags_  _U {
		$$ = $1 | (1<<2);
	}
| AGUFlags_  _AT {
		$$ = $1 | (1<<3);
	}

AGUFlags : AGUFlags_ { $$ = $1; InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createImm($1))); }
	
BHFlags_: {$$ = 2;}
| _B {$$ = 0;}
| _H {$$ = 1;}

BHFlags : BHFlags_ { $$ = $1; InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createImm($1))); }


AGUInst : RReg '=' '[' RReg '+'   RReg ']' BHFlags AGUFlags{
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::AGULoad)));
}

| '[' RReg '+'   RReg ']' '=' RReg BHFlags AGUFlags {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::AGUStore)));
}

| SVRReg '=' '[' RReg '+'   RReg ']' AGUFlags{
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::AGUSvrLd)));
}

| '[' RReg '+'   RReg ']' '=' SVRReg AGUFlags{
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::AGUSvrSt)));
}


	
XYFlag : { InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createImm(0))); };

%%
