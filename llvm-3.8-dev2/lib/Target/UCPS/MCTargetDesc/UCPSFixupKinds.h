//===-- UCPSFixupKinds.h - UCPS Specific Fixup Entries ----------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_UCPS_FIXUPKINDS_H
#define LLVM_UCPS_FIXUPKINDS_H

#include "llvm/MC/MCFixup.h"

namespace llvm {
	namespace UCPS {
		// Although most of the current fixup types reflect a unique relocation,
		// one can have multiple fixup types for a given relocation and thus need
		// to be uniquely named.
		//
		// This table *must* be in the save order of
		// MCFixupKindInfo Infos[UCPS::NumTargetFixupKinds]
		// in UCPSAsmBackend.cpp

		enum FixupKind {

			fixup_UCPS_PC17 = FirstTargetFixupKind,
			fixup_UCPS_PC28, // imm extension for PC17, bits 27-0 hold original bits 31-4

			fixup_UCPS_11_B5,
			fixup_UCPS_11_B10,

			// CallM specific fixup.
			fixup_UCPS_17,

			// imm extension. bit 27-0.
			fixup_UCPS_28,

			LastTargetFixupKind, // Markers
			NumTargetFixupKinds = LastTargetFixupKind - FirstTargetFixupKind
		};
	}  // namespace UCPS
}  // namespace llvm

#endif // LLVM_UCPS_UCPSFIXUPKINDS_H
