//===-- MMPULiteMCAsmInfo.h - MMPULite asm properties ----------------*- C++ -*--===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the declaration of the MMPULiteMCAsmInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef MMPULITETARGETASMINFO_H
#define MMPULITETARGETASMINFO_H

#include "llvm/MC/MCAsmInfoELF.h"

namespace llvm {
class Triple;

 class MMPULiteELFMCAsmInfo : public MCAsmInfoELF {
 public:
   explicit MMPULiteELFMCAsmInfo(const Triple &TT);
 };

} // namespace llvm

#endif
