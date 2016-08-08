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
def NOP : UCPMInst <(ins), (outs), "NOP"> {
  let Inst = 0;
}

class MRToDest<string header, bits<4> slotid, bits<2> dest, Operand DestTy>
        : AssignInstr<(ins MReg: $m, WFlag: $w), header, "$m$w", DestTy> {
  bits<9> m;
  bits<3> w;
  let Inst{47-44} = slotid;
  let Inst{1-0}   = dest;
}
//yangl
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

multiclass MR0Inst {
  def ToM : MRToDest<"R0.", 0b0000, 0b00, MRegO> {
    let Inst{23}    = w{0};
    let Inst{22}    = 1;
    let Inst{21-20} = c;
    let Inst{19-11} = m;
    let Inst{10-2}  = d;
  }
  def ToSHU : MRToDest<"R0.", 0b0000, 0b01, SHUT> {
    let Inst{23}    = w{0};
    let Inst{22}    = 1;
    let Inst{21-20} = c;
    let Inst{19-11} = m;
    let Inst{10-2}  = d;
  }
  def ToMACC : MRToDest<"R0.", 0b0000, 0b11, MACCTI> {
    let Inst{23}    = w{0};
    let Inst{22}    = 1;
    let Inst{21-20} = c;
    let Inst{19-11} = m;
    let Inst{10-2}  = d;
  }
}
defm MR0 : MR0Inst;//delete

def MR1ToSHU : MRToDest<"R1.", 0b0001, 0b01, SHUT> {
  let Inst{23}    = w{0};
  let Inst{19}    = 1;
  let Inst{18-17} = c;
  let Inst{16-8}  = m;
  let Inst{7-2}   = d{5-0};
}

def MR1ToMACC : MRToDest<"R1.", 0b0001, 0b11, MACCTI> {
  let Inst{23}    = w{0};
  let Inst{19}    = 1;
  let Inst{18-17} = c;
  let Inst{16-8}  = m;
  let Inst{7-2}   = d{5-0};
}

def MR2ToMACC : MRToDest<"R2.", 0b0010, 0b11, MACCTI> {
  let Inst{20}    = 1;
  let Inst{19-18} = c;
  let Inst{17-15} = w;
  let Inst{14-8}  = m{6-0};
  let Inst{7-2}   = d{5-0};
}

multiclass MR3Inst {
  def ToBIU: MRToDest <"", 0b0011, 0b10, BIUO> {
    let Inst{23}    = w{0};
    let Inst{14}    = 1;
    let Inst{13-12} = c;
    let Inst{11}    = 0;
    let Inst{10-4}  = m{6-0};
    let Inst{3-2}   = d{1-0};
  }
  def ToBIUKG: CommonAssignInstr <(ins MReg: $m), (outs BIU: $b, Condition: $c),
                                  "", "$b(KG)$c", "->", "$m"> {
    bits<8> m;
    bits<2> b;
    bits<2> c;
    let Inst{47-44} = 3;
    let Inst{14}    = 1;
    let Inst{13-12} = c;
    let Inst{11}    = 1;
    let Inst{10-4}  = m{6-0};
    let Inst{3-2}   = b;
    let Inst{1-0}   = 0b10;
  }
}
defm MR3 : MR3Inst;

class SHUIndTOptToDest<string header, bit tbopt, bits<4> slot, bits<2> dest, Operand DestTy>
        : AssignInstr<(ins TEPort: $tm, TEPort: $tn, imm32i: $imm3),
                      header, !strconcat("$tm IND $tn",
                                         !if(tbopt, "(TB=+$imm3)", "")),
                      DestTy> {
  bits<4> tm;
  bits<4> tn;
  bits<3> imm3;
  let Inst{47-44} = slot;
  let Inst{27-25} = 0b001;
  let Inst{24-23} = c;
  let Inst{22}    = tbopt;
  let Inst{21-18} = tm;
  let Inst{17-14} = tn;
  let Inst{13-11} = imm3;
  let Inst{10-2}  = d;
  let Inst{1-0}   = dest;
}//delete
//yangl, 3 kinds of base class for SHUIndToDest
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
//yangl: final shu0 test instruction, for case: "+="
def SHU0Ind_SetTBToMACC_0 : UCPSHUIndToDest_SetTB<"SHU0.", 0, 0b0110, UCPMACCTI> {
  let Inst{7-6} = 0b01;
  let Inst{5-0} = d;
}
//yangl: final shu0 test instruction, for case: "=+"
def SHU0Ind_SetTBToMACC_1 : UCPSHUIndToDest_SetTB<"SHU0.", 1, 0b0110, UCPMACCTI> {
  let Inst{7-6} = 0b01;
  let Inst{5-0} = d;
}







