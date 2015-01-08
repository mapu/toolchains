//===-- MSPUTargetMachine.h - Define TargetMachine for MSPU ---*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the MSPU specific subclass of TargetMachine.
//
//===----------------------------------------------------------------------===//

#ifndef MSPUTARGETMACHINE_H
#define MSPUTARGETMACHINE_H

#include "MSPUInstrInfo.h"
#include "MSPUISelLowering.h"
#include "MSPUFrameLowering.h"
#include "MSPUSelectionDAGInfo.h"
#include "MSPUSubtarget.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/Target/TargetFrameLowering.h"

namespace llvm {
class MSPUTargetMachine: public LLVMTargetMachine {
  std::unique_ptr<TargetLoweringObjectFile> TLOF;
  MSPUSubtarget Subtarget;
public:
  MSPUTargetMachine(const Target &T,
                    StringRef TT,
                    StringRef CPU,
                    StringRef FS,
                    const TargetOptions &Options,
                    Reloc::Model RM,
                    CodeModel::Model CM,
                    CodeGenOpt::Level OL);

  virtual const MSPUSubtarget *
  getSubtargetImpl() const override {
    return &Subtarget;
  }
  // Pass Pipeline Configuration
  virtual TargetPassConfig *
  createPassConfig(PassManagerBase &PM);

  TargetLoweringObjectFile *getObjFileLowering() const override {
    return TLOF.get();
  }
};

}    // end namespace llvm

#endif
