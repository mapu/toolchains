#ifndef MMPUDISASSEMBLER_H
#define MMPUDISASSEMBLER_H

#include "../MMPU.h"
#include "../MMPUSubtarget.h"
#include "../MCTargetDesc/MMPUMCInst.h"

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
  /// MMPUDisassemblerBase - a disasembler class for MMPU.
  class MMPUDisassembler: public MCDisassembler {
    private:
      const MCRegisterInfo *RegInfo;
      mutable const MemoryObject   *MemObj;

    public:
      /// Constructor     - Initializes the disassembler.
      MMPUDisassembler(const MCSubtargetInfo &STI,
          const MCRegisterInfo *Info, bool bigEndian)
          : MCDisassembler(STI), RegInfo(Info)
      { }

      virtual ~MMPUDisassembler() { }

      virtual DecodeStatus
      getInstruction(MCInst& instr, uint64_t& size,
                     const MemoryObject &region, uint64_t address,
                     raw_ostream &vStream, raw_ostream &cStream) const;

      // getMemoryObject
      inline const MemoryObject   *getMemoryObject() const { return MemObj; }
      inline const MCRegisterInfo *getRegInfo()      const { return RegInfo; }
  };
} // end anonymous namespace

#endif // MMPUDISASSEMBLER_H