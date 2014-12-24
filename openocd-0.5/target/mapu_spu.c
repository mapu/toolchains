/*
 * mapu_spu.c
 *
 *  Created on: Mar 5, 2012
 *      Author: root
 */
#include <config.h>
#include <target/mapu_spu.h>
#include <helper/types.h>
#include <helper/log.h>
#include <string.h>
#include <stdio.h>
#include <target/hello.h>
#include "target_type.h"
#include "target.h"
#include <target/register.h>
#include <target/breakpoints.h>
#include <unistd.h>

/* Note:
 * MaPU supports 2 kind of mode: debug mode and run mode. In each mode, MaPU can transist to
 * halted state (breakpoint/watchpoint, etc) or running state. In run mode, MaPU can be halted,
 * but memory/register access is not allowed since read/write port is not connected to OCD.
 * Remeber that debug mode of MaPU has a different meaning from what what defined as usual.
 */

// corresponding to register set
static struct mapu_spu_reg_type mapu_spu_reg_types [] =
{  { es_unknown, "Unknown Register",          ef_unknown, "unknown", ec_unknown, "unknown"},
   // i32
   { es_int,     "Integer Register",          ef_hex,     "hex",     ec_sint,    "sint"},
   // f32
   { es_sflt,    "Single Precise FP Register",ef_float,   "float",   ec_ieee754, "ieee754"},
   // f64
   {  es_dflt,   "Double Precise FP Register",ef_float,   "float",   ec_ieee754, "ieee754"},
   // address
   {  es_addr,   "Address Register" ,         ef_hex,     "hex",     ec_uint,    "uint"},
};

/*struct mapu_spu_reg_info  // record architecture-specific info for a 'struct reg' object
{
    struct mapu_spu_reg_type * type_info;
    const char *alt_name;

    uint32_t  no;   // this is also used as dwarf/gcc_no and its index;
    uint32_t  addr; // memory mapped address in SPU for this reg via jtag access

    // D1 = {R1, R0}, we call D1/R1 or D1/R0 contains each other.
    struct reg **container_regs;
    uint32_t   num_container_regs;

    // same as container registers for SPU, not used for now
    struct reg **invalidate_regs;
    uint32_t   num_invalidate_regs;

    struct target *target;
};*/

static struct mapu_spu_reg_info mapu_spu_int_reg_infos[] =
{
 { &mapu_spu_reg_types[es_int], "arg1", e_R0 , R_REG_MEM_MAP_BASE+(0<<2) , {e_S0, e_DR0}, 2, {e_S0}, 1 },
 { &mapu_spu_reg_types[es_int], "arg2", e_R1 , R_REG_MEM_MAP_BASE+(1<<2) , {e_S1, e_DR0}, 2, {e_S1}, 1 },
 { &mapu_spu_reg_types[es_int], "arg3", e_R2 , R_REG_MEM_MAP_BASE+(2<<2) , {e_S2, e_DR1}, 2, {e_S2}, 1 },
 { &mapu_spu_reg_types[es_int], "arg4", e_R3 , R_REG_MEM_MAP_BASE+(3<<2) , {e_S3, e_DR1}, 2, {e_S3}, 1 },
 { &mapu_spu_reg_types[es_int], "arg5", e_R4 , R_REG_MEM_MAP_BASE+(4<<2) , {e_S4, e_DR2}, 2, {e_S4}, 1 },
 { &mapu_spu_reg_types[es_int], "arg6", e_R5 , R_REG_MEM_MAP_BASE+(5<<2) , {e_S5, e_DR2}, 2, {e_S5}, 1 },
 { &mapu_spu_reg_types[es_int], "arg7", e_R6 , R_REG_MEM_MAP_BASE+(6<<2) , {e_S6, e_DR3}, 2, {e_S6}, 1 },
 { &mapu_spu_reg_types[es_int], "arg8", e_R7 , R_REG_MEM_MAP_BASE+(7<<2) , {e_S7, e_DR3}, 2, {e_S7}, 1 },
 { &mapu_spu_reg_types[es_int], 0     , e_R8 , R_REG_MEM_MAP_BASE+(8<<2) , {e_S8, e_DR4}, 2, {e_S8}, 1 },
 { &mapu_spu_reg_types[es_int], 0     , e_R9 , R_REG_MEM_MAP_BASE+(9<<2) , {e_S9, e_DR4}, 2, {e_S9}, 1 },
 { &mapu_spu_reg_types[es_int], 0     , e_R10, R_REG_MEM_MAP_BASE+(10<<2), {e_S10, e_DR5}, 2, {e_S10}, 1 },
 { &mapu_spu_reg_types[es_int], 0     , e_R11, R_REG_MEM_MAP_BASE+(11<<2), {e_S11, e_DR5}, 2, {e_S11}, 1 },
 { &mapu_spu_reg_types[es_int], 0     , e_R12, R_REG_MEM_MAP_BASE+(12<<2), {e_S12, e_DR6}, 2, {e_S12}, 1 },
 { &mapu_spu_reg_types[es_int], 0     , e_R13, R_REG_MEM_MAP_BASE+(13<<2), {e_S13, e_DR6}, 2, {e_S13}, 1 },
 { &mapu_spu_reg_types[es_int], 0     , e_R14, R_REG_MEM_MAP_BASE+(14<<2), {e_S14, e_DR7}, 2, {e_S14}, 1 },
 { &mapu_spu_reg_types[es_int], 0     , e_R15, R_REG_MEM_MAP_BASE+(15<<2), {e_S15, e_DR7}, 2, {e_S15}, 1 },
 { &mapu_spu_reg_types[es_int], 0     , e_R16, R_REG_MEM_MAP_BASE+(16<<2), {e_S16, e_DR8}, 2, {e_S16}, 1 },
 { &mapu_spu_reg_types[es_int], 0     , e_R17, R_REG_MEM_MAP_BASE+(17<<2), {e_S17, e_DR8}, 2, {e_S17}, 1 },
 { &mapu_spu_reg_types[es_int], 0     , e_R18, R_REG_MEM_MAP_BASE+(18<<2), {e_S18, e_DR9}, 2, {e_S18}, 1 },
 { &mapu_spu_reg_types[es_int], 0     , e_R19, R_REG_MEM_MAP_BASE+(19<<2), {e_S19, e_DR9}, 2, {e_S19}, 1 },
 { &mapu_spu_reg_types[es_int], 0     , e_R20, R_REG_MEM_MAP_BASE+(20<<2), {e_S20, e_DR10}, 2, {e_S20}, 1 },
 { &mapu_spu_reg_types[es_int], 0     , e_R21, R_REG_MEM_MAP_BASE+(21<<2), {e_S21, e_DR10}, 2, {e_S21}, 1 },
 { &mapu_spu_reg_types[es_int], 0     , e_R22, R_REG_MEM_MAP_BASE+(22<<2), {e_S22, e_DR11}, 2, {e_S22}, 1 },
 { &mapu_spu_reg_types[es_int], 0     , e_R23, R_REG_MEM_MAP_BASE+(23<<2), {e_S23, e_DR11}, 2, {e_S23}, 1 },
 { &mapu_spu_reg_types[es_int], 0     , e_R24, R_REG_MEM_MAP_BASE+(24<<2), {e_S24, e_DR12}, 2, {e_S24}, 1 },
 { &mapu_spu_reg_types[es_int], 0     , e_R25, R_REG_MEM_MAP_BASE+(25<<2), {e_S25, e_DR12}, 2, {e_S25}, 1 },
 { &mapu_spu_reg_types[es_int], 0     , e_R26, R_REG_MEM_MAP_BASE+(26<<2), {e_S26, e_DR13}, 2, {e_S26}, 1 },
 { &mapu_spu_reg_types[es_int], 0     , e_R27, R_REG_MEM_MAP_BASE+(27<<2), {e_S27, e_DR13}, 2, {e_S27}, 1 },
 { &mapu_spu_reg_types[es_int], 0     , e_R28, R_REG_MEM_MAP_BASE+(28<<2), {e_S28, e_DR14}, 2, {e_S28}, 1 },
 { &mapu_spu_reg_types[es_int], 0     , e_R29, R_REG_MEM_MAP_BASE+(29<<2), {e_S29, e_DR14}, 2, {e_S29}, 1 },
 { &mapu_spu_reg_types[es_int], 0     , e_R30, R_REG_MEM_MAP_BASE+(30<<2), {e_S30, e_DR15}, 2, {e_S30}, 1 },
 { &mapu_spu_reg_types[es_int], 0     , e_R31, R_REG_MEM_MAP_BASE+(31<<2), {e_S31, e_DR15}, 2, {e_S31}, 1 }
};

static struct mapu_spu_reg_info mapu_spu_addr_reg_infos[] =
{
 { &mapu_spu_reg_types[es_addr], 0, e_J0, J_REG_MEM_MAP_BASE+(0<<2), {}, 0, {}, 0 },
 { &mapu_spu_reg_types[es_addr], 0, e_J1, J_REG_MEM_MAP_BASE+(1<<2), {}, 0, {}, 0 },
 { &mapu_spu_reg_types[es_addr], 0, e_J2, J_REG_MEM_MAP_BASE+(2<<2), {}, 0, {}, 0 },
 { &mapu_spu_reg_types[es_addr], 0, e_J3, J_REG_MEM_MAP_BASE+(3<<2), {}, 0, {}, 0 },
 { &mapu_spu_reg_types[es_addr], 0, e_J4, J_REG_MEM_MAP_BASE+(4<<2), {}, 0, {}, 0 },
 { &mapu_spu_reg_types[es_addr], 0, e_J5, J_REG_MEM_MAP_BASE+(5<<2), {}, 0, {}, 0 },
 { &mapu_spu_reg_types[es_addr], 0, e_J6, J_REG_MEM_MAP_BASE+(6<<2), {}, 0, {}, 0 },
 { &mapu_spu_reg_types[es_addr], 0, e_J7, J_REG_MEM_MAP_BASE+(7<<2), {}, 0, {}, 0 },
 { &mapu_spu_reg_types[es_addr], 0, e_J8, J_REG_MEM_MAP_BASE+(8<<2), {}, 0, {}, 0 },
 { &mapu_spu_reg_types[es_addr], 0, e_J9, J_REG_MEM_MAP_BASE+(9<<2), {}, 0, {}, 0 },
 { &mapu_spu_reg_types[es_addr], 0, e_J10, J_REG_MEM_MAP_BASE+(10<<2), {}, 0, {}, 0 },
 { &mapu_spu_reg_types[es_addr], 0, e_J11, J_REG_MEM_MAP_BASE+(11<<2), {}, 0, {}, 0 },
 { &mapu_spu_reg_types[es_addr], 0, e_J12, J_REG_MEM_MAP_BASE+(12<<2), {}, 0, {}, 0 },
 { &mapu_spu_reg_types[es_addr], 0, e_J13, J_REG_MEM_MAP_BASE+(13<<2), {}, 0, {}, 0 },
 { &mapu_spu_reg_types[es_addr], 0, e_J14, J_REG_MEM_MAP_BASE+(14<<2), {}, 0, {}, 0 },
 { &mapu_spu_reg_types[es_addr], 0, e_J15, J_REG_MEM_MAP_BASE+(15<<2), {}, 0, {}, 0 },
 { &mapu_spu_reg_types[es_addr], 0, e_J16, J_REG_MEM_MAP_BASE+(16<<2), {}, 0, {}, 0 },
 { &mapu_spu_reg_types[es_addr], 0, e_J17, J_REG_MEM_MAP_BASE+(17<<2), {}, 0, {}, 0 },
 { &mapu_spu_reg_types[es_addr], 0, e_J18, J_REG_MEM_MAP_BASE+(18<<2), {}, 0, {}, 0 },
 { &mapu_spu_reg_types[es_addr], 0, e_J19, J_REG_MEM_MAP_BASE+(19<<2), {}, 0, {}, 0 },
 { &mapu_spu_reg_types[es_addr], 0, e_J20, J_REG_MEM_MAP_BASE+(20<<2), {}, 0, {}, 0 },
 { &mapu_spu_reg_types[es_addr], 0, e_J21, J_REG_MEM_MAP_BASE+(21<<2), {}, 0, {}, 0 },
 { &mapu_spu_reg_types[es_addr], 0, e_J22, J_REG_MEM_MAP_BASE+(22<<2), {}, 0, {}, 0 },
 { &mapu_spu_reg_types[es_addr], 0, e_J23, J_REG_MEM_MAP_BASE+(23<<2), {}, 0, {}, 0 },
 { &mapu_spu_reg_types[es_addr], 0, e_J24, J_REG_MEM_MAP_BASE+(24<<2), {}, 0, {}, 0 },
 { &mapu_spu_reg_types[es_addr], 0, e_J25, J_REG_MEM_MAP_BASE+(25<<2), {}, 0, {}, 0 },
 { &mapu_spu_reg_types[es_addr], 0, e_J26, J_REG_MEM_MAP_BASE+(26<<2), {}, 0, {}, 0 },
 { &mapu_spu_reg_types[es_addr], 0, e_J27, J_REG_MEM_MAP_BASE+(27<<2), {}, 0, {}, 0 },
 { &mapu_spu_reg_types[es_addr], "fp", e_J28, J_REG_MEM_MAP_BASE+(28<<2), {}, 0, {}, 0 },
 { &mapu_spu_reg_types[es_addr], "sp", e_J29, J_REG_MEM_MAP_BASE+(29<<2), {}, 0, {}, 0 },
 { &mapu_spu_reg_types[es_addr], "ra", e_J30, J_REG_MEM_MAP_BASE+(30<<2), {}, 0, {}, 0 },
 { &mapu_spu_reg_types[es_addr], 0,    e_J31, J_REG_MEM_MAP_BASE+(31<<2), {}, 0, {}, 0 }
};