class SHUIndTToDest<string header, bit tbopt, bits<4> slot, bits<2> dest, Operand DestTy>
        : AssignInstr<(ins TEPort: $tm, TEPort: $tn, imm32i: $imm3),
                      header, !strconcat("$tm IND $tn",
                                         !if(tbopt, "(TB=+$imm3)", "")),
                      DestTy> {
  bits<4> tm;
  bits<4> tn;
  bits<3> imm3;
  let Inst{47-44} = slot;
  let Inst{27-25} = 0b001;
  let Inst{24-23} = c;
  let Inst{22}    = tbopt;
  let Inst{21-18} = tm;
  let Inst{17-14} = tn;
  let Inst{13-11} = imm3;
  let Inst{10-2}  = d;
  let Inst{1-0}   = dest;
}//delete
multiclass SHUIndTOptInstr<string header, bit tbopt, bits<4> slot> {
  def ToM    : SHUIndTOptToDest<header, tbopt, slot, 0b00, MRegO >;
  def ToSHU  : SHUIndTOptToDest<header, tbopt, slot, 0b01, SHUT  >;
  def ToBIU  : SHUIndTOptToDest<header, tbopt, slot, 0b10, BIUO  >;
  def ToMACC : SHUIndTOptToDest<header, tbopt, slot, 0b11, MACCTI>;
}
multiclass SHUIndT<string header, bits<4> slot> {
  defm NoOpt :  SHUIndTOptInstr<header, 0, slot>;
  defm Opt   :  SHUIndTOptInstr<header, 1, slot>;
}

class SHUIndTBOptToDest<string header, bit tbopt, bits<4> slot, bits<2> dest, Operand DestTy>
        : AssignInstr<(ins TEPort: $tm, imm32i: $imm3),
                      header, !strconcat("$tm IND TB",
                                         !if(tbopt, "(TB+=$imm3)", "")),
                      DestTy> {
  bits<4> tm;
  bits<3> imm3;
  let Inst{47-44} = slot;
  let Inst{27-25} = 0b010;
  let Inst{24-23} = c;
  let Inst{22}    = tbopt;
  let Inst{21-18} = tm;
  let Inst{17-14} = 0;
  let Inst{13-11} = imm3;
  let Inst{10-2}  = d;
  let Inst{1-0}   = dest;
}
multiclass SHUIndTBOptInstr<string header, bit tbopt, bits<4> slot> {
  def ToM    : SHUIndTBOptToDest<header, tbopt, slot, 0b00, MRegO >;
  def ToSHU  : SHUIndTBOptToDest<header, tbopt, slot, 0b01, SHUT  >;
  def ToBIU  : SHUIndTBOptToDest<header, tbopt, slot, 0b10, BIUO  >;
  def ToMACC : SHUIndTBOptToDest<header, tbopt, slot, 0b11, MACCTI>;
}
multiclass SHUIndTB<string header, bits<4> slot> {
  defm NoOpt :  SHUIndTBOptInstr<header, 0, slot>;
  defm Opt   :  SHUIndTBOptInstr<header, 1, slot>;
}

class SHUDPCode<bits<2> dest, bits<4> slot> {
  field bits<48> Inst;
  bits<4> tm;
  bits<4> tn;
  bits<2> tp;
  bits<8> op;
  bits<32> f;
  bits<2>  c;
  bits<9>  d;
  let Inst{47-44} = slot;
  let Inst{29-28} = tp;
  let Inst{27-25} = 0b011;
  let Inst{24-23} = c;
  let Inst{22}    = f{3};
  let Inst{21-18} = tm;
  let Inst{17-14} = tn;
  let Inst{13-11} = f{2-0};
  let Inst{10-2}  = d;
  let Inst{1-0}   = dest;
}
multiclass SHUCombInstr<string header, bits<4> slot> {
  let DisableEncoding = "$op" in {
  def ToM    : BinaryInstr<header, 0, SHUOprtor, MRegO >, SHUDPCode<0b00, slot>;
  def ToSHU  : BinaryInstr<header, 0, SHUOprtor, SHUT  >, SHUDPCode<0b01, slot>;
  def ToBIU  : BinaryInstr<header, 0, SHUOprtor, BIUO  >, SHUDPCode<0b10, slot>;
  def ToMACC : BinaryInstr<header, 0, SHUOprtor, MACCTI>, SHUDPCode<0b11, slot>;
  }
}

class SHUCtrlCode<bits<4> slot, bit ctrl> {
  field bits<48> Inst;
  let Inst{47-44} = slot;
  let Inst{27-26} = 0b11;
  let Inst{25}    = ctrl;
  let Inst{24-0}  = 0;
}

