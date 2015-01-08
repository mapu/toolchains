//===- MSPUInstPrinter.cpp - Convert MSPU MCInst to assembly syntax -===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This class prints an MSPU MCInst to a .s file.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "asm-printer"
#include "../MSPU.h"
#include "../MSPUAsmPrinter.h"
#include "../MSPUInstFlags.h"
#include "../MCTargetDesc/MSPUMCInst.h"
#include "MSPUInstPrinter.h"
#include "llvm/MC/MCInst.h"
#include "../MCTargetDesc/MSPUMCTargetDesc.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/Support/raw_ostream.h"
#include <cstdio>

using namespace llvm;

#include "MSPUGenAsmWriter.inc"

void
MSPUInstPrinter::printInst(const MCInst *MI, raw_ostream &O,
							StringRef Annot)
{
  assert(MI->getOpcode() != MSPUInst::ImmExt && "should not print out imm-ext instructions");

  while(MI) {
    const MSPUMCInst *MSMI = static_cast<const MSPUMCInst *>(MI);
    if(MI->getOpcode() != MSPUInst::ImmExt) {

      if(MSMI->isStart()) {
        O << "m.s ";
      }

      printInstruction(MI, O); // tblgen'erated function

      // ...; insn;;
      if(MSMI->isEnd()) {
        O << ";;";
        return;
      }

      // ...; insn; imm ;;
      MSMI = MSMI->getNext();
      if(MSMI && (MSMI->getOpcode() == MSPUInst::ImmExt) && MSMI->isEnd()) {
        O << ";;\n";
        return;
      }

      // ...; insn; ...
      O << "; ";
    }
    else {
      MSMI = MSMI->getNext();
    }
  }

  return;
}

void
MSPUInstPrinter::printOperand(const MCInst *MI, unsigned OpNo,
								raw_ostream &O) const
{
	const MCOperand& MO = MI->getOperand(OpNo);

	if(MO.isReg()) {
		O << getRegisterName(MO.getReg());
	}
	else if(MO.isExpr()) {
		O << *(MO.getExpr());
	}
	else if(MO.isImm()) {
		O << MO.getImm();
	}
	else {
		assert(false && "Unknown operand");
	}
}

void
MSPUInstPrinter::printSCUExtraUnaReg(const MCInst *MI, unsigned OpNo,
								raw_ostream &O) const
{
	const MCOperand& MO = MI->getOperand(OpNo);
	unsigned reg = MO.getReg();

	O << getRegisterName(reg);
	if(MSPUReg::D0 < reg && reg<MSPUReg::D15) O << " (D)";
}

void
MSPUInstPrinter::printImmExt(const MCInst *MI, unsigned OpNo, raw_ostream &O) const
{
	const MCOperand& MO = MI->getOperand(OpNo);

	if(MO.isExpr()) {
		if(MO.getExpr()->getKind() == MCExpr::Constant) {
			const MCConstantExpr *expr = static_cast<const MCConstantExpr *>(MO.getExpr());
			O<<expr->getValue();
		}
		else		O << *(MO.getExpr());

		return;
	}

  if(MO.isFPImm()) {
    O << MO.getFPImm();
    return;
  }

	assert(MO.isImm() && "expect an imm operand here");
	int64_t val = MO.getImm();

	int64_t hi=0;

	if(const MSPUMCInst * sub = static_cast<const MSPUMCInst *>(MI)->getNext()) {
		if(sub->getOpcode() == MSPUInst::ImmExt) {
			assert(sub->getOperand(0).isImm() && "expect imm operand");
			hi = sub->getOperand(0).getImm();
		}
	}

  if(hi != 0) {
    switch(MI->getOpcode()) {
      case MSPUInst::CallMImm:
        val = (hi * MMPUEncodingBytes << 4) | (val & 0xF);
        break;

      default:
        val = (hi << 4) | (val & 0xF);
        break;
    }
  }

	O << val;
}

void
MSPUInstPrinter::printCallMFlag(const MCInst *MI, unsigned OpNo, raw_ostream &O) const
{
	const MCOperand& MO = MI->getOperand(OpNo);
	unsigned f = MO.getImm();
	if(f & 1) O << " (B)";
}

void
MSPUInstPrinter::printXferFlag(const MCInst *MI, unsigned OpNo, raw_ostream &O) const
{
	const MCOperand& MO = MI->getOperand(OpNo);
	unsigned f = MO.getImm();

	assert(((f>>1)&3) != 3 && "invalid flag, (B) and (H) coexist");

	if(f & (1<<2)) O << " (B)";
	if(f & (1<<1)) O << " (H)";
	if(f & (1<<0)) O << " (U)";
}

void
MSPUInstPrinter::printXYFlag(const MCInst *MI, unsigned OpNo, raw_ostream &O) const
{
  const MCOperand& MO = MI->getOperand(OpNo);
  unsigned f = MO.getImm();

  switch(f) {
    case 0:              break;
    case 1: O << " (X)"; break;
    case 2: O << " (Y)"; break;
    case 3: O << " (XY)"; break;
    default: break;
  }
}

