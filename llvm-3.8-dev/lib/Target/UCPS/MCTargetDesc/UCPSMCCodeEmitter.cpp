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
#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/ADT/APFloat.h"
#include "UCPSInsnLine.h"
#include "../MCTargetDesc/UCPSMCTargetDesc.h"

using namespace llvm;

STATISTIC(MCNumEmitted, "Number of MC instructions emitted");
STATISTIC(MCLineNumEmitted, "Number of MC instruction lines emitted");

namespace {
class UCPSMCCodeEmitter: public MCCodeEmitter
{
  const MCContext &CTX;

public:
  UCPSMCCodeEmitter(MCContext &ctx)
    : CTX(ctx) {
  }

  ~UCPSMCCodeEmitter() {}

  /// getMachineOpValue - Return binary encoding of operand. If the machine
  /// operand requires fixup, record the fixup and return zero.
  /// This func is called within tblgen'erated getBinaryCodeForInstr().
  unsigned getMachineOpValue(const MCInst &MI, const MCOperand &MO,
                             SmallVectorImpl<MCFixup> &Fixups,
                             const MCSubtargetInfo &STI) const;

  // getBinaryCodeForInstr - TableGen'erated function for getting the
  // binary encoding for an instruction.
  uint64_t getBinaryCodeForInstr(const MCInst &MI,
                                 SmallVectorImpl<MCFixup> &Fixups,
                                 const MCSubtargetInfo &STI) const;
  
  unsigned getMACCTIEncoding(const MCInst &MI, unsigned OpNo,
                             SmallVectorImpl<MCFixup> &Fixups,
                             const MCSubtargetInfo &STI) const;
  unsigned getMACCTEncoding(const MCInst &MI, unsigned OpNo,
                               SmallVectorImpl<MCFixup> &Fixups,
                               const MCSubtargetInfo &STI) const;
  unsigned getBIUTEncoding(const MCInst &MI, unsigned OpNo,
                                 SmallVectorImpl<MCFixup> &Fixups,
                                 const MCSubtargetInfo &STI) const;
				 
  unsigned getBIU0DestTEEncoding(const MCInst &MI, unsigned OpNo,
                                 SmallVectorImpl<MCFixup> &Fixups,
                                 const MCSubtargetInfo &STI) const;
  unsigned getBIU1DestTEEncoding(const MCInst &MI, unsigned OpNo,
                                 SmallVectorImpl<MCFixup> &Fixups,
                                 const MCSubtargetInfo &STI) const;
  unsigned getBIU2DestTEEncoding(const MCInst &MI, unsigned OpNo,
                                 SmallVectorImpl<MCFixup> &Fixups,
                                 const MCSubtargetInfo &STI) const;
				 
  unsigned getMR0DestTEncoding(const MCInst &MI, unsigned OpNo,
                               SmallVectorImpl<MCFixup> &Fixups,
                               const MCSubtargetInfo &STI) const;
  
  unsigned getMR1DestTEncoding(const MCInst &MI, unsigned OpNo,
                                 SmallVectorImpl<MCFixup> &Fixups,
                                 const MCSubtargetInfo &STI) const;

  unsigned getMR2DestTEncoding(const MCInst &MI, unsigned OpNo,
                                 SmallVectorImpl<MCFixup> &Fixups,
                                 const MCSubtargetInfo &STI) const;

  unsigned getMR3DestTEncoding(const MCInst &MI, unsigned OpNo,
                                 SmallVectorImpl<MCFixup> &Fixups,
                                 const MCSubtargetInfo &STI) const;

  unsigned getMR4DestTEncoding(const MCInst &MI, unsigned OpNo,
                                 SmallVectorImpl<MCFixup> &Fixups,
                                 const MCSubtargetInfo &STI) const;

  unsigned getMR5DestTEncoding(const MCInst &MI, unsigned OpNo,
                                 SmallVectorImpl<MCFixup> &Fixups,
                                 const MCSubtargetInfo &STI) const;

  unsigned getSHUTEncoding(const MCInst &MI, unsigned OpNo,
                           SmallVectorImpl<MCFixup> &Fixups,
                           const MCSubtargetInfo &STI) const;
  
