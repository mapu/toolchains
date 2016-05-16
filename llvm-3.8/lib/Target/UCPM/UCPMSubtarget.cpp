//===-- UCPMSubtarget.cpp - UCPM Subtarget Information ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the UCPM specific subclass of TargetSubtargetInfo.
//
//===----------------------------------------------------------------------===//

#include "UCPMSubtarget.h"
#include "UCPM.h"
#include "llvm/Support/TargetRegistry.h"

#define DEBUG_TYPE "ucpm-subtarget"

#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR
#include "UCPMGenSubtargetInfo.inc"

using namespace llvm;

UCPMSubtarget::UCPMSubtarget(const Triple &TT, StringRef CPU,
                                     StringRef FS)
  : UCPMGenSubtargetInfo(TT, CPU, FS)/*,
    DL("e-p:64:32:32-i8:8:8-i16:16:16-i32:32:32-f32:32:32-f64:32:32-n8:16:32"
       "-S32-a0:8:8-s0:32:32")*/ {
  // Determine default and/or user specified characteristics
  std::string CPUName;
  if (CPU.empty()) CPUName = "UCPM";

  // Parse features string.
  ParseSubtargetFeatures(CPUName, FS);
}
