                   %{
#include <string>
#include <bitset>

std::bitset<64> flags;
static unsigned int flagsort;
const unsigned HF=1, UF=2, TF=3, SF=4, DF=5, IF=6, LF=7, APPF=8, KPPF=9, CRF=10, BRF=11, MF=12, MLF=13, MHF=14, NCF=15, 
               CIF = 16, FF = 17, BF=18, PF = 19, RF = 20, CF = 21, SENDF = 22, S0F = 23, S1F = 24, S2F = 25, S3F = 26, SSF=27, QF=28, QLF=29, QHF=30, dLF=31, dHF=32, dMLF=33, dMHF=34, SPPF=35, IPPF=36,
               W0F = 37, W1F=38, W2F = 39, W3F = 40, W4F = 41, APPF2 = 42, KMF=43, KGF=44, KMEABLEF=45, KGEABLEF=46, KEF=47, L1F=48, L2F=49, L3F=50, L4F=51, ALLF =52, KI0F= 53, KI1F=54, KI2F=55, KI3F=56, KI4F=57,
               KI5F =58, KI6F=59, KI7F=60;
static UCPM::UCPMAsmOperand *opc, *tm, *tn, *tk, *tp, *revt, *f, *ff, *shift, *step, *qlh, *sia, *unit, *unit2, *unit3, *ut, *b, *b2, *md, *ms, *imm,*imm1,*imm2, *expr, *ipath;//unit2, b2 are used as alternative unit, such as MReg Target
static int slotid;
static unsigned condpos;
SMLoc FlagS, FlagE;
llvm::raw_ostream &OS = errs();
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

//%output  	"UCPMGenInstrParser.bison"
%defines 	"UCPMGenInstrParser.h"
%define api.pure
%name-prefix="ucpm"

%parse-param { OperandVector &Operands }

%union {
	UCPM::UCPMAsmOperand* op;
  std::string *string;
  int val;
  int token;
}
%token <val> NEGIMM IMM3 IMM IMM5 ASSIGNTO EQU NEQ ST NLT LT NST LPAREN RPAREN LBRACE RBRACE LBRACKET RBRACKET DOT MASK WAIT COMMA ADD SUB MUL CMUL LSHT RSHT
%token <val> OR AND XOR NOT NOT2 NEG ADDSUB ACC1 ACC2 ALPHA SPLIT LINEEND SHU0 SHU1 SHU2 BIU0 BIU1 BIU2 M COND SETCOND
%token <val> IALU IMAC FALU FMAC IFALU IFMAC TB TBB TBH TBW TBD TSQ IND BY
%token <val> CPRS EXPD START STOP MAX MIN ABS MERGE MDIVR MDIVQ DIVR DIVQ DIVS RECIP RSQRT SINGLE DOUBLE MR INT RMAX RMIN
%token <val> REPEAT LOOP JMP MPUSTOP
%token <val> BR CR APP KPP SPP IPP CI F U P R T B H S D I L TC C CFLAG LABEL SHU BIU SHIFT0 SHIFT1 SHIFT2 SHIFT3 SEND FLOAT Q QL QH ML MH BIT BYTE
%token <val> TRUE ASSIGN NOOP UINT DM R0 R1 R2 R3 R4 R5 IPATH WFLAG KM KE KG KMEABLE KGEABLE L1 L2 L3 L4 ALL CONFIGBIU CONFIGMFETCH CONFIGMR CONFIGMW
%token <string> IDENTIFIER
%token <op> EXPR
%token <val> TREG MINDEXN KI
%token <val> W0 W1 W2 W3 W4 K IMMSYM MREG0 MREG1 MREG2 MREG3 MREG4 MREG5 KI1215 KI1619 KI2023 KI2427 KI1227 KI1618 KI2022 KI2426 

%type <val> slots slotref slot 
%type <val> mr012345slot shuslot shu0code shu1code shu2code shu0inst shu1inst shu2inst biu0t biu1t biu2t shut shu0t shu1t shu2t biuslot biu0code biu1code biu2code biu0inst biu1inst biu2inst seqslot 
%type <val> r0inst r1inst r2inst r3inst r4inst r5inst maccdestp maccdest ialut imact ifalut ifmact shu0te shu1te shu2te b1shu2te b2shu1te ialute imacte ifalute ifmacte s1biu1t s2biu1t sshu0t sshu1t sshu2t bbiu0te bbiu1te bbiu2te
%type <val> tran0Instr tran1Instr tran2Instr tran0clause tran1clause tran2clause shuwaitInstr
%type <val> ucpshusrcTm ucpindtkclause ucpindtkclause2 ucpshusrcTk ucpindtbclause ucpindtbclause2 ucpshuexp ucpshuexp2 ucpindclause ucpindclause2 shu0dest shu1dest shu2dest mindexn w_mindexn mindexsia w_mindexsia ialuasclause biu0dest biu1dest biu2dest 
%type <val> ialudest ifaludest imacdest ifmacdest biut imulreal imulcomp imacclause ifmacclause 
%type <val> ialu imac falu fmac ifalu ifmac imm imm1 imm2 imm5 mcodeline hmacro _flag flag_ constt _constt
%type <val> ldselect lddis ldstep stinst binInstr shiftInstr compareInstr notInstr movInstr maskInstr waitInstr imm0Instr imm1Instr imm2Instr biu0imm biu1imm biu2imm setcond0Instr setcond1Instr setcond2Instr biu0cond biu1cond biu2cond
%type <val> reinst repeatexp immrep lpinst lpexp lpcond kiflag label mpustop
%type <val> shu0setcondInstr shu1setcondInstr shu2setcondInstr shu0cond shu1cond shu2cond r0setcondInstr r1setcondInstr r2setcondInstr r3setcondInstr r4setcondInstr r5setcondInstr 

%%
mcodeline: NOOP LINEEND {ADDOPERAND(Opc, UCPM::NOP, @1.S, @1.E); YYACCEPT;}
|          slots LINEEND {YYACCEPT;};
mcodeline: NOOP error { llvmerror(&@2, "No line end."); YYABORT;};
slots: slotref | slotref SPLIT slots ;
slotref : slot {
  tm = NULL;
  tn = NULL;
  tp = NULL;
  tk = NULL;
  ut = NULL;
  revt = NULL;
  ms = NULL;
  md = NULL;
  imm = NULL;
  imm1 = NULL;//dcx
  imm2 = NULL;//dcx
  f = NULL;
  ff = NULL;
  shift = NULL;
  unit = NULL;
  expr = NULL;
  b = NULL;
  opc = NULL;
  ipath = NULL;
  condpos = 0;
  FlagS = SMLoc();
  FlagE = SMLoc();
  //dcx
  qlh = NULL;
  step = NULL;
  sia = NULL;
  
};
slot: mr012345slot {ADDOPERAND(Slot, 0 + $1, @$.S, @$.E);} |
      shuslot {ADDOPERAND(Slot, 6 + $1, @$.S, @$.E);} |
      ialuslot {ADDOPERAND(Slot, 9, @$.S, @$.E);} |
      ifaluslot {ADDOPERAND(Slot, 10, @$.S, @$.E);} |
      imacslot {ADDOPERAND(Slot, 11, @$.S, @$.E);} |
      ifmacslot {ADDOPERAND(Slot, 12, @$.S, @$.E);} |
      biuslot {ADDOPERAND(Slot, 13 + $1, @$.S, @$.E);}|  
      seqslot |
      hmacro | //?
      error { llvmerror(&@1, "Unrecognized slot."); YYABORT; };
hmacro: IDENTIFIER {ADDOPERAND(HMacro, $1, @$.S, @$.E);};
hmacro: IDENTIFIER LPAREN RPAREN {ADDOPERAND(HMacro, $1, @$.S, @$.E); ADDOPERAND(Imm, 1, @$.S, @$.E);};

// start mreg -------------------------------------------------- 
//yangl
mr012345slot: mr0code {$$ = 0;} | mr1code {$$ = 1;} | mr2code {$$ = 2;} | mr3code {$$ = 3;} | mr4code {$$ = 4;} | mr5code {$$ = 5;};


mr0code: R0 DOT r0inst {
  switch ($3) {
  case 0:
    //r0inst: mindexn -> r0dest
    ADDOPERAND(Opc, UCPM::MR0ToDestCom, @$.S, @$.E);
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit2));
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(md));
    break;
    
  case 1:
    //r0inst: (m[i++/s++/a++]) -> r0dest         
    flagsort = (flags[APPF2] << 2) | (flags[SPPF] << 1) | flags[IPPF];
    sia = OPERAND(Imm, flagsort, FlagS, FlagE);
	      
    flags.reset();
	      
    ADDOPERAND(Opc, UCPM::MR0ToDestSIA, @$.S, @$.E);
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit2));
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(sia));
    break;
    
  default:
    //OS<<"Parsing R0 DOT r0inst fails!"<<"\n";
    break;
  }

}
| R0 DOT mregwait{          
      ADDOPERAND(Opc, UCPM::MR0Wait, @$.S, @$.E);
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm));  
}
| r0Toconfigmr{

		  if(flags[SF])
		      flagsort = 0x0;
		  else if(flags[IF])
		      flagsort = 0x1;
	      f = OPERAND(Imm, flagsort, FlagS, FlagE);
	      flags.reset();
	      
	   ADDOPERAND(Opc, UCPM::MR0ToConfigMR, @$.S, @$.E);
	   Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(md));
	   Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(f)); 
	   Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm)); 
}
| r0Toconfigmw{

		  if(flags[SF])
		      flagsort = 0x0;
		  else if(flags[IF])
		      flagsort = 0x1;
	      f = OPERAND(Imm, flagsort, FlagS, FlagE);
	      flags.reset();
	      
	   ADDOPERAND(Opc, UCPM::MR0ToConfigMW, @$.S, @$.E);
	   Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(md));
	   Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(f)); 
	   Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm)); 
}
| configmrTor0{
	      
	   ADDOPERAND(Opc, UCPM::ConfigMRToMR0, @$.S, @$.E);
	   Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(md));
	   Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm)); 
}
| configmwTor0{

	   ADDOPERAND(Opc, UCPM::ConfigMWToMR0, @$.S, @$.E);
	   Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(md));
	   Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm)); 
}
| r0configmfetch{
	     
		  if(flags[KI0F])
		      flagsort = 0x0;
		  else if(flags[KI1F])
		      flagsort = 0x1;
		  else if(flags[KI2F])
		      flagsort = 0x2;
		  else if(flags[KI3F])
		      flagsort = 0x3;
		  else if(flags[KI4F])
		      flagsort = 0x4;
		  else if(flags[KI5F])
		      flagsort = 0x5;
		  else if(flags[KI6F])
		      flagsort = 0x6;
		  else if(flags[KI7F])
		      flagsort = 0x7;
	      f = OPERAND(Imm, flagsort, FlagS, FlagE);
	      flags.reset();
	      
	   ADDOPERAND(Opc, UCPM::MR0ConfigMFetch, @$.S, @$.E);
	   Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(md));
	   Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(f)); 
}
| R0 DOT configbiu{
	     
		  if(flags[KMF])
		      flagsort = 0x0;
		  else if(flags[KGF])
		      flagsort = 0x2;
		  else if(flags[KEF])
		      flagsort = 0x5;
		  else if(flags[KMEABLEF])
		      flagsort = 0x1;
		  else if(flags[KGEABLEF])
		      flagsort = 0x3;
		  else if(flags[BRF])
		      flagsort = 0x4;
		  else if(flags[L1F])
		      flagsort = 0xc;
		  else if(flags[L2F])
		      flagsort = 0xd;
		  else if(flags[L3F])
		      flagsort = 0xe;
		  else if(flags[L4F])
		      flagsort = 0xf;
		  else if(flags[ALLF])
		      flagsort = 0x8;
	      f = OPERAND(Imm, flagsort, FlagS, FlagE);
	      flags.reset();
	      
	   ADDOPERAND(Opc, UCPM::MR0ConfigBIU, @$.S, @$.E);
	   Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(md));
	   Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm)); 
	   Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(f)); 
}
| r0setcondInstr{   
	    
	    ADDOPERAND(Opc, UCPM::MR0SetCond, @$.S, @$.E);
	    
	    if (imm == NULL)
	      ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
	    else
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm));
	    if (imm1 == NULL)
	      ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
	    else
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm1));   
};

mr1code: R1 DOT r1inst {
  switch ($3) {
  case 0:
    ADDOPERAND(Opc, UCPM::MR1ToDestCom, @$.S, @$.E);
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit2));
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(md));
    break;
  case 1:
    flagsort = (flags[APPF2] << 2) | (flags[SPPF] << 1) | flags[IPPF];
    sia = OPERAND(Imm, flagsort, FlagS, FlagE);	      
    flags.reset();
    ADDOPERAND(Opc, UCPM::MR1ToDestSIA, @$.S, @$.E);
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit2));
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(sia));
  default:
    break;
  }
  
}
| R1 DOT mregwait{           
      ADDOPERAND(Opc, UCPM::MR1Wait, @$.S, @$.E);
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm));  
}
| r1setcondInstr{   
	    
	    ADDOPERAND(Opc, UCPM::MR1SetCond, @$.S, @$.E);
	    
	    if (imm == NULL)
	      ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
	    else
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm));
	    if (imm1 == NULL)
	      ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
	    else
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm1));   
};

mr2code: R2 DOT r2inst {
  switch ($3) {
  case 0:
    ADDOPERAND(Opc, UCPM::MR2ToDestCom, @$.S, @$.E);
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit2));
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(md));
    break;
  case 1:
    flagsort = (flags[APPF2] << 2) | (flags[SPPF] << 1) | flags[IPPF];
    sia = OPERAND(Imm, flagsort, FlagS, FlagE);      
    flags.reset();
    ADDOPERAND(Opc, UCPM::MR2ToDestSIA, @$.S, @$.E);
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit2));
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(sia));
  default:
    break;
  }
  
}
| R2 DOT mregwait{           
      ADDOPERAND(Opc, UCPM::MR2Wait, @$.S, @$.E);
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm));  
}
| R2 DOT configbiu{
	     
		  if(flags[KMF])
		      flagsort = 0x0;
		  else if(flags[KGF])
		      flagsort = 0x2;
		  else if(flags[KEF])
		      flagsort = 0x5;
		  else if(flags[KMEABLEF])
		      flagsort = 0x1;
		  else if(flags[KGEABLEF])
		      flagsort = 0x3;
		  else if(flags[BRF])
		      flagsort = 0x4;
		  else if(flags[L1F])
		      flagsort = 0xc;
		  else if(flags[L2F])
		      flagsort = 0xd;
		  else if(flags[L3F])
		      flagsort = 0xe;
		  else if(flags[L4F])
		      flagsort = 0xf;
		  else if(flags[ALLF])
		      flagsort = 0x8;
	      f = OPERAND(Imm, flagsort, FlagS, FlagE);
	      flags.reset();
	      
	   ADDOPERAND(Opc, UCPM::MR2ConfigBIU, @$.S, @$.E);
	   Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(md));
	   Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm)); 
	   Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(f)); 
}
| r2setcondInstr{   
	    
	    ADDOPERAND(Opc, UCPM::MR2SetCond, @$.S, @$.E);
	    
	    if (imm == NULL)
	      ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
	    else
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm));
	    if (imm1 == NULL)
	      ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
	    else
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm1));   
};

mr3code: R3 DOT r3inst {
  switch ($3) {
  case 0:
    ADDOPERAND(Opc, UCPM::MR3ToDestCom, @$.S, @$.E);
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit2));
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(md));
    break;
  case 1:
    flagsort = (flags[APPF2] << 2) | (flags[SPPF] << 1) | flags[IPPF];
    sia = OPERAND(Imm, flagsort, FlagS, FlagE);	      
    flags.reset();
    ADDOPERAND(Opc, UCPM::MR3ToDestSIA, @$.S, @$.E);
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit2));
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(sia));
  default:
    break;
  }
 
}
| R3 DOT mregwait{           
      ADDOPERAND(Opc, UCPM::MR3Wait, @$.S, @$.E);
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm));  
}
| r3setcondInstr{   
	    
	    ADDOPERAND(Opc, UCPM::MR3SetCond, @$.S, @$.E);
	    
	    if (imm == NULL)
	      ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
	    else
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm));
	    if (imm1 == NULL)
	      ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
	    else
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm1));   
};

