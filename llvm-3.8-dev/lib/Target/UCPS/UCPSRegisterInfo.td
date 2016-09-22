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

// n is the AsmName, like 'R23', 'SP'.
// We only specify 'Namespace' here for UCPS registers.
// Use n to parameterize this class.
class UCPSReg< bits<5> num, string n > : Register<n> {
  field bits<5> Num = num;
  let Namespace = "UCPSReg";
}
			 
// UCPS 32-bit integer/floating-point physical registers.
// (no 64-bit integer register supported in UCPS)
def R0  : UCPSReg< 0,  "R0">, DwarfRegNum<[0]>;
def R1  : UCPSReg< 1,  "R1">, DwarfRegNum<[1]>;
def R2  : UCPSReg< 2,  "R2">, DwarfRegNum<[2]>; 
def R3  : UCPSReg< 3,  "R3">, DwarfRegNum<[3]>;
def R4  : UCPSReg< 4,  "R4">, DwarfRegNum<[4]>;
def R5  : UCPSReg< 5,  "R5">, DwarfRegNum<[5]>; 
def R6  : UCPSReg< 6,  "R6">, DwarfRegNum<[6]>;
def R7  : UCPSReg< 7,  "R7">, DwarfRegNum<[7]>;
def R8  : UCPSReg< 8,  "R8">, DwarfRegNum<[8]>; 
def R9  : UCPSReg< 9,  "R9">, DwarfRegNum<[9]>;
def R10 : UCPSReg<10, "R10">, DwarfRegNum<[10]>;
def R11 : UCPSReg<11, "R11">, DwarfRegNum<[11]>; 
def R12 : UCPSReg<12, "R12">, DwarfRegNum<[12]>;
def R13 : UCPSReg<13, "R13">, DwarfRegNum<[13]>;
def R14 : UCPSReg<14, "R14">, DwarfRegNum<[14]>; 
def R15 : UCPSReg<15, "R15">, DwarfRegNum<[15]>;
def R16 : UCPSReg<16, "R16">, DwarfRegNum<[16]>;
def R17 : UCPSReg<17, "R17">, DwarfRegNum<[17]>; 
def R18 : UCPSReg<18, "R18">, DwarfRegNum<[18]>;
def R19 : UCPSReg<19, "R19">, DwarfRegNum<[19]>;
def R20 : UCPSReg<20, "R20">, DwarfRegNum<[20]>; 
def R21 : UCPSReg<21, "R21">, DwarfRegNum<[21]>;
def R22 : UCPSReg<22, "R22">, DwarfRegNum<[22]>;
def R23 : UCPSReg<23, "R23">, DwarfRegNum<[23]>;
def R24 : UCPSReg<24, "R24">, DwarfRegNum<[24]>;
def R25 : UCPSReg<25, "R25">, DwarfRegNum<[25]>;
def R26 : UCPSReg<26, "R26">, DwarfRegNum<[26]>; 
def R27 : UCPSReg<27, "R27">, DwarfRegNum<[27]>;
def R28 : UCPSReg<28, "R28">, DwarfRegNum<[28]>;
def R29 : UCPSReg<29, "R29">, DwarfRegNum<[29]>; 
def R30 : UCPSReg<30, "R30">, DwarfRegNum<[30]>;
def R31 : UCPSReg<31, "R31">, DwarfRegNum<[31]>;

// UCPS pointer registers, J registers.
def J0  : UCPSReg< 0,  "J0">, DwarfRegNum<[32]>;
def J1  : UCPSReg< 1,  "J1">, DwarfRegNum<[33]>;
def J2  : UCPSReg< 2,  "J2">, DwarfRegNum<[34]>; 
def J3  : UCPSReg< 3,  "J3">, DwarfRegNum<[35]>;
def J4  : UCPSReg< 4,  "J4">, DwarfRegNum<[36]>;
def J5  : UCPSReg< 5,  "J5">, DwarfRegNum<[37]>; 
def J6  : UCPSReg< 6,  "J6">, DwarfRegNum<[38]>;
def J7  : UCPSReg< 7,  "J7">, DwarfRegNum<[39]>;
def J8  : UCPSReg< 8,  "J8">, DwarfRegNum<[40]>; 
def J9  : UCPSReg< 9,  "J9">, DwarfRegNum<[41]>;
def J10 : UCPSReg<10, "J10">, DwarfRegNum<[42]>;
def J11 : UCPSReg<11, "J11">, DwarfRegNum<[43]>; 
def J12 : UCPSReg<12, "J12">, DwarfRegNum<[44]>;
def J13 : UCPSReg<13, "J13">, DwarfRegNum<[45]>;
def J14 : UCPSReg<14, "J14">, DwarfRegNum<[46]>; 
def J15 : UCPSReg<15, "J15">, DwarfRegNum<[47]>;
def J16 : UCPSReg<16, "J16">, DwarfRegNum<[48]>;
def J17 : UCPSReg<17, "J17">, DwarfRegNum<[49]>; 
def J18 : UCPSReg<18, "J18">, DwarfRegNum<[50]>;
def J19 : UCPSReg<19, "J19">, DwarfRegNum<[51]>;
def J20 : UCPSReg<20, "J20">, DwarfRegNum<[52]>; 
def J21 : UCPSReg<21, "J21">, DwarfRegNum<[53]>;
def J22 : UCPSReg<22, "J22">, DwarfRegNum<[54]>;
def J23 : UCPSReg<23, "J23">, DwarfRegNum<[55]>;
def J24 : UCPSReg<24, "J24">, DwarfRegNum<[56]>;
def J25 : UCPSReg<25, "J25">, DwarfRegNum<[57]>;
def J26 : UCPSReg<26, "J26">, DwarfRegNum<[58]>; 
def J27 : UCPSReg<27, "J27">, DwarfRegNum<[59]>;
def J28 : UCPSReg<28, "FP">,  DwarfRegNum<[60]>;
def J29 : UCPSReg<29, "SP">,  DwarfRegNum<[61]>; 
def J30 : UCPSReg<30, "J30">, DwarfRegNum<[62]>;
def J31 : UCPSReg<31, "J31">, DwarfRegNum<[63]>;



// pointer size is defined with data layout, see UCPS subtarget definitions.
def PtrReg : RegisterClass<"UCPSReg", [i32], 32, (sequence "J%u", 0, 31)>;


//****************** register classes mainly for llvm-mc ****************//
def RReg : RegisterClass<"UCPSReg", [i32], 32, (sequence "R%u", 0, 31)>;
def JReg : RegisterClass<"UCPSReg", [i32], 32, (sequence "J%u", 0, 31)>;

// the SVR register , vector register in 512bit width. 
// we model it with an 16 x i32 vector. 
// TODO: Is the DwarfRegNum correct ?
def SVR0 : UCPSReg<0, "SVR0">, DwarfRegNum<[32]>;
def SVR1 : UCPSReg<1, "SVR1">, DwarfRegNum<[32]>;

def SVRReg: RegisterClass<"UCPSReg", [v16i32], 512, (sequence "SVR%u", 0, 1)>;
