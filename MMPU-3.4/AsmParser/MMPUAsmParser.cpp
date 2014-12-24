//===-- MMPUAsmParser.cpp - Parse MMPU asm to MCInst instructions -----===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "llvm/MC/MCParser/MCAsmLexer.h"
#include "llvm/MC/MCParser/MCAsmParser.h"
#include "llvm/MC/MCParser/MCAsmParserExtension.h"
#include "llvm/MC/MCParser/MCParsedAsmOperand.h"

#include "llvm/MC/MCTargetAsmParser.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCDwarf.h"
#include "llvm/MC/MCParser/MCParsedAsmOperand.h"

#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/raw_ostream.h"

#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/Twine.h"

#include "MMPUAsmOperand.h"
#include "MMPUResConflictChecker.h"
#include "MCFunction.h"
#include "../MCTargetDesc/MMPUMCInst.h"
#include "../MCTargetDesc/MMPUInsnLine.h"
#include "../MCTargetDesc/MMPUMCTargetDesc.h"

using namespace llvm;

/// MMPUAsmOperand - Instances of this class represent operands of parsed MMPU assembly instructions.
/// Note: the first operand records instruction opcode, see ARMGenAsmMatcher.inc MatchInstructionImpl().
namespace llvm
{
	namespace MMPU
	{
	  // passed arg for bison-parser
    struct InstLineToParse
    {
      SmallVectorImpl<MCParsedAsmOperand*>& Operands;
      StringRef& Line;
    };


    // for lexer and bison-parser.
    struct MMPUToken {
      union {
          MMPUAsmOperand* op;
          unsigned options;
          unsigned opc;
          unsigned reg;
          unsigned num;
          const MCExpr* expr;
      };

      const char* pos; // where the token start.
    };


    /// Begin to define HMacro related structures
    struct HMacro {
      StringRef Name;
      MCFunction *Body;

      public:
        HMacro(StringRef N, MCFunction *B) :
        Name(N), Body(B) {}
    };

    class HMacroInstantiation {
      MCFunction *Body;
      uint64_t Start;
      uint64_t Index;
      SmallVector<MCLoopBlock *, 8> Instantiation;

      public:
        MCLoopBlock *CurLoop;
        HMacroInstantiation(MCFunction *MF, uint64_t S)
        : Body(MF), Start(S), Index(0) {
        }

        bool getParsedInst(uint64_t Address, MCParsedInst* &Inst) {
          for (unsigned i = 0; i < Instantiation.size(); i++)
            if (Instantiation[i]->getParsedInst(Address, Inst)) return true;
            MCLoopBlock *MB;
          if (!Body->getLoopBlock(Index, MB)) return false;
          else {
            Index += MB->size();
            if (MB->getType() == Loop) CurLoop = MB;
            else CurLoop = NULL;
            if (MB->isNested())
              Instantiation.push_back(MB->DupThisBlock(Address));
            else
              Instantiation.push_back(new MCLoopBlock(MB->getType(), Address,
                                                      MB->getCount(), MB->getKI(),
                                                      MB->getInsts()));
                                                      return Instantiation.back()->getParsedInst(Address, Inst);
          }
          return false;
        }

        bool isFinish() {return Body->size() <= Index;}

        bool getContainer(uint64_t Address, MCLoopBlock* &MB) {
          MCParsedInst *Inst;
          for (unsigned i = 0; i < Instantiation.size(); i++) {
            if (Instantiation[i]->getParsedInst(Address, Inst)) {
              MB = Instantiation[i];
              if (!((Address >= (MB->getStart() +
                MB->Headsize())) &&
                (Address < (MB->getStart() +
                MB->Headsize() +
                MB->size()))))
                return MB->getLoopBlock(Address, MB);
              return true;
            }
          }
          MB = NULL;
          return false;
        }

        bool getTop(uint64_t Address, MCLoopBlock* &MB) {
          MCParsedInst *Inst;
          for (unsigned i = 0; i < Instantiation.size(); i++) {
            if (Instantiation[i]->getParsedInst(Address, Inst)) {
              MB = Instantiation[i];
              return true;
            }
          }
          MB = NULL;
          return false;
        }

        bool getLeafContainer(uint64_t Address, MCLoopBlock* &MB) {
          if (getContainer(Address, MB)) {
            if (MB->getLeafBlock(Address, MB))
              return MB->size() != 0;
            else return MB->size() != 0;
          }
          return false;
        }

        bool getNextBlock(uint64_t Address, MCLoopBlock* &MB) {
          if (Body->getLoopBlock(Index, MB)) {
            Index += MB->size();
            if (MB->getType() == Loop) CurLoop = MB;
            else CurLoop = NULL;
            if (MB->isNested())
              Instantiation.push_back(MB->DupThisBlock(Address));
            else
              Instantiation.push_back(new MCLoopBlock(MB->getType(), Address,
                                                      MB->getCount(), MB->getKI(),
                                                      MB->getInsts()));
                                                      MB = Instantiation.back();
                                                      return true;
          }
          MB = NULL;
          return false;
        }

        void addLoopBlock(MCLoopBlock *MB) {
          Instantiation.push_back(MB);
        }
    };
    /// The end of HMacro related structures


		class MMPUAsmParser: public MCTargetAsmParser
		{
			private:
				MCSubtargetInfo &STI;
				static const MCRegisterInfo* RegInfo;
				static MCAsmParser*          Parser;
				static SMLoc                 PreLoc;
				static const char*           CurPosition;

				// obsolete
				static unsigned              PreToken;

			public:
				MMPUAsmParser(MCSubtargetInfo &sti, MCAsmParser &parser,
		                   const MCInstrInfo &MII)
							: MCTargetAsmParser(), STI(sti)
				{
          MCAsmParserExtension::Initialize(parser);
          Parser = &parser;
          RegInfo = Parser->getContext().getRegisterInfo();
          //TheMMPUTarget.createMCRegInfo("");

          PreToken = 0;
          PreLoc = SMLoc();
          CurPosition = 0;

          /// Initilize members used by HMacro mechanism
          inHMacro = false;
          VAddr = 0;
          Ctx = &getContext();
          IgnoreHMacro = false;
          UniqueLabelID = 0;
          LoopAvailable = true;
          EnableOptimization = false;
          cachedLabel = false;
          OptContext.PreviousInst = NULL;
          OptContext.PreviousLastInst = NULL;
          OptContext._mseq = MMPUInst::NOP;
          OptContext._label = NULL;
          OptContext._ki = 0;
          OptContext._count = 0;
          OptContext.mseq = MMPUInst::NOP;
          OptContext.label = NULL;
          OptContext.ki = 0;
          OptContext.count = 0;
				}

				virtual bool
				ParseRegister(unsigned &RegNo, SMLoc &StartLoc, SMLoc &EndLoc)
				{
					return true;
				}

