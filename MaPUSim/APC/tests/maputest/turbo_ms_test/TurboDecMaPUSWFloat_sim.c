/*============================================================================*/
//  Information
/*============================================================================*/
/* 
 *  @file    TurboDecMaPUSWFloat_sim.c
 *  @brief   
 *  @author  wujunning
 *  @date    2014/03/31
 ----------------------------------------------------------------------------*/
#ifndef TURBODECMAPUSWFLOAT_SIM_C
#define TURBODECMAPUSWFLOAT_SIM_C

/*============================================================================*/
//  Header files
/*============================================================================*/
#include "TurboDecMaPUSWFloat_sim.h"
#include "crcadd.h"
extern const int interleaver_size[188];
extern const int interleaver_f1[188];
extern const int interleaver_f2[188];
extern const int interleaver_par[188][2];
extern const int interleaver_firstper[188][64];
extern const int interleaver_secondper[188][6144];

//global Variables---memory
float input_buffer_sys[128 + 64];
float input_buffer_par[128 + 64];
float input_buffer_pri[128 + 64];

branch_metric gamma_buffer0_sw[128];
branch_metric gamma_buffer1_sw[128];
state_metric beta_buffer0_sw[128];
state_metric beta_buffer1_sw[128];
float systematic_buffer0_sw[128];
float systematic_buffer1_sw[128];
float priori_buffer0_sw[128];
float priori_buffer1_sw[128];

state_metric beta_initial_value_sb1[4],beta_initial_value_sb2[4];
state_metric beta_temp_value_sb1[4],beta_temp_value_sb2[4];
state_metric alpha_initial_value_sb1[4],alpha_initial_value_sb2[4];
state_metric alpha_initial_value_sw1[4],alpha_initial_value_sw2[4];

state_metric beta_initial_value_cb1, beta_initial_value_cb2;//store tail bit Beta initial value
state_metric beta_initial_value_ep = {log1_8, log1_8, log1_8, log1_8, log1_8,
	log1_8, log1_8, log1_8};
state_metric beta_initial_value = { 0, MINVALUE, MINVALUE, MINVALUE, MINVALUE,
MINVALUE, MINVALUE, MINVALUE };


float extrinsic1[4][1536];
float extrinsic0[4][1536];

float soft_bit[4][1536];
int    hard[4][1536];

int inter_bank_idx[4][1536];
int inner_bank_idx[4][1536];

//Global Variables-- Temp Registers
float         t_systematic_prime;
branch_metric t_gamma;
state_metric  t_beta,t_pre_beta;
state_metric  t_alpha,t_pre_alpha;
float         t_le,t_llr;
//Global Variables-- Control Registers
int p_cb_len;
int p_sb_num;
int p_max_iteration_num;
int p_sb_len;
int p_sw_num;
int p_sw_len_1,p_sw_len_2;
int p_pro_len;
double p_scalling_factor;
int p_crc_type;
extern FILE *fp_blk40;
extern FILE *fp_beta_init;
void TurboDecMaPUSWFloat(int *decoded, float *systematic, float *parity1,
		float *parity2) {

	int i,j;
	float temp;
	int iteration_idx, sub_block_idx, sliding_window_idx;
	FILE *fp_log_sw;
	int   crc_ok = 0;
		
	//fp_beta_init = fopen("fp_beta_initial_value.txt","a");
	//SPU configuration
	SPU();
	
	fprintf(fp_beta_init,"p_sw_len_1:%d,p_sw_len_2:%d.\n",p_sw_len_1,p_sw_len_2);
	//calculate interleaver index (inter- or inner- bank)for each sub block
	compute_inter_inner_idx(p_cb_len, p_sb_len);

	//Calculate Initial Beta Value
	compute_beta_init(systematic, parity1, parity2, p_cb_len);

	fprintf(fp_beta_init,"const unsigned int beta_initial_value_cb1[] = {\n");
	for(i=0;i<8;i++){
		temp = beta_initial_value_cb1[i];
		fprintf(fp_beta_init,"  %#010x,\n",*(unsigned int *)&temp);
	}
	fprintf(fp_beta_init,"};\n");
	fprintf(fp_beta_init,"const unsigned int beta_initial_value_cb2[] = {\n");
	for(i=0;i<8;i++){
		temp = beta_initial_value_cb2[i];
		fprintf(fp_beta_init,"  %#010x,\n",*(unsigned int *)&temp);
	}
	fprintf(fp_beta_init,"};\n");
	//fclose(fp_beta_init);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	fp_log_sw = fopen("Extrinsic_Log_sw.log","w+");
	for (iteration_idx = 0; iteration_idx < p_max_iteration_num;
			iteration_idx++) {
			fprintf(fp_blk40,"pre_iter @ Iteration %d\n",iteration_idx);
			alpha_initial_sw(iteration_idx);
			beta_initial_sw(iteration_idx);
			//first half iteration
			fprintf(fp_log_sw,"Interation:%d.\n\n",iteration_idx);
			for (sub_block_idx = 0; sub_block_idx < p_sb_num;
					sub_block_idx++) {
				fprintf(fp_blk40,"pre_iter @ Sub_Block %d @ Iteration %d\n",sub_block_idx,iteration_idx);
				for (sliding_window_idx = 0;
						sliding_window_idx < p_sw_num;
						sliding_window_idx++) {
					fprintf(fp_blk40,"pre_iter @ Sliding Window %d @ Sub_Block %d @ Iteration %d\n",sliding_window_idx,sub_block_idx,iteration_idx);
					if (sliding_window_idx == (p_sw_num - 1)) {
						//prepare the input buffer
						for (i = 0; i < p_sw_len_2; i++) {
							systematic_buffer0_sw[128 - p_sw_len_2 + i] = systematic[sub_block_idx * p_sb_len + sliding_window_idx * p_sw_len_1 + i];
							input_buffer_sys[128 - p_sw_len_2 + i]      = systematic[sub_block_idx * p_sb_len + sliding_window_idx * p_sw_len_1 + i];
							priori_buffer0_sw[128 - p_sw_len_2 + i]     = extrinsic0[sub_block_idx][sliding_window_idx * p_sw_len_1 + i];
							input_buffer_pri[128 - p_sw_len_2 + i]      = extrinsic0[sub_block_idx][sliding_window_idx * p_sw_len_1 + i];
							input_buffer_par[128 - p_sw_len_2 + i]      = parity1[sub_block_idx * p_sb_len + sliding_window_idx * p_sw_len_1 + i];
						} //end of for
						if (sub_block_idx == (p_sb_num - 1))
							max_log_map(sub_block_idx, sliding_window_idx, p_sw_len_1, p_sw_len_2, 0, 0, p_scalling_factor, 1); //the last sliding window of the last sub-block
						else
							max_log_map(sub_block_idx, sliding_window_idx, p_sw_len_1, p_sw_len_2, 0, 0, p_scalling_factor, 3); //the lasr sliding window of the other sub-blocks
					} else {
						//prepare the input buffer
						for (i = 0; i < p_sw_len_1 + p_pro_len; i++) {
							if (i < p_sw_len_1) {
								systematic_buffer0_sw[128 - p_sw_len_1 + i] = systematic[sub_block_idx * p_sb_len + sliding_window_idx * p_sw_len_1 + i];
								input_buffer_sys[128 - p_sw_len_1 + i]      = systematic[sub_block_idx * p_sb_len + sliding_window_idx * p_sw_len_1 + i];
								priori_buffer0_sw[128 - p_sw_len_1 + i]     = extrinsic0[sub_block_idx][sliding_window_idx * p_sw_len_1 + i];
								input_buffer_pri[128 - p_sw_len_1 + i]      = extrinsic0[sub_block_idx][sliding_window_idx * p_sw_len_1 + i];
								input_buffer_par[128 - p_sw_len_1 + i]      = parity1[sub_block_idx * p_sb_len + sliding_window_idx * p_sw_len_1 + i];
							} else {
								input_buffer_sys[128 + 64 - p_pro_len - p_sw_len_1 + i] = systematic[sub_block_idx * p_sb_len + sliding_window_idx * p_sw_len_1 + i];
								input_buffer_pri[128 + 64 - p_pro_len - p_sw_len_1 + i] = extrinsic0[sub_block_idx][sliding_window_idx * p_sw_len_1 + i];
								input_buffer_par[128 + 64 - p_pro_len - p_sw_len_1 + i] = parity1[sub_block_idx * p_sb_len + sliding_window_idx * p_sw_len_1 + i];
							} //end of if
						} //end of for
						max_log_map(sub_block_idx, sliding_window_idx, p_sw_len_1, p_sw_len_2, p_pro_len, 0, p_scalling_factor, 0);
					}					//end of if
				}					//end of for
			}					//end of first half interation

			//crc check
			if(CRC_SW == 1){
				for (i = 0; i < p_cb_len; i++) {
					if (i < p_sb_len)
						*(decoded + i) = hard[0][i];
					else if (i < 2 * p_sb_len)
						*(decoded + i) = hard[1][i - p_sb_len];
					else if (i < 3 * p_sb_len)
						*(decoded + i) = hard[2][i - 2 * p_sb_len];
					else
						*(decoded + i) = hard[3][i - 3 * p_sb_len];
				}
				crc_ok += crcdec(decoded,p_cb_len,p_crc_type);
				if(crc_ok == 1){
					printf("TotalNUM:%d.InterationNum:%d.\n",p_max_iteration_num,iteration_idx+1);
					break;
				}
			}

			fprintf(fp_blk40,"post_iter @ Iteration %d\n",iteration_idx);
			//second half iteration
			for (sub_block_idx = 0; sub_block_idx < p_sb_num;
					sub_block_idx++) {
				fprintf(fp_blk40,"post_iter @ Sub_Block %d @ Iteration %d\n",sub_block_idx,iteration_idx);
				for (sliding_window_idx = 0;
						sliding_window_idx < p_sw_num;
						sliding_window_idx++) {
					fprintf(fp_blk40,"post_iter @ Sliding Window %d @ Sub_Block %d @ Iteration %d\n",sliding_window_idx,sub_block_idx,iteration_idx);
					if (sliding_window_idx == (p_sw_num - 1)) {
						//prepare the input buffer
						for (i = 0; i < p_sw_len_2; i++) {
							priori_buffer1_sw[128 - p_sw_len_2 + i] = extrinsic1[inter_bank_idx[sub_block_idx][sliding_window_idx*p_sw_len_1 + i]][inner_bank_idx[sub_block_idx][sliding_window_idx*p_sw_len_1 + i]];
							input_buffer_pri[128 - p_sw_len_2 + i]  = extrinsic1[inter_bank_idx[sub_block_idx][sliding_window_idx*p_sw_len_1 + i]][inner_bank_idx[sub_block_idx][sliding_window_idx*p_sw_len_1 + i]];
							input_buffer_par[128 - p_sw_len_2 + i]  = parity2[sub_block_idx * p_sb_len + sliding_window_idx * p_sw_len_1 + i];
							//fprintf(fp_blk40,"cid:%d,offset:%d\n",inter_bank_idx[sub_block_idx][sliding_window_idx*p_sw_len_1 + i],inner_bank_idx[sub_block_idx][sliding_window_idx*p_sw_len_1 + i]);
						}		//end of for
						if (sub_block_idx == (p_sb_num - 1))
							max_log_map(sub_block_idx, sliding_window_idx, p_sw_len_1, p_sw_len_2, 0, 1, p_scalling_factor, 2);
						else
							max_log_map(sub_block_idx, sliding_window_idx, p_sw_len_1, p_sw_len_2, 0, 1, p_scalling_factor, 3);
					} else {
						//prepare the input buffer
						for (i = 0; i < p_sw_len_1 + p_pro_len; i++) {
							if (i < p_sw_len_1) {
								priori_buffer1_sw[128 - p_sw_len_1 + i] = extrinsic1[inter_bank_idx[sub_block_idx][sliding_window_idx*p_sw_len_1 + i]][inner_bank_idx[sub_block_idx][sliding_window_idx*p_sw_len_1 + i]];
								input_buffer_pri[128 - p_sw_len_1 + i]  = extrinsic1[inter_bank_idx[sub_block_idx][sliding_window_idx*p_sw_len_1 + i]][inner_bank_idx[sub_block_idx][sliding_window_idx*p_sw_len_1 + i]];
								input_buffer_par[128 - p_sw_len_1 + i]  = parity2[sub_block_idx * p_sb_len + sliding_window_idx * p_sw_len_1 + i];
								//fprintf(fp_blk40,"cid:%d,offset:%d\n",inter_bank_idx[sub_block_idx][sliding_window_idx*p_sw_len_1 + i],inner_bank_idx[sub_block_idx][sliding_window_idx*p_sw_len_1 + i]);
							} else {
								input_buffer_pri[128 + 64 - p_pro_len - p_sw_len_1 + i] = extrinsic1[inter_bank_idx[sub_block_idx][sliding_window_idx*p_sw_len_1 + i]][inner_bank_idx[sub_block_idx][sliding_window_idx*p_sw_len_1 + i]];
								input_buffer_par[128 + 64 - p_pro_len - p_sw_len_1 + i] = parity2[sub_block_idx * p_sb_len + sliding_window_idx * p_sw_len_1 + i];
								//fprintf(fp_blk40,"cid:%d,offset:%d\n",inter_bank_idx[sub_block_idx][sliding_window_idx*p_sw_len_1 + i],inner_bank_idx[sub_block_idx][sliding_window_idx*p_sw_len_1 + i]);
							}		//end of if
						}		//end of for
						max_log_map(sub_block_idx, sliding_window_idx, p_sw_len_1, p_sw_len_2, p_pro_len, 1, p_scalling_factor, 0);
					}		//end of if
				}		//end of for
			}		//end of second half interation

			for(i=0;i<p_sb_len;i++){
				fprintf(fp_log_sw,"%4d::Pri:%10.6f,Pri+Sys1:%10.6f,Sys1:%10.6f..\n",i,extrinsic0[0][i]-*(systematic+i),extrinsic0[0][i],*(systematic+i));
			}
			for(i=0;i<p_sb_len;i++){
				fprintf(fp_log_sw,"%4d::Pri:%10.6f,Pri+Sys2:%10.6f,Sys2:%10.6f..\n",i,extrinsic1[0][i],extrinsic1[0][i]+*(systematic+i),*(systematic+i));
			}
	}		//end of Interation Decode
	fclose(fp_log_sw);
	//output
	if(CRC_SW == 0){
		for (i = 0; i < p_cb_len; i++) {
			if (i < p_sb_len)
				*(decoded + i) = hard[0][i];
			else if (i < 2 * p_sb_len)
				*(decoded + i) = hard[1][i - p_sb_len];
			else if (i < 3 * p_sb_len)
				*(decoded + i) = hard[2][i - 2 * p_sb_len];
			else
				*(decoded + i) = hard[3][i - 3 * p_sb_len];
		}
	}
}

