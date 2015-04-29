//===------- apdlgen/GenRIdent.cpp - Generate Right Side Identifier-----===//
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

#include "MMPUAPDLGen.h"
#include "MMPUAST.h"
#include "MMPUSymbol.h"
#include "MMPUBlock.h"
#include "AsmParser/MMPULiteScheduler.h"
#include <list>

using namespace llvm;
using namespace Lite;
using namespace MMPULite;

namespace MMPU {
void APDLGen::Gen_RIdent(ExprAST *E, SharedOperandVector &O) {
  TagExprAST::TagTy t = getLValType(E);
  Symbol *s = NULL;
  SmallVector<SharedOperand, 8> LoadO;
  uint64_t tempT;
  unsigned dist = BIU1LdToM - BIU0LdToM;
  int64_t Slot;
  TagExprAST::TagTy isInc = TagExprAST::INVALIDTAG;
  ExprAST *Index = NULL;

  if (E && E->getNext() && E->getNext()->getNext() &&
      isa<SymExprAST>(E->getNext()->getNext())) {
    isInc = dyn_cast<TagExprAST>(E->getNext())->getTag();
    s = dyn_cast<SymExprAST>(E->getNext()->getNext())->getSymbol();
  }

  if (E && E->getNext() && E->getNext()->getNext() &&
      E->getNext()->getNext()->getNext()) {
    Index = E->getNext()->getNext()->getNext();

    if (Index && Index->getNext() && isa<TagExprAST>(Index->getNext()))
      isInc = dyn_cast<TagExprAST>(Index->getNext())->getTag();
  }

  if (!needTemp) {
    /* Solo identifier on the right hand of assignment:
       dmem = mreg : store mreg to dm
       dmem = treg : illegal
       dmem = dmem : illegal
       mreg = mreg : legal only if load from M.r0
       mreg = treg : illegal
       mreg = dmem : load mreg from dm
       treg = mreg : load treg from mreg
       treg = dmem : load treg from dm
       treg = treg : only allowed load treg from shu treg
       dmem/mreg/treg = mac : psuedo
       mac  = dmem/mreg/treg : should not happen here
       */
    // dedute which type of storage the dest is
    TagExprAST::TagTy dest = TagExprAST::INVALIDTAG;
    if (O.size() == 3) dest = TagExprAST::STORAGE_DM;
    else if (O.size() == 4) dest = TagExprAST::STORAGE_MREG;
    else if (O.size() == 6) dest = TagExprAST::STORAGE_TREG;
    if (t == TagExprAST::STORAGE_DM) {
      if (s) {
        if (s->getID() == -1)
          s->Allocate(BIUID++);
        unsigned op = BIU0LdToM + s->getID() * dist;
        if (dest == TagExprAST::STORAGE_MREG) ;
        else if (dest == TagExprAST::STORAGE_TREG) op += 1;
        O[0] = SHARED_OPRD(Opc, op, SMLoc(), SMLoc());
        if (isInc == TagExprAST::EXPR_APDLINC) {
          Flags.set(APPF);
          Flags.set(KPPF);
        }
        O.push_back(SHARED_OPRD(Imm, Flags.to_ulong(), SMLoc(), SMLoc()));
        Flags.reset();
        O.push_back(SHARED_OPRD(Slot, 10 + s->getID(), SMLoc(), SMLoc()));
      }
    } else if (t == TagExprAST::STORAGE_MREG) {
      if (s) {
        if (s->getID() == -1)
          errs() << "Uninitialized MReg variable for \"??? = MReg\".\n";  // this is an error
        unsigned op = MR0ToM;
        if (dest == TagExprAST::STORAGE_DM) op = MR3ToBIU;
        else if (dest == TagExprAST::STORAGE_MREG) op = MR0ToM;
        else if (dest == TagExprAST::STORAGE_TREG) op = MR0ToMACC;
        O[0] = SHARED_OPRD(Opc, op, SMLoc(), SMLoc());
        O.push_back(SHARED_OPRD(Reg, MMPULiteReg::M0, SMLoc(), SMLoc()));
        O.push_back(SHARED_OPRD(Imm, s->getID(), SMLoc(), SMLoc()));
        // MRnToXX needs WF/WX flag
        O.push_back(SHARED_OPRD(Reg, MMPULiteReg::WF0, SMLoc(), SMLoc()));
        O.push_back(SHARED_OPRD(Slot, dest == TagExprAST::STORAGE_DM ? 3 : 0,
                                SMLoc(), SMLoc()));
        SrcStack.push_back(s->getID());
      }
    } else if (t == TagExprAST::STORAGE_MACREG) {
      if (s) {
        if (s->getID() == -1)
          errs() << "Uninitialized MAC variable for \"??? = MAC\".\n";
        unsigned op = IMRToBIU;
        if (dest == TagExprAST::STORAGE_DM) ;
        else if (dest == TagExprAST::STORAGE_MREG) op += 1;
        else if (dest == TagExprAST::STORAGE_TREG) op += 2;
        O[0] = SHARED_OPRD(Opc, op, SMLoc(), SMLoc());
        O.push_back(SHARED_OPRD(Reg, MMPULiteReg::MR, SMLoc(), SMLoc()));
        O.push_back(SHARED_OPRD(Imm, s->getID(), SMLoc(), SMLoc()));
        O.push_back(SHARED_OPRD(Imm, 0, SMLoc(), SMLoc())); //flags
        O.push_back(SHARED_OPRD(Slot, 13, SMLoc(), SMLoc()));
        SrcStack.push_back(s->getID());
      }
    } else {
      errs() << "Use expression : \"??? = TReg/KReg\" is illegal\n";
    }
    return;
  } else { // needTemp
    if (t == TagExprAST::STORAGE_DM) {
      /* BIU load is required */
      tempT = AllocateVReg(TagExprAST::STORAGE_TREG);
      if (s) {
        if (s->getID() == -1) s->Allocate(BIUID++);
        LoadO.push_back(SHARED_OPRD(Opc, BIU0LdToMACC + s->getID() * dist,
                                    SMLoc(), SMLoc()));
        LoadO.push_back(SHARED_OPRD(Reg, MMPULiteReg::IALU, SMLoc(), SMLoc()));
        LoadO.push_back(SHARED_OPRD(Reg, MMPULiteReg::VT, SMLoc(), SMLoc()));
        LoadO.push_back(SHARED_OPRD(Imm, tempT, SMLoc(), SMLoc()));
        LoadO.push_back(SHARED_OPRD(Imm, 0, SMLoc(), SMLoc())); // IPath
        Slot = 10 + s->getID();
      }
    } else if (t == TagExprAST::STORAGE_MREG) {
      /* MR inst is required */
      tempT = AllocateVReg(TagExprAST::STORAGE_TREG);
      if (s) {
        if (s->getID() == -1)
          errs() << "Uninitialized MReg variable.\n";  // this is an error
                                                       // use MR0 as default
        LoadO.push_back(SHARED_OPRD(Opc, MR0ToMACC, SMLoc(), SMLoc()));
        LoadO.push_back(SHARED_OPRD(Reg, MMPULiteReg::IALU, SMLoc(), SMLoc()));
        LoadO.push_back(SHARED_OPRD(Reg, MMPULiteReg::VT, SMLoc(), SMLoc()));
        LoadO.push_back(SHARED_OPRD(Imm, tempT, SMLoc(), SMLoc()));
        LoadO.push_back(SHARED_OPRD(Imm, 0, SMLoc(), SMLoc())); // IPath
        // MRnToXX needs WF/WX flag
        LoadO.push_back(SHARED_OPRD(Reg, MMPULiteReg::WF0, SMLoc(), SMLoc()));
        Slot = 0;
      }
    } else if (t == TagExprAST::STORAGE_TREG) {
      /* direct use TReg */
      if (s) {
        if (s->getID() == -1)
          assert("Uninitialized TReg variable.\n"); // this is an error
      }
      if (isInc != TagExprAST::INVALIDTAG) {
        /* Expression: treg var[index] is found here. there are two possible
           usage:
           1. index is a classic integer variable, then this is a broadcast inst
              used for operations between vector and scalar.
           2. index is a treg variable, then this is a table searching inst
              used for table searching, shuffle or irregular broadcast.
         */
        tempT = AllocateVReg(TagExprAST::STORAGE_TREG);
        if (isInc == TagExprAST::EXPR_SUB)
          LoadO.push_back(SHARED_OPRD(Opc, SHU0IndTBOptToMACC, SMLoc(), SMLoc()));
        else if (isInc == TagExprAST::EXPR_ASSIGN) {
          if (Index && Index->getNext() && Index->getNext()->getNext() &&
              isa<ExprExprAST>(Index->getNext()->getNext())) {
            Index = dyn_cast<ExprExprAST>(Index->getNext()->getNext())->getExpr();
            if (Index && Index->getNext() && isa<TagExprAST>(Index->getNext()) &&
                dyn_cast<TagExprAST>(Index->getNext())->getTag() ==
                TagExprAST::EXPR_APDLINC)
              LoadO.push_back(SHARED_OPRD(Opc, SHU0IndTOptToMACC, SMLoc(), SMLoc()));
            else
              LoadO.push_back(SHARED_OPRD(Opc, SHU0IndTNoOptToMACC, SMLoc(), SMLoc()));
          }
        } else if (isInc == TagExprAST::EXPR_IDENT) {
          LoadO.push_back(SHARED_OPRD(Opc, SHU0IndTBNoOptToMACC, SMLoc(), SMLoc()));
        }
        LoadO.push_back(SHARED_OPRD(Reg, MMPULiteReg::IALU, SMLoc(), SMLoc()));
        LoadO.push_back(SHARED_OPRD(Reg, MMPULiteReg::VT, SMLoc(), SMLoc()));
        LoadO.push_back(SHARED_OPRD(Imm, tempT, SMLoc(), SMLoc()));
        LoadO.push_back(SHARED_OPRD(Imm, 0, SMLoc(), SMLoc())); // IPath
        Slot = 4;
      }
    } else if (t == TagExprAST::STORAGE_MACREG) {
      /* Load from MAC Register, generate a Psuedo inst for it and wait
         for legalization */
      if (s) {
        if (s->getID() == -1)
          errs() << "Uninitialized MAC variable.\n";
        tempT = AllocateVReg(TagExprAST::STORAGE_MACREG);
        LoadO.push_back(SHARED_OPRD(Opc, IMRToMACC, SMLoc(), SMLoc()));
        LoadO.push_back(SHARED_OPRD(Reg, MMPULiteReg::IALU, SMLoc(), SMLoc()));
        LoadO.push_back(SHARED_OPRD(Reg, MMPULiteReg::VT, SMLoc(), SMLoc()));
        LoadO.push_back(SHARED_OPRD(Imm, tempT, SMLoc(), SMLoc()));
        LoadO.push_back(SHARED_OPRD(Imm, 0, SMLoc(), SMLoc())); // IPath
        Slot = 7;
      }
    }
  }

  if (t != TagExprAST::STORAGE_TREG) {
    LoadO.push_back(SHARED_OPRD(Reg, MMPULiteReg::NOC, SMLoc(), SMLoc()));
    if (t == TagExprAST::STORAGE_DM) {
      // flags
      if (isInc == TagExprAST::EXPR_APDLINC) {
        Flags.set(APPF);
        Flags.set(KPPF);
      }
      LoadO.push_back(SHARED_OPRD(Imm, Flags.to_ulong(), SMLoc(), SMLoc()));
      Flags.reset();
    } else if (t == TagExprAST::STORAGE_MREG) { // MReg Load
      LoadO.push_back(SHARED_OPRD(Reg, MMPULiteReg::M0, SMLoc(), SMLoc()));
      LoadO.push_back(SHARED_OPRD(Imm, s->getID(), SMLoc(), SMLoc()));
      SrcStack.push_back(s->getID());
    } else if (t == TagExprAST::STORAGE_MACREG) {
      LoadO.push_back(SHARED_OPRD(Reg, MMPULiteReg::MR, SMLoc(), SMLoc()));
      LoadO.push_back(SHARED_OPRD(Imm, s->getID(), SMLoc(), SMLoc()));
      SrcStack.push_back(s->getID());
    }
    LoadO.push_back(SHARED_OPRD(Slot, Slot, SMLoc(), SMLoc()));
    DestStack.push_back(tempT);
    addParsedInst(new MCParsedInst(LoadO));
  } else if (isInc != TagExprAST::INVALIDTAG) {
    unsigned op = CAST_TO_MMPU_OPRD(LoadO[0])->getOpc();
    LoadO.push_back(SHARED_OPRD(Reg, MMPULiteReg::NOC, SMLoc(), SMLoc()));
    LoadO.push_back(SHARED_OPRD(Reg, MMPULiteReg::VT, SMLoc(), SMLoc()));
    LoadO.push_back(SHARED_OPRD(Imm, s->getID(), SMLoc(), SMLoc()));
    SrcStack.push_back(s->getID());
    if (op == SHU0IndTNoOptToMACC || op == SHU0IndTOptToMACC)
      LoadO.push_back(SHARED_OPRD(Reg, MMPULiteReg::TBW, SMLoc(), SMLoc()));
    if (op == SHU0IndTBOptToMACC || op == SHU0IndTOptToMACC)
      LoadO.push_back(SHARED_OPRD(Imm, 4, SMLoc(), SMLoc()));
    else
      LoadO.push_back(SHARED_OPRD(Imm, 0, SMLoc(), SMLoc()));

    LoadO.push_back(SHARED_OPRD(Slot, Slot, SMLoc(), SMLoc()));
    DestStack.push_back(tempT);
    addParsedInst(new MCParsedInst(LoadO));
  }

  O.push_back(SHARED_OPRD(Reg, MMPULiteReg::VT, SMLoc(), SMLoc()));
  if (t != TagExprAST::STORAGE_TREG || isInc != TagExprAST::INVALIDTAG)
    O.push_back(SHARED_OPRD(Imm, tempT, SMLoc(), SMLoc()));
  else
    O.push_back(SHARED_OPRD(Imm, s->getID(), SMLoc(), SMLoc()));
}
}
