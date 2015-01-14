//===-- MSPUMCCodeEmitter.cpp - Convert MSPU code to machine code -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the MSPUMCCodeEmitter class
// and originates from "PPCMCCodeEmitter.cpp"
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "mccodeemitter"
#include "MSPUFixupKinds.h"
#include "MSPUMCTargetDesc.h"
#include "MSPUMCInst.h"
#include "../InstPrinter/MSPUInstPrinter.h"
#include "../MSPU.h"
#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/ADT/APFloat.h"
using namespace llvm;

// I don not know how this static var is utilized. A potential way is to sum up
// the size of .text segment. --yangyy
STATISTIC(MCNumEmitted, "Number of MC instructions emitted");

/// getMSPURegisterCoding - Given the enum value for some register, e.g.
/// MSPUReg::R0, return the number that it corresponds to (e.g. 0).
static unsigned getMSPURegisterEncoding(unsigned RegEnum) {
  switch (RegEnum) {
  case MSPUReg::R0:
    return 0;
  case MSPUReg::R1:
    return 1;
  case MSPUReg::R2:
    return 2;
  case MSPUReg::R3:
    return 3;
  case MSPUReg::R4:
    return 4;
  case MSPUReg::R5:
    return 5;
  case MSPUReg::R6:
    return 6;
  case MSPUReg::R7:
    return 7;
  case MSPUReg::R8:
    return 8;
  case MSPUReg::R9:
    return 9;
  case MSPUReg::R10:
    return 10;
  case MSPUReg::R11:
    return 11;
  case MSPUReg::R12:
    return 12;
  case MSPUReg::R13:
    return 13;
  case MSPUReg::R14:
    return 14;
  case MSPUReg::R15:
    return 15;
  case MSPUReg::R16:
    return 16;
  case MSPUReg::R17:
    return 17;
  case MSPUReg::R18:
    return 18;
  case MSPUReg::R19:
    return 19;
  case MSPUReg::R20:
    return 20;
  case MSPUReg::R21:
    return 21;
  case MSPUReg::R22:
    return 22;
  case MSPUReg::R23:
    return 23;
  case MSPUReg::R24:
    return 24;
  case MSPUReg::R25:
    return 25;
  case MSPUReg::R26:
    return 26;
  case MSPUReg::R27:
    return 27;
  case MSPUReg::R28:
    return 28;
  case MSPUReg::R29:
    return 29;
  case MSPUReg::R30:
    return 30;
  case MSPUReg::R31:
    return 31;

  case MSPUReg::J0:
    return 0;
  case MSPUReg::J1:
    return 1;
  case MSPUReg::J2:
    return 2;
  case MSPUReg::J3:
    return 3;
  case MSPUReg::J4:
    return 4;
  case MSPUReg::J5:
    return 5;
  case MSPUReg::J6:
    return 6;
  case MSPUReg::J7:
    return 7;
  case MSPUReg::J8:
    return 8;
  case MSPUReg::J9:
    return 9;
  case MSPUReg::J10:
    return 10;
  case MSPUReg::J11:
    return 11;
  case MSPUReg::J12:
    return 12;
  case MSPUReg::J13:
    return 13;
  case MSPUReg::J14:
    return 14;
  case MSPUReg::J15:
    return 15;
  case MSPUReg::J16:
    return 16;
  case MSPUReg::J17:
    return 17;
  case MSPUReg::J18:
    return 18;
  case MSPUReg::J19:
    return 19;
  case MSPUReg::J20:
    return 20;
  case MSPUReg::J21:
    return 21;
  case MSPUReg::J22:
    return 22;
  case MSPUReg::J23:
    return 23;
  case MSPUReg::J24:
    return 24;
  case MSPUReg::J25:
    return 25;
  case MSPUReg::J26:
    return 26;
  case MSPUReg::J27:
    return 27;
  case MSPUReg::J28:
    return 28;
  case MSPUReg::J29:
    return 29;
  case MSPUReg::J30:
    return 30;
  case MSPUReg::J31:
    return 31;

  case MSPUReg::D0:
    return 0;
  case MSPUReg::D1:
    return 2;
  case MSPUReg::D2:
    return 4;
  case MSPUReg::D3:
    return 6;
  case MSPUReg::D4:
    return 8;
  case MSPUReg::D5:
    return 10;
  case MSPUReg::D6:
    return 12;
  case MSPUReg::D7:
    return 14;
  case MSPUReg::D8:
    return 16;
  case MSPUReg::D9:
    return 18;
  case MSPUReg::D10:
    return 20;
  case MSPUReg::D11:
    return 22;
  case MSPUReg::D12:
    return 24;
  case MSPUReg::D13:
    return 26;
  case MSPUReg::D14:
    return 28;
  case MSPUReg::D15:
    return 30;

  case MSPUReg::KB0:
    return 0;
  case MSPUReg::KB1:
    return 1;
  case MSPUReg::KB2:
    return 2;
  case MSPUReg::KB3:
    return 3;
  case MSPUReg::KB4:
    return 4;
  case MSPUReg::KB5:
    return 5;
  case MSPUReg::KB6:
    return 6;
  case MSPUReg::KB7:
    return 7;
  case MSPUReg::KB8:
    return 8;
  case MSPUReg::KB9:
    return 9;
  case MSPUReg::KB10:
    return 10;
  case MSPUReg::KB11:
    return 11;
  case MSPUReg::KB12:
    return 12;
  case MSPUReg::KB13:
    return 13;
  case MSPUReg::KB14:
    return 14;
  case MSPUReg::KB15:
    return 15;

  case MSPUReg::KE0:
    return 0;
  case MSPUReg::KE1:
    return 1;
  case MSPUReg::KE2:
    return 2;
  case MSPUReg::KE3:
    return 3;
  case MSPUReg::KE4:
    return 4;
  case MSPUReg::KE5:
    return 5;
  case MSPUReg::KE6:
    return 6;
  case MSPUReg::KE7:
    return 7;
  case MSPUReg::KE8:
    return 8;
  case MSPUReg::KE9:
    return 9;
  case MSPUReg::KE10:
    return 10;
  case MSPUReg::KE11:
    return 11;
  case MSPUReg::KE12:
    return 12;
  case MSPUReg::KE13:
    return 13;
  case MSPUReg::KE14:
    return 14;
  case MSPUReg::KE15:
    return 15;

  case MSPUReg::KI0:
    return 0;
  case MSPUReg::KI1:
    return 1;
  case MSPUReg::KI2:
    return 2;
  case MSPUReg::KI3:
    return 3;
  case MSPUReg::KI4:
    return 4;
  case MSPUReg::KI5:
    return 5;
  case MSPUReg::KI6:
    return 6;
  case MSPUReg::KI7:
    return 7;
  case MSPUReg::KI8:
    return 8;
  case MSPUReg::KI9:
    return 9;
  case MSPUReg::KI10:
    return 10;
  case MSPUReg::KI11:
    return 11;
  case MSPUReg::KI12:
    return 12;
  case MSPUReg::KI13:
    return 13;
  case MSPUReg::KI14:
    return 14;
  case MSPUReg::KI15:
    return 15;

  case MSPUReg::KS0:
    return 0;
  case MSPUReg::KS1:
    return 1;
  case MSPUReg::KS2:
    return 2;
  case MSPUReg::KS3:
    return 3;
  case MSPUReg::KS4:
    return 4;
  case MSPUReg::KS5:
    return 5;
  case MSPUReg::KS6:
    return 6;
  case MSPUReg::KS7:
    return 7;
  case MSPUReg::KS8:
    return 8;
  case MSPUReg::KS9:
    return 9;
  case MSPUReg::KS10:
    return 10;
  case MSPUReg::KS11:
    return 11;
  case MSPUReg::KS12:
    return 12;
  case MSPUReg::KS13:
    return 13;
  case MSPUReg::KS14:
    return 14;
  case MSPUReg::KS15:
    return 15;

  case MSPUReg::KG0:
    return 0;
  case MSPUReg::KG1:
    return 1;
  case MSPUReg::KG2:
    return 2;

  case MSPUReg::KL0:
    return 0;
  case MSPUReg::KL1:
    return 1;
  case MSPUReg::KL2:
    return 2;

  case MSPUReg::KM0:
    return 0;
  case MSPUReg::KM1:
    return 1;
  case MSPUReg::KM2:
    return 2;

  case MSPUReg::SVR0:
    return 0;
  case MSPUReg::SVR1:
    return 1;

  case MSPUReg::SHU0:
    return 0;
  case MSPUReg::SHU1:
    return 1;

  case MSPUReg::T0:
    return 0;
  case MSPUReg::T1:
    return 1;
  case MSPUReg::T2:
    return 2;
  case MSPUReg::T3:
    return 3;

  case MSPUReg::MC0:
    return 0;
  case MSPUReg::MC1:
    return 1;
  case MSPUReg::MC2:
    return 2;
  case MSPUReg::MC3:
    return 3;
  case MSPUReg::MC4:
    return 4;

  default:
    llvm_unreachable("Unknown register number!");
  }
}

