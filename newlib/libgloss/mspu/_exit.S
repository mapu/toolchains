/* Stub version of _exit. */

/*
#include <limits.h>
#include "config.h"
#include <_ansi.h>
#include <_syslist.h>
#include <errno.h>
#undef errno
extern int errno;

_VOID
_DEFUN (_exit, (rc),
	int rc)
{
  errno = ENOSYS;

   Convince compiler that this function never returns.
  for (;;)
    ;
}
*/

  .global _exit
  .text

_exit:
  spustop;;
