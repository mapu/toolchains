//===-- MMPU.h - Top-level interface for MMPU representation --*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the entry points for global functions defined in the LLVM
// MMPU back-end.
//
//===----------------------------------------------------------------------===//

#ifndef TARGET_MMPU_H
#define TARGET_MMPU_H

#include "MCTargetDesc/MMPUMCTargetDesc.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm
{
	class FunctionPass;
	class formatted_raw_ostream;
	class MachineInstr;
	class MCInst;
}  // end namespace llvm;
#endif
