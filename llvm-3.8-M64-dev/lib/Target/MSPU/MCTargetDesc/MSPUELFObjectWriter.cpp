//===-- MSPUELFObjectWriter.cpp - MSPU ELF Writer ---------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "MSPUFixupKinds.h"
#include "MSPUMCTargetDesc.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCValue.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/Support/ELF.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

namespace {
// this class describes how to convert unresolved fixups to elf relocations.
class MSPUELFObjectWriter: public MCELFObjectTargetWriter {
public:
  MSPUELFObjectWriter(uint8_t OSABI);

  virtual
  ~MSPUELFObjectWriter();

protected:
  virtual unsigned getRelocType(MCContext &Ctx, const MCValue &Target,
                                const MCFixup &Fixup, bool IsPCRel) const override;
};
}

MSPUELFObjectWriter::MSPUELFObjectWriter(uint8_t OSABI)
  : MCELFObjectTargetWriter(/*Is64Bit*/false, OSABI, ELF::EM_MSPU,/* HasRelocationAddend */
                            true)
/* HasRelocationAddend: decide data struct for elf relocation, i.e., choose between rel or rela. */
{
}

MSPUELFObjectWriter::~MSPUELFObjectWriter() {
}

unsigned MSPUELFObjectWriter::getRelocType(MCContext &Ctx, const MCValue &Target,
                                           const MCFixup &Fixup,
                                           bool IsPCRel) const {
  // determine the type of the relocation
  unsigned Type;
  if (IsPCRel) {
    switch ((unsigned) Fixup.getKind()) {
    case MSPU::fixup_MSPU_PC28:
      Type = ELF::R_MSPU_PC28;
      break;
    case MSPU::fixup_MSPU_PC17:
      Type = ELF::R_MSPU_PC17;
      break;
    case FK_PCRel_1:
      Type = ELF::R_MSPU_PC_8;
      break;
    case FK_PCRel_2:
      Type = ELF::R_MSPU_PC_16;
      break;
    case FK_PCRel_4:
      Type = ELF::R_MSPU_PC_32;
      break;
    case FK_PCRel_8:
      Type = ELF::R_MSPU_PC_64;
      break;
    default:
      llvm_unreachable("invalid fixup kind!");
    }
  } else {
    switch ((unsigned) Fixup.getKind()) {
    case MSPU::fixup_MSPU_11_B5:
      Type = ELF::R_MSPU_11_B5;
      break;
    case MSPU::fixup_MSPU_11_B10:
      Type = ELF::R_MSPU_11_B10;
      break;
    case MSPU::fixup_MSPU_17:
      Type = ELF::R_MSPU_17;
      break;
    case MSPU::fixup_MSPU_28:
      Type = ELF::R_MSPU_28;
      break;
    case FK_Data_1:
      Type = ELF::R_MSPU_8;
      break;
    case FK_Data_2:
      Type = ELF::R_MSPU_16;
      break;
    case FK_Data_4:
      Type = ELF::R_MSPU_32;
      break;
    case FK_Data_8:
      Type = ELF::R_MSPU_64;
      break;
    default:
      llvm_unreachable("invalid fixup kind!");
    }
  }

  return Type;
}

MCObjectWriter *
llvm::createMSPUELFObjectWriter(raw_pwrite_stream &OS, uint8_t OSABI) {
  MCELFObjectTargetWriter *MOTW = new MSPUELFObjectWriter(OSABI);
  return createELFObjectWriter(MOTW, OS, /*IsLittleEndian=*/true);
}
