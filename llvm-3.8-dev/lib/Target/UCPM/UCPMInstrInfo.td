//===-- UCPMInstInfo.td - Target Description for UCPM Target -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file describes the UCPM instructions in TableGen format.
//
//===----------------------------------------------------------------------===//

include "UCPMInstrFormats.td"

//===----------------------------------------------------------------------===//
//			Instruction NOP for all slots
//===----------------------------------------------------------------------===//

//from original
def NOP : UCPMInst <(ins), (outs), "NOP"> {
  let Inst = 0;
}

class UCPMRToDest<string header, bits<5> slotid, bits<4> mregid, Operand DestTy>
      : UCPAssignInstr<(ins MReg: $m), header, "$m", DestTy> {
  bits<7> m;
  let Inst{47-43} = slotid;
  let Inst{14-11} = mregid;
}

//MR0 read instructions
def MR0ToDestCom : UCPMRToDest<"R0.", 0b00001, 0b0110, MR0DestT> {
  let Inst{10-4} = m;
  let Inst{3-0} = d;
}
def MR0ToDestSI : UCPMRToDest<"R0.", 0b00001, 0b1110, MR0DestT> {
  let Inst{10-4} = m;
  let Inst{3-0} = d;
}
//MR1 read instructions
def MR1ToDestCom : UCPMRToDest<"R1.", 0b00010, 0b0110, MR1DestT> {
  let Inst{10-4} = m;
  let Inst{3-0} = d;
}
def MR1ToDestSI : UCPMRToDest<"R1.", 0b00010, 0b1110, MR1DestT> {
  let Inst{10-4} = m;
  let Inst{3-0} = d;
}
//MR2 read instructions
def MR2ToDestCom : UCPMRToDest<"R2.", 0b00011, 0b0110, MR2DestT> {
  let Inst{10-4} = m;
  let Inst{3-0} = d;
}
def MR2ToDestSI : UCPMRToDest<"R2.", 0b00011, 0b1110, MR2DestT> {
  let Inst{10-4} = m;
  let Inst{3-0} = d;
}
//MR3 read instructions
def MR3ToDestCom : UCPMRToDest<"R3.", 0b00100, 0b0110, MR3DestT> {
  let Inst{10-4} = m;
  let Inst{3-0} = d;
}
def MR3ToDestSI : UCPMRToDest<"R3.", 0b00100, 0b1110, MR3DestT> {
  let Inst{10-4} = m;
  let Inst{3-0} = d;
}
//MR4 read instructions
def MR4ToDestCom : UCPMRToDest<"R4.", 0b00101, 0b0110, MR4DestT> {
  let Inst{10-4} = m;
  let Inst{3-0} = d;
}
def MR4ToDestSI : UCPMRToDest<"R4.", 0b00101, 0b1110, MR4DestT> {
  let Inst{10-4} = m;
  let Inst{3-0} = d;
}
//MR5 read instructions
def MR5ToDestCom : UCPMRToDest<"R5.", 0b00110, 0b0110, MR5DestT> {
  let Inst{10-4} = m;
  let Inst{3-0} = d;
}
def MR5ToDestSI : UCPMRToDest<"R5.", 0b00110, 0b1110, MR5DestT> {
  let Inst{10-4} = m;
  let Inst{3-0} = d;
}

// NOP
def MReg0NOP :  UCPMInst <(ins), (outs), "NOP"> {
  let Inst{47-43} = 0b00001;
  let Inst{14-11} = 0b1101;
  let Inst{3-0} = 0;
}

def MReg1NOP :  UCPMInst <(ins), (outs), "NOP"> {
  let Inst{47-43} = 0b00010;
  let Inst{12-11} = 0b10;
  let Inst{3-0} = 0b1001;
}
def MReg2NOP :  UCPMInst <(ins), (outs), "NOP"> {
  let Inst{47-43} = 0b00011;
  let Inst{13-11} = 0b010;
  let Inst{3-0} = 0b1001;
}
def MReg3NOP :  UCPMInst <(ins), (outs), "NOP"> {
  let Inst{47-43} = 0b00100;
  let Inst{12-11} = 0b10;
  let Inst{3-0} = 0b1001;
}
def MReg4NOP :  UCPMInst <(ins), (outs), "NOP"> {
  let Inst{47-43} = 0b00101;
  let Inst{13-11} = 0b010;
  let Inst{3-0} = 0b1001;
}
def MReg5NOP :  UCPMInst <(ins), (outs), "NOP"> {
  let Inst{47-43} = 0b00110;
  let Inst{12-11} = 0b10;
  let Inst{3-0} = 0b1001;
}

// start SHU Instruction -------------------------------------------------
//3 kinds of base class for SHUIndToDest
//Tn:T0-T3, "TB+=" and "TB=+"
class UCPSHUIndToDest_SetTB<string header, bit TBSymbol, bits<5> slot, Operand DestTy>
        : UCPSHUIndexAssignInstr<(ins TPort: $tm, TPort: $tn, TEPort: $tk, imm32i: $imm5),
                      header, !strconcat("{$tm,$tn} IND $tk",
                                         !if(TBSymbol, "(TB=+$imm5)", "(TB+=$imm5)")),
                      DestTy> {
  bits<2> tm;
  bits<2> tn;
  bits<4> tk;
  bits<5> imm5;
  let Inst{47-43} = slot;
  let Inst{22-19} = tk;
  let Inst{18-17} = tn;
  let Inst{16} = 0;
  let Inst{15} = TBSymbol;
  let Inst{14-10} = imm5;
  let Inst{9-8} = tm;
}
//Tn:T0-T3, no "TB+=" or "TB=+"
class UCPSHUIndToDest_noImm<string header, bit TBSymbol, bits<5> slot, Operand DestTy>
        : UCPSHUIndexAssignInstr<(ins TPort: $tm, TPort: $tn, TEPort: $tk),
                      header, "{$tm,$tn} IND $tk", DestTy> {
  bits<2> tm;
  bits<2> tn;
  bits<4> tk;
  
  let Inst{47-43} = slot;
  let Inst{22-19} = tk;
  let Inst{18-17} = tn;
  let Inst{16} = 1;
  let Inst{15} = TBSymbol;
  let Inst{14} = 0;
  let Inst{9-8} = tm;
}
// Tn:TSQ/TBB/...
class UCPSHUIndToDest_constTn<string header, bit TBSymbol, bits<5> slot, Operand DestTy>
        : UCPSHUIndexAssignInstr<(ins TPort: $tm, TEPort: $tn, TEPort: $tk), header,
                      "{$tm,$tn} IND $tk", DestTy> {
  bits<2> tm;
  bits<4> tn;
  bits<4> tk;
  let Inst{47-43} = slot;
  let Inst{22-19} = tk;
  let Inst{16} = 1;
  let Inst{15} = TBSymbol;
  let Inst{14} = 1;
  let Inst{13-10} = tn;
  let Inst{9-8} = tm;
}


// TBSymbol=0:tb,TBSymbol=1:tk
// TO MACC
def SHU0Ind_SetTBToMACC_0 : UCPSHUIndToDest_SetTB<"SHU0.", 0, 0b00111, SHU0DestTI> {
  let Inst{7-6} = 0b01;
  let Inst{5-0} = d;
}
def SHU0Ind_noImmToMACC_0 : UCPSHUIndToDest_noImm<"SHU0.", 0, 0b00111, SHU0DestTI> {
  let Inst{7-6} = 0b01;
  let Inst{5-0} = d;
}
def SHU0Ind_constTnToMACC_0 : UCPSHUIndToDest_constTn<"SHU0.", 0, 0b00111, SHU0DestTI> {
  let Inst{7-6} = 0b01;
  let Inst{5-0} = d;
}
def SHU0Ind_SetTBToMACC_1 : UCPSHUIndToDest_SetTB<"SHU0.", 1, 0b00111, SHU0DestTI> {
  let Inst{7-6} = 0b01;
  let Inst{5-0} = d;
}
def SHU0Ind_noImmToMACC_1 : UCPSHUIndToDest_noImm<"SHU0.", 1, 0b00111, SHU0DestTI> {
  let Inst{7-6} = 0b01;
  let Inst{5-0} = d;
}
def SHU0Ind_constTnToMACC_1 : UCPSHUIndToDest_constTn<"SHU0.", 1, 0b00111, SHU0DestTI> {
  let Inst{7-6} = 0b01;
  let Inst{5-0} = d;
}

def SHU1Ind_SetTBToMACC_0 : UCPSHUIndToDest_SetTB<"SHU1.", 0, 0b01000, SHU1DestTI> {
  let Inst{7-6} = 0b01;
  let Inst{5-0} = d;
}
def SHU1Ind_noImmToMACC_0 : UCPSHUIndToDest_noImm<"SHU1.", 0, 0b01000, SHU1DestTI> {
  let Inst{7-6} = 0b01;
  let Inst{5-0} = d;
}
def SHU1Ind_constTnToMACC_0 : UCPSHUIndToDest_constTn<"SHU1.", 0, 0b01000, SHU1DestTI> {
  let Inst{7-6} = 0b01;
  let Inst{5-0} = d;
}
def SHU1Ind_SetTBToMACC_1 : UCPSHUIndToDest_SetTB<"SHU1.", 1, 0b01000, SHU1DestTI> {
  let Inst{7-6} = 0b01;
  let Inst{5-0} = d;
}
def SHU1Ind_noImmToMACC_1 : UCPSHUIndToDest_noImm<"SHU1.", 1, 0b01000, SHU1DestTI> {
  let Inst{7-6} = 0b01;
  let Inst{5-0} = d;
}
def SHU1Ind_constTnToMACC_1 : UCPSHUIndToDest_constTn<"SHU1.", 1, 0b01000, SHU1DestTI> {
  let Inst{7-6} = 0b01;
  let Inst{5-0} = d;
}