				// we need to handle sub-instructions of *Inst*.
		    bool CheckResourceConflict(Resource * Table, MMPUMCInst * Inst)
		    {
		      const InstDesc* ISADesc = GetISADesc();
		      uint32_t opc = Inst->getOpcode();
		      bool conflict = false;

		      for(int idx = 0; ISADesc[idx].opcode != MMPUInst::INSTRUCTION_LIST_END; idx++)
		      {
		        if(opc == ISADesc[idx].opcode)
		        {
		          const int32_t * res_constraints = ISADesc[idx].res_constraints;

		          for(int i=0; res_constraints[i]!=-1; i+=2)
		          {
		            int32_t res = res_constraints[i];
		            if(Table[res].Cycles == 0) // free to allocate
		            {
		              Table[res].Cycles = res_constraints[i+1];
		            }
		            else
		            {
		              // use ResourceTable[res].Inst to report resource conflict.
		              return true;
		            }
		          }

		          // we stop here because we have already find description for current instruction.
		          break;
		        }
		      }

		      return false; // no resource conflict
		    }


        bool CheckInstLineResourceConflict(Resource * Table, MMPUMCInst * Inst)
        {
          if(CheckResourceConflict(Table, Inst)) return true;
          else
          {

          }

          return false;
        }

		    void ForwardCheckerClock(Resource* table)
		    {
		      for(int i=0; i<NUM_RESOURCE; i++)
		      {
		        if(table[i].Cycles > 0) table[i].Cycles--;
		      }

		      return;
		    }

				// true if some options is illegal, false if all options are legal.
				bool CheckInstOptions(const MCInst * Inst, SMLoc Loc)
				{
				  const InstDesc* InstOptionConstraints = GetISADesc();
				  uint32_t opc = Inst->getOpcode();
				  bool illegal = false;

				  for(int idx = 0; InstOptionConstraints[idx].opcode != MMPUInst::INSTRUCTION_LIST_END; idx++)
				  {
				    if(opc == InstOptionConstraints[idx].opcode)
				    {
				      // perform option checks according to option constraints.
				      InstDesc desc = InstOptionConstraints[idx];
				      int32_t imm = (int32_t) (Inst->getOperand(desc.opnum).getImm());
				      std::string option_name;

				      // 1. check masks: no options other than mask bits are allowed.
				      int32_t conflicts = imm & desc.mask;
				      if( conflicts != 0)
				      {
				        PrintInstOptions(conflicts, option_name);
				        std::string legal_option_name;
				        PrintInstOptions(~desc.mask, legal_option_name);
				        Parser->Error(Loc, "Options "+ option_name + " illegal. All legal options are "+legal_option_name);
				        illegal = true;
				      }

				      // 2. check mandatory options
              if( (imm & desc.mandatory) != desc.mandatory)
              {
                PrintInstOptions(desc.mandatory, option_name);
                Parser->Error(Loc, "Options "+ option_name + " missing but expected.");
                illegal = true;
              }

              int32_t i;
              // 3. check inclusive options
              for(i=0; desc.inclusive[i] !=0; i++)
              {
                // we expect at least one 1 exists.
                if((imm & desc.inclusive[i]) == 0)
                {
                  PrintInstOptions(desc.inclusive[i], option_name);
                  Parser->Error(Loc, "Options "+ option_name + " missing, at least one is expected.");
                  illegal = true;
                }
              }

              // 4. check exclusive options
              for(i=0; desc.exclusive[i] !=0; i++)
              {
                int32_t bits = imm & desc.exclusive[i];
                // we check how many 1's exist by clearing the rightmost 1.
                if((bits & (bits-1)) != 0) // more than one 1 exist.
                {
                  PrintInstOptions(desc.exclusive[i], option_name);
                  Parser->Error(Loc, "Options "+ option_name + " exclusive.");
                  illegal = true;
                }
              }

              // option-checking is completed for this instruction.
              if(illegal) return true;
              else break;
				    }
				  }

				  return false; // no option is illegal.
				}

			  /// ParseDirective - Parse a target specific assembler directive
			  ///
			  /// The parser is positioned just following the directive name.  The target
			  /// specific directive parser should parse the entire directive via doing or
			  /// recording any target specific work.
				/// If the directive is not target specific, return true and do nothing.
				/// If the directive is specific for the target, the entire line is parsed
				/// up to and including the end-of-statement token and false is returned.
			  ///
			  /// \param DirectiveID - the identifier token of the directive.
				virtual bool
				ParseDirective(AsmToken DirectiveID)
				{
          // not target-specific and thus error happened during parsing.

          /// HMacro related directives
          SMLoc IDLoc = DirectiveID.getLoc();
          if (DirectiveID.getString() == ".hmacro")
            return ParseDirectiveHMacro(DirectiveID, IDLoc);
          if (DirectiveID.getString() == ".endhmacro" ||
            DirectiveID.getString() == ".endhm")
            return ParseDirectiveEndHMacro(DirectiveID, IDLoc);
          if (DirectiveID.getString() == ".flushhm")
            return ParseDirectiveFlushHM(DirectiveID, IDLoc);
          if (DirectiveID.getString() == ".opt")
            return ParseDirectiveOpt(DirectiveID, IDLoc);
          if (DirectiveID.getString() == ".stopopt")
            return ParseDirectiveStopOpt(DirectiveID, IDLoc);
          return true;
				}

				virtual bool
				mnemonicIsValid(StringRef Mnemonic, unsigned VariantID);

				virtual void
				convertToMapAndConstraints(unsigned Kind,
																		const SmallVectorImpl<MCParsedAsmOperand*> &Operands)
				{
				}

				virtual bool
				ParseInstruction(ParseInstructionInfo &Info, StringRef Name,
													SMLoc NameLoc,
													SmallVectorImpl<MCParsedAsmOperand*> &Operands);

				MCInst *
				AnalyzeParsedAsmOperands(unsigned &Opcode,
                                 SmallVectorImpl<MCParsedAsmOperand*> &Operands,
                                 SmallVectorImpl<MCInst*> &TailInsts);

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

        static const char* getCurPosition() {
          return CurPosition;
        }

        static void setCurPosition(const char* pos ) {
          CurPosition = pos;
        }

        /// The following members and mathods contribute to HMacro mechanism
      private:
        MCContext *Ctx;
        StringMap<HMacro*> HMacroMap;
        std::vector<HMacroInstantiation*> ActiveHMacros;
        uint64_t VAddr;
        bool inHMacro;
        HMacro *CurHMacro;
        bool IgnoreHMacro;
        bool EnableOptimization;
        bool LoopAvailable;
        std::vector<int64_t> LocalLabels;
        int64_t UniqueLabelID;
        bool cachedLabel;
        SMLoc CurLoc;
        struct {
          MCInst *PreviousInst;
          MCInst *PreviousLastInst;
          unsigned _mseq;
          const MCSymbolRefExpr *_label;
          int64_t _count;
          int64_t _ki;
          unsigned mseq;
          const MCSymbolRefExpr *label;
          int64_t count;
          int64_t ki;
        } OptContext;

      private:
        int AnalyzeHMacro(MCInst *Inst, MCInst *CurInst, unsigned &Sym);
        void DealWithSequential();
        void DealWithRepeat(int64_t loops, MCInst *Inst, MCInst *CurInst);
        void DealWithLoop(int64_t insts, bool Mode = false);
        bool DealWithUnmatchedLoop(uint64_t KI, unsigned Level, int64_t &MaxCnt);
        void tryMergeRedundantInst(MCInst* &Inst, MCInst* &CurInst);
        void EmitCachedInst(SMLoc IDLoc, MCStreamer &Out);
        bool ParseDirectiveHMacro(AsmToken DirectiveID, SMLoc DirectiveLoc);
        bool ParseDirectiveEndHMacro(AsmToken DirectiveID, SMLoc DirectiveLoc);
        bool ParseDirectiveFlushHM(AsmToken DirectiveID, SMLoc DirectiveLoc);
        bool ParseDirectiveOpt(AsmToken DirectiveID, SMLoc DirectiveLoc);
        bool ParseDirectiveStopOpt(AsmToken DirectiveID, SMLoc DirectiveLoc);
        bool HandleHMacroEntry(StringRef Name, const HMacro *M);

		};

	}
}

