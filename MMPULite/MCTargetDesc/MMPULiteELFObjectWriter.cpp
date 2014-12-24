//===-- MMPULiteELFObjectWriter.cpp - MMPULite ELF Writer ---------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "MMPULiteFixupKinds.h"
#include "MMPULiteMCTargetDesc.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/Support/ELF.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

namespace
{
	// this class describes how to convert unresolved fixups to elf relocations.
	class MMPULiteELFObjectWriter: public MCELFObjectTargetWriter
	{
		public:
			MMPULiteELFObjectWriter(uint8_t OSABI);

			virtual
			~MMPULiteELFObjectWriter();

		protected:
			virtual unsigned
			GetRelocType(const MCValue &Target, const MCFixup &Fixup,
							bool IsPCRel,
							bool IsRelocWithSymbol,
							int64_t Addend) const;

			virtual void
			adjustFixupOffset(const MCFixup &Fixup, uint64_t &RelocOffset);
	};
}

MMPULiteELFObjectWriter::MMPULiteELFObjectWriter(uint8_t OSABI)
		: MCELFObjectTargetWriter(/*Is64Bit*/false, OSABI, ELF::EM_MMPU,/* HasRelocationAddend */true)
/* HasRelocationAddend: decide data struct for elf relocation, i.e., choose between rel or rela. */
{
}

MMPULiteELFObjectWriter::~MMPULiteELFObjectWriter()
{
}

unsigned
MMPULiteELFObjectWriter::GetRelocType(const MCValue &Target,
									const MCFixup &Fixup,
									bool IsPCRel,
									bool IsRelocWithSymbol,
									int64_t Addend) const
									{
	// determine the type of the relocation
	unsigned Type = ELF::R_MMPU_None;

	switch((unsigned) Fixup.getKind()) {
		case MMPULite::fixup_MMPULite_11:
			Type = ELF::R_MMPU_11;
			break;
		default:
			llvm_unreachable("invalid fixup kind!");
	}

	return Type;
}

void
MMPULiteELFObjectWriter::
adjustFixupOffset(const MCFixup &Fixup, uint64_t &RelocOffset)
{
	switch((unsigned) Fixup.getKind()) {
		case MMPULite::fixup_MMPULite_11:
			RelocOffset += 0;
			break;
		default:
			llvm_unreachable("invalid fixup kind!");
	}
}

MCObjectWriter *
llvm::createMMPULiteELFObjectWriter(raw_ostream &OS, uint8_t OSABI)
{
	MCELFObjectTargetWriter *MOTW = new MMPULiteELFObjectWriter(OSABI);
	return createELFObjectWriter(MOTW, OS, /*IsLittleEndian=*/true);
}
