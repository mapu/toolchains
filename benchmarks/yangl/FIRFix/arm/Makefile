
sinclude app/autoconf.mk

LIB	= $(obj)/libstubs.o
LIBCOBJS = stubs.o

LIBOBJS	= $(addprefix $(obj)/, $(LIBCOBJS))

extra-y += libstubs.o

subdir-y += $(APPSUBDIRS-y)

quiet_cmd_link_lib = LD      $@
      cmd_link_lib = $(LD) $(ld_flags) -r -o $@ $^
$(LIB):	$(LIBOBJS) 
	$(call if_changed,link_lib)
