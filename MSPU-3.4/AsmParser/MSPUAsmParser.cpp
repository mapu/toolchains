//===-- MSPUAsmParser.cpp - Parse MSPU asm to MCInst instructions -----===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "../MCTargetDesc/MSPUMCTargetDesc.h"
#include "../MCTargetDesc/MSPUMCInst.h"

#include "llvm/MC/MCParser/MCAsmLexer.h"
#include "llvm/MC/MCParser/MCAsmParser.h"
#include "llvm/MC/MCParser/MCAsmParserExtension.h"
#include "llvm/MC/MCTargetAsmParser.h"

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

/// MSPUAsmOperand - Instances of this class represent operands of parsed MSPU assembly instructions.
/// Note: the first operand records instruction opcode, see ARMGenAsmMatcher.inc MatchInstructionImpl().
namespace llvm
{
	namespace MSPU
	{
	  struct InstLineToParse
	  {
	    SmallVectorImpl<MCParsedAsmOperand*> *Operands;
	    StringRef* Line;
	  };

	  // to tag the position where this instruction stands: at the end or not.
    enum InstPos
    {
      NotLineEnd,
      LineEnd
    };

		enum AsmOperandKind
		{
			AsmOpc,
			AsmReg,
			AsmImm,
			AsmFPImm,
			AsmExpr,
			AsmPos
		};

		struct MSPUAsmOperand: public MCParsedAsmOperand
		{
			private:
				enum AsmOperandKind Kind;

				SMLoc StartLoc, EndLoc;

				union
				{
						unsigned  Opc; // the opcode.
						unsigned  Reg;
						int64_t   Imm;
						double   FPImm;
						const MCExpr* Expr;
						enum InstPos  Pos;
				};

			public:
				MSPUAsmOperand(const MSPUAsmOperand &o)
							: MCParsedAsmOperand()
				{
					Kind = o.Kind;
					StartLoc = o.StartLoc;
					EndLoc = o.EndLoc;

					switch(Kind) {
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
              llvm_unreachable("undefined Asm Operand for MSPU");
					}
				}

				MSPUAsmOperand(AsmOperandKind K)
							: MCParsedAsmOperand(), Kind(K)
				{
				}

				/// getStartLoc - Get the location of the first token of this operand.
				SMLoc
				getStartLoc() const
				{
					return StartLoc;
				}

				/// getEndLoc - Get the location of the last token of this operand.
				SMLoc
				getEndLoc() const
				{
					return EndLoc;
				}

				unsigned
				getKind() const
				{
					return Kind;
				}

				unsigned
				getReg() const
				{
					assert(Kind == AsmReg && "Invalid access!");
					return Reg;
				}

				unsigned
				getOpc() const
				{
					assert(Kind == AsmOpc && "Invalid access!");
					return Opc;
				}

				int64_t
				getImm() const
				{
					assert(Kind == AsmImm && "Invalid access!");
					return Imm;
				}

				int64_t
				getFPImm() const
				{
					assert(Kind == AsmFPImm && "Invalid access!");
					return FPImm;
				}

				const MCExpr *
				getExpr() const
				{
					assert(Kind == AsmExpr && "Invalid access!");
					return Expr;
				}

				enum InstPos
        getPos() const
        {
          assert(Kind == AsmPos && "Invalid access!");
          return Pos;
        }

				bool
				isToken() const
				{
					return Kind == AsmOpc;
				}
				bool
				isImm() const
				{
					return Kind == AsmImm;
				}
				bool
				isFPImm() const
				{
					return Kind == AsmFPImm;
				}

        bool
        isPos() const
        {
          return Kind == AsmPos;
        }

				bool
				isReg() const
				{
					return Kind == AsmReg;
				}

				bool
				isMem() const
				{
					return false;
				}

				bool
				isExpr() const
				{
					return Kind == AsmExpr;
				}

				/// print - Print a debug representation of the operand to the given stream.
				virtual void
				print(raw_ostream &OS) const;