defm SHU0IndT : SHUIndT<"SHU0.", 0b0100>;
defm SHU0IndTB : SHUIndTB<"SHU0.", 0b0100>;
defm SHU0Comb : SHUCombInstr<"SHU0.", 0b0100>;
def  SHU0Start : CtrlInstr<"SHU0", "Start">, SHUCtrlCode<0b0100, 0>;
def  SHU0Stop : CtrlInstr<"SHU0", "Stop">, SHUCtrlCode<0b0100, 1>;
defm SHU1IndT : SHUIndT<"SHU1.", 0b0101>;
defm SHU1IndTB : SHUIndTB<"SHU1.", 0b0101>;
defm SHU1Comb : SHUCombInstr<"SHU1.", 0b0101>;
def  SHU1Start : CtrlInstr<"SHU1", "Start">, SHUCtrlCode<0b0101, 0>;
def  SHU1Stop : CtrlInstr<"SHU1", "Stop">, SHUCtrlCode<0b0101, 1>;

class BIULdToDest<string header, bits<4> slot, bits<2> dest, Operand DestTy>
        : AssignInstr<(ins Flags: $f), header, "DM$f", DestTy> {
  bits<32> f;
  let Inst{47-44} = slot;
  let Inst{19-17} = 0b100;
  let Inst{16-15} = c;
  let Inst{14-11} = f{3-0};
  let Inst{10-2}  = d;
  let Inst{1-0}   = dest;
}
multiclass BIULdInstr<string header, bits<4> slot> {
  def ToM    : BIULdToDest<header, slot, 0b00, MRegO >;
  def ToSHU  : BIULdToDest<header, slot, 0b01, SHUT  >;
  def ToMACC : BIULdToDest<header, slot, 0b11, MACCTI>;
}

class BIUStInstr<string header, bits<4> slot>
        : UCPMInst<(ins Flags: $f), (outs Condition: $c),
                       !strconcat(header, "-> DM$f$c")> {
  bits<32> f;
  bits<2> c;
  let Inst{47-44} = slot;
  let Inst{19-17} = 0b101;
  let Inst{16-15} = c;
  let Inst{14-11} = f{3-0};
  let Inst{10-1}  = 0;
  let Inst{0}     = f{4};
}

class BIUKGInstr<string header, bits<4> slot>
        : AssignInstr<(ins), header, "(KG)", MRegO> {
  let Inst{47-44} = slot;
  let Inst{19-17} = 0b110;
  let Inst{16-15} = c;
  let Inst{14-11} = 0;
  let Inst{10-2}  = d;
  let Inst{1-0}   = 0b00;
}

defm BIU0Ld : BIULdInstr<"BIU0.", 0b1010>;
def BIU0St: BIUStInstr<"BIU0", 0b1010>;
def BIU0KG: BIUKGInstr<"BIU0", 0b1010>;
defm BIU1Ld : BIULdInstr<"BIU1.", 0b1011>;
def BIU1St: BIUStInstr<"BIU1", 0b1011>;
def BIU1KG: BIUKGInstr<"BIU1", 0b1011>;
defm BIU2Ld : BIULdInstr<"BIU2.", 0b1100>;
def BIU2St: BIUStInstr<"BIU2", 0b1100>;
def BIU2KG: BIUKGInstr<"BIU2", 0b1100>;

class IALUBaseCode {
  field bits<48> Inst;
  let Inst{47-44} = 0b0110;
}

class IALUCode<bits<2> dest> : IALUBaseCode {
  bits<8> op;
  bits<4> tm;
  bits<4> tn;
  bits<2> tp;
  bits<32> f;
  bits<9>  d;
  bits<2>  c;
  let Inst{30-26} = op{4-0};
  let Inst{25-24} = c;
  let Inst{23-22} = 0;
  let Inst{21-18} = f{3-0};
  let Inst{17-16} = tm{1-0};
  let Inst{15-14} = tn{1-0};
  let Inst{13-12} = tp;
  let Inst{11}    = 0;
  let Inst{10-2}  = d;
  let Inst{1-0}   = dest;
}

