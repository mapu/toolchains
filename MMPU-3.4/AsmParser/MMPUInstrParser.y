%{
#include "llvm/MC/MCParser/MCParsedAsmOperand.h"
#include "llvm/ADT/SmallVector.h"
using namespace llvm;

#define MMPUSetToken(dst, src) {}
%}

//%verbose
//%output "MMPUGenInstrParser.bison"
%defines 	"MMPUGenInstrParser.h"

%define 	api.pure

//%define 	api.prefix  mmpu
%name-prefix="mmpuinst"
//
// flex interface, yyparse(Operands) -> yylex(...)
//%parse-param { SmallVectorImpl<MCParsedAsmOperand*> & Operands }

// ragel interface, yyparse(Operands, Ref) -> yylex(..., Ref)
// bison cares only how to invoke yylex rather than its signature.
// note: bison 2.4 can only handle the last arg designated by xxx-param

%parse-param { MMPU::InstLineToParse &InstLine }
%lex-param   { MMPU::InstLineToParse &InstLine }

%debug
//%define parse.trace

//%initial-action { }

%union {

    MMPU::MMPUToken token;

}

%token t_r0 t_r1 t_r2 t_r3 t_w3
%token t_KG t_DM    t_TB    t_Pl  t_Err   t_COND  t_Copy t_Expd t_Cprs
%token t_By t_EAdd t_AddE t_Start t_Stop t_Jump t_MPU t_At

%token t_OPC t_EQ t_NE t_GT t_GE t_LT t_LE t_MDivR t_MDivQ t_DivS t_DivQ t_DivR
%token t_Max t_Min t_AddSub t_SL t_SR t_Comb t_EOS t_CMul t_ACS  t_RMax t_RMin

%token t_Abs t_Sgl t_Dbl t_Int t_UInt t_NOP t_Indx t_MR t_Recip t_RSqRt
%token t_Repeat t_Loop t_If t_EOL

%token <token> t_WPort t_BIU t_Shu t_Calc   t_IPort t_FMAC
%type  <token>           BIU   Shu   Calc     IPort   FMAC

%token <token>  t_MReg t_MRegExt t_TPort t_TPortExt t_Imm
%type  <token>    MReg   MRegExt   TPort   TPortExt

%token <token>   t_Expr
%type  <token>     Expr Label

%token <token> t_Symb

%token <token> o_TC o_C  o_NC o_WF1 o_WF2 o_WF3 o_WX1 o_WX2 o_WX3 o_WX4 o_FWC o_BWC  o_FW
%token <token> o_T o_L o_I o_CR o_B o_H o_U o_S o_D o_IND  o_BW o_M o_BR o_APP o_KPP

%token <token>  t_KIReg

%type <token>   Options_ Option_
%type <token>   Options

%type <token> inst instLine

%%

mmpuInstLine : instLine t_EOS {
    //assert( $1<15 && "no more than 14 instructions can be emited parallelly");
  }

| NOP t_EOS {
    //assert( $1<15 && "no more than 14 instructions can be emited parallelly");
  }

| LabelList NOP t_EOS {
    //assert( $2<15 && "no more than 14 instructions can be emited parallelly");
  }

| LabelList instLine t_EOS {
    //assert( $2<15 && "no more than 14 instructions can be emited parallelly");
  }

instLine: 	inst					    { $$.num = 1; }
| 					instLine ';' inst { $$.num = $1.num + 1;	}

TPort : t_TPort {
    unsigned reg = MMPU::MMPUAsmParser::getRegInfo()->getRegClass(MMPUReg::TPortRegClassID).getRegister($1.num);
    $$.reg = reg; $$.pos = $1.pos;
    MMPU::MMPUAsmOperand* op = MMPU::MMPUAsmOperand::createReg(reg);
    InstLine.Operands.push_back(op);
  }

TPortExt : t_TPortExt {
    unsigned reg = MMPU::MMPUAsmParser::getRegInfo()->getRegClass(MMPUReg::TPortRegClassID).getRegister($1.num);
    $$.reg = reg; $$.pos = $1.pos;
    MMPU::MMPUAsmOperand* op = MMPU::MMPUAsmOperand::createReg(reg);
    InstLine.Operands.push_back(op);
  }

MReg : t_MReg {
    unsigned reg = MMPU::MMPUAsmParser::getRegInfo()->getRegClass(MMPUReg::MRegRegClassID).getRegister($1.num);
    $$.reg = reg; $$.pos = $1.pos;
    MMPU::MMPUAsmOperand* op = MMPU::MMPUAsmOperand::createReg(reg);
    InstLine.Operands.push_back(op);
  }

MRegExt : t_MRegExt {
    unsigned reg = MMPU::MMPUAsmParser::getRegInfo()->getRegClass(MMPUReg::MRegRegClassID).getRegister($1.num);
    $$.reg = reg; $$.pos = $1.pos;
    MMPU::MMPUAsmOperand* op = MMPU::MMPUAsmOperand::createReg(reg);
    InstLine.Operands.push_back(op);
  }

BIU   : t_BIU   { $$.reg = $1.reg; $$.pos = $1.pos; MMPU::MMPUAsmOperand* op = MMPU::MMPUAsmOperand::createReg($1.reg); InstLine.Operands.push_back(op); }
Shu   : t_Shu   { $$.reg = $1.reg; $$.pos = $1.pos; MMPU::MMPUAsmOperand* op = MMPU::MMPUAsmOperand::createReg($1.reg); InstLine.Operands.push_back(op); }
Calc  : t_Calc  { $$.reg = $1.reg; $$.pos = $1.pos; MMPU::MMPUAsmOperand* op = MMPU::MMPUAsmOperand::createReg($1.reg); InstLine.Operands.push_back(op); }
FMAC  : t_FMAC  { $$.reg = $1.reg; $$.pos = $1.pos; MMPU::MMPUAsmOperand* op = MMPU::MMPUAsmOperand::createReg($1.reg); InstLine.Operands.push_back(op); }
IPort : '(' t_IPort ')' { $$.reg = $2.reg; $$.pos = $2.pos; MMPU::MMPUAsmOperand* op = MMPU::MMPUAsmOperand::createReg($2.reg); InstLine.Operands.push_back(op); }

Expr  : t_Expr { $$.expr = $1.expr; MMPU::MMPUAsmOperand* op = MMPU::MMPUAsmOperand::createExpr($1.expr); InstLine.Operands.push_back(op); }

Label  : t_Expr ':' {
    $$.expr = $1.expr;
    assert($1.expr->getKind()==MCExpr::SymbolRef);
    MCAsmParser* mcparser = MMPU::MMPUAsmParser::getMCParser();
    const MCSymbolRefExpr * expr = static_cast<const MCSymbolRefExpr *>($1.expr);
    MCSymbol * symb = mcparser->getContext().GetOrCreateSymbol(expr->getSymbol().getName());
    mcparser->getStreamer().EmitLabel(symb);
  }

LabelList : Label {}
| LabelList Label {}

/* ports */
Mr0 : MReg '.' t_r0  {}
Mr1 : MReg '.' t_r1  {}
Mr2 : MReg '.' t_r2  {
  assert($1.reg != MMPUReg::MIPP &&
         "invalid operand M[I++] for slot 3, must be M0 ~ M127"); }
Mr3 : MReg '.' t_r3  {
  assert($1.reg != MMPUReg::MIPP &&
         "invalid operand M[I++] for slot 4, must be M0 ~ M127"); }
Mw3 : MReg '.' t_w3  {}
MExtw3 : MRegExt '.' t_w3  {}

