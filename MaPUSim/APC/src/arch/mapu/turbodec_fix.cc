/*
 * turbodec_fix.cc
 *
 *  Created on: Jun 27, 2014
 *      Author: wujn
 */

#include "debug/TurboDec.hh"
#include "mem/packet.hh"
#include "mem/packet_access.hh"

#include "cpu.hh"
#include "utility.hh"
#include "turbodec_fix.hh"
#include "vtophys.hh"

using namespace MapuISA;

const FixPointTurboDecoder::state_metric
FixPointTurboDecoder::alpha_initial_value = {      0, MIN_EXT, MIN_EXT, MIN_EXT,
                                             MIN_EXT, MIN_EXT, MIN_EXT, MIN_EXT };

const FixPointTurboDecoder::state_metric
FixPointTurboDecoder::beta_initial_value = {      0, MIN_EXT, MIN_EXT, MIN_EXT,
                                            MIN_EXT, MIN_EXT, MIN_EXT, MIN_EXT };
const FixPointTurboDecoder::state_metric
FixPointTurboDecoder::beta_initial_value_ep = {0, 0, 0, 0,
                                               0, 0, 0, 0};

uint32_t FixPointTurboDecoder::Decoder::gamma_addr_gen() {
  uint32_t sw_len = cur_beta_len;
  return sw_len - 1 - cur_pos;
}

void FixPointTurboDecoder::Decoder::gamma() {
  uint32_t pos = gamma_addr_gen();
  DPRINTF(TurboDecGamma, "[iter: %d, id: %d, sw: %d, pos: %d] sys: %d(%#08x) @%#02x\n",
          parent->current_iter, id, parent->cur_beta_sw - 1, cur_pos, sys2beta[pos],
          *(metric_t *)&sys2beta[pos], pos);
  DPRINTF(TurboDecGamma, "[iter: %d, id: %d, sw: %d, pos: %d] sys+pri: %d(%#08x) @%#02x\n",
          parent->current_iter, id, parent->cur_beta_sw - 1, cur_pos, t_sys_pri,
          *(metric_t *)&t_sys_pri, pos);
  DPRINTF(TurboDecGamma, "[iter: %d, id: %d, sw: %d, pos: %d] par1: %d(%#08x) @%#02x\n",
          parent->current_iter, id, parent->cur_beta_sw - 1, cur_pos, par2beta[pos],
          *(metric_t *)&par2beta[pos], pos);
  t_gamma[1] = (-t_sys_pri - (metric_t)par2beta[pos]) >> 1;//gamma11
  t_gamma[0] = (-t_sys_pri + (metric_t)par2beta[pos]) >> 1;//gamma10
  if (pos < cur_beta_len) {
    memcpy(beta2gamma[pos], t_gamma, sizeof(branch_metric));
    beta2sys_pri[pos] = t_sys_pri;
    beta2sys[pos] = sys2beta[pos];
  }
#if TRACING_ON
  DPRINTF(TurboDecGamma, "[iter: %d, id: %d, sw: %d, pos: %d] gamma(): gamma10:"
          " %d(%#08x), gamma11: %d(%#08x) @%#02x\n", parent->current_iter, id,
          parent->cur_beta_sw - 1, cur_pos, t_gamma[0], *(metric_t *)&t_gamma[0],
          t_gamma[1], *(metric_t *)&t_gamma[1], pos);
  DPRINTF(TurboDecGamma, "[iter: %d, id: %d, sw: %d, pos: %d] beta():",
          parent->current_iter, id, parent->cur_beta_sw - 1, cur_pos);

  for (unsigned i = 0; i < 8; i++)
    DPRINTFR(TurboDecGamma, " %d(%#08x)", t_beta[i], *(metric_t *)&t_beta[i]);

  DPRINTFR(TurboDecGamma, " @%#02x\n", pos);
#endif
  if (pos < cur_beta_len)
    memcpy(beta_buffer[pos], t_beta, sizeof(state_metric));
}

void FixPointTurboDecoder::Decoder::beta() {
  state_metric beta0, beta1;

  beta0[0] = COMPL_SUB(t_beta[0], t_gamma[1]);
  beta1[0] = COMPL_ADD(t_beta[4], t_gamma[1]);
  beta0[1] = COMPL_SUB(t_beta[4], t_gamma[1]);
  beta1[1] = COMPL_ADD(t_beta[0], t_gamma[1]);
  beta0[2] = COMPL_SUB(t_beta[5], t_gamma[0]);
  beta1[2] = COMPL_ADD(t_beta[1], t_gamma[0]);
  beta0[3] = COMPL_SUB(t_beta[1], t_gamma[0]);
  beta1[3] = COMPL_ADD(t_beta[5], t_gamma[0]);
  beta0[4] = COMPL_SUB(t_beta[2], t_gamma[0]);
  beta1[4] = COMPL_ADD(t_beta[6], t_gamma[0]);
  beta0[5] = COMPL_SUB(t_beta[6], t_gamma[0]);
  beta1[5] = COMPL_ADD(t_beta[2], t_gamma[0]);
  beta0[6] = COMPL_SUB(t_beta[7], t_gamma[1]);
  beta1[6] = COMPL_ADD(t_beta[3], t_gamma[1]);
  beta0[7] = COMPL_SUB(t_beta[3], t_gamma[1]);
  beta1[7] = COMPL_ADD(t_beta[7], t_gamma[1]);

  for (unsigned i = 0; i < sizeof(state_metric)/sizeof(metric_t); i++)
    t_beta[i] = CMP_MOD(beta0[i], beta1[i]) ? beta0[i] : beta1[i];
}

void FixPointTurboDecoder::Decoder::alpha() {
  state_metric alpha0, alpha1;
#if TRACING_ON
  DPRINTF(TurboDecAlpha, "[iter: %d, id: %d, sw: %d, pos: %d] gamma(): gamma10:"
            " %d(%#08x), gamma11: %d(%#08x) @%#02x\n", parent->current_iter, id,
            parent->cur_beta_sw - 1, cur_pos, gamma2alpha[cur_pos][0], *(metric_t *)&gamma2alpha[cur_pos][0],
            gamma2alpha[cur_pos][1], *(metric_t *)&gamma2alpha[cur_pos][1], cur_pos);
  DPRINTF(TurboDecAlpha, "[iter: %d, id: %d, sw: %d, pos: %d] alpha():",
          parent->current_iter, id, parent->cur_alpha_sw - 1, cur_pos);

  for (unsigned i = 0; i < 8; i++)
    DPRINTFR(TurboDecAlpha, " %d(%#08x)", t_alpha[i], *(metric_t *)&t_alpha[i]);

  DPRINTFR(TurboDecAlpha, " @%#02x\n", cur_pos);
#endif
  alpha0[0] = COMPL_SUB(t_alpha[0], gamma2alpha[cur_pos][1]);
  alpha1[0] = COMPL_ADD(t_alpha[1], gamma2alpha[cur_pos][1]);
  alpha0[1] = COMPL_SUB(t_alpha[3], gamma2alpha[cur_pos][0]);
  alpha1[1] = COMPL_ADD(t_alpha[2], gamma2alpha[cur_pos][0]);
  alpha0[2] = COMPL_SUB(t_alpha[4], gamma2alpha[cur_pos][0]);
  alpha1[2] = COMPL_ADD(t_alpha[5], gamma2alpha[cur_pos][0]);
  alpha0[3] = COMPL_SUB(t_alpha[7], gamma2alpha[cur_pos][1]);
  alpha1[3] = COMPL_ADD(t_alpha[6], gamma2alpha[cur_pos][1]);
  alpha0[4] = COMPL_SUB(t_alpha[1], gamma2alpha[cur_pos][1]);
  alpha1[4] = COMPL_ADD(t_alpha[0], gamma2alpha[cur_pos][1]);
  alpha0[5] = COMPL_SUB(t_alpha[2], gamma2alpha[cur_pos][0]);
  alpha1[5] = COMPL_ADD(t_alpha[3], gamma2alpha[cur_pos][0]);
  alpha0[6] = COMPL_SUB(t_alpha[5], gamma2alpha[cur_pos][0]);
  alpha1[6] = COMPL_ADD(t_alpha[4], gamma2alpha[cur_pos][0]);
  alpha0[7] = COMPL_SUB(t_alpha[6], gamma2alpha[cur_pos][1]);
  alpha1[7] = COMPL_ADD(t_alpha[7], gamma2alpha[cur_pos][1]);

#if TRACING_ON
  DPRINTF(TurboDecAlpha, "[iter: %d, id: %d, sw: %d, pos: %d] alpha():",
          parent->current_iter, id, parent->cur_alpha_sw - 1, cur_pos);

  for (unsigned i = 0; i < 8; i++)
    DPRINTFR(TurboDecAlpha, " %d(%#08x), %d(%#08x)", alpha0[i], *(metric_t *)&alpha0[i],alpha1[i], *(metric_t *)&alpha1[i]);
  DPRINTFR(TurboDecAlpha, " @%#02x\n", cur_pos);
#endif
  for (unsigned i = 0; i < sizeof(state_metric)/sizeof(metric_t); i++)
    t_alpha[i] = CMP_MOD(alpha0[i], alpha1[i]) ? alpha0[i] : alpha1[i];

#if TRACING_ON
  DPRINTF(TurboDecAlpha, "[iter: %d, id: %d, sw: %d, pos: %d] gamma(): gamma10:"
            " %d(%#08x), gamma11: %d(%#08x) @%#02x\n", parent->current_iter, id,
            parent->cur_beta_sw - 1, cur_pos, gamma2alpha[cur_pos][0], *(metric_t *)&gamma2alpha[cur_pos][0],
            gamma2alpha[cur_pos][1], *(metric_t *)&gamma2alpha[cur_pos][1], cur_pos);
  DPRINTF(TurboDecAlpha, "[iter: %d, id: %d, sw: %d, pos: %d] alpha():",
          parent->current_iter, id, parent->cur_alpha_sw - 1, cur_pos);

  for (unsigned i = 0; i < 8; i++)
    DPRINTFR(TurboDecAlpha, " %d(%#08x)", t_alpha[i], *(metric_t *)&t_alpha[i]);

  DPRINTFR(TurboDecAlpha, " @%#02x\n", cur_pos);
#endif
}

