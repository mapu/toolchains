                   %{
#include <string>
#include <bitset>

std::bitset<32> flags;
static unsigned int flagsort;
const unsigned HF=1, UF=2, TF=3, SF=4, DF=5, IF=6, LF=7, APPF=8, KPPF=9, CRF=10, BRF=11, MF=12, TCF=13, CDF=14, NCF=15, 
               CIF = 16, FF = 17, BF=18, PF = 19, RF = 20, CF = 21, SENDF = 22, S0F = 23, S1F = 24, S2F = 25, S3F = 26, SSF=27, TTF=28;
static UCPM::UCPMAsmOperand *opc, *tm, *tn, *tk, *tp, *revt, *f, *ff, *shift, *unit, *unit2, *ut, *b, *b2, *md, *ms, *imm, *expr, *ipath;//unit2, b2 are used as alternative unit, such as MReg Target
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
%token <val> NEGIMM IMM3 IMM IMM5 ASSIGNTO EQU NEQ ST NLT LT NST LPAREN RPAREN LBRACE RBRACE DOT COMMA ADD SUB MUL CMUL LSHT RSHT
%token <val> OR AND XOR NOT NEG ADDSUB ACC1 ACC2 ALPHA SPLIT LINEEND SHU0 SHU1 SHU2 BIU0 BIU1 BIU2 M COND
%token <val> IALU IMAC FALU FMAC IFALU IFMAC MINDEXI MINDEXS TB TBB TBH TBW TBD TSQ IND BY
%token <val> CPRS EXPD START STOP MAX MIN ABS MERGE MDIVR MDIVQ DIVR DIVQ DIVS RECIP RSQRT SINGLE DOUBLE MR INT RMAX RMIN
%token <val> REPEAT LOOP JMP MPUSTOP
%token <val> BR CR APP KPP CI F U P R T B H S D I L TC C CFLAG LABEL SHU BIU SHIFT0 SHIFT1 SHIFT2 SHIFT3 SEND STEST TTEST
%token <val> TRUE ASSIGN NOOP UINT DM KG R0 R1 R2 R3 R4 R5 IPATH WFLAG
%token <string> IDENTIFIER
%token <op> EXPR
%token <val> TREG MINDEXN KI

%type <val> slots slotref slot 
%type <val> mr012345slot shuslot shu0code shu1code shu2code shu0inst shu1inst shu2inst biu0 biu1 biu2 biu0t biu1t biu2t shut shu0t shu1t shu2t
%type <val> r0inst r1inst r2inst r3inst r4inst r5inst maccdestp maccdest ialut imact ifalut ifmact
%type <val> ucpshusrcTm ucpshusrcTn ucpindtkclause ucpshusrcTk ucpindtbclause ucpshuexp ucpindclause shu0dest mindexs mindexi mindexn ialuasclause
%type <val> ialudest ifaludest imacdest ifmacdest biut imulreal imulcomp imacclause ifmacclause
%type <val> ialu imac falu fmac ifalu ifmac imm imm5 mcodeline hmacro _flag flag_ constt _constt

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
};
slot: mr012345slot {ADDOPERAND(Slot, slotid, @$.S, @$.E);} |
      shuslot {ADDOPERAND(Slot, 6 + $1, @$.S, @$.E);} |
      ialuslot {ADDOPERAND(Slot, 9, @$.S, @$.E);} |
      ifaluslot {ADDOPERAND(Slot, 10, @$.S, @$.E);} |
      imacslot {ADDOPERAND(Slot, 11, @$.S, @$.E);} |
      ifmacslot {ADDOPERAND(Slot, 12, @$.S, @$.E);} |
 //     biuslot {ADDOPERAND(Slot, 10 + slotid, @$.S, @$.E);} |
 //     seqslot |
 //     hmacro |
      error { llvmerror(&@1, "Unrecognized slot."); YYABORT; };
hmacro: IDENTIFIER {ADDOPERAND(HMacro, $1, @$.S, @$.E);};
hmacro: IDENTIFIER LPAREN RPAREN {ADDOPERAND(HMacro, $1, @$.S, @$.E); ADDOPERAND(Imm, 1, @$.S, @$.E);};

