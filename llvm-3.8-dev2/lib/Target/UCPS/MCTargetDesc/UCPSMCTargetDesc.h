//===-- UCPSMCTargetDesc.h - UCPS Target Descriptions ---------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file provides UCPS specific target descriptions.
//
//===----------------------------------------------------------------------===//

#ifndef UCPSMCTARGETDESC_H
#define UCPSMCTARGETDESC_H

#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCContext.h"

namespace llvm
{
	class Target;
	class MCObjectWriter;
	class MCAsmBackend;
	class MCCodeEmitter;

	extern Target TheUCPSTarget;

	MCObjectWriter *
	createUCPSELFObjectWriter(raw_pwrite_stream &OS, uint8_t OSABI);

	MCAsmBackend *
	createUCPSAsmBackend(const Target &T,
                       const MCRegisterInfo &MRI, const Triple &TT, StringRef CPU);

	MCCodeEmitter *
	createUCPSMCCodeEmitter(const MCInstrInfo &MCII,
                          const MCRegisterInfo & MCRI, MCContext &Ctx);
}  // End llvm namespace

// Defines symbolic names for UCPS registers.  This defines a mapping from
// register name to register number.
#define GET_REGINFO_ENUM
#include "UCPSGenRegisterInfo.inc"

// Defines symbolic names for the UCPS instructions.
#define GET_INSTRINFO_ENUM
#include "UCPSGenInstrInfo.inc"

#define GET_SUBTARGETINFO_ENUM
#include "UCPSGenSubtargetInfo.inc"

#endif
