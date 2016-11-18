//===- UCPMDisassembler.cpp - Disassembler for UCPM -------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file is part of the UCPM Disassembler.
//
//===----------------------------------------------------------------------===//

#include "UCPM.h"
#include "UCPMSubtarget.h"
#include "AsmParser/UCPMScheduler.h"
#include "llvm/MC/MCDisassembler/MCDisassembler.h"
#include "llvm/MC/MCFixedLenDisassembler.h"
#include "llvm/Support/MemoryObject.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCInst.h"
#include "llvm/Support/MathExtras.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInstrDesc.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/LEB128.h"
#include "llvm/Support/raw_ostream.h"
#include "../MCTargetDesc/UCPMInsnLine.h"

using namespace llvm;
using namespace llvm::UCPM;

#define DEBUG_TYPE "ucpm-disassembler"

#define GET_REGINFO_HEADER
#include "UCPMGenRegisterInfo.inc"


typedef MCDisassembler::DecodeStatus DecodeStatus;

namespace {
/// UCPMDisassemblerBase - a disasembler class for UCPM.
class UCPMDisassembler: public MCDisassembler {
  UCPMScheduler *Scheduler;
  
public:
/// Constructor - Initializes the disassembler.
///
  UCPMDisassembler(const MCSubtargetInfo &STI,
                       MCContext &Ctx, bool bigEndian)
    : MCDisassembler(STI, Ctx), Scheduler(new UCPMScheduler()) {}

  virtual ~UCPMDisassembler() {}

  virtual DecodeStatus getInstruction(MCInst &Instr, uint64_t &Size,
                                      ArrayRef<uint8_t> Bytes, uint64_t Address,
                                      raw_ostream &VStream,
                                      raw_ostream &CStream) const override;
};
} // end anonymous namespace

static DecodeStatus readInsnLine(ArrayRef<uint8_t> Bytes,
                                 uint64_t address, uint64_t &numBytes,
                                 UCPM::InsnLine& line) {
	numBytes = line.getSumBytes();
	if (Bytes.size() < numBytes) return MCDisassembler::Fail;
	memcpy(line.getCodeBuf(), Bytes.data(), numBytes);
	return MCDisassembler::Success;
}

static bool getReg(const void *D, unsigned RC, unsigned& RegNo) {
	const UCPMDisassembler *Dis = static_cast<const UCPMDisassembler*>(D);
  const MCRegisterInfo *RegInfo = Dis->getContext().getRegisterInfo();
	unsigned RegPtr;
	for (unsigned i = 0;
       i < RegInfo->getRegClass(RC).getNumRegs();
       i++) {
		RegPtr = RegInfo->getRegClass(RC).getRegister(i);
		if (RegNo == RegInfo->getEncodingValue(RegPtr)) {
      RegNo = RegPtr;
      return true;
    }
	}
	return false;
}

/// tryAddingSymbolicOperand - trys to add a symbolic operand in place of the
/// immediate Value in the MCInst.  The immediate Value has had any PC
/// adjustment made by the caller.  If the instruction is a branch instruction
/// then isBranch is true, else false.  If the getOpInfo() function was set as
/// part of the setupForSymbolicDisassembly() call then that function is called
/// to get any symbolic information at the Address for this instruction.  If
/// that returns non-zero then the symbolic information it returns is used to
/// create an MCExpr and that is added as an operand to the MCInst.  If
/// getOpInfo() returns zero and isBranch is true then a symbol look up for
/// Value is done and if a symbol is found an MCExpr is created with that, else
/// an MCExpr with Value is created.  This function returns true if it adds an
/// operand to the MCInst and false otherwise.
static bool tryAddingSymbolicOperand(uint64_t Value, bool isBranch,
                                     uint64_t Address, uint64_t Offset,
                                     uint64_t Width, MCInst &MI,
                                     const MCDisassembler *Dis) {
  return Dis->tryAddingSymbolicOperand(MI, Value, Address, isBranch,
                                       Offset, Width);
}

/// tryAddingPcLoadReferenceComment - trys to add a comment as to what is being
/// referenced by a load instruction with the base register that is the Pc.
/// These can often be values in a literal pool near the Address of the
/// instruction.  The Address of the instruction and its immediate Value are
/// used as a possible literal pool entry.  The SymbolLookUp call back will
/// return the name of a symbol referenced by the literal pool's entry if
/// the referenced address is that of a symbol.  Or it will return a pointer to
/// a literal 'C' string if the referenced address of the literal pool's entry
/// is an address into a section with 'C' string literals.
//static void tryAddingPcLoadReferenceComment(uint64_t Address, int Value,
//                                            const void *Decoder) {
//  const MCDisassembler *Dis = static_cast<const MCDisassembler*>(Decoder);
//  Dis->tryAddingPcLoadReferenceComment(Value, Address);
//}
/*******************************************************************/
static DecodeStatus DecodeMRegRegisterClass(MCInst &Inst,
                                            unsigned RegNo,
                                            uint64_t Address,
                                            const void *Decoder) {
	if (getReg(Decoder, UCPMReg::MRegRegClassID, RegNo)) {
	  Inst.addOperand(MCOperand::createReg(RegNo));
	  return MCDisassembler::Success;
  }
  else return MCDisassembler::Fail;
}