/************************************ start ragel and bison *****************************************/
// mmpuinstparse() --> mmpuinstlex() --> mmputoklex()
#include <MMPUGenInstrParser.h>

// refer to bison-parser for more info.
static bool mmpuInstParserNeedExpr = false;

// a direct wrapper for lexer generated by ragel.
// start is modified when this wrapper returns.
static int mmputoklex(YYSTYPE * yylval, const char * &start, const char * end);
#include "MMPUGenInstrRLexer.ragel"

static bool isEOS = false;

// called implicitly by bison-generated parser mmpuinstparse(Operands, Ref)
static int
mmpuinstlex(YYSTYPE * yylval, MMPU::InstLineToParse &InstLine)
{
  // if previous token is End-Of-Statement, we need to fake an EOF token
  // to feed bison parser.
  if(isEOS)
  {
    isEOS = false;
    return 0/* EOF for bison */;
  }

  StringRef & Ref = InstLine.Line;
  const char * start = Ref.data();
  const char * end = start + Ref.size();

  MCAsmParser* mcparser = MMPU::MMPUAsmParser::getMCParser();
  AsmLexer & mclexer = static_cast<AsmLexer&> (mcparser->getLexer());

  int token;
  const MCExpr * expr;
  bool hadErr;

  /*if(mmpuInstParserNeedExpr) {
    mclexer.setCurPtr(loc.getPointer());
    mcparser->Lex();

    loc = mcparser->getTok().getLoc();
    MMPU::MMPUAsmParser::setPreLoc(loc);

    hadErr = mcparser->parseExpression(expr, nextLoc);
    mmpuInstParserNeedExpr = false;

    if(!hadErr) {
      yylval->token.op = MMPU::MMPUAsmOperand::createExpr(expr);
      start = nextLoc.getPointer();
      token = t_Expr;
    }
    else token = t_Err;
  }
  else*/
  {
    token = mmputoklex(yylval, start, end);

    if(token == t_EOS)
    {
      isEOS = true;

      Ref = StringRef(start, end-start);
      return token;
    }

    // we call llvm parser to analyse expression
    switch(token) {
      //Fixme: since we use '('...')' to form operands, we have to prohibit use them to
      // form expression.
      //case '(':
      case t_Symb:
      case t_Imm: {

        SMLoc &loc = MMPU::MMPUAsmParser::getPreLoc();
        SMLoc nextLoc;
        mclexer.setCurPtr(loc.getPointer());
        mcparser->Lex();

        loc = mcparser->getTok().getLoc();
        MMPU::MMPUAsmParser::setPreLoc(loc);

        hadErr = mcparser->parseExpression(expr, nextLoc);
        if(!hadErr) {
          yylval->token.expr = expr;
          start = nextLoc.getPointer();
          token = t_Expr;
        }
        else
          token = t_Err;

        break;
      }

      default:
        break;
    }
  }

  Ref = StringRef(start, end-start);
  return token;
}

/* error handling for bison generated mmpuparse() */
static void
mmpuinsterror(MMPU::InstLineToParse &InstLine, const char * errBuf)
{
  // Fixme:
  // custom parsing error handling.
  //MMPU::MMPUAsmParser::getMCParser()->Error(SMLoc::getFromPointer(InstLine.Line.data()), "error incoming input");
}

#include "MMPUGenInstrParser.bison"
/************************************ end ragel and bison *****************************************/

const MCRegisterInfo* MMPU::MMPUAsmParser::RegInfo;
MCAsmParser *         MMPU::MMPUAsmParser::Parser;
SMLoc                 MMPU::MMPUAsmParser::PreLoc;
const char*           MMPU::MMPUAsmParser::CurPosition;

// obsolete
unsigned              MMPU::MMPUAsmParser::PreToken;

bool MMPU::MMPUAsmParser::
mnemonicIsValid(StringRef line, unsigned VariantID)
{
  // 1. find a start token of a mmpu instruction.
  YYSTYPE yylval;
  const char* start = line.data();
  unsigned token = mmputoklex(&yylval, start, start+line.size());

  switch(token) {
    case t_DM:     case t_MReg:   case t_MRegExt:  case t_Calc:
    case t_Shu:    case t_BIU:    case t_MR:       case t_Repeat:
    case t_Jump:   case t_Loop:   case t_NOP:      case t_DivR:
    case t_MPU:    case t_Stop:   case t_COND:     case t_FMAC:
      return true;

    default:
      break;
  }

  // 2. hmacro
  StringRef str(line.data(), start-line.data());
  if(HMacroMap.lookup(str))
    return true;

  // otherwise
  return false;
}

/// print - Print a debug representation of the operand to the given stream.
void MMPU::
MMPUAsmOperand::print(raw_ostream &OS) const
{
	switch(Kind) {
		default:
			break;
		case AsmImm:
			break;
	}
}

