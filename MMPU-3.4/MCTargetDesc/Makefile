##===- lib/Target/MMPU/TargetDesc/Makefile ----------------*- Makefile -*-===##
#
#                     The LLVM Compiler Infrastructure
#
# This file is distributed under the University of Illinois Open Source
# License. See LICENSE.TXT for details.
#
##===----------------------------------------------------------------------===##

LEVEL = ../../../..
LIBRARYNAME = LLVMMMPUDesc

# Hack: we need to include 'main' target directory to grab private headers
CPP.Flags += -I$(PROJ_OBJ_DIR)/.. -I$(PROJ_SRC_DIR)/..

include $(LEVEL)/Makefile.common

MMPUHEADERS = MMPUMCInst.h  MMPUMCTargetDesc.h
MMPUSUBDIR  = $(DESTDIR)$(PROJ_includedir)/MMPU/MCTargetDesc

install-local::
	$(Echo) Making install directory $(MMPUSUBDIR)
	$(Verb) $(MKDIR) $(MMPUSUBDIR)
	$(Echo) Installing MMPU include files
	$(Verb) for mmpu_header in $(MMPUHEADERS); do \
		$(DataInstall) $(PROJ_SRC_DIR)/$$mmpu_header $(MMPUSUBDIR)/$$mmpu_header; \
	done;
