//===-- ABINone_mspu.cpp --------------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "ABINone_mspu.h"

#include "lldb/Core/ConstString.h"
#include "lldb/Core/Error.h"
#include "lldb/Core/Module.h"
#include "lldb/Core/PluginManager.h"
#include "lldb/Core/RegisterValue.h"
#include "lldb/Core/Scalar.h"
#include "lldb/Core/ValueObjectConstResult.h"
#include "lldb/Symbol/ClangASTContext.h"
#include "lldb/Symbol/UnwindPlan.h"
#include "lldb/Target/Process.h"
#include "lldb/Target/RegisterContext.h"
#include "lldb/Target/Target.h"
#include "lldb/Target/Thread.h"

#include "llvm/ADT/Triple.h"

#include <vector>

using namespace lldb;
using namespace lldb_private;

enum e_mspu_register
{
  e_R0 = 0, e_R1, e_R2, e_R3, e_R4, e_R5, e_R6, e_R7, e_R8, e_R9,
  e_R10, e_R11, e_R12, e_R13, e_R14, e_R15, e_R16, e_R17, e_R18, e_R19,
  e_R20, e_R21, e_R22, e_R23, e_R24, e_R25, e_R26, e_R27, e_R28, e_R29,
  e_R30, e_R31,

  e_J0, e_J1, e_J2, e_J3, e_J4, e_J5, e_J6, e_J7, e_J8, e_J9,
  e_J10, e_J11, e_J12, e_J13, e_J14, e_J15, e_J16, e_J17, e_J18, e_J19,
  e_J20, e_J21, e_J22, e_J23, e_J24, e_J25, e_J26, e_J27, e_J28, e_J29,
  e_J30, e_J31,

  e_S0, e_S1, e_S2, e_S3, e_S4, e_S5, e_S6, e_S7, e_S8, e_S9,
  e_S10, e_S11, e_S12, e_S13, e_S14, e_S15, e_S16, e_S17, e_S18, e_S19,
  e_S20, e_S21, e_S22, e_S23, e_S24, e_S25, e_S26, e_S27, e_S28, e_S29,
  e_S30, e_S31,

  e_DR0, e_DR1, e_DR2, e_DR3, e_DR4, e_DR5, e_DR6, e_DR7, e_DR8, e_DR9,
  e_DR10, e_DR11, e_DR12, e_DR13, e_DR14, e_DR15,

  e_DPC, e_RRPC,

  // lldb generic registers
  dwarf_pc = e_DPC,
  dwarf_ra = e_J30,
  dwarf_sp = e_J29,
  dwarf_fp = e_J28,
  dwarf_arg1 = e_R0,
  dwarf_arg2 = e_R1,
  dwarf_arg3 = e_R2,
  dwarf_arg4 = e_R3,
  dwarf_arg5 = e_R4,
  dwarf_arg6 = e_R5,
  dwarf_arg7 = e_R6,
  dwarf_arg8 = e_R7
};

