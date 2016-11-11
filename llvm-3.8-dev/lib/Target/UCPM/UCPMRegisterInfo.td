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
//def MIPP : UCPMReg<2, "M[I++]">, DwarfRegNum<[128]>;
//def MSPP : UCPMReg<1, "M[S++]">, DwarfRegNum<[256]>;

def MReg : RegisterClass<"UCPMReg", [v16i32], 32,
                         (add (sequence "M%u", 0, 127))>;

def IALU  : UCPMReg<0, "IALU">, DwarfRegNum<[134]>;
def IMAC  : UCPMReg<1, "IMAC">, DwarfRegNum<[135]>;
def IFALU : UCPMReg<2, "IFALU">, DwarfRegNum<[136]>;
def IFMAC : UCPMReg<3, "IFMAC">, DwarfRegNum<[137]>;
def SHU0  : UCPMReg<0, "SHU0">, DwarfRegNum<[129]>;
def SHU1  : UCPMReg<1, "SHU1">, DwarfRegNum<[130]>;
def SHU2  : UCPMReg<2, "SHU2">, DwarfRegNum<[131]>;
def sSHU0  : UCPMReg<1, "SHU0">, DwarfRegNum<[129]>;//dcx:for SHU self
def sSHU1  : UCPMReg<1, "SHU1">, DwarfRegNum<[130]>;//dcx:for SHU self
def sSHU2  : UCPMReg<1, "SHU2">, DwarfRegNum<[131]>;//dcx:for SHU self
def bBIU0  : UCPMReg<0, "BIU0">, DwarfRegNum<[129]>;//dcx:for BIU self
def bBIU1  : UCPMReg<0, "BIU1">, DwarfRegNum<[130]>;//dcx:for BIU self
def bBIU2  : UCPMReg<0, "BIU2">, DwarfRegNum<[131]>;//dcx:for BIU self
def B1SHU2  : UCPMReg<1, "SHU2">, DwarfRegNum<[131]>;//dcx:for BIU1
def B2SHU1  : UCPMReg<0, "SHU1">, DwarfRegNum<[131]>;//dcx:for BIU2
def MTSHU0  : UCPMReg<2, "SHU0">, DwarfRegNum<[129]>;//MT: for MReg Target
def MTSHU1  : UCPMReg<2, "SHU1">, DwarfRegNum<[130]>;
def MTSHU2  : UCPMReg<2, "SHU2">, DwarfRegNum<[131]>;
def MTIALU  : UCPMReg<0, "IALU">, DwarfRegNum<[134]>;
def MTIMAC  : UCPMReg<1, "IMAC">, DwarfRegNum<[135]>;
def MTIFALU  : UCPMReg<0, "IFALU">, DwarfRegNum<[136]>;
def MTIFMAC  : UCPMReg<1, "IFMAC">, DwarfRegNum<[137]>;
def sIMAC   : UCPMReg<2, "IMAC">, DwarfRegNum<[135]>;
def sIFALU  : UCPMReg<1, "IFALU">, DwarfRegNum<[136]>;
	  
// def MTSHU  : RegisterClass<"UCPMReg", [v16i32], 32,
//                           (add MTSHU0, MTSHU1, MTSHU2)>;
			  
			  
// dcx: for the change of instruction document 
def ALUSHU0  : UCPMReg<0, "SHU0">, DwarfRegNum<[129]>;
def ALUSHU1  : UCPMReg<4, "SHU1">, DwarfRegNum<[130]>;
def ALUSHU2  : UCPMReg<6, "SHU2">, DwarfRegNum<[131]>;

def SHU  : RegisterClass<"UCPMReg", [v16i32], 32,
                          (add ALUSHU0, ALUSHU1, ALUSHU2)>;
			  
def ALUIALU  : UCPMReg<0, "IALU">, DwarfRegNum<[134]>;
def ALUIMAC  : UCPMReg<2, "IMAC">, DwarfRegNum<[135]>;
def ALUIFALU : UCPMReg<4, "IFALU">, DwarfRegNum<[136]>;
def ALUIFMAC : UCPMReg<6, "IFMAC">, DwarfRegNum<[137]>;

def MACC : RegisterClass<"UCPMReg", [v16i32], 32,
			 (add ALUIALU, ALUIMAC, ALUIFALU, ALUIFMAC)>;

def ALUBIU0  : UCPMReg<1, "BIU0">, DwarfRegNum<[142]>;
def ALUBIU1  : UCPMReg<2, "BIU1">, DwarfRegNum<[143]>;
def ALUBIU2  : UCPMReg<4, "BIU2">, DwarfRegNum<[144]>;

