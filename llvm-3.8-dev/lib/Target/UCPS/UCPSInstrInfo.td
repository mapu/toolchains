//===-- UCPSInstInfo.td - Target Description for UCPS Target -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file describes the UCPS instructions in TableGen format.
//
//===----------------------------------------------------------------------===//

include "UCPSInstrFormats.td"

//===----------------------------------------------------------------------===//
//			Instruction NOP for all slots
//===----------------------------------------------------------------------===//
// CallM specific, PC-irrelative address
def UImm12Ptr : Operand<iPTR> {
  let EncoderMethod = "getUImm12Encoding";
  //let DecoderMethod = "DecodeUImm12";
  //let PrintMethod = "printImmExt";
}
// branch target, PC-relative address
def SImm18Ptr : Operand<iPTR> {
  let EncoderMethod = "getSImm18Encoding";
  //let DecoderMethod = "DecodeSImm18";
  //let PrintMethod = "printImmExt";
}




// dcx SYN start -------------------------------------------------------
def CallMFlag : Operand<i32> {
  //let PrintMethod = "printCallMFlag";
}
  
class SynInst< dag outs, dag ins, string asm, bits<3> opcode,
	  		   	list<dag> pattern = [], InstrItinClass itin = NoItinerary> 
	  	: UCPSInstr < 0b011, outs, ins, asm, pattern > {
  let Inst{27-25} = opcode;
}

def CallMImm : SynInst< 
			 (outs), 
			 (ins RReg:$rs, UImm12Ptr:$i, CallMFlag:$f),
			 "$rs=CallM $i$f", 0b000>
{
	
	bits<5> rs;
	bits<12> i;
	bit f;
	let Inst{23} = 1; // allow imm ext
	let Inst{22} = f; 
	let Inst{16-5} = i;
	let Inst{4-0} = rs;			 
}

def CallMJReg : SynInst< 
			 (outs), 
			 (ins  RReg:$rs, PtrReg:$i, CallMFlag:$f),
			 "$rs=CallM $i$f", 0b000>
{
	
	bits<5> rs;
	bits<5> i;
	bit f;
	let Inst{23} = 0;
	let Inst{22} = f; 
	let Inst{9-5} = i;
	let Inst{4-0} = rs;
}
// dcx SYN end -------------------------------------------------------



// dcx SCU start -------------------------------------------------------

def SCUFlag : Operand<i32> {
  //let PrintMethod = "printSCUFlag";
}


class SCUInst< dag outs, dag ins, string asm, bits<3> opcode >
	  	: UCPSInstr <0b010, outs, ins, asm>
{
  let Inst{28-26} = opcode;
}

class BinaInst< dag outs, dag ins, string asm, bits<3> opcode >
	  	: SCUInst <outs, ins, asm, opcode>
{
  bits<6> f;
  bits<5> s1;
  bits<5> s0;
  bits<5> d;

  let Inst{14-10} = s1;
  let Inst{9-5} = s0;
  let Inst{4-0} = d;
}
class ASMInst< string opc, bits<3> opcode>
	  	: BinaInst < (outs RReg:$d), 
		  		  	 (ins RReg:$s0, RReg:$s1, SCUFlag:$f), 
		  		  	 !subst("OPC", opc, "$d = $s0 OPC $s1$f"), opcode>{					   
  let Inst{25-21} = f{4-0};
}

def Add : ASMInst<"+", 0b000>;
def Sub : ASMInst<"-", 0b001>;
def Mul : ASMInst<"*", 0b010>{					   
  let Inst{20} = f{5};
}


class CompareInst< string opc, bits<3> opcode, bits<2> x>
	  	: BinaInst < (outs RReg:$d), 
		  		  	 (ins RReg:$s0, RReg:$s1, SCUFlag:$f), 
		  		  	 !subst("OPC", opc, "$d = $s0 OPC $s1$f"), opcode>{					   
  let Inst{25} = f{4};
  let Inst{24} = x{1};
  let Inst{23} = f{2};
  let Inst{21} = x{0};
}

