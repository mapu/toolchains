//===-- MMPULiteScheduler.cpp -------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the algorithm to record resouce occupation and release
// history and analyze resources used by all parsed MCInsts.
//
//===----------------------------------------------------------------------===//

#include "MMPULiteScheduler.h"

using namespace llvm;
namespace llvm{

void MMPULiteScheduler::allocResvRecord() {
  std::map<unsigned, resv> *records = new std::map<unsigned, resv>;
  ResvBoard.push_back(*records);
}

void MMPULiteScheduler::Schedule(uint64_t VA, MCInst *MI, unsigned Sym) {
  History.push_back(*(new std::vector<use>));
  std::vector<use> &instline = History.back();
  const MCInst *MIP = MI;
  do {
    if (MIP == MI) MIP = MIP->getOperand(0).getInst();//???
    else MIP = MIP->getOperand(MIP->getNumOperands() - 1).getInst();
    const unsigned opcode = MIP->getOpcode() - MMPULite::BIU0KG;
    const uint64_t DefUsage = ResUsageMap[opcode];
    const uint64_t Usage = UsageFixup	(DefUsage, MIP);
    if (Usage != 0) {
      unsigned residx = 0;
      unsigned use_begin = instline.size();
      for (unsigned i = Invalid; i < NumResouces; i++) {
        if (testUsage(Usage, i)) {
          if (i == Invalid) break;
          instline.push_back(*(new use));
          instline.back().ResID = i;
          instline.back().Delay = ResLatMap[opcode][residx * 2];
          instline.back().Owner = MIP;
          if ((i <= MW3) && MIP->getOperand(0).isReg())
            instline.back().MID = MIP->getOperand(0).getReg();
          else
            instline.back().Period = ResLatMap[opcode][residx * 2 + 1];
          //raw_ostream &OS = errs();
          //OS << History.size() << ":" << opcode << "/" << i << "/" << ResLatMap[opcode][residx * 2] << "/" << residx << "\n";
        }
        if (testUsage(DefUsage, i)) ++residx;
      }
      FixupDoubleRegWB(instline, MIP, use_begin);
    }
    Trace(MI);
  } while (MIP->getNumOperands() != 0 &&
           MIP->getOperand(MIP->getNumOperands() - 1).isInst());
  // Begin to analyze the control flow
  if (MIP->getOpcode() == MMPULite::REPEATIMM) {
    int64_t imm = MIP->getOperand(0).getImm() - 1;
    imm = imm > 10 ? 10 : imm;
    while (imm-- > 0) History.push_back(History.back());
  } else if (MIP->getOpcode() == MMPULite::REPEATK)
    for (unsigned i = 0; i < 10; i++)//???
      History.push_back(History.back());
  while (!LoopStack.empty() && Sym) {//???
    uint64_t EndPoint = History.size();
    unsigned round = 10 / (EndPoint - LoopStack.back()) + 1;
    for (unsigned i = 0; i < round; i++)
      for (uint64_t j = LoopStack.back(); j < EndPoint; j++)
        History.push_back(History[j]);
    LoopStack.pop_back();
    --Sym;
  }
  if (MIP->getOpcode() == MMPULite::LPTO) LoopStack.push_back(History.size());
  // Begin to check collision
  for (; CurPoint < History.size(); CurPoint++) {
    if (History[CurPoint].empty())
      ResvBoard.push_back(*(new std::map<unsigned, resv>));
    else {
      for (unsigned i = 0; i < History[CurPoint].size(); i++) {
        use &curuse = History[CurPoint][i];
        while (ResvBoard.size() <= curuse.Delay)
          ResvBoard.push_back(*(new std::map<unsigned, resv>));
        if (ResvBoard[curuse.Delay].count(curuse.ResID)) {
          if (Parser) {
            Parser->Warning(curuse.Owner->getLoc(),
                           "Collision detected, Resouce ID " +
                           Twine(curuse.ResID) + "(" + Twine(ResName[curuse.ResID]) +
                           ") is used by Inst:");
            Parser->Warning(ResvBoard[curuse.Delay][curuse.ResID].Owner->getLoc(),
                          "and Inst:");
          } else {
            raw_ostream &OS = errs();
            OS << "Collision detected between inst 0x"
            << Twine::utohexstr(VA) << " and 0x"
            << Twine::utohexstr(ResvBoard[curuse.Delay][curuse.ResID].IssueTime)
            << ". Resouce ID is " << curuse.ResID << "(" << ResName[curuse.ResID]
            << ")" << ".\n";
          }
        } else {
          ResvBoard[curuse.Delay][curuse.ResID].IssueTime = VA;
          ResvBoard[curuse.Delay][curuse.ResID].ValidTime = VA + curuse.Delay;
          ResvBoard[curuse.Delay][curuse.ResID].Owner = curuse.Owner;
          if (curuse.ResID <= MW3) {
            ResvBoard[curuse.Delay][curuse.ResID].MID = curuse.MID;
            for (unsigned j = MW0; j <= MW3; j++)
              if (j != curuse.ResID &&
                  ResvBoard[curuse.Delay].count(j) &&
                  ResvBoard[curuse.Delay][j].MID == curuse.MID &&
                  curuse.MID !=  MMPULiteReg::MIPP &&
                  curuse.MID !=  MMPULiteReg::MSPP) {
                if (Parser) {
                  Parser->Warning(curuse.Owner->getLoc(),
                                  "M Register Collision detected, M" +
                                  Twine(curuse.MID - MMPULiteReg::M0) +
                                  " is used by Inst:");
                  Parser->Warning(
                    ResvBoard[curuse.Delay][curuse.ResID].Owner->getLoc(),
                    "and Inst:");
                }
                else {
                  raw_ostream &OS = errs();
                  OS << "M Register Collision detected between inst 0x"
                     << Twine::utohexstr(VA) << " and 0x" << Twine::utohexstr(
                        ResvBoard[curuse.Delay][curuse.ResID].IssueTime)
                     << ". Register ID is M" << curuse.MID - MMPULiteReg::M0
                     << ".\n";
                }
              }
          } else
            ResvBoard[curuse.Delay][curuse.ResID].Period = curuse.Period;
        }
      }
    }
    ResvBoard.erase(ResvBoard.begin());
  }
}

void MMPULiteScheduler::Trace(MCInst *MI) {
  unsigned TResID = 0;
  for (uint64_t i = History.size() - 1; i <= 0; i++) {
    if (History[i].empty()) continue;
      switch(History[i][i].Owner->getOpcode()) {
        
      }
  }
}

uint64_t MMPULiteScheduler::UsageFixup(const uint64_t DefUsage,
                                        const MCInst *MI) {
  const unsigned opc = MI->getOpcode();
  uint64_t Patch = 0xFFFFFFFFFFFFFFFFULL;
  switch (opc) {
  case MMPULite::BIU0LdToMACC:
  case MMPULite::BIU1LdToMACC:
  case MMPULite::BIU2LdToMACC:
  case MMPULite::MR0ToMACC:
  case MMPULite::MR1ToMACC:
  case MMPULite::MR2ToMACC:
  case MMPULite::SHU0CombToMACC:
  case MMPULite::SHU0IndTBNoOptToMACC:
  case MMPULite::SHU0IndTBOptToMACC:
  case MMPULite::SHU0IndTNoOptToMACC:
  case MMPULite::SHU0IndTOptToMACC:
  case MMPULite::SHU1CombToMACC:
  case MMPULite::SHU1IndTBNoOptToMACC:
  case MMPULite::SHU1IndTBOptToMACC:
  case MMPULite::SHU1IndTNoOptToMACC:
  case MMPULite::SHU1IndTOptToMACC:
    if (MI->getOperand(2).isImm())
      Patch &= ~((0x7ULL ^ (1ULL << (MI->getOperand(2).getImm() - 1))) << I0);
  case MMPULite::FALUBinToMACC:
  case MMPULite::FALUDParaToMACC:
  case MMPULite::FALUUryToMACC:
  case MMPULite::FMaCToMACC:
  case MMPULite::FMulToMACC:
  case MMPULite::IALUBinToMACC:
  case MMPULite::IALUDPToMACC:
  case MMPULite::IALUDivRToMACC:
  case MMPULite::IALUImmToMACC:
  case MMPULite::IALUTPToMACC:
  case MMPULite::IALUUryToMACC:
  case MMPULite::IMASToMACC:
  case MMPULite::IMAToMACC:
  case MMPULite::IMAccToMACC:
  case MMPULite::IMaCToMACC:
  case MMPULite::IMulToMACC:
    if (MI->getOperand(0).isReg() && MI->getOperand(1).isReg()) {
      switch (MI->getOperand(0).getReg()) {
      case MMPULiteReg::IALU:
        Patch &= ~((0xFFFFULL ^
                    (1ULL << (MI->getOperand(1).getReg() - MMPULiteReg::T0)))
                   << IALUT0);
        break;
      case MMPULiteReg::IMAC:
        Patch &= ~((0xFFFFULL ^
                    (1ULL << (MI->getOperand(1).getReg() - MMPULiteReg::T0) << 4))
                   << IALUT0);
        break;
      case MMPULiteReg::FALU:
        Patch &= ~((0xFFFFULL ^
                    (1ULL << (MI->getOperand(1).getReg() - MMPULiteReg::T0) << 8))
                   << IALUT0);
        break;
      case MMPULiteReg::FMAC:
        Patch &= ~((0xFFFFULL ^
                    (1ULL << (MI->getOperand(1).getReg() - MMPULiteReg::T0) << 12))
                   << IALUT0);
        break;
      default: break;
      }
    }
    break;
      
  case MMPULite::FALUBinToSHU:
  case MMPULite::FALUDParaToSHU:
  case MMPULite::FALUUryToSHU:
  case MMPULite::FMaCToSHU:
  case MMPULite::FMulToSHU:
  case MMPULite::IALUBinToSHU:
  case MMPULite::IALUDPToSHU:
  case MMPULite::IALUDivRToSHU:
  case MMPULite::IALUImmToSHU:
  case MMPULite::IALUTPToSHU:
  case MMPULite::IALUUryToSHU:
  case MMPULite::IMASToSHU:
  case MMPULite::IMAToSHU:
  case MMPULite::IMAccToSHU:
  case MMPULite::IMaCToSHU:
  case MMPULite::IMulToSHU:
    //Patch &= ~(1ULL << MACCO);
  case MMPULite::BIU0LdToSHU:
  case MMPULite::BIU1LdToSHU:
  case MMPULite::BIU2LdToSHU:
  case MMPULite::MR0ToSHU:
  case MMPULite::MR1ToSHU:
  case MMPULite::SHU0CombToSHU:
  case MMPULite::SHU0IndTBNoOptToSHU:
  case MMPULite::SHU0IndTBOptToSHU:
  case MMPULite::SHU0IndTNoOptToSHU:
  case MMPULite::SHU0IndTOptToSHU:
  case MMPULite::SHU1CombToSHU:
  case MMPULite::SHU1IndTBNoOptToSHU:
  case MMPULite::SHU1IndTBOptToSHU:
  case MMPULite::SHU1IndTNoOptToSHU:
  case MMPULite::SHU1IndTOptToSHU:
    if (MI->getOperand(0).isReg() && MI->getOperand(1).isReg()) {
      switch (MI->getOperand(0).getReg()) {
      case MMPULiteReg::SHU0:
        Patch &= ~((0xFULL ^
                    (1ULL << (MI->getOperand(1).getReg() - MMPULiteReg::T0)))
                   << SHU0T0);
        Patch &= ~(0xFULL << SHU1T0);
        break;
      case MMPULiteReg::SHU1:
        Patch &= ~((0xFULL ^
                    (1ULL << (MI->getOperand(1).getReg() - MMPULiteReg::T0)))
                   << SHU1T0);
        Patch &= ~(0xFULL << SHU0T0);
        break;
      default: break;
      }
    }
    break;

  case MMPULite::FALUBinToBIU:
  case MMPULite::FALUDParaToBIU:
  case MMPULite::FALUUryToBIU:
  case MMPULite::FMaCToBIU:
  case MMPULite::FMulToBIU:
  case MMPULite::IALUBinToBIU:
  case MMPULite::IALUDPToBIU:
  case MMPULite::IALUDivRToBIU:
  case MMPULite::IALUImmToBIU:
  case MMPULite::IALUTPToBIU:
  case MMPULite::IALUUryToBIU:
  case MMPULite::IMASToBIU:
  case MMPULite::IMAToBIU:
  case MMPULite::IMAccToBIU:
  case MMPULite::IMaCToBIU:
  case MMPULite::IMulToBIU:
    //Patch &= ~(1ULL << MACCO);
  case MMPULite::MR3ToBIU:
  case MMPULite::MR3ToBIUKG:
  case MMPULite::SHU0CombToBIU:
  case MMPULite::SHU0IndTBNoOptToBIU:
  case MMPULite::SHU0IndTBOptToBIU:
  case MMPULite::SHU0IndTNoOptToBIU:
  case MMPULite::SHU0IndTOptToBIU:
  case MMPULite::SHU1CombToBIU:
  case MMPULite::SHU1IndTBNoOptToBIU:
  case MMPULite::SHU1IndTBOptToBIU:
  case MMPULite::SHU1IndTNoOptToBIU:
  case MMPULite::SHU1IndTOptToBIU:
    if (MI->getOperand(0).isReg())
      Patch &= ~((0x7ULL ^ (1ULL << (MI->getOperand(0).getReg() - MMPULiteReg::BIU0)))
                 << BIU0W);
    break;

  default: return DefUsage;
  }
  return DefUsage & Patch;
}

void MMPULiteScheduler::FixupDoubleRegWB(std::vector<use> &instline, const MCInst *MI,
                                         unsigned begin) {
  const unsigned TF=3, LF=7;
  unsigned linesize = 0;
  const MCOperand *MO = NULL;
  switch (MI->getOpcode()) {
  case MMPULite::IALUUryToBIU:
  case MMPULite::IALUUryToM:
    MO = &MI->getOperand(2);
  case MMPULite::IALUUryToMACC:
    if (!MO) MO = &MI->getOperand(4);
  case MMPULite::IALUUryToSHU:
    if (!MO) MO = &MI->getOperand(3);
    if (MO->getReg() == MMPULiteReg::EXPD)
      break;
    else
      return;

  case MMPULite::IMAToBIU:
  case MMPULite::IMAToM:
  case MMPULite::IMAToMACC:
  case MMPULite::IMAToSHU:
  case MMPULite::IMAccToBIU:
  case MMPULite::IMAccToM:
  case MMPULite::IMAccToMACC:
  case MMPULite::IMAccToSHU:
  case MMPULite::IMaCToBIU:
  case MMPULite::IMaCToM:
  case MMPULite::IMaCToMACC:
  case MMPULite::IMaCToSHU:
    // The second last operand is flag
    if (MI->getOperand(MI->getNumOperands() - 1).isInst())
      MO = &MI->getOperand(MI->getNumOperands() - 2);
    else
      MO = &MI->getOperand(MI->getNumOperands() - 1);
    if (!((MO->getImm() >> 8) & (1ULL << LF)))
      return;
    break;
  case MMPULite::IMulToBIU:
  case MMPULite::IMulToM:
  case MMPULite::IMulToMACC:
  case MMPULite::IMulToSHU:
    if (MI->getOperand(MI->getNumOperands() - 1).isInst())
      MO = &MI->getOperand(MI->getNumOperands() - 2);
    else
      MO = &MI->getOperand(MI->getNumOperands() - 1);
    if ((MO->getImm() >> 8) & (1ULL << TF))
      return;
    break;
  case MMPULite::IALUDivRToBIU:
  case MMPULite::IALUDivRToM:
  case MMPULite::IALUDivRToMACC:
  case MMPULite::IALUDivRToSHU:
    break;

  case MMPULite::MR2ToMACC:
    if (MI->getOperand(MI->getNumOperands() - 1).isInst())
      MO = &MI->getOperand(MI->getNumOperands() - 2);
    else
      MO = &MI->getOperand(MI->getNumOperands() - 1);
    if (MO->getReg() == MMPULiteReg::WF0)
      return;
    break;
  default:
    return;
  }
  switch (MI->getOpcode()) {
  case MMPULite::IALUUryToBIU:
  case MMPULite::IALUDivRToBIU:
  case MMPULite::IMAToBIU:
  case MMPULite::IMAccToBIU:
  case MMPULite::IMaCToBIU:
  case MMPULite::IMulToBIU:
    linesize = instline.size();
    for (unsigned i = begin; i < linesize; i++) {
      switch (instline[i].ResID) {
      case IALUW:
      case IMACW:
      case MACCO:
      case BIU0W:
      case BIU1W:
      case BIU2W:
        instline[i].Period++;
        break;
      default: break;
      }
    }
    break;

  case MMPULite::IALUUryToM:
  case MMPULite::IALUDivRToM:
  case MMPULite::IMAToM:
  case MMPULite::IMAccToM:
  case MMPULite::IMaCToM:
  case MMPULite::IMulToM:
    linesize = instline.size();
    for (unsigned i = begin; i < linesize; i++) {
      switch (instline[i].ResID) {
      case IALUW:
      case IMACW:
      case MACCO:
        instline[i].Period++;
        break;
      case MW3:
        instline.push_back(*(new use));
        instline.back() = instline[i];
        instline.back().MID++;
        break;
      default: break;
      }
    }
    break;

  case MMPULite::IALUUryToMACC:
  case MMPULite::IALUDivRToMACC:
  case MMPULite::IMAToMACC:
  case MMPULite::IMAccToMACC:
  case MMPULite::IMaCToMACC:
  case MMPULite::IMulToMACC:
    linesize = instline.size();
    for (unsigned i = begin; i < linesize; i++) {
      if (instline[i].ResID == IMACW || instline[i].ResID == IALUW)
        instline[i].Period++;
      else if (instline[i].ResID >= IALUT0 && instline[i].ResID <= FMACT3) {
        instline.push_back(*(new use));
        instline.back() = instline[i];
        if (instline[i].ResID != IALUT3 &&
            instline[i].ResID != IMACT3 &&
            instline[i].ResID != FALUT3 &&
            instline[i].ResID != FMACT3)
          instline.back().ResID++;
        else
          instline.back().ResID -= 3;
        instline.back().Delay++;
      }
    }
    break;

  case MMPULite::IALUUryToSHU:
  case MMPULite::IALUDivRToSHU:
  case MMPULite::IMAToSHU:
  case MMPULite::IMAccToSHU:
  case MMPULite::IMaCToSHU:
  case MMPULite::IMulToSHU:
    linesize = instline.size();
    for (unsigned i = begin; i < linesize; i++) {
      if (instline[i].ResID == IMACW || instline[i].ResID == IALUW)
        instline[i].Period++;
      if (instline[i].ResID == MACCO)
        instline[i].Period++;
      else if (instline[i].ResID >= SHU0T0 && instline[i].ResID <= SHU0T3) {
        instline.push_back(*(new use));
        instline.back() = instline[i];
        if (instline[i].ResID != SHU0T3 &&
            instline[i].ResID != SHU1T3)
          instline.back().ResID++;
        else
          instline.back().ResID -= 3;
        instline.back().Delay++;
      }
    }
    break;

  case MMPULite::MR2ToMACC:
    linesize = instline.size();
    for (unsigned i = begin; i < linesize; i++)
      instline[i].Delay++;
    break;

  default:
    break;
  }
}

uint64_t MMPULiteScheduler::getLatency(unsigned Opc) {
  uint64_t usage = ResUsageMap[Opc - MMPULite::BIU0KG];
  unsigned index = 0;
  ResouceTy id = Invalid;
  if (usage & (1 << MW3))
    id = MW3;
  else if (usage & (1 << MW0))
    id = MW0;
  else if (usage & (1 << MW1))
    id = MW1;
  else if (usage & (1 << MW2))
    id = MW2;
  else if (usage & R_MACCT)
    id = IALUT0;
  else if (usage & R_SHUT)
    id = SHU0T0;
  else if (usage & R_BIUW)
    id = BIU0W;
  else if (usage & (1 << IMRW))
    id = IMRW;
  else
    return 0;
  for (unsigned i = 0; i < id; i++)
    if ((1 << i) & usage) index++;
  return ResLatMap[Opc - MMPULite::BIU0KG][index * 2];
}
  
uint64_t MMPULiteScheduler::getLatency(unsigned Opc, ResouceTy Res) {
  uint64_t usage = ResUsageMap[Opc - MMPULite::BIU0KG];
  unsigned index = 0;
  for (unsigned i = 0; i < Res; i++)
    if ((1 << i) & usage) index++;
  return ResLatMap[Opc - MMPULite::BIU0KG][index * 2];
}
  
const std::string MMPULiteScheduler::ResName[MMPULiteScheduler::NumResouces] = {
  "Invalid resource",
  "M regsiter file W0 port",
  "M regsiter file W1 port",
  "M regsiter file W2 port",
  "M regsiter file W3 port",
  "IALU output port",
  "IMAC output port",
  "IMAC MR register write port",
  "FALU output port",
  "FMAC output port",
  "FMAC MR register write port",
  "FMAC EX3 pipeline stage",
  "MACC input path 0",
  "MACC input path 1",
  "MACC input path 2",
  "MACC output port",
  "IALU T0",
  "IALU T1",
  "IALU T2",
  "IALU T3",
  "IMAC T0",
  "IMAC T1",
  "IMAC T2",
  "IMAC T3",
  "FALU T0",
  "FALU T1",
  "FALU T2",
  "FALU T3",
  "FMAC T0",
  "FMAC T1",
  "FMAC T2",
  "FMAC T3",
  "SHU0 T0",
  "SHU0 T1",
  "SHU0 T2",
  "SHU0 T3",
  "SHU1 T0",
  "SHU1 T1",
  "SHU1 T2",
  "SHU1 T3",
  "BIU0 read port",
  "BIU1 read port",
  "BIU2 read port",
  "BIU0 write port",
  "BIU1 write port",
  "BIU2 write port",
};

const uint64_t MMPULiteScheduler::ResUsageMap[INSTRUCTION_LIST_LEN] = {
  1ULL << BIU0R | 1ULL << MW0,     /// BIU0KG
  1ULL << BIU0R | 1ULL << MW0,     /// BIU0LdToM
  1ULL << BIU0R | R_MACCT | R_IPATH,   /// BIU0LdToMACC
  1ULL << BIU0R | R_SHUT,   /// BIU0LdToSHU
  0,                               /// BIU0St
  1ULL << BIU1R | 1ULL << MW1,     /// BIU1KG
  1ULL << BIU1R | 1ULL << MW1,     /// BIU1LdToM
  1ULL << BIU1R | R_MACCT | R_IPATH,   /// BIU1LdToMACC
  1ULL << BIU1R | R_SHUT,   /// BIU1LdToSHU
  0,                               /// BIU1St
  1ULL << BIU2R | 1ULL << MW2,     /// BIU2KG
  1ULL << BIU2R | 1ULL << MW2,     /// BIU2LdToM
  1ULL << BIU2R | R_MACCT | R_IPATH,   /// BIU2LdToMACC
  1ULL << BIU2R | R_SHUT,   /// BIU2LdToSHU
  0,                               /// BIU2St
  1ULL << FALUW | R_BIUW | 1ULL << MACCO,     /// FALUBinToBIU
  1ULL << FALUW | 1ULL << MW3 | 1ULL << MACCO,      /// FALUBinToM
  1ULL << FALUW | R_MACCT,    /// FALUBinToMACC
  1ULL << FALUW | R_SHUT | 1ULL << MACCO,    /// FALUBinToSHU
  1ULL << FALUW | R_BIUW | 1ULL << MACCO,     /// FALUDParaToBIU
  1ULL << FALUW | 1ULL << MW3 | 1ULL << MACCO,      /// FALUDParaToM
  1ULL << FALUW | R_MACCT,    /// FALUDParaToMACC
  1ULL << FALUW | R_SHUT | 1ULL << MACCO,    /// FALUDParaToSHU
  0,                               /// FALUStart
  0,                               /// FALUStop
  1ULL << FALUW | R_BIUW | 1ULL << MACCO,     /// FALUUryToBIU
  1ULL << FALUW | 1ULL << MW3 | 1ULL << MACCO,      /// FALUUryToM
  1ULL << FALUW | R_MACCT,    /// FALUUryToMACC
  1ULL << FALUW | R_SHUT | 1ULL << MACCO,    /// FALUUryToSHU
  1ULL << Invalid,                 /// FMA
  0,                               /// FMACStart
  0,                               /// FMACStop
  1ULL << FMRW | 1ULL << FMACEX3,  /// FMaC
  1ULL << FMACW | 1ULL << FMRW | 1ULL << FMACEX3 | R_BIUW | 1ULL << MACCO, /// FMaCToBIU
  1ULL << FMACW | 1ULL << FMRW | 1ULL << FMACEX3 | 1ULL << MW3 | 1ULL << MACCO, /// FMaCToM
  1ULL << FMACW | 1ULL << FMRW | 1ULL << FMACEX3 | R_MACCT,    /// FMaCToMACC
  1ULL << FMACW | 1ULL << FMRW | 1ULL << FMACEX3 | R_SHUT | 1ULL << MACCO,    /// FMaCToSHU
  1ULL << FMACW | 1ULL << FMACEX3 | R_BIUW | 1ULL << MACCO,     /// FMulToBIU
  1ULL << FMACW | 1ULL << FMACEX3 | 1ULL << MW3 | 1ULL << MACCO,      /// FMulToM
  1ULL << FMACW | 1ULL << FMACEX3 | R_MACCT,    /// FMulToMACC
  1ULL << FMACW | 1ULL << FMACEX3 | R_SHUT | 1ULL << MACCO,    /// FMulToSHU
  1ULL << IALUW | R_BIUW | 1ULL << MACCO,     /// IALUBinToBIU
  1ULL << IALUW | 1ULL << MW3 | 1ULL << MACCO,      /// IALUBinToM
  1ULL << IALUW | R_MACCT,    /// IALUBinToMACC
  1ULL << IALUW | R_SHUT | 1ULL << MACCO,    /// IALUBinToSHU
  1ULL << IALUW | R_BIUW | 1ULL << MACCO,     /// IALUDPToBIU
  1ULL << IALUW | 1ULL << MW3 | 1ULL << MACCO,      /// IALUDPToM
  1ULL << IALUW | R_MACCT,    /// IALUDPToMACC
  1ULL << IALUW | R_SHUT | 1ULL << MACCO,    /// IALUDPToSHU
  0,                               /// IALUDivR
  1ULL << IALUW | R_BIUW | 1ULL << MACCO,     /// IALUDivRToBIU
  1ULL << IALUW | 1ULL << MW3 | 1ULL << MACCO,      /// IALUDivRToM
  1ULL << IALUW | R_MACCT,    /// IALUDivRToMACC
  1ULL << IALUW | R_SHUT | 1ULL << MACCO,    /// IALUDivRToSHU
  0,                               /// IALUDivS
  1ULL << IALUW | R_BIUW | 1ULL << MACCO,     /// IALUImmToBIU
  1ULL << IALUW | 1ULL << MW3 | 1ULL << MACCO,      /// IALUImmToM
  1ULL << IALUW | R_MACCT,    /// IALUImmToMACC
  1ULL << IALUW | R_SHUT | 1ULL << MACCO,    /// IALUImmToSHU
  0,                               /// IALUStart
  0,                               /// IALUStop
  1ULL << IALUW | R_BIUW | 1ULL << MACCO,     /// IALUTPToBIU
  1ULL << IALUW | 1ULL << MW3 | 1ULL << MACCO,      /// IALUTPToM
  1ULL << IALUW | R_MACCT,    /// IALUTPToMACC
  1ULL << IALUW | R_SHUT | 1ULL << MACCO,    /// IALUTPToSHU
  1ULL << IALUW,                   /// IALUToCond
  1ULL << IALUW | R_BIUW | 1ULL << MACCO,     /// IALUUryToBIU
  1ULL << IALUW | 1ULL << MW3 | 1ULL << MACCO,      /// IALUUryToM
  1ULL << IALUW | R_MACCT,    /// IALUUryToMACC
  1ULL << IALUW | R_SHUT | 1ULL << MACCO,    /// IALUUryToSHU
  1ULL << IMRW,                    /// IMA
  0,                               /// IMACStart
  0,                               /// IMACStop
  1ULL << IMACW | R_BIUW | 1ULL << MACCO,     /// IMASToBIU
  1ULL << IMACW | 1ULL << MW3 | 1ULL << MACCO,      /// IMASToM
  1ULL << IMACW | R_MACCT,    /// IMASToMACC
  1ULL << IMACW | R_SHUT | 1ULL << MACCO,    /// IMASToSHU
  1ULL << IMACW | R_BIUW | 1ULL << MACCO,     /// IMAToBIU
  1ULL << IMACW | 1ULL << MW3 | 1ULL << MACCO,      /// IMAToM
  1ULL << IMACW | R_MACCT,    /// IMAToMACC
  1ULL << IMACW | R_SHUT | 1ULL << MACCO,    /// IMAToSHU
  1ULL << IMRW,                    /// IMAcc
  1ULL << IMACW | 1ULL << IMRW | R_BIUW | 1ULL << MACCO,     /// IMAccToBIU
  1ULL << IMACW | 1ULL << IMRW | 1ULL << MW3 | 1ULL << MACCO,      /// IMAccToM
  1ULL << IMACW | 1ULL << IMRW | R_MACCT,    /// IMAccToMACC
  1ULL << IMACW | 1ULL << IMRW | R_SHUT | 1ULL << MACCO,    /// IMAccToSHU
  1ULL << IMACW | R_BIUW | 1ULL << MACCO,                 /// IMRToBIU
  1ULL << IMACW | 1ULL << MW3 | 1ULL << MACCO,                 /// IMRToM
  1ULL << IMACW | R_MACCT,                 /// IMRToMACC
  1ULL << IMACW | R_SHUT | 1ULL << MACCO,                 /// IMRToSHU
  1ULL << IMRW,                    /// IMaC
  1ULL << IMACW | 1ULL << IMRW | R_BIUW | 1ULL << MACCO,     /// IMaCToBIU
  1ULL << IMACW | 1ULL << IMRW | 1ULL << MW3 | 1ULL << MACCO,      /// IMaCToM
  1ULL << IMACW | 1ULL << IMRW | R_MACCT,    /// IMaCToMACC
  1ULL << IMACW | 1ULL << IMRW | R_SHUT | 1ULL << MACCO,    /// IMaCToSHU
  1ULL << IMACW | R_BIUW | 1ULL << MACCO,     /// IMulToBIU
  1ULL << IMACW | 1ULL << MW3 | 1ULL << MACCO,      /// IMulToM
  1ULL << IMACW | R_MACCT,    /// IMulToMACC
  1ULL << IMACW | R_SHUT | 1ULL << MACCO,    /// IMulToSHU
  1ULL << Invalid,                 /// JUMP
  1ULL << Invalid,                 /// JUMPK
  0,                               /// LPTO
  1ULL << Invalid,                 /// LPTOImm
  0,                               /// MPUStop
  1ULL << MW3,                     /// MR0ToM
  R_MACCT | R_IPATH,               /// MR0ToMACC
  R_SHUT,                          /// MR0ToSHU
  R_MACCT | R_IPATH,               /// MR1ToMACC
  R_SHUT,                          /// MR1ToSHU
  R_MACCT | R_IPATH,               /// MR2ToMACC
  R_BIUW,                          /// MR3ToBIU
  R_BIUW,                          /// MR3ToBIUKG
  0,                               /// NOP
  0,                               /// REPEATIMM
  0,                               /// REPEATK
  R_BIUW,                          /// SHU0CombToBIU
  1ULL << MW3,                     /// SHU0CombToM
  R_MACCT | R_IPATH,               /// SHU0CombToMACC
  R_SHUT,                          /// SHU0CombToSHU
  R_BIUW,                          /// SHU0IndTBNoOptToBIU
  1ULL << MW3,                     /// SHU0IndTBNoOptToM
  R_MACCT | R_IPATH,               /// SHU0IndTBNoOptToMACC
  R_SHUT,                          /// SHU0IndTBNoOptToSHU
  R_BIUW,                          /// SHU0IndTBOptToBIU
  1ULL << MW3,                     /// SHU0IndTBOptToM
  R_MACCT | R_IPATH,               /// SHU0IndTBOptToMACC
  R_SHUT,                          /// SHU0IndTBOptToSHU
  R_BIUW,                          /// SHU0IndTNoOptToBIU
  1ULL << MW3,                     /// SHU0IndTNoOptToM
  R_MACCT | R_IPATH,               /// SHU0IndTNoOptToMACC
  R_SHUT,                          /// SHU0IndTNoOptToSHU
  R_BIUW,                          /// SHU0IndTOptToBIU
  1ULL << MW3,                     /// SHU0IndTOptToM
  R_MACCT | R_IPATH,               /// SHU0IndTOptToMACC
  R_SHUT,                          /// SHU0IndTOptToSHU
  0,                               /// SHU0Start
  0,                               /// SHU0Stop
  R_BIUW,                          /// SHU1CombToBIU
  1ULL << MW3,                     /// SHU1CombToM
  R_MACCT | R_IPATH,               /// SHU1CombToMACC
  R_SHUT,                          /// SHU1CombToSHU
  R_BIUW,                          /// SHU1IndTBNoOptToBIU
  1ULL << MW3,                     /// SHU1IndTBNoOptToM
  R_MACCT | R_IPATH,               /// SHU1IndTBNoOptToMACC
  R_SHUT,                          /// SHU1IndTBNoOptToSHU
  R_BIUW,                          /// SHU1IndTBOptToBIU
  1ULL << MW3,                     /// SHU1IndTBOptToM
  R_MACCT | R_IPATH,               /// SHU1IndTBOptToMACC
  R_SHUT,                          /// SHU1IndTBOptToSHU
  R_BIUW,                          /// SHU1IndTNoOptToBIU
  1ULL << MW3,                     /// SHU1IndTNoOptToM
  R_MACCT | R_IPATH,               /// SHU1IndTNoOptToMACC
  R_SHUT,                          /// SHU1IndTNoOptToSHU
  R_BIUW,                          /// SHU1IndTOptToBIU
  1ULL << MW3,                     /// SHU1IndTOptToM
  R_MACCT | R_IPATH,               /// SHU1IndTOptToMACC
  R_SHUT,                          /// SHU1IndTOptToSHU
  0,                               /// SHU1Start
  0                                /// SHU1Stop
};

const uint64_t MMPULiteScheduler::ResLatMap[INSTRUCTION_LIST_LEN][NumResouces * 2] = {
  {4, 1/*MW0*/, 1, 1/*BIU0R*/}, /// BIU0KG
  {12, 1/*MW0*/, 7, 1/*BIU0R*/}, /// BIU0LdToM
  {11, 1, 11, 1, 11, 1,/*IPATH*/ 11, 1, 11, 1, 11, 1, 11, 1,/*IALUT*/
   11, 1, 11, 1, 11, 1, 11, 1,/*IMACT*/ 11, 1, 11, 1, 11, 1, 11, 1,/*FALUT*/
   11, 1, 11, 1, 11, 1, 11, 1,/*FMACT*/ 7, 1/*BIU0R*/}, /// BIU0LdToMACC
  {11, 1, 11, 1, 11, 1, 11, 1,/*SHU0T*/ 11, 1, 11, 1, 11, 1, 11, 1,/*SHU1T*/
   7, 1/*BIU0R*/}, /// BIU0LdToSHU
  {},                           /// BIU0St
  {4, 1/*MW1*/, 1, 1/*BIU1R*/}, /// BIU1KG
  {12, 1/*MW1*/, 7, 1/*BIU1R*/}, /// BIU1LdToM
  {11, 1, 11, 1, 11, 1,/*IPATH*/ 11, 1, 11, 1, 11, 1, 11, 1,/*IALUT*/
   11, 1, 11, 1, 11, 1, 11, 1,/*IMACT*/ 11, 1, 11, 1, 11, 1, 11, 1,/*FALUT*/
   11, 1, 11, 1, 11, 1, 11, 1,/*FMACT*/ 7, 1/*BIU1R*/}, /// BIU1LdToMACC
  {11, 1, 11, 1, 11, 1, 11, 1,/*SHU0T*/ 11, 1, 11, 1, 11, 1, 8, 1,/*SHU1T*/
   7, 1/*BIU1R*/}, /// BIU1LdToSHU
  {},                           /// BIU1St
  {4, 1/*MW2*/, 1, 1/*BIU2R*/ }, /// BIU2KG
  {12, 1/*MW2*/, 7, 1/*BIU2R*/}, /// BIU2LdToM
  {11, 1, 11, 1, 11, 1,/*IPATH*/ 11, 1, 11, 1, 11, 1, 11, 1,/*IALUT*/
   11, 1, 11, 1, 11, 1, 11, 1,/*IMACT*/ 11, 1, 11, 1, 11, 1, 11, 1,/*FALUT*/
   11, 1, 11, 1, 11, 1, 11, 1,/*FMACT*/ 7, 1/*BIU2R*/}, /// BIU2LdToMACC
  {11, 1, 11, 1, 11, 1, 11, 1,/*SHU0T*/ 11, 1, 11, 1, 11, 1, 11, 1,/*SHU1T*/
   7, 1/*BIU2R*/}, /// BIU2LdToSHU
  {},                           /// BIU2St
  {5, 1/*FALUW*/, 5, 1/*MACCO*/, 6, 1/*BIU0W*/, 6, 1/*BIU1W*/, 6, 1/*BIU2W*/}, /// FALUBinToBIU
  {6, 1/*MW3*/, 5, 1/*FALUW*/, 5, 1/*MACCO*/}, /// FALUBinToM
  {5, 1/*FALUW*/, 5, 1, 5, 1, 5, 1, 5, 1,/*IALUT*/
   5, 1, 5, 1, 5, 1, 5, 1,/*IMACT*/ 5, 1, 5, 1, 5, 1, 5, 1,/*FALUT*/
   5, 1, 5, 1, 5, 1, 5, 1,/*FMACT*/},    /// FALUBinToMACC
  {5, 1/*FALUW*/, 5, 1/*MACCO*/, 6, 1, 6, 1, 6, 1, 6, 1,/*SHU0T*/
   6, 1, 6, 1, 6, 1, 6, 1/*SHU1T*/},    /// FALUBinToSHU
  {5, 1/*FALUW*/, 5, 1/*MACCO*/, 6, 1/*BIU0W*/, 6, 1/*BIU1W*/, 6, 1/*BIU2W*/}, /// FALUDParaToBIU
  {6, 1/*MW3*/, 5, 1/*FALUW*/, 5, 1/*MACCO*/}, /// FALUDParaToM
  {5, 1/*FALUW*/, 5, 1, 5, 1, 5, 1, 5, 1,/*IALUT*/
   5, 1, 5, 1, 5, 1, 5, 1,/*IMACT*/ 5, 1, 5, 1, 5, 1, 5, 1,/*FALUT*/
   5, 1, 5, 1, 5, 1, 5, 1,/*FMACT*/}, /// FALUDParaToMACC
  {5, 1/*FALUW*/, 5, 1/*MACCO*/, 6, 1, 6, 1, 6, 1, 6, 1,/*SHU0T*/
   6, 1, 6, 1, 6, 1, 6, 1/*SHU1T*/}, /// FALUDParaToSHU
  {},                           /// FALUStart
  {},                           /// FALUStop
  {5, 1/*FALUW*/, 5, 1/*MACCO*/, 6, 1/*BIU0W*/, 6, 1/*BIU1W*/, 6, 1/*BIU2W*/}, /// FALUUryToBIU
  {6, 1/*MW3*/, 5, 1/*FALUW*/, 5, 1/*MACCO*/}, /// FALUUryToM
  {5, 1/*FALUW*/, 5, 1, 5, 1, 5, 1, 5, 1,/*IALUT*/
   5, 1, 5, 1, 5, 1, 5, 1,/*IMACT*/ 5, 1, 5, 1, 5, 1, 5, 1,/*FALUT*/
   5, 1, 5, 1, 5, 1, 5, 1,/*FMACT*/}, /// FALUUryToMACC
  {5, 1/*FALUW*/, 5, 1/*MACCO*/, 6, 1, 6, 1, 6, 1, 6, 1,/*SHU0T*/
   6, 1, 6, 1, 6, 1, 6, 1/*SHU1T*/}, /// FALUUryToSHU
  {8, 1/*FMRW*/},               /// FMA
  {},                           /// FMACStart
  {},                           /// FMACStop
  {8, 1/*FMRW*/, 3, 3/*FMACEX3*/}, /// FMaC
  {9, 1/*FMACW*/, 8, 1/*FMRW*/, 3, 3/*FMACEX3*/, 9, 1/*MACCO*/,
   10, 1/*BIU0W*/, 10, 1/*BIU1W*/, 10, 1/*BIU2W*/}, /// FMaCToBIU
  {10, 1/*MW3*/, 9, 1/*FMACW*/, 8, 1/*FMRW*/, 3, 3/*FMACEX3*/, 9, 1/*MACCO*/}, /// FMaCToM
  {9, 1/*FMACW*/, 8, 1/*FMRW*/, 3, 3/*FMACEX3*/,
   9, 1, 9, 1, 9, 1, 9, 1,/*IALUT*/ 9, 1, 9, 1, 9, 1, 9, 1,/*IMACT*/
   9, 1, 9, 1, 9, 1, 9, 1,/*FALUT*/ 9, 1, 9, 1, 9, 1, 9, 1,/*FMACT*/}, /// FMaCToMACC
  {9, 1/*FMACW*/, 8, 1/*FMRW*/, 3, 3/*FMACEX3*/, 9, 1/*MACCO*/,
   10, 1, 10, 1, 10, 1, 10, 1,/*SHU0T*/ 10, 1, 10, 1, 10, 1, 10, 1/*SHU1T*/}, /// FMaCToSHU
  {6, 1/*FMACW*/, 1, 1/*FMACEX3*/, 6, 1/*MACCO*/,
   7, 1/*BIU0W*/, 7, 1/*BIU1W*/, 7, 1/*BIU2W*/}, /// FMulToBIU
  {7, 1/*MW3*/, 6, 1/*FMACW*/, 1, 1/*FMACEX3*/, 6, 1/*MACCO*/}, /// FMulToM
  {6, 1/*FMACW*/, 1, 1/*FMACEX3*/, 6, 1, 6, 1, 6, 1, 6, 1,/*IALUT*/
   6, 1, 6, 1, 6, 1, 6, 1,/*IMACT*/ 6, 1, 6, 1, 6, 1, 6, 1,/*FALUT*/
   6, 1, 6, 1, 6, 1, 6, 1,/*FMACT*/}, /// FMulToMACC
  {6, 1/*FMACW*/, 1, 1/*FMACEX3*/, 6, 1/*MACCO*/,
   7, 1, 7, 1, 7, 1, 7, 1,/*SHU0T*/ 7, 1, 7, 1, 7, 1, 7, 1/*SHU1T*/}, /// FMulToSHU
  {3, 1/*IALUW*/, 3, 1/*MACCO*/, 4, 1/*BIU0W*/, 4, 1/*BIU1W*/, 4, 1/*BIU2W*/}, /// IALUBinToBIU
  {4, 1/*MW3*/, 3, 1/*IALUW*/, 3, 1/*MACCO*/}, /// IALUBinToM
  {3, 1/*IALUW*/, 3, 1, 3, 1, 3, 1, 3, 1,/*IALUT*/
   3, 1, 3, 1, 3, 1, 3, 1,/*IMACT*/ 3, 1, 3, 1, 3, 1, 3, 1,/*FALUT*/
   3, 1, 3, 1, 3, 1, 3, 1,/*FMACT*/}, /// IALUBinToMACC
  {3, 1/*IALUW*/, 3, 1/*MACCO*/, 4, 1, 4, 1, 4, 1, 4, 1,/*SHU0T*/
   4, 1, 4, 1, 4, 1, 4, 1/*SHU1T*/}, /// IALUBinToSHU
  {3, 1/*IALUW*/, 3, 1/*MACCO*/, 4, 1/*BIU0W*/, 4, 1/*BIU1W*/, 4, 1/*BIU2W*/}, /// IALUDPToBIU
  {4, 1/*MW3*/, 3, 1/*IALUW*/, 3, 1/*MACCO*/}, /// IALUDPToM
  {3, 1/*IALUW*/, 3, 1, 3, 1, 3, 1, 3, 1,/*IALUT*/
   3, 1, 3, 1, 3, 1, 3, 1,/*IMACT*/ 3, 1, 3, 1, 3, 1, 3, 1,/*FALUT*/
   3, 1, 3, 1, 3, 1, 3, 1,/*FMACT*/}, /// IALUDPToMACC
  {3, 1/*IALUW*/, 3, 1/*MACCO*/, 4, 1, 4, 1, 4, 1, 4, 1,/*SHU0T*/
   4, 1, 4, 1, 4, 1, 4, 1/*SHU1T*/}, /// IALUDPToSHU
  {},                           /// IALUDivR
  {2, 1/*IALUW*/, 2, 1/*MACCO*/, 3, 1/*BIU0W*/, 3, 1/*BIU1W*/, 3, 1/*BIU2W*/}, /// IALUDivRToBIU
  {4, 1/*MW3*/, 2, 1/*IALUW*/, 2, 1/*MACCO*/}, /// IALUDivRToM
  {2, 1/*IALUW*/, 3, 1, 3, 1, 3, 1, 3, 1,/*IALUT*/
   3, 1, 3, 1, 3, 1, 3, 1,/*IMACT*/ 3, 1, 3, 1, 3, 1, 3, 1,/*FALUT*/
   3, 1, 3, 1, 3, 1, 3, 1,/*FMACT*/}, /// IALUDivRToMACC
  {2, 1/*IALUW*/, 2, 1/*MACCO*/, 3, 1, 3, 1, 3, 1, 3, 1,/*SHU0T*/
   3, 1, 3, 1, 3, 1, 3, 1/*SHU1T*/}, /// IALUDivRToSHU
  {},                           /// IALUDivS
  {3, 1/*IALUW*/, 3, 1/*MACCO*/, 4, 1/*BIU0W*/, 4, 1/*BIU1W*/, 4, 1/*BIU2W*/}, /// IALUImmToBIU
  {4, 1/*MW3*/, 3, 1/*IALUW*/, 3, 1/*MACCO*/},      /// IALUImmToM
  {3, 1/*IALUW*/, 3, 1, 3, 1, 3, 1, 3, 1,/*IALUT*/
   3, 1, 3, 1, 3, 1, 3, 1,/*IMACT*/ 3, 1, 3, 1, 3, 1, 3, 1,/*FALUT*/
   3, 1, 3, 1, 3, 1, 3, 1,/*FMACT*/},    /// IALUImmToMACC
  {3, 1/*IALUW*/, 3, 1/*MACCO*/, 4, 1, 4, 1, 4, 1, 4, 1,/*SHU0T*/
   4, 1, 4, 1, 4, 1, 4, 1/*SHU1T*/},    /// IALUImmToSHU
  {},                           /// IALUStart
  {},                           /// IALUStop
  {3, 1/*IALUW*/, 3, 1/*MACCO*/, 4, 1/*BIU0W*/, 4, 1/*BIU1W*/, 4, 1/*BIU2W*/}, /// IALUTPToBIU
  {4, 1/*MW3*/, 3, 1/*IALUW*/, 3, 1/*MACCO*/}, /// IALUTPToM
  {3, 1/*IALUW*/, 3, 1, 3, 1, 3, 1, 3, 1,/*IALUT*/
   3, 1, 3, 1, 3, 1, 3, 1,/*IMACT*/ 3, 1, 3, 1, 3, 1, 3, 1,/*FALUT*/
   3, 1, 3, 1, 3, 1, 3, 1,/*FMACT*/}, /// IALUTPToMACC
  {3, 1/*IALUW*/, 3, 1/*MACCO*/, 4, 1, 4, 1, 4, 1, 4, 1,/*SHU0T*/
   4, 1, 4, 1, 4, 1, 4, 1/*SHU1T*/}, /// IALUTPToSHU
  {4, 1/*IALUW*/},              /// IALUToCond
  {3, 1/*IALUW*/, 3, 1/*MACCO*/, 4, 1/*BIU0W*/, 4, 1/*BIU1W*/, 4, 1/*BIU2W*/}, /// IALUUryToBIU
  {4, 1/*MW3*/, 3, 1/*IALUW*/, 3, 1/*MACCO*/}, /// IALUUryToM
  {3, 1/*IALUW*/, 3, 1, 3, 1, 3, 1, 3, 1,/*IALUT*/
   3, 1, 3, 1, 3, 1, 3, 1,/*IMACT*/ 3, 1, 3, 1, 3, 1, 3, 1,/*FALUT*/
   3, 1, 3, 1, 3, 1, 3, 1,/*FMACT*/},    /// IALUUryToMACC
  {3, 1/*IALUW*/, 3, 1/*MACCO*/, 4, 1, 4, 1, 4, 1, 4, 1,/*SHU0T*/
   4, 1, 4, 1, 4, 1, 4, 1/*SHU1T*/},    /// IALUUryToSHU
  {6, 1/*IMRW*/},               /// IMA
  {},                           /// IMACStart
  {},                           /// IMACStop
  {7, 1/*IMACW*/, 7, 1/*MACCO*/, 8, 1/*BIU0W*/, 8, 1/*BIU1W*/, 8, 1/*BIU2W*/}, /// IMASToBIU
  {8, 1/*MW3*/, 7, 1/*IMACW*/, 7, 1/*MACCO*/}, /// IMASToM
  {7, 1/*IMACW*/, 7, 1, 7, 1, 7, 1, 7, 1,/*IALUT*/
   7, 1, 7, 1, 7, 1, 7, 1,/*IMACT*/ 7, 1, 7, 1, 7, 1, 7, 1,/*FALUT*/
   7, 1, 7, 1, 7, 1, 7, 1,/*FMACT*/}, /// IMASToMACC
  {7, 1/*IMACW*/, 7, 1/*MACCO*/, 8, 1, 8, 1, 8, 1, 8, 1,/*SHU0T*/
   8, 1, 8, 1, 8, 1, 8, 1/*SHU1T*/}, /// IMASToSHU
  {7, 1/*IMACW*/, 7, 1/*MACCO*/, 7, 1/*BIU0W*/, 7, 1/*BIU1W*/, 7, 1/*BIU2W*/}, /// IMAToBIU //Yanglei:unchanged
  {8, 1/*MW3*/, 7, 1/*IMACW*/, 7, 1/*MACCO*/}, /// IMAToM //Yanglei:unchanged
  {7, 1/*IMACW*/, 7, 1, 7, 1, 7, 1, 7, 1,/*IALUT*/
   7, 1, 7, 1, 7, 1, 7, 1,/*IMACT*/ 7, 1, 7, 1, 7, 1, 7, 1,/*FALUT*/
   7, 1, 7, 1, 7, 1, 7, 1,/*FMACT*/}, /// IMAToMACC //Yanglei:unchanged
  {7, 1/*IMACW*/, 7, 1/*MACCO*/, 7, 1, 7, 1, 7, 1, 7, 1,/*SHU0T*/
   7, 1, 7, 1, 7, 1, 7, 1/*SHU1T*/}, /// IMAToSHU //Yanglei:unchanged
  {1, 1/*IMRW*/},               /// IMAcc
  {4, 1/*IMACW*/, 2, 1/*IMRW*/, 4, 1/*MACCO*/, 5, 1/*BIU0W*/, 5, 1/*BIU1W*/, 5, 1/*BIU2W*/}, /// IMAccToBIU
  {5, 1/*MW3*/, 4, 1/*IMACW*/, 2, 1/*IMRW*/, 4, 1/*MACCO*/}, /// IMAccToM
  {4, 1/*IMACW*/, 2, 1/*IMRW*/, 4, 1, 4, 1, 4, 1, 4, 1,/*IALUT*/
   4, 1, 4, 1, 4, 1, 4, 1,/*IMACT*/ 4, 1, 4, 1, 4, 1, 4, 1,/*FALUT*/
   4, 1, 4, 1, 4, 1, 4, 1,/*FMACT*/}, /// IMAccToMACC
  {4, 1/*IMACW*/, 2, 1/*IMRW*/, 4, 1/*MACCO*/, 5, 1, 5, 1, 5, 1, 5, 1,/*SHU0T*/
   5, 1, 5, 1, 5, 1, 5, 1/*SHU1T*/}, /// IMAccToSHU
  {2, 1/*IMACW*/, 2, 1/*MACCO*/, 2, 1/*BIU0W*/, 2, 1/*BIU1W*/, 2, 1/*BIU2W*/},                           /// IMRToBIU //Yanglei:unchanged
  {3, 1/*MW3*/, 2, 1/*IMACW*/, 2, 1/*MACCO*/},                           /// IMRToM //Yanglei:unchanged
  {2, 1/*IMACW*/, 2, 1, 2, 1, 2, 1, 2, 1,/*IALUT*/
   2, 1, 2, 1, 2, 1, 2, 1,/*IMACT*/ 2, 1, 2, 1, 2, 1, 2, 1,/*FALUT*/
   2, 1, 2, 1, 2, 1, 2, 1,/*FMACT*/},                           /// IMRToMACC //Yanglei:unchanged
  {2, 1/*IMACW*/, 2, 1/*MACCO*/, 2, 1, 2, 1, 2, 1, 2, 1,/*SHU0T*/
   2, 1, 2, 1, 2, 1, 2, 1/*SHU1T*/},                           /// IMRToSHU //Yanglei:unchanged
  {6, 1/*IMRW*/},               /// IMaC
  {8, 1/*IMACW*/, 6, 1/*IMRW*/, 8, 1/*MACCO*/, 9, 1/*BIU0W*/, 9, 1/*BIU1W*/, 9, 1/*BIU2W*/}, /// IMaCToBIU
  {9, 1/*MW3*/, 8, 1/*IMACW*/, 6, 1/*IMRW*/, 8, 1/*MACCO*/}, /// IMaCToM
  {8, 1/*IMACW*/, 6, 1/*IMRW*/, 8, 1, 8, 1, 8, 1, 8, 1,/*IALUT*/
   8, 1, 8, 1, 8, 1, 8, 1,/*IMACT*/ 8, 1, 8, 1, 8, 1, 8, 1,/*FALUT*/
   8, 1, 8, 1, 8, 1, 8, 1,/*FMACT*/}, /// IMaCToMACC
  {8, 1/*IMACW*/, 6, 1/*IMRW*/, 8, 1/*MACCO*/, 9, 1, 9, 1, 9, 1, 9, 1,/*SHU0T*/
   9, 1, 9, 1, 9, 1, 9, 1/*SHU1T*/}, /// IMaCToSHU
  {5, 1/*IMACW*/, 5, 1/*MACCO*/, 6, 1/*BIU0W*/, 6, 1/*BIU1W*/, 6, 1/*BIU2W*/}, /// IMulToBIU
  {6, 1/*MW3*/, 5, 1/*IMACW*/, 5, 1/*MACCO*/}, /// IMulToM
  {5, 1/*IMACW*/, 5, 1, 5, 1, 5, 1, 5, 1,/*IALUT*/
   5, 1, 5, 1, 5, 1, 5, 1,/*IMACT*/ 5, 1, 5, 1, 5, 1, 5, 1,/*FALUT*/
   5, 1, 5, 1, 5, 1, 5, 1,/*FMACT*/},    /// IMulToMACC
  {5, 1/*IMACW*/, 5, 1/*MACCO*/, 6, 1, 6, 1, 6, 1, 6, 1,/*SHU0T*/
   6, 1, 6, 1, 6, 1, 6, 1/*SHU1T*/},    /// IMulToSHU
  {},                           /// JUMP
  {},                           /// JUMPK
  {},                           /// LPTO
  {},                           /// LPTOImm
  {},                           /// MPUStop
  {3, 1/*MW3*/},                /// MR0ToM
  {2, 1, 2, 1, 2, 1,/*IPATH*/ 2, 1, 2, 1, 2, 1, 2, 1,/*IALUT*/
   2, 1, 2, 1, 2, 1, 2, 1,/*IMACT*/ 2, 1, 2, 1, 2, 1, 2, 1,/*FALUT*/
   2, 1, 2, 1, 2, 1, 2, 1,/*FMACT*/}, /// MR0ToMACC
  {3, 1, 3, 1, 3, 1, 3, 1,/*SHU0T*/ 3, 1, 3, 1, 3, 1, 3, 1/*SHU1T*/}, /// MR0ToSHU
  {2, 1, 2, 1, 2, 1,/*IPATH*/ 2, 1, 2, 1, 2, 1, 2, 1,/*IALUT*/
   2, 1, 2, 1, 2, 1, 2, 1,/*IMACT*/ 2, 1, 2, 1, 2, 1, 2, 1,/*FALUT*/
   2, 1, 2, 1, 2, 1, 2, 1,/*FMACT*/}, /// MR1ToMACC
  {3, 1, 3, 1, 3, 1, 3, 1,/*SHU0T*/ 3, 1, 3, 1, 3, 1, 3, 1/*SHU1T*/}, /// MR1ToSHU
  {2, 1, 2, 1, 2, 1,/*IPATH*/ 2, 1, 2, 1, 2, 1, 2, 1,/*IALUT*/
   2, 1, 2, 1, 2, 1, 2, 1,/*IMACT*/ 2, 1, 2, 1, 2, 1, 2, 1,/*FALUT*/
   2, 1, 2, 1, 2, 1, 2, 1,/*FMACT*/}, /// MR2ToMACC
  {3, 1/*BIU0W*/, 3, 1/*BIU1W*/, 3, 1/*BIU2W*/}, /// MR3ToBIU
  {4, 1/*BIU0W*/, 4, 1/*BIU1W*/, 4, 1/*BIU2W*/}, /// MR3ToBIUKG
  {},                            /// NOP
  {},                            /// REPEATIMM
  {},                            /// REPEATK
  {2, 1/*BIU0W*/, 2, 1/*BIU1W*/, 2, 1/*BIU2W*/}, /// SHU0CombToBIU
  {4, 1/*MW3*/},                 /// SHU0CombToM
  {3, 1, 3, 1, 3, 1,/*IPATH*/ 3, 1, 3, 1, 3, 1, 3, 1,/*IALUT*/
   3, 1, 3, 1, 3, 1, 3, 1,/*IMACT*/ 3, 1, 3, 1, 3, 1, 3, 1,/*FALUT*/
   3, 1, 3, 1, 3, 1, 3, 1,/*FMACT*/}, /// SHU0CombToMACC
  {2, 1, 2, 1, 2, 1, 2, 1,/*SHU0T*/ 2, 1, 2, 1, 2, 1, 2, 1/*SHU1T*/}, /// SHU0CombToSHU
  {2, 1/*BIU0W*/, 2, 1/*BIU1W*/, 2, 1/*BIU2W*/}, /// SHU0IndTBNoOptToBIU
  {4, 1/*MW3*/},                 /// SHU0IndTBNoOptToM
  {3, 1, 3, 1, 3, 1,/*IPATH*/ 3, 1, 3, 1, 3, 1, 3, 1,/*IALUT*/
   3, 1, 3, 1, 3, 1, 3, 1,/*IMACT*/ 3, 1, 3, 1, 3, 1, 3, 1,/*FALUT*/
   3, 1, 3, 1, 3, 1, 3, 1,/*FMACT*/}, /// SHU0IndTBNoOptToMACC
  {2, 1, 2, 1, 2, 1, 2, 1,/*SHU0T*/ 2, 1, 2, 1, 2, 1, 2, 1/*SHU1T*/}, /// SHU0IndTBNoOptToSHU
  {2, 1/*BIU0W*/, 2, 1/*BIU1W*/, 2, 1/*BIU2W*/}, /// SHU0IndTBOptToBIU
  {4, 1/*MW3*/},                 /// SHU0IndTBOptToM
  {3, 1, 3, 1, 3, 1,/*IPATH*/ 3, 1, 3, 1, 3, 1, 3, 1,/*IALUT*/
   3, 1, 3, 1, 3, 1, 3, 1,/*IMACT*/ 3, 1, 3, 1, 3, 1, 3, 1,/*FALUT*/
   3, 1, 3, 1, 3, 1, 3, 1,/*FMACT*/}, /// SHU0IndTBOptToMACC
  {2, 1, 2, 1, 2, 1, 2, 1,/*SHU0T*/ 2, 1, 2, 1, 2, 1, 2, 1/*SHU1T*/}, /// SHU0IndTBOptToSHU
  {2, 1/*BIU0W*/, 2, 1/*BIU1W*/, 2, 1/*BIU2W*/}, /// SHU0IndTNoOptToBIU
  {4, 1/*MW3*/},                 /// SHU0IndTNoOptToM
  {3, 1, 3, 1, 3, 1,/*IPATH*/ 3, 1, 3, 1, 3, 1, 3, 1,/*IALUT*/
   3, 1, 3, 1, 3, 1, 3, 1,/*IMACT*/ 3, 1, 3, 1, 3, 1, 3, 1,/*FALUT*/
   3, 1, 3, 1, 3, 1, 3, 1,/*FMACT*/}, /// SHU0IndTNoOptToMACC
  {2, 1, 2, 1, 2, 1, 2, 1,/*SHU0T*/ 2, 1, 2, 1, 2, 1, 2, 1/*SHU1T*/}, /// SHU0IndTNoOptToSHU
  {2, 1/*BIU0W*/, 2, 1/*BIU1W*/, 2, 1/*BIU2W*/}, /// SHU0IndTOptToBIU
  {4, 1/*MW3*/},                 /// SHU0IndTOptToM
  {3, 1, 3, 1, 3, 1,/*IPATH*/ 3, 1, 3, 1, 3, 1, 3, 1,/*IALUT*/
   3, 1, 3, 1, 3, 1, 3, 1,/*IMACT*/ 3, 1, 3, 1, 3, 1, 3, 1,/*FALUT*/
   3, 1, 3, 1, 3, 1, 3, 1,/*FMACT*/}, /// SHU0IndTOptToMACC
  {2, 1, 2, 1, 2, 1, 2, 1,/*SHU0T*/ 2, 1, 2, 1, 2, 1, 2, 1/*SHU1T*/}, /// SHU0IndTOptToSHU
  {},                            /// SHU0Start
  {},                            /// SHU0Stop
  {2, 1/*BIU0W*/, 2, 1/*BIU1W*/, 2, 1/*BIU2W*/}, /// SHU1CombToBIU
  {4, 1/*MW3*/},                 /// SHU1CombToM
  {3, 1, 3, 1, 3, 1,/*IPATH*/ 3, 1, 3, 1, 3, 1, 3, 1,/*IALUT*/
   3, 1, 3, 1, 3, 1, 3, 1,/*IMACT*/ 3, 1, 3, 1, 3, 1, 3, 1,/*FALUT*/
   3, 1, 3, 1, 3, 1, 3, 1,/*FMACT*/},   /// SHU1CombToMACC
  {2, 1, 2, 1, 2, 1, 2, 1,/*SHU0T*/ 2, 1, 2, 1, 2, 1, 2, 1/*SHU1T*/},   /// SHU1CombToSHU
  {2, 1/*BIU0W*/, 2, 1/*BIU1W*/, 2, 1/*BIU2W*/}, /// SHU1IndTBNoOptToBIU
  {4, 1/*MW3*/},                   /// SHU1IndTBNoOptToM
  {3, 1, 3, 1, 3, 1,/*IPATH*/ 3, 1, 3, 1, 3, 1, 3, 1,/*IALUT*/
   3, 1, 3, 1, 3, 1, 3, 1,/*IMACT*/ 3, 1, 3, 1, 3, 1, 3, 1,/*FALUT*/
   3, 1, 3, 1, 3, 1, 3, 1,/*FMACT*/},   /// SHU1IndTBNoOptToMACC
  {2, 1, 2, 1, 2, 1, 2, 1,/*SHU0T*/ 2, 1, 2, 1, 2, 1, 2, 1/*SHU1T*/},   /// SHU1IndTBNoOptToSHU
  {2, 1/*BIU0W*/, 2, 1/*BIU1W*/, 2, 1/*BIU2W*/}, /// SHU1IndTBOptToBIU
  {4, 1/*MW3*/},                   /// SHU1IndTBOptToM
  {3, 1, 3, 1, 3, 1,/*IPATH*/ 3, 1, 3, 1, 3, 1, 3, 1,/*IALUT*/
   3, 1, 3, 1, 3, 1, 3, 1,/*IMACT*/ 3, 1, 3, 1, 3, 1, 3, 1,/*FALUT*/
   3, 1, 3, 1, 3, 1, 3, 1,/*FMACT*/},   /// SHU1IndTBOptToMACC
  {2, 1, 2, 1, 2, 1, 2, 1,/*SHU0T*/ 2, 1, 2, 1, 2, 1, 2, 1/*SHU1T*/},   /// SHU1IndTBOptToSHU
  {2, 1/*BIU0W*/, 2, 1/*BIU1W*/, 2, 1/*BIU2W*/}, /// SHU1IndTNoOptToBIU
  {4, 1/*MW3*/},                   /// SHU1IndTNoOptToM
  {3, 1, 3, 1, 3, 1,/*IPATH*/ 3, 1, 3, 1, 3, 1, 3, 1,/*IALUT*/
   3, 1, 3, 1, 3, 1, 3, 1,/*IMACT*/ 3, 1, 3, 1, 3, 1, 3, 1,/*FALUT*/
   3, 1, 3, 1, 3, 1, 3, 1,/*FMACT*/},   /// SHU1IndTNoOptToMACC
  {2, 1, 2, 1, 2, 1, 2, 1,/*SHU0T*/ 2, 1, 2, 1, 2, 1, 2, 1/*SHU1T*/},   /// SHU1IndTNoOptToSHU
  {2, 1/*BIU0W*/, 2, 1/*BIU1W*/, 2, 1/*BIU2W*/}, /// SHU1IndTOptToBIU
  {4, 1/*MW3*/},                   /// SHU1IndTOptToM
  {3, 1, 3, 1, 3, 1,/*IPATH*/ 3, 1, 3, 1, 3, 1, 3, 1,/*IALUT*/
   3, 1, 3, 1, 3, 1, 3, 1,/*IMACT*/ 3, 1, 3, 1, 3, 1, 3, 1,/*FALUT*/
   3, 1, 3, 1, 3, 1, 3, 1,/*FMACT*/}, /// SHU1IndTOptToMACC
  {2, 1, 2, 1, 2, 1, 2, 1,/*SHU0T*/ 2, 1, 2, 1, 2, 1, 2, 1/*SHU1T*/}, /// SHU1IndTOptToSHU
  {},                              /// SHU1Start
  {}                               /// SHU1Stop
};
} // namespace llvm
