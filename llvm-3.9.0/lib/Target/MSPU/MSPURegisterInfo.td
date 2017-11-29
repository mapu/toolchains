//===-- MSPURegisterInfo.td - MSPU Register defs ---------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
//  Declarations that describe the MSPU register file 
//===----------------------------------------------------------------------===//

// n is the AsmName, like 'R23', 'SP'.
// We only specify 'Namespace' here for MSPU registers.
// Use n to parameterize this class.
class MSPUReg< bits<5> num, string n > : Register<n> {
  field bits<5> Num = num;
  let Namespace = "MSPUReg";
}

// MSPU 32-bit integer/floating-point physical registers.
// (no 64-bit integer register supported in MSPU)
def R0  : MSPUReg< 0,  "R0">, DwarfRegNum<[0]>;
def R1  : MSPUReg< 1,  "R1">, DwarfRegNum<[1]>;
def R2  : MSPUReg< 2,  "R2">, DwarfRegNum<[2]>; 
def R3  : MSPUReg< 3,  "R3">, DwarfRegNum<[3]>;
def R4  : MSPUReg< 4,  "R4">, DwarfRegNum<[4]>;
def R5  : MSPUReg< 5,  "R5">, DwarfRegNum<[5]>; 
def R6  : MSPUReg< 6,  "R6">, DwarfRegNum<[6]>;
def R7  : MSPUReg< 7,  "R7">, DwarfRegNum<[7]>;
def R8  : MSPUReg< 8,  "R8">, DwarfRegNum<[8]>; 
def R9  : MSPUReg< 9,  "R9">, DwarfRegNum<[9]>;
def R10 : MSPUReg<10, "R10">, DwarfRegNum<[10]>;
def R11 : MSPUReg<11, "R11">, DwarfRegNum<[11]>; 
def R12 : MSPUReg<12, "R12">, DwarfRegNum<[12]>;
def R13 : MSPUReg<13, "R13">, DwarfRegNum<[13]>;
def R14 : MSPUReg<14, "R14">, DwarfRegNum<[14]>; 
def R15 : MSPUReg<15, "R15">, DwarfRegNum<[15]>;
def R16 : MSPUReg<16, "R16">, DwarfRegNum<[16]>;
def R17 : MSPUReg<17, "R17">, DwarfRegNum<[17]>; 
def R18 : MSPUReg<18, "R18">, DwarfRegNum<[18]>;
def R19 : MSPUReg<19, "R19">, DwarfRegNum<[19]>;
def R20 : MSPUReg<20, "R20">, DwarfRegNum<[20]>; 
def R21 : MSPUReg<21, "R21">, DwarfRegNum<[21]>;
def R22 : MSPUReg<22, "R22">, DwarfRegNum<[22]>;
def R23 : MSPUReg<23, "R23">, DwarfRegNum<[23]>;
def R24 : MSPUReg<24, "R24">, DwarfRegNum<[24]>;
def R25 : MSPUReg<25, "R25">, DwarfRegNum<[25]>;
def R26 : MSPUReg<26, "R26">, DwarfRegNum<[26]>; 
def R27 : MSPUReg<27, "R27">, DwarfRegNum<[27]>;
def R28 : MSPUReg<28, "R28">, DwarfRegNum<[28]>;
def R29 : MSPUReg<29, "R29">, DwarfRegNum<[29]>; 
def R30 : MSPUReg<30, "R30">, DwarfRegNum<[30]>;
def R31 : MSPUReg<31, "R31">, DwarfRegNum<[31]>;

