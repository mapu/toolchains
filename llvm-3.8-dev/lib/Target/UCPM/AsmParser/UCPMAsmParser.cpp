//===-- UCPMAsmParser.cpp - Parse UCPM asm to MCInst instructions -----===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "asm-parser"
#include "MCFunction.h"
#include "UCPMAsmParser.h"
#include "UCPMScheduler.h"
#include "llvm/ADT/Statistic.h"

#if !defined(NDEBUG) || defined(LLVM_ENABLE_STATS)
STATISTIC(NumHMInvoked, "Number of HMacroes invoked");
#else
static uint64_t NumHMInvoked = 0;
#endif

using namespace llvm;
using namespace llvm::UCPM;

/// UCPMAsmOperand - Instances of this class represent operands of parsed UCPM assembly instructions.
/// Note: the first operand records instruction opcode, see ARMGenAsmMatcher.inc MatchInstructionImpl().

namespace {
MCAsmLexer *Lexer;
MCAsmParser *llvmParser;
const MCRegisterInfo *MRI;
SMLoc CurLoc;
SMLoc TokStart;
}

namespace llvm {
namespace UCPM {

class HMacroInstantiation {
  MCFunction *Body;
  uint64_t Start;
  uint64_t Index;
  SmallVector<MCLoopBlock *, 8> Instantiation;
  std::string prefix;

public:
  MCLoopBlock *CurLoop;  // For debug only now
  HMacroInstantiation(MCFunction *MF, uint64_t S, std::string P)
    : Body(MF), Start(S), Index(0), prefix(P), CurLoop(NULL) {
  }

  StringRef getName() { return Body->getName(); }

  StringRef getPrefix() { return StringRef(prefix); }

  uint64_t getIndex() { return Index; }

  MCFunction *getBody() { return Body; }

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

  void Dump(raw_ostream &OS, std::string indent) {
    std::string prefix = indent;
    std::string arrow = "|-->";
    std::string step0 = "|    ";
    std::string step1 = "     ";
    OS << prefix + arrow << "No. of Fetched Lines:" << Index << "\n";
    OS << prefix + arrow << "Start Address:" << Start << "\n";
    OS << prefix + arrow << "Instantiated Block Info(" << Instantiation.size()
       << " Blocks):\n";
    prefix = step0 + prefix;
    for (unsigned i = 0; i < Instantiation.size(); i++) {
      OS << prefix + arrow << "Block " << i << " Info:\n";
      Instantiation[i]->
        Dump(OS, prefix + ((i == Instantiation.size() - 1) ? step1 : step0));
    }
  }
};
/*******************************************************************/

class UCPMAsmParser: public MCTargetAsmParser {
  const MCSubtargetInfo &STI;
  MCAsmParser &Parser;
  StringMap<HMacro*> HMacroMap;
  StringMap<bool> InvokedHM;
  std::map<std::string, std::string> PendingHM;
  std::vector<HMacroInstantiation*> ActiveHMacros;
  uint64_t VAddr;
  bool inHMacro;
  HMacro *CurHMacro;
  bool EnableOptimization;
  bool LoopAvailable;
  std::vector<int64_t> LocalLabels;
  int64_t UniqueLabelID;
  bool cachedLabel;
  UCPMScheduler *Scheduler;
  struct OptContextStruct {
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
  public:
    OptContextStruct()
      : PreviousInst(NULL), PreviousLastInst(NULL), _mseq(NOP), _label(NULL),
        _count(0), _ki(0), mseq(NOP), label(NULL), count(0), ki(0) {}
  } OptContext;

private:
  int AnalyzeHMacro(unsigned &Sym);
  void DealWithSequential(void);
  void DealWithRepeat(int64_t loops);
  void DealWithLoop(int64_t insts, bool Mode = false);
  bool DealWithUnmatchedLoop(uint64_t KI, unsigned Level, int64_t &MaxCnt);
  void tryMergeRedundantInst(MCInst* &Inst, MCInst* &CurInst);
  void EmitCachedInst(SMLoc IDLoc, MCStreamer &Out, unsigned Sym);
  void HMacroDump(void);
  void RenameHMacro(MCParsedInst *PI);
  void RenameHMacro(OperandVector &_Operands);
public:
  UCPMAsmParser(const MCSubtargetInfo &_STI, MCAsmParser &_Parser,
                    const MCInstrInfo &MII, const MCTargetOptions &Options)
    : MCTargetAsmParser(Options, _STI), STI(_STI), Parser(_Parser), CurHMacro(NULL) {
    MCAsmParserExtension::Initialize(_Parser);
    MRI = getContext().getRegisterInfo();
    Lexer = &(Parser.getLexer());
    llvmParser = &Parser;
    inHMacro = false;
    VAddr = 0;
    UniqueLabelID = 0;
    LoopAvailable = true;
    EnableOptimization = false;
    cachedLabel = false;
    Scheduler = new UCPMScheduler(Parser);
  }

