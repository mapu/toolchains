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


%type <op>     RReg JReg SVRReg Expr //FPImm

%token _CCtrl _CStat _MC _Vect _CH  _LPCtr _Stat _FIFO _MREG _BIU _SHU _KI
%token _EOS  _Err

%token _EQ _NE _GT _GE _LT _LE _PlusE _Recip _RSqRt _If _NOP _SysCall
%token _Abs _Single _Double _Int _UInt _Jump _Call _CallM _LpTo _By _Stop 
       _Break _IntEn _IntAddr

%token _T _B _H _L _N _AT _L0 _L1 _I _Flag _Column _Step
%token _U _S _D _X _Y _XY _CI _E _RF _SHIFT _DISABLE _ENABLE
%token _SL _SR
%token _Pl
%token _KM _KME _KG _KGE _BR _KE _L1 _L2 _L3 _L4 _ALL _V
%token _KI1215 _KI1619 _KI2023 _KI2427 _KI1227 _KI1618 _KI2022 _KI2426

%type <flags>  CallMFlags SCUFlags AGUFlags BHFlags SEQFlags ISFlags BIUFlags VFlag ModeFlags//KMFlags

%type <flags>  SCUFlags_ AGUFlags_ BHFlags_ SEQFlags_ ISFlags_ BIUFlags_ VFlag_ ModeFlags_


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
		InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::SEQNOP)));
	};
	
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

//FPImm: _FPImm { $$ = $1; InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>($1)); }




/************************* Syn Instructions ***************************/

CallMFlags :  { $$ = 0; InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createImm(0))); }
| _B 				  {$$ = 1; InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createImm(1))); }


/*KMFlags :   {$$ = (1<<0)     ; InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createImm(1<<0))); }
| _H 				{$$ = (1<<1); InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createImm(1<<1))); }*/

ISFlags_: _I {$$ = 0;}
| _S {$$ = 1;}

ISFlags : ISFlags_ { $$ = $1; InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createImm($1))); }

VFlag_: {$$=0;}
| _V {$$=1;}

VFlag : VFlag_ { $$ = $1; InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createImm($1))); }

BIUFlags_ :   _KM {$$ = 0;}
|  _KME {$$ = 1;}
|  _KG {$$ = 2;}
|  _KGE {$$ = 3;}
|  _BR {$$ = 4;}
|  _KE {$$ = 5;}
|  _L1 {$$ = 6;}
|  _L2 {$$ = 7;}
|  _L3 {$$ = 8;}
|  _L4 {$$ = 9;}
|  _ALL {$$ = 10;}
	
BIUFlags : BIUFlags_ { $$ = $1; InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createImm($1))); }

ModeFlags_: _KI1215{$$=0;}
| _KI1619{$$=1;}
| _KI2023{$$=2;}
| _KI2427{$$=3;}
| _KI1227{$$=4;}
| _KI1618{$$=5;}
| _KI2022{$$=6;}
| _KI2426{$$=7;}

ModeFlags : ModeFlags_ { $$ = $1; InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createImm($1))); }








SynInst: RReg '=' _CallM RReg CallMFlags {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::CallMJReg)));
	}

| RReg '='_CallM Expr CallMFlags {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::CallMImm)));
	}
| RReg '='_Stat CallMFlags {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::Stat)));
	}
| RReg '='_FIFO '[' Expr ']' CallMFlags {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::FIFOREAD)));
	}
| _FIFO '[' Expr ']' '=' RReg CallMFlags {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::FIFOWRITE)));
	}
| _MREG '[' Expr ']' '=' SVRReg {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::SVRMRegImm)));
	}
| _MREG '[' RReg ']' '=' SVRReg {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::SVRMRegRs)));
	}
| SVRReg '=' _MREG '[' Expr ']'{
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::MRegSVRImm)));
	}
| SVRReg '=' _MREG '[' RReg ']'{
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::MRegSVRRs)));
	}
| RReg '=' SVRReg '[' Expr ']' BHFlags{
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::SVRToRImm)));
	}
| RReg '=' SVRReg '[' RReg ']' BHFlags{
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::SVRToRRs)));
	}
| SVRReg '[' Expr ']' '=' RReg BHFlags{
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::RToSVRImm)));
	}
| SVRReg '[' RReg ']' '=' RReg BHFlags{
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::RToSVRRs)));
	}
| RReg '=' _MC '[' Expr ']' ISFlags{
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::MCReadImm)));
	}
| RReg '=' _MC '[' RReg ']' ISFlags{
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::MCReadRs)));
	}
| _MC '[' Expr ']' '=' RReg ISFlags{
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::MCWriteImm)));
	}
| _MC '[' RReg ']' '=' RReg ISFlags{
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::MCWriteRs)));
	}
| RReg '=' _BIU '[' Expr ']' BHFlags{
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::BIUToRImm)));
	}
| RReg '=' _BIU '[' RReg ']' BHFlags{
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::BIUToRRs)));
	}
| SVRReg '=' _BIU '[' Expr ']' {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::BIUToSVRImm)));
	}
| SVRReg '=' _BIU '[' RReg ']' {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::BIUToSVRRs)));
	}
| _BIU '[' Expr ']' '=' RReg BHFlags{
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::RToBIUImm)));
	}
