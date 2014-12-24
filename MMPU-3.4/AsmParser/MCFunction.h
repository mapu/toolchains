//===-- MCFunction.h ------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the data structures to hold a CFG reconstructed from
// machine code.
//
//===----------------------------------------------------------------------===//

#ifndef MMPU_MCFUNCTION_H
#define MMPU_MCFUNCTION_H

#include "MMPUAsmOperand.h"

#include "llvm/MC/MCParser/MCAsmLexer.h"
#include "llvm/MC/MCParser/MCParsedAsmOperand.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/MC/MCContext.h"

#include "llvm/ADT/StringRef.h"

using namespace llvm;

namespace llvm {
  namespace MMPU {
class MCParsedInst {
  SmallVector<MCParsedAsmOperand*, 256> Operands;

public:
  MCParsedInst(SmallVectorImpl<MCParsedAsmOperand*> &_Operands) {
    for (unsigned i = 0; i < _Operands.size(); i++)
      Operands.push_back(_Operands[i]);
  }

  SmallVectorImpl<MCParsedAsmOperand*> * getOperands() {
    return &Operands;
  }

  MCParsedAsmOperand* getOperand(unsigned OpNo) {
    return Operands[OpNo];
  }

  void delOperand(unsigned OpNo) {
    SmallVector<MCParsedAsmOperand*, 256>::iterator iter =
      std::find(Operands.begin(), Operands.end(), Operands[OpNo]);
    if (iter != Operands.end())
      Operands.erase(iter);
  }

  bool hasRepeat(unsigned &OpNo);
  bool hasLoop(unsigned &OpNo);
  bool hasJump(unsigned &OpNo);
  bool isPsudo();
};

enum MCBlockTy{
  None,
  Sequential,
  Repeat,
  Loop
};

class MCLoopBlock {
  MCBlockTy Type;
  uint64_t Start;
  int64_t Count;
  unsigned KI;
  std::vector<MCLoopBlock *> Blocks;
  std::vector<MCParsedInst *> Insts;
  bool Unrolled;
  bool Nested;
  bool KINested;
  MCLoopBlock *Parent;
  std::vector<MCLoopBlock *> Head;
  std::vector<MCLoopBlock *> Tail;

public:
  MCLoopBlock(MCBlockTy T, uint64_t A = 0, int64_t C = 0, unsigned _KI = 0)
    : Type(T), Start(A), Count(C), KI(_KI),
      Unrolled(true), Nested(false), KINested(false), Parent(NULL) {}

  MCLoopBlock(MCBlockTy T, uint64_t A, int64_t C, unsigned _KI,
              std::vector<MCParsedInst *> &I,
              bool Roll = false, MCLoopBlock *Parent = NULL)
    : Type(T), Start(A), Count(C), KI(_KI), Insts(I),
      Unrolled(Roll), Nested(false), KINested(false), Parent(Parent) {}

  MCLoopBlock(MCBlockTy T, uint64_t A, int64_t C, unsigned _KI,
              std::vector<MCLoopBlock *> &MB,
              bool isKI, MCLoopBlock *Parent = NULL)
    : Type(T), Start(A), Count(C), KI(_KI), Blocks(MB),
      Unrolled(true), Nested(true), KINested(isKI), Parent(Parent) {}

  MCLoopBlock *DupThisBlock(uint64_t A);

  void setType(MCBlockTy T, int64_t C, unsigned _KI = 0) {
    Type = T;
    Count = C;
    KI = _KI;
    Head.clear();
    Tail.clear();
    Unrolled = false;
    Nested = false;
    KINested = false;
  }

  void setKINested() {KINested = true;}

  std::vector<MCParsedInst *> &getInsts() { return Insts; }
  std::vector<MCLoopBlock *> &getBlocks() { return Blocks; }

