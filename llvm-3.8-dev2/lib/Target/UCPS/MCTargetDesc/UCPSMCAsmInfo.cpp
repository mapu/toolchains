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

UCPSAsmInfo::UCPSAsmInfo(const Triple &TT)
{
	IsLittleEndian = true;
	//IsMaPU = true;
	StackGrowsUp = false;
	HasStaticCtorDtorReferenceInStaticMode=true;
	Triple TheTriple(TT);

	Data8bitsDirective = "\t.byte\t";
	Data16bitsDirective = "\t.short\t";
	Data32bitsDirective = "\t.int\t";
	Data64bitsDirective = "\t.quad\t";

	ZeroDirective = "\t.zero\t"; // .space is used to point last byte reserved
	CommentString = "#";
	SeparatorString = ";" ;
	SupportsDebugInformation = true;

	GlobalDirective="\t.global\t";

	PrivateGlobalPrefix = "L";
	PointerSize=4;

	TextAlignFillValue = (uint32_t) 0x800000;
        WeakRefDirective = "\t.weak\t";
        ExceptionsType = ExceptionHandling::DwarfCFI;
}
