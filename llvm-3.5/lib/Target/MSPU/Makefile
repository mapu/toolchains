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

################## MaPU-specific Makefile rules ################
#### XXXGenInstrFLexer.flex
FLexerFile := $(filter %.flex, $(BUILT_SOURCES))
FLexerTmpFile := $(FLexerFile:%=$(ObjDir)/%.tmp)

#### XXXGenInstrRLexer.ragel
RLexerFile := $(filter %.ragel, $(BUILT_SOURCES))
RLexerTmpFile := $(RLexerFile:%=$(ObjDir)/%.tmp)

#### XXXGenInstrParser.bison
ParserFile:= $(filter %.bison, $(BUILT_SOURCES))
ParserTmpFile := $(ParserFile:%=$(ObjDir)/%.tmp)
################################################################

################## MaPU-specific Makefile rules ################
$(FLexerFile) : %.flex : $(ObjDir)/%.flex.tmp
	$(Verb) $(CMP) -s $@ $< || $(CP) $< $@

$(RLexerFile) : %.ragel : $(ObjDir)/%.ragel.tmp
	$(Verb) $(CMP) -s $@ $< || $(CP) $< $@

$(ParserFile) : %.bison : $(ObjDir)/%.bison.tmp
	$(Verb) $(CMP) -s $@ $< || $(CP) $< $@
################################################################

################## MaPU-specific Makefile rules ################
# Generate custom instruction token lexer for MaPU
$(TARGET:%=$(ObjDir)/%GenInstrFLexer.flex.tmp): \
$(ObjDir)/%GenInstrFLexer.flex.tmp : AsmParser/%InstrFLexer.l  $(ObjDir)/.dir
	$(Echo) "Building $(<F) for assembly instruction lexer with flex"
	$(Verb) flex  -o  $(call SYSPATH, $@)  $<

# Generate custom instruction token lexer for MaPU
$(TARGET:%=$(ObjDir)/%GenInstrRLexer.ragel.tmp): \
$(ObjDir)/%GenInstrRLexer.ragel.tmp : AsmParser/%InstrRLexer.rl  $(ObjDir)/.dir
	$(Echo) "Building $(<F) for assembly instruction lexer with ragel"
	$(Verb) $(RAGEL)  -o  $(call SYSPATH, $@)  $<

# Generate custom instruction parser for MaPU
$(TARGET:%=$(ObjDir)/%GenInstrParser.bison.tmp): \
$(ObjDir)/%GenInstrParser.bison.tmp : AsmParser/%InstrParser.y $(ObjDir)/.dir
	$(Echo) "Building $(<F) for assembly instruction parser with bison"
	$(Verb) bison -v -g -r state,lookahead,itemset -o $(call SYSPATH, $@) $<
################################################################

clean-local::
	-$(Verb) $(RM) -f $(INCFiles) $(FLexerFile) $(RLexerFile) $(ParserFile)