static DecodeStatus DecodeMRegO(MCInst &Inst, unsigned RegNo,
                                uint64_t Address, const void *Decoder) {
  //RegNo = RegNo & 0x7F;
	if (getReg(Decoder, UCPMReg::MRegRegClassID, RegNo)) {
	  Inst.addOperand(MCOperand::createReg(RegNo));
	  return MCDisassembler::Success;
  }
  else return MCDisassembler::Fail;
}


static DecodeStatus
DecodeMFetchBinOprtorRegisterClass(MCInst &Inst, unsigned RegNo,
                                uint64_t Address, const void *Decoder) {
  if (getReg(Decoder, UCPMReg::MFetchBinOprtorRegClassID, RegNo)) {
    Inst.addOperand(MCOperand::createReg(RegNo));
    return MCDisassembler::Success;
  } 
  else return MCDisassembler::Fail;
}

// static DecodeStatus
// DecodeWFlagRegisterClass(MCInst &Inst, unsigned RegNo,
//                          uint64_t Address, const void *Decoder) {
//   if (getReg(Decoder, UCPMReg::WFlagRegClassID, RegNo)) {
//     Inst.addOperand(MCOperand::createReg(RegNo));
//     return MCDisassembler::Success;
//   }
//   else return MCDisassembler::Fail;
// }

static DecodeStatus
DecodeIALUComOprtorRegisterClass(MCInst &Inst, unsigned RegNo,
                         uint64_t Address, const void *Decoder) {
  if (getReg(Decoder, UCPMReg::IALUComOprtorRegClassID, RegNo)) {
    Inst.addOperand(MCOperand::createReg(RegNo));
    return MCDisassembler::Success;
  }
  else return MCDisassembler::Fail;
}

static DecodeStatus
DecodeIFALUComOprtorRegisterClass(MCInst &Inst, unsigned RegNo,
                         uint64_t Address, const void *Decoder) {
  if (getReg(Decoder, UCPMReg::IFALUComOprtorRegClassID, RegNo)) {
    Inst.addOperand(MCOperand::createReg(RegNo));
    return MCDisassembler::Success;
  }
  else return MCDisassembler::Fail;
}

static DecodeStatus
DecodeBIUBinOprtorRegisterClass(MCInst &Inst, unsigned RegNo,
                         uint64_t Address, const void *Decoder) {
  if (getReg(Decoder, UCPMReg::BIUBinOprtorRegClassID, RegNo)) {
    Inst.addOperand(MCOperand::createReg(RegNo));
    return MCDisassembler::Success;
  }
  else return MCDisassembler::Fail;
}

static DecodeStatus
DecodeIALUUryRMNOprtorRegisterClass(MCInst &Inst, unsigned RegNo,
                         uint64_t Address, const void *Decoder) {
  if (getReg(Decoder, UCPMReg::IALUUryRMNOprtorRegClassID, RegNo)) {
    Inst.addOperand(MCOperand::createReg(RegNo));
    return MCDisassembler::Success;
  }
  else return MCDisassembler::Fail;
}

static DecodeStatus
DecodeIFALUTranOprtorRegisterClass(MCInst &Inst, unsigned RegNo,
                         uint64_t Address, const void *Decoder) {
  if (getReg(Decoder, UCPMReg::IFALUTranOprtorRegClassID, RegNo)) {
    Inst.addOperand(MCOperand::createReg(RegNo));
    return MCDisassembler::Success;
  }
  else return MCDisassembler::Fail;
}

static DecodeStatus
DecodeIALUUryCONOprtorRegisterClass(MCInst &Inst, unsigned RegNo,
                         uint64_t Address, const void *Decoder) {
  if (getReg(Decoder, UCPMReg::IALUUryCONOprtorRegClassID, RegNo)) {
    Inst.addOperand(MCOperand::createReg(RegNo));
    return MCDisassembler::Success;
  }
  else return MCDisassembler::Fail;
}

