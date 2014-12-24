//===-- MMPUInstrFormats.td - MMPU Instruction Formats ---*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

/* a common instruction template base for MMPU */
class MMPUInst< dag outs, dag ins, string asm,
	  			 list<dag> pattern = [],
	  		     InstrItinClass itin = NoItinerary > : Instruction
{
  let Namespace = "MMPUInst";
  let OutOperandList = outs;
  let InOperandList = ins;
  let AsmString   = asm;
  let Pattern = pattern;
  let Itinerary = itin; // InstrItinClass
  let DecoderNamespace = "MMPUDecode";

  field bits<48> Inst; // 48 bits encoding to accomodate for all instructions
  field bits<48> SoftFail;
  let Size = 6;
}

/* instruction template for pseudo instruction definition */
class PseudoInst<dag outs, dag ins, string asm, list<dag> pattern = []>
	  	: Instruction {
  let Namespace = "MMPUInst";
  let OutOperandList = outs;
  let InOperandList = ins;
  let AsmString   = asm;
  let Pattern = pattern;

  let isPseudo = 1;
  let mayLoad  = 0;
}
