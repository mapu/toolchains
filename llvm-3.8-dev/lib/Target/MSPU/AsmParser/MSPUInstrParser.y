%{
#include "llvm/MC/MCParser/MCParsedAsmOperand.h"
#include "llvm/ADT/SmallVector.h"
using namespace llvm;
llvm::raw_ostream &OS = errs();
%}

/* %verbose */
//%expect 1
//%output  	"MSPUGenInstrParser.bison"
%defines 	"MSPUGenInstrParser.h"
%define 	api.pure
//%language "C++"

//%define 	api.prefix  mspu
%name-prefix="mspuinst"

// flex interface, yyparse(Operands) -> yylex(...)
//%parse-param { OperandVector &Operands }

// ragel interface, yyparse(Operands, Ref) -> yylex(..., Ref)
// bison cares only how to invoke yylex rather than its signature.
// note: bison 2.4 can only handle the last arg designated by xxx-param
%parse-param { MSPU::InstLineToParse &InstLine }
%lex-param   { MSPU::InstLineToParse &InstLine }

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

%start MSPUInstLine

%union {

	MSPU::MSPUAsmOperand* op;
	unsigned  opc;
	unsigned  flags;

	struct __mspu_inst_line {
			unsigned bits;
			unsigned cnt;
	} line;

}

%%
MSPUInstLine : InstLine _EOS { assert($1.cnt < 5 && "at most 4 instructions allowed"); YYACCEPT; }

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
		InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::NOP)));
	};

RReg : _RReg      {OS<<"Position 3\n"; $$ = $1 ; InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>($1)); }
JReg : _JReg      {OS<<"Position 4\n"; $$ = $1 ; InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>($1)); }
DReg : _DReg      { $$ = $1 ; InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>($1)); }
KReg : _KReg      { $$ = $1 ; InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>($1)); }
KMReg : _KMReg    { $$ = $1 ; InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>($1)); }
SVRReg : _SVRReg  { $$ = $1 ; InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>($1)); }
SHUReg : _SHUReg  { $$ = $1 ; InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>($1)); }
TReg : _TReg      { $$ = $1 ; InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>($1)); }
MCReg : _MCReg      { $$ = $1 ; InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>($1)); }

Expr : _Imm   { OS<<"Position 5\n"; $$ = $1; InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>($1)); }
|      _Symb  { OS<<"Position 6\n"; $$ = $1; InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>($1)); }
|      _Expr  { OS<<"Position 7\n"; $$ = $1; InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>($1)); }

FPImm: _FPImm { $$ = $1; InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>($1)); }

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

SCUFlags : SCUFlags_ { $$ = $1; InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createImm($1))); }

SCUInst : RReg '=' RReg '+' RReg SCUFlags {
		if($6 & (1<<4))
			InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::AddcR)));
		else
			InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::AddR)));
	}

| RReg '=' RReg '-' RReg SCUFlags {
		if($6 & (1<<4))
			InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::SubcR)));
		else
			InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::SubR)));
	}

| RReg '=' RReg '*' RReg SCUFlags {
		InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::MulR)));
	}

| DReg '=' DReg '+' DReg SCUFlags {
		InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::AddD)));
	}

| DReg '=' DReg '-' DReg SCUFlags {
		InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::SubD)));
	}

| DReg '=' DReg '*' DReg SCUFlags {
		InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::MulD)));
	}

| RReg '=' RReg '&' RReg SCUFlags {
		InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::And)));
	}

| RReg '=' RReg '|' RReg SCUFlags {
		InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::Or)));
	}

| RReg '=' RReg '^' RReg SCUFlags {
		InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::XOr)));
	}

| RReg '=' RReg _EQ RReg SCUFlags {
		InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::EQR)));
	}

| RReg '=' RReg _NE RReg SCUFlags {
		InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::NER)));
	}

| RReg '=' RReg _GT RReg SCUFlags {
		InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::GTR)));
	}

| RReg '=' RReg _GE RReg SCUFlags {
		InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::GER)));
	}

| RReg '=' RReg _LT RReg SCUFlags {
		InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::LTR)));
	}

| RReg '=' RReg _LE RReg SCUFlags {
		InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::LER)));
	}

  // compare DRegs
| RReg '=' DReg _EQ DReg SCUFlags {
		InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::EQRDD)));
	}

| RReg '=' DReg _NE DReg SCUFlags {
		InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::NERDD)));
	}

| RReg '=' DReg _GT DReg SCUFlags {
		InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::GTRDD)));
	}

| RReg '=' DReg _GE DReg SCUFlags {
		InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::GERDD)));
	}

| RReg '=' DReg _LT DReg SCUFlags {
		InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::LTRDD)));
	}

| RReg '=' DReg _LE DReg SCUFlags {
		InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::LERDD)));
	}

| RReg '=' RReg _SL RReg SCUFlags {
	InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::SL)));
	}

| RReg '=' RReg _SL Expr SCUFlags {
	InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::SLImm)));
	}

| RReg '=' RReg _SR RReg SCUFlags {
	InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::SR)));
	}

