//===-- UCPMRegisterInfo.td - UCPM Register defs ---------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
//  Declarations that describe the UCPM register file
//===----------------------------------------------------------------------===//

class UCPMReg<bits<9> num, string n > : Register<n> {

  // fixme: this namespace must be the same with that for RegisterClass
  // 		definition, which is an unnecessary constraint.
  let Namespace = "UCPMReg";
  let HWEncoding = {0b0000000, num};
}

foreach i = 0-127 in 
  def M#i   : UCPMReg<i, "M["#i#"]" >, DwarfRegNum< [i] >;
def MIPP : UCPMReg<128, "M[I++]">, DwarfRegNum<[128]>;
def MSPP : UCPMReg<256, "M[S++]">, DwarfRegNum<[256]>;

def MReg : RegisterClass<"UCPMReg", [v16i32], 32,
                         (add (sequence "M%u", 0, 127), MIPP, MSPP)>;

def SHU0  : UCPMReg<0, "SHU0">, DwarfRegNum<[129]>;
def SHU1  : UCPMReg<1, "SHU1">, DwarfRegNum<[130]>;

def SHU   : RegisterClass<"UCPMReg", [v16i32], 32,
                          (add SHU0, SHU1)>;

def IALU  : UCPMReg<0, "IALU">, DwarfRegNum<[134]>;
def IMAC  : UCPMReg<1, "IMAC">, DwarfRegNum<[135]>;
def FALU  : UCPMReg<2, "FALU">, DwarfRegNum<[136]>;
def FMAC  : UCPMReg<3, "FMAC">, DwarfRegNum<[137]>;

def MACC : RegisterClass<"UCPMReg", [v16i32], 32,
			 (add IALU, IMAC, FALU, FMAC)>;

def T0  : UCPMReg<0, "T0">, DwarfRegNum<[138]>;
def T1  : UCPMReg<1, "T1">, DwarfRegNum<[139]>;
def T2  : UCPMReg<2, "T2">, DwarfRegNum<[140]>;
def T3  : UCPMReg<3, "T3">, DwarfRegNum<[141]>;
def VT  : UCPMReg<0, "VT">, DwarfRegNum<[138]>;

def TPort : RegisterClass<"UCPMReg", [v16i32], 32,
                          (sequence "T%u", 0, 3)>;

def TBB : UCPMReg<4,  "TBB">;
def TBH : UCPMReg<5,  "TBH">;
def TBW : UCPMReg<6,  "TBW">;
def TBD : UCPMReg<7,  "TBD">;
def TSQ : UCPMReg<8,  "TSQ">;

def TEPort : RegisterClass<"UCPMReg", [v16i32], 32,
                           (add (sequence "T%u", 0, 3), TBB, TBH, TBW, TBD, TSQ)>;

def BIU0  : UCPMReg<0, "BIU0">, DwarfRegNum<[142]>;
def BIU1  : UCPMReg<1, "BIU1">, DwarfRegNum<[143]>;
def BIU2  : UCPMReg<2, "BIU2">, DwarfRegNum<[144]>;

def BIU   : RegisterClass<"UCPMReg", [v16i32], 32,
                          (sequence "BIU%u", 0, 2)>;

def COMB : UCPMReg<0, "COMB">;

def SHUOprtor : RegisterClass<"UCPMReg", [i32], 32, (add COMB)>;

def MERGE : UCPMReg<6, "MERGE">;

def IALUTPOprtor : RegisterClass<"UCPMReg", [i32], 32, (add MERGE)>;

def IADD : UCPMReg<0b000000001, "+">;
def ISUB : UCPMReg<0b000000010, "-">;
def AND  : UCPMReg<0b000001000, "&">;
def OR   : UCPMReg<0b000001001, "|">;
def XOR  : UCPMReg<0b000001010, "^">;
def LSHT : UCPMReg<0b000001100, "<<">;
def RSHT : UCPMReg<0b000001101, ">>">;
def IEQU : UCPMReg<0b000010000, "==">;
def INEQ : UCPMReg<0b000010001, "!=">;
def ILT  : UCPMReg<0b000010010, ">">;
def INST : UCPMReg<0b000010011, ">=">;
def IST  : UCPMReg<0b000010100, "<">;
def INLT : UCPMReg<0b000010101, "<=">;

def IALUBinOprtor : RegisterClass<"UCPMReg", [i32], 32,
                               (add IADD, ISUB, AND,  OR,   XOR,
                                    LSHT, RSHT, IEQU, INEQ, ILT,
                                    INST, IST, INLT)>;

