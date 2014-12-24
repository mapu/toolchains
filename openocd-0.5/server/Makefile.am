include $(top_srcdir)/common.mk

METASOURCES = AUTO
noinst_LIBRARIES = libserver.a
noinst_HEADERS = server.h telnet_server.h tcl_server.h gdb_server.h
libserver_a_SOURCES = server.c telnet_server.c server_stubs.c  tcl_server.c gdb_server.c


# tcl server addons
noinst_HEADERS += 
libserver_a_SOURCES += 

EXTRA_DIST = startup.tcl

AM_CFLAGS=-std=gnu99 -Wno-sign-compare -Werror
