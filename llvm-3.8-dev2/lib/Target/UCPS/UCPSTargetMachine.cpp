//===-- UCPSTargetMachine.cpp - Define TargetMachine for UCPS -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//
//===----------------------------------------------------------------------===//

#include "UCPSTargetMachine.h"
#include "UCPS.h"
#include "UCPSMachineScheduler.h"
#include "UCPSTargetObjectFile.h"
#include "llvm/IR/PassManager.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/CommandLine.h"

using namespace llvm;

static cl::opt<bool> DisableUCPSMISched("disable-ucps-misched", cl::Hidden,
                                        cl::ZeroOrMore, cl::init(false),
                                        cl::desc("Disable UCPS MI Scheduling"));

extern "C" void LLVMInitializeUCPSTarget() {
  // Register the target.
  RegisterTargetMachine<UCPSTargetMachine> X(TheUCPSTarget);
}

static ScheduleDAGInstrs *createVLIWMachineSchedForUCPS(MachineSchedContext *C) {
  return new UCPSVLIWScheduleMI(C, make_unique<UCPSConvergingSched>());
}

static MachineSchedRegistry SchedCustomRegistry("ucps",
                                                "Run UCPS's custom scheduler",
                                                createVLIWMachineSchedForUCPS);

/// UCPSTargetMachine ctor
UCPSTargetMachine::UCPSTargetMachine(const Target &T, const Triple &TT,
                                     StringRef CPU,
                                     StringRef FS,
                                     const TargetOptions &Options,
                                     Reloc::Model RM,
                                     CodeModel::Model CM,
                                     CodeGenOpt::Level OL)
  : LLVMTargetMachine(T, "e-S64-p:32:32:32-i8:8:8-i16:16:16-i32:32:32-f32:32:32-f64:64:64-a0:8:8-s0:8:8-n32",
                      TT, CPU, FS, Options, RM, CM, OL),
    TLOF(make_unique<UCPSTargetObjectFile>()),
    Subtarget(TT, CPU, FS, *this) {
  initAsmInfo();
}

namespace {
/// UCPS Code Generator Pass Configuration Options.
class UCPSPassConfig: public TargetPassConfig {
public:
  UCPSPassConfig(UCPSTargetMachine *TM, PassManagerBase &PM)
    : TargetPassConfig(TM, PM) {
    // Enable MI scheduler.
    if (DisableUCPSMISched) disablePass(&MachineSchedulerID);
    else enablePass(&MachineSchedulerID);
  }

  UCPSTargetMachine &
  getUCPSTargetMachine() const {
    return getTM<UCPSTargetMachine>();
  }

  virtual bool addInstSelector() override {
    addPass(createUCPSISelDag(getUCPSTargetMachine()));
    return false;
  }

  virtual void addPreEmitPass() override {
    bool NoOpt = (getOptLevel() == CodeGenOpt::None);

    // Create Packets.
    if (!NoOpt) {
      addPass(createUCPSFixupHwLoop(), false);
      addPass(createUCPSPacketizer());
    }
  }

  virtual ScheduleDAGInstrs *
  createMachineScheduler(MachineSchedContext *C) const override {
    return createVLIWMachineSchedForUCPS(C);
  }

};
}    // namespace

TargetPassConfig *
UCPSTargetMachine::createPassConfig(PassManagerBase &PM) {
  return new UCPSPassConfig(this, PM);
}
