//===- MSPUDisassembler.cpp - Disassembler for MSPU -------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file is part of the MSPU Disassembler.
//
//===----------------------------------------------------------------------===//

#include "MSPUDisassembler.h"

/// tryAddingSymbolicOperand - trys to add a symbolic operand in place of the
/// immediate Value in the MCInst.
///
/// @param Value      - The immediate Value, has had any PC adjustment made by
///                     the caller.
/// @param isBranch   - If the instruction is a branch instruction
/// @param Address    - The starting address of the instruction
/// @param Offset     - The byte offset to this immediate in the instruction
/// @param Width      - The byte width  of this immediate in the instruction
///
/// If the getOpInfo() function was set when setupForSymbolicDisassembly() was
/// called then that function is called to get any symbolic information for the
/// immediate in the instruction using the Address, Offset and Width.
///
/// If getOpInfo() returns non-zero then the symbolic information it returns is
/// used to create an MCExpr and that is added as an operand to the MCInst.
///
/// If getOpInfo() returns zero and isBranch is true then a symbol look up for
/// immediate Value is done and if a symbol is found an MCExpr is created with that,
/// else an MCExpr with the immediate Value is created.  This function returns true
/// if it adds an operand to the MCInst and false otherwise.
static bool MSPUTryAddingSymbolicOperand(int64_t Value, bool isBranch,
                                     uint64_t Address, uint64_t Offset,
                                     uint64_t Width, MCInst &MI,
                                     const MCDisassembler *Dis)
{
    return Dis->tryAddingSymbolicOperand(MI, Value, Address, isBranch, Offset, Width);
}

/// readInstruction - read four bytes from the MemoryObject
/// and return 32 bit word sorted according to the given endianess
static DecodeStatus readInstruction(const MemoryObject &region,
		uint64_t address, uint64_t &numBytes, uint64_t &insnCodes, bool isLittleEndian)
{
	uint8_t Bytes[4];

	// We want to read exactly 4 Bytes of data.
	// not zero means fail.
	if (region.readBytes(address, 4, (uint8_t*) Bytes) != 0) {
		numBytes = 0;
		return MCDisassembler::Fail;
	}

	if (isLittleEndian) {
    // Encoded as a little-endian 32-bit word in the stream.
    insnCodes = (Bytes[0] << 0) | (Bytes[1] << 8) | (Bytes[2] << 16)
        | (Bytes[3] << 24);
  }
	else {
		// Encoded as a big-endian 32-bit word in the stream.
		insnCodes = (Bytes[3] << 0) | (Bytes[2] << 8) | (Bytes[1] << 16)
				| (Bytes[0] << 24);
	}

	insnCodes &= 0xFFFFFFFFu; // or it will be sign-extended.
	numBytes = 4;
	return MCDisassembler::Success;
}

static unsigned
getMSPURegister(const void *D, unsigned RC, unsigned RegNo)
{
	const MSPUDisassembler *Dis = static_cast<const MSPUDisassembler*>(D);
	return Dis->getRegInfo()->getRegClass(RC).getRegister(RegNo);
}

static DecodeStatus
DecodeRRegRegisterClass(MCInst &Inst, unsigned RegNo,
		uint64_t Address, const void *Decoder)
{
	unsigned Reg = getMSPURegister(Decoder, MSPUReg::RRegRegClassID, RegNo);
	Inst.addOperand(MCOperand::CreateReg(Reg));
	return MCDisassembler::Success;
}

static DecodeStatus
DecodeDRegRegisterClass(MCInst &Inst, unsigned RegNo,
		uint64_t Address, const void *Decoder)
{
	unsigned Reg = getMSPURegister(Decoder, MSPUReg::DRegRegClassID, RegNo>>1);
	Inst.addOperand(MCOperand::CreateReg(Reg));
	return MCDisassembler::Success;
}

static DecodeStatus
DecodeMCRegRegisterClass(MCInst &Inst, unsigned RegNo,
    uint64_t Address, const void *Decoder)
{
  unsigned Reg = getMSPURegister(Decoder, MSPUReg::MCRegRegClassID, RegNo);
  Inst.addOperand(MCOperand::CreateReg(Reg));
  return MCDisassembler::Success;
}