/// operand vector is composed of:
/// slot, opcode, operand-list [, sub-inst]
MCInst * MMPU::MMPUAsmParser::
AnalyzeParsedAsmOperands(unsigned &Opcode,
                         SmallVectorImpl<MCParsedAsmOperand*> &Operands,
                         SmallVectorImpl<MCInst*> &TailInsts)
{
	MCInst* Insts[NUMSLOTS];
	bool isFilled[NUMSLOTS];
	for(unsigned i=0; i < NUMSLOTS; i++) {
		isFilled[i] = false;
	}

	MCInst* curInst;
	SMLoc   curOptionsLoc;
  MMPUAsmOperand * op;
  unsigned slot;

  SmallVectorImpl<MCParsedAsmOperand*> *ops = &Operands;
  MCOperand MO;
  MCParsedInst *ParsedInst;
  std::vector<HMacroInstantiation*>::iterator iter;
  int64_t expr2imm = 0;

  unsigned j = 0;
  do {
    if (ops) curInst = new MCInst();

    bool instLineOptionsErr = false;
    for(unsigned i=0; ops && i < ops->size(); i++) {
      op = static_cast<MMPUAsmOperand *> ((*ops)[i]);

      switch(op->getKind()) {

        case AsmSlot: // this operand always come as the last operand.
          slot = op->getSlot();
          assert(slot<NUMSLOTS && "invalid slot number");

          if(isFilled[slot] == false) {
            /* we check instruction options for each of 14 slots here. */
            if(CheckInstOptions(curInst, curOptionsLoc))
            {
              instLineOptionsErr = true;
              // since instruction error, we replace it with NOP to relieve macro tool.
              curInst->clear();
              curInst->setOpcode(MMPUInst::NOP);
            }

            Insts[slot] = curInst;
            isFilled[slot] = true; // a slot is filled now.
          }
          else if (curInst->getOpcode() == MMPUInst::NOP) {
            delete curInst;  // remove redundant NOPs
            curInst = new MCInst();
          }
          else if (Insts[slot]->getOpcode() == MMPUInst::NOP) {
            delete Insts[slot];
            Insts[slot] = curInst;
            curInst = new MCInst();
          }
          else if(slot < NUMSLOTS - 1)
            llvm_unreachable("can not fill in same slot with more than 1 instructions");

          if(slot == NUMSLOTS - 1) { // allow more than 1 seq inst in parsing
            TailInsts.push_back(curInst);
          }

          if(i+1 < ops->size()) { // there are more operands following.
            curInst = new MCInst();
          }
          break;

        case AsmOpc:
          curInst->setOpcode(op->getOpc());
          break;

        case AsmLoc:
          curOptionsLoc = SMLoc::getFromPointer(op->getLoc());
          break;

        case AsmReg:
          curInst->addOperand(MCOperand::CreateReg(op->getReg()));
          break;

        case AsmImm:
          curInst->addOperand(MCOperand::CreateImm(op->getImm()));
          break;

        case AsmExpr:
          if (op->getExpr()->EvaluateAsAbsolute(expr2imm))
            curInst->addOperand(MCOperand::CreateImm(expr2imm));
          else
            curInst->addOperand(MCOperand::CreateExpr(op->getExpr()));
          break;

        case AsmMacro:
          if (IgnoreHMacro) break;
          if (const HMacro *HM = HMacroMap.lookup(op->getMacro()->getName()))
            HandleHMacroEntry(op->getMacro()->getName(), HM);
          break;

        default:
          llvm_unreachable("invalid parsed operand");
      }
    }
    //if(instLineOptionsErr) return 0;

    ops = NULL;
    if (!ActiveHMacros.empty() && j < ActiveHMacros.size()) {
      if (ActiveHMacros[j]->getParsedInst(VAddr, ParsedInst)) {
        MCLoopBlock *MB;
        if (ActiveHMacros[j]->getContainer(VAddr, MB))
          IgnoreHMacro = MB->isUnrolled();
        ops = ParsedInst->getOperands();
      }
      if (j++ < ActiveHMacros.size()) continue;
    }
    IgnoreHMacro = false;
    break;
  } while (1);

	if(isFilled[0]==false)	{
	  Insts[0] = new MCInst();
	  Insts[0]->setOpcode(MMPUInst::NOP);
	}

	for(unsigned j=1; j<NUMSLOTS; j++) {
    if(isFilled[j]==false)  {
      Insts[j] = new MCInst();
      Insts[j]->setOpcode(MMPUInst::NOP);
    }

		Insts[j-1]->addOperand(MCOperand::CreateInst(Insts[j]));
	}

	if(isFilled[NUMSLOTS-1]==false) // in case there is no SEQ inst
	  TailInsts.push_back(Insts[NUMSLOTS-1]);

	return Insts[0];
}

bool MMPU::MMPUAsmParser::
MatchAndEmitInstruction(SMLoc IDLoc, unsigned &Opcode,
						SmallVectorImpl<MCParsedAsmOperand*> &Operands,
						MCStreamer &Out, unsigned &ErrorInfo,
						bool MatchingInlineAsm)
{
  if (inHMacro) return false;
  SmallVector<MCInst*, 4> TailInsts;
  MCInst* CurInst;
  MCInst* Inst = AnalyzeParsedAsmOperands(Opcode, Operands, TailInsts);

  if(Inst == 0) return true; // parsing error

  CurInst = TailInsts[0];

  // Begin to analyze initiated HMacro
  unsigned Sym = 0;
  int Res = AnalyzeHMacro(Inst, CurInst, Sym);
  if (Res < 0) {
    Error(CurLoc, "Virtual Address " + Twine(VAddr) + ": Analysis returns an " +
                  "error " + Twine(Res) + "\n" +
                  "-1: Unable to issue instructions when Loop meets Repeat. " +
                  "An error probably happened in the analysis prior to this " +
                  "time.\n" +
                  "-2: Cannot find an instruction at Virtual Address " +
                  Twine(VAddr) + ". HMacro is not continuous.\n" +
                  "-3: Initialized HMacroes didn't go into loop " +
                  "simultaneously. Previous analysis encountered an error\n" +
                  "-4: Loops in HMacroes have different nested levels\n" +
                  "-5: Unable to merge loops in initialized HMacros due to"
                  + " unequal loop bodies\n" +
                  "-6: Unable to merge REPEAT @(KI) with unnested LPTO @(KI)\n"+
                  "-7: Unable to merge REPEAT @(KI) using different KIs\n" +
                  "-8: Unable to handle unmatched loops");
    ActiveHMacros.clear();
  }
  // The end of analysis

  // Eliminate redundant inst
  if (EnableOptimization) tryMergeRedundantInst(Inst, CurInst);
  if (Inst) {
    if (OptContext.mseq != MMPUInst::NOP) {
      MCOperand MO;
      MCInst *IP = CurInst;
      IP->setOpcode(OptContext.mseq);
      if (OptContext.mseq == MMPUInst::Loop) {
        MO = MCOperand::CreateExpr(OptContext.label);
        IP->addOperand(MO);
      }
      if (OptContext.mseq == MMPUInst::Loop || OptContext.mseq == MMPUInst::RepeatConds) {
        MO = MCOperand::CreateImm(-OptContext.count);
        IP->addOperand(MO);
        MO = MCOperand::CreateImm(OptContext.ki);
      }
      else
        MO = MCOperand::CreateImm(OptContext.count);
      IP->addOperand(MO);
    }
    Inst->setLoc(IDLoc);
    Out.EmitInstruction(*Inst);
  }
  OptContext.mseq = MMPUInst::NOP;
  OptContext.label = NULL;
  OptContext.ki = 0;
  OptContext.count = 0;
  if (EnableOptimization && Sym)
    EmitCachedInst(IDLoc, Out);
  ++VAddr;

  // Begin to emit labels generated by previous analysis
  while (Sym) {
    MCSymbol *S = getContext().GetOrCreateSymbol(Twine("HM")+Twine(LocalLabels.back()));
    LocalLabels.pop_back();
    Out.EmitLabel(S);
    --Sym;
  }
  if (cachedLabel) LocalLabels.push_back(UniqueLabelID);
  cachedLabel = false;

	return false; // Has any parsing error?
}

