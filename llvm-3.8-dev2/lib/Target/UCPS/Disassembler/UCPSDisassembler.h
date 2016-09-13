#ifndef UCPSDISASSEMBLER_H
#define UCPSDISASSEMBLER_H

#include "../UCPS.h"
#include "../UCPSSubtarget.h"
#include "../UCPSRegisterInfo.h"

#include "llvm-c/Disassembler.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/Object/ObjectFile.h"
#include "llvm/MC/MCDisassembler/MCDisassembler.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCFixedLenDisassembler.h"
#include "llvm/MC/MCInst.h"
#include "llvm/Support/MemoryObject.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/MathExtras.h"
#include "llvm/Support/Format.h"

using namespace llvm;

typedef MCDisassembler::DecodeStatus DecodeStatus;

namespace {
  /// UCPSDisassemblerBase - a disasembler class for UCPS.
  class UCPSDisassembler: public MCDisassembler {
    private:
      const MCRegisterInfo *RegInfo;
      mutable ArrayRef<uint8_t> MemObj;
      mutable bool isPacketEnd;

    public:
      /// Constructor     - Initializes the disassembler.
      UCPSDisassembler(const MCSubtargetInfo &STI, MCContext &Ctx,
          const MCRegisterInfo *Info, bool bigEndian)
          : MCDisassembler(STI, Ctx), RegInfo(Info), MemObj(), isPacketEnd(true)
      { }

      virtual DecodeStatus
      getInstruction(MCInst& instr, uint64_t& size,
                     ArrayRef<uint8_t> Bytes, uint64_t address,
                     raw_ostream &vStream, raw_ostream &cStream) const override;

      // getMemoryObject
      inline ArrayRef<uint8_t> getMemoryObject() const { return MemObj; }
      inline const MCRegisterInfo *getRegInfo()      const { return RegInfo; }
  };
} // end anonymous namespace

#endif // UCPSDISASSEMBLER_H
