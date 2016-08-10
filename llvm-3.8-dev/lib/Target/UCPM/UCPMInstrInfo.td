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

class UCPMRToDest<string header, bits<4> slotid, bits<4> mregid, Operand DestTy>
      : UCPAssignInstr<(ins MReg: $m), header, "$m", DestTy> {
  bits<7> m;
  let Inst{47-44} = slotid;
  let Inst{14-11} = mregid;
}

//MR0 read instructions
def MR0ToDestCom : UCPMRToDest<"R0.", 0b0000, 0b0110, MR0DestT> {
  let Inst{10-4} = m;
  let Inst{3-0} = d;
}
def MR0ToDestSI : UCPMRToDest<"R0.", 0b0000, 0b1110, MR0DestT> {
  let Inst{10-4} = m;
  let Inst{3-0} = d;
}
//MR1 read instructions
def MR1ToDestCom : UCPMRToDest<"R1.", 0b0001, 0b0110, MR1DestT> {
  let Inst{10-4} = m;
  let Inst{3-0} = d;
}
def MR1ToDestSI : UCPMRToDest<"R1.", 0b0001, 0b1110, MR1DestT> {
  let Inst{10-4} = m;
  let Inst{3-0} = d;
}
//MR2 read instructions
def MR2ToDestCom : UCPMRToDest<"R2.", 0b0010, 0b0110, MR2DestT> {
  let Inst{10-4} = m;
  let Inst{3-0} = d;
}
def MR2ToDestSI : UCPMRToDest<"R2.", 0b0010, 0b1110, MR2DestT> {
  let Inst{10-4} = m;
  let Inst{3-0} = d;
}
//MR3 read instructions
def MR3ToDestCom : UCPMRToDest<"R3.", 0b0011, 0b0110, MR3DestT> {
  let Inst{10-4} = m;
  let Inst{3-0} = d;
}
def MR3ToDestSI : UCPMRToDest<"R3.", 0b0011, 0b1110, MR3DestT> {
  let Inst{10-4} = m;
  let Inst{3-0} = d;
}
//MR4 read instructions
def MR4ToDestCom : UCPMRToDest<"R4.", 0b0100, 0b0110, MR4DestT> {
  let Inst{10-4} = m;
  let Inst{3-0} = d;
}
def MR4ToDestSI : UCPMRToDest<"R4.", 0b0100, 0b1110, MR4DestT> {
  let Inst{10-4} = m;
  let Inst{3-0} = d;
}
//MR5 read instructions
def MR5ToDestCom : UCPMRToDest<"R5.", 0b0101, 0b0110, MR5DestT> {
  let Inst{10-4} = m;
  let Inst{3-0} = d;
}
def MR5ToDestSI : UCPMRToDest<"R5.", 0b0101, 0b1110, MR5DestT> {
  let Inst{10-4} = m;
  let Inst{3-0} = d;
}



//3 kinds of base class for SHUIndToDest
//"TB+=" and "TB=+"
class UCPSHUIndToDest_SetTB<string header, bit TBSymbol, bits<4> slot, Operand DestTy>
        : UCPSHUIndexAssignInstr<(ins TPort: $tm, TPort: $tn, TEPort: $tk, imm32i: $imm5),
                      header, !strconcat("{$tm,$tn} IND $tk",
                                         !if(TBSymbol, "(TB=+$imm5)", "(TB+=$imm5)")),
                      DestTy> {
  bits<2> tm;
  bits<2> tn;
  bits<4> tk;
  bits<5> imm5;
  let Inst{47-44} = slot;
  let Inst{22-19} = tk;
  let Inst{18-17} = tn;
  let Inst{16} = 0;
  let Inst{15} = TBSymbol;
  let Inst{14-10} = imm5;
  let Inst{9-8} = tm;
}
//Select Tn
class UCPSHUIndToDest_FullTn<string header, bits<4> slot, Operand DestTy>
        : UCPSHUIndexAssignInstr<(ins TPort: $tm, TEPort: $tn, TEPort: $tk), header,
                      "{$tm,$tn} IND $tk", DestTy> {
  bits<2> tm;
  bits<4> tn;
  bits<4> tk;
  let Inst{47-44} = slot;
  let Inst{22-19} = tk;
  let Inst{9-8} = tm;
}
//No set TB, Tn: T0-T3
class UCPSHUIndToDest<string header, bits<4> slot, Operand DestTy>
        : UCPSHUIndexAssignInstr<(ins TPort: $tm, TPort: $tn, TEPort: $tk), header,
                      "{$tm,$tn} IND $tk", DestTy> {
  bits<2> tm;
  bits<2> tn;
  bits<4> tk;
  let Inst{47-44} = slot;
  let Inst{22-19} = tk;
  let Inst{9-8} = tm;
}
//final shu0 test instruction, for case: "+="
def SHU0Ind_SetTBToMACC_0 : UCPSHUIndToDest_SetTB<"SHU0.", 0, 0b0110, UCPMACCTI> {
  let Inst{7-6} = 0b01;
  let Inst{5-0} = d;
}
//final shu0 test instruction, for case: "=+"
def SHU0Ind_SetTBToMACC_1 : UCPSHUIndToDest_SetTB<"SHU0.", 1, 0b0110, UCPMACCTI> {
  let Inst{7-6} = 0b01;
  let Inst{5-0} = d;
}
def SHU1Ind_SetTBToMACC_0 : UCPSHUIndToDest_SetTB<"SHU1.", 0, 0b0111, UCPMACCTI> {
  let Inst{7-6} = 0b01;
  let Inst{5-0} = d;
}
def SHU1Ind_SetTBToMACC_1 : UCPSHUIndToDest_SetTB<"SHU1.", 1, 0b0111, UCPMACCTI> {
  let Inst{7-6} = 0b01;
  let Inst{5-0} = d;
}
def SHU2Ind_SetTBToMACC_0 : UCPSHUIndToDest_SetTB<"SHU2.", 0, 0b1000, UCPMACCTI> {
  let Inst{7-6} = 0b01;
  let Inst{5-0} = d;
}
def SHU2Ind_SetTBToMACC_1 : UCPSHUIndToDest_SetTB<"SHU2.", 1, 0b1000, UCPMACCTI> {
  let Inst{7-6} = 0b01;
  let Inst{5-0} = d;
}



//from original
def REPEATIMM : SEQInst<(ins imm32i: $imm),
                        "REPEAT", "", "$imm"> {
  bits<11> imm;
  let Inst{22-19} = 0b0001;
  let Inst{18-8}  = imm;
  let Inst{7-0}   = 0;
}

def REPEATK : SEQInst<(ins KIReg: $ki, imm32i: $imm),
                       "REPEAT", "", "$ki - $imm"> {
  bits<23> ki;
  bits<4> imm;
  let Inst{22-19} = 0b0010;
  let Inst{17-8}  = 0;
  let Inst{7-4}   = imm;
  let Inst{3-0}   = ki{3-0};
}

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

def LPTO : SEQInst<(ins Label:$l, KIReg: $ki, imm32i: $imm),
                     "LPTO", " ($l)", "$ki - $imm"> {
  bits<11> l;
  bits<23> ki;
  bits<4> imm;
  let Inst{22-19} = 0b0011;
  let Inst{18-8}  = l;
  let Inst{7-4}   = imm;
  let Inst{3-0}   = ki{3-0};
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

def MPUStop : UCPMInst<(ins), (outs), "MPUSTOP"> {
  let Inst{47-44} = 0b1101;
  let Inst{43-22} = 0;
  let Inst{22-19} = 0b0100;
  let Inst{18-0}  = 0;
}