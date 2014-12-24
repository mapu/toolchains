#ifndef __REGISTER_DESC_HH__
#define __REGISTER_DESC_HH__

#include <stdint.h>

enum e_set {
  es_unknown = 0,
  es_int =  1, // i32 for llvm
  es_sflt = 2, // f32 for llvm
  es_dflt = 3, // f64 for llvm
  es_addr = 4, // ptr for llvm
};

enum e_register
{
  e_R0 = 0, e_R1, e_R2, e_R3, e_R4, e_R5, e_R6, e_R7, e_R8, e_R9,
  e_R10, e_R11, e_R12, e_R13, e_R14, e_R15, e_R16, e_R17, e_R18, e_R19,
  e_R20, e_R21, e_R22, e_R23, e_R24, e_R25, e_R26, e_R27, e_R28, e_R29,
  e_R30, e_R31,

  e_J0, e_J1, e_J2, e_J3, e_J4, e_J5, e_J6, e_J7, e_J8, e_J9,
  e_J10, e_J11, e_J12, e_J13, e_J14, e_J15, e_J16, e_J17, e_J18, e_J19,
  e_J20, e_J21, e_J22, e_J23, e_J24, e_J25, e_J26, e_J27, e_J28, e_J29,
  e_J30, e_J31,

  e_DPC, e_RRPC,

  e_S0, e_S1, e_S2, e_S3, e_S4, e_S5, e_S6, e_S7, e_S8, e_S9,
  e_S10, e_S11, e_S12, e_S13, e_S14, e_S15, e_S16, e_S17, e_S18, e_S19,
  e_S20, e_S21, e_S22, e_S23, e_S24, e_S25, e_S26, e_S27, e_S28, e_S29,
  e_S30, e_S31,

  e_DR0, e_DR1, e_DR2, e_DR3, e_DR4, e_DR5, e_DR6, e_DR7, e_DR8, e_DR9,
  e_DR10, e_DR11, e_DR12, e_DR13, e_DR14, e_DR15
};

enum e_format {
  ef_unknown = 0,
  ef_binary=1,
  ef_decimal = 2,
  ef_hex = 3,
  ef_float = 4,

  ef_vector_sint8,
  ef_vector_sint16,
  ef_vector_sint32,

  ef_vector_uint8,
  ef_vector_uint16,
  ef_vector_uint32,
  ef_vector_uint128,

  ef_vector_float32
};

enum e_encoding
{
  ec_unknown = 0,
  ec_uint=1,
  ec_sint = 2,
  ec_ieee754 = 3,
  ec_vector = 4
};

struct mapu_spu_reg_type
{
    enum e_set set;
    const char* set_name;

    enum e_format format;
    const char* format_name;

    enum e_encoding encoding;
    const char* encoding_name;
};

struct mapu_spu_reg_desc  // record architecture-specific info for a 'struct reg' object
{
    const char *name;
    uint32_t size;   // number of bit

    struct mapu_spu_reg_type *type;
    const char *alt_name; // this is also generic name for lldb

    uint32_t id;   // this is also used as dwarf/gcc_no and its index;

    uint32_t offset; // offset in in-memory register cache

    const uint32_t super_regs[2]; // record their index
    uint32_t   num_super_regs;

    const uint32_t sub_regs[4];   // record their index
    uint32_t   num_sub_regs;
};

struct mapu_spu_reg_desc* getMAPUSPURegDescList(uint32_t * sz);
struct mapu_spu_reg_desc* getMAPUSPURegDesc(uint32_t num);

#endif /* __REGISTER_DESC_HH__ */
