//===-- UCPSAsmParser.cpp - Parse UCPS asm to MCInst instructions -----===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "../MCTargetDesc/UCPSMCTargetDesc.h"

#include "llvm/MC/MCParser/MCAsmLexer.h"
#include "llvm/MC/MCParser/MCAsmParser.h"
#include "llvm/MC/MCParser/MCAsmParserExtension.h"
#include "llvm/MC/MCParser/MCTargetAsmParser.h"

#include "llvm/MC/MCParser/MCParsedAsmOperand.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"

#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/raw_ostream.h"

#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/Twine.h"

using namespace llvm;

/// UCPSAsmOperand - Instances of this class represent operands of parsed UCPS assembly instructions.
/// Note: the first operand records instruction opcode, see ARMGenAsmMatcher.inc MatchInstructionImpl().
namespace llvm {
namespace UCPS {
struct InstLineToParse {
  OperandVector *Operands;
  SMLoc *CurLoc;
  MCAsmParser* Parser;
};

// to tag the position where this instruction stands: at the end or not.
enum InstPos {
  NotLineEnd,
  LineEnd
};

enum AsmOperandKind {
  AsmOpc,
  AsmReg,
  AsmImm,
  AsmFPImm,
  AsmExpr,
  AsmPos
};

struct UCPSAsmOperand: public MCParsedAsmOperand {
private:
  enum AsmOperandKind Kind;

  SMLoc StartLoc, EndLoc;

  union {
    unsigned Opc;    // the opcode.
    unsigned Reg;
    int64_t Imm;
    double FPImm;
    const MCExpr* Expr;
    enum InstPos Pos;
  };

public:
  UCPSAsmOperand(const UCPSAsmOperand &o)
    : MCParsedAsmOperand() {
    Kind = o.Kind;
    StartLoc = o.StartLoc;
    EndLoc = o.EndLoc;

    switch (Kind) {
    case AsmReg:
      Reg = o.Reg;
      break;
    case AsmImm:
      Imm = o.Imm;
      break;
    case AsmFPImm:
      FPImm = o.FPImm;
      break;
    case AsmExpr:
      Expr = o.Expr;
      break;
    case AsmOpc:
      Opc = o.Opc;
      break;
    case AsmPos:
      Pos = o.Pos;
      break;
    default:
      llvm_unreachable("undefined Asm Operand for UCPS");
    }
  }

  UCPSAsmOperand(AsmOperandKind K)
    : MCParsedAsmOperand(), Kind(K) {
  }

  /// getStartLoc - Get the location of the first token of this operand.
  SMLoc getStartLoc() const {
    return StartLoc;
  }

  /// getEndLoc - Get the location of the last token of this operand.
  SMLoc getEndLoc() const {
    return EndLoc;
  }

  unsigned getKind() const {
    return Kind;
  }

  unsigned getReg() const {
    assert(Kind == AsmReg && "Invalid access!");
    return Reg;
  }

  unsigned getOpc() const {
    assert(Kind == AsmOpc && "Invalid access!");
    return Opc;
  }

  int64_t getImm() const {
    assert(Kind == AsmImm && "Invalid access!");
    return Imm;
  }

  int64_t getFPImm() const {
    assert(Kind == AsmFPImm && "Invalid access!");
    return FPImm;
  }

  const MCExpr *
  getExpr() const {
    assert(Kind == AsmExpr && "Invalid access!");
    return Expr;
  }

  enum InstPos getPos() const {
    assert(Kind == AsmPos && "Invalid access!");
    return Pos;
  }

  bool isToken() const {
    return Kind == AsmOpc;
  }
  bool isImm() const {
    return Kind == AsmImm;
  }
  bool isFPImm() const {
    return Kind == AsmFPImm;
  }

  bool isPos() const {
    return Kind == AsmPos;
  }

  bool isReg() const {
    return Kind == AsmReg;
  }

  bool isMem() const {
    return false;
  }

  bool isExpr() const {
    return Kind == AsmExpr;
  }

  /// print - Print a debug representation of the operand to the given stream.
  virtual void
  print(raw_ostream &OS) const;

  static UCPSAsmOperand *
  createOpc(unsigned opcode) {
    UCPSAsmOperand *Op = new UCPSAsmOperand(AsmOpc);
    Op->Opc = opcode;
    return Op;
  }

  static UCPSAsmOperand *
  createReg(unsigned RegNum) {
    UCPSAsmOperand *Op = new UCPSAsmOperand(AsmReg);
    Op->Reg = RegNum;
    return Op;
  }

  static UCPSAsmOperand *
  createPos(enum InstPos pos) {
    UCPSAsmOperand *Op = new UCPSAsmOperand(AsmPos);
    Op->Pos = pos;
    return Op;
  }

