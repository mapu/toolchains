//===-- MMPUInstrInfo.td - Target Description for MMPU Target -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file describes the MMPU instructions in TableGen format.
// We describe a super set for MMPU ISA, and leave exception constraints to be
// handled by bison.
//
//===----------------------------------------------------------------------===//

include "MMPUInstrFormats.td"

def COption : Operand<i32> {
  let PrintMethod   = "printCOption";
  let EncoderMethod = "getCOptionEncoding";
  let DecoderMethod = "DecodeCOption";
}

def BIUOptions : Operand<i32> {
  let PrintMethod   = "printBIUOptions";
  let EncoderMethod = "getBIUOptionsEncoding";
  let DecoderMethod = "DecodeBIUOptions";
}

// BIU store instruction only
def BIUStOptions : Operand<i32> {
  let PrintMethod   = "printBIUStOptions";
  let EncoderMethod = "getBIUStOptionsEncoding";
  let DecoderMethod = "DecodeBIUStOptions";
}

def IMACOptions : Operand<i32> {
  let PrintMethod = "printIMACOptions";
  let EncoderMethod = "getIMACOptionsEncoding";
  let DecoderMethod = "DecodeIMACOptions";
}

def Mr2Options : Operand<i32> {
  let PrintMethod = "printMr2Options";
  let EncoderMethod = "getMr2OptionsEncoding";
  let DecoderMethod = "DecodeMr2Options";
}

def FMACOptions : Operand<i32> {
  let PrintMethod = "printFMACOptions";
  let EncoderMethod = "getFMACOptionsEncoding";
  let DecoderMethod = "DecodeFMACOptions";
}

def Conds : Operand<i32> {
  let PrintMethod = "printConds";
}

//===----------------------------------------------------------------------===//
//						Instructions for MMPU
// There are 2 tablegen patterns for MMPU instructions:
// one has a slot operand since we can not determine which slot it takes
// implicitly from its asm syntax, and the other has not since we can tell
// from its asm syntax which slot it takes.
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
//						Instruction NOP for all slots
//===----------------------------------------------------------------------===//
def NOP : MMPUInst < 
	 (outs),
	 (ins),
	 "NOP">
{
  let Inst{47-0} = 0;
}

def Macro : MMPUInst < 
	 (outs),
	 (ins i32imm:$i),
	 "only for macro tool">
{
  let Inst{47-44} = 14;
}

// MReg encoding pattern for read operation
def MReadReg : Operand<i32> {
//let PrintMethod =   "printMReadReg";
  let EncoderMethod = "getMReadRegEncoding";
  let DecoderMethod = "DecodeMReadReg";
}

// MReg encoding pattern for Wrt operation
def MWrtReg : Operand<i32> {
//let PrintMethod = "printMWrtReg";
  let EncoderMethod = "getMWrtRegEncoding";
  let DecoderMethod = "DecodeMWrtReg";
}

// MReg encoding pattern for Wrt operation
def MRegExt : Operand<i32> {
  let PrintMethod = "printMRegExt";
//let EncoderMethod = "getMRegExtEncoding";
  let DecoderMethod = "DecodeMRegExt";
}

def TPortExt : Operand<i32> {
  let PrintMethod = "printTPortExt";
//let EncoderMethod = "getTPortExtEncoding";
  let DecoderMethod = "DecodeTPortExt";
}

//===----------------------------------------------------------------------===//
//						Instructions for Slot 0.
//===----------------------------------------------------------------------===//
class Slot0Inst<dag outs, string asm>:MMPUInst<
	  				outs, (ins  MReg:$i, COption:$cf), asm>
{  
   bits<9> i;
   bits<2> cf;
   
   let Inst{47-44} = 0;
   let Inst{22} = 1;
   let Inst{21-20} = cf;
   let Inst{19-11} = i;
}

// Note they have different encodings.
def Mr0ToShu: Slot0Inst <
	 (outs Shu:$u, TPort:$t),
	 "$u.$t = $i.r0 $cf" >
{
  bits<2> u;
  bits<2> t;

  let Inst{5-4} = t;
  let Inst{3-2} = u;
  let Inst{1-0} = 0b01;
}

def Mr0ToCalc: Slot0Inst <
	 (outs Calc:$u, TPort:$t, IPort:$w),
	 "$u.$t $w = $i.r0 $cf" >
{
  bits<2> u;
  bits<2> w;
  bits<2> t;

  let Inst{7-6} = w;
  let Inst{5-4} = t;
  let Inst{3-2} = u;
  let Inst{1-0} = 0b11;
}

def Mr0ToMw3: Slot0Inst <
	 (outs MReg:$j),
	 "$j.w3 = $i.r0 $cf" >
{
  bits<7> j;

  let Inst{10-4} = j;
  let Inst{3-0} = 0;
}

//===----------------------------------------------------------------------===//
//						Instructions for Slot 1.
//===----------------------------------------------------------------------===//

class Slot1Inst<dag outs, string asm>:MMPUInst<
	  				outs, (ins  MReg:$i, COption:$cf), asm>
{  
  bits<2> u;
  bits<2> t;
  bits<9> i;
  bits<2> cf;

  let Inst{47-44} = 1;
  let Inst{19} = 1;
  let Inst{18-17} = cf;
  let Inst{16-8} = i;
  let Inst{5-4} = t;
  let Inst{3-2} = u;
}

def Mr1ToShu: Slot1Inst <
	 (outs Shu:$u, TPort:$t),
	 "$u.$t = $i.r1 $cf" >
{
  let Inst{1-0} = 0b01;
}

def Mr1ToCalc: Slot1Inst <
	 (outs Calc:$u, TPort:$t, IPort:$w),
	 "$u.$t $w = $i.r1 $cf" >
{
  bits<2> w;
  let Inst{7-6} = w;
  let Inst{1-0} = 0b11;
}

//===----------------------------------------------------------------------===//
//						Instructions for Slot 2.
//===----------------------------------------------------------------------===//
def Mr2ToCalc: MMPUInst <
	 (outs Calc:$u, TPort:$t, IPort:$w),
	 (ins  MReg:$i, Mr2Options:$f),
	 "$u.$t $w = $i.r2 $f" >
{
  bits<2> u;
  bits<2> t;
  bits<2> w;
  bits<5> f;
  bits<7> i;

  let Inst{47-44} = 2; // slot number
  let Inst{20} = 1;    // slot is enabled.
  let Inst{19-15} = f;
  let Inst{14-8} = i;
  let Inst{7-6} = w;
  let Inst{5-4} = t;
  let Inst{3-2} = u;
  let Inst{1-0} = 0b11;
}

//===----------------------------------------------------------------------===//
//						Instructions for Slot 3
//===----------------------------------------------------------------------===//
class Slot3Inst<dag outs, dag ins, string asm>:MMPUInst<
	  				outs, ins, asm>
{
  bits<7> i;
  bits<2> b;
  bits<2> cf;

  let Inst{47-44} = 3; // slot
  let Inst{14} =  	1; // slot is enabled.
  let Inst{13-12} = cf;
  let Inst{10-4} = i;
  let Inst{3-2} = b;
  let Inst{1-0} = 0b10;
}

def  Mr3ToBIU : Slot3Inst < 
	 (outs BIU:$b),
	 (ins  MReg:$i, COption:$cf),
	 "$b = $i.r3 $cf">
{
  let Inst{11} = 0; // no KG
}

def  Mr3ToBIUKG : Slot3Inst < 
	 (outs BIU:$b),
	 (ins  MReg:$i, COption:$cf),
	 "$b.KG = $i.r3 $cf">
{
  let Inst{11} = 1; // KG
}

//===----------------------------------------------------------------------===//
//						Instructions for shuffle slot 4 ~ 5
//===----------------------------------------------------------------------===//
// Note: all the following operands Shu:$s is for avoiding reduce/reduce conflict
// in bison file thus it needs to be the last operand. See bison file for more details.
class ShuCommon<dag outs, dag ins, string asm> : MMPUInst<
	  				outs, ins, asm>
{
  bits<1> s;
  let Inst{43} = s; // note this bit is necessary but not redundant.
}

class ShuIndxCommon<dag outs, dag ins, string asm> : ShuCommon<
	  				outs, ins, asm>
{
  bits<4> m;
  bits<4> n;
  bits<2> cf;
  
  let Inst{27-25} = 0b001;
  let Inst{24-23} = cf;
  let Inst{22} = 0;
  let Inst{21-18} = m;
  let Inst{17-14} = n;
}

class ShuIndx< bits<4> sn , string shu , bits<2> u > : ShuIndxCommon < 
	 (outs Shu:$s, TPort:$t),
	 (ins  TPort:$m, TPort:$n, COption:$cf),
	 !subst("SHU", shu, "SHU.$t = $m INDX $n $cf")>
{
  bits<2> t;
  let Inst{47-44} = sn;
  let Inst{5-4} = t;
  let Inst{3-2} = u;
  let Inst{1-0} = 0b01;
}

def Shu0Indx : ShuIndx<4, "SHU0", 0>;
def Shu1Indx : ShuIndx<5, "SHU1", 1>;

///////////////////////////////////////////
class ShuIndxToCalc< bits<4> sn , string shu > : ShuIndxCommon < 
	 (outs Calc:$u, TPort:$t, IPort:$w),
	 (ins  Shu:$s, TPort:$m, TPort:$n, COption:$cf),
	 !subst("SHU", shu, "$u.$t $w= SHU.$m INDX $n $cf") >
{
  bits<2> u;
  bits<2> t;
  bits<2> w;

  let Inst{47-44} = sn;
  let Inst{7-6} = w;
  let Inst{5-4} = t;
  let Inst{3-2} = u;
  let Inst{1-0} = 0b11;
}
def Shu0IndxToCalc : ShuIndxToCalc<4, "SHU0">;
def Shu1IndxToCalc : ShuIndxToCalc<5, "SHU1">;

//////////////////////////////////////////
class ShuIndxToBIU< bits<4> sn, string shu > : ShuIndxCommon <
	 (outs BIU:$b),
	 (ins  Shu:$s, TPort:$m, TPort:$n, COption:$cf),
	 !subst("SHU", shu, "$b = SHU.$m INDX $n $cf" )>
{
  bits<2> b;

  let Inst{47-44} = sn;
  let Inst{3-2} = b;
  let Inst{1-0} = 0b10;
}
def Shu0IndxToBIU : ShuIndxToBIU<4, "SHU0">;
def Shu1IndxToBIU : ShuIndxToBIU<5, "SHU1">;

