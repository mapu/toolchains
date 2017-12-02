//===-- UCPMMCAsmInfo.h - UCPM asm properties ----------------*- C++ -*--===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the declaration of the UCPMMCAsmInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef UCPMTARGETASMINFO_H
#define UCPMTARGETASMINFO_H

#include "llvm/MC/MCAsmInfoELF.h"

namespace llvm {
class Triple;

 class UCPMELFMCAsmInfo : public MCAsmInfoELF {
 public:
   explicit UCPMELFMCAsmInfo(const Triple &TT);
 };

} // namespace llvm

#endif