inline bool FixPointTurboDecoder::Decoder::found_cross(state_metric array) {
  bool hasPos = false;
  bool hasBigNeg = false;
  for (unsigned i = 0; i < sizeof(state_metric)/sizeof(metric_t); i++)
    if (!(array[i] & (1 << (EXTWIDTH - 1)))) hasPos = true;
    else if (!(array[i] & (1 << (EXTWIDTH - 2)))) hasBigNeg = true;
  return hasPos && hasBigNeg;
}

inline FixPointTurboDecoder::metric_t
FixPointTurboDecoder::Decoder::find_max(state_metric array) {
  metric_t temp;
  temp = array[0];
  for (unsigned i = 1; i < sizeof(state_metric)/sizeof(metric_t); i++)
    temp = (array[i] >= temp) ? array[i] : temp;
  return temp;
}

void FixPointTurboDecoder::Decoder::compute_llr(state_metric b, sys_t systematic) {
  state_metric llr_branch0, llr_branch1;
  metric_t llr0 = 0, llr1 = 0;
  memcpy(t_gamma_in_alpha, gamma2alpha[cur_pos], sizeof(branch_metric));
  state_metric alpha_ex, b_ex;

  if (!found_cross(t_alpha)) {
    for (unsigned i = 0; i < sizeof(state_metric)/sizeof(metric_t); i++)
      alpha_ex[i] = SEXT(t_alpha[i]);
  } else {
    for (unsigned i = 0; i < sizeof(state_metric)/sizeof(metric_t); i++)
      alpha_ex[i] = t_alpha[i];
  }

  if (!found_cross(b)) {
    for (unsigned i = 0; i < sizeof(state_metric)/sizeof(metric_t); i++)
      b_ex[i] = SEXT(b[i]);
  } else {
    for (unsigned i = 0; i < sizeof(state_metric)/sizeof(metric_t); i++)
      b_ex[i] = b[i];
  }

  llr_branch0[0] = alpha_ex[0] + b_ex[0] - t_gamma_in_alpha[1];
  llr_branch0[1] = alpha_ex[3] + b_ex[1] - t_gamma_in_alpha[0];
  llr_branch0[2] = alpha_ex[4] + b_ex[2] - t_gamma_in_alpha[0];
  llr_branch0[3] = alpha_ex[7] + b_ex[3] - t_gamma_in_alpha[1];
  llr_branch0[4] = alpha_ex[1] + b_ex[4] - t_gamma_in_alpha[1];
  llr_branch0[5] = alpha_ex[2] + b_ex[5] - t_gamma_in_alpha[0];
  llr_branch0[6] = alpha_ex[5] + b_ex[6] - t_gamma_in_alpha[0];
  llr_branch0[7] = alpha_ex[6] + b_ex[7] - t_gamma_in_alpha[1];

  llr0 = find_max(llr_branch0);

#if TRACING_ON
  DPRINTF(TurboDecLLR, "[iter: %d, id: %d, sw: %d, pos: %d] llr(): alpha:",
          parent->current_iter, id, parent->cur_alpha_sw - 1, cur_pos);
  for (unsigned i = 0; i < 8; i++)
    DPRINTFR(TurboDecLLR, " %d(%#08x)", t_alpha[i], *(metric_t *)&t_alpha[i]);

  DPRINTFR(TurboDecLLR, " @%#02x\n", cur_pos);
  DPRINTF(TurboDecLLR, "[iter: %d, id: %d, sw: %d, pos: %d] llr(): beta:",
          parent->current_iter, id, parent->cur_alpha_sw - 1, cur_pos);

  for (unsigned i = 0; i < 8; i++)
    DPRINTFR(TurboDecLLR, " %d(%#08x)", b[i], *(metric_t *)&b[i]);

  DPRINTFR(TurboDecLLR, " @%#02x\n", cur_pos);
  DPRINTF(TurboDecLLR, "[iter: %d, id: %d, sw: %d, pos: %d] llr(): gamma:",
          parent->current_iter, id, parent->cur_alpha_sw - 1, cur_pos);

  for (unsigned i = 0; i < 2; i++)
    DPRINTFR(TurboDecLLR, " %d(%#08x)", t_gamma_in_alpha[i], *(metric_t *)&t_gamma_in_alpha[i]);

  DPRINTFR(TurboDecLLR, " @%#02x\n", cur_pos);
#endif

  llr_branch1[0] = alpha_ex[1] + b_ex[0] + t_gamma_in_alpha[1];
  llr_branch1[1] = alpha_ex[2] + b_ex[1] + t_gamma_in_alpha[0];
  llr_branch1[2] = alpha_ex[5] + b_ex[2] + t_gamma_in_alpha[0];
  llr_branch1[3] = alpha_ex[6] + b_ex[3] + t_gamma_in_alpha[1];
  llr_branch1[4] = alpha_ex[0] + b_ex[4] + t_gamma_in_alpha[1];
  llr_branch1[5] = alpha_ex[3] + b_ex[5] + t_gamma_in_alpha[0];
  llr_branch1[6] = alpha_ex[4] + b_ex[6] + t_gamma_in_alpha[0];
  llr_branch1[7] = alpha_ex[7] + b_ex[7] + t_gamma_in_alpha[1];

  llr1 = find_max(llr_branch1);

  for (unsigned i = 0; i < 8; i++)
      DPRINTFR(TurboDecLLR, " %d(%#08x)", llr_branch0[i], *(metric_t *)&llr_branch0[i]);
  DPRINTFR(TurboDecLLR, "\n");
  for (unsigned i = 0; i < 8; i++)
      DPRINTFR(TurboDecLLR, " %d(%#08x)", llr_branch1[i], *(metric_t *)&llr_branch1[i]);
  DPRINTFR(TurboDecLLR, "\n");
  DPRINTFR(TurboDecLLR, " %d(%#08x) ,%d(%#08x)\n", llr0, *(metric_t *)&llr0, llr1, *(metric_t *)&llr1);
  t_llr = llr0 - llr1;
  t_le = t_llr - systematic;
  //t_le = SAT(t_le, 6);
  t_le = (t_le * parent->conf_regs.p_scaling_factor) >> 5;
  //t_le = (t_le >> 1) + (t_le >> 2);
 // t_le = SAT(t_le, 8);
  DPRINTF(TurboDecLLR, "[iter: %d, id: %d, sw: %d, pos: %d] llr(): llr: %d(%#08x) "
          "le: %d(%#08x) sys: %d(%#08x) @%#02x\n",
          parent->current_iter, id, parent->cur_alpha_sw - 1, cur_pos, t_llr,
          *(metric_t *)&t_llr, t_le, *(metric_t *)&t_le, systematic, *(metric_t *)&systematic, cur_pos);
}

void FixPointTurboDecoder::Decoder::llr() {
  compute_llr(beta2alpha[cur_pos], sys_pri2alpha[cur_pos]);
}

uint32_t FixPointTurboDecoder::Decoder::extrinsic_addr_gen() {
  return gamma_addr_gen() + (parent->cur_beta_sw - 1) * parent->conf_regs.p_cfg03.sw_len_1;
}