static struct mapu_spu_reg_info mapu_spu_dpc_reg_info =
 { &mapu_spu_reg_types[es_addr], "pc",    e_DPC,  DPC_MEM_MAP,  {}, 0, {}, 0 };

static struct mapu_spu_reg_info mapu_spu_rrpc_reg_info =
 { &mapu_spu_reg_types[es_addr], "flags", e_RRPC, RRPC_MEM_MAP, {}, 0, {}, 0 };

static struct mapu_spu_reg_info mapu_spu_sflt_reg_infos[] =
{
 { &mapu_spu_reg_types[es_sflt], 0, e_S0, R_REG_MEM_MAP_BASE+(0<<2), {e_R0, e_DR0}, 2, {e_R0}, 1 },
 { &mapu_spu_reg_types[es_sflt], 0, e_S1, R_REG_MEM_MAP_BASE+(1<<2), {e_R1, e_DR0}, 2, {e_R1}, 1 },
 { &mapu_spu_reg_types[es_sflt], 0, e_S2, R_REG_MEM_MAP_BASE+(2<<2), {e_R2, e_DR1}, 2, {e_R2}, 1 },
 { &mapu_spu_reg_types[es_sflt], 0, e_S3, R_REG_MEM_MAP_BASE+(3<<2), {e_R3, e_DR1}, 2, {e_R3}, 1 },
 { &mapu_spu_reg_types[es_sflt], 0, e_S4, R_REG_MEM_MAP_BASE+(4<<2), {e_R4, e_DR2}, 2, {e_R4}, 1 },
 { &mapu_spu_reg_types[es_sflt], 0, e_S5, R_REG_MEM_MAP_BASE+(5<<2), {e_R5, e_DR2}, 2, {e_R5}, 1 },
 { &mapu_spu_reg_types[es_sflt], 0, e_S6, R_REG_MEM_MAP_BASE+(6<<2), {e_R6, e_DR3}, 2, {e_R6}, 1 },
 { &mapu_spu_reg_types[es_sflt], 0, e_S7, R_REG_MEM_MAP_BASE+(7<<2), {e_R7, e_DR3}, 2, {e_R7}, 1 },
 { &mapu_spu_reg_types[es_sflt], 0, e_S8, R_REG_MEM_MAP_BASE+(8<<2), {e_R8, e_DR4}, 2, {e_R8}, 1 },
 { &mapu_spu_reg_types[es_sflt], 0, e_S9, R_REG_MEM_MAP_BASE+(9<<2), {e_R9, e_DR4}, 2, {e_R9}, 1 },
 { &mapu_spu_reg_types[es_sflt], 0, e_S10, R_REG_MEM_MAP_BASE+(10<<2), {e_R10, e_DR5}, 2, {e_R10}, 1 },
 { &mapu_spu_reg_types[es_sflt], 0, e_S11, R_REG_MEM_MAP_BASE+(11<<2), {e_R11, e_DR5}, 2, {e_R11}, 1 },
 { &mapu_spu_reg_types[es_sflt], 0, e_S12, R_REG_MEM_MAP_BASE+(12<<2), {e_R12, e_DR6}, 2, {e_R12}, 1 },
 { &mapu_spu_reg_types[es_sflt], 0, e_S13, R_REG_MEM_MAP_BASE+(13<<2), {e_R13, e_DR6}, 2, {e_R13}, 1 },
 { &mapu_spu_reg_types[es_sflt], 0, e_S14, R_REG_MEM_MAP_BASE+(14<<2), {e_R14, e_DR7}, 2, {e_R14}, 1 },
 { &mapu_spu_reg_types[es_sflt], 0, e_S15, R_REG_MEM_MAP_BASE+(15<<2), {e_R15, e_DR7}, 2, {e_R15}, 1 },
 { &mapu_spu_reg_types[es_sflt], 0, e_S16, R_REG_MEM_MAP_BASE+(16<<2), {e_R16, e_DR8}, 2, {e_R16}, 1 },
 { &mapu_spu_reg_types[es_sflt], 0, e_S17, R_REG_MEM_MAP_BASE+(17<<2), {e_R17, e_DR8}, 2, {e_R17}, 1 },
 { &mapu_spu_reg_types[es_sflt], 0, e_S18, R_REG_MEM_MAP_BASE+(18<<2), {e_R18, e_DR9}, 2, {e_R18}, 1 },
 { &mapu_spu_reg_types[es_sflt], 0, e_S19, R_REG_MEM_MAP_BASE+(19<<2), {e_R19, e_DR9}, 2, {e_R19}, 1 },
 { &mapu_spu_reg_types[es_sflt], 0, e_S20, R_REG_MEM_MAP_BASE+(20<<2), {e_R20, e_DR10}, 2, {e_R20}, 1 },
 { &mapu_spu_reg_types[es_sflt], 0, e_S21, R_REG_MEM_MAP_BASE+(21<<2), {e_R21, e_DR10}, 2, {e_R21}, 1 },
 { &mapu_spu_reg_types[es_sflt], 0, e_S22, R_REG_MEM_MAP_BASE+(22<<2), {e_R22, e_DR11}, 2, {e_R22}, 1 },
 { &mapu_spu_reg_types[es_sflt], 0, e_S23, R_REG_MEM_MAP_BASE+(23<<2), {e_R23, e_DR11}, 2, {e_R23}, 1 },
 { &mapu_spu_reg_types[es_sflt], 0, e_S24, R_REG_MEM_MAP_BASE+(24<<2), {e_R24, e_DR12}, 2, {e_R24}, 1 },
 { &mapu_spu_reg_types[es_sflt], 0, e_S25, R_REG_MEM_MAP_BASE+(25<<2), {e_R25, e_DR12}, 2, {e_R25}, 1 },
 { &mapu_spu_reg_types[es_sflt], 0, e_S26, R_REG_MEM_MAP_BASE+(26<<2), {e_R26, e_DR13}, 2, {e_R26}, 1 },
 { &mapu_spu_reg_types[es_sflt], 0, e_S27, R_REG_MEM_MAP_BASE+(27<<2), {e_R27, e_DR13}, 2, {e_R27}, 1 },
 { &mapu_spu_reg_types[es_sflt], 0, e_S28, R_REG_MEM_MAP_BASE+(28<<2), {e_R28, e_DR14}, 2, {e_R28}, 1 },
 { &mapu_spu_reg_types[es_sflt], 0, e_S29, R_REG_MEM_MAP_BASE+(29<<2), {e_R29, e_DR14}, 2, {e_R29}, 1 },
 { &mapu_spu_reg_types[es_sflt], 0, e_S30, R_REG_MEM_MAP_BASE+(30<<2), {e_R30, e_DR15}, 2, {e_R30}, 1 },
 { &mapu_spu_reg_types[es_sflt], 0, e_S31, R_REG_MEM_MAP_BASE+(31<<2), {e_R31, e_DR15}, 2, {e_R31}, 1 }
};

static struct mapu_spu_reg_info mapu_spu_dflt_reg_infos[] =
{
 { &mapu_spu_reg_types[es_dflt], 0, e_DR0, R_REG_MEM_MAP_BASE+(0<<3), {}, 0, {e_R0, e_R1, e_S0, e_S1}, 4 },
 { &mapu_spu_reg_types[es_dflt], 0, e_DR1, R_REG_MEM_MAP_BASE+(1<<3), {}, 0, {e_R2, e_R3, e_S2, e_S3}, 4 },
 { &mapu_spu_reg_types[es_dflt], 0, e_DR2, R_REG_MEM_MAP_BASE+(2<<3), {}, 0, {e_R4, e_R5, e_S4, e_S5}, 4 },
 { &mapu_spu_reg_types[es_dflt], 0, e_DR3, R_REG_MEM_MAP_BASE+(3<<3), {}, 0, {e_R6, e_R7, e_S6, e_S7}, 4 },
 { &mapu_spu_reg_types[es_dflt], 0, e_DR4, R_REG_MEM_MAP_BASE+(4<<3), {}, 0, {e_R8, e_R9, e_S8, e_S9}, 4 },
 { &mapu_spu_reg_types[es_dflt], 0, e_DR5, R_REG_MEM_MAP_BASE+(5<<3), {}, 0, {e_R10, e_R11, e_S10, e_S11}, 4 },
 { &mapu_spu_reg_types[es_dflt], 0, e_DR6, R_REG_MEM_MAP_BASE+(6<<3), {}, 0, {e_R12, e_R13, e_S12, e_S13}, 4 },
 { &mapu_spu_reg_types[es_dflt], 0, e_DR7, R_REG_MEM_MAP_BASE+(7<<3), {}, 0, {e_R14, e_R15, e_S14, e_S15}, 4 },
 { &mapu_spu_reg_types[es_dflt], 0, e_DR8, R_REG_MEM_MAP_BASE+(8<<3), {}, 0, {e_R16, e_R17, e_S16, e_S17}, 4 },
 { &mapu_spu_reg_types[es_dflt], 0, e_DR9, R_REG_MEM_MAP_BASE+(9<<3), {}, 0, {e_R18, e_R19, e_S18, e_S19}, 4 },
 { &mapu_spu_reg_types[es_dflt], 0, e_DR10, R_REG_MEM_MAP_BASE+(10<<3), {}, 0, {e_R20, e_R21, e_S20, e_S21}, 4 },
 { &mapu_spu_reg_types[es_dflt], 0, e_DR11, R_REG_MEM_MAP_BASE+(11<<3), {}, 0, {e_R22, e_R23, e_S22, e_S23}, 4 },
 { &mapu_spu_reg_types[es_dflt], 0, e_DR12, R_REG_MEM_MAP_BASE+(12<<3), {}, 0, {e_R24, e_R25, e_S24, e_S25}, 4 },
 { &mapu_spu_reg_types[es_dflt], 0, e_DR13, R_REG_MEM_MAP_BASE+(13<<3), {}, 0, {e_R26, e_R27, e_S26, e_S27}, 4 },
 { &mapu_spu_reg_types[es_dflt], 0, e_DR14, R_REG_MEM_MAP_BASE+(14<<3), {}, 0, {e_R28, e_R29, e_S28, e_S29}, 4 },
 { &mapu_spu_reg_types[es_dflt], 0, e_DR15, R_REG_MEM_MAP_BASE+(15<<3), {}, 0, {e_R30, e_R31, e_S30, e_S31}, 4 }
};