void max_log_map(int sb_idxm, int sw_idxm, int sw_lenm1, int sw_lenm2, int pro_lenm,
		int odd_evenm, float scaling_factorm, int beta_init_flag) {

	int i,j;
	int sw_pro_idx;
	//FILE *fp_blk40;
	//fp_blk40 = fopen("BLK128_LOG.log","a");
	if (odd_evenm == 0) {
		if(beta_init_flag == 1){ //the last sb,the last sw
			for(i=0;i<8;i++){//initialization of beta
				t_pre_beta[i] = beta_initial_value_cb1[i];
				beta_buffer0_sw[128 - 1][i] = t_pre_beta[i];
			}
			t_systematic_prime = input_buffer_sys[128 - 1] + input_buffer_pri[128 - 1];
			compute_gamma(t_gamma,t_systematic_prime,input_buffer_par[128 - 1]);//compute gamma
			gamma_buffer0_sw[128 - 1][0] = t_gamma[0];
			gamma_buffer0_sw[128 - 1][1] = t_gamma[1];
			//printf("sys=%10.6f,par=%10.6f.gamma10:%10.6f,gamma11:%10.6f.\n",t_systematic_prime,input_buffer_par[128 - 1],gamma_buffer0_sw[128 - 1 ][0],gamma_buffer0_sw[128 - 1][1]);
			fprintf(fp_blk40,"sys=%10.6f,pri=%10.6f,sys+pri=%10.6f,par=%10.6f.gamma10:%10.6f,gamma11:%10.6f.\n",input_buffer_sys[128 - 1],input_buffer_pri[128 - 1],t_systematic_prime,input_buffer_par[128 - 1],gamma_buffer0_sw[128 - 1 ][0],gamma_buffer0_sw[128 - 1][1]);
			for (j = 0; j < 8; j++) {
				//printf("Beta[%d]:%10.6f  .",j,t_beta[j]);
				fprintf(fp_blk40,"Beta[%d]:%10.6f  .",j,t_pre_beta[j]);
			}
			//printf("\n");
			fprintf(fp_blk40,"\n");
			for(i=126; i>127-sw_lenm2; i--){
				t_systematic_prime = input_buffer_sys[i] + input_buffer_pri[i];
				compute_gamma(t_gamma,t_systematic_prime,input_buffer_par[i]);//compute gamma
				gamma_buffer0_sw[i][0] = t_gamma[0];
				gamma_buffer0_sw[i][1] = t_gamma[1];
				//printf("sys=%10.6f,par=%10.6f.gamma10:%10.6f,gamma11:%10.6f.\n",t_systematic_prime,input_buffer_par[i],gamma_buffer0_sw[i][0],gamma_buffer0_sw[i][1]);
				fprintf(fp_blk40,"sys=%10.6f,pri=%10.6f,sys+pri=%10.6f,par=%10.6f.gamma10:%10.6f,gamma11:%10.6f.\n",input_buffer_sys[i],input_buffer_pri[i],t_systematic_prime,input_buffer_par[i],gamma_buffer0_sw[i][0],gamma_buffer0_sw[i][1]);
				compute_beta(t_beta,t_pre_beta,gamma_buffer0_sw[i+1]);//compute beta
				for (j = 0; j < 8; j++) {
					beta_buffer0_sw[i][j] = t_beta[j];
					//printf("Beta[%d]:%10.6f  .",j,t_beta[j]);
					fprintf(fp_blk40,"Beta[%d]:%10.6f  .",j,t_beta[j]);
					t_pre_beta[j]         = t_beta[j];
				}
				//printf("\n");
				fprintf(fp_blk40,"\n");
			}//end of computing gamma and beta
			for(i=0;i<8;i++){//initialization of alpha
				t_pre_alpha[i] = alpha_initial_value_sw1[sb_idxm][i];
				fprintf(fp_blk40,"Alpha[%d]:%10.6f  .",i,t_pre_alpha[i]);
			}
			fprintf(fp_blk40,"\n");
			for(i=0;i<sw_lenm2;i++){
				compute_alpha(t_alpha,t_pre_alpha,gamma_buffer0_sw[128 - sw_lenm2 + i]);
				compute_extrinsic(&t_llr, &t_le, t_pre_alpha, beta_buffer0_sw[128 - sw_lenm2 + i], gamma_buffer0_sw[128 - sw_lenm2 + i], systematic_buffer0_sw[128 - sw_lenm2+ i], priori_buffer0_sw[128 - sw_lenm2 + i], scaling_factorm);
				for (j = 0; j < 8; j++) {
					//printf("Beta[%d]:%10.6f  .",j,t_beta[j]);
					fprintf(fp_blk40,"Beta[%d]:%10.6f  .",j,beta_buffer0_sw[128 - sw_lenm2 + i][j]);
				}
				fprintf(fp_blk40,"\n");
				for(j=0;j<8;j++){
					t_pre_alpha[j] = t_alpha[j];
					//printf("Alpha[%d]:%10.6f  .",j,t_alpha[j]);
					fprintf(fp_blk40,"Alpha[%d]:%10.6f  .",j,t_alpha[j]);
				}
				fprintf(fp_blk40,"\n");
				extrinsic1[sb_idxm][sw_idxm * sw_lenm1 + i] = t_le + systematic_buffer0_sw[128 - sw_lenm2 + i];
				//printf("%d:%f,%f.\n",i,systematic_buffer0_sw[128 - sw_lenm + i],t_le);
				soft_bit[sb_idxm][sw_idxm * sw_lenm1 + i] = t_llr;
				//printf("Beta_init_flag=%d,Soft= %10.6f.Extrinsic:%10.6f,Extrinsic+sys:%10.6f\n",beta_init_flag,t_llr,t_le,t_le + systematic_buffer0_sw[128 - sw_lenm + i]);
				fprintf(fp_blk40,"Beta_init_flag=%d,Soft= %10.6f.Extrinsic:%10.6f,Extrinsic+sys:%10.6f\n",beta_init_flag,t_llr,t_le,t_le + systematic_buffer0_sw[128 - sw_lenm2 + i]);
				hard[sb_idxm][sw_idxm * sw_lenm1 + i] = (t_llr > 0) ? 0 : 1;
			}
			for(j=0;j<8;j++){
				alpha_initial_value_sw1[sb_idxm][j] = t_alpha[j];
				fprintf(fp_blk40,"Alpha[%d]:%10.6f  .",j,t_alpha[j]);
			}
			fprintf(fp_blk40,"\n");
		}else if(beta_init_flag == 3){    //end of if(beta_init_flag == 1) 
			for(i=0;i<8;i++){//initialization of beta
				t_pre_beta[i] = beta_initial_value_sb1[sb_idxm][i];
				beta_buffer0_sw[128 - 1][i] = t_pre_beta[i];
			}
			t_systematic_prime = input_buffer_sys[128 - 1] + input_buffer_pri[128 - 1];
			compute_gamma(t_gamma,t_systematic_prime,input_buffer_par[128 - 1]);//compute gamma
			gamma_buffer0_sw[128 - 1][0] = t_gamma[0];
			gamma_buffer0_sw[128 - 1][1] = t_gamma[1];
			fprintf(fp_blk40,"sys=%10.6f,pri=%10.6f,sys+pri=%10.6f,par=%10.6f.gamma10:%10.6f,gamma11:%10.6f.\n",input_buffer_sys[128 - 1],input_buffer_pri[128 - 1],t_systematic_prime,input_buffer_par[128 - 1],gamma_buffer0_sw[128 - 1 ][0],gamma_buffer0_sw[128 - 1][1]);
			for (j = 0; j < 8; j++) {
				//printf("Beta[%d]:%10.6f  .",j,t_beta[j]);
				fprintf(fp_blk40,"Beta[%d]:%10.6f  .",j,t_pre_beta[j]);
			}
			//printf("\n");
			fprintf(fp_blk40,"\n");
			for(i=126; i>127-sw_lenm2; i--){
				t_systematic_prime = input_buffer_sys[i] + input_buffer_pri[i];
				compute_gamma(t_gamma,t_systematic_prime,input_buffer_par[i]);//compute gamma
				gamma_buffer0_sw[i][0] = t_gamma[0];
				gamma_buffer0_sw[i][1] = t_gamma[1];
				//printf("sys=%10.2f,par=%10.2f.gamma10:%10.2f,gamma11:%10.2f.\n",t_systematic_prime,input_buffer_par[128 - 2 - i],gamma_buffer0_sw[128 - 1 - i][0],gamma_buffer0_sw[128 - 1 - i][1]);
				fprintf(fp_blk40,"sys=%10.6f,pri=%10.6f,sys+pri=%10.6f,par=%10.6f.gamma10:%10.6f,gamma11:%10.6f.\n",input_buffer_sys[i],input_buffer_pri[i],t_systematic_prime,input_buffer_par[i],gamma_buffer0_sw[i][0],gamma_buffer0_sw[i][1]);
				compute_beta(t_beta,t_pre_beta,gamma_buffer0_sw[i+1]);//compute beta
				for (j = 0; j < 8; j++) {
					beta_buffer0_sw[i][j] = t_beta[j];
					fprintf(fp_blk40,"Beta[%d]:%10.6f  .",j,t_beta[j]);
					t_pre_beta[j]         = t_beta[j];
				}
				fprintf(fp_blk40,"\n");
			}//end of computing gamma and beta
			for(i=0;i<8;i++){//initialization of alpha
				t_pre_alpha[i] = alpha_initial_value_sw1[sb_idxm][i];
				fprintf(fp_blk40,"Alpha[%d]:%10.6f  .",i,t_pre_alpha[i]);
			}
			fprintf(fp_blk40,"\n");
			for(i=0;i<sw_lenm2;i++){
				compute_alpha(t_alpha,t_pre_alpha,gamma_buffer0_sw[128 - sw_lenm2 + i]);
				compute_extrinsic(&t_llr, &t_le, t_pre_alpha, beta_buffer0_sw[128 - sw_lenm2 + i], gamma_buffer0_sw[128 - sw_lenm2 + i], systematic_buffer0_sw[128 - sw_lenm2 + i], priori_buffer0_sw[128 - sw_lenm2 + i], scaling_factorm);
				for (j = 0; j < 8; j++) {
					//printf("Beta[%d]:%10.6f  .",j,t_beta[j]);
					fprintf(fp_blk40,"Beta[%d]:%10.6f  .",j,beta_buffer0_sw[128 - sw_lenm2 + i][j]);
				}
				fprintf(fp_blk40,"\n");
				for(j=0;j<8;j++){
					t_pre_alpha[j] = t_alpha[j];
					//printf("Alpha[%d]:%10.6f  .",j,t_alpha[j]);
					fprintf(fp_blk40,"Alpha[%d]:%10.6f  .",j,t_alpha[j]);
				}
				fprintf(fp_blk40,"\n");
				extrinsic1[sb_idxm][sw_idxm * sw_lenm1 + i] = t_le + systematic_buffer0_sw[128 - sw_lenm2 + i];
				soft_bit[sb_idxm][sw_idxm * sw_lenm1 + i] = t_llr;
				//printf("Beta_init_flag=%d,Soft= %f.\n",beta_init_flag,t_llr);
				fprintf(fp_blk40,"Beta_init_flag=%d,Soft= %10.6f.Extrinsic:%10.6f,Extrinsic+sys:%10.6f\n",beta_init_flag,t_llr,t_le,t_le + systematic_buffer0_sw[128 - sw_lenm2 + i]);
				hard[sb_idxm][sw_idxm * sw_lenm1 + i] = (t_llr > 0) ? 0 : 1;
			}
			for(j=0;j<8;j++){
				alpha_initial_value_sw1[sb_idxm][j] = t_alpha[j];
				fprintf(fp_blk40,"Alpha[%d]:%10.6f  .",j,t_alpha[j]);
			}
			fprintf(fp_blk40,"\n");
		}else if(beta_init_flag == 0){
			for (i = 0; i < 8; i++) {//initialize the beta_temp according to the beta_initial_value_ep
				t_pre_beta[i] = beta_initial_value_ep[i];
			}
			for (sw_pro_idx = 0; sw_pro_idx < sw_lenm1 + pro_lenm; sw_pro_idx++) {
				if ((pro_lenm != 0) && (sw_pro_idx < pro_lenm)) {
					t_systematic_prime = input_buffer_sys[128 + 64 - 1 - sw_pro_idx] + input_buffer_pri[128 + 64 - 1 - sw_pro_idx];
					compute_gamma(t_gamma,t_systematic_prime,input_buffer_par[128 + 64 - 1 - sw_pro_idx]);
					compute_beta(t_beta,t_pre_beta,t_gamma);
					fprintf(fp_blk40,"sys=%10.6f,pri=%10.6f,sys+pri=%10.6f,par=%10.6f.gamma10:%10.6f,gamma11:%10.6f.\n",input_buffer_sys[128 + 64 - 1 - sw_pro_idx],input_buffer_pri[128 + 64 - 1 - sw_pro_idx],t_systematic_prime,input_buffer_par[128 + 64 - 1 - sw_pro_idx],t_gamma[0],t_gamma[1]);
					for (j = 0; j < 8; j++) {
						//printf("Beta[%d]:%10.6f  .",j,t_beta[j]);
						fprintf(fp_blk40,"Beta[%d]:%10.6f  .",j,t_pre_beta[j]);
					}
					//printf("\n");
					fprintf(fp_blk40,"\n");
					for (j = 0; j < 8; j++) {
						t_pre_beta[j] = t_beta[j];
					}
				} else {
					for (j = 0; j < 8; j++) {
						beta_buffer0_sw[128 + pro_lenm - 1 - sw_pro_idx][j] = t_pre_beta[j];
					}
					t_systematic_prime = input_buffer_sys[128 + pro_lenm - 1 - sw_pro_idx] + input_buffer_pri[128 + pro_lenm - 1 - sw_pro_idx];
					compute_gamma(t_gamma,t_systematic_prime,input_buffer_par[128 + pro_lenm - 1 - sw_pro_idx]);
					gamma_buffer0_sw[128 + pro_lenm - 1 - sw_pro_idx][0] = t_gamma[0];
					gamma_buffer0_sw[128 + pro_lenm - 1 - sw_pro_idx][1] = t_gamma[1];
					//fprintf(fp_blk40,"sys=%10.6f,pri=%10.6f,sys+pri=%10.6f,par=%10.6f.gamma10:%10.6f,gamma11:%10.6f.\n",input_buffer_sys[128 - 1],input_buffer_pri[128 - 1],t_systematic_prime,input_buffer_par[128 - 1],gamma_buffer0_sw[128 - 1 ][0],gamma_buffer0_sw[128 - 1][1]);
					//printf("sys=%10.2f,par=%10.2f.gamma10:%10.2f,gamma11:%10.2f.\n",t_systematic_prime,input_buffer_par[128 + pro_lenm - 1 - sw_pro_idx],gamma_buffer0_sw[128 + pro_lenm - 1 - sw_pro_idx][0],gamma_buffer0_sw[128 + pro_lenm - 1 - sw_pro_idx][1]);
					//printf("sys=%10.6f,par=%10.6f.gamma10:%10.6f,gamma11:%10.6f.\n",t_systematic_prime,input_buffer_par[128 - 1],gamma_buffer0_sw[128 - 1 ][0],gamma_buffer0_sw[128 - 1][1]);
					fprintf(fp_blk40,"sys=%10.6f,pri=%10.6f,sys+pri=%10.6f,par=%10.6f.gamma10:%10.6f,gamma11:%10.6f.\n",input_buffer_sys[128 + pro_lenm - 1 - sw_pro_idx],input_buffer_pri[128 + pro_lenm - 1 - sw_pro_idx],t_systematic_prime,input_buffer_par[128 + pro_lenm - 1 - sw_pro_idx],gamma_buffer0_sw[128 + pro_lenm - 1 - sw_pro_idx][0],gamma_buffer0_sw[128 + pro_lenm - 1 - sw_pro_idx][1]);
					for (j = 0; j < 8; j++) {
						//printf("Beta[%d]:%10.6f  .",j,t_beta[j]);
						fprintf(fp_blk40,"Beta[%d]:%10.6f  .",j,t_pre_beta[j]);
					}
					//printf("\n");
					fprintf(fp_blk40,"\n");
					compute_beta(t_beta,t_pre_beta,t_gamma);
					for (j = 0; j < 8; j++) {
						if((128 + pro_lenm - 2 - sw_pro_idx) >= 0){
							beta_buffer0_sw[128 + pro_lenm - 2 - sw_pro_idx][j] = t_beta[j];
							t_pre_beta[j] = t_beta[j];
						}
					}
				}//end of if
			}//end of computing gamma and beta
			if(sw_idxm == 0){
				for (j = 0; j < 8; j++) {
					beta_temp_value_sb1[sb_idxm][j] = t_pre_beta[j];
				}
			}
			for(i=0;i<8;i++){//initialization of alpha
				t_pre_alpha[i] = alpha_initial_value_sw1[sb_idxm][i];
				fprintf(fp_blk40,"Alpha[%d]:%10.6f  .",i,t_pre_alpha[i]);
			}
			fprintf(fp_blk40,"\n");
			for(i=0;i<sw_lenm1;i++){
				compute_alpha(t_alpha,t_pre_alpha,gamma_buffer0_sw[128 - sw_lenm1+ i]);
				compute_extrinsic(&t_llr, &t_le, t_pre_alpha, beta_buffer0_sw[128 - sw_lenm1 + i], gamma_buffer0_sw[128 - sw_lenm1 + i], systematic_buffer0_sw[128 - sw_lenm1 + i], priori_buffer0_sw[128 - sw_lenm1 + i], scaling_factorm);
				for (j = 0; j < 8; j++) {
					//printf("Beta[%d]:%10.6f  .",j,t_beta[j]);
					fprintf(fp_blk40,"Beta[%d]:%10.6f  .",j,beta_buffer0_sw[128 - sw_lenm1 + i][j]);
				}
				fprintf(fp_blk40,"\n");
				for(j=0;j<8;j++){
					t_pre_alpha[j] = t_alpha[j];
					fprintf(fp_blk40,"Alpha[%d]:%10.6f  .",j,t_alpha[j]);
				}
				fprintf(fp_blk40,"\n");
				extrinsic1[sb_idxm][sw_idxm * sw_lenm1 + i] = t_le + systematic_buffer0_sw[128 - sw_lenm1 + i];
				soft_bit[sb_idxm][sw_idxm * sw_lenm1 + i] = t_llr;
				fprintf(fp_blk40,"Beta_init_flag=%d,Soft= %10.6f.Extrinsic:%10.6f,Extrinsic+sys:%10.6f\n",beta_init_flag,t_llr,t_le,t_le + systematic_buffer0_sw[128 - sw_lenm1 + i]);
				//printf("Beta_init_flag=%d,Soft= %f.\n",beta_init_flag,t_llr);
				hard[sb_idxm][sw_idxm * sw_lenm1 + i] = (t_llr > 0) ? 0 : 1;
			}
			for(j=0;j<8;j++){
				alpha_initial_value_sw1[sb_idxm][j] = t_alpha[j];
				fprintf(fp_blk40,"Alpha[%d]:%10.6f  .",j,t_alpha[j]);
			}
			fprintf(fp_blk40,"\n");
		}
	} else {// odd_evenm == 1
		if(beta_init_flag == 2){ //the last sb,the last sw
			for(i=0;i<8;i++){//initialization of beta
				t_pre_beta[i] = beta_initial_value_cb2[i];
				beta_buffer1_sw[128 - 1][i] = t_pre_beta[i];
			}
			//t_systematic_prime = input_buffer_sys[128 - 1] + input_buffer_pri[128 - 1];
			t_systematic_prime = input_buffer_pri[128 - 1];
			compute_gamma(t_gamma,t_systematic_prime,input_buffer_par[128 - 1]);//compute gamma
			gamma_buffer1_sw[128 - 1][0] = t_gamma[0];
			gamma_buffer1_sw[128 - 1][1] = t_gamma[1];
			//printf("1:pri=%10.6f,par=%10.6f.gamma10:%10.6f,gamma11:%10.6f.\n",t_systematic_prime,input_buffer_par[128 - 1],gamma_buffer1_sw[128 - 1 ][0],gamma_buffer1_sw[128 - 1][1]);
			//printf("sys=%10.6f,par=%10.6f.gamma10:%10.6f,gamma11:%10.6f.\n",t_systematic_prime,input_buffer_par[128 - 1],gamma_buffer0_sw[128 - 1 ][0],gamma_buffer0_sw[128 - 1][1]);
			fprintf(fp_blk40,"pri=%10.6f,par=%10.6f.gamma10:%10.6f,gamma11:%10.6f.\n",input_buffer_pri[128 - 1],input_buffer_par[128 - 1],gamma_buffer1_sw[128 - 1 ][0],gamma_buffer1_sw[128 - 1][1]);
			for (j = 0; j < 8; j++) {
				//printf("Beta[%d]:%10.6f  .",j,t_beta[j]);
				fprintf(fp_blk40,"Beta[%d]:%10.6f  .",j,t_pre_beta[j]);
			}
			//printf("\n");
			fprintf(fp_blk40,"\n");
			for(i=126; i>127-sw_lenm2; i--){
				//t_systematic_prime = input_buffer_sys[128 - 2 - i] + input_buffer_pri[128 - 2 - i];
				t_systematic_prime = input_buffer_pri[i];
				compute_gamma(t_gamma,t_systematic_prime,input_buffer_par[i]);//compute gamma
				gamma_buffer1_sw[i][0] = t_gamma[0];
				gamma_buffer1_sw[i][1] = t_gamma[1];
				fprintf(fp_blk40,"pri=%10.6f,par=%10.6f.gamma10:%10.6f,gamma11:%10.6f.\n",input_buffer_pri[i],input_buffer_par[i],gamma_buffer1_sw[i][0],gamma_buffer1_sw[i][1]);
				compute_beta(t_beta,t_pre_beta,gamma_buffer1_sw[i+1]);//compute beta
				for (j = 0; j < 8; j++) {
					beta_buffer1_sw[i][j] = t_beta[j];
					fprintf(fp_blk40,"Beta[%d]:%10.6f  .",j,t_beta[j]);
					t_pre_beta[j]         = t_beta[j];
				}
				fprintf(fp_blk40,"\n");
			}//end of computing gamma and beta
			for(i=0;i<8;i++){//initialization of alpha
				t_pre_alpha[i] = alpha_initial_value_sw2[sb_idxm][i];
				fprintf(fp_blk40,"Alpha[%d]:%10.6f  .",i,t_pre_alpha[i]);
			}
			fprintf(fp_blk40,"\n");
			for(i=0;i<sw_lenm2;i++){
				compute_alpha(t_alpha,t_pre_alpha,gamma_buffer1_sw[128 - sw_lenm2 + i]);
				compute_extrinsic(&t_llr, &t_le, t_pre_alpha, beta_buffer1_sw[128 - sw_lenm2 + i], gamma_buffer1_sw[128 - sw_lenm2 + i], 0, priori_buffer1_sw[128 - sw_lenm2 + i], scaling_factorm);
				for(j=0;j<8;j++){
					t_pre_alpha[j] = t_alpha[j];
				}
				for (j = 0; j < 8; j++) {
					//printf("Beta[%d]:%10.6f  .",j,t_beta[j]);
					fprintf(fp_blk40,"Beta[%d]:%10.6f  .",j,beta_buffer1_sw[128 - sw_lenm2 + i][j]);
				}
				fprintf(fp_blk40,"\n");
				for(j=0;j<8;j++){
					t_pre_alpha[j] = t_alpha[j];
					//printf("Alpha[%d]:%10.6f  .",j,t_alpha[j]);
					fprintf(fp_blk40,"Alpha[%d]:%10.6f  .",j,t_alpha[j]);
				}
				fprintf(fp_blk40,"\n");
				extrinsic0[inter_bank_idx[sb_idxm][sw_idxm*sw_lenm1 + i]][inner_bank_idx[sb_idxm][sw_idxm*sw_lenm1 + i]] = t_le;
				fprintf(fp_blk40,"Beta_init_flag=%d,Soft= %10.6f.Extrinsic:%10.6f,inter_idx:%4d,inner_idx:%4d..\n",beta_init_flag,t_llr,t_le,inter_bank_idx[sb_idxm][sw_idxm*sw_lenm1 + i],inner_bank_idx[sb_idxm][sw_idxm*sw_lenm1 + i]);
			}
			for(j=0;j<8;j++){
				alpha_initial_value_sw2[sb_idxm][j] = t_alpha[j];
				fprintf(fp_blk40,"Alpha[%d]:%10.6f  .",j,t_alpha[j]);
			}
			fprintf(fp_blk40,"\n");
		}else if(beta_init_flag == 3){    //end of if(beta_init_flag == 1) 
			for(i=0;i<8;i++){//initialization of beta
				t_pre_beta[i] = beta_initial_value_sb2[sb_idxm][i];
				beta_buffer1_sw[128 - 1][i] = t_pre_beta[i];
			}
			t_systematic_prime = input_buffer_pri[128 - 1];
			compute_gamma(t_gamma,t_systematic_prime,input_buffer_par[128 - 1]);//compute gamma
			gamma_buffer1_sw[128 - 1][0] = t_gamma[0];
			gamma_buffer1_sw[128 - 1][1] = t_gamma[1];
			fprintf(fp_blk40,"pri=%10.6f,par=%10.6f.gamma10:%10.6f,gamma11:%10.6f.\n",input_buffer_pri[128 - 1],input_buffer_par[128 - 1],gamma_buffer1_sw[128 - 1 ][0],gamma_buffer1_sw[128 - 1][1]);
			for (j = 0; j < 8; j++) {
				//printf("Beta[%d]:%10.6f  .",j,t_beta[j]);
				fprintf(fp_blk40,"Beta[%d]:%10.6f  .",j,t_pre_beta[j]);
			}
			//printf("\n");
			fprintf(fp_blk40,"\n");
			for(i=126; i>127-sw_lenm2; i--){
				t_systematic_prime = input_buffer_pri[i];
				compute_gamma(t_gamma,t_systematic_prime,input_buffer_par[i]);//compute gamma
				gamma_buffer1_sw[i][0] = t_gamma[0];
				gamma_buffer1_sw[i][1] = t_gamma[1];
				//printf("sys=%10.2f,par=%10.2f.gamma10:%10.2f,gamma11:%10.2f.\n",t_systematic_prime,input_buffer_par[128 - 2 - i],gamma_buffer1_sw[128 - 1 - i][0],gamma_buffer1_sw[128 - 1 - i][1]);
				fprintf(fp_blk40,"pri=%10.6f,par=%10.6f.gamma10:%10.6f,gamma11:%10.6f.\n",input_buffer_pri[i],input_buffer_par[i],gamma_buffer1_sw[i][0],gamma_buffer1_sw[i][1]);
				compute_beta(t_beta,t_pre_beta,gamma_buffer1_sw[i+1]);//compute beta
				for (j = 0; j < 8; j++) {
					beta_buffer1_sw[i][j] = t_beta[j];
					fprintf(fp_blk40,"Beta[%d]:%10.6f  .",j,t_beta[j]);
					t_pre_beta[j]         = t_beta[j];
				}
				fprintf(fp_blk40,"\n");
			}//end of computing gamma and beta
			for(i=0;i<8;i++){//initialization of alpha
				t_pre_alpha[i] = alpha_initial_value_sw2[sb_idxm][i];
				fprintf(fp_blk40,"Alpha[%d]:%10.6f  .",i,t_pre_alpha[i]);
			}
			fprintf(fp_blk40,"\n");
			for(i=0;i<sw_lenm2;i++){
				compute_alpha(t_alpha,t_pre_alpha,gamma_buffer1_sw[128 - sw_lenm2 + i]);
				compute_extrinsic(&t_llr, &t_le, t_pre_alpha, beta_buffer1_sw[128 - sw_lenm2 + i], gamma_buffer1_sw[128 - sw_lenm2 + i], 0, priori_buffer1_sw[128 - sw_lenm2 + i], scaling_factorm);
				for (j = 0; j < 8; j++) {
					//printf("Beta[%d]:%10.6f  .",j,t_beta[j]);
					fprintf(fp_blk40,"Beta[%d]:%10.6f  .",j,beta_buffer1_sw[128 - sw_lenm2 + i][j]);
				}
				fprintf(fp_blk40,"\n");
				for(j=0;j<8;j++){
					t_pre_alpha[j] = t_alpha[j];
					//printf("Alpha[%d]:%10.6f  .",j,t_alpha[j]);
					fprintf(fp_blk40,"Alpha[%d]:%10.6f  .",j,t_alpha[j]);
				}
				fprintf(fp_blk40,"\n");
				extrinsic0[inter_bank_idx[sb_idxm][sw_idxm*sw_lenm1 + i]][inner_bank_idx[sb_idxm][sw_idxm*sw_lenm1 + i]] = t_le;
				fprintf(fp_blk40,"Beta_init_flag=%d,Soft= %10.6f.Extrinsic:%10.6f,inter_idx:%4d,inner_idx:%4d..\n",beta_init_flag,t_llr,t_le,inter_bank_idx[sb_idxm][sw_idxm*sw_lenm1 + i],inner_bank_idx[sb_idxm][sw_idxm*sw_lenm1 + i]);
			}
			for(j=0;j<8;j++){
				alpha_initial_value_sw2[sb_idxm][j] = t_alpha[j];
				fprintf(fp_blk40,"Alpha[%d]:%10.6f  .",j,t_alpha[j]);
			}
			fprintf(fp_blk40,"\n");
		}else if(beta_init_flag == 0){
			for (i = 0; i < 8; i++) {//initialize the beta_temp according to the beta_initial_value_ep
				t_pre_beta[i] = beta_initial_value_ep[i];
			}
			for (sw_pro_idx = 0; sw_pro_idx < sw_lenm1 + pro_lenm; sw_pro_idx++) {
				if ((pro_lenm != 0) && (sw_pro_idx < pro_lenm)) {
					t_systematic_prime = input_buffer_pri[128 + 64 - 1 - sw_pro_idx];
					compute_gamma(t_gamma,t_systematic_prime,input_buffer_par[128 + 64 - 1 - sw_pro_idx]);
					compute_beta(t_beta,t_pre_beta,t_gamma);
					fprintf(fp_blk40,"pri=%10.6f,par=%10.6f.gamma10:%10.6f,gamma11:%10.6f.\n",input_buffer_pri[128 + 64 - 1 - sw_pro_idx],input_buffer_par[128 + 64 - 1 - sw_pro_idx],t_gamma[0],t_gamma[1]);
					for (j = 0; j < 8; j++) {
						//printf("Beta[%d]:%10.6f  .",j,t_beta[j]);
						fprintf(fp_blk40,"Beta[%d]:%10.6f  .",j,t_pre_beta[j]);
					}
					//printf("\n");
					fprintf(fp_blk40,"\n");
					for (j = 0; j < 8; j++) {
						t_pre_beta[j] = t_beta[j];
					}
				} else {
					for (j = 0; j < 8; j++) {
						beta_buffer1_sw[128 + pro_lenm - 1 - sw_pro_idx][j] = t_pre_beta[j];
					}
					t_systematic_prime = input_buffer_pri[128 + pro_lenm - 1 - sw_pro_idx];
					compute_gamma(t_gamma,t_systematic_prime,input_buffer_par[128 + pro_lenm - 1 - sw_pro_idx]);
					gamma_buffer1_sw[128 + pro_lenm - 1 - sw_pro_idx][0] = t_gamma[0];
					gamma_buffer1_sw[128 + pro_lenm - 1 - sw_pro_idx][1] = t_gamma[1];
					fprintf(fp_blk40,"pri=%10.6f,par=%10.6f.gamma10:%10.6f,gamma11:%10.6f.\n",input_buffer_pri[128 + pro_lenm - 1 - sw_pro_idx],input_buffer_par[128 + pro_lenm - 1 - sw_pro_idx],gamma_buffer1_sw[128 + pro_lenm - 1 - sw_pro_idx][0],gamma_buffer1_sw[128 + pro_lenm - 1 - sw_pro_idx][1]);
					for (j = 0; j < 8; j++) {
						//printf("Beta[%d]:%10.6f  .",j,t_beta[j]);
						fprintf(fp_blk40,"Beta[%d]:%10.6f  .",j,t_pre_beta[j]);
					}
					//printf("\n");
					fprintf(fp_blk40,"\n");
					compute_beta(t_beta,t_pre_beta,t_gamma);
					for (j = 0; j < 8; j++) {
						if((128 + pro_lenm - 2 - sw_pro_idx) >= 0){
							beta_buffer1_sw[128 + pro_lenm - 2 - sw_pro_idx][j] = t_beta[j];
							t_pre_beta[j] = t_beta[j];
						}
					}
				}//end of if
			}//end of computing gamma and beta
			if(sw_idxm == 0){
				for (j = 0; j < 8; j++) {
					beta_temp_value_sb2[sb_idxm][j] = t_pre_beta[j];
				}
			}
			for(i=0;i<8;i++){//initialization of alpha
				t_pre_alpha[i] = alpha_initial_value_sw2[sb_idxm][i];
				fprintf(fp_blk40,"Alpha[%d]:%10.6f  .",i,t_pre_alpha[i]);
			}
			fprintf(fp_blk40,"\n");
			for(i=0;i<sw_lenm1;i++){
				compute_alpha(t_alpha,t_pre_alpha,gamma_buffer1_sw[128 - sw_lenm1 + i]);
				compute_extrinsic(&t_llr, &t_le, t_pre_alpha, beta_buffer1_sw[128 - sw_lenm1+ i], gamma_buffer1_sw[128 - sw_lenm1 + i], 0, priori_buffer1_sw[128 - sw_lenm1 + i], scaling_factorm);
				for (j = 0; j < 8; j++) {
					//printf("Beta[%d]:%10.6f  .",j,t_beta[j]);
					fprintf(fp_blk40,"Beta[%d]:%10.6f  .",j,beta_buffer1_sw[128 - sw_lenm1 + i][j]);
				}
				fprintf(fp_blk40,"\n");
				for(j=0;j<8;j++){
				t_pre_alpha[j] = t_alpha[j];
				fprintf(fp_blk40,"Alpha[%d]:%10.6f  .",j,t_alpha[j]);
				}
				fprintf(fp_blk40,"\n");
				extrinsic0[inter_bank_idx[sb_idxm][sw_idxm*sw_lenm1 + i]][inner_bank_idx[sb_idxm][sw_idxm*sw_lenm1 + i]] = t_le;
				fprintf(fp_blk40,"Beta_init_flag=%d,Soft= %10.6f.Extrinsic:%10.6f,inter_idx:%4d,inner_idx:%4d..\n",beta_init_flag,t_llr,t_le,inter_bank_idx[sb_idxm][sw_idxm*sw_lenm1 + i],inner_bank_idx[sb_idxm][sw_idxm*sw_lenm1 + i]);
			}
			for(j=0;j<8;j++){
				alpha_initial_value_sw2[sb_idxm][j] = t_alpha[j];
				fprintf(fp_blk40,"Alpha[%d]:%10.6f  .",j,t_alpha[j]);
			}
			fprintf(fp_blk40,"\n");
		}//end of 
	}
	//fclose(fp_blk40);
}

