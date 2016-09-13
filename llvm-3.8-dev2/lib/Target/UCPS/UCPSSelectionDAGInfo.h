//===-- UCPSSelectionDAGInfo.h - UCPS SelectionDAG Info -------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the UCPS subclass for TargetSelectionDAGInfo.
//
//===----------------------------------------------------------------------===//

#ifndef UCPSSELECTIONDAGINFO_H
#define UCPSSELECTIONDAGINFO_H

#include "llvm/CodeGen/SelectionDAGTargetInfo.h"

namespace llvm {

class UCPSTargetMachine;

class UCPSSelectionDAGInfo : public SelectionDAGTargetInfo {
};

}

#endif