//yangl
mr012345slot: R0 DOT r0inst {
  switch ($3) {
  case 0:
    //r0inst: mindexn -> r0dest
    ADDOPERAND(Opc, UCPM::MR0ToDestCom, @$.S, @$.E);
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit2));
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(md));
    break;
  case 1:
    //r0inst: (mindexi or mindexs) -> r0dest
    ADDOPERAND(Opc, UCPM::MR0ToDestSI, @$.S, @$.E);
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit2));
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(md));
  default:
    //OS<<"Parsing R0 DOT r0inst fails!"<<"\n";
    break;
  }
  slotid = 0;
}
| R1 DOT r1inst {
  switch ($3) {
  case 0:
    ADDOPERAND(Opc, UCPM::MR1ToDestCom, @$.S, @$.E);
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit2));
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(md));
    break;
  case 1:
    ADDOPERAND(Opc, UCPM::MR1ToDestSI, @$.S, @$.E);
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit2));
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(md));
  default:
    break;
  }
  slotid = 1;
}
| R2 DOT r2inst {
  switch ($3) {
  case 0:
    ADDOPERAND(Opc, UCPM::MR2ToDestCom, @$.S, @$.E);
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit2));
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(md));
    break;
  case 1:
    ADDOPERAND(Opc, UCPM::MR2ToDestSI, @$.S, @$.E);
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit2));
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(md));
  default:
    break;
  }
  slotid = 2;
}
| R3 DOT r3inst {
  switch ($3) {
  case 0:
    ADDOPERAND(Opc, UCPM::MR3ToDestCom, @$.S, @$.E);
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit2));
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(md));
    break;
  case 1:
    ADDOPERAND(Opc, UCPM::MR3ToDestSI, @$.S, @$.E);
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit2));
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(md));
  default:
    break;
  }
  slotid = 3;
}
| R4 DOT r4inst {
  switch ($3) {
  case 0:
    ADDOPERAND(Opc, UCPM::MR4ToDestCom, @$.S, @$.E);
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit2));
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(md));
    break;
  case 1:
    ADDOPERAND(Opc, UCPM::MR4ToDestSI, @$.S, @$.E);
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit2));
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(md));
  default:
    break;
  }
  slotid = 4;
}
| R5 DOT r5inst {
  switch ($3) {
  case 0:
    ADDOPERAND(Opc, UCPM::MR5ToDestCom, @$.S, @$.E);
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit2));
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(md));
    break;
  case 1:
    ADDOPERAND(Opc, UCPM::MR5ToDestSI, @$.S, @$.E);
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit2));
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(md));
  default:
    break;
  }
  slotid = 5;
};

r0inst: mindexn ASSIGNTO r0dest {$$ = 0} |
        mindexi ASSIGNTO r0dest {$$ = 1} |
        mindexs ASSIGNTO r0dest {$$ = 1} ;

r1inst: mindexn ASSIGNTO r1dest {$$ = 0} |
        mindexi ASSIGNTO r1dest {$$ = 1} |
        mindexs ASSIGNTO r1dest {$$ = 1} ;

r2inst: mindexn ASSIGNTO r2dest {$$ = 0} |
        mindexi ASSIGNTO r2dest {$$ = 1} |
        mindexs ASSIGNTO r2dest {$$ = 1} ;

r3inst: mindexn ASSIGNTO r3dest {$$ = 0} |
        mindexi ASSIGNTO r3dest {$$ = 1} |
        mindexs ASSIGNTO r3dest {$$ = 1} ;
        
r4inst: mindexn ASSIGNTO r4dest {$$ = 0} |
        mindexi ASSIGNTO r4dest {$$ = 1} |
        mindexs ASSIGNTO r4dest {$$ = 1} ;
        
r5inst: mindexn ASSIGNTO r5dest {$$ = 0} |
        mindexi ASSIGNTO r5dest {$$ = 1} |
        mindexs ASSIGNTO r5dest {$$ = 1} ;

mr012345slot: R0 DOT error { llvmerror(&@3, "Incorrect M.r0 inst."); YYABORT;} |
           R1 DOT error { llvmerror(&@3, "Incorrect M.r1 inst."); YYABORT;} |
           R2 DOT error { llvmerror(&@3, "Incorrect M.r2 inst."); YYABORT;} |
           R3 DOT error { llvmerror(&@3, "Incorrect M.r3 inst."); YYABORT;} |
           R4 DOT error { llvmerror(&@3, "Incorrect M.r4 inst."); YYABORT;} |
           R5 DOT error { llvmerror(&@3, "Incorrect M.r5 inst."); YYABORT;} ;
           

shuslot: shu0code {$$ = 0;} | shu1code {$$ = 1;} | shu2code {$$ = 2} ;
shu0code: SHU0 DOT shu0inst ;
shu1code: SHU1 DOT shu1inst ;
shu2code: SHU2 DOT shu2inst ;