static int spu_get_reg(struct reg *reg);
static int spu_set_reg(struct reg *reg, uint8_t *buf);
static int spu_print_reg(struct reg *reg, char * content);

static const struct reg_arch_type mapu_spu_reg_callbacks = {
  .get = spu_get_reg,
  .set = spu_set_reg,
  .print = spu_print_reg
};

/*
struct reg_desc
{
    const char *name;
    uint32_t size;                      // number of bit

    uint32_t offset;                    // offset in memory map

    void *arch_info;                    // arch-specific register info
    const struct reg_arch_type *type;
};
*/
static struct reg_desc mapu_spu_reg_descs[] =
{
 // int registers
 { "R0", 32, (0<<2), &mapu_spu_int_reg_infos[0],  &mapu_spu_reg_callbacks},
 { "R1", 32, (1<<2), &mapu_spu_int_reg_infos[1],  &mapu_spu_reg_callbacks},
 { "R2", 32, (2<<2), &mapu_spu_int_reg_infos[2],  &mapu_spu_reg_callbacks},
 { "R3", 32, (3<<2), &mapu_spu_int_reg_infos[3],  &mapu_spu_reg_callbacks},
 { "R4", 32, (4<<2), &mapu_spu_int_reg_infos[4],  &mapu_spu_reg_callbacks},
 { "R5", 32, (5<<2), &mapu_spu_int_reg_infos[5],  &mapu_spu_reg_callbacks},
 { "R6", 32, (6<<2), &mapu_spu_int_reg_infos[6],  &mapu_spu_reg_callbacks},
 { "R7", 32, (7<<2), &mapu_spu_int_reg_infos[7],  &mapu_spu_reg_callbacks},
 { "R8", 32, (8<<2), &mapu_spu_int_reg_infos[8],  &mapu_spu_reg_callbacks},
 { "R9", 32, (9<<2), &mapu_spu_int_reg_infos[9],  &mapu_spu_reg_callbacks},
 { "R10", 32, (10<<2), &mapu_spu_int_reg_infos[10],  &mapu_spu_reg_callbacks},
 { "R11", 32, (11<<2), &mapu_spu_int_reg_infos[11],  &mapu_spu_reg_callbacks},
 { "R12", 32, (12<<2), &mapu_spu_int_reg_infos[12],  &mapu_spu_reg_callbacks},
 { "R13", 32, (13<<2), &mapu_spu_int_reg_infos[13],  &mapu_spu_reg_callbacks},
 { "R14", 32, (14<<2), &mapu_spu_int_reg_infos[14],  &mapu_spu_reg_callbacks},
 { "R15", 32, (15<<2), &mapu_spu_int_reg_infos[15],  &mapu_spu_reg_callbacks},
 { "R16", 32, (16<<2), &mapu_spu_int_reg_infos[16],  &mapu_spu_reg_callbacks},
 { "R17", 32, (17<<2), &mapu_spu_int_reg_infos[17],  &mapu_spu_reg_callbacks},
 { "R18", 32, (18<<2), &mapu_spu_int_reg_infos[18],  &mapu_spu_reg_callbacks},
 { "R19", 32, (19<<2), &mapu_spu_int_reg_infos[19],  &mapu_spu_reg_callbacks},
 { "R20", 32, (20<<2), &mapu_spu_int_reg_infos[20],  &mapu_spu_reg_callbacks},
 { "R21", 32, (21<<2), &mapu_spu_int_reg_infos[21],  &mapu_spu_reg_callbacks},
 { "R22", 32, (22<<2), &mapu_spu_int_reg_infos[22],  &mapu_spu_reg_callbacks},
 { "R23", 32, (23<<2), &mapu_spu_int_reg_infos[23],  &mapu_spu_reg_callbacks},
 { "R24", 32, (24<<2), &mapu_spu_int_reg_infos[24],  &mapu_spu_reg_callbacks},
 { "R25", 32, (25<<2), &mapu_spu_int_reg_infos[25],  &mapu_spu_reg_callbacks},
 { "R26", 32, (26<<2), &mapu_spu_int_reg_infos[26],  &mapu_spu_reg_callbacks},
 { "R27", 32, (27<<2), &mapu_spu_int_reg_infos[27],  &mapu_spu_reg_callbacks},
 { "R28", 32, (28<<2), &mapu_spu_int_reg_infos[28],  &mapu_spu_reg_callbacks},
 { "R29", 32, (29<<2), &mapu_spu_int_reg_infos[29],  &mapu_spu_reg_callbacks},
 { "R30", 32, (30<<2), &mapu_spu_int_reg_infos[30],  &mapu_spu_reg_callbacks},
 { "R31", 32, (31<<2), &mapu_spu_int_reg_infos[31],  &mapu_spu_reg_callbacks},

 // address register
 { "J0", 32, (32<<2)+(0<<2), &mapu_spu_addr_reg_infos[0],  &mapu_spu_reg_callbacks},
 { "J1", 32, (32<<2)+(1<<2), &mapu_spu_addr_reg_infos[1],  &mapu_spu_reg_callbacks},
 { "J2", 32, (32<<2)+(2<<2), &mapu_spu_addr_reg_infos[2],  &mapu_spu_reg_callbacks},
 { "J3", 32, (32<<2)+(3<<2), &mapu_spu_addr_reg_infos[3],  &mapu_spu_reg_callbacks},
 { "J4", 32, (32<<2)+(4<<2), &mapu_spu_addr_reg_infos[4],  &mapu_spu_reg_callbacks},
 { "J5", 32, (32<<2)+(5<<2), &mapu_spu_addr_reg_infos[5],  &mapu_spu_reg_callbacks},
 { "J6", 32, (32<<2)+(6<<2), &mapu_spu_addr_reg_infos[6],  &mapu_spu_reg_callbacks},
 { "J7", 32, (32<<2)+(7<<2), &mapu_spu_addr_reg_infos[7],  &mapu_spu_reg_callbacks},
 { "J8", 32, (32<<2)+(8<<2), &mapu_spu_addr_reg_infos[8],  &mapu_spu_reg_callbacks},
 { "J9", 32, (32<<2)+(9<<2), &mapu_spu_addr_reg_infos[9],  &mapu_spu_reg_callbacks},
 { "J10", 32, (32<<2)+(10<<2), &mapu_spu_addr_reg_infos[10],  &mapu_spu_reg_callbacks},
 { "J11", 32, (32<<2)+(11<<2), &mapu_spu_addr_reg_infos[11],  &mapu_spu_reg_callbacks},
 { "J12", 32, (32<<2)+(12<<2), &mapu_spu_addr_reg_infos[12],  &mapu_spu_reg_callbacks},
 { "J13", 32, (32<<2)+(13<<2), &mapu_spu_addr_reg_infos[13],  &mapu_spu_reg_callbacks},
 { "J14", 32, (32<<2)+(14<<2), &mapu_spu_addr_reg_infos[14],  &mapu_spu_reg_callbacks},
 { "J15", 32, (32<<2)+(15<<2), &mapu_spu_addr_reg_infos[15],  &mapu_spu_reg_callbacks},
 { "J16", 32, (32<<2)+(16<<2), &mapu_spu_addr_reg_infos[16],  &mapu_spu_reg_callbacks},
 { "J17", 32, (32<<2)+(17<<2), &mapu_spu_addr_reg_infos[17],  &mapu_spu_reg_callbacks},
 { "J18", 32, (32<<2)+(18<<2), &mapu_spu_addr_reg_infos[18],  &mapu_spu_reg_callbacks},
 { "J19", 32, (32<<2)+(19<<2), &mapu_spu_addr_reg_infos[19],  &mapu_spu_reg_callbacks},
 { "J20", 32, (32<<2)+(20<<2), &mapu_spu_addr_reg_infos[20],  &mapu_spu_reg_callbacks},
 { "J21", 32, (32<<2)+(21<<2), &mapu_spu_addr_reg_infos[21],  &mapu_spu_reg_callbacks},
 { "J22", 32, (32<<2)+(22<<2), &mapu_spu_addr_reg_infos[22],  &mapu_spu_reg_callbacks},
 { "J23", 32, (32<<2)+(23<<2), &mapu_spu_addr_reg_infos[23],  &mapu_spu_reg_callbacks},
 { "J24", 32, (32<<2)+(24<<2), &mapu_spu_addr_reg_infos[24],  &mapu_spu_reg_callbacks},
 { "J25", 32, (32<<2)+(25<<2), &mapu_spu_addr_reg_infos[25],  &mapu_spu_reg_callbacks},
 { "J26", 32, (32<<2)+(26<<2), &mapu_spu_addr_reg_infos[26],  &mapu_spu_reg_callbacks},
 { "J27", 32, (32<<2)+(27<<2), &mapu_spu_addr_reg_infos[27],  &mapu_spu_reg_callbacks},
 { "J28", 32, (32<<2)+(28<<2), &mapu_spu_addr_reg_infos[28],  &mapu_spu_reg_callbacks},
 { "J29", 32, (32<<2)+(29<<2), &mapu_spu_addr_reg_infos[29],  &mapu_spu_reg_callbacks},
 { "J30", 32, (32<<2)+(30<<2), &mapu_spu_addr_reg_infos[30],  &mapu_spu_reg_callbacks},
 { "J31", 32, (32<<2)+(31<<2), &mapu_spu_addr_reg_infos[31],  &mapu_spu_reg_callbacks},

 { "DPC", 32, (32<<2)+(32<<2), &mapu_spu_dpc_reg_info,        &mapu_spu_reg_callbacks},
 { "RRPC",32, (32<<2)+(33<<2), &mapu_spu_rrpc_reg_info,       &mapu_spu_reg_callbacks},