/* definition for unary operations between TPorts. */
TPortBitNot : '~' TPort {}
TPortRMax : t_RMax TPort {}
TPortRMin : t_RMin TPort {}

TPortExpd : t_Expd TPort {}
TPortAbs : t_Abs TPort {}
TPortRecip : t_Recip TPort {}
TPortRSqRt : t_RSqRt TPort {}
TPortInt : t_Int TPort {}
TPortUInt : t_UInt TPort {}
TPortSgl : t_Sgl TPort {}
TPortExtInt  : t_Int TPortExt {}
TPortExtUInt : t_UInt TPortExt {}
TPortExtSgl  : t_Sgl TPortExt {}
TPortDbl : t_Dbl TPort {}

/* definition for binary operations between TPorts. */
TPortsIndx : TPort t_Indx TPort { }
TPortsAdd : TPort  '+' TPort { }
TPortsSub : TPort  '-' TPort { }
TPortsMax : t_Max '(' TPort  ',' TPort ')' { }
TPortsMin : t_Min '(' TPort  ',' TPort ')' { }
TPortsCprs : t_Cprs '(' TPort  ',' TPort ')' { }
TPortsAbsSub : t_Abs '(' TPort  '-' TPort ')' { }
TPortsAnd : TPort  '&' TPort { }
TPortsOr : TPort  '|' TPort { }
TPortsXOr : TPort  '^' TPort { }
TPortsSL : TPort  t_SL TPort { }
TPortsSR : TPort  t_SR TPort { }
TPortsSLImm : TPort  t_SL Expr { }
TPortsSRImm : TPort  t_SR Expr { }
TPortsEQ : TPort  t_EQ TPort { }
TPortsNE : TPort  t_NE TPort { }
TPortsGT : TPort  '>' TPort { }
TPortsGE : TPort  t_GE TPort { }
TPortsLT : TPort  '<' TPort { }
TPortsLE : TPort  t_LE TPort { }
TPortsMul : TPort  '*' TPort { }
TPortsCMul : TPort  t_CMul TPort { }
TPortsMulAdd : TPort '*' TPort '+' TPort  { }
TPortsAddSub : TPort  t_AddSub TPort { }

/* definition for triple operations between TPorts. */
TPortsComb : t_Comb '(' TPort ',' TPort ',' Expr ')' { }
| TPort ':' TPort t_By Expr {}

TPortsSelect : TPort '?' TPort ':' TPort  { }
TPortsAddSubMul : TPort t_AddSub TPort '*' TPort  { }

/* Note: instruction classification here is different from instrInfo.td, since each slot has
 * different encoding map.
 * Parsed asm operands are listed as: op0, op1, ..., opc, slot. */

NOP : t_NOP {
		/* an NOP indicates that all slots perform nothing. */
		InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::NOP));
    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(0));
	}

inst : t_Expr {
    const MCSymbolRefExpr *SE = dyn_cast<MCSymbolRefExpr>($1.expr);
    if (!SE)
      assert("Invalid symbol for HMacro invocation");
    else
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createMacro(SE->getSymbol()));
  }

/************************************** slot 0 ~ 3 ********************************/
| Shu '.' TPort '=' Mr0 Options {
	  InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::Mr0ToShu));
	  InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(0));
	}
| Shu '.' TPort '=' Mr1 Options {
    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::Mr1ToShu));
    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(1));
  }

| Calc '.' TPort IPort '=' Mr0 Options {
    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::Mr0ToCalc));
    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(0));
  }
| Calc '.' TPort IPort '=' Mr1 Options  {
    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::Mr1ToCalc));
    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(1));
  }
| Calc '.' TPort IPort '=' Mr2 Options {
    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::Mr2ToCalc));
    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(2));
  }
| FMAC '.' TPort IPort '=' Mr0 Options {
    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::Mr0ToCalc));
    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(0));
  }
| FMAC '.' TPort IPort '=' Mr1 Options  {
    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::Mr1ToCalc));
    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(1));
  }
| FMAC '.' TPort IPort '=' Mr2 Options {
    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::Mr2ToCalc));
    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(2));
  }

| Mw3 '=' Mr0  Options {
		InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::Mr0ToMw3));
		InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(0));
	}

| BIU '=' Mr3  Options {
		InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::Mr3ToBIU));
    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(3));
	}
| BIU '.' t_KG '=' Mr3  Options {
		InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::Mr3ToBIUKG));
    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(3));
	}

/************************************** slot 4 ~ 5 ********************************/
| Shu '.' TPort '=' TPortsIndx  Options {
    switch($1.reg) {
      case MMPUReg::Shu0 :
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::Shu0Indx));
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(4));
        break;
      case MMPUReg::Shu1 :
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::Shu1Indx));
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(5));
        break;
      default:
        llvm_unreachable("invalid operands for SHU at slot 4 or 5");
    }
	}

| Calc '.' TPort IPort '=' Shu '.' TPortsIndx Options  {
    switch($6.reg) {
      case MMPUReg::Shu0 :
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::Shu0IndxToCalc));
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(4));
        break;
      case MMPUReg::Shu1 :
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::Shu1IndxToCalc));
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(5));
        break;
      default:
        llvm_unreachable("invalid operands for SHU at slot 4 or 5");
    }
  }
| FMAC '.' TPort IPort '=' Shu '.' TPortsIndx Options  {
    switch($6.reg) {
      case MMPUReg::Shu0 :
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::Shu0IndxToCalc));
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(4));
        break;
      case MMPUReg::Shu1 :
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::Shu1IndxToCalc));
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(5));
        break;
      default:
        llvm_unreachable("invalid operands for SHU at slot 4 or 5");
    }
  }

| BIU '=' Shu '.' TPortsIndx  Options {
		switch($3.reg) {
			case MMPUReg::Shu0 :
				InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::Shu0IndxToBIU));
				InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(4));
				break;
			case MMPUReg::Shu1 :
				InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::Shu1IndxToBIU));
				InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(5));
				break;
			default:
				llvm_unreachable("invalid operands for SHU at slot 4 or 5");
		}
	}

| Mw3 '=' Shu '.' TPortsIndx  Options {
		switch($3.reg) {
			case MMPUReg::Shu0 :
				InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::Shu0IndxToMw3));
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(4));
        break;
			case MMPUReg::Shu1 :
				InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::Shu1IndxToMw3));
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(5));
        break;
			default:
				llvm_unreachable("invalid operands for SHU at slot 4 or 5");
		}
	}

| Shu '.' TPort '=' TPortsIndx '(' t_TB t_EAdd Expr ')'  Options {
		switch($1.reg) {
			case MMPUReg::Shu0 :
				InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::Shu0IndxWithImm));
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(4));
        break;
			case MMPUReg::Shu1 :
				InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::Shu1IndxWithImm));
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(5));
        break;
			default:
				llvm_unreachable("invalid operands for SHU at slot 4 or 5");
		}
	}
| Calc '.' TPort IPort '=' Shu '.' TPortsIndx '(' t_TB t_EAdd Expr ')'  Options {
    switch($6.reg) {
      case MMPUReg::Shu0 :
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::Shu0IndxWithImmToCalc));
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(4));
        break;
      case MMPUReg::Shu1 :
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::Shu1IndxWithImmToCalc));
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(5));
        break;
      default:
        llvm_unreachable("invalid operands for SHU at slot 4 or 5");
    }
  }