def SHU2Ind_SetTBToMACC_0 : UCPSHUIndToDest_SetTB<"SHU2.", 0, 0b01001, SHU2DestTI> {
  let Inst{7-6} = 0b01;
  let Inst{5-0} = d;
}
def SHU2Ind_noImmToMACC_0 : UCPSHUIndToDest_noImm<"SHU2.", 0, 0b01001, SHU2DestTI> {
  let Inst{7-6} = 0b01;
  let Inst{5-0} = d;
}
def SHU2Ind_constTnToMACC_0 : UCPSHUIndToDest_constTn<"SHU2.", 0, 0b01001, SHU2DestTI> {
  let Inst{7-6} = 0b01;
  let Inst{5-0} = d;
}
def SHU2Ind_SetTBToMACC_1 : UCPSHUIndToDest_SetTB<"SHU2.", 1, 0b01001, SHU2DestTI> {
  let Inst{7-6} = 0b01;
  let Inst{5-0} = d;
}
def SHU2Ind_noImmToMACC_1 : UCPSHUIndToDest_noImm<"SHU2.", 1, 0b01001, SHU2DestTI> {
  let Inst{7-6} = 0b01;
  let Inst{5-0} = d;
}
def SHU2Ind_constTnToMACC_1 : UCPSHUIndToDest_constTn<"SHU2.", 1, 0b01001, SHU2DestTI> {
  let Inst{7-6} = 0b01;
  let Inst{5-0} = d;
}
// TO BIU
def SHU0Ind_SetTBToBIU_0 : UCPSHUIndToDest_SetTB<"SHU0.", 0, 0b00111, SHU0DestTI> {
  let Inst{7-6} = 0b00;
  let Inst{5-0} = d;
}
def SHU0Ind_noImmToBIU_0 : UCPSHUIndToDest_noImm<"SHU0.", 0, 0b00111, SHU0DestTI> {
  let Inst{7-6} = 0b00;
  let Inst{5-0} = d;
}
def SHU0Ind_constTnToBIU_0 : UCPSHUIndToDest_constTn<"SHU0.", 0, 0b00111, SHU0DestTI> {
  let Inst{7-6} = 0b00;
  let Inst{5-0} = d;
}
def SHU0Ind_SetTBToBIU_1 : UCPSHUIndToDest_SetTB<"SHU0.", 1, 0b00111, SHU0DestTI> {
  let Inst{7-6} = 0b00;
  let Inst{5-0} = d;
}
def SHU0Ind_noImmToBIU_1 : UCPSHUIndToDest_noImm<"SHU0.", 1, 0b00111, SHU0DestTI> {
  let Inst{7-6} = 0b00;
  let Inst{5-0} = d;
}
def SHU0Ind_constTnToBIU_1 : UCPSHUIndToDest_constTn<"SHU0.", 1, 0b00111, SHU0DestTI> {
  let Inst{7-6} = 0b00;
  let Inst{5-0} = d;
}

def SHU1Ind_SetTBToBIU_0 : UCPSHUIndToDest_SetTB<"SHU1.", 0, 0b01000, SHU1DestTI> {
  let Inst{7-6} = 0b00;
  let Inst{5-0} = d;
}
def SHU1Ind_noImmToBIU_0 : UCPSHUIndToDest_noImm<"SHU1.", 0, 0b01000, SHU1DestTI> {
  let Inst{7-6} = 0b00;
  let Inst{5-0} = d;
}
def SHU1Ind_constTnToBIU_0 : UCPSHUIndToDest_constTn<"SHU1.", 0, 0b01000, SHU1DestTI> {
  let Inst{7-6} = 0b00;
  let Inst{5-0} = d;
}
def SHU1Ind_SetTBToBIU_1 : UCPSHUIndToDest_SetTB<"SHU1.", 1, 0b01000, SHU1DestTI> {
  let Inst{7-6} = 0b00;
  let Inst{5-0} = d;
}
def SHU1Ind_noImmToBIU_1 : UCPSHUIndToDest_noImm<"SHU1.", 1, 0b01000, SHU1DestTI> {
  let Inst{7-6} = 0b00;
  let Inst{5-0} = d;
}
def SHU1Ind_constTnToBIU_1 : UCPSHUIndToDest_constTn<"SHU1.", 1, 0b01000, SHU1DestTI> {
  let Inst{7-6} = 0b00;
  let Inst{5-0} = d;
}

def SHU2Ind_SetTBToBIU_0 : UCPSHUIndToDest_SetTB<"SHU2.", 0, 0b01001, SHU2DestTI> {
  let Inst{7-6} = 0b00;
  let Inst{5-0} = d;
}
def SHU2Ind_noImmToBIU_0 : UCPSHUIndToDest_noImm<"SHU2.", 0, 0b01001, SHU2DestTI> {
  let Inst{7-6} = 0b00;
  let Inst{5-0} = d;
}
def SHU2Ind_constTnToBIU_0 : UCPSHUIndToDest_constTn<"SHU2.", 0, 0b01001, SHU2DestTI> {
  let Inst{7-6} = 0b00;
  let Inst{5-0} = d;
}
def SHU2Ind_SetTBToBIU_1 : UCPSHUIndToDest_SetTB<"SHU2.", 1, 0b01001, SHU2DestTI> {
  let Inst{7-6} = 0b00;
  let Inst{5-0} = d;
}
def SHU2Ind_noImmToBIU_1 : UCPSHUIndToDest_noImm<"SHU2.", 1, 0b01001, SHU2DestTI> {
  let Inst{7-6} = 0b00;
  let Inst{5-0} = d;
}
def SHU2Ind_constTnToBIU_1 : UCPSHUIndToDest_constTn<"SHU2.", 1, 0b01001, SHU2DestTI> {
  let Inst{7-6} = 0b00;
  let Inst{5-0} = d;
}
// TO SHU
def SHU0Ind_SetTBToSHU_0 : UCPSHUIndToDest_SetTB<"SHU0.", 0, 0b00111, SHU0DestTI> {
  let Inst{7-6} = 0b00;
  let Inst{5-0} = d;
}
def SHU0Ind_noImmToSHU_0 : UCPSHUIndToDest_noImm<"SHU0.", 0, 0b00111, SHU0DestTI> {
  let Inst{7-6} = 0b00;
  let Inst{5-0} = d;
}
def SHU0Ind_constTnToSHU_0 : UCPSHUIndToDest_constTn<"SHU0.", 0, 0b00111, SHU0DestTI> {
  let Inst{7-6} = 0b00;
  let Inst{5-0} = d;
}
def SHU0Ind_SetTBToSHU_1 : UCPSHUIndToDest_SetTB<"SHU0.", 1, 0b00111, SHU0DestTI> {
  let Inst{7-6} = 0b00;
  let Inst{5-0} = d;
}
def SHU0Ind_noImmToSHU_1 : UCPSHUIndToDest_noImm<"SHU0.", 1, 0b00111, SHU0DestTI> {
  let Inst{7-6} = 0b00;
  let Inst{5-0} = d;
}
def SHU0Ind_constTnToSHU_1 : UCPSHUIndToDest_constTn<"SHU0.", 1, 0b00111, SHU0DestTI> {
  let Inst{7-6} = 0b00;
  let Inst{5-0} = d;
}

def SHU1Ind_SetTBToSHU_0 : UCPSHUIndToDest_SetTB<"SHU1.", 0, 0b01000, SHU1DestTI> {
  let Inst{7-6} = 0b00;
  let Inst{5-0} = d;
}
def SHU1Ind_noImmToSHU_0 : UCPSHUIndToDest_noImm<"SHU1.", 0, 0b01000, SHU1DestTI> {
  let Inst{7-6} = 0b00;
  let Inst{5-0} = d;
}
def SHU1Ind_constTnToSHU_0 : UCPSHUIndToDest_constTn<"SHU1.", 0, 0b01000, SHU1DestTI> {
  let Inst{7-6} = 0b00;
  let Inst{5-0} = d;
}
def SHU1Ind_SetTBToSHU_1 : UCPSHUIndToDest_SetTB<"SHU1.", 1, 0b01000, SHU1DestTI> {
  let Inst{7-6} = 0b00;
  let Inst{5-0} = d;
}
def SHU1Ind_noImmToSHU_1 : UCPSHUIndToDest_noImm<"SHU1.", 1, 0b01000, SHU1DestTI> {
  let Inst{7-6} = 0b00;
  let Inst{5-0} = d;
}
def SHU1Ind_constTnToSHU_1 : UCPSHUIndToDest_constTn<"SHU1.", 1, 0b01000, SHU1DestTI> {
  let Inst{7-6} = 0b00;
  let Inst{5-0} = d;
}

def SHU2Ind_SetTBToSHU_0 : UCPSHUIndToDest_SetTB<"SHU2.", 0, 0b01001, SHU2DestTI> {
  let Inst{7-6} = 0b00;
  let Inst{5-0} = d;
}
def SHU2Ind_noImmToSHU_0 : UCPSHUIndToDest_noImm<"SHU2.", 0, 0b01001, SHU2DestTI> {
  let Inst{7-6} = 0b00;
  let Inst{5-0} = d;
}
def SHU2Ind_constTnToSHU_0 : UCPSHUIndToDest_constTn<"SHU2.", 0, 0b01001, SHU2DestTI> {
  let Inst{7-6} = 0b00;
  let Inst{5-0} = d;
}
def SHU2Ind_SetTBToSHU_1 : UCPSHUIndToDest_SetTB<"SHU2.", 1, 0b01001, SHU2DestTI> {
  let Inst{7-6} = 0b00;
  let Inst{5-0} = d;
}
def SHU2Ind_noImmToSHU_1 : UCPSHUIndToDest_noImm<"SHU2.", 1, 0b01001, SHU2DestTI> {
  let Inst{7-6} = 0b00;
  let Inst{5-0} = d;
}
def SHU2Ind_constTnToSHU_1 : UCPSHUIndToDest_constTn<"SHU2.", 1, 0b01001, SHU2DestTI> {
  let Inst{7-6} = 0b00;
  let Inst{5-0} = d;
}

// NOP
def SHU0NOP :  UCPMInst <(ins), (outs), "NOP"> {
  let Inst{47-43} = 0b00111;
  let Inst{22-19} = 0b1111;
  let Inst{18-16} = 0b110;
  let Inst{14-12} = 0b000;
}
def SHU1NOP :  UCPMInst <(ins), (outs), "NOP"> {
  let Inst{47-43} = 0b01000;
  let Inst{22-19} = 0b1111;
  let Inst{18-16} = 0b110;
  let Inst{14-12} = 0b000;
}
def SHU2NOP :  UCPMInst <(ins), (outs), "NOP"> {
  let Inst{47-43} = 0b01001;
  let Inst{22-19} = 0b1111;
  let Inst{18-16} = 0b110;
  let Inst{14-12} = 0b000;
}

// byte-bit bit-byte
class SHUByteBit<string header, bits<5> slot, bits<2> dest, Operand DestTy>
        : UCPSHUIndexAssignInstr<(ins SHUTranOprtor: $op, TPort: $tm, Flags: $f), header, "$tm$f", DestTy>{

    bits<32> f;
    bits<2> tm;
    bits<8> op;
    
    let Inst{47-43} = slot;
    let Inst{22-15} = op{7-0};
    let Inst{12-10} = f{2-0};
    let Inst{9-8} = tm;
    let Inst{7-6} = dest;
    let Inst{5-0} = d{5-0};
}

