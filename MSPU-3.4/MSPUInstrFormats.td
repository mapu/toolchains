//===-- MSPUInstrFormats.td - MSPU Instruction Formats ---*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

/* a common instruction template base for MSPU */
class MSPUInstr< bits<4> group, dag outs, dag ins, string asm,
	  			 list<dag> pattern = [] > : Instruction
{
  // 'field' is a keyword.
  // Note the value of 'Inst' is specified distributedly.
  field bits<32> Inst;
  field bits<32> SoftFail; // mask bits for reserved encoding fields

  // 'let' is used to override an OLD value.
  let Namespace = "MSPUInst";
  let OutOperandList = outs;
  let InOperandList = ins;
  let AsmString   = asm;
  let Pattern = pattern;
  let DecoderNamespace = "MSPUDecode";
  let Size = 4; // this field is critical for disassembly

  let Inst{31-28} = group; // encoding for SEQ, SCU, AGU, ...
}

/* instruction template for Seq instruction definition. See ISA manual 6.2.3 */
class SeqInstr< dag outs, dag ins, string asm, bits<6> opcode,
	  		   	list<dag> pattern = []> 
	  	: MSPUInstr < 0b0000, outs, ins, asm, pattern > {
    let Inst{27-22} = opcode;
}

/* instruction template for SCU instruction definition. See ISA manual 6.2.1 */
class SCUInstr< dag outs, dag ins, string asm,
	  			bits<5> opcode, bits<5> flags,
	  		   	list<dag> pattern = []> 
	  	: MSPUInstr <0b0001, outs, ins, asm, pattern >{
  let Inst{27-23} = opcode;
  let Inst{22-18} = flags;
  let isCodeGenOnly = 1;
}

/* instruction template for definitions of load/store instructions with
   immediate number as address offset. See ISA manual 6.2.2 */
class LdStImmInstr< dag outs, dag ins, string asm,
	  			   bit isStore, bits<6> flags,
	  		   	   list<dag> pattern = [] > 
	  	: MSPUInstr < 0b0010, outs, ins, asm, pattern > {
  let Inst{26-21} = flags;
  let Inst{27} = isStore;

  bits<5> reg;
  bits<16> addr;

  // encoding of "simm11:base"
  let Inst{20-10} = addr{15-5}; // offset
  let Inst{9-5} = addr{4-0}; 	// base
  
  let Inst{4-0} = reg;

  let isCodeGenOnly = 1;
}

/* instruction template for all other AGU instruction definition without
   any immediate value encoded within. See ISA manual 6.2.2 */
class AGUInstr< dag outs, dag ins, string asm, bits<5> opcode,
	  		   	list<dag> pattern = []> 
	  	: MSPUInstr < 0b0011, outs, ins, asm, pattern > {
  let Inst{27-23} = opcode;
}

/* instruction template for synchronization instruction definition.
   See ISA manual 6.2.4 */
class SynInstr< dag outs, dag ins, string asm, bits<5> opcode,
	  		   	list<dag> pattern = [] > 
	  	: MSPUInstr < 0b0100, outs, ins, asm, pattern > {
  let Inst{27-23} = opcode;
  let isCodeGenOnly = 1;
}

/* instruction template for pseudo instruction definition */
class PseudoInstr<dag outs, dag ins, string asm, list<dag> pattern = []>
	  	: Instruction {
  let Namespace = "MSPUInst";
  let OutOperandList = outs;
  let InOperandList = ins;
  let AsmString   = asm;
  let Pattern = pattern;

  let isPseudo = 1;
}
