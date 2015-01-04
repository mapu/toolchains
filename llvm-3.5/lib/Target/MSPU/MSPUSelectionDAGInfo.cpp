//===-- MSPUSelectionDAGInfo.cpp - MSPU SelectionDAG Info ---------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the MSPUSelectionDAGInfo class.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "MSPU-selectiondag-info"
#include "MSPUTargetMachine.h"
using namespace llvm;

MSPUSelectionDAGInfo::MSPUSelectionDAGInfo(const DataLayout &DL)
  : TargetSelectionDAGInfo(&DL) {
}

MSPUSelectionDAGInfo::~MSPUSelectionDAGInfo() {
}