namespace {
class MSPUMCCodeEmitter: public MCCodeEmitter {
  MSPUMCCodeEmitter(const MSPUMCCodeEmitter &) LLVM_DELETED_FUNCTION;

  void
  operator=(const MSPUMCCodeEmitter &) LLVM_DELETED_FUNCTION;

private:
  // *mutable* to ignore *const*
  mutable unsigned LineOffset;
  mutable MSPUMCInst ImmExtInst;
  mutable const MCInst *prevInst;

public:
  MSPUMCCodeEmitter(const MCInstrInfo &mcii,
                    const MCSubtargetInfo &sti,
                    MCContext &ctx)
    : LineOffset(0), ImmExtInst(), prevInst(NULL) {
  }

  ~MSPUMCCodeEmitter() {
  }

  /// getMachineOpValue - Return binary encoding of operand. If the machine
  /// operand requires fixup, record the fixup and return zero.
  /// This func is called within tblgen'erated getBinaryCodeForInstr().
  unsigned getMachineOpValue(const MCInst &MI,
                             const MCOperand &MO,
                             SmallVectorImpl<MCFixup> &Fixups,
                             const MCSubtargetInfo &STI) const {
    if (MO.isReg()) {
      unsigned Reg = MO.getReg();
      return getMSPURegisterEncoding(Reg);
    }

    if (MO.isImm()) {
      return static_cast<unsigned>(MO.getImm());
    }

    if (MO.isFPImm()) {
      return static_cast<unsigned>(APFloat(MO.getFPImm()).bitcastToAPInt()
        .getHiBits(32).getLimitedValue());
    }

    if (MO.isExpr()) {
      const MCExpr *Expr = MO.getExpr();
      if (Expr->getKind() == MCExpr::Constant) {
        const MCConstantExpr *constExpr = static_cast<const MCConstantExpr *>(MO
          .getExpr());
        return constExpr->getValue();
      }
    }

    llvm_unreachable("unhandled operand type");
  }

