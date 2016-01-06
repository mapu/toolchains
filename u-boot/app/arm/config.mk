CSRCS	:= $(wildcard *.c)
ASRCS   := $(wildcard *.S)

# We don't want gcc reordering functions if possible.  This ensures that an
# application's entry point will be the first function in the application's
# source file.

CFLAGS_NTR := $(call cc-option,-fno-toplevel-reorder)
CFLAGS += $(CFLAGS_NTR) -I$(TOPDIR)/../slib/include -I$(APPTOPDIR)/include
CPPFLAGS += -I$(TOPDIR)/../slib/include -I$(APPTOPDIR)/include

