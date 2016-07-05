//===-- UCPM.h - Top-level interface for UCPM representation --*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the entry points for global functions defined in the LLVM
// UCPM back-end.
//
//===----------------------------------------------------------------------===//

#ifndef TARGET_UCPM_H
#define TARGET_UCPM_H

#include "MCTargetDesc/UCPMMCTargetDesc.h"
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
