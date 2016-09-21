//===-- UCPS.h - Top-level interface for UCPS representation --*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the entry points for global functions defined in the LLVM
// UCPS back-end.
//
//===----------------------------------------------------------------------===//

#ifndef TARGET_UCPS_H
#define TARGET_UCPS_H

#include "MCTargetDesc/UCPSMCTargetDesc.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Target/TargetMachine.h"

#define  MSPUEncodingBytes  41

namespace llvm
{
	class FunctionPass;
	class formatted_raw_ostream;
	class MachineInstr;
	class MCInst;
}  // end namespace llvm;
#endif
