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

%token <op>    _RReg _JReg _DReg _Imm  _Symb  _Expr  _FPImm
%token <op>    _KReg _KMReg _SVRReg _SHUReg _TReg _MCReg

%type <op>     RReg JReg  DReg    Expr FPImm
%type <op>     KReg KMReg SVRReg SHUReg TReg MCReg

%token _CCtrl _CStat _MC _Vect _CH  _LPCtr _Stat
%token _EOS  _Err

%token _EQ _NE _GT _GE _LT _LE _PlusE _Recip _RSqRt _If _NOP _SysCall
%token _Abs _Single _Double _Int _UInt _Jump _Call _CallM _LpTo _By _Stop

%token _T _B _H _L _N _AT _L0 _L1
%token _U _S _D _X _Y _XY _CI
%token _SL _SR
%token _Pl

%type <opc>  LdStOpc
%type <op>   LdStReg ChReg

%type <flags>  CallMFlags KMFlags XferFlags SCUFlags
%type <flags>  XferFlags_  SCUFlags_

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

InstLine:  NOPInst  {	$$.cnt = 1;	}
| SeqInst  {	$$.bits = (1<<0);		$$.cnt = 1;	}
| SCUInst  {	$$.bits = (1<<1);		$$.cnt = 1;	}
| AGUInst  {	$$.bits = (1<<2);		$$.cnt = 1;	}
| SynInst  {	$$.bits = (1<<3);		$$.cnt = 1;	}

| InstLine  _Pl NOPInst  {
		$$.cnt = $1.cnt + 1;
	}
| InstLine  _Pl SeqInst  {
		assert(($1.bits & (1<<0)) != (1<<0) && "more than one Seq instructions exist");
		$$.bits = $1.bits | (1<<0);
		$$.cnt = $1.cnt + 1;
	}
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


NOPInst: _NOP {
		InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::NOP)));
	};

RReg : _RReg      { $$ = $1 ; InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>($1)); }
JReg : _JReg      { $$ = $1 ; InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>($1)); }
DReg : _DReg      { $$ = $1 ; InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>($1)); }
KReg : _KReg      { $$ = $1 ; InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>($1)); }
KMReg : _KMReg    { $$ = $1 ; InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>($1)); }
SVRReg : _SVRReg  { $$ = $1 ; InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>($1)); }
SHUReg : _SHUReg  { $$ = $1 ; InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>($1)); }
TReg : _TReg      { $$ = $1 ; InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>($1)); }
MCReg : _MCReg      { $$ = $1 ; InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>($1)); }

Expr : _Imm   { $$ = $1; InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>($1)); }
|      _Symb  { $$ = $1; InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>($1)); }
|      _Expr  { $$ = $1; InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>($1)); }

FPImm: _FPImm { $$ = $1; InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>($1)); }

	/*************************** SCU *****************************/

  // CI D U S T
SCUFlags_ :   {$$ = 0;}
| SCUFlags_  _T {
		assert(($1 & (1<<0))==0 && "duplicate flag (T) for SCU");
		$$ = $1 | (1<<0);
	}
| SCUFlags_  _S {
		assert(($1 & (1<<1))==0 && "duplicate flag (S) for SCU");
		$$ = $1 | (1<<1);
		assert((($$>>1)&3) != 3 && "invalid flags for SCU, (S) and (D) can not coexist");
	}
| SCUFlags_  _U {
		assert(($1 & (1<<2))==0 && "duplicate flag (U) for SCU");
		$$ = $1 | (1<<2);
		assert((($$>>1)&3) != 3 && "invalid flags for SCU, (S) and (D) can not coexist");
	}
| SCUFlags_  _D {
		assert(($1 & (1<<3))==0 && "duplicate flag (D) for SCU");
		$$ = $1 | (1<<3);
	}
| SCUFlags_  _CI {
		assert(($1 & (1<<4))==0 && "duplicate flag (CI) for SCU");
		$$ = $1 | (1<<4);
	}

SCUFlags : SCUFlags_ { $$ = $1; InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createImm($1))); }

SCUInst : RReg '=' RReg '+' RReg SCUFlags {
		if($6 & (1<<4))
			InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::AddcR)));
		else
			InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::AddR)));
	}

