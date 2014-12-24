SUBDIRS = helper jimtcl jtag server target transport

lib_LIBRARIES = libopenocd.a

libopenocd_a_SOURCES = openocd.c startup_tcl.c  

noinst_HEADERS = openocd.h config.h

# install tcl and cfg files needed by openocd executable.
cfgdir = $(prefix)/bin
cfg_DATA = startup.tcl openocd.cfg openocd-sim.cfg

libopenocd_a_CFLAGS= -std=gnu99 -Wextra -Wno-sign-compare 

bin_PROGRAMS = openocd
openocd_SOURCES = main.c
openocd_LDADD = libopenocd.a \
			 $(top_builddir)/target/libtarget.a \
			 $(top_builddir)/transport/libtransport.a \
			 $(top_builddir)/jtag/libjtag.a \
			 $(top_builddir)/jtag/drivers/libdrivers.a \
			 $(top_builddir)/server/libserver.a \
			 $(top_builddir)/helper/libhelper.a \
			 $(top_builddir)/jimtcl/libjim.a \
			 libopenocd.a \
			 $(top_builddir)/target/libtarget.a \
			 $(top_builddir)/transport/libtransport.a \
			 $(top_builddir)/jtag/libjtag.a \
			 $(top_builddir)/jtag/drivers/libdrivers.a \
			 $(top_builddir)/server/libserver.a \
			 $(top_builddir)/helper/libhelper.a \
			 $(top_builddir)/jimtcl/libjim.a

openocd_LDFLAGS = -ldl -lusb
AM_CFLAGS=-std=gnu99 -Wno-sign-compare -Werror

include $(top_srcdir)/common.mk