| FMAC '.' TPort IPort '=' Shu '.' TPortsIndx '(' t_TB t_EAdd Expr ')'  Options {
    switch($6.reg) {
      case MMPUReg::Shu0 :
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::Shu0IndxWithImmToCalc));
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(4));
        break;
      case MMPUReg::Shu1 :
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::Shu1IndxWithImmToCalc));
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(5));
        break;
      default:
        llvm_unreachable("invalid operands for SHU at slot 4 or 5");
    }
  }

| BIU '=' Shu '.' TPortsIndx '(' t_TB t_EAdd Expr ')'  Options {
		switch($3.reg) {
			case MMPUReg::Shu0 :
				InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::Shu0IndxWithImmToBIU));
		    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(4));
		    break;
			case MMPUReg::Shu1 :
				InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::Shu1IndxWithImmToBIU));
	      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(5));
	      break;
			default:
				llvm_unreachable("invalid operands for SHU at slot 4 or 5");
		}
	}

| Mw3 '=' Shu '.' TPortsIndx '(' t_TB t_EAdd Expr ')'  Options {
		switch($3.reg) {
			case MMPUReg::Shu0 :
				InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::Shu0IndxWithImmToMw3));
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(4));
        break;
			case MMPUReg::Shu1 :
				InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::Shu1IndxWithImmToMw3));
	      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(5));
	      break;
			default:
				llvm_unreachable("invalid operands for SHU at slot 4 or 5");
		}
	}

////////////////////// index TB //////////////////////
| Shu '.' TPort '=' TPort t_Indx t_TB  Options {
		switch($1.reg) {
			case MMPUReg::Shu0 :
				InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::Shu0IndxTB));
		    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(4));
		    break;
			case MMPUReg::Shu1 :
				InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::Shu1IndxTB));
			  InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(5));
			  break;
			default:
				llvm_unreachable("invalid operands for SHU at slot 4 or 5");
		}
	}

| Calc '.' TPort IPort '=' Shu '.' TPort t_Indx t_TB  Options {
    switch($6.reg) {
      case MMPUReg::Shu0 :
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::Shu0IndxTBToCalc));
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(4));
        break;
      case MMPUReg::Shu1 :
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::Shu1IndxTBToCalc));
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(5));
        break;
      default:
        llvm_unreachable("invalid operands for SHU at slot 4 or 5");
    }
  }
| FMAC '.' TPort IPort '=' Shu '.' TPort t_Indx t_TB  Options {
    switch($6.reg) {
      case MMPUReg::Shu0 :
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::Shu0IndxTBToCalc));
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(4));
        break;
      case MMPUReg::Shu1 :
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::Shu1IndxTBToCalc));
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(5));
        break;
      default:
        llvm_unreachable("invalid operands for SHU at slot 4 or 5");
    }
  }

| BIU '=' Shu '.' TPort t_Indx t_TB  Options {
		switch($3.reg) {
			case MMPUReg::Shu0 :
				InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::Shu0IndxTBToBIU));
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(4));
        break;
			case MMPUReg::Shu1 :
				InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::Shu1IndxTBToBIU));
		    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(5));
		    break;
			default:
				llvm_unreachable("invalid operands for SHU at slot 4 or 5");
		}
	}

| Mw3 '=' Shu '.' TPort t_Indx t_TB  Options {
		switch($3.reg) {
			case MMPUReg::Shu0 :
				InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::Shu0IndxTBToMw3));
		    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(4));
		    break;
			case MMPUReg::Shu1 :
				InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::Shu1IndxTBToMw3));
	      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(5));
	      break;
			default:
				llvm_unreachable("invalid operands for SHU at slot 4 or 5");
		}
	}

/////////////////////////////////////////////////////////////////////////////////
| Shu '.' TPort '=' TPort t_Indx t_TB '(' t_TB t_AddE Expr ')'  Options {
		switch($1.reg) {
			case MMPUReg::Shu0 :
				InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::Shu0IndxTBWithImm));
			  InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(4));
			  break;
			case MMPUReg::Shu1 :
				InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::Shu1IndxTBWithImm));
			  InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(5));
	      break;
			default:
				llvm_unreachable("invalid operands for SHU at slot 4 or 5");
		}
	}

| Calc '.' TPort IPort '=' Shu '.' TPort t_Indx t_TB '(' t_TB t_AddE Expr ')'  Options {
    switch($6.reg) {
      case MMPUReg::Shu0 :
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::Shu0IndxTBWithImmToCalc));
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(4));
        break;
      case MMPUReg::Shu1 :
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::Shu1IndxTBWithImmToCalc));
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(5));
        break;
      default:
        llvm_unreachable("invalid operands for SHU at slot 4 or 5");
    }
  }
| FMAC '.' TPort IPort '=' Shu '.' TPort t_Indx t_TB '(' t_TB t_AddE Expr ')'  Options {
    switch($6.reg) {
      case MMPUReg::Shu0 :
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::Shu0IndxTBWithImmToCalc));
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(4));
        break;
      case MMPUReg::Shu1 :
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::Shu1IndxTBWithImmToCalc));
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(5));
        break;
      default:
        llvm_unreachable("invalid operands for SHU at slot 4 or 5");
    }
  }

| BIU '=' Shu '.' TPort t_Indx t_TB '(' t_TB t_AddE Expr ')'  Options {
		switch($3.reg) {
			case MMPUReg::Shu0 :
				InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::Shu0IndxTBWithImmToBIU));
		    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(4));
		    break;
			case MMPUReg::Shu1 :
				InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::Shu1IndxTBWithImmToBIU));
	      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(5));
	      break;
			default:
				llvm_unreachable("invalid operands for SHU at slot 4 or 5");
		}
	}

| Mw3 '=' Shu '.' TPort t_Indx t_TB '(' t_TB t_AddE Expr ')'  Options {
		switch($3.reg) {
			case MMPUReg::Shu0 :
				InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::Shu0IndxTBWithImmToMw3));
		    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(4));
		    break;
			case MMPUReg::Shu1 :
				InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::Shu1IndxTBWithImmToMw3));
	      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(5));
	      break;
			default:
				llvm_unreachable("invalid operands for SHU at slot 4 or 5");
		}
	}

	/*************************************************************/
| Shu '.' TPort '='  TPortsComb  Options {
		switch($1.reg) {
			case MMPUReg::Shu0 :
				InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::Shu0Combine));
		    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(4));
		    break;
			case MMPUReg::Shu1 :
				InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::Shu1Combine));
	      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(5));
	      break;
			default:
				llvm_unreachable("invalid operands for SHU at slot 4 or 5");
		}
	}

| Calc '.' TPort IPort '=' Shu '.' TPortsComb  Options {
    switch($6.reg) {
      case MMPUReg::Shu0 :
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::Shu0CombineToCalc));
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(4));
        break;
      case MMPUReg::Shu1 :
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::Shu1CombineToCalc));
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(5));
        break;
      default:
        llvm_unreachable("invalid operands for SHU at slot 4 or 5");
    }
  }
| FMAC '.' TPort IPort '=' Shu '.' TPortsComb  Options {
    switch($6.reg) {
      case MMPUReg::Shu0 :
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::Shu0CombineToCalc));
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(4));
        break;
      case MMPUReg::Shu1 :
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::Shu1CombineToCalc));
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(5));
        break;
      default:
        llvm_unreachable("invalid operands for SHU at slot 4 or 5");
    }
  }