////////////////////////////////////////
class  ShuIndxToMw3< bits<4> sn , string shu> : ShuIndxCommon < 
	 (outs MReg:$i),
	 (ins  Shu:$s, TPort:$m, TPort:$n, COption:$cf),
	 !subst("SHU", shu, "$i.w3 = SHU.$m INDX $n $cf") >
{
  bits<8> i;

  let Inst{47-44} = sn;
  let Inst{10-4} = i{6-0};
  let Inst{2} = i{7};	 // 1: M[I++], 0: M0~M127
}
def Shu0IndxToMw3 : ShuIndxToMw3<4, "SHU0">;
def Shu1IndxToMw3 : ShuIndxToMw3<5, "SHU1">;

/**********************************************************/
class ShuIndxWithImmCommon<dag outs, dag ins, string asm> : MMPUInst<
	  				outs, ins, asm>
{
  bits<4> m;
  bits<4> n;
  bits<3> i;
  bits<1> s;
  bits<2> cf;
  
  let Inst{43} = s; // note this bit is mandontary but not redundant.
  let Inst{27-25} = 0b001;
  let Inst{24-23} = cf;
  let Inst{22} = 1;
  let Inst{21-18} = m;
  let Inst{17-14} = n;
  let Inst{13-11} = i;
}

class  ShuIndxWithImm< bits<4> sn , string shu , bits<2> u > : ShuIndxWithImmCommon < 
	 (outs Shu:$s, TPort:$t),
	 (ins  TPort:$m, TPort:$n, i32imm:$i, COption:$cf),
	 !subst("SHU", shu, "SHU.$t = $m INDX $n (TB =+ $i) $cf")>
{
  bits<2> t;

  let Inst{47-44} = sn;

  let Inst{5-4} = t;
  let Inst{3-2} = u;
  let Inst{1-0} = 0b01;
}
def Shu0IndxWithImm : ShuIndxWithImm<4, "SHU0", 0>;
def Shu1IndxWithImm : ShuIndxWithImm<5, "SHU1", 1>;

///////////////////////////////////////////////
class  ShuIndxWithImmToCalc< bits<4> sn, string shu > : ShuIndxWithImmCommon < 
	 (outs Calc:$u, TPort:$t, IPort:$w),
	 (ins  Shu:$s, TPort:$m, TPort:$n, i32imm:$i, COption:$cf),
	 !subst("SHU", shu, "$u.$t $w = SHU.$m INDX $n (TB =+ $i) $cf" ) >
{
  bits<2> u;
  bits<2> t;
  bits<2> w;

  let Inst{47-44} = sn;
  let Inst{7-6} = w;
  let Inst{5-4} = t;
  let Inst{3-2} = u;
  let Inst{1-0} = 0b11;
}
def Shu0IndxWithImmToCalc : ShuIndxWithImmToCalc<4, "SHU0">;
def Shu1IndxWithImmToCalc : ShuIndxWithImmToCalc<5, "SHU1">;

/////////////////////////////////////////////
class ShuIndxWithImmToBIU< bits<4> sn, string shu > : ShuIndxWithImmCommon < 
	 (outs BIU:$u),
	 (ins  Shu:$s, TPort:$m, TPort:$n, i32imm:$i, COption:$cf),
	 !subst("SHU", shu,  "$u = SHU.$m INDX $n (TB =+ $i) $cf") >
{
  bits<2> u;
  bits<3> i;

  let Inst{47-44} = sn;
  let Inst{13-11} = i;
  let Inst{3-2} = u;
  let Inst{1-0} = 0b10;
}
def Shu0IndxWithImmToBIU: ShuIndxWithImmToBIU<4, "SHU0">;
def Shu1IndxWithImmToBIU: ShuIndxWithImmToBIU<5, "SHU1">;

class  ShuIndxWithImmToMw3< bits<4> sn , string shu> : ShuIndxWithImmCommon < 
	 (outs MReg:$M),
	 (ins  Shu:$s, TPort:$m, TPort:$n, i32imm:$i, COption:$cf),
	 !subst("SHU", shu, "$M.w3 = SHU.$m INDX $n (TB =+ $i) $cf")>
{
  bits<8> M;  // out0

  let Inst{47-44} = sn;
  let Inst{10-4} = M{6-0};
  let Inst{2} = M{7};// M[I++]
}
def Shu0IndxWithImmToMw3: ShuIndxWithImmToMw3<4, "SHU0">;
def Shu1IndxWithImmToMw3: ShuIndxWithImmToMw3<5, "SHU1">;

/********************************************/
class ShuIndxTBCommon<dag outs, dag ins, string asm>:ShuCommon<
	  				outs, ins, asm>
{
  bits<4> m;
  bits<2> cf;
  
  let Inst{27-25} = 0b010;
  let Inst{24-23} = cf;
  let Inst{22} = 0;
  let Inst{21-18} = m;
}

class ShuIndxTB< bits<4> sn, string shu, bits<2> u > : ShuIndxTBCommon < 
	 (outs Shu:$s, TPort:$t),
	 (ins  TPort:$m, COption:$cf),
	 !subst("SHU", shu, "SHU.$t = $m INDX TB $cf")>
{
  bits<2> t;

  let Inst{47-44} = sn;
  let Inst{5-4} = t;
  let Inst{3-2} = u;
  let Inst{1-0} = 0b01;
}

def Shu0IndxTB: ShuIndxTB<4, "SHU0", 0>;
def Shu1IndxTB: ShuIndxTB<5, "SHU1", 1>;

class ShuIndxTBToCalc< bits<4> sn, string shu > : ShuIndxTBCommon < 
	 (outs Calc:$u, TPort:$t, IPort:$w),
	 (ins  Shu:$s, TPort:$m, COption:$cf),
	 !subst("SHU", shu, "$u.$t $w = SHU.$m INDX TB $cf")>
{
  bits<2> u;
  bits<2> t;
  bits<2> w;

  let Inst{47-44} = sn;
  let Inst{7-6} = w;
  let Inst{5-4} = t;
  let Inst{3-2} = u;
  let Inst{1-0} = 0b11;
}
def Shu0IndxTBToCalc: ShuIndxTBToCalc<4, "SHU0">;
def Shu1IndxTBToCalc: ShuIndxTBToCalc<5, "SHU1">;

///////////////////////////////////
class ShuIndxTBToBIU< bits<4> sn , string shu> : ShuIndxTBCommon < 
	 (outs BIU:$u),
	 (ins  Shu:$s,  TPort:$m, COption:$cf),
	 !subst("SHU", shu, "$u = SHU.$m INDX TB $cf")>
{
  bits<2> u;

  let Inst{47-44} = sn;
  let Inst{3-2} = u;
  let Inst{1-0} = 0b10;
}
def Shu0IndxTBToBIU: ShuIndxTBToBIU<4, "SHU0">;
def Shu1IndxTBToBIU: ShuIndxTBToBIU<5, "SHU1">;

class ShuIndxTBToMw3< bits<4> sn, string shu > : ShuIndxTBCommon < 
	 (outs MReg:$M),
	 (ins  Shu:$s, TPort:$m, COption:$cf),
	 !subst("SHU", shu, "$M.w3 = SHU.$m INDX TB $cf")>
{
  bits<8> M;  // out0

  let Inst{47-44} = sn;
  let Inst{10-4} = M{6-0};
  let Inst{2} = M{7};// MReg
}
def Shu0IndxTBToMw3: ShuIndxTBToMw3<4, "SHU0">;
def Shu1IndxTBToMw3: ShuIndxTBToMw3<5, "SHU1">;

/**************************************/
class ShuIndxTBWithImmCommon<dag outs, dag ins, string asm>:ShuCommon<
	  				outs, ins, asm>
{
  bits<4> m;
  bits<3> i;
  bits<2> cf;
  
  let Inst{27-25} = 0b010;
  let Inst{24-23} = cf;
  let Inst{22} = 1;
  let Inst{21-18} = m;
  let Inst{13-11} = i;
}

class  ShuIndxTBWithImm< bits<4> sn, string shu, bits<2> u > : ShuIndxTBWithImmCommon < 
	 (outs Shu:$s, TPort:$t),
	 (ins  TPort:$m, i32imm:$i, COption:$cf),
	 !subst("SHU", shu, "SHU.$t = $m INDX TB (TB += $i) $cf")>
{
  bits<2> t;

  let Inst{47-44} = sn;
  let Inst{5-4} = t;
  let Inst{3-2} = u;
  let Inst{1-0} = 0b01;
}
def Shu0IndxTBWithImm: ShuIndxTBWithImm<4, "SHU0", 0>;
def Shu1IndxTBWithImm: ShuIndxTBWithImm<5, "SHU1", 1>;

class ShuIndxTBWithImmToCalc< bits<4> sn, string shu > : ShuIndxTBWithImmCommon < 
	 (outs Calc:$u, TPort:$t, IPort:$w),
	 (ins   Shu:$s, TPort:$m, i32imm:$i, COption:$cf),
	 !subst("SHU", shu, "$u.$t $w = SHU.$m INDX TB (TB += $i) $cf")>
{
  bits<2> u;
  bits<2> t;
  bits<2> w;

  let Inst{47-44} = sn;
  let Inst{7-6} = w;
  let Inst{5-4} = t;
  let Inst{3-2} = u;
  let Inst{1-0} = 0b11;
}
def Shu0IndxTBWithImmToCalc: ShuIndxTBWithImmToCalc<4, "SHU0">;
def Shu1IndxTBWithImmToCalc: ShuIndxTBWithImmToCalc<5, "SHU1">;

class ShuIndxTBWithImmToBIU< bits<4> sn, string shu > : ShuIndxTBWithImmCommon < 
	 (outs BIU:$u),
	 (ins  Shu:$s, TPort:$m, i32imm:$i, COption:$cf),
	 !subst("SHU", shu, "$u = SHU.$m INDX TB (TB += $i) $cf")>
{
  bits<2> u;

  let Inst{47-44} = sn;
  let Inst{3-2} = u;
  let Inst{1-0} = 0b10;
}
def Shu0IndxTBWithImmToBIU: ShuIndxTBWithImmToBIU<4, "SHU0">;
def Shu1IndxTBWithImmToBIU: ShuIndxTBWithImmToBIU<5, "SHU1">;