def SHU0ByteBitToBIU : SHUByteBit<"SHU0.", 0b00111, 0b00, SHU0DestTI >;
def SHU0ByteBitToMACC: SHUByteBit<"SHU0.", 0b00111 ,0b01, SHU0DestTI >;
def SHU0ByteBitToSHU: SHUByteBit<"SHU0.", 0b00111 ,0b00, SHU0DestTI >;

def SHU1ByteBitToBIU : SHUByteBit<"SHU1.", 0b01000, 0b00, SHU1DestTI >;
def SHU1ByteBitToMACC: SHUByteBit<"SHU1.", 0b01000 ,0b01, SHU1DestTI >;
def SHU1ByteBitToSHU : SHUByteBit<"SHU1.", 0b01000, 0b00, SHU1DestTI >;

def SHU2ByteBitToBIU : SHUByteBit<"SHU2.", 0b01001, 0b00, SHU2DestTI >;
def SHU2ByteBitToMACC: SHUByteBit<"SHU2.", 0b01001 ,0b01, SHU2DestTI >;
def SHU2ByteBitToSHU : SHUByteBit<"SHU2.", 0b01001, 0b00, SHU2DestTI >;

// WAIT 
class SHUWaitInstr<string header, bits<5> slot>
        : CommonAssignInstr<(ins imm32i: $imm7),(outs),
                                   header, "", "", "Wait $imm7">{
  bits<7>  imm7;
  
  let Inst{47-43} = slot;
  let Inst{22-15} = 0b11111101;
  let Inst{14-8} = imm7;
}
def SHU0Wait : SHUWaitInstr<"SHU0.", 0b00111>;
def SHU1Wait : SHUWaitInstr<"SHU1.", 0b01000>;
def SHU2Wait : SHUWaitInstr<"SHU2.", 0b01001>;

// SetCond  
class SHUSetCondInstr<string header, bits<5> slot>
        : UCPMInst<(ins imm32i: $imm8, imm32i: $m), (outs), !strconcat("$imm8 -> ", header,"SetCond[$m]")> 
                                   {
  bits<8>  imm8;
  bit m;
  
  let Inst{47-43} = slot;
  let Inst{22-15} = 0b11111100;
  let Inst{12-10} = 0b100;
  let Inst{8} = m;
  let Inst{7-0} = imm8;
}
def SHU0SetCond : SHUSetCondInstr<"SHU0.", 0b00111>;
def SHU1SetCond : SHUSetCondInstr<"SHU1.", 0b01000>;
def SHU2SetCond : SHUSetCondInstr<"SHU2.", 0b01001>;
// ducx end SHU Instruction -------------------------------------------------




// ducx start IALU Instruction --------------------------------------------------------------------
class IALUBaseCode {
  field bits<48> Inst;
  let Inst{47-43} = 0b01010;//?
}

class IALUCode<bits<2> dest> : IALUBaseCode {
  bits<8> op;
  bits<2> tm;
  bits<2> tn;
  bits<32> f;
  bits<4>  d;

  let Inst{11-10} = tm;
  let Inst{9-8} = tn;
  let Inst{7} = 0;
  let Inst{6-5}  = dest;
  let Inst{4-3}  = d{3-2};
  let Inst{2} = 0;
  let Inst{1-0}   = d{1-0};
}

// MERGE
class IALUTPCode<bits<2> dest> : IALUCode<dest>  {
  bits<2> tp;

  let Inst{21-18} = op{3-0};
  let Inst{16-15} = f{4-3};
  let Inst{14} = tp{1};
  let Inst{13} = 1;
  let Inst{12} = tp{0};
}
multiclass IALUTPToDest {
  def ToSHU  : TParaInstr<"", 1, "", IALUTPOprtor, SHUT  >, IALUTPCode<0b00>;
  def ToBIU  : TParaInstr<"", 1, "", IALUTPOprtor, BIUO  >, IALUTPCode<0b10>;
  def ToMACC : TParaInstr<"", 1, "", IALUTPOprtor, MACCT >, IALUTPCode<0b01>;
}

// ADD SUB
class IALUASCode<bits<2> dest> : IALUCode<dest> {
  let Inst{21-18} = op{3-0};
  let Inst{17-12} = f{5-0};
}
multiclass IALUASToDest {
  let isCodeGenOnly = 1 in {
  def ToSHU  : BinaryInstr<"", 1, IALUASOprtor, SHUT  >, IALUASCode<0b00>;
  def ToBIU  : BinaryInstr<"", 1, IALUASOprtor, BIUO  >, IALUASCode<0b10>;
  def ToMACC : BinaryInstr<"", 1, IALUASOprtor, MACCT >, IALUASCode<0b01>;
  }
}

// AND OR XOR
class IALULogicCode<bits<2> dest> : IALUCode<dest>{
  let Inst{21-18} = 0b1001;
  let Inst{14}    = 0;
  let Inst{13-12} = op{1-0};
}
multiclass IALULogicToDest {
  let isCodeGenOnly = 1 in {
  def ToSHU  : BinaryInstr<"", 1, IALULogicOprtor, SHUT  >, IALULogicCode<0b00>;
  def ToBIU  : BinaryInstr<"", 1, IALULogicOprtor, BIUO  >, IALULogicCode<0b10>;
  def ToMACC : BinaryInstr<"", 1, IALULogicOprtor, MACCT >, IALULogicCode<0b01>;
  }
}

// EQU NEQ LT NST ST NLT / MAX MIN
class IALUComCode<bits<2> dest> : IALUCode<dest>{
  let Inst{21-18} = op{5-2};
  let Inst{17-15} = f{5-3};
  let Inst{14-13} = op{1-0};
  let Inst{12}    = f{0};
}
multiclass IALUComToDest {
  let isCodeGenOnly = 1 in {
  def ComToSHU  : BinaryInstr<"", 1, IALUComOprtor, SHUT  >, IALUComCode<0b00>;
  def ComToBIU  : BinaryInstr<"", 1, IALUComOprtor, BIUO  >, IALUComCode<0b10>;
  def ComToMACC : BinaryInstr<"", 1, IALUComOprtor, MACCT >, IALUComCode<0b01>;
  }
  let isCodeGenOnly = 1 in {
  def MNToSHU  : BinaryInstr<"", 0, IALUMNOprtor, SHUT  >, IALUComCode<0b00>;
  def MNToBIU  : BinaryInstr<"", 0, IALUMNOprtor, BIUO  >, IALUComCode<0b10>;
  def MNToMACC : BinaryInstr<"", 0, IALUMNOprtor, MACCT >, IALUComCode<0b01>;
  }
}

// LSH RSH
class IALUShiftCode<bits<2> dest> : IALUCode<dest>{
  let Inst{21-18} = 0b0101;
  let Inst{17-15} = f{5-3};
  let Inst{13} = op{0};
}
multiclass IALUShiftToDest {
  let isCodeGenOnly = 1 in {
  def ToSHU  : BinaryInstr<"", 1, IALUShiftOprtor, SHUT  >, IALUShiftCode<0b00>;
  def ToBIU  : BinaryInstr<"", 1, IALUShiftOprtor, BIUO  >, IALUShiftCode<0b10>;
  def ToMACC : BinaryInstr<"", 1, IALUShiftOprtor, MACCT >, IALUShiftCode<0b01>;
  }
}

// SUBABS CPRS
class IALUSCCode<bits<2> dest> : IALUCode<dest>{
  let Inst{21-18} = op{4-1};
  let Inst{17-14} = f{5-2};
  let Inst{13} = op{0};
  let Inst{12}    = f{0};
}
multiclass IALUSCToDest {
  let isCodeGenOnly = 1 in {
  def ToSHU  : BinaryInstr<"", 0, IALUSCOprtor, SHUT  >, IALUSCCode<0b00>;
  def ToBIU  : BinaryInstr<"", 0, IALUSCOprtor, BIUO  >, IALUSCCode<0b10>;
  def ToMACC : BinaryInstr<"", 0, IALUSCOprtor, MACCT >, IALUSCCode<0b01>;
  }
}

class IALUUryCode<bits<2> dest> : IALUCode<dest> {
  let tn = 0b00;
}
// RADD RMAX RMIN EXPD	
class IALUUryRMNCode<bits<2> dest> : IALUUryCode<dest>{
  let Inst{21-18} = op{5-2};
  let Inst{17-15} = f{5-3};
  let Inst{14-13} = op{1-0};
}
multiclass IALUUryRMNToDest {
  let isCodeGenOnly = 1 in {
  def ToSHU  : UnaryInstr<"", IALUUryRMNOprtor, SHUT  >, IALUUryRMNCode<0b00>;
  def ToBIU  : UnaryInstr<"", IALUUryRMNOprtor, BIUO  >, IALUUryRMNCode<0b10>;
  def ToMACC : UnaryInstr<"", IALUUryRMNOprtor, MACCT >, IALUUryRMNCode<0b01>;
  }
}

// NOT
class IALUUryNOTCode<bits<2> dest> : IALUUryCode<dest>{
  let Inst{21-18} = 0b1001;
  let Inst{14}    = 0;
  let Inst{13-12} = op{1-0};
}
multiclass IALUUryNOTToDest {
  let isCodeGenOnly = 1 in {
  def ToSHU  : UnaryInstr<"", IALUUryNOTOprtor, SHUT  >, IALUUryNOTCode<0b00>;
  def ToBIU  : UnaryInstr<"", IALUUryNOTOprtor, BIUO  >, IALUUryNOTCode<0b10>;
  def ToMACC : UnaryInstr<"", IALUUryNOTOprtor, MACCT >, IALUUryNOTCode<0b01>;
  }
}

// ABS CONJ MINUS
class IALUUryCONCode<bits<2> dest> : IALUUryCode<dest>{
  let Inst{21-18} = op{4-1};
  let Inst{17-14} = f{5-2};
  let Inst{13} = op{0};
  let Inst{12} = f{0};
}
multiclass IALUUryCONToDest {
  let isCodeGenOnly = 1 in { //???
  def ToSHU  : UnaryInstr<"", IALUUryCONOprtor, SHUT  >, IALUUryCONCode<0b00>;
  def ToBIU  : UnaryInstr<"", IALUUryCONOprtor, BIUO  >, IALUUryCONCode<0b10>;
  def ToMACC : UnaryInstr<"", IALUUryCONOprtor, MACCT >, IALUUryCONCode<0b01>;
  }
}