| BIU '=' Shu '.' TPortsComb  Options {
		switch($3.reg) {
			case MMPUReg::Shu0 :
				InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::Shu0CombineToBIU));
		    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(4));
		    break;
			case MMPUReg::Shu1 :
				InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::Shu1CombineToBIU));
	      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(5));
	      break;
			default:
				llvm_unreachable("invalid operands for SHU at slot 4 or 5");
		}
	}

| Mw3 '=' Shu '.' TPortsComb  Options {
		switch($3.reg) {
			case MMPUReg::Shu0 :
				InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::Shu0CombineToMw3));
		    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(4));
		    break;
			case MMPUReg::Shu1 :
				InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::Shu1CombineToMw3));
	      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(5));
	      break;
			default:
				llvm_unreachable("invalid operands for SHU at slot 4 or 5");
		}
	}

	/***************************************** IALU and FALU ******************************************/
	// Shu binary
| Shu '.' TPort '=' TPortsAdd Options {
    if($6.options & MMPU::c_S || $6.options & MMPU::c_D) {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUAddToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
    } else {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUAddToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
    }
  }
| Shu '.' TPort '=' TPortsSub Options {
    if($6.options & MMPU::c_S || $6.options & MMPU::c_D) {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUSubToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
    } else {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUSubToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
    }
  }
| Shu '.' TPort '=' TPortsEQ Options {
    if($6.options & MMPU::c_S || $6.options & MMPU::c_D) {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUEQToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
    } else {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUEQToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
    }
  }
| Shu '.' TPort '=' TPortsNE Options {
    if($6.options & MMPU::c_S || $6.options & MMPU::c_D) {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUNEToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
    } else {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUNEToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
    }
  }
| Shu '.' TPort '=' TPortsGT Options {
    if($6.options & MMPU::c_S || $6.options & MMPU::c_D) {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUGTToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
    } else {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUGTToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
    }
  }
| Shu '.' TPort '=' TPortsGE Options {
    if($6.options & MMPU::c_S || $6.options & MMPU::c_D) {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUGEToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
    } else {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUGEToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
    }
  }
| Shu '.' TPort '=' TPortsLT Options {
    if($6.options & MMPU::c_S || $6.options & MMPU::c_D) {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALULTToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
    } else {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALULTToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
    }
  }
| Shu '.' TPort '=' TPortsLE Options {
    if($6.options & MMPU::c_S || $6.options & MMPU::c_D) {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALULEToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
    } else {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALULEToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
    }
  }
| Shu '.' TPort '=' TPortsAnd Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUAndToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
  }
| Shu '.' TPort '=' TPortsOr Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUOrToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
  }
| Shu '.' TPort '=' TPortsXOr Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUXOrToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
  }
| Shu '.' TPort '=' TPortsSL Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUSLToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
  }
| Shu '.' TPort '=' TPortsSR Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUSRToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
  }
| Shu '.' TPort '=' TPortsSLImm Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUSLImmToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
  }
| Shu '.' TPort '=' TPortsSRImm Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUSRImmToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
  }

// Calc binary, including only IALU, IMAC and FALU.
| Calc '.' TPort '=' TPortsAdd Options {
    if($6.options & MMPU::c_S || $6.options & MMPU::c_D) {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUAddToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
    } else {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUAddToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
    }
  }
| Calc '.' TPort '=' TPortsSub Options {
    if($6.options & MMPU::c_S || $6.options & MMPU::c_D) {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUSubToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
    } else {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUSubToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
    }
  }
| Calc '.' TPort '=' TPortsEQ Options {
    if($6.options & MMPU::c_S || $6.options & MMPU::c_D) {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUEQToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
    } else {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUEQToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
    }
  }
| Calc '.' TPort '=' TPortsNE Options {
    if($6.options & MMPU::c_S || $6.options & MMPU::c_D) {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUNEToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
    } else {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUNEToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
    }
  }
| Calc '.' TPort '=' TPortsGT Options {
    if($6.options & MMPU::c_S || $6.options & MMPU::c_D) {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUGTToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
    } else {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUGTToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
    }
  }
| Calc '.' TPort '=' TPortsGE Options {
    if($6.options & MMPU::c_S || $6.options & MMPU::c_D) {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUGEToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
    } else {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUGEToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
    }
  }
| Calc '.' TPort '=' TPortsLT Options {
    if($6.options & MMPU::c_S || $6.options & MMPU::c_D) {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALULTToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
    } else {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALULTToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
    }
  }
| Calc '.' TPort '=' TPortsLE Options {
    if($6.options & MMPU::c_S || $6.options & MMPU::c_D) {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALULEToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
    } else {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALULEToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
    }
  }
| Calc '.' TPort '=' TPortsAnd Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUAndToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
  }
| Calc '.' TPort '=' TPortsOr Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUOrToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
  }
| Calc '.' TPort '=' TPortsXOr Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUXOrToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
  }
| Calc '.' TPort '=' TPortsSL Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUSLToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
  }
| Calc '.' TPort '=' TPortsSR Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUSRToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
  }
| Calc '.' TPort '=' TPortsSLImm Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUSLImmToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
  }
| Calc '.' TPort '=' TPortsSRImm Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUSRImmToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
  }

  // Instructions which transfer result to FMAC must be FALU instructions.
| FMAC '.' TPort '=' TPortsAdd Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUAddToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
  }
| FMAC '.' TPort '=' TPortsSub Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUSubToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
  }
| FMAC '.' TPort '=' TPortsEQ Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUEQToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
  }
| FMAC '.' TPort '=' TPortsNE Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUNEToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
  }
| FMAC '.' TPort '=' TPortsGT Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUGTToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
  }
| FMAC '.' TPort '=' TPortsGE Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUGEToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
  }
| FMAC '.' TPort '=' TPortsLT Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALULTToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
  }
| FMAC '.' TPort '=' TPortsLE Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALULEToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
  }

  // MReg binary
| Mw3 '=' TPortsAdd Options {
    if($4.options & MMPU::c_S || $4.options & MMPU::c_D) {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUAddToMw3));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
    } else {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUAddToMw3));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
    }
  }
| Mw3 '=' TPortsSub Options {
    if($4.options & MMPU::c_S || $4.options & MMPU::c_D) {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUSubToMw3));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
    } else {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUSubToMw3));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
    }
  }

  ///////////////// compare ////////////////////
| Mw3 '=' TPortsEQ Options {
    if($4.options & MMPU::c_S || $4.options & MMPU::c_D) {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUEQToMw3));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
    } else {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUEQToMw3));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
    }
  }
| Mw3 '=' TPortsNE Options {
    if($4.options & MMPU::c_S || $4.options & MMPU::c_D) {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUNEToMw3));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
    } else {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUNEToMw3));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
    }
  }
| Mw3 '=' TPortsGT Options {
    if($4.options & MMPU::c_S || $4.options & MMPU::c_D) {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUGTToMw3));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
    } else {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUGTToMw3));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
    }
  }
| Mw3 '=' TPortsGE Options {
    if($4.options & MMPU::c_S || $4.options & MMPU::c_D) {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUGEToMw3));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
    } else {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUGEToMw3));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
    }
  }
| Mw3 '=' TPortsLT Options {
    if($4.options & MMPU::c_S || $4.options & MMPU::c_D) {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALULTToMw3));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
    } else {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALULTToMw3));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
    }
  }
| Mw3 '=' TPortsLE Options {
    if($4.options & MMPU::c_S || $4.options & MMPU::c_D) {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALULEToMw3));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
    } else {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALULEToMw3));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
    }
  }
  ////////////////////////////////////////////////

