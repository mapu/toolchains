//===- MMPUDisassembler.cpp - Disassembler for MMPU -------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file is part of the MMPU Disassembler.
//
//===----------------------------------------------------------------------===//

#include "../MMPU.h"
#include "../MMPUSubtarget.h"
#include "../MCTargetDesc/MMPUInsnLine.h"
#include "../MCTargetDesc/MMPUMCInst.h"
#include "MMPUDisassembler.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/MC/MCDisassembler.h"
#include "llvm/MC/MCFixedLenDisassembler.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/Object/ObjectFile.h"
#include "llvm/Object/ELF.h"
#include "llvm/Support/MemoryObject.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/MathExtras.h"
#include "llvm/Support/Format.h"
#include "llvm/Support/CommandLine.h"
#include <iostream>

#define GET_REGINFO_HEADER
#include "MMPUGenRegisterInfo.inc"

using namespace llvm;

typedef MCDisassembler::DecodeStatus DecodeStatus;

template<typename InsnType>
static void mmpu_decode_c_option(InsnType codes, int64_t& options )
{
  switch(codes)
  {
      default: break;
      case 2:  options |= MMPU::c_C; break;
      case 3:  options |= MMPU::c_NC; break;
  }
}

/// readInstruction - read four bytes from the MemoryObject
/// and return 32 bit word sorted according to the given endianess
static DecodeStatus readInsnLine(const MemoryObject &region,
		uint64_t address, uint64_t &numBytes, MMPU::InsnLine & line)
{
	numBytes = line.getSumBytes();

	if (region.readBytes(address, numBytes, line.getCodeBuf()) != 0) {
		numBytes = 0;
		return MCDisassembler::Fail;
	}

	return MCDisassembler::Success;
}

static unsigned
getReg(const void *D, unsigned RC, unsigned RegNo)
{
	const MMPUDisassembler *Dis = static_cast<const MMPUDisassembler*>(D);
	return Dis->getRegInfo()->getRegClass(RC).getRegister(RegNo);
}

/// tryAddingSymbolicOperand - trys to add a symbolic operand in place of the
/// immediate Value in the MCInst.
///
/// @param Value      - The immediate Value, has had any PC adjustment made by
///                     the caller.
/// @param isBranch   - If the instruction is a branch instruction
/// @param Address    - The starting address of the instruction
/// @param Offset     - The byte offset to this immediate in the instruction
/// @param Width      - The byte width of this immediate in the instruction
///
/// If the getOpInfo() function was set when setupForSymbolicDisassembly() was
/// called then that function is called to get any symbolic information for the
/// immediate in the instruction using the Address, Offset and Width.
/// If getOpInfo() returns non-zero then the symbolic information it returns is
/// used to create an MCExpr and that is added as an operand to the MCInst.
/// If getOpInfo() returns zero and isBranch is true then a symbol look up for
/// immediate Value is done and if a symbol is found an MCExpr is created with that,
/// else an MCExpr with the immediate Value is created.  This function returns true
/// if it adds an operand to the MCInst and false otherwise.
static bool MMPUTryAddingSymbolicOperand(int64_t Value, bool isBranch,
                                     uint64_t Address, uint64_t Offset,
                                     uint64_t Width, MCInst &MI,
                                     const MCDisassembler *Dis)
{
    return Dis->tryAddingSymbolicOperand(MI, Value, Address, isBranch, Offset, Width);
}

/*******************************************************************/
static DecodeStatus
DecodeMRegRegisterClass(MCInst &Inst, unsigned RegNo,
		uint64_t Address, const void *Decoder)
{
  unsigned Reg;
  if(RegNo<=128)
    Reg = getReg(Decoder, MMPUReg::MRegRegClassID, RegNo);
  else if(RegNo == 1<<8)
    Reg = MMPUReg::MSPP;
  else llvm_unreachable("unvalid M register encoding");

	Inst.addOperand(MCOperand::CreateReg(Reg));
	return MCDisassembler::Success;
}

static DecodeStatus
DecodeShuRegisterClass(MCInst &Inst, unsigned RegNo,
		uint64_t Address, const void *Decoder)
{
	unsigned Reg = getReg(Decoder, MMPUReg::ShuRegClassID, RegNo);
	Inst.addOperand(MCOperand::CreateReg(Reg));
	return MCDisassembler::Success;
}

