/*
 * mapu_spu.h
 *
 *  Created on: Mar 5, 2012
 *      Author: root
 */

#ifndef MAPU_SPU_H
#define MAPU_SPU_H

#include <helper/types.h>
#include <target/target.h>

#define SPU_NUM_R_REGS 32
#define SPU_NUM_J_REGS 32

#define SPU_NUM_D_REGS 16
#define SPU_NUM_PHY_REGS 	(SPU_NUM_R_REGS + SPU_NUM_J_REGS)
#define SPU_NUM_ALL_REGS 	(SPU_NUM_D_REGS + SPU_NUM_PHY_REGS)

#define SPU_DR_BYTE_NUM      9
#define SPU_DR_BIT_NUM      (SPU_DR_BYTE_NUM * 8)

// convert a 8-bit string (less than 8 bits is allowed) to a byte.
#define BYTE(bits)      ((bits>>28<<7)+(bits>><<)+(bits>><<)+(bits>><<) \
					              +(bits>><<)+(bits>><<)+(bits>><<)+(bits>><<))

#define     DPC_MEM_MAP     0xFF000000
#define    RRPC_MEM_MAP     0xFF000004

#define  SPROBE_MEM_MAP     0xFF400000
#define    IBP0_MEM_MAP     0xFF400004
#define    DBP0_MEM_MAP     0xFF400008
#define    DBP1_MEM_MAP     0xFF40000C

#define SPU_HW_BP_CTRL_REG  0xFF000100
#define SPU_STATUS_REG	    0xFF000104

#define SPU_TRAP            0x00800000 // software breakpoint codings, trap.
#define SPU_TRAP_MASK       0x7FE00000

#define OCD_CMD_HALT        0xFF400100
#define OCD_CMD_RUN         0xFF400101
#define OCD_CMD_STEP 		    0xFF400102

#define OCD_CMD_RESET 		  0xFF400103
#define OCD_CMD_DERESET 	  0xFF400104

#define OCD_CMD_ACCESS 		  0xFF400105
#define OCD_CMD_DEACCESS 	  0xFF400106

#define R_REG_MEM_MAP_BASE 	0xFF800000
#define J_REG_MEM_MAP_BASE 	0xFF800080

// jtag packet accessor macros
#define SET_BLOCK_ADDR(bytes, addr32)  	    bytes[0]=(char)((addr32    )&0xFF); \
                                            bytes[1]=(char)((addr32>> 8)&0xFF); \
                                            bytes[2]=(char)((addr32>>16)&0xFF); \
                                            bytes[3]=(char)((addr32>>24)&0xFF)

#define SET_WORD_ADDR(bytes, addr32)        bytes[4]=(char)((addr32    )&0xFF); \
                                            bytes[5]=(char)((addr32>> 8)&0xFF); \
                                            bytes[6]=(char)((addr32>>16)&0xFF)

#define SET_TARGET_SELECTED(bytes, mask)    bytes[7]=(mask)

#define SET_PACKET_DESC(bytes, desc)        bytes[8]=(desc)

#define CLEAR_PACKET(bytes)				      { for(int k = 0; k < 9; k++) bytes[k] = 0; }

// PD: Pack Desc
// bit 7~6: do-not-care
// bit 5~4:
#define PD_CTRL         0
#define PD_ACCESS       (1<<4)
#define PD_DATA         (2<<4)
#define PD_RESERVED     (3<<4)

#define PD_ACCESS_WR    0
#define PD_ACCESS_WW    1
#define PD_ACCESS_BR    2
#define PD_ACCESS_BW    3

// used in target poll
enum _mapu_spu_status_ {
	mapu_spu_reset=0,
	mapu_spu_running=1,
	mapu_spu_halted=2
};


// checker whether the input packet from ocd hardware is valid.
#define CHECK_INPUT_PACKET_VALID(bytes)   ((bytes[8]>>7 & 1) == 1)

