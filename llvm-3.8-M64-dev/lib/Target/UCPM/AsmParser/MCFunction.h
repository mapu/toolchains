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
// MCParsedAsmOperands.
//
//===----------------------------------------------------------------------===//

#ifndef UCPM_MCFUNCTION_H
#define UCPM_MCFUNCTION_H

#include "UCPMAsmParser.h"
#include "llvm/ADT/StringMap.h"

namespace llvm {
namespace UCPM {

class MCLoopBlock;
class MCFunction;
typedef std::shared_ptr<MCParsedAsmOperand> SharedOperand;
typedef std::shared_ptr<UCPMAsmOperand> SharedMMPUOprd;
typedef SmallVectorImpl<SharedOperand> SharedOperandVector;
#define CAST_TO_MMPU_OPRD(O) \
  std::static_pointer_cast<UCPM::UCPMAsmOperand>(O)

class MCParsedInst {
  SmallVector<SharedOperand, 256> Operands;
  MCParsedInst *Next;
  MCLoopBlock *Parent;

public:
  MCParsedInst(OperandVector &_Operands)
    : Operands(), Next(NULL), Parent(NULL) {
    for (unsigned i = 0; i < _Operands.size(); i++)
      Operands.push_back(SharedOperand(std::move(_Operands[i])));
  }

  MCParsedInst(SharedOperandVector &_Operands)
    : Operands(), Next(NULL), Parent(NULL) {
    for (unsigned i = 0; i < _Operands.size(); i++)
      Operands.push_back(_Operands[i]);
  }

  SharedOperandVector* getOperands() {
    return &Operands;
  }

  SharedOperand getOperand(unsigned OpNo) const {
    return Operands[OpNo];
  }

  void delOperand(unsigned OpNo) {
    SharedOperandVector::iterator iter =
      std::find(Operands.begin(), Operands.end(), Operands[OpNo]);
    if (iter != Operands.end())
      Operands.erase(iter);
  }
  
  void setParent(MCLoopBlock *P) {
    Parent = P;
    if (Next) Next->setParent(P);
  }
  
  MCLoopBlock *getParent() const { return Parent; }

  bool hasRepeat(unsigned &OpNo) const;
  bool hasLoop(unsigned &OpNo) const;
  bool hasJump(unsigned &OpNo) const;
  bool hasEmbeddedHMacro(unsigned &OpNo) const;

  unsigned size() const { return Operands.size(); }
  
  MCParsedInst *getNext() const { return Next; }
  
  void setNext(MCParsedInst *N) { Next = N; }
  
  void append(MCParsedInst *Tail) {
    if (Next) Next->append(Tail);
    else {
      Tail->setParent(Parent);
      Next = Tail;
    }
  }
  
  bool delInst(MCParsedInst *Inst) {
    MCParsedInst *p = this;
    while (p->Next)
      if (p->Next == Inst) return p->Next = p->Next->Next, true;
      else p = p->Next;
    return false;
  }
};

enum MCBlockTy {
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
  std::vector<MCFunction *> EmbeddedHMs;
  std::vector<MCLoopBlock *> Blocks;
  std::vector<MCParsedInst *> Insts;
  bool Nested;
  bool KINested;
  MCLoopBlock *Parent;
  std::vector<MCLoopBlock *> Head;
  std::vector<MCLoopBlock *> Tail;

public:
  MCLoopBlock(MCBlockTy T, uint64_t A = 0, int64_t C = 0, unsigned _KI = 0)
    : Type(T), Start(A), Count(C), KI(_KI),
      Nested(false), KINested(false), Parent(NULL) {}

  MCLoopBlock(MCBlockTy T, uint64_t A, int64_t C, unsigned _KI, 
              std::vector<MCParsedInst *> I,
              MCLoopBlock *Parent = NULL)
    : Type(T), Start(A), Count(C), KI(_KI), Insts(I),
      Nested(false), KINested(false), Parent(Parent) {
        for (uint64_t i = 0; i < Insts.size(); i++)
          Insts[i]->setParent(this);
      }

  MCLoopBlock(MCBlockTy T, uint64_t A, int64_t C, unsigned _KI, 
              std::vector<MCLoopBlock *> &MB,
              bool isKI, MCLoopBlock *Parent = NULL)
    : Type(T), Start(A), Count(C), KI(_KI), Blocks(MB),
      Nested(true), KINested(isKI), Parent(Parent) {}

  MCLoopBlock *DupThisBlock(uint64_t A); 

  void setType(MCBlockTy T, int64_t C, unsigned _KI = 0) {
    Type = T;
    Count = C;
    KI = _KI;
    Head.clear();
    Tail.clear();
    Nested = false;
    KINested = false;
  }
  
  void setKINested() { KINested = true;}

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

  int64_t getCount() const {return Count;}
  uint64_t getStart() const {return Start;}
  unsigned getKI() const {return KI;}
  unsigned getLevel() const ;
  std::vector<MCLoopBlock *> &getHead() { return Head;}
  std::vector<MCLoopBlock *> &getTail() { return Tail;}

  void setParent(MCLoopBlock *P) { Parent = P; }
  
  void setStart(uint64_t S) { Start = S;}
  
  void addParsedInst(MCParsedInst *Inst) {
    if (Inst) Inst->setParent(this);
    Insts.push_back(Inst);
  }