/*******************************************************************/
static DecodeStatus
DecodeCalcRegisterClass(MCInst &Inst, unsigned RegNo,
		uint64_t Address, const void *Decoder)
{
	unsigned Reg = getReg(Decoder, MMPUReg::CalcRegClassID, RegNo);
	Inst.addOperand(MCOperand::CreateReg(Reg));
	return MCDisassembler::Success;
}

static DecodeStatus
DecodeTPortRegisterClass(MCInst &Inst, unsigned RegNo,
		uint64_t Address, const void *Decoder)
{
	unsigned Reg = getReg(Decoder, MMPUReg::TPortRegClassID, RegNo);
	Inst.addOperand(MCOperand::CreateReg(Reg));
	return MCDisassembler::Success;
}

static DecodeStatus
DecodeIPortRegisterClass(MCInst &Inst, unsigned RegNo,
    uint64_t Address, const void *Decoder)
{
  unsigned Reg = getReg(Decoder, MMPUReg::IPortRegClassID, RegNo-1);
  Inst.addOperand(MCOperand::CreateReg(Reg));
  return MCDisassembler::Success;
}

/*******************************************************************/
static DecodeStatus
DecodeBIURegisterClass(MCInst &Inst, unsigned RegNo,
		uint64_t Address, const void *Decoder)
{
	unsigned Reg = getReg(Decoder, MMPUReg::BIURegClassID, RegNo);
	Inst.addOperand(MCOperand::CreateReg(Reg));
	return MCDisassembler::Success;
}

/*******************************************************************/
template<typename InsnType>
static DecodeStatus
DecodeUnit(MCInst &Inst, InsnType bits, uint64_t Address, const void *Decoder) {
  int64_t  id = bits & 3; // 0b11
  unsigned Reg;
  switch(id) {
    case 1:
      Reg = getReg(Decoder, MMPUReg::ShuRegClassID, bits>>2);
      break;
    case 3:
      Reg = getReg(Decoder, MMPUReg::CalcRegClassID, bits>>2);
      break;
    default:
      llvm_unreachable("invalid register group id for units (shu and calc)");
  }
  Inst.addOperand(MCOperand::CreateReg(Reg));
  return MCDisassembler::Success;
}

template<typename InsnType>
static DecodeStatus
DecodeIALUOptions(MCInst &Inst, InsnType bits, uint64_t Address, const void *Decoder) {
  int64_t  options = 0;

  if(bits & 1<<0)   options |= MMPU::c_H;  // (H)
  if(bits & 1<<1)   options |= MMPU::c_B;  // (B)
  if(bits & 1<<2)   options |= MMPU::c_U;  // (U)
  if(bits & 1<<3)   options |= MMPU::c_T;  // (T)

  mmpu_decode_c_option(bits>>4 & 3, options);

  Inst.addOperand(MCOperand::CreateImm(options));
  return MCDisassembler::Success;
}

// bits 16 - 11
// C !C M BR A++ K++
template<typename InsnType>
static DecodeStatus
DecodeBIUOptions(MCInst &Inst, InsnType bits, uint64_t Address, const void *Decoder) {
  int64_t  options = 0;

  if(bits & 1<<0) options |= MMPU::c_KPP;
  if(bits & 1<<1) options |= MMPU::c_APP;
  if(bits & 1<<2) options |= MMPU::c_BR;
  if(bits & 1<<3) options |= MMPU::c_M;

  mmpu_decode_c_option(bits>>4 & 3, options);

  Inst.addOperand(MCOperand::CreateImm(options));
  return MCDisassembler::Success;
}

template<typename InsnType>
static DecodeStatus
DecodeBIUStOptions(MCInst &Inst, InsnType bits, uint64_t Address, const void *Decoder) {
  int64_t  options = 0;

  if(bits & 1<<0) options |= MMPU::c_IND;

  if(bits & 1<<11) options |= MMPU::c_KPP;
  if(bits & 1<<12) options |= MMPU::c_APP;
  if(bits & 1<<13) options |= MMPU::c_BR;
  if(bits & 1<<14) options |= MMPU::c_M;

  mmpu_decode_c_option(bits>>15 & 3, options);

  Inst.addOperand(MCOperand::CreateImm(options));
  return MCDisassembler::Success;
}

