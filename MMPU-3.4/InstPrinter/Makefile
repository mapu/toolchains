##===- lib/Target/MMPU/InstPrinter/Makefile ----------------------------===##
#
#                     The LLVM Compiler Infrastructure
#
# This file is distributed under the University of Illinois Open Source
# License. See LICENSE.TXT for details.
#
##===----------------------------------------------------------------------===##

LEVEL = ../../../..
LIBRARYNAME = LLVMMMPUInstPrinter

# Hack: we need to include 'main' MMPU target directory to grab private headers
CPP.Flags += -I$(PROJ_OBJ_DIR)/.. -I$(PROJ_SRC_DIR)/..

include $(LEVEL)/Makefile.common

MMPUHEADERS = MMPUInstPrinter.h
MMPUSUBDIR  = $(DESTDIR)$(PROJ_includedir)/MMPU/InstPrinter

install-local::
	$(Echo) Making install directory $(MMPUSUBDIR)
	$(Verb) $(MKDIR) $(MMPUSUBDIR)
	$(Echo) Installing MMPU include files
	$(Verb) for mmpu_header in $(MMPUHEADERS); do \
		$(DataInstall) $(PROJ_SRC_DIR)/$$mmpu_header $(MMPUSUBDIR)/$$mmpu_header; \
	done;
