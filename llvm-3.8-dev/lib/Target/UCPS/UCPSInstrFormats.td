//===-- UCPSInstrFormats.td - UCPS Instruction Formats ---*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

/* a common instruction template base for UCPS */
class UCPSInst<dag ins, dag outs,
                   string asmstr,
                   list<dag> pattern = []> : Instruction {
  let Namespace = "UCPS";
  let OutOperandList = outs;
  let InOperandList = ins;
  let Pattern = pattern;
  let AsmString = asmstr;
  let Size = 6;
  let DecoderNamespace = "UCPSDecode";
  field bits<48> Inst;
  field bits<48> SoftFail;
}