void FixPointTurboDecoder::Decoder::compute_extrinsic() {
  uint32_t pos_e = extrinsic_addr_gen();
  uint32_t pos_s = gamma_addr_gen();
  t_sys_pri = (metric_t)sys2beta[pos_s] + (metric_t)parent->extrinsic[id][pos_e];
  DPRINTF(TurboDecExt, "[iter: %d, id: %d, sw: %d, pos: %d] extrinsic():"
          " %d(%#08x) sys@%#02x, ext@%#04x\n",
          parent->current_iter, id, parent->cur_beta_sw - 1, cur_pos, t_sys_pri,
          *(metric_t *)&t_sys_pri, pos_s, pos_e);
}

uint32_t FixPointTurboDecoder::Decoder::hard_decision_addr_gen() {
  return cur_pos + (parent->cur_alpha_sw - 1) * parent->conf_regs.p_cfg03.sw_len_1;
}

void FixPointTurboDecoder::Decoder::hard_decision() {
  uint32_t pos = hard_decision_addr_gen();
  bool bit = (t_llr > 0) ? 0 : 1;
  t_hard_bit_compress = (t_hard_bit_compress & ~(1 << (7 - pos % 8))) | (bit << (7 - pos % 8));
  if (pos % 8 == 7) parent->hard_bit[id][pos / 8] = t_hard_bit_compress;
  DPRINTF(TurboDecHard, "[iter: %d, id %d, pos: %d] hard_decision(): llr:"
          " %d(%#08x) hard bit: %x @%#02x\n",
          parent->current_iter, id, cur_pos, t_llr,
          *(uint32_t *)&t_llr, bit, pos);
}

void FixPointTurboDecoder::Decoder::setbuf(uint8_t idx) {
  DPRINTF(TurboDecBuf, "[iter: %d, id %d, sw: %d] decoder(): "
          "Switch buffer for pipelined sliding window %d\n",
          parent->current_iter, id, parent->cur_sw - 1, idx);
  assert(idx < 2);
  beta2gamma = parent->dec_res[id].gamma_buffer_sw[idx];
  gamma2alpha = parent->dec_res[id].gamma_buffer_sw[!idx];
  beta_buffer = parent->dec_res[id].beta_buffer_sw[idx];
  beta2alpha = parent->dec_res[id].beta_buffer_sw[!idx];
  sys2beta = parent->dec_res[id].input_buffer_sys[idx];
  beta2sys = parent->dec_res[id].sys_buffer_2nd[idx];
  sys2alpha = parent->dec_res[id].sys_buffer_2nd[!idx];
  beta2sys_pri = parent->dec_res[id].sys_pri_buffer[idx];
  sys_pri2alpha = parent->dec_res[id].sys_pri_buffer[!idx];
  par2beta = parent->dec_res[id].input_buffer_par[idx];
  cur_beta_len = parent->dec_res[id].sw_len[idx];
  cur_alpha_len = parent->dec_res[id].sw_len[!idx];
}

void FixPointTurboDecoder::Decoder::reset() {
  parent->dec_res[id].cur_sys_base = parent->conf_regs.p_sys_base +
        parent->conf_regs.p_cfg02.sb_len * sizeof(sys_t) * id;
  parent->dec_res[id].cur_par1_base = parent->conf_regs.p_par1_base +
        parent->conf_regs.p_cfg02.sb_len * sizeof(sys_t) * id;
  parent->dec_res[id].cur_par2_base = parent->conf_regs.p_par2_base +
        parent->conf_regs.p_cfg02.sb_len * sizeof(sys_t) * id;
  parent->dec_res[id].sw_len[0] = 0;
  parent->dec_res[id].sw_len[1] = 0;
  cur_load_len = 0;
  cur_pos = 0;
  DPRINTF(TurboDecReset, "[iter: %d, id %d] reset(): sys base: %#08x, "
          "par1 base: %#08x par2 base: %#08x\n",
          parent->current_iter, id, parent->dec_res[id].cur_sys_base,
          parent->dec_res[id].cur_par1_base,
          parent->dec_res[id].cur_par2_base);
}

void FixPointTurboDecoder::Decoder::reset_beta0() {
  if ((parent->cur_beta_sw == parent->conf_regs.p_cfg03.sw_num) &&
      (id == (parent->conf_regs.p_cfg02.sb_num - 1))){
    //memcpy(t_beta, parent->conf_regs.p_beta_initial_value_cb1, sizeof(t_beta));
    t_beta[0] = parent->conf_regs.p_cfg05.beta_init_l & 0x7FFF;
    t_beta[1] = parent->conf_regs.p_cfg05.beta_init_h & 0x7FFF;
    t_beta[2] = parent->conf_regs.p_cfg06.beta_init_l & 0x7FFF;
    t_beta[3] = parent->conf_regs.p_cfg06.beta_init_h & 0x7FFF;
    t_beta[4] = parent->conf_regs.p_cfg07.beta_init_l & 0x7FFF;
    t_beta[5] = parent->conf_regs.p_cfg07.beta_init_h & 0x7FFF;
    t_beta[6] = parent->conf_regs.p_cfg08.beta_init_l & 0x7FFF;
    t_beta[7] = parent->conf_regs.p_cfg08.beta_init_h & 0x7FFF;
  }
  else
    memcpy(t_beta, beta_initial_value_ep, sizeof(t_beta));

  if (parent->cur_beta_sw == parent->conf_regs.p_cfg03.sw_num &&
      id != parent->conf_regs.p_cfg02.sb_num - 1)
    memcpy(beta_init_ram[parent->cur_beta_sw - 1],
           parent->dec_res[id + 1].t_beta_init_buf, sizeof(t_beta));

  DPRINTF(TurboDecBeta, "[iter: %d, id: %d, sw: %d, pos: %d] reset_beta0():",
          parent->current_iter, id, parent->cur_beta_sw - 1, cur_pos);
#if TRACING_ON
  for (unsigned i = 0; i < 8; i++)
    DPRINTFR(TurboDecBeta, " %d(%#08x)", t_beta[i], *(uint32_t *)&t_beta[i]);

  DPRINTFR(TurboDecBeta, "\n");
#endif
}

void FixPointTurboDecoder::Decoder::reset_beta1() {
	if ((parent->cur_beta_sw == parent->conf_regs.p_cfg03.sw_num) &&
      (id == (parent->conf_regs.p_cfg02.sb_num - 1))){
    //memcpy(t_beta, parent->conf_regs.p_beta_initial_value_cb2, sizeof(t_beta));
    t_beta[0] = parent->conf_regs.p_cfg09.beta_init_l & 0x7FFF;
    t_beta[1] = parent->conf_regs.p_cfg09.beta_init_h & 0x7FFF;
    t_beta[2] = parent->conf_regs.p_cfg10.beta_init_l & 0x7FFF;
    t_beta[3] = parent->conf_regs.p_cfg10.beta_init_h & 0x7FFF;
    t_beta[4] = parent->conf_regs.p_cfg11.beta_init_l & 0x7FFF;
    t_beta[5] = parent->conf_regs.p_cfg11.beta_init_h & 0x7FFF;
    t_beta[6] = parent->conf_regs.p_cfg12.beta_init_l & 0x7FFF;
    t_beta[7] = parent->conf_regs.p_cfg12.beta_init_h & 0x7FFF;
  }
  else
    memcpy(t_beta, beta_initial_value_ep, sizeof(t_beta));

  if (parent->cur_beta_sw == parent->conf_regs.p_cfg03.sw_num &&
      id != parent->conf_regs.p_cfg02.sb_num - 1)
    memcpy(beta_init_ram[parent->cur_beta_sw - 1 + MAX_SW_NUM],
           parent->dec_res[id + 1].t_beta_init_buf, sizeof(t_beta));

  DPRINTF(TurboDecBeta, "[iter: %d, id: %d, sw: %d] reset_beta1():",
          parent->current_iter, id, parent->cur_beta_sw - 1);
#if TRACING_ON
  for (unsigned i = 0; i < 8; i++)
    DPRINTFR(TurboDecBeta, " %d(%#08x)", t_beta[i], *(uint32_t *)&t_beta[i]);

  DPRINTFR(TurboDecBeta, "\n");
#endif
}

