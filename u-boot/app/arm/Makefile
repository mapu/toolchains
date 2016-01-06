
include $(TOPDIR)/config.mk

sinclude autoconf.mk

LIB	= $(obj)libstubs.o
LIBCOBJS = stubs.o

LIBOBJS	= $(addprefix $(obj), $(LIBCOBJS))

gcclibdir	:= $(shell dirname `$(CC) -print-libgcc-file-name`)
SRCS		:= $(LIBCOBJS:.o=.c)

all:	$(obj).depend $(LIB)
ifeq ($(APPSUBDIRS-y),)
	@:
else
	@for dir in $(APPSUBDIRS-y) ; do \
	    $(MAKE) -C $$dir all || exit 1 ; \
	done
endif

$(LIB):	$(obj).depend $(LIBOBJS)
	$(call cmd_link_o_target, $(LIBOBJS))

#########################################################################

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################

