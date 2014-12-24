#ifndef MMPU_ASM_OPERAND_H
#define MMPU_ASM_OPERAND_H

#include "../MCTargetDesc/MMPUMCInst.h"
#include "../MCTargetDesc/MMPUInsnLine.h"
#include "../MCTargetDesc/MMPUMCTargetDesc.h"

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

namespace llvm
{
  namespace MMPU
  {
    enum AsmOperandKind
    {
      AsmSlot, // slot is a special kind of helper register class
      AsmOpc,
      AsmReg,
      AsmImm,
      AsmExpr,
      AsmMacro,
      AsmLoc
    };

    struct MMPUAsmOperand: public MCParsedAsmOperand
    {
      private:
        enum AsmOperandKind Kind;

        SMLoc StartLoc, EndLoc;

        union
        {
            unsigned Slot; // the slot number this instruction resides in
            unsigned Opc;  // the opcode.
            unsigned Reg;
            int64_t Imm;
            const MCExpr * Expr;
            const MCSymbol * Macro;
            const char* Loc;
        };

      public:
        MMPUAsmOperand(const MMPUAsmOperand &o)
              : MCParsedAsmOperand()
        {
          Kind = o.Kind;
          StartLoc = o.StartLoc;
          EndLoc = o.EndLoc;
          switch(Kind) {
            case AsmSlot:
              Slot = o.Slot;
              break;
            case AsmReg:
              Reg = o.Reg;
              break;
            case AsmImm:
              Imm = o.Imm;
              break;
            case AsmExpr:
              Expr = o.Expr;
              break;
            case AsmOpc:
              Opc = o.Opc;
            case AsmMacro:
              Macro = o.Macro;
              break;
            case AsmLoc:
              Loc = o.Loc;
              break;

            default:
              llvm_unreachable("unknown operand kind");
          }
        }

        MMPUAsmOperand(AsmOperandKind K)
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
        getSlot() const
        {
          assert(Kind == AsmSlot && "Invalid access!");
          return Slot;
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

        const char*
        getLoc() const
        {
          assert(Kind == AsmLoc && "Invalid access!");
          return Loc;
        }

        int64_t
        getImm() const
        {
          assert(Kind == AsmImm && "Invalid access!");
          return Imm;
        }

        const MCExpr *
        getExpr() const
        {
          assert(Kind == AsmExpr && "Invalid access!");
          return Expr;
        }

        const MCSymbol *
        getMacro() const
        {
          assert(Kind == AsmMacro && "Invalid access!");
          return Macro;
        }

        bool
        isToken() const
        {
          return Kind == AsmOpc;
        }

        bool
        isLoc() const
        {
          return Kind == AsmLoc;
        }

        bool
        isMacro() const
        {
          return Kind == AsmMacro;
        }

        bool
        isImm() const
        {
          return Kind == AsmImm;
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

        static MMPUAsmOperand *
        createSlot(unsigned slot)
        {
          MMPUAsmOperand *Op = new MMPUAsmOperand(AsmSlot);
          Op->Slot = slot;
          return Op;
        }

        static MMPUAsmOperand *
        createOpc(unsigned opcode)
        {
          MMPUAsmOperand *Op = new MMPUAsmOperand(AsmOpc);
          Op->Opc = opcode;
          return Op;
        }

        static MMPUAsmOperand *
        createLoc(const char* loc)
        {
          MMPUAsmOperand *Op = new MMPUAsmOperand(AsmLoc);
          Op->Loc = loc;
          return Op;
        }

        static MMPUAsmOperand *
        createReg(unsigned RegNum)
        {
          MMPUAsmOperand *Op = new MMPUAsmOperand(AsmReg);
          Op->Reg = RegNum;
          return Op;
        }

        static MMPUAsmOperand *
        createImm(int64_t imm)
        {
          MMPUAsmOperand *Op = new MMPUAsmOperand(AsmImm);
          Op->Imm = imm;
          return Op;
        }

        static MMPUAsmOperand *
        createExpr(const MCExpr * expr)
        {
          MMPUAsmOperand *Op = new MMPUAsmOperand(AsmExpr);
          Op->Expr = expr;
          return Op;
        }

        static MMPUAsmOperand *
        createMacro(const MCSymbol &S)
        {
          MMPUAsmOperand *Op = new MMPUAsmOperand(AsmMacro);
          Op->Macro = &S;
          return Op;
        }
    };

  }
}

#endif // MMPU_ASM_OPERAND_H