def BIU   : RegisterClass<"UCPMReg", [v16i32], 32,
                          (add ALUBIU0, ALUBIU1, ALUBIU2)>;
			  

			  
// def MTMACC : RegisterClass<"UCPMReg", [v16i32], 32,
// 			 (add MTIALU, MTIMAC, MTIFALU, MTIFMAC)>;

//yangl
			 
def BIU0  : UCPMReg<0, "BIU0">, DwarfRegNum<[142]>;
def BIU1  : UCPMReg<1, "BIU1">, DwarfRegNum<[143]>;
def BIU2  : UCPMReg<2, "BIU2">, DwarfRegNum<[144]>;
def S1BIU1  : UCPMReg<2, "BIU1">, DwarfRegNum<[144]>;//dcx:for SHU1
def S2BIU1  : UCPMReg<0, "BIU1">, DwarfRegNum<[144]>;//dcx:for SHU2

def MTBIU0  : UCPMReg<3, "BIU0">, DwarfRegNum<[142]>;//MT: for MReg Target
def MTBIU1  : UCPMReg<3, "BIU1">, DwarfRegNum<[143]>;
def MTBIU2  : UCPMReg<3, "BIU2">, DwarfRegNum<[144]>;


			  
// def MTBIU   : RegisterClass<"UCPMReg", [v16i32], 32,
//                           (sequence "MTBIU%u", 0, 2)>;
			  
def MR0Dest : RegisterClass<"UCPMReg", [v16i32], 32, (add MTIALU, MTIMAC, MTSHU0, MTBIU0)>;
def MR1Dest : RegisterClass<"UCPMReg", [v16i32], 32, (add MTIALU, MTIMAC, MTSHU1)>;
def MR2Dest : RegisterClass<"UCPMReg", [v16i32], 32, (add MTIALU, MTIMAC, MTSHU2, MTBIU2)>;
def MR3Dest : RegisterClass<"UCPMReg", [v16i32], 32, (add MTIFALU, MTIFMAC, MTSHU0)>;
def MR4Dest : RegisterClass<"UCPMReg", [v16i32], 32, (add MTIFALU, MTIFMAC, MTSHU1, MTBIU1)>;
def MR5Dest : RegisterClass<"UCPMReg", [v16i32], 32, (add MTIFALU, MTIFMAC, MTSHU2, MTBIU2)>;

// ducx
def BIU0Dest: RegisterClass<"UCPMReg", [v16i32], 32, (add SHU0, SHU1, IALU, IMAC, IFALU, IFMAC, bBIU0)>;
def BIU1Dest: RegisterClass<"UCPMReg", [v16i32], 32, (add SHU0, B1SHU2, IALU, IMAC, IFALU, IFMAC, bBIU1)>;
def BIU2Dest: RegisterClass<"UCPMReg", [v16i32], 32, (add B2SHU1, B1SHU2, IALU, IMAC, IFALU, IFMAC, bBIU2)>;
// ducx
def SHU0Dest: RegisterClass<"UCPMReg", [v16i32], 32, (add BIU0, BIU2, IALU, sIMAC, sIFALU, IFMAC, sSHU0)>;
def SHU1Dest: RegisterClass<"UCPMReg", [v16i32], 32, (add BIU0, S1BIU1, IALU, sIMAC, sIFALU, IFMAC, sSHU1)>;
def SHU2Dest: RegisterClass<"UCPMReg", [v16i32], 32, (add S2BIU1, BIU2, IALU, sIMAC, sIFALU, IFMAC, sSHU2)>;

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


// ducx start IALU ------------------------------------------------

// MERGE                                  
def f_MERGE : UCPMReg<8, "MERGE">;

def IALUTPOprtor : RegisterClass<"UCPMReg", [i32], 32, (add f_MERGE)>;

// ADD SUB
def f_IADD : UCPMReg<0b000000000, "+">;
def f_ISUB : UCPMReg<0b000000001, "-">;

def IALUASOprtor : RegisterClass<"UCPMReg", [i32], 32,
                               (add f_IADD, f_ISUB)>;			    
// AND OR XOR       
def m_AND  : UCPMReg<0b000000000, "&">;
def m_OR   : UCPMReg<0b000000001, "|">;
def m_XOR  : UCPMReg<0b000000010, "^">;

def IALULogicOprtor : RegisterClass<"UCPMReg", [i32], 32,
                               (add m_AND, m_OR, m_XOR)>;
// EQU NEQ LT NST ST NLT
def m_IEQU : UCPMReg<0b000100110, "==">;
def m_INEQ : UCPMReg<0b000100111, "!=">;
def m_ILT  : UCPMReg<0b000101000, ">">;
def m_INST : UCPMReg<0b000101010, ">=">;
def m_IST  : UCPMReg<0b000101001, "<">;
def m_INLT : UCPMReg<0b000101011, "<=">;

