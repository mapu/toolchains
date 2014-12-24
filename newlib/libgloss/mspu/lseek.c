/*
 * Stub version of lseek.
 */

#include "config.h"
#include <_ansi.h>
#include <_syslist.h>
#include <errno.h>
#undef errno
extern int errno;
#include "warning.h"

int
_DEFUN (lseek, (file, ptr, dir),
        int   file  _AND
        int   ptr   _AND
        int   dir)
{
  errno = ENOSYS;
  return ((int)-1);
}
