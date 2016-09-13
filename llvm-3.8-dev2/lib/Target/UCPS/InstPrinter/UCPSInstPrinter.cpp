//===- UCPSInstPrinter.cpp - Convert UCPS MCInst to assembly syntax -===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This class prints an UCPS MCInst to a .s file.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "asm-printer"
#include "../UCPS.h"
#include "../UCPSAsmPrinter.h"
#include "../UCPSInstFlags.h"
#include "UCPSInstPrinter.h"
#include "llvm/MC/MCInst.h"
#include "../MCTargetDesc/UCPSMCTargetDesc.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/Support/raw_ostream.h"
#include <cstdio>

using namespace llvm;

#include "UCPSGenAsmWriter.inc"

void UCPSInstPrinter::printInst(const MCInst *MI, raw_ostream &O,
                                StringRef Annot, const MCSubtargetInfo &STI) {
  assert(MI->getOpcode() != UCPSInst::ImmExt &&
         "should not print out imm-ext instructions");
  const MCInst *MSMI = MI;

  O << "\tm.s ";
  do {
    printInstruction(MSMI, O); // tblgen'erated function

    // ...; insn;;
    if (MSMI->getNumOperands() == 0
        || !MSMI->getOperand(MSMI->getNumOperands() - 1).isInst()) {
      O << ";;";
      return;
    }

    // ...; insn; imm ;;
    if (MSMI->getNumOperands() &&
        MSMI->getOperand(MSMI->getNumOperands() - 1).isInst()) {
      MSMI = MSMI->getOperand(MSMI->getNumOperands() - 1).getInst();
      if (MSMI && (MSMI->getOpcode() == UCPSInst::ImmExt)) {
        if (MSMI->getNumOperands() &&
            MSMI->getOperand(MSMI->getNumOperands() - 1).isInst()) {
          MSMI = MSMI->getOperand(MSMI->getNumOperands() - 1).getInst();
          O << ";";
        } else {
          O << ";;";
          return;
        }
      } else {
        O << ";";
      }
    } else {
      assert("Invalid instruction line end.");
    }
  } while (1);
}

void UCPSInstPrinter::printOperand(const MCInst *MI, unsigned OpNo,
                                   raw_ostream &O) const {
  const MCOperand &MO = MI->getOperand(OpNo);

  if (MO.isReg()) {
    O << getRegisterName(MO.getReg());
  } else if (MO.isExpr()) {
    O << *(MO.getExpr());
  } else if (MO.isImm()) {
    O << formatImm(MO.getImm());
  } else {
    assert(false && "Unknown operand");
  }
}

void UCPSInstPrinter::printSCUExtraUnaReg(const MCInst *MI, unsigned OpNo,
                                          raw_ostream &O) const {
  const MCOperand &MO = MI->getOperand(OpNo);
  unsigned reg = MO.getReg();

  O << getRegisterName(reg);
  if (UCPSReg::D0 < reg && reg < UCPSReg::D15) O << " (D)";
}

void UCPSInstPrinter::printImmExt(const MCInst *MI, unsigned OpNo,
                                  raw_ostream &O) const {
  const MCOperand &MO = MI->getOperand(OpNo);

  if (MO.isExpr()) {
    if (MO.getExpr()->getKind() == MCExpr::Constant) {
      const MCConstantExpr *expr =
        static_cast<const MCConstantExpr *>(MO.getExpr());
      O << expr->getValue();
    }
    else O << *(MO.getExpr());

    return;
  }

  if (MO.isFPImm()) {
    O << formatImm(MO.getFPImm());
    return;
  }

  assert(MO.isImm() && "expect an imm operand here");
  int64_t val = MO.getImm();

  int64_t hi = 0;
  const MCInst *MIP = MI;

  while (MIP->getNumOperands() &&
         MIP->getOperand(MIP->getNumOperands() - 1).isInst()) {
    MIP = MIP->getOperand(MIP->getNumOperands() - 1).getInst();
    if (MIP->getOpcode() == UCPSInst::ImmExt) {
      assert(MIP->getOperand(0).isImm() && "expect imm operand");
      hi = MIP->getOperand(0).getImm();
      break;
    }
  }

  if (hi != 0) {
    switch (MI->getOpcode()) {
    case UCPSInst::CallMImm:
      val = (hi * MMPUEncodingBytes << 4) | (val & 0xF);
      break;

    default:
      val = (hi << 4) | (val & 0xF);
      break;
    }
  }

  O << formatImm(val);
}