shu0inst: ucpshuexp ASSIGNTO shu0dest {
  switch ($3) {
  case 0:
    
    break;
  case 1:
    
    break;
  case 2:
    
    break;
  case 3://ialut, imact, ifalut, ifmacct
    switch ($1) {
    case 0: //+=
      ADDOPERAND(Opc, UCPM::SHU0Ind_SetTBToMACC_0, @$.S, @$.E); 
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
    default : break;
    }
  default: break;
  }

};
shu1inst: ucpshuexp ASSIGNTO shu0dest {
  switch ($3) {
  case 0:
    
    break;
  case 1:
    
    break;
  case 2:
    
    break;
  case 3://ialut, imact, ifalut, ifmacct
    switch ($1) {
    case 0: //+=
      ADDOPERAND(Opc, UCPM::SHU1Ind_SetTBToMACC_0, @$.S, @$.E); 
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
    default : break;
    }
  default: break;
  }

};
shu2inst: ucpshuexp ASSIGNTO shu0dest {
  switch ($3) {
  case 0:
    
    break;
  case 1:
    
    break;
  case 2:
    
    break;
  case 3://ialut, imact, ifalut, ifmacct
    switch ($1) {
    case 0: //+=
      ADDOPERAND(Opc, UCPM::SHU2Ind_SetTBToMACC_0, @$.S, @$.E); 
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
    default : break;
    }
  default: break;
  }

};
ucpshuexp: LBRACE ucpshusrcTm ucpshusrcTn RBRACE IND ucpindclause {$$ = $6};
ucpshusrcTm: t;
ucpshusrcTn: t {$$ = 1;}| 
             constt {$$ = 2;};
ucpindclause: ucpindtkclause | ucpindtbclause ;
ucpindtkclause: ucpshusrcTk _flag TB ACC2 IMM5 flag_ {$$ = 1; imm = OPERAND(Imm, $5, @5.S, @5.E);}//"=+"case
|               ucpshusrcTk {$$ = 2;};
ucpindtkclause: ucpshusrcTk _flag error flag_ { llvmerror(&@2, "IND Tk must be followed by (TB =+ Imm)"); YYABORT;};
ucpindtbclause: TB _flag TB ACC1 IMM5 flag_ {$$ = 0; imm = OPERAND(Imm, $5, @5.S, @5.E);}//"+="case
|            TB {$$ = 3;};
ucpindtkclause: TB _flag error flag_ { llvmerror(&@2, "IND TB must be followed by (TB += Imm)"); YYABORT;};
ucpshusrcTk: t {$$ = 1;}| 
             constt {$$ = 2;};



/*Destination types: 0:biu,1:unit,2:m */
//yangl 
//here biu0t is only a symbol, t is always 't0'
r0dest: ialut | imact | shu0t | biu0t ;
r1dest: ialut | imact | shu1t ;
r2dest: ialut | imact | shu2t | biu2t ;
r3dest: ifalut | ifmact | shu0t ;
r4dest: ifalut | ifmact | shu1t | biu1t ;
r5dest: ifalut | ifmact | shu2t | biu2t ;
biut: biu0t | biu1t | biu2t ;

shu0dest: maccdestp {
               /*if(ipath->getImm() >= 3) {
                 llvmerror(&@1, "SHU to Macc must use port 'I0 -I2'"); 
                 YYABORT;
               }*/
             };
             
maccdestp: maccdest LPAREN IPATH RPAREN {$$ = $1; ipath = OPERAND(Imm, $3, @3.S, @3.E);};
maccdest: ialut | imact | ifalut | ifmact ;
mindex: mindexi | mindexn ;

ialut: ialu DOT t {$$ = 3; unit = OPERAND(Reg, UCPMReg::IALU, @1.S, @1.E); unit2 = OPERAND(Reg, UCPMReg::MTIALU, @1.S, @1.E); if (!ut) ut = tk ? tk : (tp ? tp : (tn ? tn : tm));};
imact: imac DOT t {$$ = 3; unit = OPERAND(Reg, UCPMReg::IMAC, @1.S, @1.E); unit2 = OPERAND(Reg, UCPMReg::MTIMAC, @1.S, @1.E); if (!ut) ut = tk ? tk : (tp ? tp : (tn ? tn : tm));};
ifalut: ifalu DOT t {$$ = 3; unit = OPERAND(Reg, UCPMReg::IFALU, @1.S, @1.E); unit2 = OPERAND(Reg, UCPMReg::MTIFALU, @1.S, @1.E); if (!ut) ut = tk ? tk : (tp ? tp : (tn ? tn : tm));};
ifmact: ifmac DOT t {$$ = 3; unit = OPERAND(Reg, UCPMReg::IFMAC, @1.S, @1.E); unit2 = OPERAND(Reg, UCPMReg::MTIFMAC, @1.S, @1.E); if (!ut) ut = tk ? tk : (tp ? tp : (tn ? tn : tm));};

