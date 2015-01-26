//===-- MCFunction.cpp ----------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the algorithm to break down a region of
// MCParsedAsmOperands into LoopBlocks and try to reconstruct a CFG from it
// and unroll loops automatically
//
//===----------------------------------------------------------------------===//

#include "MCFunction.h"
#include "MMPULiteAsmParser.h"

using namespace llvm::MMPULite;

namespace llvm {
namespace Lite {
bool MCParsedInst::hasRepeat(unsigned &OpNo) const {
  for(OpNo = 0; OpNo < Operands.size(); OpNo++) {
    std::shared_ptr<MMPULite::MMPULiteAsmOperand> op =
      std::static_pointer_cast<MMPULite::MMPULiteAsmOperand>(Operands[OpNo]);
    if (op->isToken() && 
        (op->getOpc() == MMPULite::REPEATIMM ||
         op->getOpc() == MMPULite::REPEATK))
      return true;
  }
  return false;
}

bool MCParsedInst::hasLoop(unsigned &OpNo) const {
  for(OpNo = 0; OpNo < Operands.size(); OpNo++) {
    std::shared_ptr<MMPULite::MMPULiteAsmOperand> op =
      std::static_pointer_cast<MMPULite::MMPULiteAsmOperand>(Operands[OpNo]);
    if (op->isToken() && op->getOpc() == MMPULite::LPTO) return true;
  }
  return false;
}

bool MCParsedInst::hasJump(unsigned &OpNo) const {
  for(OpNo = 0; OpNo < Operands.size(); OpNo++) {
    std::shared_ptr<MMPULite::MMPULiteAsmOperand> op =
      std::static_pointer_cast<MMPULite::MMPULiteAsmOperand>(Operands[OpNo]);
    if (op->isToken() && 
        (op->getOpc() == MMPULite::JUMP ||
         op->getOpc() == MMPULite::JUMPK))
      return true;
  }
  return false;
}
  
bool MCParsedInst::hasEmbeddedHMacro(unsigned &OpNo) const {
  for(OpNo = 0; OpNo < Operands.size(); OpNo++) {
    std::shared_ptr<MMPULite::MMPULiteAsmOperand> op =
      std::static_pointer_cast<MMPULite::MMPULiteAsmOperand>(Operands[OpNo]);
    if (op->isHMacro() && (OpNo + 1 < Operands.size())) {
      op = std::static_pointer_cast<MMPULite::MMPULiteAsmOperand>(Operands[OpNo + 1]);
      if (op->isImm() && op->getImm() == 1)
        return true;
    }
  }
  return false;
}

/* ******************************************** *
 * MCLoopBlock methods for unrolling loops      *
 * ******************************************** */

unsigned MCLoopBlock::getLevel() const {
  if (Type == Repeat && Count <= 0) return 1;
  if (Type != Loop) return 0;
  if (!KINested) return 1;
  for (unsigned i = 0; i < Blocks.size(); i++)
    if (Blocks[i]->getLevel() != 0)
      return (Blocks[i]->getLevel())+1;
  return 1;
}

uint64_t MCLoopBlock::size() const {
  uint64_t sum = 0;
  if (Nested) {
    for (unsigned i = 0; i < Blocks.size(); i++)
      sum += Blocks[i]->Totalsize();
    return sum;
  }
  else return Insts.size();
}

uint64_t MCLoopBlock::Totalsize() const {
  return size() + Headsize() + Tailsize();
}

uint64_t MCLoopBlock::abssize() const {
  uint64_t sum = 0;
  if (Nested) {
    for (unsigned i = 0; i < Blocks.size(); i++) {
      if (Blocks[i]->getType() != Loop)
        sum += Blocks[i]->abssize();
      sum += Blocks[i]->Headabssize() +
             Blocks[i]->Tailabssize();
    }
    return sum;
  }
  else if (Type == Repeat && Count > 0) return Count;
  else if (Type == Sequential) return Insts.size();
  else if (Type == Loop) return Insts.size();
  else return 0;
}

uint64_t MCLoopBlock::Tailsize() const {
  uint64_t sum = 0;
  for (unsigned i = 0; i < Tail.size(); i++)
    sum += Tail[i]->Totalsize();
  return sum;
}

uint64_t MCLoopBlock::Headsize() const {
  uint64_t sum = 0;
  for (unsigned i = 0; i < Head.size(); i++)
    sum += Head[i]->Totalsize();
  return sum;
}

uint64_t MCLoopBlock::Tailabssize() const {
  uint64_t sum = 0;
  for (unsigned i = 0; i < Tail.size(); i++) {
    if (Tail[i]->getType() != Loop)
      sum += Tail[i]->abssize();
    sum += Tail[i]->Headabssize() +
           Tail[i]->Tailabssize();
  }
  return sum;
}

uint64_t MCLoopBlock::Headabssize() const {
  uint64_t sum = 0;
  for (unsigned i = 0; i < Head.size(); i++) {
    if (Head[i]->getType() != Loop)
      sum += Head[i]->abssize();
    sum += Head[i]->Headabssize() +
           Head[i]->Tailabssize();
  }
  return sum;
}

uint64_t MCLoopBlock::RecalStart(uint64_t S) {
  uint64_t next = S;
  Start = next;
  if (!Head.empty())
    for (unsigned i = 0; i < Head.size(); i++)
      next = Head[i]->RecalStart(next);
  if (Nested)
    for (unsigned i = 0; i < Blocks.size(); i++)
      next = Blocks[i]->RecalStart(next);
  else
    next += Insts.size();
  if (!Tail.empty())
    for (unsigned i = 0; i < Tail.size(); i++)
      next = Tail[i]->RecalStart(next);
  return next;
}

MCLoopBlock* MCLoopBlock::DupThisBlock(uint64_t A) {
    MCLoopBlock *Res = new MCLoopBlock(getType(),
                                       A,
                                       getCount(),
                                       getKI());
    for (unsigned i = 0; i < Head.size(); A += Head[i]->Totalsize(), i++)
      Res->addHeadBlock(Head[i]->DupThisBlock(A));
    if (Nested)
      for (unsigned i = 0; i < Blocks.size(); A += Blocks[i]->Totalsize(), i++)
        Res->addSubBlock(Blocks[i]->DupThisBlock(A));
    else  
      for (unsigned i = 0; i < Insts.size(); i++)
        Res->addParsedInst(A + i, Insts[i]);
    for (unsigned i = 0; i < Tail.size(); A += Tail[i]->Totalsize(), i++)
      Res->addTailBlock(Tail[i]->DupThisBlock(A));
    return Res;
  }

MCLoopBlock* MCLoopBlock::UnrollLoop(int64_t loops) {
  if (Type == Sequential) {
    MCLoopBlock *MB = new MCLoopBlock(Sequential, Start);
    while (loops) {
      if (loops > 0) {
        MB->addParsedInst(Insts.front());
        Insts.erase(Insts.begin());
        --loops;
      }
    }
    return MB;
  }
  else if (Type == Repeat) {
    if (loops == 0) return NULL;
    else if (loops < 0) { // back unrolling
      Count = Count + loops;
      if (-loops == 1)
        return new MCLoopBlock(Sequential, Start + Insts.size(), 0, 0, Insts);
      return new MCLoopBlock(Repeat, Start + Insts.size(), -loops, 0, Insts);
    }
    else if (Count <= loops && Count > 0) return this;
    else {
      Count = Count - loops;
      Start += Insts.size();
      if (Count == 1) Type = Sequential;
      if (loops == 1)
        return new MCLoopBlock(Sequential, Start - Insts.size(), 0, 0, Insts);
      return new MCLoopBlock(Repeat, Start - Insts.size(), loops, 0, Insts);
    }
  }
  else if (Type == Loop) {
    if (loops == 0) return this;
    else if (loops < 0) {
      uint64_t uloops = (uint64_t)(-loops);
      if (uloops == size()) {
        --Count;
        if (!Nested) {
          MCLoopBlock *MB = DupThisBlock(Start - size());
          MB->setType(Sequential, 0);
          Tail.insert(Tail.begin(), MB);
          MB->setParent(this);
        } else {
          for (unsigned i = 0; i < Blocks.size(); i++) {
            MCLoopBlock *MB = Blocks[i]->DupThisBlock(Start);
            Tail.insert(Tail.begin() + i, MB);
            MB->setParent(this);
          }
        }
        return this;
      } else {
        // back unrolling
        /* to simplify back unrolling loop, use the same algorithm for forward
           unrolling. To do this, reverse the source loop completely, do the 
           unrolling, and finally reverse back completely. */
        BlockReverse();
        MCLoopBlock *MB = UnrollLoop(-loops);
        BlockReverse();
        return MB;// there is a mem leakage
      }
      /*return NULL;*/ // back unrolling more or less than 1 loop is not allowed
    } else { // loops > 0
      if (!KINested) {
        while (((uint64_t) loops) >= abssize()) {
          if (!Nested)
            addHeadBlock(new MCLoopBlock(Sequential, Start, 0, 0, Insts));
          else
            for (unsigned i = 0; i < Blocks.size(); i++)
              addHeadBlock(Blocks[i]->DupThisBlock(Start));
          loops -= abssize();
          --Count;
        }
        if (loops > 0 && !Nested) {
          unsigned _loops = loops;
          if (Tail.empty() || Tailabssize() < ((uint64_t) loops)) {
            MCLoopBlock *MB = DupThisBlock(Start);
            MB->setType(Sequential, 0);
            addHeadBlock(MB->UnrollLoop(loops));
            Tail.insert(Tail.begin(), MB);
            MB->setParent(this);
            --Count;
            loops -= _loops;
          }
          while (_loops) {
            Insts.push_back(Insts.front());
            Insts.erase(Insts.begin());
            --_loops;
          }
        }
      }
      while (loops != 0) {
        if (Tail.empty()) {
          --Count;
          MCLoopBlock *MB = Blocks.front();
          if (MB->getType() == Sequential ||
              (MB->getType() == Repeat && MB->getCount() > 0)) {
            if (MB->abssize() <= ((uint64_t) loops)) {
              loops -= MB->abssize();
              Blocks.erase(Blocks.begin());
              for (unsigned i = 0; i < Blocks.size(); i++)
                addTailBlock(Blocks[i]->DupThisBlock(Start));
              addSubBlock(MB);
              addHeadBlock(MB->DupThisBlock(Start));
            } else {
              MB = MB->UnrollLoop(loops);
              addHeadBlock(MB->DupThisBlock(Start));
              for (unsigned i = 0; i < Blocks.size(); i++)
                addTailBlock(Blocks[i]->DupThisBlock(Start));
              addSubBlock(MB);
              loops = 0;
            }
          } else if (MB->getType() == Repeat) { // RepeatKI
            MB = MB->UnrollLoop(loops);
            addHeadBlock(MB->DupThisBlock(Start));
            for (unsigned i = 0; i < Blocks.size(); i++)
              addTailBlock(Blocks[i]->DupThisBlock(Start));
            addSubBlock(MB);
            loops = 0;
          } else { // Loop
            while (MB->getHead().empty()) MB->UnrollLoop(loops);
            {
              loops -= MB->Headabssize();
              for (unsigned i = 0; i < Blocks.size(); i++)
                addTailBlock(Blocks[i]->DupThisBlock(Start));
              Tail.front()->getHead().clear();
              for (unsigned i = 0; i < MB->getHead().size(); i++) {
                addHeadBlock(MB->getHead()[i]);
                addSubBlock(MB->getHead()[i]->DupThisBlock(Start));
              }
              MB->getHead().clear();
            }
          }
        } else { // !Tail.empty()
          MCLoopBlock *MB = Tail.front();
          if (MB->getType() == Sequential ||
              (MB->getType() == Repeat && MB->getCount() > 0)) {
            if (MB->abssize() <= ((uint64_t) loops)) {
              loops -= MB->abssize();
              addHeadBlock(MB);
              Tail.erase(Tail.begin());
              if (Nested) {
                addSubBlock(Blocks.front());
                Blocks.erase(Blocks.begin());
              }
            } else {
              MB = MB->UnrollLoop(loops);
              addHeadBlock(MB);
              if (Nested) {
                MB = Blocks.front()->UnrollLoop(loops);
                addSubBlock(MB);
              }
              loops = 0;
            }
          } else if (MB->getType() == Repeat) { // RepeatKI
            MB = MB->UnrollLoop(loops);
            addHeadBlock(MB);
            MB = Blocks.front()->UnrollLoop(loops);
            addSubBlock(MB);
            loops = 0;
          } else { // Loop
            while (MB->getHead().empty()) {
              MB->UnrollLoop(loops);
              Blocks.front()->UnrollLoop(loops);
            }
            loops -= MB->Headabssize();
            for (unsigned i = 0; i < MB->getHead().size(); i++) {
              addHeadBlock(MB->getHead()[i]);
              addSubBlock(Blocks.front()->getHead()[i]);
            }
            MB->getHead().clear();
            Blocks.front()->getHead().clear();
          }
        }
      }
    }
  }
  return this;
}

void MCLoopBlock::UnrollOneBlock() {
  MCLoopBlock *MB, *BMB;
  if (!Nested) return;
  if (!Tail.empty()) {
    MB = Tail.front();
    BMB = Blocks.front();
    if (!MB->getHead().empty()) {
      while (!MB->getHead().front()->getHead().empty()) {
        MB = MB->getHead().front();
        BMB = BMB->getHead().front();
      }
      addHeadBlock(MB->getHead().front());
      MB->getHead().erase(MB->getHead().begin());
      addSubBlock(BMB->getHead().front());
      BMB->getHead().erase(Blocks.begin());
    } else {
      addHeadBlock(MB);
      Tail.erase(Tail.begin());
      addSubBlock(BMB);
      Blocks.erase(Blocks.begin());
    }
    // The block which is unrolled out may have tail blocks and these tail blocks are over unrolled
    MB = Head.back();
    BMB = Blocks.back();
    while(!MB->getTail().empty()) {
      Tail.insert(Tail.begin(), MB->getTail().back());
      Tail.front()->setParent(this);
      MB->getTail().pop_back();
      Blocks.insert(Blocks.begin(), BMB->getTail().back());
      Blocks.front()->setParent(this);
      BMB->getTail().pop_back();
    }
  } else {
    --Count;
    for (unsigned i = 0; i < Blocks.size(); i++)
      addTailBlock(Blocks[i]->DupThisBlock(Start));
    UnrollOneBlock();
  }
}
  
MCLoopBlock *MCLoopBlock::BlockReverse() {
  std::vector<MCLoopBlock *> swapper = Tail;
  Tail.clear();
  for (unsigned i = Tail.size(); i > 0; i--)
    addTailBlock(Head[i - 1]->BlockReverse());
  for (unsigned i = swapper.size(); i > 0; i--)
    addHeadBlock(swapper[i - 1]->BlockReverse());
  swapper = Blocks;
  Blocks.clear();
  for (unsigned i = Blocks.size(); i > 0; i--)
    addSubBlock(Blocks[i - 1]->BlockReverse());
  for (unsigned i = Insts.size(); i > 0; i--)
    addParsedInst(Insts[i - 1]);
  RecalStart(Start);
  return this;
}

void MCLoopBlock::Flatten(std::vector<MCLoopBlock *> &VMB) {
  for (unsigned i = 0; i < Head.size(); i++)
    Head[i]->Flatten(VMB);
  VMB.push_back(this);
  for (unsigned i = 0; i < Tail.size(); i++)
    Tail[i]->Flatten(VMB);
}

bool MCLoopBlock::operator==(MCLoopBlock &RHS) {
  std::vector<MCLoopBlock *> MBL, MBR;
  uint64_t CompensL = 0, CompensR = 0;
  if (KINested != RHS.isKINested()) return false;
  else if (!KINested) {
    if ((Type == Loop && RHS.getType() != Loop) ||
        (Type != Loop && RHS.getType() == Loop))
      return false;
    if (KI != RHS.getKI()) return false;
    return abssize() == RHS.abssize();
  }
  else {
    if (KI != RHS.getKI()) return false;
    // Flatten the LHS and RHS, which will ease the way of comparison
    for (unsigned i = 0; i < Blocks.size(); i++) 
      Blocks[i]->Flatten(MBL);
    unsigned j = RHS.getStart() + RHS.Headsize();
    uint64_t RHSend = RHS.getStart() + RHS.Headsize() + RHS.size();
    MCLoopBlock *MB;
    for (; j < RHSend && RHS.getLoopBlock(j, MB); j += MB->Totalsize()) 
      MB->Flatten(MBR);
    j = 0;
    for (unsigned i = 0; i < MBL.size(); i++) {
      if (MBL[i]->getType() == Loop ||
          (MBL[i]->getType() == Repeat &&
           MBL[i]->getCount() <= 0)) {
        bool equal = false;
        for (; j < MBR.size(); j++) {
          if (*MBL[i] == *MBR[j]) {
            if (MBL[i]->getCount() != MBR[j]->getCount()) {
              if (MBL[i]->getType() == Repeat) {
                if (MBL[i]->getCount() < MBR[j]->getCount())
                  CompensR += MBR[j]->getCount() - MBL[i]->getCount();
                else
                  CompensL += MBL[i]->getCount() - MBR[j]->getCount();
              }
              else if (!MBL[i]->isKINested()){
                if (MBL[i]->getCount() < MBR[j]->getCount())
                  CompensR += (MBR[j]->getCount() - MBL[i]->getCount()) *
                              MBR[j]->abssize();
                else
                  CompensL += (MBL[i]->getCount() - MBR[j]->getCount()) *
                              MBR[j]->abssize();
              }
              else return true; // KINested Blocks are found inside of LHS and RHS.
                                // Either LHS or RHS is extracted manually.
                                // Simply return true here, and will be determined later.
            }
            j++;
            equal = true;
            break;
          }
          else if (MBR[j]->getType() == Loop ||
                   (MBR[j]->getType() == Repeat &&
                    MBR[j]->getCount() <= 0)) return true;
        }
        if(!equal) return false;
      }
    }
    for (; j < MBR.size(); j++)
      if (MBR[j]->getType() == Loop ||
          (MBR[j]->getType() == Repeat &&
           MBR[j]->getCount() <= 0)) return false;
    return abssize() + CompensL == RHS.abssize() + CompensR;
  }
}
  
void MCLoopBlock::ExpandEmbeddedHM(StringMap<HMacro*> &Map) {
  for (unsigned i = 0; i < Blocks.size(); i++)
    Blocks[i]->ExpandEmbeddedHM(Map);
  for (unsigned i = 0; i < EmbeddedHMs.size(); i++) {
    MCFunction *p = EmbeddedHMs[i];
    if (p && p->size() == 0 &&
        Map.count(StringRef(EmbeddedHMs[i]->getName()))) {
      p = EmbeddedHMs[i];
      EmbeddedHMs[i] = Map[StringRef(EmbeddedHMs[i]->getName())]->Body;
      EmbeddedHMs[i]->setStart(p->getStart());
      delete p;
    }
    if (EmbeddedHMs[i]->isEmbedded())
      EmbeddedHMs[i]->ExpandEmbeddedHM(Map);
  }
  
  if (EmbeddedHMs.size() == 0) return;
  
  if (Blocks.size() == 0 && Insts.size() != 0) {
    addSubBlock(new MCLoopBlock(Sequential, Start, 0));
    delete Blocks.back();
    Blocks.pop_back();
  }
  
  // The Embedded HMacro are merged one by one.
  
  std::vector<MCLoopBlock *> LHS = Blocks;
  std::vector<MCLoopBlock *> RHS;
  while (!EmbeddedHMs.empty()) {
    Blocks.clear();
    uint64_t start = EmbeddedHMs.front()->getStart();
    errs() << "Expanding EHM:" << EmbeddedHMs.front()->getName() << "\n";
    errs() << "R:" << EmbeddedHMs.front()->blocksize() << "|" << start
           << "L:" << LHS.size() << "|" << Start << "\n";
    if (start > Start) {
      SmallVector<SharedOperand, 8> Nops;
      Nops.push_back(SharedOperand(MMPULiteAsmOperand::createOpc(NOP)));
      if (start - Start == 1) {
        RHS.push_back(new MCLoopBlock(Sequential, Start));
        RHS.back()->addParsedInst(new MCParsedInst(Nops));
      } else {
        RHS.push_back(new MCLoopBlock(Repeat, Start, start - Start, 0));
        RHS.back()->addParsedInst(new MCParsedInst(Nops));
      }
    }
    if (Map.count(StringRef(EmbeddedHMs.front()->getName())) &&
        Map[StringRef(EmbeddedHMs.front()->getName())]->Body) {
      MCLoopBlock *MB = NULL;
      for (unsigned i = 0;
           i < Map[StringRef(EmbeddedHMs.front()->getName())]->Body->size();
           i += MB->size()) {
        if (Map[StringRef(EmbeddedHMs.front()->getName())]->Body->getLoopBlock(i, MB)) {
          if (start + i < Start)
          /* this only occurs when following instline has been parsed:
             ...||LPTO (label) @(KI) || hm() ...;
             so the first line of hm() should be removed, and this line shall be
             a NOP left by parser due to removing isolated LPTO inst. */
            continue;
          MB = MB->DupThisBlock(start + i);
          RHS.push_back(MB);
        }
      }
    }
    EmbeddedHMs.erase(EmbeddedHMs.begin());
    MergeBlock(LHS, RHS);
    RecalStart(Start);
    LHS = Blocks;
    RHS.clear();
  }
}
  
void MCLoopBlock::MergeBlock(std::vector<MCLoopBlock *> &LHS,
                             std::vector<MCLoopBlock *> &RHS) {
  errs() << "MB:\n";
  for (unsigned i = 0; i < LHS.size(); i++)
    errs() << "L" << i << ": " << LHS[i]->getType() << ":" << LHS[i]->size() << "\n";
  for (unsigned i = 0; i < RHS.size(); i++)
    errs() << "R" << i << ": " << RHS[i]->getType() << ":" << RHS[i]->size() << "\n";
  while (!LHS.empty() && !RHS.empty()) {
    uint64_t lindex = LHS.front()->getStart();
    const MCBlockTy ltype = LHS.front()->getType();
    const MCBlockTy rtype = RHS.front()->getType();
    MCLoopBlock *temp0, *temp1;
    if (ltype == Sequential) {
      if (LHS.front()->size() == 0) {
        LHS.erase(LHS.begin());
        continue;
      }
      if (rtype == Sequential) {
        if (LHS.front()->size() <= RHS.front()->size()) {
          temp0 = RHS.front()->UnrollLoop(LHS.front()->size());
          temp1 = LHS.front();
          LHS.erase(LHS.begin());
          if (RHS.front() == temp0)
            RHS.erase(RHS.begin());
        } else {
          MergeBlock(RHS, LHS);
          return;
        }
      } else if (rtype == Repeat) {
        if (RHS.front()->getCount() <= 0 ||
            uint64_t(RHS.front()->getCount()) > LHS.front()->size()) {
          temp0 = RHS.front()->UnrollLoop(LHS.front()->size());
          temp1 = LHS.front();
          LHS.erase(LHS.begin());
        } else if (LHS.front()->size() >= uint64_t(RHS.front()->getCount())) {
          temp0 = RHS.front();
          temp1 = LHS.front()->UnrollLoop(RHS.front()->getCount());
          RHS.erase(RHS.begin());
          if (LHS.front() == temp1)
            LHS.erase(LHS.begin());
        } else {
          temp0 = RHS.front()->UnrollLoop(LHS.front()->size());
          temp1 = LHS.front();
          LHS.erase(LHS.begin());
        }
      } else {
        RHS.front()->UnrollLoop(LHS.front()->size());
        std::vector<MCLoopBlock *> flat;
        RHS.front()->Flatten(flat);
        RHS.erase(RHS.begin());
        for (unsigned i = 0; i < flat.size(); i++) {
          flat[i]->getHead().clear();
          flat[i]->getTail().clear();
        }
        while (!flat.empty()) {
          RHS.insert(RHS.begin(), flat.back());
          flat.pop_back();
        }
        continue;
      }
      Blocks.push_back(new MCLoopBlock(Sequential, lindex));
      for (unsigned i = 0; i < temp1->size(); i++) {
        MCParsedInst *linst, *rinst, *cominst;
        if (temp0->getType() == Repeat)
          temp0->getParsedInst(temp0->getStart(), rinst);
        else
          temp0->getParsedInst(temp0->getStart() + i, rinst);
        temp1->getParsedInst(temp1->getStart() + i, linst);
        cominst = new MCParsedInst(*linst->getOperands());
        for (unsigned j = 0; j < rinst->size(); j++)
          cominst->getOperands()->push_back(rinst->getOperand(j));
        Blocks.back()->addParsedInst(cominst);
      }
      //delete temp1;
      //delete temp0;
    } else if (ltype == Repeat) {
      if (LHS.front()->getCount() > 0 &&
          (rtype == Loop || (rtype == Repeat &&
           RHS.front()->getCount() >= LHS.front()->getCount()))) {
        temp0 = RHS.front()->UnrollLoop(LHS.front()->getCount());
        temp1 = LHS.front();
        LHS.erase(LHS.begin());
        if (rtype == Repeat && RHS.front() == temp0)
          RHS.erase(RHS.begin());
        Blocks.push_back(new MCLoopBlock(Sequential, lindex));
        for (unsigned i = 0; i < temp1->getCount(); i++) {
          MCParsedInst *linst, *rinst, *cominst;
          if (temp0->getType() == Repeat)
            temp0->getParsedInst(temp0->getStart(), rinst);
          else
            temp0->getParsedInst(temp0->getStart() + i, rinst);
          temp1->getParsedInst(temp1->getStart(), linst);
          cominst = new MCParsedInst(*linst->getOperands());
          for (unsigned j = 0; j < rinst->size(); j++)
            cominst->getOperands()->push_back(rinst->getOperand(j));
          Blocks.back()->addParsedInst(cominst);
        }
        if (RHS.front()->getType() == Loop) {
          std::vector<MCLoopBlock *> flat;
          RHS.front()->Flatten(flat);
          for (unsigned i = 0; i < flat.size(); i++) {
            flat[i]->getHead().clear();
            flat[i]->getTail().clear();
          }
          RHS.erase(RHS.begin());
          while (!flat.empty()) {
            RHS.insert(RHS.begin(), flat.back());
            flat.pop_back();
          }
        }
        //delete temp1;
        //delete temp0;
      } else if (LHS.front()->getCount() <= 0 && rtype == Repeat &&
                 RHS.front()->getCount() <= 0 &&
                 LHS.front()->getKI() == RHS.front()->getKI() &&
                 LHS.front()->getCount() <= RHS.front()->getCount()) {
        Blocks.push_back(new MCLoopBlock(Repeat, lindex,
                                         LHS.front()->getCount(),
                                         LHS.front()->getKI()));
        temp0 = RHS.front()->UnrollLoop(LHS.front()->getCount());
        temp1 = LHS.front();
        LHS.erase(LHS.begin());
        if (RHS.front() == temp0)
          RHS.erase(RHS.begin());
        MCParsedInst *linst, *rinst, *cominst;
        temp0->getParsedInst(temp0->getStart(), rinst);
        temp1->getParsedInst(temp1->getStart(), linst);
        cominst = new MCParsedInst(*linst->getOperands());
        for (unsigned j = 0; j < rinst->size(); j++)
          cominst->getOperands()->push_back(rinst->getOperand(j));
        Blocks.back()->addParsedInst(linst);
      } else {
        MergeBlock(RHS, LHS);
        return;
      }
    } else { //Loop vs Loop
      if (rtype == Loop &&
          *LHS.front() == *RHS.front() &&
          LHS.front()->getCount() <= RHS.front()->getCount()) {
        if (LHS.front()->getCount() == RHS.front()->getCount()) {
          std::vector<MCLoopBlock *> BlocksBack = Blocks;
          Blocks.clear();
          if (!LHS.front()->isNested()) {
            LHS.front()->addSubBlock(new MCLoopBlock(Sequential, 0));
            delete LHS.front()->getBlocks().back();
            LHS.front()->getBlocks().pop_back();
          }
          if (!RHS.front()->isNested()) {
            RHS.front()->addSubBlock(new MCLoopBlock(Sequential, 0));
            delete RHS.front()->getBlocks().back();
            RHS.front()->getBlocks().pop_back();
          }
          MergeBlock(RHS.front()->getBlocks(), LHS.front()->getBlocks());
          BlocksBack.push_back(new MCLoopBlock(Loop, lindex,
                                               LHS.front()->getCount(),
                                               LHS.front()->getKI()));
          for (unsigned i = 0; i < Blocks.size(); i++)
            BlocksBack.back()->addSubBlock(Blocks[i]);
          Blocks = BlocksBack;
          LHS.erase(LHS.begin());
          RHS.erase(RHS.begin());
        } else {
          unsigned count = RHS.front()->getCount() - LHS.front()->getCount();
          while (count-- != 0)
            RHS.front()->UnrollLoop(-RHS.front()->size());
          std::vector<MCLoopBlock *> flat;
          RHS.front()->Flatten(flat);
          for (unsigned i = 0; i < flat.size(); i++) {
            flat[i]->getHead().clear();
            flat[i]->getTail().clear();
          }
          RHS.erase(RHS.begin());
          while (!flat.empty()) {
            RHS.insert(RHS.begin(), flat.back());
            flat.pop_back();
          }
          continue;
        }
      } else {
        MergeBlock(RHS, LHS);
        return;
      }
    }
  }
  while (!LHS.empty()) {
    Blocks.push_back(LHS.front());
    LHS.erase(LHS.begin());
  }
  while (!RHS.empty()) {
    Blocks.push_back(RHS.front());
    RHS.erase(RHS.begin());
  }
}
  
void MCLoopBlock::EliminateEmptyBlock(void) {
  if (!Blocks.empty()) {
    if (Blocks.back()->isNested()) 
      Blocks.back()->EliminateEmptyBlock();
    if (Blocks.back()->size() == 0) {
      delete Blocks.back();
      Blocks.pop_back();
    }
  }
}

void MCLoopBlock::Dump(raw_ostream &OS, std::string indent) {
  std::string step0 = "|    ";
  std::string step1 = "     ";
  std::string arrow = "|-->";
  std::string prefix = indent;
  OS << prefix + arrow << "Head Zone(" << Head.size() << " Blocks):\n";
  prefix = indent + step0;
  for (unsigned i = 0; i < Head.size(); i++) {
    OS << prefix + arrow << "Head " << i << " Info:\n";
    Head[i]->Dump(OS, prefix + ((i == Head.size() - 1) ? step1 : step0));
  }
  //OS << prefix << "\n";
  prefix = indent;
  if (Nested) {
    OS << prefix + arrow << "Body Zone(" << Blocks.size()
       << " Blocks): ";
    OS << "T=";
    switch (Type) {
      case None: OS << "None"; break;
      case Sequential: OS << "Sequential"; break;
      case Repeat: OS << "Repeat"; break;
      case Loop: OS << "Loop"; break;
    }
    OS << ", A=" << Start;
    OS << ", K=" << KI << ", C=" << Count << ", S=" << size();
    OS << ", AS=" << abssize() << "\n";
    prefix = indent + step0;
    for (unsigned i = 0; i < Blocks.size(); i++) {
      OS << prefix + arrow << "Block " << i << " Info:\n";
      Blocks[i]->Dump(OS, prefix + ((i == Blocks.size() - 1) ? step1 : step0));
    }
  }
  else {
    OS << prefix + arrow << "Body Zone(Unnested): ";
    OS << "T=";
    switch (Type) {
      case None: OS << "None"; break;
      case Sequential: OS << "Sequential"; break;
      case Repeat: OS << "Repeat"; break;
      case Loop: OS << "Loop"; break;
    }
    OS << ", A=" << Start;
    OS << ", K=" << KI << ", C=" << Count << ", S=" << size();
    OS << ", AS=" << abssize() << "\n";
    prefix = indent + step0;
  }
  //OS << prefix << "\n";
  prefix = indent;
  OS << prefix + arrow << "Tail Zone(" << Tail.size() << " Blocks):\n";
  prefix = indent + step1;
  for (unsigned i = 0; i < Tail.size(); i++) {
    OS << prefix + arrow << "Tail " << i << " Info:\n";
    Tail[i]->Dump(OS, prefix + ((i == Tail.size() - 1) ? step1 : step0));
  }
  prefix = indent;
  OS << prefix << "\n";
}

/* ****************************************
 * MCFunction methods
 * ***************************************/
bool MCFunction::getLoopBlock(uint64_t Address, MCLoopBlock* &MB) {
  for (unsigned i = 0; i < Blocks.size(); i++)
    if (Address >= Blocks[i]->getStart() &&
        Address < Blocks[i]->getStart() + Blocks[i]->size()) {
      MB = Blocks[i];
      return true;
    }
  return false;
}

void MCFunction::addParsedInst(MCParsedInst *Inst) {
  unsigned OpNo;
  std::shared_ptr<MMPULite::MMPULiteAsmOperand> op;
  int64_t count = 1;
  unsigned ki = 0;
  bool Sym;

  if (!CurBlock) CurBlock = addLoopBlock(new MCLoopBlock(Sequential, Index)); 
  if (!PendingLabels.empty()) {
    for (Sym = PendingLabels.back().second;
         !PendingLabels.empty() &&
         (Sym = PendingLabels.back().second);
         PendingLabels.pop_back()) {
      //now the end of loop block is arrived
      if (CurBlock->size() == 0) {
        //if (CurBlock->getType() != Loop)
          //CurBlock->setType(Sequential, 0, 0);
        CurBlock->getParent()->getBlocks().pop_back();
        if (CurBlock->getParent()->getParent())
          CurBlock = CurBlock->getParent()->addSiblingBlock(CurBlock);
        else
          CurBlock = addLoopBlock(CurBlock);
      }
      else {
        if (CurBlock->size() == 1 && CurBlock->getType() == Loop &&
            !CurBlock->isNested() && !CurBlock->getEmbeddedHM(0))
          CurBlock->setType(Repeat, CurBlock->getCount(), CurBlock->getKI()); // Simplify the single inst loop to a RepeatK
         // if current block is not a loop, then it is a block nested in the loop that is going to end
        else if (CurBlock->getType() != Loop)
          CurBlock = CurBlock->getParent(); // CurBlock must have a parent. Top level Block won't nest in a loop
        if (CurBlock->getParent())
          CurBlock = CurBlock->addSiblingBlock(new MCLoopBlock(Sequential, Index)); // this loop is also a nested one
        else
          CurBlock = addLoopBlock(new MCLoopBlock(Sequential, Index)); // this loop is at the top level
      }
    }
  }
  if (cachedLabel) {
    PendingLabels.push_back(
      std::pair<std::shared_ptr<MMPULite::MMPULiteAsmOperand>, bool>(cachedLabel, false));
    cachedLabel = nullptr;
    while (PreviousInst && PreviousInst->hasLoop(OpNo)) {
      op = CAST_TO_MMPU_OPRD(PreviousInst->getOperand(OpNo + 1));
      if (op->isImm()) return;  //absolute address is ignored in HMacro
      if (op->isExpr()) {
        const MCConstantExpr *CE = dyn_cast<MCConstantExpr>(op->getExpr());
        if (CE) return; // absolute address is ignored
        else {
          const MCSymbolRefExpr *SE = dyn_cast<MCSymbolRefExpr>(op->getExpr());
          if (SE) {
            PendingLabels.push_back(
              std::pair<std::shared_ptr<MMPULite::MMPULiteAsmOperand>, bool>(op, false));
            op = CAST_TO_MMPU_OPRD(PreviousInst->getOperand(OpNo + 2));
            ki = op->getImm();
            op = CAST_TO_MMPU_OPRD(PreviousInst->getOperand(OpNo + 3));
            count = -op->getImm();
          } else return; // unknown loop label
        }
      }
      MCLoopBlock *SubLoop = new MCLoopBlock(Loop, Index, count, ki);
      //CurBlock->passEmbeddedHM(SubLoop);
      CurBlock = CurBlock->addSubBlock(SubLoop);
      PreviousInst->delOperand(OpNo); //delete LPTO
      PreviousInst->delOperand(OpNo); //delete Label
      PreviousInst->delOperand(OpNo); //delete KI
      PreviousInst->delOperand(OpNo); //delete KI difference
      if (!PreviousInst->hasLoop(OpNo)) OpNo = Inst->size();
      unsigned OpNoH;
      while (PreviousInst->hasEmbeddedHMacro(OpNoH)) {
        if (OpNoH > OpNo) break;
        Embedded = true;
        op = CAST_TO_MMPU_OPRD(PreviousInst->getOperand(OpNoH));
        CurBlock->addEmbeddedHM(new MCFunction(*op->getHMacro(), Index - 1));
        Inst->delOperand(OpNoH);
        Inst->delOperand(OpNoH);
      }
    }
  }
  if (Inst->hasRepeat(OpNo)) {
    op = CAST_TO_MMPU_OPRD(Inst->getOperand(OpNo + 1));
    count = op->getImm();
    op = CAST_TO_MMPU_OPRD(Inst->getOperand(OpNo));
    if (op->getOpc() == MMPULite::REPEATIMM && count == 0) count = 1024;
    else if (op->getOpc() == MMPULite::REPEATK) {
      ki = count;
      op = CAST_TO_MMPU_OPRD(Inst->getOperand(OpNo + 2));
      count = -op->getImm();
    }
    if (count != 1) {
      if (CurBlock->size() != 0) {
        if (CurBlock->getType() == Loop)
          CurBlock = CurBlock->addSubBlock(new MCLoopBlock(Repeat, Index, count, ki));
        else if (CurBlock->getParent())
          CurBlock = CurBlock->addSiblingBlock(new MCLoopBlock(Repeat, Index, count, ki));
        else
          CurBlock = addLoopBlock(new MCLoopBlock(Repeat, Index, count, ki));
      }
      else {
        if (CurBlock->getType() == Loop)
          CurBlock = CurBlock->addSubBlock(new MCLoopBlock(Repeat, Index, count, ki));
        else
          CurBlock->setType(Repeat, count, ki);
      }
    }
  }
  else if (Inst->hasLoop(OpNo)) {
    op = CAST_TO_MMPU_OPRD(Inst->getOperand(OpNo + 1));
    if (op->isImm()) return;  //absolute address is ignored in HMacro
    if (op->isExpr()) {
      const MCConstantExpr *CE = dyn_cast<MCConstantExpr>(op->getExpr());
      if (CE) return; //ignored
      else {
        const MCSymbolRefExpr *SE = dyn_cast<MCSymbolRefExpr>(op->getExpr());
        if (SE) {
          cachedLabel = op;
          op = CAST_TO_MMPU_OPRD(Inst->getOperand(OpNo + 2));
          ki = op->getImm();
          op = CAST_TO_MMPU_OPRD(Inst->getOperand(OpNo + 3));
          count = -op->getImm();
        }
      }
    }
  }
  while (Inst->hasEmbeddedHMacro(OpNo)) {
    unsigned OpNoL;
    if (Inst->hasLoop(OpNoL) && OpNoL < OpNo) break;
    Embedded = true;
    op = CAST_TO_MMPU_OPRD(Inst->getOperand(OpNo));
    CurBlock->addEmbeddedHM(new MCFunction(*op->getHMacro(), Index));
    Inst->delOperand(OpNo);
    Inst->delOperand(OpNo);
  }
  CurBlock->addParsedInst(Index, Inst);
  ++Index;
  if (Inst->hasRepeat(OpNo)) {
    Inst->delOperand(OpNo); //delete Repeat
    Inst->delOperand(OpNo); //delete Imm/KI Expr
    if (count <= 0) Inst->delOperand(OpNo); //delete KI difference
    if (count != 1) {
      if (CurBlock->getParent())
        CurBlock = CurBlock->addSiblingBlock(new MCLoopBlock(Sequential, Index));
      else
        CurBlock = addLoopBlock(new MCLoopBlock(Sequential, Index));
    }
  }
  else if (Inst->hasLoop(OpNo)) {
    if (CurBlock->getType() == Loop)
      CurBlock = CurBlock->addSubBlock(new MCLoopBlock(Loop, Index, count, ki));
    else if (CurBlock->getParent())
      CurBlock = CurBlock->addSiblingBlock(new MCLoopBlock(Loop, Index, count, ki));
    else
      CurBlock = addLoopBlock(new MCLoopBlock(Loop, Index, count, ki));
    Inst->delOperand(OpNo); //delete LPTO
    Inst->delOperand(OpNo); //delete Label
    Inst->delOperand(OpNo); //delete KI
    Inst->delOperand(OpNo); //delete KI difference
    if (!Inst->hasLoop(OpNo)) OpNo = Inst->size();
    unsigned OpNoH;
    while (Inst->hasEmbeddedHMacro(OpNoH)) {
      if (OpNoH > OpNo) break;
      Embedded = true;
      op = CAST_TO_MMPU_OPRD(Inst->getOperand(OpNoH));
      CurBlock->addEmbeddedHM(new MCFunction(*op->getHMacro(), Index - 1));
      Inst->delOperand(OpNoH);
      Inst->delOperand(OpNoH);
    }
  }
  PreviousInst = Inst;
}
  
void MCFunction::Finalize(void) {
  if (CurBlock->getType() == Loop && !CurBlock->isNested() &&
      CurBlock->size() == 1 && !CurBlock->getEmbeddedHM(0))
    CurBlock->setType(Repeat, CurBlock->getCount(), CurBlock->getKI());
}

void MCFunction::EliminateEmptyBlock(void) {
  if (!Blocks.empty()) {
    if (Blocks.back()->isNested()) 
      Blocks.back()->EliminateEmptyBlock();
    if (Blocks.back()->size() == 0) {
      delete Blocks.back();
      Blocks.pop_back();
    }
  }
}
  
void MCFunction::ExpandEmbeddedHM(StringMap<HMacro*> &Map) {
  errs() << "Expanding MCFunc:" << Name << "\n";
  for (unsigned i = 0; i < Blocks.size(); i++)
    Blocks[i]->ExpandEmbeddedHM(Map);
  uint64_t next = Start;
  for (unsigned i = 0; i < Blocks.size(); i++)
    next = Blocks[i]->RecalStart(next);
  Index = next;
  Embedded = false;
}
} // namespace Lite
} // namespace llvm
