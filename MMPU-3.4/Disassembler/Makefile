##===- lib/Target/MMPU/Disassembler/Makefile -------------*- Makefile -*-===##
#
#                     The LLVM Compiler Infrastructure
#
# This file is distributed under the University of Illinois Open Source
# License. See LICENSE.TXT for details.
#
##===----------------------------------------------------------------------===##

LEVEL = ../../../..
LIBRARYNAME = LLVMMMPUDisassembler

# Hack: we need to include 'main' MMPU target directory to grab headers
CPP.Flags += -I$(PROJ_OBJ_DIR)/.. -I$(PROJ_SRC_DIR)/..

include $(LEVEL)/Makefile.common

MMPUHEADERS = MMPUDisassembler.h
MMPUSUBDIR  = $(DESTDIR)$(PROJ_includedir)/MMPU/Disassembler

install-local::
	$(Echo) Making install directory $(MMPUSUBDIR)
	$(Verb) $(MKDIR) $(MMPUSUBDIR)
	$(Echo) Installing MMPU include files
	$(Verb) for mmpu_header in $(MMPUHEADERS); do \
		$(DataInstall) $(PROJ_SRC_DIR)/$$mmpu_header $(MMPUSUBDIR)/$$mmpu_header; \
	done;
