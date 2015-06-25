#ifndef MSPUDISASSEMBLER_H
#define MSPUDISASSEMBLER_H

#include "../MSPU.h"
#include "../MSPUSubtarget.h"
#include "../MSPURegisterInfo.h"

#include "llvm-c/Disassembler.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/Object/ObjectFile.h"
#include "llvm/MC/MCDisassembler.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCFixedLenDisassembler.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCInst.h"
#include "llvm/Support/MemoryObject.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/MathExtras.h"
#include "llvm/Support/Format.h"

using namespace llvm;

typedef MCDisassembler::DecodeStatus DecodeStatus;

namespace {
  /// MSPUDisassemblerBase - a disasembler class for MSPU.
  class MSPUDisassembler: public MCDisassembler {
    private:
      const MCRegisterInfo *RegInfo;
      mutable ArrayRef<uint8_t> MemObj;
      mutable bool isPacketEnd;

    public:
      /// Constructor     - Initializes the disassembler.
      MSPUDisassembler(const MCSubtargetInfo &STI, MCContext &Ctx,
          const MCRegisterInfo *Info, bool bigEndian)
          : MCDisassembler(STI, Ctx), RegInfo(Info), MemObj(), isPacketEnd(true)
      { }

      virtual ~MSPUDisassembler() { }

      virtual DecodeStatus
      getInstruction(MCInst& instr, uint64_t& size,
                     ArrayRef<uint8_t> Bytes, uint64_t address,
                     raw_ostream &vStream, raw_ostream &cStream) const override;

      // getMemoryObject
      inline ArrayRef<uint8_t> getMemoryObject() const { return MemObj; }
      inline const MCRegisterInfo *getRegInfo()      const { return RegInfo; }
  };
} // end anonymous namespace

#endif // MSPUDISASSEMBLER_H
