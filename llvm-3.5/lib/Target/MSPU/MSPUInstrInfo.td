//===---- MSPUInstrInfo.td - Target Description for MSPU Target -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file describes the MSPU instructions in TableGen format.
//
//===----------------------------------------------------------------------===//

include "MSPUInstrFormats.td"

/****************************************************************************
           Instruction definitions only for C backend.

 ****************************************************************************/

/*
class PatLeaf<dag frag, code pred = [{}], SDNodeXForm xform = NOOP_SDNodeXForm>
 : PatFrag<(ops), frag, pred, xform>;

PatLeaf take no operands in its match pattern.

'i32' is an ValueType, 'imm' is an node def (an stub node).
It seems dag has less constraint than I thought.

Note: node/pat is for pattern matching, while operand is for asm writer or code
emitter. register is special since it works for both operand and pattern match.
*/

/// i32imm : Operand<i32>; See Target.td

def UImm1IntLeaf : PatLeaf<(i32 imm), [{ return isUInt<1>(N->getZExtValue()); }]>;
def UImm2IntLeaf : PatLeaf<(i32 imm), [{ return isUInt<2>(N->getZExtValue()); }]>;
def UImm3IntLeaf : PatLeaf<(i32 imm), [{ return isUInt<3>(N->getZExtValue()); }]>;
def UImm4IntLeaf : PatLeaf<(i32 imm), [{ return isUInt<4>(N->getZExtValue()); }]>;

// shift offset
def UImm5IntLeaf : PatLeaf<(i32 imm), [{ return isUInt<5>(N->getZExtValue()); }]>;
// pointer shift imm
def UImm5PtrLeaf : PatLeaf<(iPTR imm),[{ return isUInt<5>(N->getZExtValue()); }]>;

def UImm7IntLeaf : PatLeaf<(i32 imm), [{ return isUInt<7>(N->getZExtValue()); }]>;

// assign immediate value to R registers.
def SImm11Int : Operand<i32> {
  let EncoderMethod = "getSImm11Encoding";
  let DecoderMethod = "DecodeSImm11";
  let PrintMethod = "printImmExt";
}
def SImm11IntLeaf : PatLeaf<(i32 imm),[{ return isInt<11>(N->getSExtValue()); }]>;

// 1. assign immediate value to J registers.
// 2. branch to spu label
def SImm11Ptr : Operand<iPTR> {
  let EncoderMethod = "getSImm11Encoding";
  let DecoderMethod = "DecodeSImm11";
  let PrintMethod = "printImmExt";
}
def SImm11PtrLeaf : PatLeaf<(iPTR imm), [{ return isInt<11>(N->getSExtValue()); }]>;

def UImm12IntLeaf: PatLeaf<(i32 imm), [{ return isUInt<12>(N->getZExtValue()); }]>;

// CallM specific, PC-irrelative address
def UImm17Ptr : Operand<iPTR> {
  let EncoderMethod = "getUImm17Encoding";
  let DecoderMethod = "DecodeUImm17";
  let PrintMethod = "printImmExt";
}
// branch target, PC-relative address
def SImm17Ptr : Operand<iPTR> {
  let EncoderMethod = "getSImm17Encoding";
  let DecoderMethod = "DecodeSImm17";
  let PrintMethod = "printImmExt";
}

def SImm17PtrLeaf : PatLeaf<(iPTR imm), [{ return isInt<17>(N->getSExtValue()); }]>;

def SImm28 : Operand<i32> {
  let EncoderMethod = "getSImm28Encoding";
//let PrintMethod   = "printSImm28";
  let DecoderMethod = "DecodeSImm28";// need to sign-ext when decoding
}

def SImm32IntLeaf : PatLeaf<(i32  imm), [{ return isInt<32>(N->getSExtValue()); }]>;
def SImm32PtrLeaf : PatLeaf<(iPTR imm), [{ return isInt<32>(N->getSExtValue()); }]>;

def Ptr  : Operand<iPTR>;

def GlobalAddrLeaf: PatLeaf<(iPTR tglobaladdr), [{return true;}]>;

// Address operands.
//// This is used in operand printing in AsmWriter.
//// default val is 'printOperand'
//// ops MEANs 'operands'. PtrReg is register class for J registers.
// $ to specify operand name for asm writer and code emitter
def AddrJJ : Operand<iPTR> {
  let PrintMethod   =  "printAddrJJ";
  let EncoderMethod =    "getAddrJJEncoding";
  let DecoderMethod = "DecodeAddrJJ";
  let MIOperandInfo = (ops JReg, JReg);
}

def AddrJImm : Operand<iPTR> {
  let PrintMethod   =  "printAddrJImm";
  let EncoderMethod =    "getAddrJImmEncoding";
  let DecoderMethod = "DecodeAddrJImm";
  let MIOperandInfo = (ops JReg, i32imm);
}

// an BasicBlock SDNode produces OtherVT type, which is an operand for branch
def BasicBlock : Operand<OtherVT> {
  let EncoderMethod = "getSImm17Encoding";
  let DecoderMethod = "DecodeSImm17";
}

////////////////////////////////////////////////////////////////////////////////
/*
class ComplexPattern<ValueType ty, int numops, string fn,
                      list<SDNode> roots = [], list<SDNodeProperty> props = []> {
  ValueType Ty = ty;
  int NumOperands = numops;
  string SelectFunc = fn;
  list<SDNode> RootNodes = roots;
  list<SDNodeProperty> Properties = props;
}

NumOperands: is the number of operands returned (via pointer args) by 
			  the select function;
SelectFunc:	is the name of the function used to pattern match the max. pattern;
RootNodes: 	are the list of possible (not compulsory) root nodes of the sub-dags 
			  to match.

Note: 'roots' specifies where ComplexPattern applies to.
in AddrJImm, only 'frameindex' will invoke this check.

See SelectionDAGISel::CheckComplexPattern().
This is used in pattern matching.
*/

// Addressing modes.
// combined into SelectionDAGISel::CheckComplexPattern().
// This is for pattern-matching.

// for addresses with form [Jm + Jn]
def	AddressingJJ   : ComplexPattern<iPTR, 2, "SelectAddrJJ",
				   	 			    [], []>;
// for addresses with form [Jm + Imm]
def AddressingJImm : ComplexPattern<iPTR, 2, "SelectAddrJImm",
				   	 				[frameindex], []>;

// describe in/out operands and their data type
/*
class SDTypeProfile<int numresults, int numoperands,
                    list<SDTypeConstraint> constraints> {
  int NumResults = numresults;
  int NumOperands = numoperands;
  list<SDTypeConstraint> Constraints = constraints;
}

class SDNode<string opcode, SDTypeProfile typeprof,
             list<SDNodeProperty> props = [], string sdclass = "SDNode">
             : SDPatternOperator {
  string Opcode  = opcode; // opcode's enum name
  string SDClass = sdclass;
  list<SDNodeProperty> Properties = props;
  SDTypeProfile TypeProfile = typeprof;
}
*/

// has an 'iPTR' as the only one input operand.
def SDT_MSPU_CallSeqStart : SDCallSeqStart<[ SDTCisVT<0, iPTR> ]>;

// two operands, both input.
def SDT_MSPU_CallSeqEnd   : SDCallSeqEnd  <[ SDTCisVT<0, iPTR>,
                                        SDTCisVT<1, iPTR> ]>;

// chains connect nodes which have side effects. 
// SDNPHasChain indicates an node has an input chain and an output chain.
/* SDNPHasChain: use and produce an chain.
   SDNPInGlue only read an 'hidden' flag, so it does not mean side effect, 
   while SDNPOutGlue is an special kind of side effect. */
def CallSeqStart : SDNode<"ISD::CALLSEQ_START", SDT_MSPU_CallSeqStart,
                           [SDNPHasChain, SDNPOutGlue]>;
def CallSeqEnd   : SDNode<"ISD::CALLSEQ_END",   SDT_MSPU_CallSeqEnd,
                           [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue]>;

// operand number is '-1'???
def SDT_MSPU_Call: SDTypeProfile<0, 1, [SDTCisVT<0, iPTR>]>;

def MSPU_Call    : SDNode<"MSPU_ISD::CALL", SDT_MSPU_Call,
           [SDNPHasChain,SDNPOptInGlue,SDNPOutGlue,SDNPVariadic]>;

// fixme: we should eat J30. and let J30 live in when we handle formal
// arguments.
def MSPU_Ret           : SDNode<"MSPU_ISD::RET", SDTNone,
                           [SDNPHasChain, SDNPOptInGlue, SDNPVariadic]>;

def ADJCALLSTACKDOWN : PseudoInstr<(outs), 
					   		  (ins i32imm:$t),
                              "should neve be emitted",
                              [(CallSeqStart timm:$t)]>
{
  let Defs = [J29];
  let Uses = [J29, J31];
  let Itinerary = ItinPseudo;
}

def ADJCALLSTACKUP : PseudoInstr<(outs), 
				   	 		(ins i32imm:$t1, i32imm:$t2),
                            "should neve be emitted",
                            [(CallSeqEnd timm:$t1, timm:$t2)]>
{
  let Defs = [J29];
  let Uses = [J29, J31];
  let Itinerary = ItinPseudo;
}

def SDTADJDYNALLOC : SDTypeProfile<1, 2,
           [SDTCisVT<0, iPTR>, SDTCisVT<1, iPTR>, SDTCisVT<2, iPTR>]>;

def NodeADJDYNALLOC : SDNode<"MSPU_ISD::ADJDYNALLOC", SDTADJDYNALLOC>;

def AdjDynAlloc : PseudoInstr<(outs JReg:$dst),
				  			  (ins  JReg:$src1, Ptr:$src2),
                  "alloca should never be emitted directly",
                  [(set JReg:$dst,
				   (NodeADJDYNALLOC JReg:$src1, timm:$src2))]>
{
  let usesCustomInserter = 1;
  let Itinerary = ItinPseudo;
}

def XferAddr   : SDNode<"MSPU_ISD::XferAddr", SDTIntUnaryOp>;
def RecipNode  : SDNode<"MSPU_ISD::Recip", SDTFPUnaryOp>;

//===----------------------------------------------------------------------===//
//						SCU Instructions
//===----------------------------------------------------------------------===//
// (1) integer binary operations
class BinaryInstr< dag outs, dag ins, string asm,
	  			   bits<5> opcode, bits<5> flags,
				   list<dag> pattern=[] >
	   : SCUInstr< outs, ins, asm, opcode, flags, pattern >
{
		bits<5> d;
		bits<5> s0;
		bits<5> s1;
		let Inst{14-10} = s1;
		let Inst{9-5} = s0;
		let Inst{4-0} = d;
}

/*------------------------- Binary Instructions ----------------------------*/
class AddInstr< dag outs, dag ins, string asm, bits<5> flags,
		   list<dag> pattern=[] >
	: BinaryInstr<outs, ins, asm, 0b00001, flags, pattern>;

class SubInstr< dag outs, dag ins, string asm, bits<5> flags,
		   list<dag> pattern=[] >
	: BinaryInstr<outs, ins, asm, 0b00010, flags, pattern>;

class MulInstr< dag outs, dag ins, string asm, bits<5> flags,
		   list<dag> pattern=[] >
	: BinaryInstr<outs, ins, asm, 0b00011, flags, pattern>;

// '(T)' means truncation.
// Note: there is no difference between signed or unsigned numbers for
// truncated addition or substraction.
// 8 cases for all addition instruction, some of which are used only for
// code emitting (i.e., for instruction coding, not for code generation)

/* ------------------- Add --------------------- */
def AddSI32T : AddInstr<
				(outs I32Reg:$d), (ins  I32Reg:$s0, I32Reg:$s1),
         		"$d = $s0 + $s1 (T)", 0b00001,
         		[(set I32Reg:$d, (add I32Reg:$s0, I32Reg:$s1))] >
{ let Itinerary = ItinSCU_2_1_1; }

// for spu, integer add/sub always produces a carry-in flag.
def : Pat<(addc I32Reg:$s0, I32Reg:$s1), (AddSI32T I32Reg:$s0, I32Reg:$s1)>;

// if the operation uses carry flag, it must produce a carry.
def AddSI32CI : BinaryInstr<
				(outs I32Reg:$d), (ins  I32Reg:$s0, I32Reg:$s1),
         		"$d = $s0 + $s1 (T)(CI)", 0b10010, 0b00001,
         		[(set I32Reg:$d, (adde I32Reg:$s0, I32Reg:$s1))] >
{ let Itinerary = ItinSCU_2_1_1; }

def AddF32T : AddInstr<
				(outs F32Reg:$d), 
				(ins  F32Reg:$s0, F32Reg:$s1),
         		"$d = $s0 + $s1 (S)(T)", 0b00101,
         		[(set F32Reg:$d, (fadd F32Reg:$s0, F32Reg:$s1))] >
{ let Itinerary = ItinSCU_5_1_1; }

def AddF64T : AddInstr<
				(outs F64Reg:$d), 
				(ins  F64Reg:$s0, F64Reg:$s1),
         		"$d = $s0 + $s1 (T)", 0b00011,
         		[(set F64Reg:$d, (fadd F64Reg:$s0, F64Reg:$s1))] >
{ let Itinerary = ItinSCU_5_1_1; }

/* ------------------- Sub --------------------- */
def SubSI32T : SubInstr<
				(outs I32Reg:$d), 
				(ins  I32Reg:$s0, I32Reg:$s1),
         		"$d = $s0 - $s1 (T)", 0b00001,
         		[(set I32Reg:$d, (sub I32Reg:$s0, I32Reg:$s1))] >
{ let Itinerary = ItinSCU_2_1_1; }


// if the operation uses carry flag, it must produce a carry.
def : Pat<(subc I32Reg:$s0, I32Reg:$s1), (SubSI32T I32Reg:$s0, I32Reg:$s1)>;

def SubSI32CI : BinaryInstr<
				(outs I32Reg:$d), (ins  I32Reg:$s0, I32Reg:$s1),
         		"$d = $s0 - $s1 (T)(CI)", 0b10011, 0b00001,
         		[(set I32Reg:$d, (sube I32Reg:$s0, I32Reg:$s1))] >
{ let Itinerary = ItinSCU_2_1_1; }

def SubF32T : SubInstr<
				(outs F32Reg:$d), 
				(ins  F32Reg:$s0, F32Reg:$s1),
         		"$d = $s0 - $s1 (S)(T)", 0b00101,
         		[(set F32Reg:$d, (fsub F32Reg:$s0, F32Reg:$s1))] >
{ let Itinerary = ItinSCU_5_1_1; }

