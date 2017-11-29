//===-- UCPMTargetInfo.cpp - UCPM Target Implementation -----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "UCPM.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/TargetRegistry.h"
using namespace llvm;

Target llvm::TheUCPMTarget;

extern "C" void LLVMInitializeUCPMTargetInfo() {
  RegisterTarget<Triple::ucpm> X(TheUCPMTarget, "ucpm", "MaPU MPU");
}

// Just a place holder
extern "C" void
LLVMInitializeUCPMTarget()
{
	// Register the target.
}