class ShuIndxTBWithImmToMw3< bits<4> sn, string shu > : ShuIndxTBWithImmCommon < 
	 (outs MReg:$M),
	 (ins  Shu:$s, TPort:$m, i32imm:$i, COption:$cf),
	 !subst("SHU", shu, "$M.w3 = SHU.$m INDX TB (TB += $i) $cf")>
{
  bits<8> M;  // out0

  let Inst{47-44} = sn;
  let Inst{10-4} = M{6-0};
  let Inst{2} = M{7};// Mi.w3
}
def Shu0IndxTBWithImmToMw3: ShuIndxTBWithImmToMw3<4, "SHU0">;
def Shu1IndxTBWithImmToMw3: ShuIndxTBWithImmToMw3<5, "SHU1">;

/**************************************/
def CombineOptions : Operand<i32> {
  let PrintMethod = "printCombineOptions";
  let EncoderMethod = "getCombineOptionsEncoding";
  let DecoderMethod = "DecodeCombineOptions";
}

class ShuCombineCommon<dag outs, dag ins, string asm> : ShuCommon <
  	    outs, ins, asm >
{
  bits<4> m;
  bits<4> n;
  bits<3> i;
  bits<3> f;
  
  let Inst{27-25} = 0b011;
  let Inst{24-22} = f;
  let Inst{21-18} = m;
  let Inst{17-14} = n;
  let Inst{13-11} = i;
}

class ShuCombine< bits<4> sn, string shu , bits<2> u > : ShuCombineCommon < 
	 (outs Shu:$s, TPort:$t),
	 (ins  TPort:$m, TPort:$n, i32imm:$i, CombineOptions:$f), 
	 !subst("SHU", shu, "SHU.$t = Comb($m, $n, $i) $f")>
{
  bits<2> t;
  let Inst{47-44} = sn;
  let Inst{5-4} = t;
  let Inst{3-2} = u;
  let Inst{1-0} = 0b01;
}
def Shu0Combine: ShuCombine<4, "SHU0", 0>;
def Shu1Combine: ShuCombine<5, "SHU1", 1>;

class ShuCombineToCalc< bits<4> sn, string shu > : ShuCombineCommon < 
	 (outs Calc:$u, TPort:$t, IPort:$w),
	 (ins  Shu:$s, TPort:$m, TPort:$n, i32imm:$i, CombineOptions:$f),
	 !subst("SHU", shu, "$u.$t $w = SHU.Comb($m, $n, $i) $f")>
{
  bits<2> u;
  bits<2> t;
  bits<2> w;

  let Inst{47-44} = sn;
  let Inst{7-6} = w;
  let Inst{5-4} = t;
  let Inst{3-2} = u;
  let Inst{1-0} = 0b11;
}
def Shu0CombineToCalc : ShuCombineToCalc<4, "SHU0">;
def Shu1CombineToCalc : ShuCombineToCalc<5, "SHU1">;

class ShuCombineToBIU< bits<4> sn, string shu > : ShuCombineCommon < 
	 (outs BIU:$u),
	 (ins  Shu:$s, TPort:$m, TPort:$n, i32imm:$i, CombineOptions:$f),
	 !subst("SHU", shu, "$u = SHU.Comb($m, $n, $i) $f")>
{
  bits<2> u;

  let Inst{47-44} = sn;
  let Inst{3-2} = u;
  let Inst{1-0} = 0b10;
}
def Shu0CombineToBIU: ShuCombineToBIU<4, "SHU0">;
def Shu1CombineToBIU: ShuCombineToBIU<5, "SHU1">;

class ShuCombineToMw3< bits<4> sn , string shu> : ShuCombineCommon < 
	 (outs MReg:$M),
	 (ins  Shu:$s, TPort:$m, TPort:$n, i32imm:$i, CombineOptions:$f),
	 !subst("SHU", shu, "$M.w3 = SHU.Comb($m, $n, $i) $f")>
{
  bits<8> M;  // out0
  let Inst{47-44} = sn;
  let Inst{10-4} = M{6-0};
  let Inst{2} = M{7};// Mi.w3
}
def Shu0CombineToMw3: ShuCombineToMw3<4, "SHU0">;
def Shu1CombineToMw3: ShuCombineToMw3<5, "SHU1">;

//===----------------------------------------------------------------------===//
//						Instructions for IALU: Slot 6
//===----------------------------------------------------------------------===//
def IALUOptions : Operand<i32> {
  let PrintMethod   = "printIALUOptions";
  let EncoderMethod = "getIALUOptionsEncoding";
  let DecoderMethod = "DecodeIALUOptions";
}

// Unit encoding: use bits 3-0.
// We directly decode this operand into a register, and thus do not need a
// specific print method.
def Unit : Operand<i32> {
//let PrintMethod   = "printUnit";
  let EncoderMethod = "getUnitEncoding";
  let DecoderMethod = "DecodeUnit";
}

class IALUCommon<dag outs, dag ins, string asm> : MMPUInst < outs, ins, asm >
{
  bits<6> f;
  
  let Inst{47-44} = 6; // slot number
  let Inst{25-24} = f{5-4};
  let Inst{21-18} = f{3-0};
}

class IALUWithCOption<dag outs, dag ins, string asm> : MMPUInst < outs, ins, asm >
{
  bits<2> cf;
  
  let Inst{47-44} = 6; // slot number
  let Inst{25-24} = cf;
}

/***************** Unit Binary Instructions *****************/
class IALUBinary<dag outs, dag ins, string asm> : IALUCommon < outs, ins, asm >
{
  bits<2> t1;
  bits<2> t2;
  let Inst{17-16} = t1;
  let Inst{15-14} = t2;
}

class IALUCond <string asm, bits<5> opc> : IALUBinary <
	      (outs),
	   	  (ins  TPort:$t1, TPort:$t2, IALUOptions:$f), asm >
{
  let Inst{30-26} = opc;
  let Inst{23-22} = 0b10;
}

def IALUCondEQ: IALUCond <"Cond = $t1 == $t2$f", 0b10000>;
def IALUCondNE: IALUCond <"Cond = $t1 != $t2$f", 0b10001>;
def IALUCondGT: IALUCond <"Cond = $t1 > $t2$f",  0b10010>;
def IALUCondGE: IALUCond <"Cond = $t1 >= $t2$f", 0b10011>;
def IALUCondLT: IALUCond <"Cond = $t1 < $t2$f",  0b10100>;
def IALUCondLE: IALUCond <"Cond = $t1 <= $t2$f", 0b10101>;

class IALUBinaryToUnit<string asm, bits<5> opc> : IALUBinary <
	      (outs Unit:$u,   TPort:$t),
	   	  (ins  TPort:$t1, TPort:$t2, IALUOptions:$f), asm >
{
  bits<4> u;
  bits<2> t;
  let Inst{30-26} = opc;
  let Inst{5-4} = t;
  let Inst{3-0} = u;
}

def IALUAddToUnit: IALUBinaryToUnit <"$u.$t = $t1 + $t2$f", 0b00001>;
def IALUSubToUnit: IALUBinaryToUnit <"$u.$t = $t1 - $t2$f", 0b00010>;
def IALUMaxToUnit: IALUBinaryToUnit <"$u.$t = MAX($t1, $t2)$f", 0b00011>;
def IALUMinToUnit: IALUBinaryToUnit <"$u.$t = MIN($t1, $t2)$f", 0b00100>;
def IALUCprsToUnit: IALUBinaryToUnit <"$u.$t = Cprs($t1, $t2)$f",0b01110>;
def IALUSubAbsToUnit: IALUBinaryToUnit <"$u.$t = ABS($t1 - $t2)$f", 0b00101>;

def IALUAndToUnit: IALUBinaryToUnit <"$u.$t = $t1 & $t2$f", 0b01000>;
def IALUOrToUnit: IALUBinaryToUnit <"$u.$t = $t1 | $t2$f", 0b01001>;
def IALUXOrToUnit: IALUBinaryToUnit <"$u.$t = $t1 ^ $t2$f", 0b01010>;

def IALUSLToUnit: IALUBinaryToUnit <"$u.$t = $t1 << $t2$f", 0b01100>;
def IALUSRToUnit: IALUBinaryToUnit <"$u.$t = $t1 >> $t2$f", 0b01101>;

def IALUEQToUnit: IALUBinaryToUnit <"$u.$t = $t1 == $t2$f", 0b10000>;
def IALUNEToUnit: IALUBinaryToUnit <"$u.$t = $t1 != $t2$f", 0b10001>;
def IALUGTToUnit: IALUBinaryToUnit <"$u.$t = $t1 > $t2$f",  0b10010>;
def IALUGEToUnit: IALUBinaryToUnit <"$u.$t = $t1 >= $t2$f", 0b10011>;
def IALULTToUnit: IALUBinaryToUnit <"$u.$t = $t1 < $t2$f",  0b10100>;
def IALULEToUnit: IALUBinaryToUnit <"$u.$t = $t1 <= $t2$f", 0b10101>;

/*************** MReg Binary Instructions *******************/
class IALUBinaryToMw3<string asm, bits<5> opc> : IALUBinary <
	      (outs MReg:$mi), (ins  TPort:$t1, TPort:$t2, IALUOptions:$f), asm >
{
  bits<8> mi;
  bits<2> t;

  let Inst{30-26} = opc;
  let Inst{23-22} = 0b00;
  let Inst{10-4} = mi{6-0};
  let Inst{2} = mi{7};
  let Inst{1-0} = 0;
}

def IALUAddToMw3: IALUBinaryToMw3 <"$mi.w3 = $t1 + $t2 $f", 0b00001>;
def IALUSubToMw3: IALUBinaryToMw3 <"$mi.w3 = $t1 - $t2 $f", 0b00010>;
def IALUMaxToMw3: IALUBinaryToMw3 <"$mi.w3 = MAX($t1, $t2) $f", 0b00011>;
def IALUMinToMw3: IALUBinaryToMw3 <"$mi.w3 = MIN($t1, $t2) $f", 0b00100>;
def IALUCprsToMw3: IALUBinaryToMw3 <"$mi.w3 = Cprs($t1, $t2) $f", 0b01110>;
def IALUSubAbsToMw3: IALUBinaryToMw3 <"$mi.w3 = ABS($t1 - $t2) $f", 0b00101>;

def IALUAndToMw3: IALUBinaryToMw3 <"$mi.w3 = $t1 & $t2 $f", 0b01000>;
def IALUOrToMw3: IALUBinaryToMw3 <"$mi.w3 = $t1 | $t2 $f", 0b01001>;
def IALUXOrToMw3: IALUBinaryToMw3 <"$mi.w3 = $t1 ^ $t2 $f", 0b01010>;