static DecodeStatus
DecodeJRegRegisterClass(MCInst &Inst, unsigned RegNo,
		uint64_t Address, const void *Decoder)
{
	unsigned Reg = getMSPURegister(Decoder, MSPUReg::JRegRegClassID, RegNo);
	Inst.addOperand(MCOperand::CreateReg(Reg));
	return MCDisassembler::Success;
}

static DecodeStatus
DecodeKMRegRegisterClass(MCInst &Inst, unsigned RegNo,
		uint64_t Address, const void *Decoder)
{
	unsigned Reg = getMSPURegister(Decoder, MSPUReg::KMRegRegClassID, RegNo);
	Inst.addOperand(MCOperand::CreateReg(Reg));
	return MCDisassembler::Success;
}

static DecodeStatus
DecodeI32RegRegisterClass(MCInst &Inst, unsigned RegNo,
		uint64_t Address, const void *Decoder)
{
	unsigned Reg = getMSPURegister(Decoder, MSPUReg::I32RegRegClassID, RegNo);
	Inst.addOperand(MCOperand::CreateReg(Reg));
	return MCDisassembler::Success;
}

static DecodeStatus
DecodePtrRegRegisterClass(MCInst &Inst, unsigned RegNo,
		uint64_t Address, const void *Decoder)
{
	unsigned Reg = getMSPURegister(Decoder, MSPUReg::PtrRegRegClassID, RegNo);
	Inst.addOperand(MCOperand::CreateReg(Reg));
	return MCDisassembler::Success;
}

static DecodeStatus
DecodeSVRRegRegisterClass(MCInst &Inst, unsigned RegNo,
		uint64_t Address, const void *Decoder)
{
	unsigned Reg = getMSPURegister(Decoder, MSPUReg::SVRRegRegClassID, RegNo);
	Inst.addOperand(MCOperand::CreateReg(Reg));
	return MCDisassembler::Success;
}

static DecodeStatus
DecodeSHURegRegisterClass(MCInst &Inst, unsigned RegNo,
		uint64_t Address, const void *Decoder)
{
	unsigned Reg = getMSPURegister(Decoder, MSPUReg::SHURegRegClassID, RegNo);
	Inst.addOperand(MCOperand::CreateReg(Reg));
	return MCDisassembler::Success;
}

static DecodeStatus
DecodeTRegRegisterClass(MCInst &Inst, unsigned RegNo,
		uint64_t Address, const void *Decoder)
{
	unsigned Reg = getMSPURegister(Decoder, MSPUReg::TRegRegClassID, RegNo);
	Inst.addOperand(MCOperand::CreateReg(Reg));
	return MCDisassembler::Success;
}

///////////////////////////////////////////////////////////////////////////////////
template<typename InsnType>
static DecodeStatus
DecodeAddrJJ(MCInst &Inst, InsnType bits, uint64_t Address, const void *Decoder) {
  unsigned reg0 = bits & 0x1F;
	unsigned reg1 = (bits>>5) & 0x1F;

	unsigned RegEnum0  = getMSPURegister(Decoder, MSPUReg::PtrRegRegClassID, reg0);
  unsigned RegEnum1  = getMSPURegister(Decoder, MSPUReg::PtrRegRegClassID, reg1);

  Inst.addOperand(MCOperand::CreateReg(RegEnum0));
  Inst.addOperand(MCOperand::CreateReg(RegEnum1));

  return MCDisassembler::Success;
}

template<typename InsnType>
static DecodeStatus
DecodeAddrJImm(MCInst &Inst, InsnType bits, uint64_t Address, const void *Decoder) {
  unsigned BaseRegNo = bits & 0x1F; // 5 bits
  unsigned ImmOffset = (bits>>5) & 0x7FF; // 11 bits
  int64_t   Imm11    = SignExtend64<11>( ImmOffset );

  unsigned RegEnum  = getMSPURegister(Decoder, MSPUReg::PtrRegRegClassID, BaseRegNo);

  Inst.addOperand(MCOperand::CreateReg(RegEnum));
  Inst.addOperand(MCOperand::CreateImm(Imm11));

  return MCDisassembler::Success;
}

