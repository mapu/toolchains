//===-- UCPMELFObjectWriter.cpp - UCPM ELF Writer ---------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "UCPMFixupKinds.h"
#include "UCPMMCTargetDesc.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/Support/ELF.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

namespace {
// this class describes how to convert unresolved fixups to elf relocations.
class UCPMELFObjectWriter: public MCELFObjectTargetWriter {
public:
  UCPMELFObjectWriter(uint8_t OSABI);

  virtual
  ~UCPMELFObjectWriter();

protected:
  unsigned getRelocType(MCContext &Ctx, const MCValue &Target,
                        const MCFixup &Fixup, bool IsPCRel) const override;
};
}

UCPMELFObjectWriter::UCPMELFObjectWriter(uint8_t OSABI)
  : MCELFObjectTargetWriter(/*Is64Bit*/false, OSABI, ELF::EM_MMPU,
                            /* HasRelocationAddend */true)
/* HasRelocationAddend: decide data struct for elf relocation, i.e.,
 * choose between rel or rela. */
{
}

UCPMELFObjectWriter::~UCPMELFObjectWriter() {
}

unsigned UCPMELFObjectWriter::getRelocType(MCContext &Ctx,
                                               const MCValue &Target,
                                               const MCFixup &Fixup,
                                               bool IsPCRel) const {
  // determine the type of the relocation
  unsigned Type = ELF::R_MAPU_None;

  switch ((unsigned) Fixup.getKind()) {
  case UCPM::fixup_UCPM_11:
    Type = ELF::R_MMPU_11;
    break;
  default:
    llvm_unreachable("invalid fixup kind!");
  }

  return Type;
}

MCObjectWriter *
llvm::createUCPMELFObjectWriter(raw_pwrite_stream &OS, uint8_t OSABI) {
  MCELFObjectTargetWriter *MOTW = new UCPMELFObjectWriter(OSABI);
  return createELFObjectWriter(MOTW, OS, /*IsLittleEndian=*/true);
}