void FixPointTurboDecoder::Decoder::ld_beta_init0() {
  if ((parent->cur_beta_sw == parent->conf_regs.p_cfg03.sw_num) &&
      (id == (parent->conf_regs.p_cfg02.sb_num - 1))){
    //memcpy(t_beta, parent->conf_regs.p_beta_initial_value_cb1, sizeof(t_beta));
    t_beta[0] = parent->conf_regs.p_cfg05.beta_init_l & 0x7FFF;
    t_beta[1] = parent->conf_regs.p_cfg05.beta_init_h & 0x7FFF;
    t_beta[2] = parent->conf_regs.p_cfg06.beta_init_l & 0x7FFF;
    t_beta[3] = parent->conf_regs.p_cfg06.beta_init_h & 0x7FFF;
    t_beta[4] = parent->conf_regs.p_cfg07.beta_init_l & 0x7FFF;
    t_beta[5] = parent->conf_regs.p_cfg07.beta_init_h & 0x7FFF;
    t_beta[6] = parent->conf_regs.p_cfg08.beta_init_l & 0x7FFF;
    t_beta[7] = parent->conf_regs.p_cfg08.beta_init_h & 0x7FFF;
  }
  else
    memcpy(t_beta, beta_init_ram[parent->cur_beta_sw - 1], sizeof(t_beta));

  if (parent->cur_beta_sw == parent->conf_regs.p_cfg03.sw_num &&
      id != parent->conf_regs.p_cfg02.sb_num - 1)
    memcpy(beta_init_ram[parent->cur_beta_sw - 1],
           parent->dec_res[id + 1].t_beta_init_buf, sizeof(t_beta));

  DPRINTF(TurboDecBeta, "[iter: %d, id: %d, sw: %d, pos: %d] ld_beta_init0():",
          parent->current_iter, id, parent->cur_beta_sw - 1, cur_pos);
#if TRACING_ON
  for (unsigned i = 0; i < 8; i++)
    DPRINTFR(TurboDecBeta, " %d(%#08x)", t_beta[i], *(uint32_t *)&t_beta[i]);

  DPRINTFR(TurboDecBeta, "@(%#08x)\n", parent->cur_beta_sw - 1);
#endif
}

void FixPointTurboDecoder::Decoder::ld_beta_init1() {
  if ((parent->cur_beta_sw == parent->conf_regs.p_cfg03.sw_num) &&
      (id == (parent->conf_regs.p_cfg02.sb_num - 1))){
    //memcpy(t_beta, parent->conf_regs.p_beta_initial_value_cb2, sizeof(t_beta));
    t_beta[0] = parent->conf_regs.p_cfg09.beta_init_l & 0x7FFF;
    t_beta[1] = parent->conf_regs.p_cfg09.beta_init_h & 0x7FFF;
    t_beta[2] = parent->conf_regs.p_cfg10.beta_init_l & 0x7FFF;
    t_beta[3] = parent->conf_regs.p_cfg10.beta_init_h & 0x7FFF;
    t_beta[4] = parent->conf_regs.p_cfg11.beta_init_l & 0x7FFF;
    t_beta[5] = parent->conf_regs.p_cfg11.beta_init_h & 0x7FFF;
    t_beta[6] = parent->conf_regs.p_cfg12.beta_init_l & 0x7FFF;
    t_beta[7] = parent->conf_regs.p_cfg12.beta_init_h & 0x7FFF;
  }
  else
    memcpy(t_beta, beta_init_ram[parent->cur_beta_sw - 1 + MAX_SW_NUM], sizeof(t_beta));

  if (parent->cur_beta_sw == parent->conf_regs.p_cfg03.sw_num &&
      id != parent->conf_regs.p_cfg02.sb_num - 1)
    memcpy(beta_init_ram[parent->cur_beta_sw - 1 + MAX_SW_NUM],
           parent->dec_res[id + 1].t_beta_init_buf, sizeof(t_beta));

  DPRINTF(TurboDecBeta, "[iter: %d, id: %d, sw: %d] ld_beta_init1():",
          parent->current_iter, id, parent->cur_beta_sw - 1);
#if TRACING_ON
  for (unsigned i = 0; i < 8; i++)
    DPRINTFR(TurboDecBeta, " %d(%#08x)", t_beta[i], *(uint32_t *)&t_beta[i]);

  DPRINTFR(TurboDecBeta, "@(%#08x)\n", parent->cur_beta_sw - 1 + MAX_SW_NUM);
#endif
}

void FixPointTurboDecoder::Decoder::st_beta_init0() {
  if (parent->cur_beta_sw == 1)
    memcpy(parent->dec_res[id].t_beta_init_buf, t_beta, sizeof(t_beta));
  else
    memcpy(beta_init_ram[parent->cur_beta_sw - 2], t_beta, sizeof(t_beta));

  DPRINTF(TurboDecBeta, "[iter: %d, id: %d, sw: %d, pos: %d] st_beta_init0():",
          parent->current_iter, id, parent->cur_beta_sw - 1, cur_pos);
#if TRACING_ON
  for (unsigned i = 0; i < 8; i++)
    DPRINTFR(TurboDecBeta, " %d(%#08x)", t_beta[i], *(uint32_t *)&t_beta[i]);

  DPRINTFR(TurboDecBeta, "@(%#08x)\n", parent->cur_beta_sw - 2);
#endif
}

void FixPointTurboDecoder::Decoder::st_beta_init1() {
  if (parent->cur_beta_sw == 1)
    memcpy(parent->dec_res[id].t_beta_init_buf, t_beta, sizeof(t_beta));
  else
    memcpy(beta_init_ram[parent->cur_beta_sw - 2 + MAX_SW_NUM], t_beta, sizeof(t_beta));

  DPRINTF(TurboDecBeta, "[iter: %d, id: %d, sw: %d] st_beta_init1():",
          parent->current_iter, id, parent->cur_beta_sw - 1);
#if TRACING_ON
  for (unsigned i = 0; i < 8; i++)
    DPRINTFR(TurboDecBeta, " %d(%#08x)", t_beta[i], *(uint32_t *)&t_beta[i]);

  DPRINTFR(TurboDecBeta, "@(%#08x)\n", parent->cur_beta_sw - 2 + MAX_SW_NUM);
#endif
}

void FixPointTurboDecoder::Decoder::reset_alpha0() {
  if ((parent->cur_alpha_sw - 1 == 0) && (id == 0))
    memcpy(t_alpha, alpha_initial_value, sizeof(t_alpha));
  else if (parent->cur_alpha_sw - 1 == 0)
    memcpy(t_alpha, parent->dec_res[id - 1].alpha_initial_value_sb0, sizeof(t_alpha));

#if TRACING_ON
  DPRINTF(TurboDecAlpha, "[iter: %d, id: %d, sw: %d] reset_alpha0():",
          parent->current_iter, id, parent->cur_alpha_sw - 1);

  for (unsigned i = 0; i < 8; i++)
    DPRINTFR(TurboDecAlpha, " %d(%#08x)", t_alpha[i], *(uint32_t *)&t_alpha[i]);

  DPRINTFR(TurboDecAlpha, "\n");
#endif
}

void FixPointTurboDecoder::Decoder::reset_alpha1() {
  if ((parent->cur_alpha_sw - 1 == 0) && (id == 0))
    memcpy(t_alpha, alpha_initial_value, sizeof(t_alpha));
  else if (parent->cur_alpha_sw - 1 == 0)
    memcpy(t_alpha, parent->dec_res[id - 1].alpha_initial_value_sb1, sizeof(t_alpha));

#if TRACING_ON
  DPRINTF(TurboDecAlpha, "[iter: %d, id: %d, sw: %d] reset_alpha1():",
          parent->current_iter, id, parent->cur_alpha_sw - 1);

  for (unsigned i = 0; i < 8; i++)
    DPRINTFR(TurboDecAlpha, " %d(%#08x)", t_alpha[i], *(uint32_t *)&t_alpha[i]);

  DPRINTFR(TurboDecAlpha, "\n");
#endif
}

uint32_t FixPointTurboDecoder::Decoder::pre_interleave_addr_gen() {
  return cur_pos + (parent->cur_alpha_sw - 1) * parent->conf_regs.p_cfg03.sw_len_1;
}

