
#line 1 "drivers/apc/db_lexer.rl"
#include <apc/apclib.h>
#include <common.h>
#include "db_parser.h"
#define SAVE_TOKEN memcpy(yylval->string, ts + 1, te - ts - 2); yylval->string[te - ts - 2] = '\0'
#define TOKEN(t) (yylval->token = t)

#line 10 "drivers/apc/db_lexer.ragel.o.c"
static const char _db_lexer_actions[] = {
	0, 1, 0, 1, 1, 1, 2, 1, 
	3, 1, 4, 1, 5, 1, 6, 1, 
	7, 1, 8, 1, 9
};

static const char _db_lexer_key_offsets[] = {
	0, 0, 4, 7, 8, 10, 21
};

static const char _db_lexer_trans_keys[] = {
	32, 33, 35, 126, 34, 32, 126, 47, 
	32, 126, 13, 32, 34, 44, 47, 60, 
	62, 91, 93, 9, 10, 32, 126, 0
};

static const char _db_lexer_single_lengths[] = {
	0, 0, 1, 1, 0, 9, 0
};

static const char _db_lexer_range_lengths[] = {
	0, 2, 1, 0, 1, 1, 1
};

static const char _db_lexer_index_offsets[] = {
	0, 0, 3, 6, 8, 10, 21
};

static const char _db_lexer_trans_targs[] = {
	2, 2, 0, 5, 2, 0, 4, 0, 
	6, 0, 5, 5, 1, 5, 3, 5, 
	5, 5, 5, 5, 0, 6, 5, 5, 
	0
};

static const char _db_lexer_trans_actions[] = {
	0, 0, 0, 5, 0, 0, 0, 0, 
	0, 0, 7, 7, 0, 17, 0, 9, 
	11, 13, 15, 7, 0, 0, 19, 19, 
	0
};

static const char _db_lexer_to_state_actions[] = {
	0, 0, 0, 0, 0, 1, 0
};

static const char _db_lexer_from_state_actions[] = {
	0, 0, 0, 0, 0, 3, 0
};

static const char _db_lexer_eof_trans[] = {
	0, 0, 0, 0, 0, 0, 24
};

static const int db_lexer_start = 5;
static const int db_lexer_first_final = 5;
static const int db_lexer_error = 0;

static const int db_lexer_en_main = 5;


#line 9 "drivers/apc/db_lexer.rl"



#line 22 "drivers/apc/db_lexer.rl"

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
  
#line 93 "drivers/apc/db_lexer.ragel.o.c"
	{
	cs = db_lexer_start;
	ts = 0;
	te = 0;
	act = 0;
	}

#line 101 "drivers/apc/db_lexer.ragel.o.c"
	{
	int _klen;
	unsigned int _trans;
	const char *_acts;
	unsigned int _nacts;
	const char *_keys;

	if ( p == pe )
		goto _test_eof;
	if ( cs == 0 )
		goto _out;
_resume:
	_acts = _db_lexer_actions + _db_lexer_from_state_actions[cs];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 ) {
		switch ( *_acts++ ) {
	case 1:
#line 1 "NONE"
	{ts = p;}
	break;
#line 122 "drivers/apc/db_lexer.ragel.o.c"
		}
	}

	_keys = _db_lexer_trans_keys + _db_lexer_key_offsets[cs];
	_trans = _db_lexer_index_offsets[cs];

	_klen = _db_lexer_single_lengths[cs];
	if ( _klen > 0 ) {
		const char *_lower = _keys;
		const char *_mid;
		const char *_upper = _keys + _klen - 1;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + ((_upper-_lower) >> 1);
			if ( (*p) < *_mid )
				_upper = _mid - 1;
			else if ( (*p) > *_mid )
				_lower = _mid + 1;
			else {
				_trans += (unsigned int)(_mid - _keys);
				goto _match;
			}
		}
		_keys += _klen;
		_trans += _klen;
	}

	_klen = _db_lexer_range_lengths[cs];
	if ( _klen > 0 ) {
		const char *_lower = _keys;
		const char *_mid;
		const char *_upper = _keys + (_klen<<1) - 2;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + (((_upper-_lower) >> 1) & ~1);
			if ( (*p) < _mid[0] )
				_upper = _mid - 2;
			else if ( (*p) > _mid[1] )
				_lower = _mid + 2;
			else {
				_trans += (unsigned int)((_mid - _keys)>>1);
				goto _match;
			}
		}
		_trans += _klen;
	}

_match:
_eof_trans:
	cs = _db_lexer_trans_targs[_trans];

	if ( _db_lexer_trans_actions[_trans] == 0 )
		goto _again;

	_acts = _db_lexer_actions + _db_lexer_trans_actions[_trans];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 )
	{
		switch ( *_acts++ )
		{
	case 2:
#line 13 "drivers/apc/db_lexer.rl"
	{te = p+1;{position->start = te; SAVE_TOKEN; return LIBNAME;}}
	break;
	case 3:
#line 14 "drivers/apc/db_lexer.rl"
	{te = p+1;{;}}
	break;
	case 4:
#line 16 "drivers/apc/db_lexer.rl"
	{te = p+1;{position->start = te; return TOKEN(ST);}}
	break;
	case 5:
#line 17 "drivers/apc/db_lexer.rl"
	{te = p+1;{position->start = te; return TOKEN(LT);}}
	break;
	case 6:
#line 18 "drivers/apc/db_lexer.rl"
	{te = p+1;{position->start = te; return TOKEN(LSBRACE);}}
	break;
	case 7:
#line 19 "drivers/apc/db_lexer.rl"
	{te = p+1;{position->start = te; return TOKEN(RSBRACE);}}
	break;
	case 8:
#line 20 "drivers/apc/db_lexer.rl"
	{te = p+1;{position->start = te; return TOKEN(COMMA);}}
	break;
	case 9:
#line 15 "drivers/apc/db_lexer.rl"
	{te = p;p--;{;}}
	break;
#line 219 "drivers/apc/db_lexer.ragel.o.c"
		}
	}

_again:
	_acts = _db_lexer_actions + _db_lexer_to_state_actions[cs];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 ) {
		switch ( *_acts++ ) {
	case 0:
#line 1 "NONE"
	{ts = 0;}
	break;
#line 232 "drivers/apc/db_lexer.ragel.o.c"
		}
	}

	if ( cs == 0 )
		goto _out;
	if ( ++p != pe )
		goto _resume;
	_test_eof: {}
	if ( p == eof )
	{
	if ( _db_lexer_eof_trans[cs] > 0 ) {
		_trans = _db_lexer_eof_trans[cs] - 1;
		goto _eof_trans;
	}
	}

	_out: {}
	}

#line 41 "drivers/apc/db_lexer.rl"


  return token;
}
