//===-- MMPUMCTargetDesc.h - MMPU Target Descriptions ---------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file provides MMPU specific target descriptions.
//
//===----------------------------------------------------------------------===//

#ifndef MMPUMCTARGETDESC_H
#define MMPUMCTARGETDESC_H

#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCContext.h"

namespace llvm
{
	class Target;
	class MCObjectWriter;
	class MCAsmBackend;
	class MCCodeEmitter;

	extern Target TheMMPUTarget;

	MCObjectWriter *
	createMMPUELFObjectWriter(raw_ostream &OS, uint8_t OSABI);

	MCAsmBackend *createMMPUAsmBackend(const Target &T,
	                                     const MCRegisterInfo &MRI,
	                                     StringRef TT, StringRef CPU);

	MCCodeEmitter *
	createMMPUMCCodeEmitter(const MCInstrInfo &MCII,
                           const MCRegisterInfo & MCRI, const MCSubtargetInfo &STI, MCContext &Ctx);
}  // End llvm namespace

// Defines symbolic names for MMPU registers.  This defines a mapping from
// register name to register number.
#define GET_REGINFO_ENUM
#include "MMPUGenRegisterInfo.inc"

// Defines symbolic names for the MMPU instructions.
#define GET_INSTRINFO_ENUM
#include "MMPUGenInstrInfo.inc"

#define GET_SUBTARGETINFO_ENUM
#include "MMPUGenSubtargetInfo.inc"

#endif
