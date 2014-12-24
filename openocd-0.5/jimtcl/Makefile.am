noinst_LIBRARIES = libjim.a

libjim_a_SOURCES = jim-aio.c        jim-file.c     jim-posix.c    jim-subcmd.c \
					jim-array.c      jim-format.c   jim-readdir.c  jim-syslog.c \
					jim.c            _jim-glob.c    jim-regexp.c   _jim-tclcompat.c \
					jim-clock.c      jim-load.c     jimregexp.c    jim-win32compat.c \
					jim-eventloop.c  jim-nvp.c      jim-signal.c   _load-static-exts.c \
					jim-exec.c       jim-package.c  _jim-stdlib.c  utf8.c

noinst_HEADERS = \
	jimautoconf.h  jim-config.h  jim-eventloop.h  jim.h  jim-nvp.h \
	jimregexp.h  jim-signal.h  jim-subcmd.h  jim-win32compat.h  utf8.h
	
AM_CFLAGS=-std=gnu99 -Wno-sign-compare -Werror
