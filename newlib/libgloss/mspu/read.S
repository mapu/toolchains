/*
 * Stub version of read.
 */


/*
#include "config.h"
#include <_ansi.h>
#include <_syslist.h>
#include <errno.h>
#undef errno
extern int errno;
#include "warning.h"

int
_DEFUN (_read, (file, ptr, len),
        int   file  _AND
        char *ptr   _AND
        int   len)
{
  errno = ENOSYS;
  return -1;
}

stub_warning(_read)
*/


  # signature for syscall *read*: int _read (int   file, char *ptr, int   len);

  # compliant to SPU C ABI,
  # the value of *file* resides in R0,
  # the value of *ptr* resides in R1,
  # the value of *len* resides in R2.
  # the real syscall handler (like gem5 simulator) should utilize contents within
  # these registers.

  .global read

  .text

read:

  # let simulator handle all the remaining jobs.
  # and it also set R0 for _read() to return.
  syscall 3;; # '3' is syscall ID for read.

  jump J30;;
