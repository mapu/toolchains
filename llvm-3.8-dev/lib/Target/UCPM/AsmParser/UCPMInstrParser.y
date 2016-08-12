                   %{
#include <string>
#include <bitset>

std::bitset<16> flags;
static int flagsort;
const unsigned BF=0, HF=1, UF=2, TF=3, SF=4, DF=5, IF=6, LF=7, APPF=8, KPPF=9, CRF=10, BRF=11, MF=12, TCF=13, CDF=14, NCF=15;
static UCPM::UCPMAsmOperand *opc, *tm, *tn, *tk, *tp, *f, *unit, *ut, *b, *md, *ms, *imm, *expr, *ipath;
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
%token <val> BR CR APP KPP U T B H S D I L TC C LABEL SHU BIU 
%token <val> TRUE ASSIGN NOOP UINT DM KG R0 R1 R2 R3 R4 R5 IPATH WFLAG
%token <string> IDENTIFIER
%token <op> EXPR
%token <val> TREG MINDEXN KI

%type <val> slots slotref slot 
%type <val> mr012345slot shuslot shu0code shu1code shu2code shu0inst shu1inst shu2inst biu0 biu1 biu2 biu0t biu1t biu2t shut shu0t shu1t shu2t
%type <val> r0inst r1inst r2inst r3inst r4inst r5inst maccdestp maccdest ialut imact ifalut ifmact ucpifalut ucpifmact
%type <val> ucpshusrcTm ucpshusrcTn ucpindtkclause ucpshusrcTk ucpindtbclause ucpshuexp ucpindclause shu0dest mindexs mindexi mindexn ialuclause
%type <val> ialudest biut
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
slot: mr012345slot {ADDOPERAND(Slot, slotid, @$.S, @$.E);} |
      shuslot {ADDOPERAND(Slot, 6 + $1, @$.S, @$.E);} |
      ialuslot {ADDOPERAND(Slot, 9, @$.S, @$.E);} |
//      imacslot {ADDOPERAND(Slot, 7, @$.S, @$.E);} |
//      faluslot {ADDOPERAND(Slot, 8, @$.S, @$.E);} |
//      fmacslot {ADDOPERAND(Slot, 9, @$.S, @$.E);} |
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
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(md));
    break;
  case 1:
    //r0inst: (mindexi or mindexs) -> r0dest
    ADDOPERAND(Opc, UCPM::MR0ToDestSI, @$.S, @$.E);
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
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
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(md));
    break;
  case 1:
    ADDOPERAND(Opc, UCPM::MR1ToDestSI, @$.S, @$.E);
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
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
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(md));
    break;
  case 1:
    ADDOPERAND(Opc, UCPM::MR2ToDestSI, @$.S, @$.E);
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
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
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(md));
    break;
  case 1:
    ADDOPERAND(Opc, UCPM::MR3ToDestSI, @$.S, @$.E);
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
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
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(md));
    break;
  case 1:
    ADDOPERAND(Opc, UCPM::MR4ToDestSI, @$.S, @$.E);
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
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
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(md));
    break;
  case 1:
    ADDOPERAND(Opc, UCPM::MR5ToDestSI, @$.S, @$.E);
    Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
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
maccdest: ialut | imact | ucpifalut | ucpifmact ;
mindex: mindexi | mindexn ;

ialut: ialu DOT t {$$ = 3; unit = OPERAND(Reg, UCPMReg::IALU, @1.S, @1.E); if (!ut) ut = tk ? tk : (tp ? tp : (tn ? tn : tm));};
imact: imac DOT t {$$ = 3; unit = OPERAND(Reg, UCPMReg::IMAC, @1.S, @1.E); if (!ut) ut = tk ? tk : (tp ? tp : (tn ? tn : tm));};
ifalut: ifalu DOT t {$$ = 3; unit = OPERAND(Reg, UCPMReg::IFALU, @1.S, @1.E); if (!ut) ut = tk ? tk : (tp ? tp : (tn ? tn : tm));};
ifmact: ifmac DOT t {$$ = 3; unit = OPERAND(Reg, UCPMReg::IFMAC, @1.S, @1.E); if (!ut) ut = tk ? tk : (tp ? tp : (tn ? tn : tm));};
//yangl
ucpifalut: ifalu DOT t {$$ = 3; unit = OPERAND(Reg, UCPMReg::UCPIFALU, @1.S, @1.E); if (!ut) ut = tk ? tk : (tp ? tp : (tn ? tn : tm));};
ucpifmact: ifmac DOT t {$$ = 3; unit = OPERAND(Reg, UCPMReg::UCPIFMAC, @1.S, @1.E); if (!ut) ut = tk ? tk : (tp ? tp : (tn ? tn : tm));};
biu0t: biu0 {$$ = 3; unit = b;
             unsigned treg = MRI->getRegClass(UCPMReg::TPortRegClassID).getRegister(0);//here 't0' is assigned to biu0t
             ut = OPERAND(Reg, treg, @$.S, @$.E); }
