//===-- MSPUTargetInfo.cpp - MSPU Target Implementation -----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "../MSPU.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/TargetRegistry.h"
using namespace llvm;

Target llvm::TheMSPUTarget;

extern "C" void LLVMInitializeMSPUTargetInfo() {
  RegisterTarget<Triple::mspu, false> X(TheMSPUTarget, "mspu", "MaPU SPU");
}