| RReg '=' RReg '-' RReg SCUFlags {
		if($6 & (1<<4))
			InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::SubcR)));
		else
			InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::SubR)));
	}

| RReg '=' RReg '*' RReg SCUFlags {
		InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::MulR)));
	}

| DReg '=' DReg '+' DReg SCUFlags {
		InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::AddD)));
	}

| DReg '=' DReg '-' DReg SCUFlags {
		InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::SubD)));
	}

| DReg '=' DReg '*' DReg SCUFlags {
		InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::MulD)));
	}

| RReg '=' RReg '&' RReg SCUFlags {
		InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::And)));
	}

| RReg '=' RReg '|' RReg SCUFlags {
		InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::Or)));
	}

| RReg '=' RReg '^' RReg SCUFlags {
		InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::XOr)));
	}

| RReg '=' RReg _EQ RReg SCUFlags {
		InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::EQR)));
	}

| RReg '=' RReg _NE RReg SCUFlags {
		InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::NER)));
	}

| RReg '=' RReg _GT RReg SCUFlags {
		InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::GTR)));
	}

| RReg '=' RReg _GE RReg SCUFlags {
		InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::GER)));
	}

| RReg '=' RReg _LT RReg SCUFlags {
		InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::LTR)));
	}

| RReg '=' RReg _LE RReg SCUFlags {
		InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::LER)));
	}

  // compare DRegs
| RReg '=' DReg _EQ DReg SCUFlags {
		InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::EQRDD)));
	}

| RReg '=' DReg _NE DReg SCUFlags {
		InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::NERDD)));
	}

| RReg '=' DReg _GT DReg SCUFlags {
		InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::GTRDD)));
	}

| RReg '=' DReg _GE DReg SCUFlags {
		InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::GERDD)));
	}

| RReg '=' DReg _LT DReg SCUFlags {
		InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::LTRDD)));
	}

| RReg '=' DReg _LE DReg SCUFlags {
		InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::LERDD)));
	}

| RReg '=' RReg _SL RReg SCUFlags {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::SL)));
	}

| RReg '=' RReg _SL Expr SCUFlags {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::SLImm)));
	}

| RReg '=' RReg _SR RReg SCUFlags {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::SR)));
	}

| RReg '=' RReg _SR Expr SCUFlags {
  InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::SRImm)));
	}
	/////////////////////////////////

| RReg '=' '~' RReg SCUFlags {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::Not)));
	}

| RReg '=' _Abs RReg SCUFlags {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::Abs)));
	}

| RReg '=' _Recip RReg SCUFlags {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::Recip)));
	}

| RReg '=' _RSqRt RReg SCUFlags {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::RSqRt)));
	}

	/////////////////
| DReg '=' _Abs DReg SCUFlags {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::AbsD)));
	}

| DReg '=' _Recip DReg SCUFlags {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::RecipD)));
	}

| DReg '=' _RSqRt DReg SCUFlags {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::RSqRtD)));
	}

	////////////////////////////////////
| RReg '=' _Single RReg SCUFlags {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::Sgl)));
	}

| RReg '=' _Single DReg SCUFlags {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::Sgl)));
	}

| DReg '=' _Double RReg SCUFlags {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::Dbl)));
	}


| RReg '=' _Int RReg SCUFlags {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::Int)));
	}

| RReg '=' _Int DReg SCUFlags {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::Int)));
	}

| RReg '=' _UInt RReg SCUFlags {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::UInt)));
	}

| RReg '=' _UInt DReg SCUFlags {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::UInt)));
	}


	/********************************* AGU ********************************/
XferFlags_ :   {$$ = 0; }
| XferFlags_  _B {
		assert(($1 & (1<<2))==0 && "duplicate flags for load/store");
		$$ = $1 | (1<<2);
		assert((($$>>1)&3) != 3 && "invalid flags for load/store, (B) and (H) can not coexist");
	}
| XferFlags_  _H {
		assert(($1 & (1<<1))==0 && "duplicate flags for load/store");
		$$ = $1 | (1<<1);
		assert((($$>>1)&3) != 3 && "invalid flags for load/store, (B) and (H) can not coexist");
	}
