//===-- MMPUMCAsmInfo.h - MMPU asm properties ----------------*- C++ -*--===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the declaration of the MMPUMCAsmInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef MMPUTARGETASMINFO_H
#define MMPUTARGETASMINFO_H

#include "llvm/MC/MCAsmInfoELF.h"

namespace llvm {
  class StringRef;
  class Target;

  class MMPUMCAsmInfo : public MCAsmInfoELF {
  public:
    explicit MMPUMCAsmInfo(StringRef TT);
  };

} // namespace llvm

#endif
