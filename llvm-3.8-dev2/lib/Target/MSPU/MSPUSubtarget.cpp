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

MSPUSubtarget::MSPUSubtarget(const Triple &TT,StringRef CPU,
                             StringRef FS, const TargetMachine &TM)
				: MSPUGenSubtargetInfo(TT, CPU, FS),
				  InstrInfo(*this), FrameLowering(*this), TLInfo(TM, *this)
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
