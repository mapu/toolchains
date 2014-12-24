##===- lib/Target/MMPU/Makefile ---------------------------*- Makefile -*-===##
#
#                     The LLVM Compiler Infrastructure
#
# This file is distributed under the University of Illinois Open Source
# License. See LICENSE.TXT for details.
#
##===----------------------------------------------------------------------===##

LEVEL = ../../..
LIBRARYNAME = LLVMMMPUCodeGen
TARGET = MMPU

# Make sure that tblgen is run, first thing.
BUILT_SOURCES = MMPUGenRegisterInfo.inc \
				MMPUGenInstrInfo.inc \
				MMPUGenAsmWriter.inc \
				MMPUGenSubtargetInfo.inc \
				MMPUGenMCCodeEmitter.inc \
				MMPUGenDisassemblerTables.inc \
				MMPUGenInstrFLexer.flex \
				MMPUGenInstrRLexer.ragel \
				MMPUGenInstrParser.bison

DIRS = InstPrinter Disassembler TargetInfo MCTargetDesc AsmParser

include $(LEVEL)/Makefile.common

MMPUHEADERS = MMPU.h  MMPUSubtarget.h

MMPUINCS = MMPUGenInstrInfo.inc  MMPUGenRegisterInfo.inc  MMPUGenSubtargetInfo.inc
MMPUDIR  = $(DESTDIR)$(PROJ_includedir)/MMPU

install-local::
	$(Echo) Making install directory $(MMPUDIR)
	$(Verb) $(MKDIR) $(MMPUDIR)
	$(Echo) Installing MMPU include files
	$(Verb) for mmpu_header in $(MMPUHEADERS); do \
		$(DataInstall) $(PROJ_SRC_DIR)/$$mmpu_header $(MMPUDIR)/$$mmpu_header; \
	done;
	$(Verb) for mmpu_inc in $(MMPUINCS); do \
		$(DataInstall) $$mmpu_inc $(MMPUDIR)/$$mmpu_inc; \
	done;