template<typename InsnType>
static DecodeStatus
DecodeCombineOptions(MCInst &Inst, InsnType bits, uint64_t Address, const void *Decoder) {
  int64_t  options = 0;

  if(bits & 1) options = MMPU::c_TC;
  mmpu_decode_c_option(bits>>1 & 3, options);

  Inst.addOperand(MCOperand::CreateImm(options));
  return MCDisassembler::Success;
}


// bits 19-18: C
// bits 17-15: WF or WX
template<typename InsnType>
static DecodeStatus
DecodeMr2Options(MCInst &Inst, InsnType bits, uint64_t Address, const void *Decoder)
{
  int64_t  options = 0;

  InsnType codes = bits & 0x7;

  switch(codes)
  {
      default: break;
      case 1:  options |= MMPU::c_WF1; break;
      case 2:  options |= MMPU::c_WF2; break;
      case 3:  options |= MMPU::c_WF3; break;
      case 4:  options |= MMPU::c_WX1; break;
      case 5:  options |= MMPU::c_WX2; break;
      case 6:  options |= MMPU::c_WX3; break;
      case 7:  options |= MMPU::c_WX4; break;
  }

  codes = (bits>>3) & 0x7;
  mmpu_decode_c_option(codes, options);

  Inst.addOperand(MCOperand::CreateImm(options));
  return MCDisassembler::Success;
}

template<typename InsnType>
static DecodeStatus
DecodeFALUOptions(MCInst &Inst, InsnType bits, uint64_t Address, const void *Decoder) {
  int64_t  options = 0;
  // T U S D
  if(bits & 1<<0) options |= MMPU::c_D; 	// (D)
  if(bits & 1<<1) options |= MMPU::c_S;   // (S)
  if(bits & 1<<2) options |= MMPU::c_U;   // (U)
  if(bits & 1<<3) options |= MMPU::c_T;   // (T)

  mmpu_decode_c_option(bits>>4 & 3, options);

  Inst.addOperand(MCOperand::CreateImm(options));
  return MCDisassembler::Success;
}

template<typename InsnType>
static DecodeStatus
DecodeFALUDblOptions(MCInst &Inst, InsnType bits, uint64_t Address, const void *Decoder) {
  int64_t  options = 0;
  // T X
  if(bits & 1<<1) options |= MMPU::c_T;   // (T)

  mmpu_decode_c_option(bits>>2 & 3, options);

  Inst.addOperand(MCOperand::CreateImm(options));
  return MCDisassembler::Success;
}

template<typename InsnType>
static DecodeStatus
DecodeCOption(MCInst &Inst, InsnType bits, uint64_t Address, const void *Decoder) {
  int64_t options = 0;
  mmpu_decode_c_option(bits&3, options);
  Inst.addOperand(MCOperand::CreateImm(options));
  return MCDisassembler::Success;
}

template<typename InsnType>
static DecodeStatus
DecodeIMACOptions(MCInst &Inst, InsnType bits, uint64_t Address, const void *Decoder) {
  int64_t  options = 0;

  // bits 22-17 : C NC CR  L  I  U  B H
  // Note: we do not need to encode (L)
  if(bits & 1<<0) options |= MMPU::c_H;  // (H)
  if(bits & 1<<1) options |= MMPU::c_B;  // (B)
  if(bits & 1<<2) options |= MMPU::c_U;  // (U)
  if(bits & 1<<3) options |= MMPU::c_I;  // (I)

  if(bits & 1<<5) options |= MMPU::c_CR;  // (CR)

  mmpu_decode_c_option(bits>>6 & 3, options);

  Inst.addOperand(MCOperand::CreateImm(options));
  return MCDisassembler::Success;
}