  // for imm instructions: imm assign, imm offset for ld/st, 11-bit imm
  unsigned getSImm11Encoding(const MCInst &MI,
                             unsigned OpNo,
                             SmallVectorImpl<MCFixup> &Fixups,
                             const MCSubtargetInfo &STI) const {
    const MCOperand &MO = MI.getOperand(OpNo);
    int64_t val;

    if (MO.isImm()) val = MO.getImm();
    else if (MO.isExpr()) {
      const MCExpr *Expr = MO.getExpr();
      if (Expr->getKind() != MCExpr::Constant) {
        unsigned opc = MI.getOpcode();
        switch (opc) {
        case MSPUInst::AssignI32:
        case MSPUInst::AssignPtr:
        case MSPUInst::AssignF32:
        case MSPUInst::AssignRRegImm11:
        case MSPUInst::AssignJRegImm11:
          Fixups.push_back(
            MCFixup::Create(LineOffset, Expr, MCFixupKind(MSPU::fixup_MSPU_11_B5)));
          break;

        case MSPUInst::LdImm:
        case MSPUInst::StImm:
          Fixups.push_back(
            MCFixup::Create(LineOffset, Expr, MCFixupKind(MSPU::fixup_MSPU_11_B10)));
          break;

        default:
          llvm_unreachable("invalid instruction opcode");
        }

        return 0;
      }

      const MCConstantExpr *constExpr = static_cast<const MCConstantExpr *>(MO
        .getExpr());
      val = constExpr->getValue();
    } else llvm_unreachable(
      "getSImm11Encoding expects only expression or immediate");

    if (isInt<11>(val)) val &= 0x7FF;
    else {
      ImmExtInst.setOpcode(MSPUInst::ImmExt);
      ImmExtInst.addOperand(MCOperand::CreateImm(val));
      ImmExtInst.setPrev(const_cast<MCInst*>(&MI));
      ImmExtInst.setStart(false);
      ImmExtInst.setEnd(static_cast<const MSPUMCInst *>(&MI)->isEnd());
      val &= 0xF;
    }

    return val;
  }