def IALUSLToMw3: IALUBinaryToMw3 <"$mi.w3 = $t1 << $t2 $f", 0b01100>;
def IALUSRToMw3: IALUBinaryToMw3 <"$mi.w3 = $t1 >> $t2 $f", 0b01101>;

def IALUEQToMw3: IALUBinaryToMw3 <"$mi.w3 = $t1 == $t2 $f", 0b10000>;
def IALUNEToMw3: IALUBinaryToMw3 <"$mi.w3 = $t1 != $t2 $f", 0b10001>;
def IALUGTToMw3: IALUBinaryToMw3 <"$mi.w3 = $t1 > $t2 $f",  0b10010>;
def IALUGEToMw3: IALUBinaryToMw3 <"$mi.w3 = $t1 >= $t2 $f", 0b10011>;
def IALULTToMw3: IALUBinaryToMw3 <"$mi.w3 = $t1 < $t2 $f",  0b10100>;
def IALULEToMw3: IALUBinaryToMw3 <"$mi.w3 = $t1 <= $t2 $f", 0b10101>;

/****************** BIU Binary Instructions ***********************/
class IALUBinaryToBIU<string asm, bits<5> opc> : IALUBinary <
	      (outs BIU:$b), (ins  TPort:$t1, TPort:$t2, IALUOptions:$f), asm >
{
  bits<2> b;

  let Inst{30-26} = opc;
  let Inst{3-2} = b;
  let Inst{1-0} = 0b10;
}

def IALUAddToBIU: IALUBinaryToBIU <"$b = $t1 + $t2 $f", 0b00001>;
def IALUSubToBIU: IALUBinaryToBIU <"$b = $t1 - $t2 $f", 0b00010>;
def IALUMaxToBIU: IALUBinaryToBIU <"$b = MAX($t1, $t2) $f", 0b00011>;
def IALUMinToBIU: IALUBinaryToBIU <"$b = MIN($t1, $t2) $f", 0b00100>;
def IALUCprsToBIU: IALUBinaryToBIU <"$b = Cprs($t1, $t2) $f", 0b01110>;
def IALUSubAbsToBIU: IALUBinaryToBIU <"$b = ABS($t1 - $t2) $f", 0b00101>;

def IALUAndToBIU: IALUBinaryToBIU <"$b = $t1 & $t2 $f", 0b01000>;
def IALUOrToBIU: IALUBinaryToBIU <"$b = $t1 | $t2 $f", 0b01001>;
def IALUXOrToBIU: IALUBinaryToBIU <"$b = $t1 ^ $t2 $f", 0b01010>;

def IALUSLToBIU: IALUBinaryToBIU <"$b = $t1 << $t2 $f", 0b01100>;
def IALUSRToBIU: IALUBinaryToBIU <"$b = $t1 >> $t2 $f", 0b01101>;

def IALUEQToBIU: IALUBinaryToBIU <"$b = $t1 == $t2 $f", 0b10000>;
def IALUNEToBIU: IALUBinaryToBIU <"$b = $t1 != $t2 $f", 0b10001>;
def IALUGTToBIU: IALUBinaryToBIU <"$b = $t1 > $t2 $f",  0b10010>;
def IALUGEToBIU: IALUBinaryToBIU <"$b = $t1 >= $t2 $f", 0b10011>;
def IALULTToBIU: IALUBinaryToBIU <"$b = $t1 < $t2 $f",  0b10100>;
def IALULEToBIU: IALUBinaryToBIU <"$b = $t1 <= $t2 $f", 0b10101>;

/********* IALU Select **********/
class IALUTernary<dag outs, dag ins, string asm> : IALUCommon < outs, ins, asm >
{
  bits<2> t1;
  bits<2> t2;
  bits<2> t3;
  let Inst{17-16} = t3;
  let Inst{15-14} = t2;
  let Inst{13-12} = t1;
}
def IALUSelectToUnit : IALUTernary <
	      (outs Unit:$u,   TPort:$t),
	   	  (ins  TPort:$t1, TPort:$t2, TPort:$t3, IALUOptions:$f),
		  "$u.$t = $t1 ? $t2 : $t3 $f">
{
  bits<4> u;
  bits<2> t;

  let Inst{30-26} = 0b00110;
  let Inst{5-4} = t;
  let Inst{3-0} = u;
}

def IALUSelectToBIU : IALUTernary <
	      (outs BIU:$b),
		  (ins  TPort:$t1, TPort:$t2, TPort:$t3, IALUOptions:$f),
		  "$b = $t1 ? $t2 : $t3 $f">
{
  bits<2> b;

  let Inst{30-26} = 0b00110;
  let Inst{3-2} = b;
  let Inst{1-0} = 0b10;
}

def IALUSelectToMw3 : IALUTernary <
	      (outs MReg:$mi),
		  (ins  TPort:$t1, TPort:$t2, TPort:$t3, IALUOptions:$f),
		  "$mi.w3 = $t1 ? $t2 : $t3 $f">
{
  bits<8> mi;

  let Inst{30-26} = 0b00110;
  let Inst{10-4} = mi{6-0};
  let Inst{2} = mi{7};
  let Inst{1-0} = 0;
}

/********* IALU Shift Imm **********/
class IALUBinaryImm<dag outs, dag ins, string asm> : IALUCommon < outs, ins, asm >
{
  bits<2> t1;
  bits<5> t2;

  let Inst{17-16} = t1;
  let Inst{15-11} = t2;
}
////////////////////////////////////
class IALUShiftImmToUnit<string asm, bits<5> opc> : IALUBinaryImm <
	      (outs Unit:$u,   TPort:$t),
	   	  (ins  TPort:$t1, i32imm:$t2, IALUOptions:$f),
		  asm>
{
  bits<4> u;
  bits<2> t;

  let Inst{30-26} = opc;
  let Inst{22} = 1;
  let Inst{5-4} = t;
  let Inst{3-0} = u;
}

def IALUSLImmToUnit : IALUShiftImmToUnit <"$u.$t = $t1 << $t2 $f", 0b01100>;
def IALUSRImmToUnit : IALUShiftImmToUnit <"$u.$t = $t1 >> $t2 $f", 0b01101>;

class IALUShiftImmToMw3<string asm, bits<5> opc> : IALUBinaryImm <
	       (outs MReg:$u),
	   	  (ins  TPort:$t1, i32imm:$t2, IALUOptions:$f),
		  asm>
{
  bits<8> u;

  let Inst{30-26} = opc;
  let Inst{23-22} = 0b01;
  let Inst{10-4} = u{6-0};
  let Inst{2} = u{7};
  let Inst{1-0} = 0;
}

def IALUSLImmToMw3 : IALUShiftImmToMw3 <"$u.w3 = $t1 << $t2 $f", 0b01100>;
def IALUSRImmToMw3 : IALUShiftImmToMw3 <"$u.w3 = $t1 >> $t2 $f", 0b01101>;

class IALUShiftImmToBIU<string asm, bits<5> opc> : IALUBinaryImm <
	      (outs BIU:$u),
	   	  (ins  TPort:$t1, i32imm:$t2, IALUOptions:$f),
		  asm>
{
  bits<2> u;

  let Inst{30-26} = opc;
  let Inst{22} = 1;
  let Inst{3-2} = u;
  let Inst{1-0} = 0b10;
}

def IALUSLImmToBIU : IALUShiftImmToBIU <"$u = $t1 << $t2 $f", 0b01100>;
def IALUSRImmToBIU : IALUShiftImmToBIU <"$u = $t1 >> $t2 $f", 0b01101>;

/*************** Bitwise Not *******************/
def IALUBitNotToUnit : IALUWithCOption <
	      (outs Unit:$u, TPort:$t), (ins  TPort:$t1, COption:$cf), "$u.$t = ~$t1">
{
  bits<4> u;
  bits<2> t;
  bits<2> t1;

  let Inst{30-26} = 0b01011;
  let Inst{17-16} = t1;
  let Inst{5-4} = t;
  let Inst{3-0} = u;
}
def IALUBitNotToMw3 : IALUWithCOption <(outs MReg:$u), (ins TPort:$t1, COption:$cf),
				  					  "$u.w3 = ~$t1">
{
  bits<8> u;
  bits<2> t1;

  let Inst{30-26} = 0b01011;
  let Inst{17-16} = t1;
  let Inst{10-4} = u{6-0};
  let Inst{2} = u{7};
  let Inst{1-0} = 0;
}
def IALUBitNotToBIU : IALUWithCOption <
	      (outs BIU:$b), (ins  TPort:$t1, COption:$cf), "$b = ~$t1">
{
  bits<2> b;
  bits<2> t1;

  let Inst{30-26} = 0b01011;
  let Inst{17-16} = t1;
  let Inst{3-2} = b;
  let Inst{1-0} = 0b10;
}

/*************** rmax *******************/
def IALURMaxToUnit : IALUCommon <
	      (outs Unit:$u, TPort:$t), (ins TPort:$t1, IALUOptions:$f),
		  "$u.$t = RMax $t1 $f">
{
  bits<4> u;
  bits<2> t;
  bits<2> t1;

  let Inst{30-26} = 0b10110;
  let Inst{17-16} = t1;
  let Inst{5-4} = t;
  let Inst{3-0} = u;
}
def IALURMaxToMw3 : IALUCommon <(outs MReg:$u), (ins TPort:$t1, IALUOptions:$f),
				  		    "$u.w3 = RMax $t1 $f">
{
  bits<8> u;
  bits<2> t1;

  let Inst{30-26} = 0b10110;
  let Inst{17-16} = t1;
  let Inst{10-4} = u{6-0};
  let Inst{2} = u{7};
  let Inst{1-0} = 0;
}
def IALURMaxToBIU : IALUCommon <
	      (outs BIU:$b), (ins  TPort:$t1, IALUOptions:$f), "$b = RMax $t1 $f">
{
  bits<2> b;
  bits<2> t1;

  let Inst{30-26} = 0b10110;
  let Inst{17-16} = t1;
  let Inst{3-2} = b;
  let Inst{1-0} = 0b10;
}