				static MSPUAsmOperand *
				createOpc(unsigned opcode)
				{
					MSPUAsmOperand *Op = new MSPUAsmOperand(AsmOpc);
					Op->Opc = opcode;
					return Op;
				}

				static MSPUAsmOperand *
				createReg(unsigned RegNum)
				{
					MSPUAsmOperand *Op = new MSPUAsmOperand(AsmReg);
					Op->Reg = RegNum;
					return Op;
				}

        static MSPUAsmOperand *
        createPos(enum InstPos pos)
        {
          MSPUAsmOperand *Op = new MSPUAsmOperand(AsmPos);
          Op->Pos = pos;
          return Op;
        }

				static MSPUAsmOperand *
				createImm(int64_t imm)
				{
					MSPUAsmOperand *Op = new MSPUAsmOperand(AsmImm);
					Op->Imm = imm;
					return Op;
				}

				static MSPUAsmOperand *
				createFPImm(double imm)
				{
					MSPUAsmOperand *Op = new MSPUAsmOperand(AsmFPImm);
					Op->FPImm = imm;
					return Op;
				}

				static MSPUAsmOperand *
				createExpr(const MCExpr *  expr)
				{
					MSPUAsmOperand *Op = new MSPUAsmOperand(AsmExpr);
					Op->Expr = expr;
					return Op;
				}
		};

		/*******************************************************************/

		class MSPUAsmParser: public MCTargetAsmParser
		{
			private:
				MCSubtargetInfo &STI;

				static MCAsmParser *Parser;
				static const MCRegisterInfo *RegInfo;

				static unsigned PreToken;
				static SMLoc 		PreLoc;

			public:
				MSPUAsmParser(MCSubtargetInfo &sti, MCAsmParser &par,
		                   const MCInstrInfo &MII)
							: MCTargetAsmParser(), STI(sti)
				{
					Parser = &par;
					RegInfo = Parser->getContext().getRegisterInfo();
					//TheMSPUTarget.createMCRegInfo("");

					PreToken = 0;
					PreLoc = SMLoc();
				}

				virtual bool
				ParseRegister(unsigned &RegNo, SMLoc &StartLoc, SMLoc &EndLoc)
				{
					return true;
				}

				virtual bool
				ParseDirective(AsmToken DirectiveID)
				{
					return true;
				}

				virtual bool
				mnemonicIsValid(StringRef ref, unsigned);

				virtual void
				convertToMapAndConstraints(unsigned Kind,
																		const SmallVectorImpl<MCParsedAsmOperand*> &Operands)
				{
				}

				virtual bool
				ParseInstruction(ParseInstructionInfo &Info, StringRef Name,
													SMLoc InstLoc,
													SmallVectorImpl<MCParsedAsmOperand*> &Operands);

				virtual bool
				MatchAndEmitInstruction(SMLoc IDLoc, unsigned &Opcode,
																SmallVectorImpl<MCParsedAsmOperand*> &Operands,
																MCStreamer &Out, unsigned &ErrorInfo,
																bool MatchingInlineAsm);

				static const MCRegisterInfo *
				getRegInfo()
				{
					return RegInfo;
				}

				static MCAsmParser* getMCParser() {
					return Parser;
				}

				static unsigned getPreToken() {
					return PreToken;
				}

				static void setPreToken(unsigned val ) {
					PreToken = val;
				}

				static SMLoc& getPreLoc() {
					return PreLoc;
				}

				static void setPreLoc(const SMLoc& loc ) {
					PreLoc = loc;
				}
		};
	}
}

/************************************ start ragel and bison *****************************************/
// mmpuinstparse() --> mmpuinstlex() --> mmputoklex()
#include "MSPUGenInstrParser.h"

// a direct wrapper for lexer generated by ragel.
// start is modified when this wrapper returns.
static int msputoklex(YYSTYPE * yylval, const char * &start, const char * end);
#include "MSPUGenInstrRLexer.ragel"