void compute_extrinsic(float *llr, float *le, state_metric alpha,
		state_metric beta, branch_metric gamma, float systematic, float priori,
		float scaling_factor) {

	state_metric llr_branch0, llr_branch1;
	float llr0 = 0, llr1 = 0;

	llr_branch0[0] = alpha[0] + beta[0] - gamma[1];
	llr_branch0[1] = alpha[3] + beta[1] - gamma[0];
	llr_branch0[2] = alpha[4] + beta[2] - gamma[0];
	llr_branch0[3] = alpha[7] + beta[3] - gamma[1];
	llr_branch0[4] = alpha[1] + beta[4] - gamma[1];
	llr_branch0[5] = alpha[2] + beta[5] - gamma[0];
	llr_branch0[6] = alpha[5] + beta[6] - gamma[0];
	llr_branch0[7] = alpha[6] + beta[7] - gamma[1];

	max_8to1(&llr0, llr_branch0);

	llr_branch1[0] = alpha[1] + beta[0] + gamma[1];
	llr_branch1[1] = alpha[2] + beta[1] + gamma[0];
	llr_branch1[2] = alpha[5] + beta[2] + gamma[0];
	llr_branch1[3] = alpha[6] + beta[3] + gamma[1];
	llr_branch1[4] = alpha[0] + beta[4] + gamma[1];
	llr_branch1[5] = alpha[3] + beta[5] + gamma[0];
	llr_branch1[6] = alpha[4] + beta[6] + gamma[0];
	llr_branch1[7] = alpha[7] + beta[7] + gamma[1];

	max_8to1(&llr1, llr_branch1);

	*(llr) = llr0 - llr1;
	*(le) = (*(llr) - systematic - priori) * scaling_factor;
}