/*************** rmin *******************/
def IALURMinToUnit : IALUCommon <
	      (outs Unit:$u, TPort:$t), (ins  TPort:$t1, IALUOptions:$f), "$u.$t = RMin $t1 $f">
{
  bits<4> u;
  bits<2> t;
  bits<2> t1;

  let Inst{30-26} = 0b10111;
  let Inst{17-16} = t1;
  let Inst{5-4} = t;
  let Inst{3-0} = u;
}
def IALURMinToMw3 : IALUCommon <(outs MReg:$u), (ins  TPort:$t1, IALUOptions:$f), "$u.w3 = RMin $t1 $f">
{
  bits<8> u;
  bits<2> t1;

  let Inst{30-26} = 0b10111;
  let Inst{17-16} = t1;
  let Inst{10-4} = u{6-0};
  let Inst{2} = u{7};
  let Inst{1-0} = 0;
}
def IALURMinToBIU : IALUCommon <
	      (outs BIU:$b), (ins  TPort:$t1, IALUOptions:$f), "$b = RMin $t1 $f">
{
  bits<2> b;
  bits<2> t1;

  let Inst{30-26} = 0b10111;
  let Inst{17-16} = t1;
  let Inst{3-2} = b;
  let Inst{1-0} = 0b10;
}

/*************** expand *******************/
def IALUExpdToUnit : IALUCommon <
	      (outs Unit:$u, TPortExt:$t), (ins  TPort:$t1, IALUOptions:$f), "$u.$t = EXPD $t1 $f">
{
  bits<4> u;
  bits<2> t;
  bits<2> t1;

  let Inst{30-26} = 0b01111;
  let Inst{17-16} = t1;
  let Inst{5-4} = t;
  let Inst{3-0} = u;
}
def IALUExpdToMw3 : IALUCommon <(outs MRegExt:$u), (ins  TPort:$t1, IALUOptions:$f), "$u.w3 = EXPD $t1 $f">
{
  bits<8> u;
  bits<2> t1;

  let Inst{30-26} = 0b01111;
  let Inst{17-16} = t1;
  let Inst{10-4} = u{6-0};
  let Inst{2} = u{7};
  let Inst{1-0} = 0;
}
def IALUExpdToBIU : IALUCommon <
	      (outs BIU:$b), (ins  TPort:$t1, IALUOptions:$f), "$b = EXPD $t1 $f">
{
  bits<2> b;
  bits<2> t1;

  let Inst{30-26} = 0b01111;
  let Inst{17-16} = t1;
  let Inst{3-2} = b;
  let Inst{1-0} = 0b10;
}

/**********************************/
def IALUDivS : IALUCommon <
	  (outs),(ins  TPort:$t1,  TPort:$t2, IALUOptions:$f) ,
	  "DIVR = $t1 DIVS $t2$f">
{
  bits<2> t2;
  bits<2> t1;

  let Inst{30-26} = 0b11000;
  let Inst{17-16} = t1;
  let Inst{15-14} = t2;
}

def IALUDivQ : IALUWithCOption < (outs), (ins COption:$cf), "DIVR = DIVQ $cf">
{
  let Inst{30-26} = 0b11001;
}

def IALUMDivR : IALUWithCOption < (outs), (ins COption:$cf), "DIVR = MDIVR $cf">
{
  let Inst{30-26} = 0b11010;
}

def IALUMDivQ : IALUWithCOption < (outs), (ins COption:$cf), "DIVR = MDIVQ $cf">
{
  let Inst{30-26} = 0b11011;
}

def IALUDivRToUnit : IALUWithCOption <
	  (outs Unit:$u, TPortExt:$t), (ins COption:$cf), "$u.$t = DIVR $cf">
{
  bits<4> u;
  bits<2> t;

  let Inst{30-26} = 0b11100;
  let Inst{5-4} = t;
  let Inst{3-0} = u;
}

def IALUDivRToBIU : IALUWithCOption < (outs BIU:$u), (ins COption:$cf), "$u = DIVR $cf" >
{
  bits<2> u;
  let Inst{30-26} = 0b11100;
  let Inst{3-2} = u;
  let Inst{1-0} = 0b10;
}

def IALUDivRToMw3 : IALUWithCOption < (outs MRegExt:$u), (ins COption:$cf), "$u.w3 = DIVR $cf" >
{
  bits<8> u;
  let Inst{30-26} = 0b11100;
  let Inst{10-4} = u{6-0};
  let Inst{2} = u{7};
  let Inst{1-0} = 0;
}

//===----------------------------------------------------------------------===//
//						Instructions for FALU: Slot 8
//===----------------------------------------------------------------------===//
def FALUOptions : Operand<i32> {
  let PrintMethod = "printFALUOptions";
  let EncoderMethod = "getFALUOptionsEncoding";
  let DecoderMethod = "DecodeFALUOptions";
}

// Options for instructions with double-precise floating-point operands.
def FALUDblOptions : Operand<i32> {
  let PrintMethod = "printFALUDblOptions";
  let EncoderMethod = "getFALUDblOptionsEncoding";
  let DecoderMethod = "DecodeFALUDblOptions";
}

class FALUCommon<dag outs, dag ins, string asm> : MMPUInst < outs, ins, asm >
{
  let Inst{47-44} = 8; // slot
}

/***************** Unit Binary Instructions *****************/
class FALUBinary<dag outs, dag ins, string asm> : FALUCommon < outs, ins, asm >
{
  bits<2> t1;
  bits<2> t2;
  bits<6> f;
  let Inst{20-15} = f;
  let Inst{14-13} = t1;
  let Inst{12-11} = t2;
}

class FALUBinaryToUnit<string asm, bits<5> opc> : FALUBinary <
	      (outs Unit:$u,   TPort:$t),
	   	  (ins  TPort:$t1, TPort:$t2, FALUOptions:$f), asm >
{
  bits<4> u;
  bits<2> t;
  let Inst{25-21} = opc;
  let Inst{5-4} = t;
  let Inst{3-0} = u;
}

def FALUAddToUnit: FALUBinaryToUnit <"$u.$t = $t1 + $t2 $f", 0b00001>;
def FALUSubToUnit: FALUBinaryToUnit <"$u.$t = $t1 - $t2 $f", 0b00010>;
def FALUMaxToUnit: FALUBinaryToUnit <"$u.$t = MAX($t1, $t2) $f", 0b00011>;
def FALUMinToUnit: FALUBinaryToUnit <"$u.$t = MIN($t1, $t2) $f", 0b00100>;
def FALUAddSubToUnit: FALUBinaryToUnit <"$u.$t = $t1 +- $t2 $f", 0b00110>;

def FALUEQToUnit: FALUBinaryToUnit <"$u.$t = $t1 == $t2 $f", 0b01000>;
def FALUNEToUnit: FALUBinaryToUnit <"$u.$t = $t1 != $t2 $f", 0b01001>;
def FALUGTToUnit: FALUBinaryToUnit <"$u.$t = $t1 > $t2 $f",  0b01010>;
def FALUGEToUnit: FALUBinaryToUnit <"$u.$t = $t1 >= $t2 $f", 0b01011>;
def FALULTToUnit: FALUBinaryToUnit <"$u.$t = $t1 < $t2 $f",  0b01100>;
def FALULEToUnit: FALUBinaryToUnit <"$u.$t = $t1 <= $t2 $f", 0b01101>;

/*************** MReg Binary Instructions *******************/
class FALUBinaryToMw3<string asm, bits<5> opc> : FALUBinary <
	      (outs MReg:$mi), (ins  TPort:$t1, TPort:$t2, FALUOptions:$f), asm >
{
  bits<8> mi;
  let Inst{25-21} = opc;
  let Inst{10-4} = mi{6-0};
  let Inst{2} = mi{7};
  let Inst{1-0} = 0;
}

def FALUAddToMw3: FALUBinaryToMw3 <"$mi.w3 = $t1 + $t2 $f", 0b00001>;
def FALUSubToMw3: FALUBinaryToMw3 <"$mi.w3 = $t1 - $t2 $f", 0b00010>;
def FALUMaxToMw3: FALUBinaryToMw3 <"$mi.w3 = MAX($t1, $t2) $f", 0b00011>;
def FALUMinToMw3: FALUBinaryToMw3 <"$mi.w3 = MIN($t1, $t2) $f", 0b00100>;
def FALUAddSubToMw3: FALUBinaryToMw3 <"$mi.w3 = $t1 +- $t2 $f", 0b00110>;
def FALUEQToMw3: FALUBinaryToMw3 <"$mi.w3 = $t1 == $t2 $f", 0b01000>;
def FALUNEToMw3: FALUBinaryToMw3 <"$mi.w3 = $t1 != $t2 $f", 0b01001>;
def FALUGTToMw3: FALUBinaryToMw3 <"$mi.w3 = $t1 > $t2 $f",  0b01010>;
def FALUGEToMw3: FALUBinaryToMw3 <"$mi.w3 = $t1 >= $t2 $f", 0b01011>;
def FALULTToMw3: FALUBinaryToMw3 <"$mi.w3 = $t1 < $t2 $f",  0b01100>;
def FALULEToMw3: FALUBinaryToMw3 <"$mi.w3 = $t1 <= $t2 $f", 0b01101>;

/****************** BIU Binary Instructions ***********************/
class FALUBinaryToBIU<string asm, bits<5> opc> : FALUBinary <
	      (outs BIU:$b), (ins  TPort:$t1, TPort:$t2, FALUOptions:$f), asm >
{
  bits<2> b;
  let Inst{25-21} = opc;
  let Inst{3-2} = b;
  let Inst{1-0} = 0b10;
}

def FALUAddToBIU: FALUBinaryToBIU <"$b = $t1 + $t2 $f", 0b00001>;
def FALUSubToBIU: FALUBinaryToBIU <"$b = $t1 - $t2 $f", 0b00010>;
def FALUMaxToBIU: FALUBinaryToBIU <"$b = MAX($t1, $t2) $f", 0b00011>;
def FALUMinToBIU: FALUBinaryToBIU <"$b = MIN($t1, $t2) $f", 0b00100>;
def FALUAddSubToBIU: FALUBinaryToBIU <"$b = $t1 +- $t2 $f", 0b00110>;

def FALUEQToBIU: FALUBinaryToBIU <"$b = $t1 == $t2 $f", 0b01000>;
def FALUNEToBIU: FALUBinaryToBIU <"$b = $t1 != $t2 $f", 0b01001>;
def FALUGTToBIU: FALUBinaryToBIU <"$b = $t1 > $t2 $f",  0b01010>;
def FALUGEToBIU: FALUBinaryToBIU <"$b = $t1 >= $t2 $f", 0b01011>;
def FALULTToBIU: FALUBinaryToBIU <"$b = $t1 < $t2 $f",  0b01100>;
def FALULEToBIU: FALUBinaryToBIU <"$b = $t1 <= $t2 $f", 0b01101>;

