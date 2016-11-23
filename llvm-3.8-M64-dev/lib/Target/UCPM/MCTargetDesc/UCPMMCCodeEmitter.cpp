//===-- UCPMMCCodeEmitter.cpp - Convert UCPM code to machine code -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the UCPMMCCodeEmitter class
// and originates from "PPCMCCodeEmitter.cpp"
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "mccodeemitter"
#include "UCPMFixupKinds.h"
#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/ADT/APFloat.h"
#include "UCPMInsnLine.h"
#include "../MCTargetDesc/UCPMMCTargetDesc.h"
#include "../AsmParser/UCPMAsmParser.h"

using namespace llvm;


STATISTIC(MCNumEmitted, "Number of MC instructions emitted");
STATISTIC(MCLineNumEmitted, "Number of MC instruction lines emitted");

namespace {
class UCPMMCCodeEmitter: public MCCodeEmitter
{
  const MCContext &CTX;

public:
  UCPMMCCodeEmitter(MCContext &ctx)
    : CTX(ctx) {
  }

  ~UCPMMCCodeEmitter() {}

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
  
//   unsigned getMACCTIEncoding(const MCInst &MI, unsigned OpNo,
//                              SmallVectorImpl<MCFixup> &Fixups,
//                              const MCSubtargetInfo &STI) const;
			     
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
				 
  unsigned getSHU0DestTIEncoding(const MCInst &MI, unsigned OpNo,
                                 SmallVectorImpl<MCFixup> &Fixups,
                                 const MCSubtargetInfo &STI) const;
  
  unsigned getSHU1DestTIEncoding(const MCInst &MI, unsigned OpNo,
                                 SmallVectorImpl<MCFixup> &Fixups,
                                 const MCSubtargetInfo &STI) const;
				 