void FixPointTurboDecoder::Decoder::tick() {
  if (parent->turbodec_status.status == 0) return;
  uint32_t state = parent->dec_res[id].current_state;
  DPRINTF(TurboDecTick, "sub state: %x %d\n", state, cur_pos);
  if ((state & (1 << POST_INTERLEAVE0 | 1 << POST_INTERLEAVE1)) &&  cur_pos < cur_load_len) {
    if (cur_pos == 0)
      parent->dec_res[id].interleaver[1]->restore_context();
    if (state & (1 << POST_INTERLEAVE0)) {
      parent->dec_res[id].input_buffer_sys[0][cur_pos] = parent->post_interleave(id);
      DPRINTF(TurboDecPreInterleave, "[iter: %d, id %d, sw %d, pos: %d] post_interleave0(): %d(%#08x) @%#02x\n",
              parent->current_iter, id, parent->cur_beta_sw, cur_pos, parent->dec_res[id].input_buffer_sys[0][cur_pos],
              *(uint32_t *)&parent->dec_res[id].input_buffer_sys[0][cur_pos], cur_pos);
    } else {
      parent->dec_res[id].input_buffer_sys[1][cur_pos] = parent->post_interleave(id);
      DPRINTF(TurboDecPreInterleave, "[iter: %d, id %d, sw %d, pos: %d] post_interleave1(): %d(%#08x) @%#02x\n",
              parent->current_iter, id, parent->cur_beta_sw, cur_pos, parent->dec_res[id].input_buffer_sys[1][cur_pos],
              *(uint32_t *)&parent->dec_res[id].input_buffer_sys[1][cur_pos], cur_pos);
    }
    if (cur_pos == cur_load_len - 1)
      parent->dec_res[id].interleaver[1]->save_context();
  }
  if ((state & (1 << EXTRINSIC)) && cur_pos < cur_beta_len)
    compute_extrinsic();
  else if ((state & (1 << GAMMA0 | 1 << GAMMA1)) && cur_pos < cur_beta_len)
    t_sys_pri = (metric_t)sys2beta[gamma_addr_gen()];
  if ((state & (1 << GAMMA0 | 1 << GAMMA1)) && cur_pos < cur_beta_len)
    gamma();
  if ((state & (1 << BETA0 | 1 << BETA1)) && cur_pos < cur_beta_len) {
    beta();
    if ((state & (1 << BETA0)) && (cur_pos == cur_beta_len - 1))
      st_beta_init0();
    else if ((state & (1 << BETA1)) && (cur_pos == cur_beta_len - 1))
      st_beta_init1();
  }
  if ((state & (1 << LLR)) && cur_pos < cur_alpha_len)
    llr();
  if ((state & (1 << ALPHA0 | 1 << ALPHA1)) && cur_pos < cur_alpha_len) {
    alpha();
    if (state & (1 << ALPHA0)) {
      memcpy(parent->dec_res[id].alpha_initial_value_sb0, t_alpha, sizeof(t_alpha));
    } else {
      memcpy(parent->dec_res[id].alpha_initial_value_sb1, t_alpha, sizeof(t_alpha));
    }
  }
  if ((state & (1 << PRE_INTERLEAVE)) && cur_pos < cur_alpha_len) {
    DPRINTF(TurboDecPreInterleave, "[iter: %d, id %d, sw %d, pos: %d] pre_interleave(): "
            "%d(%#08x) cid@%#02x,offset@%#04x\n",
            parent->current_iter, id, parent->cur_alpha_sw - 1, cur_pos, t_le,
            *(uint32_t *)&t_le, parent->dec_res[id].interleaver[0]->getChipID(),
            parent->dec_res[id].interleaver[0]->getOffset());
    parent->pre_interleave(id, t_le);
  } else if ((state & (1 << LLR)) && cur_pos < cur_alpha_len) {
    parent->extrinsic[id][pre_interleave_addr_gen()] = t_le + sys2alpha[cur_pos];
    DPRINTF(TurboDecPreInterleave, "[iter: %d, id %d, sw %d, pos: %d] non_pre_interleave(): "
            "%d(%#08x) @%#02x\n",
            parent->current_iter, id, parent->cur_alpha_sw - 1, cur_pos,
            parent->extrinsic[id][pre_interleave_addr_gen()],
            *(uint32_t *)&parent->extrinsic[id][pre_interleave_addr_gen()],
            pre_interleave_addr_gen());
  }
  if ((state & (1 << HARD_DECISION)) && cur_pos < cur_alpha_len)
    hard_decision();
  cur_pos++;
  if (cur_pos < cur_beta_len || cur_pos < cur_alpha_len || cur_pos < cur_load_len)
    parent->schedule(tickEvent, curTick() + parent->copDelay);
  else {
    cur_load_len = 0;
    cur_pos = 0;
    parent->pending_status.decode = parent->pending_status.decode & ~(1 << id);
    if (!parent->pending_status.decode && !parent->decodeEvent.scheduled())
      parent->schedule(parent->decodeEvent, curTick() + parent->copDelay);
  }
}

const uint32_t FixPointTurboDecoder::DEC_STATES[] = {
  0,
  1 << LOAD_SW0,
  1 << LOAD_SW1 | 1 << EXTRINSIC | 1 << BETA0 | 1 << GAMMA0,
  1 << ALPHA0 | 1 << LLR | 1 << HARD_DECISION | 1 << EXTRINSIC | 1 << BETA0 | 1 << GAMMA0  | 1 << LOAD_SW0,
  1 << ALPHA0 | 1 << LLR | 1 << HARD_DECISION | 1 << EXTRINSIC | 1 << BETA0 | 1 << GAMMA0  | 1 << LOAD_SW1,
  1 << LOAD_PAR | 1 << POST_INTERLEAVE0,
  1 << LOAD_PAR | 1 << POST_INTERLEAVE1 | 1 << BETA1 | 1 << GAMMA1,
  1 << ALPHA1 | 1 << LLR | 1 << PRE_INTERLEAVE | 1 << BETA1 | 1 << GAMMA1 | 1 << LOAD_PAR | 1 << POST_INTERLEAVE0,
  1 << ALPHA1 | 1 << LLR | 1 << PRE_INTERLEAVE | 1 << BETA1 | 1 << GAMMA1 | 1 << LOAD_PAR | 1 << POST_INTERLEAVE1,
  1 << ITER_END,
};

FixPointTurboDecoder::FixPointTurboDecoder(const Params *p) :
  CoProcessor(p),turbodec_status(0), current_state(DEC_STATES[0]), current_iter(0),
  cur_sw(0), cur_alpha_sw(0), cur_beta_sw(0),
  cur_output_base(0), t_crc(0), t_crc_flag(0), localmemPort(NULL), decodeEvent(this),
  load_sw0Event(this), load_sw1Event(this), load_par0Event(this),
  load_par1Event(this), crcEvent(this),  abortEvent(this) {}

FixPointTurboDecoder::~FixPointTurboDecoder() {
  for (unsigned i = 0; i < NUM_SUB_DEC; i++) {
    if (dec_res[i].decoder)
      delete dec_res[i].decoder;
    if (dec_res[i].interleaver[0])
      delete dec_res[i].interleaver[0];
    if (dec_res[i].interleaver[1])
      delete dec_res[i].interleaver[1];
  }
}

void FixPointTurboDecoder::init() {
  CoProcessor::init();
  for (unsigned i = 0; i < NUM_SUB_DEC; i++) {
    dec_res[i].decoder = new Decoder(this, i);
    dec_res[i].interleaver[0] = new Interleaver(this, i);
    dec_res[i].interleaver[1] = new Interleaver(this, i);
  }
  localmemPort = &cpu->getTurboDecPort();
  assert(localmemPort != NULL);
}

Tick FixPointTurboDecoder::read(PacketPtr pkt) {
  assert(pkt->getAddr() >= copAddr && pkt->getAddr() < copAddr + copSize);

  Addr daddr = pkt->getAddr() - copAddr;
  pkt->allocate();

  DPRINTF(TurboDecCfg, " read register %#x size=%d\n", daddr, pkt->getSize());

  uint32_t data = 0;
  switch (daddr) {
  case TURBODEC_CMD:
    data = turbodec_status;
    break;
  case TURBODEC_INTR:
    clearEvent(CsuObject::TurboEvent);
    turbodec_status.intr = 0;
    break;
  default:
    panic("Tried to read Turbo Decoder at offset %#x that doesn't exist\n",
          daddr);
    break;
  }

  pkt->set<uint32_t>(data);

  pkt->makeAtomicResponse();
  return copDelay;
}

