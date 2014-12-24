/*
 * Stub version of close.
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
_DEFUN (_close, (fildes),
        int fildes)
{
  errno = ENOSYS;
  return -1;
}

stub_warning (_close)
*/


  # signature for syscall *close*: int _close (int file);

  # compliant to SPU C ABI,
  # the value of *file* resides in R0,
  # the real syscall handler (like gem5 simulator) should utilize contents within
  # these registers.

  .global close

  .text

close:

  # let simulator handle all the remaining jobs.
  # and it also set R0 for _close() to return.
  syscall 6;; # '6' is syscall ID for close.

  jump J30;;
