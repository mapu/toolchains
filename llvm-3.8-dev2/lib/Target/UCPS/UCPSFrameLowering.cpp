//===-- UCPSFrameLowering.cpp - UCPS Frame Information ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the UCPS implementation of TargetFrameLowering class.
//
//===----------------------------------------------------------------------===//

#include "UCPSISelLowering.h"
#include "UCPSFrameLowering.h"
#include "UCPSInstrInfo.h"
#include "UCPSMachineFunctionInfo.h"
#include "llvm/IR/Function.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/Support/CommandLine.h"

using namespace llvm;

void
UCPSFrameLowering::emitPrologue(MachineFunction &MF,
                                MachineBasicBlock &MBB) const
{
	MachineFrameInfo *FI = MF.getFrameInfo();
	const UCPSInstrInfo &TII =
	      *static_cast<const UCPSInstrInfo *>(MF.getSubtarget().getInstrInfo());
	MachineBasicBlock::iterator MBBI = MBB.begin();
	DebugLoc DL;

	// Get the number of bytes to allocate from the FrameInfo.
	// StackSize is for frame memory actually.
	// we always use the max memory space for argument passing.
	unsigned adj = FI->getMaxCallFrameSize();
	int NumBytes = (int) FI->getStackSize() + adj;

	// we must align "NumBytes" to 8-byte for the max size of C meta data types
	const int max_sz = 8 - 1;
	NumBytes += max_sz;
	NumBytes &= ~max_sz;

	// we call this function to generate proper debug info of variable locations.
	// see TargetFrameLowering::getFrameIndexReference() for reference.
  FI->setOffsetAdjustment(-NumBytes);

	UCPSMachineFunctionInfo *FnInfo = MF.getInfo<UCPSMachineFunctionInfo>();
  int FPOffset = FnInfo->getFPStackOffset();

  // J29 is uesed as stack pointer, and its name is "SP".
  // save frame pointer onto stack: [sp(J29) + -4]=fp(J28);
  BuildMI(MBB, MBBI, DL, TII.get(UCPSInst::StorePtrJI))
  .addReg(UCPSReg::J28).addReg(UCPSReg::J29).addImm(FPOffset).setMIFlag(MachineInstr::FrameSetup);

  // always store J30 (return address) for debug reason since debugger can not determine frame layout.
  /*if (FI->adjustsStack() || FI->hasCalls())*/{
    int RAOffset = FnInfo->getRAStackOffset();
    BuildMI(MBB, MBBI, DL, TII.get(UCPSInst::StorePtrJI))
    .addReg(UCPSReg::J30).addReg(UCPSReg::J29).addImm(RAOffset).setMIFlag(MachineInstr::FrameSetup);
  }

  BuildMI(MBB, MBBI, DL, TII.get(UCPSInst::PtrRegFromPtrReg), UCPSReg::J28)
  .addReg(UCPSReg::J29).setMIFlag(MachineInstr::FrameSetup);
  BuildMI(MBB, MBBI, DL, TII.get(UCPSInst::AssignPtr), UCPSReg::J31)
  .addImm(-NumBytes).setMIFlag(MachineInstr::FrameSetup);
  BuildMI(MBB, MBBI, DL, TII.get(UCPSInst::PtrAdd), UCPSReg::J29)
  .addReg(UCPSReg::J29).addReg(UCPSReg::J31).setMIFlag(MachineInstr::FrameSetup);

  // Note: this is a trick for adjusting alloca. It should be placed at a proper place.
  // ALLOCA adjust regs.  Iterate over ADJDYNALLOC nodes and change the offset.
  const std::vector<MachineInstr*>& AdjustInsts = FnInfo->getAllocaAdjustInsts();
  for (std::vector<MachineInstr*>::const_iterator i = AdjustInsts.begin(), e = AdjustInsts.end();
       i != e; ++i)
  {
    MachineInstr* MI = *i;
    assert((MI->getOpcode() == UCPSInst::AdjDynAlloc) && "expecte adjust alloca node");
    MachineOperand& MO = MI->getOperand(2);
    assert(MO.isImm() && "expecte immediate");

    BuildMI(MBB, MI, DL, TII.get(UCPSInst::AssignPtr), UCPSReg::J31)
    .addImm(adj).setMIFlag(MachineInstr::FrameSetup);

    BuildMI(MBB, MI, DL, TII.get(UCPSInst::PtrAdd), MI->getOperand(0).getReg())
    .addReg(MI->getOperand(1).getReg()).addReg(UCPSReg::J31).setMIFlag(MachineInstr::FrameSetup);

    MI->eraseFromParent();
  }
}