static DecodeStatus
DecodeBIUComOprtorRegisterClass(MCInst &Inst, unsigned RegNo,
                         uint64_t Address, const void *Decoder) {
  if (getReg(Decoder, UCPMReg::BIUComOprtorRegClassID, RegNo)) {
    Inst.addOperand(MCOperand::createReg(RegNo));
    return MCDisassembler::Success;
  }
  else return MCDisassembler::Fail;
}

static DecodeStatus
DecodeBIUShiftOprtorRegisterClass(MCInst &Inst, unsigned RegNo,
                                uint64_t Address, const void *Decoder) {
  if (getReg(Decoder, UCPMReg::BIUShiftOprtorRegClassID, RegNo)) {
    Inst.addOperand(MCOperand::createReg(RegNo));
    return MCDisassembler::Success;
  } 
  else return MCDisassembler::Fail;
}

static DecodeStatus
DecodeIALUMNOprtorRegisterClass(MCInst &Inst, unsigned RegNo,
                                uint64_t Address, const void *Decoder) {
  if (getReg(Decoder, UCPMReg::IALUMNOprtorRegClassID, RegNo)) {
    Inst.addOperand(MCOperand::createReg(RegNo));
    return MCDisassembler::Success;
  } 
  else return MCDisassembler::Fail;
}

static DecodeStatus
DecodeIALUSCOprtorRegisterClass(MCInst &Inst, unsigned RegNo,
                                uint64_t Address, const void *Decoder) {
  if (getReg(Decoder, UCPMReg::IALUSCOprtorRegClassID, RegNo)) {
    Inst.addOperand(MCOperand::createReg(RegNo));
    return MCDisassembler::Success;
  } 
  else return MCDisassembler::Fail;
}

static DecodeStatus
DecodeIFALUMNOprtorRegisterClass(MCInst &Inst, unsigned RegNo,
                                uint64_t Address, const void *Decoder) {
  if (getReg(Decoder, UCPMReg::IFALUMNOprtorRegClassID, RegNo)) {
    Inst.addOperand(MCOperand::createReg(RegNo));
    return MCDisassembler::Success;
  } 
  else return MCDisassembler::Fail;
}

static DecodeStatus
DecodeMFetchShiftOprtorRegisterClass(MCInst &Inst, unsigned RegNo,
                                uint64_t Address, const void *Decoder) {
  if (getReg(Decoder, UCPMReg::MFetchShiftOprtorRegClassID, RegNo)) {
    Inst.addOperand(MCOperand::createReg(RegNo));
    return MCDisassembler::Success;
  } 
  else return MCDisassembler::Fail;
}
static DecodeStatus
DecodeSHUTranOprtorRegisterClass(MCInst &Inst, unsigned RegNo,
                                uint64_t Address, const void *Decoder) {
  if (getReg(Decoder, UCPMReg::SHUTranOprtorRegClassID, RegNo)) {
    Inst.addOperand(MCOperand::createReg(RegNo));
    return MCDisassembler::Success;
  } 
  else return MCDisassembler::Fail;
}

static DecodeStatus
DecodeIALUTPOprtorRegisterClass(MCInst &Inst, unsigned RegNo,
                                uint64_t Address, const void *Decoder) {
  if (getReg(Decoder, UCPMReg::IALUTPOprtorRegClassID, RegNo)) {
    Inst.addOperand(MCOperand::createReg(RegNo));
    return MCDisassembler::Success;
  } 
  else return MCDisassembler::Fail;
  /*else {
    Inst.setOpcode(Inst.getOpcode() -
                   (UCPM::IALUTPToM - UCPM::IALUBinToM));
    return DecodeIALUBinOprtorRegisterClass(Inst, RegNo, Address, Decoder);
  }*/
}

static DecodeStatus
DecodeIALUUryNOTOprtorRegisterClass(MCInst &Inst, unsigned RegNo,
                                uint64_t Address, const void *Decoder) {
  if (getReg(Decoder, UCPMReg::IALUUryNOTOprtorRegClassID, RegNo)) {
    Inst.addOperand(MCOperand::createReg(RegNo));
    return MCDisassembler::Success;
  } 
  else return MCDisassembler::Fail;
}

static DecodeStatus
DecodeIALUShiftOprtorRegisterClass(MCInst &Inst, unsigned RegNo,
                                uint64_t Address, const void *Decoder) {
  if (getReg(Decoder, UCPMReg::IALUShiftOprtorRegClassID, RegNo)) {
    Inst.addOperand(MCOperand::createReg(RegNo));
    return MCDisassembler::Success;
  } 
  else return MCDisassembler::Fail;
}

