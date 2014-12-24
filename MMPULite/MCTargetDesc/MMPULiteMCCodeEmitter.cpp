//===-- MMPULiteMCCodeEmitter.cpp - Convert MMPULite code to machine code -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the MMPULiteMCCodeEmitter class
// and originates from "PPCMCCodeEmitter.cpp"
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "mccodeemitter"
#include "MMPULiteFixupKinds.h"
#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/ADT/APFloat.h"
#include "MMPULiteInsnLine.h"
#include "../MCTargetDesc/MMPULiteMCTargetDesc.h"

using namespace llvm;

STATISTIC(MCNumEmitted, "Number of MC instructions emitted");
STATISTIC(MCLineNumEmitted, "Number of MC instruction lines emitted");

namespace {
class MMPULiteMCCodeEmitter: public MCCodeEmitter
{
  const MCContext &CTX;
  MMPULiteMCCodeEmitter(const MMPULiteMCCodeEmitter &) LLVM_DELETED_FUNCTION;

  void
  operator=(const MMPULiteMCCodeEmitter &) LLVM_DELETED_FUNCTION;

public:
  MMPULiteMCCodeEmitter(MCContext &ctx)
    : CTX(ctx) {
  }

  ~MMPULiteMCCodeEmitter() {}

  /// getMachineOpValue - Return binary encoding of operand. If the machine
  /// operand requires fixup, record the fixup and return zero.
  /// This func is called within tblgen'erated getBinaryCodeForInstr().
  unsigned getMachineOpValue(const MCInst &MI, const MCOperand &MO,
                             SmallVectorImpl<MCFixup> &Fixups/*,
                             const MCSubtargetInfo &STI*/) const;

  // getBinaryCodeForInstr - TableGen'erated function for getting the
  // binary encoding for an instruction.
  uint64_t getBinaryCodeForInstr(const MCInst &MI,
                                 SmallVectorImpl<MCFixup> &Fixups/*,
                                 const MCSubtargetInfo &STI*/) const;
  
  unsigned getMACCTIEncoding(const MCInst &MI, unsigned OpNo,
                             SmallVectorImpl<MCFixup> &Fixups/*,
                             const MCSubtargetInfo &STI*/) const;
  
  unsigned getSHUTEncoding(const MCInst &MI, unsigned OpNo,
                           SmallVectorImpl<MCFixup> &Fixups/*,
                           const MCSubtargetInfo &STI*/) const;
  
  unsigned getMRegOEncoding(const MCInst &MI, unsigned OpNo,
                            SmallVectorImpl<MCFixup> &Fixups/*,
                            const MCSubtargetInfo &STI*/) const;
  
  void EncodeInstruction(const MCInst &MI, raw_ostream &OS,
                         SmallVectorImpl<MCFixup> &Fixups/*,
                         const MCSubtargetInfo &STI*/) const {
    MMPULite::InsnLine line;
    uint64_t code;
    const MCInst *CurInst = &MI;
    do {
      if (CurInst == &MI)
        CurInst = CurInst->getOperand(0).getInst();
      else
        CurInst = CurInst->getOperand(CurInst->getNumOperands()-1).getInst();
      code = getBinaryCodeForInstr(*CurInst, Fixups/*, STI*/);
      line.ConcatCode(code, code >> 44);
      ++MCNumEmitted;
    } while (CurInst->getNumOperands() &&
             CurInst->getOperand(CurInst->getNumOperands()-1).isInst());
    ++MCLineNumEmitted;
    line.EmitCode(OS);
  }
};
}  // end anonymous namespace

MCCodeEmitter *llvm::createMMPULiteMCCodeEmitter(const MCInstrInfo &MCII,
                                             const MCRegisterInfo &MRI,
                                             const MCSubtargetInfo &STI,
                                             MCContext &Ctx) {
  return new MMPULiteMCCodeEmitter(Ctx);
}

unsigned MMPULiteMCCodeEmitter::getMachineOpValue(const MCInst &MI,
                              const MCOperand &MO,
                              SmallVectorImpl<MCFixup> &Fixups/*,
                              const MCSubtargetInfo &STI*/) const {
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

  MMPULite::FixupKind fixupKind = MMPULite::fixup_MMPULite_11;
  if (Kind == MCExpr::Constant) 
    return static_cast<const MCConstantExpr*>(Expr)->getValue();

  //assert(Kind == MCExpr::SymbolRef && "Unsolved Expr");
  Fixups.push_back(MCFixup::Create(37/*Offset*/, MO.getExpr(),
                                   MCFixupKind(fixupKind), MI.getLoc()));

  return 0;
}

unsigned MMPULiteMCCodeEmitter::
getMACCTIEncoding(const llvm::MCInst &MI, unsigned int OpNo,
                  SmallVectorImpl<llvm::MCFixup>& Fixups/*,
                  const MCSubtargetInfo &STI*/) const {
  assert(MI.getOperand(OpNo).isReg());
  unsigned UnitBits = getMachineOpValue(MI, MI.getOperand(OpNo), Fixups/*, STI*/);
  assert(MI.getOperand(OpNo+1).isReg());
  unsigned TPortBits = getMachineOpValue(MI, MI.getOperand(OpNo+1), Fixups/*, STI*/);
  assert(MI.getOperand(OpNo+2).isImm());
  unsigned IPathBits = getMachineOpValue(MI, MI.getOperand(OpNo+2), Fixups/*, STI*/);
  
  return UnitBits | (TPortBits << 2) | (IPathBits << 4);
}

unsigned MMPULiteMCCodeEmitter::
getSHUTEncoding(const llvm::MCInst &MI, unsigned int OpNo,
                SmallVectorImpl<llvm::MCFixup>& Fixups/*,
                const MCSubtargetInfo &STI*/) const {
  assert(MI.getOperand(OpNo).isReg());
  unsigned UnitBits = getMachineOpValue(MI, MI.getOperand(OpNo), Fixups/*, STI*/);
  assert(MI.getOperand(OpNo+1).isReg());
  unsigned TPortBits = getMachineOpValue(MI, MI.getOperand(OpNo+1), Fixups/*, STI*/);
  return UnitBits | (TPortBits << 2);
}

unsigned MMPULiteMCCodeEmitter::
getMRegOEncoding(const llvm::MCInst &MI, unsigned int OpNo,
                 SmallVectorImpl<llvm::MCFixup>& Fixups/*,
                 const MCSubtargetInfo &STI*/) const {
  assert(MI.getOperand(OpNo).isReg());
  unsigned MBits = getMachineOpValue(MI, MI.getOperand(OpNo), Fixups/*, STI*/);  
  return ((MBits & 0x7F) << 2) | (MBits >> 7);
}
#include "MMPULiteGenMCCodeEmitter.inc"
