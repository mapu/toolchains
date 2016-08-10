///===-- UCPMScheduler.h ---------------------------------------------------===///
///
///                     The LLVM Compiler Infrastructure
///
/// This file is distributed under the University of Illinois Open Source
/// License. See LICENSE.TXT for details.
///
///===----------------------------------------------------------------------===///
///
/// This file defines a scheduler class for checking temporal resouce usage
/// collisions among parsed MCInsts.
///
///===----------------------------------------------------------------------===///

#ifndef UCPMSCHEDULER_H
#define UCPMSCHEDULER_H

#include "UCPMAsmParser.h"
#include "MCFunction.h"
#include <map>

namespace llvm {
namespace UCPM {
class UCPMScheduler {
public:
  enum ResouceTy {
    Invalid = 0, // Indicate usage info of an nonexisting inst, such as BIU0.DM -> BIU0
    MW0,
    MW1,
    MW2,
    MW3,
    IALUW,
    IMACW,
    IMRW,
    FALUW,
    FMACW,
    FMRW,
    FMACEX3,
    I0,
    I1,
    I2,
    MACCO,
    IALUT0,
    IALUT1,
    IALUT2,
    IALUT3,
    IMACT0,
    IMACT1,
    IMACT2,
    IMACT3,
    FALUT0,
    FALUT1,
    FALUT2,
    FALUT3,
    FMACT0,
    FMACT1,
    FMACT2,
    FMACT3,
    SHU0T0,
    SHU0T1,
    SHU0T2,
    SHU0T3,
    SHU1T0,
    SHU1T1,
    SHU1T2,
    SHU1T3,
    BIU0R,
    BIU1R,
    BIU2R,
    BIU0W,
    BIU1W,
    BIU2W,
    NumResouces
  };
  
private:
  struct resv {
    uint64_t IssueTime;
    uint64_t ValidTime;
    const MCInst *Owner;
    union {
      unsigned MID;
      uint64_t Period;
    };
  };

  struct use {
    unsigned ResID;
    uint64_t Delay;
    const MCInst *Owner;
    union {
      unsigned MID;
      uint64_t Period;
    };
  };

  std::vector< std::map<unsigned, resv> > ResvBoard;
  std::vector< std::vector<use> > History;
  std::vector< std::map<unsigned, use *> > TrackBoard;

  std::vector<uint64_t> LoopStack;

  uint64_t CurPoint;

  MCAsmParser *Parser;

  void allocResvRecord();
  void releaseResvRecord();
  bool testUsage(uint64_t U, unsigned Res) const {
    return (U & (1ULL << Res)) != 0ULL;
  }

  // The default usage table considers that every mcode type using T write port will
  // occupy all T0~T3 ports, and it can be determined and fixed up when analyzing an instance
  // of that mcode type.
  uint64_t UsageFixup(const uint64_t DefUsage, const MCInst *MI);

  // Fix up the extra resources used by double reg write-back mcodes, e.g. Tm * tn (H) -> BIU0,
  // this will occupy BIU0W and MACCO for 2 cycles.
  void FixupDoubleRegWB(std::vector<use> &instline, const MCInst *MI, unsigned begin);

public:
  //yangl #define INSTRUCTION_LIST_LEN (UCPM::INSTRUCTION_LIST_END - UCPM::BIU0KG)
  #define INSTRUCTION_LIST_LEN 1000//yangl temp, please delete
  static const std::string ResName[NumResouces];
  static const uint64_t ResUsageMap[INSTRUCTION_LIST_LEN];
  static const uint64_t ResLatMap[INSTRUCTION_LIST_LEN][NumResouces * 2];
  static const uint64_t R_SHU0T = (1ULL << SHU0T0 | 1ULL << SHU0T1 |
                                   1ULL << SHU0T2 | 1ULL << SHU0T3);
  static const uint64_t R_SHU1T = (1ULL << SHU1T0 | 1ULL << SHU1T1 |
                                   1ULL << SHU1T2 | 1ULL << SHU1T3);
  static const uint64_t R_IALUT = (1ULL << IALUT0 | 1ULL << IALUT1 |
                                   1ULL << IALUT2 | 1ULL << IALUT3);
  static const uint64_t R_IMACT = (1ULL << IMACT0 | 1ULL << IMACT1 |
                                   1ULL << IMACT2 | 1ULL << IMACT3);
  static const uint64_t R_FALUT = (1ULL << FALUT0 | 1ULL << FALUT1 |
                                   1ULL << FALUT2 | 1ULL << FALUT3);
  static const uint64_t R_FMACT = (1ULL << FMACT0 | 1ULL << FMACT1 |
                                   1ULL << FMACT2 | 1ULL << FMACT3);
  static const uint64_t R_MACCT = (R_IALUT | R_IMACT | R_FALUT | R_FMACT);
  static const uint64_t R_SHUT  = (R_SHU0T | R_SHU1T);
  static const uint64_t R_IPATH = (1ULL << I0 | 1ULL << I1 | 1ULL << I2);
  static const uint64_t R_BIUW  = (1ULL << BIU0W | 1ULL << BIU1W |
                                   1ULL << BIU2W);
  static const uint64_t R_MW    = (1ULL << MW0 | 1ULL << MW1 | 1ULL << MW2 |
                                   1ULL << MW3);
  
  UCPMScheduler(MCAsmParser &Parser) : CurPoint(0), Parser(&Parser) {
    allocResvRecord();
  };

  UCPMScheduler() : CurPoint(0) {
    Parser = NULL;
    allocResvRecord();
  };

  void Schedule(uint64_t VA, MCInst *MI, unsigned Sym);
  void Trace(MCInst *MI);
  
  static uint64_t getLatency(unsigned Opc);
  
  static uint64_t getLatency(unsigned Opc, ResouceTy Res);
};
}
}
#endif
