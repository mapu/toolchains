//===- MMPUInstPrinter.cpp - Convert MMPU MCInst to assembly syntax -===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This class prints an MMPU MCInst to a .s file.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "asm-printer"
#include "../MMPU.h"
#include "../MCTargetDesc/MMPUMCInst.h"
#include "MMPUInstPrinter.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/Support/raw_ostream.h"
#include <cstdio>

using namespace llvm;

static void mmpu_print_c_option(int64_t options, raw_ostream &O)
{
  if(options & MMPU::c_C) O << "(C)"; else
  if(options & MMPU::c_NC) O << "(!C)";
}

#include "MMPUGenAsmWriter.inc"

void
MMPUInstPrinter::printInst(const MCInst *MI, raw_ostream &O,
							StringRef Annot)
{
	O<<"    "; // start an instruction line

	bool isAllNOP = true; // test whether all inst are NOP
	printInstList(MI, O, isAllNOP); // a recursive function

	if(isAllNOP) {
		O<<"NOP";
	}

	O<<" ;;";

	printAnnotation(O, Annot);

	return;
}

void
MMPUInstPrinter::printInstList(const MCInst *MI, raw_ostream &O,
							bool & isAllNOP)
{
	unsigned num = MI->getNumOperands();

	if(MI->getOpcode() != MMPUInst::NOP) {
		// if we have already printed out some asm text for instructions
		if(!isAllNOP) O<<" ; ";
		printInstruction(MI, O); // tblgen'erated function

		isAllNOP &= false; // update isAllNOP before we start a new recursion.

		if(num > 0 && MI->getOperand(num - 1).isInst()) {
			printInstList(MI->getOperand(num - 1).getInst(), O, isAllNOP);
		}
	}
	else {
		isAllNOP &= true; // update isAllNOP before we start a new recursion.

		// We do not print out a NOP slot here.
		if( num> 0 && MI->getOperand(num - 1).isInst())
			printInstList(MI->getOperand(num - 1).getInst(), O, isAllNOP);
	}

	return;
}

void
MMPUInstPrinter::printOperand(const MCInst *MI, unsigned OpNo,
								raw_ostream &O) const
{
	const MCOperand& MO = MI->getOperand(OpNo);

	if(MO.isReg()) {
		O << getRegisterName(MO.getReg());
	}
	else if(MO.isExpr()) {
		O << *(MO.getExpr());
	}
	else if(MO.isImm()) {
		O << MO.getImm();
	}
	else {
		assert(false && "Unknown operand");
	}
}

void
MMPUInstPrinter::printUImm10(const MCInst *MI, unsigned OpNo, raw_ostream &O) const
{
	const MCOperand& MO = MI->getOperand(OpNo);

	if(MO.isExpr()) {
		O << *(MO.getExpr());
	}
	else if(MO.isImm()) {
		O << MO.getImm();
	}
	else {
		assert(false && "Unknown operand");
	}
}

void
MMPUInstPrinter::printKUImm4(const MCInst *MI, unsigned OpNo, raw_ostream &O) const
{
  const MCOperand& MO = MI->getOperand(OpNo);
  int64_t val;
  const MCExpr * expr;

  if(MO.isExpr()) {
    expr = MO.getExpr();
    if(expr->getKind()==MCExpr::Constant) {
      val = static_cast<const MCConstantExpr*>(expr)->getValue();
      //val = SignExtend64<4>( val );
      if(val != 0) {
        O <<"- "<< val;
      }
    }
    else {
      O <<"- "<< *expr;
    }
  }
  else if(MO.isImm()) {
    val = MO.getImm();
    if(val != 0) {
      O <<"- "<< val;
    }
  }
  else {
    assert(false && "Unknown operand");
  }
}

void
MMPUInstPrinter::printBIUOptions(const MCInst *MI, unsigned OpNo, raw_ostream &O) const
{
	const MCOperand& MO = MI->getOperand(OpNo);

	int64_t options = MO.getImm();

	if(options & MMPU::c_M  ) O << " (M)";
	if(options & MMPU::c_BR ) O << " (BR)";
	if(options & MMPU::c_APP) O << " (A++)";
	if(options & MMPU::c_KPP) O << " (K++)";

	mmpu_print_c_option(options, O);
}

void
MMPUInstPrinter::printBIUStOptions(const MCInst *MI, unsigned OpNo, raw_ostream &O) const
{
  const MCOperand& MO = MI->getOperand(OpNo);

  int64_t options = MO.getImm();

  if(options & MMPU::c_M  ) O << " (M)";
  if(options & MMPU::c_BR ) O << " (BR)";
  if(options & MMPU::c_APP) O << " (A++)";
  if(options & MMPU::c_KPP) O << " (K++)";

  // BIU store only.
  if(options & MMPU::c_IND) O << " (IND)";

  mmpu_print_c_option(options, O);
}

void
MMPUInstPrinter::printMRegExt(const MCInst *MI, unsigned OpNo, raw_ostream &O) const
{
  const MCOperand& MO = MI->getOperand(OpNo);
  unsigned reg = MO.getReg();
  O << getRegisterName(reg);

  unsigned num = reg - RI.getRegClass(MMPUReg::MRegRegClassID).getRegister(0);
  O << ":";
  O << num+1;
}

void
MMPUInstPrinter::printTPortExt(const MCInst *MI, unsigned OpNo, raw_ostream &O) const
{
  const MCOperand& MO = MI->getOperand(OpNo);
  unsigned reg = MO.getReg();
  O << getRegisterName(reg);

  unsigned num = reg - RI.getRegClass(MMPUReg::TPortRegClassID).getRegister(0);
  O << ":";
  O << num+1;
}

