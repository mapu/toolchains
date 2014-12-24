/*
 * turbodec_fix32.hh
 *
 *  Created on: Jun 27, 2014
 *      Author: wujn
 */

#ifndef TURBODEC_FIX32_HH_
#define TURBODEC_FIX32_HH_

#include "base/statistics.hh"
#include "base/bitunion.hh"

#include "cop.hh"
#include "params/FixPointTurboDecoder.hh"

#define FIXPOINTWIDTH 6
#define EXTWIDTH 8
#define METRICWIDTH 6
#define MAX_EXT ((1L << (METRICWIDTH)) - 1)
#define MIN_EXT (-(1L << (METRICWIDTH)))

#define SMASK (0xFFFFUL >> (16 - EXTWIDTH))
#define UMASK (0xFFFFUL >> (16 - EXTWIDTH + 1))

#define SEXT(x) (((x) & ~UMASK) ? (x) | ~UMASK : (x))

#define COMPL_SUB(x, y) (((x) + ~(y) + 1) & SMASK)
#define COMPL_ADD(x, y) (((x) + (y)) & SMASK)
#define CMP_MOD(x, y) ((((x) & ~UMASK) != 0) ^ (((y) & ~UMASK) != 0) ^\
                       (((uint16_t)(x) & UMASK) > ((uint16_t)(y) & UMASK)))

#define SAT(x, y) ((x) >= (1 << ((y) - 1)) ? (x) = ((1 << ((y) - 1)) - 1) :\
                   (x) < -(1 << ((y) - 1)) ? (x) = -(1 << ((y) - 1)) : (x))

class FixPointTurboDecoder : public CoProcessor {
  typedef int8_t sys_t;
  typedef int16_t metric_t;
  enum REG_MAP {
    TURBODEC_CMD           = 0x0,
    TURBODEC_SYS_BASE      = 0x4,
    TURBODEC_PAR1_BASE     = 0x8,
    TURBODEC_PAR2_BASE     = 0xC,
    TURBODEC_OUTPUT_BASE   = 0x10,
    TURBODEC_MAX_ITER_NUM  = 0x14,
    TURBODEC_CB_LEN        = 0x18,
    TURBODEC_SB_LEN        = 0x1C,
    TURBODEC_SB_NUM        = 0x20,
    TURBODEC_SW_LEN1       = 0x24,
    TURBODEC_SW_LEN2       = 0x28,
    TURBODEC_SW_NUM        = 0x2C,
    TURBODEC_SCALING       = 0x30,
    TURBODEC_F2            = 0x34,
    TURBODEC_E1            = 0x38,
    TURBODEC_E2            = 0x3C,
    TURBODEC_BETA_INIT_1   = 0x40,
    TURBODEC_BETA_INIT_2   = 0x60,
    TURBODEC_INTLV_CONF0   = 0x80,
    TURBODEC_INTLV_CONF1   = 0x84,
    TURBODEC_INTLV_CONF2   = 0x88,
    TURBODEC_INTLV_CONF3   = 0x8C,
    TURBODEC_CRC_TYPE      = 0x90,
    TURBODEC_CRC_SW        = 0x94
  };

  enum DEC_SUBOP {
    LOAD_SW0 = 0,
    LOAD_SW1,
    BETA0,
    ALPHA0,
    BETA1,
    ALPHA1,
    EXTRINSIC,
    PRE_INTERLEAVE,
    POST_INTERLEAVE0,
    POST_INTERLEAVE1,
    LLR,
    GAMMA0,
    GAMMA1,
    LOAD_PAR,
    HARD_DECISION,
    ITER_END,
  };

  static const int COMP_BETA0 = 1 << BETA0 | 1 << EXTRINSIC | 1 << GAMMA0;
  static const int COMP_BETA1 = 1 << BETA1 | 1 << GAMMA1;
  static const int COMP_ALPHA0 = 1 << ALPHA0 | 1 << LLR | 1 << PRE_INTERLEAVE;
  static const int COMP_ALPHA1 = 1 << ALPHA1 | 1 << LLR | 1 << PRE_INTERLEAVE;
  static const int COMP_LOAD_PAR0 = 1 << LOAD_PAR | 1 << POST_INTERLEAVE0;
  static const int COMP_LOAD_PAR1 = 1 << LOAD_PAR | 1 << POST_INTERLEAVE1;


  typedef metric_t state_metric[8];
  typedef metric_t branch_metric[2];