void UCPSInstPrinter::printCallMFlag(const MCInst *MI, unsigned OpNo,
                                     raw_ostream &O) const {
  const MCOperand &MO = MI->getOperand(OpNo);
  unsigned f = MO.getImm();
  if (f & 1) O << " (B)";
}

void UCPSInstPrinter::printXferFlag(const MCInst *MI, unsigned OpNo,
                               raw_ostream &O) const {
  const MCOperand &MO = MI->getOperand(OpNo);
  unsigned f = MO.getImm();

  assert(((f >> 1) & 3) != 3 && "invalid flag, (B) and (H) coexist");

  if (f & (1 << 2)) O << " (B)";
  if (f & (1 << 1)) O << " (H)";
  if (f & (1 << 0)) O << " (U)";
}

void UCPSInstPrinter::printXYFlag(const MCInst *MI, unsigned OpNo,
                                  raw_ostream &O) const {
  const MCOperand &MO = MI->getOperand(OpNo);
  unsigned f = MO.getImm();

  switch (f) {
  case 0: break;
  case 1: O << " (X)"; break;
  case 2: O << " (Y)"; break;
  case 3: O << " (XY)"; break;
  default: break;
  }
}

void UCPSInstPrinter::printLdStOpc(const MCInst *MI, unsigned OpNo,
                                   raw_ostream &O) const {
  const MCOperand &MO = MI->getOperand(OpNo);
  unsigned f = MO.getImm();

  if (f) O << "+=";
  else O << "+";
}

void UCPSInstPrinter::printSCUFlag(const MCInst *MI, unsigned OpNo,
                                   raw_ostream &O) const {
  const MCOperand &MO = MI->getOperand(OpNo);
  unsigned f = MO.getImm();

  assert(((f & (1 << UCPS::SCUFlag::S)) == 0 ||
          (f & (1 << UCPS::SCUFlag::D)) == 0)
         && "invalid flags for SCU, (S) and (D) coexist");

  if (f & (1 << UCPS::SCUFlag::U)) O << " (U)";
  if (f & (1 << UCPS::SCUFlag::S)) O << " (S)";
  if (f & (1 << UCPS::SCUFlag::D)) O << " (D)";
  if (f & (1 << UCPS::SCUFlag::T)) O << " (T)";
}

void UCPSInstPrinter::printKMFlag(const MCInst *MI, unsigned OpNo,
                                  raw_ostream &O) const {
  const MCOperand &MO = MI->getOperand(OpNo);
  unsigned f = MO.getImm();

  switch (f) {
  case 1: break;
  case 2: O << " (H)"; break;
  default:
    llvm_unreachable("error coding of KM-access instructions, "
                     "flag bits for (L) or (H) invalid");
  }
}

