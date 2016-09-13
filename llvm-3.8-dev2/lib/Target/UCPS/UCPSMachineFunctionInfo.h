//===- UCPSMachineFunctionInfo.h - UCPS Machine Function Info -*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares  UCPS specific per-machine-function information.
//
//===----------------------------------------------------------------------===//
#ifndef UCPSMACHINEFUNCTIONINFO_H
#define UCPSMACHINEFUNCTIONINFO_H

#include "llvm/CodeGen/MachineFunction.h"

namespace llvm {

  class UCPSMachineFunctionInfo : public MachineFunctionInfo {
    virtual void anchor();
  private:
    unsigned GlobalBaseReg;

    /// VarArgsFrameOffset - Frame offset to start of varargs area.
    int VarArgsFrameIndex;

    /// SRetReturnReg - Holds the virtual register into which the sret
    /// argument is passed.
    unsigned SRetReg;

    /// Holds for each function where on the stack the Frame Pointer must be
    /// saved. This is used on Prologue and Epilogue to emit FP save/restore
    int FPStackOffset;

    /// Holds for each function where on the stack the Return Address must be
    /// saved. This is used on Prologue and Epilogue to emit RA save/restore
    int RAStackOffset;

    // pseudo instructions for adjust start address of alloca.
    std::vector<MachineInstr*> AllocaAdjustInsts;

  public:
    UCPSMachineFunctionInfo()
      : GlobalBaseReg(0), VarArgsFrameIndex(0), SRetReg(0),
        FPStackOffset(0), RAStackOffset(0) {}

    explicit UCPSMachineFunctionInfo(MachineFunction &MF)
      : GlobalBaseReg(0), VarArgsFrameIndex(0), SRetReg(0),
        FPStackOffset(0), RAStackOffset(0) {}

    unsigned getGlobalBaseReg() const { return GlobalBaseReg; }
    void setGlobalBaseReg(unsigned Reg) { GlobalBaseReg = Reg; }

    int getVarArgsFrameIndex() const { return VarArgsFrameIndex; }
    void setVarArgsFrameIndex(int Index) { VarArgsFrameIndex = Index; }

    unsigned getSRetReg() const { return SRetReg; }
    void setSRetReg(unsigned Reg) { SRetReg = Reg; }

    int getFPStackOffset() const { return FPStackOffset; }
    void setFPStackOffset(int Off) { FPStackOffset = Off; }

    int getRAStackOffset() const { return RAStackOffset; }
    void setRAStackOffset(int Off) { RAStackOffset = Off; }

    void addAllocaAdjustInst(MachineInstr* MI) {
      AllocaAdjustInsts.push_back(MI);
    }

    const std::vector<MachineInstr*>& getAllocaAdjustInsts() {
      return AllocaAdjustInsts;
    }

  };
}

#endif