def IALUComOprtor : RegisterClass<"UCPMReg", [i32], 32,
                               (add m_IEQU, m_INEQ, m_ILT, m_INST, m_IST, m_INLT)>;

//  MAX MIN 
def m_IMAX : UCPMReg<0b000001000, "MAX">;
def m_IMIN : UCPMReg<0b000001001, "MIN">;

def IALUMNOprtor : RegisterClass<"UCPMReg", [i32], 32,
                               (add m_IMAX, m_IMIN)>;
                               
// LSH RSH
def m_LSHT : UCPMReg<0, "<<">;
def m_RSHT : UCPMReg<1, ">>">;

def IALUShiftOprtor : RegisterClass<"UCPMReg", [i32], 32,
                               (add m_LSHT, m_RSHT)>;

// SUBABS CPRS
def m_SUBABS : UCPMReg<0b000000110, "SUBABS">;
def m_CPRS : UCPMReg<0b000010110, "CPRS">;

def IALUSCOprtor : RegisterClass<"UCPMReg", [i32], 32,
                                  (add m_SUBABS, m_CPRS)>;
                                  
// RADD RMAX RMIN EXPD			       
def m_EXPD : UCPMReg<0b000101111, "EXPD">;
def m_RADD : UCPMReg<0b000100010, "RADD">;
def m_RMAX : UCPMReg<0b000001010, "RMAX">;
def m_RMIN : UCPMReg<0b000001011, "RMIN">;

def IALUUryRMNOprtor : RegisterClass<"UCPMReg", [i32], 32,
                                  (add m_EXPD, m_RADD, m_RMAX, m_RMIN)>;		       

// NOT                                  
def m_NOT : UCPMReg<0b000000011, "NOT">;

def IALUUryNOTOprtor : RegisterClass<"UCPMReg", [i32], 32, (add m_NOT)>;

// ABS CONJ MINUS
def m_ABS : UCPMReg<0b000000111, "ABS">;
def m_CONJ : UCPMReg<0b000001000, "CONJ">;
def m_MINUS : UCPMReg<0b000001001, "MINUS">;

def IALUUryCONOprtor : RegisterClass<"UCPMReg", [i32], 32,
                                  (add m_ABS, m_CONJ, m_MINUS)>;
                                  
// LSHimm RSHimm
def f_LSHTI : UCPMReg<0b000000110, "<<">;
def f_RSHTI : UCPMReg<0b000000111, ">>">;

def IALUImmOprtor : RegisterClass<"UCPMReg", [i32], 32,
                                  (add f_LSHTI, f_RSHTI)>;
//  DIV                               
def DIVSTART : UCPMReg<0b000001111, "DIVSTART">;
def DIVCONT : UCPMReg<0b000001100, "DIVCONT">;
def READQ : UCPMReg<0b000011101, "READQ">;
def READR : UCPMReg<0b000011011, "READR">;

def DIVOprtor : RegisterClass<"UCPMReg", [i32], 32,
                              (add DIVSTART, DIVCONT, READQ, READR)>;
// ducx end IALU--------------------------------------------------------		
				    
                              
// ducx start IFALU ----------------------------------------------------
                                       
// EQU NEQ LT NST ST NLT
def m_FEQU : UCPMReg<0b000100110, "==">;
def m_FNEQ : UCPMReg<0b000100111, "!=">;
def m_FLT  : UCPMReg<0b000101000, ">">;
def m_FNST : UCPMReg<0b000101010, ">=">;
def m_FST  : UCPMReg<0b000101001, "<">;
def m_FNLT : UCPMReg<0b000101011, "<=">;

def IFALUComOprtor : RegisterClass<"UCPMReg", [i32], 32,
                               (add m_FEQU, m_FNEQ, m_FLT, m_FNST, m_FST, m_FNLT)>;

//  MAX MIN 
def m_FMAX : UCPMReg<0b000001000, "MAX">;
def m_FMIN : UCPMReg<0b000001001, "MIN">;

def IFALUMNOprtor : RegisterClass<"UCPMReg", [i32], 32,
                               (add m_FMAX, m_FMIN)>;
                              
//  Transfer                        
def IntToSingle : UCPMReg<0b000011000, "SINGLE">;
def SingleToInt    : UCPMReg<0b000011010, "INT">;
def DouToSingle : UCPMReg<0b000011110, "SINGLE">;
def SingleToDou : UCPMReg<0b000011111, "DOUBLE">;



def IFALUTranOprtor : RegisterClass<"UCPMReg", [i32], 32,
                                 (add IntToSingle, SingleToInt, 
                                     DouToSingle, SingleToDou )>; 
