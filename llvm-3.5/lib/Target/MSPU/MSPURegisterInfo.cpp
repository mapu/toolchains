//===-- MSPURegisterInfo.cpp - MSPU Register Information ----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the MSPU implementation of the TargetRegisterInfo class.
//
//===----------------------------------------------------------------------===//

#include "MSPURegisterInfo.h"
#include "MSPU.h"
#include "MSPUSubtarget.h"
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
#include "MSPUGenRegisterInfo.inc"

using namespace llvm;

MSPURegisterInfo::MSPURegisterInfo(MSPUSubtarget &st,
										const TargetInstrInfo &tii)
				: MSPUGenRegisterInfo(MSPUReg::J30), Subtarget(st), TII(tii)
{
}

const MCPhysReg*
MSPURegisterInfo::getCalleeSavedRegs(const MachineFunction *MF)
										const
{
	return MSPU_CSRs_SaveList;
}

const uint32_t*
MSPURegisterInfo::getCallPreservedMask(CallingConv::ID CC) const {
  return MSPU_CSRs_RegMask;
}

/*const uint32_t*
MSPURegisterInfo::getNoPreservedMask() const {
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
MSPURegisterInfo::getReservedRegs(const MachineFunction &MF) const
{
	BitVector Reserved(getNumRegs());

	Reserved.set(MSPUReg::R31);  // a temporary register

	Reserved.set(MSPUReg::D15);  // a temporary register

	Reserved.set(MSPUReg::J31);  // a temporary register
	Reserved.set(MSPUReg::J30);  // return link register
	Reserved.set(MSPUReg::J29);  // Stack Pointer
	Reserved.set(MSPUReg::J28);  // Frame Pointer

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
MSPURegisterInfo::eliminateFrameIndex(MachineBasicBlock::iterator MI,
                                      int SPAdj, unsigned FIOperandNum,
                                      RegScavenger *RS) const
{
  assert(SPAdj == 0 && "no need");
  MachineBasicBlock & BB = *MI->getParent();
  MachineFunction *MF = BB.getParent();
  MachineFrameInfo * FrameInfo =  MF->getFrameInfo();
  int opcode, index, offset;
  const int FP = MSPUReg::J28;

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
    case MSPUInst::StackAddr: {
      DebugLoc DL = MI->getDebugLoc();

      //MachineRegisterInfo &RegInfo = MF->getRegInfo();
      MachineOperand & MO0 = MI->getOperand(0); // output
      opcode = MSPUInst::AssignPtr;

      // regalloc is already performed before, so we use physical reg
      const unsigned reg = MSPUReg::J31;
      BuildMI(BB, MI, DL, TII.get(opcode), reg).addImm(offset);

      opcode = MSPUInst::PtrAdd;
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
MSPURegisterInfo::
processFunctionBeforeFrameFinalized(MachineFunction &MF) const
{
}

const TargetRegisterClass *
MSPURegisterInfo::getPointerRegClass(const MachineFunction &MF, unsigned Kind) const {
  return &MSPUReg::PtrRegRegClass;
}

unsigned
MSPURegisterInfo::getFrameRegister(const MachineFunction &MF) const
{
	return MSPUReg::J28;
}

unsigned
MSPURegisterInfo::getEHExceptionRegister() const
{
	llvm_unreachable("What is the exception register");
}

unsigned
MSPURegisterInfo::getEHHandlerRegister() const
{
	llvm_unreachable("What is the exception handler register");
}
