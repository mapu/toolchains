%{
#include <common.h>
#include <apc/apclib.h>
#include <apc/loader.h>
int yyerror(struct vlog_param_strt *param, const char *s) { 
  printf("ERROR: %s, pos: %s\n", s, param->position.start); 
  return 0;
}
static unsigned char *vlog_buf = 0;
#define YYLEX_PARAM (&(param->position))
%}
%define api.pure
%name-prefix="vlog"
%parse-param { struct vlog_param_strt *param }

%union {
  unsigned int addr;
  unsigned char val;
}
%token <val> VALUE
%token <addr> ADDR

%type <addr> loads confs conf data

%start loads

%code provides {
extern int vloglex(YYSTYPE * yylval, struct pos_strt *position);
}

%%
loads : confs { } | { $$ = 0; };
confs: conf {
}|
       confs conf {
};
conf: ADDR data {
  send_vector($1 + param->dst, param->src, vlog_buf - (unsigned char *)param->src, 0,
              param->cpuid);
}|
      data {
  send_vector(param->dst, param->src, vlog_buf - (unsigned char *)param->src, 0,
              param->cpuid);
};
data: data VALUE {
  *vlog_buf = $2;
  vlog_buf++;
}| 
      VALUE {
  vlog_buf = (unsigned char *)param->src;
  *vlog_buf = $1;
  vlog_buf++;
};

%%