static DecodeStatus
DecodeIALUEXPDOprtorRegisterClass(MCInst &Inst, unsigned RegNo,
                                uint64_t Address, const void *Decoder) {
  if (getReg(Decoder, UCPMReg::IALUEXPDOprtorRegClassID, RegNo)) {
    Inst.addOperand(MCOperand::createReg(RegNo));
    return MCDisassembler::Success;
  } 
  else return MCDisassembler::Fail;
}

static DecodeStatus
DecodeIALUASOprtorRegisterClass(MCInst &Inst, unsigned RegNo,
                                uint64_t Address, const void *Decoder) {
  if (getReg(Decoder, UCPMReg::IALUASOprtorRegClassID, RegNo)) {
    Inst.addOperand(MCOperand::createReg(RegNo));
    return MCDisassembler::Success;
  } 
  else return MCDisassembler::Fail;
}

static DecodeStatus
DecodeIALULogicOprtorRegisterClass(MCInst &Inst, unsigned RegNo,
                                uint64_t Address, const void *Decoder) {
  if (getReg(Decoder, UCPMReg::IALULogicOprtorRegClassID, RegNo)) {
    Inst.addOperand(MCOperand::createReg(RegNo));
    return MCDisassembler::Success;
  } 
  else return MCDisassembler::Fail;
}

/*
static bool TFlag = false;
*/

static DecodeStatus
DecodeIMACMulOprtorRegisterClass(MCInst &Inst, unsigned RegNo,
		                             uint64_t Address, const void *Decoder) {
  if (getReg(Decoder, UCPMReg::IMACMulOprtorRegClassID, RegNo)) {
	  Inst.addOperand(MCOperand::createReg(RegNo));
//     // T flag in IMAC mcode is not a defined flag, but can only be extracted from opcode
//     // So use a static variable to pass this flag to DecodeFlag function
//     if (RegNo == UCPMReg::IMULT) TFlag = true;
// 
//     // "Tm * Tn" is always disassembled as "Tp +- Tn * Tm", and must be fixed
//     // according to the "op", where the real "Tp +- Tn * Tm" uses IMULS, while
//     // "Tm * Tn" uses IMUL or IMULT.
//     if (RegNo != UCPMReg::IMULS)
//      Inst.setOpcode(Inst.getOpcode() + (UCPM::IMulToBIU - UCPM::IMASToBIU));
	  return MCDisassembler::Success;
  }
  else return MCDisassembler::Fail;
}

static DecodeStatus
DecodeIFMACMulOprtorRegisterClass(MCInst &Inst, unsigned RegNo,
		                             uint64_t Address, const void *Decoder) {
  if (getReg(Decoder, UCPMReg::IFMACMulOprtorRegClassID, RegNo)) {
	  Inst.addOperand(MCOperand::createReg(RegNo));
	  return MCDisassembler::Success;
  }
  else return MCDisassembler::Fail;
}

static DecodeStatus
DecodeDIVOprtorRegisterClass(MCInst &Inst, unsigned RegNo,
                             uint64_t Address, const void *Decoder) {
	if (getReg(Decoder, UCPMReg::DIVOprtorRegClassID, RegNo)) {
    Inst.addOperand(MCOperand::createReg(RegNo));
    return MCDisassembler::Success;
  }
  else return MCDisassembler::Fail;
}

static DecodeStatus
DecodeIALUImmOprtorRegisterClass(MCInst &Inst, unsigned RegNo,
		                             uint64_t Address, const void *Decoder) {
  if (getReg(Decoder, UCPMReg::IALUImmOprtorRegClassID, RegNo)) {
	  Inst.addOperand(MCOperand::createReg(RegNo));
	  return MCDisassembler::Success;
  }
  else return MCDisassembler::Fail;
}

static DecodeStatus
DecodeConditionRegisterClass(MCInst &Inst, unsigned RegNo,
                             uint64_t Address, const void *Decoder) {
  if (getReg(Decoder, UCPMReg::ConditionRegClassID, RegNo)) {
    Inst.addOperand(MCOperand::createReg(RegNo));
    // Here fix the "op" operands which were "DisableEncoding"ed in InstInfo.td
//    if ((Inst.getOpcode() >= UCPM::SHU1CombToBIU &&
//          Inst.getOpcode() <= UCPM::SHU1CombToSHU) ||
//         (Inst.getOpcode() >= UCPM::SHU0CombToBIU &&
//          Inst.getOpcode() <= UCPM::SHU0CombToSHU))
//       return DecodeSHUOprtorRegisterClass(Inst, 0, Address, Decoder);
    /*if (Inst.getOpcode() == UCPM::IALUDivS)
      return DecodeDIVOprtorRegisterClass(Inst, 0, Address, Decoder);*/
  return MCDisassembler::Success;
  }
  else return MCDisassembler::Fail;
}



