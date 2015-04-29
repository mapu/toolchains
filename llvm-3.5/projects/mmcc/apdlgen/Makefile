##===- project/mmcc/Makefile ----------------------------*- Makefile -*-===##
# 
#                     The LLVM Compiler Infrastructure
#
# This file is distributed under the University of Illinois Open Source
# License. See LICENSE.TXT for details.
# 
##===----------------------------------------------------------------------===##
LEVEL = ../../..
LIBRARYNAME = apdlgen
BUILD_ARCHIVE = 1

# Hack: we need to include 'main' MMPULite target directory for private headers
CPP.Flags += -I$(PROJ_OBJ_DIR)/$(LEVEL)/lib/Target/MMPULite -I$(PROJ_SRC_DIR)/$(LEVEL)/lib/Target/MMPULite
CPP.Flags += -I$(PROJ_SRC_DIR)/../include

include $(LEVEL)/Makefile.common
