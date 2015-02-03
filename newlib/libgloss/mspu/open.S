/*
 * Stub version of open.
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
_DEFUN (_open, (file, flags, mode),
        char *file  _AND
        int   flags _AND
        int   mode)
{
  errno = ENOSYS;
  return -1;
}

stub_warning(_open)
*/



  # signature for syscall *open*: int _open (char *file, int   flags, int   mode);

  # compliant to SPU C ABI,
  # the value of *file* resides in R0,
  # the value of *flags* resides in R1,
  # the value of *mode* resides in R2.
  # the real syscall handler (like gem5 simulator) should utilize contents within
  # these registers.

  .global open

  .text

open:

  # let simulator handle all the remaining jobs.
  # and it also set R0 for _open() to return.
  m.s syscall 5;; # '5' is syscall ID for open.

  m.s jump J30;;
