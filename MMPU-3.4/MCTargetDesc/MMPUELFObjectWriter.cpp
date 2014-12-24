//===-- MMPUELFObjectWriter.cpp - MMPU ELF Writer ---------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "MMPUFixupKinds.h"
#include "MMPUMCTargetDesc.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/Support/ELF.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

namespace
{
	// this class describes how to convert unresolved fixups to elf relocations.
	class MMPUELFObjectWriter: public MCELFObjectTargetWriter
	{
		public:
			MMPUELFObjectWriter(uint8_t OSABI);

			virtual
			~MMPUELFObjectWriter();

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

MMPUELFObjectWriter::MMPUELFObjectWriter(uint8_t OSABI)
		: MCELFObjectTargetWriter(/*Is64Bit*/false, OSABI, ELF::EM_MMPU,/* HasRelocationAddend */true)
/* HasRelocationAddend: decide data struct for elf relocation, i.e., choose between rel or rela. */
{
}

MMPUELFObjectWriter::~MMPUELFObjectWriter()
{
}

unsigned
MMPUELFObjectWriter::GetRelocType(const MCValue &Target,
									const MCFixup &Fixup,
									bool IsPCRel,
									bool IsRelocWithSymbol,
									int64_t Addend) const
{
	// determine the type of the relocation
	unsigned Type = ELF::R_MMPU_None;

  if(IsPCRel) {
    switch((unsigned) Fixup.getKind()) {
      case FK_PCRel_1:
        Type = ELF::R_MMPU_PC_8;
        break;
      case FK_PCRel_2:
        Type = ELF::R_MMPU_PC_16;
        break;
      case FK_PCRel_4:
        Type = ELF::R_MMPU_PC_32;
        break;
      case FK_PCRel_8:
        Type = ELF::R_MMPU_PC_64;
        break;
      default:
        llvm_unreachable("invalid fixup kind!");
    }
  }
  else {
    switch((unsigned) Fixup.getKind()) {
      case MMPU::fixup_MMPU_11:
        Type = ELF::R_MMPU_11;
        break;
      case FK_Data_1:
        Type = ELF::R_MMPU_8;
        break;
      case FK_Data_2:
        Type = ELF::R_MMPU_16;
        break;
      case FK_Data_4:
        Type = ELF::R_MMPU_32;
        break;
      case FK_Data_8:
        Type = ELF::R_MMPU_64;
        break;
      default:
        llvm_unreachable("invalid fixup kind!");
    }
  }
	return Type;
}

void
MMPUELFObjectWriter::
adjustFixupOffset(const MCFixup &Fixup, uint64_t &RelocOffset)
{
	switch((unsigned) Fixup.getKind()) {
		case MMPU::fixup_MMPU_11:
			RelocOffset += 0;
			break;
		default:
			llvm_unreachable("invalid fixup kind!");
	}
}

MCObjectWriter *
llvm::createMMPUELFObjectWriter(raw_ostream &OS, uint8_t OSABI)
{
	MCELFObjectTargetWriter *MOTW = new MMPUELFObjectWriter(OSABI);
	return createELFObjectWriter(MOTW, OS, /*IsLittleEndian=*/true);
}
