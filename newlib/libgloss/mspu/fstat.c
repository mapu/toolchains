/*
 * Stub version of fstat.
 */

#include "config.h"
#include <_ansi.h>
#include <_syslist.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#undef errno
extern int errno;
#include "warning.h"

int
_DEFUN (fstat, (fildes, st),
        int          fildes _AND
        struct stat *st)
{
  st->st_mode = S_IFCHR; /* Always pretend to be a tty */
  st->st_blksize = 0;

  return (0);
}