void compute_gamma(branch_metric gamma, float sys, float par
		) {
	gamma[1] = (-sys - par) *0.5;//gamma11
	gamma[0] = (-sys + par) *0.5;//gamma10
}

void compute_beta(state_metric beta, state_metric pre_beta, branch_metric gamma) {
	state_metric beta_temp;
	int    i;
	beta_acs(beta_temp,pre_beta,gamma[1],gamma[0]);
	for(i=0;i<8;i++){
		beta[i] = beta_temp[i];
	}
}

void compute_alpha(state_metric alpha, state_metric pre_alpha, branch_metric gamma) {
	state_metric alpha_temp;
	int    i;
	alpha_acs(alpha_temp,pre_alpha,gamma[1],gamma[0]);

	for(i=0;i<8;i++){
		alpha[i] = alpha_temp[i];
	}
}
void alpha_acs(state_metric alpha, state_metric pre_alpha, float gamma_11,
		float gamma_10) {

	state_metric alpha0, alpha1;

	alpha0[0] = pre_alpha[0] - gamma_11;
	alpha1[0] = pre_alpha[1] + gamma_11;
	alpha0[1] = pre_alpha[3] - gamma_10;
	alpha1[1] = pre_alpha[2] + gamma_10;
	alpha0[2] = pre_alpha[4] - gamma_10;
	alpha1[2] = pre_alpha[5] + gamma_10;
	alpha0[3] = pre_alpha[7] - gamma_11;
	alpha1[3] = pre_alpha[6] + gamma_11;
	alpha0[4] = pre_alpha[1] - gamma_11;
	alpha1[4] = pre_alpha[0] + gamma_11;
	alpha0[5] = pre_alpha[2] - gamma_10;
	alpha1[5] = pre_alpha[3] + gamma_10;
	alpha0[6] = pre_alpha[5] - gamma_10;
	alpha1[6] = pre_alpha[4] + gamma_10;
	alpha0[7] = pre_alpha[6] - gamma_11;
	alpha1[7] = pre_alpha[7] + gamma_11;

	alpha[0] = (alpha0[0] >= alpha1[0]) ? alpha0[0] : alpha1[0];
	alpha[1] = (alpha0[1] >= alpha1[1]) ? alpha0[1] : alpha1[1];
	alpha[2] = (alpha0[2] >= alpha1[2]) ? alpha0[2] : alpha1[2];
	alpha[3] = (alpha0[3] >= alpha1[3]) ? alpha0[3] : alpha1[3];
	alpha[4] = (alpha0[4] >= alpha1[4]) ? alpha0[4] : alpha1[4];
	alpha[5] = (alpha0[5] >= alpha1[5]) ? alpha0[5] : alpha1[5];
	alpha[6] = (alpha0[6] >= alpha1[6]) ? alpha0[6] : alpha1[6];
	alpha[7] = (alpha0[7] >= alpha1[7]) ? alpha0[7] : alpha1[7];

}

