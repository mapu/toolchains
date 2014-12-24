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
#include "llvm/PassManager.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/CommandLine.h"

using namespace llvm;

static cl::opt<bool> DisableMSPUMISched("disable-mspu-misched",
      cl::Hidden, cl::ZeroOrMore, cl::init(false),
      cl::desc("Disable MSPU MI Scheduling"));

extern "C" void LLVMInitializeMSPUTarget()
{
	// Register the target.
	RegisterTargetMachine<MSPUTargetMachine> X(TheMSPUTarget);
}

static ScheduleDAGInstrs *createVLIWMachineSchedForMSPU(MachineSchedContext *C) {
  return new MSPUVLIWScheduleMI(C, new MSPUConvergingSched());
}

static MachineSchedRegistry
SchedCustomRegistry("mspu", "Run MSPU's custom scheduler",
                    createVLIWMachineSchedForMSPU);

/// MSPUTargetMachine ctor
MSPUTargetMachine::MSPUTargetMachine(const Target &T,
									StringRef TT,
									StringRef CPU,
									StringRef FS,
									const TargetOptions &Options,
									Reloc::Model RM,
									CodeModel::Model CM,
									CodeGenOpt::Level OL):
					LLVMTargetMachine(T, TT, CPU, FS, Options, RM, CM, OL),
					Subtarget(TT, CPU, FS),
					// Attention: p:?:?:? will change the data type of frame index.
					// isize:abi-align:pref-align
					/*
					 * e: little endian
					 * S: stack alignment
					 * a: aggregate
					 * s: stack object alignment
					 * n: native integer width of target cpu
					 */
					DL("e-S64-p:32:32:32-i8:8:8-i16:16:16-i32:32:32-f32:32:32-f64:64:64-a0:8:8-s0:8:8-n32"),
					InstrInfo(Subtarget),
					TLInfo(*this),
					TSInfo(*this),
					FrameLowering(Subtarget),
					InstrItins(Subtarget.getInstrItineraryData())
{
    initAsmInfo();
}

namespace
{
/// MSPU Code Generator Pass Configuration Options.
	class MSPUPassConfig: public TargetPassConfig
	{
		public:
			MSPUPassConfig(MSPUTargetMachine *TM, PassManagerBase &PM)
							: TargetPassConfig(TM, PM)
			{
		    // Enable MI scheduler.
		    if (DisableMSPUMISched)
		      disablePass(&MachineSchedulerID);
		    else
		      enablePass(&MachineSchedulerID);
			}

			MSPUTargetMachine &
			getMSPUTargetMachine() const
			{
				return getTM<MSPUTargetMachine>();
			}

			virtual bool
			addInstSelector()
			{
			  addPass(createMSPUISelDag(getMSPUTargetMachine()));
			  return false;
			}

			virtual bool
			addPreEmitPass()
			{
			  bool NoOpt = (getOptLevel() == CodeGenOpt::None);

			  // Create Packets.
			  if (!NoOpt) {
			    addPass(createMSPUPacketizer());
			  }

			  return true;
			}

		  virtual ScheduleDAGInstrs *
		  createMachineScheduler(MachineSchedContext *C) const {
		    return createVLIWMachineSchedForMSPU(C);
		  }

	};
} // namespace

TargetPassConfig *
MSPUTargetMachine::createPassConfig(PassManagerBase &PM)
{
	return new MSPUPassConfig(this, PM);
}