void
MSPUInstPrinter::printLdStOpc(const MCInst *MI, unsigned OpNo, raw_ostream &O) const
{
	const MCOperand& MO = MI->getOperand(OpNo);
	unsigned f = MO.getImm();

	if(f) O << "+=";
	else  O << "+";
}

void
MSPUInstPrinter::printSCUFlag(const MCInst *MI, unsigned OpNo, raw_ostream &O) const
{
	const MCOperand& MO = MI->getOperand(OpNo);
	unsigned f = MO.getImm();

	assert(((f & (1<<MSPU::SCUFlag::S)) == 0 ||
	        (f & (1<<MSPU::SCUFlag::D)) == 0 )  && "invalid flags for SCU, (S) and (D) coexist");

	if(f & (1<<MSPU::SCUFlag::U)) O << " (U)";
	if(f & (1<<MSPU::SCUFlag::S)) O << " (S)";
	if(f & (1<<MSPU::SCUFlag::D)) O << " (D)";
	if(f & (1<<MSPU::SCUFlag::T)) O << " (T)";
}

void
MSPUInstPrinter::printKMFlag(const MCInst *MI, unsigned OpNo, raw_ostream &O) const
{
	const MCOperand& MO = MI->getOperand(OpNo);
	unsigned f = MO.getImm()>>1;

	switch(f&1) {
		case 0: break;
		case 1: O << " (H)"; break;
		default:
			llvm_unreachable("error coding of KM-access instructions, flag bits for (L) or (H) invalid");
	}
}

void
MSPUInstPrinter::printIntrinKREG(const MCInst *MI, unsigned OpNo, raw_ostream &O) const
{
  const MCOperand& MO = MI->getOperand(OpNo);
  unsigned reg;

  if(MO.isReg()) {
    reg = MO.getReg();
  }
  else if(MO.isImm()) {
    unsigned code = MO.getImm();

    if(/*MSPUInstPrinter::KB0 <= code && */code <= MSPUInstPrinter::KB15)
      reg = MSPUReg::KB0 + (code - MSPUInstPrinter::KB0);
    else if(MSPUInstPrinter::KE0 <= code && code <= MSPUInstPrinter::KE15)
      reg = MSPUReg::KE0 + (code - MSPUInstPrinter::KE0);
    else if(MSPUInstPrinter::KS0 <= code && code <= MSPUInstPrinter::KS15)
      reg = MSPUReg::KS0 + (code - MSPUInstPrinter::KS0);
    else if(MSPUInstPrinter::KI0 <= code && code <= MSPUInstPrinter::KI15)
      reg = MSPUReg::KI0 + (code - MSPUInstPrinter::KI0);

    else {
      unsigned i = (code & 0xF) >>2;
      code &= (~0xF);
      switch(code) {
        case MSPUInstPrinter::KM0:
          reg = MSPUReg::KM0 + i; break;
        case MSPUInstPrinter::KG0:
          reg = MSPUReg::KG0 + i; break;
        case MSPUInstPrinter::KL0:
          reg = MSPUReg::KL0 + i; break;
        default:
          llvm_unreachable("expect KM, KG, or KL");
      }
    }

  }
  else {
    assert(false && "unknown operand type");
  }

  O << getRegisterName(reg);
  return;
}

void
MSPUInstPrinter::printIntrinKB(const MCInst *MI, unsigned OpNo, raw_ostream &O) const
{
  const MCOperand& MO = MI->getOperand(OpNo);
  unsigned reg;

  if(MO.isReg())
    reg = MO.getReg();
  else
    if(MO.isImm())
      reg = MSPUReg::KB0 + MO.getImm();

  O << getRegisterName(reg);
  return;
}

void
MSPUInstPrinter::printIntrinKE(const MCInst *MI, unsigned OpNo, raw_ostream &O) const
{
  const MCOperand& MO = MI->getOperand(OpNo);
  unsigned reg;

  if(MO.isReg())
    reg = MO.getReg();
  else
    if(MO.isImm())
      reg = MSPUReg::KE0 + MO.getImm();

  O << getRegisterName(reg);
  return;
}

void
MSPUInstPrinter::printIntrinKS(const MCInst *MI, unsigned OpNo, raw_ostream &O) const
{
  const MCOperand& MO = MI->getOperand(OpNo);
  unsigned reg;

  if(MO.isReg())
    reg = MO.getReg();
  else
    if(MO.isImm())
      reg = MSPUReg::KS0 + MO.getImm();

  O << getRegisterName(reg);
  return;
}

void
MSPUInstPrinter::printIntrinKI(const MCInst *MI, unsigned OpNo, raw_ostream &O) const
{
  const MCOperand& MO = MI->getOperand(OpNo);
  unsigned reg;

  if(MO.isReg())
    reg = MO.getReg();
  else
    if(MO.isImm())
      reg = MSPUReg::KI0 + MO.getImm();

  O << getRegisterName(reg);
  return;
}

