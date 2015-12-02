#include <apc/apclib.h>
#include <common.h>
#include "db_parser.h"
#define SAVE_TOKEN memcpy(yylval->string, ts + 1, te - ts - 2); yylval->string[te - ts - 2] = '\0'
#define TOKEN(t) (yylval->token = t)
%%{
  machine db_lexer;
  write data;
}%%

%%{
  main := |*
'"'(print - '"')+'"'  {position->start = te; SAVE_TOKEN; return LIBNAME;};
[ \t\n\r]             {;};
'//'print+						{;};
"<"                   {position->start = te; return TOKEN(ST);};
">"                   {position->start = te; return TOKEN(LT);};
"["                   {position->start = te; return TOKEN(LSBRACE);};
"]"                   {position->start = te; return TOKEN(RSBRACE);};
","                   {position->start = te; return TOKEN(COMMA);};
*|;
}%%
int dblex(YYSTYPE * yylval, struct pos_strt *position) {
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