def SubF64T : SubInstr<
				(outs F64Reg:$d), 
				(ins  F64Reg:$s0, F64Reg:$s1),
         		"$d = $s0 - $s1 (T)", 0b00011,
         		[(set F64Reg:$d, (fsub F64Reg:$s0, F64Reg:$s1))] >
{ let Itinerary = ItinSCU_5_1_1; }

/* ------------------- Mul --------------------- */
def MulSI32T : MulInstr<
				(outs I32Reg:$d), 
				(ins  I32Reg:$s0, I32Reg:$s1),
         		"$d = $s0 * $s1 (T)", 0b00001,
         		[(set I32Reg:$d, (mul I32Reg:$s0, I32Reg:$s1))] >
{ let Itinerary = ItinSCU_4_1_1; }

def MulF32T : MulInstr<
				(outs F32Reg:$d), 
				(ins  F32Reg:$s0, F32Reg:$s1),
         		"$d = $s0 * $s1 (S)(T)", 0b00101,
         		[(set F32Reg:$d, (fmul F32Reg:$s0, F32Reg:$s1))] >
{ let Itinerary = ItinSCU_5_1_1; }

def MulF64T : MulInstr<
				(outs F64Reg:$d), 
				(ins  F64Reg:$s0, F64Reg:$s1),
         		"$d = $s0 * $s1 (T)", 0b00011,
         		[(set F64Reg:$d, (fmul F64Reg:$s0, F64Reg:$s1))] >
{ let Itinerary = ItinSCU_5_1_1; }

// This class does not include shift.
class LogicInstr< dag outs, dag ins, string asm,
	  			  bits<5> opcode, list<dag> pattern=[] >
	   : BinaryInstr< outs, ins, asm, opcode, 0b00000, pattern >;

def And__ : LogicInstr<
				(outs I32Reg:$d), (ins  I32Reg:$s0, I32Reg:$s1),
         		"$d = $s0 & $s1", 0b10100,
         		[(set I32Reg:$d, (and I32Reg:$s0, I32Reg:$s1))] >
{ let Itinerary = ItinSCU_2_1_1; }

def Or__ : LogicInstr<
				(outs I32Reg:$d), (ins  I32Reg:$s0, I32Reg:$s1),
         		"$d = $s0 | $s1", 0b10101,
         		[(set I32Reg:$d, (or I32Reg:$s0, I32Reg:$s1))] >
{ let Itinerary = ItinSCU_2_1_1; }

def XOr__ : LogicInstr<
				(outs I32Reg:$d), (ins  I32Reg:$s0, I32Reg:$s1),
         		"$d = $s0 ^ $s1", 0b10111,
         		[(set I32Reg:$d, (xor I32Reg:$s0, I32Reg:$s1))] >
{ let Itinerary = ItinSCU_2_1_1; }

/*---------------------------- compare ---------------------------------*/
class CmpInstr< dag outs, dag ins, string asm,
	  			bits<5> opcode, bits<5> flags,
				list<dag> pattern=[] >
	   : BinaryInstr< outs, ins, asm, opcode, flags, pattern >
{
		let isCompare=1;
}

class EQInstr< dag outs, dag ins, string asm, bits<5> flags,
							 list<dag> pattern=[] > 
		: CmpInstr< outs, ins, asm, 0b11000, flags, pattern >;

class NEInstr< dag outs, dag ins, string asm, bits<5> flags,
							 list<dag> pattern=[] > 
		: CmpInstr< outs, ins, asm, 0b11001, flags, pattern >;

class GTInstr< dag outs, dag ins, string asm, bits<5> flags,
			   list<dag> pattern=[] > 
		: CmpInstr< outs, ins, asm, 0b11010, flags, pattern >;

class GEInstr< dag outs, dag ins, string asm, bits<5> flags,
			   list<dag> pattern=[] > 
		: CmpInstr< outs, ins, asm, 0b11011, flags, pattern >;

class LTInstr< dag outs, dag ins, string asm, bits<5> flags, 
				  list<dag> pattern=[] > 
		: CmpInstr< outs, ins, asm, 0b11100, flags, pattern >;

class LEInstr< dag outs, dag ins, string asm, bits<5> flags,
				  list<dag> pattern=[] >
		: CmpInstr< outs, ins, asm, 0b11101, flags, pattern >;

// no F32/F64 equal/not-equal?
// setu and setuo is not handled here.
def EQ_SI32	: EQInstr< (outs I32Reg:$d), (ins  I32Reg:$s0, I32Reg:$s1),
         		"$d = $s0 == $s1", 0b00000,
         		[(set I32Reg:$d, (seteq I32Reg:$s0, I32Reg:$s1))] >
{ let Itinerary = ItinSCU_2_1_1; }
         		
def EQ_UI32 : EQInstr< (outs I32Reg:$d), (ins  I32Reg:$s0, I32Reg:$s1),
         		"$d = $s0 == $s1 (U)", 0b01000,
         		[(set I32Reg:$d, (setueq I32Reg:$s0, I32Reg:$s1))] >
{ let Itinerary = ItinSCU_2_1_1; }

def EQ_F32 : EQInstr< (outs I32Reg:$d), (ins  F32Reg:$s0, F32Reg:$s1),
         		"$d = $s0 == $s1 (S)", 0b00100,
         		[(set I32Reg:$d, (seteq F32Reg:$s0, F32Reg:$s1))] >
{ let Itinerary = ItinSCU_5_1_1; }

def EQ_OF32 : EQInstr< (outs I32Reg:$d), (ins  F32Reg:$s0, F32Reg:$s1),
         		"$d = $s0 == $s1 (S)", 0b00100,
         		[(set I32Reg:$d, (setoeq F32Reg:$s0, F32Reg:$s1))] >
{ let Itinerary = ItinSCU_5_1_1; }

def EQ_UF32 : EQInstr< (outs I32Reg:$d), (ins  F32Reg:$s0, F32Reg:$s1),
         		"$d = $s0 == $s1 (S)", 0b00100,
         		[(set I32Reg:$d, (setueq F32Reg:$s0, F32Reg:$s1))] >
{ let Itinerary = ItinSCU_5_1_1; }
         		
def EQ_F64 : EQInstr< (outs I32Reg:$d), (ins  F64Reg:$s0, F64Reg:$s1),
         		"$d = $s0 == $s1", 0b00010,
         		[(set I32Reg:$d, (seteq F64Reg:$s0, F64Reg:$s1))] >
{ let Itinerary = ItinSCU_5_1_1; }
         		
def EQ_OF64 : EQInstr< (outs I32Reg:$d), (ins  F64Reg:$s0, F64Reg:$s1),
         		"$d = $s0 == $s1", 0b00010,
         		[(set I32Reg:$d, (setoeq F64Reg:$s0, F64Reg:$s1))] >
{ let Itinerary = ItinSCU_5_1_1; }         		

def EQ_UF64 : EQInstr< (outs I32Reg:$d), (ins  F64Reg:$s0, F64Reg:$s1),
         		"$d = $s0 == $s1", 0b00010,
         		[(set I32Reg:$d, (setueq F64Reg:$s0, F64Reg:$s1))] >
{ let Itinerary = ItinSCU_5_1_1; }



def NE_SI32	: NEInstr< (outs I32Reg:$d), (ins  I32Reg:$s0, I32Reg:$s1),
         		"$d = $s0 != $s1", 0b00000,
         		[(set I32Reg:$d, (setne I32Reg:$s0, I32Reg:$s1))] >
{ let Itinerary = ItinSCU_2_1_1; }

def NE_UI32 : NEInstr< (outs I32Reg:$d), (ins  I32Reg:$s0, I32Reg:$s1),
         		"$d = $s0 != $s1 (U)", 0b01000,
         		[(set I32Reg:$d, (setune I32Reg:$s0, I32Reg:$s1))] >
{ let Itinerary = ItinSCU_2_1_1; }

def NE_F32 : NEInstr< (outs I32Reg:$d), (ins  F32Reg:$s0, F32Reg:$s1),
         		"$d = $s0 != $s1 (S)", 0b00100,
         		[(set I32Reg:$d, (setne F32Reg:$s0, F32Reg:$s1))] >
{ let Itinerary = ItinSCU_5_1_1; }

def NE_OF32 : NEInstr< (outs I32Reg:$d), (ins  F32Reg:$s0, F32Reg:$s1),
         		"$d = $s0 != $s1 (S)", 0b00100,
         		[(set I32Reg:$d, (setone F32Reg:$s0, F32Reg:$s1))] >
{ let Itinerary = ItinSCU_5_1_1; }

def NE_UF32 : NEInstr< (outs I32Reg:$d), (ins  F32Reg:$s0, F32Reg:$s1),
         		"$d = $s0 != $s1 (S)", 0b00100,
         		[(set I32Reg:$d, (setune F32Reg:$s0, F32Reg:$s1))] >
{ let Itinerary = ItinSCU_5_1_1; }

def NE_F64 : NEInstr< (outs I32Reg:$d), (ins  F64Reg:$s0, F64Reg:$s1),
         		"$d = $s0 != $s1", 0b00010,
         		[(set I32Reg:$d, (setne F64Reg:$s0, F64Reg:$s1))] >
{ let Itinerary = ItinSCU_5_1_1; }

def NE_OF64 : NEInstr< (outs I32Reg:$d), (ins  F64Reg:$s0, F64Reg:$s1),
         		"$d = $s0 != $s1", 0b00010,
         		[(set I32Reg:$d, (setone F64Reg:$s0, F64Reg:$s1))] >
{ let Itinerary = ItinSCU_5_1_1; }

def NE_UF64 : NEInstr< (outs I32Reg:$d), (ins  F64Reg:$s0, F64Reg:$s1),
         		"$d = $s0 != $s1", 0b00010,
         		[(set I32Reg:$d, (setune F64Reg:$s0, F64Reg:$s1))] >
{ let Itinerary = ItinSCU_5_1_1; }



def GT_SI32 : GTInstr< (outs I32Reg:$d), (ins  I32Reg:$s0, I32Reg:$s1),
         		"$d = $s0 > $s1", 0b00000,
         		[(set I32Reg:$d, (setgt I32Reg:$s0, I32Reg:$s1))] >
{ let Itinerary = ItinSCU_2_1_1; }

def GT_UI32 : GTInstr< (outs I32Reg:$d), (ins  I32Reg:$s0, I32Reg:$s1),
         		"$d = $s0 > $s1 (U)", 0b01000,
         		[(set I32Reg:$d, (setugt I32Reg:$s0, I32Reg:$s1))] >
{ let Itinerary = ItinSCU_2_1_1; }

def GT_F32 : GTInstr< (outs I32Reg:$d), (ins  F32Reg:$s0, F32Reg:$s1),
         		"$d = $s0 > $s1 (S)", 0b00100,
         		[(set I32Reg:$d, (setgt F32Reg:$s0, F32Reg:$s1))] >
{ let Itinerary = ItinSCU_5_1_1; }

def GT_OF32 : GTInstr< (outs I32Reg:$d), (ins  F32Reg:$s0, F32Reg:$s1),
         		"$d = $s0 > $s1 (S)", 0b00100,
         		[(set I32Reg:$d, (setogt F32Reg:$s0, F32Reg:$s1))] >
{ let Itinerary = ItinSCU_5_1_1; }

def GT_UF32 : GTInstr< (outs I32Reg:$d), (ins  F32Reg:$s0, F32Reg:$s1),
         		"$d = $s0 > $s1 (S)", 0b00100,
         		[(set I32Reg:$d, (setugt F32Reg:$s0, F32Reg:$s1))] >
{ let Itinerary = ItinSCU_5_1_1; }

def GT_F64 : GTInstr< (outs I32Reg:$d), (ins  F64Reg:$s0, F64Reg:$s1),
         		"$d = $s0 > $s1", 0b00010,
         		[(set I32Reg:$d, (setgt F64Reg:$s0, F64Reg:$s1))] >
{ let Itinerary = ItinSCU_5_1_1; }

def GT_OF64 : GTInstr< (outs I32Reg:$d), (ins  F64Reg:$s0, F64Reg:$s1),
         		"$d = $s0 > $s1", 0b00010,
         		[(set I32Reg:$d, (setogt F64Reg:$s0, F64Reg:$s1))] >
{ let Itinerary = ItinSCU_5_1_1; }

def GT_UF64 : GTInstr< (outs I32Reg:$d), (ins  F64Reg:$s0, F64Reg:$s1),
         		"$d = $s0 > $s1", 0b00010,
         		[(set I32Reg:$d, (setugt F64Reg:$s0, F64Reg:$s1))] >
{ let Itinerary = ItinSCU_5_1_1; }



def GE_SI32 : GEInstr< (outs I32Reg:$d), (ins  I32Reg:$s0, I32Reg:$s1),
         		"$d = $s0 >= $s1", 0b00000,
         		[(set I32Reg:$d, (setge I32Reg:$s0, I32Reg:$s1))] >
{ let Itinerary = ItinSCU_2_1_1; }

def GE_UI32 : GEInstr< (outs I32Reg:$d), (ins  I32Reg:$s0, I32Reg:$s1),
         		"$d = $s0 >= $s1 (U)", 0b01000,
         		[(set I32Reg:$d, (setuge I32Reg:$s0, I32Reg:$s1))] >
{ let Itinerary = ItinSCU_2_1_1; }

def GE_F32 : GEInstr< (outs I32Reg:$d), (ins  F32Reg:$s0, F32Reg:$s1),
         		"$d = $s0 >= $s1 (S)", 0b00100,
         		[(set I32Reg:$d, (setge F32Reg:$s0, F32Reg:$s1))] >
{ let Itinerary = ItinSCU_5_1_1; }

def GE_OF32 : GEInstr< (outs I32Reg:$d), (ins  F32Reg:$s0, F32Reg:$s1),
         		"$d = $s0 >= $s1 (S)", 0b00100,
         		[(set I32Reg:$d, (setoge F32Reg:$s0, F32Reg:$s1))] >
{ let Itinerary = ItinSCU_5_1_1; }

def GE_UF32 : GEInstr< (outs I32Reg:$d), (ins  F32Reg:$s0, F32Reg:$s1),
         		"$d = $s0 >= $s1 (S)", 0b00100,
         		[(set I32Reg:$d, (setuge F32Reg:$s0, F32Reg:$s1))] >
{ let Itinerary = ItinSCU_5_1_1; }

def GE_F64 : GEInstr< (outs I32Reg:$d), (ins  F64Reg:$s0, F64Reg:$s1),
         		"$d = $s0 >= $s1", 0b00010,
         		[(set I32Reg:$d, (setge F64Reg:$s0, F64Reg:$s1))] >
{ let Itinerary = ItinSCU_5_1_1; }