// LSHimm RSHimm
class IALUImmInstr<Operand DestTy, bits<2> dest>
        : AssignInstr<(ins IALUImmOprtor: $op, TPort: $tm, imm32i: $imm5, Flags: $f),
                      "", "$tm $op $imm5$f", DestTy>, IALUBaseCode {
  bits<2>  tm;
  bits<5>  imm5;
  bits<3> f;
  bits<8> op;
  let Inst{21-18} = op{3-0};
  let Inst{17-15} = f{2-0};
  let Inst{14-12} = imm5{4-2};
  let Inst{11-10} = tm;
  let Inst{9-8} = imm5{1-0};
  let Inst{7} = 0;
  let Inst{6-5}  = dest;
  let Inst{4-3}  = d{3-2};
  let Inst{2} = 0;
  let Inst{1-0}   = d{1-0};
}
multiclass IALUImmToDest { 
  def ToSHU  : IALUImmInstr<SHUT  , 0b00>;
  def ToBIU  : IALUImmInstr<BIUO  , 0b10>;
  def ToMACC : IALUImmInstr<MACCT, 0b01>;
}

// DIV ReadQ ReadR
class IALUReadQRCode<bits<2> dest> : IALUCode<dest>{
  let Inst{21-18} = op{4-1};
  let Inst{17-14} = f{5-2};
  let Inst{13} = op{0};
  let Inst{12} = f{0};
}
multiclass IALUReadQRToDest {
  def ToSHU  : BinaryInstr<"", 0, DIVOprtor, SHUT  >, IALUReadQRCode<0b00>;
  def ToBIU  : BinaryInstr<"", 0, DIVOprtor, BIUO  >, IALUReadQRCode<0b10>;
  def ToMACC : BinaryInstr<"", 0, DIVOprtor, MACCT >, IALUReadQRCode<0b01>;
}
 
// DIV START CONT
def IALUDivSC : CommonAssignInstr<(ins DIVOprtor: $op, TPort: $tm, TPort: $tn, Flags: $f),
                                 (outs),
                                 "", "", "", "$op($tm, $tn)$f">,
                IALUBaseCode {
  bits<8> op;
  bits<2> tm;
  bits<2> tn;
  bits<3> f;
  let Inst{21-18} = op{4-1};
  let Inst{17-15} = f{2-0};
  let Inst{14} = 0;
  let Inst{13} = op{0};
  let Inst{11-10} = tm;
  let Inst{9-8} = tn;
  let Inst{7-0} = 0;
}

// MOV
class IALUMOVCode<Operand DestTy, bits<2> dest> 
            : AssignInstr<(ins TPort: $tm), "", "IALU.$tm", DestTy>, IALUBaseCode {
  bits<2> tm;
 
  let Inst{21-18} = 0b1011;
  let Inst{14-13} =0b01;
  let Inst{11-10} = tm;
  let Inst{7} = 0;
  let Inst{6-5}  = dest;
  let Inst{4-3}  = d{3-2};
  let Inst{2} = 0;
  let Inst{1-0}   = d{1-0};
}
multiclass IALUMOVToDest { 
  def ToSHU  : IALUMOVCode<SHUT  , 0b00>;
  def ToBIU  : IALUMOVCode<BIUO  , 0b10>;
  def ToMACC : IALUMOVCode<MACCT, 0b01>;
}

// WAIT
def IALUWait : CommonAssignInstr<(ins imm32i: $imm7),(outs),
                                   "", "", "", "Wait $imm7">,
                 IALUBaseCode {
  bits<7>  imm7;
  
  let Inst{21-18} = 0b1111;
  let Inst{16-15} = imm7{6-5};
  let Inst{14-13} = 0b01;
  let Inst{12-8} = imm7{4-0};
  let Inst{7-0}  = 0;
}

// NOP
def IALUNOP : CommonAssignInstr<(ins),(outs),"", "", "", "NOP">, IALUBaseCode {
  let Inst{21-18} = 0b1111;
  let Inst{17} = 0;
  let Inst{14-13} = 0b11;
}

// SetCond
def IALUSetCond : CommonAssignInstr<(ins imm32i: $imm8, Flags: $m), (outs),
                                   "", "IALU.SetCond[$m]", "->", "$imm8">,
                 IALUBaseCode {
  bits<8>  imm8;
  bit m;
  
  let Inst{21-18} = 0b1111;
  let Inst{17} = 1;
  let Inst{15} = m;
  let Inst{14-13} = 0b11;
  let Inst{7-0} = imm8;
}

// SendFlag
class IALUSendFlagCode<Operand DestTy, bits<2> dest> 
            : AssignInstr<(ins Flags: $f),
                      "", "IALU.FLAG$f", DestTy>, IALUBaseCode {
  bits<4> f;
 
  let Inst{21-18} = 0b1000;
  let Inst{13} = 0;
  let Inst{11-8} = f;
  let Inst{7} = 0;
  let Inst{6-5}  = dest;
  let Inst{4-3}  = d{3-2};
  let Inst{2} = 0;
  let Inst{1-0}   = d{1-0};
}
multiclass IALUSendFlagToDest { 
  def ToSHU  : IALUSendFlagCode<SHUT  , 0b00>;
  def ToBIU  : IALUSendFlagCode<BIUO  , 0b10>;
  def ToMACC : IALUSendFlagCode<MACCT, 0b01>;
}

// WriteFlag 
def IALUWriteFlag : CommonAssignInstr<(ins TPort: $tm, Flags: $all, Flags: $f),(outs), "", "WriteFlag$f", "->", "IALU.$tm">,
                 IALUBaseCode { 
  bits<2>  tm;
  bit all;
  bits<2> f;
  
  let Inst{21-18} = 0b1111;
  let Inst{14-13} = 0b10;
  let Inst{12} = all;
  let Inst{11-10} = tm;
  let Inst{9-8} = f;//???
  let Inst{7-0}  = 0;
}

defm IALUTP  : IALUTPToDest;
defm IALUAS :      IALUASToDest;
defm IALULogic : IALULogicToDest;
defm IALU :      IALUComToDest;
defm IALUShift : IALUShiftToDest;
defm IALUSC : IALUSCToDest;
defm IALUUryRMN:   IALUUryRMNToDest;
defm IALUUryNOT:   IALUUryNOTToDest;
defm IALUUryCON:   IALUUryCONToDest;
defm IALUImm : IALUImmToDest;
defm IALUReadQR : IALUReadQRToDest;
defm IALUMOVE:   IALUMOVToDest;
defm IALUSendFlag: IALUSendFlagToDest;
// ducx end IALU Instruction --------------------------------------------------------------------------


// ducx start IFALU Instruction --------------------------------------------------------------------------
class IFALUBaseCode {
  field bits<48> Inst;
  let Inst{47-43} = 0b01100;//?
}

class IFALUCode<bits<2> dest> : IFALUBaseCode {
  bits<8> op;
  bits<2> tm;
  bits<2> tn;
  bits<4>  d;
  bits<32> f;
  
  let Inst{22} = 1;
  let Inst{11-10} = tm;
  let Inst{9-8} = tn;
  let Inst{7} = 0;
  let Inst{6-5}  = dest;
  let Inst{4-3}  = d{3-2};
  let Inst{2} = 0;
  let Inst{1-0}   = d{1-0};
}

// EQU NEQ LT NST ST NLT / MAX MIN
class IFALUComCode<bits<2> dest> : IFALUCode<dest>{
  let Inst{21-18} = op{5-2};
  let Inst{14-13} = op{1-0};
}
multiclass IFALUComToDest {
  def ComToSHU  : BinaryInstr<"", 1, IFALUComOprtor, SHUT  >, IFALUComCode<0b00>;
  def ComToBIU  : BinaryInstr<"", 1, IFALUComOprtor, BIUO  >, IFALUComCode<0b10>;
  def ComToMACC : BinaryInstr<"", 1, IFALUComOprtor, MACCT >, IFALUComCode<0b01>;

  let isCodeGenOnly = 1 in {
  def MNToSHU  : BinaryInstr<"", 0, IFALUMNOprtor, SHUT  >, IFALUComCode<0b00>;
  def MNToBIU  : BinaryInstr<"", 0, IFALUMNOprtor, BIUO  >, IFALUComCode<0b10>;
  def MNToMACC : BinaryInstr<"", 0, IFALUMNOprtor, MACCT >, IFALUComCode<0b01>;
  }
}

class IFALUUryCode<bits<2> dest> : IFALUCode<dest> {
  let tn = 0b00;
}

// Transfer
class IFALUUryTranCode<bits<2> dest> : IFALUUryCode<dest>{
  let Inst{21-18} = op{4-1};
  let Inst{17} = f{1};
  let Inst{14} = f{0};
  let Inst{13} = op{0};
}
multiclass IFALUUryTranToDest {
  let isCodeGenOnly = 1 in {
  def ToSHU  : UnaryInstr<"", IFALUUryOprtor, SHUT  >, IFALUUryTranCode<0b00>;
  def ToBIU  : UnaryInstr<"", IFALUUryOprtor, BIUO  >, IFALUUryTranCode<0b10>;
  def ToMACC : UnaryInstr<"", IFALUUryOprtor, MACCT >, IFALUUryTranCode<0b01>;
  }
}

// NOP
def IFALUNOP : CommonAssignInstr<(ins),(outs),"", "", "", "NOP">, IFALUBaseCode {
  let Inst{21-18} = 0b1111;
  let Inst{17} = 0;
  let Inst{14-13} = 0b11;
}

defm IFALU : IFALUComToDest;
defm IFALUUryTran : IFALUUryTranToDest;

// ducx end IFALU Instruction --------------------------------------------------------------------------


// ducx start IMAC Instruction --------------------------------------------------------------------------
class IMACBaseCode {
  field bits<48> Inst;
  let Inst{47-43} = 0b01011;//?
}

class IMACCode<bits<2> dest> : IMACBaseCode {
  bits<8> op;
  bits<2> tm;
  bits<2> tn;
  bits<2> tp;
  bits<4>  d;
  bits<32> f;
  bit ff;
  bits<2> shift;
  bits<2> opc;
  
  let Inst{26-25} = opc;
  let Inst{23-21} = f{6-4};
  let Inst{19} = ff;
  let Inst{18-17} = f{3-2};
  let Inst{16-15} = tm;
  let Inst{14-13} = tn;
  let Inst{12-11} = tp;
  let Inst{10} = f{0};
  let Inst{9-8} = shift;
  let Inst{7} = 0;
  let Inst{6-5}  = dest;
  let Inst{4-3}  = d{3-2};
  let Inst{2} = 0;
  let Inst{1-0}   = d{1-0};
}
// Real
class IMACRealCode<bits<2> dest> : IMACCode<dest> {
  let Inst{20} = 0;
}
// Complex
class IMACCompCode<bits<2> dest> : IMACCode<dest> {
  let Inst{20} = 1;
}