mr4code: R4 DOT r4inst {
  switch ($3) {
  case 0:
    ADDOPERAND(Opc, UCPM::MR4ToDestCom, @$.S, @$.E);
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit2));
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(md));
    break;
  case 1:
    flagsort = (flags[APPF2] << 2) | (flags[SPPF] << 1) | flags[IPPF];
    sia = OPERAND(Imm, flagsort, FlagS, FlagE);	      
    flags.reset();
    ADDOPERAND(Opc, UCPM::MR4ToDestSIA, @$.S, @$.E);
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit2));
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(sia));
  default:
    break;
  }
 
}
| R4 DOT mregwait{           
      ADDOPERAND(Opc, UCPM::MR4Wait, @$.S, @$.E);
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm));  
}
| R4 DOT configbiu{
	     
		  if(flags[KMF])
		      flagsort = 0x0;
		  else if(flags[KGF])
		      flagsort = 0x2;
		  else if(flags[KEF])
		      flagsort = 0x5;
		  else if(flags[KMEABLEF])
		      flagsort = 0x1;
		  else if(flags[KGEABLEF])
		      flagsort = 0x3;
		  else if(flags[BRF])
		      flagsort = 0x4;
		  else if(flags[L1F])
		      flagsort = 0xc;
		  else if(flags[L2F])
		      flagsort = 0xd;
		  else if(flags[L3F])
		      flagsort = 0xe;
		  else if(flags[L4F])
		      flagsort = 0xf;
		  else if(flags[ALLF])
		      flagsort = 0x8;
	      f = OPERAND(Imm, flagsort, FlagS, FlagE);
	      flags.reset();
	      
	   ADDOPERAND(Opc, UCPM::MR4ConfigBIU, @$.S, @$.E);
	   Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(md));
	   Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm)); 
	   Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(f)); 
}
| r4setcondInstr{   
	    
	    ADDOPERAND(Opc, UCPM::MR4SetCond, @$.S, @$.E);
	    
	    if (imm == NULL)
	      ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
	    else
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm));
	    if (imm1 == NULL)
	      ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
	    else
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm1));   
};

mr5code: R5 DOT r5inst {
  switch ($3) {
  case 0:
    ADDOPERAND(Opc, UCPM::MR5ToDestCom, @$.S, @$.E);
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit2));
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(md));
    break;
  case 1:
    flagsort = (flags[APPF2] << 2) | (flags[SPPF] << 1) | flags[IPPF];
    sia = OPERAND(Imm, flagsort, FlagS, FlagE);	      
    flags.reset();
    ADDOPERAND(Opc, UCPM::MR5ToDestSIA, @$.S, @$.E);
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit2));
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(sia));
  default:
    break;
  }
  
}
| R5 DOT mregwait{           
      ADDOPERAND(Opc, UCPM::MR5Wait, @$.S, @$.E);
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm));  
}
| r5setcondInstr{   
	    
	    ADDOPERAND(Opc, UCPM::MR5SetCond, @$.S, @$.E);
	    
	    if (imm == NULL)
	      ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
	    else
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm));
	    if (imm1 == NULL)
	      ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
	    else
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm1));   
};	


mregwait: WAIT IMM5 {imm = OPERAND(Imm, $2, @2.S, @2.E);};

r0Toconfigmr: mindexn ASSIGNTO CONFIGMR LBRACKET IMM5 RBRACKET _flag siflag flag_
	  {imm = OPERAND(Imm, $5, @5.S, @5.E);};

r0Toconfigmw: mindexn ASSIGNTO CONFIGMW LBRACKET IMM5 RBRACKET _flag siflag flag_
	  {imm = OPERAND(Imm, $5, @5.S, @5.E);};

configmrTor0: CONFIGMR LBRACKET IMM5 RBRACKET ASSIGNTO mindexn
	  {imm = OPERAND(Imm, $3, @3.S, @3.E);};

configmwTor0: CONFIGMW LBRACKET IMM5 RBRACKET ASSIGNTO mindexn
	  {imm = OPERAND(Imm, $3, @3.S, @3.E);};
	  
r0configmfetch: mindexn ASSIGNTO CONFIGMFETCH LBRACKET configmflag RBRACKET;

configbiu: mindexn ASSIGNTO CONFIGBIU DOT t LBRACKET configflag RBRACKET;


r0setcondInstr: IMMSYM DOT IMM5 ASSIGNTO MREG0 DOT SETCOND LBRACKET IMM5 RBRACKET 
	  {imm = OPERAND(Imm, $3, @3.S, @3.E);imm1 = OPERAND(Imm, $9, @9.S, @9.E);};  
r1setcondInstr: IMMSYM DOT IMM5 ASSIGNTO MREG1 DOT SETCOND LBRACKET IMM5 RBRACKET 
	  {imm = OPERAND(Imm, $3, @3.S, @3.E);imm1 = OPERAND(Imm, $9, @9.S, @9.E);};  
r2setcondInstr: IMMSYM DOT IMM5 ASSIGNTO MREG2 DOT SETCOND LBRACKET IMM5 RBRACKET 
	  {imm = OPERAND(Imm, $3, @3.S, @3.E);imm1 = OPERAND(Imm, $9, @9.S, @9.E);};  
r3setcondInstr: IMMSYM DOT IMM5 ASSIGNTO MREG3 DOT SETCOND LBRACKET IMM5 RBRACKET 
	  {imm = OPERAND(Imm, $3, @3.S, @3.E);imm1 = OPERAND(Imm, $9, @9.S, @9.E);};  
r4setcondInstr: IMMSYM DOT IMM5 ASSIGNTO MREG4 DOT SETCOND LBRACKET IMM5 RBRACKET 
	  {imm = OPERAND(Imm, $3, @3.S, @3.E);imm1 = OPERAND(Imm, $9, @9.S, @9.E);};  
r5setcondInstr: IMMSYM DOT IMM5 ASSIGNTO MREG5 DOT SETCOND LBRACKET IMM5 RBRACKET 
	  {imm = OPERAND(Imm, $3, @3.S, @3.E);imm1 = OPERAND(Imm, $9, @9.S, @9.E);};  
	  
	  
r0inst: mindexn ASSIGNTO r0dest {$$ = 0} |
        M LBRACKET siaflags RBRACKET ASSIGNTO r0dest {$$ = 1};

r1inst: mindexn ASSIGNTO r1dest {$$ = 0} |
        M LBRACKET siaflags RBRACKET ASSIGNTO r1dest {$$ = 1};

r2inst: mindexn ASSIGNTO r2dest {$$ = 0} |
        M LBRACKET siaflags RBRACKET ASSIGNTO r2dest {$$ = 1};

r3inst: mindexn ASSIGNTO r3dest {$$ = 0} |
        M LBRACKET siaflags RBRACKET ASSIGNTO r3dest {$$ = 1};
        
r4inst: mindexn ASSIGNTO r4dest {$$ = 0} |
        M LBRACKET siaflags RBRACKET ASSIGNTO r4dest {$$ = 1};
        
r5inst: mindexn ASSIGNTO r5dest {$$ = 0} |
        M LBRACKET siaflags RBRACKET ASSIGNTO r5dest {$$ = 1};

mr012345slot: R0 DOT error { llvmerror(&@3, "Incorrect M.r0 inst."); YYABORT;} |
           R1 DOT error { llvmerror(&@3, "Incorrect M.r1 inst."); YYABORT;} |
           R2 DOT error { llvmerror(&@3, "Incorrect M.r2 inst."); YYABORT;} |
           R3 DOT error { llvmerror(&@3, "Incorrect M.r3 inst."); YYABORT;} |
           R4 DOT error { llvmerror(&@3, "Incorrect M.r4 inst."); YYABORT;} |
           R5 DOT error { llvmerror(&@3, "Incorrect M.r5 inst."); YYABORT;} ;
           

r0dest: ialut | imact | shu0t | biu0t ;
r1dest: ialut | imact | shu1t ;
r2dest: ialut | imact | shu2t | biu2t ;
r3dest: ifalut | ifmact | shu0t ;
r4dest: ifalut | ifmact | shu1t | biu1t ;
r5dest: ifalut | ifmact | shu2t | biu2t ;
biut: biu0t | biu1t | biu2t ;

// end mreg -------------------------------------------------- 


// start shu --------------------------------------------------
shuslot: shu0code {$$ = 0;} | shu1code {$$ = 1;} | shu2code {$$ = 2} ;
shu0code: SHU0 DOT shu0inst | tran0Instr | shu0setcondInstr;
shu1code: SHU1 DOT shu1inst | tran1Instr | shu1setcondInstr;
shu2code: SHU2 DOT shu2inst | tran2Instr | shu2setcondInstr;

shu0inst: ucpshuexp ASSIGNTO shu0dest {
	
	flagsort = (flags[SPPF] << 2) | (flags[IPPF] << 1) | flags[APPF2];
	sia = OPERAND(Imm, flagsort, FlagS, FlagE);
	
        flags.reset();
        
  switch ($3) {
  case 0://shu0
     switch ($1) {
    case 0: //+=
      ADDOPERAND(Opc, UCPM::SHU0Ind_SetTBToSHU_0, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
      if (ipath == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ipath));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(revt));//tp
      if (imm == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
        Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm));
      break;
    case 1: //=+
      ADDOPERAND(Opc, UCPM::SHU0Ind_SetTBToSHU_1, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
      if (ipath == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ipath));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tp));
      if (imm == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
        Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm));
      break;
    case 2: //tb,no imm
      ADDOPERAND(Opc, UCPM::SHU0Ind_noImmToSHU_0, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
      if (ipath == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ipath));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(revt));//tp
      break;    
    case 3: //tk,no imm
      ADDOPERAND(Opc, UCPM::SHU0Ind_noImmToSHU_1, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
       if (ipath == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ipath));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tp));
      break;
      
    default : break;
    }
    break;
  case 1://BIU
       switch ($1) {
    case 0: //+=
      ADDOPERAND(Opc, UCPM::SHU0Ind_SetTBToBIU_0, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
      if (ipath == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ipath));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(revt));//tp
      if (imm == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
        Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm));
      break;
    case 1: //=+
      ADDOPERAND(Opc, UCPM::SHU0Ind_SetTBToBIU_1, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
      if (ipath == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ipath));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tp));
      if (imm == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
        Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm));
      break;
    case 2: //tb,no imm
      ADDOPERAND(Opc, UCPM::SHU0Ind_noImmToBIU_0, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
      if (ipath == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ipath));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(revt));//tp
      break;    
    case 3: //tk,no imm
      ADDOPERAND(Opc, UCPM::SHU0Ind_noImmToBIU_1, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
       if (ipath == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ipath));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tp));
      break;
      
    default : break;
    }
    break;
  
  case 2://ialut, imact, ifalut, ifmacct
    switch ($1) {
    case 0: //+=
      ADDOPERAND(Opc, UCPM::SHU0Ind_SetTBToMACC_0, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ipath));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(revt));//tp
      if (imm == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
        Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm));
      break;
    case 1: //=+
      ADDOPERAND(Opc, UCPM::SHU0Ind_SetTBToMACC_1, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ipath));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tp));
      if (imm == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
        Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm));
      break;
    case 2: //tb,no imm
      ADDOPERAND(Opc, UCPM::SHU0Ind_noImmToMACC_0, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ipath));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(revt));//tp
      break;    
    case 3: //tk,no imm
      ADDOPERAND(Opc, UCPM::SHU0Ind_noImmToMACC_1, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ipath));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tp));
      break;
      
    default : break;
    }
    break;
  case 3://m[t]
    switch ($1) {
    case 0: //+=
      ADDOPERAND(Opc, UCPM::SHU0Ind_SetTBToM_0, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(md));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(revt));//tp
      if (imm == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
        Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm));
      break;
    case 1: //=+
      ADDOPERAND(Opc, UCPM::SHU0Ind_SetTBToM_1, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(md));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tp));
      if (imm == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
        Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm));
      break;
    case 2: //tb,no imm
      ADDOPERAND(Opc, UCPM::SHU0Ind_noImmToM_0, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(md));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(revt));//tp
      break;    
    case 3: //tk,no imm
      ADDOPERAND(Opc, UCPM::SHU0Ind_noImmToM_1, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(md));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tp));
      break;
      
    default : break;
    }
    break;
    
  case 4://m[s++,i++,a++]
    switch ($1) {
    case 0: //+=
      ADDOPERAND(Opc, UCPM::SHU0Ind_SetTBToMSIA_0, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(revt));//tp
      if (imm == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
        Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(sia));
      break;
    case 1: //=+
      ADDOPERAND(Opc, UCPM::SHU0Ind_SetTBToMSIA_1, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tp));
      if (imm == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
        Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(sia));
      break;
    case 2: //tb,no imm
      ADDOPERAND(Opc, UCPM::SHU0Ind_noImmToMSIA_0, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(revt));//tp
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(sia));
      break;    
    case 3: //tk,no imm
      ADDOPERAND(Opc, UCPM::SHU0Ind_noImmToMSIA_1, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tp));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(sia));
      break;
      
    default : break;
    }
    break;
  default: break;
  }

}
| ucpshuexp2 ASSIGNTO shu0dest {
    
	flagsort = (flags[SPPF] << 2) | (flags[IPPF] << 1) | flags[APPF2];
	sia = OPERAND(Imm, flagsort, FlagS, FlagE);
	
        flags.reset();
        
  switch ($3) {
  case 0://shu0
     switch ($1) {
    case 0: //tb
      ADDOPERAND(Opc, UCPM::SHU0Ind_constTnToSHU_0, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
       if (ipath == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ipath));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(revt));//tp
      break;    
    case 1: //tk
      ADDOPERAND(Opc, UCPM::SHU0Ind_constTnToSHU_1, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
       if (ipath == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ipath));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tp));
      break;
      
    default : break;
    }
    break;
  case 1://BIU
        switch ($1) {
    case 0: //tb
      ADDOPERAND(Opc, UCPM::SHU0Ind_constTnToBIU_0, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
       if (ipath == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ipath));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(revt));//tp
      break;    
    case 1: //tk
      ADDOPERAND(Opc, UCPM::SHU0Ind_constTnToBIU_1, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
       if (ipath == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ipath));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tp));
      break;
      
    default : break;
    }
    break;
  
  case 2://ialut, imact, ifalut, ifmacct
    switch ($1) {
    case 0: //tb
      ADDOPERAND(Opc, UCPM::SHU0Ind_constTnToMACC_0, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ipath));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(revt));//tp
      break;    
    case 1: //tk
      ADDOPERAND(Opc, UCPM::SHU0Ind_constTnToMACC_1, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ipath));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tp));
      break;
      
    default : break;
    }
    break;
    
 case 3://m[t]
     switch ($1) {
    case 0: //tb
      ADDOPERAND(Opc, UCPM::SHU0Ind_constTnToM_0, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(md));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(revt));//tp
      break;    
    case 1: //tk
      ADDOPERAND(Opc, UCPM::SHU0Ind_constTnToM_1, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(md));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tp));
    break;
      default : break;
    }
    break;
    
  case 4://m[s++,i++,a++]
     switch ($1) {
    case 0: //tb
      ADDOPERAND(Opc, UCPM::SHU0Ind_constTnToMSIA_0, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(revt));//tp
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(sia));
      break;    
    case 1: //tk
      ADDOPERAND(Opc, UCPM::SHU0Ind_constTnToMSIA_1, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tp));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(sia));
    break;
  default: break;
  }
  break;
  
 default : break;
    }
  
}
| shuwaitInstr{
            
      ADDOPERAND(Opc, UCPM::SHU0Wait, @$.S, @$.E);
      
      if (imm == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm));
    
};