class IALUTPCode<bits<2> dest> : IALUCode<dest>  {
  bits<8> op;
  bits<4> tm;
  bits<4> tn;
  bits<2> tp;
  bits<32> f;
  bits<9>  d;
  bits<2>  c;
  let Inst{30-26} = op{4-0};
  let Inst{25-24} = c;
  let Inst{23-22} = 0b00;
  let Inst{21-18} = f{3-0};
  let Inst{17-16} = tm{1-0};
  let Inst{15-14} = tn{1-0};
  let Inst{13-12} = tp;
  let Inst{11}    = 0;
  let Inst{10-2}  = d;
  let Inst{1-0}   = dest;
}
multiclass IALUTPToDest {
  def ToM    : TParaInstr<"", 1, "", IALUTPOprtor, MRegO >, IALUTPCode<0b00>;
  def ToSHU  : TParaInstr<"", 1, "", IALUTPOprtor, SHUT  >, IALUTPCode<0b01>;
  def ToBIU  : TParaInstr<"", 1, "", IALUTPOprtor, BIUO  >, IALUTPCode<0b10>;
  def ToMACC : TParaInstr<"", 1, "", IALUTPOprtor, MACCTI>, IALUTPCode<0b11>;
}
class IALUBinCode<bits<2> dest> : IALUTPCode<dest> {
  let tp = 0b00;
}
multiclass IALUBinToDest {
  let isCodeGenOnly = 1 in {
  def BinToM    : BinaryInstr<"", 1, IALUBinOprtor, MRegO >, IALUBinCode<0b00>;
  def BinToSHU  : BinaryInstr<"", 1, IALUBinOprtor, SHUT  >, IALUBinCode<0b01>;
  def BinToBIU  : BinaryInstr<"", 1, IALUBinOprtor, BIUO  >, IALUBinCode<0b10>;
  def BinToMACC : BinaryInstr<"", 1, IALUBinOprtor, MACCTI>, IALUBinCode<0b11>;
  }
  let isCodeGenOnly = 1 in {
  def DPToM     : BinaryInstr<"", 0, IALUDPOprtor, MRegO >, IALUBinCode<0b00>;
  def DPToSHU   : BinaryInstr<"", 0, IALUDPOprtor, SHUT  >, IALUBinCode<0b01>;
  def DPToBIU   : BinaryInstr<"", 0, IALUDPOprtor, BIUO  >, IALUBinCode<0b10>;
  def DPToMACC  : BinaryInstr<"", 0, IALUDPOprtor, MACCTI>, IALUBinCode<0b11>;
  }
}

// ducx start 20160805 ----- IALUBinTo BIU/MACC/SHU --------------------------------------------------------------------
class dcx_IALUBaseCode {
  field bits<48> Inst;
  let Inst{47-44} = 0b0110;//?
}

class dcx_IALUCode<bits<2> dest> : dcx_IALUBaseCode {
  bits<8> op;
  bits<2> tm;
  bits<2> tn;
  bits<6> f;
  bits<4>  d;

  let Inst{11-10} = tm;
  let Inst{9-8} = tn;
  let Inst{7} = 0;
  let Inst{6-5}  = dest;
  let Inst{4-3}  = d{3-2};
  let Inst{2} = 0;
  let Inst{1-0}   = d{1-0};
}
// ducx:ADD SUB
class dcx_IALUASCode<bits<2> dest> : dcx_IALUCode<dest> {
  let Inst{21-18} = op{3-0};
  let Inst{17-12} = f;
}
// ducx:AND OR XOR
class dcx_IALULogicCode<bits<2> dest> : dcx_IALUCode<dest>{
  let Inst{21-18} = 0b1001;
  let Inst{14}    = 0;
  let Inst{13-12} = op{1-0};
}
// ducx:EQU NEQ
class dcx_IALUCom1Code<bits<2> dest> : dcx_IALUCode<dest>{
  let Inst{21-18} = 0b1001;
  let Inst{16-15} = f{2-1};
  let Inst{14-13} = op{1-0};
  let Inst{12}    = f{0};
}
// ducx:LT NST ST NLT
class dcx_IALUCom2Code<bits<2> dest> : dcx_IALUCode<dest>{
  let Inst{21-18} = 0b1010;
  let Inst{17-15} = f{3-1};
  let Inst{14-13} = op{1-0};
  let Inst{12}    = f{0};
}
// ducx:LSH RSH
class dcx_IALUShiftCode<bits<2> dest> : dcx_IALUCode<dest>{
  let Inst{21-18} = 0b0101;
  let Inst{17-15} = f{2-0};
  let Inst{13} = op{0};
}

