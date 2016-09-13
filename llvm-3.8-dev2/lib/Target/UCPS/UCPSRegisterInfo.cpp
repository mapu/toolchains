//===-- UCPSRegisterInfo.cpp - UCPS Register Information ----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the UCPS implementation of the TargetRegisterInfo class.
//
//===----------------------------------------------------------------------===//

#include "UCPSRegisterInfo.h"
#include "UCPS.h"
#include "UCPSSubtarget.h"
#include "llvm/IR/Type.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Target/TargetInstrInfo.h"
#include "llvm/ADT/BitVector.h"
#include "llvm/ADT/STLExtras.h"

#define GET_REGINFO_TARGET_DESC
#include "UCPSGenRegisterInfo.inc"

using namespace llvm;

UCPSRegisterInfo::UCPSRegisterInfo(UCPSSubtarget &st,
										const TargetInstrInfo &tii)
				: UCPSGenRegisterInfo(UCPSReg::J30), Subtarget(st), TII(tii)
{
}

const MCPhysReg*
UCPSRegisterInfo::getCalleeSavedRegs(const MachineFunction *MF)
										const
{
	return UCPS_CSRs_SaveList;
}

const uint32_t*
UCPSRegisterInfo::getCallPreservedMask(const MachineFunction &MF,
                                       CallingConv::ID CC) const {
  return UCPS_CSRs_RegMask;
}

/*const uint32_t*
UCPSRegisterInfo::getNoPreservedMask() const {
  // The naming here is inverted: The CSR_NoRegs_Altivec has the
  // Altivec registers masked so that they're not saved and restored around
  // instructions with this preserved mask.

  if (!Subtarget.hasAltivec())
    return CSR_NoRegs_Altivec_RegMask;

  if (Subtarget.isDarwin())
    return CSR_NoRegs_Darwin_RegMask;
  return CSR_NoRegs_RegMask;
}*/

BitVector
UCPSRegisterInfo::getReservedRegs(const MachineFunction &MF) const
{
	BitVector Reserved(getNumRegs());

	Reserved.set(UCPSReg::R31);  // a temporary register

	Reserved.set(UCPSReg::D15);  // a temporary register

	Reserved.set(UCPSReg::J31);  // a temporary register
	Reserved.set(UCPSReg::J30);  // return link register
	Reserved.set(UCPSReg::J29);  // Stack Pointer
	Reserved.set(UCPSReg::J28);  // Frame Pointer

	return Reserved;
}


/// eliminateFrameIndex - This method must be overriden to eliminate abstract
/// target frame indices from instructions which may use them. The instruction
/// referenced by the iterator contains an MO_FrameIndex operand which must be
/// eliminated by this method.  This method may modify or replace the
/// specified instruction, as long as it keeps the iterator pointing at the
/// finished product.  SPAdj is the SP adjustment due to call frame setup
/// instruction.  FIOperandNum is the FI operand number.
//  This callback is performed rather late and just before instruction.
void
UCPSRegisterInfo::eliminateFrameIndex(MachineBasicBlock::iterator MI,
                                      int SPAdj, unsigned FIOperandNum,
                                      RegScavenger *RS) const
{
  assert(SPAdj == 0 && "no need");
  MachineBasicBlock & BB = *MI->getParent();
  MachineFunction *MF = BB.getParent();
  MachineFrameInfo * FrameInfo =  MF->getFrameInfo();
  int opcode, index, offset;
  const int FP = UCPSReg::J28;

  index = MI->getOperand(FIOperandNum).getIndex();
  MachineOperand & IMM = MI->getOperand(FIOperandNum + 1);
  assert(IMM.getType()==MachineOperand::MO_Immediate);
  offset = FrameInfo->getObjectOffset(index) + IMM.getImm();

  // update the offset of a stack object.
  // Attention: since ObjectOffset is used repeatedly, we can not
  // modify it.
  /*FrameInfo->setObjectOffset(index, offset);*/

  // note: target-frame-index is the operand.
  switch(MI->getOpcode())
  {
    // eliminate pseudo instruction StackAddr.
    // (i32 (stackaddr target-frame-index, target-constant))
    case UCPSInst::StackAddr: {
      DebugLoc DL = MI->getDebugLoc();

      //MachineRegisterInfo &RegInfo = MF->getRegInfo();
      MachineOperand & MO0 = MI->getOperand(0); // output
      opcode = UCPSInst::AssignPtr;

      // regalloc is already performed before, so we use physical reg
      const unsigned reg = UCPSReg::J31;
      BuildMI(BB, MI, DL, TII.get(opcode), reg).addImm(offset);

      opcode = UCPSInst::PtrAdd;
      BuildMI(BB, MI, DL, TII.get(opcode)).addOperand(MO0).addReg(FP).addReg(reg);

      MI->eraseFromParent();

      break;
    }

    default: { // common situations for load/store
      MI->getOperand(FIOperandNum).ChangeToRegister(FP, false);
      MI->getOperand(FIOperandNum + 1).ChangeToImmediate(offset);
      break;
    }
  }

  return;
}

void
UCPSRegisterInfo::
processFunctionBeforeFrameFinalized(MachineFunction &MF) const
{
}

const TargetRegisterClass *
UCPSRegisterInfo::getPointerRegClass(const MachineFunction &MF, unsigned Kind) const {
  return &UCPSReg::PtrRegRegClass;
}

unsigned
UCPSRegisterInfo::getFrameRegister(const MachineFunction &MF) const
{
	return UCPSReg::J28;
}

unsigned
UCPSRegisterInfo::getEHExceptionRegister() const
{
	llvm_unreachable("What is the exception register");
}

unsigned
UCPSRegisterInfo::getEHHandlerRegister() const
{
	llvm_unreachable("What is the exception handler register");
}
