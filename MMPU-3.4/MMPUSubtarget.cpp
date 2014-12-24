//===-- MMPUSubtarget.cpp - MMPU Subtarget Information ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the MMPU specific subclass of TargetSubtargetInfo.
//
//===----------------------------------------------------------------------===//

#include "MMPUSubtarget.h"
#include "MMPU.h"
#include "llvm/Support/TargetRegistry.h"

#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR
#include "MMPUGenSubtargetInfo.inc"

using namespace llvm;

MMPUSubtarget::MMPUSubtarget(const std::string &TT,
							const std::string &CPU,
							const std::string &FS)
				: MMPUGenSubtargetInfo(TT, CPU, FS)
{
	// Determine default and/or user specified characteristics
	std::string CPUName;
	if(CPU.empty())
		CPUName = "MMPU";

	// Parse features string.
	ParseSubtargetFeatures(CPUName, FS);
}
