/*
 * turbodec.cc
 *
 *  Created on: Apr 17, 2014
 *      Author: wangl
 */

#include "debug/TurboDecAlpha.hh"
#include "mem/packet.hh"
#include "mem/packet_access.hh"

#include "cpu.hh"
#include "utility.hh"
#include "turbodec.hh"

const TurboDecoder::state_metric
TurboDecoder::alpha_initial_value = {        0, MINVALUE, MINVALUE, MINVALUE,
                                      MINVALUE, MINVALUE, MINVALUE, MINVALUE };

const TurboDecoder::state_metric
TurboDecoder::beta_initial_value = {        0, MINVALUE, MINVALUE, MINVALUE,
                                     MINVALUE, MINVALUE, MINVALUE, MINVALUE };
const TurboDecoder::state_metric
TurboDecoder::beta_initial_value_ep = {log1_8, log1_8, log1_8, log1_8,
                                       log1_8, log1_8, log1_8, log1_8};

uint32_t TurboDecoder::Decoder::gamma_addr_gen() {
  uint32_t sw_len = cur_beta_len;
  return sw_len - 1 - cur_pos;
}

void TurboDecoder::Decoder::gamma() {
  uint32_t pos = gamma_addr_gen();
  DPRINTF(TurboDecAlpha, "[iter: %d, id: %d, sw: %d, pos: %d] sys: %f(%#08x) @%#02x\n",
          parent->current_iter, id, parent->cur_beta_sw - 1, cur_pos, sys2beta[pos],
          *(uint32_t *)&sys2beta[pos], pos);
  DPRINTF(TurboDecAlpha, "[iter: %d, id: %d, sw: %d, pos: %d] sys+pri: %f(%#08x) @%#02x\n",
          parent->current_iter, id, parent->cur_beta_sw - 1, cur_pos, t_sys_pri,
          *(uint32_t *)&t_sys_pri, pos);
  DPRINTF(TurboDecAlpha, "[iter: %d, id: %d, sw: %d, pos: %d] par1: %f(%#08x) @%#02x\n",
          parent->current_iter, id, parent->cur_beta_sw - 1, cur_pos, par2beta[pos],
          *(uint32_t *)&par2beta[pos], pos);
  t_gamma[1] = (-t_sys_pri - par2beta[pos]) * 0.5;//gamma11
  t_gamma[0] = (-t_sys_pri + par2beta[pos]) * 0.5;//gamma10
  if (pos < cur_beta_len - cur_pre_len) {
    memcpy(beta2gamma[pos], t_gamma, sizeof(branch_metric));
    beta2sys_pri[pos] = t_sys_pri;
    beta2sys[pos] = sys2beta[pos];
  }
#if TRACING_ON
  DPRINTF(TurboDecAlpha, "[iter: %d, id: %d, sw: %d, pos: %d] gamma(): gamma10:"
          " %f(%#08x), gamma11: %f(%#08x) @%#02x\n", parent->current_iter, id,
          parent->cur_beta_sw - 1, cur_pos, t_gamma[0], *(uint32_t *)&t_gamma[0],
          t_gamma[1], *(uint32_t *)&t_gamma[1], pos);
  DPRINTF(TurboDecAlpha, "[iter: %d, id: %d, sw: %d, pos: %d] beta():",
          parent->current_iter, id, parent->cur_beta_sw - 1, cur_pos);

  for (unsigned i = 0; i < 8; i++)
    DPRINTFR(TurboDecAlpha, " %f(%#08x)", t_beta[i], *(uint32_t *)&t_beta[i]);

  DPRINTFR(TurboDecAlpha, " @%#02x\n", pos);
#endif
  if (pos < cur_beta_len - cur_pre_len)
    memcpy(beta_buffer[pos], t_beta, sizeof(state_metric));
}

void TurboDecoder::Decoder::beta() {
  state_metric beta0, beta1;

  beta0[0] = t_beta[0] - t_gamma[1];
  beta1[0] = t_beta[4] + t_gamma[1];
  beta0[1] = t_beta[4] - t_gamma[1];
  beta1[1] = t_beta[0] + t_gamma[1];
  beta0[2] = t_beta[5] - t_gamma[0];
  beta1[2] = t_beta[1] + t_gamma[0];
  beta0[3] = t_beta[1] - t_gamma[0];
  beta1[3] = t_beta[5] + t_gamma[0];
  beta0[4] = t_beta[2] - t_gamma[0];
  beta1[4] = t_beta[6] + t_gamma[0];
  beta0[5] = t_beta[6] - t_gamma[0];
  beta1[5] = t_beta[2] + t_gamma[0];
  beta0[6] = t_beta[7] - t_gamma[1];
  beta1[6] = t_beta[3] + t_gamma[1];
  beta0[7] = t_beta[3] - t_gamma[1];
  beta1[7] = t_beta[7] + t_gamma[1];

  t_beta[0] = (beta0[0] >= beta1[0]) ? beta0[0] : beta1[0];
  t_beta[1] = (beta0[1] >= beta1[1]) ? beta0[1] : beta1[1];
  t_beta[2] = (beta0[2] >= beta1[2]) ? beta0[2] : beta1[2];
  t_beta[3] = (beta0[3] >= beta1[3]) ? beta0[3] : beta1[3];
  t_beta[4] = (beta0[4] >= beta1[4]) ? beta0[4] : beta1[4];
  t_beta[5] = (beta0[5] >= beta1[5]) ? beta0[5] : beta1[5];
  t_beta[6] = (beta0[6] >= beta1[6]) ? beta0[6] : beta1[6];
  t_beta[7] = (beta0[7] >= beta1[7]) ? beta0[7] : beta1[7];
}