static DecodeStatus
DecodeMACCRegisterClass(MCInst &Inst, unsigned RegNo,
                        uint64_t Address, const void *Decoder) {
	if (getReg(Decoder, UCPMReg::MACCRegClassID, RegNo)) {
    Inst.addOperand(MCOperand::createReg(RegNo));
    return MCDisassembler::Success;
  }
  else return MCDisassembler::Fail;
}

static DecodeStatus
DecodeTPortRegisterClass(MCInst &Inst, unsigned RegNo,
                         uint64_t Address, const void *Decoder) {
	if (getReg(Decoder, UCPMReg::TPortRegClassID, RegNo)) {
    Inst.addOperand(MCOperand::createReg(RegNo));
    return MCDisassembler::Success;
  }
  else return MCDisassembler::Fail;
}

static DecodeStatus
DecodeTEPortRegisterClass(MCInst &Inst, unsigned RegNo,
                          uint64_t Address, const void *Decoder) {
  if (getReg(Decoder, UCPMReg::TEPortRegClassID, RegNo)) {
    Inst.addOperand(MCOperand::createReg(RegNo));
    return MCDisassembler::Success;
  }
  else return MCDisassembler::Fail;
}

/*******************************************************************/
static DecodeStatus DecodeBIUT(MCInst &Inst, unsigned RegNo,
                               uint64_t Address, const void *Decoder) {
  unsigned Reg = RegNo & 0x3;
	if (getReg(Decoder, UCPMReg::TPortRegClassID, Reg)) {
	  Inst.addOperand(MCOperand::createReg(Reg));
    Reg = (RegNo >> 2) & 0x3;
	  if (getReg(Decoder, UCPMReg::BIURegClassID, Reg)) {
	    Inst.addOperand(MCOperand::createReg(Reg));
	    return MCDisassembler::Success;
    }
  }
  return MCDisassembler::Fail;
}


static DecodeStatus DecodeSHUT(MCInst &Inst, unsigned RegNo,
                               uint64_t Address, const void *Decoder) {
  unsigned Reg = RegNo & 0x3;
	if (getReg(Decoder, UCPMReg::TPortRegClassID, Reg)) {
	  Inst.addOperand(MCOperand::createReg(Reg));
    Reg = (RegNo >> 2) & 0x3;
	  if (getReg(Decoder, UCPMReg::SHURegClassID, Reg)) {
	    Inst.addOperand(MCOperand::createReg(Reg));
	    return MCDisassembler::Success;
    }
  }
  return MCDisassembler::Fail;
}

// static DecodeStatus DecodeMACCTI(MCInst &Inst, unsigned RegNo,
//                                  uint64_t Address, const void *Decoder) {
//   unsigned Reg = RegNo & 0x3;
//   if (DecodeTPortRegisterClass(Inst, Reg, Address, Decoder)) {
//     int64_t imm = (RegNo >> 2) & 0x3;
//       Inst.addOperand(MCOperand::createImm(imm));
//       Reg = (RegNo >>4) & 0x3;
//     if (DecodeMACCRegisterClass(Inst, Reg, Address, Decoder)) {
//       return MCDisassembler::Success;
//     }
//   }
//   return MCDisassembler::Fail;
// }

static DecodeStatus DecodeMACCT(MCInst &Inst, unsigned RegNo,
                                 uint64_t Address, const void *Decoder) {

   unsigned Reg = RegNo & 0x3;
	if (getReg(Decoder, UCPMReg::TPortRegClassID, Reg)) {
	  Inst.addOperand(MCOperand::createReg(Reg));
    Reg = (RegNo >> 2) & 0x3;
	  if (getReg(Decoder, UCPMReg::MACCRegClassID, Reg)) {
	    Inst.addOperand(MCOperand::createReg(Reg));
	    return MCDisassembler::Success;
    }
  }
  return MCDisassembler::Fail;
}

static DecodeStatus DecodeMR0DestT(MCInst &Inst, unsigned RegNo,
                                 uint64_t Address, const void *Decoder) {

   unsigned Reg = RegNo & 0x3;
	if (getReg(Decoder, UCPMReg::TPortRegClassID, Reg)) {
	  Inst.addOperand(MCOperand::createReg(Reg));
    Reg = (RegNo >> 2) & 0x3;
	  if (getReg(Decoder, UCPMReg::MR0DestRegClassID, Reg)) {
	    Inst.addOperand(MCOperand::createReg(Reg));
	    return MCDisassembler::Success;
    }
  }
  return MCDisassembler::Fail;
}