def EQ : CompareInst<"=", 0b011, 0b00>;
def NEQ : CompareInst<"!=", 0b011, 0b10>;
def GE : CompareInst<">=", 0b011, 0b11>;
def LT : CompareInst<"<", 0b011, 0b01>;


class LogicInst< string opc, bits<3> opcode , bits<2> x>
	  	: BinaInst < (outs RReg:$d), (ins RReg:$s0, RReg:$s1), 
		  		  	 !subst("OPC", opc, "$d = $s0 OPC $s1"), opcode>
{
  let Inst{25-24} = x;
  let Inst{23} = 1;
}

def And : LogicInst<"&", 0b101, 0b00>;
def Or  : LogicInst<"|", 0b101, 0b01>;
def XOr : LogicInst<"^", 0b101, 0b10>;

class ShiftInst< string opc, bits<3> opcode, bit x >
	  	: BinaInst < (outs RReg:$d), (ins RReg:$s0, RReg:$s1, SCUFlag:$f), 
		  		  	 !subst("OPC", opc, "$d = $s0 OPC $s1$f"), opcode>
{
  let Inst{25-24} = 0b10;
  let Inst{23} = f{2};
  let Inst{22} = 0;
  let Inst{21} = x;
}

def LSHT : ShiftInst<"<<", 0b111, 0>;
def RSHT  : ShiftInst<">>", 0b111, 1>;

class ShiftImmInst< string opc, bits<3> opcode, bit x >
	  	: BinaInst < (outs RReg:$d), (ins RReg:$s0, i32imm:$s1, SCUFlag:$f), 
		  		  	 !subst("OPC", opc, "$d = $s0 OPC $s1$f"), opcode>
{

  let Inst{25-24} = 0b11;
  let Inst{23} = f{2};
  let Inst{22} = 0;
  let Inst{21} = x;
}

def LSHTImm : ShiftImmInst<"<<", 0b111, 0>;
def RSHTImm  : ShiftImmInst<">>", 0b111, 1>;

class UnaInst< dag outs, dag ins, string asm, bits<3> opcode >
	  	: SCUInst <outs, ins, asm, opcode>
{
  bits<6> f;
  bits<5> s;
  bits<5> d;

  let Inst{9-5} = s;
  let Inst{4-0} = d;
}

def Not : UnaInst< (outs RReg:$d), (ins RReg:$s), "$d = ~$s", 0b101>
{
   let Inst{25-24} = 0b11;
   let Inst{23} = 1;
}

def ABS: UnaInst < (outs RReg:$d), (ins RReg:$s, SCUFlag:$f), "$d = ABS $s$f", 0b101 >
{
  let Inst{25-24} = f{4-3};
  let Inst{23} = 0;
  let Inst{22} = f{1};
}


def ReadFlag: SCUInst < (outs RReg:$d), (ins),  "$d = RF", 0b100>
{
  bits<5> d;
  let Inst{25} = 0;
  let Inst{22} = 0;
  let Inst{4-0} = d;
}

def WriteFlag: SCUInst < (outs), (ins RReg:$s), "RF = $s", 0b100 >
{
  bits<5> s;
  let Inst{25} = 0;
  let Inst{22} = 1;
  let Inst{9-5} = s;
}

def AssignImm : SCUInst < (outs RReg:$d), (ins i32imm:$imm, SCUFlag:$f), 
		  		  	 "$d = $imm$f", 0b100>
{
  bits<3> f;
  bits<16> imm;
  bits<5> d;

  let Inst{25-24} = 0b11;
  let Inst{23-21} = f{2-0};
  let Inst{20-5} = imm;
  let Inst{4-0} = d;
}

