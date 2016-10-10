#include "register_desc.hh"

// corresponding to register set
static struct mapu_spu_reg_type mapu_spu_reg_types [] =
{  { es_unknown, "Unknown Register",           ef_unknown, "unknown", ec_unknown, "unknown"},

   // i32
   { es_int,     "Integer Register",           ef_hex,     "hex",     ec_sint,    "sint"},

   // f32
   { es_sflt,    "Single Precise FP Register", ef_float,   "float",   ec_ieee754, "ieee754"},

   // f64
   {  es_dflt,   "Double Precise FP Register", ef_float,   "float",   ec_ieee754, "ieee754"},

   // address
   {  es_addr,   "Address Register" ,          ef_hex,     "hex",     ec_uint,    "uint"},
};

/*struct mapu_spu_reg_desc  // record architecture-specific info for a 'struct reg' object
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

static struct mapu_spu_reg_desc mapu_spu_reg_descs[] =
{
   { "R0",      32, &mapu_spu_reg_types[es_int], "arg1", e_R0 , (0<<2) , {e_S0, e_DR0}, 2, {e_S0}, 1 },
   { "R1",      32, &mapu_spu_reg_types[es_int], "arg2", e_R1 , (1<<2) , {e_S1, e_DR0}, 2, {e_S1}, 1 },
   { "R2",      32, &mapu_spu_reg_types[es_int], "arg3", e_R2 , (2<<2) , {e_S2, e_DR1}, 2, {e_S2}, 1 },
   { "R3",      32, &mapu_spu_reg_types[es_int], "arg4", e_R3 , (3<<2) , {e_S3, e_DR1}, 2, {e_S3}, 1 },
   { "R4",      32, &mapu_spu_reg_types[es_int], "arg5", e_R4 , (4<<2) , {e_S4, e_DR2}, 2, {e_S4}, 1 },
   { "R5",      32, &mapu_spu_reg_types[es_int], "arg6", e_R5 , (5<<2) , {e_S5, e_DR2}, 2, {e_S5}, 1 },
   { "R6",      32, &mapu_spu_reg_types[es_int], "arg7", e_R6 , (6<<2) , {e_S6, e_DR3}, 2, {e_S6}, 1 },
   { "R7",      32, &mapu_spu_reg_types[es_int], "arg8", e_R7 , (7<<2) , {e_S7, e_DR3}, 2, {e_S7}, 1 },
   { "R8",      32, &mapu_spu_reg_types[es_int], 0     , e_R8 , (8<<2) , {e_S8, e_DR4}, 2, {e_S8}, 1 },
   { "R9",      32, &mapu_spu_reg_types[es_int], 0     , e_R9 , (9<<2) , {e_S9, e_DR4}, 2, {e_S9}, 1 },
   { "R10",     32, &mapu_spu_reg_types[es_int], 0     , e_R10, (10<<2), {e_S10, e_DR5}, 2, {e_S10}, 1 },
   { "R11",     32, &mapu_spu_reg_types[es_int], 0     , e_R11, (11<<2), {e_S11, e_DR5}, 2, {e_S11}, 1 },
   { "R12",     32, &mapu_spu_reg_types[es_int], 0     , e_R12, (12<<2), {e_S12, e_DR6}, 2, {e_S12}, 1 },
   { "R13",     32, &mapu_spu_reg_types[es_int], 0     , e_R13, (13<<2), {e_S13, e_DR6}, 2, {e_S13}, 1 },
   { "R14",     32, &mapu_spu_reg_types[es_int], 0     , e_R14, (14<<2), {e_S14, e_DR7}, 2, {e_S14}, 1 },
   { "R15",     32, &mapu_spu_reg_types[es_int], 0     , e_R15, (15<<2), {e_S15, e_DR7}, 2, {e_S15}, 1 },
   { "R16",     32, &mapu_spu_reg_types[es_int], 0     , e_R16, (16<<2), {e_S16, e_DR8}, 2, {e_S16}, 1 },
   { "R17",     32, &mapu_spu_reg_types[es_int], 0     , e_R17, (17<<2), {e_S17, e_DR8}, 2, {e_S17}, 1 },
   { "R18",     32, &mapu_spu_reg_types[es_int], 0     , e_R18, (18<<2), {e_S18, e_DR9}, 2, {e_S18}, 1 },
   { "R19",     32, &mapu_spu_reg_types[es_int], 0     , e_R19, (19<<2), {e_S19, e_DR9}, 2, {e_S19}, 1 },
   { "R20",     32, &mapu_spu_reg_types[es_int], 0     , e_R20, (20<<2), {e_S20, e_DR10}, 2, {e_S20}, 1 },
   { "R21",     32, &mapu_spu_reg_types[es_int], 0     , e_R21, (21<<2), {e_S21, e_DR10}, 2, {e_S21}, 1 },
   { "R22",     32, &mapu_spu_reg_types[es_int], 0     , e_R22, (22<<2), {e_S22, e_DR11}, 2, {e_S22}, 1 },
   { "R23",     32, &mapu_spu_reg_types[es_int], 0     , e_R23, (23<<2), {e_S23, e_DR11}, 2, {e_S23}, 1 },
   { "R24",     32, &mapu_spu_reg_types[es_int], 0     , e_R24, (24<<2), {e_S24, e_DR12}, 2, {e_S24}, 1 },
   { "R25",     32, &mapu_spu_reg_types[es_int], 0     , e_R25, (25<<2), {e_S25, e_DR12}, 2, {e_S25}, 1 },
   { "R26",     32, &mapu_spu_reg_types[es_int], 0     , e_R26, (26<<2), {e_S26, e_DR13}, 2, {e_S26}, 1 },
   { "R27",     32, &mapu_spu_reg_types[es_int], 0     , e_R27, (27<<2), {e_S27, e_DR13}, 2, {e_S27}, 1 },
   { "R28",     32, &mapu_spu_reg_types[es_int], 0     , e_R28, (28<<2), {e_S28, e_DR14}, 2, {e_S28}, 1 },
   { "R29",     32, &mapu_spu_reg_types[es_int], 0     , e_R29, (29<<2), {e_S29, e_DR14}, 2, {e_S29}, 1 },
   { "R30",     32, &mapu_spu_reg_types[es_int], 0     , e_R30, (30<<2), {e_S30, e_DR15}, 2, {e_S30}, 1 },
   { "R31",     32, &mapu_spu_reg_types[es_int], 0     , e_R31, (31<<2), {e_S31, e_DR15}, 2, {e_S31}, 1 },

   { "J0",      32, &mapu_spu_reg_types[es_addr], 0    , e_J0, (32<<2)+(0<<2), {}, 0, {}, 0 },
   { "J1",      32, &mapu_spu_reg_types[es_addr], 0    , e_J1, (32<<2)+(1<<2), {}, 0, {}, 0 },
   { "J2",      32, &mapu_spu_reg_types[es_addr], 0    , e_J2, (32<<2)+(2<<2), {}, 0, {}, 0 },
   { "J3",      32, &mapu_spu_reg_types[es_addr], 0    , e_J3, (32<<2)+(3<<2), {}, 0, {}, 0 },
   { "J4",      32, &mapu_spu_reg_types[es_addr], 0    , e_J4, (32<<2)+(4<<2), {}, 0, {}, 0 },
   { "J5",      32, &mapu_spu_reg_types[es_addr], 0    , e_J5, (32<<2)+(5<<2), {}, 0, {}, 0 },
   { "J6",      32, &mapu_spu_reg_types[es_addr], 0    , e_J6, (32<<2)+(6<<2), {}, 0, {}, 0 },
   { "J7",      32, &mapu_spu_reg_types[es_addr], 0    , e_J7, (32<<2)+(7<<2), {}, 0, {}, 0 },
   { "J8",      32, &mapu_spu_reg_types[es_addr], 0    , e_J8, (32<<2)+(8<<2), {}, 0, {}, 0 },
   { "J9",      32, &mapu_spu_reg_types[es_addr], 0    , e_J9, (32<<2)+(9<<2), {}, 0, {}, 0 },
   { "J10",     32, &mapu_spu_reg_types[es_addr], 0    , e_J10, (32<<2)+(10<<2), {}, 0, {}, 0 },
   { "J11",     32, &mapu_spu_reg_types[es_addr], 0    , e_J11, (32<<2)+(11<<2), {}, 0, {}, 0 },
   { "J12",     32, &mapu_spu_reg_types[es_addr], 0    , e_J12, (32<<2)+(12<<2), {}, 0, {}, 0 },
   { "J13",     32, &mapu_spu_reg_types[es_addr], 0    , e_J13, (32<<2)+(13<<2), {}, 0, {}, 0 },
   { "J14",     32, &mapu_spu_reg_types[es_addr], 0    , e_J14, (32<<2)+(14<<2), {}, 0, {}, 0 },
   { "J15",     32, &mapu_spu_reg_types[es_addr], 0    , e_J15, (32<<2)+(15<<2), {}, 0, {}, 0 },
   { "J16",     32, &mapu_spu_reg_types[es_addr], 0    , e_J16, (32<<2)+(16<<2), {}, 0, {}, 0 },
   { "J17",     32, &mapu_spu_reg_types[es_addr], 0    , e_J17, (32<<2)+(17<<2), {}, 0, {}, 0 },
   { "J18",     32, &mapu_spu_reg_types[es_addr], 0    , e_J18, (32<<2)+(18<<2), {}, 0, {}, 0 },
   { "J19",     32, &mapu_spu_reg_types[es_addr], 0    , e_J19, (32<<2)+(19<<2), {}, 0, {}, 0 },
   { "J20",     32, &mapu_spu_reg_types[es_addr], 0    , e_J20, (32<<2)+(20<<2), {}, 0, {}, 0 },
   { "J21",     32, &mapu_spu_reg_types[es_addr], 0    , e_J21, (32<<2)+(21<<2), {}, 0, {}, 0 },
   { "J22",     32, &mapu_spu_reg_types[es_addr], 0    , e_J22, (32<<2)+(22<<2), {}, 0, {}, 0 },
   { "J23",     32, &mapu_spu_reg_types[es_addr], 0    , e_J23, (32<<2)+(23<<2), {}, 0, {}, 0 },
   { "J24",     32, &mapu_spu_reg_types[es_addr], 0    , e_J24, (32<<2)+(24<<2), {}, 0, {}, 0 },
   { "J25",     32, &mapu_spu_reg_types[es_addr], 0    , e_J25, (32<<2)+(25<<2), {}, 0, {}, 0 },
   { "J26",     32, &mapu_spu_reg_types[es_addr], 0    , e_J26, (32<<2)+(26<<2), {}, 0, {}, 0 },
   { "J27",     32, &mapu_spu_reg_types[es_addr], 0    , e_J27, (32<<2)+(27<<2), {}, 0, {}, 0 },
   { "J28",     32, &mapu_spu_reg_types[es_addr], "fp" , e_J28, (32<<2)+(28<<2), {}, 0, {}, 0 },
   { "J29",     32, &mapu_spu_reg_types[es_addr], "sp" , e_J29, (32<<2)+(29<<2), {}, 0, {}, 0 },
   { "J30",     32, &mapu_spu_reg_types[es_addr], "ra" , e_J30, (32<<2)+(30<<2), {}, 0, {}, 0 },
   { "J31",     32, &mapu_spu_reg_types[es_addr], 0    , e_J31, (32<<2)+(31<<2), {}, 0, {}, 0 },

   { "DPC",     32, &mapu_spu_reg_types[es_addr], "pc", e_DPC,  (32<<2)+(32<<2), {}, 0, {}, 0 },
   { "RRPC",    32, &mapu_spu_reg_types[es_addr],"flags",e_RRPC,(32<<2)+(33<<2), {}, 0, {}, 0 },

   // synthetic registers
   { "S0",      32, &mapu_spu_reg_types[es_sflt], 0    , e_S0, (0<<2), {e_R0, e_DR0}, 2, {e_R0}, 1 },
   { "S1",      32, &mapu_spu_reg_types[es_sflt], 0    , e_S1, (1<<2), {e_R1, e_DR0}, 2, {e_R1}, 1 },
   { "S2",      32, &mapu_spu_reg_types[es_sflt], 0    , e_S2, (2<<2), {e_R2, e_DR1}, 2, {e_R2}, 1 },
   { "S3",      32, &mapu_spu_reg_types[es_sflt], 0    , e_S3, (3<<2), {e_R3, e_DR1}, 2, {e_R3}, 1 },
   { "S4",      32, &mapu_spu_reg_types[es_sflt], 0    , e_S4, (4<<2), {e_R4, e_DR2}, 2, {e_R4}, 1 },
   { "S5",      32, &mapu_spu_reg_types[es_sflt], 0    , e_S5, (5<<2), {e_R5, e_DR2}, 2, {e_R5}, 1 },
   { "S6",      32, &mapu_spu_reg_types[es_sflt], 0    , e_S6, (6<<2), {e_R6, e_DR3}, 2, {e_R6}, 1 },
   { "S7",      32, &mapu_spu_reg_types[es_sflt], 0    , e_S7, (7<<2), {e_R7, e_DR3}, 2, {e_R7}, 1 },
   { "S8",      32, &mapu_spu_reg_types[es_sflt], 0    , e_S8, (8<<2), {e_R8, e_DR4}, 2, {e_R8}, 1 },
   { "S9",      32, &mapu_spu_reg_types[es_sflt], 0    , e_S9, (9<<2), {e_R9, e_DR4}, 2, {e_R9}, 1 },
   { "S10",     32, &mapu_spu_reg_types[es_sflt], 0    , e_S10, (10<<2), {e_R10, e_DR5}, 2, {e_R10}, 1 },
   { "S11",     32, &mapu_spu_reg_types[es_sflt], 0    , e_S11, (11<<2), {e_R11, e_DR5}, 2, {e_R11}, 1 },
   { "S12",     32, &mapu_spu_reg_types[es_sflt], 0    , e_S12, (12<<2), {e_R12, e_DR6}, 2, {e_R12}, 1 },
   { "S13",     32, &mapu_spu_reg_types[es_sflt], 0    , e_S13, (13<<2), {e_R13, e_DR6}, 2, {e_R13}, 1 },
   { "S14",     32, &mapu_spu_reg_types[es_sflt], 0    , e_S14, (14<<2), {e_R14, e_DR7}, 2, {e_R14}, 1 },
   { "S15",     32, &mapu_spu_reg_types[es_sflt], 0    , e_S15, (15<<2), {e_R15, e_DR7}, 2, {e_R15}, 1 },
   { "S16",     32, &mapu_spu_reg_types[es_sflt], 0    , e_S16, (16<<2), {e_R16, e_DR8}, 2, {e_R16}, 1 },
   { "S17",     32, &mapu_spu_reg_types[es_sflt], 0    , e_S17, (17<<2), {e_R17, e_DR8}, 2, {e_R17}, 1 },
   { "S18",     32, &mapu_spu_reg_types[es_sflt], 0    , e_S18, (18<<2), {e_R18, e_DR9}, 2, {e_R18}, 1 },
   { "S19",     32, &mapu_spu_reg_types[es_sflt], 0    , e_S19, (19<<2), {e_R19, e_DR9}, 2, {e_R19}, 1 },
   { "S20",     32, &mapu_spu_reg_types[es_sflt], 0    , e_S20, (20<<2), {e_R20, e_DR10}, 2, {e_R20}, 1 },
   { "S21",     32, &mapu_spu_reg_types[es_sflt], 0    , e_S21, (21<<2), {e_R21, e_DR10}, 2, {e_R21}, 1 },
   { "S22",     32, &mapu_spu_reg_types[es_sflt], 0    , e_S22, (22<<2), {e_R22, e_DR11}, 2, {e_R22}, 1 },
   { "S23",     32, &mapu_spu_reg_types[es_sflt], 0    , e_S23, (23<<2), {e_R23, e_DR11}, 2, {e_R23}, 1 },
   { "S24",     32, &mapu_spu_reg_types[es_sflt], 0    , e_S24, (24<<2), {e_R24, e_DR12}, 2, {e_R24}, 1 },
   { "S25",     32, &mapu_spu_reg_types[es_sflt], 0    , e_S25, (25<<2), {e_R25, e_DR12}, 2, {e_R25}, 1 },
   { "S26",     32, &mapu_spu_reg_types[es_sflt], 0    , e_S26, (26<<2), {e_R26, e_DR13}, 2, {e_R26}, 1 },
   { "S27",     32, &mapu_spu_reg_types[es_sflt], 0    , e_S27, (27<<2), {e_R27, e_DR13}, 2, {e_R27}, 1 },
   { "S28",     32, &mapu_spu_reg_types[es_sflt], 0    , e_S28, (28<<2), {e_R28, e_DR14}, 2, {e_R28}, 1 },
   { "S29",     32, &mapu_spu_reg_types[es_sflt], 0    , e_S29, (29<<2), {e_R29, e_DR14}, 2, {e_R29}, 1 },
   { "S30",     32, &mapu_spu_reg_types[es_sflt], 0    , e_S30, (30<<2), {e_R30, e_DR15}, 2, {e_R30}, 1 },
   { "S31",     32, &mapu_spu_reg_types[es_sflt], 0    , e_S31, (31<<2), {e_R31, e_DR15}, 2, {e_R31}, 1 },

   { "DR0",     64, &mapu_spu_reg_types[es_dflt], 0    , e_DR0, (0<<3), {}, 0, {e_R0, e_R1, e_S0, e_S1}, 4 },
   { "DR1",     64, &mapu_spu_reg_types[es_dflt], 0    , e_DR1, (1<<3), {}, 0, {e_R2, e_R3, e_S2, e_S3}, 4 },
   { "DR2",     64, &mapu_spu_reg_types[es_dflt], 0    , e_DR2, (2<<3), {}, 0, {e_R4, e_R5, e_S4, e_S5}, 4 },
   { "DR3",     64, &mapu_spu_reg_types[es_dflt], 0    , e_DR3, (3<<3), {}, 0, {e_R6, e_R7, e_S6, e_S7}, 4 },
   { "DR4",     64, &mapu_spu_reg_types[es_dflt], 0    , e_DR4, (4<<3), {}, 0, {e_R8, e_R9, e_S8, e_S9}, 4 },
   { "DR5",     64, &mapu_spu_reg_types[es_dflt], 0    , e_DR5, (5<<3), {}, 0, {e_R10, e_R11, e_S10, e_S11}, 4 },
   { "DR6",     64, &mapu_spu_reg_types[es_dflt], 0    , e_DR6, (6<<3), {}, 0, {e_R12, e_R13, e_S12, e_S13}, 4 },
   { "DR7",     64, &mapu_spu_reg_types[es_dflt], 0    , e_DR7, (7<<3), {}, 0, {e_R14, e_R15, e_S14, e_S15}, 4 },
   { "DR8",     64, &mapu_spu_reg_types[es_dflt], 0    , e_DR8, (8<<3), {}, 0, {e_R16, e_R17, e_S16, e_S17}, 4 },
   { "DR9",     64, &mapu_spu_reg_types[es_dflt], 0    , e_DR9, (9<<3), {}, 0, {e_R18, e_R19, e_S18, e_S19}, 4 },
   { "DR10",    64, &mapu_spu_reg_types[es_dflt], 0    , e_DR10, (10<<3), {}, 0, {e_R20, e_R21, e_S20, e_S21}, 4 },
   { "DR11",    64, &mapu_spu_reg_types[es_dflt], 0    , e_DR11, (11<<3), {}, 0, {e_R22, e_R23, e_S22, e_S23}, 4 },
   { "DR12",    64, &mapu_spu_reg_types[es_dflt], 0    , e_DR12, (12<<3), {}, 0, {e_R24, e_R25, e_S24, e_S25}, 4 },
   { "DR13",    64, &mapu_spu_reg_types[es_dflt], 0    , e_DR13, (13<<3), {}, 0, {e_R26, e_R27, e_S26, e_S27}, 4 },
   { "DR14",    64, &mapu_spu_reg_types[es_dflt], 0    , e_DR14, (14<<3), {}, 0, {e_R28, e_R29, e_S28, e_S29}, 4 },
   { "DR15",    64, &mapu_spu_reg_types[es_dflt], 0    , e_DR15, (15<<3), {}, 0, {e_R30, e_R31, e_S30, e_S31}, 4 },
};

struct mapu_spu_reg_desc* getMAPUSPURegDescList(uint32_t * sz)
{
    *sz = sizeof(mapu_spu_reg_descs) / sizeof(struct mapu_spu_reg_desc);
    return mapu_spu_reg_descs;
}

struct mapu_spu_reg_desc* getMAPUSPURegDesc(uint32_t num)
{
    if(num < sizeof(mapu_spu_reg_descs) / sizeof(struct mapu_spu_reg_desc))
        return &mapu_spu_reg_descs[num];
    else
        return 0;
}
