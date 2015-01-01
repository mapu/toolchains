//===- MMPULiteDisassembler.cpp - Disassembler for MMPULite -------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file is part of the MMPULite Disassembler.
//
//===----------------------------------------------------------------------===//

#include "MMPULite.h"
#include "MMPULiteSubtarget.h"
#include "AsmParser/MMPULiteScheduler.h"
#include "llvm/MC/MCDisassembler.h"
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
#include "../MCTargetDesc/MMPULiteInsnLine.h"

using namespace llvm;

#define DEBUG_TYPE "mmpu-disassembler"

#define GET_REGINFO_HEADER
#include "MMPULiteGenRegisterInfo.inc"


typedef MCDisassembler::DecodeStatus DecodeStatus;

namespace {
/// MMPULiteDisassemblerBase - a disasembler class for MMPULite.
class MMPULiteDisassembler: public MCDisassembler {
  MMPULiteScheduler *Scheduler;
  
public:
/// Constructor - Initializes the disassembler.
///
  MMPULiteDisassembler(const MCSubtargetInfo &STI,
                       MCContext &Ctx, bool bigEndian)
    : MCDisassembler(STI, Ctx), Scheduler(new MMPULiteScheduler()) {}

  virtual ~MMPULiteDisassembler() {}

  virtual DecodeStatus getInstruction(MCInst &Instr, uint64_t &Size,
                                      ArrayRef<uint8_t> Bytes, uint64_t Address,
                                      raw_ostream &VStream,
                                      raw_ostream &CStream) const override;
};
} // end anonymous namespace

static DecodeStatus readInsnLine(ArrayRef<uint8_t> Bytes,
                                 uint64_t address, uint64_t &numBytes,
                                 MMPULite::InsnLine& line) {
	numBytes = line.getSumBytes();
	if (Bytes.size() < numBytes) return MCDisassembler::Fail;
	memcpy(line.getCodeBuf(), Bytes.data(), numBytes);
	return MCDisassembler::Success;
}