class Tran0Inst< string opc, bits<3> opcode , bits<2> x>
	  	: UnaInst < (outs RReg:$d), (ins RReg:$s, SCUFlag:$f), 
		  		  	 !subst("OPC", opc, "$d = OPC $s$f"), opcode>
{
  let Inst{25-24} = x;
  let Inst{23-22} = f{2-1};
}

def IntToSin : Tran0Inst<"SINGLE", 0b110, 0b00>;
def IntOrSinToDou  : Tran0Inst<"DOUBLE", 0b110, 0b01>
{
  let Inst{25} = 0;
  let Inst{24} = f{4};
  let Inst{21} = f{0};
}
def SinToInt : Tran0Inst<"INT", 0b110, 0b10>;

class Tran1Inst< string opc, bits<3> opcode , bits<2> x>
	  	: BinaInst < (outs RReg:$d), (ins RReg:$s0, RReg:$s1, SCUFlag:$f), 
		  		  	 !subst("OPC", opc, "$d = OPC($s0,$s1)$f"), opcode>
{
  let Inst{25-24} = x;
  let Inst{23-22} = f{2-1};
}

def DouToInt : Tran1Inst<"INT", 0b110, 0b11>;
def DouToSin : Tran1Inst<"SINGLE", 0b111, 0b01>;


def SCUNOP : SCUInst< (outs), (ins), "NOP", 0b100>{
  let Inst{25-24} = 0b10;
}
// dcx SCU end -------------------------------------------------------


// dcx AGU start -------------------------------------------------------
class AGUInst< dag outs, dag ins, string asm, bits<2> opcode,
	  		   	list<dag> pattern = [], InstrItinClass itin = NoItinerary> 
	  	: UCPSInstr < 0b001, outs, ins, asm, pattern > {
  let Inst{28-27} = opcode;
}

def AGUFlag : Operand<i32> {
  //let PrintMethod = "printAGUFlag";
}

class AGULdSt<dag outs, dag ins, string asm, bits<2> opcode> 
		    : AGUInst< outs, ins, asm, opcode>
{
	bits<4> f;
	bits<2> BH;
	bits<5> s1;
	bits<5> s0;
	bits<5> d;	
	 
	let Inst{26} = f{3};
	let Inst{25-24} = BH;
	let Inst{23} = f{2};
	let Inst{17-16} = f{1-0};
	let Inst{15} = 0;
	let Inst{14-10} = s1;
	let Inst{9-5} = s0;
	let Inst{4-0} = d;			 
}
def AGULoad  : AGULdSt<(outs RReg:$d), 
			 (ins RReg:$s0,  RReg:$s1, AGUFlag:$BH, AGUFlag:$f),
			 "$d = [$s0 + $s1]$BH$f", 0b00>;
def AGUStore : AGULdSt<(outs), 
			 (ins RReg:$s0,  RReg:$s1, RReg:$d, AGUFlag:$BH, AGUFlag:$f),
			 "[$s0 + $s1]= $d$BH$f", 0b01>;

			 
def AGUSvrLd : AGUInst< (outs SVRReg:$svr),
                       (ins RReg:$s0,  RReg:$s1, AGUFlag:$f),
                              "$svr = [$s0 + $s1]$f", 0b00> {
	bits<4> f;
	bits<5> s1;
	bits<5> s0;
	bits<5> svr;	
	 
	let Inst{26} = f{3};
	let Inst{25-24} = 0b11;
	let Inst{23} = f{2};
	let Inst{17-16} = f{1-0};
	let Inst{15} = 0;
	let Inst{14-10} = s1;
	let Inst{9-5} = s0;
	let Inst{4-0} = svr;
}

def AGUSvrSt : AGUInst< (outs),
                       (ins RReg:$s0,  RReg:$s1, SVRReg:$svr, AGUFlag:$f),
                              "[$s0 + $s1] = $svr$f", 0b01> {
	bits<4> f;
	bits<5> s1;
	bits<5> s0;
	bits<5> svr;	
	 
	let Inst{26} = f{3};
	let Inst{25-24} = 0b11;
	let Inst{23} = f{2};
	let Inst{17-16} = f{1-0};
	let Inst{15} = 0;
	let Inst{14-10} = s1;
	let Inst{9-5} = s0;
	let Inst{4-0} = svr;
}

