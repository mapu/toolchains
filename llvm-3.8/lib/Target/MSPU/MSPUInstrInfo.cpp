//===-- MSPUInstrInfo.cpp - MSPU Instruction Information ----------------===//
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

#include "MSPUInstrInfo.h"
#include "MSPU.h"
#include "MSPUMachineFunctionInfo.h"
#include "MSPUSubtarget.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/CodeGen/DFAPacketizer.h"

#define GET_INSTRINFO_CTOR_DTOR
#include "MSPUGenInstrInfo.inc"

using namespace llvm;

MSPUInstrInfo::MSPUInstrInfo(MSPUSubtarget &ST)
				: MSPUGenInstrInfo(MSPUInst::ADJCALLSTACKDOWN, MSPUInst::ADJCALLSTACKUP),
					RI(ST, *this)
{
}

DFAPacketizer *MSPUInstrInfo::
CreateTargetScheduleState(const TargetSubtargetInfo &STI) const {
  const InstrItineraryData *II = STI.getInstrItineraryData();
  return static_cast<const MSPUSubtarget &>(STI).createDFAPacketizer(II);
}

/// isLoadFromStackSlot - If the specified machine instruction is a direct
/// load from a stack slot, return the virtual or physical register number of
/// the destination along with the FrameIndex of the loaded stack slot
/// (Note: pass arg by reference).
/// If not, return 0.  This predicate must return 0 if the instruction has
/// any side effects other than loading from the stack slot.
unsigned
MSPUInstrInfo::isLoadFromStackSlot(const MachineInstr *MI,
									int &FrameIndex) const
{
  if(MI->getOpcode() == MSPUInst::LoadI32JI ||
     MI->getOpcode() == MSPUInst::LoadPtrJI ||
     MI->getOpcode() == MSPUInst::LoadF32JI ||
     MI->getOpcode() == MSPUInst::LoadF64JI)
  {
    if(MI->getOperand(1).isFI() && MI->getOperand(2).isImm())
    {
      FrameIndex = MI->getOperand(1).getIndex();
      return MI->getOperand(0).getReg();
    }
  }

	return 0;
}

/// isStoreToStackSlot - If the specified machine instruction is a direct
/// store to a stack slot, return the virtual or physical register number of
/// the source reg along with the FrameIndex of the loaded stack slot.  If
/// not, return 0.  This predicate must return 0 if the instruction has
/// any side effects other than storing to the stack slot.
unsigned
MSPUInstrInfo::isStoreToStackSlot(const MachineInstr *MI,
									int &FrameIndex) const
{
	if(MI->getOpcode() == MSPUInst::StoreI32JI ||
	   MI->getOpcode() == MSPUInst::StoreF32JI ||
	   MI->getOpcode() == MSPUInst::StorePtrJI ||
	   MI->getOpcode() == MSPUInst::StoreF64JI)
	{
		if(MI->getOperand(1).isFI() && MI->getOperand(2).isImm())
		{
			FrameIndex = MI->getOperand(1).getIndex();
			return MI->getOperand(0).getReg();
		}
	}

	return 0;
}

/// AnalyzeBranch - Analyze the branching code at the end of MBB, returning
/// true if it cannot be understood (e.g. it's a switch dispatch or isn't
/// implemented for a target).  Upon success, this returns false and returns
/// with the following information in various cases:
///
/// 1. If this block ends with no branches (it just falls through to its succ)
///    just return false, leaving TBB/FBB null.
/// 2. If this block ends with only an unconditional branch, it sets TBB to be
///    the destination block.
/// 3. If this block ends with a conditional branch and it falls through to a
///    successor block, it sets TBB to be the branch destination block and a
///    list of operands that evaluate the condition. These operands can be
///    passed to other TargetInstrInfo methods to create new branches.
/// 4. If this block ends with a conditional branch followed by an
///    unconditional branch, it returns the 'true' destination in TBB, the
///    'false' destination in FBB, and a list of operands that evaluate the
///    condition.  These operands can be passed to other TargetInstrInfo
///    methods to create new branches.
///
/// Note that RemoveBranch and InsertBranch must be implemented to support
/// cases where this method returns success (false, see above).
///
/// If AllowModify is true, then this routine is allowed to modify the basic
/// block (e.g. delete instructions after the unconditional branch).

