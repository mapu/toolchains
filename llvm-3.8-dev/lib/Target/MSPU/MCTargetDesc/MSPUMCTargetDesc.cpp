//===-- MSPUMCTargetDesc.cpp - MSPU Target Descriptions -----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file provides MSPU specific target descriptions.
//
//===----------------------------------------------------------------------===//

#include "MSPUMCTargetDesc.h"
#include "MSPUMCAsmInfo.h"
#include "InstPrinter/MSPUInstPrinter.h"
#include "llvm/MC/MCCodeGenInfo.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/TargetRegistry.h"

#define GET_INSTRINFO_MC_DESC
#include "MSPUGenInstrInfo.inc"

#define GET_SUBTARGETINFO_MC_DESC
#include "MSPUGenSubtargetInfo.inc"

#define GET_REGINFO_MC_DESC
#include "MSPUGenRegisterInfo.inc"

using namespace llvm;

static MCInstrInfo *createMSPUMCInstrInfo() {
	MCInstrInfo *X = new MCInstrInfo();
	InitMSPUMCInstrInfo(X);
	return X;
}

static MCRegisterInfo *createMSPUMCRegisterInfo(const Triple &TT) {
	MCRegisterInfo *X = new MCRegisterInfo();
	InitMSPUMCRegisterInfo(X, MSPUReg::J30); // J30 is return address register.
	return X;
}

static MCAsmInfo *createMSPUAsmInfo(const MCRegisterInfo &MRI,
                                    const Triple &TT) {
  MCAsmInfo *MAI = new MSPUAsmInfo(TT);
  return MAI;
}

static MCCodeGenInfo *
createMSPUMCCodeGenInfo(const Triple &TT, Reloc::Model RM, CodeModel::Model CM,
					            	CodeGenOpt::Level OL) {
	MCCodeGenInfo *X = new MCCodeGenInfo();
	X->initMCCodeGenInfo(RM, CM, OL);
	return X;
}

static MCStreamer *createMSPUMCStreamer(const Triple &TT,
                                        MCContext &Ctx, MCAsmBackend &MAB,
                                        raw_pwrite_stream &_OS,
                                        MCCodeEmitter *_Emitter,
                                        bool RelaxAll) {
	if(TT.isOSDarwin()) {
		llvm_unreachable("MSPU does not support Darwin MACH-O format");
	}

	if(TT.isOSWindows()) {
		llvm_unreachable("MSPU does not support Windows COFF format");
	}

	/* RelaxAll: used in MCObjectStreamer::EmitInstruction()
	 * If true, relax an instruction before we emit it.
	 * Otherwise, we perform relaxation later until all symbols that
	 * can be resolved have already been resolved. This is done via emitting
	 * instructions to *fragment* and then relax this fragment.
	 * For spu, earlier relaxation of imm-extension intructions forbids *RelaxAll*
	 * since it will bring a wrong fixup offset.
	 * So, we relax an instruction only when we can not resolve it and we hand over it
	 * to linker.
	 */

	return createELFStreamer(Ctx, MAB, _OS, _Emitter, /*RelaxAll*/ false);
}

static MCInstPrinter *createMSPUMCInstPrinter(const Triple &TT,
                                              unsigned SyntaxVariant,
                                              const MCAsmInfo &MAI,
                                              const MCInstrInfo &MII,
                                              const MCRegisterInfo &MRI) {
  return new MSPUInstPrinter(MAI, MII, MRI);
}

static MCSubtargetInfo *
createMSPUMCSubtargetInfo(const Triple &TT, StringRef CPU, StringRef FS) {
  return createMSPUMCSubtargetInfoImpl(TT, CPU, FS);
}

extern "C" void LLVMInitializeMSPUTargetMC() {
  // Register the MC asm info.
  RegisterMCAsmInfoFn X1(TheMSPUTarget, createMSPUAsmInfo);

	// Register the MC codegen info.
	TargetRegistry::RegisterMCCodeGenInfo(TheMSPUTarget, createMSPUMCCodeGenInfo);

	// Register the MC instruction info.
	TargetRegistry::RegisterMCInstrInfo(TheMSPUTarget, createMSPUMCInstrInfo);

	// Register the MC register info.
	TargetRegistry::RegisterMCRegInfo(TheMSPUTarget, createMSPUMCRegisterInfo);

	// Register the MC code emitter
	TargetRegistry::RegisterMCCodeEmitter(TheMSPUTarget, createMSPUMCCodeEmitter);

  // Register the MC subtarget info.
  TargetRegistry::RegisterMCSubtargetInfo(TheMSPUTarget,
                                          createMSPUMCSubtargetInfo);

	// Register the asm backend
	TargetRegistry::RegisterMCAsmBackend(TheMSPUTarget, createMSPUAsmBackend);

	// Register the object streamer
	TargetRegistry::RegisterELFStreamer(TheMSPUTarget, createMSPUMCStreamer);

	TargetRegistry::RegisterMCInstPrinter(TheMSPUTarget, createMSPUMCInstPrinter);
}