template<typename InsnType>
static DecodeStatus
DecodeSImm11(MCInst &Inst, InsnType bits, uint64_t Address, const void *Decoder) {
  int64_t  val    = SignExtend64<11>( bits ); // 11 bits
  Inst.addOperand(MCOperand::CreateImm(val));
  return MCDisassembler::Success;
}

template<typename InsnType>
static DecodeStatus
DecodeXYFlag(MCInst &Inst, InsnType bits, uint64_t Address, const void *Decoder) {
  switch(bits) {
    case 0:
    case 1:
    case 2:
      bits += 1; break;

    default:
      llvm_unreachable("Invalid encoding for X/Y flags");
  }

  Inst.addOperand(MCOperand::CreateImm(bits));
  return MCDisassembler::Success;
}

template<typename InsnType>
static DecodeStatus
DecodeSImm28(MCInst &Inst, InsnType bits, uint64_t Address, const void *Decoder) {
  int64_t  val    = SignExtend64<28>( bits ); // 28 bits
  Inst.addOperand(MCOperand::CreateImm(val));
  return MCDisassembler::Success;
}

template<typename InsnType>
static DecodeStatus
DecodeUImm17(MCInst &Inst, InsnType bits, uint64_t Address, const void *Decoder) {
  Inst.addOperand(MCOperand::CreateImm(bits*MMPUEncodingBytes));
  return MCDisassembler::Success;
}

template<typename InsnType>
static DecodeStatus
DecodeRJReg(MCInst &Inst, InsnType bits, uint64_t Address, const void *Decoder) {
  unsigned reg  = bits & 0x1F;
  unsigned RegEnum;
  if(bits & (1<<5)) // is JReg
  	RegEnum	= getMSPURegister(Decoder, MSPUReg::JRegRegClassID, reg);
  else
  	RegEnum	= getMSPURegister(Decoder, MSPUReg::RRegRegClassID, reg);

  Inst.addOperand(MCOperand::CreateReg(RegEnum));
  return MCDisassembler::Success;
}

template<typename InsnType>
static DecodeStatus
DecodeRDReg(MCInst &Inst, InsnType bits, uint64_t Address, const void *Decoder) {
  unsigned reg  = bits & 0x1F;
  unsigned RegEnum;
  if(bits & (1<<5)) // is DReg, otherwise RReg
  	RegEnum	= getMSPURegister(Decoder, MSPUReg::DRegRegClassID, reg>>1);
  else
  	RegEnum	= getMSPURegister(Decoder, MSPUReg::RRegRegClassID, reg);

  Inst.addOperand(MCOperand::CreateReg(RegEnum));
  return MCDisassembler::Success;
}

template<typename InsnType>
static DecodeStatus
DecodeKReg(MCInst &Inst, InsnType bits, uint64_t Address, const void *Decoder)
{
	unsigned reg = bits & 0xF;
	unsigned rn =0;
	switch(bits >>4) {
		case 0: rn = getMSPURegister(Decoder, MSPUReg::KBRegRegClassID, reg); break;
		case 1: rn = getMSPURegister(Decoder, MSPUReg::KERegRegClassID, reg); break;
		case 2: rn = getMSPURegister(Decoder, MSPUReg::KSRegRegClassID, reg); break;
		case 3: rn = getMSPURegister(Decoder, MSPUReg::KIRegRegClassID, reg); break;
		case 5:
			reg >>= 2;
			rn = getMSPURegister(Decoder, MSPUReg::KGRegRegClassID, reg); break;
		case 6:
			reg >>= 2;
			rn = getMSPURegister(Decoder, MSPUReg::KLRegRegClassID, reg); break;
		default:
			llvm_unreachable("Invalid K register group");
	}
	Inst.addOperand(MCOperand::CreateReg(rn));
	return MCDisassembler::Success;
}

template<typename InsnType>
static DecodeStatus
DecodeLdStReg(MCInst &Inst, InsnType bits, uint64_t Address, const void *Decoder)
{
	unsigned reg = bits & 0x1F;
	unsigned rn =0;
	switch(bits >>5) {
		case 0: rn = getMSPURegister(Decoder, MSPUReg::RRegRegClassID, reg); break;
    case 1: rn = getMSPURegister(Decoder, MSPUReg::DRegRegClassID, reg>>1); break;
		case 2:
		case 3: rn = getMSPURegister(Decoder, MSPUReg::JRegRegClassID, reg); break;
		default:
			llvm_unreachable("Invalid register group for load/store operations");
	}
	Inst.addOperand(MCOperand::CreateReg(rn));
	return MCDisassembler::Success;
}

