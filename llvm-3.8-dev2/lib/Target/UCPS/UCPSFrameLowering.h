//===-- UCPSFrameLowering.h - Define frame lowering for UCPS --*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//
//
//===----------------------------------------------------------------------===//

#ifndef UCPS_FRAMEINFO_H
#define UCPS_FRAMEINFO_H

#include "UCPS.h"
#include "llvm/Target/TargetFrameLowering.h"

namespace llvm
{
	class UCPSSubtarget;

	class UCPSFrameLowering: public TargetFrameLowering
	{
		public:
			explicit
			UCPSFrameLowering(const UCPSSubtarget &sti)
							: TargetFrameLowering(TargetFrameLowering::StackGrowsDown, 8, 0)
			{
			}

			/// emitProlog/emitEpilog - These methods insert prolog and epilog code into
			/// the function.

			// note that prologue is inserted at the very beginning.
			void emitPrologue(MachineFunction &MF, MachineBasicBlock &MBB) const override;

			// note that prologue is inserted just before the last instruction
			// which is a branch instruction and returns to caller.
			void emitEpilogue(MachineFunction &MF, MachineBasicBlock &MBB) const override;

			/// hasFP - Return true if the specified function should have a dedicated
			/// frame pointer register.
			/// For most targets this is true only if the function
			/// has variable-sized alloca's or if frame pointer elimination is disabled.
			bool hasFP(const MachineFunction &MF) const override
			{
				return true;
			}

			int getFrameIndexOffset(const MachineFunction &MF, int FI) const;

		  void
		  processFunctionBeforeFrameFinalized(MachineFunction &MF,
		                                      RegScavenger *RS = nullptr) const override;

		  MachineBasicBlock::iterator
		  eliminateCallFramePseudoInstr(MachineFunction &MF,
		                                MachineBasicBlock &MBB,
		                                MachineBasicBlock::iterator MI) const override;

	};

}    // End llvm namespace

#endif
