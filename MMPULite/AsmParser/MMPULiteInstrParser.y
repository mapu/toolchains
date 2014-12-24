%{
#include <string>
#include <bitset>

#define OPERAND(TY, VALUE, START, END) \
MMPULite::MMPULiteAsmOperand::create##TY(VALUE, START, END)
#define ADDOPERAND(TY, VALUE, START, END) \
Operands.push_back(MMPULite::MMPULiteAsmOperand::create##TY(VALUE, START, END))
std::bitset<16> flags;
static int flagsort;
const unsigned BF=0, HF=1, UF=2, TF=3, SF=4, DF=5, IF=6, LF=7, APPF=8, KPPF=9, CRF=10, BRF=11, MF=12, TCF=13, CDF=14, NCF=15;
static MMPULite::MMPULiteAsmOperand *opc, *tm, *tn, *tp, *f, *unit, *ut, *b, *md, *ms, *imm, *expr, *ipath;
static int slotid;
static unsigned condpos;
SMLoc FlagS, FlagE;
%}

%code requires {
//Try to trace token's location in terms of LLVM SMLoc
#if !YYLTYPE_IS_DECLARED
typedef struct YYLTYPE {
  SMLoc S;
  SMLoc E;
} YYLTYPE;
#define YYLTYPE_IS_DECLARED 1
#endif
#define YYLLOC_DEFAULT(Current, Rhs, N)               \
  do                                                  \
    if (N) {                                          \
      (Current).S = YYRHSLOC(Rhs, 1).S;               \
      (Current).E = YYRHSLOC(Rhs, N).E;               \
    }                                                 \
    else                                              \
      (Current).S = (Current).E = YYRHSLOC(Rhs, 0).S; \
  while (0)
}

%locations

//%output  	"MMPULiteGenInstrParser.bison"
%defines 	"MMPULiteGenInstrParser.h"
%define api.pure
%name-prefix="mmpulite"

%parse-param { SmallVectorImpl<MCParsedAsmOperand*> &Operands }

%union {
	MMPULite::MMPULiteAsmOperand* op;
  std::string *string;
  int val;
  int token;
}
%token <val> NEGIMM IMM3 IMM IMM5 ASSIGNTO EQU NEQ ST NLT LT NST LPAREN RPAREN DOT COMMA ADD SUB MUL CMUL LSHT RSHT
%token <val> OR AND XOR NOT NEG ADDSUB ACC1 ACC2 ALPHA SPLIT LINEEND SHU0 SHU1 BIU0 BIU1 BIU2 M COND
%token <val> IALU IMAC FALU FMAC MINDEXI MINDEXS TB TBB TBH TBW TBD TSQ IND BY
%token <val> CPRS EXPD START STOP MAX MIN ABS MERGE MDIVR MDIVQ DIVR DIVQ DIVS RECIP RSQRT SINGLE DOUBLE MR INT RMAX RMIN
%token <val> REPEAT LOOP JMP MPUSTOP
%token <val> BR CR APP KPP U T B H S D I L TC C LABEL SHU BIU 
%token <val> TRUE ASSIGN NOOP UINT DM KG R0 R1 R2 R3 IPATH WFLAG
%token <string> IDENTIFIER
%token <op> EXPR
%token <val> TREG MINDEXN KI

%type <val> slots slotref mr012slot mr3slot mr3slot_ shuslot shu0code shu1code biuslot biuheader biucode ialuslot imacslot faluslot fmacslot seqslot
%type <val> r0inst r1inst r2inst r2instw r3inst
%type <val> shu0inst shu1inst shuexp indexp indclause indtnclause indtbclause byexp byflag byclause tripara dupara shu0ctrl shu1ctrl
%type <val> ldinst ldop stinst storeop kginst
%type <val> ialuinst ialuclause condclause utbclause uthclause iaddclause isubclause addexp subexp ubclause imaxclause maxexp iminclause minexp iabsclause abssexp iexpdclause expdexp iltclause ltexp instclause nstexp istclause stexp inltclause rshtclause nltexp bclause b_uclause mrgclause mrgexp iequclause equexp ineqclause neqexp rshtexp rshtt rshti inoflagclause inoflagexp divrop uaryexp binaryexp binaryop lshtclause lshtexp cprsexp divinst divexp divsexp divqop mdivrop mdivqop reduceexp ialuctrl
%type <val> imacinst imacclause mulexp imulclause muladsbexp mrclause mrinst exmacexp inmacexp accexp iexmacclause iinmacclause tregclause imacctrl
%type <val> faluinst faluclause tsdclause faddclause fsubclause sdclause fmaxclause fminclause fabsclause absexp recipclause recipexp rsqrtclause rsqrtexp fequclause fneqclause fltclause fnstclause fstclause fnltclause intclause intexp intop uintop tclause tosexp tdclause sclause todexp fasexp fuclause faluctrl
%type <val> fmacinst fmacclause fmulclause cmulexp fmrclause fmrinst fexmacclause finmacclause fmacctrl
%type <val> reinst repeatexp immrep regrep loops loop lpinst lpexp lpcond jmpinst condexp mpustop
%type <val> r2destp r2dest r1destp r1dest r0dest r3dest lddest shu0dest shu1dest mindex ialudest imacdest faludest fmacdest ialut imact falut fmact biu0 biu1 biu2 shu0t shu1t mindexs mindexi mindexn t constt _constt shusrct shudupara
%type <val> imbrakflag ibrakflag imakflag imbraflag imbrkflag imbrflag imaflag imkflag ibraflag ibrkflag iakflag imflag ibrflag iaflag ikflag
%type <val> ldflag stflag mbrakflag brakflag makflag mbrkflag mbraflag akflag mkflag brkflag maflag mbrflag braflag utbflag t_ubflag t_bflag ubflag b_uflag utflag t_uflag t_uibflag uibflag b_uiflag t_ibflag t_uiflag ibflag b_iflag t_iflag uiflag crlubflag crluflag crlbflag crlflag crubflag crbflag cruflag lbflag luflag lubflag tsdflag sdflag tdflag bflag crsdflag condflag uthflag thflag uhflag b__uflag
%type <val> b_flag hflag brflag mflag aflag kflag tflag uflag sflag dflag lflag crflag iflag negflag andflag tcflag kiflag trueflag cflag ncflag
%type <val> ialu imac falu fmac ctrl imm imm5 label mcodeline hmacro _flag flag_

%%
mcodeline: NOOP LINEEND {ADDOPERAND(Opc, MMPULite::NOP, @1.S, @1.E); YYACCEPT;}
|          slots LINEEND {YYACCEPT;};
mcodeline: NOOP error { llvmerror(&@2, "No line end."); YYABORT;};
slots: slotref | slotref SPLIT slots ;
slotref : slot {
  tm = NULL;
  tn = NULL;
  tp = NULL;
  ut = NULL;
  ms = NULL;
  md = NULL;
  imm = NULL;
  f = NULL;
  unit = NULL;
  expr = NULL;
  b = NULL;
  opc = NULL;
  ipath = NULL;
  condpos = 0;
  FlagS = SMLoc();
  FlagE = SMLoc();
};
slot: mr012slot condflag {ADDOPERAND(Slot, slotid, @$.S, @$.E);} |
      mr3slot condflag {ADDOPERAND(Slot, 3, @$.S, @$.E);} |
      shuslot condflag {ADDOPERAND(Slot, 4 + $1, @$.S, @$.E);} |
      ialuslot condflag {ADDOPERAND(Slot, 6, @$.S, @$.E);} |
      imacslot condflag {ADDOPERAND(Slot, 7, @$.S, @$.E);} |
      faluslot condflag {ADDOPERAND(Slot, 8, @$.S, @$.E);} |
      fmacslot condflag {ADDOPERAND(Slot, 9, @$.S, @$.E);} |
      biuslot condflag {ADDOPERAND(Slot, 10 + slotid, @$.S, @$.E);} |
      seqslot |
      hmacro |
      error { llvmerror(&@1, "Unrecognized slot."); YYABORT; };
hmacro: IDENTIFIER {ADDOPERAND(HMacro, $1, @$.S, @$.E);};
hmacro: IDENTIFIER LPAREN RPAREN {ADDOPERAND(HMacro, $1, @$.S, @$.E); ADDOPERAND(Imm, 1, @$.S, @$.E);};
mr012slot: R0 DOT r0inst {
  switch ($3) {
  case 0:
    ADDOPERAND(Opc, MMPULite::MR0ToM, @$.S, @$.E);
    Operands.push_back(md);
    Operands.push_back(NULL);
    condpos = Operands.size();
    Operands.push_back(ms);
    ADDOPERAND(Reg,
               MRI->getRegClass(MMPULiteReg::WFlagRegClassID).getRegister(0),
               @$.S, @$.E);
    break;
  case 1:
    ADDOPERAND(Opc, MMPULite::MR0ToSHU, @$.S, @$.E);
    Operands.push_back(unit);
    Operands.push_back(ut);
    Operands.push_back(NULL);
    condpos = Operands.size();
    Operands.push_back(md);
    ADDOPERAND(Reg,
               MRI->getRegClass(MMPULiteReg::WFlagRegClassID).getRegister(0),
               @$.S, @$.E);
    break;
  case 3:
    ADDOPERAND(Opc, MMPULite::MR0ToMACC, @$.S, @$.E);
    Operands.push_back(unit);
    Operands.push_back(ut);
    Operands.push_back(ipath);
    Operands.push_back(NULL);
    condpos = Operands.size();
    Operands.push_back(md);
    ADDOPERAND(Reg,
               MRI->getRegClass(MMPULiteReg::WFlagRegClassID).getRegister(0),
               @$.S, @$.E);
    break;
  default:
    break;
  }
  slotid = 0;
}
| R1 DOT r1inst {
  if ($3 == 1)
    ADDOPERAND(Opc, MMPULite::MR1ToSHU, @$.S, @$.E);
  else if ($3 == 3)
    ADDOPERAND(Opc, MMPULite::MR1ToMACC, @$.S, @$.E);
  Operands.push_back(unit);
  Operands.push_back(ut);
  if ($3 == 3) Operands.push_back(ipath);
  Operands.push_back(NULL);
  condpos = Operands.size();
  Operands.push_back(md);
  ADDOPERAND(Reg,
             MRI->getRegClass(MMPULiteReg::WFlagRegClassID).getRegister(0),
             @$.S, @$.E);
  slotid = 1;
}
| R2 DOT r2inst {
  ADDOPERAND(Opc, MMPULite::MR2ToMACC, @$.S, @$.E);
  Operands.push_back(unit);
  Operands.push_back(ut);
  Operands.push_back(ipath);
  Operands.push_back(NULL);
  condpos = Operands.size();
  Operands.push_back(md);
  ADDOPERAND(Reg,
             MRI->getRegClass(MMPULiteReg::WFlagRegClassID).getRegister(0),
             @$.S, @$.E);
  slotid = 2;
}
| R2 DOT r2instw {
  ADDOPERAND(Opc, MMPULite::MR2ToMACC, @$.S, @$.E);
  Operands.push_back(unit);
  Operands.push_back(ut);
  Operands.push_back(ipath);
  Operands.push_back(NULL);
  condpos = Operands.size();
  Operands.push_back(md);
  ADDOPERAND(Reg,
             MRI->getRegClass(MMPULiteReg::WFlagRegClassID).getRegister($3),
             @$.S, @$.E);
  slotid = 2;
};
mr3slot: mr3slot_ {
  Operands.push_back(b);
  Operands.push_back(NULL);
  condpos = Operands.size();
  Operands.push_back(md);
  if (!$1)
  ADDOPERAND(Reg,
             MRI->getRegClass(MMPULiteReg::WFlagRegClassID).getRegister(0),
             @$.S, @$.E);
};
mr3slot_: r3inst | R3 DOT r3inst { $$ = $3; };
shuslot: shu0code {$$ = 0;} | shu1code {$$ = 1;};
shu0code: SHU0 DOT shu0inst | shu0ctrl ;
shu1code: SHU1 DOT shu1inst | shu1ctrl ;
biuslot: biuheader biucode ;
biuslot: ldinst { llvmerror(&@1, "BIU0/1/2 must be specified before BIU insts"); YYABORT;};
biuheader : BIU0 {$$ = slotid = 0;} | BIU1 {$$ = slotid = 1;} | BIU2 {$$ = slotid = 2;} ;
biucode: DOT ldinst {
  flagsort = (flags[MF] << 3) | (flags[BRF] << 2) | (flags[APPF] << 1) | flags[KPPF]; 
  //$$ = $2 | (flagsort << 11);
  ADDOPERAND(Imm, flags.to_ulong() << 8 | flagsort, FlagS, FlagE);
  flags.reset();
}
|        stinst {
  flagsort = (flags[IF] << 4) | (flags[MF] << 3) | (flags[BRF] << 2) | (flags[APPF] << 1) | flags[KPPF];
  //$$ = $1 | (flagsort << 11);
  ADDOPERAND(Imm, flags.to_ulong() << 8 | flagsort, FlagS, FlagE);
  flags.reset();
}
|        kginst {$$ = $1; flagsort = 0; flags.reset();};
ialuslot: ialuinst | divinst | ialuctrl ;
imacslot: imacinst | imacctrl | mrinst {
  flagsort = (flags[CRF] << 5) | (flags[LF] << 4) | (flags[IF] << 3) | (flags[UF] << 2) | (flags[BF] << 1) | flags[HF];
  f = OPERAND(Imm, (flags.to_ulong() << 8) | flagsort, FlagS, FlagE);
  flags.reset();
  switch($1) {
  case 1: ADDOPERAND(Opc, MMPULite::IMAcc, @$.S, @$.E); break;
  case 2: ADDOPERAND(Opc, MMPULite::IMaC, @$.S, @$.E); break;
  case 3: ADDOPERAND(Opc, MMPULite::IMA, @$.S, @$.E); break;
  default: break;
  }
  Operands.push_back(NULL);
  condpos = Operands.size();
  Operands.push_back(tm);
  if ($1 >= 2) Operands.push_back(tn);
  if ($1 == 3) Operands.push_back(tp);
  Operands.push_back(f);
};

faluslot: faluinst | faluctrl ;
fmacslot: fmacinst | fmacctrl | fmrinst {
  flagsort = ((flags[TF] | flags[CRF]) << 1) | flags[DF];
  f = OPERAND(Imm, (flags.to_ulong() << 8) | flagsort, FlagS, FlagE);
  flags.reset();
  switch($1) {
  case 2: ADDOPERAND(Opc, MMPULite::FMaC, @$.S, @$.E); break;
  case 3: ADDOPERAND(Opc, MMPULite::FMA, @$.S, @$.E); break;
  default: break;
  }
  Operands.push_back(NULL);
  condpos = Operands.size();
  Operands.push_back(tm);
  Operands.push_back(tn);
  if ($1 == 3) Operands.push_back(tp);
  Operands.push_back(f);
};
seqslot: reinst | lpinst | jmpinst | mpustop ;

r0inst: r1inst {$$ = $1;} |
        mindexn ASSIGNTO r0dest {$$ = $3;} | 
        mindexi ASSIGNTO r0dest {$$ = $3;} |
        mindexs ASSIGNTO r0dest {$$ = $3;} ;
r1inst: r2inst {$$ = $1;} |
        mindexn ASSIGNTO r1destp {$$ = $3;} ;
r1inst: mindexi ASSIGNTO r1destp {$$ = $3;} | 
        mindexi ASSIGNTO r2destp {$$ = $3;} |
        mindexs ASSIGNTO r1destp {$$ = $3;} |
        mindexs ASSIGNTO r2destp {$$ = $3;} ;
r2inst: mindexn ASSIGNTO r2destp {$$ = $3;} ;
r2instw: mindexn _flag WFLAG flag_  ASSIGNTO r2destp {$$ = $3;} ;
r3inst: mindexn ASSIGNTO r3dest _flag KG flag_ {
  $$ = 1;
  ADDOPERAND(Opc, MMPULite::MR3ToBIUKG, @$.S, @$.E);
}
|       mindexn ASSIGNTO r3dest {
  $$ = 0;
  ADDOPERAND(Opc, MMPULite::MR3ToBIU, @$.S, @$.E);
} ;
mr012slot: R0 DOT error { llvmerror(&@3, "Incorrect M.r0 inst."); YYABORT;} |
           R1 DOT error { llvmerror(&@3, "Incorrect M.r1 inst."); YYABORT;} |
           R2 DOT error { llvmerror(&@3, "Incorrect M.r2 inst."); YYABORT;};
r3inst: mindexn error { llvmerror(&@2, "Incorrect M.r3 inst."); YYABORT;};

shu0inst: shuexp ASSIGNTO shu0dest {
  switch ($3) {
  case 0:
    switch ($1) {
    case 2: ADDOPERAND(Opc, MMPULite::SHU0IndTNoOptToM, @$.S, @$.E); break;
    case 3: ADDOPERAND(Opc, MMPULite::SHU0IndTOptToM, @$.S, @$.E); break;
    case 4: ADDOPERAND(Opc, MMPULite::SHU0IndTBNoOptToM, @$.S, @$.E); break;
    case 5: ADDOPERAND(Opc, MMPULite::SHU0IndTBOptToM, @$.S, @$.E); break;
    case 6: ADDOPERAND(Opc, MMPULite::SHU0CombToM, @$.S, @$.E); break;
    default : break;
    }
    Operands.push_back(md);
    break;
  case 1:
    switch ($1) {
    case 2: ADDOPERAND(Opc, MMPULite::SHU0IndTNoOptToSHU, @$.S, @$.E); break;
    case 3: ADDOPERAND(Opc, MMPULite::SHU0IndTOptToSHU, @$.S, @$.E); break;
    case 4: ADDOPERAND(Opc, MMPULite::SHU0IndTBNoOptToSHU, @$.S, @$.E); break;
    case 5: ADDOPERAND(Opc, MMPULite::SHU0IndTBOptToSHU, @$.S, @$.E); break;
    case 6: ADDOPERAND(Opc, MMPULite::SHU0CombToSHU, @$.S, @$.E); break;
    default : break;
    }
    Operands.push_back(unit);
    Operands.push_back(ut);
    break;
  case 2:
    switch ($1) {
    case 2: ADDOPERAND(Opc, MMPULite::SHU0IndTNoOptToBIU, @$.S, @$.E); break;
    case 3: ADDOPERAND(Opc, MMPULite::SHU0IndTOptToBIU, @$.S, @$.E); break;
    case 4: ADDOPERAND(Opc, MMPULite::SHU0IndTBNoOptToBIU, @$.S, @$.E); break;
    case 5: ADDOPERAND(Opc, MMPULite::SHU0IndTBOptToBIU, @$.S, @$.E); break;
    case 6: ADDOPERAND(Opc, MMPULite::SHU0CombToBIU, @$.S, @$.E); break;
    default : break;
    }
    Operands.push_back(b);
    break;
  case 3:
    switch ($1) {
    case 2: ADDOPERAND(Opc, MMPULite::SHU0IndTNoOptToMACC, @$.S, @$.E); break;
    case 3: ADDOPERAND(Opc, MMPULite::SHU0IndTOptToMACC, @$.S, @$.E); break;
    case 4: ADDOPERAND(Opc, MMPULite::SHU0IndTBNoOptToMACC, @$.S, @$.E); break;
    case 5: ADDOPERAND(Opc, MMPULite::SHU0IndTBOptToMACC, @$.S, @$.E); break;
    case 6: ADDOPERAND(Opc, MMPULite::SHU0CombToMACC, @$.S, @$.E); break;
    default : break;
    }
    Operands.push_back(unit);
    Operands.push_back(ut);
    Operands.push_back(ipath);
    break;
  default: break;
  }
  Operands.push_back(NULL);
  condpos = Operands.size();
  if ($1 == 6) Operands.push_back(opc);
  Operands.push_back(tm);
  switch ($1) {
  case 2:
  case 3:
  case 6:
    Operands.push_back(tn);
    if ($1 == 6) {
      tp = OPERAND(Reg, MMPULiteReg::T0, @$.S, @$.E);
      Operands.push_back(tp);
      Operands.push_back(f);
      break;
    }
  case 4:
  case 5:
    if (imm == NULL) ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
    else Operands.push_back(imm);
    break;
  default : break;
  }

};
shu1inst: shuexp ASSIGNTO shu1dest {
  switch ($3) {
  case 0:
    switch ($1) {
    case 2: ADDOPERAND(Opc, MMPULite::SHU1IndTNoOptToM, @$.S, @$.E); break;
    case 3: ADDOPERAND(Opc, MMPULite::SHU1IndTOptToM, @$.S, @$.E); break;
    case 4: ADDOPERAND(Opc, MMPULite::SHU1IndTBNoOptToM, @$.S, @$.E); break;
    case 5: ADDOPERAND(Opc, MMPULite::SHU1IndTBOptToM, @$.S, @$.E); break;
    case 6: ADDOPERAND(Opc, MMPULite::SHU1CombToM, @$.S, @$.E); break;
    default : break;
    }
    Operands.push_back(md);
    break;
  case 1:
    switch ($1) {
    case 2: ADDOPERAND(Opc, MMPULite::SHU1IndTNoOptToSHU, @$.S, @$.E); break;
    case 3: ADDOPERAND(Opc, MMPULite::SHU1IndTOptToSHU, @$.S, @$.E); break;
    case 4: ADDOPERAND(Opc, MMPULite::SHU1IndTBNoOptToSHU, @$.S, @$.E); break;
    case 5: ADDOPERAND(Opc, MMPULite::SHU1IndTBOptToSHU, @$.S, @$.E); break;
    case 6: ADDOPERAND(Opc, MMPULite::SHU1CombToSHU, @$.S, @$.E); break;
    default : break;
    }
    Operands.push_back(unit);
    Operands.push_back(ut);
    break;
  case 2:
    switch ($1) {
    case 2: ADDOPERAND(Opc, MMPULite::SHU1IndTNoOptToBIU, @$.S, @$.E); break;
    case 3: ADDOPERAND(Opc, MMPULite::SHU1IndTOptToBIU, @$.S, @$.E); break;
    case 4: ADDOPERAND(Opc, MMPULite::SHU1IndTBNoOptToBIU, @$.S, @$.E); break;
    case 5: ADDOPERAND(Opc, MMPULite::SHU1IndTBOptToBIU, @$.S, @$.E); break;
    case 6: ADDOPERAND(Opc, MMPULite::SHU1CombToBIU, @$.S, @$.E); break;
    default : break;
    }
    Operands.push_back(b);
    break;
  case 3:
    switch ($1) {
    case 2: ADDOPERAND(Opc, MMPULite::SHU1IndTNoOptToMACC, @$.S, @$.E); break;
    case 3: ADDOPERAND(Opc, MMPULite::SHU1IndTOptToMACC, @$.S, @$.E); break;
    case 4: ADDOPERAND(Opc, MMPULite::SHU1IndTBNoOptToMACC, @$.S, @$.E); break;
    case 5: ADDOPERAND(Opc, MMPULite::SHU1IndTBOptToMACC, @$.S, @$.E); break;
    case 6: ADDOPERAND(Opc, MMPULite::SHU1CombToMACC, @$.S, @$.E); break;
    default : break;
    }
    Operands.push_back(unit);
    Operands.push_back(ut);
    Operands.push_back(ipath);
    break;
  default: break;
  }
  Operands.push_back(NULL);
  condpos = Operands.size();
  if ($1 == 6) Operands.push_back(opc);
  Operands.push_back(tm);
  switch ($1) {
  case 2:
  case 3:
  case 6:
    Operands.push_back(tn);
    if ($1 == 6) {
      tp = OPERAND(Reg, MMPULiteReg::T0, @$.S, @$.E);
      Operands.push_back(tp);
      Operands.push_back(f);
      break;
    }
  case 4:
  case 5:
    if (imm == NULL) ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
    else Operands.push_back(imm);
    break;
  default : break;
  }
};
shuexp: indexp | byexp {
  $$ = $1;
  if (flags[LF])
    flagsort = flags[TCF] << 3;
  else if (flags[BF] || flags[HF])
    flagsort = (flags[TCF] << 3) | flags[BF] | (flags[HF] << 1);
  else
    flagsort = (1 << 2) | (flags[TCF] << 3);
  f = OPERAND(Imm, (flags.to_ulong() << 8) | flagsort, FlagS, FlagE);
  flags.reset();
};
shuexp: error { llvmerror(&@1, "Incorrect SHU inst. Legal insts are IND and COMB"); YYABORT;};
indexp: shusrct IND indclause {$$ = $3;};
indclause: indtnclause | indtbclause ;
indtnclause: shusrct _flag TB ACC2 IMM3 flag_ {$$ = 3; imm = OPERAND(Imm, $5, @5.S, @5.E);}
|            shusrct {$$ = 2;};
indtnclause: shusrct _flag error flag_ { llvmerror(&@2, "IND Tn must be followed by (TB =+ Imm)"); YYABORT;};
indtbclause: TB _flag TB ACC1 IMM3 flag_ {$$ = 5; imm = OPERAND(Imm, $5, @5.S, @5.E);}
|            TB {$$ = 4;};
indtnclause: TB _flag error flag_ { llvmerror(&@2, "IND TB must be followed by (TB += Imm)"); YYABORT;};
byclause: BY _flag shudupara flag_ {$$ = 6; opc = OPERAND(Reg, MMPULiteReg::COMB, @1.S, @1.E);};
shudupara: shusrct COMMA shusrct;
byexp: byclause _flag byflag flag_ | byclause;
shu0ctrl: SHU0 DOT ctrl {
  if ($3) ADDOPERAND(Opc, MMPULite::SHU0Stop, @$.S, @$.E);
  else ADDOPERAND(Opc, MMPULite::SHU0Start, @$.S, @$.E);
};
shu1ctrl: SHU1 DOT ctrl {
  if ($3) ADDOPERAND(Opc, MMPULite::SHU1Stop, @$.S, @$.E);
  else ADDOPERAND(Opc, MMPULite::SHU1Start, @$.S, @$.E);
};

ldinst: ldop lddest {
  unsigned distance = MMPULite::BIU1LdToM - MMPULite::BIU0LdToM;
  if ($2 == 0) {
    ADDOPERAND(Opc, MMPULite::BIU0LdToM + distance * slotid, @$.S, @$.E);
    Operands.push_back(md);
  } else {
    if ($2 == 1)
      ADDOPERAND(Opc, MMPULite::BIU0LdToSHU + distance * slotid, @$.S, @$.E);
    else if ($2 == 3)
      ADDOPERAND(Opc, MMPULite::BIU0LdToMACC + distance * slotid, @$.S, @$.E);
    Operands.push_back(unit);
    Operands.push_back(ut);
    if ($2 == 3) Operands.push_back(ipath);
  }
  Operands.push_back(NULL);
  condpos = Operands.size();
};
ldop: DM _flag ldflag flag_ ASSIGNTO | DM ASSIGNTO;
stinst: storeop {
  unsigned distance = MMPULite::BIU1St - MMPULite::BIU0St;
  ADDOPERAND(Opc, MMPULite::BIU0St + distance * slotid, @$.S, @$.E);
  Operands.push_back(NULL);
  condpos = Operands.size();
};
storeop: ASSIGNTO DM | ASSIGNTO DM _flag stflag flag_;
kginst: _flag KG flag_ ASSIGNTO mindex {
  unsigned distance = MMPULite::BIU1KG - MMPULite::BIU0KG;
  ADDOPERAND(Opc, MMPULite::BIU0KG + distance * slotid, @$.S, @$.E);
  Operands.push_back(md);
  Operands.push_back(NULL);
  condpos = Operands.size();
};

/*Operands format types: 0:none,1:unary,2:bin,3:tpara,4:dpara,5:imm*/
ialuinst: ialuclause ASSIGNTO ialudest {
  flagsort = (flags[TF] << 3) | (flags[UF] << 2) | (flags[BF]) << 1 | (flags[HF]);
  f = OPERAND(Imm, (flags.to_ulong() << 8) | flagsort, FlagS, FlagE);
  flags.reset();
  switch($3) {
  case 0:
    switch($1) {
    case 0: ADDOPERAND(Opc, MMPULite::IALUDivRToM, @$.S, @$.E); break;
    case 1: ADDOPERAND(Opc, MMPULite::IALUUryToM, @$.S, @$.E); break;
    case 2: ADDOPERAND(Opc, MMPULite::IALUBinToM, @$.S, @$.E); break;
    case 3: ADDOPERAND(Opc, MMPULite::IALUTPToM, @$.S, @$.E); break;
    case 4: ADDOPERAND(Opc, MMPULite::IALUDPToM, @$.S, @$.E); break;
    case 5: ADDOPERAND(Opc, MMPULite::IALUImmToM, @$.S, @$.E); break;
    default: break;
    }
    Operands.push_back(md);
    break;
  case 1:
    switch($1) {
    case 0: ADDOPERAND(Opc, MMPULite::IALUDivRToSHU, @$.S, @$.E); break;
    case 1: ADDOPERAND(Opc, MMPULite::IALUUryToSHU, @$.S, @$.E); break;
    case 2: ADDOPERAND(Opc, MMPULite::IALUBinToSHU, @$.S, @$.E); break;
    case 3: ADDOPERAND(Opc, MMPULite::IALUTPToSHU, @$.S, @$.E); break;
    case 4: ADDOPERAND(Opc, MMPULite::IALUDPToSHU, @$.S, @$.E); break;
    case 5: ADDOPERAND(Opc, MMPULite::IALUImmToSHU, @$.S, @$.E); break;
    default: break;
    }
    Operands.push_back(unit);
    Operands.push_back(ut);
    break;
  case 2:
    switch($1) {
    case 0: ADDOPERAND(Opc, MMPULite::IALUDivRToBIU, @$.S, @$.E); break;
    case 1: ADDOPERAND(Opc, MMPULite::IALUUryToBIU, @$.S, @$.E); break;
    case 2: ADDOPERAND(Opc, MMPULite::IALUBinToBIU, @$.S, @$.E); break;
    case 3: ADDOPERAND(Opc, MMPULite::IALUTPToBIU, @$.S, @$.E); break;
    case 4: ADDOPERAND(Opc, MMPULite::IALUDPToBIU, @$.S, @$.E); break;
    case 5: ADDOPERAND(Opc, MMPULite::IALUImmToBIU, @$.S, @$.E); break;
    default: break;
    }
    Operands.push_back(b);
    break;
  case 3:
    switch($1) {
    case 0: ADDOPERAND(Opc, MMPULite::IALUDivRToMACC, @$.S, @$.E); break;
    case 1: ADDOPERAND(Opc, MMPULite::IALUUryToMACC, @$.S, @$.E); break;
    case 2: ADDOPERAND(Opc, MMPULite::IALUBinToMACC, @$.S, @$.E); break;
    case 3: ADDOPERAND(Opc, MMPULite::IALUTPToMACC, @$.S, @$.E); break;
    case 4: ADDOPERAND(Opc, MMPULite::IALUDPToMACC, @$.S, @$.E); break;
    case 5: ADDOPERAND(Opc, MMPULite::IALUImmToMACC, @$.S, @$.E); break;
    default: break;
    }
    Operands.push_back(unit);
    Operands.push_back(ut);
    ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
    break;
  default: break;
  }
  Operands.push_back(NULL);
  condpos = Operands.size();
  if ($1 > 0) {
    Operands.push_back(opc);
    Operands.push_back(tm);
    if ($1 == 5) Operands.push_back(imm);
    else if ($1 != 0) {
      if ($1 == 1) tn = OPERAND(Reg, MMPULiteReg::T0, @$.S, @$.E);
      Operands.push_back(tn);
      if ($1 != 5 && $1 != 3) tp = OPERAND(Reg, MMPULiteReg::T0, @$.S, @$.E);
      if ($1 != 5) Operands.push_back(tp);
    }
  }
  Operands.push_back(f);
}
|         condclause ASSIGNTO ialudest{
  flagsort = (flags[UF] << 2) | (flags[BF]) << 1 | (flags[HF]);
  f = OPERAND(Imm, (flags.to_ulong() << 8) | flagsort, FlagS, FlagE);
  flags.reset();
  switch($3) {
  case 0:
    ADDOPERAND(Opc, MMPULite::IALUBinToM, @$.S, @$.E);
    Operands.push_back(md);
    break;
  case 1:
    ADDOPERAND(Opc, MMPULite::IALUBinToSHU, @$.S, @$.E);
    Operands.push_back(unit);
    Operands.push_back(ut);
    break;
  case 2:
    ADDOPERAND(Opc, MMPULite::IALUBinToBIU, @$.S, @$.E);
    Operands.push_back(b);
    break;
  case 3:
    ADDOPERAND(Opc, MMPULite::IALUBinToMACC, @$.S, @$.E);
    Operands.push_back(unit);
    Operands.push_back(ut);
    ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
    break;
  default: break;
  }
  Operands.push_back(NULL);
  condpos = Operands.size();
  Operands.push_back(opc);
  Operands.push_back(tm);
  Operands.push_back(tn);
  tp = OPERAND(Reg, MMPULiteReg::T0, @$.S, @$.E);
  Operands.push_back(tp);
  Operands.push_back(f);
}
|         condclause ASSIGNTO COND{
  flagsort = (flags[UF] << 2) | (flags[BF]) << 1 | (flags[HF]);
  f = OPERAND(Imm, (flags.to_ulong() << 8) | flagsort, FlagS, FlagE);
  flags.reset();
  ADDOPERAND(Opc, MMPULite::IALUToCond, @$.S, @$.E);
  Operands.push_back(NULL);
  condpos = Operands.size();
  Operands.push_back(opc);
  Operands.push_back(tm);
  Operands.push_back(tn);
  Operands.push_back(f);
};
ialuclause: utbclause | ubclause | bclause | b_uclause | inoflagclause | uthclause;
utbclause: iaddclause {$$ = 2; opc = OPERAND(Reg, MMPULiteReg::IADD, @$.S, @$.E);}|
           isubclause {$$ = 2; opc = OPERAND(Reg, MMPULiteReg::ISUB, @$.S, @$.E);};
iaddclause: addexp _flag utbflag flag_ | addexp;
iaddclause: addexp _flag error flag_ {llvmerror(&@3, "Invalid flag for \"Tm + Tn\". Available flags for IALU are U, T, B/H, and for FALU are T, S/D."); YYABORT;};
isubclause: subexp _flag utbflag flag_ | subexp ;
isubclause: subexp _flag error flag_ {llvmerror(&@3, "Invalid flag for \"Tm - Tn\". Available flags for IALU are U, T, B/H, and for FALU are T, S/D."); YYABORT;};
addexp: t ADD t ;
subexp: t SUB t ;
ubclause: imaxclause {$$ = 4; opc = OPERAND(Reg, MMPULiteReg::IMAX, @$.S, @$.E);}|
          iminclause {$$ = 4; opc = OPERAND(Reg, MMPULiteReg::IMIN, @$.S, @$.E);}|
          iabsclause {$$ = 4; opc = OPERAND(Reg, MMPULiteReg::IABS, @$.S, @$.E);}|
          iexpdclause {$$ = 1; opc = OPERAND(Reg, MMPULiteReg::EXPD, @$.S, @$.E);}|
          rshtclause {$$ = $1;};
condclause: iltclause {$$ = 2; opc = OPERAND(Reg, MMPULiteReg::ILT, @$.S, @$.E);}|
            instclause {$$ = 2; opc = OPERAND(Reg, MMPULiteReg::INST, @$.S, @$.E);}|
            istclause {$$ = 2; opc = OPERAND(Reg, MMPULiteReg::IST, @$.S, @$.E);}|
            inltclause {$$ = 2; opc = OPERAND(Reg, MMPULiteReg::INLT, @$.S, @$.E);};
imaxclause: maxexp _flag ubflag flag_ | maxexp ;
imaxclause: maxexp _flag error flag_ {llvmerror(&@3, "Invalid flag for MAX. Available flags for IALU are U, B/H, and for FALU are S/D."); YYABORT;};
maxexp: MAX _flag dupara  flag_;
iminclause: minexp _flag ubflag flag_ | minexp ;
iminclause: minexp _flag error flag_ {llvmerror(&@3, "Invalid flag for MIN. Available flags for IALU are U, B/H, and for FALU are S/D."); YYABORT;};
minexp: MIN _flag dupara  flag_ ;
iabsclause: abssexp _flag ubflag flag_ | abssexp ;
iabsclause: abssexp _flag error flag_ {llvmerror(&@3, "Invalid flag for ABS(Tm - Tn). Available flags are U, B/H."); YYABORT;};
abssexp: ABS _flag dupara  flag_ ;
iexpdclause: expdexp _flag ubflag flag_ | expdexp ;
iexpdclause: expdexp _flag error flag_ {llvmerror(&@3, "Invalid flag for EXPD. Available flags are U, B/H."); YYABORT;};
expdexp: EXPD t;
iltclause: ltexp _flag ubflag flag_ | ltexp ;
iltclause: ltexp _flag error flag_ {llvmerror(&@3, "Invalid flag for \"Tm > Tn\". Available flags for IALU are U, B/H, and for FALU are S/D."); YYABORT;};
ltexp: t LT t ;
instclause: nstexp _flag ubflag flag_ | nstexp ;
instclause: nstexp _flag error flag_ {llvmerror(&@3, "Invalid flag for \"Tm >= Tn\". Available flags for IALU are U, B/H, and for FALU are S/D."); YYABORT;};
nstexp: t NST t ;
istclause: stexp _flag ubflag flag_ | stexp ;
istclause: stexp _flag error flag_ {llvmerror(&@3, "Invalid flag for \"Tm < Tn\". Available flags for IALU are U, B/H, and for FALU are S/D."); YYABORT;};
stexp: t ST t ;
inltclause: nltexp _flag ubflag flag_ | nltexp;
inltclause: nltexp _flag error flag_ {llvmerror(&@$, "Invalid flag for \"Tm <= Tn\". Available flags for IALU are U, B/H, and for FALU are S/D."); YYABORT;};
nltexp: t NLT t ;
bclause: mrgclause {$$ = 3; opc = OPERAND(Reg, MMPULiteReg::MERGE, @$.S, @$.E);}|
         lshtclause;
condclause: iequclause {$$ = 2; opc = OPERAND(Reg, MMPULiteReg::IEQU, @$.S, @$.E);}|
            ineqclause {$$ = 2; opc = OPERAND(Reg, MMPULiteReg::INEQ, @$.S, @$.E);};
mrgclause: mrgexp _flag bflag flag_ | mrgexp;
mrgclause: mrgexp _flag error flag_ {llvmerror(&@3, "Invalid flag for MERGE. Available flags are B/H."); YYABORT;};
mrgexp: MERGE  _flag tripara flag_ ;
iequclause: equexp _flag bflag flag_ | equexp;
iequclause: equexp _flag error flag_ {llvmerror(&@3, "Invalid flag for \"Tm == Tn\". Available flags are B/H."); YYABORT;};
equexp: t EQU t ;
ineqclause: neqexp _flag bflag flag_ | neqexp;
ineqclause: neqexp _flag error flag_ {llvmerror(&@3, "Invalid flag for \"Tm != Tn\". Available flags are B/H."); YYABORT;};
neqexp: t NEQ t ;
rshtclause: rshtexp _flag ubflag flag_ | rshtexp;
rshtclause: rshtexp _flag error flag_ {llvmerror(&@3, "Invalid flag for \"Tm >> Tn/Imm\". Available flag are U, B/H."); YYABORT;};
rshtexp: t rshtt {$$ = 2; opc = OPERAND(Reg, MMPULiteReg::RSHT, @2.S, @2.E);}|
         t rshti {$$ = 5; opc = OPERAND(Reg, MMPULiteReg::RSHT, @2.S, @2.E);};
rshtt: RSHT t ;
rshti: RSHT imm5 {imm = OPERAND(Imm, $2, @2.S, @2.E);};
inoflagclause: inoflagexp ;
inoflagexp: divrop | uaryexp | binaryexp;
inoflagexp: inoflagexp _flag error flag_ {llvmerror(&@3, "None of flags is supported by bitwise insts and DIVR inst."); YYABORT;};
divrop: DIVR {$$ = 0;};
uaryexp: NOT t {$$ = 1; opc = OPERAND(Reg, MMPULiteReg::NOT, @1.S, @1.E);};
binaryexp: t binaryop t {$$ = 2;};
binaryop: AND {opc = OPERAND(Reg, MMPULiteReg::AND, @1.S, @1.E);}|
          OR {opc = OPERAND(Reg, MMPULiteReg::OR, @1.S, @1.E);}|
          XOR {opc = OPERAND(Reg, MMPULiteReg::XOR, @1.S, @1.E);};
lshtclause: lshtexp _flag bflag flag_ | lshtexp;
lshtexp: t LSHT t {$$ = 2; opc = OPERAND(Reg, MMPULiteReg::LSHT, @1.S, @1.E);}|
         t LSHT imm5 {$$ = 5; opc = OPERAND(Reg, MMPULiteReg::LSHT, @1.S, @1.E); imm = OPERAND(Imm, $3, @2.S, @2.E);};
uthclause: cprsexp _flag uthflag flag_ | cprsexp;
cprsexp: CPRS _flag dupara flag_ {$$ = 4; opc = OPERAND(Reg, MMPULiteReg::CPRS, @$.S, @$.E);};
uthclause: cprsexp _flag error flag_ {llvmerror(&@3, "Invalid flag for CPRS. Available flags are H, U and T."); YYABORT;};
b_uclause: reduceexp _flag b__uflag flag_ {$$ = 1;};
reduceexp: RMAX t {opc = OPERAND(Reg, MMPULiteReg::RMAX, @1.S, @1.E);}|
           RMIN t {opc = OPERAND(Reg, MMPULiteReg::RMIN, @1.S, @1.E);};

divinst: divexp ASSIGNTO DIVR {
  flags.reset();
  ADDOPERAND(Opc, MMPULite::IALUDivR, @$.S, @$.E);
  Operands.push_back(NULL);
  condpos = Operands.size();
  Operands.push_back(opc);
};
divinst: divsexp ASSIGNTO DIVR {
  ADDOPERAND(Opc, MMPULite::IALUDivS, @$.S, @$.E);
  Operands.push_back(NULL);
  condpos = Operands.size();
  ADDOPERAND(Reg, MMPULiteReg::DIVS, @$.S, @$.E);
  Operands.push_back(tm);
  Operands.push_back(tn);
  ADDOPERAND(Imm, (flags.to_ulong() << 8) | (flags[UF] << 2), FlagS, FlagE);
};
divexp: divqop | mdivrop | mdivqop;
divsexp: t DIVS t _flag uflag flag_ | t DIVS t ;
divqop: DIVQ { opc = OPERAND(Reg, MMPULiteReg::DIVQ, @$.S, @$.E); };
mdivrop: MDIVR { opc = OPERAND(Reg, MMPULiteReg::MDIVR, @$.S, @$.E); };
mdivqop: MDIVQ { opc = OPERAND(Reg, MMPULiteReg::MDIVQ, @$.S, @$.E); };

ialuctrl: ialu DOT ctrl {
  if ($3) ADDOPERAND(Opc, MMPULite::IALUStop, @$.S, @$.E);
  else ADDOPERAND(Opc, MMPULite::IALUStart, @$.S, @$.E);
};

imacinst: imacclause ASSIGNTO imacdest {
  flagsort = (flags[CRF] << 5) | (flags[LF] << 4) |
             (flags[IF] << 3) | (flags[UF] << 2) |
             (flags[BF] << 1) | flags[HF];
  f = OPERAND(Imm, (flags.to_ulong() << 8) | flagsort, FlagS, FlagE);
  switch($3) {
  case 0:
    switch($1) {
    case 0: ADDOPERAND(Opc, MMPULite::IMRToM, @$.S, @$.E); break;
    case 2: ADDOPERAND(Opc, MMPULite::IMulToM, @$.S, @$.E); break;
    case 3: ADDOPERAND(Opc, MMPULite::IMASToM, @$.S, @$.E); break;
    case 4: ADDOPERAND(Opc, MMPULite::IMAccToM, @$.S, @$.E); break;
    case 5: ADDOPERAND(Opc, MMPULite::IMaCToM, @$.S, @$.E); break;
    case 6: ADDOPERAND(Opc, MMPULite::IMAToM, @$.S, @$.E); break;
    default: break;
    }
    Operands.push_back(md);
    break;
  case 1:
    switch($1) {
    case 0: ADDOPERAND(Opc, MMPULite::IMRToSHU, @$.S, @$.E); break;
    case 2: ADDOPERAND(Opc, MMPULite::IMulToSHU, @$.S, @$.E); break;
    case 3: ADDOPERAND(Opc, MMPULite::IMASToSHU, @$.S, @$.E); break;
    case 4: ADDOPERAND(Opc, MMPULite::IMAccToSHU, @$.S, @$.E); break;
    case 5: ADDOPERAND(Opc, MMPULite::IMaCToSHU, @$.S, @$.E); break;
    case 6: ADDOPERAND(Opc, MMPULite::IMAToSHU, @$.S, @$.E); break;
    default: break;
    }
    Operands.push_back(unit);
    Operands.push_back(ut);
    break;
  case 2:
    switch($1) {
    case 0: ADDOPERAND(Opc, MMPULite::IMRToBIU, @$.S, @$.E); break;
    case 2: ADDOPERAND(Opc, MMPULite::IMulToBIU, @$.S, @$.E); break;
    case 3: ADDOPERAND(Opc, MMPULite::IMASToBIU, @$.S, @$.E); break;
    case 4: ADDOPERAND(Opc, MMPULite::IMAccToBIU, @$.S, @$.E); break;
    case 5: ADDOPERAND(Opc, MMPULite::IMaCToBIU, @$.S, @$.E); break;
    case 6: ADDOPERAND(Opc, MMPULite::IMAToBIU, @$.S, @$.E); break;
    default: break;
    }
    Operands.push_back(b);
    break;
  case 3:
    switch($1) {
    case 0: ADDOPERAND(Opc, MMPULite::IMRToMACC, @$.S, @$.E); break;
    case 2: ADDOPERAND(Opc, MMPULite::IMulToMACC, @$.S, @$.E); break;
    case 3: ADDOPERAND(Opc, MMPULite::IMASToMACC, @$.S, @$.E); break;
    case 4: ADDOPERAND(Opc, MMPULite::IMAccToMACC, @$.S, @$.E); break;
    case 5: ADDOPERAND(Opc, MMPULite::IMaCToMACC, @$.S, @$.E); break;
    case 6: ADDOPERAND(Opc, MMPULite::IMAToMACC, @$.S, @$.E); break;
    default: break;
    }
    Operands.push_back(unit);
    Operands.push_back(ut);
    ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
    break;
  default: break;
  }
  Operands.push_back(NULL);
  condpos = Operands.size();
  if ($1 != 0) {
    if ($1 <= 3) Operands.push_back(opc);
    Operands.push_back(tm);
    if ($1 != 4) {
      Operands.push_back(tn);
      if ($1 == 2 || $1 == 3 || $1 == 6) {
        if ($1 == 2) tp = OPERAND(Reg, MMPULiteReg::T0, @$.S, @$.E);
        Operands.push_back(tp);
      }
    }
  }
  Operands.push_back(f);
  flags.reset();
};
imacclause: imulclause {$$ = 2;} | muladsbexp {$$ = 3;} | mrclause {$$ = 0;} | mrinst {$$ = $1 + 3;};
mulexp: t MUL t;
imulclause: mulexp _flag t_uibflag flag_ {opc = OPERAND(Reg, MMPULiteReg::IMULT, @1.S, @1.E);}
|           mulexp _flag b_uiflag flag_ {opc = OPERAND(Reg, MMPULiteReg::IMUL, @1.S, @1.E);};
//|           mulexp _flag uiflag flag_ {opc = OPERAND(Reg, MMPULiteReg::IMUL, @1.S, @1.E);};
imulclause: mulexp _flag uiflag flag_ {llvmerror(&@3, "Either T flag or B/H flags are necessary for IMAC \"Tm * Tn\"."); YYABORT;};
imulclause: mulexp {llvmerror(&@1, "Either T flag or B/H flags are necessary for IMAC \"Tm * Tn\"."); YYABORT;};
imulclause: mulexp _flag error flag_ {llvmerror(&@3, "Invalid flag for \"Tm * Tn\". Available flags for IMAC are T, U, I, B/H, and for FMAC are T, S/D."); YYABORT;};
muladsbexp: t ADDSUB mulexp _flag hflag flag_ {MMPULite::MMPULiteAsmOperand *tmp = tp; tp = tm; tm = tn; tn = tmp; opc = OPERAND(Reg, MMPULiteReg::IMULS, @$.S, @$.E);};
muladsbexp: t ADDSUB mulexp {llvmerror(&@3, "H flag is necessary for integer complex multipication."); YYABORT;}
muladsbexp: t ADDSUB mulexp _flag error flag_ {llvmerror(&@5, "Invalid flag for \"Tp +- Tm * Tn\". Available flag is H."); YYABORT;}
mrclause: MR _flag lubflag flag_ | MR;

mrinst: exmacexp iexmacclause | exmacexp | inmacexp iinmacclause | inmacexp | accexp tregclause | accexp ;
exmacexp: MR ASSIGN t ADD mulexp {$$ = 3; MMPULite::MMPULiteAsmOperand *tmp = tp; tp = tm; tm = tn; tn = tmp;};
inmacexp: MR ACC1 mulexp {$$ = 2;};
accexp: MR ACC1 t {$$ = 1;};
iexmacclause: _flag ubflag flag_ ;
iinmacclause: _flag crlubflag flag_ ;
iinmacclause: _flag error flag_ {llvmerror(&@2, "Invalid flag for \"MR += Tm * Tn\". Available flags for IMAC are CR, L, U, B/H, and for FMAC are CR, S/D."); YYABORT;};
tregclause: _flag crlubflag flag_;
tregclause: _flag error flag_ {llvmerror(&@2, "Invalid flag for \"MR += Tm\". Available flags  are CR, U, L, B/H"); YYABORT;};
imacctrl: imac DOT ctrl {
  if ($3) ADDOPERAND(Opc, MMPULite::IMACStop, @$.S, @$.E);
  else ADDOPERAND(Opc, MMPULite::IMACStart, @$.S, @$.E);
};

faluinst: faluclause ASSIGNTO faludest {
  flagsort = (flags[TF] << 3) | (flags[UF] << 2) | (flags[SF]) << 1 | (flags[DF]);
  f = OPERAND(Imm, (flags.to_ulong() << 8) | flagsort, FlagS, FlagE);
  flags.reset();
  switch($3) {
  case 0:
    switch($1) {
    case 1: ADDOPERAND(Opc, MMPULite::FALUUryToM, @$.S, @$.E); break;
    case 2: ADDOPERAND(Opc, MMPULite::FALUBinToM, @$.S, @$.E); break;
    case 4: ADDOPERAND(Opc, MMPULite::FALUDParaToM, @$.S, @$.E); break;
    default: break;
    }
    Operands.push_back(md); 
    break;
  case 1:
    switch($1) {
    case 1: ADDOPERAND(Opc, MMPULite::FALUUryToSHU, @$.S, @$.E); break;
    case 2: ADDOPERAND(Opc, MMPULite::FALUBinToSHU, @$.S, @$.E); break;
    case 4: ADDOPERAND(Opc, MMPULite::FALUDParaToSHU, @$.S, @$.E); break;
    default: break;
    }
    Operands.push_back(unit); 
    Operands.push_back(ut); 
    break;
  case 2:
    switch($1) {
    case 1: ADDOPERAND(Opc, MMPULite::FALUUryToBIU, @$.S, @$.E); break;
    case 2: ADDOPERAND(Opc, MMPULite::FALUBinToBIU, @$.S, @$.E); break;
    case 4: ADDOPERAND(Opc, MMPULite::FALUDParaToBIU, @$.S, @$.E); break;
    default: break;
    }
    Operands.push_back(b); 
    break;
  case 3:
    switch($1) {
    case 1: ADDOPERAND(Opc, MMPULite::FALUUryToMACC, @$.S, @$.E); break;
    case 2: ADDOPERAND(Opc, MMPULite::FALUBinToMACC, @$.S, @$.E); break;
    case 4: ADDOPERAND(Opc, MMPULite::FALUDParaToMACC, @$.S, @$.E); break;
    default: break;
    }
    Operands.push_back(unit); 
    Operands.push_back(ut); 
    ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
    break;
  default: break;
  }
  Operands.push_back(NULL);
  condpos = Operands.size();
  Operands.push_back(opc);
  Operands.push_back(tm);
  if ($1 == 1) tn = OPERAND(Reg, MMPULiteReg::T0, @$.S, @$.E);
  Operands.push_back(tn);
  tp = OPERAND(Reg, MMPULiteReg::T0, @$.S, @$.E);
  Operands.push_back(tp);
  Operands.push_back(f);
};

faluclause: tsdclause | sdclause | tclause | tdclause | sclause | fuclause ;
tsdclause: faddclause {$$ = 2; opc = OPERAND(Reg, MMPULiteReg::FADD, @$.S, @$.E);}|
           fsubclause {$$ = 2; opc = OPERAND(Reg, MMPULiteReg::FSUB, @$.S, @$.E);};
faddclause: addexp _flag tsdflag flag_;
fsubclause: subexp _flag tsdflag flag_;
sdclause: fmaxclause {$$ = 4; opc = OPERAND(Reg, MMPULiteReg::FMAX, @$.S, @$.E);} |
          fminclause {$$ = 4; opc = OPERAND(Reg, MMPULiteReg::FMIN, @$.S, @$.E);};
sdclause: recipclause | rsqrtclause | intclause | fabsclause ;
sdclause: fequclause {$$ = 2; opc = OPERAND(Reg, MMPULiteReg::FEQU, @$.S, @$.E);}|
          fneqclause {$$ = 2; opc = OPERAND(Reg, MMPULiteReg::FNEQ, @$.S, @$.E);}|
          fltclause {$$ = 2; opc = OPERAND(Reg, MMPULiteReg::FLT, @$.S, @$.E);}|
          fnstclause {$$ = 2; opc = OPERAND(Reg, MMPULiteReg::FNST, @$.S, @$.E);}|
          fstclause {$$ = 2; opc = OPERAND(Reg, MMPULiteReg::FST, @$.S, @$.E);}|
          fnltclause {$$ = 2; opc = OPERAND(Reg, MMPULiteReg::FNLT, @$.S, @$.E);};
fmaxclause: maxexp _flag sdflag flag_ ;
fminclause: minexp _flag sdflag flag_ ;
fabsclause: absexp _flag sdflag flag_ ;
fabsclause: absexp _flag error flag_ {llvmerror(&@3, "Invalid flag for ABS. Available flags are S/D."); YYABORT;};
absexp: ABS t {$$ = 1; opc = OPERAND(Reg, MMPULiteReg::FABS, @1.S, @1.E);};
recipclause: recipexp _flag sdflag flag_ ;
recipclause: recipexp _flag error flag_ {llvmerror(&@3, "Invalid flag for RECIP. Available flags are S/D."); YYABORT;};
recipexp: RECIP t {$$ = 1; opc = OPERAND(Reg, MMPULiteReg::RECIP, @1.S, @1.E);};
rsqrtclause: rsqrtexp _flag sdflag flag_ ;
rsqrtclause: rsqrtexp _flag error flag_ {llvmerror(&@3, "Invalid flag for RSQRT. Available flags are S/D."); YYABORT;};
rsqrtexp: RSQRT t {$$ = 1; opc = OPERAND(Reg, MMPULiteReg::RSQRT, @1.S, @1.E);};
fequclause: equexp _flag sdflag flag_ ;
fneqclause: neqexp _flag sdflag flag_ ;
fltclause: ltexp _flag sdflag flag_ ;
fnstclause: nstexp _flag sdflag flag_ ;
fstclause: stexp _flag sdflag flag_ ;
fnltclause: nltexp _flag sdflag flag_ ;
intclause: intexp _flag tsdflag flag_ ;
intclause: intexp _flag error flag_ {llvmerror(&@3, "Invalid flag for UINT/INT. Available flags are T, S/D."); YYABORT;};
intexp: intop t {$$ = 1; opc = OPERAND(Reg, MMPULiteReg::INT, @1.S, @1.E);}|
        uintop t {$$ = 1; opc = OPERAND(Reg, MMPULiteReg::UINT, @1.S, @1.E);};
intop: INT ;
uintop: UINT ;
tclause: tosexp _flag utflag flag_ | tosexp ;
tdclause: tosexp _flag tdflag flag_ ;
tclause: tosexp _flag error flag_ {llvmerror(&@3, "Invalid flag for SINGLE. Available flags are T, U, D. D and U are mutually exclusive."); YYABORT;};
tosexp: SINGLE t {$$ = 1; opc = OPERAND(Reg, MMPULiteReg::SINGLE, @1.S, @1.E);};
sclause: todexp _flag sflag flag_ |
         fasexp _flag sflag flag_;
fasexp: t ADDSUB t {$$ = 2; opc = OPERAND(Reg, MMPULiteReg::ADDSUB, @2.S, @2.E);};
todexp: DOUBLE t {$$ = 1; opc = OPERAND(Reg, MMPULiteReg::DOUBLE, @1.S, @1.E);};
fuclause: todexp _flag uflag flag_ | todexp;
sclause: todexp _flag error flag_ {llvmerror(&@3, "Invalid flag for DOUBLE. Available flags are U, S. S and U are mutually exclusive."); YYABORT;};
sclause: fasexp _flag error flag_ {llvmerror(&@3, "Invalid flag for \"Tm +- Tn\". S flag is necessary."); YYABORT;};
faluctrl: falu DOT ctrl {
  if ($3) ADDOPERAND(Opc, MMPULite::FALUStop, @$.S, @$.E);
  else ADDOPERAND(Opc, MMPULite::FALUStart, @$.S, @$.E);
};

fmacinst: fmacclause ASSIGNTO fmacdest {
  flagsort = ((flags[TF] | flags[CRF]) << 1) | flags[DF];
  f = OPERAND(Imm, (flags.to_ulong() << 8) | flagsort, FlagS, FlagE);
  flags.reset();
  switch($3) {
  case 0:
    switch($1) {
    case 3: ADDOPERAND(Opc, MMPULite::FMaCToM, @$.S, @$.E); break;
    case 2: ADDOPERAND(Opc, MMPULite::FMulToM, @$.S, @$.E); break;
    //case 0: ADDOPERAND(Opc, MMPULite::FMRToM, @$.S, @$.E); break;
    default: break;
    }
    Operands.push_back(md);
    break;
  case 1:
    switch($1) {
    case 3: ADDOPERAND(Opc, MMPULite::FMaCToSHU, @$.S, @$.E); break;
    case 2: ADDOPERAND(Opc, MMPULite::FMulToSHU, @$.S, @$.E); break;
    //case 0: ADDOPERAND(Opc, MMPULite::FMRToSHU, @$.S, @$.E); break;
    default: break;
    }
    Operands.push_back(unit);
    Operands.push_back(ut);
    break;
  case 2:
    switch($1) {
    case 3: ADDOPERAND(Opc, MMPULite::FMaCToBIU, @$.S, @$.E); break;
    case 2: ADDOPERAND(Opc, MMPULite::FMulToBIU, @$.S, @$.E); break;
    //case 0: ADDOPERAND(Opc, MMPULite::FMRToBIU, @$.S, @$.E); break;
    default: break;
    }
    Operands.push_back(b);
    break;
  case 3:
    switch($1) {
    case 3: ADDOPERAND(Opc, MMPULite::FMaCToMACC, @$.S, @$.E); break;
    case 2: ADDOPERAND(Opc, MMPULite::FMulToMACC, @$.S, @$.E); break;
    //case 0: ADDOPERAND(Opc, MMPULite::FMRToMACC, @$.S, @$.E); break;
    default: break;
    }
    Operands.push_back(unit);
    Operands.push_back(ut);
    ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
    break;
  default: break;
  }
  Operands.push_back(NULL);
  condpos = Operands.size();
  if ($1 != 0) {
    if ($1 == 2) Operands.push_back(opc);
    Operands.push_back(tm);
    Operands.push_back(tn);
    if ($1 == 2) {
      tp = OPERAND(Reg, MMPULiteReg::T0, @$.S, @$.E);
      Operands.push_back(tp);
    }
  }
  Operands.push_back(f);
};
fmacclause: fmulclause | fmrclause | fmrinst {$$ = 3;};
fmulclause: mulexp _flag tsdflag flag_ {$$ = 2; opc = OPERAND(Reg, MMPULiteReg::FMUL, @1.S, @1.E);};
fmulclause: cmulexp _flag sflag flag_ {$$ = 2;};
cmulexp: t CMUL t {opc = OPERAND(Reg, MMPULiteReg::CMUL, @2.S, @2.E);};
fmrclause: MR _flag sdflag flag_ {$$ = 0;};
fmrinst: exmacexp fexmacclause | inmacexp finmacclause;
fexmacclause: _flag sdflag flag_ ;
finmacclause: _flag crsdflag flag_ ;
fmacctrl: fmac DOT ctrl {
  if ($3) ADDOPERAND(Opc, MMPULite::FMACStop, @$.S, @$.E);
  else ADDOPERAND(Opc, MMPULite::FMACStart, @$.S, @$.E);
};

reinst: REPEAT ALPHA _flag repeatexp {Operands.push_back(imm);};
reinst: REPEAT ALPHA _flag error {llvmerror(&@4, "Invalid condition expression."); YYABORT;}
repeatexp: immrep {ADDOPERAND(Opc, MMPULite::REPEATIMM, @-2.S, @$.E);} |
           regrep flag_ {
  ADDOPERAND(Opc, MMPULite::REPEATK, @-2.S, @$.E);
  Operands.push_back(imm);
  imm = OPERAND(Imm, 0, SMLoc(), SMLoc());
}
|          regrep SUB imm flag_ {
  ADDOPERAND(Opc, MMPULite::REPEATK, @-2.S, @$.E);
  Operands.push_back(imm);
  imm = OPERAND(Imm, $3, @3.S, @3.E);
};
immrep: imm {imm = OPERAND(Imm, $1, @$.S, @$.E);};
regrep: loops {$$ = ($1 & 0x1F) | (($1 >> 6) << 5); imm = OPERAND(Imm, $$, @$.S, @$.E);};
loops: loop | loop andflag loops {$$ = $3 << 6 | $1;};
loop: negflag kiflag {$$ = $2 | 3 << 4;} | kiflag {$$ = $1 | 1 << 5;};
lpinst: lpexp lpcond;
reinst: lpexp error {llvmerror(&@2, "Invalid condition expression."); YYABORT;}
lpexp: LOOP label ALPHA {
  ADDOPERAND(Opc, MMPULite::LPTO, @$.S, @$.E);
  if (expr) Operands.push_back(expr);
  else ADDOPERAND(Imm, $2, @2.S, @2.E);
};
lpexp: LOOP error ALPHA {llvmerror(&@2, "Invalid label. Immediate number or '(' expression ')' is expected. Try 'LPTO IMM @(KIn)' or 'LPTO (EXPR) @(KIn)'."); YYABORT;};
lpcond: _flag kiflag flag_ {
  ADDOPERAND(Imm, $2, @2.S, @2.E);
  ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
}
|       _flag kiflag SUB imm flag_ {
  ADDOPERAND(Imm, $2, @2.S, @2.E);
  ADDOPERAND(Imm, $4, @4.S, @4.E);
}
|       _flag imm flag_ {
  ADDOPERAND(Imm, ($2 & 0xF) | 0x10, @2.S, @2.E);
  ADDOPERAND(Imm, $2 >> 4, @2.S, @2.E);
}
jmpinst: JMP label ALPHA condexp {
  if (expr) Operands.push_back(expr);
  else ADDOPERAND(Imm, $2, @2.S, @2.E);
  if (imm) Operands.push_back(imm);
};
condexp: _flag trueflag flag_ {ADDOPERAND(Opc, MMPULite::JUMP, @-2.S, @$.E);}|
         regrep {ADDOPERAND(Opc, MMPULite::JUMPK, @-2.S, @$.E);};
mpustop: MPUSTOP {ADDOPERAND(Opc, MMPULite::MPUStop, @$.S, @$.E);};

/*Destination types: 0:biu,1:unit,2:m */
r2destp: r2dest LPAREN IPATH RPAREN {$$ = $1; ipath = OPERAND(Imm, $3 + 1, @3.S, @3.E);};
r2dest : ialut | imact | falut | fmact ;
r1destp : r1dest {$$ = $1; ipath = OPERAND(Imm, 0, SMLoc(), SMLoc());};
r1dest: shu0t | shu1t ;
r0dest: mindexn ;
r3dest: biu0 | biu1 | biu2 ;
lddest: r1destp | r2destp | mindex ;
shu0dest: r0dest | r2destp | r3dest | shu0t {$$ = $1; ipath = OPERAND(Imm, 0, SMLoc(), SMLoc());};
shu1dest: r0dest | r2destp | r3dest | shu1t {$$ = $1; ipath = OPERAND(Imm, 0, SMLoc(), SMLoc());};
mindex: mindexi | mindexn ;
ialudest: ialut | imact | falut | r0dest | r1destp | r3dest ;
imacdest: ialudest ;
faludest: ialudest | fmact ;
fmacdest: r0dest | r1destp | r3dest | falut | fmact;
ialut: ialu DOT t {$$ = 3; unit = OPERAND(Reg, MMPULiteReg::IALU, @1.S, @1.E); if (!ut) ut = tp ? tp : (tn ? tn : tm);};
imact: imac DOT t {$$ = 3; unit = OPERAND(Reg, MMPULiteReg::IMAC, @1.S, @1.E); if (!ut) ut = tp ? tp : (tn ? tn : tm);};
falut: falu DOT t {$$ = 3; unit = OPERAND(Reg, MMPULiteReg::FALU, @1.S, @1.E); if (!ut) ut = tp ? tp : (tn ? tn : tm);};
fmact: fmac DOT t {$$ = 3; unit = OPERAND(Reg, MMPULiteReg::FMAC, @1.S, @1.E); if (!ut) ut = tp ? tp : (tn ? tn : tm);};
biu0: BIU0 {$$ = 2; b = OPERAND(Reg, MMPULiteReg::BIU0, @$.S, @$.E);};
biu1: BIU1 {$$ = 2; b = OPERAND(Reg, MMPULiteReg::BIU1, @$.S, @$.E);};
biu2: BIU2 {$$ = 2; b = OPERAND(Reg, MMPULiteReg::BIU2, @$.S, @$.E);};
shu0t: SHU0 DOT t {$$ = 1; unit = OPERAND(Reg, MMPULiteReg::SHU0, @1.S, @1.E); if (!ut) ut = tp ? tp : (tn ? tn : tm);};
shu1t: SHU1 DOT t {$$ = 1; unit = OPERAND(Reg, MMPULiteReg::SHU1, @1.S, @1.E); if (!ut) ut = tp ? tp : (tn ? tn : tm);};
mindexs: MINDEXS {$$ = 0; ms = md; md = OPERAND(Reg, MMPULiteReg::MSPP, @$.S, @$.E);};
mindexi: MINDEXI {$$ = 0; ms = md; md = OPERAND(Reg, MMPULiteReg::MIPP, @$.S, @$.E);};
mindexn: MINDEXN {$$ = 0; ms = md;
  md = OPERAND(Reg,
               MRI->getRegClass(MMPULiteReg::MRegRegClassID).getRegister($1),
               @$.S, @$.E);
};
t: TREG {
  unsigned treg = MRI->getRegClass(MMPULiteReg::TPortRegClassID).getRegister($1);
  if (!tm) tm = OPERAND(Reg, treg, @$.S, @$.E);
  else if (!tn) tn = OPERAND(Reg, treg, @$.S, @$.E);
  else if (!tp) tp = OPERAND(Reg, treg, @$.S, @$.E);
  else if (!ut) ut = OPERAND(Reg, treg, @$.S, @$.E);
};
_constt: TBB {$$ = MMPULiteReg::TBB;} |
         TBH {$$ = MMPULiteReg::TBH;} |
         TBW {$$ = MMPULiteReg::TBW;} |
         TBD {$$ = MMPULiteReg::TBD;} |
         TSQ {$$ = MMPULiteReg::TSQ;} ;
constt: _constt {
  unsigned treg = $1;
  if (!tm) tm = OPERAND(Reg, treg, @$.S, @$.E);
  else if (!tn) tn = OPERAND(Reg, treg, @$.S, @$.E);
  else if (!tp) tp = OPERAND(Reg, treg, @$.S, @$.E);
  else if (!ut) ut = OPERAND(Reg, treg, @$.S, @$.E);
}
shusrct: t | constt;
tripara: dupara COMMA t;
dupara: t COMMA t;
imm: imm5 | IMM;
imm5: IMM3 | IMM5;

stflag: imbrakflag ;
ldflag: mbrakflag ;
imbrakflag: iflag COMMA mbrakflag | iflag | mflag COMMA ibrakflag | mflag |
            brflag COMMA imakflag | brflag | aflag COMMA imbrkflag | aflag |
            kflag COMMA imbraflag | kflag;
ibrakflag: iflag COMMA brakflag | iflag | brflag COMMA iakflag | brflag |
           aflag COMMA ibrkflag | aflag | kflag COMMA ibraflag | kflag;
imakflag: iflag COMMA makflag | iflag | mflag COMMA iakflag | mflag |
          aflag COMMA imkflag | aflag | kflag COMMA imaflag | kflag;
imbrkflag: iflag COMMA mbrkflag | iflag | mflag COMMA ibrkflag | mflag |
           brflag COMMA imkflag | brflag | kflag COMMA imbrflag | kflag;
imbraflag: iflag COMMA mbraflag | iflag | mflag COMMA ibraflag | mflag |
           brflag COMMA imaflag | brflag | aflag COMMA imbrflag | aflag;
iakflag: iflag COMMA akflag | iflag | aflag COMMA ikflag | aflag | kflag COMMA iaflag | kflag;
ibrkflag: iflag COMMA brkflag | iflag | brflag COMMA ikflag | brflag | kflag COMMA ibrflag | kflag;
ibraflag: iflag COMMA braflag | iflag | brflag COMMA iaflag | brflag | aflag COMMA ibrflag | aflag;
imkflag: iflag COMMA mkflag | iflag | mflag COMMA ikflag | mflag | kflag COMMA imflag | kflag;
imaflag: iflag COMMA maflag | iflag | mflag COMMA iaflag | mflag | aflag COMMA imflag | aflag;
imbrflag: iflag COMMA mbrflag | iflag | mflag COMMA ibrflag | mflag | brflag COMMA imflag | brflag;
ikflag: iflag COMMA kflag | iflag | kflag COMMA iflag | kflag;
iaflag: iflag COMMA aflag | iflag | aflag COMMA iflag | aflag;
ibrflag: iflag COMMA brflag | iflag | brflag COMMA iflag | brflag;
imflag: iflag COMMA mflag | iflag | mflag COMMA iflag | mflag;

mbrakflag: mflag COMMA brakflag | mflag | brflag COMMA makflag | brflag | aflag COMMA mbrkflag | aflag | kflag COMMA mbraflag | kflag ;
brakflag: brflag COMMA akflag | brflag | aflag COMMA brkflag | aflag | kflag COMMA braflag | kflag ;
makflag: mflag COMMA akflag | mflag | aflag COMMA mkflag | aflag | kflag COMMA maflag | kflag ;
mbrkflag: mflag COMMA brkflag | mflag | brflag COMMA mkflag | brflag | kflag COMMA mbrflag | kflag ;
mbraflag: mflag COMMA braflag | mflag | brflag COMMA maflag | brflag | aflag COMMA mbrflag | aflag ;
akflag: aflag COMMA kflag | aflag | kflag | kflag COMMA aflag ;
mkflag: mflag COMMA kflag | mflag | kflag | kflag COMMA mflag ;
brkflag: brflag COMMA kflag | brflag | kflag | kflag COMMA brflag ;
maflag: mflag COMMA aflag | mflag | aflag | aflag COMMA mflag ;
mbrflag: mflag COMMA brflag | mflag | brflag | brflag COMMA mflag ;
braflag: brflag COMMA aflag | brflag | aflag | aflag COMMA brflag ;
uthflag: uflag COMMA thflag | uflag | tflag COMMA uhflag | tflag | hflag COMMA utflag | hflag;
thflag: tflag COMMA hflag | tflag | hflag | hflag COMMA tflag;
uhflag: uflag COMMA hflag | uflag | hflag | hflag COMMA uflag;
utbflag: t_ubflag | ubflag;
t_bflag: tflag COMMA bflag | tflag | bflag COMMA tflag ;
ubflag: b_uflag | uflag;
b__uflag: b_flag COMMA uflag | b_flag | uflag COMMA b_flag;
b_uflag: bflag COMMA uflag | bflag | uflag COMMA bflag;
utflag: t_uflag | uflag;
t_uflag: uflag COMMA tflag | tflag | tflag COMMA uflag ;
t_uibflag: tflag COMMA uibflag | tflag | uflag COMMA t_ibflag | iflag COMMA t_ubflag | bflag COMMA t_uiflag;
uibflag: b_uiflag | uiflag;
b_uiflag: bflag COMMA uiflag | bflag | uflag COMMA b_iflag | iflag COMMA b_uflag;
t_ubflag: tflag COMMA ubflag | tflag | uflag COMMA t_bflag | bflag COMMA t_uflag;
t_ibflag: tflag COMMA ibflag | tflag | iflag COMMA t_bflag | bflag COMMA t_iflag;
t_uiflag: tflag COMMA uiflag | tflag | uflag COMMA t_iflag | iflag COMMA t_uflag;
ibflag: b_iflag | iflag;
b_iflag: bflag COMMA iflag | bflag | iflag COMMA bflag;
t_iflag: tflag COMMA iflag | tflag | iflag COMMA tflag ;
uiflag: iflag COMMA uflag | iflag | uflag | uflag COMMA iflag ;
crlubflag: crflag COMMA lubflag | crflag | lflag COMMA crubflag | lflag | uflag COMMA crlbflag | uflag | bflag COMMA crluflag | bflag;
crlbflag: crflag COMMA lbflag | crflag | lflag COMMA crbflag | lflag | bflag COMMA crlflag | bflag ;
crluflag: crflag COMMA luflag | crflag | lflag COMMA cruflag | lflag | uflag COMMA crlflag | uflag ;
crlflag: crflag COMMA lflag | crflag | lflag | lflag COMMA crflag;
crubflag: crflag COMMA ubflag | crflag | uflag COMMA crbflag | uflag | bflag COMMA cruflag | bflag ;
crbflag: crflag COMMA bflag | crflag | bflag | bflag COMMA crflag ;
cruflag: crflag COMMA uflag | crflag | uflag | uflag COMMA crflag ;
lbflag: lflag COMMA bflag | lflag | bflag | bflag COMMA lflag;
luflag: lflag COMMA uflag | lflag | uflag | uflag COMMA lflag;
lubflag: lflag COMMA ubflag | lflag | uflag COMMA lbflag | uflag | bflag COMMA luflag | bflag ;
tsdflag: tflag COMMA sdflag | sdflag | sdflag COMMA tflag;
sdflag: sflag | dflag ;
tdflag: tflag COMMA dflag | dflag | dflag COMMA tflag;
bflag: b_flag | hflag ;
crsdflag: crflag COMMA sdflag | sdflag | sdflag COMMA crflag ;
byflag: tcflag COMMA bflag | bflag COMMA tcflag | bflag | tcflag;
byflag: tcflag COMMA lflag | lflag COMMA tcflag | lflag;
byflag: error { llvmerror(&@1, "Invalid COMB flag. Available flags are TC, B/H/L."); YYABORT; }

b_flag: B {flags.set(BF);};
hflag: H {flags.set(HF);};
brflag: BR {flags.set(BRF);};
mflag: M {flags.set(MF);};
aflag: APP {flags.set(APPF);};
kflag: KPP {flags.set(KPPF);};
tflag: T {flags.set(TF);};
uflag: U {flags.set(UF);};
sflag: S {flags.set(SF);};
dflag: D {flags.set(DF);};
lflag: L {flags.set(LF);};
crflag: CR {flags.set(CRF);};
iflag: I {flags.set(IF);};
tcflag: TC {flags.set(TCF);};
negflag: NEG ;
andflag: AND ;
kiflag: KI ;
trueflag: TRUE ;
cflag: C ;
ncflag: NEG C ;
condflag: ALPHA _flag cflag flag_ {if (condpos) Operands[condpos-1] = OPERAND(Reg, MMPULiteReg::C, @$.S, @$.E);}
|         ALPHA _flag ncflag flag_ {if (condpos) Operands[condpos-1] = OPERAND(Reg, MMPULiteReg::NC, @$.S, @$.E);}
|         {if (condpos) Operands[condpos-1] = OPERAND(Reg, MMPULiteReg::NOC, @$.S, @$.E);};

_flag: LPAREN {FlagS = @$.E;};
flag_: RPAREN {FlagE = @$.S;};

ialu: IALU ;
imac: IMAC ;
falu: FALU ;
fmac: FMAC ;
ctrl: START {$$ = 0;} | STOP {$$ = 1;};
label: EXPR {$$ = 0; expr = $1;} | imm {$$ = $1; expr = 0;};
%%

