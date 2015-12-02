#include <apc/apclib.h>
#include <common.h>
#include "vlog_parser.h"
#define SAVE_TOKEN yylval->val = simple_strtoul(ts, NULL, 16)
%%{
  machine db_lexer;
  write data;
}%%

%%{
  main := |*
"@"xdigit+            {position->start = te; yylval->addr = simple_strtoul(ts + 1, NULL, 16); return ADDR;};
[ \t\n\r]             {;};
'//'print+						{;};
xdigit+               {position->start = te; SAVE_TOKEN; return VALUE;};
*|;
}%%
int vloglex(YYSTYPE * yylval, struct pos_strt *position) {
  const char *p  = position->start; 	// start point for ragel lexing.
  const char *pe = position->end;   	// lexing end.

  // vars for ragel scanners
  int cs; // init state
  const char *ts, *te;
  int act;
  const char *eof = 0;

  // variables accessed within ragel blocks
  int token = -1;

  // this is where ragel-lexer works.
  %%{
    # Initialize and execute.
    write init;
    write exec;
  }%%

  return token;
}