void beta_acs(state_metric beta, state_metric pre_beta, float gamma_11,
		float gamma_10) {

	state_metric beta0, beta1;

	beta0[0] = pre_beta[0] - gamma_11;
	beta1[0] = pre_beta[4] + gamma_11;
	beta0[1] = pre_beta[4] - gamma_11;
	beta1[1] = pre_beta[0] + gamma_11;
	beta0[2] = pre_beta[5] - gamma_10;
	beta1[2] = pre_beta[1] + gamma_10;
	beta0[3] = pre_beta[1] - gamma_10;
	beta1[3] = pre_beta[5] + gamma_10;
	beta0[4] = pre_beta[2] - gamma_10;
	beta1[4] = pre_beta[6] + gamma_10;
	beta0[5] = pre_beta[6] - gamma_10;
	beta1[5] = pre_beta[2] + gamma_10;
	beta0[6] = pre_beta[7] - gamma_11;
	beta1[6] = pre_beta[3] + gamma_11;
	beta0[7] = pre_beta[3] - gamma_11;
	beta1[7] = pre_beta[7] + gamma_11;

	beta[0] = (beta0[0] >= beta1[0]) ? beta0[0] : beta1[0];
	beta[1] = (beta0[1] >= beta1[1]) ? beta0[1] : beta1[1];
	beta[2] = (beta0[2] >= beta1[2]) ? beta0[2] : beta1[2];
	beta[3] = (beta0[3] >= beta1[3]) ? beta0[3] : beta1[3];
	beta[4] = (beta0[4] >= beta1[4]) ? beta0[4] : beta1[4];
	beta[5] = (beta0[5] >= beta1[5]) ? beta0[5] : beta1[5];
	beta[6] = (beta0[6] >= beta1[6]) ? beta0[6] : beta1[6];
	beta[7] = (beta0[7] >= beta1[7]) ? beta0[7] : beta1[7];

}