static DecodeStatus DecodeMR1DestT(MCInst &Inst, unsigned RegNo,
                                 uint64_t Address, const void *Decoder) {

   unsigned Reg = RegNo & 0x3;
	if (getReg(Decoder, UCPMReg::TPortRegClassID, Reg)) {
	  Inst.addOperand(MCOperand::createReg(Reg));
    Reg = (RegNo >> 2) & 0x3;
	  if (getReg(Decoder, UCPMReg::MR1DestRegClassID, Reg)) {
	    Inst.addOperand(MCOperand::createReg(Reg));
	    return MCDisassembler::Success;
    }
  }
  return MCDisassembler::Fail;
}
static DecodeStatus DecodeMR2DestT(MCInst &Inst, unsigned RegNo,
                                 uint64_t Address, const void *Decoder) {

   unsigned Reg = RegNo & 0x3;
	if (getReg(Decoder, UCPMReg::TPortRegClassID, Reg)) {
	  Inst.addOperand(MCOperand::createReg(Reg));
    Reg = (RegNo >> 2) & 0x3;
	  if (getReg(Decoder, UCPMReg::MR2DestRegClassID, Reg)) {
	    Inst.addOperand(MCOperand::createReg(Reg));
	    return MCDisassembler::Success;
    }
  }
  return MCDisassembler::Fail;
}

static DecodeStatus DecodeMR3DestT(MCInst &Inst, unsigned RegNo,
                                 uint64_t Address, const void *Decoder) {

   unsigned Reg = RegNo & 0x3;
	if (getReg(Decoder, UCPMReg::TPortRegClassID, Reg)) {
	  Inst.addOperand(MCOperand::createReg(Reg));
    Reg = (RegNo >> 2) & 0x3;
	  if (getReg(Decoder, UCPMReg::MR3DestRegClassID, Reg)) {
	    Inst.addOperand(MCOperand::createReg(Reg));
	    return MCDisassembler::Success;
    }
  }
  return MCDisassembler::Fail;
}

static DecodeStatus DecodeMR4DestT(MCInst &Inst, unsigned RegNo,
                                 uint64_t Address, const void *Decoder) {

   unsigned Reg = RegNo & 0x3;
	if (getReg(Decoder, UCPMReg::TPortRegClassID, Reg)) {
	  Inst.addOperand(MCOperand::createReg(Reg));
    Reg = (RegNo >> 2) & 0x3;
	  if (getReg(Decoder, UCPMReg::MR4DestRegClassID, Reg)) {
	    Inst.addOperand(MCOperand::createReg(Reg));
	    return MCDisassembler::Success;
    }
  }
  return MCDisassembler::Fail;
}

static DecodeStatus DecodeMR5DestT(MCInst &Inst, unsigned RegNo,
                                 uint64_t Address, const void *Decoder) {

   unsigned Reg = RegNo & 0x3;
	if (getReg(Decoder, UCPMReg::TPortRegClassID, Reg)) {
	  Inst.addOperand(MCOperand::createReg(Reg));
    Reg = (RegNo >> 2) & 0x3;
	  if (getReg(Decoder, UCPMReg::MR5DestRegClassID, Reg)) {
	    Inst.addOperand(MCOperand::createReg(Reg));
	    return MCDisassembler::Success;
    }
  }
  return MCDisassembler::Fail;
}

static DecodeStatus DecodeBIU0DestTE(MCInst &Inst, unsigned RegNo,
                                 uint64_t Address, const void *Decoder) {

   unsigned Reg = RegNo & 0xf;
	if (getReg(Decoder, UCPMReg::TEPortRegClassID, Reg)) {
	  Inst.addOperand(MCOperand::createReg(Reg));
    Reg = (RegNo >> 4) & 0x3;
	  if (getReg(Decoder, UCPMReg::BIU0DestRegClassID, Reg)) {
	    Inst.addOperand(MCOperand::createReg(Reg));
	    return MCDisassembler::Success;
    }
  }
  return MCDisassembler::Fail;
}

static DecodeStatus DecodeBIU1DestTE(MCInst &Inst, unsigned RegNo,
                                 uint64_t Address, const void *Decoder) {

   unsigned Reg = RegNo & 0xf;
	if (getReg(Decoder, UCPMReg::TEPortRegClassID, Reg)) {
	  Inst.addOperand(MCOperand::createReg(Reg));
    Reg = (RegNo >> 4) & 0x3;
	  if (getReg(Decoder, UCPMReg::BIU1DestRegClassID, Reg)) {
	    Inst.addOperand(MCOperand::createReg(Reg));
	    return MCDisassembler::Success;
    }
  }
  return MCDisassembler::Fail;
}