// MSPU pointer registers, J registers.
def J0  : MSPUReg< 0,  "J0">, DwarfRegNum<[32]>;
def J1  : MSPUReg< 1,  "J1">, DwarfRegNum<[33]>;
def J2  : MSPUReg< 2,  "J2">, DwarfRegNum<[34]>; 
def J3  : MSPUReg< 3,  "J3">, DwarfRegNum<[35]>;
def J4  : MSPUReg< 4,  "J4">, DwarfRegNum<[36]>;
def J5  : MSPUReg< 5,  "J5">, DwarfRegNum<[37]>; 
def J6  : MSPUReg< 6,  "J6">, DwarfRegNum<[38]>;
def J7  : MSPUReg< 7,  "J7">, DwarfRegNum<[39]>;
def J8  : MSPUReg< 8,  "J8">, DwarfRegNum<[40]>; 
def J9  : MSPUReg< 9,  "J9">, DwarfRegNum<[41]>;
def J10 : MSPUReg<10, "J10">, DwarfRegNum<[42]>;
def J11 : MSPUReg<11, "J11">, DwarfRegNum<[43]>; 
def J12 : MSPUReg<12, "J12">, DwarfRegNum<[44]>;
def J13 : MSPUReg<13, "J13">, DwarfRegNum<[45]>;
def J14 : MSPUReg<14, "J14">, DwarfRegNum<[46]>; 
def J15 : MSPUReg<15, "J15">, DwarfRegNum<[47]>;
def J16 : MSPUReg<16, "J16">, DwarfRegNum<[48]>;
def J17 : MSPUReg<17, "J17">, DwarfRegNum<[49]>; 
def J18 : MSPUReg<18, "J18">, DwarfRegNum<[50]>;
def J19 : MSPUReg<19, "J19">, DwarfRegNum<[51]>;
def J20 : MSPUReg<20, "J20">, DwarfRegNum<[52]>; 
def J21 : MSPUReg<21, "J21">, DwarfRegNum<[53]>;
def J22 : MSPUReg<22, "J22">, DwarfRegNum<[54]>;
def J23 : MSPUReg<23, "J23">, DwarfRegNum<[55]>;
def J24 : MSPUReg<24, "J24">, DwarfRegNum<[56]>;
def J25 : MSPUReg<25, "J25">, DwarfRegNum<[57]>;
def J26 : MSPUReg<26, "J26">, DwarfRegNum<[58]>; 
def J27 : MSPUReg<27, "J27">, DwarfRegNum<[59]>;
def J28 : MSPUReg<28, "FP">,  DwarfRegNum<[60]>;
def J29 : MSPUReg<29, "SP">,  DwarfRegNum<[61]>; 
def J30 : MSPUReg<30, "J30">, DwarfRegNum<[62]>;
def J31 : MSPUReg<31, "J31">, DwarfRegNum<[63]>;

// Use instances of SubRegIndex to identify subregisters.
// tbl class SubRegIndex is very much like tbl class RegAltNameIndex.
// we use defs of them to mark things.
// I guess tablegen will generate enum type for them.
let Namespace = "MSPUReg" in {
	def lo : SubRegIndex<32>;
	def hi : SubRegIndex<32>;
}