///////////////// cond ////////////////////
| t_COND '=' TPortsEQ Options {

    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUCondEQ));
    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
}
| t_COND '=' TPortsNE Options {

    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUCondNE));
    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
}
| t_COND '=' TPortsGT Options {

    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUCondGT));
    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
}
| t_COND '=' TPortsGE Options {

    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUCondGE));
    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
}
| t_COND '=' TPortsLT Options {

    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUCondLT));
    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
}
| t_COND '=' TPortsLE Options {
    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUCondLE));
    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
}
/////////////////////////////////////////////////////

| Mw3 '=' TPortsAnd Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUAndToMw3));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
  }
| Mw3 '=' TPortsOr Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUOrToMw3));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
  }
| Mw3 '=' TPortsXOr Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUXOrToMw3));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
  }
| Mw3 '=' TPortsSL Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUSLToMw3));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
  }
| Mw3 '=' TPortsSR Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUSRToMw3));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
  }
| Mw3 '=' TPortsSLImm Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUSLImmToMw3));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
  }
| Mw3 '=' TPortsSRImm Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUSRImmToMw3));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
  }

	// BIU binary
| BIU '=' TPortsAdd Options {
    if($4.options & MMPU::c_S || $4.options & MMPU::c_D) {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUAddToBIU));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
    } else {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUAddToBIU));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
    }
  }
| BIU '=' TPortsSub Options {
    if($4.options & MMPU::c_S || $4.options & MMPU::c_D) {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUSubToBIU));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
    } else {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUSubToBIU));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
    }
  }
| BIU '=' TPortsEQ Options {
    if($4.options & MMPU::c_S || $4.options & MMPU::c_D) {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUEQToBIU));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
    } else {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUEQToBIU));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
    }
  }
| BIU '=' TPortsNE Options {
    if($4.options & MMPU::c_S || $4.options & MMPU::c_D) {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUNEToBIU));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
    } else {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUNEToBIU));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
    }
  }
| BIU '=' TPortsGT Options {
    if($4.options & MMPU::c_S || $4.options & MMPU::c_D) {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUGTToBIU));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
    } else {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUGTToBIU));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
    }
  }
| BIU '=' TPortsGE Options {
    if($4.options & MMPU::c_S || $4.options & MMPU::c_D) {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUGEToBIU));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
    } else {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUGEToBIU));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
    }
  }
| BIU '=' TPortsLT Options {
    if($4.options & MMPU::c_S || $4.options & MMPU::c_D) {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALULTToBIU));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
    } else {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALULTToBIU));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
    }
  }
| BIU '=' TPortsLE Options {
    if($4.options & MMPU::c_S || $4.options & MMPU::c_D) {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALULEToBIU));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
    } else {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALULEToBIU));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
    }
  }
| BIU '=' TPortsAnd Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUAndToBIU));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
  }
| BIU '=' TPortsOr Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUOrToBIU));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
  }
| BIU '=' TPortsXOr Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUXOrToBIU));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
  }
| BIU '=' TPortsSL Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUSLToBIU));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
  }
| BIU '=' TPortsSR Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUSRToBIU));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
  }
| BIU '=' TPortsSLImm Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUSLImmToBIU));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
  }
| BIU '=' TPortsSRImm Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUSRImmToBIU));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
  }

  // +- in FALU
| Shu '.' TPort '=' TPortsAddSub Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUAddSubToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
  }
| Calc '.' TPort '=' TPortsAddSub Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUAddSubToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
  }

| FMAC '.' TPort '=' TPortsAddSub Options {
    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUAddSubToUnit));
    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
}
| Mw3 '=' TPortsAddSub Options {
    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUAddSubToMw3));
    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
}
| BIU '=' TPortsAddSub Options {
    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUAddSubToBIU));
    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
}

  /////////////////////////////////////////
	// wired binary: max, min.
	// (1) Shu
| Shu '.' TPort '=' TPortsMax Options {
  if($6.options & MMPU::c_S || $6.options & MMPU::c_D) {
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUMaxToUnit));
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
  }
  else {
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUMaxToUnit));
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
  }
}

// (2) Calc
| Calc '.' TPort '=' TPortsMax Options {
  if($6.options & MMPU::c_S || $6.options & MMPU::c_D) {
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUMaxToUnit));
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
  }
  else {
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUMaxToUnit));
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
  }
}

| FMAC '.' TPort '=' TPortsMax Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUMaxToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
}

	// (3) MReg
| Mw3 '=' TPortsMax Options {
  if($4.options & MMPU::c_S || $4.options & MMPU::c_D) {
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUMaxToMw3));
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
  }
  else {
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUMaxToMw3));
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
  }
}
	// (4) BIU
| BIU '=' TPortsMax Options {
		  if($4.options & MMPU::c_S || $4.options & MMPU::c_D) {
		        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUMaxToBIU));
		        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
		  }
		  else {
		        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUMaxToBIU));
		        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
		  }
	}

  // min
| Shu '.' TPort '=' TPortsMin Options {
  if($6.options & MMPU::c_S || $6.options & MMPU::c_D) {
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUMinToUnit));
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
  }
  else {
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUMinToUnit));
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
  }
}

// (2) Calc
| Calc '.' TPort '=' TPortsMin Options {
  if($6.options & MMPU::c_S || $6.options & MMPU::c_D) {
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUMinToUnit));
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
  }
  else {
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUMinToUnit));
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
  }
}

| FMAC '.' TPort '=' TPortsMin Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUMinToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
}

  // (3) MReg
| Mw3 '=' TPortsMin Options {
  if($4.options & MMPU::c_S || $4.options & MMPU::c_D) {
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUMinToMw3));
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
  }
  else {
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUMinToMw3));
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
  }
}
  // (4) BIU
| BIU '=' TPortsMin Options {
      if($4.options & MMPU::c_S || $4.options & MMPU::c_D) {
            InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUMinToBIU));
            InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
      }
      else {
            InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUMinToBIU));
            InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
      }
  }

// compress
| Shu '.' TPort '=' TPortsCprs Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUCprsToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
}

// (2) Calc
| Calc '.' TPort '=' TPortsCprs Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUCprsToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
}
// (3) MReg
| Mw3 '=' TPortsCprs Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUCprsToMw3));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
}
// (4) BIU
| BIU '=' TPortsCprs Options {
          InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUCprsToBIU));
          InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
}

	//// Sub-abs for IALU
| Shu '.' TPort '=' TPortsAbsSub Options {
		InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUSubAbsToUnit));
		InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
	}
//// Sub-abs
| Calc '.' TPort '=' TPortsAbsSub Options {
  InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUSubAbsToUnit));
  InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
}

| Mw3 '=' TPortsAbsSub Options {
		InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUSubAbsToMw3));
		InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
	}

| BIU '=' TPortsAbsSub Options {
		InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUSubAbsToBIU));
		InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
	}

  /// ?:
| Shu '.' TPort '=' TPortsSelect  Options {
    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUSelectToUnit));
    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
  }
| Calc '.' TPort '=' TPortsSelect  Options {
  InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUSelectToUnit));
  InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
}

| Mw3 '=' TPortsSelect  Options {
    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUSelectToMw3));
    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
  }

| BIU '=' TPortsSelect  Options {
    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUSelectToBIU));
    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
  }

  // Unary operations for ialu and falu
| Shu '.' TPort '=' TPortRecip Options {
		  InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALURecipToUnit));
		  InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
  }
| Shu '.' TPort '=' TPortRSqRt Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALURSqRtToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
  }
| Shu '.' TPort '=' TPortInt Options {
      if ($6.options & MMPU::c_S)
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUIntSglToUnit));
      else if ($6.options & MMPU::c_D)
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUIntDblToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
  }