  bool ParseRegister(unsigned &RegNo, SMLoc &StartLoc, SMLoc &EndLoc) override {
    return true;
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
  bool ParseDirective(AsmToken DirectiveID) override {
    // not target-specific and thus error happened during parsing.
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

  bool ParseDirectiveHMacro(AsmToken DirectiveID, SMLoc DirectiveLoc);

  bool ParseDirectiveEndHMacro(AsmToken DirectiveID, SMLoc DirectiveLoc);

  bool ParseDirectiveFlushHM(AsmToken DirectiveID, SMLoc DirectiveLoc);
  
  bool ParseDirectiveOpt(AsmToken DirectiveID, SMLoc DirectiveLoc);
  
  bool ParseDirectiveStopOpt(AsmToken DirectiveID, SMLoc DirectiveLoc);

  bool HandleHMacroEntry(StringRef Name, SMLoc NameLoc, const HMacro *M,
                         StringRef Prefix);

  void convertToMapAndConstraints(unsigned Kind, const OperandVector &Operands)
    override {}

  bool ParseInstruction(ParseInstructionInfo &Info, StringRef Name,
                        SMLoc NameLoc, OperandVector &Operands) override;

  bool MatchAndEmitInstruction(SMLoc IDLoc, unsigned &Opcode,
                               OperandVector &Operands, MCStreamer &Out,
                               uint64_t &ErrorInfo, bool MatchingInlineAsm) override;

  void onLabelParsed(MCSymbol *Symbol) override;
};
} // namespace UCPM
} // namespace llvm


namespace {
#define YY_INPUT(buf, result, max_size)                                   \
  do{                                                                     \
    unsigned i = 0;                                                       \
    const char *p = CurLoc.getPointer();                                  \
    if (Lexer->is(AsmToken::EndOfStatement) &&                            \
        p >= Lexer->getTok().getEndLoc().getPointer()) result = 0;        \
    else {                                                                \
      if (p == Lexer->getTok().getEndLoc().getPointer() + 1)              \
        Lexer->Lex();                                                     \
      while (i < max_size) {                                              \
        if (p == Lexer->getTok().getEndLoc().getPointer()) break;         \
        buf[i++] = tolower(*p++);                                         \
      }                                                                   \
      if (i != max_size) buf[i++] = tolower(*p++);                        \
      CurLoc = SMLoc::getFromPointer(p);                                  \
      result = i;                                                         \
    }                                                                     \
  } while (0)
#include "UCPMGenInstrFLexer.flex"

/* error handling for bison generated ucpmparse() */
void ucpmerror(YYLTYPE *Loc,
                   OperandVector &Operands,
               const char * p) {} //For suppressing "syntax error"

void llvmerror(YYLTYPE *Loc,
               const char * p) {
  llvmParser->Error(Loc->S, p);
}

#include "UCPMGenInstrParser.bison"
}

bool UCPM::UCPMAsmParser::
MatchAndEmitInstruction(SMLoc IDLoc, unsigned &Opcode, OperandVector &Operands,
                        MCStreamer &Out, uint64_t &ErrorInfo,
                        bool MatchingInlineAsm) {
  if (inHMacro) {
    Operands.clear();
    return false;
  }

  MCInst *Inst = new MCInst;
  MCInst *InstSort[17] = {};
  MCInst *CurInst = Inst, *NewInst;
  SmallVector<SharedOperand, 256> os;
  SharedOperandVector *ops;
  std::shared_ptr<UCPMAsmOperand> op;
  MCOperand MO;
  MCParsedInst *ParsedInst;
  std::vector<HMacroInstantiation*>::iterator iter;
  StringRef prefix = StringRef("/");
  StringRef new_prefix;

  // Transfer unique_ptr to shared_ptr
  for (unsigned i = 0; i < Operands.size(); i++)
    os.push_back(SharedOperand(std::move(Operands[i])));

  ops = &os;
  int OprSize = Operands.size();

  int RegNum = 0;
  unsigned j = 0;
  do {
    for(unsigned i=0; ops && i < ops->size(); i++) {
      op = std::static_pointer_cast<UCPMAsmOperand>((*ops)[i]);
      switch(op->getKind()) {
      default:
        return false;

      case AsmOpcode:
        if (op->getOpc() == UCPM::NOP) break;
        NewInst = new MCInst;
        MO = MCOperand::createInst(NewInst);
        CurInst->addOperand(MO);
        CurInst = NewInst;
        CurInst->setOpcode(op->getOpc());
        CurInst->setLoc(op->getStartLoc());
        Inst->addOperand(MO); //There is a redundant MCOperand in the first MCInst.
        break;

      case AsmRegister:
        MO = MCOperand::createReg(op->getReg());
        RegNum = op->getReg();
        CurInst->addOperand(MO);
        Inst->addOperand(MO);
        break;

      case AsmImmediate:
        MO = MCOperand::createImm(op->getImm());
        CurInst->addOperand(MO);
        Inst->addOperand(MO);
        break;

      case AsmExpression:
        MO = MCOperand::createExpr(op->getExpr());
        CurInst->addOperand(MO);
        Inst->addOperand(MO);
        break;

      case AsmHMacro:
        // When expanding a HMacro, it is necessary to distinguish whether
        // it is a new instance of a HMacro or it is an instance that has been
        // expanded and this is redundant one due to unrolling.
        new_prefix = StringRef((prefix + StringRef(*(op->getHMacro())) + StringRef("/")).str());
        if (PendingHM.count(*(op->getHMacro()))) { // Some HMacros are used before defined
          if (HMacro *HM = HMacroMap.lookup(StringRef(PendingHM[*(op->getHMacro())]))) {
            HMacroMap[StringRef(*(op->getHMacro()))]->Body = HM->Body;
            PendingHM.erase(PendingHM.find(*(op->getHMacro())));
          } else {
            Error(op->getStartLoc(), "Undefined HMacro " + PendingHM[*(op->getHMacro())]);
            PendingHM.erase(PendingHM.find(*(op->getHMacro())));
          }
        } else if (!InvokedHM.lookup(new_prefix)) {
          // If the HMacro was invoked, it is an unrolled instance
          if (HMacro *HM = HMacroMap.lookup(StringRef(*(op->getHMacro())))) {
            HandleHMacroEntry(StringRef(*(op->getHMacro())), IDLoc, HM, new_prefix);
            InvokedHM[new_prefix] = true;
          } else {
            Error(IDLoc, "Undefined HMacro " + *op->getHMacro());
            InvokedHM[new_prefix] = true;
          }
        }
        break;

      case AsmSlot:
        if (InstSort[op->getSlot()] != NULL) {
          if (j == 0)
            Error(IDLoc, "Slots collision detected."
                  " Please check following instlines:");
          else
            Error(IDLoc, "Slots collision detected when merging HMacro \"" +
                  ActiveHMacros[j - 1]->getName() +
                  "\". Please check following instlines:");
          Error(InstSort[op->getSlot()]->getLoc(), "Found Slot " +
          Twine(op->getSlot()) + " firstly used by");
          Error(op->getStartLoc(), "Found Slot " + Twine(op->getSlot()) +
          " used again by");
        }
        else {
          InstSort[op->getSlot()] = CurInst;
          CurInst->setLoc(op->getStartLoc());
        }
        break;
      }
    }
    ops = NULL;
    if (!ActiveHMacros.empty() && j < ActiveHMacros.size()) {
      if (ActiveHMacros[j]->getParsedInst(VAddr, ParsedInst)) {
        ops = ParsedInst->getOperands();
        prefix = ActiveHMacros[j]->getPrefix();
      }
      if (j++ < ActiveHMacros.size()) continue;
    }
    break;
  } while (1);
  unsigned Sym = 0;
  int Res = AnalyzeHMacro(Sym);
  if (Res < 0) {
    Twine message = "Virtual Address " + Twine(VAddr) + ": Analysis returns " +
                    "an error\n" + Twine(Res) + ": ";
    switch (Res) {
    default:
      Error(CurLoc, message + "Unknown error.\n");
      break;
    case -1:
      Error(CurLoc, message + "Unable to issue instructions when Loop meets " +
            "Repeat. An error probably occurred in the analysis prior to " +
            "this time.\n");
      break;
    case -2:
      Error(CurLoc, message + "Cannot find an instruction at Virtual Address " +
            Twine(VAddr) + ". HMacro is not continuous.\n");
      break;
    case -3:
      Error(CurLoc, message + "Instantiated HMacroes didn't go into loop " +
            "simultaneously. Previous analysis encountered an error\n");
      break;
    case -4:
      Error(CurLoc, message + "Loops in HMacroes have different nested levels" +
            "\n");
      break;
    case -5:
      Error(CurLoc, message + "Unable to merge loops in instantiated HMacros " +
            "due to unequal loop bodies\n");
      break;
    case -6:
      Error(CurLoc, message +"Unable to merge REPEAT @(KI) with unnested" +
            " LPTO @(KI)\n");
      break;
    case -7:
      Error(CurLoc, message +"Unable to merge REPEAT @(KI) using different " +
            "KIs\n");
      break;
    case -8:
      Error(CurLoc, message +"Unable to handle unmatched loops\n");
      break;
    }
    HMacroDump();
    ActiveHMacros.clear();
  }
  if (Inst->getNumOperands() == 0) {
    NewInst = new MCInst();
    NewInst->setOpcode(UCPM::NOP);
    Inst->addOperand(MCOperand::createInst(NewInst));
    CurInst = NewInst;
  }
  if (EnableOptimization) tryMergeRedundantInst(Inst, CurInst);
  if (Inst) {
    if (OptContext.mseq != UCPM::NOP) {
      MCOperand MO;
      MCInst *IP = new MCInst();
      IP->setOpcode(OptContext.mseq);
      MO = MCOperand::createInst(IP);
      CurInst->addOperand(MO);
      Inst->addOperand(MO);
      if (OptContext.mseq == UCPM::LPTO) {
        MO = MCOperand::createExpr(OptContext.label);
        IP->addOperand(MO);
        Inst->addOperand(MO);
      }
      if (OptContext.mseq == UCPM::LPTO || OptContext.mseq == UCPM::REPEATK) {
        MO = MCOperand::createImm(OptContext.ki);
        IP->addOperand(MO);
        Inst->addOperand(MO);
        MO = MCOperand::createImm(-OptContext.count);
      }
      else
        MO = MCOperand::createImm(OptContext.count);
      IP->addOperand(MO);
      Inst->addOperand(MO);
    }
    Inst->setLoc(IDLoc);
    Scheduler->Schedule(VAddr, Inst, OptContext.PreviousInst ? 0 : Sym);
    Out.EmitInstruction(*Inst, STI);
  }
  OptContext.mseq = UCPM::NOP;
  OptContext.label = NULL;
  OptContext.ki = 0;
  OptContext.count = 0;
  if (EnableOptimization && Sym)
    EmitCachedInst(IDLoc, Out, Sym);
  while (Sym) {
    MCSymbol *S = getContext().getOrCreateSymbol(Twine("HM\1") +
                                                 Twine(LocalLabels.back()));
    LocalLabels.pop_back();
    Out.EmitLabel(S);
    --Sym;
  }
  if (cachedLabel) LocalLabels.push_back(UniqueLabelID);
  cachedLabel = false;
  ++VAddr;
  return false; // Has any parsing error?
}

/// Parse an UCPM instruction mnemonic followed by its operands.
bool UCPM::UCPMAsmParser::
ParseInstruction(ParseInstructionInfo &Info, StringRef Name, SMLoc NameLoc,
                 OperandVector &Operands) {
  YY_BUFFER_STATE buf = ucpm_create_buffer(0, YY_BUF_SIZE);
  ucpm_switch_to_buffer(buf);
  ucpm_flush_buffer(buf);
  CurLoc = NameLoc;
  TokStart = NameLoc;  // Used for tracing the start location of each token

  BEGIN INITIAL;
  int err =  ucpmparse(Operands);

  ucpm_delete_buffer(buf);
  while (Lexer->isNot(AsmToken::EndOfStatement) &&
         Lexer->isNot(AsmToken::PipePipe))
    Lex();
  if (err) {
    getParser().Error(NameLoc, "Parse Instruction failed");
    if (Lexer->is(AsmToken::EndOfStatement)) {
      if (inHMacro) {
        MCParsedInst *MCPI = new MCParsedInst(Operands);
        CurHMacro->Body->addParsedInst(MCPI);
        RenameHMacro(MCPI);
      }
    }
    Lex();
    return true;
  }
  if (Lexer->is(AsmToken::EndOfStatement)) Lex();
  if (inHMacro) {
    MCParsedInst *MCPI = new MCParsedInst(Operands);
    CurHMacro->Body->addParsedInst(MCPI);
    RenameHMacro(MCPI);
  } else
    RenameHMacro(Operands);
  return false;
}

bool UCPM::UCPMAsmParser::ParseDirectiveHMacro(AsmToken DirectiveID,
                                                       SMLoc DirectiveLoc) {
  StringRef Name;
  if (inHMacro)
    return TokError("nested hmacro definition is not allowed");
  if (getParser().parseIdentifier(Name))
    return TokError("expected identifier in '.hmacro' directive");
  if (Lexer->isNot(AsmToken::EndOfStatement)) 
    return TokError("parameters are currently not supported");
  Lex();

  if (HMacroMap.lookup(Name.lower()))
    return Error(DirectiveLoc, "hmacro '" + Name.lower() + "' is already defined");

  MCFunction *Body = new MCFunction(*(new std::string(Name.lower())), 0);
  CurHMacro = new HMacro(Body->getName(), Body);
  inHMacro = true;
  return false;
}

bool UCPM::UCPMAsmParser::ParseDirectiveEndHMacro(AsmToken DirectiveID,
                                                          SMLoc DirectiveLoc) {
  if (!inHMacro)
    return TokError("no matching '.hmacro' in definition");
  if (Lexer->isNot(AsmToken::EndOfStatement)) 
    return TokError("unexpected token after '.endhmacro'");
  Lex();
  
  CurHMacro->Body->Finalize();
  CurHMacro->Body->EliminateEmptyBlock();
  HMacroMap[CurHMacro->Name] = CurHMacro;
  inHMacro = false;

  while (!CurHMacro->Body->isPendingLabelsEmpty()) {
    if (!CurHMacro->Body->PendingLabels.back().second)
      Warning(CurHMacro->Body->PendingLabels.back().first->getStartLoc(),
              "Undefined LPTO label in hmacro " + CurHMacro->Name + ".");
    CurHMacro->Body->PendingLabels.pop_back();
  }
  while (!CurHMacro->Body->isDisorderedLabelsEmpty()) {
    Warning(CurHMacro->Body->DisorderedLabels.back().first->getStartLoc(),
            "Undefined LPTO label in hmacro " + CurHMacro->Name + ".");
    CurHMacro->Body->DisorderedLabels.pop_back();
  }
  return false;
}

bool UCPM::UCPMAsmParser::
ParseDirectiveFlushHM(AsmToken DirectiveID, SMLoc DirectiveLoc) {
  if (inHMacro)
    return TokError("no matching '.endhmacro' in definition");
  if (Lexer->isNot(AsmToken::EndOfStatement)) 
    return TokError("unexpected token after '.flushhm'");
  Lex();
 
  SmallVector<std::unique_ptr<MCParsedAsmOperand>, 8> Operands;
  uint64_t ErrorInfo;
  unsigned Opcode;
  SMLoc Loc;
  while (!ActiveHMacros.empty()) {
    MatchAndEmitInstruction(Loc, Opcode, Operands, Parser.getStreamer(),
                            ErrorInfo, false);
  }
  EmitCachedInst(Loc, Parser.getStreamer(), 0);
  return false;
}

bool UCPM::UCPMAsmParser::ParseDirectiveOpt(AsmToken DirectiveID,
                                                    SMLoc DirectiveLoc) {
  EnableOptimization = true;
  return false;
}

bool UCPM::UCPMAsmParser::ParseDirectiveStopOpt(AsmToken DirectiveID,
                                                        SMLoc DirectiveLoc) {
  EnableOptimization = false;
  SMLoc Loc;
  EmitCachedInst(Loc, Parser.getStreamer(), 0);
  return false;
}

bool UCPM::UCPMAsmParser::HandleHMacroEntry(StringRef Name,
                                                    SMLoc NameLoc,
                                                    const HMacro *M,
                                                    StringRef Prefix) {
  if (ActiveHMacros.size() == 20)
    return TokError("hmacros cannot be nested more than 20 levels deep and/or "
                    "concurrently being expanded more than 20 instances");
  MCFunction *MF = M->Body;
  if (MF->isEmbedded())
    MF->ExpandEmbeddedHM(HMacroMap);
  ActiveHMacros.push_back(new HMacroInstantiation(MF, VAddr, Prefix.str()));
  return true;
}

/* onLabelParsed: In LLVM 3.5, Local temp labels are no longer collected in
 * Symbols vector, and LookupSymbol method cannot find local labels any more.
 * So here utilize onLabelParsed function to keep tracking Label emit status
 * in HMacro definitions
 */
void UCPM::UCPMAsmParser::onLabelParsed(MCSymbol *Symbol) {
  SmallVector<std::pair<SharedMMPUOprd, bool>, 8> MatchedLabels;
  // Labels out of hmacro are not insterested
  if (!inHMacro || !CurHMacro) return;

  while (!CurHMacro->Body->isPendingLabelsEmpty() &&
         (CurHMacro->Body->PendingLabels.back().first->getSymName() !=
          Symbol->getName())) {
    if (CurHMacro->Body->PendingLabels.back().second == false) {
      // The most recent loop label is not equal to this label, so move the
      // pending label into disordered labels
      Warning(CurHMacro->Body->PendingLabels.back().first->getStartLoc(),
              "Symbol \"" +
              CurHMacro->Body->PendingLabels.back().first->getSymName() +
              "\" is expected before defining Label \"" + Symbol->getName() +
              "\".");
      CurHMacro->Body->DisorderedLabels.push_back(CurHMacro->Body->PendingLabels.back());
      CurHMacro->Body->PendingLabels.pop_back();
    } else {
      // Move previously matched labels from pending labels into matched labels.
      // This may happen due to nested loops endes at the same place.
      MatchedLabels.push_back(CurHMacro->Body->PendingLabels.back());
      CurHMacro->Body->PendingLabels.pop_back();
    }
  }
  if (!CurHMacro->Body->isPendingLabelsEmpty()) {
    // Found a matching one in pending labels
    CurHMacro->Body->PendingLabels.back().second = true;
    // PendingLabels may have the same name, so all of them should
    // be set to true here, and if any disorder happens, it will be detected
    bool continuous = true;
    for (int32_t i = CurHMacro->Body->PendingLabels.size() - 1;
         i >= 0; i--) {
      if (!continuous &&
          CurHMacro->Body->PendingLabels[i].first->getSymName() ==
          Symbol->getName())
        TokError("Loop label \"" +
                 CurHMacro->Body->PendingLabels[i].first->getSymName() +
                 "\" used by several LPTOs is incorrect. " + Symbol->getName() +
                 " is disordered.");
      if (CurHMacro->Body->PendingLabels[i].first->getSymName() ==
          Symbol->getName())
        CurHMacro->Body->PendingLabels[i].second = true;
      else continuous = false;
    }
    while (!MatchedLabels.empty()) {
      CurHMacro->Body->PendingLabels.push_back(MatchedLabels.back());
      MatchedLabels.pop_back();
    }
    return;
  } else {
    // Cannot find one in pending labels, try to find it in disordered labels.
    while (!MatchedLabels.empty()) {
      CurHMacro->Body->PendingLabels.push_back(MatchedLabels.back());
      MatchedLabels.pop_back();
    }
    SmallVector<std::pair<SharedMMPUOprd, bool>, 8>::iterator iter =
      CurHMacro->Body->DisorderedLabels.begin();
    while (iter != CurHMacro->Body->DisorderedLabels.end()) {
      if (iter->first->getSymName() == Symbol->getName()) {
        TokError("Loop label \"" + iter->first->getSymName() + "\" is out of order.");
        CurHMacro->Body->PendingLabels.push_back(*iter);
        CurHMacro->Body->DisorderedLabels.erase(iter);
        CurHMacro->Body->PendingLabels.back().second = true;
        return;
      }
      iter++;
    }
    // This is a spurious label.
    Warning(Parser.getTok().getLoc(), "Spurious label \"" + Symbol->getName() +
            "\" is found in hmacro \"" + CurHMacro->Name + "\".");
  }
}

/* AnalyzeHMacro: All instantiated HMacros will be analyzed here to determine if 
 * LoopBlocks need to be unrolled or not
 */
int UCPM::UCPMAsmParser::AnalyzeHMacro(unsigned &Sym) {
  MCLoopBlock* MB;
  MCBlockTy PreType = UCPM::None, CurType;
  int64_t MaxCnt = 0;
  unsigned CheckLoops = 0;
  bool Done = false;
  for (unsigned i = 0; i < ActiveHMacros.size(); i++) {
    if (ActiveHMacros[i]->getContainer(VAddr, MB)) {
      CurType = MB->getType(VAddr);
      if (CurType == Loop) ++CheckLoops;
      // else it's in the head or tail of this loop
      if (Done) continue;
      if (MB->getType() == Loop) {
        ActiveHMacros[i]->getLeafContainer(VAddr, MB);
        // Treat unnested Loop as Sequential in order to deal
        // with unrolling inside the loop
        if (MB->getType(VAddr) == Loop) CurType = Sequential;
        else CurType = MB->getType(VAddr);
      }
      // first of all, check if there is a sequential block
      if ((CurType == Sequential && PreType == Repeat) ||
          (CurType == Repeat && PreType == Sequential)) {
        DealWithSequential();
        PreType = UCPM::None;
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
        else if (PreType == UCPM::None)
          MaxCnt = MB->getCount();
        else // loop with repeat is illegal
          return -1;
      }
      if (CurType != UCPM::None) PreType = CurType;
    }
    else return -2; // Cannot find VAddr in one of ActiveHMacros, this is an error.
  }
  if (PreType == Repeat) {
    DealWithRepeat(MaxCnt);
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
        if (CurSym != Sym) return -4; // Loop levels are not consistent;
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
  PreType = UCPM::None;
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
        else if (MB->getCount() <= 0) {
          if (MaxCnt <= 0 && KI != MB->getKI()) return -7; // RepeatKIs using different KIs
          if (MaxCnt <= 1) MaxCnt = MB->getCount(); // MaxCnt = 1: there is no Repeat before, MaxCnt < 1: there is at least one Repeat KI before.
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
            if (LowestLevel > MB->getLevel()) {
              LowestLevel = MB->getLevel();
              KI = MB->getKI();
              LastMB = MB;
            }
            else if (LowestLevel == MB->getLevel())
              return -5; // Merging unequal Loop blocks is illegal;
          }
        }
        else {
          if (MaxCnt <= 0) {
            if (!MB->isKINested()) return -6;
            LowestLevel = 1;
            LastMB = MB;
          }
          else {
            LastMB = MB;
            KI = MB->getKI();
            if (MaxCnt == 1) MaxCnt = MB->getCount();
            //LowestLevel = ActiveHMacros[i]->CurLoop->getLevel();
          }
          // else there is a RepeatImm before
        }
      }
      if (MB->getType(VAddr + 1) != UCPM::None &&
          MB->getType(VAddr + 1) != Repeat) // Record whether there was any sequential or loop block
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

void UCPM::UCPMAsmParser::DealWithSequential() {
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

void UCPM::UCPMAsmParser::DealWithRepeat(int64_t loops) {
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
          TokError("Virtual Address " + Twine(VAddr) + ": Repeat " +
                   "count is less than required unroll loops of " +
                   Twine(loops) + "\n");
          return;
        }
        Top->RecalStart(Top->getStart());
      }
    OptContext.mseq = UCPM::REPEATK;
    OptContext.label = NULL;
    OptContext.ki = MB->getKI();
    OptContext.count = loops;
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
    OptContext.mseq = UCPM::REPEATIMM;
    OptContext.label = NULL;
    OptContext.ki = 0;
    OptContext.count = loops;
  }
}