// for Mul op
class IMACRealopCode<bits<2> dest> : IMACRealCode<dest> {
  let opc = op{1-0};
}
class IMACCompopCode<bits<2> dest> : IMACCompCode<dest> {
  let opc = op{1-0};
}

// tm*tn -> d
multiclass IMulRealToDest{
    def ToSHU  : IMACBinaryInstr<"", 1, IMACMulOprtor, SHUT >, IMACRealopCode<0b00>;
    def ToBIU  : IMACBinaryInstr<"", 1, IMACMulOprtor, BIUO >, IMACRealopCode<0b10>;
    def ToMACC : IMACBinaryInstr<"", 1, IMACMulOprtor, MACCT>, IMACRealopCode<0b01>;
}
multiclass IMulCompToDest{
    def ToSHU  : IMACBinaryInstr<"", 1, IMACMulOprtor, SHUT >, IMACCompopCode<0b00>;
    def ToBIU  : IMACBinaryInstr<"", 1, IMACMulOprtor, BIUO >, IMACCompopCode<0b10>;
    def ToMACC : IMACBinaryInstr<"", 1, IMACMulOprtor, MACCT>, IMACCompopCode<0b01>;
}

// MR <- tp +|- tm*tn
class MAInstr<bit addsub>
        : ToMRwoMovInstr<(ins TPort: $tm, TPort: $tn, TPort: $tp, Flags: $f, Flags: $ff, Flags: $shift),
                            "<-", !if(addsub, "$tp + $tm * $tn$ff$f", "$tp - $tm * $tn$ff$f")>{
    bits<2> tm;
    bits<2> tn;
    bits<2> tp;    
    bits<32> f;
    bits<2> shift;
    bit ff;
    
    let Inst{24} = addsub;
}
def IMA_Realadd : MAInstr<0>, IMACRealCode<0b00>{
    let opc = 0b01;
    let d = 0;
}
def IMA_Realsub : MAInstr<1>, IMACRealCode<0b00>{
    let opc = 0b01;
    let d = 0;
}
def IMA_Compadd : MAInstr<0>, IMACCompCode<0b00>{
    let opc = 0b01;
    let d = 0;
}
def IMA_Compsub : MAInstr<1>, IMACCompCode<0b00>{
    let opc = 0b01;
    let d = 0;
}

// tp +|- tm*tn -> d
class MAMovInstr<bit addsub, Operand DestTy>
        : ToMRwMovInstr<(ins TPort: $tm, TPort: $tn, TPort: $tp, Flags: $f , Flags: $ff, Flags: $shift), 
                "<-", !if(addsub, "$tp + $tm * $tn$ff$f", "$tp - $tm * $tn$ff$f"), DestTy>{
    bits<2> tm;
    bits<2> tn;
    bits<2> tp;    
    bits<32> f;
    bits<2> shift;
    bit ff;
  
    let Inst{24} = addsub;                          
}
multiclass IMARealaddToDest{
    let opc = 0b01 in{
    def ToSHU  : MAMovInstr<0,SHUT  >, IMACRealCode<0b00>;
    def ToBIU  : MAMovInstr<0,BIUO  >, IMACRealCode<0b10>;
    def ToMACC : MAMovInstr<0,MACCT >, IMACRealCode<0b01>;
    }
}
multiclass IMARealsubToDest{
    let opc = 0b01 in{
    def ToSHU  : MAMovInstr<1,SHUT  >, IMACRealCode<0b00>;
    def ToBIU  : MAMovInstr<1,BIUO  >, IMACRealCode<0b10>;
    def ToMACC : MAMovInstr<1,MACCT >, IMACRealCode<0b01>;
    }
}
multiclass IMACompaddToDest{
    let opc = 0b01 in{
    def ToSHU  : MAMovInstr<0,SHUT  >, IMACCompCode<0b00>;
    def ToBIU  : MAMovInstr<0,BIUO  >, IMACCompCode<0b10>;
    def ToMACC : MAMovInstr<0,MACCT >, IMACCompCode<0b01>;
    }
}
multiclass IMACompsubToDest{
    let opc = 0b01 in{
    def ToSHU  : MAMovInstr<1,SHUT  >, IMACCompCode<0b00>;
    def ToBIU  : MAMovInstr<1,BIUO  >, IMACCompCode<0b10>;
    def ToMACC : MAMovInstr<1,MACCT >, IMACCompCode<0b01>;
    }
}

// MAFFT: MR <- tp +- tm*tn
class MAFFTInstr
        : ToMRwoMovInstr<(ins TPort: $tm, TPort: $tn, TPort: $tp, Flags: $f , Flags: $ff, Flags: $shift),
                            "<-", "$tp +- $tm * $tn$ff$f">{
    bits<2> tm;
    bits<2> tn;
    bits<2> tp;    
    bits<32> f;
    bits<2> shift;
    bit ff;
    
}
def IMAFFT_Real : MAFFTInstr, IMACRealCode<0b00>{
    let opc = 0b11;
    let d = 0;
}
def IMAFFT_Comp : MAFFTInstr, IMACCompCode<0b00>{
    let opc = 0b11;
    let d = 0;
}

// MAFFT: tp +- tm*tn -> d
class MAFFTMovInstr<Operand DestTy>
        : ToMRwMovInstr<(ins TPort: $tm, TPort: $tn, TPort: $tp, Flags: $f, Flags: $ff, Flags: $shift), 
                "<-", "$tp +- $tm * $tn$ff$f", DestTy>{
    bits<2> tm;
    bits<2> tn;
    bits<2> tp;    
    bits<32> f;
    bits<2> shift;
    bit ff;
                           
}
multiclass IMAFFTRealToDest{
    let opc = 0b11 in{
    def ToSHU  : MAFFTMovInstr<SHUT  >, IMACRealCode<0b00>;
    def ToBIU  : MAFFTMovInstr<BIUO  >, IMACRealCode<0b10>;
    def ToMACC : MAFFTMovInstr<MACCT >, IMACRealCode<0b01>;
    }
}
multiclass IMAFFTCompToDest{
    let opc = 0b11 in{
    def ToSHU  : MAFFTMovInstr<SHUT  >, IMACCompCode<0b00>;
    def ToBIU  : MAFFTMovInstr<BIUO  >, IMACCompCode<0b10>;
    def ToMACC : MAFFTMovInstr<MACCT >, IMACCompCode<0b01>;
    }
}

// MR +=|-= tm*tn
class MaCInstr<bit addsub>
        : ToMRwoMovInstr<(ins TPort: $tm, TPort: $tn, Flags: $f, Flags: $ff, Flags: $shift),
                            !if(addsub, "+=", "-="), "$tm * $tn$ff$f">{
    bits<2> tm;
    bits<2> tn;
    bits<32> f;
    bits<2> shift;
    bit ff;
     
    let Inst{24} = addsub;
    let Inst{12} = 1;
    let Inst{11} = f{1};
}
def IMaC_Realadd : MaCInstr<0>, IMACRealCode<0b00>{
    let opc = 0b10;
    let d = 0;
    let tp = 0;
}
def IMaC_Realsub : MaCInstr<1>, IMACRealCode<0b00>{
    let opc = 0b10;
    let d = 0;
    let tp = 0;
}
def IMaC_Compadd : MaCInstr<0>, IMACCompCode<0b00>{
    let opc = 0b10;
    let d = 0;
    let tp = 0;
}
def IMaC_Compsub : MaCInstr<1>, IMACCompCode<0b00>{
    let opc = 0b10;
    let d = 0;
    let tp = 0;
}

// MR +=|-= tm*tn -> d
class MaCMovInstr<bit addsub, Operand DestTy>
        : ToMRwMovInstr<(ins TPort: $tm, TPort: $tn, Flags: $f , Flags: $ff, Flags: $shift),
                          !if(addsub, "+=", "-="), "$tm * $tn$ff$f", DestTy>{
    bits<2> tm;
    bits<2> tn;
    bits<32> f;
    bits<2> shift;
    bit ff;
    
    let Inst{24} = addsub;
    let Inst{12} = 1;
    let Inst{11} = f{1};
}
multiclass IMaCRealaddToDest{
    let opc = 0b10, tp = 0b00 in{
    def ToSHU  : MaCMovInstr<0,SHUT  >, IMACRealCode<0b00>;
    def ToBIU  : MaCMovInstr<0,BIUO  >, IMACRealCode<0b10>;
    def ToMACC : MaCMovInstr<0,MACCT >, IMACRealCode<0b01>;
    }
}
multiclass IMaCRealsubToDest{
    let opc = 0b10, tp = 0b00in{
    def ToSHU  : MaCMovInstr<1,SHUT  >, IMACRealCode<0b00>;
    def ToBIU  : MaCMovInstr<1,BIUO  >, IMACRealCode<0b10>;
    def ToMACC : MaCMovInstr<1,MACCT >, IMACRealCode<0b01>;
    }
}
multiclass IMaCCompaddToDest{
    let opc = 0b10, tp = 0b00 in{
    def ToSHU  : MaCMovInstr<0,SHUT  >, IMACCompCode<0b00>;
    def ToBIU  : MaCMovInstr<0,BIUO  >, IMACCompCode<0b10>;
    def ToMACC : MaCMovInstr<0,MACCT >, IMACCompCode<0b01>;
    }
}
multiclass IMaCCompsubToDest{
    let opc = 0b10, tp = 0b00 in{
    def ToSHU  : MaCMovInstr<1,SHUT  >, IMACCompCode<0b00>;
    def ToBIU  : MaCMovInstr<1,BIUO  >, IMACCompCode<0b10>;
    def ToMACC : MaCMovInstr<1,MACCT >, IMACCompCode<0b01>;
    }
}

// MR +=|-= tm
class AccInstr<bit addsub>
        : ToMRwoMovInstr<(ins TPort: $tm, Flags: $f, Flags: $ff, Flags: $shift),
                            !if(addsub, "+=", "-="), "$tm$ff$f">{
    bits<2> tm;
    bits<32> f;
    bits<2> shift;
    bit ff;
      
    let Inst{24} = addsub;
    let Inst{12} = 0;
    let Inst{11} = f{1};
}
def IAcc_Realadd : AccInstr<0>, IMACRealCode<0b00>{
    let opc = 0b10;
    let d = 0;
    let tn = 0;
    let tp = 0;
}
def IAcc_Realsub : AccInstr<1>, IMACRealCode<0b00>{
    let opc = 0b10;
    let d = 0;
    let tn = 0;
    let tp = 0;
}
def IAcc_Compadd : AccInstr<0>, IMACCompCode<0b00>{
    let opc = 0b10;
    let d = 0;
    let tn = 0;
    let tp = 0;
}
def IAcc_Compsub : AccInstr<1>, IMACCompCode<0b00>{
    let opc = 0b10;
    let d = 0;
    let tn = 0;
    let tp = 0;
}

