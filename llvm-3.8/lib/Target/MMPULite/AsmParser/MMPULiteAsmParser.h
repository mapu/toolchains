//===-- MMPULiteAsmParser.h - Parse MMPULite asm to MCInst instructions -----===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MMPULITEASMPARSER_H
#define MMPULITEASMPARSER_H

//#include "../MMPULiteInsnFlags.h"
#include "../MCTargetDesc/MMPULiteInsnLine.h"
#include "../MCTargetDesc/MMPULiteMCTargetDesc.h"

#include "llvm/MC/MCParser/AsmLexer.h"
#include "llvm/MC/MCParser/MCAsmLexer.h"
#include "llvm/MC/MCParser/MCAsmParser.h"
#include "llvm/MC/MCParser/MCAsmParserExtension.h"
#include "llvm/MC/MCParser/MCParsedAsmOperand.h"

#include "llvm/MC/MCParser/MCTargetAsmParser.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCDwarf.h"

#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/raw_ostream.h"

#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/Twine.h"

namespace llvm {
namespace Lite {
class MCFunction;
}
}
using namespace llvm::Lite;

namespace llvm {
namespace MMPULite {
enum AsmOperandKind {
  AsmHMacro, // slot is a special kind of helper register class
  AsmOpcode,
  AsmRegister,
  AsmImmediate,
  AsmExpression,
  AsmSlot
};

struct MMPULiteAsmOperand: public MCParsedAsmOperand {
  enum AsmOperandKind Kind;
  SMLoc StartLoc, EndLoc;

  union {
    std::string *Name; // the HMacro name
    unsigned Opc;  // the opcode.
    unsigned Reg;
    int64_t Imm;
    const MCExpr *Expr;
  };

public:
  MMPULiteAsmOperand(const MMPULiteAsmOperand &o)
    : MCParsedAsmOperand() {
    Kind = o.Kind;
    StartLoc = o.StartLoc;
    EndLoc = o.EndLoc;
    switch(Kind) {
    case AsmHMacro:
      Name = o.Name;
      break;
    case AsmRegister:
      Reg = o.Reg;
      break;
    case AsmImmediate:
      Imm = o.Imm;
      break;
    case AsmExpression:
      Expr = o.Expr;
      break;
    case AsmOpcode:
      Opc = o.Opc;
      break;
    case AsmSlot:
      Imm = o.Imm;
      break;
    }
  }

  MMPULiteAsmOperand(AsmOperandKind K)
        : MCParsedAsmOperand(), Kind(K) {}

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

  std::string *getHMacro() const {
    assert(Kind == AsmHMacro && "Invalid access!");
    return Name;
  }

  unsigned getReg() const {
    assert(Kind == AsmRegister && "Invalid access!");
    return Reg;
  }

  unsigned getOpc() const {
    assert(Kind == AsmOpcode && "Invalid access!");
    return Opc;
  }

  int64_t getImm() const {
    assert(Kind == AsmImmediate && "Invalid access!");
    return Imm;
  }

  int64_t getSlot() const {
    assert(Kind == AsmSlot && "Invalid access!");
    return Imm;
  }

  const MCExpr * getExpr() const {
    assert(Kind == AsmExpression && "Invalid access!");
    return Expr;
  }

  StringRef getSymName() const {
    assert(Kind == AsmExpression && "Invalid access!");
    if (!dyn_cast<MCSymbolRefExpr>(Expr)) return StringRef();
    else return dyn_cast<MCSymbolRefExpr>(Expr)->getSymbol().getName();
  }

  bool isHMacro() const {
    return Kind == AsmHMacro;
  }

  bool isToken() const {
    return Kind == AsmOpcode;
  }

  bool isImm() const {
    return Kind == AsmImmediate;
  }

  bool isSlot() const {
    return Kind == AsmSlot;
  }

  bool isReg() const {
    return Kind == AsmRegister;
  }

  bool isMem() const {
    return false;
  }

  bool isExpr() const {
    return Kind == AsmExpression;
  }