| RReg '=' RReg _SR Expr SCUFlags {
  InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::SRImm)));
	}
	/////////////////////////////////

| RReg '=' '~' RReg SCUFlags {
	InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::Not)));
	}

| RReg '=' _Abs RReg SCUFlags {
	InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::Abs)));
	}

| RReg '=' _Recip RReg SCUFlags {
	InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::Recip)));
	}

| RReg '=' _RSqRt RReg SCUFlags {
	InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::RSqRt)));
	}

	/////////////////
| DReg '=' _Abs DReg SCUFlags {
	InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::AbsD)));
	}

| DReg '=' _Recip DReg SCUFlags {
	InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::RecipD)));
	}

| DReg '=' _RSqRt DReg SCUFlags {
	InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::RSqRtD)));
	}

	////////////////////////////////////
| RReg '=' _Single RReg SCUFlags {
	InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::Sgl)));
	}

| RReg '=' _Single DReg SCUFlags {
	InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::Sgl)));
	}

| DReg '=' _Double RReg SCUFlags {
	InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::Dbl)));
	}


| RReg '=' _Int RReg SCUFlags {
	InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::Int)));
	}

| RReg '=' _Int DReg SCUFlags {
	InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::Int)));
	}

| RReg '=' _UInt RReg SCUFlags {
	InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::UInt)));
	}

| RReg '=' _UInt DReg SCUFlags {
	InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::UInt)));
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

XferFlags : XferFlags_ { $$ = $1; InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createImm($1))); }

LdStReg : _RReg { $$ = $1; InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>($1)); }
| _JReg         { $$ = $1; InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>($1)); }
| _DReg         { $$ = $1; InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>($1)); }

LdStOpc : '+' { $$ = 0; InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createImm(0))); }
| _PlusE       { $$ = 1; InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createImm(1))); }

ChReg : _RReg { $$ = $1; InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>($1)); }
|       _JReg { $$ = $1; InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>($1)); }

	///// imm assign
AGUInst :  RReg '=' RReg {
		InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::Xfer)));
	}

| RReg '=' JReg {
		InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::Xfer)));
	}

| JReg '=' RReg {
		InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::Xfer)));
	}

| JReg '=' JReg {
		InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::Xfer)));
	}

| DReg '=' DReg {
		InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::XferDD)));
	}

| RReg '=' _Stat {
		InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::GetStat)));
	}

| RReg '=' Expr {
	InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::AssignRRegImm11)));
	}

| JReg '=' Expr {
	InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::AssignJRegImm11)));
	}

| JReg '=' JReg '+' JReg {
	InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::PtrAdd)));
	}

| JReg '=' JReg '-' JReg {
	InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::PtrSub)));
	}

| JReg '=' JReg _SL JReg {
	InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::PtrShiftLeft)));
	}

| JReg '=' JReg _SL Expr {
	InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::PtrShiftLeftImm)));
	}

	/************* write channel *************/
| _CH '[' Expr ']' '=' ChReg {
	InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::DirWrtCh)));
}

| _CH '[' ChReg ']' '=' ChReg {
	InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::IndirWrtCh)));
}

	/************* read channel *************/
| RReg '=' _CH '[' Expr ']' {
	InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::DirReadCh)));
}

| JReg '=' _CH '[' Expr ']' {
	InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::DirReadCh)));
}

| RReg '=' _CH '[' RReg ']' {
	InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::IndirReadCh)));
}

| RReg '=' _CH '[' JReg ']' {
	InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::IndirReadCh)));
}

| JReg '=' _CH '[' RReg ']' {
	InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::IndirReadCh)));
}

| JReg '=' _CH '[' JReg ']' {
	InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::IndirReadCh)));
}

	/********* Read channel number ********/
| RReg '=' _CH '[' Expr ']' _N {
	InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::DirReadChNum)));
}

| JReg '=' _CH '[' Expr ']' _N {
	InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::DirReadChNum)));
}

| RReg '=' _CH '[' RReg ']' _N {
	InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::IndirReadChNum)));
}

| RReg '=' _CH '[' JReg ']' _N {
	InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::IndirReadChNum)));
}

| JReg '=' _CH '[' RReg ']' _N {
	InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::IndirReadChNum)));
}

| JReg '=' _CH '[' JReg ']' _N {
	InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::IndirReadChNum)));
}

	/****************** R/J Register load/store atom **********/
| RReg '=' '[' JReg LdStOpc   JReg ']' _AT {
	InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::LdAtom)));
}

| JReg '=' '[' JReg LdStOpc   JReg ']' _AT {
	InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::LdAtom)));
}

| '[' JReg LdStOpc   JReg ']' '=' LdStReg _AT {
	InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::StAtom)));
}

	/****************** Vector Register load/store **********/
| SVRReg '=' '[' JReg LdStOpc   JReg ']' {
	InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::VecLd)));
}

| '[' JReg LdStOpc   JReg ']' '=' SVRReg {
	InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::VecSt)));
}

	/***** Normal load/store with imm offset instructions ****/
