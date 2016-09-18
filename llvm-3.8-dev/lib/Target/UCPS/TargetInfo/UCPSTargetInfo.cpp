//===-- UCPSTargetInfo.cpp - UCPS Target Implementation -----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "UCPS.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/TargetRegistry.h"
using namespace llvm;

Target llvm::TheUCPSTarget;

extern "C" void LLVMInitializeUCPSTargetInfo() {
  RegisterTarget<Triple::ucps> X(TheUCPSTarget, "ucps", "MaPU MPU");
}

// Just a place holder
extern "C" void
LLVMInitializeUCPSTarget()
{
	// Register the target.
}