def AGUNOP : AGUInst< (outs), (ins), "NOP", 0b11>;

// dcx AGU end -------------------------------------------------------------------



// dcx SEQ start -------------------------------------------------------------------

def SEQFlag : Operand<i32> {
  //let PrintMethod = "printSEQFlag";
}

/*----------- jump ------------*/
// absolute branch is barrier, while conditional branch is not.
class JumpInstr< dag outs, dag ins, string asm, bit opcode,
	  		   	 list<dag> pattern = [] >
		: SeqInstr<outs, ins, asm, opcode, pattern > {
	let isTerminator=1;
	let isPredicable = 0;
}

// branch to a PC-relative immediate address.
def JumpImm : JumpInstr<
	    (outs), (ins SImm18Ptr:$t, SEQFlag:$f),
        "jump $t$f", 0 >
{
	bits<2> f;
	bits<18> t;
	let Inst{26} = 1;
	let Inst{25-24} = f{1-0};
	let Inst{23} = 0;
	let Inst{22-5}	= t;
	let isBranch = 1;
	let isBarrier = 1;
	//let Itinerary = ItinSEQNoReg_ImmExt;
}
// conditionally branch to a PC-relative immediate address.
def JumpImmCond : JumpInstr<
	    (outs), (ins I32Reg:$c, SImm18Ptr:$t, SEQFlag:$f),
        "if $c, jump $t$f", 0 >
{
	bits<5> c;
	bits<2> f;
	bits<18> t;
	let Inst{26} = 0;
	let Inst{25-24} = f{1-0};
	let Inst{23} = 0;
	let Inst{22-5}	= t;
	let Inst{4-0} = c;
	let isBranch = 1;
	//let Itinerary = ItinSEQ_ImmExt_1;
}
// branch to a pointer address which is PC-irrelative.
def JumpPtr : JumpInstr< 
	    (outs), (ins RReg:$t, SEQFlag:$f),
		"jump $t$f", 1 >
{
	bits<5> t;
	bits<2> f;
	let Inst{26} = 1;
	let Inst{25-24} = f{1-0};
	let Inst{23} = 0;
  	let Inst{9-5} = t;
	let isBarrier = 1;
 	let isIndirectBranch = 1;
	//let Itinerary = ItinSEQ_1;
}
// conditionally branch to a pointer address which is PC-irrelative.
def JumpPtrCond : JumpInstr< 
	    (outs), (ins I32Reg:$c, RReg:$t, SEQFlag:$f),
		"if $c, jump $t$f", 1 >
{
	bits<5> c;
	bits<5> t;
	bits<2> f;
	let Inst{26} = 0;
	let Inst{25-24} = f{1-0};
	let Inst{23} = 0;
 	let Inst{9-5}	= t;
	let Inst{4-0} = c;
 	let isIndirectBranch = 1;
	//let Itinerary = ItinSEQ_1;
}

/*------------------------- call ---------------------------------*/
class CallInstr< dag outs, dag ins, string asm, bit opcode,
	  		   	 list<dag> pattern = [] >
		: SeqInstr<outs, ins, asm, opcode, pattern> {
	let isPredicable = 0;
  //let hasSideEffects = 0;
	let isCall = 1;
	let hasDelaySlot = 0;
	
	let Defs = [ J30 ];

    // these defs are actually caller-saved registers, and they can
    // not live across a call-function instruction.
	// We should use register mask to support call clobber.
	/*let Defs = [ R0, R1, R2, R3, R4, R5, R6, R7, R8, R9,
			   	 R10, R11, R12, R13, R14, R15,
				 J0, J1, J2, J3, J4, J5, J6, J7, J8, J9,
			   	 J10, J11, J12, J13, J14, J15,
				 D0, D1, D2, D3, D4, D5, D6, D7 ];*/
}