def IMAX : UCPMReg<0b000000011, "MAX">;
def IMIN : UCPMReg<0b000000100, "MIN">;
def IABS : UCPMReg<0b000000101, "ABS">;
def CPRS : UCPMReg<0b000001110, "CPRS">;

def IALUDPOprtor : RegisterClass<"UCPMReg", [i32], 32,
                                 (add IMAX, IMIN, IABS, CPRS)>;

def LSHTI : UCPMReg<0, "<<">;
def RSHTI : UCPMReg<1, ">>">;

def IALUImmOprtor : RegisterClass<"UCPMReg", [i32], 32,
                                  (add LSHTI, RSHTI)>;

def EXPD : UCPMReg<0b000001111, "EXPD">;
def NOT  : UCPMReg<0b000001011, "NOT">;
def RMAX : UCPMReg<0b000010110, "RMAX">;
def RMIN : UCPMReg<0b000010111, "RMIN">;

def IALUUryOprtor : RegisterClass<"UCPMReg", [i32], 32,
                                  (add EXPD, NOT, RMAX, RMIN)>;

def DIVS : UCPMReg<0, "DIVS">;
def DIVQ : UCPMReg<1, "DIVQ">;
def MDIVR : UCPMReg<2, "MDIVR">;
def MDIVQ : UCPMReg<3, "MDIVQ">;

def DIVOprtor : RegisterClass<"UCPMReg", [i32], 32,
                              (add DIVS, DIVQ, MDIVR, MDIVQ)>;

def FADD : UCPMReg<0b000000001, "+">;
def FSUB : UCPMReg<0b000000010, "-">;
def ADDSUB : UCPMReg<0b000000110, "+-">;
def FEQU : UCPMReg<0b000001000, "==">;
def FNEQ : UCPMReg<0b000001001, "!=">;
def FLT  : UCPMReg<0b000001010, ">">;
def FNST : UCPMReg<0b000001011, ">=">;
def FST  : UCPMReg<0b000001100, "<">;
def FNLT : UCPMReg<0b000001101, "<=">;

def FALUBinOprtor : RegisterClass<"UCPMReg", [i32], 32,
                                  (add FADD, FSUB, ADDSUB, FEQU, FNEQ,
                                       FLT,  FNST, FST,    FNLT)>;

def FMAX : UCPMReg<0b000000011, "MAX">;
def FMIN : UCPMReg<0b000000100, "MIN">;

def FALUDPOprtor : RegisterClass<"UCPMReg", [i32], 32,
                                 (add FMAX, FMIN)>;

def FABS : UCPMReg<0b000000101, "ABS">;
def SINGLE : UCPMReg<0b000010000, "SINGLE">;
def DOUBLE : UCPMReg<0b000010001, "DOUBLE">;
def INT  : UCPMReg<0b000010010, "INT">;
def UINT : UCPMReg<0b000010011, "UINT">;
def RECIP : UCPMReg<0b000011000, "RECIP">;
def RSQRT : UCPMReg<0b000011001, "RSQRT">;

def FALUUryOprtor : RegisterClass<"UCPMReg", [i32], 32,
                                 (add FABS,  SINGLE, DOUBLE, INT, UINT,
                                      RECIP, RSQRT)>;

def IMUL : UCPMReg<2, "*">;
def IMULT : UCPMReg<1, "*">;
def IMULS : UCPMReg<3, "*">;
def MR : UCPMReg<0, "MR">;

def IMACMulOprtor : RegisterClass<"UCPMReg", [i32], 32,
                                  (add IMUL, IMULT, IMULS)>;

def FMUL : UCPMReg<0, "*">;
def CMUL : UCPMReg<1, "**">;

def FMACMulOprtor : RegisterClass<"UCPMReg", [i32], 32,
                                  (add FMUL, CMUL)>;

def C : UCPMReg<2, "@(C)">;
def NC : UCPMReg<3, "@(!C)">;
def NOC : UCPMReg<0, " ">;
/* 
class RegisterClass<string namespace, list<ValueType> regTypes, int alignment,
                    dag regList, RegAltNameIndex idx = NoRegAltName>
*/

def Condition : RegisterClass<"UCPMReg", [i32], 32, (add C, NC, NOC)>;

foreach i = 1-3 in 
  def WF#i   : UCPMReg<i, "(WF"#i#")" >;
  
foreach i = 1-4 in 
  def WX#i   : UCPMReg<!add(i, 3), "(WX"#i#")" >;
  

def WF0   : UCPMReg<0, " " >;
def WFlag : RegisterClass<"UCPMReg", [i32], 32,
                          (add (sequence "WF%u", 0, 3),
                           (sequence "WX%u", 1, 4))>;