static RegisterInfo g_register_infos[] = 
{
  //  NAME      ALT      SZ OFF        ENCODING         FORMAT                COMPILER  DWARF    GENERIC                     GDB      LLDB NATIVE VALUE REGS    INVALIDATE REGS
  //  ======    =======  == ===        =============    ============          ========= ======== =========================== ======== =========== ==========    ===============
    { "R0",     "arg1",  4, 0*4,       eEncodingSint , eFormatHex,          { e_R0    , e_R0  ,  LLDB_REGNUM_GENERIC_ARG1  , e_R0   , e_R0     }, NULL,              NULL},
    { "R1",     "arg2",  4, 1*4,       eEncodingSint , eFormatHex,          { e_R1    , e_R1  ,  LLDB_REGNUM_GENERIC_ARG2  , e_R1   , e_R1     }, NULL,              NULL},
    { "R2",     "arg3",  4, 2*4,       eEncodingSint , eFormatHex,          { e_R2    , e_R2  ,  LLDB_REGNUM_GENERIC_ARG3  , e_R2   , e_R2     }, NULL,              NULL},
    { "R3",     "arg4",  4, 3*4,       eEncodingSint , eFormatHex,          { e_R3    , e_R3  ,  LLDB_REGNUM_GENERIC_ARG4  , e_R3   , e_R3     }, NULL,              NULL},
    { "R4",     "arg5",  4, 4*4,       eEncodingSint , eFormatHex,          { e_R4    , e_R4  ,  LLDB_REGNUM_GENERIC_ARG5  , e_R4   , e_R4     }, NULL,              NULL},
    { "R5",     "arg6",  4, 5*4,       eEncodingSint , eFormatHex,          { e_R5    , e_R5  ,  LLDB_REGNUM_GENERIC_ARG6  , e_R5   , e_R5     }, NULL,              NULL},
    { "R6",     "arg7",  4, 6*4,       eEncodingSint , eFormatHex,          { e_R6    , e_R6  ,  LLDB_REGNUM_GENERIC_ARG7  , e_R6   , e_R6     }, NULL,              NULL},
    { "R7",     "arg8",  4, 7*4,       eEncodingSint , eFormatHex,          { e_R7    , e_R7  ,  LLDB_REGNUM_GENERIC_ARG8  , e_R7   , e_R7     }, NULL,              NULL},
    { "R8",     NULL,    4, 8*4,       eEncodingSint , eFormatHex,          { e_R8    , e_R8  ,  LLDB_INVALID_REGNUM       , e_R8   , e_R8     }, NULL,              NULL},
    { "R9",     NULL,    4, 9*4,       eEncodingSint , eFormatHex,          { e_R9    , e_R9  ,  LLDB_INVALID_REGNUM       , e_R9   , e_R9     }, NULL,              NULL},
    { "R10",    NULL,    4, 10*4,      eEncodingSint, eFormatHex,           { e_R10   , e_R10 ,  LLDB_INVALID_REGNUM       , e_R10  , e_R10    }, NULL,              NULL},
    { "R11",    NULL,    4, 11*4,      eEncodingSint, eFormatHex,           { e_R11   , e_R11 ,  LLDB_INVALID_REGNUM       , e_R11  , e_R11    }, NULL,              NULL},
    { "R12",    NULL,    4, 12*4,      eEncodingSint, eFormatHex,           { e_R12   , e_R12 ,  LLDB_INVALID_REGNUM       , e_R12  , e_R12    }, NULL,              NULL},
    { "R13",    NULL,    4, 13*4,      eEncodingSint, eFormatHex,           { e_R13   , e_R13 ,  LLDB_INVALID_REGNUM       , e_R13  , e_R13    }, NULL,              NULL},
    { "R14",    NULL,    4, 14*4,      eEncodingSint, eFormatHex,           { e_R14   , e_R14 ,  LLDB_INVALID_REGNUM       , e_R14  , e_R14    }, NULL,              NULL},
    { "R15",    NULL,    4, 15*4,      eEncodingSint, eFormatHex,           { e_R15   , e_R15 ,  LLDB_INVALID_REGNUM       , e_R15  , e_R15    }, NULL,              NULL},
    { "R16",    NULL,    4, 16*4,      eEncodingSint, eFormatHex,           { e_R16   , e_R16 ,  LLDB_INVALID_REGNUM       , e_R16  , e_R16    }, NULL,              NULL},
    { "R17",    NULL,    4, 17*4,      eEncodingSint, eFormatHex,           { e_R17   , e_R17 ,  LLDB_INVALID_REGNUM       , e_R17  , e_R17    }, NULL,              NULL},
    { "R18",    NULL,    4, 18*4,      eEncodingSint, eFormatHex,           { e_R18   , e_R18 ,  LLDB_INVALID_REGNUM       , e_R18  , e_R18    }, NULL,              NULL},
    { "R19",    NULL,    4, 19*4,      eEncodingSint, eFormatHex,           { e_R19   , e_R19 ,  LLDB_INVALID_REGNUM       , e_R19  , e_R19    }, NULL,              NULL},
    { "R20",    NULL,    4, 20*4,      eEncodingSint, eFormatHex,           { e_R20   , e_R20 ,  LLDB_INVALID_REGNUM       , e_R20  , e_R20    }, NULL,              NULL},
    { "R21",    NULL,    4, 21*4,      eEncodingSint, eFormatHex,           { e_R21   , e_R21 ,  LLDB_INVALID_REGNUM       , e_R21  , e_R21    }, NULL,              NULL},
    { "R22",    NULL,    4, 22*4,      eEncodingSint, eFormatHex,           { e_R22   , e_R22 ,  LLDB_INVALID_REGNUM       , e_R22  , e_R22    }, NULL,              NULL},
    { "R23",    NULL,    4, 23*4,      eEncodingSint, eFormatHex,           { e_R23   , e_R23 ,  LLDB_INVALID_REGNUM       , e_R23  , e_R23    }, NULL,              NULL},
    { "R24",    NULL,    4, 24*4,      eEncodingSint, eFormatHex,           { e_R24   , e_R24 ,  LLDB_INVALID_REGNUM       , e_R24  , e_R24    }, NULL,              NULL},
    { "R25",    NULL,    4, 25*4,      eEncodingSint, eFormatHex,           { e_R25   , e_R25 ,  LLDB_INVALID_REGNUM       , e_R25  , e_R25    }, NULL,              NULL},
    { "R26",    NULL,    4, 26*4,      eEncodingSint, eFormatHex,           { e_R26   , e_R26 ,  LLDB_INVALID_REGNUM       , e_R26  , e_R26    }, NULL,              NULL},
    { "R27",    NULL,    4, 27*4,      eEncodingSint, eFormatHex,           { e_R27   , e_R27 ,  LLDB_INVALID_REGNUM       , e_R27  , e_R27    }, NULL,              NULL},
    { "R28",    NULL,    4, 28*4,      eEncodingSint, eFormatHex,           { e_R28   , e_R28 ,  LLDB_INVALID_REGNUM       , e_R28  , e_R28    }, NULL,              NULL},
    { "R29",    NULL,    4, 29*4,      eEncodingSint, eFormatHex,           { e_R29   , e_R29 ,  LLDB_INVALID_REGNUM       , e_R29  , e_R29    }, NULL,              NULL},
    { "R30",    NULL,    4, 30*4,      eEncodingSint, eFormatHex,           { e_R30   , e_R30 ,  LLDB_INVALID_REGNUM       , e_R30  , e_R30    }, NULL,              NULL},
    { "R31",    NULL,    4, 31*4,      eEncodingSint, eFormatHex,           { e_R31   , e_R31 ,  LLDB_INVALID_REGNUM       , e_R31  , e_R31    }, NULL,              NULL},

    { "J0",     NULL,    4, (32+0)*4,  eEncodingUint , eFormatPointer,      { e_J0    , e_J0  ,  LLDB_INVALID_REGNUM       , e_J0   , e_J0     }, NULL,              NULL},
    { "J1",     NULL,    4, (32+1)*4,  eEncodingUint , eFormatPointer,      { e_J1    , e_J1  ,  LLDB_INVALID_REGNUM       , e_J1   , e_J1     }, NULL,              NULL},
    { "J2",     NULL,    4, (32+2)*4,  eEncodingUint , eFormatPointer,      { e_J2    , e_J2  ,  LLDB_INVALID_REGNUM       , e_J2   , e_J2     }, NULL,              NULL},
    { "J3",     NULL,    4, (32+3)*4,  eEncodingUint , eFormatPointer,      { e_J3    , e_J3  ,  LLDB_INVALID_REGNUM       , e_J3   , e_J3     }, NULL,              NULL},
    { "J4",     NULL,    4, (32+4)*4,  eEncodingUint , eFormatPointer,      { e_J4    , e_J4  ,  LLDB_INVALID_REGNUM       , e_J4   , e_J4     }, NULL,              NULL},
    { "J5",     NULL,    4, (32+5)*4,  eEncodingUint , eFormatPointer,      { e_J5    , e_J5  ,  LLDB_INVALID_REGNUM       , e_J5   , e_J5     }, NULL,              NULL},
    { "J6",     NULL,    4, (32+6)*4,  eEncodingUint , eFormatPointer,      { e_J6    , e_J6  ,  LLDB_INVALID_REGNUM       , e_J6   , e_J6     }, NULL,              NULL},
    { "J7",     NULL,    4, (32+7)*4,  eEncodingUint , eFormatPointer,      { e_J7    , e_J7  ,  LLDB_INVALID_REGNUM       , e_J7   , e_J7     }, NULL,              NULL},
    { "J8",     NULL,    4, (32+8)*4,  eEncodingUint , eFormatPointer,      { e_J8    , e_J8  ,  LLDB_INVALID_REGNUM       , e_J8   , e_J8     }, NULL,              NULL},
    { "J9",     NULL,    4, (32+9)*4,  eEncodingUint , eFormatPointer,      { e_J9    , e_J9  ,  LLDB_INVALID_REGNUM       , e_J9   , e_J9     }, NULL,              NULL},
    { "J10",    NULL,    4, (32+10)*4, eEncodingUint, eFormatPointer,       { e_J10   , e_J10 ,  LLDB_INVALID_REGNUM       , e_J10  , e_J10    }, NULL,              NULL},
    { "J11",    NULL,    4, (32+11)*4, eEncodingUint, eFormatPointer,       { e_J11   , e_J11 ,  LLDB_INVALID_REGNUM       , e_J11  , e_J11    }, NULL,              NULL},
    { "J12",    NULL,    4, (32+12)*4, eEncodingUint, eFormatPointer,       { e_J12   , e_J12 ,  LLDB_INVALID_REGNUM       , e_J12  , e_J12    }, NULL,              NULL},
    { "J13",    NULL,    4, (32+13)*4, eEncodingUint, eFormatPointer,       { e_J13   , e_J13 ,  LLDB_INVALID_REGNUM       , e_J13  , e_J13    }, NULL,              NULL},
    { "J14",    NULL,    4, (32+14)*4, eEncodingUint, eFormatPointer,       { e_J14   , e_J14 ,  LLDB_INVALID_REGNUM       , e_J14  , e_J14    }, NULL,              NULL},
    { "J15",    NULL,    4, (32+15)*4, eEncodingUint, eFormatPointer,       { e_J15   , e_J15 ,  LLDB_INVALID_REGNUM       , e_J15  , e_J15    }, NULL,              NULL},
    { "J16",    NULL,    4, (32+16)*4, eEncodingUint, eFormatPointer,       { e_J16   , e_J16 ,  LLDB_INVALID_REGNUM       , e_J16  , e_J16    }, NULL,              NULL},
    { "J17",    NULL,    4, (32+17)*4, eEncodingUint, eFormatPointer,       { e_J17   , e_J17 ,  LLDB_INVALID_REGNUM       , e_J17  , e_J17    }, NULL,              NULL},
    { "J18",    NULL,    4, (32+18)*4, eEncodingUint, eFormatPointer,       { e_J18   , e_J18 ,  LLDB_INVALID_REGNUM       , e_J18  , e_J18    }, NULL,              NULL},
    { "J19",    NULL,    4, (32+19)*4, eEncodingUint, eFormatPointer,       { e_J19   , e_J19 ,  LLDB_INVALID_REGNUM       , e_J19  , e_J19    }, NULL,              NULL},
    { "J20",    NULL,    4, (32+20)*4, eEncodingUint, eFormatPointer,       { e_J20   , e_J20 ,  LLDB_INVALID_REGNUM       , e_J20  , e_J20    }, NULL,              NULL},
    { "J21",    NULL,    4, (32+21)*4, eEncodingUint, eFormatPointer,       { e_J21   , e_J21 ,  LLDB_INVALID_REGNUM       , e_J21  , e_J21    }, NULL,              NULL},
    { "J22",    NULL,    4, (32+22)*4, eEncodingUint, eFormatPointer,       { e_J22   , e_J22 ,  LLDB_INVALID_REGNUM       , e_J22  , e_J22    }, NULL,              NULL},
    { "J23",    NULL,    4, (32+23)*4, eEncodingUint, eFormatPointer,       { e_J23   , e_J23 ,  LLDB_INVALID_REGNUM       , e_J23  , e_J23    }, NULL,              NULL},
    { "J24",    NULL,    4, (32+24)*4, eEncodingUint, eFormatPointer,       { e_J24   , e_J24 ,  LLDB_INVALID_REGNUM       , e_J24  , e_J24    }, NULL,              NULL},
    { "J25",    NULL,    4, (32+25)*4, eEncodingUint, eFormatPointer,       { e_J25   , e_J25 ,  LLDB_INVALID_REGNUM       , e_J25  , e_J25    }, NULL,              NULL},
    { "J26",    NULL,    4, (32+26)*4, eEncodingUint, eFormatPointer,       { e_J26   , e_J26 ,  LLDB_INVALID_REGNUM       , e_J26  , e_J26    }, NULL,              NULL},
    { "J27",    NULL,    4, (32+27)*4, eEncodingUint, eFormatPointer,       { e_J27   , e_J27 ,  LLDB_INVALID_REGNUM       , e_J27  , e_J27    }, NULL,              NULL},
    { "J28",    "fp",    4, (32+28)*4, eEncodingUint, eFormatPointer,       { e_J28   , e_J28 ,  LLDB_REGNUM_GENERIC_FP    , e_J28  , e_J28    }, NULL,              NULL},
    { "J29",    "sp",    4, (32+29)*4, eEncodingUint, eFormatPointer,       { e_J29   , e_J29 ,  LLDB_REGNUM_GENERIC_SP    , e_J29  , e_J29    }, NULL,              NULL},
    { "J30",    "ra",    4, (32+30)*4, eEncodingUint, eFormatPointer,       { e_J30   , e_J30 ,  LLDB_REGNUM_GENERIC_RA    , e_J30  , e_J30    }, NULL,              NULL},
    { "J31",    NULL,    4, (32+31)*4, eEncodingUint, eFormatPointer,       { e_J31   , e_J31 ,  LLDB_INVALID_REGNUM       , e_J31  , e_J31    }, NULL,              NULL},

    { "S0",     NULL,    4, 0*4,       eEncodingIEEE754 , eFormatFloat,     { e_S0    , e_S0  ,  LLDB_INVALID_REGNUM       , e_S0   , e_S0     }, NULL,              NULL},
    { "S1",     NULL,    4, 1*4,       eEncodingIEEE754 , eFormatFloat,     { e_S1    , e_S1  ,  LLDB_INVALID_REGNUM       , e_S1   , e_S1     }, NULL,              NULL},
    { "S2",     NULL,    4, 2*4,       eEncodingIEEE754 , eFormatFloat,     { e_S2    , e_S2  ,  LLDB_INVALID_REGNUM       , e_S2   , e_S2     }, NULL,              NULL},
    { "S3",     NULL,    4, 3*4,       eEncodingIEEE754 , eFormatFloat,     { e_S3    , e_S3  ,  LLDB_INVALID_REGNUM       , e_S3   , e_S3     }, NULL,              NULL},
    { "S4",     NULL,    4, 4*4,       eEncodingIEEE754 , eFormatFloat,     { e_S4    , e_S4  ,  LLDB_INVALID_REGNUM       , e_S4   , e_S4     }, NULL,              NULL},
    { "S5",     NULL,    4, 5*4,       eEncodingIEEE754 , eFormatFloat,     { e_S5    , e_S5  ,  LLDB_INVALID_REGNUM       , e_S5   , e_S5     }, NULL,              NULL},
    { "S6",     NULL,    4, 6*4,       eEncodingIEEE754 , eFormatFloat,     { e_S6    , e_S6  ,  LLDB_INVALID_REGNUM       , e_S6   , e_S6     }, NULL,              NULL},
    { "S7",     NULL,    4, 7*4,       eEncodingIEEE754 , eFormatFloat,     { e_S7    , e_S7  ,  LLDB_INVALID_REGNUM       , e_S7   , e_S7     }, NULL,              NULL},
    { "S8",     NULL,    4, 8*4,       eEncodingIEEE754 , eFormatFloat,     { e_S8    , e_S8  ,  LLDB_INVALID_REGNUM       , e_S8   , e_S8     }, NULL,              NULL},
    { "S9",     NULL,    4, 9*4,       eEncodingIEEE754 , eFormatFloat,     { e_S9    , e_S9  ,  LLDB_INVALID_REGNUM       , e_S9   , e_S9     }, NULL,              NULL},
    { "S10",    NULL,    4, 10*4,      eEncodingIEEE754, eFormatFloat,      { e_S10   , e_S10 ,  LLDB_INVALID_REGNUM       , e_S10  , e_S10    }, NULL,              NULL},
    { "S11",    NULL,    4, 11*4,      eEncodingIEEE754, eFormatFloat,      { e_S11   , e_S11 ,  LLDB_INVALID_REGNUM       , e_S11  , e_S11    }, NULL,              NULL},
    { "S12",    NULL,    4, 12*4,      eEncodingIEEE754, eFormatFloat,      { e_S12   , e_S12 ,  LLDB_INVALID_REGNUM       , e_S12  , e_S12    }, NULL,              NULL},
    { "S13",    NULL,    4, 13*4,      eEncodingIEEE754, eFormatFloat,      { e_S13   , e_S13 ,  LLDB_INVALID_REGNUM       , e_S13  , e_S13    }, NULL,              NULL},
    { "S14",    NULL,    4, 14*4,      eEncodingIEEE754, eFormatFloat,      { e_S14   , e_S14 ,  LLDB_INVALID_REGNUM       , e_S14  , e_S14    }, NULL,              NULL},
    { "S15",    NULL,    4, 15*4,      eEncodingIEEE754, eFormatFloat,      { e_S15   , e_S15 ,  LLDB_INVALID_REGNUM       , e_S15  , e_S15    }, NULL,              NULL},
    { "S16",    NULL,    4, 16*4,      eEncodingIEEE754, eFormatFloat,      { e_S16   , e_S16 ,  LLDB_INVALID_REGNUM       , e_S16  , e_S16    }, NULL,              NULL},
    { "S17",    NULL,    4, 17*4,      eEncodingIEEE754, eFormatFloat,      { e_S17   , e_S17 ,  LLDB_INVALID_REGNUM       , e_S17  , e_S17    }, NULL,              NULL},
    { "S18",    NULL,    4, 18*4,      eEncodingIEEE754, eFormatFloat,      { e_S18   , e_S18 ,  LLDB_INVALID_REGNUM       , e_S18  , e_S18    }, NULL,              NULL},
    { "S19",    NULL,    4, 19*4,      eEncodingIEEE754, eFormatFloat,      { e_S19   , e_S19 ,  LLDB_INVALID_REGNUM       , e_S19  , e_S19    }, NULL,              NULL},
    { "S20",    NULL,    4, 20*4,      eEncodingIEEE754, eFormatFloat,      { e_S20   , e_S20 ,  LLDB_INVALID_REGNUM       , e_S20  , e_S20    }, NULL,              NULL},
    { "S21",    NULL,    4, 21*4,      eEncodingIEEE754, eFormatFloat,      { e_S21   , e_S21 ,  LLDB_INVALID_REGNUM       , e_S21  , e_S21    }, NULL,              NULL},
    { "S22",    NULL,    4, 22*4,      eEncodingIEEE754, eFormatFloat,      { e_S22   , e_S22 ,  LLDB_INVALID_REGNUM       , e_S22  , e_S22    }, NULL,              NULL},
    { "S23",    NULL,    4, 23*4,      eEncodingIEEE754, eFormatFloat,      { e_S23   , e_S23 ,  LLDB_INVALID_REGNUM       , e_S23  , e_S23    }, NULL,              NULL},
    { "S24",    NULL,    4, 24*4,      eEncodingIEEE754, eFormatFloat,      { e_S24   , e_S24 ,  LLDB_INVALID_REGNUM       , e_S24  , e_S24    }, NULL,              NULL},
    { "S25",    NULL,    4, 25*4,      eEncodingIEEE754, eFormatFloat,      { e_S25   , e_S25 ,  LLDB_INVALID_REGNUM       , e_S25  , e_S25    }, NULL,              NULL},
    { "S26",    NULL,    4, 26*4,      eEncodingIEEE754, eFormatFloat,      { e_S26   , e_S26 ,  LLDB_INVALID_REGNUM       , e_S26  , e_S26    }, NULL,              NULL},
    { "S27",    NULL,    4, 27*4,      eEncodingIEEE754, eFormatFloat,      { e_S27   , e_S27 ,  LLDB_INVALID_REGNUM       , e_S27  , e_S27    }, NULL,              NULL},
    { "S28",    NULL,    4, 28*4,      eEncodingIEEE754, eFormatFloat,      { e_S28   , e_S28 ,  LLDB_INVALID_REGNUM       , e_S28  , e_S28    }, NULL,              NULL},
    { "S29",    NULL,    4, 29*4,      eEncodingIEEE754, eFormatFloat,      { e_S29   , e_S29 ,  LLDB_INVALID_REGNUM       , e_S29  , e_S29    }, NULL,              NULL},
    { "S30",    NULL,    4, 30*4,      eEncodingIEEE754, eFormatFloat,      { e_S30   , e_S30 ,  LLDB_INVALID_REGNUM       , e_S30  , e_S30    }, NULL,              NULL},
    { "S31",    NULL,    4, 31*4,      eEncodingIEEE754, eFormatFloat,      { e_S31   , e_S31 ,  LLDB_INVALID_REGNUM       , e_R31  , e_R31    }, NULL,              NULL},

    { "DR0",    NULL,    8, 0*8,       eEncodingIEEE754 , eFormatFloat,     { e_DR0   , e_DR0 ,  LLDB_INVALID_REGNUM       , e_DR0   , e_DR0   }, NULL,              NULL},
    { "DR1",    NULL,    8, 1*8,       eEncodingIEEE754 , eFormatFloat,     { e_DR1   , e_DR1 ,  LLDB_INVALID_REGNUM       , e_DR1   , e_DR1   }, NULL,              NULL},
    { "DR2",    NULL,    8, 2*8,       eEncodingIEEE754 , eFormatFloat,     { e_DR2   , e_DR2 ,  LLDB_INVALID_REGNUM       , e_DR2   , e_DR2   }, NULL,              NULL},
    { "DR3",    NULL,    8, 3*8,       eEncodingIEEE754 , eFormatFloat,     { e_DR3   , e_DR3 ,  LLDB_INVALID_REGNUM       , e_DR3   , e_DR3   }, NULL,              NULL},
    { "DR4",    NULL,    8, 4*8,       eEncodingIEEE754 , eFormatFloat,     { e_DR4   , e_DR4 ,  LLDB_INVALID_REGNUM       , e_DR4   , e_DR4   }, NULL,              NULL},
    { "DR5",    NULL,    8, 5*8,       eEncodingIEEE754 , eFormatFloat,     { e_DR5   , e_DR5 ,  LLDB_INVALID_REGNUM       , e_DR5   , e_DR5   }, NULL,              NULL},
    { "DR6",    NULL,    8, 6*8,       eEncodingIEEE754 , eFormatFloat,     { e_DR6   , e_DR6 ,  LLDB_INVALID_REGNUM       , e_DR6   , e_DR6   }, NULL,              NULL},
    { "DR7",    NULL,    8, 7*8,       eEncodingIEEE754 , eFormatFloat,     { e_DR7   , e_DR7 ,  LLDB_INVALID_REGNUM       , e_DR7   , e_DR7   }, NULL,              NULL},
    { "DR8",    NULL,    8, 8*8,       eEncodingIEEE754 , eFormatFloat,     { e_DR8   , e_DR8 ,  LLDB_INVALID_REGNUM       , e_DR8   , e_DR8   }, NULL,              NULL},
    { "DR9",    NULL,    8, 9*8,       eEncodingIEEE754 , eFormatFloat,     { e_DR9   , e_DR9 ,  LLDB_INVALID_REGNUM       , e_DR9   , e_DR9   }, NULL,              NULL},
    { "DR10",   NULL,    8, 10*8,      eEncodingIEEE754, eFormatFloat,      { e_DR10  , e_DR10,  LLDB_INVALID_REGNUM       , e_DR10  , e_DR10  }, NULL,              NULL},
    { "DR11",   NULL,    8, 11*8,      eEncodingIEEE754, eFormatFloat,      { e_DR11  , e_DR11,  LLDB_INVALID_REGNUM       , e_DR11  , e_DR11  }, NULL,              NULL},
    { "DR12",   NULL,    8, 12*8,      eEncodingIEEE754, eFormatFloat,      { e_DR12  , e_DR12,  LLDB_INVALID_REGNUM       , e_DR12  , e_DR12  }, NULL,              NULL},
    { "DR13",   NULL,    8, 13*8,      eEncodingIEEE754, eFormatFloat,      { e_DR13  , e_DR13,  LLDB_INVALID_REGNUM       , e_DR13  , e_DR13  }, NULL,              NULL},
    { "DR14",   NULL,    8, 14*8,      eEncodingIEEE754, eFormatFloat,      { e_DR14  , e_DR14,  LLDB_INVALID_REGNUM       , e_DR14  , e_DR14  }, NULL,              NULL},
    { "DR15",   NULL,    8, 15*8,      eEncodingIEEE754, eFormatFloat,      { e_DR15  , e_DR15,  LLDB_INVALID_REGNUM       , e_DR15  , e_DR15  }, NULL,              NULL},

    { "DPC",    "pc",    4, (64+0)*4,  eEncodingUint, eFormatPointer,       { e_DPC   , e_J30 ,  LLDB_REGNUM_GENERIC_PC    , e_J30   , e_J30   }, NULL,              NULL},
    { "RRPC",   "ppc",   4, (64+1)*4,  eEncodingUint, eFormatPointer,       { e_RRPC  , e_J31 ,  LLDB_REGNUM_GENERIC_FLAGS , e_J31   , e_J31   }, NULL,              NULL},
};