void FixPointTurboDecoder::decode() {
  uint8_t sw_len_1 = conf_regs.p_cfg03.sw_len_1;
  uint8_t sw_len_2 = conf_regs.p_cfg03.sw_len_2;
  if (turbodec_status.status == 0) return;
  DPRINTF(TurboDecCore, "State: %x\n", current_state);
  for (unsigned i = 0; i < conf_regs.p_cfg02.sb_num; i++)
    if (pending_status != 0)
      return;
  for (unsigned i = 0; i < conf_regs.p_cfg02.sb_num; i++)
    dec_res[i].current_state = current_state;
  if (current_state == DEC_STATES[0]) {
    // INIT State
    current_state = DEC_STATES[1];
    cur_sw = 0;
    cur_alpha_sw = 0;
    cur_beta_sw = 0;
    assert(!decodeEvent.scheduled());
    schedule(decodeEvent, curTick() + copDelay);
  } else if (current_state == DEC_STATES[1]) {
    // First sw loading
    current_state = DEC_STATES[2];
    for (unsigned i = 0; i < conf_regs.p_cfg02.sb_num; i++) {
      dec_res[i].interleaver[0]->reset();
      dec_res[i].interleaver[1]->reset();
      dec_res[i].decoder->reset();
    }
    pending_status.loadsw0 = true;
    assert(!load_sw0Event.scheduled());
    schedule(load_sw0Event, curTick());
  } else if (current_state == DEC_STATES[2]) {
    // Second sw loading and first sw beta
    current_state = DEC_STATES[3];
    cur_beta_sw++;
    for (unsigned i = 0; i < conf_regs.p_cfg02.sb_num; i++) {
      dec_res[i].decoder->setbuf(0);
      if (current_iter == 0)
        dec_res[i].decoder->reset_beta0();
      else
        dec_res[i].decoder->ld_beta_init0();
      pending_status.decode = pending_status.decode | (1 << i);
      assert(!dec_res[i].decoder->tickEvent.scheduled());
      schedule(dec_res[i].decoder->tickEvent, curTick());;
    }
    if (cur_sw < conf_regs.p_cfg03.sw_num) {
      pending_status.loadsw1 = true;
      assert(!load_sw1Event.scheduled());
      schedule(load_sw1Event, curTick());
    }
  } else if (current_state == DEC_STATES[3]) {
    // First sw alpha, second sw beta and next first sw loading
    if (cur_alpha_sw < conf_regs.p_cfg03.sw_num)
      cur_alpha_sw++;
    else
      for (unsigned i = 0; i < conf_regs.p_cfg02.sb_num; i++)
        dec_res[i].current_state &= ~(COMP_ALPHA0);
    if (cur_beta_sw < conf_regs.p_cfg03.sw_num) {
      cur_beta_sw++;
      current_state = DEC_STATES[4];
    } else {
      for (unsigned i = 0; i < conf_regs.p_cfg02.sb_num; i++)
        dec_res[i].current_state &= ~(COMP_BETA0);
      current_state = DEC_STATES[5];
    }
    for (unsigned i = 0; i < conf_regs.p_cfg02.sb_num; i++) {
      dec_res[i].decoder->setbuf(1);
      if (current_iter == 0)
        dec_res[i].decoder->reset_beta0();
      else
        dec_res[i].decoder->ld_beta_init0();
      dec_res[i].decoder->reset_alpha0();
      pending_status.decode = pending_status.decode | (1 << i);
      assert(!dec_res[i].decoder->tickEvent.scheduled());
      schedule(dec_res[i].decoder->tickEvent, curTick());
    }
    if (cur_sw < conf_regs.p_cfg03.sw_num) {
      pending_status.loadsw0 = true;
      schedule(load_sw0Event, curTick());
    }
  } else if (current_state == DEC_STATES[4]) {
    // Second sw alpha, next first sw beta and next second sw loading
    if (cur_sw < conf_regs.p_cfg03.sw_num) {
      pending_status.loadsw1 = true;
      schedule(load_sw1Event, curTick());
      current_state = DEC_STATES[3];
    } else if (cur_beta_sw < conf_regs.p_cfg03.sw_num) {
      current_state = DEC_STATES[3];
    } else {
      current_state = DEC_STATES[5];
    }
    if (cur_alpha_sw < conf_regs.p_cfg03.sw_num)
      cur_alpha_sw++;
    else
      for (unsigned i = 0; i < conf_regs.p_cfg02.sb_num; i++)
        dec_res[i].current_state &= ~(COMP_ALPHA0);
    if (cur_beta_sw < conf_regs.p_cfg03.sw_num)
      cur_beta_sw++;
    else
      for (unsigned i = 0; i < conf_regs.p_cfg02.sb_num; i++)
        dec_res[i].current_state &= ~(COMP_BETA0);
    for (unsigned i = 0; i < conf_regs.p_cfg02.sb_num; i++) {
      dec_res[i].decoder->setbuf(0);
      if (current_iter == 0)
        dec_res[i].decoder->reset_beta0();
      else
        dec_res[i].decoder->ld_beta_init0();
      dec_res[i].decoder->reset_alpha0();
      pending_status.decode = pending_status.decode | (1 << i);
      assert(!dec_res[i].decoder->tickEvent.scheduled());
      schedule(dec_res[i].decoder->tickEvent, curTick());
    }
  } else if (current_state == DEC_STATES[5]) {
    if (current_iter < conf_regs.p_cfg01.max_iter_num) {
      current_state = DEC_STATES[6];
      cur_sw = 0;
      cur_alpha_sw = 0;
      cur_beta_sw = 0;
      for (unsigned i = 0; i < conf_regs.p_cfg02.sb_num; i++) {
        dec_res[i].interleaver[0]->reset();
        dec_res[i].interleaver[1]->reset();
        dec_res[i].decoder->reset();
      }
      for (unsigned i = 0; i < conf_regs.p_cfg02.sb_num; i++) {
        dec_res[i].decoder->setload(cur_sw < conf_regs.p_cfg03.sw_num - 1 ?
        		                sw_len_1 : sw_len_2);
        pending_status.decode = pending_status.decode | (1 << i);
        assert(!dec_res[i].decoder->tickEvent.scheduled());
        schedule(dec_res[i].decoder->tickEvent, curTick());
      }
      pending_status.loadpar0 = true;
      schedule(load_par0Event, curTick());
      if (conf_regs.p_cfg01.crc_sw && current_iter != 0) {
        reset_crc();
        pending_status.crc = true;
        assert(!crcEvent.scheduled());
        schedule(crcEvent, curTick());
      }
    } else {
      current_state = DEC_STATES[0];
      pending_status.abort = true;
      assert(!abortEvent.scheduled());
      schedule(abortEvent, curTick());
    }
  } else if (current_state == DEC_STATES[6]) {
    current_state = DEC_STATES[7];
    cur_beta_sw++;
    for (unsigned i = 0; i < conf_regs.p_cfg02.sb_num; i++) {
      dec_res[i].decoder->setbuf(0);
      if (current_iter == 0)
        dec_res[i].decoder->reset_beta1();
      else
        dec_res[i].decoder->ld_beta_init1();
      pending_status.decode = pending_status.decode | (1 << i);
      assert(!dec_res[i].decoder->tickEvent.scheduled());
      schedule(dec_res[i].decoder->tickEvent, curTick());
    }
    if (cur_sw < conf_regs.p_cfg03.sw_num) {
      for (unsigned i = 0; i < conf_regs.p_cfg02.sb_num; i++)
        dec_res[i].decoder->setload(cur_sw < conf_regs.p_cfg03.sw_num - 1 ?
        		                    sw_len_1 : sw_len_2);
      pending_status.loadpar1 = true;
      schedule(load_par1Event, curTick());
    } else
      for (unsigned i = 0; i < conf_regs.p_cfg02.sb_num; i++)
        dec_res[i].current_state &= ~(COMP_LOAD_PAR1);
  } else if (current_state == DEC_STATES[7]) {
    if (cur_alpha_sw < conf_regs.p_cfg03.sw_num)
      cur_alpha_sw++;
    else
      for (unsigned i = 0; i < conf_regs.p_cfg02.sb_num; i++)
        dec_res[i].current_state &= ~(COMP_ALPHA1);
    if (cur_beta_sw < conf_regs.p_cfg03.sw_num) {
      cur_beta_sw++;
      current_state = DEC_STATES[8];
    } else {
      for (unsigned i = 0; i < conf_regs.p_cfg02.sb_num; i++)
        dec_res[i].current_state &= ~(COMP_BETA1);
      current_state = DEC_STATES[9];
    }
    for (unsigned i = 0; i < conf_regs.p_cfg02.sb_num; i++) {
      dec_res[i].decoder->setbuf(1);
      if (current_iter == 0)
        dec_res[i].decoder->reset_beta1();
      else
        dec_res[i].decoder->ld_beta_init1();
      dec_res[i].decoder->reset_alpha1();
      pending_status.decode = pending_status.decode | (1 << i);
      assert(!dec_res[i].decoder->tickEvent.scheduled());
      schedule(dec_res[i].decoder->tickEvent, curTick());
    }
    if (cur_sw < conf_regs.p_cfg03.sw_num) {
      for (unsigned i = 0; i < conf_regs.p_cfg02.sb_num; i++)
        dec_res[i].decoder->setload(cur_sw < conf_regs.p_cfg03.sw_num - 1 ?
        		                     sw_len_1 : sw_len_2);
      pending_status.loadpar0 = true;
      schedule(load_par0Event, curTick());
    } else
      for (unsigned i = 0; i < conf_regs.p_cfg02.sb_num; i++)
        dec_res[i].current_state &= ~(COMP_LOAD_PAR0);
  } else if (current_state == DEC_STATES[8]) {
    if (cur_sw < conf_regs.p_cfg03.sw_num) {
      current_state = DEC_STATES[7];
      for (unsigned i = 0; i < conf_regs.p_cfg02.sb_num; i++)
        dec_res[i].decoder->setload(cur_sw < conf_regs.p_cfg03.sw_num - 1 ?
        		                     sw_len_1 : sw_len_2);
      pending_status.loadpar1 = true;
      schedule(load_par1Event, curTick());
    } else {
      if (cur_beta_sw < conf_regs.p_cfg03.sw_num)
        current_state = DEC_STATES[7];
      else
        current_state = DEC_STATES[9];
      for (unsigned i = 0; i < conf_regs.p_cfg02.sb_num; i++)
        dec_res[i].current_state &= ~(COMP_LOAD_PAR1);
    }
    if (cur_alpha_sw < conf_regs.p_cfg03.sw_num)
      cur_alpha_sw++;
    else
      for (unsigned i = 0; i < conf_regs.p_cfg02.sb_num; i++)
        dec_res[i].current_state &= ~(COMP_ALPHA1);
    if (cur_beta_sw < conf_regs.p_cfg03.sw_num)
      cur_beta_sw++;
    else
      for (unsigned i = 0; i < conf_regs.p_cfg02.sb_num; i++)
        dec_res[i].current_state &= ~(COMP_BETA1);
    for (unsigned i = 0; i < conf_regs.p_cfg02.sb_num; i++) {
      dec_res[i].decoder->setbuf(0);
      if (current_iter == 0)
        dec_res[i].decoder->reset_beta1();
      else
        dec_res[i].decoder->ld_beta_init1();
      dec_res[i].decoder->reset_alpha1();
      pending_status.decode = pending_status.decode | (1 << i);
      assert(!dec_res[i].decoder->tickEvent.scheduled());
      schedule(dec_res[i].decoder->tickEvent, curTick());
    }
  } else if (current_state == DEC_STATES[9]) {
    current_iter++; 
    turbodec_status.final_iter_num = current_iter;
    current_state = DEC_STATES[0];
    assert(!decodeEvent.scheduled());
    schedule(decodeEvent, curTick());
  }
}