// ducx:ADD SUB
multiclass dcx_IALUASToDest {
  def ASToSHU  : dcx_BinaryInstr<"", 1, dcx_IALUASOprtor, SHUT  >, dcx_IALUASCode<0b00>;
  def ASToBIU  : dcx_BinaryInstr<"", 1, dcx_IALUASOprtor, BIUO  >, dcx_IALUASCode<0b10>;
  def ASToMACC : dcx_BinaryInstr<"", 1, dcx_IALUASOprtor, MACCT >, dcx_IALUASCode<0b01>;
}
// ducx:AND OR XOR
multiclass dcx_IALULogicToDest {
  def ToSHU  : dcx_BinaryInstr<"", 1, dcx_IALULogicOprtor, SHUT  >, dcx_IALULogicCode<0b00>;
  def ToBIU  : dcx_BinaryInstr<"", 1, dcx_IALULogicOprtor, BIUO  >, dcx_IALULogicCode<0b10>;
  def ToMACC : dcx_BinaryInstr<"", 1, dcx_IALULogicOprtor, MACCT >, dcx_IALULogicCode<0b01>;
}
// ducx:EQU NEQ
multiclass dcx_IALUCom1ToDest {
  def ToSHU  : dcx_BinaryInstr<"", 1, dcx_IALUCom1Oprtor, SHUT  >, dcx_IALUCom1Code<0b00>;
  def ToBIU  : dcx_BinaryInstr<"", 1, dcx_IALUCom1Oprtor, BIUO  >, dcx_IALUCom1Code<0b10>;
  def ToMACC : dcx_BinaryInstr<"", 1, dcx_IALUCom1Oprtor, MACCT >, dcx_IALUCom1Code<0b01>;
}
// ducx:LT NST ST NLT
multiclass dcx_IALUCom2ToDest {
  def ToSHU  : dcx_BinaryInstr<"", 1, dcx_IALUCom2Oprtor, SHUT  >, dcx_IALUCom2Code<0b00>;
  def ToBIU  : dcx_BinaryInstr<"", 1, dcx_IALUCom2Oprtor, BIUO  >, dcx_IALUCom2Code<0b10>;
  def ToMACC : dcx_BinaryInstr<"", 1, dcx_IALUCom2Oprtor, MACCT >, dcx_IALUCom2Code<0b01>;
}
// ducx:LSH RSH
multiclass dcx_IALUShiftToDest {
  def ToSHU  : dcx_BinaryInstr<"", 1, dcx_IALUShiftOprtor, SHUT  >, dcx_IALUShiftCode<0b00>;
  def ToBIU  : dcx_BinaryInstr<"", 1, dcx_IALUShiftOprtor, BIUO  >, dcx_IALUShiftCode<0b10>;
  def ToMACC : dcx_BinaryInstr<"", 1, dcx_IALUShiftOprtor, MACCT >, dcx_IALUShiftCode<0b01>;
}
defm dcx_IALU :      dcx_IALUASToDest;
defm dcx_IALULogic : dcx_IALULogicToDest;
defm dcx_IALUCom1 :  dcx_IALUCom1ToDest;
defm dcx_IALUCom2 :  dcx_IALUCom2ToDest;
defm dcx_IALUShift : dcx_IALUShiftToDest;
// ducx end 20160805--------------------------------------------------------------------------

class IALUUryCode<bits<2> dest> : IALUBinCode<dest> {
  let tn = 0b0000;
}
multiclass IALUUryToDest {
  let isCodeGenOnly = 1 in {
  def ToM    : UnaryInstr<"", IALUUryOprtor, MRegO >, IALUUryCode<0b00>;
  def ToSHU  : UnaryInstr<"", IALUUryOprtor, SHUT  >, IALUUryCode<0b01>;
  def ToBIU  : UnaryInstr<"", IALUUryOprtor, BIUO  >, IALUUryCode<0b10>;
  def ToMACC : UnaryInstr<"", IALUUryOprtor, MACCTI>, IALUUryCode<0b11>;
  }
}
class IALUImmInstr<Operand DestTy, bits<2> dest>
        : AssignInstr<(ins IALUImmOprtor: $op, TPort: $tm, imm32i: $imm, Flags: $f),
                      "", "$tm $op $imm$f", DestTy>, IALUBaseCode {
  bits<2>  tm;
  bits<5>  imm;
  bits<32> f;
  bit      op;
  let Inst{30-27} = 0b0110;
  let Inst{26}    = op;
  let Inst{25-24} = c;
  let Inst{23-22} = 0b01;
  let Inst{21-18} = f{3-0};
  let Inst{17-16} = tm;
  let Inst{15-11} = imm;
  let Inst{10-2}  = d;
  let Inst{1-0}   = dest;
}
multiclass IALUImmToDest {
  def ToM    : IALUImmInstr<MRegO , 0b00>;
  def ToSHU  : IALUImmInstr<SHUT  , 0b01>;
  def ToBIU  : IALUImmInstr<BIUO  , 0b10>;
  def ToMACC : IALUImmInstr<MACCTI, 0b11>;
}
class DIVRInstr<Operand DestTy, bits<2> dest>
        : AssignInstr<(ins), "", "DIVR", DestTy>, IALUCode<dest> {
  let tm = 0;
  let tn = 0;
  let tp = 0;
  let f  = 0;
  let op = 0b00011100;
}
multiclass DivRToDest {
  def ToM    : DIVRInstr<MRegO , 0b00>;
  def ToSHU  : DIVRInstr<SHUT  , 0b01>;
  def ToBIU  : DIVRInstr<BIUO  , 0b10>;
  def ToMACC : DIVRInstr<MACCTI, 0b11>;
}

