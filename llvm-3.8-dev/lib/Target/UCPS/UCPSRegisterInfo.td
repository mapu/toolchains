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
def MIPP : UCPSReg<2, "M[I++]">, DwarfRegNum<[128]>;
def MSPP : UCPSReg<1, "M[S++]">, DwarfRegNum<[256]>;

def MReg : RegisterClass<"UCPSReg", [v16i32], 32,
                         (add (sequence "M%u", 0, 127), MIPP, MSPP)>;

def IALU  : UCPSReg<0, "IALU">, DwarfRegNum<[134]>;
def IMAC  : UCPSReg<1, "IMAC">, DwarfRegNum<[135]>;
def IFALU : UCPSReg<2, "IFALU">, DwarfRegNum<[136]>;
def IFMAC : UCPSReg<3, "IFMAC">, DwarfRegNum<[137]>;
def SHU0  : UCPSReg<0, "SHU0">, DwarfRegNum<[129]>;
def SHU1  : UCPSReg<1, "SHU1">, DwarfRegNum<[130]>;
def SHU2  : UCPSReg<2, "SHU2">, DwarfRegNum<[131]>;
def B1SHU2  : UCPSReg<1, "SHU2">, DwarfRegNum<[131]>;//dcx:for BIU1
def B2SHU1  : UCPSReg<0, "SHU1">, DwarfRegNum<[131]>;//dcx:for BIU2
def MTSHU0  : UCPSReg<2, "SHU0">, DwarfRegNum<[129]>;//MT: for MReg Target
def MTSHU1  : UCPSReg<2, "SHU1">, DwarfRegNum<[130]>;
def MTSHU2  : UCPSReg<2, "SHU2">, DwarfRegNum<[131]>;
def MTIALU  : UCPSReg<0, "IALU">, DwarfRegNum<[134]>;
def MTIMAC  : UCPSReg<1, "IMAC">, DwarfRegNum<[135]>;
def MTIFALU  : UCPSReg<0, "IFALU">, DwarfRegNum<[136]>;
def MTIFMAC  : UCPSReg<1, "IFMAC">, DwarfRegNum<[137]>;

def SHU  : RegisterClass<"UCPSReg", [v16i32], 32,
                          (add SHU0, SHU1, SHU2)>;
def MTSHU  : RegisterClass<"UCPSReg", [v16i32], 32,
                          (add MTSHU0, MTSHU1, MTSHU2)>;

def MACC : RegisterClass<"UCPSReg", [v16i32], 32,
			 (add IALU, IMAC, IFALU, IFMAC)>;
			 
def MTMACC : RegisterClass<"UCPSReg", [v16i32], 32,
			 (add MTIALU, MTIMAC, MTIFALU, MTIFMAC)>;

//yangl
			 
def BIU0  : UCPSReg<0, "BIU0">, DwarfRegNum<[142]>;
def BIU1  : UCPSReg<1, "BIU1">, DwarfRegNum<[143]>;
def BIU2  : UCPSReg<2, "BIU2">, DwarfRegNum<[144]>;

def MTBIU0  : UCPSReg<3, "BIU0">, DwarfRegNum<[142]>;//MT: for MReg Target
def MTBIU1  : UCPSReg<3, "BIU1">, DwarfRegNum<[143]>;
def MTBIU2  : UCPSReg<3, "BIU2">, DwarfRegNum<[144]>;

def BIU   : RegisterClass<"UCPSReg", [v16i32], 32,
                          (sequence "BIU%u", 0, 2)>;
def MTBIU   : RegisterClass<"UCPSReg", [v16i32], 32,
                          (sequence "MTBIU%u", 0, 2)>;
			  
def MR0Dest : RegisterClass<"UCPSReg", [v16i32], 32, (add MTIALU, MTIMAC, MTSHU0, MTBIU0)>;
def MR1Dest : RegisterClass<"UCPSReg", [v16i32], 32, (add MTIALU, MTIMAC, MTSHU1)>;
def MR2Dest : RegisterClass<"UCPSReg", [v16i32], 32, (add MTIALU, MTIMAC, MTSHU2, MTBIU2)>;
def MR3Dest : RegisterClass<"UCPSReg", [v16i32], 32, (add MTIFALU, MTIFMAC, MTSHU0)>;
def MR4Dest : RegisterClass<"UCPSReg", [v16i32], 32, (add MTIFALU, MTIFMAC, MTSHU1, MTBIU1)>;
def MR5Dest : RegisterClass<"UCPSReg", [v16i32], 32, (add MTIFALU, MTIFMAC, MTSHU2, MTBIU2)>;

// ducx
def BIU0Dest: RegisterClass<"UCPSReg", [v16i32], 32, (add SHU0, SHU1, IALU, IMAC, IFALU, IFMAC)>;
def BIU1Dest: RegisterClass<"UCPSReg", [v16i32], 32, (add SHU0, B1SHU2, IALU, IMAC, IFALU, IFMAC)>;
def BIU2Dest: RegisterClass<"UCPSReg", [v16i32], 32, (add B2SHU1, B1SHU2, IALU, IMAC, IFALU, IFMAC)>;


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

