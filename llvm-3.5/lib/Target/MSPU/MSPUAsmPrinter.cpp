//===-- MSPUAsmPrinter.cpp - Print machine instrs to MSPU assembly --===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains a printer that converts from our internal representation
// of machine-dependent LLVM code to MSPU assembly language. This printer is
// the output mechanism used by `llc'.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "asm-printer"
#include "MSPU.h"
#include "MSPUAsmPrinter.h"
#include "MSPUMachineFunctionInfo.h"
#include "MSPUTargetMachine.h"
#include "MSPUSubtarget.h"
#include "InstPrinter/MSPUInstPrinter.h"
#include "MCTargetDesc/MSPUMCInst.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Module.h"
#include "llvm/ADT/SetVector.h"
#include "llvm/Analysis/ConstantFolding.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/CodeGen/MachineModuleInfoImpls.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCSection.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/MathExtras.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/Format.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/Target/TargetLoweringObjectFile.h"
#include "llvm/Target/TargetRegisterInfo.h"
#include "llvm/Target/TargetInstrInfo.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringExtras.h"

using namespace llvm;

void MSPUAsmPrinter::EmitEndOfAsmFile(Module &M)
{
    const TargetLoweringObjectFileELF &TLOFELF =
      static_cast<const TargetLoweringObjectFileELF &>(getObjFileLowering());

    MachineModuleInfoELF &MMIELF = MMI->getObjFileInfo<MachineModuleInfoELF>();

    // Output stubs for external and common global variables.
    MachineModuleInfoELF::SymbolListTy Stubs = MMIELF.GetGVStubList();
    if (!Stubs.empty()) {
      OutStreamer.SwitchSection(TLOFELF.getDataRelSection());
      unsigned PtrSize =
          TM.getSubtargetImpl()->getDataLayout()->getPointerSize(0);

      for (unsigned i = 0, e = Stubs.size(); i != e; ++i) {
        OutStreamer.EmitLabel(Stubs[i].first);
        OutStreamer.EmitSymbolValue(Stubs[i].second.getPointer(), PtrSize);
      }
      Stubs.clear();
    }
}

void MSPUAsmPrinter::EmitStartOfAsmFile(Module &M) {
  // Use unified assembler syntax.
}

// isBlockOnlyReachableByFallthrough - We need to override this since the
// default AsmPrinter does not print labels for any basic block that
// is only reachable by a fall through. That works for all cases except
// for the case in which the basic block is reachable by a fall through but
// through an indirect from a jump table. In this case, the jump table
// will contain a label not defined by AsmPrinter.
//
bool
MSPUAsmPrinter::isBlockOnlyReachableByFallthrough(const MachineBasicBlock *MBB) const
{
	if(MBB->hasAddressTaken()) {
		return false;
	}
	return AsmPrinter::isBlockOnlyReachableByFallthrough(MBB);
}

/// printMachineInstruction -- Print out a single MSPU MI to
/// the current output stream.
/// Note: Caller of this function uses bundle iterator.
void
MSPUAsmPrinter::EmitInstruction(const MachineInstr *MI)
{
	if(MI->isBundle()) {
		std::vector<const MachineInstr*> BundleMIs;

		const MachineBasicBlock *MBB = MI->getParent();
		MachineBasicBlock::const_instr_iterator MII = MI;
		++MII;
		unsigned int IgnoreCount = 0;

		// scan MachineInstr list and collect all the machine instructions into a vector.
		while(MII != MBB->end() && MII->isInsideBundle()) {
			const MachineInstr *MInst = MII;
			if(MInst->getOpcode() == TargetOpcode::DBG_VALUE ||
					MInst->getOpcode() == TargetOpcode::IMPLICIT_DEF) {
				IgnoreCount++;
				++MII;
				continue;
			}

			BundleMIs.push_back(MInst);
			++MII;
		}

		unsigned Size = BundleMIs.size();
		assert((Size+IgnoreCount) == MI->getBundleSize() && "Corrupt Bundle!");
		// tag each machine instr if it is inside bundle.
		for(unsigned Index = 0; Index < Size; Index++) {
		  MSPUMCInst MCI;
			MSPULowerToMC(BundleMIs[Index], MCI, *this);
      MCI.setStart(Index == 0);
      MCI.setEnd(Index == (Size - 1));
      EmitToStreamer(OutStreamer, MCI);
		}
	}
	else { // a solo instruction
	  MSPUMCInst MCI;
		MSPULowerToMC(MI, MCI, *this);
		MCI.setStart(true);
		MCI.setEnd(true);
    EmitToStreamer(OutStreamer, MCI);
	}

	return;
}

static MCInstPrinter *
createMSPUMCInstPrinter(const Target &T,
						unsigned SyntaxVariant,
						const MCAsmInfo &MAI,
						const MCInstrInfo &MII,
						const MCRegisterInfo &MRI,
						const MCSubtargetInfo &STI)
{
	if(SyntaxVariant == 0)
		return (new MSPUInstPrinter(MAI, MII, MRI));
	else
		return NULL;
}

extern "C" void LLVMInitializeMSPUAsmPrinter()
{
	RegisterAsmPrinter<MSPUAsmPrinter> X(TheMSPUTarget);

	TargetRegistry::RegisterMCInstPrinter(TheMSPUTarget,
											createMSPUMCInstPrinter);
}