/* For MSPU, there are several following situations:
 * (1) no branch between 2 sequential basic block, which can be potentially combined.
 *
 * (2) 	jump bb;
 *
 * (3)	if cond, jump bb;
 * 		(then fallthrough)
 *
 * (4)	if cond, jump bb;
 * 		jump other-bb;
 *
 * All these situations compose a combination of conditional branch and absolute branch.
 *
 * every basic block in a program ends with a "Terminator" instruction, which
 * indicates (via branching) which block should be executed after the current
 * block is finished. These terminator instructions produce control flow, not
 * values. See http://llvm.org/docs/LangRef.html#terminators
 */
bool
MSPUInstrInfo::AnalyzeBranch(MachineBasicBlock &MBB,
								MachineBasicBlock *&TBB /*true*/,
								MachineBasicBlock *&FBB /*false*/,
								SmallVectorImpl<MachineOperand> &Cond,
								bool AllowModify) const
{
	/*// The last element of a llvm basic block is a sentinel instruction.
	MachineBasicBlock::iterator I = MBB.end();
	MachineBasicBlock::iterator UncondBranch = MBB.end();
	while(I != MBB.begin()) { // if I == MBB.begin(), MBB contains only one instr.
		--I; // locate the 'real' end of this basic block

		if(I->isDebugValue()) // skip this instruction
			continue;

		// I should be a terminator: call, return, branch, ...
		if(!isUnpredicatedTerminator(I))
			break;

		// Terminator is not a branch
		if(!I->isBranch())
			return true;

		//Handle Unconditional branches
		if(I->getOpcode() == MSPUInst::AbsBrBasicBlock) {
			UncondBranch = I;

			if(!AllowModify) {
				TBB = I->getOperand(0).getMBB();
				continue;
			}

			// Since this is a absolute branch, successor instructions are dead codes.
			// But we need to keep the 'end' instruction as sentinel.
			while(llvm::next(I) != MBB.end())
				llvm::next(I)->eraseFromParent();

			Cond.clear();
			FBB = 0;

			if(MBB.isLayoutSuccessor(I->getOperand(0).getMBB())) {
				TBB = 0;
				I->eraseFromParent();
				I = MBB.end();
				UncondBranch = MBB.end();
				continue;
			}

			TBB = I->getOperand(0).getMBB();
			continue;
		}

		unsigned Opcode = I->getOpcode();
		if(Opcode != MSPUInst::CondBrBasicBlock)
			return true;*/

		// There is chance that we perform following transformation for MSPU,
		// but here we simply ignore this at present.

		/*MachineOperand &mo = I->getOperand(1);
		assert(mo.getType()==MachineOperand::MO_Immediate);
		MSPU_CC::CondCodes BranchCode = (MSPU_CC::CondCodes) mo.getImm();

		if(Cond.empty()) {
			MachineBasicBlock *TargetBB = I->getOperand(0).getMBB();
			if(AllowModify && UncondBranch != MBB.end() &&
							MBB.isLayoutSuccessor(TargetBB)) {

				// Transform following codes
				//
				//    if R1, jump L1;
				//    		 jump L2;
				// 	  ...  ...
				// L1:
				//    ..
				// L2: (fall through)
				//
				// into
				//
				//   if !R1, jump L1;
				// L1: (fall through)
				//   ...
				// L2: (fall through)
				//
				BranchCode = GetOppositeBranchCondition(BranchCode);
				MachineBasicBlock::iterator OldInst = I;
				BuildMI(MBB, UncondBranch, MBB.findDebugLoc(I), get(Opcode))
								.addMBB(UncondBranch->getOperand(0).getMBB()).addImm(BranchCode);
				BuildMI(MBB, UncondBranch, MBB.findDebugLoc(I), get(MSPUInst::AbsBrBasicBlock))
								.addMBB(TargetBB);

				OldInst->eraseFromParent();
				UncondBranch->eraseFromParent();

				UncondBranch = MBB.end();
				I = MBB.end();
				continue;
			}
			FBB = TBB;
			TBB = I->getOperand(0).getMBB();
			Cond.push_back(MachineOperand::CreateImm(BranchCode));
			continue;
		}*/

		//FIXME: Handle subsequent conditional branches
		//For now, we can't handle multiple conditional branches
	//}

	// all the previous "return true"s indicate a not-understood situation.
  // success (false, see above)
	return /*false*/ true;
}