// call via to a PC-relative immediate address.
def CallImm : CallInstr<
	    (outs), (ins SImm18Ptr:$t, SEQFlag:$f),
        "call $t$f", 0 >
{
	bits<18> t;
	bits<2> f;
	let Inst{26} = 1;
	let Inst{25-24} = f{1-0};
	let Inst{23} = 1;
	let Inst{22-5}	= t;
	//let Itinerary = ItinSEQNoReg_ImmExt;
}
// conditionally call via a PC-relative immediate address.
def CallImmCond : CallInstr<
	    (outs), (ins I32Reg:$c, SImm18Ptr:$t, SEQFlag:$f),
        "if $c, call $t$f", 0 >
{
	bits<5> c;
	bits<18> t;
	bits<2> f;
	let Inst{26} = 0;
	let Inst{25-24} = f{1-0};
	let Inst{23} = 1;
	let Inst{22-5}	= t;
	let Inst{4-0} = c;
	//let Itinerary = ItinSEQ_ImmExt_1;
}
// call via a pointer address which is PC-irrelative.
def CallPtr : CallInstr< 
	    (outs), (ins RReg:$t, SEQFlag:$f),
		"call $t$f", 1 >
{
	bits<5> t;
	bits<2> f;
	let Inst{26} = 1;
	let Inst{25-24} = f{1-0};
	let Inst{23} = 1;
	let Inst{9-5}	= t;
	//let Itinerary = ItinSEQ_1;
}
// conditionally call via a pointer address which is PC-irrelative.
def CallPtrCond : CallInstr< 
	    (outs), (ins I32Reg:$c, RReg:$t, SEQFlag:$f),
		"if $c, call $t$f", 1 >
{
	bits<5> c;
	bits<5> t;
	bits<2> f;
	let Inst{26} = 0;
	let Inst{25-24} = f{1-0};
	let Inst{23} = 1;
	let Inst{9-5}	= t;
	let Inst{4-0} = c;
	//let Itinerary = ItinSEQ_1;
}

/*************** Loop ************/
def LoopL0 : SeqInstr<(outs), (ins SImm18Ptr:$t, RReg:$r),
		"LpTo $t BY $r (L0)", 1>
{
	bits<18> t;
	bits<5> r;
	let Inst{26-23} = 0b0110;
	let Inst{22-5} = t;
	let Inst{4-0} = r;
	let isTerminator = 1;
	//let Itinerary = ItinSEQ_ImmExt_1;
}

def LoopL1 : SeqInstr< 
	    (outs),
 		(ins SImm18Ptr:$t, RReg:$r),
		"LpTo $t BY $r (L1)", 1>
{
	bits<18> t;
	bits<5> r;
	let Inst{26-23} = 0b1110;
	let Inst{22-5} = t;
	let Inst{4-0} = r;
}

def Stop : SeqInstr< (outs), (ins), "SPU.Stop", 1>
{
	let Inst{26-22} = 0b10011;
}

def Break : SeqInstr< (outs), (ins), "DbgBreak", 1>
{
	let Inst{26-22} = 0b10111;
}

def Int : SeqInstr< (outs), (ins SEQFlag:$f), "IntEn$f", 1>
{
	bits<2> f;
	let Inst{26} = 0;
	let Inst{25-24} = f{1-0};
	let Inst{23-22} =0b11;
}

def IntAddr : SeqInstr< (outs), (ins RReg:$r), "IntAddr[$r]", 1>
{
	bits<5> r;
	let Inst{26-22} = 0b11011;
	let Inst{4-0} = r;
}

def NOP : SeqInstr<(outs), (ins), "NOP", 1>
{
	let Inst{26-22} = 0b11111;
	//let Itinerary = ItinNop;
}

// dcx SEQ end -------------------------------------------------------------------