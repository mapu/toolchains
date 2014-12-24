//===-- MSPUSelectionDAGInfo.h - MSPU SelectionDAG Info -------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the MSPU subclass for TargetSelectionDAGInfo.
//
//===----------------------------------------------------------------------===//

#ifndef MSPUSELECTIONDAGINFO_H
#define MSPUSELECTIONDAGINFO_H

#include "llvm/Target/TargetSelectionDAGInfo.h"

namespace llvm {

class MSPUTargetMachine;

class MSPUSelectionDAGInfo : public TargetSelectionDAGInfo {
public:
  explicit MSPUSelectionDAGInfo(const MSPUTargetMachine &TM);
  ~MSPUSelectionDAGInfo();
};

}

#endif
