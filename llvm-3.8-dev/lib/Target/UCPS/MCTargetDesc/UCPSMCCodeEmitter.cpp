//===-- UCPSMCCodeEmitter.cpp - Convert UCPS code to machine code -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the UCPSMCCodeEmitter class
// and originates from "PPCMCCodeEmitter.cpp"
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "mccodeemitter"
#include "UCPSFixupKinds.h"
#include "UCPSMCTargetDesc.h"
#include "../InstPrinter/UCPSInstPrinter.h"
#include "../UCPS.h"
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

/// getUCPSRegisterCoding - Given the enum value for some register, e.g.
/// UCPSReg::R0, return the number that it corresponds to (e.g. 0).
static unsigned getUCPSRegisterEncoding(unsigned RegEnum) {
  switch (RegEnum) {
  case UCPSReg::R0:
    return 0;
  case UCPSReg::R1:
    return 1;
  case UCPSReg::R2:
    return 2;
  case UCPSReg::R3:
    return 3;
  case UCPSReg::R4:
    return 4;
  case UCPSReg::R5:
    return 5;
  case UCPSReg::R6:
    return 6;
  case UCPSReg::R7:
    return 7;
  case UCPSReg::R8:
    return 8;
  case UCPSReg::R9:
    return 9;
  case UCPSReg::R10:
    return 10;
  case UCPSReg::R11:
    return 11;
  case UCPSReg::R12:
    return 12;
  case UCPSReg::R13:
    return 13;
  case UCPSReg::R14:
    return 14;
  case UCPSReg::R15:
    return 15;
  case UCPSReg::R16:
    return 16;
  case UCPSReg::R17:
    return 17;
  case UCPSReg::R18:
    return 18;
  case UCPSReg::R19:
    return 19;
  case UCPSReg::R20:
    return 20;
  case UCPSReg::R21:
    return 21;
  case UCPSReg::R22:
    return 22;
  case UCPSReg::R23:
    return 23;
  case UCPSReg::R24:
    return 24;
  case UCPSReg::R25:
    return 25;
  case UCPSReg::R26:
    return 26;
  case UCPSReg::R27:
    return 27;
  case UCPSReg::R28:
    return 28;
  case UCPSReg::R29:
    return 29;
  case UCPSReg::R30:
    return 30;
  case UCPSReg::R31:
    return 31;

  case UCPSReg::J0:
    return 0;
  case UCPSReg::J1:
    return 1;
  case UCPSReg::J2:
    return 2;
  case UCPSReg::J3:
    return 3;
  case UCPSReg::J4:
    return 4;
  case UCPSReg::J5:
    return 5;
  case UCPSReg::J6:
    return 6;
  case UCPSReg::J7:
    return 7;
  case UCPSReg::J8:
    return 8;
  case UCPSReg::J9:
    return 9;
  case UCPSReg::J10:
    return 10;
  case UCPSReg::J11:
    return 11;
  case UCPSReg::J12:
    return 12;
  case UCPSReg::J13:
    return 13;
  case UCPSReg::J14:
    return 14;
  case UCPSReg::J15:
    return 15;
  case UCPSReg::J16:
    return 16;
  case UCPSReg::J17:
    return 17;
  case UCPSReg::J18:
    return 18;
  case UCPSReg::J19:
    return 19;
  case UCPSReg::J20:
    return 20;
  case UCPSReg::J21:
    return 21;
  case UCPSReg::J22:
    return 22;
  case UCPSReg::J23:
    return 23;
  case UCPSReg::J24:
    return 24;
  case UCPSReg::J25:
    return 25;
  case UCPSReg::J26:
    return 26;
  case UCPSReg::J27:
    return 27;
  case UCPSReg::J28:
    return 28;
  case UCPSReg::J29:
    return 29;
  case UCPSReg::J30:
    return 30;
  case UCPSReg::J31:
    return 31;

//   case UCPSReg::D0:
//     return 0;
//   case UCPSReg::D1:
//     return 2;
//   case UCPSReg::D2:
//     return 4;
//   case UCPSReg::D3:
//     return 6;
//   case UCPSReg::D4:
//     return 8;
//   case UCPSReg::D5:
//     return 10;
//   case UCPSReg::D6:
//     return 12;
//   case UCPSReg::D7:
//     return 14;
//   case UCPSReg::D8:
//     return 16;
//   case UCPSReg::D9:
//     return 18;
//   case UCPSReg::D10:
//     return 20;
//   case UCPSReg::D11:
//     return 22;
//   case UCPSReg::D12:
//     return 24;
//   case UCPSReg::D13:
//     return 26;
//   case UCPSReg::D14:
//     return 28;
//   case UCPSReg::D15:
//     return 30;
// 
//   case UCPSReg::KB0:
//     return 0;
//   case UCPSReg::KB1:
//     return 1;
//   case UCPSReg::KB2:
//     return 2;
//   case UCPSReg::KB3:
//     return 3;
//   case UCPSReg::KB4:
//     return 4;
//   case UCPSReg::KB5:
//     return 5;
//   case UCPSReg::KB6:
//     return 6;
//   case UCPSReg::KB7:
//     return 7;
//   case UCPSReg::KB8:
//     return 8;
//   case UCPSReg::KB9:
//     return 9;
//   case UCPSReg::KB10:
//     return 10;
//   case UCPSReg::KB11:
//     return 11;
//   case UCPSReg::KB12:
//     return 12;
//   case UCPSReg::KB13:
//     return 13;
//   case UCPSReg::KB14:
//     return 14;
//   case UCPSReg::KB15:
//     return 15;
// 
//   case UCPSReg::KE0:
//     return 0;
//   case UCPSReg::KE1:
//     return 1;
//   case UCPSReg::KE2:
//     return 2;
//   case UCPSReg::KE3:
//     return 3;
//   case UCPSReg::KE4:
//     return 4;
//   case UCPSReg::KE5:
//     return 5;
//   case UCPSReg::KE6:
//     return 6;
//   case UCPSReg::KE7:
//     return 7;
//   case UCPSReg::KE8:
//     return 8;
//   case UCPSReg::KE9:
//     return 9;
//   case UCPSReg::KE10:
//     return 10;
//   case UCPSReg::KE11:
//     return 11;
//   case UCPSReg::KE12:
//     return 12;
//   case UCPSReg::KE13:
//     return 13;
//   case UCPSReg::KE14:
//     return 14;
//   case UCPSReg::KE15:
//     return 15;
// 
//   case UCPSReg::KI0:
//     return 0;
//   case UCPSReg::KI1:
//     return 1;
//   case UCPSReg::KI2:
//     return 2;
//   case UCPSReg::KI3:
//     return 3;
//   case UCPSReg::KI4:
//     return 4;
//   case UCPSReg::KI5:
//     return 5;
//   case UCPSReg::KI6:
//     return 6;
//   case UCPSReg::KI7:
//     return 7;
//   case UCPSReg::KI8:
//     return 8;
//   case UCPSReg::KI9:
//     return 9;
//   case UCPSReg::KI10:
//     return 10;
//   case UCPSReg::KI11:
//     return 11;
//   case UCPSReg::KI12:
//     return 12;
//   case UCPSReg::KI13:
//     return 13;
//   case UCPSReg::KI14:
//     return 14;
//   case UCPSReg::KI15:
//     return 15;
// 
//   case UCPSReg::KS0:
//     return 0;
//   case UCPSReg::KS1:
//     return 1;
//   case UCPSReg::KS2:
//     return 2;
//   case UCPSReg::KS3:
//     return 3;
//   case UCPSReg::KS4:
//     return 4;
//   case UCPSReg::KS5:
//     return 5;
//   case UCPSReg::KS6:
//     return 6;
//   case UCPSReg::KS7:
//     return 7;
//   case UCPSReg::KS8:
//     return 8;
//   case UCPSReg::KS9:
//     return 9;
//   case UCPSReg::KS10:
//     return 10;
//   case UCPSReg::KS11:
//     return 11;
//   case UCPSReg::KS12:
//     return 12;
//   case UCPSReg::KS13:
//     return 13;
//   case UCPSReg::KS14:
//     return 14;
//   case UCPSReg::KS15:
//     return 15;
// 
//   case UCPSReg::KG0:
//     return 0;
//   case UCPSReg::KG1:
//     return 1;
//   case UCPSReg::KG2:
//     return 2;
// 
//   case UCPSReg::KL0:
//     return 0;
//   case UCPSReg::KL1:
//     return 1;
//   case UCPSReg::KL2:
//     return 2;
// 
//   case UCPSReg::KM0:
//     return 0;
//   case UCPSReg::KM1:
//     return 1;
//   case UCPSReg::KM2:
//     return 2;
// 
  case UCPSReg::SVR0:
    return 0;
  case UCPSReg::SVR1:
    return 1;
// 
//   case UCPSReg::SHU0:
//     return 0;
//   case UCPSReg::SHU1:
//     return 1;
// 
//   case UCPSReg::T0:
//     return 0;
//   case UCPSReg::T1:
//     return 1;
//   case UCPSReg::T2:
//     return 2;
//   case UCPSReg::T3:
//     return 3;
// 
//   case UCPSReg::MC0:
//     return 0;
//   case UCPSReg::MC1:
//     return 1;
//   case UCPSReg::MC2:
//     return 2;
//   case UCPSReg::MC3:
//     return 3;
//   case UCPSReg::MC4:
//     return 4;

  default:
    llvm_unreachable("Unknown register number!");
  }
}