static const uint32_t k_num_register_infos = sizeof(g_register_infos)/sizeof(RegisterInfo);
static bool g_register_info_names_constified = false;

const lldb_private::RegisterInfo *
ABINone_mspu::GetRegisterInfoArray (uint32_t &count)
{
    // Make the C-string names and alt_names for the register infos into const 
    // C-string values by having the ConstString unique the names in the global
    // constant C-string pool.
    if (!g_register_info_names_constified)
    {
        g_register_info_names_constified = true;
        for (uint32_t i=0; i<k_num_register_infos; ++i)
        {
            if (g_register_infos[i].name)
                g_register_infos[i].name = ConstString(g_register_infos[i].name).GetCString();
            if (g_register_infos[i].alt_name)
                g_register_infos[i].alt_name = ConstString(g_register_infos[i].alt_name).GetCString();
        }
    }

    count = k_num_register_infos;
    return g_register_infos;
}

size_t
ABINone_mspu::GetRedZoneSize () const
{
    return 0;
}

//------------------------------------------------------------------
// Static Functions
//------------------------------------------------------------------
ABISP
ABINone_mspu::CreateInstance (const ArchSpec &arch)
{
    static ABISP g_abi_mspu_sp;

    if (arch.GetTriple().getArch() == llvm::Triple::mspu)
    {
      if (!g_abi_mspu_sp)
        g_abi_mspu_sp.reset (new ABINone_mspu());

      return g_abi_mspu_sp;
    }

    return ABISP();
}