 // sflt registers
 { "S0", 32, (0<<2), &mapu_spu_sflt_reg_infos[0],  &mapu_spu_reg_callbacks},
 { "S1", 32, (1<<2), &mapu_spu_sflt_reg_infos[1],  &mapu_spu_reg_callbacks},
 { "S2", 32, (2<<2), &mapu_spu_sflt_reg_infos[2],  &mapu_spu_reg_callbacks},
 { "S3", 32, (3<<2), &mapu_spu_sflt_reg_infos[3],  &mapu_spu_reg_callbacks},
 { "S4", 32, (4<<2), &mapu_spu_sflt_reg_infos[4],  &mapu_spu_reg_callbacks},
 { "S5", 32, (5<<2), &mapu_spu_sflt_reg_infos[5],  &mapu_spu_reg_callbacks},
 { "S6", 32, (6<<2), &mapu_spu_sflt_reg_infos[6],  &mapu_spu_reg_callbacks},
 { "S7", 32, (7<<2), &mapu_spu_sflt_reg_infos[7],  &mapu_spu_reg_callbacks},
 { "S8", 32, (8<<2), &mapu_spu_sflt_reg_infos[8],  &mapu_spu_reg_callbacks},
 { "S9", 32, (9<<2), &mapu_spu_sflt_reg_infos[9],  &mapu_spu_reg_callbacks},
 { "S10", 32, (10<<2), &mapu_spu_sflt_reg_infos[10],  &mapu_spu_reg_callbacks},
 { "S11", 32, (11<<2), &mapu_spu_sflt_reg_infos[11],  &mapu_spu_reg_callbacks},
 { "S12", 32, (12<<2), &mapu_spu_sflt_reg_infos[12],  &mapu_spu_reg_callbacks},
 { "S13", 32, (13<<2), &mapu_spu_sflt_reg_infos[13],  &mapu_spu_reg_callbacks},
 { "S14", 32, (14<<2), &mapu_spu_sflt_reg_infos[14],  &mapu_spu_reg_callbacks},
 { "S15", 32, (15<<2), &mapu_spu_sflt_reg_infos[15],  &mapu_spu_reg_callbacks},
 { "S16", 32, (16<<2), &mapu_spu_sflt_reg_infos[16],  &mapu_spu_reg_callbacks},
 { "S17", 32, (17<<2), &mapu_spu_sflt_reg_infos[17],  &mapu_spu_reg_callbacks},
 { "S18", 32, (18<<2), &mapu_spu_sflt_reg_infos[18],  &mapu_spu_reg_callbacks},
 { "S19", 32, (19<<2), &mapu_spu_sflt_reg_infos[19],  &mapu_spu_reg_callbacks},
 { "S20", 32, (20<<2), &mapu_spu_sflt_reg_infos[20],  &mapu_spu_reg_callbacks},
 { "S21", 32, (21<<2), &mapu_spu_sflt_reg_infos[21],  &mapu_spu_reg_callbacks},
 { "S22", 32, (22<<2), &mapu_spu_sflt_reg_infos[22],  &mapu_spu_reg_callbacks},
 { "S23", 32, (23<<2), &mapu_spu_sflt_reg_infos[23],  &mapu_spu_reg_callbacks},
 { "S24", 32, (24<<2), &mapu_spu_sflt_reg_infos[24],  &mapu_spu_reg_callbacks},
 { "S25", 32, (25<<2), &mapu_spu_sflt_reg_infos[25],  &mapu_spu_reg_callbacks},
 { "S26", 32, (26<<2), &mapu_spu_sflt_reg_infos[26],  &mapu_spu_reg_callbacks},
 { "S27", 32, (27<<2), &mapu_spu_sflt_reg_infos[27],  &mapu_spu_reg_callbacks},
 { "S28", 32, (28<<2), &mapu_spu_sflt_reg_infos[28],  &mapu_spu_reg_callbacks},
 { "S29", 32, (29<<2), &mapu_spu_sflt_reg_infos[29],  &mapu_spu_reg_callbacks},
 { "S30", 32, (30<<2), &mapu_spu_sflt_reg_infos[30],  &mapu_spu_reg_callbacks},
 { "S31", 32, (31<<2), &mapu_spu_sflt_reg_infos[31],  &mapu_spu_reg_callbacks},

 // dflt registers
 { "DR0", 64, (0<<3), &mapu_spu_dflt_reg_infos[0],  &mapu_spu_reg_callbacks},
 { "DR1", 64, (1<<3), &mapu_spu_dflt_reg_infos[1],  &mapu_spu_reg_callbacks},
 { "DR2", 64, (2<<3), &mapu_spu_dflt_reg_infos[2],  &mapu_spu_reg_callbacks},
 { "DR3", 64, (3<<3), &mapu_spu_dflt_reg_infos[3],  &mapu_spu_reg_callbacks},
 { "DR4", 64, (4<<3), &mapu_spu_dflt_reg_infos[4],  &mapu_spu_reg_callbacks},
 { "DR5", 64, (5<<3), &mapu_spu_dflt_reg_infos[5],  &mapu_spu_reg_callbacks},
 { "DR6", 64, (6<<3), &mapu_spu_dflt_reg_infos[6],  &mapu_spu_reg_callbacks},
 { "DR7", 64, (7<<3), &mapu_spu_dflt_reg_infos[7],  &mapu_spu_reg_callbacks},
 { "DR8", 64, (8<<3), &mapu_spu_dflt_reg_infos[8],  &mapu_spu_reg_callbacks},
 { "DR9", 64, (9<<3), &mapu_spu_dflt_reg_infos[9],  &mapu_spu_reg_callbacks},
 { "DR10", 64, (10<<3), &mapu_spu_dflt_reg_infos[10],  &mapu_spu_reg_callbacks},
 { "DR11", 64, (11<<3), &mapu_spu_dflt_reg_infos[11],  &mapu_spu_reg_callbacks},
 { "DR12", 64, (12<<3), &mapu_spu_dflt_reg_infos[12],  &mapu_spu_reg_callbacks},
 { "DR13", 64, (13<<3), &mapu_spu_dflt_reg_infos[13],  &mapu_spu_reg_callbacks},
 { "DR14", 64, (14<<3), &mapu_spu_dflt_reg_infos[14],  &mapu_spu_reg_callbacks},
 { "DR15", 64, (15<<3), &mapu_spu_dflt_reg_infos[15],  &mapu_spu_reg_callbacks}
};

// a bit 1 means a target already exists.
static uint32_t mapu_spu_target_bits = 0x0;

static int
spu_get_free_bit(void)
{
	int pos = 0;
	do {
		if((~mapu_spu_target_bits & 1 << pos) != 0)
			break;
		else
			pos++;
	} while(1);

	return pos;
}

static int
spu_use_free_bit(void)
{
	int pos = spu_get_free_bit();
	mapu_spu_target_bits |= (1 << pos);
	return pos;
}

// read [address] to buffer, where address
// This function do not need to check access priority and target status
// which are recorded by openocd.
static int
spu_read_word_reserved(struct target *target, uint32_t address, uint32_t* buffer)
{
  // in case we misuse this command
  if(address & 0xFF000000 == 0)
  {
    LOG_ERROR("misuse read command to access reserved memory zone");
    return ERROR_FAIL;
  }

	struct mapu_spu_common *mapu_spu = target_to_mapu_spu(target);
	struct mapu_jtag *jtag_info = &mapu_spu->jtag_info;
	uint8_t inst[9];
	SET_BLOCK_ADDR(inst, 0);
	SET_WORD_ADDR(inst, address);
	SET_TARGET_SELECTED(inst, target->spu_mask);
	SET_PACKET_DESC(inst, PD_ACCESS | PD_ACCESS_WR);
	mapu_jtag_drscan_package(jtag_info, inst, true);

	CLEAR_PACKET(inst);
	SET_TARGET_SELECTED(inst, target->spu_mask);
	mapu_jtag_drscan_package(jtag_info, inst, true);

	memcpy(buffer, inst, 4);
	return ERROR_OK;
}

// write buffer [address].
// This function do not need to check access priority and target status recorded by openocd.
static int
spu_write_word_reserved(struct target *target, uint32_t address, uint32_t val)
{
  // in case we misuse this command
  if(address & 0xFF000000 == 0)
  {
    LOG_ERROR("misuse write command to access reserved memory zone");
    return ERROR_FAIL;
  }

	struct mapu_spu_common *mapu_spu = target_to_mapu_spu(target);
	struct mapu_jtag *jtag_info = &mapu_spu->jtag_info;
	uint8_t inst[9];
	SET_BLOCK_ADDR(inst, val);
	SET_WORD_ADDR(inst, address);
	SET_TARGET_SELECTED(inst, target->spu_mask);
	SET_PACKET_DESC(inst, PD_ACCESS | PD_ACCESS_WW);
	return mapu_jtag_drscan_package(jtag_info, inst, true);
}

static uint32_t spu_check_status(struct target *target)
{
  uint32_t status, hw_bp_status;
  spu_read_word_reserved(target, SPU_STATUS_REG,     &status);
  spu_read_word_reserved(target, SPU_HW_BP_CTRL_REG, &hw_bp_status);
  switch(GET_SPU_STATUS(status))
  {
    case mapu_spu_reset:
      target->state = TARGET_RESET;
      break;
    case mapu_spu_running:
      target->state = TARGET_RUNNING;
      break;
    case mapu_spu_halted:
      target->state = TARGET_HALTED;
      break;
    default:
      target->state = TARGET_UNKNOWN;
      break;
  }

  struct mapu_spu_common *mapu_spu = target_to_mapu_spu(target);
  mapu_spu->status       = status;
  mapu_spu->hw_bp_status = hw_bp_status;
  return status;
}

/* at present, size must be 4. and count is the number of words. */
static int
mapu_spu_read_memory(struct target *target, uint32_t address, uint32_t size, uint32_t count, uint8_t *buffer)
{
	if(target->state != TARGET_HALTED) {
		LOG_WARNING("target not halted");
		return ERROR_TARGET_NOT_HALTED;
	}

	struct mapu_spu_common *mapu_spu = target_to_mapu_spu(target);
	struct mapu_jtag *jtag_info = &mapu_spu->jtag_info;

	LOG_DEBUG("address: 0x%8.8" PRIx32 ", size: 0x%8.8" PRIx32 ", count: 0x%8.8" PRIx32 "", address, size, count);

	/* sanitize arguments */
	if(((size != 4) && (size != 2) && (size != 1)) || (count == 0) || !(buffer))
		return ERROR_INVALID_ARGUMENTS;

	if(((size == 4) && (address & 0x3u)) || ((size == 2) && (address & 0x1u)))
		return ERROR_TARGET_UNALIGNED_ACCESS;

	if(address & 0xFF000000) {
		uint32_t val;
		for(int i = 0; i < (count * size); i += size) {
			spu_read_word_reserved(target, address+i, &val);
			memcpy(buffer+i, &val, 4);
		}
		return ERROR_OK;
	}

	uint8_t inst[9];
	h_u32_to_le(inst, address);
	h_u24_to_le(inst + 4, count);
	SET_TARGET_SELECTED(inst, target->spu_mask);
	SET_PACKET_DESC(inst, PD_ACCESS | PD_ACCESS_BR);
	// read block
	mapu_jtag_drscan_package(jtag_info, inst, true);

	uint32_t t32;
	uint16_t t16;

	// after sending the command, we exchange the data we read.
	for(int i = 0; i < (count * size); i += size) {
		CLEAR_PACKET(inst);
		SET_TARGET_SELECTED(inst, target->spu_mask);
		mapu_jtag_drscan_package(jtag_info, inst, true);
		switch(size) {
			case 4:
				t32 = le_to_h_u32(inst);
				target_buffer_set_u32(target, &buffer[i], t32);
				break;
			case 2:
				t16 = le_to_h_u16(inst);
				target_buffer_set_u16(target, &buffer[i], t16);
				break;
			case 1:
				buffer[i] = inst[0];
				break;
			default:
				break;
		}
	}

	return ERROR_OK;
}

