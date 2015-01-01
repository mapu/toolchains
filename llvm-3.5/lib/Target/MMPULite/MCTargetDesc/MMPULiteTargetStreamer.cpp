//===-- MMPULiteTargetStreamer.cpp - MMPULite Target Streamer Methods -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file provides MMPULite specific target streamer methods.
//
//===----------------------------------------------------------------------===//

#include "MMPULiteTargetStreamer.h"
#include "InstPrinter/MMPULiteInstPrinter.h"
#include "llvm/Support/FormattedStream.h"

using namespace llvm;

// pin vtable to this file
MMPULiteTargetStreamer::MMPULiteTargetStreamer(MCStreamer &S) : MCTargetStreamer(S) {}

void MMPULiteTargetStreamer::anchor() {}

MMPULiteTargetAsmStreamer::MMPULiteTargetAsmStreamer(MCStreamer &S,
                                               formatted_raw_ostream &OS)
    : MMPULiteTargetStreamer(S), OS(OS) {}

void MMPULiteTargetAsmStreamer::emitMMPULiteRegisterIgnore(unsigned reg) {
  OS << "\t.register "
     << "%" << StringRef(MMPULiteInstPrinter::getRegisterName(reg)).lower()
     << ", #ignore\n";
}

void MMPULiteTargetAsmStreamer::emitMMPULiteRegisterScratch(unsigned reg) {
  OS << "\t.register "
     << "%" << StringRef(MMPULiteInstPrinter::getRegisterName(reg)).lower()
     << ", #scratch\n";
}

MMPULiteTargetELFStreamer::MMPULiteTargetELFStreamer(MCStreamer &S)
    : MMPULiteTargetStreamer(S) {}

MCELFStreamer &MMPULiteTargetELFStreamer::getStreamer() {
  return static_cast<MCELFStreamer &>(Streamer);
}