void max_8to1(float *hmax, state_metric input) {
	int i;
	float temp;
	temp = input[0];
	for (i = 1; i < 8; i++) {
		temp = (input[i] >= temp) ? input[i] : temp;
	}
	*(hmax) = temp;
}

void hard_decision(int *out_bits, float *llr, int len) {
	int i;
	for (i = 0; i < len; i++) {
		out_bits[i] = (llr[i] > 0) ? 0 : 1;
	}
}
void compute_inter_inner_idx(int cb_length, int sb_length) {
	int j;
	int interleave_idx, f1, f2;
	int whole_gi[6144], whole_pi[6144];
	int e1, e2, m;
	int as[4][1536] = { 0 };

	//calculate: index, f1, f2
	for (interleave_idx = 0;
			interleaver_size[interleave_idx] != cb_length
					&& interleave_idx < 188; interleave_idx++) {
		if (interleave_idx == 188) {
			printf("Illegal code block length!\n");
			exit(0);
		}
	}
	f1 = interleaver_f1[interleave_idx];
	f2 = interleaver_f2[interleave_idx];

	whole_gi[0] = (f1 + f2) % cb_length;
	whole_pi[0] = 0;
	for (j = 1; j < cb_length; j++) {
		whole_gi[j] = (whole_gi[j - 1] + 2 * f2) % cb_length;
		whole_pi[j] = (whole_gi[j - 1] + whole_pi[j - 1]) % cb_length;
	}
	if (cb_length <= 768) {
		as[0][0] = (f1 + f2) / sb_length;
		inter_bank_idx[0][0] = 0;
		e1 = ((2 * f2) % cb_length) / sb_length;
		e2 = ((2 * f2) % cb_length) % sb_length;
		m = 1;
		for (j = 1; j < sb_length; j++) {
			as[0][j] = (as[0][j - 1] + e1 + (whole_gi[j - 1]%sb_length + e2 > sb_length ? 1 : 0)) % m;
			inter_bank_idx[0][j] = (inter_bank_idx[0][j - 1] + as[0][j - 1] + (whole_gi[j - 1] % sb_length + whole_pi[j - 1] % sb_length > sb_length ? 1 : 0)) % m;
			inner_bank_idx[0][j] = whole_pi[j] % sb_length;
		}
	} else if (cb_length <= 1536) {
		as[0][0] = (whole_gi[0]) / sb_length;
		as[1][0] = (whole_gi[sb_length]) / sb_length;
		inter_bank_idx[0][0] = (whole_pi[0]) / sb_length;
		inter_bank_idx[1][0] = (whole_pi[sb_length]) / sb_length;
		e1 = ((2 * f2) % cb_length) / sb_length;
		e2 = ((2 * f2) % cb_length) % sb_length;
		m = 2;
		fprintf(fp_blk40,"as[0][0]:%d,whole_gi[0]=%d,as[1][0]:%d\n",as[0][0],whole_gi[0],as[1][0]);
		for (j = 1; j < sb_length; j++) {
			as[0][j] = (as[0][j - 1] + e1
					+ (whole_gi[j - 1]%sb_length + e2 > sb_length ? 1 : 0)) % m;
			inter_bank_idx[0][j] = (inter_bank_idx[0][j - 1] + as[0][j - 1]
					+ (whole_gi[j - 1] % sb_length + whole_pi[j - 1] % sb_length > sb_length ? 1 : 0))
					% m;
			as[1][j] = (as[1][j - 1] + e1
					+ (whole_gi[j - 1]%sb_length + e2 > sb_length ? 1 : 0)) % m;
			inter_bank_idx[1][j] = (inter_bank_idx[1][j - 1] + as[1][j - 1]
					+ (whole_gi[j - 1] % sb_length + whole_pi[j - 1] % sb_length > sb_length ? 1 : 0))
					% m;
			inner_bank_idx[0][j] = whole_pi[j] % sb_length;
			inner_bank_idx[1][j] = whole_pi[j] % sb_length;
			fprintf(fp_blk40,"as[0][%d]:%d,whole_gi[%d]=%d,as[1][%d]:%d\n",j,as[0][j],j,whole_gi[j]%sb_length,j,as[1][j]);
		}
	} else {
		sb_length = cb_length / 4;
		as[0][0] = (whole_gi[0]) / sb_length;
		as[1][0] = (whole_gi[sb_length]) / sb_length;
		as[2][0] = (whole_gi[sb_length * 2]) / sb_length;
		as[3][0] = (whole_gi[sb_length * 3]) / sb_length;
		inter_bank_idx[0][0] = (whole_pi[0]) / sb_length;
		inter_bank_idx[1][0] = (whole_pi[sb_length]) / sb_length;
		inter_bank_idx[2][0] = (whole_pi[sb_length * 2]) / sb_length;
		inter_bank_idx[3][0] = (whole_pi[sb_length * 3]) / sb_length;
		e1 = ((2 * f2) % cb_length) / sb_length;
		e2 = ((2 * f2) % cb_length) % sb_length;
		m = 4;
		for (j = 1; j < sb_length; j++) {
			as[0][j] = (as[0][j - 1] + e1
					+ (whole_gi[j - 1]%sb_length + e2 > sb_length ? 1 : 0)) % m;
			inter_bank_idx[0][j] = (inter_bank_idx[0][j - 1] + as[0][j - 1]
					+ (whole_gi[j - 1] % sb_length + whole_pi[j - 1] % sb_length > sb_length ? 1 : 0))
					% m;
			as[1][j] = (as[1][j - 1] + e1
					+ (whole_gi[j - 1]%sb_length + e2 > sb_length ? 1 : 0)) % m;
			inter_bank_idx[1][j] = (inter_bank_idx[1][j - 1] + as[1][j - 1]
					+ (whole_gi[j - 1] % sb_length + whole_pi[j - 1] % sb_length > sb_length ? 1 : 0))
					% m;
			as[2][j] = (as[2][j - 1] + e1
					+ (whole_gi[j - 1]%sb_length + e2 > sb_length ? 1 : 0)) % m;
			inter_bank_idx[2][j] = (inter_bank_idx[2][j - 1] + as[2][j - 1]
					+ (whole_gi[j - 1] % sb_length + whole_pi[j - 1] % sb_length > sb_length ? 1 : 0))
					% m;
			as[3][j] = (as[3][j - 1] + e1
					+ (whole_gi[j - 1]%sb_length + e2 > sb_length ? 1 : 0)) % m;
			inter_bank_idx[3][j] = (inter_bank_idx[3][j - 1] + as[3][j - 1]
					+ (whole_gi[j - 1] % sb_length + whole_pi[j - 1] % sb_length > sb_length ? 1 : 0))
					% m;
			inner_bank_idx[0][j] = whole_pi[j] % sb_length;
			inner_bank_idx[1][j] = whole_pi[j] % sb_length;
			inner_bank_idx[2][j] = whole_pi[j] % sb_length;
			inner_bank_idx[3][j] = whole_pi[j] % sb_length;
		}
	}
}