def GE_OF64 : GEInstr< (outs I32Reg:$d), (ins  F64Reg:$s0, F64Reg:$s1),
         		"$d = $s0 >= $s1", 0b00010,
         		[(set I32Reg:$d, (setoge F64Reg:$s0, F64Reg:$s1))] >
{ let Itinerary = ItinSCU_5_1_1; }

def GE_UF64 : GEInstr< (outs I32Reg:$d), (ins  F64Reg:$s0, F64Reg:$s1),
         		"$d = $s0 >= $s1", 0b00010,
         		[(set I32Reg:$d, (setuge F64Reg:$s0, F64Reg:$s1))] >
{ let Itinerary = ItinSCU_5_1_1; }



def LT_SI32 : LTInstr< (outs I32Reg:$d), (ins  I32Reg:$s0, I32Reg:$s1),
         		"$d = $s0 < $s1", 0b00000,
         		[(set I32Reg:$d, (setlt I32Reg:$s0, I32Reg:$s1))] >
{ let Itinerary = ItinSCU_2_1_1; }

def LT_UI32 : LTInstr< (outs I32Reg:$d), (ins  I32Reg:$s0, I32Reg:$s1),
         		"$d = $s0 < $s1 (U)", 0b01000,
         		[(set I32Reg:$d, (setult I32Reg:$s0, I32Reg:$s1))] >
{ let Itinerary = ItinSCU_2_1_1; }

def LT_F32 : LTInstr< (outs I32Reg:$d), (ins  F32Reg:$s0, F32Reg:$s1),
         		"$d = $s0 < $s1 (S)", 0b00100,
         		[(set I32Reg:$d, (setlt F32Reg:$s0, F32Reg:$s1))] >
{ let Itinerary = ItinSCU_5_1_1; }

def LT_OF32 : LTInstr< (outs I32Reg:$d), (ins  F32Reg:$s0, F32Reg:$s1),
         		"$d = $s0 < $s1 (S)", 0b00100,
         		[(set I32Reg:$d, (setolt F32Reg:$s0, F32Reg:$s1))] >
{ let Itinerary = ItinSCU_5_1_1; }

def LT_UF32 : LTInstr< (outs I32Reg:$d), (ins  F32Reg:$s0, F32Reg:$s1),
         		"$d = $s0 < $s1 (S)", 0b00100,
         		[(set I32Reg:$d, (setult F32Reg:$s0, F32Reg:$s1))] >
{ let Itinerary = ItinSCU_5_1_1; }

def LT_F64 : LTInstr< (outs I32Reg:$d), (ins  F64Reg:$s0, F64Reg:$s1),
         		"$d = $s0 < $s1", 0b00010,
         		[(set I32Reg:$d, (setlt F64Reg:$s0, F64Reg:$s1))] >
{ let Itinerary = ItinSCU_5_1_1; }

def LT_OF64 : LTInstr< (outs I32Reg:$d), (ins  F64Reg:$s0, F64Reg:$s1),
         		"$d = $s0 < $s1", 0b00010,
         		[(set I32Reg:$d, (setolt F64Reg:$s0, F64Reg:$s1))] >
{ let Itinerary = ItinSCU_5_1_1; }

def LT_UF64 : LTInstr< (outs I32Reg:$d), (ins  F64Reg:$s0, F64Reg:$s1),
         		"$d = $s0 < $s1", 0b00010,
         		[(set I32Reg:$d, (setult F64Reg:$s0, F64Reg:$s1))] >
{ let Itinerary = ItinSCU_5_1_1; }



def LE_SI32 : LEInstr< (outs I32Reg:$d), (ins  I32Reg:$s0, I32Reg:$s1),
         		"$d = $s0 <= $s1", 0b00000,
         		[(set I32Reg:$d, (setle I32Reg:$s0, I32Reg:$s1))] >
{ let Itinerary = ItinSCU_2_1_1; }

def LE_UI32 : LEInstr< (outs I32Reg:$d), (ins  I32Reg:$s0, I32Reg:$s1),
         		"$d = $s0 <= $s1 (U)", 0b01000,
         		[(set I32Reg:$d, (setule I32Reg:$s0, I32Reg:$s1))] >
{ let Itinerary = ItinSCU_2_1_1; }

def LE_F32 : LEInstr< (outs I32Reg:$d), (ins  F32Reg:$s0, F32Reg:$s1),
         		"$d = $s0 <= $s1 (S)", 0b00100,
         		[(set I32Reg:$d, (setle F32Reg:$s0, F32Reg:$s1))] >
{ let Itinerary = ItinSCU_5_1_1; }

def LE_OF32 : LEInstr< (outs I32Reg:$d), (ins  F32Reg:$s0, F32Reg:$s1),
         		"$d = $s0 <= $s1 (S)", 0b00100,
         		[(set I32Reg:$d, (setole F32Reg:$s0, F32Reg:$s1))] >
{ let Itinerary = ItinSCU_5_1_1; }

def LE_UF32 : LEInstr< (outs I32Reg:$d), (ins  F32Reg:$s0, F32Reg:$s1),
         		"$d = $s0 <= $s1 (S)", 0b00100,
         		[(set I32Reg:$d, (setule F32Reg:$s0, F32Reg:$s1))] >
{ let Itinerary = ItinSCU_5_1_1; }

def LE_F64 : LEInstr< (outs I32Reg:$d), (ins  F64Reg:$s0, F64Reg:$s1),
         		"$d = $s0 <= $s1", 0b00010,
         		[(set I32Reg:$d, (setle F64Reg:$s0, F64Reg:$s1))] >
{ let Itinerary = ItinSCU_5_1_1; }

def LE_UF64 : LEInstr< (outs I32Reg:$d), (ins  F64Reg:$s0, F64Reg:$s1),
         		"$d = $s0 <= $s1", 0b00010,
         		[(set I32Reg:$d, (setule F64Reg:$s0, F64Reg:$s1))] >
{ let Itinerary = ItinSCU_5_1_1; }

def LE_OF64 : LEInstr< (outs I32Reg:$d), (ins  F64Reg:$s0, F64Reg:$s1),
         		"$d = $s0 <= $s1", 0b00010,
         		[(set I32Reg:$d, (setole F64Reg:$s0, F64Reg:$s1))] >
{ let Itinerary = ItinSCU_5_1_1; }


/*-------------------------shift-----------------------------------*/
def I32ShiftLeft : BinaryInstr<
		   	 	(outs I32Reg:$d), (ins I32Reg:$s0, I32Reg:$s1),
         		"$d = $s0 << $s1", 0b10000, 0b00000,
         		[(set I32Reg:$d, (shl I32Reg:$s0, I32Reg:$s1))] >
{ let Itinerary = ItinSCU_2_1_1; }

def I32ShiftLeftImm : BinaryInstr< 
		   	 	(outs I32Reg:$d), (ins  I32Reg:$s0, i32imm:$s1),
         		"$d = $s0 << $s1", 0b10000, 0b10000,
         		[(set I32Reg:$d, (shl I32Reg:$s0, UImm5IntLeaf:$s1))] >
{ let Itinerary = ItinSCU_2_1_1; }

// signed: arithmetic shift right
def SI32ShiftRight : BinaryInstr< 
		   	 	(outs I32Reg:$d), (ins  I32Reg:$s0, I32Reg:$s1),
         		"$d = $s0 >> $s1", 0b10001, 0b00000,
         		[(set I32Reg:$d, (sra I32Reg:$s0, I32Reg:$s1))] >
{ let Itinerary = ItinSCU_2_1_1; }

def SI32ShiftRightImm : BinaryInstr< 
		   	 	(outs I32Reg:$d), (ins  I32Reg:$s0, i32imm:$s1),
         		"$d = $s0 >> $s1", 0b10001, 0b10000,
         		[(set I32Reg:$d, (sra I32Reg:$s0, UImm5IntLeaf:$s1))] >
{ let Itinerary = ItinSCU_2_1_1; }

// unsigned: logic shift right
def UI32ShiftRight : BinaryInstr< 
		   	 	(outs I32Reg:$d), (ins  I32Reg:$s0, I32Reg:$s1),
         		"$d = $s0 >> $s1 (U)", 0b10001, 0b01000,
         		[(set I32Reg:$d, (srl I32Reg:$s0, I32Reg:$s1))] >
{ let Itinerary = ItinSCU_2_1_1; }

def UI32ShiftRightImm : BinaryInstr< 
		   	 	(outs I32Reg:$d), (ins  I32Reg:$s0, i32imm:$s1),
         		"$d = $s0 >> $s1 (U)", 0b10001, 0b11000,
         		[(set I32Reg:$d, (srl I32Reg:$s0, UImm5IntLeaf:$s1))] >
{ let Itinerary = ItinSCU_2_1_1; }

/*---------------------unary instructions------------------*/
class UnaryInstr< dag outs, dag ins, string asm,
	  			  bits<5> opcode, bits<5> flags,
		  		  list<dag> pattern=[] >
	   : SCUInstr< outs, ins, asm, opcode, flags, pattern >
{
		bits<5> d;
		bits<5> s;

		let Inst{9-5} = s;
		let Inst{4-0} = d;
}

// SI32/UI32/double --> single
def SingleFromSInt : UnaryInstr<
		 (outs F32Reg:$d), (ins  I32Reg:$s),
         "$d = single $s", 0b00100, 0b00000,
		 [(set F32Reg:$d, (sint_to_fp I32Reg:$s))]>
{ let Itinerary = ItinSCU_5_1 ; }

def SingleFromUInt : UnaryInstr<
		 (outs F32Reg:$d), (ins  I32Reg:$s),
         "$d = single $s (U)", 0b00100, 0b01000,
		 [(set F32Reg:$d, (uint_to_fp I32Reg:$s))]>
{ let Itinerary = ItinSCU_5_1; }

def SingleFromDouble : UnaryInstr<
		 (outs F32Reg:$d), (ins  F64Reg:$s),
         "$d = single $s", 0b00100, 0b00010,
		 [(set F32Reg:$d, (fround F64Reg:$s))]>
{ let Itinerary = ItinSCU_5_1; }

def SingleFromDoubleT : UnaryInstr<
		 (outs F32Reg:$d), (ins  F64Reg:$s),
         "$d = single $s (T)", 0b00100, 0b00011,
		 [(set F32Reg:$d, (fround F64Reg:$s))]>
{ let Itinerary = ItinSCU_5_1; }

// SI32/U3I32/single --> double
def DoubleFromSInt : UnaryInstr<
		 (outs F64Reg:$d), (ins  I32Reg:$s),
         "$d = double $s", 0b00101, 0b00000,
		 [(set F64Reg:$d, (sint_to_fp I32Reg:$s))]>
{ let Itinerary = ItinSCU_5_1; }

def DoubleFromUInt : UnaryInstr<
		 (outs F64Reg:$d), (ins  I32Reg:$s),
         "$d = double $s (U)", 0b00101, 0b01000,
		 [(set F64Reg:$d, (uint_to_fp I32Reg:$s))]>
{ let Itinerary = ItinSCU_5_1; }

def DoubleFromSingle : UnaryInstr<
		 (outs F64Reg:$d), (ins  F32Reg:$s),
         "$d = double $s (S)", 0b00101, 0b00100,
		 [(set F64Reg:$d, (fextend F32Reg:$s))]>
{ let Itinerary = ItinSCU_5_1; }

// single/double --> SI32
def SI32FromSingle : UnaryInstr<
		 (outs I32Reg:$d), (ins  F32Reg:$s),
         "$d = int $s (S)", 0b00110, 0b00100,
		 [(set I32Reg:$d, (fp_to_sint F32Reg:$s))]>
{ let Itinerary = ItinSCU_5_1; }

def SI32FromDouble : UnaryInstr<
		 (outs I32Reg:$d), (ins  F64Reg:$s),
         "$d = int $s", 0b00110, 0b00010,
		 [(set I32Reg:$d, (fp_to_sint F64Reg:$s))]>
{ let Itinerary = ItinSCU_5_1; }

def UI32FromSingle : UnaryInstr<
		 (outs I32Reg:$d), (ins  F32Reg:$s),
         "$d = uint $s (S)", 0b00111, 0b00100,
		 [(set I32Reg:$d, (fp_to_uint F32Reg:$s))]>
{ let Itinerary = ItinSCU_5_1; }

def UI32FromDouble : UnaryInstr<
		 (outs I32Reg:$d), (ins  F64Reg:$s),
         "$d = uint $s", 0b00111, 0b00010,
		 [(set I32Reg:$d, (fp_to_uint F64Reg:$s))]>
{ let Itinerary = ItinSCU_5_1; }

// abs for single/double/SInt32
def AbsSingle : UnaryInstr<
		 (outs F32Reg:$d), (ins  F32Reg:$s),
         "$d = abs $s (S)", 0b01000, 0b00100,
		 [(set F32Reg:$d, (fabs F32Reg:$s))]>
{ let Itinerary = ItinSCU_5_1; }

def AbsDouble : UnaryInstr<
		 (outs F64Reg:$d), (ins  F64Reg:$s),
         "$d = abs $s", 0b01000, 0b00010,
		 [(set F64Reg:$d, (fabs F64Reg:$s))]>
{ let Itinerary = ItinSCU_5_1; }

def AbsSI32 : UnaryInstr<
		 (outs I32Reg:$d), (ins  I32Reg:$s),
         "$d = abs $s", 0b01000, 0b00000,
		 [/*(set I32Reg:$d, (abs I32Reg:$s))*/]>
{ let Itinerary = ItinSCU_5_1; }

// recip for single/double
def RecipSingle : UnaryInstr<
		 (outs F32Reg:$d), (ins  F32Reg:$s),
         "$d = recip $s (S)", 0b01001, 0b00100,
		 [(set F32Reg:$d, (RecipNode F32Reg:$s))]>
{ let Itinerary = ItinSCU_5_1; }

def RecipDouble : UnaryInstr<
		 (outs F64Reg:$d), (ins  F64Reg:$s),
         "$d = recip $s", 0b01001, 0b00010,
		 [(set F64Reg:$d, (RecipNode F64Reg:$s))]>
{ let Itinerary = ItinSCU_5_1; }

/*------------------------AGU Instructions----------------------*/
// load byte, half-word, word with imm offset (J + imm) and
// signed/unsigned extension.
class LdImmInstr< dag outs, dag ins, string asm, bits<6> flags,
		 list<dag> pattern=[] >
	   : LdStImmInstr< outs, ins, asm, 0b0, flags, pattern >;

def LoadSI8JI : LdImmInstr<
    		  (outs I32Reg:$reg), (ins AddrJImm:$addr),
                  "$reg = [$addr] (B)", 0b100000,
                  [(set I32Reg:$reg, (sextloadi8 AddressingJImm:$addr))]>
{ let Itinerary = ItinAGU_5_1_1; }