  static UCPSAsmOperand *
  createImm(int64_t imm) {
    UCPSAsmOperand *Op = new UCPSAsmOperand(AsmImm);
    Op->Imm = imm;
    return Op;
  }

  static UCPSAsmOperand *
  createFPImm(double imm) {
    UCPSAsmOperand *Op = new UCPSAsmOperand(AsmFPImm);
    Op->FPImm = imm;
    return Op;
  }

  static UCPSAsmOperand *
  createExpr(const MCExpr * expr) {
    UCPSAsmOperand *Op = new UCPSAsmOperand(AsmExpr);
    Op->Expr = expr;
    return Op;
  }
};

/*******************************************************************/

class UCPSAsmParser: public MCTargetAsmParser {
private:
  const MCSubtargetInfo &STI;

  static MCAsmParser *Parser;
  static const MCRegisterInfo *RegInfo;

  static unsigned PreToken;
  static SMLoc PreLoc;

public:
  UCPSAsmParser(const MCSubtargetInfo &sti, MCAsmParser &par, const MCInstrInfo &MII,
                const MCTargetOptions &Options)
    : MCTargetAsmParser(Options, sti), STI(sti) {
    Parser = &par;
    RegInfo = Parser->getContext().getRegisterInfo();
    //TheUCPSTarget.createMCRegInfo("");

    PreToken = 0;
    PreLoc = SMLoc();
  }

  virtual bool ParseRegister(unsigned &RegNo, SMLoc &StartLoc, SMLoc &EndLoc)
    override {
    return true;
  }

  virtual bool ParseDirective(AsmToken DirectiveID) override {
    return true;
  }

  virtual void convertToMapAndConstraints(unsigned Kind,
                                          const OperandVector &Operands)
                                            override {}

  virtual bool
  ParseInstruction(ParseInstructionInfo &Info,
                   StringRef Name,
                   SMLoc InstLoc,
                   OperandVector &Operands) override;

  virtual bool
  MatchAndEmitInstruction(SMLoc IDLoc,
                          unsigned &Opcode,
                          OperandVector &Operands,
                          MCStreamer &Out,
                          uint64_t &ErrorInfo,
                          bool MatchingInlineAsm) override;

  static const MCRegisterInfo *
  getRegInfo() {
    return RegInfo;
  }

  static MCAsmParser* getMCParser() {
    return Parser;
  }

  static unsigned getPreToken() {
    return PreToken;
  }

  static void setPreToken(unsigned val) {
    PreToken = val;
  }

  static SMLoc& getPreLoc() {
    return PreLoc;
  }

  static void setPreLoc(const SMLoc& loc) {
    PreLoc = loc;
  }
};
}
}

/************************************ start ragel and bison *****************************************/
// mmpuinstparse() --> mmpuinstlex() --> mmputoklex()
#include "UCPSGenInstrParser.h"

// a direct wrapper for lexer generated by ragel.
// start is modified when this wrapper returns.
static int ucpstoklex(YYSTYPE *yylval, MCAsmParser* parser, SMLoc &CurLoc);
#include "UCPSGenInstrRLexer.ragel"

// called implicitly by bison-generated parser ucpsinstparse(Operands, Ref)
static int ucpsinstlex(YYSTYPE * yylval, UCPS::InstLineToParse &InstLine) {
  // if previous token is End-Of-Statement, we need to fake an EOF token
  // to feed bison parser.
  AsmLexer & mclexer = static_cast<AsmLexer&>(InstLine.Parser->getLexer());
  int token = ucpstoklex(yylval, InstLine.Parser, *InstLine.CurLoc);
  while (InstLine.CurLoc->getPointer() >=
         mclexer.getTok().getEndLoc().getPointer())
    mclexer.Lex();

  // we call llvm parser to analyse expression
  switch (token) {
  case '(':
  case _Symb:
  case _Imm: {
    SMLoc nextLoc;
    /*mclexer.setCurPtr(loc.getPointer());*/

    const MCExpr * expr;
    bool hadErr;
    hadErr = InstLine.Parser->parseExpression(expr, nextLoc);
    if (!hadErr) {
      yylval->op = UCPS::UCPSAsmOperand::createExpr(expr);
      *InstLine.CurLoc = nextLoc;
      token = _Expr;
    } else token = _Err;

    break;
  }

  default:
    break;
  }
  return token;
}

/* error handling for bison generated yyparse() */
static void ucpsinsterror(UCPS::InstLineToParse &InstLine,
                          const char * errBuf) {
  // Fixme:
  // custom parsing error handling.
  //UCPS::UCPSAsmParser::getMCParser()->Error(SMLoc::getFromPointer(InstLine.Line->data()), "error incoming input");
}

#include "UCPSGenInstrParser.bison"
/************************************ end ragel and bison *****************************************/

