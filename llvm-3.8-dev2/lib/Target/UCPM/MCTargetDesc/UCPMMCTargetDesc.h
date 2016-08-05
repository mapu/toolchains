//===-- UCPMMCTargetDesc.h - UCPM Target Descriptions ---------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file provides UCPM specific target descriptions.
//
//===----------------------------------------------------------------------===//

#ifndef UCPMMCTARGETDESC_H
#define UCPMMCTARGETDESC_H

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

	extern Target TheUCPMTarget;

	MCObjectWriter *
	createUCPMELFObjectWriter(raw_pwrite_stream &OS, uint8_t OSABI);

	MCAsmBackend *
	createUCPMAsmBackend(const Target &T, const MCRegisterInfo &MRI,
	                         const Triple &TT, StringRef CPU);

	MCCodeEmitter *
	createUCPMMCCodeEmitter(const MCInstrInfo &MCII,
                              const MCRegisterInfo & MCRI,
                              MCContext &Ctx);
  
  
  /// createX86_64ELFORelocationInfo - Construct X86-64 ELF relocation info.
  MCRelocationInfo *createUCPMELFRelocationInfo(MCContext &Ctx);
}  // End llvm namespace

// Defines symbolic names for UCPM registers.  This defines a mapping from
// register name to register number.
#define GET_REGINFO_ENUM
#include "UCPMGenRegisterInfo.inc"

// Defines symbolic names for the UCPM instructions.
#define GET_INSTRINFO_ENUM
#include "UCPMGenInstrInfo.inc"

#define GET_SUBTARGETINFO_ENUM
#include "UCPMGenSubtargetInfo.inc"

#endif