#define CHECK_SOC_OCD_BUSY(bytes)         ((bytes[4]>>0 & 1) == 1)
#define CHECK_CMD_PACKET_ILLEGAL(bytes)   ((bytes[4]>>1 & 1) == 1)
#define CHECK_DATA_PACKET_ILLEGAL(bytes)  ((bytes[4]>>2 & 1) == 1)


// these macro are used to check local ocd (to compare with soc ocd).
// whether monitor is ready for receiving packages.
// this is no use since we always check soc ocd before we send a
// debug packet.
#define CHECK_OCD_READY(status)           ((status>>0 & 1) == 1)

// hardware breakpoint is hit, including watchpoint.
#define CHECK_OCD_HW_BP(status)           ((status>>1 & 1) == 1)

// stop request from arm
#define CHECK_ARM_STOP_SPU_REQ(status)    ((status>>2 & 1) == 1)

// software breakpoint is hit.
#define CHECK_OCD_SW_BP(status)           ((status>>3 & 1) == 1)

// if some internal operations want to stop spu itself (just an internal-stop request)
// for example, spu.stop instruction is emitted and will be executed soon.
#define CHECK_SPU_STOP_SPU_REQ(status)    ((status>>4 & 1) == 1)

// after ocd halts spu execution (thus can debug and enter into debug mode),
// a request ack signal is set. (ack for an external-stop request)
#define CHECK_OCD_STOP_SPU_ACK(status)    ((status>>5 & 1) == 1)

#define GET_SPU_STATUS(status)             (status>>6 & 3)

#include <jtag/jtag.h>

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

/*
enum e_generic {
  eg_unknown = 0,
  eg_pc,
  eg_sp,
  eg_fp,
  eg_ra,
  eg_flags,
  eg_arg0, eg_arg1, eg_arg2, eg_arg3, eg_arg4,
  eg_arg5, eg_arg6, eg_arg7, eg_arg8, eg_arg9
};
*/

struct mapu_spu_reg_type
{
    enum e_set set;
    const char* set_name;

    enum e_format format;
    const char* format_name;

    enum e_encoding encoding;
    const char* encoding_name;
};

struct mapu_spu_reg_info  // record architecture-specific info for a 'struct reg' object
{
    struct mapu_spu_reg_type * type;
    const char *alt_name; // this is also generic name for lldb

    uint32_t  no;   // this is also used as dwarf/gcc_no and its index;
    uint32_t  addr; // memory mapped address in SPU for this reg via jtag access

    const uint32_t super_regs[2]; // record their index
    uint32_t   num_super_regs;

    const uint32_t sub_regs[4]; // record their index
    uint32_t   num_sub_regs;
};

struct mapu_jtag
{
	struct jtag_tap *tap;

	uint32_t scann_size;
	uint32_t scann_instr;
	uint32_t cur_scan_chain;

	uint32_t intest_instr;
};

void mapu_jtag_set_instr(struct mapu_jtag *jtag_info, int new_instr);
int  mapu_jtag_drscan_package(struct mapu_jtag *jtag_info, uint8_t *data, bool isout);

struct mapu_spu_bp_reg    // matcher
{
  // bit index
  // hw-bp is enabled(unmasked) if bits[mask_bit] = 0
  // hw-bp is disabled(masked)  if bits[mask_bit] = 1
  uint32_t mask_bit;

  /* memory-mapped address of this monitor register in SPU address space. */
  uint32_t mem_map;
};

struct mapu_spu_common
{
		uint32_t common_magic;

		uint32_t status;       // spu status
		uint32_t hw_bp_status; // hardware bp status

		struct mapu_jtag jtag_info;
		struct reg_cache *reg_cache;

		/* working area for fastdata access */
		//  struct working_area *fast_data_area;
		int bp_scanned;

		int cnt_ibp_regs;
		int cnt_dbp_regs;
		int num_ibp_regs_avail;
		int num_dbp_regs_avail;
		struct mapu_spu_bp_reg *ibp_reg_list;
		struct mapu_spu_bp_reg *dbp_reg_list;
};

static inline struct mapu_spu_common *
target_to_mapu_spu(struct target *target)
{
	return (struct mapu_spu_common*) target->arch_info;
}

#endif /* MAPU_SPU_H */