  /// print - Print a debug representation of the operand to the given stream.
  void print(raw_ostream &OS) const {
    OS << "<";
    switch(Kind) {
      case AsmImmediate:
        OS << "Imm:";
        break;
      case AsmOpcode:
        OS << "Inst:";
        break;
      case AsmRegister:
        OS << "Operand:";
        break;
      case AsmHMacro:
        OS << "HMacro:";
        break;
      case AsmExpression:
        OS << "Expr:";
        break;
      case AsmSlot:
        OS << "Slot:" << Imm << ">";
        return;
    }
    if (StartLoc.isValid() && EndLoc.isValid() &&
        StartLoc.getPointer() <= EndLoc.getPointer())
      OS << StringRef(StartLoc.getPointer(),
                      EndLoc.getPointer() - StartLoc.getPointer() + 1);
    else OS << "N/A";
    OS << ">";
  }

  static MMPULiteAsmOperand *createHMacro(std::string *Name,
                                          SMLoc S = SMLoc(), SMLoc E = SMLoc()) {
    MMPULiteAsmOperand *Op = new MMPULiteAsmOperand(AsmHMacro);
    Op->Name = Name;
    Op->StartLoc = S;
    Op->EndLoc = E;
    return Op;
  }

static MMPULiteAsmOperand *createOpc(unsigned opcode,
                                     SMLoc S = SMLoc(), SMLoc E = SMLoc()) {
    MMPULiteAsmOperand *Op = new MMPULiteAsmOperand(AsmOpcode);
    Op->Opc = opcode;
    Op->StartLoc = S;
    Op->EndLoc = E;
    return Op;
  }

  static MMPULiteAsmOperand *createReg(unsigned RegNum,
                                       SMLoc S = SMLoc(), SMLoc E = SMLoc()) {
    MMPULiteAsmOperand *Op = new MMPULiteAsmOperand(AsmRegister);
    Op->Reg = RegNum;
    Op->StartLoc = S;
    Op->EndLoc = E;
    return Op;
  }

  static MMPULiteAsmOperand *createImm(int64_t imm,
                                       SMLoc S = SMLoc(), SMLoc E = SMLoc()) {
    MMPULiteAsmOperand *Op = new MMPULiteAsmOperand(AsmImmediate);
    Op->Imm = imm;
    Op->StartLoc = S;
    Op->EndLoc = E;
    return Op;
  }

  static MMPULiteAsmOperand *createSlot(int64_t imm,
                                        SMLoc S = SMLoc(), SMLoc E = SMLoc()) {
    MMPULiteAsmOperand *Op = new MMPULiteAsmOperand(AsmSlot);
    Op->Imm = imm;
    Op->StartLoc = S;
    Op->EndLoc = E;
    return Op;
  }

  static MMPULiteAsmOperand *createExpr(const MCExpr *expr,
                                        SMLoc S = SMLoc(), SMLoc E = SMLoc()) {
    MMPULiteAsmOperand *Op = new MMPULiteAsmOperand(AsmExpression);
    Op->Expr = expr;
    Op->StartLoc = S;
    Op->EndLoc = E;
    return Op;
  }
};
#define OPERAND(TY, VALUE, START, END) \
MMPULite::MMPULiteAsmOperand::create##TY(VALUE, START, END)
#define SHARED_OPRD(TY, VALUE, START, END) \
SharedMMPUOprd(OPERAND(TY, VALUE, START, END))
#define ADDOPERAND(TY, VALUE, START, END) \
Operands.push_back(std::unique_ptr<MMPULite::MMPULiteAsmOperand>(std::unique_ptr<MMPULite::MMPULiteAsmOperand>(OPERAND(TY, VALUE, START, END))))

  
struct HMacro {
  StringRef Name;
  MCFunction *Body;
    
public:
  HMacro(StringRef N, MCFunction *B)
    : Name(N), Body(B) {}
};
  
} // namespace MMPULite
} // namespace llvm

#endif