/*
// register class for 32-bit floating-point numbers
class Reg32bit< bits<5> num, string n, list<Register> subs = [] > : MSPUReg<num, n> {
  let SubRegs = subs; // 'SubRegs': tbl class 'Register' attribute
  let SubRegIndices = [lo];
  let CoveredBySubRegs = 1; // 'Register' value is determined by its subregisters.
}

// these are synthetic registers composed of one single R-register.
def S0  : Reg32bit< 0, "R0",  [R0]>, DwarfRegNum<[64]>;
def S1  : Reg32bit< 1, "R1",  [R1]>, DwarfRegNum<[65]>;
def S2  : Reg32bit< 2, "R2",  [R2]>, DwarfRegNum<[66]>;
def S3  : Reg32bit< 3, "R3",  [R3]>, DwarfRegNum<[67]>;
def S4  : Reg32bit< 4, "R4",  [R4]>, DwarfRegNum<[68]>;
def S5  : Reg32bit< 5, "R5",  [R5]>, DwarfRegNum<[69]>;
def S6  : Reg32bit< 6, "R6",  [R6]>, DwarfRegNum<[70]>;
def S7  : Reg32bit< 7, "R7",  [R7]>, DwarfRegNum<[71]>;
def S8  : Reg32bit< 8, "R8",  [R8]>, DwarfRegNum<[72]>;
def S9  : Reg32bit< 9, "R9",  [R9]>, DwarfRegNum<[73]>;
def S10 : Reg32bit<10, "R10", [R10]>, DwarfRegNum<[74]>;
def S11 : Reg32bit<11, "R11", [R11]>, DwarfRegNum<[75]>;
def S12 : Reg32bit<12, "R12", [R12]>, DwarfRegNum<[76]>;
def S13 : Reg32bit<13, "R13", [R13]>, DwarfRegNum<[77]>;
def S14 : Reg32bit<14, "R14", [R14]>, DwarfRegNum<[78]>;
def S15 : Reg32bit<15, "R15", [R15]>, DwarfRegNum<[79]>;
def S16 : Reg32bit<16, "R16", [R16]>, DwarfRegNum<[80]>;
def S17 : Reg32bit<17, "R17", [R17]>, DwarfRegNum<[81]>;
def S18 : Reg32bit<18, "R18", [R18]>, DwarfRegNum<[82]>;
def S19 : Reg32bit<19, "R19", [R19]>, DwarfRegNum<[83]>;
def S20 : Reg32bit<20, "R20", [R20]>, DwarfRegNum<[84]>;
def S21 : Reg32bit<21, "R21", [R21]>, DwarfRegNum<[85]>;
def S22 : Reg32bit<22, "R22", [R22]>, DwarfRegNum<[86]>;
def S23 : Reg32bit<23, "R23", [R23]>, DwarfRegNum<[87]>;
def S24 : Reg32bit<24, "R24", [R24]>, DwarfRegNum<[88]>;
def S25 : Reg32bit<25, "R25", [R25]>, DwarfRegNum<[89]>;
def S26 : Reg32bit<26, "R26", [R26]>, DwarfRegNum<[90]>;
def S27 : Reg32bit<27, "R27", [R27]>, DwarfRegNum<[91]>;
def S28 : Reg32bit<28, "R28", [R28]>, DwarfRegNum<[92]>;
def S29 : Reg32bit<29, "R29", [R29]>, DwarfRegNum<[93]>;
def S30 : Reg32bit<30, "R30", [R30]>, DwarfRegNum<[94]>;
def S31 : Reg32bit<31, "R31", [R31]>, DwarfRegNum<[95]>;
*/

// Reg64bit - 64-bit register class for MaPU 64-bit double-precise fp registers.
class Reg64bit< bits<5> num, string n, list<Register> subs = [] > : MSPUReg<num, n> {
  let SubRegs = subs; // 'SubRegs': tbl class 'Register' attribute
  let SubRegIndices = [lo, hi];
  let CoveredBySubRegs = 1; // 'Register' value is determined by its subregisters.
}

// floating-point registers for 64-bit fp values (doubles).
// These are groups of Rx's.
// the coding of D8 should be 16, since this will spare some
// effort for the hardware instruction decoder.
// for now the coding value does not matter at all and might
// be used later.
def D0  : Reg64bit< 0, "DR0",  [R0,   R1]>, DwarfRegNum<[96]>;
def D1  : Reg64bit< 2, "DR1",  [R2,   R3]>, DwarfRegNum<[97]>;
def D2  : Reg64bit< 4, "DR2",  [R4,   R5]>, DwarfRegNum<[98]>;
def D3  : Reg64bit< 6, "DR3",  [R6,   R7]>, DwarfRegNum<[99]>;
def D4  : Reg64bit< 8, "DR4",  [R8,   R9]>, DwarfRegNum<[100]>;
def D5  : Reg64bit<10, "DR5",  [R10, R11]>, DwarfRegNum<[101]>;
def D6  : Reg64bit<12, "DR6",  [R12, R13]>, DwarfRegNum<[102]>;
def D7  : Reg64bit<14, "DR7",  [R14, R15]>, DwarfRegNum<[103]>;
def D8  : Reg64bit<16, "DR8",  [R16, R17]>, DwarfRegNum<[104]>;
def D9  : Reg64bit<18, "DR9",  [R18, R19]>, DwarfRegNum<[105]>;
def D10 : Reg64bit<20, "DR10", [R20, R21]>, DwarfRegNum<[106]>;
def D11 : Reg64bit<22, "DR11", [R22, R23]>, DwarfRegNum<[107]>;
def D12 : Reg64bit<24, "DR12", [R24, R25]>, DwarfRegNum<[108]>;
def D13 : Reg64bit<26, "DR13", [R26, R27]>, DwarfRegNum<[109]>;
def D14 : Reg64bit<28, "DR14", [R28, R29]>, DwarfRegNum<[110]>;
def D15 : Reg64bit<30, "DR15", [R30, R31]>, DwarfRegNum<[111]>;

