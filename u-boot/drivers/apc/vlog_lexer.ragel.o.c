
#line 1 "drivers/apc/vlog_lexer.rl"
#include <apc/apclib.h>
#include <common.h>
#include "vlog_parser.h"
#define SAVE_TOKEN yylval->val = simple_strtoul(ts, NULL, 16)

#line 9 "drivers/apc/vlog_lexer.ragel.o.c"
static const char _db_lexer_actions[] = {
	0, 1, 0, 1, 1, 1, 2, 1, 
	3, 1, 4, 1, 5
};

static const char _db_lexer_key_offsets[] = {
	0, 0, 1, 3, 9, 21, 23, 29
};

static const char _db_lexer_trans_keys[] = {
	47, 32, 126, 48, 57, 65, 70, 97, 
	102, 13, 32, 47, 64, 9, 10, 48, 
	57, 65, 70, 97, 102, 32, 126, 48, 
	57, 65, 70, 97, 102, 48, 57, 65, 
	70, 97, 102, 0
};

static const char _db_lexer_single_lengths[] = {
	0, 1, 0, 0, 4, 0, 0, 0
};

static const char _db_lexer_range_lengths[] = {
	0, 0, 1, 3, 4, 1, 3, 3
};

static const char _db_lexer_index_offsets[] = {
	0, 0, 2, 4, 8, 17, 19, 23
};

static const char _db_lexer_indicies[] = {
	0, 1, 2, 1, 3, 3, 3, 1, 
	4, 4, 5, 7, 4, 6, 6, 6, 
	1, 2, 8, 6, 6, 6, 9, 3, 
	3, 3, 10, 0
};

static const char _db_lexer_trans_targs[] = {
	2, 0, 5, 7, 4, 1, 6, 3, 
	4, 4, 4
};

static const char _db_lexer_trans_actions[] = {
	0, 0, 0, 0, 5, 0, 0, 0, 
	9, 11, 7
};

static const char _db_lexer_to_state_actions[] = {
	0, 0, 0, 0, 1, 0, 0, 0
};

static const char _db_lexer_from_state_actions[] = {
	0, 0, 0, 0, 3, 0, 0, 0
};

static const char _db_lexer_eof_trans[] = {
	0, 0, 0, 0, 0, 9, 10, 11
};

static const int db_lexer_start = 4;
static const int db_lexer_first_final = 4;
static const int db_lexer_error = 0;

static const int db_lexer_en_main = 4;


#line 8 "drivers/apc/vlog_lexer.rl"



#line 17 "drivers/apc/vlog_lexer.rl"

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
  
#line 96 "drivers/apc/vlog_lexer.ragel.o.c"
	{
	cs = db_lexer_start;
	ts = 0;
	te = 0;
	act = 0;
	}

#line 104 "drivers/apc/vlog_lexer.ragel.o.c"
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
#line 125 "drivers/apc/vlog_lexer.ragel.o.c"
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
	_trans = _db_lexer_indicies[_trans];
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
#line 13 "drivers/apc/vlog_lexer.rl"
	{te = p+1;{;}}
	break;
	case 3:
#line 12 "drivers/apc/vlog_lexer.rl"
	{te = p;p--;{position->start = te; yylval->addr = simple_strtoul(ts + 1, NULL, 16); return ADDR;}}
	break;
	case 4:
#line 14 "drivers/apc/vlog_lexer.rl"
	{te = p;p--;{;}}
	break;
	case 5:
#line 15 "drivers/apc/vlog_lexer.rl"
	{te = p;p--;{position->start = te; SAVE_TOKEN; return VALUE;}}
	break;
#line 207 "drivers/apc/vlog_lexer.ragel.o.c"
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
#line 220 "drivers/apc/vlog_lexer.ragel.o.c"
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

#line 36 "drivers/apc/vlog_lexer.rl"


  return token;
}