bool
ABINone_mspu::PrepareTrivialCall (Thread &thread,
                                    addr_t sp, 
                                    addr_t func_addr, 
                                    addr_t return_addr, 
                                    llvm::ArrayRef<addr_t> args) const
{
    RegisterContext *reg_ctx = thread.GetRegisterContext().get();
    if (!reg_ctx)
        return false;    
    uint32_t pc_reg_num = reg_ctx->ConvertRegisterKindToRegisterNumber (eRegisterKindGeneric, LLDB_REGNUM_GENERIC_PC);
    uint32_t sp_reg_num = reg_ctx->ConvertRegisterKindToRegisterNumber (eRegisterKindGeneric, LLDB_REGNUM_GENERIC_SP);
    
    // When writing a register value down to memory, the register info used 
    // to write memory just needs to have the correct size of a 32 bit register, 
    // the actual register it pertains to is not important, just the size needs 
    // to be correct. Here we use "eax"...
    const RegisterInfo *reg_info_32 = reg_ctx->GetRegisterInfoByName("eax");
    if (!reg_info_32)
        return false; // TODO this should actually never happen

    // Make room for the argument(s) on the stack

    Error error;
    RegisterValue reg_value;
    
    // Write any arguments onto the stack
    sp -= 4 * args.size();
    
    // Align the SP    
    sp &= ~(16ull-1ull); // 16-byte alignment
    
    addr_t arg_pos = sp;
    
    for (addr_t arg : args)
    {
        reg_value.SetUInt32(arg);
        error = reg_ctx->WriteRegisterValueToMemory (reg_info_32,
                                                     arg_pos,
                                                     reg_info_32->byte_size,
                                                     reg_value);
        if (error.Fail())
            return false;
        arg_pos += 4;
    }
    
    // The return address is pushed onto the stack (yes after we just set the
    // alignment above!).
    sp -= 4;
    reg_value.SetUInt32(return_addr);
    error = reg_ctx->WriteRegisterValueToMemory (reg_info_32, 
                                                 sp, 
                                                 reg_info_32->byte_size, 
                                                 reg_value);
    if (error.Fail())
        return false;
    
    // %esp is set to the actual stack value.
    
    if (!reg_ctx->WriteRegisterFromUnsigned (sp_reg_num, sp))
        return false;
    
    // %eip is set to the address of the called function.
    
    if (!reg_ctx->WriteRegisterFromUnsigned (pc_reg_num, func_addr))
        return false;
    
    return true;
}