| XferFlags_  _U {
		assert(($1 & (1<<0))==0 && "duplicate flags for load/store");
		$$ = $1 | (1<<0);
	}

XferFlags : XferFlags_ { $$ = $1; InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createImm($1))); }

LdStReg : _RReg { $$ = $1; InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>($1)); }
| _JReg         { $$ = $1; InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>($1)); }
| _DReg         { $$ = $1; InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>($1)); }

LdStOpc : '+' { $$ = 0; InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createImm(0))); }
| _PlusE       { $$ = 1; InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createImm(1))); }

ChReg : _RReg { $$ = $1; InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>($1)); }
|       _JReg { $$ = $1; InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>($1)); }

	///// imm assign
AGUInst :  RReg '=' RReg {
		InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::Xfer)));
	}

| RReg '=' JReg {
		InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::Xfer)));
	}

| JReg '=' RReg {
		InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::Xfer)));
	}

| JReg '=' JReg {
		InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::Xfer)));
	}

| DReg '=' DReg {
		InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::XferDD)));
	}

| RReg '=' _Stat {
		InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::GetStat)));
	}

| RReg '=' Expr {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::AssignRRegImm11)));
	}

| JReg '=' Expr {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::AssignJRegImm11)));
	}

| JReg '=' JReg '+' JReg {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::PtrAdd)));
	}

| JReg '=' JReg '-' JReg {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::PtrSub)));
	}

| JReg '=' JReg _SL JReg {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::PtrShiftLeft)));
	}

| JReg '=' JReg _SL Expr {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::PtrShiftLeftImm)));
	}

	/************* write channel *************/
| _CH '[' Expr ']' '=' ChReg {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::DirWrtCh)));
}

| _CH '[' ChReg ']' '=' ChReg {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::IndirWrtCh)));
}

	/************* read channel *************/
| RReg '=' _CH '[' Expr ']' {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::DirReadCh)));
}

| JReg '=' _CH '[' Expr ']' {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::DirReadCh)));
}

| RReg '=' _CH '[' RReg ']' {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::IndirReadCh)));
}

| RReg '=' _CH '[' JReg ']' {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::IndirReadCh)));
}

| JReg '=' _CH '[' RReg ']' {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::IndirReadCh)));
}

| JReg '=' _CH '[' JReg ']' {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::IndirReadCh)));
}

	/********* Read channel number ********/
| RReg '=' _CH '[' Expr ']' _N {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::DirReadChNum)));
}

| JReg '=' _CH '[' Expr ']' _N {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::DirReadChNum)));
}

| RReg '=' _CH '[' RReg ']' _N {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::IndirReadChNum)));
}

| RReg '=' _CH '[' JReg ']' _N {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::IndirReadChNum)));
}

| JReg '=' _CH '[' RReg ']' _N {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::IndirReadChNum)));
}

| JReg '=' _CH '[' JReg ']' _N {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::IndirReadChNum)));
}

	/****************** R/J Register load/store atom **********/
| RReg '=' '[' JReg LdStOpc   JReg ']' _AT {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::LdAtom)));
}

| JReg '=' '[' JReg LdStOpc   JReg ']' _AT {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::LdAtom)));
}

| '[' JReg LdStOpc   JReg ']' '=' LdStReg _AT {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::StAtom)));
}

	/****************** Vector Register load/store **********/
| SVRReg '=' '[' JReg LdStOpc   JReg ']' {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::VecLd)));
}

| '[' JReg LdStOpc   JReg ']' '=' SVRReg {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::VecSt)));
}

	/***** Normal load/store with imm offset instructions ****/
| RReg '=' '[' JReg LdStOpc Expr ']' XferFlags {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::LdImm)));
}

| DReg '=' '[' JReg LdStOpc Expr ']' XferFlags {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::LdImm)));
}

| JReg '=' '[' JReg LdStOpc Expr ']' XferFlags {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::LdImm)));
}

| '[' JReg LdStOpc Expr ']' '=' LdStReg XferFlags {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::StImm)));
}

/***** Normal load/store instructions ****/
| RReg '=' '[' JReg LdStOpc JReg ']' XferFlags {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::Ld)));
}