void TurboDecoder::Decoder::alpha() {
  state_metric alpha0, alpha1;

  alpha0[0] = t_alpha[0] - gamma2alpha[cur_pos][1];
  alpha1[0] = t_alpha[1] + gamma2alpha[cur_pos][1];
  alpha0[1] = t_alpha[3] - gamma2alpha[cur_pos][0];
  alpha1[1] = t_alpha[2] + gamma2alpha[cur_pos][0];
  alpha0[2] = t_alpha[4] - gamma2alpha[cur_pos][0];
  alpha1[2] = t_alpha[5] + gamma2alpha[cur_pos][0];
  alpha0[3] = t_alpha[7] - gamma2alpha[cur_pos][1];
  alpha1[3] = t_alpha[6] + gamma2alpha[cur_pos][1];
  alpha0[4] = t_alpha[1] - gamma2alpha[cur_pos][1];
  alpha1[4] = t_alpha[0] + gamma2alpha[cur_pos][1];
  alpha0[5] = t_alpha[2] - gamma2alpha[cur_pos][0];
  alpha1[5] = t_alpha[3] + gamma2alpha[cur_pos][0];
  alpha0[6] = t_alpha[5] - gamma2alpha[cur_pos][0];
  alpha1[6] = t_alpha[4] + gamma2alpha[cur_pos][0];
  alpha0[7] = t_alpha[6] - gamma2alpha[cur_pos][1];
  alpha1[7] = t_alpha[7] + gamma2alpha[cur_pos][1];

  t_alpha[0] = (alpha0[0] >= alpha1[0]) ? alpha0[0] : alpha1[0];
  t_alpha[1] = (alpha0[1] >= alpha1[1]) ? alpha0[1] : alpha1[1];
  t_alpha[2] = (alpha0[2] >= alpha1[2]) ? alpha0[2] : alpha1[2];
  t_alpha[3] = (alpha0[3] >= alpha1[3]) ? alpha0[3] : alpha1[3];
  t_alpha[4] = (alpha0[4] >= alpha1[4]) ? alpha0[4] : alpha1[4];
  t_alpha[5] = (alpha0[5] >= alpha1[5]) ? alpha0[5] : alpha1[5];
  t_alpha[6] = (alpha0[6] >= alpha1[6]) ? alpha0[6] : alpha1[6];
  t_alpha[7] = (alpha0[7] >= alpha1[7]) ? alpha0[7] : alpha1[7];

  float max,min;
  max = t_alpha[0];
  min = t_alpha[0];
  for (unsigned i = 1; i < sizeof(state_metric)/sizeof(float); i++){
    max = (t_alpha[i] >= max) ? t_alpha[i] : max;
    min = (t_alpha[i] <= min) ? t_alpha[i] : min;
  }
  if(((max-min)> 65535) && ((max-min)<(2147483647.0/2.0)))
	  std::cout<<"diff  "<<(max-min)<<std::endl;

#if TRACING_ON
  DPRINTF(TurboDecAlpha, "[iter: %d, id: %d, sw: %d, pos: %d] alpha():",
          parent->current_iter, id, parent->cur_alpha_sw - 1, cur_pos);

  for (unsigned i = 0; i < 8; i++)
    DPRINTFR(TurboDecAlpha, " %f(%#08x)", t_alpha[i], *(uint32_t *)&t_alpha[i]);

  DPRINTFR(TurboDecAlpha, " @%#02x\n", cur_pos);
#endif
}

inline float TurboDecoder::Decoder::find_max(state_metric array) {
  float temp;
  temp = array[0];
  for (unsigned i = 1; i < sizeof(state_metric)/sizeof(float); i++)
    temp = (array[i] >= temp) ? array[i] : temp;
  return temp;
}

void TurboDecoder::Decoder::compute_llr(state_metric b, float systematic) {
  state_metric llr_branch0, llr_branch1;
  float llr0 = 0, llr1 = 0;
  memcpy(t_gamma_in_alpha, gamma2alpha[cur_pos], sizeof(branch_metric));

  llr_branch0[0] = t_alpha[0] + b[0] - t_gamma_in_alpha[1];
  llr_branch0[1] = t_alpha[3] + b[1] - t_gamma_in_alpha[0];
  llr_branch0[2] = t_alpha[4] + b[2] - t_gamma_in_alpha[0];
  llr_branch0[3] = t_alpha[7] + b[3] - t_gamma_in_alpha[1];
  llr_branch0[4] = t_alpha[1] + b[4] - t_gamma_in_alpha[1];
  llr_branch0[5] = t_alpha[2] + b[5] - t_gamma_in_alpha[0];
  llr_branch0[6] = t_alpha[5] + b[6] - t_gamma_in_alpha[0];
  llr_branch0[7] = t_alpha[6] + b[7] - t_gamma_in_alpha[1];
  llr0 = find_max(llr_branch0);

#if TRACING_ON
  DPRINTF(TurboDecAlpha, "[iter: %d, id: %d, sw: %d, pos: %d] llr(): alpha:",
          parent->current_iter, id, parent->cur_alpha_sw - 1, cur_pos);
  for (unsigned i = 0; i < 8; i++)
    DPRINTFR(TurboDecAlpha, " %f(%#08x)", t_alpha[i], *(uint32_t *)&t_alpha[i]);

  DPRINTFR(TurboDecAlpha, " @%#02x\n", cur_pos);
  DPRINTF(TurboDecAlpha, "[iter: %d, id: %d, sw: %d, pos: %d] llr(): beta:",
          parent->current_iter, id, parent->cur_alpha_sw - 1, cur_pos);

  for (unsigned i = 0; i < 8; i++)
    DPRINTFR(TurboDecAlpha, " %f(%#08x)", b[i], *(uint32_t *)&b[i]);

  DPRINTFR(TurboDecAlpha, " @%#02x\n", cur_pos);
  DPRINTF(TurboDecAlpha, "[iter: %d, id: %d, sw: %d, pos: %d] llr(): gamma:",
          parent->current_iter, id, parent->cur_alpha_sw - 1, cur_pos);

  for (unsigned i = 0; i < 2; i++)
    DPRINTFR(TurboDecAlpha, " %f(%#08x)", t_gamma_in_alpha[i], *(uint32_t *)&t_gamma_in_alpha[i]);

  DPRINTFR(TurboDecAlpha, " @%#02x\n", cur_pos);
#endif


  llr_branch1[0] = t_alpha[1] + b[0] + t_gamma_in_alpha[1];
  llr_branch1[1] = t_alpha[2] + b[1] + t_gamma_in_alpha[0];
  llr_branch1[2] = t_alpha[5] + b[2] + t_gamma_in_alpha[0];
  llr_branch1[3] = t_alpha[6] + b[3] + t_gamma_in_alpha[1];
  llr_branch1[4] = t_alpha[0] + b[4] + t_gamma_in_alpha[1];
  llr_branch1[5] = t_alpha[3] + b[5] + t_gamma_in_alpha[0];
  llr_branch1[6] = t_alpha[4] + b[6] + t_gamma_in_alpha[0];
  llr_branch1[7] = t_alpha[7] + b[7] + t_gamma_in_alpha[1];

  llr1 = find_max(llr_branch1);

  t_llr = llr0 - llr1;
  t_le = (t_llr - systematic) * parent->conf_regs.p_scaling_factor;
  DPRINTF(TurboDecAlpha, "[iter: %d, id: %d, sw: %d, pos: %d] llr(): llr: %f(%#08x) "
          "le: %f(%#08x) @%#02x\n",
          parent->current_iter, id, parent->cur_alpha_sw - 1, cur_pos, t_llr,
          *(uint32_t *)&t_llr, t_le, *(uint32_t *)&t_le, cur_pos);
}