  // for CallM
  unsigned getUImm17Encoding(const MCInst &MI,
                             unsigned OpNo,
                             SmallVectorImpl<MCFixup> &Fixups,
                             const MCSubtargetInfo &STI) const {
    const MCOperand &MO = MI.getOperand(OpNo);
    int64_t val;

    if (MO.isImm()) val = MO.getImm();
    else if (MO.isExpr()) {
      const MCExpr *Expr = MO.getExpr();
      if (Expr->getKind() != MCExpr::Constant) {
        Fixups.push_back(
          MCFixup::Create(LineOffset, Expr, MCFixupKind(MSPU::fixup_MSPU_17)));
        return 0;
      }
      const MCConstantExpr *constExpr = static_cast<const MCConstantExpr *>(MO
        .getExpr());
      val = constExpr->getValue();
    } else
    llvm_unreachable("getUImm17Encoding expects only expression or immediate");

    return val / MMPUEncodingBytes;
  }

  unsigned getSImm17Encoding(const MCInst &MI,
                             unsigned OpNo,
                             SmallVectorImpl<MCFixup> &Fixups,
                             const MCSubtargetInfo &STI) const {
    const MCOperand &MO = MI.getOperand(OpNo);
    int64_t val;

    if (MO.isImm()) val = MO.getImm();
    else if (MO.isExpr()) {
      const MCExpr *Expr = MO.getExpr();
      if (Expr->getKind() != MCExpr::Constant) {
        Fixups.push_back(
          MCFixup::Create(LineOffset, Expr, MCFixupKind(MSPU::fixup_MSPU_PC17)));
        return 0;
      }

      const MCConstantExpr *constExpr = static_cast<const MCConstantExpr *>(MO
        .getExpr());
      val = constExpr->getValue();
    } else llvm_unreachable(
      "getSImm17Encoding expects only expression or immediate");

    if (isInt<17>(val)) val &= 0x1FFFF;
    else {
      ImmExtInst.setOpcode(MSPUInst::ImmExt);
      ImmExtInst.addOperand(MCOperand::CreateImm(val));
      ImmExtInst.setPrev(const_cast<MCInst*>(&MI));
      ImmExtInst.setStart(false);
      ImmExtInst.setEnd(static_cast<const MSPUMCInst *>(&MI)->isEnd());
      val &= 0xF;
    }

    return val;
  }

  unsigned getSImm28Encoding(const MCInst &MI,
                             unsigned OpNo,
                             SmallVectorImpl<MCFixup> &Fixups,
                             const MCSubtargetInfo &STI) const {
    const MCOperand &MO = MI.getOperand(OpNo);
    int64_t val;

    if (MO.isImm()) val = MO.getImm();
    else if (MO.isExpr()) {
      const MCExpr *Expr = MO.getExpr();
      if (Expr->getKind() != MCExpr::Constant) {
        switch (prevInst->getOpcode()) {
        case MSPUInst::CallImm:
        case MSPUInst::JumpImm:
        case MSPUInst::CallImmCond:
        case MSPUInst::JumpImmCond:
        case MSPUInst::LoopL0:
        case MSPUInst::LoopL1:
          Fixups.push_back(
            MCFixup::Create(LineOffset, Expr,
                            MCFixupKind(MSPU::fixup_MSPU_PC28)));
          break;

        case MSPUInst::AssignI32:
        case MSPUInst::AssignPtr:
        case MSPUInst::AssignF32:
        case MSPUInst::AssignRRegImm11:
        case MSPUInst::AssignJRegImm11:
        case MSPUInst::StImm:
        case MSPUInst::LdImm:
          Fixups.push_back(
            MCFixup::Create(LineOffset, Expr,
                            MCFixupKind(MSPU::fixup_MSPU_28)));
          break;

        default:
          break;
        }

        return 0;
      }

      const MCConstantExpr *constExpr = static_cast<const MCConstantExpr *>(MO
        .getExpr());
      val = constExpr->getValue();
    } else llvm_unreachable(
      "getSImm17Encoding expects only expression or immediate");

    return val >> 4;
  }

