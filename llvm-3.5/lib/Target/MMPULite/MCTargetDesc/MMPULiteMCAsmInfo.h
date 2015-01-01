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

#include "llvm/MC/MCAsmInfo.h"

namespace llvm {
  class StringRef;
  class Target;

  class MMPULiteELFMCAsmInfo : public MCAsmInfo {
  public:
    explicit MMPULiteELFMCAsmInfo(/*const Target &T,*/ StringRef TT);
  };

} // namespace llvm

#endif
