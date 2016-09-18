//===- UCPSDisassembler.cpp - Disassembler for UCPS -------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file is part of the UCPS Disassembler.
//
//===----------------------------------------------------------------------===//

#include "UCPS.h"
#include "UCPSSubtarget.h"
#include "AsmParser/UCPSScheduler.h"
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
#include "../MCTargetDesc/UCPSInsnLine.h"

using namespace llvm;
using namespace llvm::UCPS;

#define DEBUG_TYPE "ucps-disassembler"

#define GET_REGINFO_HEADER
#include "UCPSGenRegisterInfo.inc"


typedef MCDisassembler::DecodeStatus DecodeStatus;

namespace {
/// UCPSDisassemblerBase - a disasembler class for UCPS.
class UCPSDisassembler: public MCDisassembler {
  UCPSScheduler *Scheduler;
  
public:
/// Constructor - Initializes the disassembler.
///
  UCPSDisassembler(const MCSubtargetInfo &STI,
                       MCContext &Ctx, bool bigEndian)
    : MCDisassembler(STI, Ctx), Scheduler(new UCPSScheduler()) {}

  virtual ~UCPSDisassembler() {}

  virtual DecodeStatus getInstruction(MCInst &Instr, uint64_t &Size,
                                      ArrayRef<uint8_t> Bytes, uint64_t Address,
                                      raw_ostream &VStream,
                                      raw_ostream &CStream) const override;
};
} // end anonymous namespace

static DecodeStatus readInsnLine(ArrayRef<uint8_t> Bytes,
                                 uint64_t address, uint64_t &numBytes,
                                 UCPS::InsnLine& line) {
	numBytes = line.getSumBytes();
	if (Bytes.size() < numBytes) return MCDisassembler::Fail;
	memcpy(line.getCodeBuf(), Bytes.data(), numBytes);
	return MCDisassembler::Success;
}