//**************** synthetic register classes mainly for llc *************//
def I1Reg : RegisterClass<"MSPUReg", [i1], 1, (sequence "R%u", 0, 31)>;
def I32Reg : RegisterClass<"MSPUReg", [i32], 32, (sequence "R%u", 0, 31)>;
def F32Reg : RegisterClass<"MSPUReg", [f32], 32, (sequence "R%u", 0, 31)>;
def F64Reg : RegisterClass<"MSPUReg", [f64], 64, (sequence "D%u", 0, 15)>;

// pointer size is defined with data layout, see MSPU subtarget definitions.
def PtrReg : RegisterClass<"MSPUReg", [i32], 32, (sequence "J%u", 0, 31)>;


//****************** register classes mainly for llvm-mc ****************//
def RReg : RegisterClass<"MSPUReg", [i32], 32, (sequence "R%u", 0, 31)>;
def JReg : RegisterClass<"MSPUReg", [i32], 32, (sequence "J%u", 0, 31)>;
def DReg : RegisterClass<"MSPUReg", [f64], 64, (sequence "D%u", 0, 15)>;

// Registers for SYN unit.
def KB0  : MSPUReg< 0,  "KB0">, DwarfRegNum<[32]>;
def KB1  : MSPUReg< 1,  "KB1">, DwarfRegNum<[33]>;
def KB2  : MSPUReg< 2,  "KB2">, DwarfRegNum<[34]>; 
def KB3  : MSPUReg< 3,  "KB3">, DwarfRegNum<[35]>;
def KB4  : MSPUReg< 4,  "KB4">, DwarfRegNum<[36]>;
def KB5  : MSPUReg< 5,  "KB5">, DwarfRegNum<[37]>; 
def KB6  : MSPUReg< 6,  "KB6">, DwarfRegNum<[38]>;
def KB7  : MSPUReg< 7,  "KB7">, DwarfRegNum<[39]>;
def KB8  : MSPUReg< 8,  "KB8">, DwarfRegNum<[40]>; 
def KB9  : MSPUReg< 9,  "KB9">, DwarfRegNum<[41]>;
def KB10 : MSPUReg<10, "KB10">, DwarfRegNum<[42]>;
def KB11 : MSPUReg<11, "KB11">, DwarfRegNum<[43]>; 
def KB12 : MSPUReg<12, "KB12">, DwarfRegNum<[44]>;
def KB13 : MSPUReg<13, "KB13">, DwarfRegNum<[45]>;
def KB14 : MSPUReg<14, "KB14">, DwarfRegNum<[46]>; 
def KB15 : MSPUReg<15, "KB15">, DwarfRegNum<[47]>;

def KBReg : RegisterClass<"MSPUReg", [i32], 32, (sequence "KB%u", 0, 15)>;