| Shu '.' TPort '=' TPortExtInt Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUIntDblToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
  }
| Shu '.' TPort '=' TPortUInt Options {
      if ($6.options & MMPU::c_S)
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUUIntSglToUnit));
      else if ($6.options & MMPU::c_D)
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUUIntDblToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
  }
| Shu '.' TPort '=' TPortExtUInt Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUUIntDblToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
  }
| Shu '.' TPort '=' TPortBitNot  Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUBitNotToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
  }
| Shu '.' TPort '=' TPortRMax Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALURMaxToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
  }
| Shu '.' TPort '=' TPortRMin Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALURMinToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
  }
| Shu '.' TPortExt '=' TPortExpd Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUExpdToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
  }
| Shu '.' TPort '=' TPortSgl Options {
      if ($6.options & MMPU::c_D)
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUSglDblToUnit));
      else
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUSglIntToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
  }
| Shu '.' TPort '=' TPortExtSgl Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUSglDblToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
  }
| Shu '.' TPort '=' TPortDbl Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUDblToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
  }

| Calc '.' TPort '=' TPortRecip Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALURecipToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
  }
| Calc '.' TPort '=' TPortRSqRt Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALURSqRtToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
  }
| Calc '.' TPort '=' TPortInt Options {
      if ($6.options & MMPU::c_S)
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUIntSglToUnit));
      else if ($6.options & MMPU::c_D)
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUIntDblToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
  }
| Calc '.' TPort '=' TPortExtInt Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUIntDblToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
  }
| Calc '.' TPort '=' TPortUInt Options {
      if ($6.options & MMPU::c_S)
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUUIntSglToUnit));
      else if ($6.options & MMPU::c_D)
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUUIntDblToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
  }
| Calc '.' TPort '=' TPortExtUInt Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUUIntDblToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
  }
| Calc '.' TPort '=' TPortSgl Options {
      if ($6.options & MMPU::c_D)
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUSglDblToUnit));
      else
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUSglIntToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
  }
| Calc '.' TPort '=' TPortExtSgl Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUSglDblToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
  }
| Calc '.' TPort '=' TPortDbl Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUDblToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
  }

  // Instructions which transfer result to FMAC must be FALU instructions.
| FMAC '.' TPort '=' TPortRecip Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALURecipToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
  }
| FMAC '.' TPort '=' TPortRSqRt Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALURSqRtToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
  }
| FMAC '.' TPort '=' TPortInt Options {
      if ($6.options & MMPU::c_S)
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUIntSglToUnit));
      else if ($6.options & MMPU::c_D)
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUIntDblToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
  }
| FMAC '.' TPort '=' TPortExtInt Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUIntDblToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
  }
| FMAC '.' TPort '=' TPortUInt Options {
      if ($6.options & MMPU::c_S)
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUUIntSglToUnit));
      else if ($6.options & MMPU::c_D)
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUUIntDblToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
  }
| FMAC '.' TPort '=' TPortExtUInt Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUUIntDblToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
  }
| FMAC '.' TPort '=' TPortSgl Options {
      if ($6.options & MMPU::c_D)
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUSglDblToUnit));
      else
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUSglIntToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
  }
| FMAC '.' TPort '=' TPortExtSgl Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUSglDblToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
  }
| FMAC '.' TPort '=' TPortDbl Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUDblToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
  }

| Calc '.' TPort '=' TPortBitNot  Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUBitNotToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
  }
| Calc '.' TPort '=' TPortRMax Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALURMaxToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
  }
| Calc '.' TPort '=' TPortRMin Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALURMinToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
  }
| Calc '.' TPortExt '=' TPortExpd Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUExpdToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
  }

| Mw3 '=' TPortRecip Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALURecipToMw3));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
  }
| Mw3 '=' TPortRSqRt Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALURSqRtToMw3));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
  }
| Mw3 '=' TPortInt Options {
      if ($4.options & MMPU::c_S)
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUIntSglToMw3));
      else if ($4.options & MMPU::c_D)
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUIntDblToMw3));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
  }
| Mw3 '=' TPortExtInt Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUIntDblToMw3));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
  }
| Mw3 '=' TPortUInt Options {
      if ($4.options & MMPU::c_S)
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUUIntSglToMw3));
      else if ($4.options & MMPU::c_D)
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUUIntDblToMw3));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
  }
| Mw3 '=' TPortExtUInt Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUUIntDblToMw3));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
  }
| Mw3 '=' TPortBitNot  Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUBitNotToMw3));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
  }
| Mw3 '=' TPortRMax Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALURMaxToMw3));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
  }
| Mw3 '=' TPortRMin Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALURMinToMw3));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
  }
| MExtw3 '=' TPortExpd Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUExpdToMw3));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
  }
| Mw3 '=' TPortSgl Options {
      if ($4.options & MMPU::c_D)
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUSglDblToMw3));
      else
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUSglIntToMw3));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
  }
| Mw3 '=' TPortExtSgl Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUSglDblToMw3));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
  }
| Mw3 '=' TPortDbl Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUDblToMw3));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
  }

| BIU '=' TPortRecip Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALURecipToBIU));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
  }
| BIU '=' TPortRSqRt Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALURSqRtToBIU));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
  }
| BIU '=' TPortInt Options {
      if ($4.options & MMPU::c_S)
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUIntSglToBIU));
      else if ($4.options & MMPU::c_D)
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUIntDblToBIU));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
  }
| BIU '=' TPortExtInt Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUIntDblToBIU));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
  }
| BIU '=' TPortUInt Options {
      if ($4.options & MMPU::c_S)
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUUIntSglToBIU));
      else if ($4.options & MMPU::c_D)
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUUIntDblToBIU));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
  }
| BIU '=' TPortExtUInt Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUUIntDblToBIU));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
  }
| BIU '=' TPortBitNot  Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUBitNotToBIU));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
  }
| BIU '=' TPortRMax Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALURMaxToBIU));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
  }
| BIU '=' TPortRMin Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALURMinToBIU));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
  }
| BIU '=' TPortExpd Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUExpdToBIU));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
  }
| BIU '=' TPortSgl Options {
      if ($4.options & MMPU::c_D)
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUSglDblToBIU));
      else
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUSglIntToBIU));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
  }
| BIU '=' TPortExtSgl Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUSglDblToBIU));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
  }
| BIU '=' TPortDbl Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUDblToBIU));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
  }

	// abs of falu
| Shu '.' TPort '=' TPortAbs Options {
		InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUAbsToUnit));
		InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
	}
// abs of falu
| Calc '.' TPort '=' TPortAbs Options {
    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUAbsToUnit));
    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
  }
| FMAC '.' TPort '=' TPortAbs Options {
    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUAbsToUnit));
    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
  }
| Mw3 '=' TPortAbs Options {
		InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUAbsToMw3));
		InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
	}
| BIU '=' TPortAbs Options {
		InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FALUAbsToBIU));
		InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(8));
	}

| t_DivR '=' TPort t_DivS TPort Options {
		InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUDivS));
		InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
	}

| t_DivR '=' t_DivQ  Options {
		InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUDivQ));
		InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
	}

| t_DivR '=' t_MDivR  Options {
		InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUMDivR));
		InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
	}

| t_DivR '=' t_MDivQ  Options {
		InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUMDivQ));
		InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
	}

| Shu '.' TPortExt '=' t_DivR  Options {
		InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUDivRToUnit));
		InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
	}