static bool 
ReadIntegerArgument (Scalar           &scalar,
                     unsigned int     bit_width,
                     bool             is_signed,
                     Process          *process,
                     addr_t           &current_stack_argument)
{
    
    uint32_t byte_size = (bit_width + (8-1))/8;
    Error error;
    if (process->ReadScalarIntegerFromMemory(current_stack_argument, byte_size, is_signed, scalar, error))
    {
        current_stack_argument += byte_size;
        return true;
    }
    return false;
}

bool
ABINone_mspu::GetArgumentValues (Thread &thread,
                                   ValueList &values) const
{
    unsigned int num_values = values.GetSize();
    unsigned int value_index;
    
    // Get the pointer to the first stack argument so we have a place to start 
    // when reading data
    
    RegisterContext *reg_ctx = thread.GetRegisterContext().get();
    
    if (!reg_ctx)
        return false;
    
    addr_t sp = reg_ctx->GetSP(0);
    
    if (!sp)
        return false;
    
    addr_t current_stack_argument = sp + 4; // jump over return address
    
    for (value_index = 0;
         value_index < num_values;
         ++value_index)
    {
        Value *value = values.GetValueAtIndex(value_index);
        
        if (!value)
            return false;
        
        // We currently only support extracting values with Clang QualTypes.
        // Do we care about others?
        CompilerType compiler_type (value->GetCompilerType());
        if (compiler_type)
        {
            bool is_signed;
            
            if (compiler_type.IsIntegerType (is_signed))
            {
                ReadIntegerArgument(value->GetScalar(),
                                    compiler_type.GetBitSize(&thread),
                                    is_signed,
                                    thread.GetProcess().get(), 
                                    current_stack_argument);
            }
            else if (compiler_type.IsPointerType())
            {
                ReadIntegerArgument(value->GetScalar(),
                                    compiler_type.GetBitSize(&thread),
                                    false,
                                    thread.GetProcess().get(),
                                    current_stack_argument);
            }
        }
    }
    
    return true;
}