  unsigned getMRegOEncoding(const MCInst &MI, unsigned OpNo,
                            SmallVectorImpl<MCFixup> &Fixups,
                            const MCSubtargetInfo &STI) const;
  
  void encodeInstruction(const MCInst &MI, raw_ostream &OS,
                         SmallVectorImpl<MCFixup> &Fixups,
                         const MCSubtargetInfo &STI) const override {
    UCPS::InsnLine line;
    uint64_t code;
    const MCInst *CurInst = &MI;
    do {
      if (CurInst == &MI)
        CurInst = CurInst->getOperand(0).getInst();
      else
        CurInst = CurInst->getOperand(CurInst->getNumOperands()-1).getInst();
      code = getBinaryCodeForInstr(*CurInst, Fixups, STI);
      line.ConcatCode(code, code >> 44);
      ++MCNumEmitted;
    } while (CurInst->getNumOperands() &&
             CurInst->getOperand(CurInst->getNumOperands()-1).isInst());
    ++MCLineNumEmitted;
    line.EmitCode(OS);
  }
};
}  // end anonymous namespace

MCCodeEmitter *llvm::createUCPSMCCodeEmitter(const MCInstrInfo &MCII,
                                             const MCRegisterInfo &MRI,
                                             MCContext &Ctx) {
  return new UCPSMCCodeEmitter(Ctx);
}

/// getMachineOpValue - Return binary encoding of operand. If the machine
/// operand requires relocation, record the relocation and return zero.
unsigned UCPSMCCodeEmitter::getMachineOpValue(const MCInst &MI,
                              const MCOperand &MO,
                              SmallVectorImpl<MCFixup> &Fixups,
                              const MCSubtargetInfo &STI) const {
  if (MO.isReg()) 
    return CTX.getRegisterInfo()->getEncodingValue(MO.getReg());
  else if (MO.isImm())
    return static_cast<unsigned>(MO.getImm());
  else if (MO.isFPImm())
    return static_cast<unsigned>(APFloat(MO.getFPImm()).bitcastToAPInt()
                                 .getHiBits(32).getLimitedValue());

  // MO must be an Expr.
  assert(MO.isExpr());

  const MCExpr *Expr = MO.getExpr();
  MCExpr::ExprKind Kind = Expr->getKind();

  UCPS::FixupKind fixupKind = UCPS::fixup_UCPS_11;
  if (Kind == MCExpr::Constant) 
    return static_cast<const MCConstantExpr*>(Expr)->getValue();

  //assert(Kind == MCExpr::SymbolRef && "Unsolved Expr");
  Fixups.push_back(MCFixup::create(37/*Offset*/, MO.getExpr(),
                                   MCFixupKind(fixupKind), MI.getLoc()));

  return 0;
}

unsigned UCPSMCCodeEmitter::
getMACCTIEncoding(const llvm::MCInst &MI, unsigned int OpNo,
                  SmallVectorImpl<llvm::MCFixup>& Fixups,
                  const MCSubtargetInfo &STI) const {
  assert(MI.getOperand(OpNo).isReg());
  unsigned UnitBits = getMachineOpValue(MI, MI.getOperand(OpNo), Fixups, STI);
  assert(MI.getOperand(OpNo+1).isReg());
  unsigned TPortBits = getMachineOpValue(MI, MI.getOperand(OpNo+1), Fixups, STI);
  assert(MI.getOperand(OpNo+2).isImm());
  unsigned IPathBits = getMachineOpValue(MI, MI.getOperand(OpNo+2), Fixups, STI);

  return (TPortBits) | (IPathBits << 2) | (UnitBits << 4);
}
unsigned UCPSMCCodeEmitter::
getMACCTEncoding(const llvm::MCInst &MI, unsigned int OpNo,
                  SmallVectorImpl<llvm::MCFixup>& Fixups,
                  const MCSubtargetInfo &STI) const {
  assert(MI.getOperand(OpNo).isReg());
  unsigned UnitBits = getMachineOpValue(MI, MI.getOperand(OpNo), Fixups, STI);
  assert(MI.getOperand(OpNo+1).isReg());
  unsigned TPortBits = getMachineOpValue(MI, MI.getOperand(OpNo+1), Fixups, STI);

  return (TPortBits) | (UnitBits << 2);
}
unsigned UCPSMCCodeEmitter::
getBIUTEncoding(const llvm::MCInst &MI, unsigned int OpNo,
                  SmallVectorImpl<llvm::MCFixup>& Fixups,
                  const MCSubtargetInfo &STI) const {
  assert(MI.getOperand(OpNo).isReg());
  unsigned UnitBits = getMachineOpValue(MI, MI.getOperand(OpNo), Fixups, STI);
  assert(MI.getOperand(OpNo+1).isReg());
  unsigned TPortBits = getMachineOpValue(MI, MI.getOperand(OpNo+1), Fixups, STI);

  return (TPortBits) | (UnitBits << 2);
}