// get and set SVR
enum MSPU_SVR {
  SVR0 = 0, // 0b'0
  SVR1 = 1, // 0b'1
};

// encoding for "BH"
enum MSPU_ACCESS_Mode {
  SVR_B = 4, // 0b'10
  SVR_H = 2, // 0b'01
  SVR_W = 0, // 0b'00
};

enum MSPU_SHU {
  SHU0 = 0, // 0b'0
  SHU1 = 1, // 0b'1
};

enum MSPU_T {
  T0 = 0, // 0b'00
  T1 = 1, // 0b'01
  T2 = 2, // 0b'10
  T3 = 3, // 0b'11
};

void
MSPUInstPrinter::printIntrinAccessMode(const MCInst *MI, unsigned OpNo, raw_ostream &O) const
{
  const MCOperand& MO = MI->getOperand(OpNo);
  unsigned f = MO.getImm();

  switch(f&0x3) {
    case SVR_W: break;
    case SVR_H: O << "(H)"; break;
    case SVR_B: O << "(B)"; break;
    default:
      llvm_unreachable("error coding of SVR access modes");
  }
}

void
MSPUInstPrinter::printIntrinSVR(const MCInst *MI, unsigned OpNo, raw_ostream &O) const
{
  const MCOperand& MO = MI->getOperand(OpNo);
  unsigned f = MO.getImm();

  switch(f&0x1) {
    case SVR0: O << "SVR0"; break;
    case SVR1: O << "SVR1"; break;
    default:
      llvm_unreachable("error coding of SVR");
  }
}

void
MSPUInstPrinter::printIntrinT(const MCInst *MI, unsigned OpNo, raw_ostream &O) const
{
  const MCOperand& MO = MI->getOperand(OpNo);
  unsigned f = MO.getImm();

  switch(f&0x3) {
    case T0: O << "T0"; break;
    case T1: O << "T1"; break;
    case T2: O << "T2"; break;
    case T3: O << "T3"; break;
    default:
      llvm_unreachable("error coding of T registers");
  }
}

void
MSPUInstPrinter::printIntrinSHU(const MCInst *MI, unsigned OpNo, raw_ostream &O) const
{
  const MCOperand& MO = MI->getOperand(OpNo);
  unsigned f = MO.getImm();

  switch(f&0x1) {
    case SHU0: O << "SHU0"; break;
    case SHU1: O << "SHU1"; break;
    default:
      llvm_unreachable("error coding of SVR access modes");
  }
}

void
MSPUInstPrinter::printIntrinKM(const MCInst *MI, unsigned OpNo, raw_ostream &O) const
{
  const MCOperand& MO = MI->getOperand(OpNo);
  unsigned reg;

  if(MO.isReg())
    reg = MO.getReg();
  else
    if(MO.isImm())
      reg = MSPUReg::KM0 + MO.getImm();

  O << getRegisterName(reg);
  return;
}



void
MSPUInstPrinter::printIntrinKG(const MCInst *MI, unsigned OpNo, raw_ostream &O) const
{
  const MCOperand& MO = MI->getOperand(OpNo);
  unsigned reg;

  if(MO.isReg())
    reg = MO.getReg();
  else
    if(MO.isImm())
      reg = MSPUReg::KG0 + MO.getImm();

  O << getRegisterName(reg);
  return;
}

void
MSPUInstPrinter::printIntrinKL(const MCInst *MI, unsigned OpNo, raw_ostream &O) const
{
  const MCOperand& MO = MI->getOperand(OpNo);
  unsigned reg;

  if(MO.isReg())
    reg = MO.getReg();
  else
    if(MO.isImm())
      reg = MSPUReg::KL0 + MO.getImm();

  O << getRegisterName(reg);
  return;
}

void
MSPUInstPrinter::printIntrinMC(const MCInst *MI, unsigned OpNo, raw_ostream &O) const
{
  const MCOperand& MO = MI->getOperand(OpNo);
  unsigned reg;

  if(MO.isReg())
    reg = MO.getReg();
  else
    if(MO.isImm())
      reg = MSPUReg::MC0 + MO.getImm();

  O << getRegisterName(reg);
  return;
}

void
MSPUInstPrinter::printAddrJImm(const MCInst *MI, unsigned OpNo, raw_ostream &O) const
{
	const MCOperand& MO0 = MI->getOperand(OpNo);
	const MCOperand& MO1 = MI->getOperand(OpNo+1);

	O << getRegisterName(MO0.getReg());
	O << " + " << MO1.getImm();
}

void
MSPUInstPrinter::printAddrJJ(const MCInst *MI, unsigned OpNo, raw_ostream &O) const
{
	const MCOperand& MO0 = MI->getOperand(OpNo);
	const MCOperand& MO1 = MI->getOperand(OpNo + 1);

	O << getRegisterName(MO0.getReg());
	O << " + " << getRegisterName(MO1.getReg());
}
