//===-- UCPMMCTargetDesc.cpp - UCPM Target Descriptions -----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file provides UCPM specific target descriptions.
//
//===----------------------------------------------------------------------===//

#include "UCPMMCTargetDesc.h"
#include "UCPMMCAsmInfo.h"
#include "InstPrinter/UCPMInstPrinter.h"
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
#include "UCPMGenInstrInfo.inc"

#define GET_SUBTARGETINFO_MC_DESC
#include "UCPMGenSubtargetInfo.inc"

#define GET_REGINFO_MC_DESC
#include "UCPMGenRegisterInfo.inc"

using namespace llvm;

static MCInstrInfo *
createUCPMMCInstrInfo() {
  MCInstrInfo *X = new MCInstrInfo();
  InitUCPMMCInstrInfo(X);
  return X;
}

static MCRegisterInfo *
createUCPMMCRegisterInfo(const Triple &TT) {
  MCRegisterInfo *X = new MCRegisterInfo();
  InitUCPMMCRegisterInfo(X, 0);    // 0 is used to disguise return address register.
  return X;
}

static MCSubtargetInfo *
createUCPMMCSubtargetInfo(const Triple &TT, StringRef CPU, StringRef FS) {
  return createUCPMMCSubtargetInfoImpl(TT, CPU, FS);
}

static MCInstPrinter *createUCPMMCInstPrinter(const Triple &TT,
                                                  unsigned SyntaxVariant,
                                                  const MCAsmInfo &MAI,
                                                  const MCInstrInfo &MII,
                                                  const MCRegisterInfo &MRI) {
  return new UCPMInstPrinter(MAI, MII, MRI);
}

static MCCodeGenInfo *
createUCPMMCCodeGenInfo(const Triple &TT, Reloc::Model RM,
                            CodeModel::Model CM, CodeGenOpt::Level OL) {
  MCCodeGenInfo *X = new MCCodeGenInfo();
  X->initMCCodeGenInfo(RM, CM, OL);
  return X;
}

static MCStreamer *
createUCPMMCStreamer(const Triple &TT, MCContext &Ctx, MCAsmBackend &MAB,
                         raw_pwrite_stream &_OS, MCCodeEmitter *_Emitter,
                         bool RelaxAll) {
  MCStreamer *S = createELFStreamer(Ctx, MAB, _OS, _Emitter, RelaxAll);

  if (TT.isOSDarwin()) {
    llvm_unreachable("UCPM does not support Darwin MACH-O format");
  }

  if (TT.isOSWindows()) {
    llvm_unreachable("UCPM does not support Windows COFF format");
  }
  return S;
}

static MCRelocationInfo *createUCPMMCRelocationInfo(const Triple &TT,
                                                        MCContext &Ctx) {
  //if (TheTriple.isOSBinFormatELF())
  //return createUCPMELFRelocationInfo(Ctx);
  // Default to the stock relocation info.
  return llvm::createMCRelocationInfo(TT, Ctx);
}

static MCAsmInfo *createUCPMAsmInfo(const MCRegisterInfo &MRI,
                                    const Triple &TT) {
  MCAsmInfo *MAI = new UCPMELFMCAsmInfo(TT);
  return MAI;
}

extern "C" void LLVMInitializeUCPMTargetMC() {
  // Register the MC asm info.
  RegisterMCAsmInfoFn X1(TheUCPMTarget, createUCPMAsmInfo);

  // Register the MC codegen info.
  TargetRegistry::RegisterMCCodeGenInfo(TheUCPMTarget,
                                        createUCPMMCCodeGenInfo);

  // Register the MC instruction info.
  TargetRegistry::RegisterMCInstrInfo(TheUCPMTarget,
                                      createUCPMMCInstrInfo);

  // Register the MC register info.
  TargetRegistry::RegisterMCRegInfo(TheUCPMTarget,
                                    createUCPMMCRegisterInfo);

  // Register the MC subtarget info.
  TargetRegistry::RegisterMCSubtargetInfo(TheUCPMTarget,
                                          createUCPMMCSubtargetInfo);

  // Register the MC code emitter
  TargetRegistry::RegisterMCCodeEmitter(TheUCPMTarget,
                                        createUCPMMCCodeEmitter);

  // Register the asm backend
  TargetRegistry::RegisterMCAsmBackend(TheUCPMTarget,
                                       createUCPMAsmBackend);

  // Register the object streamer
  TargetRegistry::RegisterELFStreamer(TheUCPMTarget,
                                      createUCPMMCStreamer);

  TargetRegistry::RegisterMCInstPrinter(TheUCPMTarget,
                                        createUCPMMCInstPrinter);
}
