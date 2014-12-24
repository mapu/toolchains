//===-- MCFunction.cpp ----------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the algorithm to break down a region of machine code
// into basic blocks and try to reconstruct a CFG from it.
//
//===----------------------------------------------------------------------===//

#include "MCFunction.h"

namespace llvm {
  namespace MMPU {
bool MCParsedInst::hasRepeat(unsigned &OpNo) {
  for(OpNo = 0; OpNo < Operands.size(); OpNo++) {
    MMPU::MMPUAsmOperand *op = static_cast<MMPU::MMPUAsmOperand*> (Operands[OpNo]);
    if (op->isToken() &&
        (op->getOpc() == MMPUInst::RepeatImm ||
         op->getOpc() == MMPUInst::RepeatConds))
      return true;
  }
  return false;
}

bool MCParsedInst::hasLoop(unsigned &OpNo) {
  for(OpNo = 0; OpNo < Operands.size(); OpNo++) {
    MMPU::MMPUAsmOperand *op = static_cast<MMPU::MMPUAsmOperand*> (Operands[OpNo]);
    if (op->isToken() && op->getOpc() == MMPUInst::Loop) return true;
  }
  return false;
}

bool MCParsedInst::hasJump(unsigned &OpNo) {
  /*for(OpNo = 0; OpNo < Operands.size(); OpNo++) {
    MMPU::MMPUAsmOperand *op = static_cast<MMPU::MMPUAsmOperand*> (Operands[OpNo]);
    if (op->isToken() &&
        (op->getOpc() == MMPUInst::Jump ||
         op->getOpc() == MMPUInst::JumpConds))
      return true;
  }*/
  return false;
}

bool MCParsedInst::isPsudo() {
  for(unsigned i= 0; i < Operands.size(); ++i) {
    MMPU::MMPUAsmOperand *op = static_cast<MMPU::MMPUAsmOperand*> (Operands[i]);
    if (!op->isMacro()) return false;
  }
  return true;
}

/* ******************************************** *
 * MCLoopBlock methods for unrolling loops      *
 * ******************************************** */

unsigned MCLoopBlock::getLevel() {
  if (Type == Repeat && Count <= 0) return 1;
  if (Type != Loop) return 0;
  if (!KINested) return 1;
  for (unsigned i = 0; i < Blocks.size(); i++)
    if (Blocks[i]->getLevel() != 0)
      return (Blocks[i]->getLevel())+1;
    return 1;
}

uint64_t MCLoopBlock::size() {
  uint64_t sum = 0;
  if (Nested) {
    for (unsigned i = 0; i < Blocks.size(); i++)
      sum += Blocks[i]->Totalsize();
    return sum;
  }
  else return Insts.size();
}

uint64_t MCLoopBlock::Totalsize() {
  return size() + Headsize() + Tailsize();
}

uint64_t MCLoopBlock::abssize() {
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

uint64_t MCLoopBlock::Tailsize() {
  uint64_t sum = 0;
  for (unsigned i = 0; i < Tail.size(); i++)
    sum += Tail[i]->Totalsize();
  return sum;
}

uint64_t MCLoopBlock::Headsize() {
  uint64_t sum = 0;
  for (unsigned i = 0; i < Head.size(); i++)
    sum += Head[i]->Totalsize();
  return sum;
}
uint64_t MCLoopBlock::Tailabssize() {
  uint64_t sum = 0;
  for (unsigned i = 0; i < Tail.size(); i++) {
    if (Tail[i]->getType() != Loop)
      sum += Tail[i]->abssize();
    sum += Tail[i]->Headabssize() +
    Tail[i]->Tailabssize();
  }
  return sum;
}

uint64_t MCLoopBlock::Headabssize() {
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
  bool Roll;
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
      //if (Count == 1) Type = Sequential;
      if (-loops == 1)
        return new MCLoopBlock(Sequential, Start + Insts.size(), 0, 0, Insts, true);
      return new MCLoopBlock(Repeat, Start + Insts.size(), -loops, 0, Insts, true);
    }
    else if (Count <= loops && Count > 0) return this;
    else {
      Roll = Unrolled;
      Unrolled = true;
      Count = Count - loops;
      Start += Insts.size();
      if (Count == 1) Type = Sequential;
      if (loops == 1)
        return new MCLoopBlock(Sequential, Start - Insts.size(), 0, 0, Insts, Roll);
      return new MCLoopBlock(Repeat, Start - Insts.size(), loops, 0, Insts, Roll);
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
        }
        else {
          for (unsigned i = 0; i < Blocks.size(); i++) {
            MCLoopBlock *MB = Blocks[i]->DupThisBlock(Start);
            Tail.insert(Tail.begin() + i, MB);
            MB->setParent(this);
          }
        }
        return this;
      }
      else return NULL; // unrolling more or less than 1 loop is not allowed
    }
    else { // loops > 0
      if (!KINested) {
        while (((uint64_t) loops) >= abssize()) {
          if (!Nested)
            addHeadBlock(new MCLoopBlock(Sequential, Start, 0, 0, Insts, false));
          else
            for (unsigned i = 0; i < Blocks.size(); i++)
              addHeadBlock(Blocks[i]->DupThisBlock(Start));
          loops -= abssize();
          --Count;
        }
        if (loops > 0 && !Nested) {
          unsigned _loops = loops;
          if (Tail.empty() ||
              Tailabssize() < ((uint64_t) loops)) {
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
            }
            else {
              MB = MB->UnrollLoop(loops);
              addHeadBlock(MB->DupThisBlock(Start));
              for (unsigned i = 0; i < Blocks.size(); i++)
                addTailBlock(Blocks[i]->DupThisBlock(Start));
              addSubBlock(MB);
              loops = 0;
            }
          }
          else if (MB->getType() == Repeat) { // RepeatKI
            MB = MB->UnrollLoop(loops);
            addHeadBlock(MB->DupThisBlock(Start));
            for (unsigned i = 0; i < Blocks.size(); i++)
              addTailBlock(Blocks[i]->DupThisBlock(Start));
            addSubBlock(MB);
            loops = 0;
          }
          else { // Loop
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
        }
        else { // !Tail.empty()
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
            }
            else {
              MB = MB->UnrollLoop(loops);
              addHeadBlock(MB);
              if (Nested) {
                MB = Blocks.front()->UnrollLoop(loops);
                addSubBlock(MB);
              }
              loops = 0;
            }
          }
          else if (MB->getType() == Repeat) { // RepeatKI
            MB = MB->UnrollLoop(loops);
            addHeadBlock(MB);
            MB = Blocks.front()->UnrollLoop(loops);
            addSubBlock(MB);
            loops = 0;
          }
          else { // Loop
            while (MB->getHead().empty()) {
              MB->UnrollLoop(loops);
              Blocks.front()->UnrollLoop(loops);
            }
            {
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
    }
    else {
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
  }
  else {
    --Count;
    for (unsigned i = 0; i < Blocks.size(); i++)
      addTailBlock(Blocks[i]->DupThisBlock(Start));
    UnrollOneBlock();
  }
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
        if (*(MBL[i]) == *MBR[j]) {
          if (MBL[i]->getCount() != MBR[j]->getCount()) {
            if (MBL[i]->getType() == Repeat) {
              if (MBL[i]->getCount() < MBR[j]->getCount())
                CompensR += MBR[j]->getCount() - MBL[i]->getCount();
              else
                CompensL += MBL[i]->getCount() - MBR[j]->getCount();
            }
            else if (!MBL[i]->isKINested()){
              if (MBL[i]->getCount() < MBR[j]->getCount())
                CompensR += (MBR[j]->getCount() - MBL[i]->getCount()) * MBR[j]->abssize();
              else
                CompensL += (MBL[i]->getCount() - MBR[j]->getCount()) * MBR[j]->abssize();
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

void MCLoopBlock::EliminateEmptyBlock() {
  if (!Blocks.empty()) {
    if (Blocks.back()->isNested())
      Blocks.back()->EliminateEmptyBlock();
    if (Blocks.back()->size() == 0) {
      delete Blocks.back();
      Blocks.pop_back();
    }
  }
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
  MMPU::MMPUAsmOperand *op;
  int64_t count = 1;
  unsigned ki = 0;
  const MCSymbol *Sym;

  if (!CurBlock) CurBlock = addLoopBlock(new MCLoopBlock(Sequential, Index));
  if (!PendingLabels.empty()) {
    for (Sym = Ctx->LookupSymbol(PendingLabels.back()->getName());
         Sym->isDefined();
         Sym = Ctx->LookupSymbol(PendingLabels.back()->getName())) {
      //now the end of loop block is arrived
      if (CurBlock->size() == 0) {
        if (CurBlock->getType() != Loop)
          CurBlock->setType(Sequential, 0, 0);
        CurBlock->getParent()->getBlocks().pop_back();
        if (CurBlock->getParent()->getParent())
          CurBlock = CurBlock->getParent()->addSiblingBlock(CurBlock);
        else
          CurBlock = addLoopBlock(CurBlock);
      }
      else {
        if (CurBlock->size() == 1 && CurBlock->getType() == Loop && !CurBlock->isNested())
          CurBlock->setType(Repeat, CurBlock->getCount(), CurBlock->getKI()); // Simplify the single inst loop to a RepeatK
         // if current block is not a loop, then it is a block nested in the loop that is going to end
        else if (CurBlock->getType() != Loop)
          CurBlock = CurBlock->getParent(); // CurBlock must have a parent. Top level Block won't nest in a loop
        if (CurBlock->getParent())
          CurBlock = CurBlock->addSiblingBlock(new MCLoopBlock(Sequential, Index)); // this loop is also a nested one
        else
          CurBlock = addLoopBlock(new MCLoopBlock(Sequential, Index)); // this loop is at the top level
      }
      PendingLabels.pop_back();
      if (PendingLabels.empty()) break;
    }
  }
  if (cachedLabel) {
    PendingLabels.push_back(cachedLabel);
    cachedLabel = NULL;
    while (PreviousInst && PreviousInst->hasLoop(OpNo)) {
      op = static_cast<MMPU::MMPUAsmOperand*> (PreviousInst->getOperand(OpNo - 3));
      if (op->isImm()) return;  //absolute address is ignored in HMacro
      if (op->isExpr()) {
        const MCConstantExpr *CE = dyn_cast<MCConstantExpr>(op->getExpr());
        if (CE) return; //ignored
        else {
          const MCSymbolRefExpr *SE = dyn_cast<MCSymbolRefExpr>(op->getExpr());
          if (SE) {
            PendingLabels.push_back(&(SE->getSymbol()));
            op = static_cast<MMPU::MMPUAsmOperand*> (PreviousInst->getOperand(OpNo - 1));
            ki = op->getImm();
            op = static_cast<MMPU::MMPUAsmOperand*> (PreviousInst->getOperand(OpNo - 2));
            if (op->isImm())
              count = op->getImm();
            else if (op->isExpr())
              op->getExpr()->EvaluateAsAbsolute(count);
            count = -count;
          }
        }
      }
      CurBlock = CurBlock->addSubBlock(new MCLoopBlock(Loop, Index, count, ki));
      PreviousInst->delOperand(OpNo); //delete LPTO
      PreviousInst->delOperand(OpNo); //delete Slot
      PreviousInst->delOperand(OpNo-1); //delete KI
      PreviousInst->delOperand(OpNo-2); //delete KI difference
      PreviousInst->delOperand(OpNo-3); //delete Label
    }
  }
  if (Inst->hasRepeat(OpNo)) {
    op = static_cast<MMPU::MMPUAsmOperand*> (Inst->getOperand(OpNo - 1));
    if (op->isImm())
      count = op->getImm();
    else if (op->isExpr())
      op->getExpr()->EvaluateAsAbsolute(count);
    op = static_cast<MMPU::MMPUAsmOperand*> (Inst->getOperand(OpNo));
    if (op->getOpc() == MMPUInst::RepeatImm && count == 0) count = 1024;
    else if (op->getOpc() == MMPUInst::RepeatConds) {
      ki = count;
      op = static_cast<MMPU::MMPUAsmOperand*> (Inst->getOperand(OpNo-2));
      if (op->isImm())
        count = op->getImm();
      else if (op->isExpr())
        op->getExpr()->EvaluateAsAbsolute(count);
      count = -count;
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
    op = static_cast<MMPU::MMPUAsmOperand*> (Inst->getOperand(OpNo - 3));
    if (op->isImm()) return;  //absolute address is ignored in HMacro
    if (op->isExpr()) {
      const MCConstantExpr *CE = dyn_cast<MCConstantExpr>(op->getExpr());
      if (CE) return; //ignored
      else {
        const MCSymbolRefExpr *SE = dyn_cast<MCSymbolRefExpr>(op->getExpr());
        if (SE) {
          cachedLabel = &(SE->getSymbol());
          op = static_cast<MMPU::MMPUAsmOperand*> (Inst->getOperand(OpNo - 1));
          ki = op->getImm();
          op = static_cast<MMPU::MMPUAsmOperand*> (Inst->getOperand(OpNo - 2));
          if (op->isImm())
            count = op->getImm();
          else if (op->isExpr())
            op->getExpr()->EvaluateAsAbsolute(count);
          count = -count;
        }
      }
    }
  }
  /*if (CurBlock->getType() == Sequential &&
    Inst->isPsudo()) {
    if (CurBlock->size()) {
      if (CurBlock->getType() == Loop)
          CurBlock = CurBlock->addSubBlock(new MCLoopBlock(None, Index));
      else if (CurBlock->getParent())
        CurBlock = CurBlock->addSiblingBlock(new MCLoopBlock(None, Index));
      else
        CurBlock = addLoopBlock(new MCLoopBlock(None, Index));
    }
    else CurBlock->setType(None, 0, 0);
  }*/
  CurBlock->addParsedInst(Index, Inst);
  ++Index;
  if (Inst->hasRepeat(OpNo)) {
    Inst->delOperand(OpNo); //delete Repeat
    Inst->delOperand(OpNo); //delete Slot
    Inst->delOperand(OpNo-1); //delete Imm/KI difference
    if (count <= 0)
      Inst->delOperand(OpNo-2); //delete KI
    if (count != 1) {
      if (CurBlock->getParent())
        CurBlock = CurBlock->addSiblingBlock(new MCLoopBlock(Sequential, Index));
      else
        CurBlock = addLoopBlock(new MCLoopBlock(Sequential, Index));
    }
  }
  else if (Inst->hasLoop(OpNo)) {
    Inst->delOperand(OpNo); //delete LPTO
    Inst->delOperand(OpNo); //delete Slot
    Inst->delOperand(OpNo-1); //delete KI
    Inst->delOperand(OpNo-2); //delete KI difference
    Inst->delOperand(OpNo-3); //delete Label
    if (CurBlock->getType() == Loop)
      CurBlock = CurBlock->addSubBlock(new MCLoopBlock(Loop, Index, count, ki));
    else if (CurBlock->getParent())
      CurBlock = CurBlock->addSiblingBlock(new MCLoopBlock(Loop, Index, count, ki));
    else
      CurBlock = addLoopBlock(new MCLoopBlock(Loop, Index, count, ki));
  }
  PreviousInst = Inst;
}

void MCFunction::EliminateEmptyBlock() {
  if (!Blocks.empty()) {
    if (Blocks.back()->isNested())
      Blocks.back()->EliminateEmptyBlock();
    if (Blocks.back()->size() == 0) {
      delete Blocks.back();
      Blocks.pop_back();
    }
  }
}

  } // namespace MMPU
} // namespace llvm