static int
mapu_spu_write_memory(struct target *target, uint32_t address, uint32_t size, uint32_t count, const uint8_t *buffer)
{
	if(target->state != TARGET_HALTED) {
		LOG_WARNING("target not halted");
		return ERROR_TARGET_NOT_HALTED;
	}

	struct mapu_spu_common *mapu_spu = target_to_mapu_spu(target);
	struct mapu_jtag *jtag_info = &mapu_spu->jtag_info;

	LOG_DEBUG("address: 0x%8.8" PRIx32 ", size: 0x%8.8" PRIx32 ", count: 0x%8.8" PRIx32 "", address, size, count);

	/* sanitize arguments */
	if(((size != 4) && (size != 2) && (size != 1)) || (count == 0) || !(buffer))
		return ERROR_INVALID_ARGUMENTS;

	if(((size == 4) && (address & 0x3u)) || ((size == 2) && (address & 0x1u)))
		return ERROR_TARGET_UNALIGNED_ACCESS;

	if(address & 0xFF000000) {
		uint32_t val;
		for(int i = 0; i < (count * size); i += size) {
			memcpy((uint8_t *) &val, buffer + i, 4);
			spu_write_word_reserved(target, address, val);
		}
		return ERROR_OK;
	}

	uint8_t inst[9];
	h_u32_to_le(inst, address);
	h_u24_to_le(inst + 4, count);
	SET_TARGET_SELECTED(inst, target->spu_mask);
	inst[8] = 0x13;    // write package, block write
	mapu_jtag_drscan_package(jtag_info, inst, true);

	uint32_t t32;
	uint16_t t16;
	for(int i = 0; i < (count * size); i += size) {
		switch(size) {
			case 4:
				t32 = target_buffer_get_u32(target, &buffer[i]);
				h_u32_to_le(inst, t32);
				break;
			case 2:
				t16 = target_buffer_get_u16(target, &buffer[i]);
				h_u16_to_le(inst, t16);
				break;
			case 1:
				inst[0] = buffer[i];
				break;
			default:
				break;
		}

		SET_TARGET_SELECTED(inst, target->spu_mask);
		inst[8] = 0x20;
		mapu_jtag_drscan_package(jtag_info, inst, true);
	}

	return ERROR_OK;
}

/* sometimes bulk access is used, like when image size is lager than 128 bytes.
 * This simply calls write_memory().
 * @a count means how many WORDS should be accessed. */
static int
mapu_spu_bulk_write_memory(struct target *target, uint32_t address, uint32_t count, const uint8_t *buffer)
{
	if(target->state != TARGET_HALTED) {
		LOG_WARNING("target not halted");
		return ERROR_TARGET_NOT_HALTED;
	}

	LOG_DEBUG("address: 0x%8.8" PRIx32 ", count: 0x%8.8" PRIx32 "", address, count);

	/* check alignment */
	if(address & 0x3u)
		return ERROR_TARGET_UNALIGNED_ACCESS;

	/* Be careful: data in buffer are target endian. */
	return mapu_spu_write_memory(target, address, 4, count, buffer);
}

static int
mapu_spu_finish(struct target *target)
{
	/*if(target->state != TARGET_HALTED) {
	 LOG_WARNING("target not halted");
	 return ERROR_TARGET_NOT_HALTED;
	 }*/

	struct mapu_spu_common *mapu_spu = target_to_mapu_spu(target);
	struct mapu_jtag *jtag_info = &mapu_spu->jtag_info;

	uint8_t inst[9];
	SET_BLOCK_ADDR(inst, 0);
	SET_WORD_ADDR(inst, 0);
	SET_TARGET_SELECTED(inst, target->spu_mask);
	SET_PACKET_DESC(inst, 0x30);
	mapu_jtag_drscan_package(jtag_info, inst, true);

	return ERROR_OK;
}

static int
spu_get_reg(struct reg *reg)
{
	struct mapu_spu_reg_info *reg_info = reg->desc->arch_info;
	struct target *target = reg->target;

	if(target->state != TARGET_HALTED) {
		LOG_WARNING("target not halted");
		return ERROR_TARGET_NOT_HALTED;
	}

	switch(reg->desc->size)
	{
	  case 32:
	  {
	    if(! *reg->valid) {
	      mapu_spu_read_memory(target, reg_info->addr,
	                           DIV_ROUND_UP(reg->desc->size, 8), 1, reg->value);
	      *reg->valid = true;
	    }

      return ERROR_OK;
	  }
	  case 64:
    {
      if(! (*reg->valid && *(reg->valid+1))) {
        mapu_spu_read_memory(target, reg_info->addr,
                             DIV_ROUND_UP(reg->desc->size, 8), 1, reg->value);
        *reg->valid = true;
        *(reg->valid+1) = true;
      }

      return ERROR_OK;
    }

	  default:
	    LOG_ERROR("illegal register size");
	    return ERROR_FAIL;
	}
}

static int
spu_set_reg(struct reg *reg, uint8_t *buf)
{
	struct mapu_spu_reg_info *reg_info = reg->desc->arch_info;
	struct target *target = reg->target;

	if(target->state != TARGET_HALTED) {
		LOG_WARNING("target not halted");
		return ERROR_TARGET_NOT_HALTED;
	}

	memcpy(reg->rewrite_value, buf, DIV_ROUND_UP(reg->desc->size, 8));
	*reg->dirty = true;

	if(reg->desc->size==64)	  *(reg->dirty+1) = true;

	return ERROR_OK;
}

static int
spu_print_reg(struct reg *reg, char * content)
{
	struct mapu_spu_reg_info *reg_info = reg->desc->arch_info;
	struct mapu_spu_reg_type *type = reg_info->type;
	double dbl;
	float flt;

	int i;
	int sz = 0;
	sz = sprintf(content, "%-7s %-9s", reg->desc->name, type->set_name);
  // 4 bytes: 0, 1, 2, 3 (upper_indx).
  // 8 bytes: 0, 1, ..., 7 (upper_indx).
	int upper_indx = (reg->desc->size + 7) / 8 - 1;

	bool valid = *reg->valid;
	if(reg->desc->size==64) valid = valid && *(reg->valid+1);

	if(valid) {
    sz += sprintf(content + sz, ": ");

	  for(i = upper_indx; i >= 0; i--)
	    sz += sprintf(content + sz, "%02X", *( (unsigned char*) (reg->value) + i) );

		switch(type->set) {
			case es_sflt:
				memcpy(&flt, reg->value, sizeof(flt));
				sz += sprintf(content + sz, " (%e)", flt);
				break;
      case es_dflt:
				memcpy(&dbl, reg->value, sizeof(dbl));
				sz += sprintf(content + sz, " (%e)", dbl);
				break;

      default:
				break;
		}
	}

  bool dirty = *reg->dirty;
  if(reg->desc->size==64) dirty = dirty && *(reg->dirty+1);

  if(dirty) {
    sz += sprintf(content + sz, " --> ");

    for(i = upper_indx; i >= 0; i--)
      sz += sprintf(content + sz, "%02X", *( (unsigned char*) (reg->rewrite_value) + i) );

    switch(type->set) {
      case es_sflt:
        memcpy(&flt, reg->rewrite_value, sizeof(flt));
        sz += sprintf(content + sz, " (%e)", flt);
        break;
      case es_dflt:
        memcpy(&dbl, reg->rewrite_value, sizeof(dbl));
        sz += sprintf(content + sz, " (%e)", dbl);
        break;

      default:
        break;
    }
  }

	return ERROR_OK;
}

static int
spu_clear_reg_flags(struct target *target)
{
  struct reg *head_reg = target->reg_cache->reg_list;

  // implement register modification and invalidate value copy
  for(int k = 0; k < target->reg_cache->num_regs; k++) {
    struct reg * reg = &head_reg[k];

    // invalidate its value.
    *reg->valid = false;
    *reg->dirty = false;
    if(reg->desc->size == 64) *(reg->dirty+1) = false;
  }

  return ERROR_OK;
}

static int
spu_apply_reg_change(struct target *target)
{
  struct reg *head_reg = target->reg_cache->reg_list;

  // implement register modification and invalidate value copy
  for(int k = 0; k < target->reg_cache->num_regs; k++) {
    struct reg * reg = &head_reg[k];

    // invalidate its value.
    *reg->valid = false;

    struct mapu_spu_reg_info *reg_info = reg->desc->arch_info;
    if(*reg->dirty) {
      mapu_spu_write_memory(target, reg_info->addr, DIV_ROUND_UP(reg->desc->size, 8),
                            1, reg->rewrite_value);

      *reg->dirty = false;
      if(reg->desc->size == 64) *(reg->dirty+1) = false;
    }
  }

  return ERROR_OK;
}

static int
spu_retreat_bp(struct target *target)
{
  // breakpoint
  struct breakpoint *bp;
  for(bp = target->breakpoints; bp; bp = bp->next)
  {
    if(bp->set == 0)
      continue;

    bp->set = 0;

    if(bp->type == BKPT_HARD)
    {
      LOG_INFO("hw-bp %d at 0x%X is cleared", bp->unique_id, bp->revised_address);
      continue;
    }

    uint32_t insn;
    memcpy(&insn, bp->orig_instr, bp->length);
    target_write_u32(target, bp->revised_address, insn);
    LOG_INFO("sw-bp %d at 0x%X is cleared", bp->unique_id, bp->revised_address);
  }

  // watchpoint
  struct watchpoint *wp;
  for(wp = target->watchpoints; wp; wp = wp->next)
  {
    if(wp->set == 0)
      continue;

    wp->set = 0;
    LOG_INFO("watchpoint %d is cleared", wp->unique_id, wp->revised_address);
  }

  return ERROR_OK;
}

