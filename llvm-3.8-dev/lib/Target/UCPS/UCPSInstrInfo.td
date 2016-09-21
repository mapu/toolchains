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

// dcx SYN start
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
// dcx SYN end