//===-- UCPSTargetMachine.h - Define TargetMachine for UCPS ---*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the UCPS specific subclass of TargetMachine.
//
//===----------------------------------------------------------------------===//

#ifndef UCPSTARGETMACHINE_H
#define UCPSTARGETMACHINE_H

#include "UCPSInstrInfo.h"
#include "UCPSISelLowering.h"
#include "UCPSFrameLowering.h"
#include "UCPSSelectionDAGInfo.h"
#include "UCPSSubtarget.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/Target/TargetFrameLowering.h"

namespace llvm {
class UCPSTargetMachine: public LLVMTargetMachine {
  std::unique_ptr<TargetLoweringObjectFile> TLOF;
  UCPSSubtarget Subtarget;
public:
  UCPSTargetMachine(const Target &T, const Triple &TT,
                    StringRef CPU,
                    StringRef FS,
                    const TargetOptions &Options,
                    Reloc::Model RM,
                    CodeModel::Model CM,
                    CodeGenOpt::Level OL);

  // Pass Pipeline Configuration
  virtual TargetPassConfig *
  createPassConfig(PassManagerBase &PM);

  TargetLoweringObjectFile *getObjFileLowering() const override {
    return TLOF.get();
  }

  const UCPSSubtarget *getSubtargetImpl(const Function &) const override {
    return &Subtarget;
  }
};

}    // end namespace llvm

#endif
