//===-- UCPSRegisterInfo.td - UCPS Register defs ---------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
//  Declarations that describe the UCPS register file
//===----------------------------------------------------------------------===//

class UCPSReg<bits<9> num, string n > : Register<n> {

  // fixme: this namespace must be the same with that for RegisterClass
  // 		definition, which is an unnecessary constraint.
  let Namespace = "UCPSReg";
  let HWEncoding = {0b0000000, num};
}

foreach i = 0-127 in 
  def M#i   : UCPSReg<i, "M["#i#"]" >, DwarfRegNum< [i] >;
def MIPP : UCPSReg<128, "M[I++]">, DwarfRegNum<[128]>;
def MSPP : UCPSReg<256, "M[S++]">, DwarfRegNum<[256]>;

def MReg : RegisterClass<"UCPSReg", [v16i32], 32,
                         (add (sequence "M%u", 0, 127), MIPP, MSPP)>;

def SHU0  : UCPSReg<0, "SHU0">, DwarfRegNum<[129]>;
def SHU1  : UCPSReg<1, "SHU1">, DwarfRegNum<[130]>;

def SHU   : RegisterClass<"UCPSReg", [v16i32], 32,
                          (add SHU0, SHU1)>;

def IALU  : UCPSReg<0, "IALU">, DwarfRegNum<[134]>;
def IMAC  : UCPSReg<1, "IMAC">, DwarfRegNum<[135]>;
def FALU  : UCPSReg<2, "FALU">, DwarfRegNum<[136]>;
def FMAC  : UCPSReg<3, "FMAC">, DwarfRegNum<[137]>;

def MACC : RegisterClass<"UCPSReg", [v16i32], 32,
			 (add IALU, IMAC, FALU, FMAC)>;

def T0  : UCPSReg<0, "T0">, DwarfRegNum<[138]>;
def T1  : UCPSReg<1, "T1">, DwarfRegNum<[139]>;
def T2  : UCPSReg<2, "T2">, DwarfRegNum<[140]>;
def T3  : UCPSReg<3, "T3">, DwarfRegNum<[141]>;
def VT  : UCPSReg<0, "VT">, DwarfRegNum<[138]>;

def TPort : RegisterClass<"UCPSReg", [v16i32], 32,
                          (sequence "T%u", 0, 3)>;

def TBB : UCPSReg<4,  "TBB">;
def TBH : UCPSReg<5,  "TBH">;
def TBW : UCPSReg<6,  "TBW">;
def TBD : UCPSReg<7,  "TBD">;
def TSQ : UCPSReg<8,  "TSQ">;

def TEPort : RegisterClass<"UCPSReg", [v16i32], 32,
                           (add (sequence "T%u", 0, 3), TBB, TBH, TBW, TBD, TSQ)>;

def BIU0  : UCPSReg<0, "BIU0">, DwarfRegNum<[142]>;
def BIU1  : UCPSReg<1, "BIU1">, DwarfRegNum<[143]>;
def BIU2  : UCPSReg<2, "BIU2">, DwarfRegNum<[144]>;

def BIU   : RegisterClass<"UCPSReg", [v16i32], 32,
                          (sequence "BIU%u", 0, 2)>;

def COMB : UCPSReg<0, "COMB">;

def SHUOprtor : RegisterClass<"UCPSReg", [i32], 32, (add COMB)>;

def MERGE : UCPSReg<6, "MERGE">;

def IALUTPOprtor : RegisterClass<"UCPSReg", [i32], 32, (add MERGE)>;

def IADD : UCPSReg<0b000000001, "+">;
def ISUB : UCPSReg<0b000000010, "-">;
def AND  : UCPSReg<0b000001000, "&">;
def OR   : UCPSReg<0b000001001, "|">;
def XOR  : UCPSReg<0b000001010, "^">;
def LSHT : UCPSReg<0b000001100, "<<">;
def RSHT : UCPSReg<0b000001101, ">>">;
def IEQU : UCPSReg<0b000010000, "==">;
def INEQ : UCPSReg<0b000010001, "!=">;
def ILT  : UCPSReg<0b000010010, ">">;
def INST : UCPSReg<0b000010011, ">=">;
def IST  : UCPSReg<0b000010100, "<">;
def INLT : UCPSReg<0b000010101, "<=">;

def IALUBinOprtor : RegisterClass<"UCPSReg", [i32], 32,
                               (add IADD, ISUB, AND,  OR,   XOR,
                                    LSHT, RSHT, IEQU, INEQ, ILT,
                                    INST, IST, INLT)>;

