//===-- MMPULiteMCTargetDesc.h - MMPULite Target Descriptions ---------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file provides MMPULite specific target descriptions.
//
//===----------------------------------------------------------------------===//

#ifndef MMPULITEMCTARGETDESC_H
#define MMPULITEMCTARGETDESC_H

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

	extern Target TheMMPULiteTarget;

	MCObjectWriter *
	createMMPULiteELFObjectWriter(raw_pwrite_stream &OS, uint8_t OSABI);

	MCAsmBackend *
	createMMPULiteAsmBackend(const Target &T, const MCRegisterInfo &MRI,
	                         const Triple &TT, StringRef CPU);

	MCCodeEmitter *
	createMMPULiteMCCodeEmitter(const MCInstrInfo &MCII,
                              const MCRegisterInfo & MCRI,
                              MCContext &Ctx);
  
  
  /// createX86_64ELFORelocationInfo - Construct X86-64 ELF relocation info.
  MCRelocationInfo *createMMPULiteELFRelocationInfo(MCContext &Ctx);
}  // End llvm namespace

// Defines symbolic names for MMPULite registers.  This defines a mapping from
// register name to register number.
#define GET_REGINFO_ENUM
#include "MMPULiteGenRegisterInfo.inc"

// Defines symbolic names for the MMPULite instructions.
#define GET_INSTRINFO_ENUM
#include "MMPULiteGenInstrInfo.inc"

#define GET_SUBTARGETINFO_ENUM
#include "MMPULiteGenSubtargetInfo.inc"

#endif