def LoadSI16JI : LdImmInstr<
    		  (outs I32Reg:$reg), (ins AddrJImm:$addr),
                  "$reg = [$addr] (H)", 0b010000,
                  [(set I32Reg:$reg, (sextloadi16 AddressingJImm:$addr))]>
{ let Itinerary = ItinAGU_5_1_1; }

def LoadUI8JI : LdImmInstr<
    		  (outs I32Reg:$reg), (ins AddrJImm:$addr),
                  "$reg = [$addr] (B)(U)", 0b101000,
                  [(set I32Reg:$reg, (zextloadi8 AddressingJImm:$addr))]>
{ let Itinerary = ItinAGU_5_1_1; }

def LoadUI16JI : LdImmInstr<
    		  (outs I32Reg:$reg), (ins AddrJImm:$addr),
                  "$reg = [$addr] (H)(U)", 0b011000,
                  [(set I32Reg:$reg, (zextloadi16 AddressingJImm:$addr))]>
{ let Itinerary = ItinAGU_5_1_1; }

def LoadI32JI  : LdImmInstr<
    		  (outs I32Reg:$reg), (ins AddrJImm:$addr),
                  "$reg = [$addr]", 0b000000,
                  [(set I32Reg:$reg, (load AddressingJImm:$addr))]>
{ let Itinerary = ItinAGU_5_1_1; }

def LoadF32JI   : LdImmInstr<
    		  (outs F32Reg:$reg), (ins AddrJImm:$addr),
                  "$reg = [$addr]", 0b000000,
                  [(set F32Reg:$reg, (load AddressingJImm:$addr))]>
{ let Itinerary = ItinAGU_5_1_1; }

def LoadF64JI   : LdImmInstr<
    		  (outs F64Reg:$reg), (ins AddrJImm:$addr),
                  "$reg = [$addr]", 0b000100,
                  [(set F64Reg:$reg, (load AddressingJImm:$addr))]>
{ let Itinerary = ItinAGU_5_1_1; }

def LoadPtrJI   : LdImmInstr<
    		  (outs JReg:$reg), (ins AddrJImm:$addr),
                  "$reg = [$addr]", 0b000010,
                  [(set JReg:$reg, (load AddressingJImm:$addr))]>
{ let Itinerary = ItinAGU_5_1_1; }

def LoadPtrOffsetJI   : LdImmInstr<
    		  (outs JReg:$reg), (ins AddrJImm:$addr),
                  "$reg = [$addr]", 0b000010,
                  [(set JReg:$reg, (sextloadi32 AddressingJImm:$addr))]>
{ let Itinerary = ItinAGU_5_1_1; }

/*--------------------- store: with imm's as offset -----------------------*/
class StImmInstr< dag outs, dag ins, string asm, bits<6> flags,
		 list<dag> pattern=[] >
	   : LdStImmInstr< outs, ins, asm, 1, flags, pattern >;

def StoreI8JI : StImmInstr<
    		  (outs), (ins I32Reg:$reg, AddrJImm:$addr),
                  "[$addr] = $reg (B)", 0b100000,
		  [(truncstorei8 I32Reg:$reg, AddressingJImm:$addr)]>
{ let Itinerary = ItinAGU_1_1_1; }

def StoreI16JI : StImmInstr<
    		  (outs), (ins I32Reg:$reg, AddrJImm:$addr),
                  "[$addr] = $reg (H)", 0b010000,
		  [(truncstorei16 I32Reg:$reg, AddressingJImm:$addr)]>
{ let Itinerary = ItinAGU_1_1_1; }

def StoreI32JI   : StImmInstr<
    		  (outs), (ins I32Reg:$reg, AddrJImm:$addr),
                  "[$addr] = $reg", 0b000000,
		  [(store I32Reg:$reg, AddressingJImm:$addr)]>
{ let Itinerary = ItinAGU_1_1_1; }

def StoreF32JI   : StImmInstr<
    		  (outs), (ins F32Reg:$reg, AddrJImm:$addr),
                  "[$addr] = $reg", 0b000000,
		  [(store F32Reg:$reg, AddressingJImm:$addr)]>
{ let Itinerary = ItinAGU_1_1_1; }

def StoreF64JI   : StImmInstr<
    		  (outs), (ins F64Reg:$reg, AddrJImm:$addr),
                  "[$addr] = $reg", 0b000100,
		  [(store F64Reg:$reg, AddressingJImm:$addr)]>
{ let Itinerary = ItinAGU_1_1_1; }

def StorePtrJI   : StImmInstr<
    		  (outs), (ins JReg:$reg, AddrJImm:$addr),
                  "[$addr] = $reg", 0b000010,
		  [(store JReg:$reg, AddressingJImm:$addr)]>
{ let Itinerary = ItinAGU_1_1_1; }

/*-------------------- All other AGU instructions ----------------------*/
// assign immediate value to registers
class AssignImmInstr< dag outs, dag ins, string asm,
				   bits<6> flags,
	  		   	   list<dag> pattern = [] > :
				   AGUInstr< outs, ins, asm, 0b00001, pattern >
{
		bits<5> d;
		bits<11> s;

		let Inst{22} = 0b1; // allow imm-ext
  		let Inst{21-16} = flags;
		let Inst{15-5} = s;
		let Inst{4-0} = d;
  		let isCodeGenOnly = 1;
}

def AssignI32 : AssignImmInstr< 
	     (outs I32Reg:$d),
		 (ins SImm11Int:$s),
       	 "$d = $s", 0b000000,
         [(set I32Reg:$d, SImm32IntLeaf:$s)]>
{ let Itinerary = ItinAGU_2_1; }

def AssignPtr : AssignImmInstr< 
	     (outs JReg:$d),
		 (ins  SImm11Ptr:$s),
       	 "$d = $s", 0b000010,
         [(set JReg:$d, SImm32PtrLeaf:$s)]>
{ let Itinerary = ItinAGU_2_1; }

/* // to be deleted.
def AssignPtrGlobalAddr : AssignImmInstr< 
	     (outs JReg:$d),
		 (ins  SImm11Ptr:$s),
       	 "$d = $s", 0b000010,
         [(set JReg:$d, GlobalAddrLeaf:$s)]>
{ let Itinerary = ItinAGU_2_1; }
*/

/* Semantic Patterns for Instruction Selection */
// Jump tables, global addresses, and constant pools
def : Pat<(jumptable:$in),   (AssignPtr tjumptable:$in)>;
def : Pat<(tglobaladdr:$t), (AssignPtr tglobaladdr:$t)>;
def : Pat<(tconstpool:$in),  (AssignPtr tconstpool:$in)>;

def : Pat<(XferAddr jumptable:$in),   (AssignPtr tjumptable:$in)>;
def : Pat<(XferAddr tglobaladdr:$in), (AssignPtr tglobaladdr:$in)>;
def : Pat<(XferAddr tconstpool:$in),  (AssignPtr tconstpool:$in)>;


def AssignF32 : AssignImmInstr< 
	     (outs F32Reg:$d), 
		 (ins f32imm:$s),
       	 "$d = $s", 0b000000,
         [(set F32Reg:$d, fpimm:$s)]>
{ let Itinerary = ItinAGU_2_1; }

// addr computation.
class PtrBinaryInstr< dag outs, dag ins, string asm,
					bits<5> opcode,
	  		   		list<dag> pattern = [] >
				: AGUInstr< outs, ins, asm, opcode, pattern >
{
		bits<5> d;
		bits<5> s0;
		bits<5> s1;

		let Inst{22} = 0;
  		let Inst{21-15} = 0b0001100; // encoding of flags
		let Inst{14-10} = s1;
		let Inst{9-5} = s0;
		let Inst{4-0} = d;
}

def StackAddr : PseudoInstr<(outs JReg:$dst), (ins AddrJImm:$addr),
                            "should never be emitted ...",
                            [(set JReg:$dst, AddressingJImm:$addr)]>
{
   // frame index is undetermined yet.
   // see eliminateFrameIndex().
   let usesCustomInserter = 0;
   let Itinerary = ItinPseudo;
}

// select between i32
def SelectI32 : PseudoInstr<(outs I32Reg:$d), (ins I32Reg:$c, I32Reg:$v1, I32Reg:$v2),
                            "should never be emitted ...",
                            [(set I32Reg:$d, (select I32Reg:$c, I32Reg:$v1, I32Reg:$v2))]>
{ 
	let usesCustomInserter = 1;
	let isSelect = 1;
    let Itinerary = ItinPseudo;
}

// select between f32
def SelectF32 : PseudoInstr<(outs F32Reg:$d), (ins I32Reg:$c, F32Reg:$v1, F32Reg:$v2),
                            "should never be emitted ...",
                            [(set F32Reg:$d, (select I32Reg:$c, F32Reg:$v1, F32Reg:$v2))]>
{ 
	let usesCustomInserter = 1;
	let isSelect = 1;
    let Itinerary = ItinPseudo;
}

// select between f64
def SelectF64 : PseudoInstr<(outs F64Reg:$d), (ins I32Reg:$c, F64Reg:$v1, F64Reg:$v2),
                            "should never be emitted ...",
                            [(set F64Reg:$d, (select I32Reg:$c, F64Reg:$v1, F64Reg:$v2))]>
{
	let usesCustomInserter = 1;
	let isSelect = 1;
    let Itinerary = ItinPseudo;
}

def PtrAdd : PtrBinaryInstr< 
		 (outs JReg:$d), 
		 (ins  JReg:$s0, JReg:$s1),
         "$d = $s0 + $s1", 0b00100,
		 [(set JReg:$d, (add JReg:$s0, JReg:$s1))]>
{ let Itinerary = ItinAGU_2_1_1; }

def PtrSub : PtrBinaryInstr< 
		 (outs JReg:$d), 
		 (ins  JReg:$s0, JReg:$s1),
         "$d = $s0 - $s1", 0b00101,
		 [(set JReg:$d, (add JReg:$s0, JReg:$s1))]>
{ let Itinerary = ItinAGU_2_1_1; }

def PtrShiftLeft : PtrBinaryInstr< 
		   	 	(outs JReg:$d), (ins  JReg:$s0, JReg:$s1),
         		"$d = $s0 << $s1", 0b00110,
         		[(set JReg:$d, (shl JReg:$s0, JReg:$s1))] >
{ let Itinerary = ItinAGU_2_1_1; }

def PtrShiftLeftImm : PtrBinaryInstr< 
		   	 	(outs JReg:$d), (ins  JReg:$s0, i32imm:$s1),
         		"$d = $s0 << $s1", 0b00111,
         		[(set JReg:$d, (shl JReg:$s0, UImm5PtrLeaf:$s1))] >
{ let Itinerary = ItinAGU_2_1_1; }

/*------------ Transfer register. ---------------*/
class TransferInstr< dag outs, dag ins, string asm,
	  				  bits<5> opcode, bits<7> flags,
					  list<dag> pattern>
			: AGUInstr < outs, ins, asm, opcode, pattern >
{
		bits<5> d;
		bits<5> s;

		let Inst{22} = 0;
		let Inst{21-15} = flags;
		let Inst{9-5} = s;
		let Inst{4-0} = d;
		let isCodeGenOnly = 1;
}

def I32RegFromI32Reg : TransferInstr<
		(outs I32Reg:$d), 
		(ins  I32Reg:$s),
       	"$d = $s", 0b01000, 0b0000000,
        [/*(set I32Reg:$d, I32Reg:$s)*/]>
{ let Itinerary = ItinAGU_2_1; }

def F32RegFromF32Reg : TransferInstr<
		(outs F32Reg:$d), 
		(ins  F32Reg:$s),
       	"$d = $s", 0b01000, 0b0000000,
        [/*(set F32Reg:$d, F32Reg:$s)*/]>
{ let Itinerary = ItinAGU_2_1; }

def PtrRegFromPtrReg : TransferInstr<
		(outs JReg:$d), 
		(ins  JReg:$s),
       	"$d = $s", 0b01000, 0b0001100,
        [/*(set JReg:$d, JReg:$s)*/]>
{ let Itinerary = ItinAGU_2_1; }

def F64RegFromF64Reg : TransferInstr <
		(outs F64Reg:$d), 
		(ins  F64Reg:$s),
       	"$d = $s", 0b01001, 0b0000010,
        [/*(set F64Reg:$d, F64Reg:$s)*/]>
{ let Itinerary = ItinAGU_2_1; }

def I32RegFromPtrReg : TransferInstr<
		(outs I32Reg:$d), 
		(ins  JReg:$s),
       	"$d = $s", 0b01000, 0b0001000,
        [/*(set I32Reg:$d, (JReg:$s))*/]>
{ let Itinerary = ItinAGU_2_1; }

def PtrRegFromI32Reg : TransferInstr<
		(outs JReg:$d), 
		(ins  I32Reg:$s),
       	"$d = $s", 0b01000, 0b0000100,
        [/*(set JReg:$d, (I32Reg:$s))*/]>
{ let Itinerary = ItinAGU_2_1; }

/*----------------------------------------------------------------------------*/
// load integers in byte, half-word, word with signed/unsigned
// extension. Jm's are used as offset.
class LdStInstr< dag outs, dag ins, string asm,
	  			 bits<5> opcode, bits<7> flags,
		 		 list<dag> pattern=[] >
	   : AGUInstr< outs, ins, asm, opcode, pattern >
{
		bits<5> reg;
		bits<10> addr;

		let Inst{22} = 0;
		let Inst{21-15} = flags;
		let Inst{14-10} = addr{9-5}; // offset
		let Inst{9-5} = addr{4-0}; 	 // base
		let Inst{4-0} = reg;
		let isCodeGenOnly = 1;
}

def AtomLoad : LdStInstr<
    		  (outs I32Reg:$reg), (ins AddrJJ:$addr),
              "$reg = [$addr] (AT)", 0b11010, 0b0001000,
              [(set I32Reg:$reg, (atomic_load AddressingJJ:$addr))]>
{ let Itinerary = ItinAGU_5_1_1; }

def AtomStore : LdStInstr<
    		  (outs), (ins AddrJJ:$addr, I32Reg:$reg),
              "[$addr] = $reg (AT)", 0b11011, 0b0001000,
              [(atomic_store I32Reg:$reg, AddressingJJ:$addr)]>
{ let Itinerary = ItinAGU_1_1_1; }

// Fixme: this does not work because I32Reg:$r is a temperary.
/*
def : Pattern<(set I32Reg:$r, (atomic_load_add AddressingJJ:$addr, I32Reg:$reg)),
	      [(set I32Reg:$r, (AddSI32T (AtomLoad AddressingJJ:$addr), I32Reg:$reg)),
		   (AtomStore AddressingJJ:$addr, I32Reg:$r)]>;
*/