namespace {
class UCPSMCCodeEmitter: public MCCodeEmitter {

private:
  // *mutable* to ignore *const*
  mutable unsigned LineOffset;
  mutable MCInst ImmExtInst;
  mutable const MCInst *InstNeedsExt;

public:
  UCPSMCCodeEmitter(const MCInstrInfo &mcii,
                    MCContext &ctx)
    : LineOffset(0), ImmExtInst(), InstNeedsExt(NULL) {
  }

  ~UCPSMCCodeEmitter() {
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
      return getUCPSRegisterEncoding(Reg);
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
//   unsigned getSImm11Encoding(const MCInst &MI,
//                              unsigned OpNo,
//                              SmallVectorImpl<MCFixup> &Fixups,
//                              const MCSubtargetInfo &STI) const {
//     const MCOperand &MO = MI.getOperand(OpNo);
//     int64_t val;
// 
//     if (MO.isImm()) val = MO.getImm();
//     else if (MO.isExpr()) {
//       const MCExpr *Expr = MO.getExpr();
//       if (Expr->getKind() != MCExpr::Constant) {
//         unsigned opc = MI.getOpcode();
//         switch (opc) {
//         case UCPSInst::AssignI32:
//         case UCPSInst::AssignPtr:
//         case UCPSInst::AssignF32:
//         case UCPSInst::AssignRRegImm11:
//         case UCPSInst::AssignJRegImm11:
//           Fixups.push_back(
//             MCFixup::create(LineOffset, Expr, MCFixupKind(UCPS::fixup_UCPS_11_B5)));
//           break;
// 
//         case UCPSInst::LdImm:
//         case UCPSInst::StImm:
//           Fixups.push_back(
//             MCFixup::create(LineOffset, Expr, MCFixupKind(UCPS::fixup_UCPS_11_B10)));
//           break;
// 
//         default:
//           llvm_unreachable("invalid instruction opcode");
//         }
//         InstNeedsExt = &MI;
// 
//         return 0;
//       }
// 
//       const MCConstantExpr *constExpr = static_cast<const MCConstantExpr *>(MO
//         .getExpr());
//       val = constExpr->getValue();
//     } else llvm_unreachable(
//       "getSImm11Encoding expects only expression or immediate");
// 
//     if (isInt<11>(val)) val &= 0x7FF;
//     else {
//       InstNeedsExt = &MI;
//       ImmExtInst.setOpcode(UCPSInst::ImmExt);
//       ImmExtInst.addOperand(MCOperand::createImm(val));
//       val &= 0xF;
//     }
// 
//     return val;
//   }

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
          MCFixup::create(LineOffset, Expr, MCFixupKind(UCPS::fixup_UCPS_17)));
        return 0;
      }
      const MCConstantExpr *constExpr = static_cast<const MCConstantExpr *>(MO
        .getExpr());
      val = constExpr->getValue();
    } else
    llvm_unreachable("getUImm17Encoding expects only expression or immediate");

    return val / MSPUEncodingBytes;
  }

  // dcx
    unsigned getUImm12Encoding(const MCInst &MI,
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
          MCFixup::create(LineOffset, Expr, MCFixupKind(UCPS::fixup_UCPS_12)));
        return 0;
      }
      const MCConstantExpr *constExpr = static_cast<const MCConstantExpr *>(MO
        .getExpr());
      val = constExpr->getValue();
    } else
    llvm_unreachable("getUImm12Encoding expects only expression or immediate");

    return val / MSPUEncodingBytes;
  }
  
    unsigned getSImm18Encoding(const MCInst &MI,
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
          MCFixup::create(LineOffset, Expr, MCFixupKind(UCPS::fixup_UCPS_PC18)));
        InstNeedsExt = &MI;
        return 0;
      }

      const MCConstantExpr *constExpr = static_cast<const MCConstantExpr *>(MO
        .getExpr());
      val = constExpr->getValue();
    } else llvm_unreachable(
      "getSImm18Encoding expects only expression or immediate");

    if (isInt<18>(val)) val &= 0x3FFFF;
    else {
      InstNeedsExt = &MI;
      //ImmExtInst.setOpcode(UCPSInst::ImmExt);
      ImmExtInst.addOperand(MCOperand::createImm(val));
      val &= 0xF;
    }

    return val;
  }

  
