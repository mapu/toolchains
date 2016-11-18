//===-- MSPU.td - Describe the MSPU Target Machine -------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Target-independent interfaces which we are implementing
//===----------------------------------------------------------------------===//

include "llvm/Target/Target.td"

//===----------------------------------------------------------------------===//
// MSPU Subtarget features.
//===----------------------------------------------------------------------===//
/*
class SubtargetFeature<string name, string attr,  string value, 
	  				   string desc, list<SubtargetFeature> implies = []> 
*/
// each def describes a feature. Subtarget class composes of A FEW of features.
// 'attr' is a class attribute (the counterpart of class method) for 
// subclasses of Subtarget. 'true' is its initial value.
// See the definition of class MSPUSubtarget.

// How to use SubtargetFeature name??? -mattr=xxx
/*
def FeatureV9: SubtargetFeature<"v9", "IsV9", "true", 
			   				    "Enable MSPU-V9 instructions">;
*/

//===----------------------------------------------------------------------===//
// Register File, Calling Conv, Instruction Descriptions
//===----------------------------------------------------------------------===//
include "MSPUSchedule.td"
include "MSPURegisterInfo.td"
include "MSPUCallingConv.td"
include "MSPUInstrInfo.td"

// class InstrInfo {  bit isLittleEndianEncoding = 0;}
// Target can specify its instructions in either big- or little-endian formats.
// For instance, while both Sparc and PowerPC are big-endian platforms, the
// Sparc manual specifies its instructions in the format [31..0] (big), while
// PowerPC specifies them using the format [0..31] (little).
let isLittleEndianEncoding=0 in
def MSPUInstrInfo : InstrInfo;

//===----------------------------------------------------------------------===//
// MSPU processors supported.
//===----------------------------------------------------------------------===//
// 'MSPUItineraries' is a def in MSPUSchedule.td
class MSPUProcessor<string name, list<SubtargetFeature> features = []> : 
	  		ProcessorModel<name, MSPUSchedModel, features>;

def : MSPUProcessor<"mspu">; // -mcpu=xxxxx

// MSPU Uses the MC printer for assembler output, so make sure the TableGen
// AsmWriter bits get associated with the correct class.
def MSPUAsmWriter : AsmWriter {
  let AsmWriterClassName  = "InstPrinter";
}

def MSPUAsmParser : AsmParser {
  let ShouldEmitMatchRegisterName = 0;
}

//===----------------------------------------------------------------------===//
// Declare the target which we are implementing
//===----------------------------------------------------------------------===//
def MSPU : Target {
  let InstructionSet = MSPUInstrInfo;
  let AssemblyWriters = [MSPUAsmWriter];
  let AssemblyParsers = [MSPUAsmParser];
}
