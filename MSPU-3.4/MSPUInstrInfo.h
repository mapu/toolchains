//===-- MSPUInstrInfo.h - MSPU Instruction Information --------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the MSPU implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef MSPU_INSTR_INFO_H
#define MSPU_INSTR_INFO_H

#include "MSPURegisterInfo.h"
#include "llvm/Target/TargetInstrInfo.h"

#define GET_INSTRINFO_HEADER
#include "MSPUGenInstrInfo.inc"

namespace llvm
{

/// MSPU_II - This namespace holds all of the target specific flags that
/// instruction info tracks.
///
	namespace MSPU_II
	{
		enum
		{
			Pseudo = (1 << 0), Load = (1 << 1), Store = (1 << 2), DelaySlot = (1 << 3)
		};
	}

	class MSPUInstrInfo: public MSPUGenInstrInfo
	{
		private:
			const MSPURegisterInfo RI;

		public:
			explicit
			MSPUInstrInfo(MSPUSubtarget &ST);

			/// getRegisterInfo - TargetInstrInfo is a superset of MRegister info.  As
			/// such, whenever a client has an instance of instruction info, it should
			/// always be able to get register info as well (through this method).
			///
			virtual const MSPURegisterInfo &
			getRegisterInfo() const
			{
				return RI;
			}

			/// isLoadFromStackSlot - If the specified machine instruction is a direct
			/// load from a stack slot, return the virtual or physical register number of
			/// the destination along with the FrameIndex of the loaded stack slot.  If
			/// not, return 0.  This predicate must return 0 if the instruction has
			/// any side effects other than loading from the stack slot.
			virtual unsigned
			isLoadFromStackSlot(const MachineInstr *MI, int &FrameIndex) const;

			/// isStoreToStackSlot - If the specified machine instruction is a direct
			/// store to a stack slot, return the virtual or physical register number of
			/// the source reg along with the FrameIndex of the loaded stack slot.  If
			/// not, return 0.  This predicate must return 0 if the instruction has
			/// any side effects other than storing to the stack slot.
			virtual unsigned
			isStoreToStackSlot(const MachineInstr *MI, int &FrameIndex) const;

			/// emitFrameIndexDebugValue - Emit a target-dependent form of
			/// DBG_VALUE encoding the address of a frame index.
			virtual MachineInstr *
			emitFrameIndexDebugValue(MachineFunction &MF,
										int FrameIx,
										uint64_t Offset,
										const MDNode *MDPtr,
										DebugLoc dl) const;

			virtual bool
			AnalyzeBranch(MachineBasicBlock &MBB,
							MachineBasicBlock *&TBB,
							MachineBasicBlock *&FBB,
							SmallVectorImpl<MachineOperand> &Cond,
							bool AllowModify = false) const;

			virtual unsigned
			RemoveBranch(MachineBasicBlock &MBB) const;

			virtual unsigned
			InsertBranch(MachineBasicBlock &MBB,
							MachineBasicBlock *TBB,
							MachineBasicBlock *FBB,
							const SmallVectorImpl<MachineOperand> &Cond,
							DebugLoc DL) const;

			virtual void
			copyPhysReg(MachineBasicBlock &MBB,
						MachineBasicBlock::iterator I,
						DebugLoc DL,
						unsigned DestReg,
						unsigned SrcReg,
						bool KillSrc) const;

			virtual void
			storeRegToStackSlot(MachineBasicBlock &MBB,
								MachineBasicBlock::iterator MBBI,
								unsigned SrcReg,
								bool isKill,
								int FrameIndex,
								const TargetRegisterClass *RC,
								const TargetRegisterInfo *TRI) const;
			virtual DFAPacketizer*
			CreateTargetScheduleState(const TargetMachine *TM,
										const ScheduleDAG *DAG) const;

			virtual void
			loadRegFromStackSlot(MachineBasicBlock &MBB,
									MachineBasicBlock::iterator MBBI,
									unsigned DestReg,
									int FrameIndex,
									const TargetRegisterClass *RC,
									const TargetRegisterInfo *TRI) const;

			virtual unsigned
			getGlobalBaseReg(MachineFunction *MF) const;

			virtual bool
			isSchedulingBoundary(const MachineInstr *MI,
									const MachineBasicBlock *MBB,
									const MachineFunction &MF) const;
	};

}

#endif // MSPU_INSTR_INFO_H