static int
spu_deploy_bp(struct target *target)
{
  struct mapu_spu_common *mapu_spu = target_to_mapu_spu(target);

  // bp: both sw and hw
  uint32_t hw_bp_mask = 0;
  struct breakpoint *breakpoint;
  int indx = 0;
  bool isSW;
  for(breakpoint = target->breakpoints; breakpoint; breakpoint = breakpoint->next)
  {
    if(breakpoint->type == BKPT_HARD)
    {
      isSW = false;
      spu_write_word_reserved(target, mapu_spu->ibp_reg_list[indx].mem_map,
                     breakpoint->revised_address);
      hw_bp_mask |= (1 << mapu_spu->ibp_reg_list[indx].mask_bit);
      indx++;
    }
    else if(breakpoint->type == BKPT_SOFT)
    {
      uint32_t insn, trap;
      isSW = true;
      memcpy(&insn, breakpoint->orig_instr, breakpoint->length);
      target_write_u32(target, breakpoint->revised_address, SPU_TRAP|(insn&(1<<31)));
      target_read_u32(target, breakpoint->revised_address, &trap);
      if(trap == SPU_TRAP|(insn&(1<<31))) {
        breakpoint->set = 1; /* Any nice value but 0 */
      }
      else {
        LOG_ERROR("Unable to deploy breakpoint at address 0x%X." PRIx32
                  "Check whether the memory address is readable/writable",
                  breakpoint->address);

        return ERROR_TARGET_RESOURCE_NOT_AVAILABLE;
      }
    }
    else {
      LOG_ERROR("unknown type for breakpoint at 0x%X", breakpoint->address);
      return ERROR_TARGET_FAILURE;
    }

    breakpoint->set = 1;

    // info dump in the end
    if(breakpoint->address == breakpoint->revised_address) {
      LOG_INFO("%s-bp %d at 0x%X is deployed", isSW?"sw":"hw",
               breakpoint->unique_id, breakpoint->revised_address);
    }
    else {
      LOG_INFO("%s-bp %d at 0x%X is deployed (instead of 0x%X)", isSW?"sw":"hw",
               breakpoint->unique_id, breakpoint->revised_address, breakpoint->address);
    }
  }

  // handle watchpoint
  struct watchpoint *wp;
  indx = 0;
  for(wp = target->watchpoints; wp; wp = wp->next)
  {
    // clear bits 31-28
    uint32_t addr = wp->revised_address & 0xFFFFFFF;

    // monitor mode
    addr |= ((wp->mode & 0x3) << 30);
    addr |= ((wp->rw   & 0x3) << 28);

    spu_write_word_reserved(target, mapu_spu->dbp_reg_list[indx].mem_map, addr);
    spu_write_word_reserved(target, mapu_spu->dbp_reg_list[indx].mem_map, wp->value);

    wp->set = 1;

    hw_bp_mask |= (1 << mapu_spu->dbp_reg_list[indx].mask_bit);
    indx++;

    switch(wp->mode)
    {
      case MODE_MEM:
        if(wp->revised_address == wp->address)
          LOG_INFO("watchpoint %d is deployed at 0x%X", wp->unique_id, wp->revised_address);
        else
          LOG_INFO("watchpoint %d is deployed at 0x%X (rather than 0x%X)", wp->unique_id,
                   wp->revised_address, wp->address);
        break;

      case MODE_DATA:
        LOG_INFO("watchpoint %d is deployed for monitoring value 0x%X", wp->unique_id, wp->value);
        break;

      case MODE_ALL:
        if(wp->revised_address == wp->address)
          LOG_INFO("watchpoint %d is deployed at 0x%X for monitoring value 0x%X", wp->unique_id,
                   wp->revised_address, wp->value);
        else
          LOG_INFO("watchpoint %d is deployed at 0x%X (rather than 0x%X) for monitoring value 0x%X",
                   wp->unique_id, wp->revised_address, wp->address, wp->value);
        break;

      default:
        LOG_ERROR("invalid watchpoint mode (%i)", wp->mode);
        return ERROR_FAIL;
    }
  }

  spu_write_word_reserved(target, SPU_HW_BP_CTRL_REG, ~hw_bp_mask);
  return ERROR_OK;
}

static int
spu_start_to_run(struct target *target)
{
  spu_write_word_reserved(target, OCD_CMD_DEACCESS, 0);

  // force spu out of debug mode and run.
  spu_write_word_reserved(target, OCD_CMD_RUN, 0);
  return ERROR_OK;
}

// *tail*: if true, we check the last instruction in the line. otherwise, we check the first one.
static uint32_t
spu_revise_bp_addr(struct target *target, uint32_t address, bool tail)
{
  // revise bp address according to vliw rules
  uint32_t revised_addr = address & (~0x3);
  if(revised_addr != address) {
    LOG_WARNING("bp addr 0x%X is not aligned, auto-align it to 0x%X", address, revised_addr);
  }

  uint32_t instr;

  // we check the last instruction slot for software breakpoints.
  if(tail)
  {
    do {
      mapu_spu_read_memory(target, revised_addr, 4, 1, (uint8_t *)&instr);
      if(instr & (1 << 31)) break;
      else { revised_addr += 4; }
    } while(revised_addr < address+16); // check at most 4 instructions.

    // we have found the last slot.
    if(revised_addr < address+16)
      return revised_addr;
  }
  // On the other hand, we check the first instruction slot for hardware breakpoints.
  else
  {
    while(revised_addr != 0 && address < revised_addr + 16) {
      mapu_spu_read_memory(target, revised_addr-4, 4, 1, (uint8_t *) &instr);
      if(instr & (1 << 31)) break;
      else { revised_addr -= 4; }
    }

    if(address < revised_addr + 16)
      return revised_addr;
  }

  // illegal breakpoint address
  LOG_ERROR("breakpoint address 0x%x is probably out of bound, please check", address);
  return (uint32_t)(-1);
}

static int
spu_revise_bp(struct target *target, struct breakpoint *breakpoint)
{
  // revise bp address according to vliw rules
  uint32_t revised_addr, instr;

  // we check the first instruction slot for software breakpoints.
  if(breakpoint->type == BKPT_SOFT)
  {
    revised_addr = spu_revise_bp_addr(target, breakpoint->address, false);
    if(revised_addr == (uint32_t) (-1))
      return ERROR_FAIL;
    else
    {
      mapu_spu_read_memory(target, revised_addr, 4, 1, (uint8_t *) &instr);
      memcpy(breakpoint->orig_instr, &instr, 4);
      // we do not write trap into target memory here, instead we do it when we resume.
      breakpoint->trap = SPU_TRAP;
    }
  }
  // On the other hand, we check the first instruction slot for hardware breakpoints.
  else if(breakpoint->type == BKPT_HARD)
  {
    revised_addr = spu_revise_bp_addr(target, breakpoint->address, false);
    if(revised_addr == (uint32_t)(-1))
      return ERROR_FAIL;
  }

  breakpoint->revised_address = revised_addr;
  return ERROR_OK;
}

/* we use control flow to handle breakpoint hit as follows:
 * _from_bp_hit_:
 *    jump _bp_hit_next_;;
 */
static int
spu_handle_bp_hit(struct target *target, uint32_t bp_addr, uint32_t* br_insn)
{
  return ERROR_OK;
}

static int
spu_handle_wp_hit(struct target *target)
{
  return ERROR_OK;
}

// upper layer assure target is halted.
static int
mapu_spu_add_breakpoint(struct target *target, struct breakpoint *breakpoint)
{
	struct mapu_spu_common *spu = target_to_mapu_spu(target);
	struct mapu_spu_bp_reg *bp_reg_list = spu->ibp_reg_list;

	if(breakpoint->set) {    // whether this bp is set onto debug target.
		LOG_WARNING("breakpoint already set");
		return ERROR_OK;
	}

	// revise bp address according to vliw rules.
	// if revision fails, breakpoint address is illegal.
	if(	spu_revise_bp(target, breakpoint) != ERROR_OK)
	{
	  // breakpoint_free(target, breakpoint) is called at upper layer.
	  return ERROR_TARGET_RESOURCE_NOT_AVAILABLE;
	}

  uint32_t revised_addr = breakpoint->revised_address;
  struct breakpoint * bp = breakpoint_find(target, revised_addr);
  if(bp && bp != breakpoint) { // whether the revised address points to an already-existed bp

    // since breakpoint_add_internal() already filtered direct overlap of address,
    // so if this condition is true, 'addr' must be a revised address.
    LOG_WARNING("breakpoint already added at 0x%x (instead of 0x%x)",
                bp->revised_address, breakpoint->address);

    // since 'breakpoint' won't be added at all.
    // breakpoint_free(target, breakpoint) is called at upper layer.
    return ERROR_TARGET_RESOURCE_NOT_AVAILABLE;
  }

  bool isSW;

	switch(breakpoint->type)
	{
	  case BKPT_HARD:
      isSW = false;
      if(spu->num_ibp_regs_avail <= 0) {
        LOG_ERROR("Can not find unused hw-breakpoint register for hw-bp %d at 0x%x",
                  breakpoint->unique_id, breakpoint->address);

        return ERROR_TARGET_RESOURCE_NOT_AVAILABLE;
      }

      spu->num_ibp_regs_avail--;
      break;

	  case BKPT_SOFT:
	    isSW = true;
	    break;

	  default:
	    LOG_ERROR("unknown breakpoint type");
	}

  if(revised_addr == breakpoint->address) {
    LOG_INFO("%s-bp %d at 0x%X is added", isSW?"sw":"hw",
             breakpoint->unique_id, revised_addr);
  }
  else {
    LOG_INFO("%s-bp %d at 0x%X is added (instead of 0x%X)", isSW?"sw":"hw",
             breakpoint->unique_id, revised_addr, breakpoint->address);
  }

  breakpoint->set = 0;
	return ERROR_OK;
}

static int
mapu_spu_add_watchpoint(struct target *target, struct watchpoint *watchpoint)
{
	// upper layer garanteens target is already halted.
	struct mapu_spu_common *spu = target_to_mapu_spu(target);

  if(spu->num_dbp_regs_avail <= 0) {
    LOG_ERROR("Can not find unused watchpoint register for hw-wp %d at 0x%x",
              watchpoint->unique_id, watchpoint->address);

    return ERROR_TARGET_RESOURCE_NOT_AVAILABLE;
  }

	struct mapu_spu_bp_reg * dbp_reg_list = spu->dbp_reg_list;

	if(watchpoint->set) {    // whether this wp is already set to debug target.
		LOG_WARNING("watchpoint already set");
		return ERROR_OK;
	}

	// adjust wp address according to alignment constraint.
  uint32_t addr;

	switch(watchpoint->length)
	{
	  case 1: addr = watchpoint->address         ; break;
	  case 2: addr = watchpoint->address & (~0x1); break;
	  case 4: addr = watchpoint->address & (~0x3); break;
	  default: LOG_ERROR("invalid length of watchpoint"); return ERROR_FAIL;
	}

  watchpoint->revised_address = addr;
  watchpoint->set = 0;
  spu->num_dbp_regs_avail--;

	if(addr != watchpoint->address) {
		LOG_WARNING("watchpoint addr 0x%X is not aligned, auto-align it to 0x%X", watchpoint->address, addr);
	}

	switch(watchpoint -> mode)
	{
	  case MODE_MEM:
	    LOG_INFO("watchpoint %i is added at 0x%X", watchpoint->unique_id, addr); break;
    case MODE_DATA:
      LOG_INFO("watchpoint %i is added for monitoring value 0x%X", watchpoint->unique_id, watchpoint->value); break;
    case MODE_ALL:
      LOG_INFO("watchpoint %i is added at 0x%X for monitoring value 0x%X", watchpoint->unique_id, addr, watchpoint->value); break;
    default:
      LOG_ERROR("invalid watchpoint mode ('%i')", watchpoint->mode);
      return ERROR_FAIL;
	}

  return ERROR_OK;
}

static int
mapu_spu_remove_watchpoint(struct target *target, struct watchpoint *watchpoint)
{
  if(watchpoint->set) {
    LOG_WARNING("watchpoint 0x%x is already set onto program thus can not be removed",
                watchpoint->unique_id);

    return ERROR_FAIL;
  }

  uint32_t status = spu_check_status(target);
  if(target->state != TARGET_HALTED) {
    LOG_WARNING("target is in %s status when to remove watchpoint 0x%x, which is not allowed.",
                target_state_name(target), watchpoint->unique_id);

    return ERROR_TARGET_NOT_HALTED;
  }

  struct mapu_spu_common *spu = target_to_mapu_spu(target);
  spu->num_dbp_regs_avail++;
  watchpoint->set = 0;
  return ERROR_OK;
}