static bool isEOS = false;

// called implicitly by bison-generated parser mspuinstparse(Operands, Ref)
static int
mspuinstlex(YYSTYPE * yylval, MSPU::InstLineToParse &InstLine)
{
  // if previous token is End-Of-Statement, we need to fake an EOF token
  // to feed bison parser.
  if(isEOS)
  {
    isEOS = false;
    return 0/* EOF for bison */;
  }

  StringRef & Ref = *(InstLine.Line);
  const char * start = Ref.data();
  const char * end = start+Ref.size();
  int token = msputoklex(yylval, start, end);

  if(token == _EOS)
  {
    isEOS = true;

    Ref = StringRef(start, end-start);
    return token;
  }

  MCAsmParser* mcparser = MSPU::MSPUAsmParser::getMCParser();
  AsmLexer & mclexer = static_cast<AsmLexer&> (mcparser->getLexer());

  // we call llvm parser to analyse expression
  switch(token) {
    case '(':
    case _Symb:
    case _Imm: {
      SMLoc &loc = MSPU::MSPUAsmParser::getPreLoc();
      SMLoc nextLoc;
      mclexer.setCurPtr(loc.getPointer());
      mcparser->Lex();

      const MCExpr * expr;
      bool hadErr;
      hadErr = mcparser->parseExpression(expr, nextLoc);
      if(!hadErr) {
        yylval->op = MSPU::MSPUAsmOperand::createExpr(expr);
        start = nextLoc.getPointer();
        token = _Expr;
      }
      else
        token = _Err;

      break;
    }

    default:
      break;
  }

  Ref = StringRef(start, end-start);
  return token;
}

/* error handling for bison generated yyparse() */
static void
mspuinsterror(MSPU::InstLineToParse &InstLine, const char * errBuf)
{
  // Fixme:
	// custom parsing error handling.
  //MSPU::MSPUAsmParser::getMCParser()->Error(SMLoc::getFromPointer(InstLine.Line->data()), "error incoming input");
}

#include "MSPUGenInstrParser.bison"
/************************************ end ragel and bison *****************************************/

// print - Print a debug representation of the operand to the given stream.
void MSPU::MSPUAsmOperand::
print(raw_ostream &OS) const
{
	switch(Kind) {
		default:
			break;
		case AsmImm:
			break;
	}
}

const MCRegisterInfo* MSPU::MSPUAsmParser::RegInfo;
MCAsmParser *         MSPU::MSPUAsmParser::Parser;

unsigned          		MSPU::MSPUAsmParser::PreToken;
SMLoc		          		MSPU::MSPUAsmParser::PreLoc;

bool MSPU::MSPUAsmParser::
mnemonicIsValid(StringRef ref, unsigned u)
{
	YYSTYPE yylval;
	const char* start = ref.data();
	unsigned token = msputoklex(&yylval, start, start + ref.size());

	switch(token) {
		case _RReg:		  case _JReg:		case _DReg:		case _KReg:
		case _KMReg:		case _SVRReg:	case _SHUReg:	case _Jump:		case _If:		case _Call:
		case _CallM:		case _LpTo:		case _NOP:		case _Stop:		case '[': case _CCtrl:
		case _SysCall:
		case _MC: case _CH: case _LPCtr: case _MCReg:
			return true;

		default:
			break;
	}

	return false;
}

