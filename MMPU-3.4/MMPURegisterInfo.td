//===-- MMPURegisterInfo.td - MMPU Register defs ---------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
//  Declarations that describe the MMPU register file 
//===----------------------------------------------------------------------===//

// n is the AsmName, like 'R23', 'SP'.
// We only specify 'Namespace' here for MMPU registers.
// Use n to parameterize this class.
class MMPUReg< bits<8> num, string n > : Register<n> {
  field bits<8> Num = num;

  // fixme: this namespace must be the same with that for RegisterClass
  // 		definition, which is an unnecessary constraint.
  let Namespace = "MMPUReg";
}

// The encoding for Mx is "I++ (1 bits) - index (7 bits)"
def M0   : MMPUReg< 0  , "M0" >, DwarfRegNum< [0  ] >;
def M1   : MMPUReg< 1  , "M1" >, DwarfRegNum< [1  ] >;
def M2   : MMPUReg< 2  , "M2" >, DwarfRegNum< [2  ] >;
def M3   : MMPUReg< 3  , "M3" >, DwarfRegNum< [3  ] >;
def M4   : MMPUReg< 4  , "M4" >, DwarfRegNum< [4  ] >;
def M5   : MMPUReg< 5  , "M5" >, DwarfRegNum< [5  ] >;
def M6   : MMPUReg< 6  , "M6" >, DwarfRegNum< [6  ] >;
def M7   : MMPUReg< 7  , "M7" >, DwarfRegNum< [7  ] >;
def M8   : MMPUReg< 8  , "M8" >, DwarfRegNum< [8  ] >;
def M9   : MMPUReg< 9  , "M9" >, DwarfRegNum< [9  ] >;
def M10  : MMPUReg< 10 , "M10" >, DwarfRegNum< [10 ] >;
def M11  : MMPUReg< 11 , "M11" >, DwarfRegNum< [11 ] >;
def M12  : MMPUReg< 12 , "M12" >, DwarfRegNum< [12 ] >;
def M13  : MMPUReg< 13 , "M13" >, DwarfRegNum< [13 ] >;
def M14  : MMPUReg< 14 , "M14" >, DwarfRegNum< [14 ] >;
def M15  : MMPUReg< 15 , "M15" >, DwarfRegNum< [15 ] >;
def M16  : MMPUReg< 16 , "M16" >, DwarfRegNum< [16 ] >;
def M17  : MMPUReg< 17 , "M17" >, DwarfRegNum< [17 ] >;
def M18  : MMPUReg< 18 , "M18" >, DwarfRegNum< [18 ] >;
def M19  : MMPUReg< 19 , "M19" >, DwarfRegNum< [19 ] >;
def M20  : MMPUReg< 20 , "M20" >, DwarfRegNum< [20 ] >;
def M21  : MMPUReg< 21 , "M21" >, DwarfRegNum< [21 ] >;
def M22  : MMPUReg< 22 , "M22" >, DwarfRegNum< [22 ] >;
def M23  : MMPUReg< 23 , "M23" >, DwarfRegNum< [23 ] >;
def M24  : MMPUReg< 24 , "M24" >, DwarfRegNum< [24 ] >;
def M25  : MMPUReg< 25 , "M25" >, DwarfRegNum< [25 ] >;
def M26  : MMPUReg< 26 , "M26" >, DwarfRegNum< [26 ] >;
def M27  : MMPUReg< 27 , "M27" >, DwarfRegNum< [27 ] >;
def M28  : MMPUReg< 28 , "M28" >, DwarfRegNum< [28 ] >;
def M29  : MMPUReg< 29 , "M29" >, DwarfRegNum< [29 ] >;
def M30  : MMPUReg< 30 , "M30" >, DwarfRegNum< [30 ] >;
def M31  : MMPUReg< 31 , "M31" >, DwarfRegNum< [31 ] >;
def M32  : MMPUReg< 32 , "M32" >, DwarfRegNum< [32 ] >;
def M33  : MMPUReg< 33 , "M33" >, DwarfRegNum< [33 ] >;
def M34  : MMPUReg< 34 , "M34" >, DwarfRegNum< [34 ] >;
def M35  : MMPUReg< 35 , "M35" >, DwarfRegNum< [35 ] >;
def M36  : MMPUReg< 36 , "M36" >, DwarfRegNum< [36 ] >;
def M37  : MMPUReg< 37 , "M37" >, DwarfRegNum< [37 ] >;
def M38  : MMPUReg< 38 , "M38" >, DwarfRegNum< [38 ] >;
def M39  : MMPUReg< 39 , "M39" >, DwarfRegNum< [39 ] >;
def M40  : MMPUReg< 40 , "M40" >, DwarfRegNum< [40 ] >;
def M41  : MMPUReg< 41 , "M41" >, DwarfRegNum< [41 ] >;
def M42  : MMPUReg< 42 , "M42" >, DwarfRegNum< [42 ] >;
def M43  : MMPUReg< 43 , "M43" >, DwarfRegNum< [43 ] >;
def M44  : MMPUReg< 44 , "M44" >, DwarfRegNum< [44 ] >;
def M45  : MMPUReg< 45 , "M45" >, DwarfRegNum< [45 ] >;
def M46  : MMPUReg< 46 , "M46" >, DwarfRegNum< [46 ] >;
def M47  : MMPUReg< 47 , "M47" >, DwarfRegNum< [47 ] >;
def M48  : MMPUReg< 48 , "M48" >, DwarfRegNum< [48 ] >;
def M49  : MMPUReg< 49 , "M49" >, DwarfRegNum< [49 ] >;
def M50  : MMPUReg< 50 , "M50" >, DwarfRegNum< [50 ] >;
def M51  : MMPUReg< 51 , "M51" >, DwarfRegNum< [51 ] >;
def M52  : MMPUReg< 52 , "M52" >, DwarfRegNum< [52 ] >;
def M53  : MMPUReg< 53 , "M53" >, DwarfRegNum< [53 ] >;
def M54  : MMPUReg< 54 , "M54" >, DwarfRegNum< [54 ] >;
def M55  : MMPUReg< 55 , "M55" >, DwarfRegNum< [55 ] >;
def M56  : MMPUReg< 56 , "M56" >, DwarfRegNum< [56 ] >;
def M57  : MMPUReg< 57 , "M57" >, DwarfRegNum< [57 ] >;
def M58  : MMPUReg< 58 , "M58" >, DwarfRegNum< [58 ] >;
def M59  : MMPUReg< 59 , "M59" >, DwarfRegNum< [59 ] >;
def M60  : MMPUReg< 60 , "M60" >, DwarfRegNum< [60 ] >;
def M61  : MMPUReg< 61 , "M61" >, DwarfRegNum< [61 ] >;
def M62  : MMPUReg< 62 , "M62" >, DwarfRegNum< [62 ] >;
def M63  : MMPUReg< 63 , "M63" >, DwarfRegNum< [63 ] >;
def M64  : MMPUReg< 64 , "M64" >, DwarfRegNum< [64 ] >;
def M65  : MMPUReg< 65 , "M65" >, DwarfRegNum< [65 ] >;
def M66  : MMPUReg< 66 , "M66" >, DwarfRegNum< [66 ] >;
def M67  : MMPUReg< 67 , "M67" >, DwarfRegNum< [67 ] >;
def M68  : MMPUReg< 68 , "M68" >, DwarfRegNum< [68 ] >;
def M69  : MMPUReg< 69 , "M69" >, DwarfRegNum< [69 ] >;
def M70  : MMPUReg< 70 , "M70" >, DwarfRegNum< [70 ] >;
def M71  : MMPUReg< 71 , "M71" >, DwarfRegNum< [71 ] >;
def M72  : MMPUReg< 72 , "M72" >, DwarfRegNum< [72 ] >;
def M73  : MMPUReg< 73 , "M73" >, DwarfRegNum< [73 ] >;
def M74  : MMPUReg< 74 , "M74" >, DwarfRegNum< [74 ] >;
def M75  : MMPUReg< 75 , "M75" >, DwarfRegNum< [75 ] >;
def M76  : MMPUReg< 76 , "M76" >, DwarfRegNum< [76 ] >;
def M77  : MMPUReg< 77 , "M77" >, DwarfRegNum< [77 ] >;
def M78  : MMPUReg< 78 , "M78" >, DwarfRegNum< [78 ] >;
def M79  : MMPUReg< 79 , "M79" >, DwarfRegNum< [79 ] >;
def M80  : MMPUReg< 80 , "M80" >, DwarfRegNum< [80 ] >;
def M81  : MMPUReg< 81 , "M81" >, DwarfRegNum< [81 ] >;
def M82  : MMPUReg< 82 , "M82" >, DwarfRegNum< [82 ] >;
def M83  : MMPUReg< 83 , "M83" >, DwarfRegNum< [83 ] >;
def M84  : MMPUReg< 84 , "M84" >, DwarfRegNum< [84 ] >;
def M85  : MMPUReg< 85 , "M85" >, DwarfRegNum< [85 ] >;
def M86  : MMPUReg< 86 , "M86" >, DwarfRegNum< [86 ] >;
def M87  : MMPUReg< 87 , "M87" >, DwarfRegNum< [87 ] >;
def M88  : MMPUReg< 88 , "M88" >, DwarfRegNum< [88 ] >;
def M89  : MMPUReg< 89 , "M89" >, DwarfRegNum< [89 ] >;
def M90  : MMPUReg< 90 , "M90" >, DwarfRegNum< [90 ] >;
def M91  : MMPUReg< 91 , "M91" >, DwarfRegNum< [91 ] >;
def M92  : MMPUReg< 92 , "M92" >, DwarfRegNum< [92 ] >;
def M93  : MMPUReg< 93 , "M93" >, DwarfRegNum< [93 ] >;
def M94  : MMPUReg< 94 , "M94" >, DwarfRegNum< [94 ] >;
def M95  : MMPUReg< 95 , "M95" >, DwarfRegNum< [95 ] >;
def M96  : MMPUReg< 96 , "M96" >, DwarfRegNum< [96 ] >;
def M97  : MMPUReg< 97 , "M97" >, DwarfRegNum< [97 ] >;
def M98  : MMPUReg< 98 , "M98" >, DwarfRegNum< [98 ] >;
def M99  : MMPUReg< 99 , "M99" >, DwarfRegNum< [99 ] >;
def M100 : MMPUReg< 100, "M100" >, DwarfRegNum< [100] >;
def M101 : MMPUReg< 101, "M101" >, DwarfRegNum< [101] >;
def M102 : MMPUReg< 102, "M102" >, DwarfRegNum< [102] >;
def M103 : MMPUReg< 103, "M103" >, DwarfRegNum< [103] >;
def M104 : MMPUReg< 104, "M104" >, DwarfRegNum< [104] >;
def M105 : MMPUReg< 105, "M105" >, DwarfRegNum< [105] >;
def M106 : MMPUReg< 106, "M106" >, DwarfRegNum< [106] >;
def M107 : MMPUReg< 107, "M107" >, DwarfRegNum< [107] >;
def M108 : MMPUReg< 108, "M108" >, DwarfRegNum< [108] >;
def M109 : MMPUReg< 109, "M109" >, DwarfRegNum< [109] >;
def M110 : MMPUReg< 110, "M110" >, DwarfRegNum< [110] >;
def M111 : MMPUReg< 111, "M111" >, DwarfRegNum< [111] >;
def M112 : MMPUReg< 112, "M112" >, DwarfRegNum< [112] >;
def M113 : MMPUReg< 113, "M113" >, DwarfRegNum< [113] >;
def M114 : MMPUReg< 114, "M114" >, DwarfRegNum< [114] >;
def M115 : MMPUReg< 115, "M115" >, DwarfRegNum< [115] >;
def M116 : MMPUReg< 116, "M116" >, DwarfRegNum< [116] >;
def M117 : MMPUReg< 117, "M117" >, DwarfRegNum< [117] >;
def M118 : MMPUReg< 118, "M118" >, DwarfRegNum< [118] >;
def M119 : MMPUReg< 119, "M119" >, DwarfRegNum< [119] >;
def M120 : MMPUReg< 120, "M120" >, DwarfRegNum< [120] >;
def M121 : MMPUReg< 121, "M121" >, DwarfRegNum< [121] >;
def M122 : MMPUReg< 122, "M122" >, DwarfRegNum< [122] >;
def M123 : MMPUReg< 123, "M123" >, DwarfRegNum< [123] >;
def M124 : MMPUReg< 124, "M124" >, DwarfRegNum< [124] >;
def M125 : MMPUReg< 125, "M125" >, DwarfRegNum< [125] >;
def M126 : MMPUReg< 126, "M126" >, DwarfRegNum< [126] >;
def M127 : MMPUReg< 127, "M127" >, DwarfRegNum< [127] >;
// We treat M[I++], M[S++] as a special register.
def MIPP : MMPUReg< 128, "M[I++]">, DwarfRegNum<[128]>;
def MSPP : MMPUReg< 129, "M[S++]">, DwarfRegNum<[129]>;

