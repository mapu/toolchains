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