/* get register file access property, can not check this status via SPU register status. */
/*static int
 mapu_spu_access_enter(struct target *target)
 {
 LOG_DEBUG("Function Entry Point");
 mapu_spu_word_write(target, OCD_CMD_ACCESS, 0);
 struct mapu_spu_common *mapu_spu = target_to_mapu_spu(target);

 do {
 uint32_t stat;
 mapu_spu_word_read(target, SPU_STATUS_REG, &stat);

 if(CHECK_OCD_STOP_SPU_ACK(stat)) {
 mapu_spu->debug = true;
 break;
 }
 else usleep(50000);
 } while(1);
 return ERROR_OK;
 }*/

/* force the debug target out of debug mode. */
/*static int
 mapu_spu_access_exit(struct target *target)
 {
 LOG_DEBUG("Function Entry Point");
 mapu_spu_word_write(target, OCD_CMD_DEACCESS, 0);
 struct mapu_spu_common *mapu_spu = target_to_mapu_spu(target);
 do {
 uint32_t stat;
 mapu_spu_word_read(target, SPU_STATUS_REG, &stat);

 if(!CHECK_OCD_STOP_SPU_ACK(stat)) {
 mapu_spu->debug = false;
 break;
 }
 else usleep(50000);
 } while(1);
 return ERROR_OK;
 }*/

// This function simply interpret current 'status', which is recorded in poll.
static int
mapu_spu_arch_state(struct target *target)
{
	struct mapu_spu_common *mapu_spu = target_to_mapu_spu(target);
	uint32_t status = mapu_spu->status;

	char stop_sources[20] = {0};
  char stop_reason[20] = {0};

  if(CHECK_ARM_STOP_SPU_REQ(status)) strcat(stop_sources, "ARM; ");
  if(CHECK_OCD_STOP_SPU_ACK(status)) strcat(stop_sources, "OCD; ");
  if(CHECK_SPU_STOP_SPU_REQ(status)) strcat(stop_sources, "internal");

  if(CHECK_OCD_HW_BP(status)) {
    uint32_t hw_bp_stat = mapu_spu->hw_bp_status;
    uint32_t enable_bits = (~hw_bp_stat) & 0x7;
    hw_bp_stat >>= 24;
    hw_bp_stat &= enable_bits;
    if(hw_bp_stat & 1 << 0)   strcat(stop_reason, "HBP ");
    if(hw_bp_stat & 1 << 1)   strcat(stop_reason, "WP0 ");
    if(hw_bp_stat & 1 << 2)   strcat(stop_reason, "WP1 ");
  }

  if(CHECK_OCD_SW_BP(status)) strcat(stop_reason, "SBP ");
  if(target->debug_reason == DBG_REASON_SINGLESTEP) strcat(stop_reason, "Step");

	LOG_USER("Target Name   Type          State       Local OCD   Stop Sources        Stop Reason");
	LOG_USER("------------  ------------  ----------  ----------  ------------------  -----------");
	LOG_USER("%-14s%-14s%-12s%-12s%-20s%-12s",
	         target->cmd_name,
	         target_type_name(target),
	         target_state_name(target),
	         CHECK_OCD_READY(status)?"ready":"not ready",
	         stop_sources,
	         stop_reason);

	return ERROR_OK;
}

/* poll current target status, see arch_state()
 * The first time poll is run displays original status of target.
 * The second time poll is run check the status when OCD_CMD_HALT
 * is already run, thus get an extra Debug Source *OCD*. */
static int
mapu_spu_poll(struct target *target)
{
  struct mapu_spu_common *mapu_spu = target_to_mapu_spu(target);
  uint32_t status = spu_check_status(target);

  // if target is halted, we want it to stay at this status *steady*, so we will send
  // *OCD_CMD_HALT*.
	if(target->state == TARGET_HALTED)
	{
			spu_write_word_reserved(target, OCD_CMD_HALT, 0);   // halt spu and ocd steady
			spu_write_word_reserved(target, OCD_CMD_ACCESS, 0); // allow access to memory for ocd
      spu_retreat_bp(target);

      // Determine why target is halted.
			// if a target is halted, there are following reasons:
			// 1. spu.stop; 2. single step; 3. watch-point; 4. hw-breakpoint; 5. sw-breakpoint;
			// 6. interrupted (from tcl "halt" or from debugger ctrl-c).

			// 1. spu.stop;
			if(CHECK_SPU_STOP_SPU_REQ(status))
			{
			  target->process_exited = true;
			  target->gdb_connected = false;
			  target->debug_reason = DBG_REASON_DBGRQ;
        return ERROR_OK;
			}

			// 2: single step. there is no bit for ocd to indicate a single step, so we have to handle it specially.
			if(target->debug_reason == DBG_REASON_SINGLESTEP)
			  return ERROR_OK;

			if(CHECK_OCD_HW_BP(status)) {
		    uint32_t hw_bp_status = mapu_spu->hw_bp_status;
	      uint32_t enable_bits = (~hw_bp_status) & 0x7; // bit 2~0: mask bits
	      hw_bp_status >>= 24; // bit 26~25: hw bp hit bits
	      hw_bp_status &= enable_bits;

	      // 3. watch-point
	      if( (hw_bp_status>>1 & 1) || (hw_bp_status>>2 & 1) )
	      {
	        target->debug_reason = DBG_REASON_WATCHPOINT;
	        return ERROR_OK;
	      }

	      // 4. hw-breakpoint
        if(hw_bp_status>>0 & 1)
        {
          target->debug_reason = DBG_REASON_BREAKPOINT;
          return ERROR_OK;
        }
		  }

		  // 5. sw-breakpoint
		  if(CHECK_OCD_SW_BP(status))
      {
        target->debug_reason = DBG_REASON_BREAKPOINT;
        return ERROR_OK;
      }

		  // 6. interrupted
      target->debug_reason = DBG_REASON_DBGRQ;
      return ERROR_OK;
	}
	else
	{
	  target->debug_reason = DBG_REASON_NOTHALTED;
    return ERROR_OK;
	}
}

// todo: add breakpoint set/unset
// to make a breakpoint work, two basic things has to be done:
// 1. add it to openocd-created target.
// 2. activate it onto debug target: the hardware.
// this is useful when you soft-reset the target. all debug things shall be saved.
static int
mapu_spu_assert_reset(struct target *target)
{
	target->cs_count=0;
	target->process_exited=false;
	spu_write_word_reserved(target, OCD_CMD_RESET, 0);

	if(target->reset_halt) {
		spu_write_word_reserved(target, OCD_CMD_HALT, 0);
		spu_write_word_reserved(target, OCD_CMD_DERESET, 0);
	}

  spu_clear_reg_flags(target);

	return ERROR_OK;
}

/* Notice: we will do nothing here. Instead we leave dereset signal to be asserted elsewhere. */
static int
mapu_spu_deassert_reset(struct target *target)
{
	/*uint32_t stat;
	if(target->state == TARGET_RESET) {
		spu_write_word(target, OCD_CMD_DERESET, 0);

		spu_read_word(target, SPU_STATUS_REG, &stat);
		switch(GET_SPU_STATUS(stat)) {
			case mapu_spu_reset:
				LOG_WARNING("DERESET target failed, target remains in RESET status");
				target->state = TARGET_RESET;
				break;
			case mapu_spu_running:
				target->state = TARGET_RUNNING;
				break;
			case mapu_spu_halted:
				target->state = TARGET_HALTED;
				break;
			default:
				target->state = TARGET_UNKNOWN;
		}
	}*/

	return ERROR_OK;
}

// reserved
/*static int
mapu_spu_save_context(struct target *target)
{
  struct mapu_spu_common *mapu_spu = target_to_mapu_spu(target);

  uint32_t size = 4;
  target_read_memory(target, R_REG_MEM_MAP_BASE, size, SPU_NUM_R_REGS, (uint8_t *) mapu_spu->reg_buf);
  target_read_memory(target, J_REG_MEM_MAP_BASE, size, SPU_NUM_J_REGS, (uint8_t *) mapu_spu->reg_buf);
  struct reg *reg_list = target->reg_cache->reg_list;

  int k;
  for(k = 0; k < target->reg_cache->num_regs; k++) {
    *reg_list[k].dirty = 0;
    *reg_list[k].valid = 1;
  }

  return ERROR_OK;
}*/

static int
mapu_spu_halt(struct target *target)
{
	struct mapu_spu_common *mapu_spu = target_to_mapu_spu(target);
	uint32_t status = spu_check_status(target);

	switch(target->state) {
		case TARGET_RESET:
		case TARGET_RUNNING:
		case TARGET_HALTED:
			// After target has halted (bp...), drive local ocd into steady debug mode via OCD_CMD_HALT.
		  // Note: even target is halted, local ocd might be in unsteady *HALT* state.
			spu_write_word_reserved(target, OCD_CMD_HALT, 0);
			break;

		default:
			LOG_WARNING("Curent SPU state is UNKNOWN. Target need reset.");
			return ERROR_TARGET_FAILURE;
	}

  if(target->state == TARGET_RESET) spu_write_word_reserved(target, OCD_CMD_DERESET, 0);

	// make sure it does HALT here.
	int repeat = 0;
	do {
	  status = spu_check_status(target);
		if( target->state == TARGET_HALTED && CHECK_OCD_STOP_SPU_ACK(status))
		{
      spu_write_word_reserved(target, OCD_CMD_ACCESS, 0); // allow access to memory for ocd
      spu_retreat_bp(target);
			break;
		}
		else if(repeat < 600) {
			usleep(100000);    // 1 minute
			LOG_OUTPUT(".");
		}
		else {
			LOG_WARNING("timeout when try to halt target, please retry.");
			return ERROR_TARGET_TIMEOUT;
		}

		repeat++;

	} while(1);

	return ERROR_OK;
}

static int
mapu_spu_remove_breakpoint(struct target *target, struct breakpoint *breakpoint)
{
  /* get pointers to arch-specific information */
  struct mapu_spu_common *mapu_spu = target_to_mapu_spu(target);
  struct mapu_spu_bp_reg *ibp_reg_list = mapu_spu->ibp_reg_list;

  uint32_t status = spu_check_status(target);
  if(target->state != TARGET_HALTED ) {
    LOG_WARNING("target is in %s status when to remove bp at 0x%x, which is not allowed.",
                target_state_name(target), breakpoint->address);
    return ERROR_TARGET_NOT_HALTED;
  }

  if(breakpoint->set) {
    LOG_WARNING("breakpoint 0x%x is already set onto program thus can not be removed",
                breakpoint->address);

    return ERROR_FAIL;
  }

  switch(breakpoint->type)
  {
    case BKPT_HARD:
      LOG_INFO("remove hw-bp %d at 0x%x", breakpoint->unique_id, breakpoint->address);
      mapu_spu->num_ibp_regs_avail++;
      breakpoint->set = 0;
      break;

    case BKPT_SOFT:
      LOG_INFO("remove sw-bp %d at 0x%x", breakpoint->unique_id, breakpoint->address);
      breakpoint->set = 0;
      break;

    default:
      LOG_ERROR("unknown breakpoint type");
  }

  return ERROR_OK;
}