//   unsigned getSImm17Encoding(const MCInst &MI,
//                              unsigned OpNo,
//                              SmallVectorImpl<MCFixup> &Fixups,
//                              const MCSubtargetInfo &STI) const {
//     const MCOperand &MO = MI.getOperand(OpNo);
//     int64_t val;
// 
//     if (MO.isImm()) val = MO.getImm();
//     else if (MO.isExpr()) {
//       const MCExpr *Expr = MO.getExpr();
//       if (Expr->getKind() != MCExpr::Constant) {
//         Fixups.push_back(
//           MCFixup::create(LineOffset, Expr, MCFixupKind(UCPS::fixup_UCPS_PC17)));
//         InstNeedsExt = &MI;
//         return 0;
//       }
// 
//       const MCConstantExpr *constExpr = static_cast<const MCConstantExpr *>(MO
//         .getExpr());
//       val = constExpr->getValue();
//     } else llvm_unreachable(
//       "getSImm17Encoding expects only expression or immediate");
// 
//     if (isInt<17>(val)) val &= 0x1FFFF;
//     else {
//       InstNeedsExt = &MI;
//       ImmExtInst.setOpcode(UCPSInst::ImmExt);
//       ImmExtInst.addOperand(MCOperand::createImm(val));
//       val &= 0xF;
//     }
// 
//     return val;
//   }

