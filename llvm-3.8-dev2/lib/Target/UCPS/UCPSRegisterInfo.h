//===-- UCPSRegisterInfo.h - UCPS Register Information Impl ---*- C++ -*-===//
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

#ifndef UCPSREGISTERINFO_H
#define UCPSREGISTERINFO_H

#include "llvm/Target/TargetRegisterInfo.h"

#define GET_REGINFO_HEADER
#include "UCPSGenRegisterInfo.inc"

namespace llvm {

class UCPSSubtarget;
class TargetInstrInfo;
class Type;

struct UCPSRegisterInfo : public UCPSGenRegisterInfo {
  UCPSSubtarget &Subtarget;
  const TargetInstrInfo &TII;

  UCPSRegisterInfo(UCPSSubtarget &st, const TargetInstrInfo &tii);

  /// Code Generation virtual methods...
  const uint16_t *getCalleeSavedRegs(const MachineFunction *MF = 0) const;
  const uint32_t *getCallPreservedMask(const MachineFunction &MF,
                                       CallingConv::ID CC) const;
  //const uint32_t *getNoPreservedMask() const;

  BitVector getReservedRegs(const MachineFunction &MF) const;

  void eliminateFrameIndex(MachineBasicBlock::iterator MI,
                           int SPAdj, unsigned FIOperandNum,
                           RegScavenger *RS = NULL) const;

  void processFunctionBeforeFrameFinalized(MachineFunction &MF) const;

  // Debug information queries.
  unsigned getFrameRegister(const MachineFunction &MF) const;

  // Exception handling queries.
  unsigned getEHExceptionRegister() const;
  unsigned getEHHandlerRegister() const;

  const TargetRegisterClass *
  getPointerRegClass(const MachineFunction &MF, unsigned Kind=0) const;

};

} // end namespace llvm

#endif
