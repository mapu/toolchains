##===- lib/Target/MSPU/Disassembler/Makefile -------------*- Makefile -*-===##
#
#                     The LLVM Compiler Infrastructure
#
# This file is distributed under the University of Illinois Open Source
# License. See LICENSE.TXT for details.
#
##===----------------------------------------------------------------------===##

LEVEL = ../../../..
LIBRARYNAME = LLVMMSPUDisassembler

# Hack: we need to include 'main' MSPU target directory to grab headers
CPP.Flags += -I$(PROJ_OBJ_DIR)/.. -I$(PROJ_SRC_DIR)/..

include $(LEVEL)/Makefile.common

MSPUHEADERS = MSPUDisassembler.h
MSPUSUBDIR  = $(DESTDIR)$(PROJ_includedir)/MSPU/Disassembler

install-local::
	$(Echo) Making install directory $(MSPUSUBDIR)
	$(Verb) $(MKDIR) $(MSPUSUBDIR)
	$(Echo) Installing MSPU include files
	$(Verb) for mspu_header in $(MSPUHEADERS); do \
		$(DataInstall) $(PROJ_SRC_DIR)/$$mspu_header $(MSPUSUBDIR)/$$mspu_header; \
	done;
