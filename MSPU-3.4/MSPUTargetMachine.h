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

namespace llvm
{
	class MSPUTargetMachine: public LLVMTargetMachine
	{
			MSPUSubtarget Subtarget;
			const DataLayout DL;       // Calculates type, size & alignment
			MSPUInstrInfo InstrInfo;
			MSPUTargetLowering TLInfo;
			MSPUSelectionDAGInfo TSInfo;
			MSPUFrameLowering FrameLowering;
			const InstrItineraryData& InstrItins;
		public:
			MSPUTargetMachine(const Target &T,
								StringRef TT,
								StringRef CPU,
								StringRef FS,
								const TargetOptions &Options,
								Reloc::Model RM,
								CodeModel::Model CM,
								CodeGenOpt::Level OL);

			virtual const MSPUInstrInfo *
			getInstrInfo() const
			{
				return &InstrInfo;
			}
			virtual const TargetFrameLowering *
			getFrameLowering() const
			{
				return &FrameLowering;
			}
			virtual const MSPUSubtarget *
			getSubtargetImpl() const
			{
				return &Subtarget;
			}
			virtual const MSPURegisterInfo *
			getRegisterInfo() const
			{
				return &(InstrInfo.getRegisterInfo());
			}
			virtual const MSPUTargetLowering*
			getTargetLowering() const
			{
				return &TLInfo;
			}
			virtual const MSPUSelectionDAGInfo*
			getSelectionDAGInfo() const
			{
				return &TSInfo;
			}
			virtual const DataLayout *
			getDataLayout() const
			{
				return &DL;
			}
			virtual const InstrItineraryData*
			getInstrItineraryData() const
			{
				return &InstrItins;
			}
			// Pass Pipeline Configuration
			virtual TargetPassConfig *
			createPassConfig(PassManagerBase &PM);
	};

}  // end namespace llvm

#endif