  MCBlockTy getType() { return Type; }
  MCBlockTy getType(uint64_t Address) {
      for (unsigned i = 0; i < Head.size(); i++)
        if (Address >= Head[i]->getStart() &&
            Address < Head[i]->getStart() + Head[i]->Totalsize())
          return Head[i]->getType(Address);
      for (unsigned i = 0; i < Tail.size(); i++)
        if (Address >= Tail[i]->getStart() &&
            Address < Tail[i]->getStart() + Tail[i]->Totalsize())
          return Tail[i]->getType(Address);
      return Type;
  }

  int64_t getCount() {return Count;}
  uint64_t getStart() {return Start;}
  unsigned getKI() {return KI;}
  unsigned getLevel();
  std::vector<MCLoopBlock *> &getHead() {return Head;}
  std::vector<MCLoopBlock *> &getTail() {return Tail;}

  void setParent(MCLoopBlock *P) { Parent = P; }

  void addParsedInst(MCParsedInst *Inst) { Insts.push_back(Inst); }

  uint64_t size();
  uint64_t Totalsize();
  uint64_t Tailsize();
  uint64_t Headsize();

  uint64_t abssize();
  uint64_t Tailabssize();
  uint64_t Headabssize();

  void addParsedInst(uint64_t Address, MCParsedInst *Inst) {
    if (Address == (Start + Headsize() + Insts.size())) Insts.push_back(Inst);
    else if (Address < (Start + Headsize() + Insts.size())) Insts[Address - Start] = Inst;
    else return;
  }

  bool getParsedInst(uint64_t Address, MCParsedInst* &Inst) {
    MCLoopBlock *MB;
    if (getLeafBlock(Address, MB))
      return MB->getParsedInst(Address, Inst);
    else if (Nested) return false;
    else if (Insts.size() > (Address - Start - Headsize())) {
      Inst = Insts[Address - Start - Headsize()];
      return true;
    }
    return false;
  }

  bool getLeafBlock(uint64_t Address, MCLoopBlock* &MB) {
    if (!Head.empty())
      for (unsigned i = 0; i < Head.size(); i++)
        if (Address >= Head[i]->getStart() &&
            Address < Head[i]->getStart() + Head[i]->Totalsize()) {
          MB = Head[i];
          if (MB->getLeafBlock(Address, MB))
            return MB->size() != 0;
          else return MB->size() != 0;
        }
    for (unsigned i = 0; i < Blocks.size(); i++)
      if (Address >= Blocks[i]->getStart() &&
          Address < Blocks[i]->getStart() + Blocks[i]->Totalsize()) {
        MB = Blocks[i];
        if (MB->getLeafBlock(Address, MB))
          return MB->size() != 0;
        else return MB->size() != 0;
      }
    if (!Tail.empty())
      for (unsigned i = 0; i < Tail.size(); i++)
        if (Address >= Tail[i]->getStart() &&
            Address < Tail[i]->getStart() + Tail[i]->Totalsize()) {
          MB = Tail[i];
          if (MB->getLeafBlock(Address, MB))
            return MB->size() != 0;
          else return MB->size() != 0;
      }
    return false;
  }

  bool getLoopBlock(uint64_t Address, MCLoopBlock* &MB) {
    if (!Head.empty())
      for (unsigned i = 0; i < Head.size(); i++)
        if (Address >= Head[i]->getStart() &&
            Address < Head[i]->getStart() + Head[i]->Totalsize()) {
          MB = Head[i];
          if (MB->size()) return true;
        }
    for (unsigned i = 0; i < Blocks.size(); i++)
      if (Address >= Blocks[i]->getStart() &&
          Address < Blocks[i]->getStart() + Blocks[i]->Totalsize()) {
        MB = Blocks[i];
        if (MB->size()) return true;
      }
    if (!Tail.empty())
      for (unsigned i = 0; i < Tail.size(); i++)
        if (Address >= Tail[i]->getStart() &&
            Address < Tail[i]->getStart() + Tail[i]->Totalsize()) {
          MB = Tail[i];
          if (MB->size()) return true;
        }
    return false;
  }

  MCParsedInst *RollOneInst();

  MCLoopBlock *UnrollLoop(int64_t loops);