  unsigned getAddrJJEncoding(const MCInst &MI,
                             unsigned OpNo,
                             SmallVectorImpl<MCFixup> &Fixups,
                             const MCSubtargetInfo &STI) const {
    unsigned bits;
    // [J(s0) + J(s1)]
    const MCOperand &MO0 = MI.getOperand(OpNo);
    assert(MO0.isReg());
    bits = getMSPURegisterEncoding(MO0.getReg());

    const MCOperand &MO1 = MI.getOperand(OpNo + 1);
    assert(MO1.isReg());
    bits |= (getMSPURegisterEncoding(MO1.getReg()) << 5);
    return bits;
  }

  unsigned getRJRegEncoding(const MCInst &MI,
                            unsigned OpNo,
                            SmallVectorImpl<MCFixup> &Fixups,
                            const MCSubtargetInfo &STI) const {
    const MCOperand &MO = MI.getOperand(OpNo);
    assert(MO.isReg());
    unsigned Reg = MO.getReg();
    unsigned code = getMSPURegisterEncoding(Reg);
    if (MSPUReg::J0 <= Reg && Reg <= MSPUReg::J31) code |= (1 << 5);
    else if (Reg < MSPUReg::R0 && MSPUReg::R31 < Reg)
    llvm_unreachable("Invalid RJReg code, beyond boundary");
    return code;
  }

  unsigned getXYFlagEncoding(const MCInst &MI,
                             unsigned OpNo,
                             SmallVectorImpl<MCFixup> &Fixups,
                             const MCSubtargetInfo &STI) const {
    const MCOperand &MO = MI.getOperand(OpNo);
    assert(MO.isImm());
    unsigned code = MO.getImm();
    switch (code) {
    case 1:
    case 2:
    case 3:
      code -= 1;
      break;
    case 0:
      break;

    default:
      ;
      llvm_unreachable("Invalid encoding for X/Y flags");
    }

    return code;
  }

  unsigned getRDRegEncoding(const MCInst &MI,
                            unsigned OpNo,
                            SmallVectorImpl<MCFixup> &Fixups,
                            const MCSubtargetInfo &STI) const {
    const MCOperand &MO = MI.getOperand(OpNo);
    assert(MO.isReg());
    unsigned Reg = MO.getReg();
    unsigned code = getMSPURegisterEncoding(Reg);
    if (MSPUReg::D0 <= Reg && Reg <= MSPUReg::D15) code |= (1 << 5);
    else if (Reg < MSPUReg::R0 && MSPUReg::R31 < Reg)
    llvm_unreachable("Invalid RJReg code, beyond boundary");
    return code;
  }

  unsigned getKRegEncoding(const MCInst &MI,
                           unsigned OpNo,
                           SmallVectorImpl<MCFixup> &Fixups,
                           const MCSubtargetInfo &STI) const {
    const MCOperand &MO = MI.getOperand(OpNo);
    unsigned Reg = MO.getReg();
    unsigned code = getMSPURegisterEncoding(Reg);

    if (MSPUReg::KB0 <= Reg && Reg <= MSPUReg::KB15) code |= (0 << 4);
    else if (MSPUReg::KE0 <= Reg && Reg <= MSPUReg::KE15) code |= (1 << 4);
    else if (MSPUReg::KS0 <= Reg && Reg <= MSPUReg::KS15) code |= (2 << 4);
    else if (MSPUReg::KI0 <= Reg && Reg <= MSPUReg::KI15) code |= (3 << 4);

    else if (MSPUReg::KG0 <= Reg && Reg <= MSPUReg::KG2) {
      code <<= 2;
      code |= (5 << 4);
    } else if (MSPUReg::KL0 <= Reg && Reg <= MSPUReg::KL2) {
      code <<= 2;
      code |= (6 << 4);
    } else
    llvm_unreachable("Invalid K Register code, beyond boundary");

    return code;
  }

