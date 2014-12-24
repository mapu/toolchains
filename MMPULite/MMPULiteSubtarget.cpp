//===-- MMPULiteSubtarget.cpp - MMPULite Subtarget Information ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the MMPULite specific subclass of TargetSubtargetInfo.
//
//===----------------------------------------------------------------------===//

#include "MMPULiteSubtarget.h"
#include "MMPULite.h"
#include "llvm/Support/TargetRegistry.h"

#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR
#include "MMPULiteGenSubtargetInfo.inc"

using namespace llvm;

MMPULiteSubtarget::MMPULiteSubtarget(const std::string &TT,
							const std::string &CPU,
							const std::string &FS)
				: MMPULiteGenSubtargetInfo(TT, CPU, FS)
{
	// Determine default and/or user specified characteristics
	std::string CPUName;
	if(CPU.empty())
		CPUName = "MMPULite";

	// Parse features string.
	ParseSubtargetFeatures(CPUName, FS);
}
