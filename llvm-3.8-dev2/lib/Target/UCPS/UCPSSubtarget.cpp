//===-- UCPSSubtarget.cpp - UCPS Subtarget Information ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the UCPS specific subclass of TargetSubtargetInfo.
//
//===----------------------------------------------------------------------===//

#include "UCPSSubtarget.h"
#include "UCPS.h"
#include "llvm/Support/TargetRegistry.h"

#define DEBUG_TYPE "UCPS-subtarget"
#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR
#include "UCPSGenSubtargetInfo.inc"

#include "UCPSGenDFAPacketizer.inc"

using namespace llvm;

UCPSSubtarget::UCPSSubtarget(const Triple &TT,StringRef CPU,
                             StringRef FS, const TargetMachine &TM)
				: UCPSGenSubtargetInfo(TT, CPU, FS),
				  InstrInfo(*this), FrameLowering(*this), TLInfo(TM, *this)
{
	// Determine default and/or user specified characteristics
	std::string CPUName = CPU;
	if(CPUName.empty())
		CPUName = "ucps";

	// Parse features string.
	ParseSubtargetFeatures(CPUName, FS);

	// Initialize scheduling itinerary for the specified CPU,
	// using llvm::MCSubtargetInfo::getInstrItineraryForCPU
	InstrItins = getInstrItineraryForCPU(CPUName);
}