shu1inst: ucpshuexp ASSIGNTO shu1dest {

	flagsort = (flags[SPPF] << 2) | (flags[IPPF] << 1) | flags[APPF2];
	sia = OPERAND(Imm, flagsort, FlagS, FlagE);
	
        flags.reset();
        
  switch ($3) {
  case 0://shu1
     switch ($1) {
    case 0: //+=
      ADDOPERAND(Opc, UCPM::SHU1Ind_SetTBToSHU_0, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
       if (ipath == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ipath));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(revt));//tp
      if (imm == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
        Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm));
      break;
    case 1: //=+
      ADDOPERAND(Opc, UCPM::SHU1Ind_SetTBToSHU_1, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
      if (ipath == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ipath));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tp));
      if (imm == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
        Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm));
      break;
    case 2: //tb,no imm
      ADDOPERAND(Opc, UCPM::SHU1Ind_noImmToSHU_0, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
      if (ipath == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ipath));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(revt));//tp
      break;    
    case 3: //tk,no imm
      ADDOPERAND(Opc, UCPM::SHU1Ind_noImmToSHU_1, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
      if (ipath == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ipath));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tp));
      break;
      
    default : break;
    }
    break;
  case 1://BIU
       switch ($1) {
    case 0: //+=
      ADDOPERAND(Opc, UCPM::SHU1Ind_SetTBToBIU_0, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
       if (ipath == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ipath));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(revt));//tp
      if (imm == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
        Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm));
      break;
    case 1: //=+
      ADDOPERAND(Opc, UCPM::SHU1Ind_SetTBToBIU_1, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
      if (ipath == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ipath));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tp));
      if (imm == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
        Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm));
      break;
    case 2: //tb,no imm
      ADDOPERAND(Opc, UCPM::SHU1Ind_noImmToBIU_0, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
      if (ipath == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ipath));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(revt));//tp
      break;    
    case 3: //tk,no imm
      ADDOPERAND(Opc, UCPM::SHU1Ind_noImmToBIU_1, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
      if (ipath == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ipath));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tp));
      break;
      
    default : break;
    }
    break;
  
  case 2://ialut, imact, ifalut, ifmacct
    switch ($1) {
    case 0: //+=
      ADDOPERAND(Opc, UCPM::SHU1Ind_SetTBToMACC_0, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ipath));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(revt));//tp
      if (imm == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
        Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm));
      break;
    case 1: //=+
      ADDOPERAND(Opc, UCPM::SHU1Ind_SetTBToMACC_1, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ipath));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tp));
      if (imm == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
        Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm));
      break;
    case 2: //tb,no imm
      ADDOPERAND(Opc, UCPM::SHU1Ind_noImmToMACC_0, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ipath));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(revt));//tp
      break;    
    case 3: //tk,no imm
      ADDOPERAND(Opc, UCPM::SHU1Ind_noImmToMACC_1, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ipath));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tp));
      break;
      
    default : break;
    }
    break;
    
  case 3://m[t]
    switch ($1) {
    case 0: //+=
      ADDOPERAND(Opc, UCPM::SHU1Ind_SetTBToM_0, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(md));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(revt));//tp
      if (imm == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
        Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm));
      break;
    case 1: //=+
      ADDOPERAND(Opc, UCPM::SHU1Ind_SetTBToM_1, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(md));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tp));
      if (imm == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
        Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm));
      break;
    case 2: //tb,no imm
      ADDOPERAND(Opc, UCPM::SHU1Ind_noImmToM_0, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(md));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(revt));//tp
      break;    
    case 3: //tk,no imm
      ADDOPERAND(Opc, UCPM::SHU1Ind_noImmToM_1, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(md));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tp));
      break;
      
    default : break;
    }
    break;
    
  case 4://m[s++,i++,a++]
    switch ($1) {
    case 0: //+=
      ADDOPERAND(Opc, UCPM::SHU1Ind_SetTBToMSIA_0, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(revt));//tp
      if (imm == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
        Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(sia));
      break;
    case 1: //=+
      ADDOPERAND(Opc, UCPM::SHU1Ind_SetTBToMSIA_1, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tp));
      if (imm == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
        Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(sia));
      break;
    case 2: //tb,no imm
      ADDOPERAND(Opc, UCPM::SHU1Ind_noImmToMSIA_0, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(revt));//tp
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(sia));
      break;    
    case 3: //tk,no imm
      ADDOPERAND(Opc, UCPM::SHU1Ind_noImmToMSIA_1, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tp));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(sia));
      break;
      
    default : break;
    }
    break;
  default: break;
  }

}
| ucpshuexp2 ASSIGNTO shu1dest {

	flagsort = (flags[SPPF] << 2) | (flags[IPPF] << 1) | flags[APPF2];
	sia = OPERAND(Imm, flagsort, FlagS, FlagE);
	
        flags.reset();
        
  switch ($3) {
  case 0://shu1
     switch ($1) {
    case 0: //tb
      ADDOPERAND(Opc, UCPM::SHU1Ind_constTnToSHU_0, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
      if (ipath == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ipath));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(revt));//tp
      break;    
    case 1: //tk
      ADDOPERAND(Opc, UCPM::SHU1Ind_constTnToSHU_1, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
       if (ipath == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ipath));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tp));
      break;
      
    default : break;
    }
    break;
  case 1://BIU
      switch ($1) {
    case 0: //tb
      ADDOPERAND(Opc, UCPM::SHU1Ind_constTnToBIU_0, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
      if (ipath == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ipath));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(revt));//tp
      break;    
    case 1: //tk
      ADDOPERAND(Opc, UCPM::SHU1Ind_constTnToBIU_1, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
       if (ipath == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ipath));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tp));
      break;
      
    default : break;
    }
    break;
  
  case 2://ialut, imact, ifalut, ifmacct
    switch ($1) {
    case 0: //tb
      ADDOPERAND(Opc, UCPM::SHU1Ind_constTnToMACC_0, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ipath));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(revt));//tp
      break;    
    case 1: //tk
      ADDOPERAND(Opc, UCPM::SHU1Ind_constTnToMACC_1, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ipath));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tp));
      break;
      
    default : break;
    }
    break;
case 3://m[t]
     switch ($1) {
    case 0: //tb
      ADDOPERAND(Opc, UCPM::SHU1Ind_constTnToM_0, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(md));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(revt));//tp
      break;    
    case 1: //tk
      ADDOPERAND(Opc, UCPM::SHU1Ind_constTnToM_1, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(md));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tp));
    break;
   default : break;
    }
    break;
  case 4://m[s++,i++,a++]
     switch ($1) {
    case 0: //tb
      ADDOPERAND(Opc, UCPM::SHU1Ind_constTnToMSIA_0, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(revt));//tp
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(sia));
      break;    
    case 1: //tk
      ADDOPERAND(Opc, UCPM::SHU1Ind_constTnToMSIA_1, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tp));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(sia));
    break;
  default: break;
  }
  break;
 default : break;
    }
    
}
| shuwaitInstr{
            
      ADDOPERAND(Opc, UCPM::SHU1Wait, @$.S, @$.E);
      
      if (imm == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm));
    
};

shu2inst: ucpshuexp ASSIGNTO shu2dest {
      
	flagsort = (flags[SPPF] << 2) | (flags[IPPF] << 1) | flags[APPF2];
	sia = OPERAND(Imm, flagsort, FlagS, FlagE);
	
        flags.reset();
        
  switch ($3) {
  case 0://shu2
     switch ($1) {
    case 0: //+=
      ADDOPERAND(Opc, UCPM::SHU2Ind_SetTBToSHU_0, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
       if (ipath == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ipath));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(revt));//tp
      if (imm == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
        Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm));
      break;
    case 1: //=+
      ADDOPERAND(Opc, UCPM::SHU2Ind_SetTBToSHU_1, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
       if (ipath == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ipath));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tp));
      if (imm == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
        Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm));
      break;
    case 2: //tb,no imm
      ADDOPERAND(Opc, UCPM::SHU2Ind_noImmToSHU_0, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
       if (ipath == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ipath));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(revt));//tp
      break;    
    case 3: //tk,no imm
      ADDOPERAND(Opc, UCPM::SHU2Ind_noImmToSHU_1, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
       if (ipath == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ipath));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tp));
      break;
      
    default : break;
    }
    break;
  case 1://BIU
     switch ($1) {
    case 0: //+=
      ADDOPERAND(Opc, UCPM::SHU2Ind_SetTBToBIU_0, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
       if (ipath == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ipath));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(revt));//tp
      if (imm == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
        Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm));
      break;
    case 1: //=+
      ADDOPERAND(Opc, UCPM::SHU2Ind_SetTBToBIU_1, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
       if (ipath == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ipath));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tp));
      if (imm == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
        Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm));
      break;
    case 2: //tb,no imm
      ADDOPERAND(Opc, UCPM::SHU2Ind_noImmToBIU_0, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
       if (ipath == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ipath));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(revt));//tp
      break;    
    case 3: //tk,no imm
      ADDOPERAND(Opc, UCPM::SHU2Ind_noImmToBIU_1, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
       if (ipath == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ipath));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tp));
      break;
      
    default : break;
    }
    
    break;
  
  case 2://ialut, imact, ifalut, ifmacct
    switch ($1) {
    case 0: //+=
      ADDOPERAND(Opc, UCPM::SHU2Ind_SetTBToMACC_0, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ipath));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(revt));//tp
      if (imm == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
        Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm));
      break;
    case 1: //=+
      ADDOPERAND(Opc, UCPM::SHU2Ind_SetTBToMACC_1, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ipath));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tp));
      if (imm == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
        Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm));
      break;
    case 2: //tb,no imm
      ADDOPERAND(Opc, UCPM::SHU2Ind_noImmToMACC_0, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ipath));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(revt));//tp
      break;    
    case 3: //tk,no imm
      ADDOPERAND(Opc, UCPM::SHU2Ind_noImmToMACC_1, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ipath));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tp));
      break;
      
    default : break;
    }
    break;
case 3://m[t]
    switch ($1) {
    case 0: //+=
      ADDOPERAND(Opc, UCPM::SHU2Ind_SetTBToM_0, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(md));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(revt));//tp
      if (imm == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
        Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm));
      break;
    case 1: //=+
      ADDOPERAND(Opc, UCPM::SHU2Ind_SetTBToM_1, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(md));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tp));
      if (imm == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
        Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm));
      break;
    case 2: //tb,no imm
      ADDOPERAND(Opc, UCPM::SHU2Ind_noImmToM_0, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(md));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(revt));//tp
      break;    
    case 3: //tk,no imm
      ADDOPERAND(Opc, UCPM::SHU2Ind_noImmToM_1, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(md));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tp));
      break;
      
    default : break;
    }
    break;
    
  case 4://m[s++,i++,a++]
    switch ($1) {
    case 0: //+=
      ADDOPERAND(Opc, UCPM::SHU2Ind_SetTBToMSIA_0, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(revt));//tp
      if (imm == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
        Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(sia));
      break;
    case 1: //=+
      ADDOPERAND(Opc, UCPM::SHU2Ind_SetTBToMSIA_1, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tp));
      if (imm == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
        Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(sia));
      break;
    case 2: //tb,no imm
      ADDOPERAND(Opc, UCPM::SHU2Ind_noImmToMSIA_0, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(revt));//tp
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(sia));
      break;    
    case 3: //tk,no imm
      ADDOPERAND(Opc, UCPM::SHU2Ind_noImmToMSIA_1, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tp));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(sia));
      break;
      
    default : break;
    }
    break;
    
  default: break;
  }

}
| ucpshuexp2 ASSIGNTO shu2dest {
    
	flagsort = (flags[SPPF] << 2) | (flags[IPPF] << 1) | flags[APPF2];
	sia = OPERAND(Imm, flagsort, FlagS, FlagE);
	
        flags.reset();
        
  switch ($3) {
  case 0://shu2
     switch ($1) {
    case 0: //tb
      ADDOPERAND(Opc, UCPM::SHU2Ind_constTnToSHU_0, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
       if (ipath == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ipath));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(revt));//tp
      break;    
    case 1: //tk
      ADDOPERAND(Opc, UCPM::SHU2Ind_constTnToSHU_1, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
       if (ipath == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ipath));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tp));
      break;
      
    default : break;
    }
    break;
  case 1://biu
    switch ($1) {
    case 0: //tb
      ADDOPERAND(Opc, UCPM::SHU2Ind_constTnToBIU_0, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
       if (ipath == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ipath));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(revt));//tp
      break;    
    case 1: //tk
      ADDOPERAND(Opc, UCPM::SHU2Ind_constTnToBIU_1, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
       if (ipath == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ipath));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tp));
      break;
      
    default : break;
    }
    
    break;
  
  case 2://ialut, imact, ifalut, ifmacct
    switch ($1) {
    case 0: //tb
      ADDOPERAND(Opc, UCPM::SHU2Ind_constTnToMACC_0, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ipath));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(revt));//tp
      break;    
    case 1: //tk
      ADDOPERAND(Opc, UCPM::SHU2Ind_constTnToMACC_1, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ipath));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tp));
      break;
      
    default : break;
    }
    break;
 case 3://m[t]
     switch ($1) {
    case 0: //tb
      ADDOPERAND(Opc, UCPM::SHU2Ind_constTnToM_0, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(md));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(revt));//tp
      break;    
    case 1: //tk
      ADDOPERAND(Opc, UCPM::SHU2Ind_constTnToM_1, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(md));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tp));
    break;
   default : break;
    }
    break;
  case 4://m[s++,i++,a++]
     switch ($1) {
    case 0: //tb
      ADDOPERAND(Opc, UCPM::SHU2Ind_constTnToMSIA_0, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(revt));//tp
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(sia));
      break;    
    case 1: //tk
      ADDOPERAND(Opc, UCPM::SHU2Ind_constTnToMSIA_1, @$.S, @$.E); 
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tp));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(sia));
    break;
  default: break;
  }
  break;
 default : break;
    }
    
}
| shuwaitInstr{
            
      ADDOPERAND(Opc, UCPM::SHU2Wait, @$.S, @$.E);
      
      if (imm == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm));
    
};
//tn:t0-t3
ucpshuexp: LBRACE ucpshusrcTm COMMA t RBRACE IND ucpindclause {$$ = $7}; 
ucpshusrcTm: t;
/*ucpshusrcTn: t {$$ = 1;}| 
             constt {$$ = 2;};*/
ucpindclause: ucpindtkclause | ucpindtbclause ;
ucpindtkclause: ucpshusrcTk _flag TB ACC2 IMM5 flag_ {$$ = 1; imm = OPERAND(Imm, $5, @5.S, @5.E);}//"=+"case
|               ucpshusrcTk {$$ = 3;};
ucpindtkclause: ucpshusrcTk _flag error flag_ { llvmerror(&@2, "IND Tk must be followed by (TB =+ Imm)"); YYABORT;};
ucpindtbclause: TB _flag TB ACC1 IMM5 flag_ {$$ = 0; imm = OPERAND(Imm, $5, @5.S, @5.E); 
		unsigned treg = MRI->getRegClass(UCPMReg::TPortRegClassID).getRegister(0);//tp is unused
		revt = OPERAND(Reg, treg, @$.S, @$.E)}//"+="case
|            	TB {$$ = 2; unsigned treg = MRI->getRegClass(UCPMReg::TPortRegClassID).getRegister(0);//tp is unused
                revt = OPERAND(Reg, treg, @$.S, @$.E)};
ucpindtbclause: TB _flag error flag_ { llvmerror(&@2, "IND TB must be followed by (TB += Imm)"); YYABORT;};
ucpshusrcTk: t {$$ = 1;}| 
             constt {$$ = 2;};

//tn:TSQ/TBB/...
ucpshuexp2: LBRACE ucpshusrcTm COMMA constt RBRACE IND ucpindclause2 {$$ = $7}; 
ucpindclause2: ucpindtkclause2 | ucpindtbclause2 ;
ucpindtkclause2: ucpshusrcTk {$$ = 1;};
ucpindtbclause2: TB {$$ = 0; unsigned treg = MRI->getRegClass(UCPMReg::TPortRegClassID).getRegister(0);//tp is unused
                revt = OPERAND(Reg, treg, @$.S, @$.E)};

