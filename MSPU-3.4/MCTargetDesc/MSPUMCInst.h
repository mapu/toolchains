//===- MSPUMCInst.h - MSPU sub-class of MCInst ----------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This class extends MCInst to allow some VLIW annotations.
//
//===----------------------------------------------------------------------===//

#ifndef MSPUMCINST_H
#define MSPUMCINST_H

#include "../MSPUTargetMachine.h"
#include "llvm/MC/MCInst.h"

namespace llvm {
  class MCOperand;

  class MSPUMCInst: public MCInst {

  private:
    // MCID is set during instruction lowering.
    // It is needed in order to access TSFlags for
    // use in checking MC instruction properties.
    const MCInstrDesc *MCID;

  public:
    explicit MSPUMCInst():
      MCInst(), MCID(0) {};

    MSPUMCInst(const MCInstrDesc& mcid):
      MCInst(), MCID(&mcid) {};

    // Return the slots used by the insn.
    unsigned getUnits(const MSPUTargetMachine* TM) const;

    void setDesc(const MCInstrDesc& mcid) { MCID = &mcid; };
    const MCInstrDesc& getDesc(void) const { return *MCID; };

    static bool
    InstAllowsImmExt(const MCInst &MI);
  };

  // we reserve this array to alieveate memory management.
  extern MSPUMCInst MSPUMCInstArray[4];
}

#endif
