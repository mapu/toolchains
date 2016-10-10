/*============================================================================*/
//  Information
/*============================================================================*/
/* 
 *  @file    TurboDecMaPUSWFloat_sim.h
 *  @brief   
 *  @author  wujunning
 *  @date    2014/03/10
 *----------------------------------------------------------------------------*/
#ifndef TURBODECMAPUSWFLOAT_SIM_H
#define TURBODECMAPUSWFLOAT_SIM_H

/*============================================================================*/
//  Header files
/*============================================================================*/
#include "stdlib.h"
#include "stdio.h"
#include "math.h"
#include "define.h"

/*===========================================================================*/
// Function definition
/*===========================================================================*/

void TurboDecMaPUSWFloat(int *decoded, float *systematic, float *parity1,
		float *parity2);
void max_log_map(int sb_idxm, int sw_idxm, int sw_lenm1, int sw_lenm2, int pro_lenm,
		int odd_evenm, float scaling_factorm, int beta_init_flag);
void compute_extrinsic(float *llr, float *le,
		state_metric alpha, state_metric beta, branch_metric gamma,
		float systematic, float priori, float scaling_factor);
void compute_gamma(branch_metric gamma, float sys, float par
		);
void compute_beta(state_metric beta, state_metric pre_beta, branch_metric gamma);
void compute_alpha(state_metric alpha, state_metric pre_alpha, branch_metric gamma);
void alpha_acs(state_metric alpha, state_metric pre_alpha, float gamma_11,
		float gamma_10);
void beta_acs(state_metric beta, state_metric pre_beta, float gamma_11,
		float gamma_10);
void max_8to1(float *hmax, state_metric input);
void hard_decision(int *out_bits, float *llr, int len);
void compute_inter_inner_idx(int cb_length, int sub_length);
void compute_beta_init(float *systematic, float *parity1, float *parity2,
		int cb_length);
void SPU();
void alpha_initial_sw(int iter_idx);
void beta_initial_sw(int iter_idx);
#endif
