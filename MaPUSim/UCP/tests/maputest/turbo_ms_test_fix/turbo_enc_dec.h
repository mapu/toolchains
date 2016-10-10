#ifndef TURBO_ENC_DEC_H
#define TURBO_ENC_DEC_H

#include "stdlib.h"
#include "math.h"

#include "qpp_interleaver.h"
#include "define.h"
#include "crc.h"


void make_interleaver_table ( int* table, int cb_size );
void constitue_encoder (int* stream_out, int* stream, int length);
void turbo_encoder(int* info_systematic, int* info_parity1, int* info_parity2, int* info_codeblock, int length_cb );

int SPU();
void compute_beta_init(float *systematic, float *parity1, float *parity2, int cb_length,int flag);
void max_log_map_dec1( float *sys_llr, float *parity_llr, float *La_llr, float *alpha_init, float *beta_init, int len, int pu_no, int sw_offset);
void max_log_map_dec2( float *parity_llr, float *La_llr, float *alpha_init, float *beta_init, int len, int pu_no, int sw_offset);
void compute_alpha(state_metric alpha, state_metric pre_alpha, branch_metric gamma);
void alpha_acs(state_metric alpha, state_metric pre_alpha, float gamma_11, float gamma_10);
void compute_beta(state_metric beta, state_metric pre_beta, branch_metric gamma) ;
void beta_acs(state_metric beta, state_metric pre_beta, float gamma_11, float gamma_10);
//void compute_extrinsic(float *llr, float *le, state_metric alpha, state_metric beta, branch_metric gamma, float systematic, float priori, float scaling_factor);
void compute_extrinsic(float *llr, float *le, state_metric alpha, state_metric beta, branch_metric gamma, float sys_la, float scaling_factor);
void max_8to1(float *hmax, state_metric input);

int turbo_decoder(int *decode_bit, float *sys_llr, float *parity1_llr, float *parity2_llr);
void interl_addr_gen(int cb_len, int sb_len);
void compute_inter_inner_idx(int cb_length, int sb_length);
void rd_beta_init(float *beta_init, float *sys_llr, float *parity_llr, float *La_llr, int rd_len);

void test_interl_addr_gen();
#endif 
