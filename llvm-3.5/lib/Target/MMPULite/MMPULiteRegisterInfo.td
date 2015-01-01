//===-- MMPULiteRegisterInfo.td - MMPULite Register defs ---------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
//  Declarations that describe the MMPULite register file
//===----------------------------------------------------------------------===//

class MMPULiteReg<bits<9> num, string n > : Register<n> {

  // fixme: this namespace must be the same with that for RegisterClass
  // 		definition, which is an unnecessary constraint.
  let Namespace = "MMPULiteReg";
  let HWEncoding = {0b0000000, num};
}

foreach i = 0-127 in 
  def M#i   : MMPULiteReg<i, "M["#i#"]" >, DwarfRegNum< [i] >;
def MIPP : MMPULiteReg<128, "M[I++]">, DwarfRegNum<[128]>;
def MSPP : MMPULiteReg<256, "M[S++]">, DwarfRegNum<[256]>;

def MReg : RegisterClass<"MMPULiteReg", [v16i32], 32,
                         (add (sequence "M%u", 0, 127), MIPP, MSPP)>;

def SHU0  : MMPULiteReg<0, "SHU0">, DwarfRegNum<[129]>;
def SHU1  : MMPULiteReg<1, "SHU1">, DwarfRegNum<[130]>;

def SHU   : RegisterClass<"MMPULiteReg", [v16i32], 32,
                          (add SHU0, SHU1)>;

def IALU  : MMPULiteReg<0, "IALU">, DwarfRegNum<[134]>;
def IMAC  : MMPULiteReg<1, "IMAC">, DwarfRegNum<[135]>;
def FALU  : MMPULiteReg<2, "FALU">, DwarfRegNum<[136]>;
def FMAC  : MMPULiteReg<3, "FMAC">, DwarfRegNum<[137]>;

def MACC : RegisterClass<"MMPULiteReg", [v16i32], 32,
			 (add IALU, IMAC, FALU, FMAC)>;

def T0  : MMPULiteReg<0, "T0">, DwarfRegNum<[138]>;
def T1  : MMPULiteReg<1, "T1">, DwarfRegNum<[139]>;
def T2  : MMPULiteReg<2, "T2">, DwarfRegNum<[140]>;
def T3  : MMPULiteReg<3, "T3">, DwarfRegNum<[141]>;

def TPort : RegisterClass<"MMPULiteReg", [v16i32], 32,
                          (sequence "T%u", 0, 3)>;

def TBB : MMPULiteReg<4,  "TBB">;
def TBH : MMPULiteReg<5,  "TBH">;
def TBW : MMPULiteReg<6,  "TBW">;
def TBD : MMPULiteReg<7,  "TBD">;
def TSQ : MMPULiteReg<8,  "TSQ">;

def TEPort : RegisterClass<"MMPULiteReg", [v16i32], 32,
                           (add (sequence "T%u", 0, 3), TBB, TBH, TBW, TBD, TSQ)>;

def BIU0  : MMPULiteReg<0, "BIU0">, DwarfRegNum<[142]>;
def BIU1  : MMPULiteReg<1, "BIU1">, DwarfRegNum<[143]>;
def BIU2  : MMPULiteReg<2, "BIU2">, DwarfRegNum<[144]>;

def BIU   : RegisterClass<"MMPULiteReg", [v16i32], 32,
                          (sequence "BIU%u", 0, 2)>;

def COMB : MMPULiteReg<0, "COMB">;

def SHUOprtor : RegisterClass<"MMPULiteReg", [i32], 32, (add COMB)>;

def MERGE : MMPULiteReg<6, "MERGE">;

def IALUTPOprtor : RegisterClass<"MMPULiteReg", [i32], 32, (add MERGE)>;

def IADD : MMPULiteReg<0b000000001, "+">;
def ISUB : MMPULiteReg<0b000000010, "-">;
def AND  : MMPULiteReg<0b000001000, "&">;
def OR   : MMPULiteReg<0b000001001, "|">;
def XOR  : MMPULiteReg<0b000001010, "^">;
def LSHT : MMPULiteReg<0b000001100, "<<">;
def RSHT : MMPULiteReg<0b000001101, ">>">;
def IEQU : MMPULiteReg<0b000010000, "==">;
def INEQ : MMPULiteReg<0b000010001, "!=">;
def ILT  : MMPULiteReg<0b000010010, ">">;
def INST : MMPULiteReg<0b000010011, ">=">;
def IST  : MMPULiteReg<0b000010100, "<">;
def INLT : MMPULiteReg<0b000010101, "<=">;

def IALUBinOprtor : RegisterClass<"MMPULiteReg", [i32], 32,
                               (add IADD, ISUB, AND,  OR,   XOR,
                                    LSHT, RSHT, IEQU, INEQ, ILT,
                                    INST, IST, INLT)>;

