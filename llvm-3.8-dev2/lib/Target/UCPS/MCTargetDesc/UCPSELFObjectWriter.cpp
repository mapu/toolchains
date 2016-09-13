//===-- UCPSELFObjectWriter.cpp - UCPS ELF Writer ---------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "UCPSFixupKinds.h"
#include "UCPSMCTargetDesc.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCValue.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/Support/ELF.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

namespace {
// this class describes how to convert unresolved fixups to elf relocations.
class UCPSELFObjectWriter: public MCELFObjectTargetWriter {
public:
  UCPSELFObjectWriter(uint8_t OSABI);

  virtual
  ~UCPSELFObjectWriter();

protected:
  virtual unsigned getRelocType(MCContext &Ctx, const MCValue &Target,
                                const MCFixup &Fixup, bool IsPCRel) const override;
};
}

UCPSELFObjectWriter::UCPSELFObjectWriter(uint8_t OSABI)
  : MCELFObjectTargetWriter(/*Is64Bit*/false, OSABI, ELF::EM_UCPS,/* HasRelocationAddend */
                            true)
/* HasRelocationAddend: decide data struct for elf relocation, i.e., choose between rel or rela. */
{
}

UCPSELFObjectWriter::~UCPSELFObjectWriter() {
}

unsigned UCPSELFObjectWriter::getRelocType(MCContext &Ctx, const MCValue &Target,
                                           const MCFixup &Fixup,
                                           bool IsPCRel) const {
  // determine the type of the relocation
  unsigned Type;
  if (IsPCRel) {
    switch ((unsigned) Fixup.getKind()) {
    case UCPS::fixup_UCPS_PC28:
      Type = ELF::R_UCPS_PC28;
      break;
    case UCPS::fixup_UCPS_PC17:
      Type = ELF::R_UCPS_PC17;
      break;
    case FK_PCRel_1:
      Type = ELF::R_UCPS_PC_8;
      break;
    case FK_PCRel_2:
      Type = ELF::R_UCPS_PC_16;
      break;
    case FK_PCRel_4:
      Type = ELF::R_UCPS_PC_32;
      break;
    case FK_PCRel_8:
      Type = ELF::R_UCPS_PC_64;
      break;
    default:
      llvm_unreachable("invalid fixup kind!");
    }
  } else {
    switch ((unsigned) Fixup.getKind()) {
    case UCPS::fixup_UCPS_11_B5:
      Type = ELF::R_UCPS_11_B5;
      break;
    case UCPS::fixup_UCPS_11_B10:
      Type = ELF::R_UCPS_11_B10;
      break;
    case UCPS::fixup_UCPS_17:
      Type = ELF::R_UCPS_17;
      break;
    case UCPS::fixup_UCPS_28:
      Type = ELF::R_UCPS_28;
      break;
    case FK_Data_1:
      Type = ELF::R_UCPS_8;
      break;
    case FK_Data_2:
      Type = ELF::R_UCPS_16;
      break;
    case FK_Data_4:
      Type = ELF::R_UCPS_32;
      break;
    case FK_Data_8:
      Type = ELF::R_UCPS_64;
      break;
    default:
      llvm_unreachable("invalid fixup kind!");
    }
  }

  return Type;
}

MCObjectWriter *
llvm::createUCPSELFObjectWriter(raw_pwrite_stream &OS, uint8_t OSABI) {
  MCELFObjectTargetWriter *MOTW = new UCPSELFObjectWriter(OSABI);
  return createELFObjectWriter(MOTW, OS, /*IsLittleEndian=*/true);
}
