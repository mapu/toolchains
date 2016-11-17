//===-- UCPSInstrFormats.td - UCPS Instruction Formats ---*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

/* a common instruction template base for UCPS */

// dcx
class UCPSInstr< bits<3> group, dag outs, dag ins, string asm,
	  			 list<dag> pattern = [] > : Instruction, Sched<[]>
{
  // 'field' is a keyword.
  // Note the value of 'Inst' is specified distributedly.
  field bits<32> Inst;
  field bits<32> SoftFail; // mask bits for reserved encoding fields

  // 'let' is used to override an OLD value.
  let Namespace = "UCPSInst";
  let OutOperandList = outs;
  let InOperandList = ins;
  let AsmString   = asm;
  let Pattern = pattern;
  let DecoderNamespace = "UCPSDecode";
  let Size = 4; // this field is critical for disassembly

  let Inst{31-29} = group; // encoding for SEQ, SCU, AGU, ...
}

class SeqInstr< dag outs, dag ins, string asm, bit opcode,
				list<dag> pattern = []>
	      : UCPSInstr <0b000, outs, ins, asm, pattern>{
  let Inst{27} = opcode;
}
	      
	      
	      