void compute_beta_init(float *systematic, float *parity1, float *parity2,
		int cb_length) {

	float sys1[3], sys2[3], par1[3], par2[3];

	//extract Four stream Tail bits
	sys1[0] = *(systematic + cb_length);
	sys1[1] = *(parity2 + cb_length);
	sys1[2] = *(parity1 + cb_length + 1);
	par1[0] = *(parity1 + cb_length);
	par1[1] = *(systematic + cb_length + 1);
	par1[2] = *(parity2 + cb_length + 1);
	sys2[0] = *(systematic + cb_length + 2);
	sys2[1] = *(parity2 + cb_length + 2);
	sys2[2] = *(parity1 + cb_length + 3);
	par2[0] = *(parity1 + cb_length + 2);
	par2[1] = *(systematic + cb_length + 3);
	par2[2] = *(parity2 + cb_length + 3);

		//Cal the initial Beta value for MAP 1 
	beta_initial_value_cb1[0] = (sys1[0] + par1[0]) *0.5
			+ (sys1[1] + par1[1]) *0.5 + (sys1[2] + par1[2]) *0.5;
	beta_initial_value_cb1[1] = (-sys1[0] - par1[0]) *0.5
			+ (sys1[1] + par1[1]) *0.5 + (sys1[2] + par1[2]) *0.5;
	beta_initial_value_cb1[2] = (-sys1[0] + par1[0]) *0.5
			+ (-sys1[1] - par1[1]) *0.5 + (sys1[2] + par1[2]) *0.5;
	beta_initial_value_cb1[3] = (sys1[0] - par1[0]) *0.5
			+ (-sys1[1] - par1[1]) *0.5 + (sys1[2] + par1[2]) *0.5;
	beta_initial_value_cb1[4] = (sys1[0] - par1[0]) *0.5
			+ (-sys1[1] + par1[1]) *0.5 + (-sys1[2] - par1[2]) *0.5;
	beta_initial_value_cb1[5] = (-sys1[0] + par1[0]) *0.5
			+ (-sys1[1] + par1[1]) *0.5 + (-sys1[2] - par1[2]) *0.5;
	beta_initial_value_cb1[6] = (-sys1[0] - par1[0]) *0.5
			+ (sys1[1] - par1[1]) *0.5 + (-sys1[2] - par1[2]) *0.5;
	beta_initial_value_cb1[7] = (sys1[0] + par1[0]) *0.5
			+ (sys1[1] - par1[1]) *0.5 + (-sys1[2] - par1[2]) *0.5;

	//Cal the initial Beta value for MAP 2
	beta_initial_value_cb2[0] = (sys2[0] + par2[0]) * 0.5
			+ (sys2[1] + par2[1]) *0.5 + (sys2[2] + par2[2]) *0.5;
	beta_initial_value_cb2[1] = (-sys2[0] - par2[0]) *0.5
			+ (sys2[1] + par2[1]) *0.5 + (sys2[2] + par2[2]) *0.5;
	beta_initial_value_cb2[2] = (-sys2[0] + par2[0]) *0.5
			+ (-sys2[1] - par2[1]) *0.5 + (sys2[2] + par2[2]) *0.5;
	beta_initial_value_cb2[3] = (sys2[0] - par2[0]) *0.5
			+ (-sys2[1] - par2[1]) *0.5 + (sys2[2] + par2[2]) *0.5;
	beta_initial_value_cb2[4] = (sys2[0] - par2[0]) *0.5
			+ (-sys2[1] + par2[1]) *0.5 + (-sys2[2] - par2[2]) *0.5;
	beta_initial_value_cb2[5] = (-sys2[0] + par2[0]) *0.5
			+ (-sys2[1] + par2[1]) *0.5 + (-sys2[2] - par2[2]) *0.5;
	beta_initial_value_cb2[6] = (-sys2[0] - par2[0]) *0.5
			+ (sys2[1] - par2[1]) *0.5 + (-sys2[2] - par2[2]) *0.5;
	beta_initial_value_cb2[7] = (sys2[0] + par2[0]) *0.5
			+ (sys2[1] - par2[1]) *0.5 + (-sys2[2] - par2[2]) *0.5;
}