  unsigned getSHU2DestTIEncoding(const MCInst &MI, unsigned OpNo,
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

  unsigned getshuMRegOEncoding(const MCInst &MI, unsigned OpNo,
                            SmallVectorImpl<MCFixup> &Fixups,
                            const MCSubtargetInfo &STI) const;

  void encodeInstruction(const MCInst &MI, raw_ostream &OS,
                         SmallVectorImpl<MCFixup> &Fixups,
                         const MCSubtargetInfo &STI) const override {
	std::bitset<NUMSLOTS> SlotFlag;
	uint64_t SlotCode[NUMSLOTS] = {0};
	uint64_t test = 0;
	//NOP for individual slots
	for(int i = 0; i < NUMSLOTS; i++)
		SlotCode[i] = getBinaryCodeForInstr(*UCPM::NOPInst[i], Fixups, STI);

	SlotFlag.reset();


    UCPM::InsnLine line;
    uint64_t code;
    const MCInst *CurInst = &MI;
    do {
      if (CurInst == &MI)
        CurInst = CurInst->getOperand(0).getInst();
      else
        CurInst = CurInst->getOperand(CurInst->getNumOperands()-1).getInst();
      code = getBinaryCodeForInstr(*CurInst, Fixups, STI);     
      //NOP
      if(code == 0)
        break;
      SlotFlag[code >> 43] = 1;
      SlotCode[code >> 43] = code;
      //line.ConcatCode(code, code >> 44);
      ++MCNumEmitted;
    } while (CurInst->getNumOperands() &&
             CurInst->getOperand(CurInst->getNumOperands()-1).isInst());

    // dcx 20161123: for change of M128->M64
    for(int i = 10; i<14; i++)
    {
      uint64_t tempcode1 = SlotCode[i] >> 8;
      uint64_t tempcode2 = SlotCode[i] & 0b1111111;
      uint64_t tempcode3 = SlotCode[i] >> 43;
      SlotCode[i] = ((tempcode1<<7) | tempcode2) & 0x3ffffffffff | (tempcode3<<43);
    }
	
    for(int i = 0; i < NUMSLOTS; i++)
      line.ConcatCode(SlotCode[i], i);
    ++MCLineNumEmitted;
    line.EmitCode(OS);
  }
};
}  // end anonymous namespace

MCCodeEmitter *llvm::createUCPMMCCodeEmitter(const MCInstrInfo &MCII,
                                             const MCRegisterInfo &MRI,
                                             MCContext &Ctx) {
  return new UCPMMCCodeEmitter(Ctx);
}

/// getMachineOpValue - Return binary encoding of operand. If the machine
/// operand requires relocation, record the relocation and return zero.
unsigned UCPMMCCodeEmitter::getMachineOpValue(const MCInst &MI,
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

  UCPM::FixupKind fixupKind = UCPM::fixup_UCPM_11;
  if (Kind == MCExpr::Constant) 
    return static_cast<const MCConstantExpr*>(Expr)->getValue();

  //assert(Kind == MCExpr::SymbolRef && "Unsolved Expr");
  Fixups.push_back(MCFixup::create(37/*Offset*/, MO.getExpr(),
                                   MCFixupKind(fixupKind), MI.getLoc()));

  return 0;
}

// unsigned UCPMMCCodeEmitter::
// getMACCTIEncoding(const llvm::MCInst &MI, unsigned int OpNo,
//                   SmallVectorImpl<llvm::MCFixup>& Fixups,
//                   const MCSubtargetInfo &STI) const {
//   assert(MI.getOperand(OpNo).isReg());
//   unsigned UnitBits = getMachineOpValue(MI, MI.getOperand(OpNo), Fixups, STI);
//   assert(MI.getOperand(OpNo+1).isReg());
//   unsigned TPortBits = getMachineOpValue(MI, MI.getOperand(OpNo+1), Fixups, STI);
//   assert(MI.getOperand(OpNo+2).isImm());
//   unsigned IPathBits = getMachineOpValue(MI, MI.getOperand(OpNo+2), Fixups, STI);
// 
//   return (TPortBits) | (IPathBits << 2) | (UnitBits << 4);
// }

unsigned UCPMMCCodeEmitter::
getMACCTEncoding(const llvm::MCInst &MI, unsigned int OpNo,
                  SmallVectorImpl<llvm::MCFixup>& Fixups,
                  const MCSubtargetInfo &STI) const {
  assert(MI.getOperand(OpNo).isReg());
  unsigned UnitBits = getMachineOpValue(MI, MI.getOperand(OpNo), Fixups, STI);
  assert(MI.getOperand(OpNo+1).isReg());
  unsigned TPortBits = getMachineOpValue(MI, MI.getOperand(OpNo+1), Fixups, STI);

  return (TPortBits) | (UnitBits << 2);
}
unsigned UCPMMCCodeEmitter::
getBIUTEncoding(const llvm::MCInst &MI, unsigned int OpNo,
                  SmallVectorImpl<llvm::MCFixup>& Fixups,
                  const MCSubtargetInfo &STI) const {
  assert(MI.getOperand(OpNo).isReg());
  unsigned UnitBits = getMachineOpValue(MI, MI.getOperand(OpNo), Fixups, STI);
  assert(MI.getOperand(OpNo+1).isReg());
  unsigned TPortBits = getMachineOpValue(MI, MI.getOperand(OpNo+1), Fixups, STI);

  return (TPortBits) | (UnitBits << 2);
}

unsigned UCPMMCCodeEmitter::
getMR0DestTEncoding(const llvm::MCInst &MI, unsigned int OpNo,
                  SmallVectorImpl<llvm::MCFixup>& Fixups,
                  const MCSubtargetInfo &STI) const {
  assert(MI.getOperand(OpNo).isReg());
  unsigned UnitBits = getMachineOpValue(MI, MI.getOperand(OpNo), Fixups, STI);
  assert(MI.getOperand(OpNo+1).isReg());
  unsigned TPortBits = getMachineOpValue(MI, MI.getOperand(OpNo+1), Fixups, STI);

  return (UnitBits << 2) | TPortBits;
}

unsigned UCPMMCCodeEmitter::
getMR1DestTEncoding(const llvm::MCInst &MI, unsigned int OpNo,
                  SmallVectorImpl<llvm::MCFixup>& Fixups,
                  const MCSubtargetInfo &STI) const {
  assert(MI.getOperand(OpNo).isReg());
  unsigned UnitBits = getMachineOpValue(MI, MI.getOperand(OpNo), Fixups, STI);
  assert(MI.getOperand(OpNo+1).isReg());
  unsigned TPortBits = getMachineOpValue(MI, MI.getOperand(OpNo+1), Fixups, STI);

  return (UnitBits << 2) | TPortBits;
}

unsigned UCPMMCCodeEmitter::
getMR2DestTEncoding(const llvm::MCInst &MI, unsigned int OpNo,
                  SmallVectorImpl<llvm::MCFixup>& Fixups,
                  const MCSubtargetInfo &STI) const {
  assert(MI.getOperand(OpNo).isReg());
  unsigned UnitBits = getMachineOpValue(MI, MI.getOperand(OpNo), Fixups, STI);
  assert(MI.getOperand(OpNo+1).isReg());
  unsigned TPortBits = getMachineOpValue(MI, MI.getOperand(OpNo+1), Fixups, STI);

  return (UnitBits << 2) | TPortBits;
}

unsigned UCPMMCCodeEmitter::
getMR3DestTEncoding(const llvm::MCInst &MI, unsigned int OpNo,
                  SmallVectorImpl<llvm::MCFixup>& Fixups,
                  const MCSubtargetInfo &STI) const {
  assert(MI.getOperand(OpNo).isReg());
  unsigned UnitBits = getMachineOpValue(MI, MI.getOperand(OpNo), Fixups, STI);
  assert(MI.getOperand(OpNo+1).isReg());
  unsigned TPortBits = getMachineOpValue(MI, MI.getOperand(OpNo+1), Fixups, STI);

  return (UnitBits << 2) | TPortBits;
}

unsigned UCPMMCCodeEmitter::
getMR4DestTEncoding(const llvm::MCInst &MI, unsigned int OpNo,
                  SmallVectorImpl<llvm::MCFixup>& Fixups,
                  const MCSubtargetInfo &STI) const {
  assert(MI.getOperand(OpNo).isReg());
  unsigned UnitBits = getMachineOpValue(MI, MI.getOperand(OpNo), Fixups, STI);
  assert(MI.getOperand(OpNo+1).isReg());
  unsigned TPortBits = getMachineOpValue(MI, MI.getOperand(OpNo+1), Fixups, STI);

  return (UnitBits << 2) | TPortBits;
}

unsigned UCPMMCCodeEmitter::
getMR5DestTEncoding(const llvm::MCInst &MI, unsigned int OpNo,
                  SmallVectorImpl<llvm::MCFixup>& Fixups,
                  const MCSubtargetInfo &STI) const {
  assert(MI.getOperand(OpNo).isReg());
  unsigned UnitBits = getMachineOpValue(MI, MI.getOperand(OpNo), Fixups, STI);
  assert(MI.getOperand(OpNo+1).isReg());
  unsigned TPortBits = getMachineOpValue(MI, MI.getOperand(OpNo+1), Fixups, STI);

  return (UnitBits << 2) | TPortBits;
}

unsigned UCPMMCCodeEmitter::
getSHUTEncoding(const llvm::MCInst &MI, unsigned int OpNo,
                SmallVectorImpl<llvm::MCFixup>& Fixups,
                const MCSubtargetInfo &STI) const {
  assert(MI.getOperand(OpNo).isReg());
  unsigned UnitBits = getMachineOpValue(MI, MI.getOperand(OpNo), Fixups, STI);
  assert(MI.getOperand(OpNo+1).isReg());
  unsigned TPortBits = getMachineOpValue(MI, MI.getOperand(OpNo+1), Fixups, STI);
  return (UnitBits << 2) | TPortBits;
}

unsigned UCPMMCCodeEmitter::
getMRegOEncoding(const llvm::MCInst &MI, unsigned int OpNo,
                 SmallVectorImpl<llvm::MCFixup>& Fixups,
                 const MCSubtargetInfo &STI) const {
  assert(MI.getOperand(OpNo).isReg());
  unsigned MBits = getMachineOpValue(MI, MI.getOperand(OpNo), Fixups, STI);
  //return ((MBits & 0x7F) << 2) | (MBits >> 7);
  return MBits;
}

// ducx
unsigned UCPMMCCodeEmitter::
getBIU0DestTEEncoding(const llvm::MCInst &MI, unsigned int OpNo,
                  SmallVectorImpl<llvm::MCFixup>& Fixups,
                  const MCSubtargetInfo &STI) const {
  assert(MI.getOperand(OpNo).isReg());
  unsigned UnitBits = getMachineOpValue(MI, MI.getOperand(OpNo), Fixups, STI);
  assert(MI.getOperand(OpNo+1).isReg());
  unsigned TPortBits = getMachineOpValue(MI, MI.getOperand(OpNo+1), Fixups, STI);

  return (UnitBits << 4) | TPortBits;
}
unsigned UCPMMCCodeEmitter::
getBIU1DestTEEncoding(const llvm::MCInst &MI, unsigned int OpNo,
                  SmallVectorImpl<llvm::MCFixup>& Fixups,
                  const MCSubtargetInfo &STI) const {
  assert(MI.getOperand(OpNo).isReg());
  unsigned UnitBits = getMachineOpValue(MI, MI.getOperand(OpNo), Fixups, STI);
  assert(MI.getOperand(OpNo+1).isReg());
  unsigned TPortBits = getMachineOpValue(MI, MI.getOperand(OpNo+1), Fixups, STI);

  return (UnitBits << 4) | TPortBits;
}
unsigned UCPMMCCodeEmitter::
getBIU2DestTEEncoding(const llvm::MCInst &MI, unsigned int OpNo,
                  SmallVectorImpl<llvm::MCFixup>& Fixups,
                  const MCSubtargetInfo &STI) const {
  assert(MI.getOperand(OpNo).isReg());
  unsigned UnitBits = getMachineOpValue(MI, MI.getOperand(OpNo), Fixups, STI);
  assert(MI.getOperand(OpNo+1).isReg());
  unsigned TPortBits = getMachineOpValue(MI, MI.getOperand(OpNo+1), Fixups, STI);

  return (UnitBits << 4) | TPortBits;
}
unsigned UCPMMCCodeEmitter::
getSHU0DestTIEncoding(const llvm::MCInst &MI, unsigned int OpNo,
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
unsigned UCPMMCCodeEmitter::
getSHU1DestTIEncoding(const llvm::MCInst &MI, unsigned int OpNo,
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
unsigned UCPMMCCodeEmitter::
getSHU2DestTIEncoding(const llvm::MCInst &MI, unsigned int OpNo,
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
unsigned UCPMMCCodeEmitter::
getshuMRegOEncoding(const llvm::MCInst &MI, unsigned int OpNo,
                 SmallVectorImpl<llvm::MCFixup>& Fixups,
                 const MCSubtargetInfo &STI) const {
  assert(MI.getOperand(OpNo).isReg());
  unsigned MBits = getMachineOpValue(MI, MI.getOperand(OpNo), Fixups, STI);
  return MBits;
}

#include "UCPMGenMCCodeEmitter.inc"