void UCPM::UCPMAsmParser::DealWithLoop(int64_t insts, bool Mode) {
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
        while (MB->getCount() > insts) MB->UnrollLoop(-MB->size());
        Top->RecalStart(Top->getStart());
      }
    }
    MCSymbol *Sym = getContext().getOrCreateSymbol(Twine("HM\1")+Twine(++UniqueLabelID));
    cachedLabel = true;
    const MCSymbolRefExpr *SE = MCSymbolRefExpr::create(Sym, getContext());
    OptContext.mseq = UCPM::LPTO;
    OptContext.label = SE;
    OptContext.ki = MB->getKI();
    OptContext.count = insts;
  }
}

bool UCPM::UCPMAsmParser::DealWithUnmatchedLoop(uint64_t KI,
                                                unsigned Level,
                                                int64_t &MaxCnt) {
  MCLoopBlock *MB;
  MCBlockTy PreType = UCPM::None;
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

void UCPM::UCPMAsmParser::tryMergeRedundantInst(MCInst* &Inst,
                                                MCInst* &CurInst) {
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
  if (OptContext.mseq == UCPM::LPTO || OptContext._mseq == UCPM::LPTO)
    identical = false;
  else if (OptContext.mseq == UCPM::REPEATK &&
           OptContext._mseq == UCPM::REPEATK)
    identical = false;
  else if (OptContext.mseq == UCPM::REPEATK) {
    if (OptContext.count == 0) identical = false;
    else if (OptContext._mseq == UCPM::REPEATIMM &&
             OptContext._count > -OptContext.count)
      identical = false;
  }
  else if (OptContext._mseq == UCPM::REPEATK) {
    if (OptContext._count == 0) identical = false;
    else if (OptContext.mseq == UCPM::REPEATIMM &&
             OptContext.count > -OptContext._count)
      identical = false;
  }
  const MCInst *IP = Inst, *_IP = OptContext.PreviousInst;
  if (identical && IP->getNumOperands() != _IP->getNumOperands())
    identical = false;
  else {
    IP = IP->getOperand(0).getInst();
    _IP = _IP->getOperand(0).getInst();
    do {
      do {
        if (IP->getOpcode() != _IP->getOpcode()) {
          if (_IP->getNumOperands() == 0 ||
              !_IP->getOperand(_IP->getNumOperands() - 1).isInst()) {
            identical = false;
            break;
          }
          else {
            _IP = _IP->getOperand(_IP->getNumOperands() - 1).getInst();
            continue;
          }
        }
        if (IP->getNumOperands() == 0 && _IP->getNumOperands() == 0)
          break;
        if (IP->getNumOperands() != _IP->getNumOperands()) {
          if ((IP->getNumOperands() + 1 == _IP->getNumOperands() &&
               !IP->getOperand(IP->getNumOperands() - 1).isInst()) ||
              (IP->getNumOperands() == _IP->getNumOperands() + 1 &&
               IP->getOperand(IP->getNumOperands() - 1).isInst())) {
            if (IP->getNumOperands() == 0 || _IP->getNumOperands() == 0) break;
          }
          else {
            identical = false;
            break;
          }
        }
        for (unsigned i = 0; i < IP->getNumOperands() - 1; i++) {
          if (IP->getOperand(i).isImm() && _IP->getOperand(i).isImm() &&
              IP->getOperand(i).getImm() == _IP->getOperand(i).getImm())
            continue;
          if (IP->getOperand(i).isReg() && _IP->getOperand(i).isReg() &&
              IP->getOperand(i).getReg() == _IP->getOperand(i).getReg())
            continue;
          identical = false;
          break;
        }
        if (IP->getNumOperands() <= _IP->getNumOperands() && identical) {
          unsigned i = IP->getNumOperands() - 1;
          if (IP->getOperand(i).isImm() && _IP->getOperand(i).isImm() &&
              IP->getOperand(i).getImm() == _IP->getOperand(i).getImm())
            break;  //true
          if (IP->getOperand(i).isReg() && _IP->getOperand(i).isReg() &&
              IP->getOperand(i).getReg() == _IP->getOperand(i).getReg())
            break;  //true
          if (IP->getOperand(i).isInst() && _IP->getOperand(i).isInst())
            break;  //true
          identical = false;
          break;
        }
        else break;
      } while (identical);
      if (IP->getNumOperands() == 0) break;
      else if (!IP->getOperand(IP->getNumOperands() - 1).isInst()) break;
      else IP = IP->getOperand(IP->getNumOperands() - 1).getInst();
    } while (identical);
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
  if (OptContext._mseq == UCPM::NOP) {
    if (OptContext.mseq == UCPM::NOP) {
      OptContext._mseq = UCPM::REPEATIMM;
      OptContext._count = 2;
    }
    else {
      OptContext._mseq = OptContext.mseq;
      OptContext._count = OptContext.count + 1;
      OptContext._ki = OptContext.ki;
    }
  }
  else {
    if (OptContext.mseq == UCPM::NOP)
      OptContext._count++;
    else if (OptContext.mseq == UCPM::REPEATK) {
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

void UCPM::UCPMAsmParser::EmitCachedInst(SMLoc IDLoc, MCStreamer &Out, unsigned Sym) {
  MCInst *Inst = OptContext.PreviousInst;
  if (!Inst) return;
  MCInst *CurInst = OptContext.PreviousLastInst;
  if (OptContext._mseq != UCPM::NOP) {
    MCOperand MO;
    MCInst *IP = new MCInst();
    IP->setOpcode(OptContext._mseq);
    MO = MCOperand::createInst(IP);
    CurInst->addOperand(MO);
    Inst->addOperand(MO);
    if (OptContext._mseq == UCPM::LPTO) {
      MO = MCOperand::createExpr(OptContext._label);
      IP->addOperand(MO);
      Inst->addOperand(MO);
    }
    if (OptContext._mseq == UCPM::LPTO || OptContext._mseq == UCPM::REPEATK) {
      MO = MCOperand::createImm(OptContext._ki);
      IP->addOperand(MO);
      Inst->addOperand(MO);
      MO = MCOperand::createImm(-OptContext._count);
    }
    else
      MO = MCOperand::createImm(OptContext._count);
    IP->addOperand(MO);
    Inst->addOperand(MO);
  }
  Inst->setLoc(IDLoc);
  Scheduler->Schedule(VAddr, Inst, Sym);
  Out.EmitInstruction(*Inst, STI);
  OptContext.PreviousInst = NULL;
  OptContext.PreviousLastInst = NULL;
  OptContext._mseq = UCPM::NOP;
  OptContext._label = NULL;
  OptContext._ki = 0;
  OptContext._count = 0;
}

void UCPM::UCPMAsmParser::HMacroDump() {
  raw_ostream &OS = errs();
  OS.changeColor(raw_ostream::SAVEDCOLOR, true);
  OS << "Dumping Active HMacro List:\n"
     << "(T: Type; A: Address; K: KI; C: Count; S: Size; AS: Absolute Size; "
     << "(*): Current Block)\n";
  for (unsigned i = 0; i < ActiveHMacros.size(); i++) {
    MCFunction *MF = ActiveHMacros[i]->getBody();
    MCLoopBlock *MB;// = ActiveHMacros[i]->CurLoop;
    OS << "\"" << ActiveHMacros[i]->getName() << "\":\n";
    ActiveHMacros[i]->Dump(OS, "");
    OS << "|-->Original HMacro Info:\n";
    unsigned j = 0;
    for (; j < MF->size() && MF->getLoopBlock(j, MB); j += MB->size()) {
      std::string curtag = "(*)";
      if (MB == ActiveHMacros[i]->CurLoop)
        OS.changeColor(raw_ostream::RED, true);
      else curtag = "";
      OS << "     |-->" << curtag << "Block at " << j << " Info:\n";
      std::string indent;
      if (j + MB->size() < MF->size()) indent = "     |    ";
      else indent = "          ";
      MB->Dump(OS, indent);
      OS.resetColor();
      OS.changeColor(raw_ostream::SAVEDCOLOR, true);
    }
  }
  OS << "\n";
  OS.resetColor();
}

void UCPM::UCPMAsmParser::RenameHMacro(MCParsedInst *PI) {
  for(unsigned i= 0; i < PI->size(); ++i) {
    std::shared_ptr<UCPM::UCPMAsmOperand> op =
      std::static_pointer_cast<UCPM::UCPMAsmOperand>(PI->getOperand(i));
    if (op->isHMacro()) {
      const HMacro *HM = HMacroMap.lookup(StringRef(*(op->getHMacro())));
      std::string OldName = *(op->getHMacro());
      op->getHMacro()->append(Twine("\1").str());
      op->getHMacro()->append(Twine(NumHMInvoked).str());
      if (!HM) PendingHM[*(op->getHMacro())] = OldName;
      HMacro *newHM = new HMacro(StringRef(*op->getHMacro()),
                                 HM ? HM->Body : NULL);
      HMacroMap[StringRef(*op->getHMacro())] = newHM;
      ++NumHMInvoked;
    }
  }
}

void UCPM::UCPMAsmParser::
RenameHMacro(OperandVector &_Operands) {
  for(unsigned i= 0; i < _Operands.size(); ++i) {
    UCPM::UCPMAsmOperand* op =
      static_cast<UCPM::UCPMAsmOperand *>(_Operands[i].get());
    if (op->isHMacro()) {
      const HMacro *HM = HMacroMap.lookup(StringRef(*(op->getHMacro())));
      std::string OldName = *(op->getHMacro());
      op->getHMacro()->append(Twine("\1").str());
      op->getHMacro()->append(Twine(NumHMInvoked).str());
      if (!HM) PendingHM[*(op->getHMacro())] = OldName;
      HMacro *newHM = new HMacro(StringRef(*op->getHMacro()),
                                 HM ? HM->Body : NULL);
      HMacroMap[StringRef(*op->getHMacro())] = newHM;
      ++NumHMInvoked;
    }
  }
}

/// Force static initialization.
extern "C"
void
LLVMInitializeUCPMAsmParser() {
  RegisterMCAsmParser<UCPM::UCPMAsmParser> X(TheUCPMTarget);
}
/*
#define GET_REGISTER_MATCHER
#define GET_MATCHER_IMPLEMENTATION
#include "UCPMGenAsmMatcher.inc"
*/
