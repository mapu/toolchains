//===-- MSPUFixupKinds.h - MSPU Specific Fixup Entries ----------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_MSPU_FIXUPKINDS_H
#define LLVM_MSPU_FIXUPKINDS_H

#include "llvm/MC/MCFixup.h"

namespace llvm {
	namespace MSPU {
		// Although most of the current fixup types reflect a unique relocation,
		// one can have multiple fixup types for a given relocation and thus need
		// to be uniquely named.
		//
		// This table *must* be in the save order of
		// MCFixupKindInfo Infos[MSPU::NumTargetFixupKinds]
		// in MSPUAsmBackend.cpp

		enum FixupKind {

			fixup_MSPU_PC17 = FirstTargetFixupKind,
			fixup_MSPU_PC28, // imm extension for PC17, bits 27-0 hold original bits 31-4

			fixup_MSPU_11_B5,
			fixup_MSPU_11_B10,

			// CallM specific fixup.
			fixup_MSPU_17,
			// dcx
			fixup_MSPU_12,
			
			// imm extension. bit 27-0.
			fixup_MSPU_28,

			LastTargetFixupKind, // Markers
			NumTargetFixupKinds = LastTargetFixupKind - FirstTargetFixupKind
		};
	}  // namespace MSPU
}  // namespace llvm

#endif // LLVM_MSPU_MSPUFIXUPKINDS_H