/// InsertBranch - Insert branch code into the end of the specified
/// MachineBasicBlock.  The operands to this method are the same as those
/// returned by AnalyzeBranch.  This is only invoked in cases where
/// AnalyzeBranch returns success. It returns the number of instructions
/// inserted.
///
/// It is also invoked by tail merging to add unconditional branches in
/// cases where AnalyzeBranch doesn't apply because there was no original
/// branch to analyze.  At least this much must be implemented, else tail
/// merging needs to be disabled.
unsigned
MSPUInstrInfo::InsertBranch(MachineBasicBlock &MBB, MachineBasicBlock *TBB,
								MachineBasicBlock *FBB,
								ArrayRef<MachineOperand> Cond,
								DebugLoc DL) const
{
	// TBB is
	/*assert(TBB && "InsertBranch must not be told to insert a fallthrough");

	assert((Cond.size() == 1 || Cond.size() == 0) &&
			"MSPU branch conditions should have one component!");

	if(Cond.empty()) {
		assert(!FBB && "Unconditional branch with multiple successors!");
		BuildMI(&MBB, DL, get(MSPUInst::JumpBasicBlock)).addMBB(TBB);
		return 1;
	}

	//Conditional branch
	assert(Cond[0].getType()==MachineOperand::MO_Immediate && "---");
//	unsigned CC = Cond[0].getImm();

	if(IsIntegerCC(CC))
		BuildMI(&MBB, DL, get(MSPUInst::CondBrBasicBlock)).addMBB(TBB).addImm(CC);
	else
		BuildMI(&MBB, DL, get(MSPUReg::FBCOND)).addMBB(TBB).addImm(CC);
	if(!FBB)
		return 1;

	BuildMI(&MBB, DL, get(MSPUInst::JumpBasicBlock)).addMBB(FBB);*/
	return 0;
}

/// RemoveBranch - Remove the branching code at the end of the specific MBB.
/// This is only invoked in cases where AnalyzeBranch returns success. It
/// returns the number of instructions that were removed.
unsigned
MSPUInstrInfo::RemoveBranch(MachineBasicBlock &MBB) const
{
	//MachineBasicBlock::iterator I = MBB.end();
	unsigned Count = 0;
	/*while(I != MBB.begin()) {
		--I;

		if(I->isDebugValue())
			continue;

		if(I->getOpcode() != MSPUInst::JumpBasicBlock && I->getOpcode() != MSPUInst::JumpBasicBlockCond)
			break;  // Not a branch

		I->eraseFromParent(); // 'Parent' is THE basic block

		// this function remove several sequential branch instructions, from back to
		// front. I guess branching will be rebuilt somewhere else (like InsertBranch).
		I = MBB.end();
		++Count;
	}*/

	return Count;
}