  static const state_metric alpha_initial_value;
  static const state_metric beta_initial_value;
  static const state_metric beta_initial_value_ep;
  typedef struct {
    Addr p_sys_base;
    Addr p_par1_base;
    Addr p_par2_base;
    Addr p_output_base;
    uint32_t p_max_interation_num;
    uint32_t p_crc_type;
    uint32_t p_crc_sw;
    uint32_t p_cb_len;
    uint32_t p_sb_len;
    uint32_t p_sb_num;
    uint32_t p_sw_len_1;
    uint32_t p_sw_len_2;
    uint32_t p_sw_num;
    uint32_t p_scaling_factor;
    uint32_t p_f2;
    uint32_t p_e1;
    uint32_t p_e2;
    state_metric p_beta_initial_value_cb1;
    state_metric p_beta_initial_value_cb2;
  } turbo_conf;

  static const uint32_t DEC_STATES[];

  static const uint32_t NUM_SUB_DEC = 4;
  static const uint32_t NUM_PIPELINE_SW = 2;
  static const uint32_t MAX_SW_LEN = 128;
  static const uint32_t MIN_SW_LEN = 16;
  static const uint32_t MAX_CB_LEN = 6144;
  static const uint32_t MAX_SW_NUM = MAX_CB_LEN / NUM_SUB_DEC / MIN_SW_LEN;

  class Interleaver {
	FixPointTurboDecoder *parent;
    uint32_t id;
    uint32_t t_as;
    uint32_t t_gi;
    Addr offset;
    uint32_t cid;

    uint32_t _t_as;
    uint32_t _t_gi;
    Addr _offset;
    uint32_t _cid;

    uint32_t t_as0;
    uint32_t t_gi0;
    uint32_t cid0;

  public:
    Interleaver(FixPointTurboDecoder* td, uint32_t idx) :
      parent(td), id(idx),
      t_as(0), t_gi(0), offset(0), cid(0),
      _t_as(0), _t_gi(0), _offset(0), _cid(0),
      t_as0(0), t_gi0(0), cid0(0) {}
    ~Interleaver() {}

    void compute_inter_inner_idx();

    void reset();

    void set(uint32_t as, uint32_t gi, uint32_t cid) {
      t_as0 = as;
      t_gi0 = gi;
      cid0 = cid;
      _t_as = as;
      _t_gi = gi;
      _cid = cid;
    }

    void save_context() {
      _t_as = t_as;
      _t_gi = t_gi;
      _offset = offset;
      _cid = cid;
    }

    void restore_context() {
      t_as = _t_as;
      t_gi = _t_gi;
      offset = _offset;
      cid = _cid;
    }

    Addr getOffset() const { return offset; }
    Addr getChipID() const { return cid; }
  };

  class Decoder {
    FixPointTurboDecoder *parent;
    uint32_t id;
    uint32_t cur_pos;
    uint32_t cur_load_len;
    uint32_t cur_beta_len;
    uint32_t cur_alpha_len;
    metric_t t_sys_pri;
    branch_metric t_gamma;
    state_metric  t_beta;
    state_metric  t_alpha;
    branch_metric t_gamma_in_alpha;
    metric_t t_llr;
    metric_t t_le;
    uint8_t t_hard_bit_compress;
    branch_metric *gamma2alpha;
    branch_metric *beta2gamma;
    state_metric *beta_buffer;
    state_metric *beta2alpha;
    sys_t *par2beta;
    sys_t *beta2sys;
    sys_t *beta2sys_pri;
    sys_t *sys2beta;
    sys_t *sys2alpha;
    sys_t *sys_pri2alpha;
    state_metric beta_init_ram[MAX_SW_NUM * 2];

    void beta();

    void alpha();

    uint32_t gamma_addr_gen();

    void gamma();

    uint32_t hard_decision_addr_gen();

    void hard_decision();

    inline bool found_cross(state_metric array);

    inline metric_t find_max(state_metric array);

    void compute_llr(state_metric b, sys_t systematic);

    uint32_t extrinsic_addr_gen();

    uint32_t pre_interleave_addr_gen();

    void compute_extrinsic();

    void llr();

  public:
    Decoder(FixPointTurboDecoder* td, uint32_t idx) :
      parent(td), id(idx), cur_pos(0), cur_load_len(0), cur_beta_len(0),
      cur_alpha_len(0), t_sys_pri(0), t_llr(0), t_le(0),
      t_hard_bit_compress(0), gamma2alpha(NULL), beta2gamma(NULL),
      beta_buffer(NULL), beta2alpha(NULL), par2beta(NULL),
      beta2sys(NULL), beta2sys_pri(NULL),
      sys2beta(NULL), sys2alpha(NULL), sys_pri2alpha(NULL),
      tickEvent(this) {}
    ~Decoder() {}