void TurboDecoder::Decoder::llr() {
  compute_llr(beta2alpha[cur_pos], sys_pri2alpha[cur_pos]);
}

uint32_t TurboDecoder::Decoder::extrinsic_addr_gen() {
  return gamma_addr_gen() + (parent->cur_beta_sw - 1) * parent->conf_regs.p_sw_len_1;
}

void TurboDecoder::Decoder::compute_extrinsic() {
  uint32_t pos_e = extrinsic_addr_gen();
  uint32_t pos_s = gamma_addr_gen();
  t_sys_pri = sys2beta[pos_s] + parent->extrinsic[id][pos_e];
  DPRINTF(TurboDecAlpha, "[iter: %d, id: %d, sw: %d, pos: %d] extrinsic():"
          " %f(%#08x) sys@%#02x, ext@%#04x\n",
          parent->current_iter, id, parent->cur_beta_sw - 1, cur_pos, t_sys_pri,
          *(uint32_t *)&t_sys_pri, pos_s, pos_e);
}

uint32_t TurboDecoder::Decoder::hard_decision_addr_gen() {
  return cur_pos + (parent->cur_alpha_sw - 1) * parent->conf_regs.p_sw_len_1;
}

void TurboDecoder::Decoder::hard_decision() {
  uint32_t pos = hard_decision_addr_gen();
  bool bit = (t_llr > 0) ? 0 : 1;
  t_hard_bit_compress = (t_hard_bit_compress & ~(1 << (7 - pos % 8))) | (bit << (7 - pos % 8));
  if (pos % 8 == 7) parent->hard_bit[id][pos / 8] = t_hard_bit_compress;
  DPRINTF(TurboDecAlpha, "[iter: %d, id %d, pos: %d] hard_decision(): llr:"
          " %f(%#08x) hard bit: %x @%#02x\n",
          parent->current_iter, id, cur_pos, t_llr,
          *(uint32_t *)&t_llr, bit, pos);
}