Error
ABINone_mspu::SetReturnValueObject(lldb::StackFrameSP &frame_sp, lldb::ValueObjectSP &new_value_sp)
{
    Error error;
    if (!new_value_sp)
    {
        error.SetErrorString("Empty value object for return value.");
        return error;
    }
    
    CompilerType compiler_type = new_value_sp->GetCompilerType();
    if (!compiler_type)
    {
        error.SetErrorString ("Null clang type for return value.");
        return error;
    }
    
    Thread *thread = frame_sp->GetThread().get();
    
    bool is_signed;
    uint32_t count;
    bool is_complex;
    
    RegisterContext *reg_ctx = thread->GetRegisterContext().get();

    bool set_it_simple = false;
    if (compiler_type.IsIntegerType (is_signed) || compiler_type.IsPointerType())
    {
        DataExtractor data;
        size_t num_bytes = new_value_sp->GetData(data, error);
        lldb::offset_t offset = 0;
        if (num_bytes <= 8)
        {
            const RegisterInfo *eax_info = reg_ctx->GetRegisterInfoByName("eax", 0);
            if (num_bytes <= 4)
            {
                uint32_t raw_value = data.GetMaxU32(&offset, num_bytes);
        
                if (reg_ctx->WriteRegisterFromUnsigned (eax_info, raw_value))
                    set_it_simple = true;
            }
            else
            {
                uint32_t raw_value = data.GetMaxU32(&offset, 4);
        
                if (reg_ctx->WriteRegisterFromUnsigned (eax_info, raw_value))
                {
                    const RegisterInfo *edx_info = reg_ctx->GetRegisterInfoByName("edx", 0);
                    uint32_t raw_value = data.GetMaxU32(&offset, num_bytes - offset);
                
                    if (reg_ctx->WriteRegisterFromUnsigned (edx_info, raw_value))
                        set_it_simple = true;
                }
            }
        }
        else
        {
            error.SetErrorString("We don't support returning longer than 64 bit integer values at present.");
        }
    }
    else if (compiler_type.IsFloatingPointType (count, is_complex))
    {
        if (is_complex)
            error.SetErrorString ("We don't support returning complex values at present");
        else
            error.SetErrorString ("We don't support returning float values at present");
    }
    
    if (!set_it_simple)
        error.SetErrorString ("We only support setting simple integer return types at present.");
    
    return error;
}

