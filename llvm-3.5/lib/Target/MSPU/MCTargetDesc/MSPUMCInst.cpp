//===- MSPUMCInst.cpp - MSPU sub-class of MCInst --------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This class extends MCInst to allow some MSPU VLIW annotations.
//
//===----------------------------------------------------------------------===//

#include "../MSPUInstrInfo.h"
#include "MSPUMCInst.h"
#include "MSPUMCTargetDesc.h"

using namespace llvm;

// Return the slots used by the insn.
unsigned MSPUMCInst::getUnits(const MSPUTargetMachine* TM) const {
  const MSPUInstrInfo* QII =
    static_cast<const MSPUInstrInfo *>(TM->getSubtargetImpl()->getInstrInfo());
  const InstrItineraryData* II = TM->getSubtargetImpl()->getInstrItineraryData();
  const InstrStage*
    IS = II->beginStage(QII->get(this->getOpcode()).getSchedClass());

  return (IS->getUnits());
}

bool
MSPUMCInst::InstAllowsImmExt(const MCInst &MI)
{
  unsigned opc = MI.getOpcode();

  switch(opc) {
    case MSPUInst::AssignRRegImm11:
    case MSPUInst::AssignJRegImm11:
    case MSPUInst::LdImm:
    case MSPUInst::StImm:
    case MSPUInst::JumpImm:
    case MSPUInst::CallImm:
    case MSPUInst::LoopL0:
    case MSPUInst::LoopL1:
    case MSPUInst::JumpImmCond:
    case MSPUInst::CallImmCond:
      return true;

    default:
      return false;;
  }

  return false;
}

namespace llvm {
  MSPUMCInst MSPUMCInstArray[4];
}