def IMAX : MMPULiteReg<0b000000011, "MAX">;
def IMIN : MMPULiteReg<0b000000100, "MIN">;
def IABS : MMPULiteReg<0b000000101, "ABS">;
def CPRS : MMPULiteReg<0b000001110, "CPRS">;

def IALUDPOprtor : RegisterClass<"MMPULiteReg", [i32], 32,
                                 (add IMAX, IMIN, IABS, CPRS)>;

def LSHTI : MMPULiteReg<0, "<<">;
def RSHTI : MMPULiteReg<1, ">>">;

def IALUImmOprtor : RegisterClass<"MMPULiteReg", [i32], 32,
                                  (add LSHTI, RSHTI)>;

def EXPD : MMPULiteReg<0b000001111, "EXPD">;
def NOT  : MMPULiteReg<0b000001011, "NOT">;
def RMAX : MMPULiteReg<0b000010110, "RMAX">;
def RMIN : MMPULiteReg<0b000010111, "RMIN">;

def IALUUryOprtor : RegisterClass<"MMPULiteReg", [i32], 32,
                                  (add EXPD, NOT, RMAX, RMIN)>;

def DIVS : MMPULiteReg<0, "DIVS">;
def DIVQ : MMPULiteReg<1, "DIVQ">;
def MDIVR : MMPULiteReg<2, "MDIVR">;
def MDIVQ : MMPULiteReg<3, "MDIVQ">;

def DIVOprtor : RegisterClass<"MMPULiteReg", [i32], 32,
                              (add DIVS, DIVQ, MDIVR, MDIVQ)>;

def FADD : MMPULiteReg<0b000000001, "+">;
def FSUB : MMPULiteReg<0b000000010, "-">;
def ADDSUB : MMPULiteReg<0b000000110, "+-">;
def FEQU : MMPULiteReg<0b000001000, "==">;
def FNEQ : MMPULiteReg<0b000001001, "!=">;
def FLT  : MMPULiteReg<0b000001010, ">">;
def FNST : MMPULiteReg<0b000001011, ">=">;
def FST  : MMPULiteReg<0b000001100, "<">;
def FNLT : MMPULiteReg<0b000001101, "<=">;

def FALUBinOprtor : RegisterClass<"MMPULiteReg", [i32], 32,
                                  (add FADD, FSUB, ADDSUB, FEQU, FNEQ,
                                       FLT,  FNST, FST,    FNLT)>;

def FMAX : MMPULiteReg<0b000000011, "MAX">;
def FMIN : MMPULiteReg<0b000000100, "MIN">;

def FALUDPOprtor : RegisterClass<"MMPULiteReg", [i32], 32,
                                 (add FMAX, FMIN)>;

def FABS : MMPULiteReg<0b000000101, "ABS">;
def SINGLE : MMPULiteReg<0b000010000, "SINGLE">;
def DOUBLE : MMPULiteReg<0b000010001, "DOUBLE">;
def INT  : MMPULiteReg<0b000010010, "INT">;
def UINT : MMPULiteReg<0b000010011, "UINT">;
def RECIP : MMPULiteReg<0b000011000, "RECIP">;
def RSQRT : MMPULiteReg<0b000011001, "RSQRT">;

def FALUUryOprtor : RegisterClass<"MMPULiteReg", [i32], 32,
                                 (add FABS,  SINGLE, DOUBLE, INT, UINT,
                                      RECIP, RSQRT)>;

def IMUL : MMPULiteReg<2, "*">;
def IMULT : MMPULiteReg<1, "*">;
def IMULS : MMPULiteReg<3, "*">;
def MR : MMPULiteReg<0, "MR">;

def IMACMulOprtor : RegisterClass<"MMPULiteReg", [i32], 32,
                                  (add IMUL, IMULT, IMULS)>;

def FMUL : MMPULiteReg<0, "*">;
def CMUL : MMPULiteReg<1, "**">;

def FMACMulOprtor : RegisterClass<"MMPULiteReg", [i32], 32,
                                  (add FMUL, CMUL)>;

def C : MMPULiteReg<2, "@(C)">;
def NC : MMPULiteReg<3, "@(!C)">;
def NOC : MMPULiteReg<0, " ">;
/* 
class RegisterClass<string namespace, list<ValueType> regTypes, int alignment,
                    dag regList, RegAltNameIndex idx = NoRegAltName>
*/

def Condition : RegisterClass<"MMPULiteReg", [i32], 32, (add C, NC, NOC)>;

foreach i = 1-3 in 
  def WF#i   : MMPULiteReg<i, "(WF"#i#")" >;
  
foreach i = 1-4 in 
  def WX#i   : MMPULiteReg<!add(i, 3), "(WX"#i#")" >;
  

def WF0   : MMPULiteReg<0, " " >;
def WFlag : RegisterClass<"MMPULiteReg", [i32], 32,
                          (add (sequence "WF%u", 0, 3),
                           (sequence "WX%u", 1, 4))>;