/***************** FALU Unit Unary Instructions *****************/
class FALUUnary<dag outs, dag ins, string asm> : FALUCommon < outs, ins, asm >
{
  bits<2> t1;
  bits<6> f;
  let Inst{20-15} = f;
  let Inst{14-13} = t1;
}

class FALUUnaryToUnit<string asm, bits<5> opc> : FALUUnary <
	      (outs Unit:$u,   TPort:$t),
	   	  (ins  TPort:$t1, FALUOptions:$f), asm >
{
  bits<4> u;
  bits<2> t;
  let Inst{25-21} = opc;
  let Inst{5-4} = t;
  let Inst{3-0} = u;
}

def FALUAbsToUnit: FALUUnaryToUnit<"$u.$t = ABS $t1 $f", 0b00101>;
def FALUDblToUnit: FALUUnaryToUnit<"$u.$t = DOUBLE $t1 $f", 0b10001>;
def FALUSglIntToUnit: FALUUnaryToUnit<"$u.$t = SINGLE $t1 $f", 0b10000>;
def FALUSglDblToUnit: FALUCommon <
	  (outs Unit:$u, TPort:$t), (ins TPort:$t1, FALUDblOptions:$f),
	  				 			   "$u.$t = SINGLE $t1 (D)$f" >
{
  bits<4> u;
  bits<2> t;
  bits<2> t1;
  bits<4> f;
  let Inst{25-21} = 0b10000;
  let Inst{20-17} = f;
  let Inst{16-15} = 0b01;
  let Inst{14-13} = t1;
  let Inst{5-4} = t;
  let Inst{3-0} = u;
}
def FALUIntSglToUnit: FALUUnaryToUnit<"$u.$t = INT $t1 $f", 0b10010>;
def FALUIntDblToUnit: FALUCommon <
	  (outs Unit:$u, TPort:$t), (ins TPort:$t1, FALUDblOptions:$f),
	  				 			   "$u.$t = INT $t1 (D)$f" >
{
  bits<4> u;
  bits<2> t;
  bits<2> t1;
  bits<4> f;
  let Inst{25-21} = 0b10010;
  let Inst{20-17} = f;
  let Inst{16-15} = 0b01;
  let Inst{14-13} = t1;
  let Inst{5-4} = t;
  let Inst{3-0} = u;
}
def FALUUIntSglToUnit: FALUUnaryToUnit<"$u.$t = UINT $t1 $f", 0b10011>;
def FALUUIntDblToUnit: FALUCommon <
	  (outs Unit:$u, TPort:$t), (ins TPort:$t1, FALUDblOptions:$f),
	  				 			   "$u.$t = UINT $t1 (D)$f" >
{
  bits<4> u;
  bits<2> t;
  bits<2> t1;
  bits<4> f;
  let Inst{25-21} = 0b10011;
  let Inst{20-17} = f;
  let Inst{16-15} = 0b01;
  let Inst{14-13} = t1;
  let Inst{5-4} = t;
  let Inst{3-0} = u;
}
def FALURecipToUnit: FALUUnaryToUnit<"$u.$t = RECIP $t1 $f", 0b11000>;
def FALURSqRtToUnit: FALUUnaryToUnit<"$u.$t = RSQRT $t1 $f", 0b11001>;

/*************** MReg Binary Instructions *******************/
class FALUUnaryToMw3<string asm, bits<5> opc> : FALUUnary <
	      (outs MReg:$mi), (ins  TPort:$t1, FALUOptions:$f), asm >
{
  bits<8> mi;
  let Inst{25-21} = opc;
  let Inst{10-4} = mi{6-0};
  let Inst{2} = mi{7};
  let Inst{1-0} = 0;
}

def FALUAbsToMw3: FALUUnaryToMw3<"$mi.w3 = ABS $t1 $f", 0b00101>;
def FALUDblToMw3: FALUUnaryToMw3<"$mi.w3 = DOUBLE $t1 $f", 0b10001>;
def FALUSglIntToMw3: FALUUnaryToMw3<"$mi.w3 = SINGLE $t1 $f", 0b10000>;
def FALUSglDblToMw3: FALUCommon <
	  (outs MReg:$mi), (ins TPort:$t1, FALUDblOptions:$f),
	  				 			 "$mi.w3 = SINGLE $t1 (D)$f" >
{
  bits<8> mi;
  bits<2> t1;
  bits<4> f;
  let Inst{25-21} = 0b10000;
  let Inst{20-17} = f;
  let Inst{16-15} = 0b01;
  let Inst{14-13} = t1;
  let Inst{10-4} = mi{6-0};
  let Inst{2} = mi{7};
  let Inst{1-0} = 0;
}
def FALUIntSglToMw3: FALUUnaryToMw3<"$mi.w3 = INT $t1 $f", 0b10010>;
def FALUIntDblToMw3: FALUCommon <
	  (outs MReg:$mi), (ins TPort:$t1, FALUDblOptions:$f),
	  				 			 "$mi.w3 = INT $t1 (D)$f" >
{
  bits<8> mi;
  bits<2> t1;
  bits<4> f;
  let Inst{25-21} = 0b10010;
  let Inst{20-17} = f;
  let Inst{16-15} = 0b01;
  let Inst{14-13} = t1;
  let Inst{10-4} = mi{6-0};
  let Inst{2} = mi{7};
  let Inst{1-0} = 0;
}
def FALUUIntSglToMw3: FALUUnaryToMw3<"$mi.w3 = UINT $t1 $f", 0b10011>;
def FALUUIntDblToMw3: FALUCommon <
	  (outs MReg:$mi), (ins TPort:$t1, FALUDblOptions:$f),
	  				 			 "$mi.w3 = UINT $t1 (D)$f" >
{
  bits<8> mi;
  bits<2> t1;
  bits<4> f;
  let Inst{25-21} = 0b10011;
  let Inst{20-17} = f;
  let Inst{16-15} = 0b01;
  let Inst{14-13} = t1;
  let Inst{10-4} = mi{6-0};
  let Inst{2} = mi{7};
  let Inst{1-0} = 0;
}
def FALURecipToMw3: FALUUnaryToMw3<"$mi.w3 = RECIP $t1 $f", 0b11000>;
def FALURSqRtToMw3: FALUUnaryToMw3<"$mi.w3 = RSQRT $t1 $f", 0b11001>;

/****************** FALU BIU Unary Instructions ***********************/
class FALUUnaryToBIU<string asm, bits<5> opc> : FALUUnary <
	      (outs BIU:$b), (ins  TPort:$t1, FALUOptions:$f), asm >
{
  bits<2> b;

  let Inst{25-21} = opc;
  let Inst{3-2} = b;
  let Inst{1-0} = 0b10;
}

def FALUAbsToBIU: FALUUnaryToBIU<"$b = ABS $t1 $f", 0b00101>;
def FALUDblToBIU: FALUUnaryToBIU<"$b = DOUBLE $t1 $f", 0b10001>;
def FALUSglIntToBIU: FALUUnaryToBIU<"$b = SINGLE $t1 $f", 0b10000>;
def FALUSglDblToBIU: FALUCommon <
	  (outs BIU:$b), (ins TPort:$t1, FALUDblOptions:$f),
	  				 			 "$b = SINGLE $t1 (D)$f" >
{
  bits<2> b;
  bits<2> t1;
  bits<4> f;
  let Inst{25-21} = 0b10000;
  let Inst{20-17} = f;
  let Inst{16-15} = 0b01;
  let Inst{14-13} = t1;
  let Inst{3-2} = b;
  let Inst{1-0} = 0b10;
}
def FALUIntSglToBIU: FALUUnaryToBIU<"$b = INT $t1 $f", 0b10010>;
def FALUIntDblToBIU: FALUCommon <
	  (outs BIU:$b), (ins TPort:$t1, FALUDblOptions:$f),
	  				 			 "$b = INT $t1 (D)$f" >
{
  bits<2> b;
  bits<2> t1;
  bits<4> f;
  let Inst{25-21} = 0b10010;
  let Inst{20-17} = f;
  let Inst{16-15} = 0b01;
  let Inst{14-13} = t1;
  let Inst{3-2} = b;
  let Inst{1-0} = 0b10;
}
def FALUUIntSglToBIU: FALUUnaryToBIU<"$b = UINT $t1 $f", 0b10011>;
def FALUUIntDblToBIU: FALUCommon <
	  (outs BIU:$b), (ins TPort:$t1, FALUDblOptions:$f),
	  				 			 "$b = UINT $t1 (D)$f" >
{
  bits<2> b;
  bits<2> t1;
  bits<4> f;
  let Inst{25-21} = 0b10011;
  let Inst{20-17} = f;
  let Inst{16-15} = 0b01;
  let Inst{14-13} = t1;
  let Inst{3-2} = b;
  let Inst{1-0} = 0b10;
}
def FALURecipToBIU: FALUUnaryToBIU<"$b = RECIP $t1 $f", 0b11000>;
def FALURSqRtToBIU: FALUUnaryToBIU<"$b = RSQRT $t1 $f", 0b11001>;

