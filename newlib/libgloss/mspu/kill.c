/*
 * Stub version of kill.
 */

#include "config.h"
#include <_ansi.h>
#include <_syslist.h>
#include <errno.h>
#undef errno
extern int errno;
#include "warning.h"

int
_DEFUN (kill, (pid, sig),
        int pid  _AND
        int sig)
{
  errno = ENOSYS;
  return -1;
}