// MR +=|-= tm -> d 
class AccMovInstr<bit addsub, Operand DestTy>
        : ToMRwMovInstr<(ins TPort: $tm, Flags: $f , Flags: $ff,Flags: $shift),
                          !if(addsub, "+=", "-="), "$tm$ff$f", DestTy>{
    bits<2> tm;
    bits<2> tn;
    bits<32> f;
    bits<2> shift;
    bit ff;
    
    let Inst{24} = addsub;
    let Inst{12} = 0;
    let Inst{11} = f{1};
}
multiclass IAccRealaddToDest{
    let opc = 0b10, tn =0b00, tp = 0b00 in{
    def ToSHU  : AccMovInstr<0,SHUT  >, IMACRealCode<0b00>;
    def ToBIU  : AccMovInstr<0,BIUO  >, IMACRealCode<0b10>;
    def ToMACC : AccMovInstr<0,MACCT >, IMACRealCode<0b01>;
    }
}
multiclass IAccRealsubToDest{
    let opc = 0b10, tn =0b00, tp = 0b00 in{
    def ToSHU  : AccMovInstr<1,SHUT  >, IMACRealCode<0b00>;
    def ToBIU  : AccMovInstr<1,BIUO  >, IMACRealCode<0b10>;
    def ToMACC : AccMovInstr<1,MACCT >, IMACRealCode<0b01>;
    }
}
multiclass IAccCompaddToDest{
    let opc = 0b10, tn =0b00, tp = 0b00 in{
    def ToSHU  : AccMovInstr<0,SHUT  >, IMACCompCode<0b00>;
    def ToBIU  : AccMovInstr<0,BIUO  >, IMACCompCode<0b10>;
    def ToMACC : AccMovInstr<0,MACCT >, IMACCompCode<0b01>;
    }
}
multiclass IAccCompsubToDest{
    let opc = 0b10, tn =0b00, tp = 0b00 in{
    def ToSHU  : AccMovInstr<1,SHUT  >, IMACCompCode<0b00>;
    def ToBIU  : AccMovInstr<1,BIUO  >, IMACCompCode<0b10>;
    def ToMACC : AccMovInstr<1,MACCT >, IMACCompCode<0b01>;
    }
}

// MOV
class IMACMOVCode<Operand DestTy, bits<2> dest> 
            : AssignInstr<(ins TPort: $tm, Flags: $f), "", "IMAC.$tm$f", DestTy>, IMACBaseCode {
  bits<2> tm;
  bit f;

  let Inst{26-25} = 0b10;
  let Inst{16-15} = tm;
  let Inst{14-13} = 0b10;
  let Inst{12} = 0;
  let Inst{11} = f;
  let Inst{7} = 0;
  let Inst{6-5}  = dest;
  let Inst{4-3}  = d{3-2};
  let Inst{2} = 0;
  let Inst{1-0}   = d{1-0};
  
}
multiclass IMACMOVToDest { 
  def ToSHU  : IMACMOVCode<SHUT  , 0b00>;
  def ToBIU  : IMACMOVCode<BIUO  , 0b10>;
  def ToMACC : IMACMOVCode<MACCT, 0b01>;
}

// SendFlag
class IMACSendFlagCode<Operand DestTy, bits<2> dest> 
            : AssignInstr<(ins Flags: $f),
                      "", "IMAC.FLAG$f", DestTy>, IMACBaseCode {
  bits<3> f;
 
  let Inst{26-25} = 0b10;
  let Inst{24} = 0;
  let Inst{23-22} = f{2-1};
  let Inst{18} = 0;
  let Inst{17} = f{0};
  let Inst{14-13} = 0b01;
  let Inst{12-11} = 0b00;
  let Inst{7} = 0;
  let Inst{6-5}  = dest;
  let Inst{4-3}  = d{3-2};
  let Inst{2} = 0;
  let Inst{1-0}   = d{1-0};
}
multiclass IMACSendFlagToDest { 
  def ToSHU  : IMACSendFlagCode<SHUT  , 0b00>;
  def ToBIU  : IMACSendFlagCode<BIUO  , 0b10>;
  def ToMACC : IMACSendFlagCode<MACCT, 0b01>;
}

// WriteFlag 
def IMACWriteFlag : CommonAssignInstr<(ins TPort: $tm, Flags: $f),(outs), "", "FLAG$f", "->", "IMAC.$tm">,
                 IMACBaseCode { 
  bits<2>  tm;
  bits<3> f;
 
  let Inst{26-25} = 0b10;
  let Inst{24} = 1;
  let Inst{23-22} = f{2-1};
  let Inst{18} = 0;
  let Inst{17} = f{0};
  let Inst{16-15} = tm;
  let Inst{14-13} = 0b01;
  let Inst{12-11} = 0b00;
}

// WAIT
def IMACWait : CommonAssignInstr<(ins imm32i: $imm7),(outs),
                                   "", "", "", "Wait $imm7">,
                 IMACBaseCode {
  bits<7>  imm7;
  
  let Inst{26-23} = 0b1011;
  let Inst{21-15} = imm7;
  let Inst{14-13} = 0b11;
  let Inst{12-11} = 0b00;
  let Inst{7-0}  = 0;
}

// NOP
def IMACNOP : CommonAssignInstr<(ins),(outs),"", "", "", "NOP">, IMACBaseCode {
  let Inst{26-25} = 0b10;
  let Inst{23} = 0;
  let Inst{14-13} = 0b11;
  let Inst{12-11} = 0b00;
}

// SetCond
def IMACSetCond : CommonAssignInstr<(ins imm32i: $imm8, Flags: $m), (outs),
                                   "", "IMAC.SetCond[$m]", "->", "$imm8">,
                 IMACBaseCode {
  bits<8>  imm8;
  bit m;
  
  let Inst{26-25} = 0b10;
  let Inst{24} = m;
  let Inst{23} = 1;
  let Inst{14-13} = 0b11;
  let Inst{12-11} = 0b00;
  let Inst{7-0} = imm8;
}

defm IMulReal : IMulRealToDest;
defm IMulComp : IMulCompToDest;

defm IMARealadd : IMARealaddToDest;
defm IMARealsub : IMARealsubToDest;
defm IMACompadd : IMACompaddToDest;
defm IMACompsub : IMACompsubToDest;

defm IMAFFTReal : IMAFFTRealToDest;
defm IMAFFTComp : IMAFFTCompToDest;

defm IMaCRealadd : IMaCRealaddToDest;
defm IMaCRealsub : IMaCRealsubToDest;
defm IMaCCompadd : IMaCCompaddToDest;
defm IMaCCompsub : IMaCCompsubToDest;

defm IAccRealadd : IAccRealaddToDest;
defm IAccRealsub : IAccRealsubToDest;
defm IAccCompadd : IAccCompaddToDest;
defm IAccCompsub : IAccCompsubToDest;

defm IMACMOV : IMACMOVToDest;
defm IMACSendFlag: IMACSendFlagToDest;
// ducx end IMAC Instruction --------------------------------------------------------------------------


// ducx start IFMAC Instruction --------------------------------------------------------------------------
class IFMACBaseCode {
  field bits<48> Inst;
  let Inst{47-43} = 0b01101;//?
}

class IFMACCode<bits<2> dest> : IFMACBaseCode {
  bits<8> op;
  bits<2> tm;
  bits<2> tn;
  bits<4>  d;
  bits<32> f;
  
  let Inst{26-23} = op{3-0};
  let Inst{22} = f{0};
  let Inst{20-19} = tn;
  let Inst{16-15} = tm;
  let Inst{14-11} = 0b0101;
  let Inst{7} = 0;
  let Inst{6-5}  = dest;
  let Inst{4-3}  = d{3-2};
  let Inst{2} = 0;
  let Inst{1-0}   = d{1-0};
}

multiclass IFMACToDest{
    def ToSHU  : BinaryInstr<"", 1, IFMACMulOprtor, SHUT >, IFMACCode<0b00>;
    def ToBIU  : BinaryInstr<"", 1, IFMACMulOprtor, BIUO >, IFMACCode<0b10>;
    def ToMACC : BinaryInstr<"", 1, IFMACMulOprtor, MACCT>, IFMACCode<0b01>;
}

// NOP
def IFMACNOP : CommonAssignInstr<(ins),(outs),"", "", "", "NOP">, IFMACBaseCode {
  let Inst{26-25} = 0b10;
  let Inst{23} = 0;
  let Inst{14-13} = 0b11;
  let Inst{12-11} = 0b00;
}

defm IFMAC : IFMACToDest;
// ducx end IFMAC Instruction --------------------------------------------------------------------------



// ducx start BIU Instruction --------------------------------------------------------------------------

// load: discrete data
class BIULdToDest_dis<string header, bits<5> slot, bits<2> dest, Operand DestTy>
        : BIUAssignInstr<(ins TPort: $tm, TPort: $ts, Flags: $f, Flags: $QLH), header, "$tm($ts)$f", DestTy>{

    bits<32> f;
    bits<2> tm;
    bits<2> ts;
    bits<2> QLH;
    
    let Inst{47-43} = slot;
    let Inst{20-18} = 0b111;
    let Inst{17-16} = f{2-1};
    let Inst{15-14} = QLH;
    let Inst{13-12} = tm;
    let Inst{11-10} = ts;
    let Inst{9} = f{0};
    let Inst{8-7} = dest;
    let Inst{6-5}  = d{5-4};
    let Inst{4} = 0;
    let Inst{3-0}   = d{3-0};
}

// multiclass BIULdInstr_dis<string header, bits<5> slot>{
//     def ToSHU  : BIULdToDest_dis<header, slot ,0b00, SHUT >;
//     def ToMACC : BIULdToDest_dis<header, slot ,0b01, MACCT>;
// }

// ducx 20160822: biu0 test
def BIU0disLdToSHU : BIULdToDest_dis<"BIU0.", 0b01110, 0b00, BIU0DestTE >;
def BIU0disLdToMACC: BIULdToDest_dis<"BIU0.", 0b01110 ,0b01, BIU0DestTE >;
// ducx 20160829: biu1 test
def BIU1disLdToSHU : BIULdToDest_dis<"BIU1.", 0b01111, 0b00, BIU1DestTE >;
def BIU1disLdToMACC: BIULdToDest_dis<"BIU1.", 0b01111 ,0b01, BIU1DestTE >;
// ducx 20160830: biu2 test
def BIU2disLdToSHU : BIULdToDest_dis<"BIU2.", 0b10000, 0b00, BIU2DestTE >;
def BIU2disLdToMACC: BIULdToDest_dis<"BIU2.", 0b10000 ,0b01, BIU2DestTE >;



