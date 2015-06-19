//===- MSPUMCInstLower.cpp - Convert MSPU MachineInstr to an MCInst -===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains code to lower MSPU MachineInstrs to their corresponding
// MCInst records.
//
//===----------------------------------------------------------------------===//

#include "MSPU.h"
#include "MSPUAsmPrinter.h"
#include "MSPUMachineFunctionInfo.h"
#include "llvm/IR/Constants.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCStreamer.h"

using namespace llvm;

static MCOperand LowerSymbolOperand(const MachineOperand& MO, const MCSymbol* Symbol,
                              MSPUAsmPrinter& Printer) {

	const MCExpr *ME;
  ME = MCSymbolRefExpr::Create(Symbol, MCSymbolRefExpr::VK_None, Printer.OutContext);
  return (MCOperand::CreateExpr(ME));
}

// Create an MCInst from a MachineInstr
void llvm::MSPULowerToMC(const MachineInstr* MI, MCInst& MCI, MCInst *Head,
                            MSPUAsmPrinter& Printer) {
  MCI.setOpcode(MI->getOpcode());

  for (unsigned i = 0, e = MI->getNumOperands(); i < e; i++) {

    const MachineOperand &MO = MI->getOperand(i);
    MCOperand MCO;
    MCSymbol* Sym;

    switch (MO.getType()) {
    default:
      MI->dump();
      assert(0 && "unknown operand type");
      break;

    case MachineOperand::MO_Register:
      // Ignore all implicit register operands.
      if (MO.isImplicit()) continue;
      MCO = MCOperand::CreateReg(MO.getReg());
      break;

    case MachineOperand::MO_FPImmediate: {
    	const APFloat& Val = MO.getFPImm()->getValueAPF();
      // FP immediates are used only when setting GPRs, so they may be dealt
      // with like regular immediates from this point on.
      MCO = MCOperand::CreateImm( *( Val.bitcastToAPInt().getRawData() ) );
      break;
    }

    case MachineOperand::MO_Immediate:
      MCO = MCOperand::CreateImm(MO.getImm());
      break;

    case MachineOperand::MO_MachineBasicBlock:
      MCO = MCOperand::CreateExpr(MCSymbolRefExpr::Create(MO.getMBB()->getSymbol(),
               Printer.OutContext));
      break;

    case MachineOperand::MO_GlobalAddress:
    	Sym = Printer.getSymbol(MO.getGlobal());
      MCO = LowerSymbolOperand(MO, Sym, Printer);
      //Printer.OutStreamer.EmitSymbolAttribute(Sym, MCSA_Global);
      break;

    case MachineOperand::MO_ExternalSymbol:
    	Sym = Printer.GetExternalSymbolSymbol(MO.getSymbolName());
      MCO = LowerSymbolOperand(MO, Sym, Printer);
      //Printer.OutStreamer.EmitSymbolAttribute(Sym, MCSA_Global);
      break;

    case MachineOperand::MO_JumpTableIndex:
      MCO = LowerSymbolOperand(MO, Printer.GetJTISymbol(MO.getIndex()), Printer);
      break;

    case MachineOperand::MO_ConstantPoolIndex:
      MCO = LowerSymbolOperand(MO, Printer.GetCPISymbol(MO.getIndex()), Printer);
      break;

    case MachineOperand::MO_BlockAddress:
      MCO = LowerSymbolOperand(MO, Printer.GetBlockAddressSymbol(MO.getBlockAddress()),Printer);
      break;

    case MachineOperand::MO_RegisterMask:
      break;
    }

    MCI.addOperand(MCO);
    if (Head && (&MCI != Head)) Head->addOperand(MCO);
  }

}