def KE0  : MSPUReg< 0,  "KE0">, DwarfRegNum<[32]>;
def KE1  : MSPUReg< 1,  "KE1">, DwarfRegNum<[33]>;
def KE2  : MSPUReg< 2,  "KE2">, DwarfRegNum<[34]>; 
def KE3  : MSPUReg< 3,  "KE3">, DwarfRegNum<[35]>;
def KE4  : MSPUReg< 4,  "KE4">, DwarfRegNum<[36]>;
def KE5  : MSPUReg< 5,  "KE5">, DwarfRegNum<[37]>; 
def KE6  : MSPUReg< 6,  "KE6">, DwarfRegNum<[38]>;
def KE7  : MSPUReg< 7,  "KE7">, DwarfRegNum<[39]>;
def KE8  : MSPUReg< 8,  "KE8">, DwarfRegNum<[40]>; 
def KE9  : MSPUReg< 9,  "KE9">, DwarfRegNum<[41]>;
def KE10 : MSPUReg<10, "KE10">, DwarfRegNum<[42]>;
def KE11 : MSPUReg<11, "KE11">, DwarfRegNum<[43]>; 
def KE12 : MSPUReg<12, "KE12">, DwarfRegNum<[44]>;
def KE13 : MSPUReg<13, "KE13">, DwarfRegNum<[45]>;
def KE14 : MSPUReg<14, "KE14">, DwarfRegNum<[46]>; 
def KE15 : MSPUReg<15, "KE15">, DwarfRegNum<[47]>;
def KEReg : RegisterClass<"MSPUReg", [i32], 32, (sequence "KE%u", 0, 15)>;

def KI0  : MSPUReg< 0,  "KI0">, DwarfRegNum<[32]>;
def KI1  : MSPUReg< 1,  "KI1">, DwarfRegNum<[33]>;
def KI2  : MSPUReg< 2,  "KI2">, DwarfRegNum<[34]>; 
def KI3  : MSPUReg< 3,  "KI3">, DwarfRegNum<[35]>;
def KI4  : MSPUReg< 4,  "KI4">, DwarfRegNum<[36]>;
def KI5  : MSPUReg< 5,  "KI5">, DwarfRegNum<[37]>; 
def KI6  : MSPUReg< 6,  "KI6">, DwarfRegNum<[38]>;
def KI7  : MSPUReg< 7,  "KI7">, DwarfRegNum<[39]>;
def KI8  : MSPUReg< 8,  "KI8">, DwarfRegNum<[40]>; 
def KI9  : MSPUReg< 9,  "KI9">, DwarfRegNum<[41]>;
def KI10 : MSPUReg<10, "KI10">, DwarfRegNum<[42]>;
def KI11 : MSPUReg<11, "KI11">, DwarfRegNum<[43]>; 
def KI12 : MSPUReg<12, "KI12">, DwarfRegNum<[44]>;
def KI13 : MSPUReg<13, "KI13">, DwarfRegNum<[45]>;
def KI14 : MSPUReg<14, "KI14">, DwarfRegNum<[46]>; 
def KI15 : MSPUReg<15, "KI15">, DwarfRegNum<[47]>;

def KIReg : RegisterClass<"MSPUReg", [i32], 32, (sequence "KI%u", 0, 15)>;

def KS0  : MSPUReg< 0,  "KS0">, DwarfRegNum<[32]>;
def KS1  : MSPUReg< 1,  "KS1">, DwarfRegNum<[33]>;
def KS2  : MSPUReg< 2,  "KS2">, DwarfRegNum<[34]>; 
def KS3  : MSPUReg< 3,  "KS3">, DwarfRegNum<[35]>;
def KS4  : MSPUReg< 4,  "KS4">, DwarfRegNum<[36]>;
def KS5  : MSPUReg< 5,  "KS5">, DwarfRegNum<[37]>; 
def KS6  : MSPUReg< 6,  "KS6">, DwarfRegNum<[38]>;
def KS7  : MSPUReg< 7,  "KS7">, DwarfRegNum<[39]>;
def KS8  : MSPUReg< 8,  "KS8">, DwarfRegNum<[40]>; 
def KS9  : MSPUReg< 9,  "KS9">, DwarfRegNum<[41]>;
def KS10 : MSPUReg<10, "KS10">, DwarfRegNum<[42]>;
def KS11 : MSPUReg<11, "KS11">, DwarfRegNum<[43]>; 
def KS12 : MSPUReg<12, "KS12">, DwarfRegNum<[44]>;
def KS13 : MSPUReg<13, "KS13">, DwarfRegNum<[45]>;
def KS14 : MSPUReg<14, "KS14">, DwarfRegNum<[46]>; 
def KS15 : MSPUReg<15, "KS15">, DwarfRegNum<[47]>;

