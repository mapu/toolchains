//===-- llvm/Target/MSPUTargetObjectFile.h - MSPU Obj. Info -*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_TARGET_MSPU_TARGETOBJECTFILE_H
#define LLVM_TARGET_MSPU_TARGETOBJECTFILE_H

#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"

namespace llvm
{
	/* At present base class TargetLoweringObjectFileELF is used when initializing
	 * TargetLowering class, see MSPUTargetLowering for more info.
	 * So MSPUTargetObjectFile is not used under current implementation.
	 */
	class MSPUTargetObjectFile: public TargetLoweringObjectFileELF
	{
		private:
			const MCSection *SmallDataSection;
			const MCSection *SmallBSSSection;
		public:
			void
			Initialize(MCContext &Ctx, const TargetMachine &TM);

			/// IsGlobalInSmallSection - Return true if this global address should be
			/// placed into small data/bss section.
			bool
			IsGlobalInSmallSection(const GlobalValue *GV,
									const TargetMachine &TM,
									SectionKind Kind) const;

			bool
			IsGlobalInSmallSection(const GlobalValue *GV,
									const TargetMachine &TM) const;

			const MCSection *
			SelectSectionForGlobal(const GlobalValue *GV,
									SectionKind Kind,
									Mangler &Mang,
									const TargetMachine &TM) const;
	};
}  // end namespace llvm

#endif
