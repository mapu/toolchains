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
def SHU0Ind_SetTBToMACC_0 : UCPSHUIndToDest_SetTB<"SHU0.", 0, 0b0110, MACCTI> {
  let Inst{7-6} = 0b01;
  let Inst{5-0} = d;
}
//final shu0 test instruction, for case: "=+"
def SHU0Ind_SetTBToMACC_1 : UCPSHUIndToDest_SetTB<"SHU0.", 1, 0b0110, MACCTI> {
  let Inst{7-6} = 0b01;
  let Inst{5-0} = d;
}
def SHU1Ind_SetTBToMACC_0 : UCPSHUIndToDest_SetTB<"SHU1.", 0, 0b0111, MACCTI> {
  let Inst{7-6} = 0b01;
  let Inst{5-0} = d;
}
def SHU1Ind_SetTBToMACC_1 : UCPSHUIndToDest_SetTB<"SHU1.", 1, 0b0111, MACCTI> {
  let Inst{7-6} = 0b01;
  let Inst{5-0} = d;
}
def SHU2Ind_SetTBToMACC_0 : UCPSHUIndToDest_SetTB<"SHU2.", 0, 0b1000, MACCTI> {
  let Inst{7-6} = 0b01;
  let Inst{5-0} = d;
}
def SHU2Ind_SetTBToMACC_1 : UCPSHUIndToDest_SetTB<"SHU2.", 1, 0b1000, MACCTI> {
  let Inst{7-6} = 0b01;
  let Inst{5-0} = d;
}