void
MSPUInstrInfo::copyPhysReg(MachineBasicBlock &MBB,
							MachineBasicBlock::iterator I,
							DebugLoc DL,
							unsigned DestReg,
							unsigned SrcReg,
							bool KillSrc) const
{
  unsigned opc;
	if(MSPUReg::I32RegRegClass.contains(DestReg, SrcReg))
	  opc = MSPUInst::I32RegFromI32Reg;

	else if(MSPUReg::F32RegRegClass.contains(DestReg, SrcReg))
	  opc = MSPUInst::F32RegFromF32Reg;

	else if(MSPUReg::F64RegRegClass.contains(DestReg, SrcReg))
	  opc = MSPUInst::F64RegFromF64Reg;

	else if(MSPUReg::PtrRegRegClass.contains(DestReg, SrcReg))
	  opc = MSPUInst::PtrRegFromPtrReg;

	else if(MSPUReg::PtrRegRegClass.contains(DestReg) && MSPUReg::I32RegRegClass.contains(SrcReg))
	  opc = MSPUInst::PtrRegFromI32Reg;

	else if(MSPUReg::I32RegRegClass.contains(DestReg) && MSPUReg::PtrRegRegClass.contains(SrcReg))
	  opc = MSPUInst::I32RegFromPtrReg;

	else
		llvm_unreachable("Impossible reg-to-reg copy");

	BuildMI(MBB, I, DL, get(opc), DestReg).addReg(SrcReg, getKillRegState(KillSrc));
}

void
MSPUInstrInfo::
storeRegToStackSlot(MachineBasicBlock &MBB, MachineBasicBlock::iterator MII,
                    unsigned SrcReg, bool isKill, int FI,
                    const TargetRegisterClass *RC,
                    const TargetRegisterInfo *TRI) const
{
  DebugLoc DL = MBB.findDebugLoc(MII);
  unsigned opc;

  // On the order of operands here: think "[base + frameindex + 0] = SrcReg".
  if(RC == &MSPUReg::I32RegRegClass || RC == &MSPUReg::RRegRegClass)
    opc = MSPUInst::StoreI32JI;

  else if(RC == &MSPUReg::F32RegRegClass)
    opc = MSPUInst::StoreF32JI;

  else if(RC == &MSPUReg::PtrRegRegClass || RC == &MSPUReg::JRegRegClass)
    opc = MSPUInst::StorePtrJI;

  else if(RC == &MSPUReg::F64RegRegClass || RC == &MSPUReg::DRegRegClass)
    opc = MSPUInst::StoreF64JI;

  else
    llvm_unreachable("Can't store this register to stack slot");

  BuildMI(MBB, MII, DL, get(opc)).addReg(SrcReg, getKillRegState(isKill)).addFrameIndex(FI).addImm(0);
}

void
MSPUInstrInfo::
loadRegFromStackSlot(MachineBasicBlock &MBB, MachineBasicBlock::iterator MII,
						unsigned DestReg, int FI, const TargetRegisterClass *RC,
						const TargetRegisterInfo *TRI) const
{
  DebugLoc DL = MBB.findDebugLoc(MII);
  unsigned opc;

  // On the order of operands here: think "[base + frameindex + 0] = SrcReg".
  if(RC == &MSPUReg::I32RegRegClass || RC == &MSPUReg::RRegRegClass)
    opc = MSPUInst::LoadI32JI;

  else if(RC == &MSPUReg::F32RegRegClass)
    opc = MSPUInst::LoadF32JI;

  else if(RC == &MSPUReg::PtrRegRegClass  || RC == &MSPUReg::JRegRegClass)
    opc = MSPUInst::LoadPtrJI;

  else if(RC == &MSPUReg::F64RegRegClass    ||  RC == &MSPUReg::DRegRegClass)
    opc = MSPUInst::LoadF64JI;

  else
    llvm_unreachable("Can't load this register from stack slot");

  BuildMI(MBB, MII, DL, get(opc), DestReg).addFrameIndex(FI).addImm(0);
}

bool
MSPUInstrInfo::isSchedulingBoundary(const MachineInstr *MI,
                                   const MachineBasicBlock *MBB,
                                   const MachineFunction &MF) const {
  // Debug info is never a scheduling boundary. It's necessary to be explicit
  // due to the special treatment of IT instructions below, otherwise a
  // dbg_value followed by an IT will result in the IT instruction being
  // considered a scheduling hazard, which is wrong. It should be the actual
  // instruction preceding the dbg_value instruction(s), just like it is
  // when debug info is not present.
  if (MI->isDebugValue())
    return false;

  // Terminators and labels can't be scheduled around.
  if (MI->getDesc().isTerminator() || MI->isLabel() || MI->isInlineAsm())
    return true;

  return false;
}