bool MSPU::MSPUAsmParser::
MatchAndEmitInstruction(SMLoc IDLoc, unsigned &Opcode,
						SmallVectorImpl<MCParsedAsmOperand*> &Operands,
						MCStreamer &Out, unsigned &ErrorInfo,
						bool MatchingInlineAsm)
{
  const MCExpr * expr;
  MCInst Inst;
  MSPUAsmOperand * op;
  bool isTail;
  bool isHead = true;
  unsigned n = Operands.size();

  for(unsigned i = 0; i<n; ++i) {
    op = static_cast<MSPUAsmOperand *> (Operands[i]);
    switch(op->getKind()) {
      // this is the last push-backed operand for an instruction.
      case AsmOpc:
        isTail = i == n-1;
        if(isTail) Inst.setEnd(true);
        else Inst.setEnd(false);

        if(isHead) { Inst.setStart(true); isHead = false; }
        else Inst.setStart(false);

        Inst.setOpcode(op->getOpc());
        Inst.setLoc(IDLoc);
        Out.EmitInstruction(Inst);
        Inst.clear(); // clear old operands
        break;
      case AsmReg:
        Inst.addOperand(MCOperand::CreateReg(op->getReg()));
        break;
      case AsmImm:
        Inst.addOperand(MCOperand::CreateImm(op->getImm()));
        break;
      case AsmFPImm:
        Inst.addOperand(MCOperand::CreateFPImm(op->getFPImm()));
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
        Inst.addOperand(MCOperand::CreateExpr(expr));
        break;

      default:
        llvm_unreachable("unknown operand");
    }
  }

	return false; // any error?
}

/// Parse an MSPU instruction mnemonic followed by its operands.
bool
MSPU::MSPUAsmParser::
ParseInstruction(ParseInstructionInfo &Info, StringRef Name,
					SMLoc InstLoc,
					SmallVectorImpl<MCParsedAsmOperand*> &Operands)
{
  MCAsmParser* mcparser = MSPU::MSPUAsmParser::getMCParser();
  AsmLexer & mclexer = static_cast<AsmLexer &> (mcparser->getLexer());

  SMLoc loc = mclexer.getLoc(); // the position for current token start.
  const char * lineStart = loc.getPointer();

  // check until comments, ";;", \n, \r, \0 and buffer end.
  StringRef lineRef = mclexer.LexUntilEndOfStatement();
  const char * lineEnd = lineRef.data() + lineRef.size();

  // bison parser needs to reach end of file (token <= YYEOF=0) to complete parsing.
  // so ragel lexer need ";;" to fake a EOF token.
  if(*lineEnd == ';' && *(lineEnd + 1) == ';')
    lineEnd +=2;

  lineRef = StringRef(lineStart, lineEnd - lineStart);
  InstLineToParse instLine = {&Operands, &lineRef};

  // parsing based on flex lexer.
  /*if(*lineEnd == ';' && *(lineEnd + 1) == ';') {
    mclexer.setCurPtr(lineStart);
    flexInstLineBuf = msputok_scan_bytes(lineStart, lineEnd - lineStart);

    // mspuinstlex is called back within this function.
    int err = mspuinstparse(line);

    msputok_delete_buffer(flexInstLineBuf);

    mclexer.setCurPtr(lineEnd);
    mclexer.Lex();

    if(err) {
      Parser->Error(getPreLoc(), "unrecognized token");
      return true;
    }

  }
  else {
    Parser->Error(SMLoc::getFromPointer(lineEnd), "expect \";;\" ");
    return true;
  }*/

  // parsing based on ragel lexer.
  if(lineStart <= lineEnd) {
    mclexer.setCurPtr(lineStart);

    // mspuinstlex is called back within this function.
    int err = mspuinstparse(instLine);

    mclexer.setCurPtr(instLine.Line->data());
    mclexer.Lex();

    if(err) {
      Parser->Error(getPreLoc(), "invalid token");
      return true;
    }
  }
  else {
    Parser->Error(getPreLoc(), "error instruction line");
    return true;
  }

  return false;
}

/// Force static initialization.
extern "C" void LLVMInitializeMSPUAsmParser()
{
	RegisterMCAsmParser<MSPU::MSPUAsmParser> Y(TheMSPUTarget);
}

/*
#define GET_REGISTER_MATCHER
#define GET_MATCHER_IMPLEMENTATION
#include "MSPUGenAsmMatcher.inc"
*/