// load: common/step
class BIULdToDest_com<string header, bits<5> slot, bits<2> dest, Operand DestTy>
        : BIUAssignInstr<(ins TPort: $tm, Flags: $f), header, "$tm$f", DestTy>{

    bits<32> f;
    bits<2> tm;
  
    let Inst{47-43} = slot;
    let Inst{20-18} = 0b111;
    let Inst{17-16} = f{2-1};
    let Inst{15-14} = 0b11;
    let Inst{13-12} = tm;
    let Inst{11-10} = 0b00;
    let Inst{9} = f{0};
    let Inst{8-7} = dest;
    let Inst{6-5}  = d{5-4};
    let Inst{4} = 0;
    let Inst{3-0}   = d{3-0};
}

// multiclass BIULdInstr_com<string header, bits<5> slot>{
//     def ToSHU  : BIULdToDest_com<header, slot ,0b00, SHUT >;
//     def ToMACC : BIULdToDest_com<header, slot ,0b01, MACCT>;
// }

// ducx 20160822: biu0 test
def BIU0ComLdToSHU : BIULdToDest_com<"BIU0.", 0b01110, 0b00, BIU0DestTE >;
def BIU0ComLdToMACC: BIULdToDest_com<"BIU0.", 0b01110 ,0b01, BIU0DestTE >;
// ducx 20160829: biu1 test
def BIU1ComLdToSHU : BIULdToDest_com<"BIU1.", 0b01111, 0b00, BIU1DestTE >;
def BIU1ComLdToMACC: BIULdToDest_com<"BIU1.", 0b01111 ,0b01, BIU1DestTE >;
// ducx 20160830: biu2 test
def BIU2ComLdToSHU : BIULdToDest_com<"BIU2.", 0b10000, 0b00, BIU2DestTE >;
def BIU2ComLdToMACC: BIULdToDest_com<"BIU2.", 0b10000 ,0b01, BIU2DestTE >;



// store
class BIUStInstr<string header, bits<5> slot>
        : UCPMInst<(ins TPort: $tm, TPort: $ts, Flags: $f, Flags: $SQLH), (outs), !strconcat(header,"$tm($ts)", "-> DM$f")>{
            
    bits<32> f;
    bits<2> tm;
    bits<2> ts;
    bits<2> SQLH;

    let Inst{47-43} = slot;
    let Inst{20-18} = 0b100;
    let Inst{17-16} = f{2-1};
    let Inst{15-14} = SQLH;
    let Inst{13-12} = tm;
    let Inst{11-10} = ts;
    let Inst{9} = f{0};
    let Inst{8-6} = 0b100;
    let Inst{5-0}   = 0;
}
 
// ADD SUB AND OR
class BIUBinInstr<string header, bits<5> slot>
        : UCPMInst<(ins BIUBinOprtor: $op, TPort: $td, TPort: $ts0, TPort: $ts1, imm32i: $d, imm32i: $s0, imm32i: $s1), (outs), !strconcat(header,"$ts0[$s0] $op $ts1[$s1]", "-> $td[d]")>{
            
    bits<2> td;
    bits<2> ts0;
    bits<2> ts1;
    bits<8> op;
    bits<4> d;
    bits<4> s0;
    bits<4> s1;

    let Inst{47-43} = slot;
    let Inst{20-18} = op{2-0};
    let Inst{17-16} = td;
    let Inst{15-12} = d;
    let Inst{11-10} = ts0;
    let Inst{9-6} = s0;
    let Inst{5-4} = ts1;
    let Inst{3-0} = s1;
}
 
// LSH RSH
class BIUShiftInstr<string header, bits<5> slot>
        : UCPMInst<(ins BIUShiftOprtor: $op, TPort: $td, TPort: $ts, imm32i: $imm, imm32i: $d, imm32i: $s), (outs), !strconcat(header,"$ts[$s] $op $imm", "-> $td[$d]")>{
            
    bits<2> ts;
    bits<2> td;
    bits<8> op;
    bits<2> imm;
    bits<4> s;
    bits<4> d;

    let Inst{47-43} = slot;
    let Inst{20-18} = op{6-4};
    let Inst{17-16} = td;
    let Inst{15-12} = d;
    let Inst{11-10} = imm;
    let Inst{9-6} = op{3-0};
    let Inst{5-4} = ts;
    let Inst{3-0} = s;
}

// ==  !=
class BIUComInstr<string header, bits<5> slot>
        : UCPMInst<(ins BIUComOprtor: $op, TPort: $td, TPort: $ts, imm32i: $d, imm32i: $s), (outs), !strconcat(header,"$ts[$s] $op 0", "-> $td[$d]")>{
            
    bits<2> td;
    bits<2> ts;
    bits<8> op;
    bits<4> d;
    bits<4> s;

    let Inst{47-43} = slot;
    let Inst{20-18} = op{6-4};
    let Inst{17-16} = td;
    let Inst{15-12} = d;
    let Inst{9-6} = op{3-0};
    let Inst{5-4} = ts;
    let Inst{3-0} = s;
}

// MaskGen
class BIUMaskGenInstr<string header, bits<5> slot>
        : UCPMInst<(ins TPort: $td, TPort: $ts, imm32i: $d, imm32i: $s, Flags: $f), (outs), !strconcat(header,"$ts[$s]$f", "-> $td[$d].Mask")>{
            
    bits<2> td;
    bits<2> ts;
    bits<4> d;
    bits<4> s;
    bits<2> f;

    let Inst{47-43} = slot;
    let Inst{20-18} = 0b100;
    let Inst{17-16} = td;
    let Inst{15-12} = d;
    let Inst{11-10} = f;
    let Inst{9-6} = 0b0011;
    let Inst{5-4} = ts;
    let Inst{3-0} = s;
}

// NOT
class BIUNOTInstr<string header, bits<5> slot>
        : UCPMInst<(ins TPort: $ts, TPort: $td, imm32i: $s, imm32i: $d), (outs), !strconcat(header,"~$ts[$s]", "-> $td[$d]")>{
  
    bits<2> ts;
    bits<2> td;
    bits<4> s;
    bits<4> d;

    let Inst{47-43} = slot;
    let Inst{20-18} = 0b100;
    let Inst{17-16} = td;
    let Inst{15-12} = d;
    let Inst{9-6} = 0b0010;
    let Inst{5-4} = ts;
    let Inst{3-0} = s;
}

// Imm
class BIUImmInstr<string header, bits<5> slot>
        : UCPMInst<(ins TPort: $td, imm32i: $d, imm32i: $imm8, Flags: $f, Flags: $ff), (outs), !strconcat("$imm8 -> ", header,"$td[$d]$f")>{
  
    bits<8> imm8;
    bits<2> td;
    bits<4> d;
    bits<2> f;
    bits<2> ff;

    let Inst{47-43} = slot;
    let Inst{20-18} = 0b110;
    let Inst{17-16} = td;
    let Inst{15-12} = d;
    let Inst{11-10} = f;
    let Inst{9-4} = imm8{7-2};
    let Inst{3-2} = ff;
    let Inst{1-0} = imm8{1-0};
}

// Mov
class BIUMovInstr<string header, bits<5> slot>
        : UCPMInst<(ins TPort: $ts, TPort: $td, imm32i: $s, imm32i: $d, Flags: $fs, Flags: $fd1, Flags: $fd2), (outs), !strconcat(header,"$ts[$s]$fs", "-> $td[$d]$fd1$fd2")>{
  
    bits<2> ts;
    bits<2> td;
    bits<4> s;
    bits<4> d;
    bits<2> fs;
    bits<2> fd1;
    bits<2> fd2;

    let Inst{47-43} = slot;
    let Inst{20-18} = 0b101;
    let Inst{17-16} = td;
    let Inst{15-12} = d;
    let Inst{11-10} = ts;
    let Inst{9-6} = s;
    let Inst{5-4} = fd1;
    let Inst{3-2} = fd2;
    let Inst{1-0} = fs;
}

// WAIT 
class BIUWaitInstr<string header, bits<5> slot>
        : CommonAssignInstr<(ins imm32i: $imm7),(outs),
                                   header, "", "", "Wait $imm7">{
  bits<7>  imm7;
  
  let Inst{47-43} = slot;
  let Inst{20-17} = 0b1000;
  let Inst{16-10} = imm7;
  let Inst{9-6} = 0b1000;
}

// NOP   ??? can not confirm which biu
// class BIUNOP<string header, bits<5> slot>
//         : CommonAssignInstr<(ins),(outs),"", "", "", "NOP">{
//   let Inst{47-43} = slot;
//   let Inst{20-16} = 0b10011;
//   let Inst{9-6} = 0b1000;
// }
def BIU0NOP: UCPMInst <(ins), (outs), "NOP"> {
  let Inst{47-43} = 0b01110;
  let Inst{20-16} = 0b10011;
  let Inst{9-6} = 0b1000;
}
def BIU1NOP: UCPMInst <(ins), (outs), "NOP">{
  let Inst{47-43} = 0b01111;
  let Inst{20-16} = 0b10011;
  let Inst{9-6} = 0b1000;
}
def BIU2NOP: UCPMInst <(ins), (outs), "NOP">{
  let Inst{47-43} = 0b10000;
  let Inst{20-16} = 0b10011;
  let Inst{9-6} = 0b1000;
}

// SetCond  
class BIUSetCondInstr<string header, bits<5> slot>
        : UCPMInst<(ins imm32i: $imm8, imm32i: $m), (outs), !strconcat("$imm8 -> ", header,"SetCond[$m]")> 
                                   {
  bits<8>  imm8;
  bit m;
  
  let Inst{47-43} = slot;
  let Inst{20-16} = 0b10010;
  let Inst{12} = m;
  let Inst{11-10} = imm8{7-6};
  let Inst{9-6} = 0b1000;
  let Inst{5-0} = imm8{5-0};
}

