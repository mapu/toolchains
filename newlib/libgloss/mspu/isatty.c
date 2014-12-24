/*
 * Stub version of isatty.
 */

#include "config.h"
#include <_ansi.h>
#include <_syslist.h>
#include <errno.h>
#undef errno
extern int errno;
#include "warning.h"

/*
 * isatty -- returns 1 if connected to a terminal device,
 *           returns 0 if not. Since we're hooked up to a
 *           serial port, we'll say yes _AND return a 1.
 */

/* Standard file descriptors.  */
#define STDIN_FILENO  0 /* Standard input.  */
#define STDOUT_FILENO 1 /* Standard output.  */
#define STDERR_FILENO 2 /* Standard error output.  */

int
_DEFUN (isatty, (file),
        int file)
{
  errno = ENOSYS; // no operating system

  switch(file)
  {
    case STDIN_FILENO:
    case STDOUT_FILENO:
    case STDERR_FILENO:
      return 1;

    default:
      return 0;
  }
}