//dcx
shu0dest: biu0t | biu2t | maccdestp | sshu0t | mindexn | mindexsia;
shu1dest: biu0t | s1biu1t | maccdestp | sshu1t | mindexn | mindexsia;
shu2dest: s2biu1t | biu2t | maccdestp | sshu2t | mindexn | mindexsia;

mindexsia: M LBRACKET siaflags RBRACKET {$$=4;}| M {$$=4;};
//shu0dest: maccdestp {
//               /*if(ipath->getImm() >= 3) {
//                 llvmerror(&@1, "SHU to Macc must use port 'I0 -I2'"); 
//                 YYABORT;
//               }*/
//             };
             
maccdestp: maccdest LPAREN IPATH RPAREN {$$ = $1; ipath = OPERAND(Imm, $3, @3.S, @3.E);};
maccdest: ialut | imact | ifalut | ifmact ;



// dcx
tran0Instr: tran0clause ASSIGNTO shu0dest {

      	if(flags[BF])
            flagsort = 0x1;
        else if(flags[SF])
            flagsort = 0x2;
        else 
            flagsort = 0x4;  
        f = OPERAND(Imm, flagsort, FlagS, FlagE);
    
    
	flagsort = (flags[SPPF] << 2) | (flags[IPPF] << 1) | flags[APPF2];
	sia = OPERAND(Imm, flagsort, FlagS, FlagE);
	
        flags.reset();
        
 switch ($3) {
   case 0://to shu0
        ADDOPERAND(Opc, UCPM::SHU0ByteBitToSHU, @$.S, @$.E); 
        Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));//unit'T
	if (ipath == NULL)
	  ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
	else
	  Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ipath));
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(opc));
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(f));
        break;
   case 1://to biu
        ADDOPERAND(Opc, UCPM::SHU0ByteBitToBIU, @$.S, @$.E); 
        Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));//unit'T
	if (ipath == NULL)
	  ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
	else
	  Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ipath));
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(opc));
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(f));
        break;
   case 2://to macc
        ADDOPERAND(Opc, UCPM::SHU0ByteBitToMACC, @$.S, @$.E); 
        Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));//unit'T
	if (ipath == NULL)
	  ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
	else
	  Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ipath));
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(opc));
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(f));
        break;
   case 3://to m[t]
        ADDOPERAND(Opc, UCPM::SHU0ByteBitToM, @$.S, @$.E); 
        Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(md));
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(opc));
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(f));
        break;
   case 4://to m[s++,i++,a++]
        ADDOPERAND(Opc, UCPM::SHU0ByteBitToMSIA, @$.S, @$.E); 
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(opc));
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(f));
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(sia));
        break;
        
    default: break;
 }
    
};
tran1Instr: tran1clause ASSIGNTO shu1dest {
 
      	if(flags[BF])
            flagsort = 0x1;
        else if(flags[SF])
            flagsort = 0x2;
        else 
            flagsort = 0x4;  
        f = OPERAND(Imm, flagsort, FlagS, FlagE);
    
    
	flagsort = (flags[SPPF] << 2) | (flags[IPPF] << 1) | flags[APPF2];
	sia = OPERAND(Imm, flagsort, FlagS, FlagE);
	
        flags.reset();
        
 switch ($3) {
   case 0://to shu0
        ADDOPERAND(Opc, UCPM::SHU1ByteBitToSHU, @$.S, @$.E); 
        Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));//unit'T
	if (ipath == NULL)
	  ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
	else
	  Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ipath));
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(opc));
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(f));
        break;
   case 1://to biu
        ADDOPERAND(Opc, UCPM::SHU1ByteBitToBIU, @$.S, @$.E); 
        Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));//unit'T
	if (ipath == NULL)
	  ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
	else
	  Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ipath));
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(opc));
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(f));
        break;
   case 2://to macc
        ADDOPERAND(Opc, UCPM::SHU1ByteBitToMACC, @$.S, @$.E); 
        Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));//unit'T
	if (ipath == NULL)
	  ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
	else
	  Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ipath));
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(opc));
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(f));
        break;
   case 3://to m[t]
        ADDOPERAND(Opc, UCPM::SHU1ByteBitToM, @$.S, @$.E); 
        Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(md));
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(opc));
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(f));
        break;
   case 4://to m[s++,i++,a++]
        ADDOPERAND(Opc, UCPM::SHU1ByteBitToMSIA, @$.S, @$.E); 
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(opc));
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(f));
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(sia));
        break;
        
    default: break;
 }
    
};
tran2Instr: tran2clause ASSIGNTO shu2dest {
 
      	if(flags[BF])
            flagsort = 0x1;
        else if(flags[SF])
            flagsort = 0x2;
        else 
            flagsort = 0x4;  
        f = OPERAND(Imm, flagsort, FlagS, FlagE);
    
    
	flagsort = (flags[SPPF] << 2) | (flags[IPPF] << 1) | flags[APPF2];
	sia = OPERAND(Imm, flagsort, FlagS, FlagE);
	
        flags.reset();
        
 switch ($3) {
   case 0://to shu0
        ADDOPERAND(Opc, UCPM::SHU2ByteBitToSHU, @$.S, @$.E); 
        Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));//unit'T
	if (ipath == NULL)
	  ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
	else
	  Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ipath));
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(opc));
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(f));
        break;
   case 1://to biu
        ADDOPERAND(Opc, UCPM::SHU2ByteBitToBIU, @$.S, @$.E); 
        Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));//unit'T
	if (ipath == NULL)
	  ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
	else
	  Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ipath));
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(opc));
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(f));
        break;
   case 2://to macc
        ADDOPERAND(Opc, UCPM::SHU2ByteBitToMACC, @$.S, @$.E); 
        Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));//unit'T
	if (ipath == NULL)
	  ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
	else
	  Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ipath));
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(opc));
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(f));
        break;
   case 3://to m[t]
        ADDOPERAND(Opc, UCPM::SHU2ByteBitToM, @$.S, @$.E); 
        Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(md));
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(opc));
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(f));
        break;
   case 4://to m[s++,i++,a++]
        ADDOPERAND(Opc, UCPM::SHU2ByteBitToMSIA, @$.S, @$.E); 
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(opc));
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(f));
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(sia));
        break;
        
    default: break;
 }
    
};
tran0clause:  bit0clause {opc = OPERAND(Reg, UCPMReg::BIT, @$.S, @$.E);} |
               byte0clause {opc = OPERAND(Reg, UCPMReg::BYTE, @$.S, @$.E);} ;
bit0clause:  BIT SHU0 DOT t _flag shuflag flag_ | BIT SHU0 DOT t ; 
byte0clause: BYTE SHU0 DOT t _flag shuflag flag_ | BYTE SHU0 DOT t ;
 
tran1clause:  bit1clause {opc = OPERAND(Reg, UCPMReg::BIT, @$.S, @$.E);} |
               byte1clause {opc = OPERAND(Reg, UCPMReg::BYTE, @$.S, @$.E);} ;
bit1clause:  BIT SHU1 DOT t _flag shuflag flag_ | BIT SHU1 DOT t ; 
byte1clause: BYTE SHU1 DOT t _flag shuflag flag_ | BYTE SHU1 DOT t ;

tran2clause:  bit2clause {opc = OPERAND(Reg, UCPMReg::BIT, @$.S, @$.E);} |
               byte2clause {opc = OPERAND(Reg, UCPMReg::BYTE, @$.S, @$.E);} ;
bit2clause:  BIT SHU2 DOT t _flag shuflag flag_ | BIT SHU2 DOT t ; 
byte2clause: BYTE SHU2 DOT t _flag shuflag flag_ | BYTE SHU2 DOT t ;

shuwaitInstr: WAIT IMM5 {imm = OPERAND(Imm, $2, @2.S, @2.E);};

shu0setcondInstr: shu0cond{   
	    
	    ADDOPERAND(Opc, UCPM::SHU0SetCond, @$.S, @$.E);
	    
	    if (imm == NULL)
	      ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
	    else
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm));
	    if (imm1 == NULL)
	      ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
	    else
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm1));   
	  };
shu1setcondInstr: shu1cond{   
	    
	    ADDOPERAND(Opc, UCPM::SHU1SetCond, @$.S, @$.E);
	    
	    if (imm == NULL)
	      ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
	    else
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm));
	    if (imm1 == NULL)
	      ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
	    else
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm1));   
	  };
shu2setcondInstr: shu2cond{   
	    
	    ADDOPERAND(Opc, UCPM::SHU2SetCond, @$.S, @$.E);
	    
	    if (imm == NULL)
	      ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
	    else
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm));
	    if (imm1 == NULL)
	      ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
	    else
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm1));   
	  };

shu0cond: IMMSYM DOT IMM5 ASSIGNTO SHU0 DOT SETCOND LBRACKET IMM5 RBRACKET 
	  {imm = OPERAND(Imm, $3, @3.S, @3.E);imm1 = OPERAND(Imm, $9, @9.S, @9.E);};      
shu1cond: IMMSYM DOT IMM5 ASSIGNTO SHU1 DOT SETCOND LBRACKET IMM5 RBRACKET 
	  {imm = OPERAND(Imm, $3, @3.S, @3.E);imm1 = OPERAND(Imm, $9, @9.S, @9.E);};
shu2cond: IMMSYM DOT IMM5 ASSIGNTO SHU2 DOT SETCOND LBRACKET IMM5 RBRACKET 
	  {imm = OPERAND(Imm, $3, @3.S, @3.E);imm1 = OPERAND(Imm, $9, @9.S, @9.E);};
 
// end shu -------------------------------------------------- 

ialuslot: IALU DOT ialutodest ;
ialutodest: ialuasclause ASSIGNTO ialudest {
  flagsort = (~(flags[UF]) << 5) | (flags[BF] << 4) | (flags[SF] << 3) | (flags[TF] << 2) | (flags[CIF] << 1) | flags[FF];
  flags.reset();
  f = OPERAND(Imm, flagsort, FlagS, FlagE);
  switch ($3) {
    case 1://to shu
      ADDOPERAND(Opc, UCPM::IALUASToSHU, @$.S, @$.E);
      break;
    case 2://to macc
      ADDOPERAND(Opc, UCPM::IALUASToMACC, @$.S, @$.E);
      break;
    case 3://to biu
      ADDOPERAND(Opc, UCPM::IALUASToBIU, @$.S, @$.E);
      break;
    default:
      break;
  }
  Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit3));
  Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));//unit'T
  Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(opc));//ADD or SUB
  Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
  Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
  Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(f));
}
| ialulogicclause ASSIGNTO ialudest {
  switch ($3) {
    case 1://to shu
      ADDOPERAND(Opc, UCPM::IALULogicToSHU, @$.S, @$.E);
      break;
    case 2://to macc
      ADDOPERAND(Opc, UCPM::IALULogicToMACC, @$.S, @$.E);
      break;
    case 3://to biu
      ADDOPERAND(Opc, UCPM::IALULogicToBIU, @$.S, @$.E);
      break;
    default:
      break;
  }
  Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit3));
  Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));//unit'T
  Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(opc));//AND OR XOR
  Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
  Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
};
ialuasclause: iaddclause {opc = OPERAND(Reg, UCPMReg::f_IADD, @$.S, @$.E);} |
              isubclause {opc = OPERAND(Reg, UCPMReg::f_ISUB, @$.S, @$.E);};
iaddclause: addexp _flag ialuflags flag_ /*{
              //check flags type
              unsigned long temp, tmpflag;
              tmpflag = flags.to_ulong();
              OS<<"tmpflag: "<<tmpflag<<"\n";
              temp = tmpflag | (1 << TF) | (1 << CIF) | (1 << FF) | (1 << BF) | (1 << SF) | (1 << UF)
                 - (1 << TF) | (1 << CIF) | (1 << FF) | (1 << BF) | (1 << SF) | (1 << UF);
              OS<<"temp: "<<temp<<"\n";
              if(temp != 0) {
                OS<<"error flag: "<<temp<<"\n";
                llvmerror(&@3, "Invalid flag for \"Tm + Tn\". Available flags for IALU are T, CI, F, B/S, U"); YYABORT; 
              }
            }*/
          | addexp;
isubclause: subexp _flag ialuflags flag_ | subexp ;
addexp: t ADD t ;
subexp: t SUB t ;
ialulogicclause: iandclause {opc = OPERAND(Reg, UCPMReg::m_AND, @$.S, @$.E);} | 
                 iorclause {opc = OPERAND(Reg, UCPMReg::m_OR, @$.S, @$.E);} | 
                 ixorclause {opc = OPERAND(Reg, UCPMReg::m_XOR, @$.S, @$.E);} ;
iandclause: t AND t ;
iorclause : t OR t ;
ixorclause : t XOR t ;


ialudest: shut {$$ = 1;} | maccdest {$$ = 2;} | biut {$$ = 3;} ;


shut: shu0t | shu1t | shu2t ;

ifaluslot: ifalutodest ;
ifalutodest: ifalucomclause ASSIGNTO ifaludest {
  switch ($3) {
    case 1://to shu
      ADDOPERAND(Opc, UCPM::IFALUComToSHU, @$.S, @$.E); 
      break;
    case 2://to macc
      ADDOPERAND(Opc, UCPM::IFALUComToMACC, @$.S, @$.E);
      break;
    case 3://to biu
      ADDOPERAND(Opc, UCPM::IFALUComToBIU, @$.S, @$.E);
      break;
    default:
      break;
  }
  Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit3));
  Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));//unit'T
  Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(opc));//AND OR XOR
  Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
  Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
};/*
| ifaluurytran ASSIGNTO ifaludest {
  
};
ifaluurytran: ifinttosingle | ifdoutosingle | ifinttodouble | ifsintodouble | ifsintoint | ifdoutoint ;
ifinttosingle: 
ifdoutosingle:
ifinttodouble:
ifsintodouble:
ifsintoint:
ifdoutoint:*/

ifalucomclause: ifm_FEQUclause {opc = OPERAND(Reg, UCPMReg::m_FEQU, @$.S, @$.E);} |
                ifm_FNEQclause {opc = OPERAND(Reg, UCPMReg::m_FNEQ, @$.S, @$.E);} |
                ifm_FLTclause  {opc = OPERAND(Reg, UCPMReg::m_FLT, @$.S, @$.E);}  |
                ifm_FNSTclause {opc = OPERAND(Reg, UCPMReg::m_FNST, @$.S, @$.E);} |
                ifm_FSTclause  {opc = OPERAND(Reg, UCPMReg::m_FST, @$.S, @$.E);}  |
                ifm_FNLTclause {opc = OPERAND(Reg, UCPMReg::m_FNLT, @$.S, @$.E);} ;
ifm_FEQUclause: t EQU t ;
ifm_FNEQclause: t NEQ t ;
ifm_FLTclause: t LT t ;
ifm_FNSTclause: t NST t ;
ifm_FSTclause: t ST t ;
ifm_FNLTclause: t NLT t ;
ifaludest: ialudest ;