void SPU(){

	int i,j;
	//Setting the parameters
	p_crc_type = CRC_TYPE;
	p_cb_len = LEN;
    p_sb_num = LEN / SUBBLOCK_LEN;
    p_max_iteration_num = MAX_ITERATION_NUM;
    p_sb_len = SUBBLOCK_LEN;
    p_sw_num = SUBBLOCK_LEN / SLIDING_WINDOW_LEN + (SUBBLOCK_LEN % SLIDING_WINDOW_LEN != 0);
    p_sw_len_1 = SLIDING_WINDOW_LEN;
    if((SUBBLOCK_LEN % SLIDING_WINDOW_LEN) == 0)
		p_sw_len_2 = SLIDING_WINDOW_LEN;
	else 
		p_sw_len_2 = SUBBLOCK_LEN - (SUBBLOCK_LEN / SLIDING_WINDOW_LEN)*SLIDING_WINDOW_LEN;
    p_pro_len  = PROLOG_LEN;
    p_scalling_factor = SCALLING_FACTOR;
	
	// initialization of alpha and beta initial value
	for(i=0;i<4;i++){
		for(j=0;j<8;j++){
			if(i==0){
				alpha_initial_value_sb1[i][j] = beta_initial_value[j];
				alpha_initial_value_sb2[i][j] = beta_initial_value[j];
			}else{
				alpha_initial_value_sb1[i][j] = beta_initial_value_ep[j];
				alpha_initial_value_sb2[i][j] = beta_initial_value_ep[j];
			}
			beta_initial_value_sb1[i][j] = beta_initial_value_ep[j];
			beta_initial_value_sb2[i][j] = beta_initial_value_ep[j];
		}
	}

	//initiallization of ALL MEMORYS
	for(i=0;i<192;i++){
		input_buffer_sys[i] = 0;
		input_buffer_par[i] = 0;
		input_buffer_pri[i] = 0;
	}
		
	for(i=0;i<128;i++){
		for(j=0;j<2;j++){
			gamma_buffer0_sw[i][j] = 0;
			gamma_buffer1_sw[i][j] = 0;
		}
		for(j=0;j<8;j++){
			beta_buffer0_sw[i][j] = 0;
			beta_buffer1_sw[i][j] = 0;
		}
	}
	for(i=0;i<128;i++){
		systematic_buffer0_sw[i] = 0;
		systematic_buffer1_sw[i] = 0;
		priori_buffer0_sw[i] = 0;
		priori_buffer1_sw[i] = 0;
	}

	for(i=0;i<4;i++){
		for(j=0;j<1536;j++){
			extrinsic0[i][j]     = 0;
			extrinsic1[i][j]     = 0;
			soft_bit[i][j]       = 0;
			hard[i][j]           = 0;
			inter_bank_idx[i][j] = 0;
			inner_bank_idx[i][j] = 0;
		}
	}
}

void alpha_initial_sw(int iter_idx){
	int i, j;
	state_metric alpha_temp_value_sw1[4],alpha_temp_value_sw2[4];
	if (!iter_idx) {
		for (i = 0; i < p_sb_num; i++) {
			for (j = 0; j < 8; j++) {
				alpha_initial_value_sw1[i][j] = alpha_initial_value_sb1[i][j];
				alpha_initial_value_sw2[i][j] = alpha_initial_value_sb2[i][j];
			}
		}
	} else {
		for(i=0;i<4;i++){//temp the alpha initial value for SW
			for(j=0;j<8;j++){
				alpha_temp_value_sw1[i][j] = alpha_initial_value_sw1[i][j];
				alpha_temp_value_sw2[i][j] = alpha_initial_value_sw2[i][j];
			}
		}
		for (i = 0; i < p_sb_num-1; i++) {
			for (j = 0; j < 8; j++) {
				alpha_initial_value_sw1[i + 1][j]
						= alpha_temp_value_sw1[i][j];
				alpha_initial_value_sw2[i + 1][j]
						= alpha_temp_value_sw2[i][j];
			}
		}
		for (j = 0; j < 8; j++) {
			alpha_initial_value_sw1[0][j] = alpha_initial_value_sb1[0][j];
			alpha_initial_value_sw2[0][j] = alpha_initial_value_sb2[0][j];
			//alpha_initial_value_sw1[0][j] = alpha_temp_value_sw1[p_sb_num-1][j];
			//alpha_initial_value_sw2[0][j] = alpha_temp_value_sw2[p_sb_num-1][j];
		}
	}
}

void beta_initial_sw(int iter_idx){
	int i, j;
	if (iter_idx) {
		for (i = 0; i < p_sb_num-1; i++) {
			for (j = 0; j < 8; j++) {
				//beta_initial_value_sb1[i][j] = beta_temp_value_sb1[i+1][j];
				//beta_initial_value_sb2[i][j] 	= beta_temp_value_sb2[i+1][j];
			}
		}
	}
}
#endif
