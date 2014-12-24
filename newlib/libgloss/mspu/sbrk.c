/* Version of sbrk for no operating system. */

#include "config.h"
#include <_syslist.h>

// positions provided by linker
extern char  __mspu_heap_start[];
extern char  __mspu_stack_top[];

// next available heap position
static char *mspu_heap_ptr = 0;

// current SP
unsigned __mspu_get_sp();

// exit
void _exit(int);

void *
sbrk (incr) int incr;
{

   char* prev;

   if (mspu_heap_ptr == 0)
     mspu_heap_ptr = (char *) & __mspu_heap_start;

   prev = mspu_heap_ptr;
   mspu_heap_ptr += incr;

   if(mspu_heap_ptr <  (char *) & __mspu_stack_top  &&
      mspu_heap_ptr <= (char *) __mspu_get_sp())
     return (void *) prev;

   //return (void *) (-1);

   // for gem5, we better exit simulation to expose runtime error early.
   // 0x1019 is a weired value to indicate sbrk error.
   _exit(0x1019);
}
