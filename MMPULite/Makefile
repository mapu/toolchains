##===- lib/Target/MMPULite/Makefile ---------------------------*- Makefile -*-===##
#
#                     The LLVM Compiler Infrastructure
#
# This file is distributed under the University of Illinois Open Source
# License. See LICENSE.TXT for details.
#
##===----------------------------------------------------------------------===##

LEVEL = ../../..
LIBRARYNAME = LLVMMMPULiteCodeGen
TARGET = MMPULite

# Make sure that tblgen is run, first thing.
BUILT_SOURCES = MMPULiteGenRegisterInfo.inc \
				MMPULiteGenInstrInfo.inc \
				MMPULiteGenAsmWriter.inc \
				MMPULiteGenSubtargetInfo.inc \
				MMPULiteGenMCCodeEmitter.inc \
				MMPULiteGenDisassemblerTables.inc \
				MMPULiteGenInstrParser.bison \
				MMPULiteGenInstrFLexer.flex

DIRS = InstPrinter Disassembler TargetInfo MCTargetDesc AsmParser

include $(LEVEL)/Makefile.common
