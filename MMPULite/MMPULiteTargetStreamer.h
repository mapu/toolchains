//===-- MMPULiteTargetStreamer.h - MMPULite Target Streamer ----------*- C++ -*--===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef MMPULITETARGETSTREAMER_H
#define MMPULITETARGETSTREAMER_H

#include "llvm/MC/MCELFStreamer.h"
#include "llvm/MC/MCStreamer.h"

namespace llvm {
class MMPULiteTargetStreamer : public MCTargetStreamer {
  virtual void anchor();

public:
  MMPULiteTargetStreamer(MCStreamer &S);
  /// Emit ".register <reg>, #ignore".
  virtual void emitMMPULiteRegisterIgnore(unsigned reg) = 0;
  /// Emit ".register <reg>, #scratch".
  virtual void emitMMPULiteRegisterScratch(unsigned reg) = 0;
};

// This part is for ascii assembly output
class MMPULiteTargetAsmStreamer : public MMPULiteTargetStreamer {
  formatted_raw_ostream &OS;

public:
  MMPULiteTargetAsmStreamer(MCStreamer &S, formatted_raw_ostream &OS);
  virtual void emitMMPULiteRegisterIgnore(unsigned reg);
  virtual void emitMMPULiteRegisterScratch(unsigned reg);

};

// This part is for ELF object output
class MMPULiteTargetELFStreamer : public MMPULiteTargetStreamer {
public:
  MMPULiteTargetELFStreamer(MCStreamer &S);
  MCELFStreamer &getStreamer();
  virtual void emitMMPULiteRegisterIgnore(unsigned reg) {}
  virtual void emitMMPULiteRegisterScratch(unsigned reg) {}
};
} // end namespace llvm

#endif