//   unsigned getSImm28Encoding(const MCInst &MI,
//                              unsigned OpNo,
//                              SmallVectorImpl<MCFixup> &Fixups,
//                              const MCSubtargetInfo &STI) const {
//     const MCOperand &MO = MI.getOperand(OpNo);
//     int64_t val;
// 
//     if (MO.isImm()) val = MO.getImm();
//     else if (MO.isExpr()) {
//       const MCExpr *Expr = MO.getExpr();
//       if (Expr->getKind() != MCExpr::Constant) {
//         switch (InstNeedsExt->getOpcode()) {
//         case UCPSInst::CallImm:
//         case UCPSInst::JumpImm:
//         case UCPSInst::CallImmCond:
//         case UCPSInst::JumpImmCond:
//         case UCPSInst::LoopL0:
//         case UCPSInst::LoopL1:
//           Fixups.push_back(
//             MCFixup::create(LineOffset, Expr,
//                             MCFixupKind(UCPS::fixup_UCPS_PC28)));
//           break;
// 
//         case UCPSInst::AssignI32:
//         case UCPSInst::AssignPtr:
//         case UCPSInst::AssignF32:
//         case UCPSInst::AssignRRegImm11:
//         case UCPSInst::AssignJRegImm11:
//         case UCPSInst::StImm:
//         case UCPSInst::LdImm:
//           Fixups.push_back(
//             MCFixup::create(LineOffset, Expr,
//                             MCFixupKind(UCPS::fixup_UCPS_28)));
//           break;
// 
//         default:
//           break;
//         }
// 
//         return 0;
//       }
// 
//       const MCConstantExpr *constExpr = static_cast<const MCConstantExpr *>(MO
//         .getExpr());
//       val = constExpr->getValue();
//     } else llvm_unreachable(
//       "getSImm17Encoding expects only expression or immediate");
// 
//     return val >> 4;
//   }

  unsigned getAddrJJEncoding(const MCInst &MI,
                             unsigned OpNo,
                             SmallVectorImpl<MCFixup> &Fixups,
                             const MCSubtargetInfo &STI) const {
    unsigned bits;
    // [J(s0) + J(s1)]
    const MCOperand &MO0 = MI.getOperand(OpNo);
    assert(MO0.isReg());
    bits = getUCPSRegisterEncoding(MO0.getReg());

    const MCOperand &MO1 = MI.getOperand(OpNo + 1);
    assert(MO1.isReg());
    bits |= (getUCPSRegisterEncoding(MO1.getReg()) << 5);
    return bits;
  }

  unsigned getRJRegEncoding(const MCInst &MI,
                            unsigned OpNo,
                            SmallVectorImpl<MCFixup> &Fixups,
                            const MCSubtargetInfo &STI) const {
    const MCOperand &MO = MI.getOperand(OpNo);
    assert(MO.isReg());
    unsigned Reg = MO.getReg();
    unsigned code = getUCPSRegisterEncoding(Reg);
    if (UCPSReg::J0 <= Reg && Reg <= UCPSReg::J31) code |= (1 << 5);
    else if (Reg < UCPSReg::R0 && UCPSReg::R31 < Reg)
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

//   unsigned getRDRegEncoding(const MCInst &MI,
//                             unsigned OpNo,
//                             SmallVectorImpl<MCFixup> &Fixups,
//                             const MCSubtargetInfo &STI) const {
//     const MCOperand &MO = MI.getOperand(OpNo);
//     assert(MO.isReg());
//     unsigned Reg = MO.getReg();
//     unsigned code = getUCPSRegisterEncoding(Reg);
//     if (UCPSReg::D0 <= Reg && Reg <= UCPSReg::D15) code |= (1 << 5);
//     else if (Reg < UCPSReg::R0 && UCPSReg::R31 < Reg)
//     llvm_unreachable("Invalid RJReg code, beyond boundary");
//     return code;
//   }

//   unsigned getKRegEncoding(const MCInst &MI,
//                            unsigned OpNo,
//                            SmallVectorImpl<MCFixup> &Fixups,
//                            const MCSubtargetInfo &STI) const {
//     const MCOperand &MO = MI.getOperand(OpNo);
//     unsigned Reg = MO.getReg();
//     unsigned code = getUCPSRegisterEncoding(Reg);
// 
//     if (UCPSReg::KB0 <= Reg && Reg <= UCPSReg::KB15) code |= (0 << 4);
//     else if (UCPSReg::KE0 <= Reg && Reg <= UCPSReg::KE15) code |= (1 << 4);
//     else if (UCPSReg::KS0 <= Reg && Reg <= UCPSReg::KS15) code |= (2 << 4);
//     else if (UCPSReg::KI0 <= Reg && Reg <= UCPSReg::KI15) code |= (3 << 4);
// 
//     else if (UCPSReg::KG0 <= Reg && Reg <= UCPSReg::KG2) {
//       code <<= 2;
//       code |= (5 << 4);
//     } else if (UCPSReg::KL0 <= Reg && Reg <= UCPSReg::KL2) {
//       code <<= 2;
//       code |= (6 << 4);
//     } else
//     llvm_unreachable("Invalid K Register code, beyond boundary");
// 
//     return code;
//   }

//   unsigned getIntrinKREGEncoding(const MCInst &MI,
//                                  unsigned OpNo,
//                                  SmallVectorImpl<MCFixup> &Fixups,
//                                  const MCSubtargetInfo &STI) const {
//     const MCOperand &MO = MI.getOperand(OpNo);
//     unsigned code;
//     if (MO.isImm()) {
// 
//       code = MO.getImm();
// 
//       if ((code & (~0xF)) == (UCPSInstPrinter::KM0 & (~0xF))) {
// 
//         switch (MI.getOpcode()) {
//         case UCPSInst::__SetHiKM:
//         case UCPSInst::__GetHiKM:
//           code |= 2;    // set flag (H)
//           break;
//         default:
//           code |= 1;    // set flag (L)
//         }
// 
//       }
// 
//       return code;
//     }
// 
//     llvm_unreachable("only KM reg is expected here.");
//   }

//   unsigned getLdStRegEncoding(const MCInst &MI,
//                               unsigned OpNo,
//                               SmallVectorImpl<MCFixup> &Fixups,
//                               const MCSubtargetInfo &STI) const {
//     const MCOperand &MO = MI.getOperand(OpNo);
//     unsigned Reg = MO.getReg();
//     unsigned code = getUCPSRegisterEncoding(Reg);
// 
//     if (UCPSReg::R0 <= Reg && Reg <= UCPSReg::R31) code |= (0 << 5);    // 00
//     else if (UCPSReg::D0 <= Reg && Reg <= UCPSReg::D15) code |= (1 << 5);    // 01
//     else if (UCPSReg::J0 <= Reg && Reg <= UCPSReg::J31) code |= (2 << 5);    // 10
//     else
//     llvm_unreachable("Invalid Register code for load/store, beyond boundary");
// 
//     return code;
//   }

//   unsigned getAddrJImmEncoding(const MCInst &MI,
//                                unsigned OpNo,
//                                SmallVectorImpl<MCFixup> &Fixups,
//                                const MCSubtargetInfo &STI) const {
//     // J + Imm
//     const MCOperand &MO0 = MI.getOperand(OpNo);
//     assert(MO0.isReg());
//     unsigned regCode = getUCPSRegisterEncoding(MO0.getReg());
//     unsigned addr = getSImm11Encoding(MI, OpNo + 1, Fixups, STI);
//     return (addr << 5) | regCode;
//   }

  // getBinaryCodeForInstr - TableGen'erated function for getting the
  // binary encoding for an instruction.
  uint64_t
  getBinaryCodeForInstr(const MCInst &MI,
                        SmallVectorImpl<MCFixup> &Fixups,
                        const MCSubtargetInfo &STI) const;

  void encodeInstruction(const MCInst &MI,
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

//       if (ImmExtInst.getNumOperands() &&
//           ImmExtInst.getOpcode() == UCPSInst::ImmExt) {
//         for (unsigned i = 0; i < 4; ++i) {
//           OS << (unsigned char) codes;
//           codes >>= 8;
//         }
//         ++MCNumEmitted;
//         LineOffset += 4;
//         codes = getBinaryCodeForInstr(ImmExtInst, Fixups, STI);
// 
//         // reset *ImmExtInst*
//         ImmExtInst.setOpcode(UCPSInst::NOP);
//         ImmExtInst.clear();
//       }
      if (MSMI->getNumOperands() &&
          MSMI->getOperand(MSMI->getNumOperands() - 1).isInst())
        MSMI = MSMI->getOperand(MSMI->getNumOperands() - 1).getInst();
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
// end UCPSMCCodeEmitter
}// end anonymous namespace

MCCodeEmitter *
llvm::createUCPSMCCodeEmitter(const MCInstrInfo &MCII,
                              const MCRegisterInfo &MRI,
                              MCContext &Ctx) {
  return new UCPSMCCodeEmitter(MCII, Ctx);
}

#include "UCPSGenMCCodeEmitter.inc"