// since we Adjust Offset in emitPrologue() via setOffsetAdjustment(), there probably is
// a mismatch between Stack-Size and Offset-Adjustment, so we directly use Object-Offset.
// Refer to eliminateFrameIndex().
int UCPSFrameLowering::getFrameIndexOffset(const MachineFunction &MF,
                                         int FI) const {
  const MachineFrameInfo *MFI = MF.getFrameInfo();
  return MFI->getObjectOffset(FI) /*+ MFI->getStackSize() -
    getOffsetOfLocalArea() + MFI->getOffsetAdjustment()*/;
}

void
UCPSFrameLowering::emitEpilogue(MachineFunction &MF,
									MachineBasicBlock &MBB) const
{
	MachineBasicBlock::iterator MBBI = MBB.getLastNonDebugInstr(); // jump J30
  const UCPSInstrInfo &TII =
        *static_cast<const UCPSInstrInfo *>(MF.getSubtarget().getInstrInfo());
  DebugLoc dl = MBBI->getDebugLoc();
	assert(MBBI->getOpcode() == UCPSInst::Return &&
			 "Can only put epilog before linkage instruction!");

	BuildMI(MBB, MBBI, dl, TII.get(UCPSInst::PtrRegFromPtrReg), UCPSReg::J29).addReg(UCPSReg::J28); // sp = fp

	//MachineFrameInfo *FI = MF.getFrameInfo();
	UCPSMachineFunctionInfo *FnInfo     = MF.getInfo<UCPSMachineFunctionInfo>();

	// always store J30 (return address) for debug reason since debugger can not determine frame layout.
  /*if (FI->adjustsStack() || FI->hasCalls())*/ {
    int RAOffset = FnInfo->getRAStackOffset();
    BuildMI(MBB, MBBI, dl, TII.get(UCPSInst::LoadPtrJI), UCPSReg::J30).addReg(UCPSReg::J29).addImm(RAOffset);
  }

	int FPOffset = FnInfo->getFPStackOffset();
	BuildMI(MBB, MBBI, dl, TII.get(UCPSInst::LoadPtrJI), UCPSReg::J28).addReg(UCPSReg::J29).addImm(FPOffset); // fp = [sp+ -4]
}

// Note: we use max memory space for arg-passing just like Hexagon.
MachineBasicBlock::iterator UCPSFrameLowering::
eliminateCallFramePseudoInstr(MachineFunction &MF,
                              MachineBasicBlock &MBB,
                              MachineBasicBlock::iterator MI) const {

//  const UCPSInstrInfo &TII =
//        *static_cast<const UCPSInstrInfo *>(MF.getSubtarget().getInstrInfo());
  DebugLoc dl = MI->getDebugLoc();

  switch(MI->getOpcode()) {
    case UCPSInst::ADJCALLSTACKDOWN: {
      /*MachineOperand MO = MI->getOperand(0);
      assert(MO.isImm() && "expect an immediate");
      int64_t imm = MO.getImm();
      if(imm != 0) {
        BuildMI(MBB, MI, dl, TII.get(UCPSInst::AssignPtr), UCPSReg::J31).addImm(-imm);
        BuildMI(MBB, MI, dl, TII.get(UCPSInst::PtrAdd), UCPSReg::J29).addReg(UCPSReg::J29).addReg(
              UCPSReg::J31);
      }*/

      return MBB.erase(MI);
    }

    case UCPSInst::ADJCALLSTACKUP: {
      /*MachineOperand MO = MI->getOperand(0);
      assert(MO.isImm() && "expect an immediate");
      int64_t imm = MO.getImm();
      if(imm != 0) {
        BuildMI(MBB, MI, dl, TII.get(UCPSInst::AssignPtr), UCPSReg::J31).addImm(imm);
        BuildMI(MBB, MI, dl, TII.get(UCPSInst::PtrAdd), UCPSReg::J29).addReg(UCPSReg::J29).addReg(
              UCPSReg::J31);
      }*/

      return MBB.erase(MI);
    }

    default:
      llvm_unreachable("Unexpected call frame instruction");
  }

}

void UCPSFrameLowering::
processFunctionBeforeFrameFinalized(MachineFunction &MF,
                                    RegScavenger *RS) const
{
  MachineFrameInfo *MFI = MF.getFrameInfo();
  UCPSMachineFunctionInfo *FnInfo = MF.getInfo<UCPSMachineFunctionInfo>();
//  CallingConv::ID CallConv = MF.getFunction()->getCallingConv();

  if (hasFP(MF)) {
    FnInfo->setFPStackOffset(-4);
    MFI->CreateFixedObject(4,-4,true);
  }

  // always store J30 (return address) for debug reason since debugger can not determine frame layout.
  /*if (MFI->adjustsStack() || MFI->hasCalls())*/ {
    FnInfo->setRAStackOffset(-8);
    MFI->CreateFixedObject(4,-8,true);
  }
}