def IALUDivR : CommonAssignInstr<(ins DIVOprtor: $op), (outs Condition: $c),
                                 "", "DIVR$c", "->", "$op">,
               IALUBaseCode {
  bits<2> op;
  bits<2> c;
  let Inst{30-28} = 0b110;
  let Inst{27-26} = op;
  let Inst{25-24} = c;
  let Inst{23-0}  = 0;
}
def IALUDivS : CommonAssignInstr<(ins DIVOprtor: $op, TPort: $tm, TPort: $tn, Flags: $f),
                                 (outs Condition: $c),
                                 "", "DIVR$c", "->", "$tm $op $tn$f">,
               IALUBaseCode {
  bits<2> op;
  bits<2> tm;
  bits<2> tn;
  bits<2>  c;
  bits<32> f;
  let Inst{30-26} = 0b11000;
  let Inst{25-24} = c;
  let Inst{23-22} = 0;
  let Inst{21-18} = f{3-0};
  let Inst{17-16} = tm;
  let Inst{15-14} = tn;
  let Inst{13-12} = op;
  let Inst{11-0}  = 0;
}
def IALUToCond : CommonAssignInstr<(ins IALUBinOprtor: $op, TPort: $tm, TPort: $tn, Flags: $f),
                                   (outs Condition: $c),
                                   "", "Cond$c", "->", "$tm $op $tn$f">,
                 IALUBaseCode {
  bits<5> op;
  bits<2> tm;
  bits<2> tn;
  bits<2>  c;
  bits<32> f;
  let Inst{30-26} = op;
  let Inst{25-24} = c;
  let Inst{23-22} = 0b10;
  let Inst{21-18} = f{3-0};
  let Inst{17-16} = tm;
  let Inst{15-14} = tn;
  let Inst{13-0}  = 0;
}

class IALUCtrlCode<bit ctrl> : IALUBaseCode{
  let Inst{30-27} = 0b1111;
  let Inst{26}    = ctrl;
  let Inst{25-0}  = 0;
}

defm IALUTP  : IALUTPToDest;
defm IALU    : IALUBinToDest;
defm IALUUry : IALUUryToDest;
defm IALUImm : IALUImmToDest;
defm IALUDivR: DivRToDest;
def IALUStart : CtrlInstr<"IALU", "Start">, IALUCtrlCode<0>;
def IALUStop : CtrlInstr<"IALU", "Stop">, IALUCtrlCode<1>;

class FALUBaseCode {
  field bits<48> Inst;
  let Inst{47-44} = 0b1000;
}
class FALUCode<bits<2> dest> : FALUBaseCode {
  bits<4> tm;
  bits<4> tn;
  bits<2> tp;
  bits<8> op;
  bits<32> f;
  bits<9>  d;
  bits<2>  c;
  let Inst{27-26} = tp;
  let Inst{25-21} = op{4-0};
  let Inst{20-19} = c;
  let Inst{18-15} = f{3-0};
  let Inst{14-13} = tm{1-0};
  let Inst{12-11} = tn{1-0};
  let Inst{10-2}  = d;
  let Inst{1-0}   = dest;
}
multiclass FALUToDest {
  def BinToM    : BinaryInstr<"", 1, FALUBinOprtor, MRegO >, FALUCode<0b00>;
  def BinToSHU  : BinaryInstr<"", 1, FALUBinOprtor, SHUT  >, FALUCode<0b01>;
  def BinToBIU  : BinaryInstr<"", 1, FALUBinOprtor, BIUO  >, FALUCode<0b10>;
  def BinToMACC : BinaryInstr<"", 1, FALUBinOprtor, MACCTI>, FALUCode<0b11>;
  let isCodeGenOnly = 1 in {
  def DParaToM    : BinaryInstr<"", 0, FALUDPOprtor, MRegO >, FALUCode<0b00>;
  def DParaToSHU  : BinaryInstr<"", 0, FALUDPOprtor, SHUT  >, FALUCode<0b01>;
  def DParaToBIU  : BinaryInstr<"", 0, FALUDPOprtor, BIUO  >, FALUCode<0b10>;
  def DParaToMACC : BinaryInstr<"", 0, FALUDPOprtor, MACCTI>, FALUCode<0b11>;
  }
  let isCodeGenOnly = 1 in {
  def UryToM    : UnaryInstr<"", FALUUryOprtor, MRegO >, FALUCode<0b00>;
  def UryToSHU  : UnaryInstr<"", FALUUryOprtor, SHUT  >, FALUCode<0b01>;
  def UryToBIU  : UnaryInstr<"", FALUUryOprtor, BIUO  >, FALUCode<0b10>;
  def UryToMACC : UnaryInstr<"", FALUUryOprtor, MACCTI>, FALUCode<0b11>;
  }
}

class FALUCtrlCode<bit ctrl> : FALUBaseCode{
  let Inst{25-22} = 0b1111;
  let Inst{21}    = ctrl;
  let Inst{20-0}  = 0;
}

