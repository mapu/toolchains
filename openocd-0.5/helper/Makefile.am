include $(top_srcdir)/common.mk

noinst_LIBRARIES = libhelper.a

CONFIGFILES = options.c time_support_common.c

AM_CFLAGS=-std=gnu99 -Wno-sign-compare -Werror

libhelper_a_SOURCES = \
	binarybuffer.c \
	$(CONFIGFILES) \
	configuration.c \
	log.c \
	command.c \
	time_support.c \
	replacements.c \
	fileio.c \
	util.c

libhelper_a_SOURCES += ioutil.c



noinst_HEADERS = \
	binarybuffer.h \
	configuration.h \
	ioutil.h \
	util.h \
	types.h \
	log.h \
	command.h \
	time_support.h \
	replacements.h \
	fileio.h \
	system.h

EXTRA_DIST = startup.tcl

