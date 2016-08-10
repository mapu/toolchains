//===- UCPMInstPrinter.cpp - Convert UCPM MCInst to assembly syntax -===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This class prints an UCPM MCInst to a .s file.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "asm-printer"
#include "../UCPM.h"
//#include "../UCPMInsnFlags.h"
#include "UCPMInstPrinter.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

STATISTIC(MCNumEmitted, "Number of MC instructions emitted");
STATISTIC(MCLineNumEmitted, "Number of MC instruction lines emitted");

#include "UCPMGenAsmWriter.inc"

void UCPMInstPrinter::printInst(const MCInst *MI, raw_ostream &O,
                                    StringRef Annot, const MCSubtargetInfo &STI) {
	O<<"\t"; // at start of line

	bool isAllNOP = true; // test whether all inst are NOP
	const MCInst *curinst = MI;
	std::string split = "";
	do {
	  if (curinst == MI) 
	    curinst = curinst->getOperand(0).getInst();
	  else
	    curinst = curinst->getOperand(curinst->getNumOperands()-1).getInst();
	 /*yangl if (curinst->getOpcode() != UCPM::NOP) {
	    O << split;
      printInstruction(curinst, O);
      ++MCNumEmitted;
	    split = "|| ";
	    isAllNOP = false;
	  }*/
	} while (curinst->getNumOperands() && curinst->getOperand(curinst->getNumOperands()-1).isInst());
	if(isAllNOP) {
		O<<"NOP";
	}
  ++MCLineNumEmitted;
	O << ";";
	printAnnotation(O, Annot);

	return;
}

void UCPMInstPrinter::printOperand(const MCInst *MI, unsigned OpNo,
                                       raw_ostream &O) const {
	const MCOperand& MO = MI->getOperand(OpNo);

	if(MO.isReg()) {
		O << getRegisterName(MO.getReg());
	}
	else if(MO.isExpr()) {
		O << *(MO.getExpr());
	}
	else if(MO.isImm()) 
		switch(MI->getOpcode()) {
		/*case UCPM::LPTO:
		case UCPM::JUMP:
		case UCPM::JUMPK:
      if (OpNo == 0)
		    O << formatImm(MO.getImm() * 41);
      else
        O << MO.getImm();
		  break;*/
		default:
		  O << MO.getImm();
		  break;
		}
	else {
		assert(false && "Unknown operand");
	}
}

void UCPMInstPrinter::printFlags(const MCInst *MI, unsigned OpNo,
                                     raw_ostream &O) const {
	const MCOperand& MO = MI->getOperand(OpNo);

	const unsigned BF=0, HF=1, UF=2, TF=3, SF=4, DF=5, IF=6, LF=7, APPF=8,\
	  KPPF=9, CRF=10, BRF=11, MF=12, TCF=13;

	int64_t flags = MO.getImm() >> 8;
	std::string comma = "";
	if(flags) {
	  O << "(";
	  if(flags & (1 << BF)) {O << comma << "B"; comma = ", ";}
	  if(flags & (1 << HF)) {O << comma << "H"; comma = ", ";}
	  if(flags & (1 << UF)) {O << comma << "U"; comma = ", ";}
	  if(flags & (1 << TF)) {O << comma << "T"; comma = ", ";}
	  if(flags & (1 << SF)) {O << comma << "S"; comma = ", ";}
	  if(flags & (1 << DF)) {O << comma << "D"; comma = ", ";}
	  if(flags & (1 << IF)) {O << comma << "I"; comma = ", ";}
	  if(flags & (1 << LF)) {O << comma << "L"; comma = ", ";}
	  if(flags & (1 << APPF)) {O << comma << "A++"; comma = ", ";}
	  if(flags & (1 << KPPF)) {O << comma << "K++"; comma = ", ";}
	  if(flags & (1 << CRF)) {O << comma << "CR"; comma = ", ";}
	  if(flags & (1 << BRF)) {O << comma << "BR"; comma = ", ";}
	  if(flags & (1 << MF)) {O << comma << "M"; comma = ", ";}
	  if(flags & (1 << TCF)) {O << comma << "TC"; comma = ", ";}
	  O << ")";
	}
}

void UCPMInstPrinter::printKIReg(const MCInst *MI, unsigned OpNo,
                                     raw_ostream &O) const {
	const MCOperand& MO = MI->getOperand(OpNo);

	unsigned kis = (MO.getImm()) & 0x3F;
        if (kis & 0x10) O << "!";
	O << "KI" << (kis & 0xF);
	kis = (MO.getImm()>>5) & 0x3F;
        if (kis) {
	  O << "&";
	  if (kis & 0x10) O << "!";
	  O << "KI" << (kis & 0xF);
	}
	kis = (MO.getImm()>>11) & 0x3F;
        if (kis) {
	  O << "&";
	  if (kis & 0x10) O << "!";
	  O << "KI" << (kis & 0xF);
	}
	kis = MO.getImm()>>17;
        if (kis) {
	  O << "&";
	  if (kis & 0x10) O << "!";
	  O << "KI" << (kis & 0xF);
	}
}

void UCPMInstPrinter::printIPath(const MCInst *MI, unsigned OpNo,
                                     raw_ostream &O) const {
	const MCOperand& MO = MI->getOperand(OpNo);

	unsigned path = (MO.getImm()) & 0x3;
	if (path)
	  O << "(I" << path - 1 << ")";
}

void UCPMInstPrinter::printMACCTI(const MCInst *MI,
                                      unsigned int OpNo,
                                      raw_ostream &O) const {
	printOperand(MI, OpNo, O);
  O << ".";
  printOperand(MI, OpNo + 1, O);
  printIPath(MI, OpNo + 2, O);
}

void UCPMInstPrinter::printSHUT(const MCInst *MI,
                                    unsigned int OpNo,
                                    raw_ostream &O) const {
	printOperand(MI, OpNo, O);
  O << ".";
  printOperand(MI, OpNo + 1, O);
}
