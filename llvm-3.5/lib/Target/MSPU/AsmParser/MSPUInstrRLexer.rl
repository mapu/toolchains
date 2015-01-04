/*
 * replace the lexer generated by flex
 */

%%{
	machine MSPUInstrLexer;
	write data;
}%%

%%{ #// sub FSMs
	main := |*
		'r'i ( '0' | ([1-9][0-9]*)) {
		assert(atoi(ts+1)<32);
		yylval->op = MSPU::MSPUAsmOperand::createReg((MSPU::MSPUAsmParser::getRegInfo())->getRegClass(MSPUReg::RRegRegClassID).getRegister(atoi(ts+1)));
		MSPU::MSPUAsmParser::setPreLoc(SMLoc::getFromPointer(ts)); start = te; return _RReg;
	};

'j'i ('0'|([1-9][0-9]*)) {
		assert(atoi(ts+1)<32);
		yylval->op = MSPU::MSPUAsmOperand::createReg((MSPU::MSPUAsmParser::getRegInfo())->getRegClass(MSPUReg::JRegRegClassID).getRegister(atoi(ts+1)));
		MSPU::MSPUAsmParser::setPreLoc(SMLoc::getFromPointer(ts)); start = te; return _JReg;
	};

"sp"i {
		yylval->op = MSPU::MSPUAsmOperand::createReg(MSPUReg::J29);
		MSPU::MSPUAsmParser::setPreLoc(SMLoc::getFromPointer(ts)); start = te; return _JReg;
	};

"fp"i {
		yylval->op = MSPU::MSPUAsmOperand::createReg(MSPUReg::J28);
		MSPU::MSPUAsmParser::setPreLoc(SMLoc::getFromPointer(ts)); start = te; return _JReg;
	};

"dr"i('0'|([1-9][0-9]*)) {
		assert(atoi(ts+2)<16);
		yylval->op = MSPU::MSPUAsmOperand::createReg((MSPU::MSPUAsmParser::getRegInfo())->getRegClass(MSPUReg::DRegRegClassID).getRegister(atoi(ts+2)));
		MSPU::MSPUAsmParser::setPreLoc(SMLoc::getFromPointer(ts)); start = te; return _DReg;
	};

"kb"i('0'|([1-9][0-9]*)) {
		assert(atoi(ts+2)<16);
		yylval->op = MSPU::MSPUAsmOperand::createReg((MSPU::MSPUAsmParser::getRegInfo())->getRegClass(MSPUReg::KBRegRegClassID).getRegister(atoi(ts+2)));
		MSPU::MSPUAsmParser::setPreLoc(SMLoc::getFromPointer(ts)); start = te; return _KReg;
	};

"ke"i('0'|([1-9][0-9]*)) {
		assert(atoi(ts+2)<16);
		yylval->op = MSPU::MSPUAsmOperand::createReg((MSPU::MSPUAsmParser::getRegInfo())->getRegClass(MSPUReg::KERegRegClassID).getRegister(atoi(ts+2)));
		MSPU::MSPUAsmParser::setPreLoc(SMLoc::getFromPointer(ts)); start = te; return _KReg;
	};

"ks"i('0'|([1-9][0-9]*)) {
		assert(atoi(ts+2)<16);
		yylval->op = MSPU::MSPUAsmOperand::createReg((MSPU::MSPUAsmParser::getRegInfo())->getRegClass(MSPUReg::KSRegRegClassID).getRegister(atoi(ts+2)));
		MSPU::MSPUAsmParser::setPreLoc(SMLoc::getFromPointer(ts)); start = te; return _KReg;
	};

"ki"i('0'|([1-9][0-9]*)) {
		assert(atoi(ts+2)<16);
		yylval->op = MSPU::MSPUAsmOperand::createReg((MSPU::MSPUAsmParser::getRegInfo())->getRegClass(MSPUReg::KIRegRegClassID).getRegister(atoi(ts+2)));
		MSPU::MSPUAsmParser::setPreLoc(SMLoc::getFromPointer(ts)); start = te; return _KReg;
	};

"km"i('0'|'1'|'2') {
		yylval->op = MSPU::MSPUAsmOperand::createReg((MSPU::MSPUAsmParser::getRegInfo())->getRegClass(MSPUReg::KMRegRegClassID).getRegister(atoi(ts+2)));
		MSPU::MSPUAsmParser::setPreLoc(SMLoc::getFromPointer(ts)); start = te; return _KMReg;
	};

"kg"i('0'|'1'|'2') {
		yylval->op = MSPU::MSPUAsmOperand::createReg((MSPU::MSPUAsmParser::getRegInfo())->getRegClass(MSPUReg::KGRegRegClassID).getRegister(atoi(ts+2)));
		MSPU::MSPUAsmParser::setPreLoc(SMLoc::getFromPointer(ts)); start = te; return _KReg;
	};

"kl"i('0'|'1'|'2') {
		yylval->op = MSPU::MSPUAsmOperand::createReg((MSPU::MSPUAsmParser::getRegInfo())->getRegClass(MSPUReg::KLRegRegClassID).getRegister(atoi(ts+2)));
		MSPU::MSPUAsmParser::setPreLoc(SMLoc::getFromPointer(ts)); start = te; return _KReg;
	};

"svr"i('0'|'1') {
		yylval->op = MSPU::MSPUAsmOperand::createReg((MSPU::MSPUAsmParser::getRegInfo())->getRegClass(MSPUReg::SVRRegRegClassID).getRegister(atoi(ts+3)));
		MSPU::MSPUAsmParser::setPreLoc(SMLoc::getFromPointer(ts)); start = te; return _SVRReg;
	};

"shu"i('0'|'1') {
		yylval->op = MSPU::MSPUAsmOperand::createReg((MSPU::MSPUAsmParser::getRegInfo())->getRegClass(MSPUReg::SHURegRegClassID).getRegister(atoi(ts+3)));
		MSPU::MSPUAsmParser::setPreLoc(SMLoc::getFromPointer(ts)); start = te; return _SHUReg;
	};

"t"i('0'|'1'|'2'|'3') {
		yylval->op = MSPU::MSPUAsmOperand::createReg((MSPU::MSPUAsmParser::getRegInfo())->getRegClass(MSPUReg::TRegRegClassID).getRegister(atoi(ts+1)));
		MSPU::MSPUAsmParser::setPreLoc(SMLoc::getFromPointer(ts)); start = te; return _TReg;
	};


"mc"i[ \t]*'.'[ \t]*"r0"i {
    yylval->op = MSPU::MSPUAsmOperand::createReg(MSPUReg::MC0);
    MSPU::MSPUAsmParser::setPreLoc(SMLoc::getFromPointer(ts)); start = te; return _MCReg;
  };

"mc"i[ \t]*'.'[ \t]*"r1"i {
    yylval->op = MSPU::MSPUAsmOperand::createReg(MSPUReg::MC1);
    MSPU::MSPUAsmParser::setPreLoc(SMLoc::getFromPointer(ts)); start = te; return _MCReg;
  };

"mc"i[ \t]*'.'[ \t]*"w0"i {
    yylval->op = MSPU::MSPUAsmOperand::createReg(MSPUReg::MC2);
    MSPU::MSPUAsmParser::setPreLoc(SMLoc::getFromPointer(ts)); start = te; return _MCReg;
  };

"mc"i[ \t]*'.'[ \t]*"w1"i {
    yylval->op = MSPU::MSPUAsmOperand::createReg(MSPUReg::MC3);
    MSPU::MSPUAsmParser::setPreLoc(SMLoc::getFromPointer(ts)); start = te; return _MCReg;
  };

"mc"i[ \t]*'.'[ \t]*"w2"i {
    yylval->op = MSPU::MSPUAsmOperand::createReg(MSPUReg::MC4);
    MSPU::MSPUAsmParser::setPreLoc(SMLoc::getFromPointer(ts)); start = te; return _MCReg;
  };

"cctrl"i { MSPU::MSPUAsmParser::setPreLoc(SMLoc::getFromPointer(ts)); start = te; return _CCtrl; };
"cstat"i { MSPU::MSPUAsmParser::setPreLoc(SMLoc::getFromPointer(ts)); start = te; return _CStat; };
"stat"i  { MSPU::MSPUAsmParser::setPreLoc(SMLoc::getFromPointer(ts)); start = te; return _Stat; };
"lpctr"i { MSPU::MSPUAsmParser::setPreLoc(SMLoc::getFromPointer(ts)); start = te; return _LPCtr; };
"mc"i    { MSPU::MSPUAsmParser::setPreLoc(SMLoc::getFromPointer(ts)); start = te; return _MC; };
"vect"i  { MSPU::MSPUAsmParser::setPreLoc(SMLoc::getFromPointer(ts)); start = te; return _Vect;};

"==" { MSPU::MSPUAsmParser::setPreLoc(SMLoc::getFromPointer(ts)); start = te; return _EQ; };
"!=" { MSPU::MSPUAsmParser::setPreLoc(SMLoc::getFromPointer(ts)); start = te; return _NE; };
">"  { MSPU::MSPUAsmParser::setPreLoc(SMLoc::getFromPointer(ts)); start = te; return _GT; };
">=" { MSPU::MSPUAsmParser::setPreLoc(SMLoc::getFromPointer(ts)); start = te; return _GE; };
"<"  { MSPU::MSPUAsmParser::setPreLoc(SMLoc::getFromPointer(ts)); start = te; return _LT; };
"<=" { MSPU::MSPUAsmParser::setPreLoc(SMLoc::getFromPointer(ts)); start = te; return _LE; };

"<<" { MSPU::MSPUAsmParser::setPreLoc(SMLoc::getFromPointer(ts)); start = te; return _SL; };
">>" { MSPU::MSPUAsmParser::setPreLoc(SMLoc::getFromPointer(ts)); start = te; return _SR; };

";;" { MSPU::MSPUAsmParser::setPreLoc(SMLoc::getFromPointer(ts)); start = te; return _EOS; };
";" { MSPU::MSPUAsmParser::setPreLoc(SMLoc::getFromPointer(ts)); start = te; return _Pl; };
"+=" { MSPU::MSPUAsmParser::setPreLoc(SMLoc::getFromPointer(ts)); start = te; return _PlusE; };

"abs"i 	 	{ MSPU::MSPUAsmParser::setPreLoc(SMLoc::getFromPointer(ts)); start = te; return _Abs;};
"rsqrt"i 	 	{ MSPU::MSPUAsmParser::setPreLoc(SMLoc::getFromPointer(ts)); start = te; return _RSqRt;};
"recip"i 	 	{ MSPU::MSPUAsmParser::setPreLoc(SMLoc::getFromPointer(ts)); start = te; return _Recip;};
"single"i	{ MSPU::MSPUAsmParser::setPreLoc(SMLoc::getFromPointer(ts)); start = te; return _Single;};
"double"i	{ MSPU::MSPUAsmParser::setPreLoc(SMLoc::getFromPointer(ts)); start = te; return _Double;};
"int"i 		{ MSPU::MSPUAsmParser::setPreLoc(SMLoc::getFromPointer(ts)); start = te; return _Int;};
"uint"i 		{ MSPU::MSPUAsmParser::setPreLoc(SMLoc::getFromPointer(ts)); start = te; return _UInt;};
"jump"i 		{ MSPU::MSPUAsmParser::setPreLoc(SMLoc::getFromPointer(ts)); start = te; return _Jump;};
"call"i 		{ MSPU::MSPUAsmParser::setPreLoc(SMLoc::getFromPointer(ts)); start = te; return _Call;};
"callm"i 		{ MSPU::MSPUAsmParser::setPreLoc(SMLoc::getFromPointer(ts)); start = te; return _CallM;};
"syscall"i  { MSPU::MSPUAsmParser::setPreLoc(SMLoc::getFromPointer(ts)); start = te; return _SysCall;};
"if"i 			{ MSPU::MSPUAsmParser::setPreLoc(SMLoc::getFromPointer(ts)); start = te; return _If;};
"lpto"i 		{ MSPU::MSPUAsmParser::setPreLoc(SMLoc::getFromPointer(ts)); start = te; return _LpTo;};
"by"i 			{ MSPU::MSPUAsmParser::setPreLoc(SMLoc::getFromPointer(ts)); start = te; return _By;};
"stop"i |
"spu"i[ \t]*'.'[ \t]*"stop"i |
"spustop"i { MSPU::MSPUAsmParser::setPreLoc(SMLoc::getFromPointer(ts)); start = te; return _Stop;};
"nop"i { MSPU::MSPUAsmParser::setPreLoc(SMLoc::getFromPointer(ts)); start = te; return _NOP;};
"ch"i { MSPU::MSPUAsmParser::setPreLoc(SMLoc::getFromPointer(ts)); start = te; return _CH;};

'+'	|
'~'	|
'-'	|
'*'	|
'&'	|
'|'	|
'^'	|
'='	|
','	|
'.' |
'['	|
']'	|
'(' |
')' |
'{' |
'}' |
'#' 	{MSPU::MSPUAsmParser::setPreLoc(SMLoc::getFromPointer(ts)); start = te; return *ts;};

'('[ \t]*"l0"i[ \t]*')' { MSPU::MSPUAsmParser::setPreLoc(SMLoc::getFromPointer(ts)); start = te; return _L0;};
'('[ \t]*"l1"i[ \t]*')' { MSPU::MSPUAsmParser::setPreLoc(SMLoc::getFromPointer(ts)); start = te; return _L1;};
'('[ \t]*"at"i[ \t]*')' { MSPU::MSPUAsmParser::setPreLoc(SMLoc::getFromPointer(ts)); start = te; return _AT;};
'('[ \t]*"t"i[ \t]*')' { MSPU::MSPUAsmParser::setPreLoc(SMLoc::getFromPointer(ts)); start = te; return _T;};
'('[ \t]*"b"i[ \t]*')' { MSPU::MSPUAsmParser::setPreLoc(SMLoc::getFromPointer(ts)); start = te; return _B;};
'('[ \t]*'h'i[ \t]*')' { MSPU::MSPUAsmParser::setPreLoc(SMLoc::getFromPointer(ts)); start = te; return _H;};
'('[ \t]*'l'i[ \t]*')' { MSPU::MSPUAsmParser::setPreLoc(SMLoc::getFromPointer(ts)); start = te; return _L;};
'('[ \t]*'n'i[ \t]*')' { MSPU::MSPUAsmParser::setPreLoc(SMLoc::getFromPointer(ts)); start = te; return _N;};
'('[ \t]*'u'i[ \t]*')' { MSPU::MSPUAsmParser::setPreLoc(SMLoc::getFromPointer(ts)); start = te; return _U;};
'('[ \t]*'s'i[ \t]*')' { MSPU::MSPUAsmParser::setPreLoc(SMLoc::getFromPointer(ts)); start = te; return _S;};
'('[ \t]*'d'i[ \t]*')' { MSPU::MSPUAsmParser::setPreLoc(SMLoc::getFromPointer(ts)); start = te; return _D;};
'('[ \t]*"ci"i[ \t]*')' { MSPU::MSPUAsmParser::setPreLoc(SMLoc::getFromPointer(ts)); start = te; return _CI;};
'('[ \t]*'x'i[ \t]*')' { MSPU::MSPUAsmParser::setPreLoc(SMLoc::getFromPointer(ts)); start = te; return _X;};
'('[ \t]*'y'i[ \t]*')' { MSPU::MSPUAsmParser::setPreLoc(SMLoc::getFromPointer(ts)); start = te; return _Y;};
'('[ \t]*"xy"i[ \t]*')' { MSPU::MSPUAsmParser::setPreLoc(SMLoc::getFromPointer(ts)); start = te; return _XY;};

'-'?([0-9]+|("0x"i[0-9a-f]+))	{
    // let llvm handle expression
		// yylval->op = MSPU::MSPUAsmOperand::createImm(strtol(ts, NULL, 0));
		MSPU::MSPUAsmParser::setPreLoc(SMLoc::getFromPointer(ts)); start = te; return _Imm;
	};

'-'?[0-9]+'.'[0-9]*                |
'-'?[0-9]+'.'[0-9]*('e'i['-''+']?[0-9]+)  |
'-'?'.'[0-9]+			          |
'-'?'.'[0-9]+('e'i['-''+']?[0-9]+)  |
'-'?[0-9]+('e'i['-''+']?[0-9]+)     {
		yylval->op = MSPU::MSPUAsmOperand::createFPImm(strtod(ts, NULL));
		MSPU::MSPUAsmParser::setPreLoc(SMLoc::getFromPointer(ts)); start = te; return _FPImm;
	};

[a-zA-Z_][a-zA-Z_0-9]* {
    // let llvm handle expression
    //StringRef * ref = new StringRef(MSPU::MSPUAsmParser::getParser()->getTok().getString().data(), strlen(ts));
    //yylval->op = MSPU::MSPUAsmOperand::createExpr(ref);
		MSPU::MSPUAsmParser::setPreLoc(SMLoc::getFromPointer(ts)); start = te; return _Symb;
	};

[ \t\n\r]+			{};

any 	{ MSPU::MSPUAsmParser::setPreLoc(SMLoc::getFromPointer(ts)); start = te; return *ts; /*printf("flex scanned unknown character: %c\n", *ts);*/ };

  *|;

}%%

// a direct wrapper for lexer generated by ragel.
// *start* is modified when this wrapper returns.
static int msputoklex(YYSTYPE * yylval, const char * &start, const char * end)
{
  const char*p  = start; 	// start point for ragel lexing.
  const char*pe = end;   	// line end.

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