def COMB : UCPSReg<0, "COMB">;

def SHUOprtor : RegisterClass<"UCPSReg", [i32], 32, (add COMB)>;



// ducx start IALU ------------------------------------------------

// MERGE                                  
def f_MERGE : UCPSReg<8, "MERGE">;

def IALUTPOprtor : RegisterClass<"UCPSReg", [i32], 32, (add f_MERGE)>;

// ADD SUB
def f_IADD : UCPSReg<0b000000000, "+">;
def f_ISUB : UCPSReg<0b000000001, "-">;

def IALUASOprtor : RegisterClass<"UCPSReg", [i32], 32,
                               (add f_IADD, f_ISUB)>;			    
// AND OR XOR       
def m_AND  : UCPSReg<0b000000000, "&">;
def m_OR   : UCPSReg<0b000000001, "|">;
def m_XOR  : UCPSReg<0b000000010, "^">;

def IALULogicOprtor : RegisterClass<"UCPSReg", [i32], 32,
                               (add m_AND, m_OR, m_XOR)>;
// EQU NEQ LT NST ST NLT
def m_IEQU : UCPSReg<0b000100110, "==">;
def m_INEQ : UCPSReg<0b000100111, "!=">;
def m_ILT  : UCPSReg<0b000101000, ">">;
def m_INST : UCPSReg<0b000101010, ">=">;
def m_IST  : UCPSReg<0b000101001, "<">;
def m_INLT : UCPSReg<0b000101011, "<=">;

def IALUComOprtor : RegisterClass<"UCPSReg", [i32], 32,
                               (add m_IEQU, m_INEQ, m_ILT, m_INST, m_IST, m_INLT)>;

//  MAX MIN 
def m_IMAX : UCPSReg<0b000001000, "MAX">;
def m_IMIN : UCPSReg<0b000001001, "MIN">;

def IALUMNOprtor : RegisterClass<"UCPSReg", [i32], 32,
                               (add m_IMAX, m_IMIN)>;
                               
// LSH RSH
def m_LSHT : UCPSReg<0, "<<">;
def m_RSHT : UCPSReg<1, ">>">;

def IALUShiftOprtor : RegisterClass<"UCPSReg", [i32], 32,
                               (add m_LSHT, m_RSHT)>;

// SUBABS CPRS
def m_SUBABS : UCPSReg<0b000000110, "SUBABS">;
def m_CPRS : UCPSReg<0b000010110, "CPRS">;

def IALUSCOprtor : RegisterClass<"UCPSReg", [i32], 32,
                                  (add m_SUBABS, m_CPRS)>;
                                  
// RADD RMAX RMIN EXPD			       
def m_EXPD : UCPSReg<0b000101111, "EXPD">;
def m_RADD : UCPSReg<0b000100010, "RADD">;
def m_RMAX : UCPSReg<0b000001010, "RMAX">;
def m_RMIN : UCPSReg<0b000001011, "RMIN">;

def IALUUryRMNOprtor : RegisterClass<"UCPSReg", [i32], 32,
                                  (add m_EXPD, m_RADD, m_RMAX, m_RMIN)>;		       

// NOT                                  
def m_NOT : UCPSReg<0b000000011, "NOT">;

def IALUUryNOTOprtor : RegisterClass<"UCPSReg", [i32], 32, (add m_NOT)>;

// ABS CONJ MINUS
def m_ABS : UCPSReg<0b000000111, "ABS">;
def m_CONJ : UCPSReg<0b000001000, "CONJ">;
def m_MINUS : UCPSReg<0b000001001, "MINUS">;

def IALUUryCONOprtor : RegisterClass<"UCPSReg", [i32], 32,
                                  (add m_ABS, m_CONJ, m_MINUS)>;
                                  
// LSHimm RSHimm
def f_LSHTI : UCPSReg<0b000000110, "<<">;
def f_RSHTI : UCPSReg<0b000000111, ">>">;

def IALUImmOprtor : RegisterClass<"UCPSReg", [i32], 32,
                                  (add f_LSHTI, f_RSHTI)>;
//  DIV                               
def DIVSTART : UCPSReg<0b000011110, "DIVSTART">;
def DIVCONT : UCPSReg<0b000011000, "DIVCONT">;
def READQ : UCPSReg<0b000011101, "READQ">;
def READR : UCPSReg<0b000011011, "READR">;

def DIVOprtor : RegisterClass<"UCPSReg", [i32], 32,
                              (add DIVSTART, DIVCONT, READQ, READR)>;
// ducx end IALU--------------------------------------------------------		
				    
                              
// ducx start IFALU ----------------------------------------------------
                                       
// EQU NEQ LT NST ST NLT
def m_FEQU : UCPSReg<0b000100110, "==">;
def m_FNEQ : UCPSReg<0b000100111, "!=">;
def m_FLT  : UCPSReg<0b000101000, ">">;
def m_FNST : UCPSReg<0b000101010, ">=">;
def m_FST  : UCPSReg<0b000101001, "<">;
def m_FNLT : UCPSReg<0b000101011, "<=">;