imacslot: imacinst ;
imacinst: imacclause ASSIGNTO imacdest {
  flagsort = (flags[LF] << 6) | (flags[TF] << 5) | (~flags[UF] << 4) | (flags[FF] << 3) | (flags[PF] << 2);
  //C or SEND flag
  if(flags[CF] | flags[SENDF])
    flagsort |= 0x1;
  f = OPERAND(Imm, flagsort, FlagS, FlagE);
  //ff
  if(flags[BF] | flags[RF])
    flagsort = 0x0;
  else if(flags[SF] | flags[IF])
    flagsort = 0x1;
  else {
    llvmerror(&@1, "B/R/S/I flag error!"); 
    YYABORT;
  }
  ff = OPERAND(Imm, flagsort, FlagS, FlagE);
  //SHIFT flag
  if(flags[S0F])
    flagsort = 0x0;
  else if(flags[S1F])
    flagsort = 0x1;
  else if(flags[S2F])
    flagsort = 0x2;
  else if(flags[S3F])
    flagsort = 0x3;
  else
    flagsort = 0x0;
  shift = OPERAND(Imm, flagsort, FlagS, FlagE);
  flags.reset();
  switch ($1) {
    case 0://imulreal
      switch ($3) {
        case 1://to shu
        ADDOPERAND(Opc, UCPM::IMulRealToSHU, @$.S, @$.E); 
        break;
      case 2://to macc
        ADDOPERAND(Opc, UCPM::IMulRealToMACC, @$.S, @$.E);
        break;
      case 3://to biu
        ADDOPERAND(Opc, UCPM::IMulRealToBIU, @$.S, @$.E);
        break;
      }
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit3));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));//unit'T
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(opc));//IMUL
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(revt));//tp
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(f));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ff));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(shift));
      //***
      break;
    case 1://imulcomp
      switch ($3) {
        case 1://to shu
        ADDOPERAND(Opc, UCPM::IMulCompToSHU, @$.S, @$.E); 
        break;
      case 2://to macc
        ADDOPERAND(Opc, UCPM::IMulCompToMACC, @$.S, @$.E);
        break;
      case 3://to biu
        ADDOPERAND(Opc, UCPM::IMulCompToBIU, @$.S, @$.E);
        break;
      }
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit3));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));//unit'T
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(opc));//IMUL
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(revt));//tp
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(f));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ff));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(shift));
      break;

    default:
      break;
  }
};
imacclause: imulreal {
              $$ = 0; 
              opc = OPERAND(Reg, UCPMReg::IMUL, @$.S, @$.E); 
              unsigned treg = MRI->getRegClass(UCPMReg::TPortRegClassID).getRegister(0);//tp is unused
              revt = OPERAND(Reg, treg, @$.S, @$.E);
            } |
            imulcomp {$$ = 1; opc = OPERAND(Reg, UCPMReg::IMUL, @$.S, @$.E);};
imulreal: imulexp _flag imacrealflag flag_ _flag imacflags flag_ | 
          imulexp _flag imacrealflag flag_ ;
imulcomp: imulexp _flag imaccompflag flag_ _flag imacflags flag_ | 
          imulexp _flag imaccompflag flag_ ;
imulexp: t MUL t ;
imacdest: ialudest;


//ducx start ifmac -----------------------------------------------------
ifmacslot: IFMAC DOT ifmacinst ;
ifmacinst: ifmacclause ASSIGNTO ifmacdest {

    flagsort = flags[TF];
    f = OPERAND(Imm, flagsort, FlagS, FlagE);
    
   	flagsort = (flags[APPF2] << 2) | (flags[SPPF] << 1) | flags[IPPF];
	sia = OPERAND(Imm, flagsort, FlagS, FlagE);
        flags.reset();
    
 switch ($3)
 {
     case 0://to shu
      ADDOPERAND(Opc, UCPM::IFMACToSHU, @$.S, @$.E);
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit3));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(opc));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(f));
      break;
      
    case 1://to macc
      ADDOPERAND(Opc, UCPM::IFMACToMACC, @$.S, @$.E);
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit3));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(opc));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(f));
      break;
      
    case 2://to biu
      ADDOPERAND(Opc, UCPM::IFMACToBIU, @$.S, @$.E);
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit3));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(opc));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(f));
      break;
      
    case 3://to m[t]
      ADDOPERAND(Opc, UCPM::IFMACToM, @$.S, @$.E);
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(md));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(opc));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(f));
      break;
      
    case 4://to m[s++/i++/a++]
        
      ADDOPERAND(Opc, UCPM::IFMACToMSIA, @$.S, @$.E);
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(opc));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(f));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(sia));
      break;
      
    default:
      break;
 
 }
 
};

ifmacclause: fmul {opc = OPERAND(Reg, UCPMReg::FMUL, @$.S, @$.E); } |
            ifaddclause {opc = OPERAND(Reg, UCPMReg::FADD, @$.S, @$.E);} |
            ifsubclause {opc = OPERAND(Reg, UCPMReg::FSUB, @$.S, @$.E);};
            
fmul: imulexp _flag FLOAT flag_ _flag ifmacTflag flag_ | 
      imulexp _flag FLOAT flag_ ;
      
ifaddclause: addexp _flag ifmacTflag flag_ | addexp ;
ifsubclause: subexp _flag ifmacTflag flag_ | subexp ;
              
ifmacdest: shut {$$ = 0;} | maccdest {$$ = 1;} | biut {$$ = 2;}  | mindexn | mindexsia;

// ducx end ifmac --------------------------------------------------



// ducx start biu --------------------------------------------------
biuslot: biu0code {$$ = 0;} | biu1code {$$ = 1;} | biu2code {$$ = 2;} ;
biu0code: BIU0 DOT biu0inst | setcond0Instr | imm0Instr;
biu1code: BIU1 DOT biu1inst | setcond1Instr | imm1Instr;
biu2code: BIU2 DOT biu2inst | setcond2Instr | imm2Instr;

biu0inst: ldselect ASSIGNTO biu0dest {
    

    flagsort = (flags[MF] << 2) | (flags[APPF] << 1) | flags[BRF];
    f = OPERAND(Imm, flagsort, FlagS, FlagE);
    
    switch ($3) {
    
        case 0://to biu0
	  switch ($1) {
	    case 0://lddis
    
	      //qlh flag
	      if(flags[LF])
		  flagsort = 0x0;
	      else if(flags[QHF])
		  flagsort = 0x1;
	      else if(flags[QLF])
		  flagsort = 0x2;
	      qlh = OPERAND(Imm, flagsort, FlagS, FlagE);
	      flags.reset();
        
	      ADDOPERAND(Opc, UCPM::BIU0disLdToBIU, @$.S, @$.E); 
              Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));//unit'T
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(f));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(qlh));
            break;
        
            case 1://ldstep
    
	      flags.reset();

	      ADDOPERAND(Opc, UCPM::BIU0ComLdToBIU, @$.S, @$.E); 
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));//unit'T
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(f));
	    break;

	  default:
	    break;
       }
       break;
       
	case 1://to shu
          switch ($1) {
	    case 0://lddis
    
	      //qlh flag
	      if(flags[LF])
		  flagsort = 0x0;
	      else if(flags[QHF])
		  flagsort = 0x1;
	      else if(flags[QLF])
		  flagsort = 0x2;
	      qlh = OPERAND(Imm, flagsort, FlagS, FlagE);
	      flags.reset();
        
	      ADDOPERAND(Opc, UCPM::BIU0disLdToSHU, @$.S, @$.E); 
              Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));//unit'T
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(f));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(qlh));
            break;
        
            case 1://ldstep
    
	      flags.reset();

	      ADDOPERAND(Opc, UCPM::BIU0ComLdToSHU, @$.S, @$.E); 
    
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));//unit'T
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(f));

	    break;

	  default:
	    break;
       }
       break;
        
        case 2://to macc
          switch ($1) {
	    case 0://lddis
    
	      //qlh flag
	      if(flags[LF])
		  flagsort = 0x0;
	      else if(flags[QHF])
		  flagsort = 0x1;
	      else if(flags[QLF])
		  flagsort = 0x2;
	      qlh = OPERAND(Imm, flagsort, FlagS, FlagE);
	      flags.reset();
        
	      ADDOPERAND(Opc, UCPM::BIU0disLdToMACC, @$.S, @$.E); 
              Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));//unit'T
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(f));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(qlh));
            break;
        
            case 1://ldstep
    
	      flags.reset();

	      ADDOPERAND(Opc, UCPM::BIU0ComLdToMACC, @$.S, @$.E); 
    
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));//unit'T
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(f));

	    break;

	  default:
	    break;
       }
       break;
       
        case 3://to Wx.m[t]
       
	     if(flags[W0F])
		flagsort = 0x0;
	     else if(flags[W2F])
		flagsort = 0x1;
	      ff = OPERAND(Imm, flagsort, FlagS, FlagE);
	      
	  switch ($1) {
	    case 0://lddis
    
	      //qlh flag
	      if(flags[LF])
		  flagsort = 0x0;
	      else if(flags[QHF])
		  flagsort = 0x1;
	      else if(flags[QLF])
		  flagsort = 0x2;
	      qlh = OPERAND(Imm, flagsort, FlagS, FlagE);
	      
	      flags.reset();
        
	      ADDOPERAND(Opc, UCPM::BIU0disLdToM, @$.S, @$.E); 
              Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(md));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(f));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(qlh));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ff));
            break;
        
            case 1://ldstep
    
	      flags.reset();

	      ADDOPERAND(Opc, UCPM::BIU0ComLdToM, @$.S, @$.E); 
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(md));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(f));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ff));
	    break;

	  default:
	    break;
       }
       break;
       
         case 4://to Wx.m[s++,i++,a++]
       
	     if(flags[W0F])
		flagsort = 0x0;
	     else if(flags[W2F])
		flagsort = 0x1;
	      ff = OPERAND(Imm, flagsort, FlagS, FlagE);
	      
	     flagsort = (flags[APPF2] << 2) | (flags[SPPF] << 1) | flags[IPPF];
		sia = OPERAND(Imm, flagsort, FlagS, FlagE);
	
	  switch ($1) {
	    case 0://lddis
    
	      //qlh flag
	      if(flags[LF])
		  flagsort = 0x0;
	      else if(flags[QHF])
		  flagsort = 0x1;
	      else if(flags[QLF])
		  flagsort = 0x2;
	      qlh = OPERAND(Imm, flagsort, FlagS, FlagE);
	      
	      flags.reset();
        
	      ADDOPERAND(Opc, UCPM::BIU0disLdToMSIA, @$.S, @$.E); 
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(f));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(qlh));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ff));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(sia));
            break;
        
            case 1://ldstep
    
	      flags.reset();

	      ADDOPERAND(Opc, UCPM::BIU0ComLdToMSIA, @$.S, @$.E); 
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(f));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ff));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(sia));
	    break;

	  default:
	    break;
       }
       break;
       
    default:
      break;
    }
   

}
| stinst{
      
	flagsort = (flags[MF] << 2) | (flags[APPF] << 1) | flags[BRF];
	f = OPERAND(Imm, flagsort, FlagS, FlagE);
     
        //qlh flag
        if(flags[LF])
            flagsort = 0x0;
        else if(flags[HF])
            flagsort = 0x1;
        else if(flags[QF])
            flagsort = 0x2;
        else
            flagsort = 0x3;
        qlh = OPERAND(Imm, flagsort, FlagS, FlagE);
        flags.reset();
        
      ADDOPERAND(Opc, UCPM::BIU0St, @$.S, @$.E); 
        
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(f));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(qlh));      
  
}
| binInstr{
        
      ADDOPERAND(Opc, UCPM::BIU0Bin, @$.S, @$.E);

      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(opc));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tp));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      if (imm2 == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm2));
      if (imm == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm));
      if (imm1 == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm1));
     
}
| shiftInstr{
       
      ADDOPERAND(Opc, UCPM::BIU0Shift, @$.S, @$.E);

      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(opc));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      if (imm1 == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm1));
      if (imm2 == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm2));
      if (imm == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm));
     
}
| compareInstr{
       
      ADDOPERAND(Opc, UCPM::BIU0Com, @$.S, @$.E);

      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(opc));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      if (imm1 == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm1));
      if (imm == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm));
     
}
| notInstr{
     
      ADDOPERAND(Opc, UCPM::BIU0NOT, @$.S, @$.E);

      //Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(opc));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      if (imm == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm));
      if (imm1 == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm1));
     
}
| movInstr{
        
       
        if(flags[BF])
            flagsort = 0x0;
        else if(flags[SF])
            flagsort = 0x1;
        else 
            flagsort = 0x2;
        f = OPERAND(Imm, flagsort, FlagS, FlagE);
        
        if(flags[MLF])
            flagsort = 0x1;
        else if(flags[MHF])
            flagsort = 0x2;
        else if(flags[HF])
            flagsort = 0x3; 
        else 
	    flagsort = 0x0;//LF
        ff = OPERAND(Imm, flagsort, FlagS, FlagE);
        
        if(flags[dMLF])
            flagsort = 0x1;
        else if(flags[dMHF])
            flagsort = 0x2;
        else if(flags[dHF])
            flagsort = 0x3;  
        else 
	    flagsort = 0x0;//LF
        qlh = OPERAND(Imm, flagsort, FlagS, FlagE);
        
        flags.reset();
        
      ADDOPERAND(Opc, UCPM::BIU0Mov, @$.S, @$.E);


      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      if (imm == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm));
      if (imm1 == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm1));
	
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ff));
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(f));
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(qlh));
     
}
| maskInstr{
        
       
        if(flags[BF])
            flagsort = 0x0;
        else if(flags[SF])
            flagsort = 0x1;
        else 
            flagsort = 0x2;
        f = OPERAND(Imm, flagsort, FlagS, FlagE);
        
        flags.reset();
        
      ADDOPERAND(Opc, UCPM::BIU0MaskGen, @$.S, @$.E);


      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      if (imm1 == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm1));
      if (imm == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm));

	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(f));
	
     
}
| waitInstr{
            
      ADDOPERAND(Opc, UCPM::BIU0Wait, @$.S, @$.E);
      
      if (imm == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm));
    
}
| t _flag K flag_ ASSIGNTO biu0dest{
    
             if(flags[W0F])
		flagsort = 0x0;
	     else if(flags[W2F])
		flagsort = 0x1;
	      ff = OPERAND(Imm, flagsort, FlagS, FlagE);
	
		      
	     flagsort = (flags[APPF2] << 2) | (flags[SPPF] << 1) | flags[IPPF];
		sia = OPERAND(Imm, flagsort, FlagS, FlagE);
	      
	     flags.reset();
	      
    switch ($6) {
    
      case 3://to Wx.m[t]
       	   	      	      
	      ADDOPERAND(Opc, UCPM::BIU0KGToM, @$.S, @$.E); 
              Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(md));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ff));
         
       break;
       
        case 4://to Wx.m[s++,i++,a++]

	      ADDOPERAND(Opc, UCPM::BIU0KGToMSIA, @$.S, @$.E); 
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ff));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(sia));
 
         break;
       
      default:
	break;
    }
};
 
 
biu1inst: ldselect ASSIGNTO biu1dest {
 
    
    flagsort = (flags[MF] << 2) | (flags[APPF] << 1) | flags[BRF];
    f = OPERAND(Imm, flagsort, FlagS, FlagE);
    
	
    switch ($3) {
    
        case 0://to biu1
	  switch ($1) {
	    case 0://lddis
    
	      //qlh flag
	      if(flags[LF])
		  flagsort = 0x0;
	      else if(flags[QHF])
		  flagsort = 0x1;
	      else if(flags[QLF])
		  flagsort = 0x2;
	      qlh = OPERAND(Imm, flagsort, FlagS, FlagE);
	      flags.reset();
        
	      ADDOPERAND(Opc, UCPM::BIU1disLdToBIU, @$.S, @$.E); 
              Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));//unit'T
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(f));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(qlh));
            break;
        
            case 1://ldstep
    
	      flags.reset();

	      ADDOPERAND(Opc, UCPM::BIU1ComLdToBIU, @$.S, @$.E); 
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));//unit'T
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(f));
	    break;

	  default:
	    break;
       }
       break;
       
	case 1://to shu
          switch ($1) {
	    case 0://lddis
    
	      //qlh flag
	      if(flags[LF])
		  flagsort = 0x0;
	      else if(flags[QHF])
		  flagsort = 0x1;
	      else if(flags[QLF])
		  flagsort = 0x2;
	      qlh = OPERAND(Imm, flagsort, FlagS, FlagE);
	      flags.reset();
        
	      ADDOPERAND(Opc, UCPM::BIU1disLdToSHU, @$.S, @$.E); 
              Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));//unit'T
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(f));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(qlh));
            break;
        
            case 1://ldstep
    
	      flags.reset();

	      ADDOPERAND(Opc, UCPM::BIU1ComLdToSHU, @$.S, @$.E); 
    
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));//unit'T
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(f));

	    break;

	  default:
	    break;
       }
       break;
        
        case 2://to macc
          switch ($1) {
	    case 0://lddis
    
	      //qlh flag
	      if(flags[LF])
		  flagsort = 0x0;
	      else if(flags[QHF])
		  flagsort = 0x1;
	      else if(flags[QLF])
		  flagsort = 0x2;
	      qlh = OPERAND(Imm, flagsort, FlagS, FlagE);
	      flags.reset();
        
	      ADDOPERAND(Opc, UCPM::BIU1disLdToMACC, @$.S, @$.E); 
              Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));//unit'T
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(f));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(qlh));
            break;
        
            case 1://ldstep
    
	      flags.reset();

	      ADDOPERAND(Opc, UCPM::BIU1ComLdToMACC, @$.S, @$.E); 
    
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));//unit'T
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(f));

	    break;

	  default:
	    break;
       }
       break;
       
        case 3://to Wx.m[t]
       
	     if(flags[W1F])
		flagsort = 0x0;
	     else if(flags[W3F])
	        flagsort = 0x1;
	     ff = OPERAND(Imm, flagsort, FlagS, FlagE);
	      
	  switch ($1) {
	    case 0://lddis
    
	      //qlh flag
	      if(flags[LF])
		  flagsort = 0x0;
	      else if(flags[QHF])
		  flagsort = 0x1;
	      else if(flags[QLF])
		  flagsort = 0x2;
	      qlh = OPERAND(Imm, flagsort, FlagS, FlagE);
	      
	      flags.reset();
        
	      ADDOPERAND(Opc, UCPM::BIU1disLdToM, @$.S, @$.E); 
              Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(md));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(f));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(qlh));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ff));
            break;
        
            case 1://ldstep
    
	      flags.reset();

	      ADDOPERAND(Opc, UCPM::BIU1ComLdToM, @$.S, @$.E); 
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(md));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(f));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ff));
	    break;

	  default:
	    break;
       }
       break;
       
         case 4://to Wx.m[s++,i++,a++]
       
	     if(flags[W1F])
		flagsort = 0x0;
	     else if(flags[W3F])
	        flagsort = 0x1;
	     ff = OPERAND(Imm, flagsort, FlagS, FlagE);
	      
	     flagsort = (flags[APPF2] << 2) | (flags[SPPF] << 1) | flags[IPPF];
		sia = OPERAND(Imm, flagsort, FlagS, FlagE);
	
	  switch ($1) {
	    case 0://lddis
    
	      //qlh flag
	      if(flags[LF])
		  flagsort = 0x0;
	      else if(flags[QHF])
		  flagsort = 0x1;
	      else if(flags[QLF])
		  flagsort = 0x2;
	      qlh = OPERAND(Imm, flagsort, FlagS, FlagE);
	      
	      flags.reset();
        
	      ADDOPERAND(Opc, UCPM::BIU1disLdToMSIA, @$.S, @$.E); 
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(f));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(qlh));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ff));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(sia));
            break;
        
            case 1://ldstep
    
	      flags.reset();

	      ADDOPERAND(Opc, UCPM::BIU1ComLdToMSIA, @$.S, @$.E); 
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(f));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ff));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(sia));
	    break;

	  default:
	    break;
       }
       break;
       
    default:
      break;
    }
  
  
}
| stinst{
      
	flagsort = (flags[MF] << 2) | (flags[APPF] << 1) | flags[BRF];
	f = OPERAND(Imm, flagsort, FlagS, FlagE);
     
        //qlh flag
        if(flags[LF])
            flagsort = 0x0;
        else if(flags[HF])
            flagsort = 0x1;
        else if(flags[QF])
            flagsort = 0x2;
        else
            flagsort = 0x3;
        qlh = OPERAND(Imm, flagsort, FlagS, FlagE);
        flags.reset();
        
      ADDOPERAND(Opc, UCPM::BIU1St, @$.S, @$.E); 
        
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(f));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(qlh));      
  
}
| binInstr{
        
      ADDOPERAND(Opc, UCPM::BIU1Bin, @$.S, @$.E);

      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(opc));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tp));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      if (imm2 == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm2));
      if (imm == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm));
      if (imm1 == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm1));
     
}
| shiftInstr{
       
      ADDOPERAND(Opc, UCPM::BIU1Shift, @$.S, @$.E);

      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(opc));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      if (imm1 == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm1));
      if (imm2 == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm2));
      if (imm == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm));
     
}
| compareInstr{
       
      ADDOPERAND(Opc, UCPM::BIU1Com, @$.S, @$.E);

      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(opc));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      if (imm1 == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm1));
      if (imm == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm));
     
}
| notInstr{
     
      ADDOPERAND(Opc, UCPM::BIU1NOT, @$.S, @$.E);

      //Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(opc));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      if (imm == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm));
      if (imm1 == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm1));
     
}
| movInstr{
        
       
        if(flags[BF])
            flagsort = 0x0;
        else if(flags[SF])
            flagsort = 0x1;
        else 
            flagsort = 0x2;
        f = OPERAND(Imm, flagsort, FlagS, FlagE);
        
        if(flags[MLF])
            flagsort = 0x1;
        else if(flags[MHF])
            flagsort = 0x2;
        else if(flags[HF])
            flagsort = 0x3; 
        else 
	    flagsort = 0x0;//LF
        ff = OPERAND(Imm, flagsort, FlagS, FlagE);
        
        if(flags[dMLF])
            flagsort = 0x1;
        else if(flags[dMHF])
            flagsort = 0x2;
        else if(flags[dHF])
            flagsort = 0x3;  
        else 
	    flagsort = 0x0;//LF
        qlh = OPERAND(Imm, flagsort, FlagS, FlagE);
        
        
        flags.reset();
        
      ADDOPERAND(Opc, UCPM::BIU1Mov, @$.S, @$.E);


      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      if (imm == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm));
      if (imm1 == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm1));
	
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ff));
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(f));
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(qlh));
     
}
| maskInstr{
        
       
        if(flags[BF])
            flagsort = 0x0;
        else if(flags[SF])
            flagsort = 0x1;
        else 
            flagsort = 0x2;
        f = OPERAND(Imm, flagsort, FlagS, FlagE);
        
        flags.reset();
        
      ADDOPERAND(Opc, UCPM::BIU1MaskGen, @$.S, @$.E);


      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      if (imm1 == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm1));
      if (imm == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm));

	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(f));
	
     
}
| waitInstr{
            
      ADDOPERAND(Opc, UCPM::BIU1Wait, @$.S, @$.E);
      
      if (imm == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm));
    
}
| t _flag K flag_ ASSIGNTO biu1dest{
    
             if(flags[W1F])
		flagsort = 0x0;
	     else if(flags[W3F])
	        flagsort = 0x1;
	     ff = OPERAND(Imm, flagsort, FlagS, FlagE);
	
		      
	     flagsort = (flags[APPF2] << 2) | (flags[SPPF] << 1) | flags[IPPF];
		sia = OPERAND(Imm, flagsort, FlagS, FlagE);
	      
	     flags.reset();
	      
    switch ($6) {
    
      case 3://to Wx.m[t]
       	   	      	      
	      ADDOPERAND(Opc, UCPM::BIU1KGToM, @$.S, @$.E); 
              Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(md));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ff));
         
       break;
       
        case 4://to Wx.m[s++,i++,a++]

	      ADDOPERAND(Opc, UCPM::BIU1KGToMSIA, @$.S, @$.E); 
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ff));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(sia));
 
         break;
       
    default:
      break;
  }
}; 

