//===-- MMPUTargetInfo.cpp - MMPU Target Implementation -----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "../MMPU.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/TargetRegistry.h"
using namespace llvm;

Target llvm::TheMMPUTarget;

extern "C" void LLVMInitializeMMPUTargetInfo() {
  RegisterTarget<Triple::mmpu> X(TheMMPUTarget, "mmpu", "MaPU MPU");
}

// Just a place holder
extern "C" void
LLVMInitializeMMPUTarget()
{
	// Register the target.
}
