#include "arch/ucp/isa.hh"
#include "base/bitfield.hh"
#include "cpu/base.hh"
#include "cpu/thread_context.hh"
#include "debug/UcpPRA.hh"

namespace UcpISA {

std::string ISA::miscRegNames[NumMiscRegs] = {
    "Carry flag",
    "Loop enable",
    "Loop0 start",
    "Loop0 end",
    "Loop0 number",
    "Loop1 start",
    "Loop1 end",
    "Loop1 number", "","","","","","","","",
    "MC.r0", "MC.r1", "MC.w0", "MC.w1", "MC.w2",
    "MI.r0", "MI.r1", "MI.w0", "MI.w1", "MI.w2",
    "MS.r0", "MS.r1", "MS.w0", "MS.w1", "MS.w2","",
    "KB0", "KB1", "KB2", "KB3", "KB4", "KB5", "KB6", "KB7", "KB8", "KB9", "KB10", "KB11", "KB12", "KB13", "KB14", "KB15",
    "KE0", "KE1", "KE2", "KE3", "KE4", "KE5", "KE6", "KE7", "KE8", "KE9", "KE10", "KE11", "KE12", "KE13", "KE14", "KE15",
    "KS0", "KS1", "KS2", "KS3", "KS4", "KS5", "KS6", "KS7", "KS8", "KS9", "KS10", "KS11", "KS12", "KS13", "KS14", "KS15",
    "KI0", "KI1", "KI2", "KI3", "KI4", "KI5", "KI6", "KI7", "KI8", "KI9", "KI10", "KI11", "KI12", "KI13", "KI14", "KI15",
    "KM0L", "KM0L", "KM0H", "KM0H", "KM1L", "KM1L", "KM1H", "KM1H", "KM2L", "KM2L", "KM2H", "KM2H", "", "", "", "",
    "KG0", "", "", "", "KG1", "", "", "", "KG2", "", "", "", "", "", "", "",
    "KL0", "KL1", "KL2", "KL3", "KL4", "KL5", "KL6", "KL7", "KL8", "KL9", "KL10", "KL11", "KL12", "KL13", "KL14", "KL15",
    "KC0", "KC1", "KC2", "KC3", "KC4", "KC5", "KC6", "KC7", "KC8", "KC9", "KC10", "KC11", "KC12", "KC13", "KC14", "KC15",

    // MPU
    "MPU condition",
    "MPU Loop enable",
    "MPU Loop0 start",
    "MPU Loop1 start",
    "MPU Loop2 start",
    "MPU Loop3 start",
    "MPU Loop0 end",
    "MPU Loop1 end",
    "MPU Loop2 end",
    "MPU Loop3 end",

    "MPU Loop0 number",
    "MPU Loop1 number",
    "MPU Loop2 number",
    "MPU Loop3 number"

};

ISA::ISA() {
  miscRegFile.resize(NumMiscRegs);

  for (int i = 0; i < NumMiscRegs; i++)
    miscRegFile[i].resize(1);

  clear();

  // initialize constant SHU T registers.
  /*const uint8_t TBB[64]  = {0};
   const uint16_t TBH[32] = {0x0100};
   const uint32_t TBW[16] = {0x03020100};
   const uint64_t TBD[8]  = {0x0706050403020100};*/
  const uint8_t TSQ[64] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
                            15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27,
                            28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
                            41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53,
                            54, 55, 56, 57, 58, 59, 60, 61, 62, 63 };
  uint8_t *ub = MPURegFile[NumMRegs + 4];
  uint16_t *uh = MPURegFile[NumMRegs + 5];
  uint32_t *uw = MPURegFile[NumMRegs + 6];
  uint64_t *ud = MPURegFile[NumMRegs + 7];
  memset(ub, 0x0, 64);
  for (unsigned i = 0; i < 32; i++)
    uh[i] = 0x0100;
  for (unsigned i = 0; i < 16; i++)
    uw[i] = 0x03020100;
  for (unsigned i = 0; i < 8; i++)
    ud[i] = 0x0706050403020100ULL;
  MPURegFile[NumMRegs + 8] = TSQ;
  ub = MPURegFile[NumMRegs + NumShu0TRegs + 4];
  uh = MPURegFile[NumMRegs + NumShu0TRegs + 5];
  uw = MPURegFile[NumMRegs + NumShu0TRegs + 6];
  ud = MPURegFile[NumMRegs + NumShu0TRegs + 7];
  memset(ub, 0x0, 64);
  for (unsigned i = 0; i < 32; i++)
    uh[i] = 0x0100;
  for (unsigned i = 0; i < 16; i++)
    uw[i] = 0x03020100;
  for (unsigned i = 0; i < 8; i++)
    ud[i] = 0x0706050403020100ULL;
  MPURegFile[NumMRegs + NumShu0TRegs + 8] = TSQ;
}