| Calc '.' TPortExt '=' t_DivR  Options {
    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUDivRToUnit));
    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
  }

| MExtw3 '=' t_DivR  Options {
		InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUDivRToMw3));
		InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
	}

| BIU '=' t_DivR  Options {
		InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IALUDivRToBIU));
		InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(6));
	}

	/************************************ MAC Slots (7, 9) *****************************/
	//------- MUL -------
| Mw3 '=' TPortsMul Options {
		if ($4.options & (MMPU::c_S)  || $4.options & (MMPU::c_D)) {
			InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FMACMulToMw3));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(9));
		}
		else {
			InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IMACMulToMw3));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(7));
		}
	}

| MExtw3 '=' TPortsMul Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IMACMulExtToMw3));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(7));
  }

| Mw3 '=' t_MR t_AddE TPortsMul Options {
    if ($6.options & (MMPU::c_S)  || $6.options & (MMPU::c_D)) {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FMACMulAccOnMRXferToMw3));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(9));
    }
    else {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IMACMulAccOnMRXferToMw3));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(7));
    }
  }

| Mw3 '=' t_MR t_AddE TPort Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IMACAccOnMRXferToMw3));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(7));
  }

| MExtw3 '=' t_MR t_AddE TPortsMul Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IMACMulExtAccOnMRXferToMw3));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(7));
  }

| Mw3 '=' TPortsCMul Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FMACComplexMulToMw3));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(9));
  }

| BIU '=' TPortsMul Options {
	  if ($4.options & (MMPU::c_S)  || $4.options & (MMPU::c_D)) {
	    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FMACMulToBIU));
	    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(9));
	  }
	  else {
	    if($4.options & (MMPU::c_T))
	      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IMACMulToBIU));
	    else
	      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IMACMulExtToBIU));
	    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(7));
	  }
	}

| BIU '=' t_MR t_AddE TPortsMul Options {
    if ($6.options & (MMPU::c_S)  || $6.options & (MMPU::c_D)) {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FMACMulAccOnMRXferToBIU));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(9));
    }
    else {
      if($6.options & (MMPU::c_L))
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IMACMulExtAccOnMRXferToBIU));
      else
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IMACMulAccOnMRXferToBIU));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(7));
    }
  }

| BIU '=' t_MR t_AddE TPort Options {
    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IMACAccOnMRXferToBIU));
    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(7));
  }

| BIU '=' TPortsCMul Options {
    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FMACComplexMulToBIU));
    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(9));
  }

| Shu '.' TPort '=' TPortsMul Options {
    if ($6.options & (MMPU::c_S)  || $6.options & (MMPU::c_D)) {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FMACMulToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(9));
    }
    else {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IMACMulToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(7));
    }
	}

| Shu '.' TPortExt '=' TPortsMul Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IMACMulExtToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(7));
  }

| Shu '.' TPort '=' t_MR t_AddE TPortsMul Options {
    if ($8.options & (MMPU::c_S)  || $8.options & (MMPU::c_D)) {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FMACMulAccOnMRXferToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(9));
    }
    else {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IMACMulAccOnMRXferToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(7));
    }
  }

| Shu '.' TPortExt '=' t_MR t_AddE TPortsMul Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IMACMulExtAccOnMRXferToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(7));
  }

| Shu '.' TPort '=' t_MR t_AddE TPort Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IMACAccOnMRXferToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(7));
  }

| Shu '.' TPort '=' TPortsCMul Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FMACComplexMulToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(9));
  }

| Calc '.' TPort '=' TPortsMul Options {
    if ($6.options & (MMPU::c_S)  || $6.options & (MMPU::c_D)) {
      if($1.reg != MMPUReg::FALU) // the result operand of FMAC passes only to FALU
      {
        MCAsmParser* Parser = llvm::MMPU::MMPUAsmParser::getMCParser();
        SMLoc Loc = SMLoc::getFromPointer($1.pos);
        Parser->Error(Loc, "Only FALU or FMAC is legal to receive this result-operand.");
      }

      // we always generate this instruction to avoid llvm cracking.
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FMACMulToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(9));
    }
    else {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IMACMulToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(7));
    }
  }

| FMAC '.' TPort '=' TPortsMul Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FMACMulToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(9));
  }

| Calc '.' TPortExt '=' TPortsMul Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IMACMulExtToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(7));
  }

| Calc '.' TPort '=' t_MR t_AddE TPortsMul Options {
    if ($8.options & (MMPU::c_S)  || $8.options & (MMPU::c_D)) {
      if($1.reg != MMPUReg::FALU) // the result operand of FMAC passes only to FALU
      {
        MCAsmParser* Parser = llvm::MMPU::MMPUAsmParser::getMCParser();
        SMLoc Loc = SMLoc::getFromPointer($1.pos);
        Parser->Error(Loc, "Only FALU or FMAC is legal to receive this result-operand.");
      }

      // we always generate this instruction to avoid llvm cracking.
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FMACMulAccOnMRXferToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(9));
    }
    else {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IMACMulAccOnMRXferToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(7));
    }
  }

| FMAC '.' TPort '=' t_MR t_AddE TPortsMul Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FMACMulAccOnMRXferToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(9));
  }

| Calc '.' TPortExt '=' t_MR t_AddE TPortsMul Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IMACMulExtAccOnMRXferToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(7));
  }

| Calc '.' TPort '=' t_MR t_AddE TPort Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IMACAccOnMRXferToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(7));
  }

| Calc '.' TPort '=' TPortsCMul Options {
      if($1.reg != MMPUReg::FALU) // the result operand of FMAC passes only to FALU
      {
        MCAsmParser* Parser = llvm::MMPU::MMPUAsmParser::getMCParser();
        SMLoc Loc = SMLoc::getFromPointer($1.pos);
        Parser->Error(Loc, "Only FALU or FMAC is legal to receive this result-operand.");
      }

      // we always generate this instruction to avoid llvm cracking.
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FMACComplexMulToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(9));
  }

| FMAC '.' TPort '=' TPortsCMul Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FMACComplexMulToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(9));
  }

	//-------MULADD-------
| t_MR '=' TPortsMulAdd Options {
    if ($4.options & (MMPU::c_S)  || $4.options & (MMPU::c_D)) {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FMACMulAddToMR));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(9));
    }
    else {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IMACMulAddToMR));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(7));
    }
	}

	//-------MAC-------
| t_MR t_AddE TPortsMul Options {
    if ($4.options & (MMPU::c_S)  || $4.options & (MMPU::c_D)) {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::FMACMulAccOnMR));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(9));
    }
    else {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IMACMulAccOnMR));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(7));
    }
	}

	//------- ACC, only for IMAC --------
| t_MR t_AddE TPort Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IMACAccOnMR));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(7));
	}

//-------AddSubMul--------
| Mw3 '=' TPortsAddSubMul Options {
    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IMACAddSubMulToMw3));
    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(7));
	}

| BIU '=' TPortsAddSubMul Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IMACAddSubMulToBIU));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(7));
  }

| Shu '.' TPort '=' TPortsAddSubMul Options {
    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IMACAddSubMulToUnit));
    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(7));
  }

| Calc '.' TPort '=' TPortsAddSubMul Options {
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::IMACAddSubMulToUnit));
      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(7));
  }