void UCPSInstPrinter::printIntrinKREG(const MCInst *MI, unsigned OpNo,
                                      raw_ostream &O) const {
  const MCOperand &MO = MI->getOperand(OpNo);
  unsigned reg;

  if (MO.isReg()) {
    reg = MO.getReg();
  } else if (MO.isImm()) {
    unsigned code = MO.getImm();

    if (/*UCPSInstPrinter::KB0 <= code && */code <= UCPSInstPrinter::KB15)
      reg = UCPSReg::KB0 + (code - UCPSInstPrinter::KB0);
    else if (UCPSInstPrinter::KE0 <= code && code <= UCPSInstPrinter::KE15)
      reg = UCPSReg::KE0 + (code - UCPSInstPrinter::KE0);
    else if (UCPSInstPrinter::KS0 <= code && code <= UCPSInstPrinter::KS15)
      reg = UCPSReg::KS0 + (code - UCPSInstPrinter::KS0);
    else if (UCPSInstPrinter::KI0 <= code && code <= UCPSInstPrinter::KI15)
      reg = UCPSReg::KI0 + (code - UCPSInstPrinter::KI0);
    else {
      unsigned i = (code & 0xF) >> 2;
      code &= (~0xF);
      switch (code) {
      case UCPSInstPrinter::KM0:
        reg = UCPSReg::KM0 + i;
        break;
      case UCPSInstPrinter::KG0:
        reg = UCPSReg::KG0 + i;
        break;
      case UCPSInstPrinter::KL0:
        reg = UCPSReg::KL0 + i;
        break;
      default:
        llvm_unreachable("expect KM, KG, or KL");
      }
    }
  } else {
    assert(false && "unknown operand type");
  }

  O << getRegisterName(reg);
  return;
}

void UCPSInstPrinter::printIntrinKB(const MCInst *MI, unsigned OpNo,
                                    raw_ostream &O) const {
  const MCOperand &MO = MI->getOperand(OpNo);
  unsigned reg;

  if (MO.isReg()) reg = MO.getReg();
  else if (MO.isImm()) reg = UCPSReg::KB0 + MO.getImm();

  O << getRegisterName(reg);
  return;
}

void UCPSInstPrinter::printIntrinKE(const MCInst *MI, unsigned OpNo,
                                    raw_ostream &O) const {
  const MCOperand &MO = MI->getOperand(OpNo);
  unsigned reg;

  if (MO.isReg()) reg = MO.getReg();
  else if (MO.isImm()) reg = UCPSReg::KE0 + MO.getImm();

  O << getRegisterName(reg);
  return;
}

void UCPSInstPrinter::printIntrinKS(const MCInst *MI, unsigned OpNo,
                                    raw_ostream &O) const {
  const MCOperand &MO = MI->getOperand(OpNo);
  unsigned reg;

  if (MO.isReg()) reg = MO.getReg();
  else if (MO.isImm()) reg = UCPSReg::KS0 + MO.getImm();

  O << getRegisterName(reg);
  return;
}

void UCPSInstPrinter::printIntrinKI(const MCInst *MI, unsigned OpNo,
                                    raw_ostream &O) const {
  const MCOperand &MO = MI->getOperand(OpNo);
  unsigned reg;

  if (MO.isReg()) reg = MO.getReg();
  else if (MO.isImm()) reg = UCPSReg::KI0 + MO.getImm();

  O << getRegisterName(reg);
  return;
}

// get and set SVR
enum UCPS_SVR {
  SVR0 = 0, // 0b'0
  SVR1 = 1, // 0b'1
};

// encoding for "BH"
enum UCPS_ACCESS_Mode {
  SVR_B = 4, // 0b'10
  SVR_H = 2, // 0b'01
  SVR_W = 0, // 0b'00
};

enum UCPS_SHU {
  SHU0 = 0, // 0b'0
  SHU1 = 1, // 0b'1
};

enum UCPS_T {
  T0 = 0, // 0b'00
  T1 = 1, // 0b'01
  T2 = 2, // 0b'10
  T3 = 3, // 0b'11
};

void UCPSInstPrinter::printIntrinAccessMode(const MCInst *MI, unsigned OpNo,
                                            raw_ostream &O) const {
  const MCOperand &MO = MI->getOperand(OpNo);
  unsigned f = MO.getImm();

  switch (f & 0x3) {
  case SVR_W: break;
  case SVR_H: O << "(H)"; break;
  case SVR_B: O << "(B)"; break;
  default:
    llvm_unreachable("error coding of SVR access modes");
  }
}