ValueObjectSP
ABINone_mspu::GetReturnValueObjectImpl (Thread &thread,
                                          CompilerType &compiler_type) const
{
    Value value;
    ValueObjectSP return_valobj_sp;
    
    if (!compiler_type)
        return return_valobj_sp;
    
    //value.SetContext (Value::eContextTypeClangType, compiler_type.GetOpaqueQualType());
    value.SetCompilerType (compiler_type);
    
    RegisterContext *reg_ctx = thread.GetRegisterContext().get();
        if (!reg_ctx)
        return return_valobj_sp;
        
    bool is_signed;
            
    if (compiler_type.IsIntegerType (is_signed))
    {
        size_t bit_width = compiler_type.GetBitSize(&thread);
        
        unsigned eax_id = reg_ctx->GetRegisterInfoByName("eax", 0)->kinds[eRegisterKindLLDB];
        unsigned edx_id = reg_ctx->GetRegisterInfoByName("edx", 0)->kinds[eRegisterKindLLDB];
        
        switch (bit_width)
        {
            default:
            case 128:
                // Scalar can't hold 128-bit literals, so we don't handle this
                return return_valobj_sp;
            case 64:
                uint64_t raw_value;
                raw_value = thread.GetRegisterContext()->ReadRegisterAsUnsigned(eax_id, 0) & 0xffffffff;
                raw_value |= (thread.GetRegisterContext()->ReadRegisterAsUnsigned(edx_id, 0) & 0xffffffff) << 32;
                if (is_signed)
                    value.GetScalar() = (int64_t)raw_value;
                else
                    value.GetScalar() = (uint64_t)raw_value;
                break;
            case 32:
                if (is_signed)
                    value.GetScalar() = (int32_t)(thread.GetRegisterContext()->ReadRegisterAsUnsigned(eax_id, 0) & 0xffffffff);
                else
                    value.GetScalar() = (uint32_t)(thread.GetRegisterContext()->ReadRegisterAsUnsigned(eax_id, 0) & 0xffffffff);
                break;
            case 16:
                if (is_signed)
                    value.GetScalar() = (int16_t)(thread.GetRegisterContext()->ReadRegisterAsUnsigned(eax_id, 0) & 0xffff);
                else
                    value.GetScalar() = (uint16_t)(thread.GetRegisterContext()->ReadRegisterAsUnsigned(eax_id, 0) & 0xffff);
                break;
            case 8:
                if (is_signed)
                    value.GetScalar() = (int8_t)(thread.GetRegisterContext()->ReadRegisterAsUnsigned(eax_id, 0) & 0xff);
                else
                    value.GetScalar() = (uint8_t)(thread.GetRegisterContext()->ReadRegisterAsUnsigned(eax_id, 0) & 0xff);
                break;
        }
    }
    else if (compiler_type.IsPointerType ())
    {
        unsigned eax_id = reg_ctx->GetRegisterInfoByName("eax", 0)->kinds[eRegisterKindLLDB];
        uint32_t ptr = thread.GetRegisterContext()->ReadRegisterAsUnsigned(eax_id, 0) & 0xffffffff;
        value.GetScalar() = ptr;
    }
    else
    {
        // not handled yet
        return return_valobj_sp;
    }
    
    // If we get here, we have a valid Value, so make our ValueObject out of it:
    
    return_valobj_sp = ValueObjectConstResult::Create(thread.GetStackFrameAtIndex(0).get(),
                                                      value,
                                                      ConstString(""));
    return return_valobj_sp;
}