biu0t: biu0 {$$ = 3; unit = b; unit2 = b2;
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
biu2: BIU2 {$$ = 2; b = OPERAND(Reg, UCPMReg::BIU2, @$.S, @$.E); b2 = OPERAND(Reg, UCPMReg::MTBIU2, @$.S, @$.E);};
shu0t: SHU0 DOT t {$$ = 1; unit = OPERAND(Reg, UCPMReg::SHU0, @1.S, @1.E); unit2 = OPERAND(Reg, UCPMReg::MTSHU0, @1.S, @1.E); if (!ut) ut = tk ? tk : (tp ? tp : (tn ? tn : tm));};
shu1t: SHU1 DOT t {$$ = 1; unit = OPERAND(Reg, UCPMReg::SHU1, @1.S, @1.E); unit2 = OPERAND(Reg, UCPMReg::MTSHU1, @1.S, @1.E); if (!ut) ut = tk ? tk : (tp ? tp : (tn ? tn : tm));};
shu2t: SHU2 DOT t {$$ = 1; unit = OPERAND(Reg, UCPMReg::SHU2, @1.S, @1.E); unit2 = OPERAND(Reg, UCPMReg::MTSHU2, @1.S, @1.E); if (!ut) ut = tk ? tk : (tp ? tp : (tn ? tn : tm));};
mindexs: MINDEXS {$$ = 0; ms = md; md = OPERAND(Reg, UCPMReg::MSPP, @$.S, @$.E);};
mindexi: MINDEXI {$$ = 0; ms = md; md = OPERAND(Reg, UCPMReg::MIPP, @$.S, @$.E);};
mindexn: MINDEXN {$$ = 0; ms = md;
  md = OPERAND(Reg,
               MRI->getRegClass(UCPMReg::MRegRegClassID).getRegister($1),
               @$.S, @$.E);
};

ialuslot: IALU DOT ialutodest ;
ialutodest: ialuasclause ASSIGNTO ialudest {
  flagsort = (flags[UF] << 5) | (flags[BF] << 4) | (flags[SF] << 3) | (flags[TF] << 2) | (flags[CIF] << 1) | flags[FF];
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
  Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
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
  Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
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
  Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
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
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
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
      Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
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


//ducx start ifmac
ifmacslot: ifmacinst ;
ifmacinst: ifmacclause ASSIGNTO ifmacdest {
    OS<<"Position 1\n";
    flagsort = flags[TTF];
    f = OPERAND(Imm, flagsort, FlagS, FlagE);
    flags.reset();
    
    switch ($3) {
    case 1://to shu
      ADDOPERAND(Opc, UCPM::IFMACToSHU, @$.S, @$.E);
      break;
    case 2://to macc
      ADDOPERAND(Opc, UCPM::IFMACToMACC, @$.S, @$.E);
      break;
    case 3://to biu
      ADDOPERAND(Opc, UCPM::IFMACToBIU, @$.S, @$.E);
      break;
    default:
      break;
  }
  Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
  Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
  Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(opc));
  Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
  Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
  Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(f));
};

ifmacclause: fmul {OS<<"Position 8\n";opc = OPERAND(Reg, UCPMReg::FMUL, @$.S, @$.E); } |
            ifaddclause {opc = OPERAND(Reg, UCPMReg::FADD, @$.S, @$.E);} |
            ifsubclause {opc = OPERAND(Reg, UCPMReg::FSUB, @$.S, @$.E);};
            
fmul: imulexp _flag ifmacflag flag_ _flag ifmacTflags flag_ | 
      imulexp _flag ifmacflag flag_ {OS<<"Position 7\n";};
      
ifaddclause: addexp _flag ifmacTflags flag_ | addexp ;
ifsubclause: subexp _flag ifmacTflags flag_ | subexp ;
              

ifmacdest: imacdest;
// ducx end ifmac



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
imm5: IMM3 | IMM5;
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
ifmacflag:     STEST   {OS<<"Position 6\n";flags.set(SSF);};
ifmacTflags:   TTEST   {flags.set(TTF);};