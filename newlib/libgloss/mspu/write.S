/*
 * Stub version of write.
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
_DEFUN (_write, (file, ptr, len),
        int   file  _AND
        char *ptr   _AND
        int   len)
{
  errno = ENOSYS;
  __mspu_syscall(4);
  return 0;
}

stub_warning(_write)
*/

  # signature for syscall *write*: int _write (int file, char * ptr, int len);

  # compliant to SPU C ABI,
  # the value of *file* resides in R0,
  # the value of *ptr* resides in R1,
  # the value of *len* resides in R2.
  # the real syscall handler (like gem5 simulator) should utilize contents within
  # these registers.

  .global write

  .text

write:

  # let simulator handle all the remaining jobs.
  # and it also set R0 for _write() to return.
  m.s syscall 4;; # '4' is syscall ID for write.

  m.s jump J30;;