biu2inst: ldselect ASSIGNTO biu2dest {
 
    

    flagsort = (flags[MF] << 2) | (flags[APPF] << 1) | flags[BRF];
    f = OPERAND(Imm, flagsort, FlagS, FlagE);
    
    switch ($3) {
    
        case 0://to biu2
	  switch ($1) {
	    case 0://lddis
    
	      //qlh flag
	      if(flags[LF])
		  flagsort = 0x0;
	      else if(flags[QHF])
		  flagsort = 0x1;
	      else if(flags[QLF])
		  flagsort = 0x2;
	      qlh = OPERAND(Imm, flagsort, FlagS, FlagE);
	      flags.reset();
        
	      ADDOPERAND(Opc, UCPM::BIU2disLdToBIU, @$.S, @$.E); 
              Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));//unit'T
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(f));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(qlh));
            break;
        
            case 1://ldstep
    
	      flags.reset();

	      ADDOPERAND(Opc, UCPM::BIU2ComLdToBIU, @$.S, @$.E); 
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));//unit'T
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(f));
	    break;

	  default:
	    break;
       }
       break;
       
	case 1://to shu
          switch ($1) {
	    case 0://lddis
    
	      //qlh flag
	      if(flags[LF])
		  flagsort = 0x0;
	      else if(flags[QHF])
		  flagsort = 0x1;
	      else if(flags[QLF])
		  flagsort = 0x2;
	      qlh = OPERAND(Imm, flagsort, FlagS, FlagE);
	      flags.reset();
        
	      ADDOPERAND(Opc, UCPM::BIU2disLdToSHU, @$.S, @$.E); 
              Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));//unit'T
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(f));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(qlh));
            break;
        
            case 1://ldstep
    
	      flags.reset();

	      ADDOPERAND(Opc, UCPM::BIU2ComLdToSHU, @$.S, @$.E); 
    
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));//unit'T
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(f));

	    break;

	  default:
	    break;
       }
       break;
        
        case 2://to macc
          switch ($1) {
	    case 0://lddis
    
	      //qlh flag
	      if(flags[LF])
		  flagsort = 0x0;
	      else if(flags[QHF])
		  flagsort = 0x1;
	      else if(flags[QLF])
		  flagsort = 0x2;
	      qlh = OPERAND(Imm, flagsort, FlagS, FlagE);
	      flags.reset();
        
	      ADDOPERAND(Opc, UCPM::BIU2disLdToMACC, @$.S, @$.E); 
              Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));//unit'T
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(f));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(qlh));
            break;
        
            case 1://ldstep
    
	      flags.reset();

	      ADDOPERAND(Opc, UCPM::BIU2ComLdToMACC, @$.S, @$.E); 
    
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));//unit'T
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(f));

	    break;

	  default:
	    break;
       }
       break;
       
       
       case 3://to Wx.m[t]
       
	     if(flags[W4F])
		flagsort = 0x0;
	      ff = OPERAND(Imm, flagsort, FlagS, FlagE);
	      
	  switch ($1) {
	    case 0://lddis
    
	      //qlh flag
	      if(flags[LF])
		  flagsort = 0x0;
	      else if(flags[QHF])
		  flagsort = 0x1;
	      else if(flags[QLF])
		  flagsort = 0x2;
	      qlh = OPERAND(Imm, flagsort, FlagS, FlagE);
	      
	      flags.reset();
        
	      ADDOPERAND(Opc, UCPM::BIU2disLdToM, @$.S, @$.E); 
              Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(md));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(f));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(qlh));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ff));
            break;
        
            case 1://ldstep
    
	      flags.reset();

	      ADDOPERAND(Opc, UCPM::BIU2ComLdToM, @$.S, @$.E); 
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(md));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(f));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ff));
	    break;

	  default:
	    break;
       }
       break;
       
        case 4://to Wx.m[s++,i++,a++]
       
	     if(flags[W4F])
		flagsort = 0x0;
	      ff = OPERAND(Imm, flagsort, FlagS, FlagE);
	      
	     flagsort = (flags[APPF2] << 2) | (flags[SPPF] << 1) | flags[IPPF];
		sia = OPERAND(Imm, flagsort, FlagS, FlagE);
	
	  switch ($1) {
	    case 0://lddis
    
	      //qlh flag
	      if(flags[LF])
		  flagsort = 0x0;
	      else if(flags[QHF])
		  flagsort = 0x1;
	      else if(flags[QLF])
		  flagsort = 0x2;
	      qlh = OPERAND(Imm, flagsort, FlagS, FlagE);
	      
	      flags.reset();
        
	      ADDOPERAND(Opc, UCPM::BIU2disLdToMSIA, @$.S, @$.E); 
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(f));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(qlh));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ff));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(sia));
            break;
        
            case 1://ldstep
    
	      flags.reset();

	      ADDOPERAND(Opc, UCPM::BIU2ComLdToMSIA, @$.S, @$.E); 
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(f));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ff));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(sia));
	    break;

	  default:
	    break;
       }
       break;
       
    default:
      break;
    }
   
  
}
| stinst{
      
	flagsort = (flags[MF] << 2) | (flags[APPF] << 1) | flags[BRF];
	f = OPERAND(Imm, flagsort, FlagS, FlagE);
     
        //qlh flag
        if(flags[LF])
            flagsort = 0x0;
        else if(flags[HF])
            flagsort = 0x1;
        else if(flags[QF])
            flagsort = 0x2;
        else
            flagsort = 0x3;
        qlh = OPERAND(Imm, flagsort, FlagS, FlagE);
        flags.reset();
        
      ADDOPERAND(Opc, UCPM::BIU2St, @$.S, @$.E); 
        
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(f));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(qlh));      
  
}
| binInstr{
        
      ADDOPERAND(Opc, UCPM::BIU2Bin, @$.S, @$.E);

      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(opc));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tp));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      if (imm2 == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm2));
      if (imm == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm));
      if (imm1 == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm1));
     
}
| shiftInstr{
       
      ADDOPERAND(Opc, UCPM::BIU2Shift, @$.S, @$.E);

      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(opc));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      if (imm1 == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm1));
      if (imm2 == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm2));
      if (imm == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm));
     
}
| compareInstr{
       
      ADDOPERAND(Opc, UCPM::BIU2Com, @$.S, @$.E);

      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(opc));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      if (imm1 == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm1));
      if (imm == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm));
     
}
| notInstr{
        
      ADDOPERAND(Opc, UCPM::BIU2NOT, @$.S, @$.E);

      //Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(opc));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      if (imm == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm));
      if (imm1 == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm1));
     
}
| movInstr{
        
       
        if(flags[BF])
            flagsort = 0x0;
        else if(flags[SF])
            flagsort = 0x1;
        else 
            flagsort = 0x2;
        f = OPERAND(Imm, flagsort, FlagS, FlagE);
        
	if(flags[MLF])
            flagsort = 0x1;
        else if(flags[MHF])
            flagsort = 0x2;
        else if(flags[HF])
            flagsort = 0x3; 
        else 
	    flagsort = 0x0;//LF
        ff = OPERAND(Imm, flagsort, FlagS, FlagE);
        
        if(flags[dMLF])
            flagsort = 0x1;
        else if(flags[dMHF])
            flagsort = 0x2;
        else if(flags[dHF])
            flagsort = 0x3;  
        else 
	    flagsort = 0x0;//LF
        qlh = OPERAND(Imm, flagsort, FlagS, FlagE);
        
        flags.reset();
        
      ADDOPERAND(Opc, UCPM::BIU2Mov, @$.S, @$.E);


      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      if (imm == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm));
      if (imm1 == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm1));
	
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ff));
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(f));
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(qlh));
     
}
| maskInstr{
        
       
        if(flags[BF])
            flagsort = 0x0;
        else if(flags[SF])
            flagsort = 0x1;
        else 
            flagsort = 0x2;
        f = OPERAND(Imm, flagsort, FlagS, FlagE);
        
        flags.reset();
        
      ADDOPERAND(Opc, UCPM::BIU2MaskGen, @$.S, @$.E);


      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
      if (imm1 == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm1));
      if (imm == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm));

	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(f));
	
     
}
| waitInstr{
            
      ADDOPERAND(Opc, UCPM::BIU2Wait, @$.S, @$.E);
      
      if (imm == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm));
    
}
| t _flag K flag_ ASSIGNTO biu2dest{
    
    switch ($6) {
    
      case 3://to Wx.m[t]
       
	      if(flags[W4F])
		flagsort = 0x0;
	      ff = OPERAND(Imm, flagsort, FlagS, FlagE);
	      	      
	      flags.reset();
        
	      ADDOPERAND(Opc, UCPM::BIU2KGToM, @$.S, @$.E); 
              Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(md));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ff));
         
       break;
       
        case 4://to Wx.m[s++,i++,a++]
       
	     if(flags[W4F])
		flagsort = 0x0;
	      ff = OPERAND(Imm, flagsort, FlagS, FlagE);
	      
	     flagsort = (flags[APPF2] << 2) | (flags[SPPF] << 1) | flags[IPPF];
		sia = OPERAND(Imm, flagsort, FlagS, FlagE);
	      
	      flags.reset();
        
	      ADDOPERAND(Opc, UCPM::BIU2KGToMSIA, @$.S, @$.E); 
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ff));
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(sia));
 
         break;
       
    default:
      break;
  }	
}; 

