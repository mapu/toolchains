//===-- llvm/Target/UCPSTargetObjectFile.h - UCPS Obj. Info -*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_TARGET_UCPS_TARGETOBJECTFILE_H
#define LLVM_TARGET_UCPS_TARGETOBJECTFILE_H

#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"

namespace llvm
{
	/* At present base class TargetLoweringObjectFileELF is used when initializing
	 * TargetLowering class, see UCPSTargetLowering for more info.
	 * So UCPSTargetObjectFile is not used under current implementation.
	 */
	class UCPSTargetObjectFile: public TargetLoweringObjectFileELF
	{
		private:
			MCSection *SmallDataSection;
			MCSection *SmallBSSSection;

	    unsigned getSmallestAddressableSize(const Type *Ty, const GlobalValue *GV,
	        const TargetMachine &TM) const;
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

			MCSection *
			SelectSectionForGlobal(const GlobalValue *GV,
									SectionKind Kind,
									Mangler &Mang,
									const TargetMachine &TM) const override;

	    bool isSmallDataEnabled() const;

	    unsigned getSmallDataSize() const;
	};
}  // end namespace llvm

#endif
