##===- project/mmcc/Makefile ----------------------------*- Makefile -*-===##
# 
#                     The LLVM Compiler Infrastructure
#
# This file is distributed under the University of Illinois Open Source
# License. See LICENSE.TXT for details.
# 
##===----------------------------------------------------------------------===##
LEVEL = ../..
TOOLNAME = mmcc

LINK_COMPONENTS := native

BUILT_SOURCES += ../../lib/Target/MMPULite/AsmParser/MMPULiteScheduler.cpp
BUILT_SOURCES += ../../lib/Target/MMPULite/AsmParser/MCFunction.cpp
BUILT_SOURCES += MMPUGenFrontEndLexer.flex
BUILT_SOURCES += MMPUGenFrontEndParser.bison

CPP.Flags += -I$(PROJ_OBJ_DIR)/../../lib/Target/MMPULite -I$(PROJ_SRC_DIR)/../../lib/Target/MMPULite
CPP.Flags += -I$(PROJ_OBJ_DIR) -I$(PROJ_SRC_DIR)

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
$(ObjDir)/%GenFrontEndLexer.flex.tmp : %FrontEndLexer.l  $(ObjDir)/.dir
	$(Echo) "Building $(<F) for assembly instruction lexer with flex"
	$(Verb) flex  -o  $(call SYSPATH, $@)  $<

# Generate custom instruction parser for MaPU
$(ObjDir)/%GenFrontEndParser.bison.tmp : %FrontEndParser.y $(ObjDir)/.dir
	$(Echo) "Building $(<F) for assembly instruction parser with bison"
	$(Verb) bison -v -g -r state,lookahead,itemset -o $(call SYSPATH, $@) $<
################################################################

clean-local::
	-$(Verb) $(RM) -f $(INCFiles) $(FLexerFile) $(RLexerFile) $(ParserFile)
	