template<typename InsnType>
static DecodeStatus
DecodeFMACOptions(MCInst &Inst, InsnType bits, uint64_t Address, const void *Decoder) {
  int64_t  options = 0;

  // bits 18-17 T/CR S/D

  if(bits & 1) options |= MMPU::c_D;
  else         options |= MMPU::c_S;

  // list all FMAS instructions.
  switch(Inst.getOpcode()) {
    case MMPUInst::FMACMulToMw3:
    case MMPUInst::FMACMulToBIU:
    case MMPUInst::FMACMulToUnit:
      if(bits & 1<<1) options |= MMPU::c_T; // (T)
      break;

    case MMPUInst::FMACMulAccOnMRXferToMw3:
    case MMPUInst::FMACMulAccOnMRXferToBIU:
    case MMPUInst::FMACMulAccOnMRXferToUnit:

    case MMPUInst::FMACComplexMulToMw3:
    case MMPUInst::FMACComplexMulToBIU:
    case MMPUInst::FMACComplexMulToUnit:

    case MMPUInst::FMACMulAddToMR:
    case MMPUInst::FMACMulAccOnMR:
      if(bits & 1<<1) options |= MMPU::c_CR; // (CR)
      break;

    default:
      llvm_unreachable("unknown fmac instruction opcode");
  }

  mmpu_decode_c_option(bits>>2, options);

  Inst.addOperand(MCOperand::CreateImm(options));
  return MCDisassembler::Success;
}

template<typename InsnType>
static DecodeStatus
DecodeUImm10(MCInst &Inst, InsnType bits, uint64_t Address, const void *Decoder) {
  unsigned imm = bits;
  const MCDisassembler *Dis = static_cast<const MCDisassembler *>(Decoder);
  if (!MMPUTryAddingSymbolicOperand(imm, true, Address, 0x25, 2, Inst, Dis))
    Inst.addOperand(MCOperand::CreateImm(bits*MMPU::SumEncodingBytes));
  return MCDisassembler::Success;
}

template<typename InsnType>
static DecodeStatus
DecodeMRegExt(MCInst &Inst, InsnType bits, uint64_t Address, const void *Decoder) {
  unsigned Reg = getReg(Decoder, MMPUReg::MRegRegClassID, bits);
  Inst.addOperand(MCOperand::CreateReg(Reg));
  return MCDisassembler::Success;
}

template<typename InsnType>
static DecodeStatus
DecodeTPortExt(MCInst &Inst, InsnType bits, uint64_t Address, const void *Decoder) {
  unsigned Reg = getReg(Decoder, MMPUReg::TPortRegClassID, bits);
  Inst.addOperand(MCOperand::CreateReg(Reg));
  return MCDisassembler::Success;
}

/********************************************************************************/
#include "MMPUGenDisassemblerTables.inc"
/********************************************************************************/

static cl::opt<bool>
ShowCodeByInsn("show-code-by-insn", cl::desc("When disassembling instructions, print"
                                             "encodings by instructions."));

DecodeStatus MMPUDisassembler::getInstruction(MCInst &Inst, uint64_t &Size,
		const MemoryObject &Region, uint64_t Address, raw_ostream &vStream,
		raw_ostream &CStream) const
{
	uint64_t codes=0, numBytes=0;
	MMPU::InsnLine line;
	MCInst * formerInst, *curInst;
	Size=0;

	DecodeStatus result = readInsnLine(Region, Address, numBytes, line);

	if(result == MCDisassembler::Success) {
		line.ExtractCode(codes, 0);

		result = decodeInstruction(DecoderTableMMPUDecode48, Inst, codes, Address, this, STI);
		if(result != MCDisassembler::Success) return result;

		if(ShowCodeByInsn) CStream << "[";
		if(ShowCodeByInsn) CStream << format("%7x ", codes);

		formerInst = &Inst;
		for(unsigned i=1; i<NUMSLOTS; i++)
		{
			line.ExtractCode(codes, i);
			curInst = new MCInst();
			// Calling the auto-generated decoder function.
			result = decodeInstruction(DecoderTableMMPUDecode48, *curInst, codes, Address, this, STI);
			if(result != MCDisassembler::Success) return result;

			if(ShowCodeByInsn) CStream << format("%7x ", codes);

			formerInst->addOperand(MCOperand::CreateInst(curInst));
			formerInst = curInst;
		}
		if(ShowCodeByInsn) CStream << "]";

		Size = numBytes;

	}

	return result;
}

namespace llvm {
	extern Target TheMMPUTarget;
}

static MCDisassembler *createMMPUDisassembler(const Target &T,
                       const MCSubtargetInfo &STI) {
  return new MMPUDisassembler(STI, T.createMCRegInfo(""), true);
}

extern "C" void LLVMInitializeMMPUDisassembler() {
  // Register the disassembler.
  TargetRegistry::RegisterMCDisassembler(TheMMPUTarget,
                                         createMMPUDisassembler);
}
