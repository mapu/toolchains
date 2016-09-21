//===-- UCPSInstInfo.td - Target Description for UCPS Target -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file describes the UCPS instructions in TableGen format.
//
//===----------------------------------------------------------------------===//

include "UCPSInstrFormats.td"

//===----------------------------------------------------------------------===//
//			Instruction NOP for all slots
//===----------------------------------------------------------------------===//

//from original
def NOP : UCPSInst <(ins), (outs), "NOP"> {
  let Inst = 0;
}