static DecodeStatus
DecodeSImm17(MCInst &Inst, uint64_t bits, uint64_t Address, const void *Decoder);

#include "MSPUGenDisassemblerTables.inc"

// simm17 is used only in branch instructions, say, jump, call.
static DecodeStatus
DecodeSImm17(MCInst &Inst, uint64_t bits, uint64_t Address, const void *Decoder) {
  const MSPUDisassembler *Dis = static_cast<const MSPUDisassembler*>(Decoder);
  uint64_t numBytes, codes;
  int64_t  imm;
  MCInst ExtInst;
  DecodeStatus result = readInstruction(*Dis->getMemoryObject(), Address+4,
                                        numBytes, codes, true/*little endian*/);

  if(result == MCDisassembler::Success)
  {
    uint32_t opc = codes >> 28;
    opc  &= 7;
    if(opc == 5)
    {
      imm = bits & 0xF;
      codes &= 0xFFFFFFF;
      imm |= (codes<<4);
      imm = SignExtend64<32>( imm ); // 32-bits imm
    }
    else
      imm = SignExtend64<17>( bits ); // 17-bits
  }
  else
    imm = SignExtend64<17>( bits ); // 17-bits imm

  if( !MSPUTryAddingSymbolicOperand(imm       /* imm value */,
                                    true      /* isBranch */,
                                    Address,  /* instruction address */
                                    0         /* byte offset of this imm */,
                                    4         /* byte width of this imm */,
                                    Inst, Dis) )
  {
    Inst.addOperand(MCOperand::CreateImm(imm));
  }

  return MCDisassembler::Success;
}

// decode into *Inst*
DecodeStatus MSPUDisassembler::getInstruction(MCInst &Inst, uint64_t &Size,
		const MemoryObject &Region, uint64_t Address, raw_ostream &vStream,
		raw_ostream &cStream) const
{
  // we pass *Region*
  MemObj = &Region;

  Size = 0; // initialize Size.

  uint64_t codes, numBytes;  // template<typename InsnType>
  DecodeStatus result = readInstruction(Region, Address, numBytes, codes, true/*little endian*/);
  if(result != MCDisassembler::Success) return result;

  int i = 0;
  MCInst * ITR = &Inst;
  ITR->setStart(true);
  ITR->setPrev(0);
  while((codes & (1 << 31)) == 0 && i<3) {// expect for a next instruction.
    ITR->setEnd(false);
    // generated decode function
    result = decodeInstruction(DecoderTableMSPUDecode32, *ITR, codes & (~(1 << 31)), Address+Size, this, STI);
    if(result != MCDisassembler::Success)   return result;
    Size += 4;

    MCInst * next = &(MSPUMCInstArray[i]); i++;

    ITR->setNext(next);
    next->setPrev(ITR);

    ITR = next;
    ITR->setStart(false);
    ITR->clear(); // to clear *MSPUMCInstArray* operands to avoid pollution.

    result = readInstruction(Region, Address+Size, numBytes, codes, true/*little endian*/);
    if(result != MCDisassembler::Success) return result;
  }

  ITR->setEnd(true);
  ITR->setNext(0);
  result = decodeInstruction(DecoderTableMSPUDecode32, *ITR, codes & (~(1 << 31)), Address+Size, this, STI);
  if(result == MCDisassembler::Success) Size += 4;

	return result;
}

extern Target llvm::TheMSPUTarget;

static MCDisassembler *createMSPUDisassembler(const Target &T,
                       const MCSubtargetInfo &STI) {
  return new MSPUDisassembler(STI, T.createMCRegInfo(""), true);
}

extern "C" void LLVMInitializeMSPUDisassembler() {
  // Register the disassembler.
  TargetRegistry::RegisterMCDisassembler(TheMSPUTarget,
                                         createMSPUDisassembler);
}