def r0  : MMPUReg< 0,  "r0">, DwarfRegNum<[129]>;
def r1  : MMPUReg< 1,  "r1">, DwarfRegNum<[129]>;
def r2  : MMPUReg< 2,  "r2">, DwarfRegNum<[129]>;
def r3  : MMPUReg< 3,  "r3">, DwarfRegNum<[129]>;

def w0  : MMPUReg< 0,  "w0">, DwarfRegNum<[129]>;
def w1  : MMPUReg< 1,  "w1">, DwarfRegNum<[129]>;
def w2  : MMPUReg< 2,  "w2">, DwarfRegNum<[129]>;
def w3  : MMPUReg< 3,  "w3">, DwarfRegNum<[129]>;

/*********************************************************/
// We mimic some function units as registers since they affect encoding.
def Shu0  : MMPUReg< 0b0100,  "SHU0">, DwarfRegNum<[129]>;
def Shu1  : MMPUReg< 0b0101,  "SHU1">, DwarfRegNum<[130]>;

def IALU  : MMPUReg< 0b1100,  "IALU">, DwarfRegNum<[134]>;
def IMAC  : MMPUReg< 0b1101,  "IMAC">, DwarfRegNum<[135]>;
def FALU  : MMPUReg< 0b1110,  "FALU">, DwarfRegNum<[136]>;
def FMAC  : MMPUReg< 0b1111,  "FMAC">, DwarfRegNum<[137]>;