| DReg '=' '[' JReg LdStOpc JReg ']' XferFlags {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::Ld)));
}

| JReg '=' '[' JReg LdStOpc JReg ']' XferFlags {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::Ld)));
}

| '[' JReg LdStOpc JReg ']' '=' LdStReg XferFlags {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::St)));
}

	/************************* Seq Instructions ***************************/
SeqInst: _Jump Expr {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::JumpImm)));
	}

| _Jump JReg {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::JumpPtr)));
	}

| _If RReg ',' _Jump Expr {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::JumpImmCond)));
	}

| _If RReg ',' _Jump JReg {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::JumpPtrCond)));
	}

| _Call Expr {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::CallImm)));
	}

| _Call JReg {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::CallPtr)));
	}

| _SysCall Expr {
  InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::SysCallImm)));
  }

| _SysCall RReg {
  InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::SysCall)));
  }

| _If RReg ',' _Call Expr {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::CallImmCond)));
	}

| _If RReg ',' _Call JReg {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::CallPtrCond)));
	}

| _LpTo Expr _By JReg _L0 {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::LoopL0)));
	}

| _LpTo Expr _By JReg _L1 {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::LoopL1)));
	}

| _Stop {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::Stop)));
	}

/************************* Syn Instructions ***************************/

CallMFlags :  { $$ = 0; InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createImm(0))); }
| _B 				  { $$ = 1; InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createImm(1))); }

KMFlags :   {$$ = (1<<0)     ; InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createImm(1<<0))); }
| _H 				{$$ = (1<<1); InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createImm(1<<1))); }

SynInst: _CallM JReg CallMFlags {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::CallMJReg)));
	}

| _CallM Expr CallMFlags {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::CallMImm)));
	}

| KReg '=' RReg {
		InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::WrtK)));
	}

| KMReg '=' RReg KMFlags {
		InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::WrtKM)));
	}

/*| _CCtrl '=' RReg {
		InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::WrtCCtrl)));
	}*/

| RReg '=' KReg XYFlag {
		InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::ReadK)));
	}

| RReg '=' KMReg KMFlags XYFlag {
		InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::ReadKM)));
	}

| RReg '=' _CStat {
		InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::ReadCStat)));
	}

| RReg '=' _LPCtr {
    InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::ReadLPCtr)));
  }


| _LPCtr '=' RReg {
    InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::WriteLPCtr)));
  }

/********************************** SVR<--> R ************************************/
| SVRReg '[' RReg ']' '=' RReg XferFlags XYFlag {
		InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::SVRWrt)));
	}

| RReg '=' SVRReg '[' RReg ']' XferFlags XYFlag {
		InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::SVRRead)));
	}

/************************ with Imm Index  ***************************/
| SVRReg '[' Expr ']' '=' RReg XferFlags {
		InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::SVRImmWrt)));
	}

| RReg '=' SVRReg '[' Expr ']' XferFlags {
		InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::SVRImmRead)));
	}

/********************************** MC Config ************************************/
| MCReg '=' RReg XYFlag {
                InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::SetMC)));
}
| RReg '=' MCReg XYFlag {
                InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::GetMC)));
}

/********************************** T<-> SVR ************************************/
| SHUReg '.' TReg '=' SVRReg XYFlag {
	        InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::TPortFromSVR)));
}
| SVRReg '=' SHUReg '.' TReg XYFlag {
	        InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::SVRFromTPort)));
}

/********************************** T<-> R ************************************/
| SHUReg '.' TReg '=' _Vect RReg XferFlags XYFlag {
	        InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::SHUTPortWrt)));
}

| SHUReg '.' TReg '=' _Vect DReg XferFlags XYFlag {
	        InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::SHUTPortWrt)));
}

| RReg '=' SHUReg '.' TReg XferFlags XYFlag {
	        InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::SHUTPortRead)));
}

| DReg '=' SHUReg '.' TReg XferFlags XYFlag {
	        InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::SHUTPortRead)));
}

XYFlag : { InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createImm(0))); }
| _X { InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createImm(1))); }
| _Y { InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createImm(2))); }
|_XY { InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createImm(3))); }

%%