// BIUKG
// class BIUKGInstr<string header, bits<5> slot, bit dest, Operand DestTy>
//         : MRegAssignInstr<(ins TPort: $tm), header, "$tm", DestTy>{
// 
//     
//     bits<2> tm;
//     
//     let Inst{47-43} = slot;
//     let Inst{20-18} = 0b100;
//     let Inst{14} = dest;
//     let Inst{13-12} = d{7-6};  
//     let Inst{11-10} = tm;
//     let Inst{9-6} = 0b1001;
//     let Inst{5-0} = d{5-0};
// }
// def BIU0KG : BIUKGInstr<"BIU0.", 0b01110, 1, BIU0DestTE >;



//defm BIU0disLd : BIULdInstr_dis<"BIU0.", 0b01110>;
//defm BIU0comLd : BIULdInstr_com<"BIU0.", 0b01110>;
def BIU0St : BIUStInstr<"BIU0.", 0b01110>;
def BIU0Bin : BIUBinInstr<"BIU0.", 0b01110>;
def BIU0Shift : BIUShiftInstr<"BIU0.", 0b01110>;
def BIU0Com : BIUComInstr<"BIU0.", 0b01110>;
def BIU0NOT : BIUNOTInstr<"BIU0.", 0b01110>;
def BIU0Imm : BIUImmInstr<"BIU0.", 0b01110>;
def BIU0Mov : BIUMovInstr<"BIU0.", 0b01110>;
def BIU0MaskGen : BIUMaskGenInstr<"BIU0.", 0b01110>;
def BIU0Wait : BIUWaitInstr<"BIU0.", 0b01110>;
def BIU0SetCond : BIUSetCondInstr<"BIU0.", 0b01110>;

//defm BIU1disLd : BIULdInstr_dis<"BIU1.", 0b01111>;
//defm BIU1comLd : BIULdInstr_com<"BIU1.", 0b01111>;
def BIU1St : BIUStInstr<"BIU1.", 0b01111>;
def BIU1Bin : BIUBinInstr<"BIU1.", 0b01111>;
def BIU1Shift : BIUShiftInstr<"BIU1.", 0b01111>;
def BIU1Com : BIUComInstr<"BIU1.", 0b01111>;
def BIU1NOT : BIUNOTInstr<"BIU1.", 0b01111>;
def BIU1Imm : BIUImmInstr<"BIU1.", 0b01111>;
def BIU1Mov : BIUMovInstr<"BIU1.", 0b01111>;
def BIU1MaskGen : BIUMaskGenInstr<"BIU1.", 0b01111>;
def BIU1Wait : BIUWaitInstr<"BIU1.", 0b01111>;
def BIU1SetCond : BIUSetCondInstr<"BIU1.", 0b01111>;

//defm BIU2disLd : BIULdInstr_dis<"BIU2.", 0b10000>;
//defm BIU2comLd : BIULdInstr_com<"BIU2.", 0b10000>;
def BIU2St : BIUStInstr<"BIU2.", 0b10000>;
def BIU2Bin : BIUBinInstr<"BIU2.", 0b10000>;
def BIU2Shift : BIUShiftInstr<"BIU2.", 0b10000>;
def BIU2Com : BIUComInstr<"BIU2.", 0b10000>;
def BIU2NOT : BIUNOTInstr<"BIU2.", 0b10000>;
def BIU2Imm : BIUImmInstr<"BIU2.", 0b10000>;
def BIU2Mov : BIUMovInstr<"BIU2.", 0b10000>;
def BIU2MaskGen : BIUMaskGenInstr<"BIU2.", 0b10000>;
def BIU2Wait : BIUWaitInstr<"BIU2.", 0b10000>;
def BIU2SetCond : BIUSetCondInstr<"BIU2.", 0b10000>;
// ducx end BIU Instruction --------------------------------------------------------------------------


// ducx start MFetch Instruction ----------------------------------------------------------------------------  

// RepeatImm
def REPEATIMM : SEQInst<(ins imm32i: $imm),
                        "REPEAT", "", "$imm"> {
  bits<12> imm;
  let Inst{22-21} = 0b01;
  let Inst{19-16}  = 0b1101;
  let Inst{15-4}   = imm;
}

// RepeatKI
def REPEATK : SEQInst<(ins KIReg: $ki, imm32i: $imm),
                       "REPEAT", "", "$ki - $imm"> {
  bits<4> ki;
  bits<5> imm;
  let Inst{22-21} = 0b01;
  let Inst{20}  = imm{4};
  let Inst{19-16}  = 0b1100;
  let Inst{7-4}   = imm{3-0};
  let Inst{3-0}   = ki;
}

// lpto
def LPTO : SEQInst<(ins Label:$l, KIReg: $ki, imm32i: $imm),
                     "LPTO", " $l", "$ki - $imm"> {
  bits<12> l;
  bits<4> ki;
  bits<5> imm;
  let Inst{22-21} = 0b00;
  let Inst{20}  = imm{4};
  let Inst{19-8}  = l;
  let Inst{7-4}   = imm{3-0};
  let Inst{3-0}   = ki;
}

// // JUMP label ??? if
// def JUMPlabel : UCPMInst<(ins Label: $l, Flags: $s, Flags: $cond), (outs), !strconcat("JUMP", " $l$s")> {
//   bits<12> l;
//   bit s;
//   bits<8> cond;
//   let Inst{47-43} = 0b00000;
//   let Inst{22-21} = 0b10;
//   let Inst{20} = s;
//   let Inst{19-8} = l;
//   let Inst{7-0}  = cond;//??
// }
// 
// // JUMP ki ??? if
// def JUMPK : UCPMInst<(ins KIReg: $ki, Flags: $s, Flags: $cond), (outs), !strconcat("JUMP", " $ki$s")> {
//   bits<8> ki;
//   bit s;
//   bits<8> cond;
//   let Inst{47-43} = 0b00000;
//   let Inst{22-21} = 0b11;
//   let Inst{20} = s;
//   let Inst{15-8} = ki;
//   let Inst{7-0}  = cond;//??
// }

// STOP
def MPUStop : UCPMInst<(ins), (outs), "MPUSTOP"> {
  let Inst{47-43} = 0b00000;
  let Inst{22-20} = 0b010;
  let Inst{19-16} = 0b1110;
  let Inst{15-0}  = 0;
}

// ADD SUB ST NLT EQU NEQ AND OR  
def MFetchBinInstr : CommonAssignInstr<(ins MFetchBinOprtor: $op, KIReg: $kid, KIReg: $kis0,
                                   KIReg: $kis1), (outs), "", "$kid", "->", "$kis0 $op $kis1">{
    bits<8> op;
    bits<8> kid;
    bits<4> kis0;
    bits<4> kis1;
    
    let Inst{47-43} = 0b00000;
    let Inst{22-21} = 0b01;
    let Inst{19-16} = op{3-0};
    let Inst{15-8} = kid;
    let Inst{7-4} = kis1;
    let Inst{3-0} = kis0;
}

// LSHT RSHT  
def MFetchShiftInstr : CommonAssignInstr<(ins MFetchShiftOprtor: $op, KIReg: $kid, KIReg: $kis1, KIReg: $kis0), (outs), "", "$kid", "->", "$kis1 $op $kis0">{
    bits<8> op;
    bits<8> kid;
    bits<4> kis1;
    bits<4> kis0;
    
    let Inst{47-43} = 0b00000;
    let Inst{22-21} = 0b01;
    let Inst{19-16} = op{3-0};
    let Inst{15-8} = kid;
    let Inst{7-4} = kis1;
    let Inst{3-0} = kis0;
}

// NOT
def MFetchNOTInstr
        : CommonAssignInstr<(ins KIReg: $kid, KIReg: $kis), (outs), "", "$kid", "->", "~$kis">{
  
    bits<8> kid;
    bits<4> kis;

    let Inst{47-43} = 0b00000;
    let Inst{22-21} = 0b01;
    let Inst{19-16} = 0b1010;
    let Inst{15-8} = kid;
    let Inst{3-0} = kis;
}

// MOV
def MFetchMOVInstr
        : CommonAssignInstr<(ins KIReg: $kid, KIReg: $kis, Flags: $CR), (outs), "", "$kid$CR", "->", "$kis">{
  
    
    bits<8> kid;
    bits<4> kis;
    bit CR;
    
    let Inst{47-43} = 0b00000;
    let Inst{22-21} = 0b01;
    let Inst{19-16} = 0b1011;
    let Inst{15-8} = kid;
    let Inst{7} = CR;
    let Inst{3-0} = kis;
}

// NOP
def MFetchNOP:  UCPMInst <(ins), (outs), "NOP"> {
  let Inst{47-43} = 0b00000;
  let Inst{22-21} = 0b10;
  let Inst{7-4} = 0b1101;
  let Inst{3-0} = 0b1011;
}

// ReadRegi
// def MFetchReadRegiInstr
//         : CommonAssignInstr<(ins KIReg: $kid, Flags: $B), (outs), "", "$kid", "->", "Regi$B">{
//   
//     bits<7> kid;
//     bit B;
//     bits<2> ;
//     
//     let Inst{47-43} = 0b00000;
//     let Inst{22-21} = 0b01;
//     let Inst{19-16} = 0b1111;
//     let Inst{15} = B;
//     let Inst{14-8} = kid;
//     let Inst{7-6} = 0b10;
//     let Inst{5-4} = ?;
// }

// WriteRegi
// def MFetchWriteRegiInstr
//         : CommonAssignInstr<(ins KIReg: $kis, Flags: $B), (outs), "", "WriteRegi$B", "->", "$kis">{
//   
//     bits<4> kis;
//     bit B;
//     
//     let Inst{47-43} = 0b00000;
//     let Inst{22-21} = 0b01;
//     let Inst{19-16} = 0b1111;
//     let Inst{15} = B;
//     let Inst{7-6} = 0b11;
//     let Inst{5-4} = ?;
//     let Inst{3-0} = kis;
// }


// ducx end MFetch Instruction -----------------------------------------------------------------------------  


//from original



def JUMPK : SEQInst<(ins Label:$l, KIReg: $ki),
                     "JUMP", " ($l)", "$ki"> {
  bits<10> l;
  bits<23> ki;
  let Inst{36-33} = 0b0110;
  let Inst{32-23} = l;
  let Inst{22-0}  = ki;
}

def JUMP : SEQInst<(ins Label:$l),
                     "JUMP", " ($l)", "TRUE"> {
  bits<10> l;
  let Inst{36-33} = 0b0101;
  let Inst{32-23} = l;
  let Inst{22-0}  = 0;
}



def LPTOImm : SEQInst<(ins Label:$l, imm32i: $imm),
                     "LPTO", " ($l)", "$imm"> {
  bits<11> l;
  bits<23> ki;
  bits<8> imm;
  let Inst{22-19} = 0b1000;
  let Inst{18-8}  = l;
  let Inst{7-0}   = imm;
}