  uint64_t size() const; 
  uint64_t Totalsize() const; 
  uint64_t Tailsize() const; 
  uint64_t Headsize() const;

  uint64_t abssize() const;
  uint64_t Tailabssize() const; 
  uint64_t Headabssize() const;
 
  void addParsedInst(uint64_t Address, MCParsedInst *Inst) {
    if (Inst) Inst->setParent(this);
    if (Address == (Start + Headsize() + Insts.size()))
      Insts.push_back(Inst);
    else if (Address < (Start + Headsize() + Insts.size()))
      Insts[Address - Start] = Inst;
    else return;
  }

  bool getParsedInst(uint64_t Address, MCParsedInst* &Inst) const {
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

  bool getLeafBlock(uint64_t Address, MCLoopBlock* &MB) const {
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

  bool getLoopBlock(uint64_t Address, MCLoopBlock* &MB) const {
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
  
  MCLoopBlock *BlockReverse();

  MCLoopBlock *addSubBlock(MCLoopBlock* MB) {
    if (!Nested) {
      Nested = true;
      if (Insts.size())
        Blocks.push_back(new MCLoopBlock(Sequential, Start, 0, 0, Insts, this));
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
  
  void addEmbeddedHM(MCFunction* MF) { EmbeddedHMs.push_back(MF); }
  
  MCFunction *getEmbeddedHM(unsigned Num) const {
    if (Num >= EmbeddedHMs.size()) return NULL;
    else return EmbeddedHMs[Num];
  }

  void passEmbeddedHM(MCLoopBlock *Child) {
    for (unsigned i = 0; i < EmbeddedHMs.size(); i++)
      Child->addEmbeddedHM(EmbeddedHMs[i]);
    EmbeddedHMs.clear();
  }
  
  void ExpandEmbeddedHM(StringMap<HMacro*> &Map);
  
  void MergeBlock(std::vector<MCLoopBlock *> &LHS,
                  std::vector<MCLoopBlock *> &RHS);
  
  bool isNested(void) const { return Nested; }

  bool isKINested(void) const { return KINested; }

  MCLoopBlock *getParent(void) { return Parent; }

  bool isLoopEnd(uint64_t Addr) {
    return Addr == Start + Headsize() + size() - 1;
  }

  uint64_t RecalStart(uint64_t S);

  void Flatten(std::vector<MCLoopBlock *> &VMB);

  bool operator==(MCLoopBlock &RHS);

  bool operator!=(MCLoopBlock &RHS) { return !(*this == RHS); }

  void EliminateEmptyBlock(void);

  void Dump(raw_ostream &OS, std::string indent);
  
  bool delInst(MCParsedInst *Inst) {
    std::vector<MCParsedInst *>::iterator iter;
    for (iter = Insts.begin(); iter != Insts.end(); iter++) {
      if (*iter == NULL) continue;
      if ((*iter)->getNext() && (*iter)->delInst(Inst))
        return true;
      else if ((*iter)->getNext() && *iter == Inst)
        return *iter = (*iter)->getNext(), true;
      else if (*iter == Inst)
        return *iter = NULL, true;
    }
    return false;
  }

  bool findInstPos(MCParsedInst *Inst, uint64_t &Pos) const {
    for (Pos = 0; Pos < Insts.size(); Pos++) {
      MCParsedInst *p = Insts[Pos];
      while (p != NULL)
        if (p == Inst) return true;
        else p = p->getNext();
    }
    return false;
  }
  
  MCLoopBlock *getNextBlock() const {
    if (!Parent) return NULL;
    for (unsigned i = 0; i < Parent->getBlocks().size() - 1; i++)
      if (Parent->getBlocks()[i] == this)
        return Parent->getBlocks()[i+1];
    return Parent->getNextBlock();
  }
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
  SharedMMPUOprd cachedLabel;
  MCParsedInst *PreviousInst;
  bool Embedded;

public:
  SmallVector<std::pair<SharedMMPUOprd, bool>, 8> PendingLabels;
  SmallVector<std::pair<SharedMMPUOprd, bool>, 8> DisorderedLabels;
  MCFunction(StringRef Name, uint64_t Start)
    : Name(Name), Start(Start), Index(0), CurBlock(NULL),
    cachedLabel(nullptr), PreviousInst(NULL), Embedded(false) {}
  
  unsigned blocksize() const { return Blocks.size(); }

  StringRef getName() const { return Name; }
  
  uint64_t getStart() const { return Start; }
  
  void setStart(uint64_t S) { Start = S; }

  MCLoopBlock *addLoopBlock(MCLoopBlock *BB) {
    Blocks.push_back(BB);
    BB->setParent(NULL);
    return Blocks.back();
  }

  bool getLoopBlock(uint64_t Address, MCLoopBlock* &MB);

  void addParsedInst(MCParsedInst *Inst); 

  uint64_t size(void) const { return Index; }
  
  void Finalize(void);

  void EliminateEmptyBlock(void);
  
  bool isEmbedded(void) const { return Embedded; }
  
  void ExpandEmbeddedHM(StringMap<HMacro*> &Map);

  bool isPendingLabelsEmpty(void) const { return PendingLabels.empty(); }

  bool isDisorderedLabelsEmpty(void) const { return DisorderedLabels.empty(); }
};
}
}

#endif