defm FALU : FALUToDest;
def FALUStart : CtrlInstr<"FALU", "Start">, FALUCtrlCode<0>;
def FALUStop : CtrlInstr<"FALU", "Stop">, FALUCtrlCode<1>;

class IMACBaseCode {
  field bits<48> Inst;
  let Inst{47-44} = 0b0111;
}
class IMACCode<bits<2> dest> : IMACBaseCode {
  bits<4> tm;
  bits<4> tn;
  bits<2> tp;
  bits<8> op;
  bits<32> f;
  bits<9>  d;
  bits<2>  c;
  bits<4> opc;
  let Inst{28-25} = opc;
  let Inst{24-23} = c;
  let Inst{22-17} = f{5-0};
  let Inst{16-15} = tm{1-0};
  let Inst{14-13} = tn{1-0};
  let Inst{12-11} = tp;
  let Inst{10-2}  = d;
  let Inst{1-0}   = dest;
}

class IMACMulCode<bits<2> dest> : IMACCode<dest> {
  let opc{3-2} = 0b00;
  let opc{1-0} = op{1-0};
}
multiclass IMACToDest {
  def MASToM    : TParaInstr<"", 0, "$tp +-", IMACMulOprtor, MRegO >, IMACMulCode<0b00>;
  def MASToSHU  : TParaInstr<"", 0, "$tp +-", IMACMulOprtor, SHUT  >, IMACMulCode<0b01>;
  def MASToBIU  : TParaInstr<"", 0, "$tp +-", IMACMulOprtor, BIUO  >, IMACMulCode<0b10>;
  def MASToMACC : TParaInstr<"", 0, "$tp +-", IMACMulOprtor, MACCTI>, IMACMulCode<0b11>;
  let isCodeGenOnly = 1 in {
  def MulToM    : BinaryInstr<"", 1, IMACMulOprtor, MRegO >, IMACMulCode<0b00>;
  def MulToSHU  : BinaryInstr<"", 1, IMACMulOprtor, SHUT  >, IMACMulCode<0b01>;
  def MulToBIU  : BinaryInstr<"", 1, IMACMulOprtor, BIUO  >, IMACMulCode<0b10>;
  def MulToMACC : BinaryInstr<"", 1, IMACMulOprtor, MACCTI>, IMACMulCode<0b11>;
  }
}

class MAInstr
        : ToMRwoMovInstr<(ins TPort: $tm, TPort: $tn, TPort: $tp, Flags: $f),
                         "<-", "$tp + $tm * $tn$f"> {
  bits<4> tm;
  bits<4> tn;
  bits<2> tp;
  bits<32> f;
}

class MAwMovInstr<Operand DestTy>
        : ToMRwMovInstr<(ins TPort: $tm, TPort: $tn, TPort: $tp, Flags: $f),
                        "<-", "$tp + $tm * $tn$f", DestTy> {
  bits<4> tm;
  bits<4> tn;
  bits<2> tp;
  bits<32> f;
}

class MaCInstr
        : ToMRwoMovInstr<(ins TPort: $tm, TPort: $tn, Flags: $f),
                         "+=", "$tm * $tn$f"> {
  bits<4> tm;
  bits<4> tn;
  bits<32> f;
}

class MaCwMovInstr<Operand DestTy>
        : ToMRwMovInstr<(ins TPort: $tm, TPort: $tn, Flags: $f),
                        "+=", "$tm * $tn$f", DestTy> {
  bits<4> tm;
  bits<4> tn;
  bits<32> f;
}

class AccInstr
        : ToMRwoMovInstr<(ins TPort: $tm, Flags: $f),
                         "+=", "$tm$f"> {
  bits<4> tm;
  bits<32> f;
}

class AccwMovInstr<Operand DestTy>
        : ToMRwMovInstr<(ins TPort: $tm, Flags: $f),
                        "+=", "$tm$f", DestTy> {
  bits<4> tm;
  bits<32> f;
}

class RdMRInstr<Operand DestTy>
        : ToMRwMovInstr<(ins), "", "", DestTy>;
        