def T0  : MMPUReg< 0,  "T0">, DwarfRegNum<[129]>;
def T1  : MMPUReg< 1,  "T1">, DwarfRegNum<[129]>;
def T2  : MMPUReg< 2,  "T2">, DwarfRegNum<[129]>;
def T3  : MMPUReg< 3,  "T3">, DwarfRegNum<[129]>;

def TBB  : MMPUReg< 0,  "TBB">, DwarfRegNum<[129]>;
def TBH  : MMPUReg< 1,  "TBH">, DwarfRegNum<[129]>;
def TBW  : MMPUReg< 2,  "TBW">, DwarfRegNum<[129]>;
def TBD  : MMPUReg< 3,  "TBD">, DwarfRegNum<[129]>;
def TSQ  : MMPUReg< 0,  "TSQ">, DwarfRegNum<[129]>;

/*********************************************************/
def BIU0  : MMPUReg< 0b1000,  "BIU0">, DwarfRegNum<[131]>;
def BIU1  : MMPUReg< 0b1001,  "BIU1">, DwarfRegNum<[132]>;
def BIU2  : MMPUReg< 0b1010,  "BIU2">, DwarfRegNum<[133]>;

/*********************************************************/
def KI0  : MMPUReg< 0,  "KI0">, DwarfRegNum<[32]>;
def KI1  : MMPUReg< 1,  "KI1">, DwarfRegNum<[33]>;
def KI2  : MMPUReg< 2,  "KI2">, DwarfRegNum<[34]>; 
def KI3  : MMPUReg< 3,  "KI3">, DwarfRegNum<[35]>;
def KI4  : MMPUReg< 4,  "KI4">, DwarfRegNum<[36]>;
def KI5  : MMPUReg< 5,  "KI5">, DwarfRegNum<[37]>; 
def KI6  : MMPUReg< 6,  "KI6">, DwarfRegNum<[38]>;
def KI7  : MMPUReg< 7,  "KI7">, DwarfRegNum<[39]>;
def KI8  : MMPUReg< 8,  "KI8">, DwarfRegNum<[40]>; 
def KI9  : MMPUReg< 9,  "KI9">, DwarfRegNum<[41]>;
def KI10 : MMPUReg<10, "KI10">, DwarfRegNum<[42]>;
def KI11 : MMPUReg<11, "KI11">, DwarfRegNum<[43]>; 
def KI12 : MMPUReg<12, "KI12">, DwarfRegNum<[44]>;
def KI13 : MMPUReg<13, "KI13">, DwarfRegNum<[45]>;
def KI14 : MMPUReg<14, "KI14">, DwarfRegNum<[46]>; 
def KI15 : MMPUReg<15, "KI15">, DwarfRegNum<[47]>;