  unsigned getIntrinKREGEncoding(const MCInst &MI,
                                 unsigned OpNo,
                                 SmallVectorImpl<MCFixup> &Fixups,
                                 const MCSubtargetInfo &STI) const {
    const MCOperand &MO = MI.getOperand(OpNo);
    unsigned code;
    if (MO.isImm()) {

      code = MO.getImm();

      if ((code & (~0xF)) == (MSPUInstPrinter::KM0 & (~0xF))) {

        switch (MI.getOpcode()) {
        case MSPUInst::__SetHiKM:
        case MSPUInst::__GetHiKM:
          code |= 2;    // set flag (H)
          break;
        default:
          code |= 1;    // set flag (L)
        }

      }

      return code;
    }

    llvm_unreachable("only KM reg is expected here.");
  }

  unsigned getLdStRegEncoding(const MCInst &MI,
                              unsigned OpNo,
                              SmallVectorImpl<MCFixup> &Fixups,
                              const MCSubtargetInfo &STI) const {
    const MCOperand &MO = MI.getOperand(OpNo);
    unsigned Reg = MO.getReg();
    unsigned code = getMSPURegisterEncoding(Reg);

    if (MSPUReg::R0 <= Reg && Reg <= MSPUReg::R31) code |= (0 << 5);    // 00
    else if (MSPUReg::D0 <= Reg && Reg <= MSPUReg::D15) code |= (1 << 5);    // 01
    else if (MSPUReg::J0 <= Reg && Reg <= MSPUReg::J31) code |= (2 << 5);    // 10
    else
    llvm_unreachable("Invalid Register code for load/store, beyond boundary");

    return code;
  }

  unsigned getAddrJImmEncoding(const MCInst &MI,
                               unsigned OpNo,
                               SmallVectorImpl<MCFixup> &Fixups,
                               const MCSubtargetInfo &STI) const {
    // J + Imm
    const MCOperand &MO0 = MI.getOperand(OpNo);
    assert(MO0.isReg());
    unsigned regCode = getMSPURegisterEncoding(MO0.getReg());
    unsigned addr = getSImm11Encoding(MI, OpNo + 1, Fixups, STI);
    return (addr << 5) | regCode;
  }

  // getBinaryCodeForInstr - TableGen'erated function for getting the
  // binary encoding for an instruction.
  uint64_t
  getBinaryCodeForInstr(const MCInst &MI,
                        SmallVectorImpl<MCFixup> &Fixups,
                        const MCSubtargetInfo &STI) const;

  void EncodeInstruction(const MCInst &MI,
                         raw_ostream &OS,
                         SmallVectorImpl<MCFixup> &Fixups,
                         const MCSubtargetInfo &STI) const override
                         {
    // this is helpful if we need to encode imm-ext
    LineOffset = 0;
    ImmExtInst.clear();
    const MCInst *MSMI = &MI;

    do {
      // at the beginning we encode *MI* ordinarily.
      uint64_t codes = getBinaryCodeForInstr(*MSMI, Fixups, STI);

      prevInst = MSMI;
      if (ImmExtInst.getNumOperands() &&
          ImmExtInst.getOpcode() == MSPUInst::ImmExt) {
        MSMI = &ImmExtInst;
        for (unsigned i = 0; i < 4; ++i) {
          OS << (unsigned char) codes;
          codes >>= 8;
        }
        ++MCNumEmitted;
        LineOffset += 4;
        codes = getBinaryCodeForInstr(*MSMI, Fixups, STI);

        // reset *ImmExtInst*
        ImmExtInst.setOpcode(MSPUInst::NOP);
        ImmExtInst.clear();
      }
      if (prevInst->getNumOperands() &&
          prevInst->getOperand(prevInst->getNumOperands() - 1).isInst())
        MSMI = prevInst->getOperand(prevInst->getNumOperands() - 1).getInst();
      else {
        codes |= (1 << 31);
        MSMI = NULL;
      }

      for (unsigned i = 0; i < 4; ++i) {
        OS << (unsigned char) codes;
        codes >>= 8;
      }
      ++MCNumEmitted;
      LineOffset += 4;
    } while (MSMI);
  }    // end EncodeInstruction()

};
// end MSPUMCCodeEmitter
}// end anonymous namespace

MCCodeEmitter *
llvm::createMSPUMCCodeEmitter(const MCInstrInfo &MCII,
                              const MCRegisterInfo &MRI,
                              const MCSubtargetInfo &STI,
                              MCContext &Ctx) {
  return new MSPUMCCodeEmitter(MCII, STI, Ctx);
}

#include "MSPUGenMCCodeEmitter.inc"