multiclass IMAToDest {
  let opc = 0b1101 in {
  def ToM    : MAwMovInstr<MRegO >, IMACCode<0b00>;
  def ToSHU  : MAwMovInstr<SHUT  >, IMACCode<0b01>;
  def ToBIU  : MAwMovInstr<BIUO  >, IMACCode<0b10>;
  def ToMACC : MAwMovInstr<MACCTI>, IMACCode<0b11>;
  }
}
multiclass IMaCToDest {
  let opc = 0b1010, tp = 0b00 in {
  def ToM    : MaCwMovInstr<MRegO >, IMACCode<0b00>;
  def ToSHU  : MaCwMovInstr<SHUT  >, IMACCode<0b01>;
  def ToBIU  : MaCwMovInstr<BIUO  >, IMACCode<0b10>;
  def ToMACC : MaCwMovInstr<MACCTI>, IMACCode<0b11>;
  }
}
multiclass IAccToDest {
  let opc = 0b1100, tn = 0b0000, tp = 0b00 in {
  def ToM    : AccwMovInstr<MRegO >, IMACCode<0b00>;
  def ToSHU  : AccwMovInstr<SHUT  >, IMACCode<0b01>;
  def ToBIU  : AccwMovInstr<BIUO  >, IMACCode<0b10>;
  def ToMACC : AccwMovInstr<MACCTI>, IMACCode<0b11>;
  }
}
multiclass IRdMRToDest {
  let opc = 0b1010, tm = 0b0000, tn = 0b0000, tp = 0b00 in {
  def ToM    : RdMRInstr<MRegO >, IMACCode<0b00>;
  def ToSHU  : RdMRInstr<SHUT  >, IMACCode<0b01>;
  def ToBIU  : RdMRInstr<BIUO  >, IMACCode<0b10>;
  def ToMACC : RdMRInstr<MACCTI>, IMACCode<0b11>;
  }
}

def IMA : MAInstr, IMACCode<0b00> {
  let opc = 0b1000;
  let d  = 0;
}
def IMaC : MaCInstr, IMACCode<0b00> {
  let opc = 0b1001;
  let d  = 0;
  let tp = 0;
}
def IMAcc : AccInstr, IMACCode<0b00> {
  let opc = 0b1011;
  let d  = 0;
  let tn = 0;
  let tp = 0;
}
class IMACCtrlCode<bit ctrl> : IMACBaseCode{
  let Inst{28-26} = 0b111;
  let Inst{25}    = ctrl;
  let Inst{24-0}  = 0;
}

defm I    : IMACToDest;
defm IMA  : IMAToDest;
defm IMaC : IMaCToDest;
defm IMAcc: IAccToDest;
defm IMR  : IRdMRToDest;
def IMACStart : CtrlInstr<"IMAC", "Start">, IMACCtrlCode<0>;
def IMACStop : CtrlInstr<"IMAC", "Stop">, IMACCtrlCode<1>;

class FMACBaseCode {
  field bits<48> Inst;
  let Inst{47-44} = 0b1001;
}

class FMACCode<bits<2> dest> : FMACBaseCode{
  bits<4> tm;
  bits<4> tn;
  bits<2> tp;
  bits<32> f;
  bits<8> op;
  bits<9>  d;
  bits<2>  c;
  bits<3> opc;
  let Inst{23-21} = opc;
  let Inst{20-19} = c;
  let Inst{18-17} = f{1-0};
  let Inst{16-15} = tm{1-0};
  let Inst{14-13} = tn{1-0};
  let Inst{12-11} = tp;
  let Inst{10-2}  = d;
  let Inst{1-0}   = dest;
}
class FMACMulCode<bits<2> dest> : FMACCode<dest> {
  let opc{2} = op{0};
  let opc{1-0} = 0b01;
}
multiclass FMulToDest {
  def ToM    : BinaryInstr<"", 1, FMACMulOprtor, MRegO >, FMACMulCode<0b00>;
  def ToSHU  : BinaryInstr<"", 1, FMACMulOprtor, SHUT  >, FMACMulCode<0b01>;
  def ToBIU  : BinaryInstr<"", 1, FMACMulOprtor, BIUO  >, FMACMulCode<0b10>;
  def ToMACC : BinaryInstr<"", 1, FMACMulOprtor, MACCTI>, FMACMulCode<0b11>;
}
multiclass FMaCToDest {
  let opc = 0b100, tp = 0b00 in {
  def ToM    : MaCwMovInstr<MRegO >, FMACCode<0b00>;
  def ToSHU  : MaCwMovInstr<SHUT  >, FMACCode<0b01>;
  def ToBIU  : MaCwMovInstr<BIUO  >, FMACCode<0b10>;
  def ToMACC : MaCwMovInstr<MACCTI>, FMACCode<0b11>;
  }
}
class FMACCtrlCode<bit ctrl> : FMACBaseCode{
  let Inst{23-22} = 0b11;
  let Inst{21}    = ctrl;
  let Inst{20-0}  = 0;
}
defm FMul : FMulToDest;
def FMaC : MaCInstr, FMACCode<0b00> {
  let opc = 0b011;
  let tp = 0;
}
def FMA : MAInstr, FMACCode<0b00> {
  let opc = 0b010;
}
defm FMaC : FMaCToDest;
def FMACStart : CtrlInstr<"FMAC", "Start">, FMACCtrlCode<0>;
def FMACStop : CtrlInstr<"FMAC", "Stop">, FMACCtrlCode<1>;

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