biu1t: biu1 {$$ = 3; unit = b;
             unsigned treg = MRI->getRegClass(UCPMReg::TPortRegClassID).getRegister(0);//here 't0' is assigned to biu0t
             ut = OPERAND(Reg, treg, @$.S, @$.E); }
biu2t: biu2 {$$ = 3; unit = b;
             unsigned treg = MRI->getRegClass(UCPMReg::TPortRegClassID).getRegister(0);//here 't0' is assigned to biu0t
             ut = OPERAND(Reg, treg, @$.S, @$.E); }
biu0: BIU0 {$$ = 2; b = OPERAND(Reg, UCPMReg::BIU0, @$.S, @$.E);};
biu1: BIU1 {$$ = 2; b = OPERAND(Reg, UCPMReg::BIU1, @$.S, @$.E);};
biu2: BIU2 {$$ = 2; b = OPERAND(Reg, UCPMReg::BIU2, @$.S, @$.E);};
shu0t: SHU0 DOT t {$$ = 1; unit = OPERAND(Reg, UCPMReg::SHU0, @1.S, @1.E); if (!ut) ut = tk ? tk : (tp ? tp : (tn ? tn : tm));};
shu1t: SHU1 DOT t {$$ = 1; unit = OPERAND(Reg, UCPMReg::SHU1, @1.S, @1.E); if (!ut) ut = tk ? tk : (tp ? tp : (tn ? tn : tm));};
shu2t: SHU2 DOT t {$$ = 1; unit = OPERAND(Reg, UCPMReg::SHU2, @1.S, @1.E); if (!ut) ut = tk ? tk : (tp ? tp : (tn ? tn : tm));};
mindexs: MINDEXS {$$ = 0; ms = md; md = OPERAND(Reg, UCPMReg::MSPP, @$.S, @$.E);};
mindexi: MINDEXI {$$ = 0; ms = md; md = OPERAND(Reg, UCPMReg::MIPP, @$.S, @$.E);};
mindexn: MINDEXN {$$ = 0; ms = md;
  md = OPERAND(Reg,
               MRI->getRegClass(UCPMReg::MRegRegClassID).getRegister($1),
               @$.S, @$.E);
};
//***
ialuslot: ialuclause ASSIGNTO ialudest {
  ADDOPERAND(Opc, UCPM::IALUASToSHU, @$.S, @$.E);
  Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(unit));
  Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(ut));//unit't
  Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(opc));//ADD or SUB
  Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tm));
  Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(tn));
  
  Operands.push_back(std::unique_ptr<UCPM::UCPMAsmOperand>(f));
}
ialuclause: iaddclause {$$ = 2; opc = OPERAND(Reg, UCPMReg::f_IADD, @$.S, @$.E);f = OPERAND(Imm, 0, @$.S, @$.E);};//|
           //isubclause {$$ = 2; opc = OPERAND(Reg, UCPMReg::ISUB, @$.S, @$.E);};
iaddclause: addexp;
//iaddclause: addexp _flag error flag_ {llvmerror(&@3, "Invalid flag for \"Tm + Tn\". Available flags for IALU are U, T, B/H, and for FALU are T, S/D."); YYABORT;};
//isubclause: subexp _flag utbflag flag_ | subexp ;
//isubclause: subexp _flag error flag_ {llvmerror(&@3, "Invalid flag for \"Tm - Tn\". Available flags for IALU are U, T, B/H, and for FALU are T, S/D."); YYABORT;};
addexp: t ADD t ;
//subexp: t SUB t ;
ialudest: shut | maccdest | biut ;


shut: shu0t | shu1t | shu2t ;


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