def KSReg : RegisterClass<"MSPUReg", [i32], 32, (sequence "KS%u", 0, 15)>;

def KG0  : MSPUReg< 0,  "KG0">, DwarfRegNum<[32]>;
def KG1  : MSPUReg< 1,  "KG1">, DwarfRegNum<[33]>;
def KG2  : MSPUReg< 2,  "KG2">, DwarfRegNum<[34]>;

def KGReg : RegisterClass<"MSPUReg", [i32], 32, (sequence "KG%u", 0, 2)>;

def KL0  : MSPUReg< 0,  "KL0">, DwarfRegNum<[32]>;
def KL1  : MSPUReg< 1,  "KL1">, DwarfRegNum<[33]>;
def KL2  : MSPUReg< 2,  "KL2">, DwarfRegNum<[34]>;

def KLReg : RegisterClass<"MSPUReg", [i32], 32, (sequence "KL%u", 0, 2)>;

def KM0  : MSPUReg< 0,  "KM0">, DwarfRegNum<[32]>;
def KM1  : MSPUReg< 1,  "KM1">, DwarfRegNum<[33]>;
def KM2  : MSPUReg< 2,  "KM2">, DwarfRegNum<[34]>;

def KMReg : RegisterClass<"MSPUReg", [i32], 32, (sequence "KM%u", 0, 2)>;

// the SVR register , vector register in 512bit width. 
// we model it with an 16 x i32 vector. 
// TODO: Is the DwarfRegNum correct ?
def SVR0 : MSPUReg<0, "SVR0">, DwarfRegNum<[32]>;
def SVR1 : MSPUReg<1, "SVR1">, DwarfRegNum<[32]>;

def SVRReg: RegisterClass<"MSPUReg", [v16i32], 512, (sequence "SVR%u", 0, 1)>;

// the SHU selector. 
// we model it with an i1 register. 
// TODO: Does the SHU seclector can be modeled is this way ?
def SHU0 : MSPUReg<0, "SHU0">, DwarfRegNum<[32]>;
def SHU1 : MSPUReg<1, "SHU1">, DwarfRegNum<[32]>;

def SHUReg: RegisterClass<"MSPUReg", [i1], 1, (sequence "SHU%u", 0, 1)>;

// the T register , vector register in 512bit width. 
// we model it with an 32 x i32 vector. 
// TODO: Is the DwarfRegNum correct ?
def T0 : MSPUReg<0, "T0">, DwarfRegNum<[32]>;
def T1 : MSPUReg<1, "T1">, DwarfRegNum<[32]>;
def T2 : MSPUReg<2, "T2">, DwarfRegNum<[32]>;
def T3 : MSPUReg<3, "T3">, DwarfRegNum<[32]>;

def TReg: RegisterClass<"MSPUReg", [v16i32], 512, (sequence "T%u", 0, 3)>;

def MC0 : MSPUReg<0, "MC.r0">, DwarfRegNum<[32]>;
def MC1 : MSPUReg<1, "MC.r1">, DwarfRegNum<[32]>;
def MC2 : MSPUReg<2, "MC.w0">, DwarfRegNum<[32]>;
def MC3 : MSPUReg<3, "MC.w1">, DwarfRegNum<[32]>;
def MC4 : MSPUReg<4, "MC.w2">, DwarfRegNum<[32]>;
def MCReg: RegisterClass<"MSPUReg", [i32], 32, (sequence "MC%u", 0, 4)>;