static DecodeStatus DecodeBIU2DestTE(MCInst &Inst, unsigned RegNo,
                                 uint64_t Address, const void *Decoder) {

   unsigned Reg = RegNo & 0xf;
	if (getReg(Decoder, UCPMReg::TEPortRegClassID, Reg)) {
	  Inst.addOperand(MCOperand::createReg(Reg));
    Reg = (RegNo >> 4) & 0x3;
	  if (getReg(Decoder, UCPMReg::BIU2DestRegClassID, Reg)) {
	    Inst.addOperand(MCOperand::createReg(Reg));
	    return MCDisassembler::Success;
    }
  }
  return MCDisassembler::Fail;
}

static DecodeStatus DecodeSHU0DestTI(MCInst &Inst, unsigned RegNo,
                                 uint64_t Address, const void *Decoder) {

  unsigned Reg = RegNo & 0x3;
  if (DecodeTPortRegisterClass(Inst, Reg, Address, Decoder)) {
    int64_t imm = (RegNo >> 2) & 0x3;
      Inst.addOperand(MCOperand::createImm(imm));
      Reg = (RegNo >>4) & 0x3;
     if (getReg(Decoder, UCPMReg::SHURegClassID, Reg)) {
      Inst.addOperand(MCOperand::createReg(Reg));
      return MCDisassembler::Success;
    }
  }
  return MCDisassembler::Fail;
}

static DecodeStatus DecodeSHU1DestTI(MCInst &Inst, unsigned RegNo,
                                 uint64_t Address, const void *Decoder) {

  unsigned Reg = RegNo & 0x3;
  if (DecodeTPortRegisterClass(Inst, Reg, Address, Decoder)) {
    int64_t imm = (RegNo >> 2) & 0x3;
      Inst.addOperand(MCOperand::createImm(imm));
      Reg = (RegNo >>4) & 0x3;
     if (getReg(Decoder, UCPMReg::SHURegClassID, Reg)) {
      Inst.addOperand(MCOperand::createReg(Reg));
      return MCDisassembler::Success;
    }
  }
  return MCDisassembler::Fail;
}

static DecodeStatus DecodeSHU2DestTI(MCInst &Inst, unsigned RegNo,
                                 uint64_t Address, const void *Decoder) {

  unsigned Reg = RegNo & 0x3;
  if (DecodeTPortRegisterClass(Inst, Reg, Address, Decoder)) {
    int64_t imm = (RegNo >> 2) & 0x3;
      Inst.addOperand(MCOperand::createImm(imm));
      Reg = (RegNo >>4) & 0x3;
     if (getReg(Decoder, UCPMReg::SHURegClassID, Reg)) {
      Inst.addOperand(MCOperand::createReg(Reg));
      return MCDisassembler::Success;
    }
  }
  return MCDisassembler::Fail;
}

