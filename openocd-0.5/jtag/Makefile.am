include $(top_srcdir)/common.mk

METASOURCES = AUTO
noinst_LIBRARIES = libjtag.a

SUBDIRS =
DRIVERFILES =
libjtag_a_LIBADD =

CLEANFILES =

BUILT_SOURCES =

SUBDIRS += drivers
libjtag_a_LIBADD += 


libjtag_a_SOURCES = adapter.c  commands.c  core.c  interface.c  interfaces.c  tcl.c

noinst_HEADERS = commands.h  driver.h  interface.h  interfaces.h  jtag.h  swd.h \
	 minidriver.h

EXTRA_DIST = startup.tcl

AM_CFLAGS=-std=gnu99 -Wno-sign-compare -Werror
