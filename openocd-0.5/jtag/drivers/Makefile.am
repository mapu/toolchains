include $(top_srcdir)/common.mk

METASOURCES = AUTO
noinst_LIBRARIES = libdrivers.a

SUBDIRS =
DRIVERFILES =
libdrivers_a_LIBADD =

CLEANFILES =

BUILT_SOURCES =

libdrivers_a_SOURCES = driver.c  ft2232.c  usb_common.c ftdi.c bitbang.c dummy.c fifo.c

noinst_HEADERS = usb_common.h bitbang.h ftdi.h fifo.h

EXTRA_DIST = startup.tcl

AM_CFLAGS=-std=gnu99 -Wno-sign-compare -Werror