void ISA::clear() {
  for (int i = 0; i < NumMiscRegs; i++) {
    for (int j = 0; j < miscRegFile[i].size(); j++) {
      if (i >= MISCREG_KL_BASE && i< MISCREG_KC_BASE)
        miscRegFile[i][j] = 1;
      else
        miscRegFile[i][j] = 0;
    }
  }
}

MiscReg ISA::readMiscRegNoEffect(int misc_reg, ThreadID tid) {
  unsigned reg_sel = tid;
  DPRINTF(UcpPRA, "[tid:%i]: Reading Misc Register:%u (%s) (%lx).\n",
          tid, misc_reg, miscRegNames[misc_reg],
          miscRegFile[misc_reg][reg_sel]);
  return miscRegFile[misc_reg][reg_sel];
}

//@TODO: Ucp MT's register view automatically connects
//       Status to TCStatus depending on current thread
//template <class TC>
MiscReg ISA::readMiscReg(int misc_reg, ThreadContext *tc, ThreadID tid) {
  unsigned reg_sel = tid;
  DPRINTF(UcpPRA, "[tid:%i]: Reading Misc Register:%u (%s) with effect (%lx).\n",
          tid, misc_reg, miscRegNames[misc_reg], miscRegFile[misc_reg][reg_sel]);

  return miscRegFile[misc_reg][reg_sel];
}

MPUReg &
ISA::readMPUReg(int mpu_reg, ThreadID tid) {
  DPRINTF(UcpPRA, "[tid:%i]: Reading MPU Register:%u as ", tid, mpu_reg);
  for (int i = 0; i < 64; i++)
    DPRINTFR(UcpPRA, "%#02x ", MPURegFile[mpu_reg][i]);
  DPRINTFR(UcpPRA, ".\n");
  return MPURegFile[mpu_reg];
}

void ISA::setMiscRegNoEffect(int misc_reg, const MiscReg &val, ThreadID tid) {
  unsigned reg_sel = tid;
  DPRINTF(UcpPRA,
          "[tid:%i]: Setting (direct set) Misc Register:%u (%s) " "to %#x.\n", tid,
          misc_reg, miscRegNames[misc_reg], val);

  miscRegFile[misc_reg][reg_sel] = val;
}

void ISA::setMiscReg(int misc_reg,
                     const MiscReg &val,
                     ThreadContext *tc,
                     ThreadID tid) {
  int reg_sel = tid;

  DPRINTF(UcpPRA,
          "[tid:%i]: Setting Misc Register:%u (%s) " "to %#x, with effect.\n", tid,
          misc_reg, miscRegNames[misc_reg], val);

  miscRegFile[misc_reg][reg_sel] = val;
  if (misc_reg >= MISCREG_MC_BASE && misc_reg < MISCREG_MC_BASE + 5) {
    MRegCtrlCtx[misc_reg - MISCREG_MC_BASE].MStart = miscRegFile[misc_reg][reg_sel] & 0x7F;
    MRegCtrlCtx[misc_reg - MISCREG_MC_BASE].StepSize =
        (miscRegFile[misc_reg][reg_sel] >> 8) & 0x3;
    MRegCtrlCtx[misc_reg - MISCREG_MC_BASE].MEnd =
        (MRegCtrlCtx[misc_reg - MISCREG_MC_BASE].MStart +
            (miscRegFile[misc_reg][reg_sel] >> 24));
    if (misc_reg - MISCREG_MC_BASE < 2) {
      MRegCtrlCtx[misc_reg - MISCREG_MC_BASE].IStart = MRegCtrlCtx[misc_reg - MISCREG_MC_BASE].MStart;
      MRegCtrlCtx[misc_reg - MISCREG_MC_BASE].IEnd =
        MRegCtrlCtx[misc_reg - MISCREG_MC_BASE].IStart +
        ((miscRegFile[misc_reg][reg_sel] >> 12) & 0x7F);
    } else {
      MRegCtrlCtx[misc_reg - MISCREG_MC_BASE].IStart = MRegCtrlCtx[misc_reg - MISCREG_MC_BASE].MStart;
      MRegCtrlCtx[misc_reg - MISCREG_MC_BASE].IEnd = MRegCtrlCtx[misc_reg - MISCREG_MC_BASE].MEnd;
    }
    MRegCtrlCtx[misc_reg - MISCREG_MC_BASE].CurIdx = MRegCtrlCtx[misc_reg - MISCREG_MC_BASE].IStart;
  }
}