/*********************************************************/
def I0  : MMPUReg< 0,  "(I0)">, DwarfRegNum<[129]>;
def I1  : MMPUReg< 1,  "(I1)">, DwarfRegNum<[129]>;
def I2  : MMPUReg< 2,  "(I2)">, DwarfRegNum<[129]>;

/*********************************************************/
def WF1  : MMPUReg< 0,  "(WF1)">, DwarfRegNum<[129]>;
def WF2  : MMPUReg< 0,  "(WF2)">, DwarfRegNum<[129]>;
def WF3  : MMPUReg< 0,  "(WF3)">, DwarfRegNum<[129]>;

def WX1  : MMPUReg< 0,  "(WX1)">, DwarfRegNum<[129]>;
def WX2  : MMPUReg< 0,  "(WX2)">, DwarfRegNum<[129]>;
def WX3  : MMPUReg< 0,  "(WX3)">, DwarfRegNum<[129]>;
def WX4  : MMPUReg< 0,  "(WX4)">, DwarfRegNum<[129]>;
/*
def WFlag : RegisterClass<"MMPUReg", [v16i32], 32,
			 			 (add WF1, WF2, WF3, WX1, WX2, WX3, WX4)>;
*/


//===------------------------------------------------------------------===//
//							register classes
//===------------------------------------------------------------------===//