ldselect: lddis{
            $$ = 0; 
        } |
        ldstep{
            $$ = 1; 
        };
lddis:      t _flag t flag_ _flag biuflags flag_ _flag lddisflag flag_ |
            t _flag t flag_ _flag lddisflag flag_ ;
          
ldstep:     t  _flag biuflags flag_ | t;
        

biu0dest: shu0te | shu1te | ialute | imacte | ifalute | ifmacte | bbiu0te | w_mindexn | w_mindexsia;
biu1dest: shu0te | b1shu2te | ialute | imacte | ifalute | ifmacte | bbiu1te | w_mindexn | w_mindexsia;
biu2dest: b2shu1te | b1shu2te | ialute | imacte | ifalute | ifmacte | bbiu2te | w_mindexn | w_mindexsia;

w_mindexn: wflag DOT MINDEXN {
  $$ = 3; ms = md;
  md = OPERAND(Reg, MRI->getRegClass(UCPMReg::MRegRegClassID).getRegister($3), @$.S, @$.E);
};

w_mindexsia: wflag DOT M LBRACKET siaflags RBRACKET {$$=4;} | wflag DOT M {$$=4;};


stinst:  t _flag t flag_ ASSIGNTO DM _flag biuflags flag_ _flag storeflag flag_ |
         t _flag t flag_ ASSIGNTO DM _flag biuflags flag_ |
         t _flag t flag_ ASSIGNTO DM _flag storeflag flag_|
         t _flag t flag_ ASSIGNTO DM;


binInstr:     addclause {opc = OPERAND(Reg, UCPMReg::BIUADD, @$.S, @$.E);} | 
              subclause {opc = OPERAND(Reg, UCPMReg::BIUSUB, @$.S, @$.E);} | 
              andclause {opc = OPERAND(Reg, UCPMReg::BIUAND, @$.S, @$.E);} |
               orclause {opc = OPERAND(Reg, UCPMReg::BIUOR, @$.S, @$.E);} ;
addclause: t LBRACKET IMM5 RBRACKET ADD t LBRACKET IMM5 RBRACKET ASSIGNTO t LBRACKET IMM5 RBRACKET
	  {imm = OPERAND(Imm, $3, @3.S, @3.E);imm1 = OPERAND(Imm, $8, @8.S, @8.E);imm2 = OPERAND(Imm, $13, @13.S, @13.E);};
subclause: t LBRACKET IMM5 RBRACKET SUB t LBRACKET IMM5 RBRACKET ASSIGNTO t LBRACKET IMM5 RBRACKET
	  {imm = OPERAND(Imm, $3, @3.S, @3.E);imm1 = OPERAND(Imm, $8, @8.S, @8.E);imm2 = OPERAND(Imm, $13, @13.S, @13.E);};
andclause: t LBRACKET IMM5 RBRACKET AND t LBRACKET IMM5 RBRACKET ASSIGNTO t LBRACKET IMM5 RBRACKET
	  {imm = OPERAND(Imm, $3, @3.S, @3.E);imm1 = OPERAND(Imm, $8, @8.S, @8.E);imm2 = OPERAND(Imm, $13, @13.S, @13.E);};
orclause : t LBRACKET IMM5 RBRACKET OR t LBRACKET IMM5 RBRACKET ASSIGNTO t LBRACKET IMM5 RBRACKET
	  {imm = OPERAND(Imm, $3, @3.S, @3.E);imm1 = OPERAND(Imm, $8, @8.S, @8.E);imm2 = OPERAND(Imm, $13, @13.S, @13.E);}; 

shiftInstr :   lshtclause {opc = OPERAND(Reg, UCPMReg::BIULSHT, @$.S, @$.E);} |
               rshtclause {opc = OPERAND(Reg, UCPMReg::BIURSHT, @$.S, @$.E);} ;
lshtclause: t LBRACKET IMM5 RBRACKET LSHT IMM5 ASSIGNTO t LBRACKET IMM5 RBRACKET
	     {imm = OPERAND(Imm, $3, @3.S, @3.E);imm1 = OPERAND(Imm, $6, @6.S, @6.E);imm2 = OPERAND(Imm, $10, @10.S, @10.E);};
rshtclause: t LBRACKET IMM5 RBRACKET RSHT IMM5 ASSIGNTO t LBRACKET IMM5 RBRACKET
	     {imm = OPERAND(Imm, $3, @3.S, @3.E);imm1 = OPERAND(Imm, $6, @6.S, @6.E);imm2 = OPERAND(Imm, $10, @10.S, @10.E);};

compareInstr :  equclause {opc = OPERAND(Reg, UCPMReg::BIUEQU, @$.S, @$.E);} |
               neqclause {opc = OPERAND(Reg, UCPMReg::BIUNEQ, @$.S, @$.E);} ;
equclause: t LBRACKET IMM5 RBRACKET EQU IMM5 ASSIGNTO t LBRACKET IMM5 RBRACKET
	     {imm = OPERAND(Imm, $3, @3.S, @3.E);imm1 = OPERAND(Imm, $10, @10.S, @10.E);};
neqclause: t LBRACKET IMM5 RBRACKET NEQ IMM5 ASSIGNTO t LBRACKET IMM5 RBRACKET
	     {imm = OPERAND(Imm, $3, @3.S, @3.E);imm1 = OPERAND(Imm, $10, @10.S, @10.E);};

notInstr:  NOT2 t LBRACKET IMM5 RBRACKET ASSIGNTO t LBRACKET IMM5 RBRACKET
	     {imm = OPERAND(Imm, $4, @4.S, @4.E);imm1 = OPERAND(Imm, $9, @9.S, @9.E);};

movInstr: t LBRACKET IMM5 RBRACKET _flag biuImmflag flag_ ASSIGNTO t LBRACKET IMM5 RBRACKET _flag biuImm2flag flag_ _flag biuImm1flag flag_
	  {imm = OPERAND(Imm, $3, @3.S, @3.E);imm1 = OPERAND(Imm, $11, @11.S, @11.E);}
	  | t LBRACKET IMM5 RBRACKET ASSIGNTO t LBRACKET IMM5 RBRACKET 
	  {imm = OPERAND(Imm, $3, @3.S, @3.E);imm1 = OPERAND(Imm, $8, @8.S, @8.E);};

maskInstr: t LBRACKET IMM5 RBRACKET _flag biuImm2flag flag_ ASSIGNTO t LBRACKET IMM5 RBRACKET DOT MASK
           {imm = OPERAND(Imm, $3, @3.S, @3.E);imm1 = OPERAND(Imm, $11, @11.S, @11.E);}
           | t LBRACKET IMM5 RBRACKET ASSIGNTO t LBRACKET IMM5 RBRACKET DOT MASK
           {imm = OPERAND(Imm, $3, @3.S, @3.E);imm1 = OPERAND(Imm, $8, @8.S, @8.E);};

waitInstr: WAIT IMM5 {imm = OPERAND(Imm, $2, @2.S, @2.E);};

setcond0Instr: biu0cond{   
	    
	    ADDOPERAND(Opc, UCPM::BIU0SetCond, @$.S, @$.E);
	    
	    if (imm == NULL)
	      ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
	    else
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm));
	    if (imm1 == NULL)
	      ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
	    else
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm1));   
	  };
setcond1Instr: biu1cond{   
	    
	    ADDOPERAND(Opc, UCPM::BIU1SetCond, @$.S, @$.E);
	    
	    if (imm == NULL)
	      ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
	    else
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm));
	    if (imm1 == NULL)
	      ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
	    else
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm1));   
	  };
setcond2Instr: biu2cond{   
	    
	    ADDOPERAND(Opc, UCPM::BIU2SetCond, @$.S, @$.E);
	    
	    if (imm == NULL)
	      ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
	    else
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm));
	    if (imm1 == NULL)
	      ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
	    else
	      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm1));   
	  };

biu0cond: IMMSYM DOT IMM5 ASSIGNTO BIU0 DOT SETCOND LBRACKET IMM5 RBRACKET 
	  {imm = OPERAND(Imm, $3, @3.S, @3.E);imm1 = OPERAND(Imm, $9, @9.S, @9.E);};      
biu1cond: IMMSYM DOT IMM5 ASSIGNTO BIU1 DOT SETCOND LBRACKET IMM5 RBRACKET 
	  {imm = OPERAND(Imm, $3, @3.S, @3.E);imm1 = OPERAND(Imm, $9, @9.S, @9.E);};
biu2cond: IMMSYM DOT IMM5 ASSIGNTO BIU2 DOT SETCOND LBRACKET IMM5 RBRACKET 
	  {imm = OPERAND(Imm, $3, @3.S, @3.E);imm1 = OPERAND(Imm, $9, @9.S, @9.E);};
    	  

imm0Instr: biu0imm{
            
        if(flags[BF])
            flagsort = 0x0;
        else if(flags[SF])
            flagsort = 0x1;
        else if(flags[UF])
            flagsort = 0x2;
        else 
            flagsort = 0x3;  
        f = OPERAND(Imm, flagsort, FlagS, FlagE);
        
        if(flags[MLF])
            flagsort = 0x1;
        else if(flags[MHF])
            flagsort = 0x2;
        else if(flags[HF])
            flagsort = 0x3;  
        else
            flagsort = 0x0;//LF
        ff = OPERAND(Imm, flagsort, FlagS, FlagE);
        
        flags.reset();
        
      ADDOPERAND(Opc, UCPM::BIU0Imm, @$.S, @$.E);

      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));

      if (imm1 == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm1));
      if (imm == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm));  
	
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(f));
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ff));
};
imm1Instr: biu1imm{
            
        if(flags[BF])
            flagsort = 0x0;
        else if(flags[SF])
            flagsort = 0x1;
        else if(flags[UF])
            flagsort = 0x2;
        else 
            flagsort = 0x3;  
        f = OPERAND(Imm, flagsort, FlagS, FlagE);
        
        if(flags[MLF])
            flagsort = 0x1;
        else if(flags[MHF])
            flagsort = 0x2;
        else if(flags[HF])
            flagsort = 0x3;  
        else
            flagsort = 0x0;//LF
        ff = OPERAND(Imm, flagsort, FlagS, FlagE);
        
        flags.reset();
        
      ADDOPERAND(Opc, UCPM::BIU1Imm, @$.S, @$.E);

      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));

      if (imm1 == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm1));
      if (imm == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm));  
	
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(f));
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ff));
};
imm2Instr: biu2imm{
            
        if(flags[BF])
            flagsort = 0x0;
        else if(flags[SF])
            flagsort = 0x1;
        else if(flags[UF])
            flagsort = 0x2;
        else 
            flagsort = 0x3;  
        f = OPERAND(Imm, flagsort, FlagS, FlagE);
        
        if(flags[MLF])
            flagsort = 0x1;
        else if(flags[MHF])
            flagsort = 0x2;
        else if(flags[HF])
            flagsort = 0x3;  
        else
            flagsort = 0x0;//LF
        ff = OPERAND(Imm, flagsort, FlagS, FlagE);
        
        flags.reset();
        
      ADDOPERAND(Opc, UCPM::BIU2Imm, @$.S, @$.E);

      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));

      if (imm1 == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm1));
      if (imm == NULL)
        ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
      else
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm));  
	
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(f));
	Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ff));
};

biu0imm: IMMSYM DOT IMM5 ASSIGNTO BIU0 DOT t LBRACKET IMM5 RBRACKET _flag biuImm2flag flag_ _flag biuImmflag flag_
	  {imm = OPERAND(Imm, $3, @3.S, @3.E);imm1 = OPERAND(Imm, $9, @9.S, @9.E);}
	 | IMMSYM DOT IMM5 ASSIGNTO BIU0 DOT t LBRACKET IMM5 RBRACKET _flag biuImm2flag flag_ 
	  {imm = OPERAND(Imm, $3, @3.S, @3.E);imm1 = OPERAND(Imm, $9, @9.S, @9.E);}
	 | IMMSYM DOT IMM5 ASSIGNTO BIU0 DOT t LBRACKET IMM5 RBRACKET 
	  {imm = OPERAND(Imm, $3, @3.S, @3.E);imm1 = OPERAND(Imm, $9, @9.S, @9.E);};
biu1imm: IMMSYM DOT IMM5 ASSIGNTO BIU1 DOT t LBRACKET IMM5 RBRACKET _flag biuImm2flag flag_ _flag biuImmflag flag_
	  {imm = OPERAND(Imm, $3, @3.S, @3.E);imm1 = OPERAND(Imm, $9, @9.S, @9.E);}
	 | IMMSYM DOT IMM5 ASSIGNTO BIU1 DOT t LBRACKET IMM5 RBRACKET _flag biuImm2flag flag_ 
	  {imm = OPERAND(Imm, $3, @3.S, @3.E);imm1 = OPERAND(Imm, $9, @9.S, @9.E);}
	 | IMMSYM DOT IMM5 ASSIGNTO BIU1 DOT t LBRACKET IMM5 RBRACKET 
	  {imm = OPERAND(Imm, $3, @3.S, @3.E);imm1 = OPERAND(Imm, $9, @9.S, @9.E);};
biu2imm: IMMSYM DOT IMM5 ASSIGNTO BIU2 DOT t LBRACKET IMM5 RBRACKET _flag biuImm2flag flag_ _flag biuImmflag flag_
	  {imm = OPERAND(Imm, $3, @3.S, @3.E);imm1 = OPERAND(Imm, $9, @9.S, @9.E);}
	 | IMMSYM DOT IMM5 ASSIGNTO BIU2 DOT t LBRACKET IMM5 RBRACKET _flag biuImm2flag flag_ 
	  {imm = OPERAND(Imm, $3, @3.S, @3.E);imm1 = OPERAND(Imm, $9, @9.S, @9.E);}
	 | IMMSYM DOT IMM5 ASSIGNTO BIU2 DOT t LBRACKET IMM5 RBRACKET 
	  {imm = OPERAND(Imm, $3, @3.S, @3.E);imm1 = OPERAND(Imm, $9, @9.S, @9.E);};
      
// ducx end biu --------------------------------------------------


// ducx start seq --------------------------------------------------
seqslot: reinst | lpinst | mpustop;

reinst: REPEAT ALPHA _flag repeatexp {Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(imm));};
repeatexp: immrep{ADDOPERAND(Opc, UCPM::REPEATIMM, @-2.S, @$.E);}; //-2 flag_
immrep: IMM5 {imm = OPERAND(Imm, $1, @$.S, @$.E); };

lpinst: lpexp lpcond;
lpexp: LOOP label ALPHA{
  OS<<"Position 3\n";
  ADDOPERAND(Opc, UCPM::LPTO, @$.S, @$.E);
  if(expr) Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(expr));
  else ADDOPERAND(Imm, $2, @2.S, @2.E);
};
lpcond: _flag kiflag flag_{
  ADDOPERAND(Imm, $2, @2.S, @2.E);
  ADDOPERAND(Imm, 0, SMLoc(), SMLoc());
}
| _flag kiflag SUB IMM5 flag_{
 OS<<"Position 2\n";
  ADDOPERAND(Imm, $2, @2.S, @2.E);
  ADDOPERAND(Imm, $4, @4.S, @4.E);
};

mpustop:MPUSTOP {ADDOPERAND(Opc, UCPM::MPUStop, @$.S, @$.E);};

// ducx end seq --------------------------------------------------


