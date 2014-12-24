//===-- MSPUMCAsmInfo.h - MSPU asm properties ----------------*- C++ -*--===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the declaration of the MSPUMCAsmInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef MSPUTARGETASMINFO_H
#define MSPUTARGETASMINFO_H

#include "llvm/MC/MCAsmInfoELF.h"

namespace llvm {
  class StringRef;
  class Target;

  class MSPUAsmInfo : public MCAsmInfoELF {
  public:
    explicit MSPUAsmInfo(StringRef TT);
  };

} // namespace llvm

#endif