void TurboDecoder::Decoder::setbuf(uint8_t idx) {
  DPRINTF(TurboDecAlpha, "[iter: %d, id %d, sw: %d] decoder(): "
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
  cur_beta_len = parent->dec_res[id].sw_len[idx] + parent->dec_res[id].pre_len[idx];
  cur_pre_len = parent->dec_res[id].pre_len[idx];
  cur_alpha_len = parent->dec_res[id].sw_len[!idx];
}

void TurboDecoder::Decoder::reset() {
  parent->dec_res[id].cur_sys_base = parent->conf_regs.p_sys_base +
        parent->conf_regs.p_sb_len * sizeof(float) * id;
  parent->dec_res[id].cur_par1_base = parent->conf_regs.p_par1_base +
        parent->conf_regs.p_sb_len * sizeof(float) * id;
  parent->dec_res[id].cur_par2_base = parent->conf_regs.p_par2_base +
        parent->conf_regs.p_sb_len * sizeof(float) * id;
  parent->dec_res[id].sw_len[0] = 0;
  parent->dec_res[id].sw_len[1] = 0;
  cur_load_len = 0;
  cur_pos = 0;
  DPRINTF(TurboDecAlpha, "[iter: %d, id %d] reset(): sys base: %#08x, "
          "par1 base: %#08x par2 base: %#08x\n",
          parent->current_iter, id, parent->dec_res[id].cur_sys_base,
          parent->dec_res[id].cur_par1_base,
          parent->dec_res[id].cur_par2_base);
}

void TurboDecoder::Decoder::reset_beta0() {
  if ((parent->cur_beta_sw == parent->conf_regs.p_sw_num) &&
      id == parent->conf_regs.p_sb_num - 1)
    memcpy(t_beta, parent->conf_regs.p_beta_initial_value_cb1, sizeof(t_beta));
  else
    memcpy(t_beta, beta_initial_value_ep, sizeof(t_beta));

  if (parent->cur_beta_sw == parent->conf_regs.p_sw_num &&
      id != parent->conf_regs.p_sb_num - 1)
    memcpy(beta_init_ram[parent->cur_beta_sw - 1],
           parent->dec_res[id + 1].t_beta_init_buf, sizeof(t_beta));

  DPRINTF(TurboDecAlpha, "[iter: %d, id: %d, sw: %d, pos: %d] reset_beta0():",
          parent->current_iter, id, parent->cur_beta_sw - 1, cur_pos);
#if TRACING_ON
  for (unsigned i = 0; i < 8; i++)
    DPRINTFR(TurboDecAlpha, " %f(%#08x)", t_beta[i], *(uint32_t *)&t_beta[i]);

  DPRINTFR(TurboDecAlpha, "\n");
#endif
}

void TurboDecoder::Decoder::reset_beta1() {
  if ((parent->cur_beta_sw == parent->conf_regs.p_sw_num) &&
      id == parent->conf_regs.p_sb_num - 1)
    memcpy(t_beta, parent->conf_regs.p_beta_initial_value_cb2, sizeof(t_beta));
  else
    memcpy(t_beta, beta_initial_value_ep, sizeof(t_beta));

  if (parent->cur_beta_sw == parent->conf_regs.p_sw_num &&
      id != parent->conf_regs.p_sb_num - 1)
    memcpy(beta_init_ram[parent->cur_beta_sw - 1 + MAX_SW_NUM],
           parent->dec_res[id + 1].t_beta_init_buf, sizeof(t_beta));

  DPRINTF(TurboDecAlpha, "[iter: %d, id: %d, sw: %d] reset_beta1():",
          parent->current_iter, id, parent->cur_beta_sw - 1);
#if TRACING_ON
  for (unsigned i = 0; i < 8; i++)
    DPRINTFR(TurboDecAlpha, " %f(%#08x)", t_beta[i], *(uint32_t *)&t_beta[i]);

  DPRINTFR(TurboDecAlpha, "\n");
#endif
}

void TurboDecoder::Decoder::ld_beta_init0() {
  if ((parent->cur_beta_sw == parent->conf_regs.p_sw_num) &&
      id == parent->conf_regs.p_sb_num - 1)
    memcpy(t_beta, parent->conf_regs.p_beta_initial_value_cb1, sizeof(t_beta));
  else
    memcpy(t_beta, beta_init_ram[parent->cur_beta_sw - 1], sizeof(t_beta));

  if (parent->cur_beta_sw == parent->conf_regs.p_sw_num &&
      id != parent->conf_regs.p_sb_num - 1)
    memcpy(beta_init_ram[parent->cur_beta_sw - 1],
           parent->dec_res[id + 1].t_beta_init_buf, sizeof(t_beta));

  DPRINTF(TurboDecAlpha, "[iter: %d, id: %d, sw: %d, pos: %d] ld_beta_init0():",
          parent->current_iter, id, parent->cur_beta_sw - 1, cur_pos);
#if TRACING_ON
  for (unsigned i = 0; i < 8; i++)
    DPRINTFR(TurboDecAlpha, " %f(%#08x)", t_beta[i], *(uint32_t *)&t_beta[i]);

  DPRINTFR(TurboDecAlpha, "@(%#08x)\n", parent->cur_beta_sw - 1);
#endif
}

void TurboDecoder::Decoder::ld_beta_init1() {
  if ((parent->cur_beta_sw == parent->conf_regs.p_sw_num) &&
      id == parent->conf_regs.p_sb_num - 1)
    memcpy(t_beta, parent->conf_regs.p_beta_initial_value_cb2, sizeof(t_beta));
  else
    memcpy(t_beta, beta_init_ram[parent->cur_beta_sw - 1 + MAX_SW_NUM], sizeof(t_beta));

  if (parent->cur_beta_sw == parent->conf_regs.p_sw_num &&
      id != parent->conf_regs.p_sb_num - 1)
    memcpy(beta_init_ram[parent->cur_beta_sw - 1 + MAX_SW_NUM],
           parent->dec_res[id + 1].t_beta_init_buf, sizeof(t_beta));

  DPRINTF(TurboDecAlpha, "[iter: %d, id: %d, sw: %d] ld_beta_init1():",
          parent->current_iter, id, parent->cur_beta_sw - 1);
#if TRACING_ON
  for (unsigned i = 0; i < 8; i++)
    DPRINTFR(TurboDecAlpha, " %f(%#08x)", t_beta[i], *(uint32_t *)&t_beta[i]);

  DPRINTFR(TurboDecAlpha, "@(%#08x)\n", parent->cur_beta_sw - 1 + MAX_SW_NUM);
#endif
}

void TurboDecoder::Decoder::st_beta_init0() {
  if (parent->cur_beta_sw == 1)
    memcpy(parent->dec_res[id].t_beta_init_buf, t_beta, sizeof(t_beta));
  else
    memcpy(beta_init_ram[parent->cur_beta_sw - 2], t_beta, sizeof(t_beta));

  DPRINTF(TurboDecAlpha, "[iter: %d, id: %d, sw: %d, pos: %d] st_beta_init0():",
          parent->current_iter, id, parent->cur_beta_sw - 1, cur_pos);
#if TRACING_ON
  for (unsigned i = 0; i < 8; i++)
    DPRINTFR(TurboDecAlpha, " %f(%#08x)", t_beta[i], *(uint32_t *)&t_beta[i]);

  DPRINTFR(TurboDecAlpha, "@(%#08x)\n", parent->cur_beta_sw - 2);
#endif
}

void TurboDecoder::Decoder::st_beta_init1() {
  if (parent->cur_beta_sw == 1)
    memcpy(parent->dec_res[id].t_beta_init_buf, t_beta, sizeof(t_beta));
  else
    memcpy(beta_init_ram[parent->cur_beta_sw - 2 + MAX_SW_NUM], t_beta, sizeof(t_beta));

  DPRINTF(TurboDecAlpha, "[iter: %d, id: %d, sw: %d] st_beta_init1():",
          parent->current_iter, id, parent->cur_beta_sw - 1);
#if TRACING_ON
  for (unsigned i = 0; i < 8; i++)
    DPRINTFR(TurboDecAlpha, " %f(%#08x)", t_beta[i], *(uint32_t *)&t_beta[i]);

  DPRINTFR(TurboDecAlpha, "@(%#08x)\n", parent->cur_beta_sw - 2 + MAX_SW_NUM);
#endif
}

void TurboDecoder::Decoder::reset_alpha0() {
  if ((parent->cur_alpha_sw - 1 == 0) && (id == 0))
    memcpy(t_alpha, alpha_initial_value, sizeof(t_alpha));
  else if (parent->cur_alpha_sw - 1 == 0)
    memcpy(t_alpha, parent->dec_res[id - 1].alpha_initial_value_sb0, sizeof(t_alpha));

#if TRACING_ON
  DPRINTF(TurboDecAlpha, "[iter: %d, id: %d, sw: %d] reset_alpha0():",
          parent->current_iter, id, parent->cur_alpha_sw - 1);

  for (unsigned i = 0; i < 8; i++)
    DPRINTFR(TurboDecAlpha, " %f(%#08x)", t_alpha[i], *(uint32_t *)&t_alpha[i]);

  DPRINTFR(TurboDecAlpha, "\n");
#endif
}

void TurboDecoder::Decoder::reset_alpha1() {
  if ((parent->cur_alpha_sw - 1 == 0) && (id == 0))
    memcpy(t_alpha, alpha_initial_value, sizeof(t_alpha));
  else if (parent->cur_alpha_sw - 1 == 0)
    memcpy(t_alpha, parent->dec_res[id - 1].alpha_initial_value_sb1, sizeof(t_alpha));

#if TRACING_ON
  DPRINTF(TurboDecAlpha, "[iter: %d, id: %d, sw: %d] reset_alpha1():",
          parent->current_iter, id, parent->cur_alpha_sw - 1);

  for (unsigned i = 0; i < 8; i++)
    DPRINTFR(TurboDecAlpha, " %f(%#08x)", t_alpha[i], *(uint32_t *)&t_alpha[i]);

  DPRINTFR(TurboDecAlpha, "\n");
#endif
}

uint32_t TurboDecoder::Decoder::pre_interleave_addr_gen() {
  return cur_pos + (parent->cur_alpha_sw - 1) * parent->conf_regs.p_sw_len_1;
}

void TurboDecoder::Decoder::tick() {
  if (parent->status == 0) return;
  uint32_t state = parent->dec_res[id].current_state;
  DPRINTF(TurboDecAlpha, "sub state: %x %d\n", state, cur_pos);
  if ((state & (1 << POST_INTERLEAVE0 | 1 << POST_INTERLEAVE1)) &&  cur_pos < cur_load_len) {
    if (cur_pos == 0)
      parent->dec_res[id].interleaver[1]->restore_context();
    if (state & (1 << POST_INTERLEAVE0)) {
      parent->dec_res[id].input_buffer_sys[0][cur_pos] = parent->post_interleave(id);
      DPRINTF(TurboDecAlpha, "[iter: %d, id %d, sw %d, pos: %d] post_interleave0(): %f(%#08x) @%#02x\n",
              parent->current_iter, id, parent->cur_beta_sw, cur_pos, parent->dec_res[id].input_buffer_sys[0][cur_pos],
              *(uint32_t *)&parent->dec_res[id].input_buffer_sys[0][cur_pos], cur_pos);
    } else {
      parent->dec_res[id].input_buffer_sys[1][cur_pos] = parent->post_interleave(id);
      DPRINTF(TurboDecAlpha, "[iter: %d, id %d, sw %d, pos: %d] post_interleave1(): %f(%#08x) @%#02x\n",
              parent->current_iter, id, parent->cur_beta_sw, cur_pos, parent->dec_res[id].input_buffer_sys[1][cur_pos],
              *(uint32_t *)&parent->dec_res[id].input_buffer_sys[1][cur_pos], cur_pos);
    }
    if (cur_pos == cur_load_len - cur_pre_len - 1)
      parent->dec_res[id].interleaver[1]->save_context();
  }
  if ((state & (1 << EXTRINSIC)) && cur_pos < cur_beta_len)
    compute_extrinsic();
  else if ((state & (1 << GAMMA0 | 1 << GAMMA1)) && cur_pos < cur_beta_len)
    t_sys_pri = sys2beta[gamma_addr_gen()];
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
    if (state & (1 << ALPHA0))
      memcpy(parent->dec_res[id].alpha_initial_value_sb0, t_alpha, sizeof(t_alpha));
    else
      memcpy(parent->dec_res[id].alpha_initial_value_sb1, t_alpha, sizeof(t_alpha));
  }
  if ((state & (1 << PRE_INTERLEAVE)) && cur_pos < cur_alpha_len) {
    DPRINTF(TurboDecAlpha, "[iter: %d, id %d, sw %d, pos: %d] pre_interleave(): "
            "%f(%#08x) cid@%#02x,offset@%#04x\n",
            parent->current_iter, id, parent->cur_alpha_sw - 1, cur_pos, t_le,
            *(uint32_t *)&t_le, parent->dec_res[id].interleaver[0]->getChipID(),
            parent->dec_res[id].interleaver[0]->getOffset());
    parent->pre_interleave(id, t_le);
  } else if ((state & (1 << LLR)) && cur_pos < cur_alpha_len) {
    parent->extrinsic[id][pre_interleave_addr_gen()] = t_le + sys2alpha[cur_pos];
    DPRINTF(TurboDecAlpha, "[iter: %d, id %d, sw %d, pos: %d] non_pre_interleave(): "
            "%f(%#08x) @%#02x\n",
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

const uint32_t TurboDecoder::DEC_STATES[] = {
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

TurboDecoder::TurboDecoder(const Params *p) :
  CoProcessor(p), status(0), current_state(DEC_STATES[0]), current_iter(0),
  cur_sw(0), cur_alpha_sw(0), cur_beta_sw(0),
  cur_output_base(0), t_crc(0), localmemPort(NULL), decodeEvent(this),
  load_sw0Event(this), load_sw1Event(this), load_par0Event(this),
  load_par1Event(this), crcEvent(this),  abortEvent(this) {}

TurboDecoder::~TurboDecoder() {
  for (unsigned i = 0; i < NUM_SUB_DEC; i++) {
    if (dec_res[i].decoder)
      delete dec_res[i].decoder;
    if (dec_res[i].interleaver[0])
      delete dec_res[i].interleaver[0];
    if (dec_res[i].interleaver[1])
      delete dec_res[i].interleaver[1];
  }
}

void TurboDecoder::init() {
  CoProcessor::init();
  for (unsigned i = 0; i < NUM_SUB_DEC; i++) {
    dec_res[i].decoder = new Decoder(this, i);
    dec_res[i].interleaver[0] = new Interleaver(this, i);
    dec_res[i].interleaver[1] = new Interleaver(this, i);
  }
  localmemPort = &cpu->getTurboDecPort();
  assert(localmemPort != NULL);
}

Tick TurboDecoder::read(PacketPtr pkt) {
  assert(pkt->getAddr() >= copAddr && pkt->getAddr() < copAddr + copSize);

  Addr daddr = pkt->getAddr() - copAddr;
  pkt->allocate();

  DPRINTF(TurboDecAlpha, " read register %#x size=%d\n", daddr, pkt->getSize());

  uint32_t data = 0;

  switch (daddr) {
  case TURBODEC_CMD:
    data = status;
    break;
  case TURBODEC_INTR:
    clearEvent(CsuObject::TurboEvent);
    intr = 0;
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

void TurboDecoder::decode() {
  if (status == 0) return;
  DPRINTF(TurboDecAlpha, "State: %x\n", current_state);
  for (unsigned i = 0; i < conf_regs.p_sb_num; i++)
    if (pending_status != 0)
      return;
  for (unsigned i = 0; i < conf_regs.p_sb_num; i++)
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
    for (unsigned i = 0; i < conf_regs.p_sb_num; i++) {
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
    for (unsigned i = 0; i < conf_regs.p_sb_num; i++) {
      dec_res[i].decoder->setbuf(0);
      if (current_iter == 0)
        dec_res[i].decoder->reset_beta0();
      else
        dec_res[i].decoder->ld_beta_init0();
      pending_status.decode = pending_status.decode | (1 << i);
      assert(!dec_res[i].decoder->tickEvent.scheduled());
      schedule(dec_res[i].decoder->tickEvent, curTick());;
    }
    if (cur_sw < conf_regs.p_sw_num) {
      pending_status.loadsw1 = true;
      assert(!load_sw1Event.scheduled());
      schedule(load_sw1Event, curTick());
    }
  } else if (current_state == DEC_STATES[3]) {
    // First sw alpha, second sw beta and next first sw loading
    if (cur_alpha_sw < conf_regs.p_sw_num)
      cur_alpha_sw++;
    else
      for (unsigned i = 0; i < conf_regs.p_sb_num; i++)
        dec_res[i].current_state &= ~(COMP_ALPHA0);
    if (cur_beta_sw < conf_regs.p_sw_num) {
      cur_beta_sw++;
      current_state = DEC_STATES[4];
    } else {
      for (unsigned i = 0; i < conf_regs.p_sb_num; i++)
        dec_res[i].current_state &= ~(COMP_BETA0);
      current_state = DEC_STATES[5];
    }
    for (unsigned i = 0; i < conf_regs.p_sb_num; i++) {
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
    if (cur_sw < conf_regs.p_sw_num) {
      pending_status.loadsw0 = true;
      schedule(load_sw0Event, curTick());
    }
  } else if (current_state == DEC_STATES[4]) {
    // Second sw alpha, next first sw beta and next second sw loading
    if (cur_sw < conf_regs.p_sw_num) {
      pending_status.loadsw1 = true;
      schedule(load_sw1Event, curTick());
      current_state = DEC_STATES[3];
    } else if (cur_beta_sw < conf_regs.p_sw_num) {
      current_state = DEC_STATES[3];
    } else {
      current_state = DEC_STATES[5];
    }
    if (cur_alpha_sw < conf_regs.p_sw_num)
      cur_alpha_sw++;
    else
      for (unsigned i = 0; i < conf_regs.p_sb_num; i++)
        dec_res[i].current_state &= ~(COMP_ALPHA0);
    if (cur_beta_sw < conf_regs.p_sw_num)
      cur_beta_sw++;
    else
      for (unsigned i = 0; i < conf_regs.p_sb_num; i++)
        dec_res[i].current_state &= ~(COMP_BETA0);
    for (unsigned i = 0; i < conf_regs.p_sb_num; i++) {
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
    if (current_iter < conf_regs.p_max_interation_num) {
      current_state = DEC_STATES[6];
      cur_sw = 0;
      cur_alpha_sw = 0;
      cur_beta_sw = 0;
      for (unsigned i = 0; i < conf_regs.p_sb_num; i++) {
        dec_res[i].interleaver[0]->reset();
        dec_res[i].interleaver[1]->reset();
        dec_res[i].decoder->reset();
      }
      for (unsigned i = 0; i < conf_regs.p_sb_num; i++) {
        dec_res[i].decoder->setload(cur_sw < conf_regs.p_sw_num - 1 ?
                                    conf_regs.p_sw_len_1 + conf_regs.p_pre_len :
                                    conf_regs.p_sw_len_2 + conf_regs.p_pre_len);
        pending_status.decode = pending_status.decode | (1 << i);
        assert(!dec_res[i].decoder->tickEvent.scheduled());
        schedule(dec_res[i].decoder->tickEvent, curTick());
      }
      pending_status.loadpar0 = true;
      schedule(load_par0Event, curTick());
      if (conf_regs.p_crc_sw && current_iter != 0) {
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
    for (unsigned i = 0; i < conf_regs.p_sb_num; i++) {
      dec_res[i].decoder->setbuf(0);
      if (current_iter == 0)
        dec_res[i].decoder->reset_beta1();
      else
        dec_res[i].decoder->ld_beta_init1();
      pending_status.decode = pending_status.decode | (1 << i);
      assert(!dec_res[i].decoder->tickEvent.scheduled());
      schedule(dec_res[i].decoder->tickEvent, curTick());
    }
    if (cur_sw < conf_regs.p_sw_num) {
      for (unsigned i = 0; i < conf_regs.p_sb_num; i++)
        dec_res[i].decoder->setload(cur_sw < conf_regs.p_sw_num - 1 ?
                                    conf_regs.p_sw_len_1 + conf_regs.p_pre_len :
                                    conf_regs.p_sw_len_2 + conf_regs.p_pre_len);
      pending_status.loadpar1 = true;
      schedule(load_par1Event, curTick());
    } else
      for (unsigned i = 0; i < conf_regs.p_sb_num; i++)
        dec_res[i].current_state &= ~(COMP_LOAD_PAR1);
  } else if (current_state == DEC_STATES[7]) {
    if (cur_alpha_sw < conf_regs.p_sw_num)
      cur_alpha_sw++;
    else
      for (unsigned i = 0; i < conf_regs.p_sb_num; i++)
        dec_res[i].current_state &= ~(COMP_ALPHA1);
    if (cur_beta_sw < conf_regs.p_sw_num) {
      cur_beta_sw++;
      current_state = DEC_STATES[8];
    } else {
      for (unsigned i = 0; i < conf_regs.p_sb_num; i++)
        dec_res[i].current_state &= ~(COMP_BETA1);
      current_state = DEC_STATES[9];
    }
    for (unsigned i = 0; i < conf_regs.p_sb_num; i++) {
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
    if (cur_sw < conf_regs.p_sw_num) {
      for (unsigned i = 0; i < conf_regs.p_sb_num; i++)
        dec_res[i].decoder->setload(cur_sw < conf_regs.p_sw_num - 1 ?
                                conf_regs.p_sw_len_1 + conf_regs.p_pre_len :
                                conf_regs.p_sw_len_2 + conf_regs.p_pre_len);
      pending_status.loadpar0 = true;
      schedule(load_par0Event, curTick());
    } else
      for (unsigned i = 0; i < conf_regs.p_sb_num; i++)
        dec_res[i].current_state &= ~(COMP_LOAD_PAR0);
  } else if (current_state == DEC_STATES[8]) {
    if (cur_sw < conf_regs.p_sw_num) {
      current_state = DEC_STATES[7];
      for (unsigned i = 0; i < conf_regs.p_sb_num; i++)
        dec_res[i].decoder->setload(cur_sw < conf_regs.p_sw_num - 1 ?
                                    conf_regs.p_sw_len_1 + conf_regs.p_pre_len :
                                    conf_regs.p_sw_len_2 + conf_regs.p_pre_len);
      pending_status.loadpar1 = true;
      schedule(load_par1Event, curTick());
    } else {
      if (cur_beta_sw < conf_regs.p_sw_num)
        current_state = DEC_STATES[7];
      else
        current_state = DEC_STATES[9];
      for (unsigned i = 0; i < conf_regs.p_sb_num; i++)
        dec_res[i].current_state &= ~(COMP_LOAD_PAR1);
    }
    if (cur_alpha_sw < conf_regs.p_sw_num)
      cur_alpha_sw++;
    else
      for (unsigned i = 0; i < conf_regs.p_sb_num; i++)
        dec_res[i].current_state &= ~(COMP_ALPHA1);
    if (cur_beta_sw < conf_regs.p_sw_num)
      cur_beta_sw++;
    else
      for (unsigned i = 0; i < conf_regs.p_sb_num; i++)
        dec_res[i].current_state &= ~(COMP_BETA1);
    for (unsigned i = 0; i < conf_regs.p_sb_num; i++) {
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
    conf_regs.p_pre_len = 0;
    current_state = DEC_STATES[0];
    assert(!decodeEvent.scheduled());
    schedule(decodeEvent, curTick());
  }
}

void TurboDecoder::load_sw0() {
  PortProxy phymem(*localmemPort);
  uint32_t total = 0;
  DPRINTF(TurboDecAlpha, "Load sliding window %d\n", cur_sw);
  for (unsigned i = 0; i < conf_regs.p_sb_num; i++) {
    dec_res[i].sw_len[0] = (cur_sw == conf_regs.p_sw_num - 1) ?
                           conf_regs.p_sw_len_2 : conf_regs.p_sw_len_1;
    dec_res[i].pre_len[0] = ((cur_sw == conf_regs.p_sw_num - 1) &&
                             (i == conf_regs.p_sb_num - 1)) ?
                            0 : conf_regs.p_pre_len;
    uint32_t total_len = dec_res[i].sw_len[0] + dec_res[i].pre_len[0];
    phymem.readBlob(dec_res[i].cur_sys_base,
                    (uint8_t *)dec_res[i].input_buffer_sys[0],
                    total_len * sizeof(float));
    phymem.readBlob(dec_res[i].cur_par1_base,
                    (uint8_t *)dec_res[i].input_buffer_par[0],
                    total_len * sizeof(float));
    dec_res[i].cur_sys_base += dec_res[i].sw_len[0] * sizeof(float);
    dec_res[i].cur_par1_base += dec_res[i].sw_len[0] * sizeof(float);
    total += total_len / 64 * 2;
  }
  cur_sw++;
  pending_status.loadsw0 = false;
  if (!decodeEvent.scheduled())
    schedule(decodeEvent, curTick() + copDelay * total);
}

void TurboDecoder::load_sw1() {
  PortProxy phymem(*localmemPort);
  uint32_t total = 0;
  DPRINTF(TurboDecAlpha, "Load sliding window %d\n", cur_sw);
  for (unsigned i = 0; i < conf_regs.p_sb_num; i++) {
    dec_res[i].sw_len[1] = (cur_sw == conf_regs.p_sw_num - 1) ?
                           conf_regs.p_sw_len_2 : conf_regs.p_sw_len_1;
    dec_res[i].pre_len[1] = ((cur_sw == conf_regs.p_sw_num - 1) &&
                             (i == conf_regs.p_sb_num - 1)) ?
                            0 : conf_regs.p_pre_len;
    uint32_t total_len = dec_res[i].sw_len[1] + dec_res[i].pre_len[1];
    phymem.readBlob(dec_res[i].cur_sys_base,
                    (uint8_t *)dec_res[i].input_buffer_sys[1],
                    total_len * sizeof(float));
    phymem.readBlob(dec_res[i].cur_par1_base,
                    (uint8_t *)dec_res[i].input_buffer_par[1],
                    total_len * sizeof(float));
    dec_res[i].cur_sys_base += dec_res[i].sw_len[1] * sizeof(float);
    dec_res[i].cur_par1_base += dec_res[i].sw_len[1] * sizeof(float);
    total += total_len / 64  * 2;
  }
  cur_sw++;
  pending_status.loadsw1 = false;
  if (!decodeEvent.scheduled())
    schedule(decodeEvent, curTick() + copDelay * total);
}

void TurboDecoder::load_par0() {
  PortProxy phymem(*localmemPort);
  uint32_t total = 0;
  for (unsigned i = 0; i < conf_regs.p_sb_num; i++) {
    dec_res[i].sw_len[0] = (cur_sw == conf_regs.p_sw_num - 1) ?
                           conf_regs.p_sw_len_2 : conf_regs.p_sw_len_1;
    dec_res[i].pre_len[0] = ((cur_sw == conf_regs.p_sw_num - 1) &&
                             (i == conf_regs.p_sb_num - 1)) ?
                            0 : conf_regs.p_pre_len;
    uint32_t total_len = dec_res[i].sw_len[0] + dec_res[i].pre_len[0];
    phymem.readBlob(dec_res[i].cur_par2_base,
                    (uint8_t *)dec_res[i].input_buffer_par[0],
                    total_len * sizeof(float));
    dec_res[i].cur_par2_base += dec_res[i].sw_len[0] * sizeof(float);
    total += total_len / 64;
  }
  cur_sw++;
  pending_status.loadpar0 = false;
  if (!decodeEvent.scheduled())
    schedule(decodeEvent, curTick() + copDelay * total);
}

void TurboDecoder::load_par1() {
  PortProxy phymem(*localmemPort);
  uint32_t total = 0;
  for (unsigned i = 0; i < conf_regs.p_sb_num; i++) {
    dec_res[i].sw_len[1] = (cur_sw == conf_regs.p_sw_num - 1) ?
                           conf_regs.p_sw_len_2 : conf_regs.p_sw_len_1;
    dec_res[i].pre_len[1] = ((cur_sw == conf_regs.p_sw_num - 1) &&
                             (i == conf_regs.p_sb_num - 1)) ?
                            0 : conf_regs.p_pre_len;
    uint32_t total_len = dec_res[i].sw_len[1] + dec_res[i].pre_len[1];
    phymem.readBlob(dec_res[i].cur_par2_base,
                    (uint8_t *)dec_res[i].input_buffer_par[1],
                    total_len * sizeof(float));
    dec_res[i].cur_par2_base += dec_res[i].sw_len[1] * sizeof(float);
    total += total_len / 64 ;
  }
  cur_sw++;
  pending_status.loadpar1 = false;
  if (!decodeEvent.scheduled())
    schedule(decodeEvent, curTick() + copDelay * total);
}

void TurboDecoder::pre_interleave(uint8_t id, float value) {
  Addr cid = dec_res[id].interleaver[0]->getChipID();
  Addr offset = dec_res[id].interleaver[0]->getOffset();
  dec_res[id].interleaver[0]->compute_inter_inner_idx();
  extrinsic[cid][offset] = value;
}

float TurboDecoder::post_interleave(uint8_t id) {
  Addr cid = dec_res[id].interleaver[1]->getChipID();
  Addr offset = dec_res[id].interleaver[1]->getOffset();
  DPRINTF(TurboDecAlpha, "Interleave: cid:%d, offset:%#04x\n", cid, offset);
  dec_res[id].interleaver[1]->compute_inter_inner_idx();
  return extrinsic[cid][offset];
}

void TurboDecoder::Interleaver::reset() {
  _t_as = t_as0;
  _t_gi = t_gi0;
  _offset = 0;
  _cid = cid0;
  t_as = t_as0;
  t_gi = t_gi0;
  offset = 0;
  cid = cid0;
  DPRINTFS(TurboDecAlpha, parent, "Interleaver%d: reset\n", id);
}

void TurboDecoder::Interleaver::compute_inter_inner_idx() {
  if (parent->conf_regs.p_sb_num > 1)
    cid = (cid + t_as +
           (t_gi + offset > parent->conf_regs.p_sb_len ? 1 : 0)) %
            parent->conf_regs.p_sb_num;

  offset = (t_gi + offset) % parent->conf_regs.p_sb_len;
  if (parent->conf_regs.p_sb_num > 1)
      t_as = (t_as + parent->conf_regs.p_e1 +
              (t_gi + parent->conf_regs.p_e2 > parent->conf_regs.p_sb_len ? 1 : 0)) %
                  parent->conf_regs.p_sb_num;
  t_gi = (t_gi + 2 * parent->conf_regs.p_f2) % parent->conf_regs.p_sb_len;

  DPRINTFS(TurboDecAlpha, parent, "Interleaver%d: t_as=%d, t_gi=%d, cid=%d, offset=%d\n", id, t_as, t_gi, cid, offset);
 // using namespace std;
  //DPRINTFNR("Interleaver%d: cid:%d,offset:%d\n", id, cid, offset);
}

void TurboDecoder::reset_crc() {
  t_crc = 0;
}

void TurboDecoder::crc() {
  for (unsigned i = 0; i < conf_regs.p_cb_len / 8; i++){
    MapuISA::crc_sim(t_crc,
      hard_bit[i / (conf_regs.p_sb_len / 8)][i % (conf_regs.p_sb_len / 8)],
      conf_regs.p_crc_type);
    DPRINTF(TurboDecAlpha, "iteration: %d, t_crc:%x, data_in:%x\n",
            current_iter, t_crc,
            hard_bit[i / (conf_regs.p_sb_len / 8)][i % (conf_regs.p_sb_len / 8)]);
  }
  if (t_crc == 0) {
    DPRINTF(TurboDecAlpha, "CRC passed at the %dth iteration. "
            "Turbo decoding will be terminated at %d\n", current_iter,
            curTick() + conf_regs.p_cb_len / 8 * copDelay);
    assert(!abortEvent.scheduled());
    schedule(abortEvent, curTick() + conf_regs.p_cb_len / 8 * copDelay);
  }
  pending_status.crc = false;
}

void TurboDecoder::abort() {
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
  for (unsigned i = 0; i < conf_regs.p_sb_num; i++) {
    phymem.writeBlob(cur_output_base, (uint8_t *)hard_bit[i],
                     conf_regs.p_sb_len / 8);
    cur_output_base += conf_regs.p_sb_len / 8;
    total += conf_regs.p_sb_len;
  }
  bitsDecoded += conf_regs.p_cb_len;
  status = 0;
  intr = 1;
  sendEvent(CsuObject::TurboEvent);
  pending_status.abort = false;
}

Tick TurboDecoder::write(PacketPtr pkt) {
  assert(pkt->getAddr() >= copAddr && pkt->getAddr() < copAddr + copSize);

  Addr daddr = pkt->getAddr() - copAddr;

  if (daddr == TURBODEC_CMD && !decodeEvent.scheduled()) {
    status = 1;
    clearEvent(CsuObject::TurboEvent);
    current_iter = 0;
    current_state = DEC_STATES[0];
    pending_status = 0;
    for (unsigned i = 0; i < conf_regs.p_sb_num; i++) {
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

  DPRINTF(TurboDecAlpha, " write register %#x value %#x size=%d\n", daddr,
          pkt->get<uint32_t>(), pkt->getSize());

  uint32_t data = 0;

  data = pkt->get<uint32_t>();

  switch (daddr) {
  case TURBODEC_SYS_BASE:
    conf_regs.p_sys_base = data;
    break;
  case TURBODEC_PAR1_BASE:
    conf_regs.p_par1_base = data;
    break;
  case TURBODEC_PAR2_BASE:
    conf_regs.p_par2_base = data;
    break;
  case TURBODEC_OUTPUT_BASE:
    conf_regs.p_output_base = data;
    break;
  case TURBODEC_MAX_ITER_NUM:
    conf_regs.p_max_interation_num = data;
    break;
  case TURBODEC_CB_LEN:
    conf_regs.p_cb_len = data;
    break;
  case TURBODEC_SB_LEN:
    conf_regs.p_sb_len = data;
    break;
  case TURBODEC_SB_NUM:
    conf_regs.p_sb_num = data;
    break;
  case TURBODEC_SW_LEN1:
    conf_regs.p_sw_len_1 = data;
    break;
  case TURBODEC_SW_LEN2:
    conf_regs.p_sw_len_2 = data;
    break;
  case TURBODEC_SW_NUM:
    conf_regs.p_sw_num = data;
    break;
  case TURBODEC_PRE_LEN:
    conf_regs.p_pre_len = 0;//data;
    break;
  case TURBODEC_SCALING:
    conf_regs.p_scaling_factor = *(float *)&data;
    break;
  case TURBODEC_F2:
    conf_regs.p_f2 = data;
    break;
  case TURBODEC_E1:
    conf_regs.p_e1 = data;
    break;
  case TURBODEC_E2:
    conf_regs.p_e2 = data;
    break;
  case TURBODEC_INTLV_CONF0:
    dec_res[0].interleaver[0]->set(data >> 24, data & 0xffff, (data >> 16) & 0xff);
    dec_res[0].interleaver[1]->set(data >> 24, data & 0xffff, (data >> 16) & 0xff);
    break;
  case TURBODEC_INTLV_CONF1:
    dec_res[1].interleaver[0]->set(data >> 24, data & 0xffff, (data >> 16) & 0xff);
    dec_res[1].interleaver[1]->set(data >> 24, data & 0xffff, (data >> 16) & 0xff);
    break;
  case TURBODEC_INTLV_CONF2:
    dec_res[2].interleaver[0]->set(data >> 24, data & 0xffff, (data >> 16) & 0xff);
    dec_res[2].interleaver[1]->set(data >> 24, data & 0xffff, (data >> 16) & 0xff);
    break;
  case TURBODEC_INTLV_CONF3:
    dec_res[3].interleaver[0]->set(data >> 24, data & 0xffff, (data >> 16) & 0xff);
    dec_res[3].interleaver[1]->set(data >> 24, data & 0xffff, (data >> 16) & 0xff);
    break;
  case TURBODEC_CRC_TYPE:
     conf_regs.p_crc_type = data;
     break;
  case TURBODEC_CRC_SW:
     conf_regs.p_crc_sw = data;
     break;
  default:
    if (daddr >= TURBODEC_BETA_INIT_1 && daddr < TURBODEC_BETA_INIT_2)
      conf_regs.p_beta_initial_value_cb1[(daddr - TURBODEC_BETA_INIT_1) / 4] =
        *(float *)&data;
    else if (daddr >= TURBODEC_BETA_INIT_2 && daddr < TURBODEC_INTLV_CONF0)
      conf_regs.p_beta_initial_value_cb2[(daddr - TURBODEC_BETA_INIT_2) / 4] =
        *(float *)&data;
    else {
      panic("Tried to write Turbo Decoder at offset %#x that doesn't exist\n", daddr);
      break;
    }
  }
  pkt->makeAtomicResponse();
  return copDelay;
}

void TurboDecoder::regStats() {
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

TurboDecoder *TurboDecoderParams::create() {
  return new TurboDecoder(this);
}

