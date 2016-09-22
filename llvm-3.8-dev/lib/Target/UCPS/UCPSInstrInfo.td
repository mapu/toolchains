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

def UImm12Ptr : Operand<iPTR> {
  let EncoderMethod = "getUImm12Encoding";
  //let DecoderMethod = "DecodeUImm12";
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
  bits<4> f;
  bits<5> s1;
  bits<5> s0;
  bits<5> d;

  let Inst{28-26} = opcode;
  let Inst{14-10} = s1;
  let Inst{9-5} = s0;
  let Inst{4-0} = d;
}
class RInst< string opc, bits<3> opcode>
	  	: BinaInst < (outs RReg:$d), 
		  		  	 (ins RReg:$s0, RReg:$s1, SCUFlag:$f), 
		  		  	 !subst("OPC", opc, "$d = $s0 OPC $s1$f"), opcode>{					   
  let Inst{25} = 1;
  let Inst{24-21} = f{3-0};
}

class SInst< string opc, bits<3> opcode >
	  	: BinaInst < (outs RReg:$d), 
		  		  	 (ins RReg:$s0, RReg:$s1, SCUFlag:$f), 
		  		  	 !subst("OPC", opc, "$d = $s0 OPC $s1 (S)$f"), opcode >{
  let Inst{25} = 0;
  let Inst{24-21} = f{3-0};
}


def AddR : RInst<"+", 0b000>;
def SubR : RInst<"-", 0b001>;
def MulR : RInst<"*", 0b010>;

def AddS : SInst<"+", 0b000>;
def SubS : SInst<"-", 0b001>;
def MulS : SInst<"*", 0b010>;

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