ialut: ialu DOT t {$$ = 2; unit = OPERAND(Reg, UCPMReg::IALU, @1.S, @1.E); unit2 = OPERAND(Reg, UCPMReg::MTIALU, @1.S, @1.E); unit3 = OPERAND(Reg, UCPMReg::ALUIALU, @1.S, @1.E); if (!ut) ut = tk ? tk : (tp ? tp : (tn ? tn : tm));};
imact: imac DOT t {$$ = 2; unit = OPERAND(Reg, UCPMReg::IMAC, @1.S, @1.E); unit2 = OPERAND(Reg, UCPMReg::MTIMAC, @1.S, @1.E); unit3 = OPERAND(Reg, UCPMReg::ALUIMAC, @1.S, @1.E);if (!ut) ut = tk ? tk : (tp ? tp : (tn ? tn : tm));};
ifalut: ifalu DOT t {$$ = 2; unit = OPERAND(Reg, UCPMReg::IFALU, @1.S, @1.E); unit2 = OPERAND(Reg, UCPMReg::MTIFALU, @1.S, @1.E); unit3 = OPERAND(Reg, UCPMReg::ALUIFALU, @1.S, @1.E);if (!ut) ut = tk ? tk : (tp ? tp : (tn ? tn : tm));};
ifmact: ifmac DOT t {$$ = 2; unit = OPERAND(Reg, UCPMReg::IFMAC, @1.S, @1.E); unit2 = OPERAND(Reg, UCPMReg::MTIFMAC, @1.S, @1.E); unit3 = OPERAND(Reg, UCPMReg::ALUIFMAC, @1.S, @1.E);if (!ut) ut = tk ? tk : (tp ? tp : (tn ? tn : tm));};

ialute: ialu DOT shusrct {$$ = 2; unit = OPERAND(Reg, UCPMReg::IALU, @1.S, @1.E); if (!ut) ut = tk ? tk : (tp ? tp : (tn ? tn : tm));};
imacte: imac DOT shusrct {$$ = 2; unit = OPERAND(Reg, UCPMReg::IMAC, @1.S, @1.E); if (!ut) ut = tk ? tk : (tp ? tp : (tn ? tn : tm));};
ifalute: ifalu DOT shusrct {$$ = 2; unit = OPERAND(Reg, UCPMReg::IFALU, @1.S, @1.E); if (!ut) ut = tk ? tk : (tp ? tp : (tn ? tn : tm));};
ifmacte: ifmac DOT shusrct {$$ = 2; unit = OPERAND(Reg, UCPMReg::IFMAC, @1.S, @1.E); if (!ut) ut = tk ? tk : (tp ? tp : (tn ? tn : tm));};

/*biu0t: biu0 {$$ = 3; unit = b; unit2 = b2;
             unsigned treg = MRI->getRegClass(UCPMReg::TPortRegClassID).getRegister(0);//here 't0' is assigned to biu0t
             ut = OPERAND(Reg, treg, @$.S, @$.E); }
biu1t: biu1 {$$ = 3; unit = b; unit2 = b2;
             unsigned treg = MRI->getRegClass(UCPMReg::TPortRegClassID).getRegister(0);//here 't0' is assigned to biu0t
             ut = OPERAND(Reg, treg, @$.S, @$.E); }
biu2t: biu2 {$$ = 3; unit = b; unit2 = b2;
             unsigned treg = MRI->getRegClass(UCPMReg::TPortRegClassID).getRegister(0);//here 't0' is assigned to biu0t
             ut = OPERAND(Reg, treg, @$.S, @$.E); }
biu0: BIU0 {$$ = 2; b = OPERAND(Reg, UCPMReg::BIU0, @$.S, @$.E); b2 = OPERAND(Reg, UCPMReg::MTBIU0, @$.S, @$.E);};
biu1: BIU1 {$$ = 2; b = OPERAND(Reg, UCPMReg::BIU1, @$.S, @$.E); b2 = OPERAND(Reg, UCPMReg::MTBIU1, @$.S, @$.E);};
biu2: BIU2 {$$ = 2; b = OPERAND(Reg, UCPMReg::BIU2, @$.S, @$.E); b2 = OPERAND(Reg, UCPMReg::MTBIU2, @$.S, @$.E);};*/

shu0t: SHU0 DOT t {$$ = 1; unit = OPERAND(Reg, UCPMReg::SHU0, @1.S, @1.E); unit2 = OPERAND(Reg, UCPMReg::MTSHU0, @1.S, @1.E); unit3 = OPERAND(Reg, UCPMReg::ALUSHU0, @1.S, @1.E); if (!ut) ut = tk ? tk : (tp ? tp : (tn ? tn : tm));};
shu1t: SHU1 DOT t {$$ = 1; unit = OPERAND(Reg, UCPMReg::SHU1, @1.S, @1.E); unit2 = OPERAND(Reg, UCPMReg::MTSHU1, @1.S, @1.E); unit3 = OPERAND(Reg, UCPMReg::ALUSHU1, @1.S, @1.E); if (!ut) ut = tk ? tk : (tp ? tp : (tn ? tn : tm));};
shu2t: SHU2 DOT t {$$ = 1; unit = OPERAND(Reg, UCPMReg::SHU2, @1.S, @1.E); unit2 = OPERAND(Reg, UCPMReg::MTSHU2, @1.S, @1.E); unit3 = OPERAND(Reg, UCPMReg::ALUSHU2, @1.S, @1.E); if (!ut) ut = tk ? tk : (tp ? tp : (tn ? tn : tm));};

//dcx FOR BIU
shu0te: SHU0 DOT shusrct {$$ = 1; unit = OPERAND(Reg, UCPMReg::SHU0, @1.S, @1.E); if (!ut) ut = tk ? tk : (tp ? tp : (tn ? tn : tm));};
shu1te: SHU1 DOT shusrct {$$ = 1; unit = OPERAND(Reg, UCPMReg::SHU1, @1.S, @1.E); if (!ut) ut = tk ? tk : (tp ? tp : (tn ? tn : tm));};
shu2te: SHU2 DOT shusrct {$$ = 1; unit = OPERAND(Reg, UCPMReg::SHU2, @1.S, @1.E); if (!ut) ut = tk ? tk : (tp ? tp : (tn ? tn : tm));};
b1shu2te: SHU2 DOT shusrct {$$ = 1; unit = OPERAND(Reg, UCPMReg::B1SHU2, @1.S, @1.E); if (!ut) ut = tk ? tk : (tp ? tp : (tn ? tn : tm));};
b2shu1te: SHU1 DOT shusrct {$$ = 1; unit = OPERAND(Reg, UCPMReg::B2SHU1, @1.S, @1.E); if (!ut) ut = tk ? tk : (tp ? tp : (tn ? tn : tm));};
bbiu0te:BIU0 DOT shusrct {$$ = 0; unit = OPERAND(Reg, UCPMReg::bBIU0, @1.S, @1.E); if (!ut) ut = tk ? tk : (tp ? tp : (tn ? tn : tm));};
bbiu1te:BIU1 DOT shusrct {$$ = 0; unit = OPERAND(Reg, UCPMReg::bBIU1, @1.S, @1.E); if (!ut) ut = tk ? tk : (tp ? tp : (tn ? tn : tm));};
bbiu2te:BIU2 DOT shusrct {$$ = 0; unit = OPERAND(Reg, UCPMReg::bBIU2, @1.S, @1.E); if (!ut) ut = tk ? tk : (tp ? tp : (tn ? tn : tm));};


//dcx FOR SHU
biu0t: BIU0 DOT t {$$ = 1; unit = OPERAND(Reg, UCPMReg::BIU0, @1.S, @1.E); unit2 = OPERAND(Reg, UCPMReg::MTBIU0, @1.S, @1.E); unit3 = OPERAND(Reg, UCPMReg::ALUBIU0, @1.S, @1.E); if (!ut) ut = tk ? tk : (tp ? tp : (tn ? tn : tm));};
biu1t: BIU1 DOT t {$$ = 1; unit = OPERAND(Reg, UCPMReg::BIU1, @1.S, @1.E); unit2 = OPERAND(Reg, UCPMReg::MTBIU1, @1.S, @1.E); unit3 = OPERAND(Reg, UCPMReg::ALUBIU1, @1.S, @1.E); if (!ut) ut = tk ? tk : (tp ? tp : (tn ? tn : tm));};
biu2t: BIU2 DOT t {$$ = 1; unit = OPERAND(Reg, UCPMReg::BIU2, @1.S, @1.E); unit2 = OPERAND(Reg, UCPMReg::MTBIU2, @1.S, @1.E); unit3 = OPERAND(Reg, UCPMReg::ALUBIU2, @1.S, @1.E); if (!ut) ut = tk ? tk : (tp ? tp : (tn ? tn : tm));};
s1biu1t: BIU1 DOT t {$$ = 1; unit = OPERAND(Reg, UCPMReg::S1BIU1, @1.S, @1.E); if (!ut) ut = tk ? tk : (tp ? tp : (tn ? tn : tm));};
s2biu1t: BIU1 DOT t {$$ = 1; unit = OPERAND(Reg, UCPMReg::S2BIU1, @1.S, @1.E); if (!ut) ut = tk ? tk : (tp ? tp : (tn ? tn : tm));};
sshu0t:SHU0 DOT t {$$ = 0; unit = OPERAND(Reg, UCPMReg::sSHU0, @1.S, @1.E); if (!ut) ut = tk ? tk : (tp ? tp : (tn ? tn : tm));};
sshu1t:SHU1 DOT t {$$ = 0; unit = OPERAND(Reg, UCPMReg::sSHU1, @1.S, @1.E); if (!ut) ut = tk ? tk : (tp ? tp : (tn ? tn : tm));};
sshu2t:SHU2 DOT t {$$ = 0; unit = OPERAND(Reg, UCPMReg::sSHU2, @1.S, @1.E); if (!ut) ut = tk ? tk : (tp ? tp : (tn ? tn : tm));};


//mindexs: MINDEXS {$$ = 0; ms = md; md = OPERAND(Reg, UCPMReg::MSPP, @$.S, @$.E);};
//mindexi: MINDEXI {$$ = 0; ms = md; md = OPERAND(Reg, UCPMReg::MIPP, @$.S, @$.E);};
mindexn: MINDEXN {$$ = 3; ms = md;
  md = OPERAND(Reg,
               MRI->getRegClass(UCPMReg::MRegRegClassID).getRegister($1),
               @$.S, @$.E);
};



t: TREG {
  unsigned treg = MRI->getRegClass(UCPMReg::TPortRegClassID).getRegister($1);
  if (!tm) tm = OPERAND(Reg, treg, @$.S, @$.E);
  else if (!tn) tn = OPERAND(Reg, treg, @$.S, @$.E);
  else if (!tp) tp = OPERAND(Reg, treg, @$.S, @$.E);
  else if (!tk) tk = OPERAND(Reg, treg, @$.S, @$.E);
  else if (!ut) ut = OPERAND(Reg, treg, @$.S, @$.E);
};
constt: _constt {
  unsigned treg = $1;
  if (!tm) tm = OPERAND(Reg, treg, @$.S, @$.E);
  else if (!tn) tn = OPERAND(Reg, treg, @$.S, @$.E);
  else if (!tp) tp = OPERAND(Reg, treg, @$.S, @$.E);
  else if (!tk) tk = OPERAND(Reg, treg, @$.S, @$.E);
  else if (!ut) ut = OPERAND(Reg, treg, @$.S, @$.E);
}
_constt: TBB {$$ = UCPMReg::TBB;} |
         TBH {$$ = UCPMReg::TBH;} |
         TBW {$$ = UCPMReg::TBW;} |
         TBD {$$ = UCPMReg::TBD;} |
         TSQ {$$ = UCPMReg::TSQ;} ;
shusrct: t | constt;
tripara: dupara COMMA t;
dupara: t COMMA t;

_flag: LPAREN {FlagS = @$.E;};
flag_: RPAREN {FlagE = @$.S;};
imm: imm5 | IMM;
imm1: imm5 | IMM;
imm2: imm5 | IMM;
imm5: IMM3 | IMM5 ; 
ialu: IALU ;
imac: IMAC ;
falu: FALU ;
fmac: FMAC ;
ifalu: IFALU ;
ifmac: IFMAC ;


ialuflags: ialuflag | ialuflag COMMA ialuflags;
ialuflag:    T   {flags.set(TF);}  |
             CI  {flags.set(CIF);} |
             F   {flags.set(FF);}  |
             B   {flags.set(BF);}  |
             S   {flags.set(SF);}  |
             U   {flags.set(UF);}  ;
imacrealflag:  B   {flags.set(BF);}  |
               S   {flags.set(SF);}  ;
imaccompflag:  R   {flags.set(RF);}  |
               I   {flags.set(IF);}  ;
imacflags: imacflagpart2 | imacflagpart2 COMMA imacflags;
imacflagpart2: U   {flags.set(UF);}  |
               L   {flags.set(LF);}  |
               P   {flags.set(PF);}  |
               T   {flags.set(TF);}  |
               F   {flags.set(FF);}  |
               CFLAG        {flags.set(CF);}  |
               SEND      {flags.set(SENDF);}  |
               SHIFT0    {flags.set(S0F);}    |
               SHIFT1    {flags.set(S1F);}    |
               SHIFT2    {flags.set(S2F);}    |
               SHIFT3    {flags.set(S3F);}    ;
               
//ducx
ifmacTflag:    T       {flags.set(TF);};

biuflags : biuflag | biuflag COMMA biuflags;
biuflag :      BR  {flags.set(BRF);}   |
               M  {flags.set(MF);}     |
               APP  {flags.set(APPF);} ;
                          
lddisflag :    QL {flags.set(QLF);}    | 
               QH {flags.set(QHF);}    |
               L {flags.set(LF);}      ;
               
//ldstepflag :   S {flags.set(SF);}      ;

storeflag :    H {flags.set(HF);}      |
               Q {flags.set(QF);}      |
               L {flags.set(LF);}      ;

biuImmflag :   L {flags.set(LF);}      |
               ML {flags.set(MLF);}     |
               MH {flags.set(MHF);}     |
               H {flags.set(HF);}      ;
               
biuImm1flag :  L {flags.set(dLF);}      |
               ML {flags.set(dMLF);}     |
               MH {flags.set(dMHF);}     |
               H {flags.set(dHF);}      ;
               
biuImm2flag :  B {flags.set(BF);}      |
               S {flags.set(SF);}     |
               U {flags.set(UF);}      ;
               
shuflag :  B {flags.set(BF);}      |
           S {flags.set(SF);}     ;

siaflags : siaflag | siaflag COMMA siaflags;
siaflag :      SPP  {flags.set(SPPF);}   |
               IPP  {flags.set(IPPF);}     |
               APP  {flags.set(APPF2);} ;
               
wflag:	       W0 {flags.set(W0F);}     |
               W1 {flags.set(W1F);}     |
               W2 {flags.set(W2F);}     |
               W3 {flags.set(W3F);}     |
               W4 {flags.set(W4F);}     ;    
               
configflag:   KM {flags.set(KMF);}     |
	      KG {flags.set(KGF);}     |
	      KE {flags.set(KEF);}     |
	      KMEABLE {flags.set(KMEABLEF);}     |
	      KGEABLE {flags.set(KGEABLEF);}     |
	      BR {flags.set(BRF);}     |
	      L1 {flags.set(L1F);}     |
	      L2 {flags.set(L2F);}     |
	      L3 {flags.set(L3F);}     |
	      L4 {flags.set(L4F);}     |
	      ALL {flags.set(ALLF);}     ;
    
configmflag:  KI1215 {flags.set(KI0F);} |
	      KI1619 {flags.set(KI1F);} |
	      KI2023 {flags.set(KI2F);} |
	      KI2427 {flags.set(KI3F);} |
	      KI1227 {flags.set(KI4F);} |
	      KI1618 {flags.set(KI5F);} |
	      KI2022 {flags.set(KI6F);} |
	      KI2426 {flags.set(KI7F);} ;
	      
siflag :      S   {flags.set(SF);}  |
              I   {flags.set(IF);}  ;

kiflag: KI;
label: EXPR{$$=0;expr=$1;  OS<<"Position 1\n";} | IMM5 {$$=$1;expr=0;};