unsigned UCPSMCCodeEmitter::
getMR0DestTEncoding(const llvm::MCInst &MI, unsigned int OpNo,
                  SmallVectorImpl<llvm::MCFixup>& Fixups,
                  const MCSubtargetInfo &STI) const {
  assert(MI.getOperand(OpNo).isReg());
  unsigned UnitBits = getMachineOpValue(MI, MI.getOperand(OpNo), Fixups, STI);
  assert(MI.getOperand(OpNo+1).isReg());
  unsigned TPortBits = getMachineOpValue(MI, MI.getOperand(OpNo+1), Fixups, STI);

  return (UnitBits << 2) | TPortBits;
}

unsigned UCPSMCCodeEmitter::
getMR1DestTEncoding(const llvm::MCInst &MI, unsigned int OpNo,
                  SmallVectorImpl<llvm::MCFixup>& Fixups,
                  const MCSubtargetInfo &STI) const {
  assert(MI.getOperand(OpNo).isReg());
  unsigned UnitBits = getMachineOpValue(MI, MI.getOperand(OpNo), Fixups, STI);
  assert(MI.getOperand(OpNo+1).isReg());
  unsigned TPortBits = getMachineOpValue(MI, MI.getOperand(OpNo+1), Fixups, STI);

  return (UnitBits << 2) | TPortBits;
}

unsigned UCPSMCCodeEmitter::
getMR2DestTEncoding(const llvm::MCInst &MI, unsigned int OpNo,
                  SmallVectorImpl<llvm::MCFixup>& Fixups,
                  const MCSubtargetInfo &STI) const {
  assert(MI.getOperand(OpNo).isReg());
  unsigned UnitBits = getMachineOpValue(MI, MI.getOperand(OpNo), Fixups, STI);
  assert(MI.getOperand(OpNo+1).isReg());
  unsigned TPortBits = getMachineOpValue(MI, MI.getOperand(OpNo+1), Fixups, STI);

  return (UnitBits << 2) | TPortBits;
}

unsigned UCPSMCCodeEmitter::
getMR3DestTEncoding(const llvm::MCInst &MI, unsigned int OpNo,
                  SmallVectorImpl<llvm::MCFixup>& Fixups,
                  const MCSubtargetInfo &STI) const {
  assert(MI.getOperand(OpNo).isReg());
  unsigned UnitBits = getMachineOpValue(MI, MI.getOperand(OpNo), Fixups, STI);
  assert(MI.getOperand(OpNo+1).isReg());
  unsigned TPortBits = getMachineOpValue(MI, MI.getOperand(OpNo+1), Fixups, STI);

  return (UnitBits << 2) | TPortBits;
}

unsigned UCPSMCCodeEmitter::
getMR4DestTEncoding(const llvm::MCInst &MI, unsigned int OpNo,
                  SmallVectorImpl<llvm::MCFixup>& Fixups,
                  const MCSubtargetInfo &STI) const {
  assert(MI.getOperand(OpNo).isReg());
  unsigned UnitBits = getMachineOpValue(MI, MI.getOperand(OpNo), Fixups, STI);
  assert(MI.getOperand(OpNo+1).isReg());
  unsigned TPortBits = getMachineOpValue(MI, MI.getOperand(OpNo+1), Fixups, STI);

  return (UnitBits << 2) | TPortBits;
}