/* @a current = 1: continue on current pc, otherwise continue at <address> */
/* @handle_breakpoints, the front-end may request us not to handle breakpoints */
static int
mapu_spu_step(struct target *target, int current, uint32_t address, int handle_breakpoints)
{
  uint32_t status = spu_check_status(target);

  if(target->state == TARGET_RESET) {
    spu_write_word_reserved(target, OCD_CMD_HALT, 0);
    spu_write_word_reserved(target, OCD_CMD_DERESET, 0);
    spu_write_word_reserved(target, OCD_CMD_ACCESS, 0);
    status = spu_check_status(target);
  }

  if(target->state == TARGET_HALTED )
  {
    // when we are stepping at an sw breakpoint.
    if(CHECK_OCD_SW_BP(status))
    {
      LOG_INFO("stepping at a software breakpoint");

      spu_write_word_reserved(target, OCD_CMD_HALT, 0);   // halt spu and ocd steady
      spu_write_word_reserved(target, OCD_CMD_ACCESS, 0); // allow access to memory for ocd
      spu_retreat_bp(target);

      // get breakpoint address
      uint32_t bp_addr, insn;
      spu_read_word_reserved(target, DPC_MEM_MAP, &bp_addr);
      bp_addr = spu_revise_bp_addr(target, bp_addr, false);
      if(bp_addr == (uint32_t)(-1)) return ERROR_FAIL;
      target_read_u32(target, bp_addr, &insn);
      spu_write_word_reserved(target, SPROBE_MEM_MAP, insn);
    }

    spu_apply_reg_change(target);
    spu_deploy_bp(target);

    // step
    spu_write_word_reserved(target, OCD_CMD_DEACCESS, 0);
    spu_write_word_reserved(target, OCD_CMD_STEP, 0);

    target->debug_reason = DBG_REASON_SINGLESTEP;
    //target_call_event_callbacks(target, TARGET_EVENT_RESUMED);

    LOG_DEBUG("target stepped");

    // make sure it does HALT after one cycle execution.
    int repeat = 0;
    do {
      spu_check_status(target);
      if(target->state == TARGET_HALTED)
      {
        spu_write_word_reserved(target, OCD_CMD_HALT, 0);   // halt spu and ocd steady
        spu_write_word_reserved(target, OCD_CMD_ACCESS, 0); // allow access to memory for ocd
        spu_retreat_bp(target);
        break;
      }

      repeat++;
      if(repeat > 0) usleep(100000);
      if(repeat > 100)
      {
        LOG_WARNING("Timeout to halt after single-step.");
        return ERROR_TARGET_TIMEOUT;
      }

    } while(1);

  }
  else {
    LOG_WARNING("target is in %s status when to step, which is not allowed.", target_state_name(target));
    return ERROR_TARGET_NOT_HALTED;
  }

  //target_call_event_callbacks(target, TARGET_EVENT_HALTED);
  return ERROR_OK;
}

static int
mapu_spu_resume(struct target *target, int current, uint32_t address, int handle_breakpoints, int debug_execution)
{
  uint32_t status = spu_check_status(target);

  // halt to prepare for resuming.
	if(target->state == TARGET_RESET) {
		spu_write_word_reserved(target, OCD_CMD_HALT, 0);
		spu_write_word_reserved(target, OCD_CMD_DERESET, 0);
		spu_write_word_reserved(target, OCD_CMD_ACCESS, 0);
		status = spu_check_status(target);
	}

	if(target->state == TARGET_HALTED )
	{
    // when we resume at an sw breakpoint.
    if(CHECK_OCD_SW_BP(status))
    {
      spu_write_word_reserved(target, OCD_CMD_HALT, 0);   // halt spu and ocd steady
      spu_write_word_reserved(target, OCD_CMD_ACCESS, 0); // allow access to memory for ocd
      spu_retreat_bp(target);

      // get breakpoint address
      uint32_t bp_addr;
      spu_read_word_reserved(target, DPC_MEM_MAP, &bp_addr);
      uint32_t insn;
      target_read_u32(target, bp_addr, &insn);
      spu_write_word_reserved(target, SPROBE_MEM_MAP, insn);
    }

    spu_apply_reg_change(target);
    spu_deploy_bp(target);

    spu_start_to_run(target);
  }
  else {
    LOG_WARNING("target is in %s status when to resume, which is not allowed.", target_state_name(target));
    return ERROR_TARGET_NOT_HALTED;
  }

	target_call_event_callbacks(target, TARGET_EVENT_RESUMED);
	return ERROR_OK;
}

static int
mapu_spu_soft_reset_halt(struct target *target)
{
  target->cs_count=0;
  target->process_exited=false;
	mapu_spu_assert_reset(target);
	mapu_spu_deassert_reset(target);
	mapu_spu_halt(target);
	return ERROR_OK;
}

static int
mapu_spu_get_gdb_reg_list(struct target *target, struct reg** reg_list[], int *reg_list_size)
{
	/* get pointers to arch-specific information */
	struct mapu_spu_common *mapu_spu = target_to_mapu_spu(target);

	/* include floating point registers */
	*reg_list_size = mapu_spu->reg_cache->num_regs;
	*reg_list = & mapu_spu->reg_cache->reg_list;

	return ERROR_OK;
}

static struct reg_cache *
spu_build_reg_cache(struct target *target)
{
	/* get pointers to arch-specific information */
	struct mapu_spu_common * spu = target_to_mapu_spu(target);
	struct reg_cache **cache_p = register_get_last_cache_p(&target->reg_cache);

  struct reg_cache *cache = malloc( sizeof(struct reg_cache) );

  cache->next = 0;
	cache->name = "MaPU SPU Registers";
  cache->num_regs = sizeof(mapu_spu_reg_descs) / sizeof(struct reg_desc);
  cache->reg_list = calloc(cache->num_regs, sizeof(struct reg));;

	(*cache_p) = cache;
	spu->reg_cache = cache;

	// note that we also buffer DPC and RRPC
	void *value         = malloc( (SPU_NUM_PHY_REGS + 2) * 4 );
  bool *valid         = calloc(  SPU_NUM_PHY_REGS + 2, sizeof(bool));

	void *rewrite_value = malloc( (SPU_NUM_PHY_REGS + 2) * 4 );
	bool *dirty         = calloc(  SPU_NUM_PHY_REGS + 2, sizeof(bool));

	for(int i=0; i<cache->num_regs; i++)
	{
	  cache->reg_list[i].desc = &mapu_spu_reg_descs[i];

	  cache->reg_list[i].value = cache->reg_list[i].desc->offset + value;
    cache->reg_list[i].valid = cache->reg_list[i].desc->offset / 4 + valid;

	  cache->reg_list[i].rewrite_value = cache->reg_list[i].desc->offset + rewrite_value;
    cache->reg_list[i].dirty =         cache->reg_list[i].desc->offset / 4 +  dirty;

	  cache->reg_list[i].target = target;
	}

	return cache;
}

static struct mapu_spu_bp_reg ibp_regs[] =
{
 {0, IBP0_MEM_MAP}
};

static struct mapu_spu_bp_reg dbp_regs[] =
{
 {1, DBP0_MEM_MAP},
 {2, DBP1_MEM_MAP}
};

// this function is used only once, it can be merged into mapu_spu_target_create().
static int
spu_init_arch_info(struct target *target, struct jtag_tap *tap)
{
	struct mapu_spu_common *spu = calloc(1, sizeof(struct mapu_spu_common));
	target->arch_info = spu;
	spu->jtag_info.tap = tap;

	// breakpoints: instruction breakpont
	spu->cnt_ibp_regs       = sizeof(ibp_regs)/sizeof(struct mapu_spu_bp_reg);
	spu->num_ibp_regs_avail = sizeof(ibp_regs)/sizeof(struct mapu_spu_bp_reg);
	spu->ibp_reg_list = ibp_regs;

	// watchpoint: data breakpont
  spu->cnt_dbp_regs = sizeof(dbp_regs)/sizeof(struct mapu_spu_bp_reg);
  spu->num_dbp_regs_avail = sizeof(dbp_regs)/sizeof(struct mapu_spu_bp_reg);
  spu->dbp_reg_list = dbp_regs;

	if(target->virt == false)
		target->spu_mask = (1 << spu_use_free_bit());

	return ERROR_OK;
}

static int
mapu_spu_target_create(struct target *target, Jim_Interp *interp)
{
	return ERROR_OK;
}

static int
mapu_spu_init_target(struct command_context *cmd_ctx, struct target *target)
{
  spu_init_arch_info(target, target->tap);
  spu_build_reg_cache(target);

  // not connected to gdb client
  target->gdb_connected = false;
  target->cs_count = 0;
  target->process_exited=false;

  return ERROR_OK;
}

static int
mapu_spu_examine(struct target *target)
{
//	if (!target_was_examined(target))
//	{
//		retval = mips_ejtag_get_idcode(ejtag_info, &idcode);
//		if (retval != ERROR_OK)
//			return retval;
//		ejtag_info->idcode = idcode;
//
//		if (((idcode >> 1) & 0x7FF) == 0x29)
//		{
//			/* we are using a pic32mx so select ejtag port
//			 * as it is not selected by default */
//			mips_ejtag_set_instr(ejtag_info, MTAP_SW_ETAP);
//			LOG_DEBUG("PIC32MX Detected - using EJTAG Interface");
//			mapu_spu->is_pic32mx = true;
//		}
//	}
//
//	/* init rest of ejtag interface */
//	if ((retval = mips_ejtag_init(ejtag_info)) != ERROR_OK)
//		return retval;
//
//	if ((retval = mips32_examine(target)) != ERROR_OK)
//		return retval;

	target_set_examined(target);
	LOG_DEBUG("MaPU SPU Examined");
	return ERROR_OK;
}

static const struct command_registration mapu_spu_command_handlers[] =
    { { .name = "mapu",
        .mode = COMMAND_ANY,
        .help = "MaPU target commands",
        .chain = NULL },

      COMMAND_REGISTRATION_DONE
    };

struct target_type mapu_spu_target_type = {
		.name = "mapu_spu",
		.commands = mapu_spu_command_handlers,
		.poll = mapu_spu_poll,
		.arch_state = mapu_spu_arch_state,
		//.target_request_data = NULL,
		.spu_finish = mapu_spu_finish,
		.halt = mapu_spu_halt,
		.resume = mapu_spu_resume,
		.step = mapu_spu_step,
		.assert_reset = mapu_spu_assert_reset,
		.deassert_reset = mapu_spu_deassert_reset,
		.soft_reset_halt = mapu_spu_soft_reset_halt,
		.get_gdb_reg_list = mapu_spu_get_gdb_reg_list,
		.read_memory = mapu_spu_read_memory,
		.write_memory = mapu_spu_write_memory,
		.bulk_write_memory = mapu_spu_bulk_write_memory,
		//.checksum_memory = NULL,
		//.blank_check_memory = NULL,
		//.run_algorithm = mapu_spu_run_algorithm,
		.add_breakpoint = mapu_spu_add_breakpoint,
		.remove_breakpoint = mapu_spu_remove_breakpoint,
		.add_watchpoint = mapu_spu_add_watchpoint,
		.remove_watchpoint = mapu_spu_remove_watchpoint,
		.target_create = mapu_spu_target_create,
		.init_target = mapu_spu_init_target,
		.examine = mapu_spu_examine
};