//===----------------------------------------------------------------------===//
//						Instructions for IMAC: Slots 7
//===----------------------------------------------------------------------===//
class IMACComon<dag outs, dag ins, string asm>
	  : MMPUInst < outs, ins, asm >
{
  let Inst{47-44} = 7; // slot
}
////////////////////////////
class IMACMul<dag outs, dag ins, string asm> : IMACComon < outs, ins, asm >
{
  bits<2> tm;
  bits<2> tn;
  bits<8> f;
  let Inst{28-25} = 0b0001;
  let Inst{24-17} = f;
  let Inst{16-15} = tm;
  let Inst{14-13} = tn;
}
def  IMACMulToMw3 : IMACMul < 
	 (outs MReg:$m),
	 (ins  TPort:$tm, TPort:$tn, IMACOptions:$f),
	 "$m.w3 = $tm * $tn (T)$f">
{
  bits<7> m;
  let Inst{10-4}  = m;
  let Inst{3-0}   = 0;
}
def  IMACMulToBIU : IMACMul < 
	 (outs BIU:$b),
	 (ins  TPort:$tm, TPort:$tn, IMACOptions:$f),
	 "$b = $tm * $tn (T)$f">
{
  bits<2> b;
  let Inst{10-4}  = 0;
  let Inst{3-2} = b;
  let Inst{1-0} = 0b10;
}
def  IMACMulToUnit : IMACMul < 
	 (outs Unit:$u, TPort:$t),
	 (ins  TPort:$tm, TPort:$tn, IMACOptions:$f),
	 "$u.$t = $tm * $tn (T)$f">
{
  bits<4> u;
  bits<2> t;
  let Inst{10-6}  = 0;
  let Inst{5-4}   = t;
  let Inst{3-0}   = u;
}
////////////////////////////
class IMACMulAccOnMRXfer<dag outs, dag ins, string asm> : IMACComon < outs, ins, asm >
{
  bits<2> tm;
  bits<2> tn;
  bits<8> f;
  let Inst{28-25} = 0b1010;
  let Inst{24-17} = f;
  let Inst{16-15} = tm;
  let Inst{14-13} = tn;
}
def  IMACMulAccOnMRXferToMw3 : IMACMulAccOnMRXfer < 
	 (outs MReg:$m),
	 (ins  TPort:$tm, TPort:$tn, IMACOptions:$f),
	 "$m.w3 = MR += $tm * $tn$f">
{
  bits<7> m;
  let Inst{10-4}  = m;
  let Inst{3-0}   = 0;
}
def  IMACMulAccOnMRXferToBIU : IMACMulAccOnMRXfer < 
	 (outs BIU:$b),
	 (ins  TPort:$tm, TPort:$tn, IMACOptions:$f),
	 "$b = MR += $tm * $tn$f">
{
  bits<2> b;
  let Inst{10-4}  = 0;
  let Inst{3-2} = b;
  let Inst{1-0} = 0b10;
}
def  IMACMulAccOnMRXferToUnit : IMACMulAccOnMRXfer < 
	 (outs Unit:$u, TPort:$t),
	 (ins  TPort:$tm, TPort:$tn, IMACOptions:$f),
	 "$u.$t = MR += $tm * $tn$f">
{
  bits<4> u;
  bits<2> t;
  let Inst{10-6}  = 0;
  let Inst{5-4}   = t;
  let Inst{3-0}   = u;
}
////////////////////////////
class IMACMulExt<dag outs, dag ins, string asm> : IMACComon < outs, ins, asm >
{
  bits<2> tm;
  bits<2> tn;
  bits<8> f;
  let Inst{28-25} = 0b0010;
  let Inst{24-17} = f;
  let Inst{16-15} = tm;
  let Inst{14-13} = tn;
}
def  IMACMulExtToMw3 : IMACMulExt < 
	 (outs MRegExt:$m),
	 (ins  TPort:$tm, TPort:$tn, IMACOptions:$f),
	 "$m.w3 = $tm * $tn $f">
{
  bits<7> m;
  let Inst{10-4}  = m;
  let Inst{3-0}   = 0;
}
def  IMACMulExtToBIU : IMACMulExt < 
	 (outs BIU:$b),
	 (ins  TPort:$tm, TPort:$tn, IMACOptions:$f),
	 "$b = $tm * $tn $f">
{
  bits<2> b;
  let Inst{10-4}  = 0;
  let Inst{3-2} = b;
  let Inst{1-0} = 0b10;
}
def  IMACMulExtToUnit : IMACMulExt < 
	 (outs Unit:$u, TPortExt:$t),
	 (ins  TPort:$tm, TPort:$tn, IMACOptions:$f),
	 "$u.$t = $tm * $tn $f">
{
  bits<4> u;
  bits<2> t;
  let Inst{10-6}  = 0;
  let Inst{5-4}   = t;
  let Inst{3-0}   = u;
}
////////////////////////////
class IMACMulExtAccOnMRXfer<dag outs, dag ins, string asm> : IMACComon < outs, ins, asm >
{
  bits<2> tm;
  bits<2> tn;
  bits<8> f;
  let Inst{28-25} = 0b1010;
  let Inst{24-22} = f{7-5};
  let Inst{21} = 1;
  let Inst{20-17} = f{3-0};
  let Inst{16-15} = tm;
  let Inst{14-13} = tn;
}
def  IMACMulExtAccOnMRXferToMw3 : IMACMulExtAccOnMRXfer < 
	 (outs MRegExt:$m),
	 (ins  TPort:$tm, TPort:$tn, IMACOptions:$f),
	 "$m.w3 = MR += $tm * $tn (L)$f">
{
  bits<7> m;
  let Inst{10-4}  = m;
  let Inst{3-0}   = 0;
}
def  IMACMulExtAccOnMRXferToBIU : IMACMulExtAccOnMRXfer < 
	 (outs BIU:$b),
	 (ins  TPort:$tm, TPort:$tn, IMACOptions:$f),
	 "$b = MR += $tm * $tn (L)$f">
{
  bits<2> b;
  let Inst{10-4}  = 0;
  let Inst{3-2} = b;
  let Inst{1-0} = 0b10;
}
def  IMACMulExtAccOnMRXferToUnit : IMACMulExtAccOnMRXfer < 
	 (outs Unit:$u, TPortExt:$t),
	 (ins  TPort:$tm, TPort:$tn, IMACOptions:$f),
	 "$u.$t = MR += $tm * $tn (L)$f">
{
  bits<4> u;
  bits<2> t;
  let Inst{10-6}  = 0;
  let Inst{5-4}   = t;
  let Inst{3-0}   = u;
}
////////////////////////////
class IMACAccOnMRXfer<dag outs, dag ins, string asm> : IMACComon < outs, ins, asm >
{
  bits<2> tm;
  bits<8> f;
  let Inst{28-25} = 0b1100;
  let Inst{24-17} = f;
  let Inst{16-15} = tm;
}
def  IMACAccOnMRXferToMw3 : IMACAccOnMRXfer < 
	 (outs MReg:$m),
	 (ins  TPort:$tm, IMACOptions:$f),
	 "$m.w3 = MR += $tm $f">
{
  bits<7> m;
  let Inst{10-4}  = m;
  let Inst{3-0}   = 0;
}
def  IMACAccOnMRXferToBIU : IMACAccOnMRXfer < 
	 (outs BIU:$b),
	 (ins  TPort:$tm, IMACOptions:$f),
	 "$b = MR += $tm $f">
{
  bits<2> b;
  let Inst{10-4}  = 0;
  let Inst{3-2} = b;
  let Inst{1-0} = 0b10;
}
def  IMACAccOnMRXferToUnit : IMACAccOnMRXfer < 
	 (outs Unit:$u, TPort:$t),
	 (ins  TPort:$tm, IMACOptions:$f),
	 "$u.$t = MR += $tm $f">
{
  bits<4> u;
  bits<2> t;
  let Inst{10-6}  = 0;
  let Inst{5-4}   = t;
  let Inst{3-0}   = u;
}
///////////////////////////////
def  IMACMulAddToMR : IMACComon < 
	 (outs),
	 (ins  TPort:$tm, TPort:$tn, TPort:$tp, IMACOptions:$f),
	 "MR = $tm * $tn + $tp $f">
{
  bits<2> tm;
  bits<2> tn;
  bits<2> tp;
  bits<8> f;
  let Inst{28-25} = 0b1000;
  let Inst{24-17} = f;
  let Inst{16-15} = tm;
  let Inst{14-13} = tn;
  let Inst{12-11} = tp;
}
/////////////////////////////////
def  IMACMulAccOnMR : IMACComon < 
	 (outs),
	 (ins  TPort:$tm, TPort:$tn, IMACOptions:$f),
	 "MR += $tm * $tn $f">
{
  bits<2> tm;
  bits<2> tn;
  bits<8> f;
  let Inst{28-25} = 0b1001;
  let Inst{24-17} = f;
  let Inst{16-15} = tm;
  let Inst{14-13} = tn;
}
///////////
def  IMACAccOnMR : IMACComon < 
	 (outs),
	 (ins  TPort:$tm, IMACOptions:$f),
	 "MR += $tm $f">
{
  bits<2> tm;
  bits<8> f;
  let Inst{28-25} = 0b1011;
  let Inst{24-17} = f;
  let Inst{16-15} = tm;
}
///////////////////////////////////
class IMACAddSubMul<dag outs, dag ins, string asm> : IMACComon < outs, ins, asm >
{
  bits<8> f;
  bits<2> tm;
  bits<2> tn;
  bits<2> tp;
  let Inst{28-25} = 0b0011;
  let Inst{24-17} = f;
  let Inst{16-15} = tm;
  let Inst{14-13} = tn;
  let Inst{12-11} = tp;
}
def  IMACAddSubMulToMw3 : IMACAddSubMul < 
	 (outs MReg:$m),
	 (ins  TPort:$tp, TPort:$tm, TPort:$tn, IMACOptions:$f),
	 "$m.w3 = $tp +- $tm * $tn $f">
{
  bits<8> m;
  let Inst{10-4}  = m{6-0};
  let Inst{2}     = m{7};
  let Inst{1-0}   = 0;
}
def  IMACAddSubMulToBIU : IMACAddSubMul < 
	 (outs BIU:$b),
	 (ins  TPort:$tp, TPort:$tm, TPort:$tn, IMACOptions:$f),
	 "$b = $tp +- $tm * $tn $f">
{
  bits<2> b;
  let Inst{3-2}   = b;
  let Inst{1-0}   = 0b10;
}
def  IMACAddSubMulToUnit : IMACAddSubMul < 
	 (outs Unit:$u, TPort:$t),
	 (ins  TPort:$tp, TPort:$tm, TPort:$tn, IMACOptions:$f),
	 "$u.$t = $tp +- $tm * $tn $f">
{
  bits<4> u;
  bits<2> t;
  let Inst{5-4}   = t;
  let Inst{3-0}   = u;
}

/************************************* FMAC: Slot 9 **************************************/
/* Note: calculators IALU, IMAC, and FALU are peers, meanwhile FMAC can only
 * share operands with FALU. */
class FMACCommon<dag outs, dag ins, string asm> : MMPUInst < outs, ins, asm >
{
  bits<4> f;
  let Inst{47-44} = 9; // slot
  let Inst{20-17} = f;
}
//////////////////////////////
class FMACMul<dag outs, dag ins, string asm> : FMACCommon < outs, ins, asm >
{
  bits<2> tm;
  bits<2> tn;
  let Inst{16-15} = tm;
  let Inst{14-13} = tn;
  let Inst{23-21} = 0b001;
}