// this is created on function entry, i.e., no instruction of this function
// has been executed. So this function must be at stack top.
bool
ABINone_mspu::CreateFunctionEntryUnwindPlan (UnwindPlan &unwind_plan)
{
    unwind_plan.Clear();
    unwind_plan.SetRegisterKind (eRegisterKindDWARF);
    unwind_plan.SetReturnAddressRegister(dwarf_ra);

    UnwindPlan::RowSP row(new UnwindPlan::Row);

    // Our Call Frame Address is the stack pointer value
    row->GetCFAValue().SetIsRegisterPlusOffset(dwarf_sp, 0);

    row->SetRegisterLocationToAtCFAPlusOffset(dwarf_pc, -4, false);

    unwind_plan.AppendRow (row);
    unwind_plan.SetSourceName ("mspu at-func-entry default");
    unwind_plan.SetSourcedFromCompiler (eLazyBoolNo);

    return true;
}

/* prologue for spu function
 *
 * 0:   [SP + -4] = FP;;
 * 4:   [SP + -8] = J30;;
 * 8:   FP = SP;;
 * 12:  J31 = -32;;
 * 16:  SP = SP + J31;;
 * 20:  ......
 */

bool
ABINone_mspu::CreateDefaultUnwindPlan (UnwindPlan &unwind_plan)
{
    unwind_plan.Clear();
    unwind_plan.SetRegisterKind (eRegisterKindDWARF);
    unwind_plan.SetReturnAddressRegister(dwarf_ra);

    UnwindPlan::RowSP row(new UnwindPlan::Row);
    const int32_t ptr_size = 4;

    // if offset >= 20, current frame is already built up.

    // Our Call Frame Address is the stack pointer value
    row->GetCFAValue().SetIsRegisterPlusOffset(dwarf_fp, 0);

    row->SetOffset (20);
    
    row->SetRegisterLocationToAtCFAPlusOffset(dwarf_fp, ptr_size * -1, true);
    row->SetRegisterLocationToAtCFAPlusOffset(dwarf_ra, ptr_size * -2, true);
    row->SetRegisterLocationToIsCFAPlusOffset(dwarf_sp, 0, true);

    unwind_plan.AppendRow (row);
    unwind_plan.SetSourceName ("mspu default unwind plan");
    unwind_plan.SetSourcedFromCompiler (eLazyBoolNo);
    unwind_plan.SetUnwindPlanValidAtAllInstructions (eLazyBoolNo);
    return true;
}

bool
ABINone_mspu::RegisterIsVolatile (const RegisterInfo *reg_info)
{
    return !RegisterIsCalleeSaved (reg_info);
}

bool
ABINone_mspu::RegisterIsCalleeSaved (const RegisterInfo *reg_info)
{
    if (reg_info)
    {
        // Callee-Saved registers are R16~R31, J16~J31, S16~S31, DR8~DR15
        const char *name = reg_info->name;
        long int idx;
        switch (name[0]) {
        case 'R':
        case 'J':
        case 'S':
          idx = ::strtol(name+1, NULL, 10);
          return (16 <= idx && idx <= 31);

        case 'D':
          if(name[1] == 'R') {
            idx = ::strtol(name+2, NULL, 10);
            return (8 <= idx && idx <= 15);
          }

        default:
          return false;
        }
    }

    return false;
}

void
ABINone_mspu::Initialize()
{
    PluginManager::RegisterPlugin (GetPluginNameStatic(),
                                   "ABI for MaPU SPU targets",
                                   CreateInstance);    
}

void
ABINone_mspu::Terminate()
{
    PluginManager::UnregisterPlugin (CreateInstance);
}

lldb_private::ConstString
ABINone_mspu::GetPluginNameStatic ()
{
    static ConstString g_short_name("abi.none-mspu");
    return g_short_name;
}

//------------------------------------------------------------------
// PluginInterface protocol
//------------------------------------------------------------------
lldb_private::ConstString
ABINone_mspu::GetPluginName()
{
    return GetPluginNameStatic();
}

uint32_t
ABINone_mspu::GetPluginVersion()
{
    return 1;
}