| _BIU '[' RReg ']' '=' RReg BHFlags{
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::RToBIURs)));
	}
| _BIU '[' Expr ']' '=' SVRReg BIUFlags {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::SVRToBIUImm)));
	}
| _BIU '[' RReg ']' '=' SVRReg BIUFlags {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::SVRToBIURs)));
	}
| _SHU '[' RReg ']' '=' SVRReg {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::SVRToSHURs)));
	}
| _SHU '[' RReg ']' '=' RReg BHFlags VFlag{
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::RToSHURs)));
	}
| SVRReg '=' _SHU '[' RReg ']' {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::SHUToSVRRs)));
	}
| RReg '=' _SHU '[' RReg ']' BHFlags{
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::SHUToRRs)));
	}
| _KI '=' SVRReg ModeFlags {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::SVRToKI)));
	}
| _KI '[' RReg ']' '=' RReg {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::RToKI)));
	}
| SVRReg '=' _KI ModeFlags {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::KIToSVR)));
	}
| RReg '=' _KI '[' RReg ']' {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::KIToR)));
	}
/*************************** SCU Instructions*****************************/


SCUFlags_ :   {$$ = 0b110000;}
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
| SCUFlags_  _S {
		$$ = $1 & 0b101111;
	}
| SCUFlags_  _SHIFT {
		$$ = $1 & 0b011111;
	}
| SCUFlags_  _I {
		$$ = $1 | (1<<0);
	}
| SCUFlags_  _E {
		$$ = $1 | (1<<1);
	}
| SCUFlags_  _L {
		$$ = $1 | (1<<0);
	}
| SCUFlags_  _D 
	
SCUFlags : SCUFlags_ { $$ = $1; InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createImm($1))); }


SCUInst : RReg '=' RReg '+' RReg SCUFlags {
		InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::Add)));
	}
| RReg '=' RReg '-' RReg SCUFlags {
		InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::Sub)));
	}
| RReg '=' RReg '*' RReg SCUFlags  {
		InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::Mul)));
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
| RReg '=' '~' RReg {
		InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::Not)));
	}
| RReg '=' RReg _SL RReg SCUFlags{
		InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::LSHT)));
	}
| RReg '=' RReg _SR RReg SCUFlags {
		InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::RSHT)));
	}
| RReg '=' RReg _SL Expr SCUFlags {
		InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::LSHTImm)));
	}	
| RReg '=' RReg _SR Expr SCUFlags {
		InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::RSHTImm)));
	}	
| RReg '=' RReg _EQ RReg SCUFlags{
		InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::EQ)));
	}
| RReg '=' RReg _NE RReg SCUFlags {
		InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::NEQ)));
	}
| RReg '=' RReg _LT RReg SCUFlags {
		InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::LT)));
	}	
| RReg '=' RReg _GE RReg SCUFlags {
		InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::GE)));
	}
| RReg '=' Expr SCUFlags {
		InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::AssignImm)));
	}
| RReg '=' _Abs RReg SCUFlags {
		InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::ABS)));
	}
| RReg '=' _RF {
		InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::ReadFlag)));
	}
| _RF '=' RReg {
		InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::WriteFlag)));
	}
| RReg '=' _Single RReg SCUFlags {
		InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::IntToSin)));
	}
| RReg '=' _Double RReg SCUFlags {
		InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::IntOrSinToDou)));
	}
| RReg '=' _Int RReg SCUFlags {
		InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::SinToInt)));
	}
| RReg '=' _Int '{' RReg ',' RReg '}' SCUFlags {
		InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::DouToInt)));
	}
| RReg '=' _Single '{' RReg ',' RReg '}' SCUFlags {
		InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::DouToSin)));
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

/************************* Seq Instructions ***************************/
SEQFlags_ :   {$$ = 0;}
| _DISABLE { $$ = 1;}
| _ENABLE { $$ = 2;}


SEQFlags : SEQFlags_ { $$ = $1; InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createImm($1))); }
	
	
SeqInst: _Jump Expr SEQFlags {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::JumpImm)));
	}
| _If RReg ',' _Jump Expr SEQFlags {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::JumpImmCond)));
	}
| _Jump RReg SEQFlags {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::JumpPtr)));
	}
| _If RReg ',' _Jump RReg SEQFlags {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::JumpPtrCond)));
	}
| _Call Expr SEQFlags {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::CallImm)));
	}
| _If RReg ',' _Call Expr SEQFlags {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::CallImmCond)));
	}
| _Call RReg SEQFlags {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::CallPtr)));
	}
| _If RReg ',' _Call RReg SEQFlags {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::CallPtrCond)));
	}
| _LpTo Expr _By RReg _L0 {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::LoopL0)));
	}
| _LpTo Expr _By RReg _L1 {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::LoopL1)));
	}
| _Stop {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::Stop)));
	}
| _Break {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::Break)));
	}
| _IntEn SEQFlags {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::Int)));
	}
| _IntAddr '[' RReg ']' {
	InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createOpc(UCPSInst::IntAddr)));
	}
	
//XYFlag : { InstLine.Operands->push_back(std::unique_ptr<UCPS::UCPSAsmOperand>(UCPS::UCPSAsmOperand::createImm(0))); };

%%