// ducx start IALU Instruction --------------------------------------------------------------------
class IALUBaseCode {
  field bits<48> Inst;
  let Inst{47-44} = 0b0110;//?
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
                      "", "$tm $op $imm$f", DestTy>, IALUBaseCode {
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
  let Inst{47-44} = 0b0111;//?
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

defm IFALU : IFALUComToDest;
defm IFALUUryTran : IFALUUryTranToDest;

// ducx end IFALU Instruction --------------------------------------------------------------------------


// ducx start IMAC Instruction --------------------------------------------------------------------------
class IMACBaseCode {
  field bits<48> Inst;
  let Inst{47-44} = 0b1000;//?
}

class IMACCode<bits<2> dest> : IMACBaseCode {
  bits<8> op;
  bits<2> tm;
  bits<2> tn;
  bits<2> tp;
  bits<4>  d;
  bits<32> f;
  bits<2> shift;
  bits<2> opc;
  
  let Inst{26-25} = opc;
  let Inst{23-21} = f{7-5};
  let Inst{19-17} = f{4-2};
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

// tm*tn -> d
multiclass IMulRealToDest{
    let opc = 0b00 in{
    def ToSHU  : BinaryInstr<"", 1, IMACMulOprtor, SHUT >, IMACRealCode<0b00>;
    def ToBIU  : BinaryInstr<"", 1, IMACMulOprtor, BIUO >, IMACRealCode<0b10>;
    def ToMACC : BinaryInstr<"", 1, IMACMulOprtor, MACCT>, IMACRealCode<0b01>;
    }
}
multiclass IMulCompToDest{
    let opc = 0b00 in{
    def ToSHU  : BinaryInstr<"", 1, IMACMulOprtor, SHUT >, IMACCompCode<0b00>;
    def ToBIU  : BinaryInstr<"", 1, IMACMulOprtor, BIUO >, IMACCompCode<0b10>;
    def ToMACC : BinaryInstr<"", 1, IMACMulOprtor, MACCT>, IMACCompCode<0b01>;
    }
}

// MR <- tp +|- tm*tn
class MAInstr<bit addsub>
        : ToMRwoMovInstr<(ins TPort: $tm, TPort: $tn, TPort: $tp, Flags: $f , Flags: $shift),
                            "<-", !if(addsub, "$tp + $tm * $tn$f", "$tp - $tm * $tn$f")>{
    bits<2> tm;
    bits<2> tn;
    bits<2> tp;    
    bits<32> f;
    bits<2> shift;
    
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
        : ToMRwMovInstr<(ins TPort: $tm, TPort: $tn, TPort: $tp, Flags: $f , Flags: $shift), 
                "<-", !if(addsub, "$tp + $tm * $tn$f", "$tp - $tm * $tn$f"), DestTy>{
    bits<2> tm;
    bits<2> tn;
    bits<2> tp;    
    bits<32> f;
    bits<2> shift;
  
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
        : ToMRwoMovInstr<(ins TPort: $tm, TPort: $tn, TPort: $tp, Flags: $f , Flags: $shift),
                            "<-", "$tp +- $tm * $tn$f">{
    bits<2> tm;
    bits<2> tn;
    bits<2> tp;    
    bits<32> f;
    bits<2> shift;
    
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
        : ToMRwMovInstr<(ins TPort: $tm, TPort: $tn, TPort: $tp, Flags: $f , Flags: $shift), 
                "<-", "$tp +- $tm * $tn$f", DestTy>{
    bits<2> tm;
    bits<2> tn;
    bits<2> tp;    
    bits<32> f;
    bits<2> shift;
                           
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
        : ToMRwoMovInstr<(ins TPort: $tm, TPort: $tn, Flags: $f , Flags: $shift),
                            !if(addsub, "+=", "-="), "$tm * $tn$f">{
    bits<2> tm;
    bits<2> tn;
    bits<32> f;
    bits<2> shift;
     
    let Inst{24} = addsub;
    let Inst{12} = 1;
    let Inst{11} = f{1};
}
def IMaC_Realadd : MaCInstr<0>, IMACRealCode<0b00>{
    let opc = 0b10;
    let d = 0;
}
def IMaC_Realsub : MaCInstr<1>, IMACRealCode<0b00>{
    let opc = 0b10;
    let d = 0;
}
def IMaC_Compadd : MaCInstr<0>, IMACCompCode<0b00>{
    let opc = 0b10;
    let d = 0;
}
def IMaC_Compsub : MaCInstr<1>, IMACCompCode<0b00>{
    let opc = 0b10;
    let d = 0;
}

// MR +=|-= tm*tn -> d
class MaCMovInstr<bit addsub, Operand DestTy>
        : ToMRwMovInstr<(ins TPort: $tm, TPort: $tn, Flags: $f , Flags: $shift),
                          !if(addsub, "+=", "-="), "$tm * $tn$f", DestTy>{
    bits<2> tm;
    bits<2> tn;
    bits<32> f;
    bits<2> shift;
    
    let Inst{24} = addsub;
    let Inst{12} = 1;
    let Inst{11} = f{1};
}
multiclass IMaCRealaddToDest{
    let opc = 0b10 in{
    def ToSHU  : MaCMovInstr<0,SHUT  >, IMACRealCode<0b00>;
    def ToBIU  : MaCMovInstr<0,BIUO  >, IMACRealCode<0b10>;
    def ToMACC : MaCMovInstr<0,MACCT >, IMACRealCode<0b01>;
    }
}
multiclass IMaCRealsubToDest{
    let opc = 0b10 in{
    def ToSHU  : MaCMovInstr<1,SHUT  >, IMACRealCode<0b00>;
    def ToBIU  : MaCMovInstr<1,BIUO  >, IMACRealCode<0b10>;
    def ToMACC : MaCMovInstr<1,MACCT >, IMACRealCode<0b01>;
    }
}
multiclass IMaCCompaddToDest{
    let opc = 0b10 in{
    def ToSHU  : MaCMovInstr<0,SHUT  >, IMACCompCode<0b00>;
    def ToBIU  : MaCMovInstr<0,BIUO  >, IMACCompCode<0b10>;
    def ToMACC : MaCMovInstr<0,MACCT >, IMACCompCode<0b01>;
    }
}
multiclass IMaCCompsubToDest{
    let opc = 0b10 in{
    def ToSHU  : MaCMovInstr<1,SHUT  >, IMACCompCode<0b00>;
    def ToBIU  : MaCMovInstr<1,BIUO  >, IMACCompCode<0b10>;
    def ToMACC : MaCMovInstr<1,MACCT >, IMACCompCode<0b01>;
    }
}

// MR +=|-= tm
class AccInstr<bit addsub>
        : ToMRwoMovInstr<(ins TPort: $tm, Flags: $f , Flags: $shift),
                            !if(addsub, "+=", "-="), "$tm$f">{
    bits<2> tm;
    bits<32> f;
    bits<2> shift;
     
    let Inst{24} = addsub;
    let Inst{12} = 0;
    let Inst{11} = f{1};
}
def IAcc_Realadd : AccInstr<0>, IMACRealCode<0b00>{
    let opc = 0b10;
    let d = 0;
    let tn = 0;
}
def IAcc_Realsub : AccInstr<1>, IMACRealCode<0b00>{
    let opc = 0b10;
    let d = 0;
    let tn = 0;
}
def IAcc_Compadd : AccInstr<0>, IMACCompCode<0b00>{
    let opc = 0b10;
    let d = 0;
    let tn = 0;
}
def IAcc_Compsub : AccInstr<1>, IMACCompCode<0b00>{
    let opc = 0b10;
    let d = 0;
    let tn = 0;
}

// MR +=|-= tm -> d 
class AccMovInstr<bit addsub, Operand DestTy>
        : ToMRwMovInstr<(ins TPort: $tm, Flags: $f , Flags: $shift),
                          !if(addsub, "+=", "-="), "$tm$f", DestTy>{
    bits<2> tm;
    bits<2> tn;
    bits<32> f;
    bits<2> shift;
    
    let Inst{24} = addsub;
    let Inst{12} = 0;
    let Inst{11} = f{1};
}
multiclass IAccRealaddToDest{
    let opc = 0b10, tn =0b00 in{
    def ToSHU  : AccMovInstr<0,SHUT  >, IMACRealCode<0b00>;
    def ToBIU  : AccMovInstr<0,BIUO  >, IMACRealCode<0b10>;
    def ToMACC : AccMovInstr<0,MACCT >, IMACRealCode<0b01>;
    }
}
multiclass IAccRealsubToDest{
    let opc = 0b10, tn =0b00 in{
    def ToSHU  : AccMovInstr<1,SHUT  >, IMACRealCode<0b00>;
    def ToBIU  : AccMovInstr<1,BIUO  >, IMACRealCode<0b10>;
    def ToMACC : AccMovInstr<1,MACCT >, IMACRealCode<0b01>;
    }
}
multiclass IAccCompaddToDest{
    let opc = 0b10, tn =0b00 in{
    def ToSHU  : AccMovInstr<0,SHUT  >, IMACCompCode<0b00>;
    def ToBIU  : AccMovInstr<0,BIUO  >, IMACCompCode<0b10>;
    def ToMACC : AccMovInstr<0,MACCT >, IMACCompCode<0b01>;
    }
}
multiclass IAccCompsubToDest{
    let opc = 0b10, tn =0b00 in{
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
  
  let Inst{26-25} = 0b10;
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
  let Inst{47-44} = 0b1001;//?
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

defm IFMAC : IFMACToDest;
// ducx end IFMAC Instruction --------------------------------------------------------------------------



// ducx start BIU Instruction --------------------------------------------------------------------------

// load: discrete data
class BIULdToDest_dis<string header, bits<4> slot, bits<2> dest, Operand DestTy>
        : BIUAssignInstr<(ins TPort: $tm, TPort: $ts, Flags: $f, Flags: $QLH), header, "$tm($ts)$f", DestTy>{

    bits<32> f;
    bits<2> tm;
    bits<2> ts;
    bits<2> QLH;
    
    let Inst{47-44} = slot;
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

multiclass BIULdInstr_dis<string header, bits<4> slot>{
    def ToSHU  : BIULdToDest_dis<header, slot ,0b00, SHUT >;
    def ToMACC : BIULdToDest_dis<header, slot ,0b01, MACCT>;
}

// load: common/step
class BIULdToDest_com<string header, bits<4> slot, bits<2> dest, Operand DestTy>
        : BIUAssignInstr<(ins TPort: $tm, TPort: $ts, Flags: $f, Flags: $step), header, "$tm($ts)$f", DestTy>{

    bits<32> f;
    bits<2> tm;
    bits<2> ts;
    bit step;

    
    let Inst{47-44} = slot;
    let Inst{20-18} = 0b111;
    let Inst{17-16} = f{2-1};
    let Inst{15-14} = 0b11;
    let Inst{13-12} = tm;
    let Inst{11} = 0;
    let Inst{10} = step;
    let Inst{9} = f{0};
    let Inst{8-7} = dest;
    let Inst{6-5}  = d{5-4};
    let Inst{4} = 0;
    let Inst{3-0}   = d{3-0};
}

multiclass BIULdInstr_com<string header, bits<4> slot>{
    def ToSHU  : BIULdToDest_com<header, slot ,0b00, SHUT >;
    def ToMACC : BIULdToDest_com<header, slot ,0b01, MACCT>;
}

// store
class BIUStInstr<string header, bits<4> slot>
        : UCPMInst<(ins TPort: $tm, TPort: $ts, Flags: $f, Flags: $SQLH), (outs), !strconcat(header,"$tm($ts)", "-> DM$f")>{
            
    bits<32> f;
    bits<2> tm;
    bits<2> ts;
    bits<2> SQLH;

    let Inst{47-44} = slot;
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
class BIUBinInstr<string header, bits<4> slot>
        : UCPMInst<(ins BIUBinOprtor: $op, TPort: $tm, TPort: $tn, TPort: $tp, imm32i: $m, imm32i: $n, imm32i: $p), (outs), !strconcat(header,"$tm $op $tn", "-> $tp")>{
            
    bits<32> f;
    bits<2> tm;
    bits<2> tn;
    bits<2> tp;
    bits<8> op;
    bits<4> m;
    bits<4> n;
    bits<4> p;

    let Inst{47-44} = slot;
    let Inst{20-18} = op{2-0};
    let Inst{17-16} = tm;
    let Inst{15-12} = m;
    let Inst{11-10} = tn;
    let Inst{9-6} = n;
    let Inst{5-4} = tp;
    let Inst{3-0} = p;
}
 
// LSH RSH
class BIUShiftInstr<string header, bits<4> slot>
        : UCPMInst<(ins BIUShiftOprtor: $op, TPort: $tm, TPort: $tp, imm32i: $imm, imm32i: $m, imm32i: $p), (outs), !strconcat(header,"$tm $op $imm", "-> $tp")>{
            
    bits<32> f;
    bits<2> tm;
    bits<2> tp;
    bits<8> op;
    bits<2> imm;
    bits<4> m;
    bits<4> p;

    let Inst{47-44} = slot;
    let Inst{20-18} = op{6-4};
    let Inst{17-16} = tm;
    let Inst{15-12} = m;
    let Inst{11-10} = imm;
    let Inst{9-6} = op{3-0};
    let Inst{5-4} = tp;
    let Inst{3-0} = p;
}

defm BIU0disLd : BIULdInstr_dis<"BIU0.", 0b1010>;
defm BIU0comLd : BIULdInstr_com<"BIU0.", 0b1010>;
def BIU0St : BIUStInstr<"BIU0.", 0b1010>;
def BIU0Bin : BIUBinInstr<"BIU0.", 0b1010>;
def BIU0Shift : BIUShiftInstr<"BIU0.", 0b1010>;

defm BIU1disLd : BIULdInstr_dis<"BIU1.", 0b1011>;
defm BIU1comLd : BIULdInstr_com<"BIU1.", 0b1011>;
def BIU1St : BIUStInstr<"BIU1.", 0b1011>;
def BIU1Bin : BIUBinInstr<"BIU1.", 0b1011>;
def BIU1Shift : BIUShiftInstr<"BIU1.", 0b1011>;

defm BIU2disLd : BIULdInstr_dis<"BIU2.", 0b1100>;
defm BIU2comLd : BIULdInstr_com<"BIU2.", 0b1100>;
def BIU2St : BIUStInstr<"BIU2.", 0b1100>;
def BIU2Bin : BIUBinInstr<"BIU2.", 0b1100>;
def BIU2Shift : BIUShiftInstr<"BIU2.", 0b1100>;

// ducx end BIU Instruction --------------------------------------------------------------------------



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