//===-- MSPUAsmPrinter.h - Print machine code to an MSPU .s file ----===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// MSPU Assembly printer class.
//
//===----------------------------------------------------------------------===//

#ifndef MSPU_ASM_PRINTER_H
#define MSPU_ASM_PRINTER_H

#include "MSPU.h"
#include "MSPUTargetMachine.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/raw_ostream.h"

namespace llvm
{
	// Note:
	// This class plays mixed roles: it converts MachineInstr into MCInst if backend
	// supoorts MC layer. Or it will directly write out asm to iostream from MachineInstr
	// like Sparc backend does.
class LLVM_LIBRARY_VISIBILITY MSPUAsmPrinter: public AsmPrinter {
  const MSPUSubtarget *Subtarget;

  MCOperand LowerSymbolOperand(const MachineOperand& MO, const MCSymbol* Symbol) {
    const MCExpr *ME;
    ME = MCSymbolRefExpr::create(Symbol, MCSymbolRefExpr::VK_None, OutContext);
    return (MCOperand::createExpr(ME));
  }

  void LowerToMC(const MachineInstr *MI, MCInst &MCI, MCInst *Head);

public:
  explicit MSPUAsmPrinter(TargetMachine &TM, std::unique_ptr<MCStreamer> Streamer) :
      AsmPrinter(TM, std::move(Streamer)) {
  }

  bool runOnMachineFunction(MachineFunction &Fn) override {
    Subtarget = &Fn.getSubtarget<MSPUSubtarget>();
    return AsmPrinter::runOnMachineFunction(Fn);
  }

  virtual const char *
  getPassName() const {
    return "MSPU Assembly Printer";
  }

  virtual bool
  isBlockOnlyReachableByFallthrough(const MachineBasicBlock *MBB) const;

  virtual void
  EmitInstruction(const MachineInstr *MI);

  // Targets can, or in the case of EmitInstruction, must implement these to
  // customize output.

  /// EmitStartOfAsmFile - This virtual method can be overridden by targets
  /// that want to emit something at the start of their file.
  void EmitStartOfAsmFile(Module &);

  /// EmitEndOfAsmFile - This virtual method can be overridden by targets that
  /// want to emit something at the end of their file.
  void EmitEndOfAsmFile(Module &);
};

}  // end of llvm namespace

#endif