static bool getReg(const void *D, unsigned RC, unsigned& RegNo) {
	const UCPSDisassembler *Dis = static_cast<const UCPSDisassembler*>(D);
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
/*yangl static DecodeStatus DecodeMRegRegisterClass(MCInst &Inst,
                                            unsigned RegNo,
                                            uint64_t Address,
                                            const void *Decoder) {
	if (getReg(Decoder, UCPSReg::MRegRegClassID, RegNo)) {
	  Inst.addOperand(MCOperand::createReg(RegNo));
	  return MCDisassembler::Success;
  }
  else return MCDisassembler::Fail;
}

static DecodeStatus DecodeMRegO(MCInst &Inst, unsigned RegNo,
                                uint64_t Address, const void *Decoder) {
  RegNo = (RegNo >> 2) | ((RegNo & 0x3) << 7);
	if (getReg(Decoder, UCPSReg::MRegRegClassID, RegNo)) {
	  Inst.addOperand(MCOperand::createReg(RegNo));
	  return MCDisassembler::Success;
  }
  else return MCDisassembler::Fail;
}

static DecodeStatus
DecodeSHUOprtorRegisterClass(MCInst &Inst, unsigned RegNo,
		                         uint64_t Address, const void *Decoder) {
	if (getReg(Decoder, UCPSReg::SHUOprtorRegClassID, RegNo)) {
	  Inst.addOperand(MCOperand::createReg(RegNo));
	  return MCDisassembler::Success;
  }
  else return MCDisassembler::Fail;
}

static DecodeStatus
DecodeIALUUryOprtorRegisterClass(MCInst &Inst, unsigned RegNo,
		                             uint64_t Address, const void *Decoder) {
	if (getReg(Decoder, UCPSReg::IALUUryOprtorRegClassID, RegNo)) {
	  Inst.addOperand(MCOperand::createReg(RegNo));
	  return MCDisassembler::Success;
  }
  else return MCDisassembler::Fail;
}

static DecodeStatus
DecodeIALUDPOprtorRegisterClass(MCInst &Inst, unsigned RegNo,
                                uint64_t Address, const void *Decoder) {
  if (getReg(Decoder, UCPSReg::IALUDPOprtorRegClassID, RegNo)) {
    Inst.addOperand(MCOperand::createReg(RegNo));
    return MCDisassembler::Success;
  } else {
    Inst.setOpcode(Inst.getOpcode() +
                   (UCPS::IALUUryToM - UCPS::IALUDPToM));
    return DecodeIALUUryOprtorRegisterClass(Inst, RegNo, Address, Decoder);
  }
}

static DecodeStatus
DecodeIALUBinOprtorRegisterClass(MCInst &Inst, unsigned RegNo,
                                 uint64_t Address, const void *Decoder) {
  if (getReg(Decoder, UCPSReg::IALUBinOprtorRegClassID, RegNo)) {
    Inst.addOperand(MCOperand::createReg(RegNo));
    return MCDisassembler::Success;
  } else {
   Inst.setOpcode(Inst.getOpcode() +
                   (UCPS::IALUDPToM - UCPS::IALUBinToM));
    return DecodeIALUDPOprtorRegisterClass(Inst, RegNo, Address, Decoder);
  }
}

static DecodeStatus
DecodeIALUTPOprtorRegisterClass(MCInst &Inst, unsigned RegNo,
                                uint64_t Address, const void *Decoder) {
  if (getReg(Decoder, UCPSReg::IALUTPOprtorRegClassID, RegNo)) {
    Inst.addOperand(MCOperand::createReg(RegNo));
    return MCDisassembler::Success;
  } else {
    Inst.setOpcode(Inst.getOpcode() -
                   (UCPS::IALUTPToM - UCPS::IALUBinToM));
    return DecodeIALUBinOprtorRegisterClass(Inst, RegNo, Address, Decoder);
  }
}

static DecodeStatus
DecodeFALUUryOprtorRegisterClass(MCInst &Inst, unsigned RegNo,
                                 uint64_t Address, const void *Decoder) {
  if (getReg(Decoder, UCPSReg::FALUUryOprtorRegClassID, RegNo)) {
    Inst.addOperand(MCOperand::createReg(RegNo));
    return MCDisassembler::Success;
  }
  else return MCDisassembler::Fail;
}

static DecodeStatus
DecodeFALUDPOprtorRegisterClass(MCInst &Inst, unsigned RegNo,
                                uint64_t Address, const void *Decoder) {
  if (getReg(Decoder, UCPSReg::FALUDPOprtorRegClassID, RegNo)) {
    Inst.addOperand(MCOperand::createReg(RegNo));
    return MCDisassembler::Success;
  } else {
    Inst.setOpcode(Inst.getOpcode() +
                   (UCPS::FALUUryToM - UCPS::FALUDParaToM));
    return DecodeFALUUryOprtorRegisterClass(Inst, RegNo, Address, Decoder);
  }
}

static DecodeStatus
DecodeFALUBinOprtorRegisterClass(MCInst &Inst, unsigned RegNo,
		                             uint64_t Address, const void *Decoder) {
	if (getReg(Decoder, UCPSReg::FALUBinOprtorRegClassID, RegNo)) {
	  Inst.addOperand(MCOperand::createReg(RegNo));
	  return MCDisassembler::Success;
  } else {
    Inst.setOpcode(Inst.getOpcode() +
                   (UCPS::FALUDParaToM - UCPS::FALUBinToM));
    return DecodeFALUDPOprtorRegisterClass(Inst, RegNo, Address, Decoder);
  }
}

static bool TFlag = false;

static DecodeStatus
DecodeIMACMulOprtorRegisterClass(MCInst &Inst, unsigned RegNo,
		                             uint64_t Address, const void *Decoder) {
  if (getReg(Decoder, UCPSReg::IMACMulOprtorRegClassID, RegNo)) {
	  Inst.addOperand(MCOperand::createReg(RegNo));
    // T flag in IMAC mcode is not a defined flag, but can only be extracted from opcode
    // So use a static variable to pass this flag to DecodeFlag function
    if (RegNo == UCPSReg::IMULT) TFlag = true;

    // "Tm * Tn" is always disassembled as "Tp +- Tn * Tm", and must be fixed
    // according to the "op", where the real "Tp +- Tn * Tm" uses IMULS, while
    // "Tm * Tn" uses IMUL or IMULT.
    if (RegNo != UCPSReg::IMULS)
     Inst.setOpcode(Inst.getOpcode() + (UCPS::IMulToBIU - UCPS::IMASToBIU));
	  return MCDisassembler::Success;
  }
  else return MCDisassembler::Fail;
}

static DecodeStatus
DecodeFMACMulOprtorRegisterClass(MCInst &Inst, unsigned RegNo,
		                             uint64_t Address, const void *Decoder) {
	if (getReg(Decoder, UCPSReg::FMACMulOprtorRegClassID, RegNo)) {
	  Inst.addOperand(MCOperand::createReg(RegNo));
	  return MCDisassembler::Success;
  }
  else return MCDisassembler::Fail;
}

static DecodeStatus
DecodeDIVOprtorRegisterClass(MCInst &Inst, unsigned RegNo,
                             uint64_t Address, const void *Decoder) {
	if (getReg(Decoder, UCPSReg::DIVOprtorRegClassID, RegNo)) {
    Inst.addOperand(MCOperand::createReg(RegNo));
    return MCDisassembler::Success;
  }
  else return MCDisassembler::Fail;
}*/

/*yangl static DecodeStatus
DecodeIALUImmOprtorRegisterClass(MCInst &Inst, unsigned RegNo,
		                             uint64_t Address, const void *Decoder) {
  if (getReg(Decoder, UCPSReg::IALUImmOprtorRegClassID, RegNo)) {
	  Inst.addOperand(MCOperand::createReg(RegNo));
	  return MCDisassembler::Success;
  }
  else return MCDisassembler::Fail;
}

static DecodeStatus
DecodeConditionRegisterClass(MCInst &Inst, unsigned RegNo,
                             uint64_t Address, const void *Decoder) {
  if (getReg(Decoder, UCPSReg::ConditionRegClassID, RegNo)) {
    Inst.addOperand(MCOperand::createReg(RegNo));
    // Here fix the "op" operands which were "DisableEncoding"ed in InstInfo.td
   if ((Inst.getOpcode() >= UCPS::SHU1CombToBIU &&
         Inst.getOpcode() <= UCPS::SHU1CombToSHU) ||
        (Inst.getOpcode() >= UCPS::SHU0CombToBIU &&
         Inst.getOpcode() <= UCPS::SHU0CombToSHU))
      return DecodeSHUOprtorRegisterClass(Inst, 0, Address, Decoder);*/
    /*if (Inst.getOpcode() == UCPS::IALUDivS)
      return DecodeDIVOprtorRegisterClass(Inst, 0, Address, Decoder);*/
  /*yangl  return MCDisassembler::Success;
  }
  else return MCDisassembler::Fail;
}*/

/*yangl static DecodeStatus
DecodeWFlagRegisterClass(MCInst &Inst, unsigned RegNo,
                         uint64_t Address, const void *Decoder) {
  if (getReg(Decoder, UCPSReg::WFlagRegClassID, RegNo)) {
    Inst.addOperand(MCOperand::createReg(RegNo));
    return MCDisassembler::Success;
  }
  else return MCDisassembler::Fail;
}

static DecodeStatus
DecodeMACCRegisterClass(MCInst &Inst, unsigned RegNo,
                        uint64_t Address, const void *Decoder) {
	if (getReg(Decoder, UCPSReg::MACCRegClassID, RegNo)) {
    Inst.addOperand(MCOperand::createReg(RegNo));
    return MCDisassembler::Success;
  }
  else return MCDisassembler::Fail;
}

static DecodeStatus
DecodeTPortRegisterClass(MCInst &Inst, unsigned RegNo,
                         uint64_t Address, const void *Decoder) {
	if (getReg(Decoder, UCPSReg::TPortRegClassID, RegNo)) {
    Inst.addOperand(MCOperand::createReg(RegNo));
    return MCDisassembler::Success;
  }
  else return MCDisassembler::Fail;
}

static DecodeStatus
DecodeTEPortRegisterClass(MCInst &Inst, unsigned RegNo,
                          uint64_t Address, const void *Decoder) {
  if (getReg(Decoder, UCPSReg::TEPortRegClassID, RegNo)) {
    Inst.addOperand(MCOperand::createReg(RegNo));
    return MCDisassembler::Success;
  }
  else return MCDisassembler::Fail;
}*/

/*******************************************************************/
/*yangl static DecodeStatus
DecodeBIURegisterClass(MCInst &Inst, unsigned RegNo,
		                   uint64_t Address, const void *Decoder) {
	if (getReg(Decoder, UCPSReg::BIURegClassID, RegNo)) {
    Inst.addOperand(MCOperand::createReg(RegNo));
    return MCDisassembler::Success;
  }
  else return MCDisassembler::Fail;
}

static DecodeStatus DecodeSHUT(MCInst &Inst, unsigned RegNo,
                               uint64_t Address, const void *Decoder) {
  unsigned Reg = RegNo & 0x3;
	if (getReg(Decoder, UCPSReg::SHURegClassID, Reg)) {
	  Inst.addOperand(MCOperand::createReg(Reg));
    Reg = (RegNo >> 2) & 0x3;
	  if (getReg(Decoder, UCPSReg::TPortRegClassID, Reg)) {
	    Inst.addOperand(MCOperand::createReg(Reg));
	    return MCDisassembler::Success;
    }
  }
  return MCDisassembler::Fail;
}

static DecodeStatus DecodeMACCTI(MCInst &Inst, unsigned RegNo,
                                 uint64_t Address, const void *Decoder) {
	unsigned Reg = RegNo & 0x3;
  if (DecodeMACCRegisterClass(Inst, Reg, Address, Decoder)) {
    Reg = (RegNo >> 2) & 0x3;
    if (DecodeTPortRegisterClass(Inst, Reg, Address, Decoder)) {
      int64_t imm = (RegNo >> 4) & 0x3;
      Inst.addOperand(MCOperand::createImm(imm));
      return MCDisassembler::Success;
    }
  }
  return MCDisassembler::Fail;
}

static DecodeStatus DecodeFlags(MCInst &Inst, unsigned Flags,
                                uint64_t Address, const void *Decoder){
	const unsigned BF=0, HF=1, UF=2, TF=3, SF=4, DF=5, IF=6, LF=7,
                 APPF=8, KPPF=9, CRF=10, BRF=11, MF=12, TCF=13;
	unsigned op = Inst.getOpcode();
  if (op >= UCPS::BIU0KG && op <= UCPS::BIU2St) {
	  Flags |= (Flags & 0x1) << (KPPF + 8);
	  Flags |= (Flags & 0x2) << (APPF + 7);
	  Flags |= (Flags & 0x4) << (BRF + 6);
	  Flags |= (Flags & 0x8) << (MF + 5);
	  Flags |= (Flags & 0x10) << (IF + 4);
  }
  else if (op >= UCPS::IALUBinToBIU && op <= UCPS::IALUUryToSHU) {
	  Flags |= (Flags & 0x1) << (HF + 8);
	  Flags |= (Flags & 0x2) << (BF + 7);
	  Flags |= (Flags & 0x4) << (UF + 6);
	  Flags |= (Flags & 0x8) << (TF + 5);
  }
  else if (op >= UCPS::FALUBinToBIU && op <= UCPS::FALUUryToSHU) {
	  Flags |= (Flags & 0x1) << (DF + 8);
	  Flags |= (Flags & 0x2) << (SF + 7);
	  Flags |= (Flags & 0x4) << (UF + 6);
	  Flags |= (Flags & 0x8) << (TF + 5);
  }
  else if (op >= UCPS::FMA && op <= UCPS::FMulToSHU) {
	  Flags |= (Flags & 0x1) ? 1 << (DF + 8) : 1 << (SF + 8);
    if (op >= UCPS::FMaC && op <= UCPS::FMaCToSHU)
      Flags |= (Flags & 0x2) << (CRF + 7);
	  else Flags |= (Flags & 0x2) << (TF + 7);
  }
  else if (op >= UCPS::IMA && op <= UCPS::IMulToSHU) {
    if (TFlag) {
      Flags |= 1 << (TF + 8);
      TFlag = false;
    }
	  Flags |= (Flags & 0x1) << (HF + 8);
	  Flags |= (Flags & 0x2) << (BF + 7);
	  Flags |= (Flags & 0x4) << (UF + 6);
	  Flags |= (Flags & 0x8) << (IF + 5);
	  Flags |= (Flags & 0x10) << (LF + 4);
	  Flags |= (Flags & 0x20) << (CRF + 3);
  }
  else if (op >= UCPS::SHU0CombToBIU && op <= UCPS::SHU1Stop) {
	  Flags |= (Flags & 0x1) << (BF + 8);
	  Flags |= (Flags & 0x2) << (HF + 7);
	  Flags |= ((Flags & 0x7)==0) << (LF + 8);
    Flags |= (Flags & 0x8) << (TCF + 5);
  }
  else return MCDisassembler::Fail;
	Inst.addOperand(MCOperand::createImm(Flags));
	return MCDisassembler::Success;
}*/
/*******************************************************************/
static DecodeStatus
DecodeLabel(MCInst &Inst, unsigned Label,
            uint64_t Address, const void *Decoder) {
  unsigned imm = Label;
  const MCDisassembler* Dis = static_cast<const MCDisassembler *>(Decoder);
  if (!tryAddingSymbolicOperand(Label * 41, true, Address, 0x25, 2, Inst, Dis))
    Inst.addOperand(MCOperand::createImm(imm));
  return MCDisassembler::Success;
}

/********************************************************************************/
#include "UCPSGenDisassemblerTables.inc"
/********************************************************************************/

DecodeStatus UCPSDisassembler::
getInstruction(MCInst &Instr, uint64_t &Size,
               ArrayRef<uint8_t> Bytes, uint64_t Address,
               raw_ostream &VStream,
               raw_ostream &CStream) const {
  static std::vector<uint64_t> LoopStack;
	uint64_t codes=0, numBytes=0;
	UCPS::InsnLine line;
	MCInst *formerInst, *curInst;
	Size=0;

	DecodeStatus result = readInsnLine(Bytes, Address, numBytes, line);
  /*yangl if (!result) return result;
	Size = numBytes;

	formerInst = &Instr;
	for (unsigned i=0; i < NUMSLOTS; i++) {
		line.ExtractCode(codes, i);
		formerInst->addOperand(MCOperand::createInst(curInst = new MCInst()));
		// Calling the auto-generated decoder function.
		result = decodeInstruction(DecoderTableUCPSDecode48,
                               *curInst, codes, Address, this, STI);
		if(!result) return result;
		formerInst = curInst;
	}
	unsigned Sym = 0;
	while (!LoopStack.empty() && Address/41 == LoopStack.back()) {
    ++Sym;
    LoopStack.pop_back();
  }
	Scheduler->Schedule(Address, &Instr, Sym);
  if (formerInst->getOpcode() == UCPS::LPTO*) {
    if (formerInst->getOperand(0).isImm() &&
        formerInst->getOperand(0).getImm() != 0)
    LoopStack.push_back(formerInst->getOperand(0).getImm() - 1);
  }*/

	return result;
}

namespace llvm {
	extern Target TheUCPSTarget;
}

static MCDisassembler *createUCPSDisassembler(const Target &T,
                                                  const MCSubtargetInfo &STI,
                                                  MCContext &Ctx) {
  return new UCPSDisassembler(STI, Ctx, false);
}

extern "C" void LLVMInitializeUCPSDisassembler() {
  // Register the disassembler.
  TargetRegistry::RegisterMCDisassembler(TheUCPSTarget,
                                         createUCPSDisassembler);
}