void UCPSInstPrinter::printIntrinSVR(const MCInst *MI, unsigned OpNo,
                                     raw_ostream &O) const {
  const MCOperand &MO = MI->getOperand(OpNo);
  unsigned f = MO.getImm();

  switch (f & 0x1) {
  case SVR0: O << "SVR0"; break;
  case SVR1: O << "SVR1"; break;
  default: llvm_unreachable("error coding of SVR");
  }
}

void UCPSInstPrinter::printIntrinT(const MCInst *MI, unsigned OpNo,
                                   raw_ostream &O) const {
  const MCOperand &MO = MI->getOperand(OpNo);
  unsigned f = MO.getImm();

  switch (f & 0x3) {
  case T0: O << "T0"; break;
  case T1: O << "T1"; break;
  case T2: O << "T2"; break;
  case T3: O << "T3"; break;
  default: llvm_unreachable("error coding of T registers");
  }
}

void UCPSInstPrinter::printIntrinSHU(const MCInst *MI, unsigned OpNo,
                                     raw_ostream &O) const {
  const MCOperand &MO = MI->getOperand(OpNo);
  unsigned f = MO.getImm();

  switch (f & 0x1) {
  case SHU0: O << "SHU0"; break;
  case SHU1: O << "SHU1"; break;
  default: llvm_unreachable("error coding of SVR access modes");
  }
}

void UCPSInstPrinter::printIntrinKM(const MCInst *MI, unsigned OpNo,
                                    raw_ostream &O) const {
  const MCOperand &MO = MI->getOperand(OpNo);
  unsigned reg;

  if (MO.isReg()) reg = MO.getReg();
  else if (MO.isImm()) reg = UCPSReg::KM0 + MO.getImm();

  O << getRegisterName(reg);
  return;
}

void UCPSInstPrinter::printIntrinKG(const MCInst *MI, unsigned OpNo,
                                    raw_ostream &O) const {
  const MCOperand &MO = MI->getOperand(OpNo);
  unsigned reg;

  if (MO.isReg()) reg = MO.getReg();
  else if (MO.isImm()) reg = UCPSReg::KG0 + MO.getImm();

  O << getRegisterName(reg);
  return;
}

void UCPSInstPrinter::printIntrinKL(const MCInst *MI, unsigned OpNo,
                                    raw_ostream &O) const {
  const MCOperand &MO = MI->getOperand(OpNo);
  unsigned reg;

  if (MO.isReg()) reg = MO.getReg();
  else if (MO.isImm()) reg = UCPSReg::KL0 + MO.getImm();

  O << getRegisterName(reg);
  return;
}

void UCPSInstPrinter::printIntrinMC(const MCInst *MI, unsigned OpNo,
                                    raw_ostream &O) const {
  const MCOperand &MO = MI->getOperand(OpNo);
  unsigned reg;

  if (MO.isReg()) reg = MO.getReg();
  else if (MO.isImm()) reg = UCPSReg::MC0 + MO.getImm();

  O << getRegisterName(reg);
  return;
}

void UCPSInstPrinter::printAddrJImm(const MCInst *MI, unsigned OpNo,
                                    raw_ostream &O) const {
  const MCOperand &MO0 = MI->getOperand(OpNo);
  const MCOperand &MO1 = MI->getOperand(OpNo + 1);

  O << getRegisterName(MO0.getReg());
  O << " + " << MO1.getImm();
}

void UCPSInstPrinter::printAddrJJ(const MCInst *MI, unsigned OpNo,
                                  raw_ostream &O) const {
  const MCOperand &MO0 = MI->getOperand(OpNo);
  const MCOperand &MO1 = MI->getOperand(OpNo + 1);

  O << getRegisterName(MO0.getReg());
  O << " + " << getRegisterName(MO1.getReg());
}
