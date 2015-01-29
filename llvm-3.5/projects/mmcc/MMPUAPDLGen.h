//===-- MMPUAPDLGen.h - MMPU APDLGen class -------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===--------------------------------------------------------------------===//
//
//
//===--------------------------------------------------------------------===//

#ifndef MMPUAPDLGEN_H
#define MMPUAPDLGEN_H

#include "AsmParser/MMPULiteAsmParser.h"
#include "AsmParser/MCFunction.h"
#include "MMPUAST.h"
#include "MMPUDAG.h"

#include <vector>
#include <bitset>
#include "llvm/Support/raw_ostream.h"

using namespace llvm;
using namespace llvm::Lite;

//===----------------------------------------------------------------------===//
// Block
//===----------------------------------------------------------------------===//

namespace MMPU {

class Block;
  
class APDLGen {
  raw_ostream *O;
  unsigned OptLevel;
  MCLoopBlock *APDLFunc;
  Symbol *PrevSym;
  /* Use this unified ID for both virtual TReg and MReg, may be
     also the DM in the future. */
  uint64_t VRegID;
  unsigned BIUID;
  uint64_t KRegID;
  unsigned LoopLabel;
  bool needTemp;
  
  std::bitset<32> Flags;
  
  /* record the usage of all VRegs */
  struct Usage {
    TagExprAST::TagTy type;
    std::vector<MCParsedInst *> consumers;
    MCParsedInst *producer;
    //unsigned MMPULiteReg;
  public:
    Usage() : producer(NULL)/*, MMPULiteReg(MMPULiteReg::NoRegister)*/ {}
  };
  
  std::map<uint64_t, struct Usage> VRegTable;
  
  std::map<MCParsedInst *, std::vector<uint64_t> > SrcTable;
  std::map<MCParsedInst *, std::vector<uint64_t> > DestTable;
  
  std::vector<uint64_t> SrcStack;
  std::vector<uint64_t> DestStack;
  
  std::map<MCParsedInst *, uint64_t> GlobalTerminals;
  
  /* Optimization data structures */
  struct BlockInfo {
    MCLoopBlock *Block;
    std::vector<uint64_t> Entry;
    std::vector<uint64_t> Terminal;
    bool Scheduled;
  public:
    BlockInfo() : Block(NULL), Scheduled(false) {}
  };
  
  std::map<MCLoopBlock *, struct BlockInfo> LeafBlockInfo;
  
  struct LoopInfo {
    uint64_t KReg;
    uint64_t OrigSize;
    uint64_t SplitSize;
  };
  std::map<uint64_t, struct LoopInfo> LoopInfoMap;
  
  /*struct GlobDAGNode {
    MCLoopBlock *Block;
    Graph<struct GlobDAGNode *> DAG;
  };*/
  
  Graph<MCLoopBlock *> GlobalDAG;
  
  std::vector<MCLoopBlock *> HMacros;
  
  std::vector<MCParsedInst *> FragmentInsts;
  
  struct lifespan {
    uint64_t birth;
    uint64_t retire;
    uint64_t loop;
    uint64_t loopsize;
  public:
    lifespan() : birth(0), retire(0), loop(1), loopsize(0) {}
  };
  
  uint64_t AllocateVReg(TagExprAST::TagTy type) {
    VRegTable[VRegID].type = type;
    return VRegID++;
  }
  
  uint64_t AllocateKReg(void) {
    return KRegID++;
  }

  void getSrc(std::vector<uint64_t> &Stack, MCParsedInst *MCI);
  
  void addParsedInst(MCParsedInst *Inst) {
    APDLFunc->addParsedInst(Inst);
    SrcTable[Inst] = SrcStack;
    DestTable[Inst] = DestStack;
    while (!SrcStack.empty()) {
      VRegTable[SrcStack.back()].consumers.push_back(Inst);
      SrcStack.pop_back();
    }
    while (!DestStack.empty()) {
      VRegTable[DestStack.back()].producer = Inst;
      DestStack.pop_back();
    }
  }
  
  bool hasDest(unsigned Opc) const;
  
  bool hasTDest(unsigned Opc) const;
  
  bool hasMDest(unsigned Opc) const;
  
  bool UseIPath(unsigned Opc) const;
  
  bool CheckCollision(MCParsedInst *I1, MCParsedInst *I2) const;
  
  void EstimatePipelineUsage(MCLoopBlock *Pipeline, uint64_t &Bubbles) const;
  
  void genBlock(Block *B);
  
  void genAST(ExprAST *E);
  
  void Gen_Assign(ExprAST *Expr);
  
  void Gen_AssignOp(ExprAST *Dest, ExprAST *Expr);
  
  TagExprAST::TagTy getLValType(ExprAST *E);
  
  void Gen_RVal(ExprAST *E, SharedOperandVector &O);
  
  void Gen_IALU_Binary(ExprAST *E, TagExprAST::TagTy Op,
                       SharedOperandVector &O);
  
  void Gen_IMAC_Binary(ExprAST *E, TagExprAST::TagTy Op,
                       SharedOperandVector &O);
  
  void Gen_RIdent(ExprAST *E, SharedOperandVector &O);
  
  void Gen_Const(ExprAST *E, SharedOperandVector &O);
  
  MCInst *Translate(MCParsedInst *Inst);
  
public:
  APDLGen(raw_ostream *_O)
    : O(_O), OptLevel(0), APDLFunc(NULL), PrevSym(NULL), VRegID(3),
      BIUID(0), KRegID(12), LoopLabel(0), needTemp(false) {
    VRegTable[0].type = TagExprAST::STORAGE_DM;
    VRegTable[1].type = TagExprAST::STORAGE_DM;
    VRegTable[2].type = TagExprAST::STORAGE_DM;
    Flags.reset();
  }
  
  APDLGen(raw_ostream *_O, unsigned Level)
    : O(_O), OptLevel(0), APDLFunc(NULL), PrevSym(NULL), VRegID(3),
      BIUID(0), KRegID(12), LoopLabel(0), needTemp(false) {
    VRegTable[0].type = TagExprAST::STORAGE_DM;
    VRegTable[1].type = TagExprAST::STORAGE_DM;
    VRegTable[2].type = TagExprAST::STORAGE_DM;
    Flags.reset();
  }
  
  void setOptLevel(unsigned Level) { OptLevel = Level; }
  
  void GenAPDLInst(Block *BAst);
  
  /* Legalize the MCParsedInsts generated in GenAPDLInst stage,
     including TReg destination determine, NOP insertion */
  void Legalize(void);
  
  /* Allocate TReg and MReg */
  void RegAllocation(void);
  
  /* Generate APDL file */
  void Emit(void);
  
  /* Global blocks schedule */
  void GlobalSchedule(void);
  
  /* Software pipelining */
  void SoftPipe(void);
  
  /* Allocation all resouces except Regs */
  void ResourceAllocation(void);
  
  /* Generate tree structure of APDL */
  void GenAPDLTree(void);
  
  /* Allocation all resouces except Regs */
  void FixupFragmentInst(void);
};
}
#endif