def IFALUComOprtor : RegisterClass<"UCPSReg", [i32], 32,
                               (add m_FEQU, m_FNEQ, m_FLT, m_FNST, m_FST, m_FNLT)>;

//  MAX MIN 
def m_FMAX : UCPSReg<0b000001000, "MAX">;
def m_FMIN : UCPSReg<0b000001001, "MIN">;

def IFALUMNOprtor : RegisterClass<"UCPSReg", [i32], 32,
                               (add m_FMAX, m_FMIN)>;
                              
//  Transfer                        
def IntToSINGLE : UCPSReg<0b000011000, "SINGLE">;
def DouToSINGLE : UCPSReg<0b000011110, "SINGLE">;
def IntToDOUBLE : UCPSReg<0b000011001, "DOUBLE">;
def SinToDOUBLE : UCPSReg<0b000011111, "DOUBLE">;
def SinToINT  : UCPSReg<0b000011010, "INT">;
def DouToINT  : UCPSReg<0b000011011, "INT">;

def IFALUUryOprtor : RegisterClass<"UCPSReg", [i32], 32,
                                 (add IntToSINGLE, DouToSINGLE, IntToDOUBLE, 
                                     SinToDOUBLE, SinToINT, DouToINT)>; 
// ducx end IFALU ------------------------------------------------------
                        
                                     
                                     
// ducx start IMAC ------------------------------------------------------
def IMUL : UCPSReg<0, "*">;               
def IMACMulOprtor : RegisterClass<"UCPSReg", [i32], 32, (add IMUL)>;             
// ducx end IMAC ------------------------------------------------------



// ducx start IFMAC ------------------------------------------------------
def FMUL : UCPSReg<0b000001000, "*">;
def FADD : UCPSReg<0b000001010, "+">;
def FSUB : UCPSReg<0b000001011, "-">;

def IFMACMulOprtor : RegisterClass<"UCPSReg", [i32], 32, (add FMUL, FADD, FSUB)>;
// ducx end IFMAC ------------------------------------------------------                                 
           

// ducx start BIU ------------------------------------------------------
def BIUADD : UCPSReg<0b000000000, "+">;
def BIUSUB : UCPSReg<0b000000001, "-">;
def BIUAND : UCPSReg<0b000000010, "&">;
def BIUOR : UCPSReg<0b000000011, "|">;

def BIUBinOprtor : RegisterClass<"UCPSReg", [i32], 32, (add BIUADD, BIUSUB, BIUAND, BIUOR)>;

def BIULSHT : UCPSReg<0b001001010, "<<">;
def BIURSHT : UCPSReg<0b001001011, ">>">;

def BIUShiftOprtor : RegisterClass<"UCPSReg", [i32], 32, (add BIULSHT, BIURSHT)>;

def BIUEQU : UCPSReg<0b001000001, "==0">;
def BIUNEQ : UCPSReg<0b001000000, "!=0">;

def BIUComOprtor : RegisterClass<"UCPSReg", [i32], 32, (add BIUEQU, BIUNEQ)>;

// ducx end BIU ------------------------------------------------------                                 
                                 
        
// ducx start MFetch -----------------------------------------------------

def MFetchADD : UCPSReg<0b000000000, "+">;
def MFetchSUB : UCPSReg<0b000000001, "-">;
def MFetchST  : UCPSReg<0b000000100, "<">;
def MFetchNLT : UCPSReg<0b000000101, "<=">;
def MFetchEQU : UCPSReg<0b000000110, "==">;
def MFetchNEQ : UCPSReg<0b000000111, "!=">;
def MFetchAND : UCPSReg<0b000001000, "&">;
def MFetchOR : UCPSReg<0b000001001, "|">;

def MFetchBinOprtor : RegisterClass<"UCPSReg", [i32], 32, (add MFetchADD, MFetchSUB, MFetchST, 
                                                           MFetchNLT, MFetchEQU, MFetchNEQ, MFetchAND, MFetchOR )>;

def MFetchLSHTimm  : UCPSReg<0b000000010, "<<">;
def MFetchRSHTimm : UCPSReg<0b000000011, ">>">;

def MFetchShiftOprtor : RegisterClass<"UCPSReg", [i32], 32, (add MFetchLSHTimm, MFetchRSHTimm )>;

                                                           
// ducx end MFetch --------------------------------------------------------

// def ADDSUB : UCPSReg<0b000000110, "+-">;
// 
// 
// def FALUBinOprtor : RegisterClass<"UCPSReg", [i32], 32,
//                                   (add FADD, FSUB, ADDSUB)>;  


// def IMUL : UCPSReg<2, "*">;
// def IMULT : UCPSReg<1, "*">;
// def IMULS : UCPSReg<3, "*">;
// def MR : UCPSReg<0, "MR">;
// 
// def IMACMulOprtor : RegisterClass<"UCPSReg", [i32], 32,
//                                   (add IMUL, IMULT, IMULS)>;



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
