//===-- UCPSInstrInfo.h - UCPS Instruction Information --------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the UCPS implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef UCPS_INSTR_INFO_H
#define UCPS_INSTR_INFO_H

#include "UCPSRegisterInfo.h"
#include "llvm/Target/TargetInstrInfo.h"

#define GET_INSTRINFO_HEADER
#include "UCPSGenInstrInfo.inc"

namespace llvm
{

/// UCPS_II - This namespace holds all of the target specific flags that
/// instruction info tracks.
///
	namespace UCPS_II
	{
		enum
		{
			Pseudo = (1 << 0), Load = (1 << 1), Store = (1 << 2), DelaySlot = (1 << 3)
		};
	}

	class UCPSInstrInfo: public UCPSGenInstrInfo
	{
		private:
			const UCPSRegisterInfo RI;

		public:
			explicit
			UCPSInstrInfo(UCPSSubtarget &ST);

			/// getRegisterInfo - TargetInstrInfo is a superset of MRegister info.  As
			/// such, whenever a client has an instance of instruction info, it should
			/// always be able to get register info as well (through this method).
			///
			const UCPSRegisterInfo &getRegisterInfo() const {
				return RI;
			}

			/// isLoadFromStackSlot - If the specified machine instruction is a direct
			/// load from a stack slot, return the virtual or physical register number of
			/// the destination along with the FrameIndex of the loaded stack slot.  If
			/// not, return 0.  This predicate must return 0 if the instruction has
			/// any side effects other than loading from the stack slot.
			unsigned
			isLoadFromStackSlot(const MachineInstr *MI, int &FrameIndex) const override;

			/// isStoreToStackSlot - If the specified machine instruction is a direct
			/// store to a stack slot, return the virtual or physical register number of
			/// the source reg along with the FrameIndex of the loaded stack slot.  If
			/// not, return 0.  This predicate must return 0 if the instruction has
			/// any side effects other than storing to the stack slot.
			unsigned
			isStoreToStackSlot(const MachineInstr *MI, int &FrameIndex) const override;

			bool
			AnalyzeBranch(MachineBasicBlock &MBB,
							MachineBasicBlock *&TBB,
							MachineBasicBlock *&FBB,
							SmallVectorImpl<MachineOperand> &Cond,
							bool AllowModify = false) const override;

			unsigned
			RemoveBranch(MachineBasicBlock &MBB) const override;

			unsigned
			InsertBranch(MachineBasicBlock &MBB,
							MachineBasicBlock *TBB,
							MachineBasicBlock *FBB,
							ArrayRef<MachineOperand> Cond,
							DebugLoc DL) const override;

			void
			copyPhysReg(MachineBasicBlock &MBB,
						MachineBasicBlock::iterator I,
						DebugLoc DL,
						unsigned DestReg,
						unsigned SrcReg,
						bool KillSrc) const override;

			void
			storeRegToStackSlot(MachineBasicBlock &MBB,
								MachineBasicBlock::iterator MBBI,
								unsigned SrcReg,
								bool isKill,
								int FrameIndex,
								const TargetRegisterClass *RC,
								const TargetRegisterInfo *TRI) const override;
			DFAPacketizer*
			CreateTargetScheduleState(const TargetSubtargetInfo &STI) const override;

			void
			loadRegFromStackSlot(MachineBasicBlock &MBB,
									MachineBasicBlock::iterator MBBI,
									unsigned DestReg,
									int FrameIndex,
									const TargetRegisterClass *RC,
									const TargetRegisterInfo *TRI) const override;

			bool
			isSchedulingBoundary(const MachineInstr *MI,
									const MachineBasicBlock *MBB,
									const MachineFunction &MF) const override;
	};

}

#endif // UCPS_INSTR_INFO_H
