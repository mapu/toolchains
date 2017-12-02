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

namespace {
// this class describes how to convert unresolved fixups to elf relocations.
class MMPULiteELFObjectWriter: public MCELFObjectTargetWriter {
public:
  MMPULiteELFObjectWriter(uint8_t OSABI);

  virtual
  ~MMPULiteELFObjectWriter();

protected:
  unsigned getRelocType(MCContext &Ctx, const MCValue &Target,
                        const MCFixup &Fixup, bool IsPCRel) const override;
};
}

MMPULiteELFObjectWriter::MMPULiteELFObjectWriter(uint8_t OSABI)
  : MCELFObjectTargetWriter(/*Is64Bit*/false, OSABI, ELF::EM_MMPU,
                            /* HasRelocationAddend */true)
/* HasRelocationAddend: decide data struct for elf relocation, i.e.,
 * choose between rel or rela. */
{
}

MMPULiteELFObjectWriter::~MMPULiteELFObjectWriter() {
}

unsigned MMPULiteELFObjectWriter::getRelocType(MCContext &Ctx,
                                               const MCValue &Target,
                                               const MCFixup &Fixup,
                                               bool IsPCRel) const {
  // determine the type of the relocation
  unsigned Type = ELF::R_MAPU_None;

  switch ((unsigned) Fixup.getKind()) {
  case MMPULite::fixup_MMPULite_11:
    Type = ELF::R_MMPU_11;
    break;
  default:
    llvm_unreachable("invalid fixup kind!");
  }

  return Type;
}

MCObjectWriter *
llvm::createMMPULiteELFObjectWriter(raw_pwrite_stream &OS, uint8_t OSABI) {
  MCELFObjectTargetWriter *MOTW = new MMPULiteELFObjectWriter(OSABI);
  return createELFObjectWriter(MOTW, OS, /*IsLittleEndian=*/true);
}