def  FMACMulToMw3 : FMACMul < 
	 (outs MReg:$m),
	 (ins  TPort:$tm, TPort:$tn, FMACOptions:$f),
	 "$m.w3 = $tm * $tn $f">
{
  bits<8> m;
  let Inst{10-4}  = m{6-0};
  let Inst{2}   = m{7};
  let Inst{1-0} = 0;
}

def  FMACMulToBIU : FMACMul < 
	 (outs BIU:$b),
	 (ins  TPort:$tm, TPort:$tn, FMACOptions:$f),
	 "$b = $tm * $tn $f">
{
  bits<2> b;
  let Inst{3-2}   = b;
  let Inst{1-0}   = 0b10;
}

def  FMACMulToUnit : FMACMul < 
	 (outs Unit:$u, TPort:$t),
	 (ins  TPort:$tm, TPort:$tn, FMACOptions:$f),
	 "$u.$t = $tm * $tn $f">
{
  bits<4> u;
  bits<2> t;
  let Inst{5-4}   = t;
  let Inst{3-0}   = u;
}
//////////////////////////////
class FMACMulAccOnMRXfer<dag outs, dag ins, string asm> : FMACCommon < outs, ins, asm >
{
  bits<2> tm;
  bits<2> tn;
  let Inst{16-15} = tm;
  let Inst{14-13} = tn;
  let Inst{23-21} = 0b100;
}

def  FMACMulAccOnMRXferToMw3 : FMACMulAccOnMRXfer < 
	 (outs MReg:$m),
	 (ins  TPort:$tm, TPort:$tn, FMACOptions:$f),
	 "$m.w3 = MR += $tm * $tn $f">
{
  bits<8> m;
  let Inst{10-4}  = m{6-0};
  let Inst{2}   = m{7};
  let Inst{1-0} = 0;
}

def  FMACMulAccOnMRXferToBIU : FMACMulAccOnMRXfer < 
	 (outs BIU:$b),
	 (ins  TPort:$tm, TPort:$tn, FMACOptions:$f),
	 "$b = MR += $tm * $tn $f">
{
  bits<2> b;
  let Inst{3-2}   = b;
  let Inst{1-0}   = 0b10;
}

def  FMACMulAccOnMRXferToUnit : FMACMulAccOnMRXfer < 
	 (outs Unit:$u, TPort:$t),
	 (ins  TPort:$tm, TPort:$tn, FMACOptions:$f),
	 "$u.$t = MR += $tm * $tn $f">
{
  bits<4> u;
  bits<2> t;
  let Inst{5-4}   = t;
  let Inst{3-0}   = u;
}
///////////////////////
class FMACComplexMul<dag outs, dag ins, string asm> : FMACCommon < outs, ins, asm >
{
  bits<2> tm;
  bits<2> tn;
  let Inst{16-15} = tm;
  let Inst{14-13} = tn;
  let Inst{23-21} = 0b101;
}
def  FMACComplexMulToMw3 : FMACComplexMul < 
	 (outs MReg:$m),
	 (ins  TPort:$tm, TPort:$tn, FMACOptions:$f),
	 "$m.w3 = $tm ** $tn $f">
{
  bits<8> m;
  let Inst{10-4}  = m{6-0};
  let Inst{2}   = m{7};
  let Inst{1-0} = 0;
}
def  FMACComplexMulToBIU : FMACComplexMul < 
	 (outs BIU:$b),
	 (ins  TPort:$tm, TPort:$tn, FMACOptions:$f),
	 "$b = $tm ** $tn $f">
{
  bits<2> b;
  let Inst{3-2}   = b;
  let Inst{1-0}   = 0b10;
}
def  FMACComplexMulToUnit : FMACComplexMul < 
	 (outs Unit:$u, TPort:$t),
	 (ins  TPort:$tm, TPort:$tn, FMACOptions:$f),
	 "$u.$t = $tm ** $tn $f">
{
  bits<4> u;
  bits<2> t;
  let Inst{5-4}   = t;
  let Inst{3-0}   = u;
}
///////////////////////
def  FMACMulAddToMR : FMACCommon < 
	 (outs),
	 (ins  TPort:$tm, TPort:$tn, TPort:$tp, FMACOptions:$f),
	 "MR = $tm * $tn + $tp $f">
{
  bits<2> tp;
  bits<2> tm;
  bits<2> tn;
  let Inst{23-21} = 0b010;
  let Inst{16-15} = tm;
  let Inst{14-13} = tn;
  let Inst{12-11} = tp;
}
////////////////////////////
def  FMACMulAccOnMR: FMACCommon < 
	 (outs),
	 (ins  TPort:$tm, TPort:$tn, FMACOptions:$f),
	 "MR += $tm * $tn $f">
{
  bits<2> tm;
  bits<2> tn;
  let Inst{23-21} = 0b011;
  let Inst{16-15} = tm;
  let Inst{14-13} = tn;
}

//===----------------------------------------------------------------------===//
//						Instructions for BIU Slots 10 - 12
//===----------------------------------------------------------------------===//
class  BIULdToShu < bits<4> sn, string biu > : MMPUInst < 
	 (outs Shu:$u, TPort:$t),
	 (ins  BIUOptions:$f),
	 !subst("BIU", biu, "$u.$t = BIU.DM $f")>
{
  bits<1> u;
  bits<2> t;
  bits<6> f;

  let Inst{47-44} = sn;
  let Inst{19-17} = 0b100;
  let Inst{16-11} = f;
  let Inst{5-4} = t;
  let Inst{2} = u;
  let Inst{1-0} = 0b01;
}

def BIU0LdToShu :BIULdToShu<10, "BIU0">; 
def BIU1LdToShu :BIULdToShu<11, "BIU1">; 
def BIU2LdToShu :BIULdToShu<12, "BIU2">; 

class  BIULdToCalc < bits<4> sn, string biu > : MMPUInst < 
	 (outs Calc:$u, TPort:$t, IPort:$w),
	 (ins  BIUOptions:$f),
	 !subst("BIU", biu, "$u.$t $w = BIU.DM $f")>
{
  bits<2> u;
  bits<2> t;
  bits<2> w;
  bits<6> f;

  let Inst{47-44} = sn;
  let Inst{19-17} = 0b100;
  let Inst{16-11} = f;
  let Inst{7-6} = w;
  let Inst{5-4} = t;
  let Inst{3-2} = u;
  let Inst{1-0} = 0b11;
}

def BIU0LdToCalc :BIULdToCalc<10, "BIU0">; 
def BIU1LdToCalc :BIULdToCalc<11, "BIU1">; 
def BIU2LdToCalc :BIULdToCalc<12, "BIU2">; 

class  BIULdToMw< bits<4> sn, string asm > : MMPUInst < 
	 (outs MReg:$m), (ins  BIUOptions:$f), asm>
{
  bits<8> m;
  bits<6> f;

  let Inst{47-44} = sn;
  let Inst{19-17} = 0b100;
  let Inst{16-11} = f;
  let Inst{10-4} = m{6-0};
  let Inst{2} = m{7};
}

def BIU0LdToMw0 :BIULdToMw<10, "$m.w0 = BIU0.DM $f">; 
def BIU1LdToMw1 :BIULdToMw<11, "$m.w1 = BIU1.DM $f">; 
def BIU2LdToMw2 :BIULdToMw<12, "$m.w2 = BIU2.DM $f">; 

class  BIUStToDM < bits<4> sn, string biu >: MMPUInst < 
	 (outs),
	 (ins BIUStOptions:$f),
	 !subst("BIU", biu, "DM = BIU $f")>
{
  bits<17> f;
  bit ind;

  let Inst{47-44} = sn;
  let Inst{19-17} = 0b101;
  let Inst{16-0}  = f;
}

def BIU0StToDM :BIUStToDM<10, "BIU0">; 
def BIU1StToDM :BIUStToDM<11, "BIU1">; 
def BIU2StToDM :BIUStToDM<12, "BIU2">;

class  BIUMovToMw < bits<4> sn, string asm >: MMPUInst < 
	 (outs MReg:$m), (ins COption:$cf), asm >
{
  bits<8> m;
  bits<2> cf;

  let Inst{47-44} = sn;
  let Inst{19-17} = 0b110;
  let Inst{16-15} = cf;
  let Inst{10-4} = m{6-0};
  let Inst{2} = m{7};
}

def BIU0MovToMw0 :BIUMovToMw<10, "$m.w0 = BIU0.KG $cf">; 
def BIU1MovToMw1 :BIUMovToMw<11, "$m.w1 = BIU1.KG $cf">; 
def BIU2MovToMw2 :BIUMovToMw<12, "$m.w2 = BIU2.KG $cf">;

//===----------------------------------------------------------------------===//
//						Instructions for 13
//===----------------------------------------------------------------------===//
class BranchCommon< dag outs, dag ins, string asm > :
	    MMPUInst<outs, ins, asm>
{
  let Inst{47-44} = 13;
}

def KUImm4 : Operand<i32>
{
  let PrintMethod = "printKUImm4";
//let EncoderMethod = "getKUImm4Encoding";
//let DecoderMethod = "DecodeKUImm4";
}

def  RepeatImm : BranchCommon <
	   (outs ), (ins i32imm:$imm), "REPEAT AT ( $imm )">
{
  bits<10> imm;
  
  let Inst{21-18} = 0b0001;
  let Inst{17-8} = imm;
}

def  RepeatConds : BranchCommon < 
	   (outs ), (ins KUImm4:$v, Conds:$k), "REPEAT AT ( $k $v )">
{
  bits<4> k;
  bits<4> v;
  
  let Inst{21-18} = 0b0010;
  let Inst{7-4} = v;
  let Inst{3-0} = k;
}

// used for branch, could be imm or label, so we set a specific decode method.
def UImm10 : Operand<i32>
{
  let PrintMethod = "printUImm10";
  let EncoderMethod = "getUImm10Encoding";
  let DecoderMethod = "DecodeUImm10";
}

def Loop : BranchCommon <
	  (outs) , (ins UImm10:$i, KUImm4:$v, Conds:$k), "LPTO $i AT ( $k $v )">
{
  bits<10> i;
  bits<4> v;
  bits<4> k;

  let Inst{21-18} = 0b0011;
  let Inst{17-8} = i;
  let Inst{7-4} = v;
  let Inst{3-0} = k;
}

def MPUStop : BranchCommon <(outs) , (ins), "MPU.Stop"> 
{
  let Inst{21-18} = 0b0100;
}
