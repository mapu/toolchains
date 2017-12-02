//===-- MMPULiteFixupKinds.h - MMPULite Specific Fixup Entries ----------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_MMPULITE_FIXUPKINDS_H
#define LLVM_MMPULITE_FIXUPKINDS_H

#include "llvm/MC/MCFixup.h"

namespace llvm {
	namespace MMPULite {
		// Although most of the current fixup types reflect a unique relocation,
		// one can have multiple fixup types for a given relocation and thus need
		// to be uniquely named.
		//
		// This table *must* be in the save order of
		// MCFixupKindInfo Infos[MMPULite::NumTargetFixupKinds]
		// in MMPULiteAsmBackend.cpp.
		enum FixupKind {

			// Branch fixups.
			fixup_MMPULite_11 = FirstTargetFixupKind,

			// Markers
			LastTargetFixupKind,
			NumTargetFixupKinds = LastTargetFixupKind - FirstTargetFixupKind
		};
	}  // namespace MMPULite
}  // namespace llvm

#endif // LLVM_MMPULite_MMPULiteFIXUPKINDS_H
