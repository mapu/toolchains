//===-- MMPULiteELFRelocationInfo.cpp ----------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/MMPULiteMCTargetDesc.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/MC/MCRelocationInfo.h"
#include "llvm/Object/ELFObjectFile.h"
#include "llvm/Support/ELF.h"

using namespace llvm;
using namespace object;
using namespace ELF;

namespace {
class MMPULiteELFRelocationInfo : public MCRelocationInfo {
public:
  MMPULiteELFRelocationInfo(MCContext &Ctx) : MCRelocationInfo(Ctx) {}

  const MCExpr *createExprForRelocation(RelocationRef Rel) {
    uint64_t RelType; Rel.getType(RelType);
    symbol_iterator SymI = Rel.getSymbol();

    StringRef SymName; SymI->getName(SymName);
    uint64_t  SymAddr; SymI->getAddress(SymAddr);
    uint64_t  SymSize; SymI->getSize(SymSize);
    int64_t  Addend;  getELFRelocationAddend(Rel, Addend);

    MCSymbol *Sym = Ctx.GetOrCreateSymbol(SymName);
    // FIXME: check that the value is actually the same.
    if (Sym->isVariable() == false)
      Sym->setVariableValue(MCConstantExpr::Create(SymAddr, Ctx));

    const MCExpr *Expr = 0;
    // If hasAddend is true, then we need to add Addend (r_addend) to Expr.
    bool hasAddend = true;

    switch(RelType) {
    default:
      Expr = MCSymbolRefExpr::Create(Sym, Ctx);
      break;
    }
    if (Expr && hasAddend && Addend != 0)
      Expr = MCBinaryExpr::CreateAdd(Expr,
                                     MCConstantExpr::Create(Addend, Ctx),
                                     Ctx);
    return Expr;
  }
};
} // End unnamed namespace

/// createMMPULiteELFRelocationInfo - Construct an MMPULite Mach-O RelocationInfo.
MCRelocationInfo *llvm::createMMPULiteELFRelocationInfo(MCContext &Ctx) {
  // We only handle MMPULite-64 for now.
  return new MMPULiteELFRelocationInfo(Ctx);
}