  void UnrollOneBlock();

  bool isUnrolled() { return Unrolled; }

  MCLoopBlock *addSubBlock(MCLoopBlock* MB) {
    if (!Nested) {
      Nested = true;
      if (Insts.size())
        Blocks.push_back(new MCLoopBlock(Sequential, Start, 0, 0, Insts, false, this));
    }
    if (MB->getType() == Loop ||
        (MB->getType() == Repeat &&
         MB->getCount() <= 0))
      KINested = true;
    Blocks.push_back(MB);
    MB->setParent(this);
    return MB;
  }

  MCLoopBlock *addSiblingBlock(MCLoopBlock* MB) {
    unsigned side = 1;
    if (MB->getStart() < Start) side = 0;
    if (Parent) {
      for (unsigned i = 0; i < Parent->getHead().size(); i++)
        if (Parent->getHead()[i] == this) {
          Parent->getHead().insert(Parent->getHead().begin() + i + side, MB);
          MB->setParent(this->Parent);
          return MB;
        }
      for (unsigned i = 0; i < Parent->getBlocks().size(); i++)
        if (Parent->getBlocks()[i] == this) {
          Parent->getBlocks().insert(Parent->getBlocks().begin() + i + side, MB);
          MB->setParent(this->Parent);
          if (MB->getType() == Loop ||
              (MB->getType() == Repeat &&
               MB->getCount() <= 0))
            Parent->setKINested();
          return MB;
        }
      for (unsigned i = 0; i < Parent->getTail().size(); i++)
        if (Parent->getTail()[i] == this) {
          Parent->getTail().insert(Parent->getTail().begin() + i + side, MB);
          MB->setParent(this->Parent);
          return MB;
        }
    }
    return NULL;
  }

  void addHeadBlock(MCLoopBlock* MB) {
    Head.push_back(MB);
    MB->setParent(this);
  }

  void addTailBlock(MCLoopBlock* MB) {
    Tail.push_back(MB);
    MB->setParent(this);
  }

  bool isNested() const { return Nested; }

  bool isKINested() const { return KINested; }

  MCLoopBlock *getParent() { return Parent; }

  bool isLoopEnd(uint64_t Addr) {
    return Addr == Start + Headsize() + size() - 1;
  }

  uint64_t RecalStart(uint64_t S);

  void Flatten(std::vector<MCLoopBlock *> &VMB);

  bool operator==(MCLoopBlock &RHS);

  bool operator!=(MCLoopBlock &RHS) { return !(*this == RHS); }
  void EliminateEmptyBlock();

};

/// MCFunction - Represents a named function in machine code, containing
/// multiple MCLoopBlocks.
class MCFunction {
  const StringRef Name;
  uint64_t Start;
  uint64_t Index;
  // Keep BBs sorted by address.
  std::vector<MCLoopBlock *> Blocks;
  MCLoopBlock *CurBlock;
  MCContext *Ctx;
  std::vector<const MCSymbol*> PendingLabels;
  const MCSymbol *cachedLabel;
  MCParsedInst *PreviousInst;

private:
  bool tryMergeBlock();
public:
  MCFunction(StringRef Name, uint64_t Start, MCContext *Ctx)
    : Name(Name), Start(Start), Index(0), CurBlock(NULL), Ctx(Ctx),
    cachedLabel(NULL), PreviousInst(NULL) {}

  typedef std::vector<MCLoopBlock *>::const_iterator iterator;
  iterator begin() const { return Blocks.begin(); }
  iterator end() const { return Blocks.end(); }

  StringRef getName() const { return Name; }

  MCLoopBlock *addLoopBlock(MCLoopBlock *BB) {
    Blocks.push_back(BB);
    BB->setParent(NULL);
    return Blocks.back();
  }

  bool getLoopBlock(uint64_t Address, MCLoopBlock* &MB);
  void addParsedInst(MCParsedInst *Inst);

  uint64_t size() const { return Index; }
  void EliminateEmptyBlock();
};

  } // MMPU

} // llvm

#endif