def  AtomLoadAdd : PseudoInstr<(outs I32Reg:$r), (ins AddrJJ:$addr, I32Reg:$reg),
			"AtomLoadAdd should not be emitted",
			[(set I32Reg:$r, (atomic_load_add AddressingJJ:$addr, I32Reg:$reg))]>
{
  let usesCustomInserter = 1;
  let Itinerary = ItinAGU_5_1_1_1;
}

def __atomicld: LdStInstr<(outs I32Reg:$reg), (ins JReg:$base, JReg:$offset),
                    "$reg = [$base + $offset] (AT)", 0b11010, 0b0001000,
                    [( set I32Reg:$reg, (int_mspu_atomicld JReg:$base, JReg:$offset) )]>
{
   bits<5> offset;
   bits<5> base;
		let Inst{14-10} = offset; // offset
		let Inst{9-5} = base; 	 // base
  let Itinerary = ItinAGU_5_1_1;
}

def __atomicst: LdStInstr<(outs I32Reg:$ret), (ins JReg:$base, JReg:$offset, I32Reg:$reg),
                    "[$base + $offset] = $reg (AT)", 0b11011, 0b0001000,
                    [( set I32Reg:$ret, (int_mspu_atomicst JReg:$base, JReg:$offset, I32Reg:$reg))]>
{
   bits<5> offset;
   bits<5> base;
		let Inst{14-10} = offset; // offset
		let Inst{9-5} = base; 	 // base
  string Constraints = "$reg = $ret";
  let Itinerary = ItinAGU_1_1_1;
}
/**********************************************************************/
class LoadInstr< dag outs, dag ins, string asm,
	  			 bits<7> flags,
		 		 list<dag> pattern=[] >
	   : LdStInstr< outs, ins, asm, 0b11000, flags, pattern >;

/*--------------------load: use Jm's as offset--------------------*/
def LoadSI8 : LoadInstr<
    		  (outs I32Reg:$reg), (ins AddrJJ:$addr),
                  "$reg = [$addr] (B)", 0b1001000,
                  [(set I32Reg:$reg, (sextloadi8 AddressingJJ:$addr))]>
{ let Itinerary = ItinAGU_5_1_1; }

def LoadSI16 : LoadInstr<
    		  (outs I32Reg:$reg), (ins AddrJJ:$addr),
                  "$reg = [$addr] (H)", 0b0101000,
                  [(set I32Reg:$reg, (sextloadi16 AddressingJJ:$addr))]>
{ let Itinerary = ItinAGU_5_1_1; }

def LoadUI8 : LoadInstr<
    		  (outs I32Reg:$reg), (ins AddrJJ:$addr),
                  "$reg = [$addr] (B)(U)", 0b1011000,
                  [(set I32Reg:$reg, (zextloadi8 AddressingJJ:$addr))]>
{ let Itinerary = ItinAGU_5_1_1; }

def LoadUI16 : LoadInstr< 
    		  (outs I32Reg:$reg), (ins AddrJJ:$addr),
                  "$reg = [$addr] (H)(U)", 0b1110000,
                  [(set I32Reg:$reg, (zextloadi16 AddressingJJ:$addr))]>
{ let Itinerary = ItinAGU_5_1_1; }

def LoadI32   : LoadInstr<
    		  (outs I32Reg:$reg), (ins AddrJJ:$addr),
                  "$reg = [$addr]", 0b0001000,
                  [(set I32Reg:$reg, (load AddressingJJ:$addr))]>
{ let Itinerary = ItinAGU_5_1_1; }

def LoadF32   : LoadInstr<
    		  (outs F32Reg:$reg), (ins AddrJJ:$addr),
                  "$reg = [$addr]", 0b0001000,
                  [(set F32Reg:$reg, (load AddressingJJ:$addr))]>
{ let Itinerary = ItinAGU_5_1_1; }

def LoadF64   : LoadInstr<
    		  (outs F64Reg:$reg), (ins AddrJJ:$addr),
                  "$reg = [$addr]", 0b0001010,
                  [(set F64Reg:$reg, (load AddressingJJ:$addr))]>
{ let Itinerary = ItinAGU_5_1_1; }

def LoadPtr   : LoadInstr<
    		  (outs JReg:$reg), (ins AddrJJ:$addr),
                  "$reg = [$addr]", 0b0001100,
                  [(set JReg:$reg, (load AddressingJJ:$addr))]>
{ let Itinerary = ItinAGU_5_1_1; }

def LoadPtrOffset   : LoadInstr<
    		  (outs JReg:$reg), (ins AddrJJ:$addr),
                  "$reg = [$addr]", 0b0001100,
                  [(set JReg:$reg, (sextloadi32 AddressingJJ:$addr))]>
{ let Itinerary = ItinAGU_5_1_1; }

/*--------------------- store: with Jm's as offset -----------------------*/
class StoreInstr< dag outs, dag ins, string asm,
	  			 bits<7> flags,
		 		 list<dag> pattern=[] >
	   : LdStInstr< outs, ins, asm, 0b11001, flags, pattern >;

def StoreI8 : StoreInstr<
    		  (outs), (ins I32Reg:$reg, AddrJJ:$addr),
                  "[$addr] = $reg (B)", 0b1001000,
		  [(truncstorei8 I32Reg:$reg, AddressingJJ:$addr)]>
{ let Itinerary = ItinAGU_1_1_1; }

def StoreI16 : StoreInstr<
    		  (outs), (ins I32Reg:$reg, AddrJJ:$addr),
                  "[$addr] = $reg (H)", 0b0101000,
		  [(truncstorei16 I32Reg:$reg, AddressingJJ:$addr)]>
{ let Itinerary = ItinAGU_1_1_1; }

def StoreI32   : StoreInstr<
    		  (outs), (ins I32Reg:$reg, AddrJJ:$addr),
                  "[$addr] = $reg", 0b0001000,
		  [(store I32Reg:$reg, AddressingJJ:$addr)]>
{ let Itinerary = ItinAGU_1_1_1; }

def StoreF32   : StoreInstr<
    		  (outs), (ins F32Reg:$reg, AddrJJ:$addr),
                  "[$addr] = $reg", 0b0001000,
		  [(store F32Reg:$reg, AddressingJJ:$addr)]>
{ let Itinerary = ItinAGU_1_1_1; }

def StoreF64   : StoreInstr<
    		  (outs), (ins F64Reg:$reg, AddrJJ:$addr),
                  "[$addr] = $reg", 0b0001010,
		  [(store F64Reg:$reg, AddressingJJ:$addr)]>
{ let Itinerary = ItinAGU_1_1_1; }

def StorePtr   : StoreInstr<
    		  (outs), (ins JReg:$reg, AddrJJ:$addr),
                  "[$addr] = $reg", 0b0001100,
		  [(store JReg:$reg, AddressingJJ:$addr)]>
{ let Itinerary = ItinAGU_1_1_1; }

//===----------------------------------------------------------------------===//
//						Seq Instructions
//===----------------------------------------------------------------------===//

/*----------- jump ------------*/
// absolute branch is barrier, while conditional branch is not.
class JumpInstr< dag outs, dag ins, string asm, bits<6> opcode,
	  		   	 list<dag> pattern = [] >
		: SeqInstr<outs, ins, asm, opcode, pattern > {
	let isTerminator=1;
	let isPredicable = 0;
}

// branch to a PC-relative immediate address.
def JumpImm : JumpInstr<
	    (outs), (ins SImm17Ptr:$t),
        "jump $t", 0b010001 >
{
	bits<17> t;
	let Inst{21-5}	= t;
	let isBranch = 1;
	let isBarrier = 1;
	let Itinerary = ItinSEQNoReg;
}

def JumpBasicBlock : JumpInstr<
	    (outs), (ins BasicBlock:$t),
        "jump $t", 0b010001,
		[(br bb:$t)] > // 'bb' is an SDNode for BasicBlock.
{
	bits<17> t;
	let Inst{21-5}	= t;
	let isBarrier = 1;
	let isBranch = 1;
	let isCodeGenOnly = 1;// jump imm
	let Itinerary = ItinSEQNoReg;
}

// branch to a pointer address which is PC-irrelative.
def JumpPtr : JumpInstr< 
	    (outs), (ins JReg:$t),
		"jump $t", 0b010000,
	 	[(brind JReg:$t)]>
{
	bits<5> t;
  	let Inst{9-5}	= t;
	let isBarrier = 1;
 	let isIndirectBranch = 1;
	let Itinerary = ItinSEQ_1;
}

def Return : JumpInstr< 
	    (outs), (ins),
		"jump J30", 0b010000,
	 	[( MSPU_Ret /*brind JReg:$t*/ )]>
{
  let Inst{9-5}	= 30;
  let isBarrier = 1;
  let isIndirectBranch = 1;
  let isReturn = 1;
  let Uses = [J30];
	let Itinerary = ItinSEQ_1;
}

// conditionally branch to a PC-relative immediate address.
def JumpImmCond : JumpInstr<
	    (outs), (ins I32Reg:$c, SImm17Ptr:$t),
        "if $c, jump $t", 0b010011 >
{
	bits<5> c;
	bits<17> t;
	let Inst{21-5}	= t;
	let Inst{4-0} = c;

	let isBranch = 1;
	let Itinerary = ItinSEQ_1;
}

def JumpBasicBlockCond : JumpInstr<
	    (outs), (ins I32Reg:$c, BasicBlock:$t),
        "if $c, jump $t", 0b010011, [(brcond I32Reg:$c, bb:$t)] >
{
	bits<5> c;
	bits<17> t;
	let Inst{21-5}	= t;
	let Inst{4-0} = c;

	let isBranch = 1;
	let isCodeGenOnly = 1; // if rx, jump imm
	let Itinerary = ItinSEQ_1;
}

// conditionally branch to a pointer address which is PC-irrelative.
def JumpPtrCond : JumpInstr< 
	    (outs), (ins I32Reg:$c, JReg:$t),
		"if $c, jump $t", 0b010010 >
{
	bits<5> c;
	bits<5> t;
 	let Inst{9-5}	= t;
	let Inst{4-0} = c;

 	let isIndirectBranch = 1;
	let Itinerary = ItinSEQ_1;
}

