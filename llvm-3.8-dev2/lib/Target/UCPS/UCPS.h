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

#define  MMPUEncodingBytes  41

namespace llvm {
class UCPSAsmPrinter;
class UCPSTargetMachine;

class FunctionPass;
class formatted_raw_ostream;
class MachineInstr;
class MCInst;

FunctionPass *createUCPSISelDag(UCPSTargetMachine &TM);

FunctionPass *createUCPSPacketizer();

FunctionPass *createUCPSISelDag(UCPSTargetMachine &TM);

FunctionPass *createUCPSFixupHwLoop();
}  // end namespace llvm;

namespace llvm
{
	// Enums corresponding to UCPS condition codes, both icc's and fcc's.  These
	// values must be kept in sync with the ones in the .td file.
	namespace UCPS_CC  // UCPS_ Condition Code
	{
		enum CondCodes
		{
			//ICC_A   =  8   ,  // Always
			//ICC_N   =  0   ,  // Never
			ICC_NE = 9,    // Not Equal
			ICC_E = 1,    // Equal
			ICC_G = 10,    // Greater
			ICC_LE = 2,    // Less or Equal
			ICC_GE = 11,    // Greater or Equal
			ICC_L = 3,    // Less

			ICC_GU = 12,    // Greater Unsigned
			ICC_LEU = 4,    // Less or Equal Unsigned
			ICC_CC = 13,    // Carry Clear/Great or Equal Unsigned
			ICC_CS = 5,    // Carry Set/Less Unsigned

			ICC_POS = 14,    // Positive
			ICC_NEG = 6,    // Negative
			ICC_VC = 15,    // Overflow Clear
			ICC_VS = 7,    // Overflow Set

			//FCC_A   =  8+16,  // Always
			//FCC_N   =  0+16,  // Never
			FCC_U = 7 + 16,    // Unordered: one of an operand is NaN, thus can not be ordered
			FCC_G = 6 + 16,    // Greater
			FCC_UG = 5 + 16,    // Unordered or Greater
			FCC_L = 4 + 16,    // Less
			FCC_UL = 3 + 16,    // Unordered or Less
			FCC_LG = 2 + 16,    // Less or Greater
			FCC_NE = 1 + 16,    // Not Equal: Less or Greater or Unordered
			FCC_E = 9 + 16,    // Equal
			FCC_UE = 10 + 16,    // Unordered or Equal
			FCC_GE = 11 + 16,    // Greater or Equal
			FCC_UGE = 12 + 16,    // Unordered or Greater or Equal
			FCC_LE = 13 + 16,    // Less or Equal
			FCC_ULE = 14 + 16,    // Unordered or Less or Equal
			FCC_O = 15 + 16
		// ordered, i.e., less, greater, or equal
		// Ordered
		};
	}

	// this function prints a suffix for conditional branch.
	inline static const char *
	UCPSCondCodeToString(UCPS_CC::CondCodes CC)
	{
		switch(CC) {
			case UCPS_CC::ICC_NE:
				return "ne";
			case UCPS_CC::ICC_E:
				return "e";
			case UCPS_CC::ICC_G:
				return "g";
			case UCPS_CC::ICC_LE:
				return "le";
			case UCPS_CC::ICC_GE:
				return "ge";
			case UCPS_CC::ICC_L:
				return "l";
			case UCPS_CC::ICC_GU:
				return "gu";
			case UCPS_CC::ICC_LEU:
				return "leu";
			case UCPS_CC::ICC_CC:
				return "cc";
			case UCPS_CC::ICC_CS:
				return "cs";
			case UCPS_CC::ICC_POS:
				return "pos";
			case UCPS_CC::ICC_NEG:
				return "neg";
			case UCPS_CC::ICC_VC:
				return "vc";
			case UCPS_CC::ICC_VS:
				return "vs";
			case UCPS_CC::FCC_U:
				return "u";
			case UCPS_CC::FCC_G:
				return "g";
			case UCPS_CC::FCC_UG:
				return "ug";
			case UCPS_CC::FCC_L:
				return "l";
			case UCPS_CC::FCC_UL:
				return "ul";
			case UCPS_CC::FCC_LG:
				return "lg";
			case UCPS_CC::FCC_NE:
				return "ne";
			case UCPS_CC::FCC_E:
				return "e";
			case UCPS_CC::FCC_UE:
				return "ue";
			case UCPS_CC::FCC_GE:
				return "ge";
			case UCPS_CC::FCC_UGE:
				return "uge";
			case UCPS_CC::FCC_LE:
				return "le";
			case UCPS_CC::FCC_ULE:
				return "ule";
			case UCPS_CC::FCC_O:
				return "o";
		}
		llvm_unreachable("Invalid cond code");
	}
}    // end namespace llvm
#endif
