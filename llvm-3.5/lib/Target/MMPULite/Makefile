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