void
MMPUInstPrinter::printIALUOptions(const MCInst *MI, unsigned OpNo, raw_ostream &O) const
{
	const MCOperand& MO = MI->getOperand(OpNo);

	int64_t options = MO.getImm();

	if(options & MMPU::c_T) O << "(T)";
	if(options & MMPU::c_U) O << "(U)";
	if(options & MMPU::c_B) O << "(B)";
	if(options & MMPU::c_H) O << "(H)";

	mmpu_print_c_option(options, O);
}

void
MMPUInstPrinter::printCOption(const MCInst *MI, unsigned OpNo, raw_ostream &O) const
{
  const MCOperand& MO = MI->getOperand(OpNo);
  int64_t options = MO.getImm();

  mmpu_print_c_option(options, O);
}

void
MMPUInstPrinter::printMr2Options(const MCInst *MI, unsigned OpNo, raw_ostream &O) const
{
  const MCOperand& MO = MI->getOperand(OpNo);
  int64_t options = MO.getImm();

  if(options & MMPU::c_WF1) O << "(WF1)";
  if(options & MMPU::c_WF2) O << "(WF2)";
  if(options & MMPU::c_WF3) O << "(WF3)";
  if(options & MMPU::c_WX1) O << "(WX1)";
  if(options & MMPU::c_WX2) O << "(WX2)";
  if(options & MMPU::c_WX3) O << "(WX3)";
  if(options & MMPU::c_WX4) O << "(WX4)";

  mmpu_print_c_option(options, O);
}

void
MMPUInstPrinter::printCombineOptions(const MCInst *MI, unsigned OpNo, raw_ostream &O) const
{
  const MCOperand& MO = MI->getOperand(OpNo);
  int64_t options = MO.getImm();

  if(options & MMPU::c_TC) O << "(TC)";
  mmpu_print_c_option(options, O);
}

void
MMPUInstPrinter::printFALUDblOptions(const MCInst *MI, unsigned OpNo, raw_ostream &O) const
{
	const MCOperand& MO = MI->getOperand(OpNo);

	int64_t options = MO.getImm();

	if(options & MMPU::c_T) O << "(T)";

	mmpu_print_c_option(options, O);
}

void
MMPUInstPrinter::printFALUOptions(const MCInst *MI, unsigned OpNo, raw_ostream &O) const
{
  const MCOperand& MO = MI->getOperand(OpNo);

  int64_t options = MO.getImm();

  if(options & MMPU::c_T) O << "(T)";
  if(options & MMPU::c_U) O << "(U)";
  if(options & MMPU::c_S) O << "(S)";
  if(options & MMPU::c_D) O << "(D)";

  mmpu_print_c_option(options, O);
}

void
MMPUInstPrinter::printIMACOptions(const MCInst *MI, unsigned OpNo, raw_ostream &O) const
{
  const MCOperand& MO = MI->getOperand(OpNo);

  int64_t options = MO.getImm();
  if(options & MMPU::c_H)  O << "(H)";
  if(options & MMPU::c_B)  O << "(B)";
  if(options & MMPU::c_U)  O << "(U)";
  if(options & MMPU::c_I)  O << "(I)";
  if(options & MMPU::c_CR) O << "(CR)";

  mmpu_print_c_option(options, O);
}

void
MMPUInstPrinter::printFMACOptions(const MCInst *MI, unsigned OpNo, raw_ostream &O) const
{
  const MCOperand& MO = MI->getOperand(OpNo);

  int64_t options = MO.getImm();
  if(options & MMPU::c_T)  O << "(T)"; else
  if(options & MMPU::c_CR) O << "(CR)";

  if(options & MMPU::c_D) O << "(D)"; else
  if(options & MMPU::c_S) O << "(S)";

  mmpu_print_c_option(options, O);
}

void
MMPUInstPrinter::printConds(const MCInst *MI, unsigned OpNo, raw_ostream &O) const
{
	const MCOperand& MO = MI->getOperand(OpNo);

	unsigned Conds = MO.getImm();
	unsigned KIBITSi = (Conds >> 0      )  &   ( (1 << 5) -1 );  //The bits of KIi
	unsigned KIBITSj = (Conds >> 5      )  &   ( (1 << 6) -1 );  //The bits of KIj
	unsigned KIBITSk = (Conds >> (5+6)  )  &   ( (1 << 6) -1 );  //The bits of KIk
	unsigned KIBITSl = (Conds >> (5+6+6))  &   ( (1 << 6) -1 );  //The bits of KIl

	if( (KIBITSi >>4) & 0x1 ) O << "!";
	O<<"KI"<< ( KIBITSi & 0xF);

	if( (KIBITSj >> 5) & 0x1 ){
	   O <<" & ";
	   if( (KIBITSj >> 4) & 0x1)  O << "!";
	   O<<"KI"<< ( KIBITSj & 0xF);
	}

	if( (KIBITSk >> 5) & 0x1 ){
	   O <<" & ";
	   if( (KIBITSk >> 4) & 0x1)  O << "!";
	   O<<"KI"<< ( KIBITSk & 0xF);
	}

	if( (KIBITSl >> 5) & 0x1 ){
	   O <<" & ";
	   if( (KIBITSl >> 4) & 0x1)  O << "!";
	   O<<"KI"<< ( KIBITSl & 0xF);
	}
}