/* 
class RegisterClass<string namespace, list<ValueType> regTypes, int alignment,
                    dag regList, RegAltNameIndex idx = NoRegAltName>
*/

/***************************************************************/
def MReg : RegisterClass<"MMPUReg", [v16i32], 32,
			  			  (add (sequence "M%u", 0, 127), MIPP, MSPP)>;

def RPort : RegisterClass<"MMPUReg", [v16i32], 32,
			  			  (sequence "r%u", 0, 3)>;

def WPort : RegisterClass<"MMPUReg", [v16i32], 32,
			  			  (sequence "w%u", 0, 3)>;

/***************************************************************/
def Shu   : RegisterClass<"MMPUReg", [v16i32], 32,
			   			  (add Shu0, Shu1)>;

def Calc : RegisterClass<"MMPUReg", [v16i32], 32,
			 			 (add IALU, IMAC, FALU, FMAC)>;

//////////////////////////////////////////////////
def TPort : RegisterClass<"MMPUReg", [v16i32], 32,
			  			  (add (sequence "T%u", 0, 3), TBB, TBH, TBW, TBD, TSQ)>;

def IPort : RegisterClass<"MMPUReg", [v16i32], 32,
			  			  (sequence "I%u", 0, 2)>;

/***************************************************************/
def BIU   : RegisterClass<"MMPUReg", [v16i32], 32,
			  			  (sequence "BIU%u", 0, 2)>;

/***************************************************************/
def KIReg : RegisterClass<"MMPUReg", [i32], 32, (sequence "KI%u", 0, 15)>;