def IMAX : UCPSReg<0b000000011, "MAX">;
def IMIN : UCPSReg<0b000000100, "MIN">;
def IABS : UCPSReg<0b000000101, "ABS">;
def CPRS : UCPSReg<0b000001110, "CPRS">;

def IALUDPOprtor : RegisterClass<"UCPSReg", [i32], 32,
                                 (add IMAX, IMIN, IABS, CPRS)>;

def LSHTI : UCPSReg<0, "<<">;
def RSHTI : UCPSReg<1, ">>">;

def IALUImmOprtor : RegisterClass<"UCPSReg", [i32], 32,
                                  (add LSHTI, RSHTI)>;

def EXPD : UCPSReg<0b000001111, "EXPD">;
def NOT  : UCPSReg<0b000001011, "NOT">;
def RMAX : UCPSReg<0b000010110, "RMAX">;
def RMIN : UCPSReg<0b000010111, "RMIN">;

def IALUUryOprtor : RegisterClass<"UCPSReg", [i32], 32,
                                  (add EXPD, NOT, RMAX, RMIN)>;

def DIVS : UCPSReg<0, "DIVS">;
def DIVQ : UCPSReg<1, "DIVQ">;
def MDIVR : UCPSReg<2, "MDIVR">;
def MDIVQ : UCPSReg<3, "MDIVQ">;

def DIVOprtor : RegisterClass<"UCPSReg", [i32], 32,
                              (add DIVS, DIVQ, MDIVR, MDIVQ)>;

def FADD : UCPSReg<0b000000001, "+">;
def FSUB : UCPSReg<0b000000010, "-">;
def ADDSUB : UCPSReg<0b000000110, "+-">;
def FEQU : UCPSReg<0b000001000, "==">;
def FNEQ : UCPSReg<0b000001001, "!=">;
def FLT  : UCPSReg<0b000001010, ">">;
def FNST : UCPSReg<0b000001011, ">=">;
def FST  : UCPSReg<0b000001100, "<">;
def FNLT : UCPSReg<0b000001101, "<=">;

def FALUBinOprtor : RegisterClass<"UCPSReg", [i32], 32,
                                  (add FADD, FSUB, ADDSUB, FEQU, FNEQ,
                                       FLT,  FNST, FST,    FNLT)>;

def FMAX : UCPSReg<0b000000011, "MAX">;
def FMIN : UCPSReg<0b000000100, "MIN">;

def FALUDPOprtor : RegisterClass<"UCPSReg", [i32], 32,
                                 (add FMAX, FMIN)>;

def FABS : UCPSReg<0b000000101, "ABS">;
def SINGLE : UCPSReg<0b000010000, "SINGLE">;
def DOUBLE : UCPSReg<0b000010001, "DOUBLE">;
def INT  : UCPSReg<0b000010010, "INT">;
def UINT : UCPSReg<0b000010011, "UINT">;
def RECIP : UCPSReg<0b000011000, "RECIP">;
def RSQRT : UCPSReg<0b000011001, "RSQRT">;

def FALUUryOprtor : RegisterClass<"UCPSReg", [i32], 32,
                                 (add FABS,  SINGLE, DOUBLE, INT, UINT,
                                      RECIP, RSQRT)>;

def IMUL : UCPSReg<2, "*">;
def IMULT : UCPSReg<1, "*">;
def IMULS : UCPSReg<3, "*">;
def MR : UCPSReg<0, "MR">;

def IMACMulOprtor : RegisterClass<"UCPSReg", [i32], 32,
                                  (add IMUL, IMULT, IMULS)>;

def FMUL : UCPSReg<0, "*">;
def CMUL : UCPSReg<1, "**">;

def FMACMulOprtor : RegisterClass<"UCPSReg", [i32], 32,
                                  (add FMUL, CMUL)>;

def C : UCPSReg<2, "@(C)">;
def NC : UCPSReg<3, "@(!C)">;
def NOC : UCPSReg<0, " ">;
/* 
class RegisterClass<string namespace, list<ValueType> regTypes, int alignment,
                    dag regList, RegAltNameIndex idx = NoRegAltName>
*/

def Condition : RegisterClass<"UCPSReg", [i32], 32, (add C, NC, NOC)>;

foreach i = 1-3 in 
  def WF#i   : UCPSReg<i, "(WF"#i#")" >;
  
foreach i = 1-4 in 
  def WX#i   : UCPSReg<!add(i, 3), "(WX"#i#")" >;
  

def WF0   : UCPSReg<0, " " >;
def WFlag : RegisterClass<"UCPSReg", [i32], 32,
                          (add (sequence "WF%u", 0, 3),
                           (sequence "WX%u", 1, 4))>;