static DecodeStatus DecodeFlags(MCInst &Inst, unsigned Flags,
                                uint64_t Address, const void *Decoder){
	const unsigned HF=1, UF=2, TF=3, SF=4, DF=5, IF=6, LF=7, APPF=8, KPPF=9, CRF=10, BRF=11, MF=12, MLF=13, MHF=14, NCF=15, 
               CIF = 16, FF = 17, BF=18, PF = 19, RF = 20, CF = 21, SENDF = 22, S0F = 23, S1F = 24, S2F = 25, S3F = 26, SSF=27,
	       QF=28, QLF=29, QHF=30, dLF=31, dHF=32, dMLF=33, dMHF=34, SPPF=35, IPPF=36, W0F = 37, W1F=38, W2F = 39, W3F = 40,
	       W4F = 41, APPF2 = 42, KMF=43, KGF=44, KMEABLEF=45, KGEABLEF=46, KEF=47, L1F=48, L2F=49, L3F=50, L4F=51, ALLF =52,
	       KI0F= 53, KI1F=54, KI2F=55, KI3F=56, KI4F=57, KI5F =58, KI6F=59, KI7F=60, NF=61, VF=62, AF=63;
	unsigned op = Inst.getOpcode();
  if (op >= UCPM::BIU0Bin && op <= UCPM::BIU2disLdToSHU) {
	  Flags |= (Flags & 0x1) << (BRF-0);
	  Flags |= (Flags & 0x2) << (APPF-1);
	  Flags |= (Flags & 0x4) << (MF-2);
  }
  else if (op >= UCPM::IALUASToBIU && op <= UCPM::IALUWriteFlag) {
	  Flags |= (Flags & 0x1) << (FF-0 );
	  Flags |= (Flags & 0x2) << (CIF-1);
	  Flags |= (Flags & 0x4) << (TF-2);
	  Flags |= (Flags & 0x8) << (SF-3);
	  Flags |= (Flags & 0x10) << (BF-4);
	//  Flags |= (Flags & 0x20) << (UF-5);
  }
//   else if (op >= UCPM::FALUBinToBIU && op <= UCPM::FALUUryToSHU) {
// 	  Flags |= (Flags & 0x1) << (DF + 8);
// 	  Flags |= (Flags & 0x2) << (SF + 7);
// 	  Flags |= (Flags & 0x4) << (UF + 6);
// 	  Flags |= (Flags & 0x8) << (TF + 5);
//   }
//   else if (op >= UCPM::FMA && op <= UCPM::FMulToSHU) {
// 	  Flags |= (Flags & 0x1) ? 1 << (DF + 8) : 1 << (SF + 8);
//     if (op >= UCPM::FMaC && op <= UCPM::FMaCToSHU)
//       Flags |= (Flags & 0x2) << (CRF + 7);
// 	  else Flags |= (Flags & 0x2) << (TF + 7);
//   }
//   else if (op >= UCPM::IMA && op <= UCPM::IMulToSHU) {
//     if (TFlag) {
//       Flags |= 1 << (TF + 8);
//       TFlag = false;
//     }
// 	  Flags |= (Flags & 0x1) << (HF + 8);
// 	  Flags |= (Flags & 0x2) << (BF + 7);
// 	  Flags |= (Flags & 0x4) << (UF + 6);
// 	  Flags |= (Flags & 0x8) << (IF + 5);
// 	  Flags |= (Flags & 0x10) << (LF + 4);
// 	  Flags |= (Flags & 0x20) << (CRF + 3);
//   }
  else if (op >= UCPM::SHU0ByteBitToBIU && op <= UCPM::SHU2Wait) {
	  Flags |= (Flags & 0x1) << (APPF2-0);
	  Flags |= (Flags & 0x2) << (IPPF-1);
	  Flags |= (Flags & 0X4) << (SPPF-2);
  }
  else return MCDisassembler::Fail;
	Inst.addOperand(MCOperand::createImm(Flags));
	return MCDisassembler::Success;
}
/*******************************************************************/
static DecodeStatus
DecodeLabel(MCInst &Inst, unsigned Label,
            uint64_t Address, const void *Decoder) {
  unsigned imm = Label;
  const MCDisassembler* Dis = static_cast<const MCDisassembler *>(Decoder);
  if (!tryAddingSymbolicOperand(Label * 46, true, Address, 0x25, 2, Inst, Dis))
    Inst.addOperand(MCOperand::createImm(imm));
  return MCDisassembler::Success;
}

/********************************************************************************/
#include "UCPMGenDisassemblerTables.inc"
/********************************************************************************/

DecodeStatus UCPMDisassembler::
getInstruction(MCInst &Instr, uint64_t &Size,
               ArrayRef<uint8_t> Bytes, uint64_t Address,
               raw_ostream &VStream,
               raw_ostream &CStream) const {
  static std::vector<uint64_t> LoopStack;
	uint64_t codes=0, numBytes=0;
	UCPM::InsnLine line;
	MCInst *formerInst, *curInst;
	Size=0;

	DecodeStatus result = readInsnLine(Bytes, Address, numBytes, line);
  if (!result) return result;
	Size = numBytes;

	formerInst = &Instr;
	for (unsigned i=0; i < NUMSLOTS; i++) {
		line.ExtractCode(codes, i);
		formerInst->addOperand(MCOperand::createInst(curInst = new MCInst()));
		// Calling the auto-generated decoder function.
		result = decodeInstruction(DecoderTableUCPMDecode48,
                               *curInst, codes, Address, this, STI);
		if(!result) return result;
		formerInst = curInst;
	}
	unsigned Sym = 0;
	while (!LoopStack.empty() && Address/46 == LoopStack.back()) {
    ++Sym;
    LoopStack.pop_back();
  }
	Scheduler->Schedule(Address, &Instr, Sym);
  if (formerInst->getOpcode() == UCPM::LPTO) {
    if (formerInst->getOperand(0).isImm() &&
        formerInst->getOperand(0).getImm() != 0)
    LoopStack.push_back(formerInst->getOperand(0).getImm() - 1);
  }

	return result;
}

namespace llvm {
	extern Target TheUCPMTarget;
}

static MCDisassembler *createUCPMDisassembler(const Target &T,
                                                  const MCSubtargetInfo &STI,
                                                  MCContext &Ctx) {
  return new UCPMDisassembler(STI, Ctx, false);
}

extern "C" void LLVMInitializeUCPMDisassembler() {
  // Register the disassembler.
  TargetRegistry::RegisterMCDisassembler(TheUCPMTarget,
                                         createUCPMDisassembler);
}