// ducx end IFALU ------------------------------------------------------
                        
                                     
                                     
// ducx start IMAC ------------------------------------------------------
def IMUL : UCPMReg<0, "*">;               
def IMACMulOprtor : RegisterClass<"UCPMReg", [i32], 32, (add IMUL)>;             
// ducx end IMAC ------------------------------------------------------


// ducx start SHU ------------------------------------------------------
def BIT : UCPMReg<0b011111001, "BIT">;    
def BYTE : UCPMReg<0b011111000, "BYTE">;  
def SHUTranOprtor : RegisterClass<"UCPMReg", [i32], 32, (add BIT,BYTE)>;             
// ducx end SHU ------------------------------------------------------



// ducx start IFMAC ------------------------------------------------------
def FMUL : UCPMReg<0b000000010, "*">;
def FADD : UCPMReg<0b000001010, "+">;
def FSUB : UCPMReg<0b000001011, "-">;

def IFMACMulOprtor : RegisterClass<"UCPMReg", [i32], 32, (add FMUL, FADD, FSUB)>;
// ducx end IFMAC ------------------------------------------------------                                 
           

// ducx start BIU ------------------------------------------------------
def BIUADD : UCPMReg<0b000000000, "+">;
def BIUSUB : UCPMReg<0b000000001, "-">;
def BIUAND : UCPMReg<0b000000010, "&">;
def BIUOR : UCPMReg<0b000000011, "|">;

def BIUBinOprtor : RegisterClass<"UCPMReg", [i32], 32, (add BIUADD, BIUSUB, BIUAND, BIUOR)>;

def BIULSHT : UCPMReg<0b001001010, "<<">;
def BIURSHT : UCPMReg<0b001001011, ">>">;

def BIUShiftOprtor : RegisterClass<"UCPMReg", [i32], 32, (add BIULSHT, BIURSHT)>;

def BIUEQU : UCPMReg<0b001000001, "==0">;
def BIUNEQ : UCPMReg<0b001000000, "!=0">;

def BIUComOprtor : RegisterClass<"UCPMReg", [i32], 32, (add BIUEQU, BIUNEQ)>;

// ducx end BIU ------------------------------------------------------                                 
                                 
        
// ducx start MFetch -----------------------------------------------------

def MFetchADD : UCPMReg<0b000000000, "+">;
def MFetchSUB : UCPMReg<0b000000001, "-">;
def MFetchST  : UCPMReg<0b000000100, "<">;
def MFetchNLT : UCPMReg<0b000000101, "<=">;
def MFetchEQU : UCPMReg<0b000000110, "==">;
def MFetchNEQ : UCPMReg<0b000000111, "!=">;
def MFetchAND : UCPMReg<0b000001000, "&">;
def MFetchOR : UCPMReg<0b000001001, "|">;

def MFetchBinOprtor : RegisterClass<"UCPMReg", [i32], 32, (add MFetchADD, MFetchSUB, MFetchST, 
                                                           MFetchNLT, MFetchEQU, MFetchNEQ, MFetchAND, MFetchOR )>;

def MFetchLSHTimm  : UCPMReg<0b000000010, "<<">;
def MFetchRSHTimm : UCPMReg<0b000000011, ">>">;

def MFetchShiftOprtor : RegisterClass<"UCPMReg", [i32], 32, (add MFetchLSHTimm, MFetchRSHTimm )>;

                                                           
// ducx end MFetch --------------------------------------------------------




/* 
class RegisterClass<string namespace, list<ValueType> regTypes, int alignment,
                    dag regList, RegAltNameIndex idx = NoRegAltName>
*/

// def C : UCPMReg<2, "@(C)">;
// def NC : UCPMReg<3, "@(!C)">;
// def NOC : UCPMReg<0, " ">;

def Mode0 : UCPMReg<0, "(Mode0)">;
def Mode1 : UCPMReg<1, "(Mode1)">;
def NMode0 : UCPMReg<2, "(!Mode0)">;
def NMode1 : UCPMReg<3, "(!Mode1)">;

def Condition : RegisterClass<"UCPMReg", [i32], 32, (add Mode0, Mode1, NMode0, NMode1)>;


// foreach i = 1-3 in 
//   def WF#i   : UCPMReg<i, "(WF"#i#")" >;
//   
// foreach i = 1-4 in 
//   def WX#i   : UCPMReg<!add(i, 3), "(WX"#i#")" >;
//   
// 
// def WF0   : UCPMReg<0, " " >;
// def WFlag : RegisterClass<"UCPMReg", [i32], 32,
//                           (add (sequence "WF%u", 0, 3),
//                            (sequence "WX%u", 1, 4))>;
