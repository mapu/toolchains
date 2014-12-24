##===- lib/Target/MMPULite/AsmParser/Makefile ----------------*- Makefile -*-===##
#
#                     The LLVM Compiler Infrastructure
#
# This file is distributed under the University of Illinois Open Source
# License. See LICENSE.TXT for details.
#
##===----------------------------------------------------------------------===##

LEVEL = ../../../..
LIBRARYNAME = LLVMMMPULiteAsmParser

# Hack: we need to include 'main' MMPULite target directory for private headers
CPP.Flags += -I$(PROJ_OBJ_DIR)/.. -I$(PROJ_SRC_DIR)/..

include $(LEVEL)/Makefile.common


# Make sure flex and bison is run, first thing.
#BUILT_SOURCES = MMPULiteGenInstrParser.bison \
#				MMPULiteGenInstrLexer.flex