/*************************** slot 10 ~ 12, BIU instructions **********************************/
| Shu '.' TPort '=' t_BIU '.' t_DM Options {
		unsigned right = $5.reg;
		switch(right) {
			case MMPUReg::BIU0:
				InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::BIU0LdToShu));
			  InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(10));
				break;
			case MMPUReg::BIU1:
				InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::BIU1LdToShu));
		    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(11));
		    break;
			case MMPUReg::BIU2:
				InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::BIU2LdToShu));
	      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(12));
	      break;
			default:
				llvm_unreachable("invalid operand for BIU instructions, must be BIUx");
		}
	}

| Calc '.' TPort IPort '=' t_BIU '.' t_DM Options {
    unsigned right = $6.reg;
    switch(right) {
      case MMPUReg::BIU0:
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::BIU0LdToCalc));
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(10));
        break;
      case MMPUReg::BIU1:
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::BIU1LdToCalc));
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(11));
        break;
      case MMPUReg::BIU2:
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::BIU2LdToCalc));
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(12));
        break;
      default:
        llvm_unreachable("invalid operand for BIU instructions, must be BIUx");
    }
  }

| FMAC '.' TPort IPort '=' t_BIU '.' t_DM Options {
    unsigned right = $6.reg;
    switch(right) {
      case MMPUReg::BIU0:
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::BIU0LdToCalc));
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(10));
        break;
      case MMPUReg::BIU1:
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::BIU1LdToCalc));
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(11));
        break;
      case MMPUReg::BIU2:
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::BIU2LdToCalc));
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(12));
        break;
      default:
        llvm_unreachable("invalid operand for BIU instructions, must be BIUx");
    }
  }

| MReg '.' t_WPort '=' t_BIU '.' t_DM Options {
		unsigned w = $3.reg;
		unsigned right = $5.reg;
		switch(right) {
			case MMPUReg::BIU0:
			  assert(w == MMPUReg::w0 && "invalid operand for BIU instructions, must be w0");
				InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::BIU0LdToMw0));
	      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(10));
				break;
			case MMPUReg::BIU1:
			  assert(w == MMPUReg::w1 && "invalid operand for BIU instructions, must be w1");
				InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::BIU1LdToMw1));
        InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(11));
        break;
			case MMPUReg::BIU2:
			  assert(w == MMPUReg::w2 && "invalid operand for BIU instructions, must be w2");
				InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::BIU2LdToMw2));
	      InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(12));
	      break;
			default:
				llvm_unreachable("invalid operand for BIU instructions, must be BIUx");
		}
	}

| t_DM '=' t_BIU Options {
		unsigned right = $3.reg;
		switch(right) {
			case MMPUReg::BIU0:
				InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::BIU0StToDM));
		    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(10));
		    break;
			case MMPUReg::BIU1:
				InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::BIU1StToDM));
				InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(11));
				break;
			case MMPUReg::BIU2:
				InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::BIU2StToDM));
				InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(12));
				break;
			default:
				llvm_unreachable("invalid operand for BIU instructions, must be BIUx");
		}
	}

| MReg '.' t_WPort '=' t_BIU '.' t_KG  Options {
		unsigned w = $3.reg;
		unsigned right = $5.reg;
		switch(right) {
			case MMPUReg::BIU0:
			  assert(w == MMPUReg::w0 && "invalid operand for BIU instructions, must be w0");
				InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::BIU0MovToMw0));
			  InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(10));
			  break;
			case MMPUReg::BIU1:
			  assert(w == MMPUReg::w1 && "invalid operand for BIU instructions, must be w1");
				InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::BIU1MovToMw1));
			  InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(11));
			  break;
			case MMPUReg::BIU2:
			  assert(w == MMPUReg::w2 && "invalid operand for BIU instructions, must be w2");
				InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::BIU2MovToMw2));
			  InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(12));
			  break;
			default:
				llvm_unreachable("invalid operand for BIU instructions, must be BIUx");
		}
	}

	/*****************************************************************************/
	// slot 13: branch instructions
	/*****************************************************************************/
| t_Repeat t_At '(' Expr ')' {
	  InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::RepeatImm));
	  InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(13));
	}

| t_Repeat t_At '(' t_KIReg ')' {
    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createImm( 0));
    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createImm($4.options));
	  InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::RepeatConds));
	  InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(13));
	}

| t_Repeat t_At '(' t_KIReg '-' { mmpuInstParserNeedExpr=true; } Expr ')' {
    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createImm($4.options));
    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::RepeatConds));
    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(13));
  }

| Loop  Expr t_At '(' t_KIReg ')' {
    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createImm( 0));
    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createImm($5.options));
    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::Loop));
    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(13));
  }

| Loop  Expr t_At '(' t_KIReg '-' { mmpuInstParserNeedExpr=true; } Expr ')' {
    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createImm($5.options));
    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::Loop));
    InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(13));
  }

| Stop {
		InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createOpc(MMPUInst::MPUStop));
		InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createSlot(13));
	}

Loop : t_Loop { mmpuInstParserNeedExpr=true; }

Stop : t_MPU '.' t_Stop {}
| t_Stop {}

/******************************************************************************
 * instruction options which can be applied to any instruction *syntactically*.
 ******************************************************************************/
Option_ : o_KPP { $$.options = MMPU::c_KPP; $$.pos = $1.pos; }
| o_APP         { $$.options = MMPU::c_APP; $$.pos = $1.pos; }
| o_BR          { $$.options = MMPU::c_BR;  $$.pos = $1.pos; }
| o_M           { $$.options = MMPU::c_M;   $$.pos = $1.pos; }
| o_T           { $$.options = MMPU::c_T;   $$.pos = $1.pos; }
| o_U           { $$.options = MMPU::c_U;   $$.pos = $1.pos; }
| o_B           { $$.options = MMPU::c_B;   $$.pos = $1.pos; }
| o_H           { $$.options = MMPU::c_H;   $$.pos = $1.pos; }
| o_S           { $$.options = MMPU::c_S;   $$.pos = $1.pos; }
| o_D           { $$.options = MMPU::c_D;   $$.pos = $1.pos; }
| o_IND         { $$.options = MMPU::c_IND; $$.pos = $1.pos; }
| o_C           { $$.options = MMPU::c_C;   $$.pos = $1.pos; }
| o_NC          { $$.options = MMPU::c_NC;  $$.pos = $1.pos; }
| o_L           { $$.options = MMPU::c_L;   $$.pos = $1.pos; }
| o_CR          { $$.options = MMPU::c_CR;  $$.pos = $1.pos; }
| o_I           { $$.options = MMPU::c_I;   $$.pos = $1.pos; }
| o_TC          { $$.options = MMPU::c_TC;  $$.pos = $1.pos; }
| o_WF1         { $$.options = MMPU::c_WF1; $$.pos = $1.pos; }
| o_WF2         { $$.options = MMPU::c_WF2; $$.pos = $1.pos; }
| o_WF3         { $$.options = MMPU::c_WF3; $$.pos = $1.pos; }
| o_WX1         { $$.options = MMPU::c_WX1; $$.pos = $1.pos; }
| o_WX2         { $$.options = MMPU::c_WX2; $$.pos = $1.pos; }
| o_WX3         { $$.options = MMPU::c_WX3; $$.pos = $1.pos; }
| o_WX4         { $$.options = MMPU::c_WX4; $$.pos = $1.pos; }

Options_ :          { $$.options = 0; $$.pos = MMPU::MMPUAsmParser::getCurPosition(); }
| Options_ Option_  { if($1.options == 0) { $$.pos = $2.pos; } $$.options |= $2.options; }

Options : Options_ {$$.options = $1.options; InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createImm($1.options));
                                             InstLine.Operands.push_back(MMPU::MMPUAsmOperand::createLoc($1.pos));}

%%
