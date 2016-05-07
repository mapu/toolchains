#
# (C) Copyright 2000-2006
# Wolfgang Denk, DENX Software Engineering, wd@denx.de.
#
# SPDX-License-Identifier:	GPL-2.0+
#

APPTOPDIR	:= $(CURDIR)
extra-y        := testMat 
#
# Some versions of make do not handle trailing white spaces properly;
# leading to build failures. The problem was found with GNU Make 3.80.
# Using 'strip' as a workaround for the problem.
#
ELF := $(strip $(extra-y))

CSRCS	:=  $(basename $(notdir $(wildcard $(APPTOPDIR)/../app/$(ELF)/*.c)))

OBJS := $(filter-out $(ELF), $(CSRCS))

COBJS	:= $(OBJS:=.o)

extra-y += $(addsuffix .srec,$(extra-y)) $(addsuffix .bin,$(extra-y))

clean-files  := *.srec *.bin

COBJS := $(addprefix $(obj)/, $(COBJS))

ELF-y	:= $(addprefix $(obj)/, $(ELF))

targets += $(patsubst $(obj)/%,%,$(LIB)) $(ELF-y).o $(COBJS) 


# We don't want gcc reordering functions if possible.  This ensures that an
# application's entry point will be the first function in the application's
# source file.
ccflags-y += $(call cc-option,-fno-toplevel-reorder)
ccflags-y += -I$(APPTOPDIR)/../../spu/include
cppflags-y += -I$(APPTOPDIR)/../../spu/include

#########################################################################

quiet_cmd_link_elf = LD      $@
      cmd_link_elf = $(LD) $(LDFLAGS) -g -Ttext $(CONFIG_STANDALONE_LOAD_ADDR) \
		     -o $@ -e $(SYM_PREFIX)$(@F) $< $(COBJS) $(LIB) $(PLATFORM_LIBGCC) \
		     $(obj)/../libstubs.o

$(ELF-y): $(obj)/%: $(obj)/%.o $(COBJS) $(LIB) FORCE
	$(call if_changed,link_elf)
	install -m 644 -D $@ $(APPTOPDIR)/../../fs_root/app/$(ELF)

$(obj)/%.srec: OBJCOPYFLAGS := -O srec
$(obj)/%.srec: $(obj)/% FORCE
	$(call if_changed,objcopy)

$(obj)/%.bin: OBJCOPYFLAGS := -O binary
$(obj)/%.bin: $(obj)/% FORCE
	$(call if_changed,objcopy)