static bool getReg(const void *D, unsigned RC, unsigned& RegNo) {
	const MMPULiteDisassembler *Dis = static_cast<const MMPULiteDisassembler*>(D);
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
static void tryAddingPcLoadReferenceComment(uint64_t Address, int Value,
                                            const void *Decoder) {
  const MCDisassembler *Dis = static_cast<const MCDisassembler*>(Decoder);
  Dis->tryAddingPcLoadReferenceComment(Value, Address);
}
/*******************************************************************/
static DecodeStatus DecodeMRegRegisterClass(MCInst &Inst,
                                            unsigned RegNo,
                                            uint64_t Address,
                                            const void *Decoder) {
	if (getReg(Decoder, MMPULiteReg::MRegRegClassID, RegNo)) {
	  Inst.addOperand(MCOperand::CreateReg(RegNo));
	  return MCDisassembler::Success;
  }
  else return MCDisassembler::Fail;
}

static DecodeStatus DecodeMRegO(MCInst &Inst, unsigned RegNo,
                                uint64_t Address, const void *Decoder) {
  RegNo = (RegNo >> 2) | ((RegNo & 0x3) << 7);
	if (getReg(Decoder, MMPULiteReg::MRegRegClassID, RegNo)) {
	  Inst.addOperand(MCOperand::CreateReg(RegNo));
	  return MCDisassembler::Success;
  }
  else return MCDisassembler::Fail;
}

static DecodeStatus
DecodeSHUOprtorRegisterClass(MCInst &Inst, unsigned RegNo,
		                         uint64_t Address, const void *Decoder) {
	if (getReg(Decoder, MMPULiteReg::SHUOprtorRegClassID, RegNo)) {
	  Inst.addOperand(MCOperand::CreateReg(RegNo));
	  return MCDisassembler::Success;
  }
  else return MCDisassembler::Fail;
}

static DecodeStatus
DecodeIALUUryOprtorRegisterClass(MCInst &Inst, unsigned RegNo,
		                             uint64_t Address, const void *Decoder) {
	if (getReg(Decoder, MMPULiteReg::IALUUryOprtorRegClassID, RegNo)) {
	  Inst.addOperand(MCOperand::CreateReg(RegNo));
	  return MCDisassembler::Success;
  }
  else return MCDisassembler::Fail;
}

static DecodeStatus
DecodeIALUDPOprtorRegisterClass(MCInst &Inst, unsigned RegNo,
                                uint64_t Address, const void *Decoder) {
  if (getReg(Decoder, MMPULiteReg::IALUDPOprtorRegClassID, RegNo)) {
    Inst.addOperand(MCOperand::CreateReg(RegNo));
    return MCDisassembler::Success;
  } else {
    Inst.setOpcode(Inst.getOpcode() +
                   (MMPULite::IALUUryToM - MMPULite::IALUDPToM));
    return DecodeIALUUryOprtorRegisterClass(Inst, RegNo, Address, Decoder);
  }
}

static DecodeStatus
DecodeIALUBinOprtorRegisterClass(MCInst &Inst, unsigned RegNo,
                                 uint64_t Address, const void *Decoder) {
  if (getReg(Decoder, MMPULiteReg::IALUBinOprtorRegClassID, RegNo)) {
    Inst.addOperand(MCOperand::CreateReg(RegNo));
    return MCDisassembler::Success;
  } else {
    Inst.setOpcode(Inst.getOpcode() +
                   (MMPULite::IALUDPToM - MMPULite::IALUBinToM));
    return DecodeIALUDPOprtorRegisterClass(Inst, RegNo, Address, Decoder);
  }
}

static DecodeStatus
DecodeIALUTPOprtorRegisterClass(MCInst &Inst, unsigned RegNo,
                                uint64_t Address, const void *Decoder) {
  if (getReg(Decoder, MMPULiteReg::IALUTPOprtorRegClassID, RegNo)) {
    Inst.addOperand(MCOperand::CreateReg(RegNo));
    return MCDisassembler::Success;
  } else {
    Inst.setOpcode(Inst.getOpcode() -
                   (MMPULite::IALUTPToM - MMPULite::IALUBinToM));
    return DecodeIALUBinOprtorRegisterClass(Inst, RegNo, Address, Decoder);
  }
}

static DecodeStatus
DecodeFALUUryOprtorRegisterClass(MCInst &Inst, unsigned RegNo,
                                 uint64_t Address, const void *Decoder) {
  if (getReg(Decoder, MMPULiteReg::FALUUryOprtorRegClassID, RegNo)) {
    Inst.addOperand(MCOperand::CreateReg(RegNo));
    return MCDisassembler::Success;
  }
  else return MCDisassembler::Fail;
}

static DecodeStatus
DecodeFALUDPOprtorRegisterClass(MCInst &Inst, unsigned RegNo,
                                uint64_t Address, const void *Decoder) {
  if (getReg(Decoder, MMPULiteReg::FALUDPOprtorRegClassID, RegNo)) {
    Inst.addOperand(MCOperand::CreateReg(RegNo));
    return MCDisassembler::Success;
  } else {
    Inst.setOpcode(Inst.getOpcode() +
                   (MMPULite::FALUUryToM - MMPULite::FALUDParaToM));
    return DecodeFALUUryOprtorRegisterClass(Inst, RegNo, Address, Decoder);
  }
}

static DecodeStatus
DecodeFALUBinOprtorRegisterClass(MCInst &Inst, unsigned RegNo,
		                             uint64_t Address, const void *Decoder) {
	if (getReg(Decoder, MMPULiteReg::FALUBinOprtorRegClassID, RegNo)) {
	  Inst.addOperand(MCOperand::CreateReg(RegNo));
	  return MCDisassembler::Success;
  } else {
    Inst.setOpcode(Inst.getOpcode() +
                   (MMPULite::FALUDParaToM - MMPULite::FALUBinToM));
    return DecodeFALUDPOprtorRegisterClass(Inst, RegNo, Address, Decoder);
  }
}

static bool TFlag = false;

static DecodeStatus
DecodeIMACMulOprtorRegisterClass(MCInst &Inst, unsigned RegNo,
		                             uint64_t Address, const void *Decoder) {
  if (getReg(Decoder, MMPULiteReg::IMACMulOprtorRegClassID, RegNo)) {
	  Inst.addOperand(MCOperand::CreateReg(RegNo));
    // T flag in IMAC mcode is not a defined flag, but can only be extracted from opcode
    // So use a static variable to pass this flag to DecodeFlag function
    if (RegNo == MMPULiteReg::IMULT) TFlag = true;

    // "Tm * Tn" is always disassembled as "Tp +- Tn * Tm", and must be fixed
    // according to the "op", where the real "Tp +- Tn * Tm" uses IMULS, while
    // "Tm * Tn" uses IMUL or IMULT.
    if (RegNo != MMPULiteReg::IMULS)
      Inst.setOpcode(Inst.getOpcode() + (MMPULite::IMulToBIU - MMPULite::IMASToBIU));
	  return MCDisassembler::Success;
  }
  else return MCDisassembler::Fail;
}

static DecodeStatus
DecodeFMACMulOprtorRegisterClass(MCInst &Inst, unsigned RegNo,
		                             uint64_t Address, const void *Decoder) {
	if (getReg(Decoder, MMPULiteReg::FMACMulOprtorRegClassID, RegNo)) {
	  Inst.addOperand(MCOperand::CreateReg(RegNo));
	  return MCDisassembler::Success;
  }
  else return MCDisassembler::Fail;
}

static DecodeStatus
DecodeDIVOprtorRegisterClass(MCInst &Inst, unsigned RegNo,
                             uint64_t Address, const void *Decoder) {
	if (getReg(Decoder, MMPULiteReg::DIVOprtorRegClassID, RegNo)) {
    Inst.addOperand(MCOperand::CreateReg(RegNo));
    return MCDisassembler::Success;
  }
  else return MCDisassembler::Fail;
}

static DecodeStatus
DecodeIALUImmOprtorRegisterClass(MCInst &Inst, unsigned RegNo,
		                             uint64_t Address, const void *Decoder) {
  if (getReg(Decoder, MMPULiteReg::IALUImmOprtorRegClassID, RegNo)) {
	  Inst.addOperand(MCOperand::CreateReg(RegNo));
	  return MCDisassembler::Success;
  }
  else return MCDisassembler::Fail;
}

static DecodeStatus
DecodeConditionRegisterClass(MCInst &Inst, unsigned RegNo,
                             uint64_t Address, const void *Decoder) {
  if (getReg(Decoder, MMPULiteReg::ConditionRegClassID, RegNo)) {
    Inst.addOperand(MCOperand::CreateReg(RegNo));
    // Here fix the "op" operands which were "DisableEncoding"ed in InstInfo.td
    if ((Inst.getOpcode() >= MMPULite::SHU1CombToBIU &&
         Inst.getOpcode() <= MMPULite::SHU1CombToSHU) ||
        (Inst.getOpcode() >= MMPULite::SHU0CombToBIU &&
         Inst.getOpcode() <= MMPULite::SHU0CombToSHU))
      return DecodeSHUOprtorRegisterClass(Inst, 0, Address, Decoder);
    /*if (Inst.getOpcode() == MMPULite::IALUDivS)
      return DecodeDIVOprtorRegisterClass(Inst, 0, Address, Decoder);*/
    return MCDisassembler::Success;
  }
  else return MCDisassembler::Fail;
}

static DecodeStatus
DecodeWFlagRegisterClass(MCInst &Inst, unsigned RegNo,
                         uint64_t Address, const void *Decoder) {
  if (getReg(Decoder, MMPULiteReg::WFlagRegClassID, RegNo)) {
    Inst.addOperand(MCOperand::CreateReg(RegNo));
    return MCDisassembler::Success;
  }
  else return MCDisassembler::Fail;
}

static DecodeStatus
DecodeMACCRegisterClass(MCInst &Inst, unsigned RegNo,
                        uint64_t Address, const void *Decoder) {
	if (getReg(Decoder, MMPULiteReg::MACCRegClassID, RegNo)) {
    Inst.addOperand(MCOperand::CreateReg(RegNo));
    return MCDisassembler::Success;
  }
  else return MCDisassembler::Fail;
}

static DecodeStatus
DecodeTPortRegisterClass(MCInst &Inst, unsigned RegNo,
                         uint64_t Address, const void *Decoder) {
	if (getReg(Decoder, MMPULiteReg::TPortRegClassID, RegNo)) {
    Inst.addOperand(MCOperand::CreateReg(RegNo));
    return MCDisassembler::Success;
  }
  else return MCDisassembler::Fail;
}

static DecodeStatus
DecodeTEPortRegisterClass(MCInst &Inst, unsigned RegNo,
                          uint64_t Address, const void *Decoder) {
  if (getReg(Decoder, MMPULiteReg::TEPortRegClassID, RegNo)) {
    Inst.addOperand(MCOperand::CreateReg(RegNo));
    return MCDisassembler::Success;
  }
  else return MCDisassembler::Fail;
}

/*******************************************************************/
static DecodeStatus
DecodeBIURegisterClass(MCInst &Inst, unsigned RegNo,
		                   uint64_t Address, const void *Decoder) {
	if (getReg(Decoder, MMPULiteReg::BIURegClassID, RegNo)) {
    Inst.addOperand(MCOperand::CreateReg(RegNo));
    return MCDisassembler::Success;
  }
  else return MCDisassembler::Fail;
}

static DecodeStatus DecodeSHUT(MCInst &Inst, unsigned RegNo,
                               uint64_t Address, const void *Decoder) {
  unsigned Reg = RegNo & 0x3;
	if (getReg(Decoder, MMPULiteReg::SHURegClassID, Reg)) {
	  Inst.addOperand(MCOperand::CreateReg(Reg));
    Reg = (RegNo >> 2) & 0x3;
	  if (getReg(Decoder, MMPULiteReg::TPortRegClassID, Reg)) {
	    Inst.addOperand(MCOperand::CreateReg(Reg));
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
      Inst.addOperand(MCOperand::CreateImm(imm));
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
  if (op >= MMPULite::BIU0KG && op <= MMPULite::BIU2St) {
	  Flags |= (Flags & 0x1) << (KPPF + 8);
	  Flags |= (Flags & 0x2) << (APPF + 7);
	  Flags |= (Flags & 0x4) << (BRF + 6);
	  Flags |= (Flags & 0x8) << (MF + 5);
	  Flags |= (Flags & 0x10) << (IF + 4);
  }
  else if (op >= MMPULite::IALUBinToBIU && op <= MMPULite::IALUUryToSHU) {
	  Flags |= (Flags & 0x1) << (HF + 8);
	  Flags |= (Flags & 0x2) << (BF + 7);
	  Flags |= (Flags & 0x4) << (UF + 6);
	  Flags |= (Flags & 0x8) << (TF + 5);
  }
  else if (op >= MMPULite::FALUBinToBIU && op <= MMPULite::FALUUryToSHU) {
	  Flags |= (Flags & 0x1) << (DF + 8);
	  Flags |= (Flags & 0x2) << (SF + 7);
	  Flags |= (Flags & 0x4) << (UF + 6);
	  Flags |= (Flags & 0x8) << (TF + 5);
  }
  else if (op >= MMPULite::FMA && op <= MMPULite::FMulToSHU) {
	  Flags |= (Flags & 0x1) ? 1 << (DF + 8) : 1 << (SF + 8);
    if (op >= MMPULite::FMaC && op <= MMPULite::FMaCToSHU)
      Flags |= (Flags & 0x2) << (CRF + 7);
	  else Flags |= (Flags & 0x2) << (TF + 7);
  }
  else if (op >= MMPULite::IMA && op <= MMPULite::IMulToSHU) {
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
  else if (op >= MMPULite::SHU0CombToBIU && op <= MMPULite::SHU1Stop) {
	  Flags |= (Flags & 0x1) << (BF + 8);
	  Flags |= (Flags & 0x2) << (HF + 7);
	  Flags |= ((Flags & 0x7)==0) << (LF + 8);
    Flags |= (Flags & 0x8) << (TCF + 5);
  }
  else return MCDisassembler::Fail;
	Inst.addOperand(MCOperand::CreateImm(Flags));
	return MCDisassembler::Success;
}
/*******************************************************************/
static DecodeStatus
DecodeLabel(MCInst &Inst, unsigned Label,
            uint64_t Address, const void *Decoder) {
  unsigned imm = Label;
  const MCDisassembler* Dis = static_cast<const MCDisassembler *>(Decoder);
  if (!tryAddingSymbolicOperand(Label * 41, true, Address, 0x25, 2, Inst, Dis))
    Inst.addOperand(MCOperand::CreateImm(imm));
  return MCDisassembler::Success;
}

/********************************************************************************/
#include "MMPULiteGenDisassemblerTables.inc"
/********************************************************************************/

DecodeStatus MMPULiteDisassembler::
getInstruction(MCInst &Instr, uint64_t &Size,
               ArrayRef<uint8_t> Bytes, uint64_t Address,
               raw_ostream &VStream,
               raw_ostream &CStream) const {
  static std::vector<uint64_t> LoopStack;
	uint64_t codes=0, numBytes=0;
	MMPULite::InsnLine line;
	MCInst *formerInst, *curInst;
	Size=0;

	DecodeStatus result = readInsnLine(Bytes, Address, numBytes, line);
  if (!result) return result;
	Size = numBytes;

	formerInst = &Instr;
	for (unsigned i=0; i < NUMSLOTS; i++) {
		line.ExtractCode(codes, i);
		formerInst->addOperand(MCOperand::CreateInst(curInst = new MCInst()));
		// Calling the auto-generated decoder function.
		result = decodeInstruction(DecoderTableMMPULiteDecode48,
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
  if (formerInst->getOpcode() == MMPULite::LPTO) {
    if (formerInst->getOperand(0).isImm() &&
        formerInst->getOperand(0).getImm() != 0)
    LoopStack.push_back(formerInst->getOperand(0).getImm() - 1);
  }

	return result;
}

namespace llvm {
	extern Target TheMMPULiteTarget;
}

static MCDisassembler *createMMPULiteDisassembler(const Target &T,
                                                  const MCSubtargetInfo &STI,
                                                  MCContext &Ctx) {
  return new MMPULiteDisassembler(STI, Ctx, false);
}

extern "C" void LLVMInitializeMMPULiteDisassembler() {
  // Register the disassembler.
  TargetRegistry::RegisterMCDisassembler(TheMMPULiteTarget,
                                         createMMPULiteDisassembler);
}