| RReg '=' '[' JReg LdStOpc Expr ']' XferFlags {
	InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::LdImm)));
}

| DReg '=' '[' JReg LdStOpc Expr ']' XferFlags {
	InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::LdImm)));
}

| JReg '=' '[' JReg LdStOpc Expr ']' XferFlags {
	InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::LdImm)));
}

| '[' JReg LdStOpc Expr ']' '=' LdStReg XferFlags {
	InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::StImm)));
}

/***** Normal load/store instructions ****/
| RReg '=' '[' JReg LdStOpc JReg ']' XferFlags {
	InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::Ld)));
}

| DReg '=' '[' JReg LdStOpc JReg ']' XferFlags {
	InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::Ld)));
}

| JReg '=' '[' JReg LdStOpc JReg ']' XferFlags {
	InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::Ld)));
}

| '[' JReg LdStOpc JReg ']' '=' LdStReg XferFlags {
	InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::St)));
}

	/************************* Seq Instructions ***************************/
SeqInst: _Jump Expr {
	InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::JumpImm)));
	}

| _Jump JReg {
	InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::JumpPtr)));
	}

| _If RReg ',' _Jump Expr {
	InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::JumpImmCond)));
	}

| _If RReg ',' _Jump JReg {
	InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::JumpPtrCond)));
	}

| _Call Expr {
	InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::CallImm)));
	}

| _Call JReg {
	InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::CallPtr)));
	}

| _SysCall Expr {
  InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::SysCallImm)));
  }

| _SysCall RReg {
  InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::SysCall)));
  }

| _If RReg ',' _Call Expr {
	InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::CallImmCond)));
	}

| _If RReg ',' _Call JReg {
	InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::CallPtrCond)));
	}

| _LpTo Expr _By JReg _L0 {
	InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::LoopL0)));
	}

| _LpTo Expr _By JReg _L1 {
	InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::LoopL1)));
	}

| _Stop {
	InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::Stop)));
	}

/************************* Syn Instructions ***************************/

CallMFlags :  { $$ = 0; InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createImm(0))); }
| _B 				  {OS<<"Position 2\n"; $$ = 1; InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createImm(1))); }

KMFlags :   {$$ = (1<<0)     ; InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createImm(1<<0))); }
| _H 				{$$ = (1<<1); InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createImm(1<<1))); }

SynInst: RReg '=' _CallM JReg CallMFlags {OS<<"Position 1\n";
	InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::CallMJReg)));
	}

| RReg '='_CallM Expr CallMFlags {
	InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::CallMImm)));
	}

| KReg '=' RReg {
		InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::WrtK)));
	}

| KMReg '=' RReg KMFlags {
		InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::WrtKM)));
	}

/*| _CCtrl '=' RReg {
		InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::WrtCCtrl)));
	}*/

| RReg '=' KReg XYFlag {
		InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::ReadK)));
	}

| RReg '=' KMReg KMFlags XYFlag {
		InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::ReadKM)));
	}

| RReg '=' _CStat {
		InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::ReadCStat)));
	}

| RReg '=' _LPCtr {
    InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::ReadLPCtr)));
  }


| _LPCtr '=' RReg {
    InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::WriteLPCtr)));
  }

/********************************** SVR<--> R ************************************/
| SVRReg '[' RReg ']' '=' RReg XferFlags XYFlag {
		InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::SVRWrt)));
	}

| RReg '=' SVRReg '[' RReg ']' XferFlags XYFlag {
		InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::SVRRead)));
	}

/************************ with Imm Index  ***************************/
| SVRReg '[' Expr ']' '=' RReg XferFlags {
		InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::SVRImmWrt)));
	}

| RReg '=' SVRReg '[' Expr ']' XferFlags {
		InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::SVRImmRead)));
	}

/********************************** MC Config ************************************/
| MCReg '=' RReg XYFlag {
                InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::SetMC)));
}
| RReg '=' MCReg XYFlag {
                InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::GetMC)));
}

/********************************** T<-> SVR ************************************/
| SHUReg '.' TReg '=' SVRReg XYFlag {
	        InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::TPortFromSVR)));
}
| SVRReg '=' SHUReg '.' TReg XYFlag {
	        InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::SVRFromTPort)));
}

/********************************** T<-> R ************************************/
| SHUReg '.' TReg '=' _Vect RReg XferFlags XYFlag {
	        InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::SHUTPortWrt)));
}

| SHUReg '.' TReg '=' _Vect DReg XferFlags XYFlag {
	        InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::SHUTPortWrt)));
}

| RReg '=' SHUReg '.' TReg XferFlags XYFlag {
	        InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::SHUTPortRead)));
}

| DReg '=' SHUReg '.' TReg XferFlags XYFlag {
	        InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createOpc(MSPUInst::SHUTPortRead)));
}

XYFlag : { InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createImm(0))); }
| _X { InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createImm(1))); }
| _Y { InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createImm(2))); }
|_XY { InstLine.Operands->push_back(std::unique_ptr<MSPU::MSPUAsmOperand>(MSPU::MSPUAsmOperand::createImm(3))); }

%%
