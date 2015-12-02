%{
#include <common.h>
#include <apc/apclib.h>
int yyerror(struct pos_strt *position, const char *s) { 
  printf("ERROR: %s, pos: %s\n", s, position->start); 
  return 0;
}
extern struct list_head apclib_db;
extern struct list_head slib_db;
extern struct list_head mlib_db;
int mcnt = 0;
mlib_t *m_pool[256] = {NULL};
%}
%define api.pure
%name-prefix="db"
%parse-param { struct pos_strt *position }
%lex-param   { struct pos_strt *position }

%union {
  char string[256];
  struct record_strt *record_t;
  int token;
}
%token <token> ST LT LSBRACE RSBRACE COMMA
%token <string> LIBNAME

%type <token> db mlist records
%type <record_t> record

%start db

%code provides {
extern int dblex(YYSTYPE * yylval, struct pos_strt *position);
}

%%
db : records { $$ = 0; } | { $$ = 0; };
records: record {
	INIT_LIST_HEAD(&$1->list);
	list_add_tail(&$1->list, &apclib_db);
  $$ = 0;
}|
         record records {
	INIT_LIST_HEAD(&$1->list);
	list_add_tail(&$1->list, &apclib_db);
  $$ = 0;
};
record: ST LIBNAME COMMA LSBRACE mlist RSBRACE LT {
  int i = 0;
  slib_t *p = NULL;
  struct list_head *entry;
  list_for_each(entry, &slib_db) {
    p = list_entry(entry, slib_t, list);
    if (p && !strncmp(p->name, $2, strlen($2))) break;
    else p = NULL;
  }
  if (!p) yyerror(position, "cannot find slib");
  $$ = malloc(sizeof(struct record_strt));
  $$->slib = p;
  $$->mlibs = malloc(sizeof(struct mlib_t *) * mcnt);
  for (i = 0; i < mcnt; i++)
    $$->mlibs[i] = m_pool[i];
  $$->mcnt = mcnt;
  mcnt = 0;
}|
        ST LIBNAME COMMA LSBRACE RSBRACE LT {
  slib_t *p = NULL;
  struct list_head *entry;
  list_for_each(entry, &slib_db) {
    p = list_entry(entry, slib_t, list);
    if (p && !strncmp(p->name, $2, strlen($2))) break;
    else p = NULL;
  }
  if (!p) yyerror(position, "cannot find slib");
  $$ = malloc(sizeof(struct record_strt));
  $$->slib = p;
  $$->mlibs = NULL;
  $$->mcnt = 0;
};
mlist: mlist LIBNAME COMMA {
  mlib_t *p = NULL;
  struct list_head *entry;
  list_for_each(entry, &mlib_db) {
    p = list_entry(entry, mlib_t, list);
    if (p && !strncmp(p->name, $2, strlen($2))) break;
    else p = NULL;
  }
  if (!p) yyerror(position, "cannot find mlib");
  m_pool[mcnt] = p;
  mcnt++;
}| 
       LIBNAME COMMA {
  mlib_t *p = NULL;
  struct list_head *entry;
  list_for_each(entry, &mlib_db) {
    p = list_entry(entry, mlib_t, list);
    if (p && !strncmp(p->name, $1, strlen($1))) break;
    else p = NULL;
  }
  if (!p) yyerror(position, "cannot find mlib");
  m_pool[mcnt] = p;
  mcnt++;
};

%%

