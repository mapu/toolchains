//===-- MMPUMCTargetDesc.cpp - MMPU Target Descriptions -----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file provides MMPU specific target descriptions.
//
//===----------------------------------------------------------------------===//

#include "MMPUMCTargetDesc.h"
#include "MMPUMCAsmInfo.h"
#include "InstPrinter/MMPUInstPrinter.h"
#include "llvm/MC/MCCodeGenInfo.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MachineLocation.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/TargetRegistry.h"

#define GET_INSTRINFO_MC_DESC
#include "MMPUGenInstrInfo.inc"

#define GET_SUBTARGETINFO_MC_DESC
#include "MMPUGenSubtargetInfo.inc"

#define GET_REGINFO_MC_DESC
#include "MMPUGenRegisterInfo.inc"

using namespace llvm;

static MCAsmInfo *createMMPUMCAsmInfo(const MCRegisterInfo &MRI,
                                         StringRef TT) {
  MCAsmInfo *MAI = new MMPUMCAsmInfo(TT);
  return MAI;
}

static MCInstrInfo *
createMMPUMCInstrInfo()
{
	MCInstrInfo *X = new MCInstrInfo();
	InitMMPUMCInstrInfo(X);
	return X;
}

static MCRegisterInfo *
createMMPUMCRegisterInfo(StringRef TT)
{
	MCRegisterInfo *X = new MCRegisterInfo();
	InitMMPUMCRegisterInfo(X, 0); // 0 is used to disguise return address register.
	return X;
}

static MCSubtargetInfo *
createMMPUMCSubtargetInfo(StringRef TT, StringRef CPU,
							StringRef FS)
{
	MCSubtargetInfo *X = new MCSubtargetInfo();
	InitMMPUMCSubtargetInfo(X, TT, CPU, FS);
	return X;
}

static MCInstPrinter *createMMPUMCInstPrinter(const Target &T,
                                              unsigned SyntaxVariant,
                                              const MCAsmInfo &MAI,
                                              const MCInstrInfo &MII,
                                              const MCRegisterInfo &MRI,
                                              const MCSubtargetInfo &STI) {
  return new MMPUInstPrinter(MAI, MII, MRI);
}

static MCCodeGenInfo *
createMMPUMCCodeGenInfo(StringRef TT, Reloc::Model RM,
						CodeModel::Model CM,
						CodeGenOpt::Level OL)
{
	MCCodeGenInfo *X = new MCCodeGenInfo();
	X->InitMCCodeGenInfo(RM, CM, OL);
	return X;
}

static MCStreamer *
createMMPUMCStreamer(const Target &T, StringRef TT,
					MCContext &Ctx,
					MCAsmBackend &MAB,
					raw_ostream &OS,
					MCCodeEmitter *Emitter,
					bool RelaxAll,
					bool NoExecStack)
{
	Triple TheTriple(TT);

	if(TheTriple.isOSDarwin()) {
		llvm_unreachable("MMPU does not support Darwin MACH-O format");
	}

	if(TheTriple.isOSWindows()) {
		llvm_unreachable("MMPU does not support Windows COFF format");
	}

	return createELFStreamer(Ctx, 0, MAB, OS, Emitter, RelaxAll, NoExecStack);
}

extern "C" void
LLVMInitializeMMPUTargetMC()
{
	// Register the MC asm info.
    RegisterMCAsmInfoFn X1(TheMMPUTarget, createMMPUMCAsmInfo);
    //TargetRegistry::RegisterMCAsmInfo(TheMMPUTarget, createMMPUMCAsmInfo);

	// Register the MC codegen info.
	TargetRegistry::RegisterMCCodeGenInfo(TheMMPUTarget, createMMPUMCCodeGenInfo);

	// Register the MC instruction info.
	TargetRegistry::RegisterMCInstrInfo(TheMMPUTarget, createMMPUMCInstrInfo);

	// Register the MC register info.
	TargetRegistry::RegisterMCRegInfo(TheMMPUTarget, createMMPUMCRegisterInfo);

	// Register the MC subtarget info.
	TargetRegistry::RegisterMCSubtargetInfo(TheMMPUTarget, createMMPUMCSubtargetInfo);

	// Register the MC code emitter
	TargetRegistry::RegisterMCCodeEmitter(TheMMPUTarget, createMMPUMCCodeEmitter);

	// Register the asm backend
	TargetRegistry::RegisterMCAsmBackend(TheMMPUTarget, createMMPUAsmBackend);

	// Register the object streamer
	TargetRegistry::RegisterMCObjectStreamer(TheMMPUTarget, createMMPUMCStreamer);

	TargetRegistry::RegisterMCInstPrinter(TheMMPUTarget, createMMPUMCInstPrinter);
}
