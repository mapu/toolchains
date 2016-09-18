//===-- UCPSMCAsmInfo.cpp - UCPS asm properties -------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the declarations of the UCPSMCAsmInfo properties.
//
//===----------------------------------------------------------------------===//

#include "UCPSMCAsmInfo.h"
#include "llvm/ADT/Triple.h"

using namespace llvm;

UCPSELFMCAsmInfo::UCPSELFMCAsmInfo(const Triple &TT)
{
	IsLittleEndian = true;
	StackGrowsUp = false;
	HasStaticCtorDtorReferenceInStaticMode=true;

	Data8bitsDirective = "\t.byte\t";
	Data16bitsDirective = "\t.half\t";
	Data32bitsDirective = "\t.word\t";
	Data64bitsDirective = "\t.quad\t";

	ZeroDirective = "\t.bes\t"; // .space is used to point last byte reserved
	CommentString = "#";
	SupportsDebugInformation = true;

	GlobalDirective="\t.global\t";

	PrivateGlobalPrefix = "L";
	PointerSize=4;
}
