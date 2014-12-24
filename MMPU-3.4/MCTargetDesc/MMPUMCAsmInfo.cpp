//===-- MMPUMCAsmInfo.cpp - MMPU asm properties -------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the declarations of the MMPUMCAsmInfo properties.
//
//===----------------------------------------------------------------------===//

#include "MMPUMCAsmInfo.h"
#include "llvm/ADT/Triple.h"

using namespace llvm;

MMPUMCAsmInfo::MMPUMCAsmInfo(StringRef TT)
{
	IsLittleEndian = true;
	IsMaPU = true;
	StackGrowsUp = false;
	HasStaticCtorDtorReferenceInStaticMode=true;
	Triple TheTriple(TT);

	Data8bitsDirective = "\t.byte\t";
	Data16bitsDirective = "\t.half\t";
	Data32bitsDirective = "\t.word\t";
	Data64bitsDirective = "\t.quad\t";

	ZeroDirective = "\t.bes\t"; // .space is used to point last byte reserved
	CommentString = "#";
	SupportsDebugInformation = true;
	SeparatorString = ";;";

	//ExternDirective = "\t.ref\t";
	GlobalDirective="\t.global\t";

	PrivateGlobalPrefix = "L";
	PointerSize=4;
}
