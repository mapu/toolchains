#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_INIT([mocd], [0.0.1], [yangyy@zklc.com])
AM_INIT_AUTOMAKE([foreign -Wall -Werror])
AC_CONFIG_SRCDIR([openocd.c])

# AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_RANLIB
# Checks for libraries.

AM_PROG_CC_C_O

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h fcntl.h inttypes.h limits.h malloc.h netdb.h netinet/in.h stddef.h stdint.h stdlib.h string.h strings.h sys/ioctl.h sys/param.h sys/socket.h sys/time.h syslog.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_INT8_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_CHECK_DECLS([sys_siglist])
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_LSTAT_FOLLOWS_SLASHED_SYMLINK
AC_FUNC_MALLOC
AC_FUNC_MKTIME
AC_FUNC_REALLOC
# AC_FUNC_STRNLEN
AC_FUNC_STRTOD
AC_CHECK_FUNCS([alarm dup2 floor getcwd gethostbyname gethostname gettimeofday inet_ntoa isascii localtime_r memchr memmove memset mkdir pow realpath regcomp rmdir select socket sqrt strcasecmp strchr strdup strerror strncasecmp strndup strpbrk strrchr strstr strtol strtoul strtoull sysinfo])

AC_CONFIG_FILES([Makefile
                 helper/Makefile
                 jimtcl/Makefile
                 jtag/Makefile
                 jtag/drivers/Makefile
                 server/Makefile
                 target/Makefile
                 transport/Makefile])
AC_CONFIG_HEADERS([config.h])
AC_OUTPUT
