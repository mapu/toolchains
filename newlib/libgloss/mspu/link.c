/*
 * Stub version of link.
 */

#include "config.h"
#include <_ansi.h>
#include <_syslist.h>
#include <errno.h>
#undef errno
extern int errno;
#include "warning.h"

int
_DEFUN (link, (existing, new),
        char *existing _AND
        char *new)
{
  errno = ENOSYS;
  return -1;
}
