//===-- UCPSMCAsmInfo.h - UCPS asm properties ----------------*- C++ -*--===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the declaration of the UCPSMCAsmInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef UCPSTARGETASMINFO_H
#define UCPSTARGETASMINFO_H

#include "llvm/MC/MCAsmInfoELF.h"

namespace llvm {
class Triple;

 class UCPSELFMCAsmInfo : public MCAsmInfoELF {
 public:
   explicit UCPSELFMCAsmInfo(const Triple &TT);
 };

} // namespace llvm

#endif
