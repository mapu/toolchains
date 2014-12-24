##===- lib/Target/MSPU/Makefile ---------------------------*- Makefile -*-===##
#
#                     The LLVM Compiler Infrastructure
#
# This file is distributed under the University of Illinois Open Source
# License. See LICENSE.TXT for details.
#
##===----------------------------------------------------------------------===##

LEVEL = ../../..
LIBRARYNAME = LLVMMSPUCodeGen
TARGET = MSPU

# Make sure that tblgen is run, first thing.
BUILT_SOURCES = MSPUGenRegisterInfo.inc \
				MSPUGenInstrInfo.inc \
				MSPUGenAsmWriter.inc \
				MSPUGenDAGISel.inc \
				MSPUGenSubtargetInfo.inc \
				MSPUGenCallingConv.inc \
				MSPUGenDFAPacketizer.inc \
				MSPUGenMCCodeEmitter.inc \
				MSPUGenDisassemblerTables.inc \
				MSPUGenInstrFLexer.flex \
				MSPUGenInstrRLexer.ragel \
				MSPUGenInstrParser.bison

DIRS = InstPrinter Disassembler TargetInfo MCTargetDesc AsmParser

include $(LEVEL)/Makefile.common

MSPUHEADERS = MSPUFrameLowering.h  MSPUISelLowering.h MSPUSubtarget.h \
							MSPU.h  MSPURegisterInfo.h  MSPUTargetMachine.h \
							MSPUInstrInfo.h      MSPUSelectionDAGInfo.h

MSPUINCS = MSPUGenInstrInfo.inc  MSPUGenRegisterInfo.inc  MSPUGenSubtargetInfo.inc
MSPUDIR  = $(DESTDIR)$(PROJ_includedir)/MSPU

install-local::
	$(Echo) Making install directory $(MSPUDIR)
	$(Verb) $(MKDIR) $(MSPUDIR)
	$(Echo) Installing MSPU include files
	$(Verb) for mspu_header in $(MSPUHEADERS); do \
		$(DataInstall) $(PROJ_SRC_DIR)/$$mspu_header $(MSPUDIR)/$$mspu_header; \
	done;
	$(Verb) for mspu_inc in $(MSPUINCS); do \
		$(DataInstall) $$mspu_inc $(MSPUDIR)/$$mspu_inc; \
	done;