    const std::string name() const {
      return csprintf("%s.dec%d", parent->params()->name, id);
    }

    void setbuf(uint8_t idx);

    void setload(uint32_t len) {
      cur_load_len = len;
    }

    void reset_alpha0();

    void reset_alpha1();

    void reset_beta0();

    void reset_beta1();

    void ld_beta_init0();

    void ld_beta_init1();

    void st_beta_init0();

    void st_beta_init1();

    void reset();

    void tick();

    EventWrapper<Decoder, &Decoder::tick> tickEvent;

  };

  BitUnion16(STATUSREG)
    Bitfield<0> loadsw0;
    Bitfield<1> loadsw1;
    Bitfield<2> loadpar0;
    Bitfield<3> loadpar1;
    Bitfield<4> crc;
    Bitfield<5> abort;
    Bitfield<6,9> decode;
  EndBitUnion(STATUSREG)

  typedef struct {
    uint32_t current_state;
    uint32_t sw_len[NUM_PIPELINE_SW];
    uint32_t cur_sys_base;
    uint32_t cur_par1_base;
    uint32_t cur_par2_base;
    sys_t input_buffer_sys[NUM_PIPELINE_SW][MAX_SW_LEN];
    sys_t input_buffer_par[NUM_PIPELINE_SW][MAX_SW_LEN];
    sys_t sys_buffer_2nd[NUM_PIPELINE_SW][MAX_SW_LEN];
    sys_t sys_pri_buffer[NUM_PIPELINE_SW][MAX_SW_LEN];
    branch_metric gamma_buffer_sw[NUM_PIPELINE_SW][MAX_SW_LEN];
    state_metric beta_buffer_sw[NUM_PIPELINE_SW][MAX_SW_LEN];
    state_metric alpha_initial_value_sb0;
    state_metric alpha_initial_value_sb1;
    state_metric t_beta_init_buf;
    Decoder *decoder;
    Interleaver *interleaver[2];
  } sub_dec_resources;

  sys_t extrinsic[NUM_SUB_DEC][MAX_CB_LEN / NUM_SUB_DEC];

  uint8_t hard_bit[NUM_SUB_DEC][MAX_CB_LEN / NUM_SUB_DEC];

  sub_dec_resources dec_res[NUM_SUB_DEC];

  turbo_conf conf_regs;

  uint32_t status;

  uint32_t current_state;

  uint32_t current_iter;

  uint32_t cur_sw;

  uint32_t cur_alpha_sw;

  uint32_t cur_beta_sw;

  STATUSREG pending_status;

  uint32_t cur_output_base;

  uint32_t t_crc;

  /** LocalMem interface. */
  MasterPort *localmemPort;

  void decode();

  void load_sw0();

  void load_par0();

  void load_sw1();

  void load_par1();

  void pre_interleave(uint8_t id, sys_t value);

  sys_t post_interleave(uint8_t id);

  void reset_crc();

  void crc();

  void abort();

  EventWrapper<FixPointTurboDecoder, &FixPointTurboDecoder::decode> decodeEvent;

  EventWrapper<FixPointTurboDecoder, &FixPointTurboDecoder::load_sw0> load_sw0Event;

  EventWrapper<FixPointTurboDecoder, &FixPointTurboDecoder::load_sw1> load_sw1Event;

  EventWrapper<FixPointTurboDecoder, &FixPointTurboDecoder::load_par0> load_par0Event;

  EventWrapper<FixPointTurboDecoder, &FixPointTurboDecoder::load_par1> load_par1Event;

  EventWrapper<FixPointTurboDecoder, &FixPointTurboDecoder::crc> crcEvent;

  EventWrapper<FixPointTurboDecoder, &FixPointTurboDecoder::abort> abortEvent;

  /** Pure virtual function that the device must implement. Called
   * when a read command is recieved by the port.
   * @param pkt Packet describing this request
   * @return number of ticks it took to complete
   */
  virtual Tick read(PacketPtr pkt);

  /** Pure virtual function that the device must implement. Called when a
   * write command is recieved by the port.
   * @param pkt Packet describing this request
   * @return number of ticks it took to complete
   */
  virtual Tick write(PacketPtr pkt);

public:
  typedef FixPointTurboDecoderParams Params;
  FixPointTurboDecoder(const Params *p);
  ~FixPointTurboDecoder();
  virtual void init();
  // register statistics for this object
  virtual void regStats();

  Stats::Scalar bitsDecoded;
  Stats::Formula decBandwidth;
};

#endif /* TURBODEC_HH_ */
