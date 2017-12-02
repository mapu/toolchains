//===-- MMPULiteTargetInfo.cpp - MMPULite Target Implementation -----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "MMPULite.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/TargetRegistry.h"
using namespace llvm;

Target llvm::TheMMPULiteTarget;

extern "C" void LLVMInitializeMMPULiteTargetInfo() {
  RegisterTarget<Triple::mmpulite> X(TheMMPULiteTarget, "mmpulite", "MaPU MPU");
}

// Just a place holder
extern "C" void
LLVMInitializeMMPULiteTarget()
{
	// Register the target.
}