/*------------------------- call ---------------------------------*/
class CallInstr< dag outs, dag ins, string asm, bits<6> opcode,
	  		   	 list<dag> pattern = [] >
		: SeqInstr<outs, ins, asm, opcode, pattern> {
	let isPredicable = 0;
  //let hasSideEffects = 0;
	let isCall = 1;
	let hasDelaySlot = 0;

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
	    (outs), (ins SImm17Ptr:$t, variable_ops),
        "call $t", 0b100001,
		[(MSPU_Call SImm17PtrLeaf:$t)]>
{
	bits<17> t;
	let Inst{21-5}	= t;
	let Itinerary = ItinSEQNoReg;
}

def : Pat<(MSPU_Call  tglobaladdr:$t), (CallImm  tglobaladdr:$t)>;
def : Pat<(MSPU_Call texternalsym:$t), (CallImm texternalsym:$t)>;

// call via a pointer address which is PC-irrelative.
def CallPtr : CallInstr< 
	    (outs), (ins JReg:$t, variable_ops),
		"call $t", 0b100000,
	 	[(MSPU_Call JReg:$t)]>
{
	bits<5> t;
  let Inst{9-5}	= t;
	let Itinerary = ItinSEQ_1;
}

// conditionally call via a PC-relative immediate address.
def CallImmCond : CallInstr<
	    (outs), (ins I32Reg:$c, SImm17Ptr:$t, variable_ops),
        "if $c, call $t", 0b100011 >
{
	bits<5> c;
	bits<17> t;
	let Inst{21-5}	= t;
	let Inst{4-0} = c;
	let Itinerary = ItinSEQ_1;
}

// conditionally call via a pointer address which is PC-irrelative.
def CallPtrCond : CallInstr< 
	    (outs), (ins I32Reg:$c, JReg:$t, variable_ops),
		"if $c, call $t", 0b100010 >
{
	bits<5> c;
	bits<5> t;
 	let Inst{9-5}	= t;
	let Inst{4-0} = c;
	let Itinerary = ItinSEQ_1;
}

// this is a virtual instruction for simulating syscall
def SysCall : CallInstr< 
	    (outs), (ins I32Reg:$r),
		"syscall $r", 0b111110,
		[(int_mspu_syscall I32Reg:$r)]>
{
  bits<5> r;
  let Inst{9-5} = r;
}

def SysCallImm : CallInstr< 
	    (outs), (ins i32imm:$r),
		"syscall $r", 0b111111,
		[(int_mspu_syscall UImm12IntLeaf:$r)]>
{
  bits<12> r;
  let Inst{21-10} = r;
}

/*************** Loop ************/
def LoopL0 : SeqInstr<(outs), (ins SImm17Ptr:$t, JReg:$j),
		"LpTo $t BY $j (L0)", 0b110001, [(int_mspu_loop tblockaddress:$t, JReg:$j)]>
{
	bits<17> t;
	bits<5> j;
	let Inst{21-5} = t;
	let Inst{4-0} = j;
  let Itinerary = ItinSEQ_1;
}

def LoopL1 : SeqInstr< 
	    (outs),
 		(ins SImm17Ptr:$t, PtrReg:$j),
		"LpTo $t BY $j (L1)", 0b110011>
{
	bits<17> t;
	bits<5> j;
	let Inst{21-5} = t;
	let Inst{4-0} = j;
}

def NOP : SeqInstr< 
	    (outs),	(ins),
		"NOP", 0b000000, [(int_mspu_delay)]>
{
	let Inst{21-0} = 0;
  let Itinerary = ItinSEQNoReg;
}

def Stop : SeqInstr< 
	    (outs),	(ins),
		"SPU.Stop", 0b000100>
{
	let Inst{21-0} = 0;
}

/**************************************************************************
                            Intrinnsics for SYN
 **************************************************************************/
//// KB
def IntrinKB: Operand<i32> {
  let PrintMethod =    "printIntrinKB";
}

def __SetKB : SynInstr<(outs), (ins IntrinKB:$i, I32Reg:$r),
                    "$i = $r", 0b00100,
                    [(int_mspu_setKB UImm4IntLeaf:$i, I32Reg:$r)]>
{
  bits<5> r;
  bits<4> i;
  let Inst{11-7} = r;
  let Inst{6-4}  = 0b000;
  let Inst{3-0}  = i;
  let Itinerary = ItinSYN_1_1;
}

def __GetKB : SynInstr<(outs I32Reg:$r), (ins IntrinKB:$i),
                    "$r = $i", 0b00101,
                    [(set I32Reg:$r, (int_mspu_getKB UImm4IntLeaf:$i))]>
{
  bits<5> r;
  bits<4> i;
  let Inst{11-7} = r;
  let Inst{6-4}  = 0b000;
  let Inst{3-0}  = i;
  let Itinerary = ItinSYN_2_1;
}

//// KE
def IntrinKE: Operand<i32> {
  let PrintMethod =    "printIntrinKE";
}

def __SetKE : SynInstr<(outs), (ins IntrinKE:$i, I32Reg:$r),
                    "$i = $r", 0b00100,
                    [(int_mspu_setKE UImm4IntLeaf:$i, I32Reg:$r)]>
{
  bits<5> r;
  bits<4> i;
  let Inst{11-7} = r;
  let Inst{6-4}  = 0b001;
  let Inst{3-0}  = i;
  let Itinerary = ItinSYN_1_1;
}

def __GetKE : SynInstr<(outs I32Reg:$r), (ins IntrinKE:$i),
                    "$r = $i", 0b00101,
                    [(set I32Reg:$r, (int_mspu_getKE UImm4IntLeaf:$i))]>
{
  bits<5> r;
  bits<4> i;
  let Inst{11-7} = r;
  let Inst{6-4}  = 0b001;
  let Inst{3-0}  = i;
  let Itinerary = ItinSYN_2_1;
}

//// KS
def IntrinKS: Operand<i32> {
  let PrintMethod =    "printIntrinKS";
}

def __SetKS : SynInstr<(outs), (ins IntrinKS:$i, I32Reg:$r),
                    "$i = $r", 0b00100,
                    [(int_mspu_setKS UImm4IntLeaf:$i, I32Reg:$r)]>
{
  bits<5> r;
  bits<4> i;
  let Inst{11-7} = r;
  let Inst{6-4}  = 0b010;
  let Inst{3-0}  = i;
  let Itinerary = ItinSYN_1_1;
}

def __GetKS : SynInstr<(outs I32Reg:$r), (ins IntrinKS:$i),
                    "$r = $i", 0b00101,
                    [(set I32Reg:$r, (int_mspu_getKS UImm4IntLeaf:$i))]>
{
  bits<5> r;
  bits<4> i;
  let Inst{11-7} = r;
  let Inst{6-4}  = 0b010;
  let Inst{3-0}  = i;
  let Itinerary = ItinSYN_2_1;
}

//// KI
def IntrinKI: Operand<i32> {
  let PrintMethod =    "printIntrinKI";
}

def __SetKI : SynInstr<(outs), (ins IntrinKI:$i, I32Reg:$r),
                    "$i = $r", 0b00100,
                    [(int_mspu_setKI UImm4IntLeaf:$i, I32Reg:$r)]>
{
  bits<5> r;
  bits<4> i;
  let Inst{11-7} = r;
  let Inst{6-4}  = 0b011;
  let Inst{3-0}  = i;
  let Itinerary = ItinSYN_1_1;
}

def __GetKI : SynInstr<(outs I32Reg:$r), (ins IntrinKI:$i),
                    "$r = $i", 0b00101,
                    [(set I32Reg:$r, (int_mspu_getKI UImm4IntLeaf:$i))]>
{
  bits<5> r;
  bits<4> i;
  let Inst{11-7} = r;
  let Inst{6-4}  = 0b011;
  let Inst{3-0}  = i;
  let Itinerary = ItinSYN_2_1;
}

//// KM
def IntrinKM: Operand<i32> {
  let PrintMethod =    "printIntrinKM";
}

def __SetKM : SynInstr<(outs), (ins IntrinKM:$i, I32Reg:$r),
                    "$i = $r", 0b00100,
                    [(int_mspu_setKM UImm4IntLeaf:$i, I32Reg:$r)]>
{
  bits<5> r;
  bits<2> i;
  let Inst{11-7} = r;
  let Inst{6-4}  = 0b100;
  let Inst{3-2}  = i;
  let Inst{1-0}  = 0b01;
  let Itinerary = ItinSYN_1_1;
}

def __GetKM : SynInstr<(outs I32Reg:$r), (ins IntrinKM:$i),
                    "$r = $i", 0b00101,
                    [(set I32Reg:$r, (int_mspu_getKM UImm4IntLeaf:$i))]>
{
  bits<5> r;
  bits<2> i;
  let Inst{11-7} = r;
  let Inst{6-4}  = 0b100;
  let Inst{3-2}  = i;
  let Inst{1-0}  = 0b01;
  let Itinerary = ItinSYN_2_1;
}

def __SetHiKM : SynInstr<(outs), (ins IntrinKM:$i, I32Reg:$r),
                    "$i = $r (H)", 0b00100,
                    [(int_mspu_setHiKM UImm4IntLeaf:$i, I32Reg:$r)]>
{
  bits<5> r;
  bits<2> i;
  let Inst{11-7} = r;
  let Inst{6-4}  = 0b100;
  let Inst{3-2}  = i;
  let Inst{1}  = 1;
  let Inst{0}  = 0;
  let Itinerary = ItinSYN_1_1;
}

def __GetHiKM : SynInstr<(outs I32Reg:$r), (ins IntrinKM:$i),
                    "$r = $i (H)", 0b00101,
                    [(set I32Reg:$r, (int_mspu_getHiKM UImm4IntLeaf:$i))]>
{
  bits<5> r;
  bits<2> i;
  let Inst{11-7} = r;
  let Inst{6-4}  = 0b100;
  let Inst{3-2}  = i;
  let Inst{1}  = 1;
  let Inst{0}  = 0;
  let Itinerary = ItinSYN_2_1;
}

//// KG
def IntrinKG: Operand<i32> {
  let PrintMethod =    "printIntrinKG";
}

def __SetKG : SynInstr<(outs), (ins IntrinKG:$i, I32Reg:$r),
                    "$i = $r", 0b00100,
                    [(int_mspu_setKG UImm4IntLeaf:$i, I32Reg:$r)]>
{
  bits<5> r;
  bits<2> i;
  let Inst{11-7} = r;
  let Inst{6-4}  = 0b101;
  let Inst{3-2}  = i;
  let Itinerary = ItinSYN_1_1;
}

def __GetKG : SynInstr<(outs I32Reg:$r), (ins IntrinKG:$i),
                    "$r = $i", 0b00101,
                    [(set I32Reg:$r, (int_mspu_getKG UImm4IntLeaf:$i))]>
{
  bits<5> r;
  bits<2> i;
  let Inst{11-7} = r;
  let Inst{6-4}  = 0b101;
  let Inst{3-2}  = i;
  let Itinerary = ItinSYN_2_1;
}

//// KL
def IntrinKL: Operand<i32> {
  let PrintMethod =    "printIntrinKL";
}

def __SetKL : SynInstr<(outs), (ins IntrinKL:$i, I32Reg:$r),
                    "$i = $r", 0b00100,
                    [(int_mspu_setKL UImm4IntLeaf:$i, I32Reg:$r)]>
{
  bits<5> r;
  bits<2> i;
  let Inst{11-7} = r;
  let Inst{6-4}  = 0b110;
  let Inst{3-2}  = i;
  let Itinerary = ItinSYN_1_1;
}

def __GetKL : SynInstr<(outs I32Reg:$r), (ins IntrinKL:$i),
                    "$r = $i", 0b00101,
                    [(set I32Reg:$r, (int_mspu_getKL UImm4IntLeaf:$i))]>
{
  bits<5> r;
  bits<2> i;
  let Inst{11-7} = r;
  let Inst{6-4}  = 0b110;
  let Inst{3-2}  = i;
  let Itinerary = ItinSYN_2_1;
}

//////////////////////////////////////////////////////////////////
def __GetCStat : SynInstr<(outs I32Reg:$r), (ins),
                    "$r = CStat", 0b00101,
                    [(set I32Reg:$r, (int_mspu_getCStat))]>
{
  bits<5> r;
  let Inst{11-7} = r;
  let Inst{6-4}  = 0b111;
	let Itinerary = ItinSYN_2;
}
//////////////////////////////////
def __SetLPCtrl : SynInstr<(outs), (ins I32Reg:$r),
                    "LPCTR = $r", 0b01110,
                    [(int_mspu_setLPCtrl I32Reg:$r)]>
{
  bits<5> r;
  let Inst{11-7} = r;
	let Itinerary = ItinSYN_1;
}

def __GetLPCtrl : SynInstr<(outs I32Reg:$r), (ins),
                    "$r = LPCTR", 0b01111,
                    [(set I32Reg:$r, (int_mspu_getLPCtrl))]>
{
  bits<5> r;
  let Inst{4-0} = r;
	let Itinerary = ItinSYN_2;
}

//////////////////////////////////
def IntrinMC: Operand<i32> {
  let PrintMethod =    "printIntrinMC";
}

def __SetMC : SynInstr<(outs), (ins IntrinMC:$i, I32Reg:$r),
                    "$i = $r", 0b01100,
                    [(int_mspu_setMC UImm3IntLeaf:$i, I32Reg:$r)]>
{
  bits<5> r;
  bits<3> i;
  let Inst{11-7} = r;
  let Inst{2-0} = i;
  let Itinerary = ItinSYN_1_1;
}

def __GetMC : SynInstr<(outs I32Reg:$r), (ins IntrinMC:$i),
                    "$r = $i", 0b01101,
                    [(set I32Reg:$r, (int_mspu_getMC UImm3IntLeaf:$i))]>
{
  bits<5> r;
  bits<3> i;
  let Inst{11-7} = r;
  let Inst{2-0} = i;
  let Itinerary = ItinSYN_2_1;
}

//////////////////// callm ////////////////////////////
def __CallM : SynInstr<(outs), (ins UImm17Ptr:$d),
                    "CallM $d", 0b00001,
                    [(int_mspu_callm GlobalAddrLeaf:$d)]>
{
  
  bits<17> d;
  let Inst{22} = 1;
  let Inst{21-5} = d;
	let Itinerary = ItinSYNNoReg;
}

def __CallMB : SynInstr<(outs), (ins UImm17Ptr:$d),
                    "CallM $d (B)", 0b00001,
                    [(int_mspu_callmb GlobalAddrLeaf:$d)]>
{
  bits<17> d;
  let Inst{22} = 1;
  let Inst{21-5} = d;
  let Inst{0} = 1;
	let Itinerary = ItinSYNNoReg;
}

def __CallMIndr : SynInstr<(outs), (ins PtrReg:$d),
                    "CallM $d", 0b00001,
                    [(int_mspu_callm PtrReg:$d)]>
{
  
  bits<5> d;
  let Inst{9-5} = d;
	let Itinerary = ItinSYN_1;
}

def __CallMIndrB : SynInstr<(outs), (ins PtrReg:$d),
                    "CallM $d (B)", 0b00001,
                    [(int_mspu_callmb PtrReg:$d)]>
{
  bits<5> d;
  let Inst{9-5} = d;
  let Inst{0} = 1;
	let Itinerary = ItinSYN_1;
}

////// intrinsics for SVR access /////
def IntrinSVR: Operand<i32> {
  let PrintMethod =    "printIntrinSVR";
}

def IntrinAccessMode: Operand<i32> {
  let PrintMethod =    "printIntrinAccessMode";
}

def __setSVR: SynInstr<(outs), (ins IntrinSVR:$svr, I32Reg:$indx, I32Reg:$val, IntrinAccessMode:$mode),
                    "$svr[ $indx ] = $val $mode", 0b10000,
                    [( int_mspu_setSVR UImm1IntLeaf:$svr, I32Reg:$indx, I32Reg:$val, UImm2IntLeaf:$mode )]>
{
  bits<5> indx;
  bits<5> val;
  bits<2> mode;
  bits<1> svr;
  let Inst{22-21} = mode;
  let Inst{18-14} = indx;
  let Inst{11-7} = val;
  let Inst{5} = svr;
  let Itinerary = ItinSYN_1_1;
}

def __getSVR: SynInstr<(outs I32Reg:$val), (ins IntrinSVR:$svr, I32Reg:$indx, IntrinAccessMode:$mode),
                    "$val= $svr[ $indx ] $mode", 0b10001,
                    [( set I32Reg:$val, (int_mspu_getSVR UImm1IntLeaf:$svr, I32Reg:$indx, UImm2IntLeaf:$mode) )]>
{
  bits<5> indx;
  bits<5> val;
  bits<2> mode;
  bits<1> svr;
  let Inst{22-21} = mode;
  let Inst{11-7} = indx;
  let Inst{4-0} = val;
  let Inst{5} = svr;
  let Itinerary = ItinSYN_1_1;
}

/////// shu access /////////////
def IntrinSHU: Operand<i32> {
  let PrintMethod =    "printIntrinSHU";
}

def IntrinT: Operand<i32> {
  let PrintMethod =    "printIntrinT";
}

def __setSHU: SynInstr<(outs), (ins IntrinSHU:$shu, IntrinT:$t, I32Reg:$val, IntrinAccessMode:$mode),
                    "$shu.$t = Vect $val $mode", 0b01010,
                    [( int_mspu_setSHU UImm1IntLeaf:$shu, UImm2IntLeaf:$t, I32Reg:$val, UImm2IntLeaf:$mode )]>
{
  bits<1> shu;
  bits<2> t;
  bits<2> mode;
  bits<5> val;
  let Inst{22-21} = mode;
  let Inst{11-7} = val;
  let Inst{2} = shu;
  let Inst{1-0} = t;
  let Itinerary = ItinSYN_1_1;
}

def __getSHU: SynInstr<(outs I32Reg:$val), (ins IntrinSHU:$shu, IntrinT:$t, IntrinAccessMode:$mode),
                    "$val = $shu.$t $mode", 0b01001,
                    [( set I32Reg:$val, (int_mspu_getSHU UImm1IntLeaf:$shu, UImm2IntLeaf:$t, UImm2IntLeaf:$mode) )]>
{
  bits<1> shu;
  bits<2> t;
  bits<2> mode;
  bits<5> val;
  let Inst{22-21} = mode;
  let Inst{11-7} = val;
  let Inst{2} = shu;
  let Inst{1-0} = t;
  let Itinerary = ItinSYN_1_1;
}

def __setSHU64: SynInstr<(outs), (ins IntrinSHU:$shu, IntrinT:$t, F64Reg:$val),
                    "$shu.$t = Vect $val", 0b01010,
                    [( int_mspu_setSHU64 UImm1IntLeaf:$shu, UImm2IntLeaf:$t, F64Reg:$val )]>
{
  bits<1> shu;
  bits<2> t;
  bits<5> val;
  let Inst{19} = 1; // DR
  let Inst{11-7} = val;
  let Inst{2} = shu;
  let Inst{1-0} = t;
  let Itinerary = ItinSYN_1_1;
}

def __getSHU64: SynInstr<(outs F64Reg:$val), (ins IntrinSHU:$shu, IntrinT:$t),
                    "$val = $shu.$t", 0b01001,
                    [( set F64Reg:$val, (int_mspu_getSHU64 UImm1IntLeaf:$shu, UImm2IntLeaf:$t) )]>
{
  bits<1> shu;
  bits<2> t;
  bits<5> val;
  let Inst{19} = 1; // DR
  let Inst{11-7} = val;
  let Inst{2} = shu;
  let Inst{1-0} = t;
  let Itinerary = ItinSYN_1_1;
}

// void __mspu_setSHUWithSVR(const int SHU, const int T, const int svr);
def __setSHUWithSVR: SynInstr<(outs), (ins IntrinSHU:$shu, IntrinT:$t, IntrinSVR:$svr),
                    "$shu.$t = $svr", 0b01000,
                    [( int_mspu_setSHUWithSVR UImm1IntLeaf:$shu, UImm2IntLeaf:$t, UImm1IntLeaf:$svr )]>
{
  bits<1> shu;
  bits<2> t;
  bits<1> svr;
  let Inst{5} = svr;
  let Inst{2} = shu;
  let Inst{1-0} = t;
  let Itinerary = ItinSYN_1_1;
}

//void __mspu_setSVRWithSHU(const int svr, const int SHU, const int T);
def __setSVRWithSHU: SynInstr<(outs), (ins IntrinSVR:$svr, IntrinSHU:$shu, IntrinT:$t),
                    "$svr = $shu.$t", 0b01001,
                    [( int_mspu_setSVRWithSHU UImm1IntLeaf:$svr, UImm1IntLeaf:$shu, UImm2IntLeaf:$t )]>
{
  bits<1> shu;
  bits<2> t;
  bits<1> svr;
  let Inst{5} = svr;
  let Inst{2} = shu;
  let Inst{1-0} = t;
  let Itinerary = ItinSYN_1_1;
}

////////  channel access ////////
class ChInstr< dag outs, dag ins, string asm, bits<5> opc, list<dag> pattern >
	  	: AGUInstr<outs, ins, asm, opc, pattern>
{
	  bits<5> d;
	  bits<5> s;
	  
	  let Inst{9-5} = s{4-0};
	  let Inst{4-0} = d{4-0};
	  
	  let isCodeGenOnly = 1;
}

def __SetCh : ChInstr<(outs), (ins i32imm:$d, I32Reg:$s), "CH[$d] = $s",
                       0b10000, [(int_mspu_setCh UImm5IntLeaf:$d, I32Reg:$s)]>
{ let Itinerary = ItinAGU_1_1; }

def __SetChIndr : ChInstr<(outs), (ins I32Reg:$s, I32Reg:$d), "CH[$d] = $s",
                       0b10001, [(int_mspu_setChIndr I32Reg:$d, I32Reg:$s)]>
{ let Itinerary = ItinAGU_1_1; }

def __GetCh : ChInstr<(outs I32Reg:$d), (ins i32imm:$s), "$d = CH[$s]",
                       0b10010, [(set I32Reg:$d, (int_mspu_getCh UImm5IntLeaf:$s))]>
{ let Itinerary = ItinAGU_2_1; }

def __GetChIndr : ChInstr<(outs I32Reg:$d), (ins I32Reg:$s), "$d = CH[$s]",
                       0b10011, [(set I32Reg:$d, (int_mspu_getChIndr I32Reg:$s))]>
{ let Itinerary = ItinAGU_2_1; }

def __GetChNum : ChInstr<(outs I32Reg:$d), (ins i32imm:$s), "$d = CH[$s] (N)",
                       0b10100, [(set I32Reg:$d, (int_mspu_getChNum UImm5IntLeaf:$s))]>
{ let Itinerary = ItinAGU_2_1; }

def __GetChNumIndr : ChInstr<(outs I32Reg:$d), (ins I32Reg:$s), "$d = CH[$s] (N)",
                       0b10101, [(set I32Reg:$d, (int_mspu_getChNumIndr I32Reg:$s))]>
{ let Itinerary = ItinAGU_2_1; }



/****************************************************************************************
 *
 *          Instruction definitions only for binary tools.
 *
 ***************************************************************************************/




def SCUFlag : Operand<i32> {
  let PrintMethod = "printSCUFlag";
}

class SCUInst< dag outs, dag ins, string asm, bits<5> opcode >
	  	: MSPUInstr <0b0001, outs, ins, asm>
{
  let Inst{27-23} = opcode;
}

class BinaInst< dag outs, dag ins, string asm, bits<5> opcode, bit D >
	  	: SCUInst <outs, ins, asm, opcode>
{
  bits<3> f;
  bits<5> s1;
  bits<5> s0;
  bits<5> d;

  let Inst{27-23} = opcode;
  let Inst{21-20} = f{2-1};
  let Inst{19} = D;
  let Inst{18} = f{0};
  let Inst{14-10} = s1;
  let Inst{9-5} = s0;
  let Inst{4-0} = d;
}

class RInst< string opc, bits<5> opcode, bit D >
	  	: BinaInst < (outs RReg:$d), 
		  		  	 (ins RReg:$s0, RReg:$s1, SCUFlag:$f), 
		  		  	 !subst("OPC", opc, "$d = $s0 OPC $s1$f"), opcode, D >;

class DInst< string opc, bits<5> opcode, bit D >
	  	: BinaInst < (outs DReg:$d), 
		  		  	 (ins DReg:$s0, DReg:$s1, SCUFlag:$f), 
		  		  	 !subst("OPC", opc, "$d = $s0 OPC $s1 (D)$f"), opcode, D >;

class RDDInst< string opc, bits<5> opcode, bit D >
	  	: BinaInst < (outs RReg:$d), 
		  		  	 (ins DReg:$s0, DReg:$s1, SCUFlag:$f), 
		  		  	 !subst("OPC", opc, "$d = $s0 OPC $s1 (D)$f"), opcode, D >;

def AddR : RInst<"+", 0b00001, 0>;
def SubR : RInst<"-", 0b00010, 0>;

def AddcR : BinaInst < (outs RReg:$d), 
		  		  	 (ins RReg:$s0, RReg:$s1, SCUFlag:$f), 
		  		  	 "$d = $s0 + $s1$f (CI)", 0b10010, 0 >;

def SubcR : BinaInst < (outs RReg:$d), 
		  		  	 (ins RReg:$s0, RReg:$s1, SCUFlag:$f), 
		  		  	 "$d = $s0 - $s1$f (CI)", 0b10011, 0 >;

def MulR : RInst<"*", 0b00011, 0>;

def AddD : DInst<"+", 0b00001, 1>;
def SubD : DInst<"-", 0b00010, 1>;
def MulD : DInst<"*", 0b00011, 1>;

def And : RInst<"&", 0b10100, 0>;
def Or  : RInst<"|", 0b10101, 0>;
def XOr : RInst<"^", 0b10111, 0>;

def EQR : RInst<"==", 0b11000, 0>;
def NER : RInst<"!=", 0b11001, 0>;
def GTR : RInst<">" , 0b11010, 0>;
def GER : RInst<">=", 0b11011, 0>;
def LTR : RInst<"<",  0b11100, 0>;
def LER : RInst<"<=", 0b11101, 0>;

def EQRDD : RDDInst<"==", 0b11000, 1>;
def NERDD : RDDInst<"!=", 0b11001, 1>;
def GTRDD : RDDInst<">" , 0b11010, 1>;
def GERDD : RDDInst<">=", 0b11011, 1>;
def LTRDD : RDDInst<"<",  0b11100, 1>;
def LERDD : RDDInst<"<=", 0b11101, 1>;

class ShiftImmInst< string opc, bits<5> opcode >
	  	: BinaInst < (outs RReg:$d), (ins RReg:$s0, i32imm:$s1, SCUFlag:$f), 
		  		  	 !subst("OPC", opc, "$d = $s0 OPC $s1$f"), opcode, 0 >
{
  let Inst{22} = 1;
}

def SLImm : ShiftImmInst<"<<", 0b10000>;
def SRImm : ShiftImmInst<">>", 0b10001>;

class ShiftInst< string opc, bits<5> opcode >
	  	: BinaInst < (outs RReg:$d), (ins RReg:$s0, RReg:$s1, SCUFlag:$f), 
		  		  	 !subst("OPC", opc, "$d = $s0 OPC $s1$f"), opcode, 0 >
{
  let Inst{22} = 0;
}

def SL : ShiftInst<"<<", 0b10000>;
def SR : ShiftInst<">>", 0b10001>;

///////////////////////////////////////////////
// in unary operation, opcode designates output register type, while option
// designates its intput register type. The same way with binary operations.
class UnaInst< dag outs, dag ins, string asm, bits<5> opcode >
	  	: SCUInst < outs, ins, asm, opcode>
{
  bits<3> f;
  bits<5> s;
  bits<5> d;

  let Inst{27-23} = opcode;
  let Inst{21-20} = f{2-1};
  let Inst{18} = f{0};
  let Inst{9-5} = s;
  let Inst{4-0} = d;
}

class RRUnaInst< string opc, bits<5> opcode > :
	    UnaInst<(outs RReg:$d), (ins RReg:$s, SCUFlag:$f),
				!subst("OPC", opc, "$d = OPC $s$f"), opcode>
{
    let Inst{19} = 0;
}

class DDUnaInst< string opc, bits<5> opcode > :
	    UnaInst<(outs DReg:$d), (ins DReg:$s, SCUFlag:$f),
				!subst("OPC", opc, "$d = OPC $s (D)$f"), opcode>
{
    let Inst{19} = 1;
}

def Abs:RRUnaInst <"ABS", 0b01000>;
def Recip:RRUnaInst <"RECIP", 0b01001>;
def RSqRt:RRUnaInst <"RSQRT", 0b01010>;
def Not:RRUnaInst <"~", 0b10110>;

def AbsD:DDUnaInst <"ABS", 0b01000>;
def RecipD:DDUnaInst <"RECIP", 0b01001>;
def RSqRtD:DDUnaInst <"RSQRT", 0b01010>;

// DReg/RReg and 1 extra bit to designate DR.
// Note this is a little different from RDReg for printing.
def SCUExtraUnaReg : Operand<i32> { // output operand
  let DecoderMethod = "DecodeRDReg";
  let EncoderMethod = "getRDRegEncoding";
  let PrintMethod = "printSCUExtraUnaReg";
}

class ExtraUnaInst< dag outs, string opc, bits<5> opcode >
	  	: SCUInst <outs, (ins SCUExtraUnaReg:$s, SCUFlag:$f), 
		  		   !subst("OPC", opc, "$d = OPC $s$f"), opcode >
{
  bits<4> f;
  bits<6> s;
  bits<5> d;

  let Inst{27-23} = opcode;
  let Inst{21-20} = f{2-1};
  let Inst{19} = s{5};
  let Inst{18} = f{0};
  let Inst{9-5} = s{4-0};
  let Inst{4-0} = d;
}

class OutDUnaInst< string opc, bits<5> opcode >
	  	: ExtraUnaInst < (outs DReg:$d), opc, opcode >;

class OutRUnaInst< string opc, bits<5> opcode >
	  	: ExtraUnaInst < (outs RReg:$d), opc, opcode >;

def Sgl :OutRUnaInst <"Single", 0b00100>;
def Dbl :OutDUnaInst <"Double", 0b00101>;
def Int :OutRUnaInst <"Int", 0b00110>;
def UInt:OutRUnaInst <"UInt", 0b00111>;

//===----------------------------------------------------------------------===//
//						Instructions for SYN
//===----------------------------------------------------------------------===//
class SynInst< dag outs, dag ins, string asm, bits<5> opcode,
	  		   	list<dag> pattern = [], InstrItinClass itin = NoItinerary> 
	  	: MSPUInstr < 0b0100, outs, ins, asm, pattern > {
  let Inst{27-23} = opcode;
}

/************
def XYFlag: Operand<i32> {
  let PrintMethod =    "printXYFlag";
  let EncoderMethod =    "getXYFlagEncoding";
  let DecoderMethod = "DecodeXYFlag";
}
************/

def CallMXYFlag: Operand<i32> {
  let PrintMethod =    "printXYFlag";
//let EncoderMethod =    "getCallMXYFlagEncoding";
//let DecoderMethod = "DecodeCallMXYFlag";
}

def CallMFlag : Operand<i32> {
  let PrintMethod = "printCallMFlag";

/* we treat flags as imm, thus we do not need special handling.
   Tablegen helps us in such way. */
}

def CallMImm : SynInst< 
			 (outs), 
			 (ins UImm17Ptr:$i, CallMFlag:$f),
			 "CallM $i$f", 0b00001>
{
	bits<17> i;
	bit f;
	let Inst{22} = 1; // allow imm ext
	let Inst{21-5} = i;
	let Inst{0} = f;			 
}

def CallMJReg : SynInst< 
			 (outs), 
			 (ins PtrReg:$i, CallMFlag:$f),
			 "CallM $i$f", 0b00001>
{
	bits<5> i;
	bit f;
	let Inst{22} = 0;
	let Inst{9-5} = i;
	let Inst{0} = f;
}

/********************** K <-- R *********************/
// GPReg operand is to represent Rx/Jx/DRx registers that are used in ISA for
// scalar computation.
def KReg: Operand<i32> {
//let PrintMethod =    "printReg";
  let EncoderMethod =    "getKRegEncoding";
  let DecoderMethod = "DecodeKReg";
}

class WrtKInst< dag outs, dag ins, string asm> 
	  	: SynInst < outs, ins, asm, 0b00100>;

// KB KE KI KS, each has 16 children
def WrtK: WrtKInst <
	  		 (outs KReg:$k), 
			 (ins  RReg:$r),
			 "$k = $r">
{
	bits<7> k;
	bits<5> r;

	let Inst{11-7} = r;
	let Inst{6-0} = k;
}

// KM KG KL, each has 3 children
def KMFlag : Operand<i32> {
  // We treat flags as imm, thus we do not need special handling in encoding.
  // Tablegen helps us in such way.
  let PrintMethod = "printKMFlag";
}

def WrtKM : WrtKInst< 
			 (outs KMReg:$k), 
			 (ins  RReg:$r, KMFlag:$f),
			 "$k = $r$f">
{
	bits<2> k;
	bits<5> r;
	bits<2> f;

	let Inst{11-7} = r;
	let Inst{6-4} = 0b100;
	let Inst{3-2} = k;
	let Inst{1-0} = f;
}

def WrtCCtrl : WrtKInst< 
			 (outs),
			 (ins RReg:$r), 
			 "CCtrl = $r">
{
	bits<5> r;
	let Inst{11-7} = r;
	let Inst{6-4} = 0b111;
}

/******************** R <-- K ********************/
class ReadKInst< dag outs, dag ins, string asm> 
	  	: SynInst < outs, ins, asm, 0b00101>;

def ReadK : ReadKInst< 
			 (outs RReg:$r),
			 (ins  KReg:$k), 
			 "$r = $k">
{
	bits<7> k;
	bits<5> r;

	let Inst{11-7} = r;
	let Inst{6-0} = k;
}

def ReadKM : ReadKInst< 
			 (outs RReg:$r),
			 (ins  KMReg:$k, KMFlag:$f), 
			 "$r = $k$f">
{
	bits<2> k;
	bits<5> r;
	bits<2> f;

	let Inst{11-7} = r;
	let Inst{6-4} = 0b100;
	let Inst{3-2} = k;
	let Inst{1-0} = f;
}

def ReadCStat : ReadKInst< 
			 (outs  RReg:$r),
			 (ins), 
			 "$r = CStat">
{
	bits<5> r;
	let Inst{11-7} = r;
	let Inst{6-4} = 0b111;
}

def ReadLPCtr: SynInst < (outs  RReg:$r), (ins),
			   		   	 "$r = LPCTR", 0b01111>
{
	bits<5> r;
	let Inst{4-0} = r;
}

def WriteLPCtr: SynInst < (outs), (ins  RReg:$r),
			   		   	 "LPCTR = $r", 0b01110>
{
	bits<5> r;
	let Inst{11-7} = r;
}

/********************************* AGU *********************************/
class AGUInst< dag outs, dag ins, string asm, bits<5> opcode,
	  		   	list<dag> pattern = [], InstrItinClass itin = NoItinerary> 
	  	: MSPUInstr < 0b0011, outs, ins, asm, pattern > {
  let Inst{27-23} = opcode;
}

/*********** Channel access***********/
// JReg or RReg. for each regsiter, 1 extra bit encoding to tell whether it
// is a JReg or not.
def RJReg: Operand<i32> {
//let PrintMethod =    "printReg";
  let EncoderMethod =    "getRJRegEncoding";
  let DecoderMethod = "DecodeRJReg";
}

class IndirChInst< dag outs, dag ins, string asm, bits<5> opc >
	  	: AGUInst<outs, ins, asm, opc>
{
	  bits<6> d;
	  bits<6> s;
	  
	  let Inst{18}  = s{5};
	  let Inst{17}  = d{5};
	  let Inst{9-5} = s{4-0};
	  let Inst{4-0} = d{4-0};
}

def IndirWrtCh: IndirChInst < 
	  (outs RJReg:$d), (ins RJReg:$s),
	  "CH[ $d ] = $s", 0b10001>;

def IndirReadCh: IndirChInst < 
	  (outs RJReg:$d), (ins RJReg:$s),
	  "$d = CH[ $s ]", 0b10011>;

def IndirReadChNum: IndirChInst < 
	  (outs RJReg:$d), (ins RJReg:$s),
	  "$d = CH[ $s ] (N)", 0b10101>;

def DirWrtCh: AGUInst < 
	  (outs i32imm:$d), (ins RJReg:$s),
	  "CH[ $d ] = $s", 0b10000>
{
	  bits<5> d;
	  bits<6> s;
	  
	  let Inst{18}  = s{5};
	  let Inst{9-5} = s{4-0};
	  let Inst{4-0} = d;
}

def DirReadCh: AGUInst < 
	  (outs RJReg:$d), (ins i32imm:$s),
	  "$d = CH[ $s ]", 0b10010> {
	  bits<6> d;
	  bits<5> s;
	 
	  let Inst{17}  = d{5};
	  let Inst{9-5} = s;
	  let Inst{4-0} = d{4-0};
}

def DirReadChNum: AGUInst < 
	  (outs RJReg:$d), (ins i32imm:$s),
	  "$d = CH[ $s ] (N)", 0b10100> {
	  bits<6> d;
	  bits<5> s;
	 
	  let Inst{17}  = d{5};
	  let Inst{9-5} = s;
	  let Inst{4-0} = d{4-0};
}

/******************** SVR <---> R ********************/
// BHU: bit 22 - 20
def XferFlag : Operand<i32> {
  let PrintMethod = "printXferFlag";
}

def SVRWrt : SynInst< (outs SVRReg:$svr),
                       (ins   RReg:$ri, RReg:$rs, XferFlag:$f),
                              "$svr [ $ri ] = $rs$f",
                              0b10000> {
        bits<5> ri;
        bits<5> rs;
        bits<3> f;
        bits<1> svr;
        let Inst{22-20} = f;
        let Inst{18-14} = ri;
        let Inst{11-7}  = rs;
        let Inst{5}     = svr;
}

def SVRRead : SynInst< (outs RReg:$rd),
                         (ins  SVRReg:$svr,RReg:$ri, XferFlag:$f ),
                              "$rd  = $svr [ $ri ]$f",
                              0b10001>{
        bits<5> rd;
        bits<5> ri;
        bits<3> f;
        bits<1> svr;
        let Inst{22-20} = f;
        let Inst{11-7}  = ri;
        let Inst{4-0} = rd;
        let Inst{5}     = svr;
}

def SVRImmWrt : SynInst< (outs SVRReg:$svr),
                       (ins   i32imm:$imm, RReg:$rs, XferFlag:$f),
                              "$svr [ $imm ] = $rs$f",
                              0b10010> {
        bits<3> f;
        bits<6> imm;
        bits<5> rs;
        bits<1> svr;
        let Inst{22-20} = f;
        let Inst{19-14} = imm;
        let Inst{11-7}  = rs;
        let Inst{5}     = svr;
}

def SVRImmRead : SynInst< (outs RReg:$rd),
                            (ins  SVRReg:$svr,i32imm:$imm, XferFlag:$f ),
                              "$rd = $svr [ $imm ]$f",
                              0b10011>{
		bits<3> f;
        bits<5> rd;
        bits<6> imm;
        bits<1> svr;
        let Inst{22-20} = f;
        let Inst{19-14} = imm;
        let Inst{5}     = svr;
		let Inst{4-0}   = rd;
}

/******************** Config MC ********************/
def SetMC : SynInst< (outs MCReg:$mc), (ins  RReg:$rm),
                     "$mc = $rm", 0b01100>{
        bits<3> mc;
        bits<5> rm;

        let Inst{11-7}=rm;
        let Inst{2-0}=mc;
}

def GetMC : SynInst< (outs RReg:$rm), (ins MCReg:$mc),
                     "$rm  = $mc", 0b01101>
{
        bits<3> mc;
        bits<5> rm;

        let Inst{11-7}=rm;
        let Inst{2-0}=mc;
}

/******************** T <--> SVR <--> DR ********************/
def TPortFromSVR : SynInst< (outs SHUReg:$shu,TReg:$tj),
                           (ins  SVRReg:$svr),
                           "$shu.$tj = $svr",
                           0b01000>{
        bits<1> shu;
        bits<2> tj;
        bits<1> svr;

        let Inst{5}   = svr;
        let Inst{1-0} = tj;
        let Inst{2}   = shu;
}

def SVRFromTPort : SynInst< (outs SVRReg:$svr),
                           (ins  SHUReg:$shu,TReg:$tj),
                           "$svr = $shu.$tj",
                           0b01001>{
        bits<1> shu;
        bits<2> tj;
        bits<1> svr;

        let Inst{5}   = svr;
        let Inst{1-0} = tj;
        let Inst{2}   = shu;
}

/*******************************************/
def RDReg: Operand<i32> {
//let PrintMethod =    "printReg"; // use default.
  let EncoderMethod =    "getRDRegEncoding";
  let DecoderMethod = "DecodeRDReg";
}

class SHUTPortAccess <dag outs, dag ins, string asm, bits<5> opcode>
	    : SynInst<outs, ins, asm, opcode>
{
	bits<1> shu;
    bits<2> t;
    bits<6> r;
    bits<3> f;

	let Inst{22-20}  =f;
    let Inst{19}   = r{5};
	let Inst{2}   = shu;
	let Inst{1-0} = t;
    let Inst{11-7}   = r{4-0};
}

def SHUTPortWrt: SHUTPortAccess <(outs SHUReg:$shu, TReg:$t),
				  		   (ins RDReg:$r, XferFlag:$f),
			 		   	   "$shu.$t = Vect $r$f", 0b01010>;

def SHUTPortRead: SHUTPortAccess <(outs RDReg:$r),
				  					  (ins SHUReg:$shu, TReg:$t, XferFlag:$f),
			 		   				  "$r = $shu.$t$f", 0b01011>;

/****************** Normal load/store at address [J+J] **********/

def LdStOpc : Operand<i32> {
  let PrintMethod = "printLdStOpc";
}

// extra 2 bits to tell whether it's DR, R or J.
def LdStReg: Operand<i32> {
//let PrintMethod =    "printReg";
  let EncoderMethod =    "getLdStRegEncoding";
  let DecoderMethod = "DecodeLdStReg";
}

// Note: following patterns represent more than supported.
// [J + J]
class LdStInst<dag outs, dag ins, string asm, bits<5> opcode>
	  : AGUInst< outs, ins, asm, opcode>
{
	bits<7> d;
	bits<3> f;

	bits<5> s1;	
	bits<5> s0;
	bit opc;
	
	let Inst{21-19} = f;
	let Inst{18} = 1;
	let Inst{17-16} = d{6-5};
	let Inst{15} = opc;

	let Inst{14-10} = s1;
	let Inst{9-5} = s0;

	let Inst{4-0} = d{4-0};
}

def Ld : LdStInst<
   		  (outs LdStReg:$d),
		  (ins JReg:$s0, LdStOpc:$opc, JReg:$s1, XferFlag:$f),
		  "$d = [$s0 $opc $s1]$f", 0b11000>;

def St: LdStInst<
   		  (outs),
		  (ins JReg:$s0, LdStOpc:$opc, JReg:$s1, LdStReg:$d, XferFlag:$f),
		  "[$s0 $opc $s1] = $d$f", 0b11001>;

/****************** Normal load/store at address [J+Imm] **********/
class LdStImmInst<dag outs, dag ins, string asm, bit isStore>
	  : MSPUInstr < 0b0010, outs, ins, asm >
{
	bits<3> f;
	bits<7> d;
	bit opc;
	bits<11> imm;	
	bits<5> base;	
	
	let Inst{27} = isStore;
	let Inst{26-24} = f;
	let Inst{23} = d{5};
	let Inst{22} = d{6};
	let Inst{21} = opc;

	let Inst{20-10} = imm;
	let Inst{9-5} = base;

	let Inst{4-0} = d{4-0};
}

def LdImm: LdStImmInst<
   		  (outs LdStReg:$d),
		  (ins JReg:$base, LdStOpc:$opc, SImm11Ptr:$imm, XferFlag:$f),
		  "$d = [$base $opc $imm]$f", 0 >;

def StImm: LdStImmInst<
   		  (outs),
		  (ins JReg:$base, LdStOpc:$opc, SImm11Ptr:$imm, LdStReg:$d, XferFlag:$f),
		  "[$base $opc $imm] = $d$f", 1 >;

/****************** Atom load/store, only [J + J] *******************/
class LdStAtomInst<dag outs, dag ins, string asm,
	  				   bits<5> opcode>
	  : AGUInst< outs, ins, asm, opcode>
{
	bits<7> d;
	bit opc;
	bits<5> s1;	
	bits<5> s0;	
	
	let Inst{18} = 1;
	let Inst{17-16} = d{6-5};
	let Inst{15} = opc;

	let Inst{14-10} = s1;
	let Inst{9-5} = s0;

	let Inst{4-0} = d{4-0};
}

def LdAtom: LdStAtomInst<
   		  (outs LdStReg:$d),
		  (ins JReg:$s0, LdStOpc:$opc, JReg:$s1),
		  "$d = [$s0 $opc $s1] (AT)", 0b11010>;

def StAtom: LdStAtomInst<
   		  (outs),
		  (ins JReg:$s0, LdStOpc:$opc, JReg:$s1, LdStReg:$d),
		  "[$s0 $opc $s1] = $d (AT)", 0b11011>;

/****************** Vector Register load/store **********/
class VecLdStInst<dag outs, dag ins, string asm,
	  			  bits<5> opcode>
	  : AGUInst< outs, ins, asm, opcode>
{
	bit d;
	bit opc;

	bits<5> s1;	
	bits<5> s0;	
	
	let Inst{18} = 1;
	let Inst{15} = opc;

	let Inst{14-10} = s1;
	let Inst{9-5} = s0;

	let Inst{0} = d;
}

def VecLd: VecLdStInst<
   		  (outs SVRReg:$d),
		  (ins JReg:$s0, LdStOpc:$opc, JReg:$s1),
		  "$d = [$s0 $opc $s1]", 0b11100>;

def VecSt: LdStInst<
   		  (outs),
		  (ins JReg:$s0, LdStOpc:$opc, JReg:$s1, SVRReg:$d),
		  "[$s0 $opc $s1] = $d", 0b11101>;

/**************** Assign imm to J/R *********************/
// we can not determine whether to perform imm-extending (bit 22).
// thus we leave it to operand-encoding or fixup-applying,
// or even relocation.
class AssignImm11Inst< dag outs, dag ins, string asm, bit isJReg > :
				   AGUInst< outs, ins, asm, 0b00001 >
{
		bits<5> d;
		bits<11> s;

		let Inst{22} = 1; // allow imm extension
		let Inst{17} = isJReg;

		let Inst{15-5} = s;
		let Inst{4-0} = d;
}

def AssignRRegImm11 : AssignImm11Inst <
	  (outs RReg:$d), (ins SImm11Int:$s), "$d = $s", 0 >;

def AssignJRegImm11 : AssignImm11Inst <
	  (outs JReg:$d), (ins SImm11Ptr:$s), "$d = $s", 1 >;

/**************** extending imm *********************/
def ImmExt: MSPUInstr < 0b0101, 
	  (outs), (ins SImm28:$i), "Imm $i">
{
  bits<28> i;
  let Inst{27-0} = i;
}

def Xfer:  AGUInst< (outs RJReg:$d), (ins RJReg:$s), "$d = $s", 0b01000 >
{
  bits<6> d;
  bits<6> s;

  let Inst{18} = s{5};
  let Inst{17} = d{5};
  let Inst{9-5} = s{4-0};
  let Inst{4-0} = d{4-0};
}

def XferDD:  AGUInst< (outs DReg:$d), (ins DReg:$s), "$d = $s", 0b01001 >
{
  bits<5> d;
  bits<5> s;

  let Inst{9-5} = s;
  let Inst{4-0} = d;
}

def GetStat: AGUInst < (outs I32Reg:$d), (ins), "$d = STAT", 0b01010 >
{
		bits<5> d;
		let Inst{4-0} = d;
}