void ISA::setMPUReg(int mpu_reg, const MPUReg &val, ThreadID tid) {
  DPRINTF(UcpPRA, "[tid:%i]: Setting MPU Register:%u to ", tid, mpu_reg);
  for (int i = 0; i < 64; i++)
    DPRINTFR(UcpPRA, "%#02x ", val[i]);
  DPRINTFR(UcpPRA, ".\n");
  MPURegFile[mpu_reg] = val;
}

void ISA::setPartialMPUReg(RegIndex mpu_reg, int size, int idx,
                           IntReg val, ThreadID tid) {
  if (size == 2)
    ((uint16_t *)(MPURegFile[mpu_reg]))[idx] = val;
  else if (size == 4)
    ((uint32_t *)(MPURegFile[mpu_reg]))[idx] = val;
  else
    MPURegFile[mpu_reg][idx] = val;

  DPRINTF(UcpPRA, "[tid:%i]: Setting MPU Register:%u to ", tid,
          mpu_reg);
  for (int i = 0; i < 64; i++)
    DPRINTFR(UcpPRA, "%#02x ", MPURegFile[mpu_reg][i]);
  DPRINTFR(UcpPRA, ".\n");
}

int ISA::calcMIndex(int reg) {
  uint8_t slotnum = (reg >> 4);
  if (slotnum >= 2)
    slotnum -= 8;
  uint8_t &MStart = MRegCtrlCtx[slotnum].MStart;
  uint8_t &MEnd = MRegCtrlCtx[slotnum].MEnd;
  uint8_t &StepSize = MRegCtrlCtx[slotnum].StepSize;
  uint8_t &IStart = MRegCtrlCtx[slotnum].IStart;
  uint8_t &IEnd = MRegCtrlCtx[slotnum].IEnd;
  uint8_t &CurIdx = MRegCtrlCtx[slotnum].CurIdx;
  uint8_t _CurIdx = CurIdx;
  if (((reg >> 2) & 0x3) == 1) {
    CurIdx += StepSize;
    if (CurIdx >= NumMRegs) CurIdx -= NumMRegs;
    if (CurIdx == IEnd) CurIdx = IStart;
    if (CurIdx == MEnd) CurIdx = MStart;
  } else if (((reg >> 2) & 0x3) == 2) {
    IStart += StepSize;
    IEnd += StepSize;
    if (IStart >= NumMRegs) IStart -= NumMRegs;
    if (IStart == MEnd) IStart = MStart;
    if (IEnd >= NumMRegs) IEnd -= NumMRegs;
    if (IEnd > MEnd) IEnd = MStart + StepSize;
    CurIdx = IStart;
  }
  return _CurIdx;
}

int ISA::flattenMPURegIndex(int reg) {
  // reg contains dest. unit coding and probably
  // S++ and I++ flags, so the real index needs to be
  // calculated here
  uint8_t gid = reg & 3;
  uint8_t uid = (reg >> 2) & 3;
  uint8_t idx = (reg >> 4) & 3; // mask Ipath
  switch (gid) {
  case 0x0:
    if (uid != 0)
      return calcMIndex(reg);
    else if (idx < NumMRegs)
      return reg >> 4;
    else
      return 0;
  case 0x1:
    if(uid == 2)
      return SYN_SVR_BASE + idx;
    else
      return (reg >> 4) + NumMRegs + NumShu0TRegs * uid;
  case 0x2:
    //if (idx != 0) return -1;
    return BIU0W + uid;
  case 0x3:
    return idx + NumMRegs + NumShu0TRegs + NumShu1TRegs + NumIaluTRegs * uid;
  default:
    return 0;
  }
  return reg;
}

}