void FixPointTurboDecoder::load_sw0() {
  PortProxy phymem(*localmemPort);
  uint32_t total = 0;
  uint8_t sw_len_1 = conf_regs.p_cfg03.sw_len_1;
  uint8_t sw_len_2 = conf_regs.p_cfg03.sw_len_2;
  DPRINTF(TurboDecLoad, "Load sliding window %d\n", cur_sw);
  for (unsigned i = 0; i < conf_regs.p_cfg02.sb_num; i++) {
	dec_res[i].sw_len[0] = (cur_sw == conf_regs.p_cfg03.sw_num - 1) ?
	                       sw_len_2 : sw_len_1;
    uint32_t total_len = dec_res[i].sw_len[0];
    phymem.readBlob(dec_res[i].cur_sys_base,
                    (uint8_t *)dec_res[i].input_buffer_sys[0],
                    total_len * sizeof(sys_t));
    phymem.readBlob(dec_res[i].cur_par1_base,
                    (uint8_t *)dec_res[i].input_buffer_par[0],
                    total_len * sizeof(sys_t));
    dec_res[i].cur_sys_base += dec_res[i].sw_len[0] * sizeof(sys_t);
    dec_res[i].cur_par1_base += dec_res[i].sw_len[0] * sizeof(sys_t);
    total += total_len / 64 * 2;
  }
  cur_sw++;
  pending_status.loadsw0 = false;
  if (!decodeEvent.scheduled())
    schedule(decodeEvent, curTick() + copDelay * total);
}

void FixPointTurboDecoder::load_sw1() {
  PortProxy phymem(*localmemPort);
  uint32_t total = 0;
  uint8_t sw_len_1 = conf_regs.p_cfg03.sw_len_1;
  uint8_t sw_len_2 = conf_regs.p_cfg03.sw_len_2;
  DPRINTF(TurboDecLoad, "Load sliding window %d\n", cur_sw);
  for (unsigned i = 0; i < conf_regs.p_cfg02.sb_num; i++) {
	dec_res[i].sw_len[1] = (cur_sw == conf_regs.p_cfg03.sw_num - 1) ?
	                       sw_len_2 : sw_len_1;
    uint32_t total_len = dec_res[i].sw_len[1];
    phymem.readBlob(dec_res[i].cur_sys_base,
                    (uint8_t *)dec_res[i].input_buffer_sys[1],
                    total_len * sizeof(sys_t));
    phymem.readBlob(dec_res[i].cur_par1_base,
                    (uint8_t *)dec_res[i].input_buffer_par[1],
                    total_len * sizeof(sys_t));
    dec_res[i].cur_sys_base += dec_res[i].sw_len[1] * sizeof(sys_t);
    dec_res[i].cur_par1_base += dec_res[i].sw_len[1] * sizeof(sys_t);
    total += total_len / 64  * 2;
  }
  cur_sw++;
  pending_status.loadsw1 = false;
  if (!decodeEvent.scheduled())
    schedule(decodeEvent, curTick() + copDelay * total);
}

void FixPointTurboDecoder::load_par0() {
  PortProxy phymem(*localmemPort);
  uint32_t total = 0;
  uint8_t sw_len_1 = conf_regs.p_cfg03.sw_len_1;
  uint8_t sw_len_2 = conf_regs.p_cfg03.sw_len_2;
  for (unsigned i = 0; i < conf_regs.p_cfg02.sb_num; i++) {
	dec_res[i].sw_len[0] = (cur_sw == conf_regs.p_cfg03.sw_num - 1) ?
	                       sw_len_2 : sw_len_1;
    uint32_t total_len = dec_res[i].sw_len[0];
    phymem.readBlob(dec_res[i].cur_par2_base,
                    (uint8_t *)dec_res[i].input_buffer_par[0],
                    total_len * sizeof(sys_t));
    dec_res[i].cur_par2_base += dec_res[i].sw_len[0] * sizeof(sys_t);
    total += total_len / 64;
  }
  cur_sw++;
  pending_status.loadpar0 = false;
  if (!decodeEvent.scheduled())
    schedule(decodeEvent, curTick() + copDelay * total);
}

void FixPointTurboDecoder::load_par1() {
  PortProxy phymem(*localmemPort);
  uint32_t total = 0;
  uint8_t sw_len_1 = conf_regs.p_cfg03.sw_len_1;
  uint8_t sw_len_2 = conf_regs.p_cfg03.sw_len_2;
  for (unsigned i = 0; i < conf_regs.p_cfg02.sb_num; i++) {
	dec_res[i].sw_len[1] = (cur_sw == conf_regs.p_cfg03.sw_num - 1) ?
	                       sw_len_2 : sw_len_1;
    uint32_t total_len = dec_res[i].sw_len[1];
    phymem.readBlob(dec_res[i].cur_par2_base,
                    (uint8_t *)dec_res[i].input_buffer_par[1],
                    total_len * sizeof(sys_t));
    dec_res[i].cur_par2_base += dec_res[i].sw_len[1] * sizeof(sys_t);
    total += total_len / 64 ;
  }
  cur_sw++;
  pending_status.loadpar1 = false;
  if (!decodeEvent.scheduled())
    schedule(decodeEvent, curTick() + copDelay * total);
}

void FixPointTurboDecoder::pre_interleave(uint8_t id, sys_t value) {
  Addr cid = dec_res[id].interleaver[0]->getChipID();
  Addr offset = dec_res[id].interleaver[0]->getOffset();
  dec_res[id].interleaver[0]->compute_inter_inner_idx();
  extrinsic[cid][offset] = value;
}

FixPointTurboDecoder::sys_t FixPointTurboDecoder::post_interleave(uint8_t id) {
  Addr cid = dec_res[id].interleaver[1]->getChipID();
  Addr offset = dec_res[id].interleaver[1]->getOffset();
  DPRINTF(TurboDecPostInterleave, "Interleave: cid:%d, offset:%#04x\n", cid, offset);
  dec_res[id].interleaver[1]->compute_inter_inner_idx();
  return extrinsic[cid][offset];
}

void FixPointTurboDecoder::Interleaver::reset() {
  _t_as = t_as0;
  _t_gi = t_gi0;
  _offset = 0;
  _cid = cid0;
  t_as = t_as0;
  t_gi = t_gi0;
  offset = 0;
  cid = cid0;
  DPRINTFS(TurboDec, parent, "Interleaver%d: reset\n", id);
}

void FixPointTurboDecoder::Interleaver::compute_inter_inner_idx() {
  if (parent->conf_regs.p_cfg02.sb_num > 1)
    cid = (cid + t_as +
           (t_gi + offset > parent->conf_regs.p_cfg02.sb_len ? 1 : 0)) %
            parent->conf_regs.p_cfg02.sb_num;

  offset = (t_gi + offset) % parent->conf_regs.p_cfg02.sb_len;
  if (parent->conf_regs.p_cfg02.sb_num > 1)
      t_as = (t_as + parent->conf_regs.p_cfg04.e1 +
              (t_gi + parent->conf_regs.p_cfg04.e2 > parent->conf_regs.p_cfg02.sb_len ? 1 : 0)) %
                  parent->conf_regs.p_cfg02.sb_num;
  t_gi = (t_gi + 2 * parent->conf_regs.p_cfg04.f2) % parent->conf_regs.p_cfg02.sb_len;

  DPRINTFS(TurboDecInterleaver, parent,
           "Interleaver%d: t_as=%d, t_gi=%d, cid=%d, offset=%d\n",
           id, t_as, t_gi, cid, offset);
 // using namespace std;
  //DPRINTFNR("Interleaver%d: cid:%d,offset:%d\n", id, cid, offset);
}