// print - Print a debug representation of the operand to the given stream.
void UCPS::UCPSAsmOperand::print(raw_ostream &OS) const {
  switch (Kind) {
  default:
    break;
  case AsmImm:
    break;
  }
}

const MCRegisterInfo* UCPS::UCPSAsmParser::RegInfo;
MCAsmParser * UCPS::UCPSAsmParser::Parser;

unsigned UCPS::UCPSAsmParser::PreToken;
SMLoc UCPS::UCPSAsmParser::PreLoc;

bool UCPS::UCPSAsmParser::MatchAndEmitInstruction(SMLoc IDLoc,
                                                  unsigned &Opcode,
                                                  OperandVector &Operands,
                                                  MCStreamer &Out,
                                                  uint64_t &ErrorInfo,
                                                  bool MatchingInlineAsm) {
  const MCExpr * expr;
  MCInst *InstHead = new MCInst(), *prevInst = NULL, *curInst = InstHead,
    *secInst = NULL;
  UCPSAsmOperand * op;
  bool isTail;
  //bool isHead = true;
  unsigned n = Operands.size();
  MCOperand MO;

  for (unsigned i = 0; i < n; ++i) {
    op = static_cast<UCPSAsmOperand *>(Operands[i].get());
    switch (op->getKind()) {
    // this is the last push-backed operand for an instruction.
    case AsmOpc:
      isTail = i == n - 1;
      /*if (isTail) curInst->setEnd(true);
      else curInst->setEnd(false);

      if (isHead) {
        curInst->setStart(true);
        isHead = false;
      } else curInst->setStart(false);*/

      curInst->setOpcode(op->getOpc());
      curInst->setLoc(IDLoc);
      if (prevInst && prevInst != InstHead)
    	prevInst->addOperand(MCOperand::createInst(curInst));
      prevInst = curInst;
      if (!isTail) {
    	  if (curInst == InstHead) {
    	    secInst = new MCInst();
    	    curInst = secInst;
    	  } else curInst = new MCInst();
      }
      break;
    case AsmReg:
      curInst->addOperand(MCOperand::createReg(op->getReg()));
      break;
    case AsmImm:
      curInst->addOperand(MCOperand::createImm(op->getImm()));
      break;
    case AsmFPImm:
      curInst->addOperand(MCOperand::createFPImm(op->getFPImm()));
      break;
    case AsmExpr:
      expr = op->getExpr();
      /*
       symbOld = Parser->getContext().LookupSymbol(*ref);
       symbNew = Parser->getContext().GetOrCreateSymbol(*ref);

       if(!symbOld) {// symbol does not exists beforehand.
       Parser->getStreamer().EmitSymbolAttribute(symbNew, MCSA_Global);
       }

       delete( ref ); // allocated for symbol name in flex file

       expr = MCSymbolRefExpr::Create(symbNew, MCSymbolRefExpr::VK_None, Parser->getContext());
       */
      MO = MCOperand::createExpr(expr);
      curInst->addOperand(MO);
      if (curInst != InstHead) InstHead->addOperand(MO);
      break;

    default:
      llvm_unreachable("unknown operand");
    }
  }
  if (secInst) InstHead->addOperand(MCOperand::createInst(secInst));
  Out.EmitInstruction(*InstHead, STI);

  return false;    // any error?
}

/// Parse an UCPS instruction mnemonic followed by its operands.
bool UCPS::UCPSAsmParser::ParseInstruction(ParseInstructionInfo &Info,
                                           StringRef Name,
                                           SMLoc InstLoc,
                                           OperandVector &Operands) {
  MCAsmParser *mcparser = UCPS::UCPSAsmParser::getMCParser();
  AsmLexer &mclexer = static_cast<AsmLexer &>(mcparser->getLexer());

  SMLoc CurLoc = mclexer.getLoc();    // the position for current token start.
  InstLineToParse instLine = { &Operands, &CurLoc, mcparser};

  // parsing based on ragel lexer.
  if (Name != "m.s") {
    Parser->Error(InstLoc, "error instruction line");
    return true;
  }

  int err = ucpsinstparse(instLine);

  if (err) {
    mcparser->Error(CurLoc, "Parse Instruction failed");
    while (mclexer.isNot(AsmToken::EndOfStatement)) Lex();
    if (mclexer.is(AsmToken::EndOfStatement)) Lex();
    return true;
  }

  return false;
}

/// Force static initialization.
extern "C" void LLVMInitializeUCPSAsmParser() {
  RegisterMCAsmParser<UCPS::UCPSAsmParser> Y(TheUCPSTarget);
}

/*
 #define GET_REGISTER_MATCHER
 #define GET_MATCHER_IMPLEMENTATION
 #include "UCPSGenAsmMatcher.inc"
 */
