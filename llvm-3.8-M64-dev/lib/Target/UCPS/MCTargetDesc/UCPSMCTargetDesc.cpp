//===-- UCPSMCTargetDesc.cpp - UCPS Target Descriptions -----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file provides UCPS specific target descriptions.
//
//===----------------------------------------------------------------------===//

#include "UCPSMCTargetDesc.h"
#include "UCPSMCAsmInfo.h"
#include "InstPrinter/UCPSInstPrinter.h"
//#include "llvm/MC/MCELF.h"
//#include "llvm/MC/MCELFStreamer.h"
#include "llvm/MC/MCCodeGenInfo.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MachineLocation.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/TargetRegistry.h"

#define GET_INSTRINFO_MC_DESC
#include "UCPSGenInstrInfo.inc"

#define GET_SUBTARGETINFO_MC_DESC
#include "UCPSGenSubtargetInfo.inc"

#define GET_REGINFO_MC_DESC
#include "UCPSGenRegisterInfo.inc"

using namespace llvm;

static MCInstrInfo *
createUCPSMCInstrInfo() {
  MCInstrInfo *X = new MCInstrInfo();
  InitUCPSMCInstrInfo(X);
  return X;
}

static MCRegisterInfo *
createUCPSMCRegisterInfo(const Triple &TT) {
  MCRegisterInfo *X = new MCRegisterInfo();
  InitUCPSMCRegisterInfo(X, 0);    // 0 is used to disguise return address register.
  return X;
}

static MCSubtargetInfo *
createUCPSMCSubtargetInfo(const Triple &TT, StringRef CPU, StringRef FS) {
  return createUCPSMCSubtargetInfoImpl(TT, CPU, FS);
}

static MCInstPrinter *createUCPSMCInstPrinter(const Triple &TT,
                                                  unsigned SyntaxVariant,
                                                  const MCAsmInfo &MAI,
                                                  const MCInstrInfo &MII,
                                                  const MCRegisterInfo &MRI) {
  return new UCPSInstPrinter(MAI, MII, MRI);
}

static MCCodeGenInfo *
createUCPSMCCodeGenInfo(const Triple &TT, Reloc::Model RM,
                            CodeModel::Model CM, CodeGenOpt::Level OL) {
  MCCodeGenInfo *X = new MCCodeGenInfo();
  X->initMCCodeGenInfo(RM, CM, OL);
  return X;
}

static MCStreamer *
createUCPSMCStreamer(const Triple &TT, MCContext &Ctx, MCAsmBackend &MAB,
                         raw_pwrite_stream &_OS, MCCodeEmitter *_Emitter,
                         bool RelaxAll) {
  MCStreamer *S = createELFStreamer(Ctx, MAB, _OS, _Emitter, RelaxAll);

  if (TT.isOSDarwin()) {
    llvm_unreachable("UCPS does not support Darwin MACH-O format");
  }

  if (TT.isOSWindows()) {
    llvm_unreachable("UCPS does not support Windows COFF format");
  }
  return S;
}

static MCRelocationInfo *createUCPSMCRelocationInfo(const Triple &TT,
                                                        MCContext &Ctx) {
  //if (TheTriple.isOSBinFormatELF())
  //return createUCPSELFRelocationInfo(Ctx);
  // Default to the stock relocation info.
  return llvm::createMCRelocationInfo(TT, Ctx);
}

static MCAsmInfo *createUCPSAsmInfo(const MCRegisterInfo &MRI,
                                    const Triple &TT) {
  MCAsmInfo *MAI = new UCPSELFMCAsmInfo(TT);
  return MAI;
}

extern "C" void LLVMInitializeUCPSTargetMC() {
  // Register the MC asm info.
  RegisterMCAsmInfoFn X1(TheUCPSTarget, createUCPSAsmInfo);

  // Register the MC codegen info.
  TargetRegistry::RegisterMCCodeGenInfo(TheUCPSTarget,
                                        createUCPSMCCodeGenInfo);

  // Register the MC instruction info.
  TargetRegistry::RegisterMCInstrInfo(TheUCPSTarget,
                                      createUCPSMCInstrInfo);

  // Register the MC register info.
  TargetRegistry::RegisterMCRegInfo(TheUCPSTarget,
                                    createUCPSMCRegisterInfo);

  // Register the MC subtarget info.
  TargetRegistry::RegisterMCSubtargetInfo(TheUCPSTarget,
                                          createUCPSMCSubtargetInfo);

  // Register the MC code emitter
  TargetRegistry::RegisterMCCodeEmitter(TheUCPSTarget,
                                        createUCPSMCCodeEmitter);

  // Register the asm backend
  TargetRegistry::RegisterMCAsmBackend(TheUCPSTarget,
                                       createUCPSAsmBackend);

  // Register the object streamer
  TargetRegistry::RegisterELFStreamer(TheUCPSTarget,
                                      createUCPSMCStreamer);

  TargetRegistry::RegisterMCInstPrinter(TheUCPSTarget,
                                        createUCPSMCInstPrinter);
}
