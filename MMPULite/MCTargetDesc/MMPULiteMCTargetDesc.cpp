//===-- MMPULiteMCTargetDesc.cpp - MMPULite Target Descriptions -----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file provides MMPULite specific target descriptions.
//
//===----------------------------------------------------------------------===//

#include "MMPULiteMCTargetDesc.h"
#include "MMPULiteMCAsmInfo.h"
#include "MMPULiteTargetStreamer.h"
#include "InstPrinter/MMPULiteInstPrinter.h"
#include "llvm/MC/MCELF.h"
#include "llvm/MC/MCELFStreamer.h"
#include "llvm/MC/MCCodeGenInfo.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MachineLocation.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/TargetRegistry.h"

#define GET_INSTRINFO_MC_DESC
#include "MMPULiteGenInstrInfo.inc"

#define GET_SUBTARGETINFO_MC_DESC
#include "MMPULiteGenSubtargetInfo.inc"

#define GET_REGINFO_MC_DESC
#include "MMPULiteGenRegisterInfo.inc"

using namespace llvm;

static MCInstrInfo *
createMMPULiteMCInstrInfo()
{
	MCInstrInfo *X = new MCInstrInfo();
	InitMMPULiteMCInstrInfo(X);
	return X;
}

static MCRegisterInfo *
createMMPULiteMCRegisterInfo(StringRef TT)
{
	MCRegisterInfo *X = new MCRegisterInfo();
	InitMMPULiteMCRegisterInfo(X, 0); // 0 is used to disguise return address register.
	return X;
}

static MCSubtargetInfo *
createMMPULiteMCSubtargetInfo(StringRef TT, StringRef CPU,
							StringRef FS)
{
	MCSubtargetInfo *X = new MCSubtargetInfo();
	InitMMPULiteMCSubtargetInfo(X, TT, CPU, FS);
	return X;
}

static MCInstPrinter *createMMPULiteMCInstPrinter(const Target &T,
                                              unsigned SyntaxVariant,
                                              const MCAsmInfo &MAI,
                                              const MCInstrInfo &MII,
                                              const MCRegisterInfo &MRI,
                                              const MCSubtargetInfo &STI) {
  return new MMPULiteInstPrinter(MAI, MII, MRI);
}

static MCCodeGenInfo *
createMMPULiteMCCodeGenInfo(StringRef TT, Reloc::Model RM,
						CodeModel::Model CM,
						CodeGenOpt::Level OL)
{
	MCCodeGenInfo *X = new MCCodeGenInfo();
	X->InitMCCodeGenInfo(RM, CM, OL);
	return X;
}

static MCStreamer *
createMMPULiteMCStreamer(const Target &T, StringRef TT,
			 MCContext &Ctx, MCAsmBackend &MAB,
                         raw_ostream &_OS, MCCodeEmitter *_Emitter,
                         /*const MCSubtargetInfo &STI, */bool RelaxAll,
                         bool NoExecStack)
{
	Triple TheTriple(TT);

        MCStreamer *S =
          createELFStreamer(Ctx, 0, MAB, _OS, _Emitter, RelaxAll, NoExecStack);

	if(TheTriple.isOSDarwin()) {
		llvm_unreachable("MMPULite does not support Darwin MACH-O format");
	}

	if(TheTriple.isOSWindows()) {
		llvm_unreachable("MMPULite does not support Windows COFF format");
	}
        //new MMPULiteTargetELFStreamer(*S);
	return S;
}

static MCRelocationInfo *createMMPULiteMCRelocationInfo(StringRef TT,
                                                         MCContext &Ctx) {
  Triple TheTriple(TT);
  //if (TheTriple.isOSBinFormatELF())
    return createMMPULiteELFRelocationInfo(Ctx);
  // Default to the stock relocation info.
  return llvm::createMCRelocationInfo(TT, Ctx);
}

extern "C" void
LLVMInitializeMMPULiteTargetMC()
{
	// Register the MC asm info.
	RegisterMCAsmInfo<MMPULiteELFMCAsmInfo> X(TheMMPULiteTarget);

	// Register the MC codegen info.
	TargetRegistry::RegisterMCCodeGenInfo(TheMMPULiteTarget, createMMPULiteMCCodeGenInfo);

	// Register the MC instruction info.
	TargetRegistry::RegisterMCInstrInfo(TheMMPULiteTarget, createMMPULiteMCInstrInfo);

	// Register the MC register info.
	TargetRegistry::RegisterMCRegInfo(TheMMPULiteTarget, createMMPULiteMCRegisterInfo);

	// Register the MC subtarget info.
	TargetRegistry::RegisterMCSubtargetInfo(TheMMPULiteTarget, createMMPULiteMCSubtargetInfo);

	// Register the MC code emitter
	TargetRegistry::RegisterMCCodeEmitter(TheMMPULiteTarget, createMMPULiteMCCodeEmitter);

	// Register the asm backend
	TargetRegistry::RegisterMCAsmBackend(TheMMPULiteTarget, createMMPULiteAsmBackend);

	// Register the object streamer
	TargetRegistry::RegisterMCObjectStreamer(TheMMPULiteTarget, createMMPULiteMCStreamer);

	TargetRegistry::RegisterMCInstPrinter(TheMMPULiteTarget, createMMPULiteMCInstPrinter);
  
  // Register the MC relocation info.
  TargetRegistry::RegisterMCRelocationInfo(TheMMPULiteTarget, createMMPULiteMCRelocationInfo);
}
