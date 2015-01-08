//===-- MSPUSubtarget.cpp - MSPU Subtarget Information ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the MSPU specific subclass of TargetSubtargetInfo.
//
//===----------------------------------------------------------------------===//

#include "MSPUSubtarget.h"
#include "MSPU.h"
#include "llvm/Support/TargetRegistry.h"

#define DEBUG_TYPE "MSPU-subtarget"
#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR
#include "MSPUGenSubtargetInfo.inc"

#include "MSPUGenDFAPacketizer.inc"

using namespace llvm;

MSPUSubtarget::MSPUSubtarget(const std::string &TT,
							const std::string &CPU,
							const std::string &FS, TargetMachine &TM)
				: MSPUGenSubtargetInfo(TT, CPU, FS),
				  DL("e-S64-p:32:32:32-i8:8:8-i16:16:16-i32:32:32-f32:32:32-f64:64:64-a0:8:8-s0:8:8-n32"),
				  InstrInfo(*this), FrameLowering(*this), TLInfo(TM), TSInfo(DL)
{
	// Determine default and/or user specified characteristics
	std::string CPUName = CPU;
	if(CPUName.empty())
		CPUName = "mspu";

	// Parse features string.
	ParseSubtargetFeatures(CPUName, FS);

	// Initialize scheduling itinerary for the specified CPU,
	// using llvm::MCSubtargetInfo::getInstrItineraryForCPU
	InstrItins = getInstrItineraryForCPU(CPUName);
}
