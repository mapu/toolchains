include $(top_srcdir)/common.mk

METASOURCES = AUTO
noinst_LIBRARIES = libtarget.a
noinst_HEADERS = algorithm.h  breakpoints.h  hello.h  mapu_spu.h  register.h \
 target.h  target_request.h  target_type.h  trace.h image.h
libtarget_a_SOURCES = algorithm.c  breakpoints.c  hello.c  mapu_jtag.c  mapu_spu.c \
		 register.c  target.c  target_request.c  trace.c image.c

EXTRA_DIST = startup.tcl

AM_CFLAGS=-std=gnu99 -Wno-sign-compare -Werror
