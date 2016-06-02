//===-- MSPUTargetMachine.cpp - Define TargetMachine for MSPU -----------===//
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

#include "MSPUTargetMachine.h"
#include "MSPU.h"
#include "MSPUMachineScheduler.h"
#include "MSPUTargetObjectFile.h"
#include "llvm/IR/PassManager.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/CommandLine.h"

using namespace llvm;

static cl::opt<bool> DisableMSPUMISched("disable-mspu-misched", cl::Hidden,
                                        cl::ZeroOrMore, cl::init(false),
                                        cl::desc("Disable MSPU MI Scheduling"));

extern "C" void LLVMInitializeMSPUTarget() {
  // Register the target.
  RegisterTargetMachine<MSPUTargetMachine> X(TheMSPUTarget);
}

static ScheduleDAGInstrs *createVLIWMachineSchedForMSPU(MachineSchedContext *C) {
  return new MSPUVLIWScheduleMI(C, make_unique<MSPUConvergingSched>());
}

static MachineSchedRegistry SchedCustomRegistry("mspu",
                                                "Run MSPU's custom scheduler",
                                                createVLIWMachineSchedForMSPU);

/// MSPUTargetMachine ctor
MSPUTargetMachine::MSPUTargetMachine(const Target &T, const Triple &TT,
                                     StringRef CPU,
                                     StringRef FS,
                                     const TargetOptions &Options,
                                     Reloc::Model RM,
                                     CodeModel::Model CM,
                                     CodeGenOpt::Level OL)
  : LLVMTargetMachine(T, "e-S64-p:32:32:32-i8:8:8-i16:16:16-i32:32:32-f32:32:32-f64:64:64-a0:8:8-s0:8:8-n32",
                      TT, CPU, FS, Options, RM, CM, OL),
    TLOF(make_unique<MSPUTargetObjectFile>()),
    Subtarget(TT, CPU, FS, *this) {
  initAsmInfo();
}

namespace {
/// MSPU Code Generator Pass Configuration Options.
class MSPUPassConfig: public TargetPassConfig {
public:
  MSPUPassConfig(MSPUTargetMachine *TM, PassManagerBase &PM)
    : TargetPassConfig(TM, PM) {
    // Enable MI scheduler.
    if (DisableMSPUMISched) disablePass(&MachineSchedulerID);
    else enablePass(&MachineSchedulerID);
  }

  MSPUTargetMachine &
  getMSPUTargetMachine() const {
    return getTM<MSPUTargetMachine>();
  }

  virtual bool addInstSelector() override {
    addPass(createMSPUISelDag(getMSPUTargetMachine()));
    return false;
  }

  virtual void addPreEmitPass() override {
    bool NoOpt = (getOptLevel() == CodeGenOpt::None);

    // Create Packets.
    if (!NoOpt) {
      addPass(createMSPUFixupHwLoop(), false);
      addPass(createMSPUPacketizer());
    }
  }

  virtual ScheduleDAGInstrs *
  createMachineScheduler(MachineSchedContext *C) const override {
    return createVLIWMachineSchedForMSPU(C);
  }

};
}    // namespace

TargetPassConfig *
MSPUTargetMachine::createPassConfig(PassManagerBase &PM) {
  return new MSPUPassConfig(this, PM);
}