void FixPointTurboDecoder::reset_crc() {
  t_crc = 0;
}

void FixPointTurboDecoder::crc() {
  for (unsigned i = 0; i < conf_regs.p_cfg02.cb_len / 8; i++){
    MapuISA::crc_sim(t_crc,
      hard_bit[i / (conf_regs.p_cfg02.sb_len / 8)][i % (conf_regs.p_cfg02.sb_len / 8)],
      conf_regs.p_cfg01.crc_type);
    DPRINTF(TurboDecCRC, "iteration: %d, t_crc:%x, data_in:%x\n",
            current_iter, t_crc,
            hard_bit[i / (conf_regs.p_cfg02.sb_len / 8)][i % (conf_regs.p_cfg02.sb_len / 8)]);
  }
  if (t_crc == 0)
	t_crc_flag++;
  else
	t_crc_flag = 0;
  if (t_crc_flag == conf_regs.p_cfg01.crc_num) {
    DPRINTF(TurboDecCRC, "CRC passed at the %dth iteration. "
            "Turbo decoding will be terminated at %d\n", current_iter,
            curTick() + conf_regs.p_cfg02.cb_len / 8 * copDelay);
    turbodec_status.crc_ok = 1;
    turbodec_status.final_iter_num = current_iter;
    assert(!abortEvent.scheduled());
    schedule(abortEvent, curTick() + conf_regs.p_cfg02.cb_len / 8 * copDelay);
  }
  pending_status.crc = false;
}

void FixPointTurboDecoder::abort() {
  PortProxy phymem(*localmemPort);
  uint32_t total = 0;
  if (decodeEvent.scheduled())
    deschedule(decodeEvent);
  if (load_sw0Event.scheduled())
    deschedule(load_sw0Event);
  if (load_sw1Event.scheduled())
    deschedule(load_sw1Event);
  if (load_par0Event.scheduled())
    deschedule(load_par0Event);
  if (load_par1Event.scheduled())
    deschedule(load_par1Event);
  if (crcEvent.scheduled())
    deschedule(crcEvent);
  if (abortEvent.scheduled())
    deschedule(abortEvent);
  for (unsigned i = 0; i < conf_regs.p_cfg02.sb_num; i++) {
    phymem.writeBlob(cur_output_base, (uint8_t *)hard_bit[i],
                     conf_regs.p_cfg02.sb_len / 8);
    cur_output_base += conf_regs.p_cfg02.sb_len / 8;
    total += conf_regs.p_cfg02.sb_len;
  }
  bitsDecoded += conf_regs.p_cfg02.cb_len;
  turbodec_status.status = 0;
  turbodec_status.intr = 1;
  sendEvent(CsuObject::TurboEvent);
  pending_status.abort = false;
}

Tick FixPointTurboDecoder::write(PacketPtr pkt) {
  assert(pkt->getAddr() >= copAddr && pkt->getAddr() < copAddr + copSize);

  Addr daddr = pkt->getAddr() - copAddr;

  if (daddr == TURBODEC_CMD && !decodeEvent.scheduled()) {
    turbodec_status = 0x80000000;
    clearEvent(CsuObject::TurboEvent);
    current_iter = 0;
    current_state = DEC_STATES[0];
    pending_status = 0;
    for (unsigned i = 0; i < conf_regs.p_cfg02.sb_num; i++) {
      memset(extrinsic[i], 0, sizeof(extrinsic[i]));
      memcpy(dec_res[i].alpha_initial_value_sb0, beta_initial_value_ep,
             sizeof(state_metric));
      memcpy(dec_res[i].alpha_initial_value_sb1, beta_initial_value_ep,
             sizeof(state_metric));
    }
    cur_output_base = conf_regs.p_output_base;
    schedule(decodeEvent, curTick() + copDelay);
    pkt->makeAtomicResponse();
    return copDelay;
  }

  DPRINTF(TurboDecCfg, " write register %#x value %#x size=%d\n", daddr,
          pkt->get<uint32_t>(), pkt->getSize());

  uint32_t data = 0;

  data = pkt->get<uint32_t>();

  switch (daddr) {
  case TURBODEC_SYS_BASE:
    conf_regs.p_sys_base = vtophys(cpu->cpuId(),data);
    break;
  case TURBODEC_PAR1_BASE:
    conf_regs.p_par1_base = vtophys(cpu->cpuId(),data);
    break;
  case TURBODEC_PAR2_BASE:
    conf_regs.p_par2_base = vtophys(cpu->cpuId(),data);
    break;
  case TURBODEC_OUTPUT_BASE:
    conf_regs.p_output_base = vtophys(cpu->cpuId(),data);
    break;
  case TURBODEC_CFG01:
    conf_regs.p_cfg01 = data;
    break;
  case TURBODEC_CFG02:
    conf_regs.p_cfg02 = data;
    break;
  case TURBODEC_CFG03:
    conf_regs.p_cfg03 = data;
    break;
  case TURBODEC_SCALING:
    conf_regs.p_scaling_factor = data;
    break;
  case TURBODEC_CFG04:
    conf_regs.p_cfg04 = data;
    break;
  case TURBODEC_CFG05:
    conf_regs.p_cfg05 = data;
    break;
  case TURBODEC_CFG06:
    conf_regs.p_cfg06 = data;
    break;
  case TURBODEC_CFG07:
    conf_regs.p_cfg07 = data;
    break;
  case TURBODEC_CFG08:
    conf_regs.p_cfg08 = data;
    break;
  case TURBODEC_CFG09:
    conf_regs.p_cfg09 = data;
    break;
  case TURBODEC_CFG10:
    conf_regs.p_cfg10 = data;
    break;
  case TURBODEC_CFG11:
    conf_regs.p_cfg11 = data;
    break;
  case TURBODEC_CFG12:
    conf_regs.p_cfg12 = data;
    break;
  case TURBODEC_CFG13:
    conf_regs.p_cfg13 = data;
    dec_res[0].interleaver[0]->set(conf_regs.p_cfg13.as, conf_regs.p_cfg13.gi, conf_regs.p_cfg13.ts);
    dec_res[0].interleaver[1]->set(conf_regs.p_cfg13.as, conf_regs.p_cfg13.gi, conf_regs.p_cfg13.ts);
    break;
  case TURBODEC_CFG14:
    conf_regs.p_cfg14 = data;
    dec_res[1].interleaver[0]->set(conf_regs.p_cfg14.as, conf_regs.p_cfg14.gi, conf_regs.p_cfg14.ts);
    dec_res[1].interleaver[1]->set(conf_regs.p_cfg14.as, conf_regs.p_cfg14.gi, conf_regs.p_cfg14.ts);
    break;
  case TURBODEC_CFG15:
    conf_regs.p_cfg15 = data;
    dec_res[2].interleaver[0]->set(conf_regs.p_cfg15.as, conf_regs.p_cfg15.gi, conf_regs.p_cfg15.ts);
    dec_res[2].interleaver[1]->set(conf_regs.p_cfg15.as, conf_regs.p_cfg15.gi, conf_regs.p_cfg15.ts);
    break;
  case TURBODEC_CFG16:
    conf_regs.p_cfg16 = data;
    dec_res[3].interleaver[0]->set(conf_regs.p_cfg16.as, conf_regs.p_cfg16.gi, conf_regs.p_cfg16.ts);
    dec_res[3].interleaver[1]->set(conf_regs.p_cfg16.as, conf_regs.p_cfg16.gi, conf_regs.p_cfg16.ts);
    break;
    default:
      panic("Tried to write Turbo Decoder at offset %#x that doesn't exist\n", daddr);
      break;
    }
    pkt->makeAtomicResponse();
    return copDelay;
}

void FixPointTurboDecoder::regStats() {
  using namespace Stats;
  bitsDecoded
    .name(name() + ".bytes_copied")
    .desc("Number of bits decoded by Turbo Decoder")
    .prereq(bitsDecoded)
    ;

  decBandwidth
    .name(name() + ".txBandwidth")
    .desc("Decoder Bandwidth (bits/s)")
    .precision(0)
    .prereq(bitsDecoded)
    ;

  decBandwidth = bitsDecoded / simSeconds;
}

FixPointTurboDecoder *FixPointTurboDecoderParams::create() {
  return new FixPointTurboDecoder(this);
}

