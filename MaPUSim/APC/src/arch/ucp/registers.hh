#ifndef __ARCH_UCP_REGISTERS_HH__
#define __ARCH_UCP_REGISTERS_HH__

#include "arch/ucp/generated/max_inst_regs.hh"
#include "base/misc.hh"
#include "base/types.hh"
#include "mpuregister.hh"

namespace UcpISA
{
using UcpISAInst::MaxInstSrcRegs;
using UcpISAInst::MaxInstDestRegs;
using UcpISAInst::MaxMiscDestRegs;

// Constants Related to the number of registers
const int NumIntArchRegs = 36;
const int NumIntSpecialRegs = 9;
const int NumFloatArchRegs = 32;
const int NumFloatSpecialRegs = 5;

const int MaxShadowRegSets = 16; // Maximum number of shadow register sets
const int NumIntRegs = NumIntArchRegs + NumIntSpecialRegs;        //HI & LO Regs
const int NumIntJRegs = 36;
const int NumDoubleRegs = NumIntRegs/2;
const int NumFloatRegs = NumFloatArchRegs + NumFloatSpecialRegs;//

const int NumMRegs = 128;
const int NumShu0TRegs = 16;
const int NumShu1TRegs = 16;
const int NumIaluTRegs = 4;
const int NumFaluTRegs = 4;
const int NumImacTRegs = 4;
const int NumFmacTRegs = 4;
const int NumMPUGPRegs = NumMRegs + NumShu0TRegs + NumShu1TRegs
        + NumIaluTRegs + NumFaluTRegs + NumImacTRegs
        + NumFmacTRegs;
enum {
  IMACMRL = NumMPUGPRegs,  // 176
  IMACMRH,
  FMACMR,
  BIU0W,
  BIU1W,
  BIU2W,
  DIVQU,
  DIVRE,
  DIV_CNT,
  SYN_SVR_BASE = 185,
  SYN_SVR_END = 186,
  MPUREG_NUMREGS
};

// MPU special registers
const int SHUTCL = 13;
const int SHUTCH = 14;
const int SHUTB = 15;
/*const int SHU0TCL = (13 << 4) | 1;
const int SHU0TCH = (14 << 4) | 1;
const int SHU0TB = (15 << 4) | 1;
const int SHU1TCL = (13 << 4) | 5;
const int SHU1TCH = (14 << 4) | 5;
const int SHU1TB = (15 << 4) | 5;*/

const uint32_t UCP32_QNAN = 0x7fbfffff;
const uint64_t UCP64_QNAN = ULL(0x7ff7ffffffffffff);

// semantically meaningful register indices
const int ZeroReg = 0xFFFFFFFF;
const int SyscallSuccessReg = 0xFE;
const int FirstArgumentReg = 0;
const int ReturnValueReg = 0;

const int GlobalPointerReg = 28;
const int StackPointerReg = 29;
const int FramePointerReg = 30;
const int ReturnAddressReg = 31;

const int SyscallPseudoReturnReg = 3;

const int NumKRegs = 128;

//@TODO: Implementing ShadowSets needs to
//edit this value such that:
//TotalArchRegs = NumIntArchRegs * ShadowSets
const int TotalArchRegs = NumIntArchRegs;

// These help enumerate all the registers for dependence tracking.
const int J_Base_DepTag = NumIntRegs;
const int FP_Base_DepTag = J_Base_DepTag + NumIntJRegs;
const int DR_Base_DepTag = FP_Base_DepTag + NumFloatRegs;
const int Ctrl_Base_DepTag = DR_Base_DepTag + NumDoubleRegs;

// Enumerate names for 'Control' Registers in the CPU
// Reference MIPS32 Arch. for Programmers, Vol. III, Ch.8
// (Register Number-Register Select) Summary of Register
//------------------------------------------------------
// The first set of names classify the CP0 names as Register Banks
// for easy indexing when using the 'RD + SEL' index combination
// in CP0 instructions.
enum MiscRegIndex{
    MISCREG_CI = 0,

    MISCREG_LPEN,
    MISCREG_LP0S,
    MISCREG_LP0E,
    MISCREG_LP0N,
    MISCREG_LP1S,
    MISCREG_LP1E,
    MISCREG_LP1N,

    MISCREG_MC_BASE = 16,
    MISCREG_MC_END  = 20,

    MISCREG_KB_BASE = 32,
    MISCREG_KE_BASE = 48,
    MISCREG_KS_BASE = 64,
    MISCREG_KI_BASE = 80,
    MISCREG_KM_BASE = 96,
    MISCREG_KG_BASE = 112,
    MISCREG_KL_BASE = 128,
    MISCREG_KC_BASE = 144,
    //MISCREG_KREGS_END = 160,

    // MPU
    MISCREG_MPUCOND = 160,

    MISCREG_MLPEN,
    MISCREG_MLP0S,
    MISCREG_MLP1S,
    MISCREG_MLP2S,
    MISCREG_MLP3S,
    MISCREG_MLP0E,
    MISCREG_MLP1E,
    MISCREG_MLP2E,
    MISCREG_MLP3E,
    MISCREG_MLP0N,
    MISCREG_MLP1N,
    MISCREG_MLP2N,
    MISCREG_MLP3N,
    MISCREG_NUMREGS
};

const int TotalDataRegs = NumIntRegs + NumFloatRegs + NumIntJRegs + NumDoubleRegs;

const int NumMiscRegs = MISCREG_NUMREGS;

const int MPU_Base_DepTag = Ctrl_Base_DepTag + NumMiscRegs;

const int NumMPURegs = MPUREG_NUMREGS;

const int Max_DepTag = MPU_Base_DepTag + NumMPURegs;

const int TotalNumRegs = NumIntRegs + NumFloatRegs + NumIntJRegs + NumMiscRegs + NumDoubleRegs + NumMPURegs;

typedef uint16_t  RegIndex;

typedef uint32_t IntReg;

typedef uint32_t IntJReg;

// floating point register file entry type
typedef uint32_t FloatRegBits;
typedef float FloatReg;
typedef uint64_t DoubleRegBits;
typedef double DoubleReg;

// cop-0/cop-1 system control register
typedef uint32_t MiscReg;

union Vector{
		uint8_t vub[64];
	};

typedef union {
    IntReg   intreg;
    IntJReg   intjreg;
    FloatReg fpreg;
    DoubleReg doublereg;
    MiscReg  ctrlreg;
} AnyReg;

typedef MPURegister MPUReg;

struct BiuContext {
  uint32_t KB[4];
  uint32_t KE[4];
  uint32_t KS[4];
  uint32_t KI[4];
  uint32_t KM[2];
  uint32_t KG;
  uint32_t KL;
  uint32_t KC[4];
  BiuContext &operator =(const MPURegister &rhs);
};

} // namespace UcpISA

#endif