unsigned UCPSMCCodeEmitter::
getMR5DestTEncoding(const llvm::MCInst &MI, unsigned int OpNo,
                  SmallVectorImpl<llvm::MCFixup>& Fixups,
                  const MCSubtargetInfo &STI) const {
  assert(MI.getOperand(OpNo).isReg());
  unsigned UnitBits = getMachineOpValue(MI, MI.getOperand(OpNo), Fixups, STI);
  assert(MI.getOperand(OpNo+1).isReg());
  unsigned TPortBits = getMachineOpValue(MI, MI.getOperand(OpNo+1), Fixups, STI);

  return (UnitBits << 2) | TPortBits;
}

unsigned UCPSMCCodeEmitter::
getSHUTEncoding(const llvm::MCInst &MI, unsigned int OpNo,
                SmallVectorImpl<llvm::MCFixup>& Fixups,
                const MCSubtargetInfo &STI) const {
  assert(MI.getOperand(OpNo).isReg());
  unsigned UnitBits = getMachineOpValue(MI, MI.getOperand(OpNo), Fixups, STI);
  assert(MI.getOperand(OpNo+1).isReg());
  unsigned TPortBits = getMachineOpValue(MI, MI.getOperand(OpNo+1), Fixups, STI);
  return (UnitBits << 2) | TPortBits;
}

unsigned UCPSMCCodeEmitter::
getMRegOEncoding(const llvm::MCInst &MI, unsigned int OpNo,
                 SmallVectorImpl<llvm::MCFixup>& Fixups,
                 const MCSubtargetInfo &STI) const {
  assert(MI.getOperand(OpNo).isReg());
  unsigned MBits = getMachineOpValue(MI, MI.getOperand(OpNo), Fixups, STI);
  return ((MBits & 0x7F) << 2) | (MBits >> 7);
}

// ducx
unsigned UCPSMCCodeEmitter::
getBIU0DestTEEncoding(const llvm::MCInst &MI, unsigned int OpNo,
                  SmallVectorImpl<llvm::MCFixup>& Fixups,
                  const MCSubtargetInfo &STI) const {
  assert(MI.getOperand(OpNo).isReg());
  unsigned UnitBits = getMachineOpValue(MI, MI.getOperand(OpNo), Fixups, STI);
  assert(MI.getOperand(OpNo+1).isReg());
  unsigned TPortBits = getMachineOpValue(MI, MI.getOperand(OpNo+1), Fixups, STI);

  return (UnitBits << 4) | TPortBits;
}
unsigned UCPSMCCodeEmitter::
getBIU1DestTEEncoding(const llvm::MCInst &MI, unsigned int OpNo,
                  SmallVectorImpl<llvm::MCFixup>& Fixups,
                  const MCSubtargetInfo &STI) const {
  assert(MI.getOperand(OpNo).isReg());
  unsigned UnitBits = getMachineOpValue(MI, MI.getOperand(OpNo), Fixups, STI);
  assert(MI.getOperand(OpNo+1).isReg());
  unsigned TPortBits = getMachineOpValue(MI, MI.getOperand(OpNo+1), Fixups, STI);

  return (UnitBits << 4) | TPortBits;
}
unsigned UCPSMCCodeEmitter::
getBIU2DestTEEncoding(const llvm::MCInst &MI, unsigned int OpNo,
                  SmallVectorImpl<llvm::MCFixup>& Fixups,
                  const MCSubtargetInfo &STI) const {
  assert(MI.getOperand(OpNo).isReg());
  unsigned UnitBits = getMachineOpValue(MI, MI.getOperand(OpNo), Fixups, STI);
  assert(MI.getOperand(OpNo+1).isReg());
  unsigned TPortBits = getMachineOpValue(MI, MI.getOperand(OpNo+1), Fixups, STI);

  return (UnitBits << 4) | TPortBits;
}
#include "UCPSGenMCCodeEmitter.inc"
