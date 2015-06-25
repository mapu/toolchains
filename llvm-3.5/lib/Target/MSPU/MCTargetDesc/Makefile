##===- lib/Target/MSPU/TargetDesc/Makefile ----------------*- Makefile -*-===##
#
#                     The LLVM Compiler Infrastructure
#
# This file is distributed under the University of Illinois Open Source
# License. See LICENSE.TXT for details.
#
##===----------------------------------------------------------------------===##

LEVEL = ../../../..
LIBRARYNAME = LLVMMSPUDesc

# Hack: we need to include 'main' target directory to grab private headers
CPP.Flags += -I$(PROJ_OBJ_DIR)/.. -I$(PROJ_SRC_DIR)/..

include $(LEVEL)/Makefile.common

MSPUHEADERS = MSPUMCTargetDesc.h
MSPUSUBDIR  = $(DESTDIR)$(PROJ_includedir)/MSPU/MCTargetDesc

install-local::
	$(Echo) Making install directory $(MSPUSUBDIR)
	$(Verb) $(MKDIR) $(MSPUSUBDIR)
	$(Echo) Installing MSPU include files
	$(Verb) for mspu_header in $(MSPUHEADERS); do \
		$(DataInstall) $(PROJ_SRC_DIR)/$$mspu_header $(MSPUSUBDIR)/$$mspu_header; \
	done;
