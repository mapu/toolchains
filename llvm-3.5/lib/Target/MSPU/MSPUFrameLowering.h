//===-- MSPUFrameLowering.h - Define frame lowering for MSPU --*- C++ -*-===//
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

#ifndef MSPU_FRAMEINFO_H
#define MSPU_FRAMEINFO_H

#include "MSPU.h"
#include "llvm/Target/TargetFrameLowering.h"

namespace llvm
{
	class MSPUSubtarget;

	class MSPUFrameLowering: public TargetFrameLowering
	{
		public:
			explicit
			MSPUFrameLowering(const MSPUSubtarget &sti)
							: TargetFrameLowering(TargetFrameLowering::StackGrowsDown, 8, 0)
			{
			}

			/// emitProlog/emitEpilog - These methods insert prolog and epilog code into
			/// the function.

			// note that prologue is inserted at the very beginning.
			virtual void
			emitPrologue(MachineFunction &MF) const;

			// note that prologue is inserted just before the last instruction
			// which is a branch instruction and returns to caller.
			virtual void
			emitEpilogue(MachineFunction &MF, MachineBasicBlock &MBB) const;

			/// hasFP - Return true if the specified function should have a dedicated
			/// frame pointer register.
			/// For most targets this is true only if the function
			/// has variable-sized alloca's or if frame pointer elimination is disabled.
			virtual bool
			hasFP(const MachineFunction &MF) const
			{
				return true;
			}

			int getFrameIndexOffset(const MachineFunction &MF, int FI) const;

		  /// processFunctionBeforeCalleeSavedScan - This method is called immediately
		  /// before PrologEpilogInserter scans the physical registers used to determine
		  /// what callee saved registers should be spilled. This method is optional.
		  virtual void
		  processFunctionBeforeCalleeSavedScan(MachineFunction &MF,
		                                       RegScavenger *RS = NULL) const;

		  virtual void
		  eliminateCallFramePseudoInstr(MachineFunction &MF,
		                                MachineBasicBlock &MBB,
		                                MachineBasicBlock::iterator MI) const;

	};

}    // End llvm namespace

#endif