/// Parse an MMPU instruction mnemonic followed by its operands.
bool
MMPU::MMPUAsmParser::
ParseInstruction(ParseInstructionInfo &Info, StringRef Name, SMLoc NameLoc,
                 SmallVectorImpl<MCParsedAsmOperand*> &Operands)
{
  MCAsmParser* mcparser = MMPU::MMPUAsmParser::getMCParser();
  AsmLexer & mclexer = static_cast<AsmLexer &> (mcparser->getLexer());

  SMLoc loc = mclexer.getLoc(); // the position for current token start.
  const char * lineStart = loc.getPointer();

  // check until (a) comments, (b) ";;", (c) \n, \r, '\0' and (d) buffer end.
  StringRef lineRef = mclexer.LexUntilEndOfStatement();
  const char * lineEnd = lineRef.data() + lineRef.size();

  // bison parser needs to reach end of file (token <= YYEOF=0) to complete parsing.
  // so ragel lexer need ";;" to fake a EOF token.
  if(*lineEnd == ';' && *(lineEnd + 1) == ';')
    lineEnd +=2;

  lineRef = StringRef(lineStart, lineEnd - lineStart);
  InstLineToParse instLine = {Operands, lineRef};

  // parsing based on ragel lexer.
  if(lineStart <= lineEnd) {
    mclexer.setCurPtr(lineStart);

    // mmpuinstlex is called back within this function.
    int err = mmpuinstparse(instLine);

    mclexer.setCurPtr(instLine.Line.data());
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

  // in HMacro definition context
  if (inHMacro) {
    CurHMacro->Body->addParsedInst(new MCParsedInst(Operands));
    Operands.clear();
  }

  CurLoc = NameLoc;  // Used for locating errors

  return false;
}

bool MMPU::MMPUAsmParser::ParseDirectiveHMacro(AsmToken DirectiveID,
                                               SMLoc DirectiveLoc) {
  StringRef Name;
  if (inHMacro)
    return Parser->TokError("nested hmacro definition is not allowed");
  if (Parser->parseIdentifier(Name))
    return Parser->TokError("expected identifier in '.hmacro' directive");
  if (Parser->getLexer().isNot(AsmToken::EndOfStatement))
    return Parser->TokError("parameters are currently not supported");
  Lex();

  if (HMacroMap.lookup(Name))
    return Error(DirectiveLoc, "hmacro '" + Name + "' is already defined");

  MCFunction *Body = new MCFunction(Name, 0, Ctx);
  CurHMacro = new HMacro(Name, Body);
  inHMacro = true;
  return false;
}

bool MMPU::MMPUAsmParser::ParseDirectiveEndHMacro(AsmToken DirectiveID,
                                                  SMLoc DirectiveLoc) {
  if (!inHMacro)
    return Parser->TokError("no matching '.hmacro' in definition");
  if (Parser->getLexer().isNot(AsmToken::EndOfStatement))
    return Parser->TokError("unexpected token after '.endhmacro'");
  Lex();

  CurHMacro->Body->EliminateEmptyBlock();
  HMacroMap[CurHMacro->Name] = CurHMacro;
  inHMacro = false;
  return false;
}

bool MMPU::MMPUAsmParser::ParseDirectiveFlushHM(AsmToken DirectiveID,
                                                SMLoc DirectiveLoc) {
  if (inHMacro)
    return Parser->TokError("no matching '.endhmacro' in definition");
  if (Parser->getLexer().isNot(AsmToken::EndOfStatement))
    return Parser->TokError("unexpected token after '.flushhm'");
  Lex();

  SmallVector<MCParsedAsmOperand*, 256> Operands;
  unsigned ErrorInfo;
  unsigned Opcode;
  SMLoc Loc;
  while (!ActiveHMacros.empty())
    MatchAndEmitInstruction(Loc, Opcode, Operands, Parser->getStreamer(),
                            ErrorInfo, false);
  EmitCachedInst(Loc, Parser->getStreamer());
  return false;
}

bool MMPU::MMPUAsmParser::ParseDirectiveOpt(AsmToken DirectiveID,
                                            SMLoc DirectiveLoc) {
  EnableOptimization = true;
  return false;
}

bool MMPU::MMPUAsmParser::ParseDirectiveStopOpt(AsmToken DirectiveID,
                                                SMLoc DirectiveLoc) {
  EnableOptimization = false;
  SMLoc Loc;
  EmitCachedInst(Loc, Parser->getStreamer());
  return false;
}

bool MMPU::MMPUAsmParser::HandleHMacroEntry(StringRef Name,
                                            const HMacro *M) {
  if (ActiveHMacros.size() == 20)
    return TokError("hmacros cannot be nested more than 20 levels deep \
    and/or concurrently being expanded more than 20 instances");
  MCFunction *MF = M->Body;
  ActiveHMacros.push_back(new HMacroInstantiation(MF, VAddr));
  return true;
}

/* AnalyzeHMacro: All instantiated HMacros will be analyzed here to determine if
* LoopBlocks need to be unrolled or not
*/
int MMPU::MMPUAsmParser::AnalyzeHMacro(MCInst *Inst, MCInst *CurInst, unsigned &Sym) {
  MCLoopBlock* MB;
  MCBlockTy PreType = None, CurType;
  int64_t MaxCnt = 0;
  unsigned CheckLoops = 0;
  bool Done = false;
  for (unsigned i = 0; i < ActiveHMacros.size(); i++) {
    if (ActiveHMacros[i]->getContainer(VAddr, MB)) {
      CurType = MB->getType(VAddr);
      if (CurType == Loop) ++CheckLoops; // else it's in the head or tail of this loop
        if (Done) continue;
        if (MB->getType() == Loop) {
          ActiveHMacros[i]->getLeafContainer(VAddr, MB);
          if (MB->getType(VAddr) == Loop) CurType = Sequential; // Consider unnested Loop as Sequential in order to deal
            // with unrolling inside loops
            else CurType = MB->getType(VAddr);
        }
        // first of all, check if there is a sequential block
        if ((CurType == Sequential && PreType == Repeat) ||
          (CurType == Repeat && PreType == Sequential)) {
          DealWithSequential();
        PreType = None;
        Done = true;
        continue; //do not return here, because Loop is not handled yet
        }
        else if (CurType == Repeat) {
          if (PreType == Repeat) {
            if ((MaxCnt > 0 && MB->getCount() > 0 && MaxCnt > MB->getCount()) ||
              (MaxCnt <= 0 && MB->getCount() < 0 && MaxCnt > MB->getCount()) ||
              (MaxCnt <= 0 && MB->getCount() > 0))
              MaxCnt = MB->getCount();
          }
          else if (PreType == None)
            MaxCnt = MB->getCount();
          else // loop with repeat is illegal
            return -1;
        }
        if (CurType != None) PreType = CurType;
    }
    else return -2; // Cannot find VAddr in one of ActiveHMacros, this is an error.
  }
  if (PreType == Repeat) {
    DealWithRepeat(MaxCnt, Inst, CurInst);
    LoopAvailable = false;
  }
  else LoopAvailable = true;
  // Check Loops
  if (CheckLoops != 0) {
    if (CheckLoops != ActiveHMacros.size()) return -3; // Illegal! if any ActiveHMacro is in loop, all must be in loop
      else {
        for (unsigned i = 0; i < ActiveHMacros.size(); i++) {
          unsigned CurSym = 0;
          ActiveHMacros[i]->getLeafContainer(VAddr, MB);
          while (MB) {
            if (!MB->isLoopEnd(VAddr)) {
              if (VAddr < (MB->getStart() + MB->size()))
                break;
              // else may be in the tail of MB
            }
            else if (MB->getType(VAddr) == Loop) ++CurSym;
            MB = MB->getParent();
          }
          if (i == 0) Sym = CurSym;
          if (CurSym != Sym)
            return -4; // Loop levels are not consistent;
        }
      }
  }
  unsigned j = 0;
  while(j < ActiveHMacros.size()) {
    if (!ActiveHMacros[j]->getContainer(VAddr + 1, MB) &&
      ActiveHMacros[j]->isFinish()) {
      std::vector<HMacroInstantiation*>::iterator iter =
      std::find(ActiveHMacros.begin(), ActiveHMacros.end(), ActiveHMacros[j]);
    delete ActiveHMacros[j];
    ActiveHMacros.erase(iter);
    continue;
    }
    ++j;
  }
  // blocks are all sequential or all repeats have been dealed with,
  // so probably loop blocks follow current blocks.
  MaxCnt = 1;
  PreType = None;
  MCLoopBlock *LastMB = NULL;
  uint64_t KI = 0;
  unsigned LowestLevel = 0;
  for (unsigned i = 0; i < ActiveHMacros.size(); i++) {
    if (ActiveHMacros[i]->getContainer(VAddr + 1, MB) ||
      ActiveHMacros[i]->getNextBlock(VAddr + 1, MB)) {
      while (MB->getType() == Loop) {
        if ((MB->getType(VAddr + 1) == Loop) &&
          (MB->getStart() + MB->Headsize() == (VAddr + 1)))
          break;   // a new beginning loop;
        if (!MB->getLoopBlock(VAddr + 1, MB))
          return 2;// in an unnested loop;
      }
      // Now MB is:
      // not a loop;
    // a new beginning loop;

    // first of all, check if there is a sequential block accompany with a loop block
    if ((MB->getType(VAddr + 1) == Sequential && PreType == Loop) ||
        (MB->getType(VAddr + 1) == Loop && PreType == Sequential)) {
      ++VAddr;
      DealWithLoop(1);
      --VAddr;
      return 3;
    }
    else if (MB->getType(VAddr + 1) == Repeat) {
      if (MB->getCount() <= 0 && LastMB != NULL) {
        if (!LastMB->isKINested()) return -6; // RepeatKI meets unnested Loop
          if (LowestLevel == 1 && KI != MB->getKI()) return -6;
          MaxCnt = MB->getCount();
        LowestLevel = 1;
        KI = MB->getKI();
        continue;
      }
      //else if (MB->getCount() <= 0) return 4;  // Maybe there is an potential error that RepeatKI meets Loop
      else if (MB->getCount() <= 0) {
        if (MaxCnt <= 0 && KI != MB->getKI()) return -7; // RepeatKIs using different KIs
          if (MaxCnt <= 1) MaxCnt = MB->getCount(); // MaxCnt = 1: there isn't any Repeat before, MaxCnt < 1: there is at least one Repeat KI before.
            KI = MB->getKI();
      }
      else if (MaxCnt <= 1 || MB->getCount() < MaxCnt)
        MaxCnt = MB->getCount();
    }
    else if (MB->getType(VAddr + 1) == Loop) {
      if (LastMB != NULL) {
        if (*LastMB == *MB)
          MaxCnt = (MaxCnt <= 0) &&
          (MaxCnt > MB->getCount()) ? MB->getCount() : MaxCnt;
        else {
          // Loops are unequal, perhaps one of them is extracted from a KINested Loop.
          if (LowestLevel == 0) LowestLevel = LastMB->getLevel();
          if (LowestLevel > MB/*ActiveHMacros[i]->CurLoop*/->getLevel()) {
            LowestLevel = MB/*ActiveHMacros[i]->CurLoop*/->getLevel();
            KI = MB/*ActiveHMacros[i]->CurLoop*/->getKI();
            LastMB = MB;
          }
          else if (LowestLevel == MB/*ActiveHMacros[i]->CurLoop*/->getLevel())
            return -5; // Merging unequal Loop blocks is illegal;
        }
      }
      else {
        if (MaxCnt <= 0) {
          if (!MB->isKINested()) return -6;
          LowestLevel = 1;
          LastMB = MB/*ActiveHMacros[i]->CurLoop*/;
        }
        else {
          LastMB = MB/*ActiveHMacros[i]->CurLoop*/;
          KI = MB/*ActiveHMacros[i]->CurLoop*/->getKI();
          if (MaxCnt == 1) MaxCnt = MB->getCount();
          //LowestLevel = ActiveHMacros[i]->CurLoop->getLevel();
        }
        // else there is a RepeatImm before
      }
    }
    if (MB->getType(VAddr + 1) != None && MB->getType(VAddr + 1) != Repeat) // Record whether there was any sequential or loop block
      PreType = MB->getType(VAddr + 1);
    }
  }
  if (LastMB == NULL) return 4;
  if (MaxCnt == 1) MaxCnt = 0; // this should be unreachable
    ++VAddr;
  if (LowestLevel)
    if (!DealWithUnmatchedLoop(KI, LowestLevel, MaxCnt)) return -8;
  if (MaxCnt <= 1024) DealWithLoop(MaxCnt);
    --VAddr;
  return 0;
}

void MMPU::MMPUAsmParser::DealWithSequential() {
  MCLoopBlock *MB, *NewMB;
  for (unsigned i = 0; i < ActiveHMacros.size(); i++)
    if (ActiveHMacros[i]->getLeafContainer(VAddr, MB))
      if (MB->getType(VAddr) == Repeat) {
        MCLoopBlock *Top;
        ActiveHMacros[i]->getTop(VAddr, Top);
        NewMB = MB->UnrollLoop(MB->size());
        if (MB->getParent()) MB->addSiblingBlock(NewMB);
        else ActiveHMacros[i]->addLoopBlock(NewMB);
        Top->RecalStart(Top->getStart());
      }
}

void MMPU::MMPUAsmParser::DealWithRepeat(int64_t loops, MCInst *Inst, MCInst *CurInst) {
  MCLoopBlock *MB, *NewMB;
  if (loops <= 0) { // insts are all repeat KI
    for (unsigned i = 0; i < ActiveHMacros.size(); i++)
      if (ActiveHMacros[i]->getLeafContainer(VAddr, MB) &&
        MB->getType(VAddr) == Repeat) {
        MCLoopBlock *Top;
        ActiveHMacros[i]->getTop(VAddr, Top);
        if (MB->getCount() > loops) {
          NewMB = MB->UnrollLoop(loops - MB->getCount());
          if (NewMB != MB) {
            if (MB->getParent()) MB->addSiblingBlock(NewMB);
            else ActiveHMacros[i]->addLoopBlock(NewMB);
          }
        }
        else if (MB->getCount() < loops) {
          Error(CurLoc, "Virtual Address " + Twine(VAddr) + ": Repeat \
                        count is less than required unroll loops of " +
                        Twine(loops) + "\n");
          return;
        }
        Top->RecalStart(Top->getStart());
      }
      OptContext.mseq = MMPUInst::RepeatConds;
      OptContext.label = NULL;
      OptContext.ki = MB->getKI();
      OptContext.count = loops;
 /*     unsigned k = 0;
      MCSymbol *S = NULL;
      const MCConstantExpr *CE = MCConstantExpr::Create(loops, *Ctx);
      do {
        S = Ctx->LookupSymbol(Twine("KI" + Twine(MB->getKI()) +
                                     "CALIBR" + Twine(k++)));
      } while (S);
      S = Ctx->GetOrCreateSymbol(Twine("KI" + Twine(MB->getKI()) +
                                       "CALIBR" + Twine(k-1)));
      //S->setVariableValue(CE);
      Parser->getStreamer().EmitAssignment(S, CE);*/
  }
  else { // loops > 0, insts are all repeatImm;
    for (unsigned i = 0; i < ActiveHMacros.size(); i++)
      if (ActiveHMacros[i]->getLeafContainer(VAddr, MB))
        if (MB->getType(VAddr) == Repeat) {
          MCLoopBlock *Top;
          ActiveHMacros[i]->getTop(VAddr, Top);
          NewMB = MB->UnrollLoop(loops);
          if (NewMB != MB) {
            if (MB->getParent()) MB->addSiblingBlock(NewMB);
            else ActiveHMacros[i]->addLoopBlock(NewMB);
          }
          Top->RecalStart(Top->getStart());
        }
    OptContext.mseq = MMPUInst::RepeatImm;
    OptContext.label = NULL;
    OptContext.ki = 0;
    OptContext.count = loops;
  }
}

void MMPU::MMPUAsmParser::DealWithLoop(int64_t insts, bool Mode) {
  MCLoopBlock *MB;
  if (insts > 0) {
    for (unsigned i = 0; i < ActiveHMacros.size(); i++)
      if (ActiveHMacros[i]->getContainer(VAddr, MB) ||
        ActiveHMacros[i]->getNextBlock(VAddr, MB)) {
        while ((MB->getType(VAddr) == Loop) &&
          (MB->getStart() + MB->Headsize() != VAddr))
          if (MB->isNested()) MB->getLoopBlock(VAddr, MB);
          else return;
          if (MB->getType(VAddr) == Loop) { // Only Loop should be handled with
            MCLoopBlock *Top;
            ActiveHMacros[i]->getTop(VAddr, Top);
            MCLoopBlock *SubMB = NULL;
            if (Mode) {
              if (!(MB->getLoopBlock(VAddr, SubMB)) ||
                (SubMB->getType(VAddr) != Loop))
                continue;
              SubMB->UnrollLoop(insts);
            }
            else MB->UnrollLoop(insts);
//             if (Mode) {
//               MB->getTail().front()->getHead().push_back(MB->getHead().back());
//               SubMB->getHead().push_back(MB->getBlocks().back());
//               MB->getHead().pop_back();
//               MB->getBlocks().pop_back();
//             }
            //if (MB->getParent()) MB->addSiblingBlock(NewMB);
            //else ActiveHMacros[i]->addLoopBlock(NewMB);
            Top->RecalStart(Top->getStart());
          }
      }
  }
  else { //insts <= 0, means a new loop block waiting for issuing
    for (unsigned i = 0; i < ActiveHMacros.size(); i++) {
      if (ActiveHMacros[i]->getContainer(VAddr, MB) ||
        ActiveHMacros[i]->getNextBlock(VAddr, MB)) {
        MCLoopBlock *Top;
        ActiveHMacros[i]->getTop(VAddr, Top);
        while ((MB->getType(VAddr) == Loop) &&
          (MB->getStart() + MB->Headsize() != VAddr)) {
          if (MB->isNested()) MB->getLoopBlock(VAddr, MB); // Find the highest beginning loop,
            // the first sub block of a loop cannot be a loop
            else return;// in an unnested loop;
        }
        MCLoopBlock *SubMB = NULL;
        if (!LoopAvailable) {
          DealWithLoop(1);
          Top->RecalStart(Top->getStart());
          return;
        }
        if ((MB->getType(VAddr) == Loop) &&
          MB->isKINested() &&
          (MB->getLoopBlock(VAddr, SubMB)) &&
          (SubMB->getType(VAddr) == Loop)) {
          DealWithLoop(1, true);
        break;
        //Top->RecalStart(Top->getStart());
        }
      }
    }
    for (unsigned i = 0; i < ActiveHMacros.size(); i++) {
      if (ActiveHMacros[i]->getContainer(VAddr, MB) ||
        ActiveHMacros[i]->getNextBlock(VAddr, MB)) {
        MCLoopBlock *Top;
        ActiveHMacros[i]->getTop(VAddr, Top);
        while ((MB->getType(VAddr) == Loop) &&
          (MB->getStart() + MB->Headsize() != VAddr)) {
          if (MB->isNested()) MB->getLoopBlock(VAddr, MB); // Find the highest beginning loop,
            // the first sub block of a loop cannot be a loop
            else return;// in an unnested loop;
        }
        while (MB->getCount() > insts) {
          MB->UnrollLoop(-MB->size());
          /*if (NewMB != MB) {
          if (MB->getParent()) MB->addSiblingBlock(NewMB);
          else ActiveHMacros[i]->addLoopBlock(NewMB);
        }*/
        }
        Top->RecalStart(Top->getStart());
      }
    }
    MCSymbol *Sym = getContext().GetOrCreateSymbol(Twine("HM")+Twine(++UniqueLabelID));
    cachedLabel = true;
    const MCSymbolRefExpr *SE = MCSymbolRefExpr::Create(Sym, getContext());
    OptContext.mseq = MMPUInst::Loop;
    OptContext.label = SE;
    OptContext.ki = MB->getKI();
    OptContext.count = insts;
/*    unsigned k = 0;
    MCSymbol *S = NULL;
    const MCConstantExpr *CE = MCConstantExpr::Create(insts, *Ctx);
    do {
      S = Ctx->LookupSymbol(Twine("KI" + Twine(MB->getKI()) +
                                   "CALIBR" + Twine(k++)));
    } while (S);
    S = Ctx->GetOrCreateSymbol(Twine("KI" + Twine(MB->getKI()) +
                                     "CALIBR" + Twine(k-1)));
    Parser->getStreamer().EmitAssignment(S, CE);*/
  }
}
bool MMPU::MMPUAsmParser::DealWithUnmatchedLoop(uint64_t KI,
                                                unsigned Level,
                                                int64_t &MaxCnt) {
  MCLoopBlock *MB;
  MCBlockTy PreType = None;
  MaxCnt = 0;
  for (unsigned i = 0; i < ActiveHMacros.size(); i++) {
    if (ActiveHMacros[i]->getContainer(VAddr, MB) ||
      ActiveHMacros[i]->getNextBlock(VAddr, MB)) {
      MCLoopBlock *Top;
    ActiveHMacros[i]->getTop(VAddr, Top);
    while ((MB->getType(VAddr) == Loop) &&
      (MB->getStart() + MB->Headsize() != VAddr)) {
      if (MB->isNested()) MB->getLoopBlock(VAddr, MB); // Find the highest beginning loop
        else return false;// in an unnested loop;
    }
    if (MB->getType(VAddr) != Loop) {
      if (MB->getType(VAddr) == Sequential) MaxCnt = 1;
      else if (MB->getCount() <= 0) PreType = Repeat;
      else if (MaxCnt > MB->getCount() || MaxCnt <= 0)
        MaxCnt = MB->getCount();
      continue;
    }
    while ((MB->getType(VAddr) == Loop) &&
      (MB->getStart() + MB->Headsize() != VAddr)) {
      if (MB->isNested()) MB->getLoopBlock(VAddr, MB); // Find the highest beginning loop
        else return false;// in an unnested loop;
    }
    if (MB->getKI() == KI) {
      if (MB->getLevel() != Level) return false;
    }
    else {
      unsigned NumBlocks = MB->getHead().size();
      MCLoopBlock *tmp = MB;
      while (MB->getLevel() > Level) {
        if (!MB->isKINested()) return false;
        MB->UnrollOneBlock();
        Top->RecalStart(Top->getStart());
        if (MB->getHead().back()->getType() == Loop ||
            (MB->getHead().back()->getType() == Repeat &&
             MB->getHead().back()->getCount() <= 0)) {
          if (MB->getHead().back()->getLevel() == Level) {
            if (PreType == Repeat && MB->getHead().back()->getType() != Repeat)
              return false;
            else if (MB->getHead().back()->getKI() != KI)
              return false;
            else {
              if (MB->getHead().back()->getType() == Repeat) PreType = Repeat;
              //else if (MaxCnt <= 0 &&
              //         MaxCnt > MB->getHead().back()->getCount())
              //MaxCnt = MB->getHead().back()->getCount();
              MB = MB->getHead().back();
              break;
            }
          }
          else MB = MB->getHead().back();
        }
      }
      if (MB->getKI() != KI) return false;
      MB = tmp->getHead()[NumBlocks];
      while (!MB->getHead().empty()) MB = MB->getHead().front();
    }
    if (MB->getType() == Sequential) MaxCnt = 1;
    else if ((MB->getType() == Repeat && MB->getCount() > 1) &&
      (MaxCnt <= 0 || MaxCnt > MB->getCount()))
      MaxCnt = MB->getCount();
    else if (MB->getType() == Loop && MaxCnt <= 0 && MaxCnt > MB->getCount())
      MaxCnt = MB->getCount();
    }
    else return false;
  }
  if (PreType == Repeat) MaxCnt = 1025;
  return true;
}

void MMPU::MMPUAsmParser::tryMergeRedundantInst(MCInst* &Inst, MCInst* &CurInst) {
  bool identical = true;
  if (!OptContext.PreviousInst) {
    OptContext.PreviousInst = Inst;
    OptContext.PreviousLastInst = CurInst;
    OptContext._mseq = OptContext.mseq;
    OptContext._label = OptContext.label;
    OptContext._count = OptContext.count;
    OptContext._ki = OptContext.ki;
    Inst = NULL;
    CurInst = NULL;
    return;
  }
  if (OptContext.mseq == MMPUInst::Loop || OptContext._mseq == MMPUInst::Loop)
    identical = false;
  else if (OptContext.mseq == MMPUInst::RepeatConds &&
    OptContext._mseq == MMPUInst::RepeatConds)
    identical = false;
  else if (OptContext.mseq == MMPUInst::RepeatConds) {
    if (OptContext.count == 0) identical = false;
    else if (OptContext._mseq == MMPUInst::RepeatImm &&
      OptContext._count > -OptContext.count)
      identical = false;
  }
  else if (OptContext._mseq == MMPUInst::RepeatConds) {
    if (OptContext._count == 0) identical = false;
    else if (OptContext.mseq == MMPUInst::RepeatImm &&
      OptContext.count > -OptContext._count)
      identical = false;
  }
  const MCInst *IP = Inst, *_IP = OptContext.PreviousInst;
  for (unsigned i = 0; i < 14; i++) {
    if (IP->getOpcode() == _IP->getOpcode()) {
      if (IP->getNumOperands() == _IP->getNumOperands()) {
        for (unsigned j = 0; j < IP->getNumOperands(); j++) {
          if (IP->getOperand(j).isImm() && _IP->getOperand(j).isImm() &&
              IP->getOperand(j).getImm() == _IP->getOperand(j).getImm())
            continue;  //true
          if (IP->getOperand(j).isReg() && _IP->getOperand(j).isReg() &&
              IP->getOperand(j).getReg() == _IP->getOperand(j).getReg())
            continue;  //true
          if (IP->getOperand(j).isInst() && _IP->getOperand(j).isInst())
            continue;
          identical = false;
          break;
        }
        if (!identical) break;
        else if (i != 13){
          IP = IP->getOperand(IP->getNumOperands() - 1).getInst();
          _IP = _IP->getOperand(_IP->getNumOperands() - 1).getInst();
        }
      }
      else {
        identical = false;
        break;
      }
    }
    else {
      identical = false;
      break;
    }
  }
  if (!identical) {
    MCInst *tmp;
    tmp = OptContext.PreviousInst;
    OptContext.PreviousInst = Inst;
    Inst = tmp;
    tmp = OptContext.PreviousLastInst;
    OptContext.PreviousLastInst = CurInst;
    CurInst = tmp;
    unsigned mseqtmp = OptContext._mseq;
    OptContext._mseq = OptContext.mseq;
    OptContext.mseq = mseqtmp;
    const MCSymbolRefExpr *setmp;
    setmp = OptContext._label;
    OptContext._label = OptContext.label;
    OptContext.label = setmp;
    uint64_t lltmp;
    lltmp = OptContext._ki;
    OptContext._ki = OptContext.ki;
    OptContext.ki = lltmp;
    lltmp = OptContext._count;
    OptContext._count = OptContext.count;
    OptContext.count = lltmp;
    return;
  }
  if (OptContext._mseq == MMPUInst::NOP) {
    if (OptContext.mseq == MMPUInst::NOP) {
      OptContext._mseq = MMPUInst::RepeatImm;
      OptContext._count = 2;
    }
    else {
      OptContext._mseq = OptContext.mseq;
      OptContext._count = OptContext.count + 1;
      OptContext._ki = OptContext.ki;
    }
  }
  else {
    if (OptContext.mseq == MMPUInst::NOP)
      OptContext._count++;
    else if (OptContext.mseq == MMPUInst::RepeatConds) {
      OptContext._mseq = OptContext.mseq;
      OptContext._count += OptContext.count;
      OptContext._ki = OptContext.ki;
    }
    else
      OptContext._count += OptContext.count;
  }
  Inst = NULL;
  CurInst = NULL;
}

void MMPU::MMPUAsmParser::EmitCachedInst(SMLoc IDLoc, MCStreamer &Out) {
  MCInst *Inst = OptContext.PreviousInst;
  if (!Inst) return;
  MCInst *CurInst = OptContext.PreviousLastInst;
  if (OptContext._mseq != MMPUInst::NOP) {
    MCOperand MO;
    MCInst *IP = CurInst;
    IP->setOpcode(OptContext._mseq);
    if (OptContext._mseq == MMPUInst::Loop) {
      MO = MCOperand::CreateExpr(OptContext._label);
      IP->addOperand(MO);
    }
    if (OptContext._mseq == MMPUInst::Loop || OptContext._mseq == MMPUInst::RepeatConds) {
      MO = MCOperand::CreateImm(-OptContext._count);
      IP->addOperand(MO);
      MO = MCOperand::CreateImm(OptContext._ki);
    }
    else
      MO = MCOperand::CreateImm(OptContext._count);
    IP->addOperand(MO);
  }
  Inst->setLoc(IDLoc);
  Out.EmitInstruction(*Inst);
  OptContext.PreviousInst = NULL;
  OptContext.PreviousLastInst = NULL;
  OptContext._mseq = MMPUInst::NOP;
  OptContext._label = NULL;
  OptContext._ki = 0;
  OptContext._count = 0;
}

/// Force static initialization.
extern "C"
void
LLVMInitializeMMPUAsmParser()
{
	RegisterMCAsmParser<MMPU::MMPUAsmParser> Y(TheMMPUTarget);
}

/*
#define GET_REGISTER_MATCHER
#define GET_MATCHER_IMPLEMENTATION
#include "MMPUGenAsmMatcher.inc"
*/
