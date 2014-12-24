#include "turbo_enc_dec.h"


float sys_buffer[MAX_SW_LEN] = {0};//��ˮ��ʱ��2����ͬʱ���㣬һ������gamma��beta��һ������llr��alpha��le
float sys_buffer2[MAX_SW_LEN] = {0};
float parity_bufer[MAX_SW_LEN] = {0};
//float Le_buffer[MAX_SW_LEN] = {0};
float La_buffer[MAX_SW_LEN] = {0};//��ˮ��ʱ��2����ͬʱ���㣬һ������gamma��beta��һ������llr��alpha��le
float La_buffer2[MAX_SW_LEN] = {0};
//float sys_la_buffer[MAX_SW_LEN]={0};

// Reg
state_metric alpha_init_buffer1, alpha_init_buffer2;
state_metric beta_init_buffer1, beta_init_buffer2;

//RAM
branch_metric gamma_buffer[MAX_SW_LEN];
state_metric beta_buffer[MAX_SW_LEN];
branch_metric gamma_buffer2[MAX_SW_LEN];
state_metric beta_buffer2[MAX_SW_LEN];

state_metric beta_init_ram1[PU_NUM*(SW_NUM)], beta_init_ram2[PU_NUM*(SW_NUM)];
state_metric alpha_init_ram1[PU_NUM], alpha_init_ram2[PU_NUM];

state_metric beta_init_value1, beta_init_value2;
state_metric equal_prb_init = {log1_8, log1_8, log1_8, log1_8, log1_8, log1_8, log1_8, log1_8};
state_metric unequal_prb_init = {0, MIN_VALUE, MIN_VALUE, MIN_VALUE, MIN_VALUE, MIN_VALUE, MIN_VALUE, MIN_VALUE};

float Le[PU_NUM][MAX_SB_LEN] ;

//int hard_bit[PU_NUM][MAX_SB_LEN] = {0};
int hard_bit[MAX_CB_LEN] = {0};

//ROM or Calculate in real-time
int inter_bank_idx[PU_NUM][MAX_SB_LEN] ;
int inner_bank_idx[PU_NUM][MAX_SB_LEN] ;

//Reg
branch_metric t_gamma;
state_metric t_beta, t_pre_beta;
state_metric t_alpha, t_pre_alpha;
float t_le, t_llr;

//Reg
int cb_len, sb_len, max_iter_num, sb_num, sw_num, sw_len;
int sw_len_1, sw_len_2; 
int crc_type;
float dec_scaling_factor;
int prolog_len;



void make_interleaver_table ( int* table, int cb_size )
{
	// Variables:
	int    i;
	int    size = 0;
	float f1   = 0.0;
	float f2   = 0.0;

	// Execute:
	for (i=0; i<188; i++) {
		if (cb_size == interleaver_size[i]) {
			size = interleaver_size[i];
			f1   = (float)interleaver_f1[i];
			f2   = (float)interleaver_f2[i];
			break;
		}
	}

	for (i=0; i<size; i++) {
		*(table+i) = (int)fmod(((f1 * i) + (f2 * pow((float)i, 2.0))) , (float)size);
	}
}
void constitue_encoder (int* stream_out, int* stream, int length)
{
	// Variables:
	int     i;          
	int     feedback;  
	int     x;          
	int     s_register[4] = {0, 0, 0, 0};	

	// Execute:
	for (i=0; i<length; i++) {
		feedback =  (s_register[2] + s_register[3]) % 2;        
		s_register[0] = (*(stream+i) + feedback) % 2;              
		x = (s_register[0] + s_register[1] + s_register[3]) % 2; 
		*(stream_out+i) = x;

		// cyclic shift��
		s_register[3] = s_register[2];
		s_register[2] = s_register[1];
		s_register[1] = s_register[0];
	}

	// terminated bit:6 bit
	for (i=0; i<3; i++) {

		feedback =  (s_register[2] + s_register[3]) % 2; 
		s_register[0] = (feedback + feedback) % 2; 
		x = (s_register[0] + s_register[1] + s_register[3]) % 2; 

		// terminated bit
		*(stream_out+(length+2*i))   = feedback;
		*(stream_out+(length+2*i+1)) = x;


		s_register[3] = s_register[2];
		s_register[2] = s_register[1];
		s_register[1] = s_register[0];
	}
}


/*----------------------------------------------------------------------------*/
//void turbo_encoder(int* info_sys_llr, int* info_parity1, int* info_parity2, int* info_codeblock, int length_cb )
// @brief: turbo encoder
// @info_sys_llr: output signals 
// @info_parity1: output of the 1st constitute encoder
// @info_parity2: output of the 2nd constitute encoder
// @info_codeblock: input
// @length_cb: input
/*----------------------------------------------------------------------------*/
void turbo_encoder(int* info_sys_llr, int* info_parity1, int* info_parity2, int* info_codeblock, int length_cb )
{
	// Variables:
	int     i;     
	int*    inter; 
	int*    interleaver_table;
	int*    tmp_parity1;
	int*    tmp_parity2;
	int     tail[12] = {0,0,0,0,0,0,0,0,0,0,0,0};  

	// Initialization: 
	interleaver_table = (int*)malloc(sizeof(int) * length_cb);
	make_interleaver_table(interleaver_table, length_cb);

	// sys_llr bits:
	for (i=0; i<length_cb; i++)
	{
		*(info_sys_llr+i) = *(info_codeblock+i);
	}

	// Execute:
	// Constitute encoder 1#:
	tmp_parity1 = (int*)malloc(sizeof(int) * (6+length_cb));
	constitue_encoder(tmp_parity1, info_codeblock, length_cb);
	for (i=0; i<length_cb; i++)
	{
		*(info_parity1+i) = *(tmp_parity1+i);
	}

	// interleave:
	inter = (int*)malloc(sizeof(int) * length_cb);
	for (i=0; i<length_cb; i++){
		*(inter+i) = *(info_codeblock + *(interleaver_table+i));
	}

	// Constitute encoder #2:
	tmp_parity2 = (int*) malloc(sizeof(int) * (6+length_cb));
	constitue_encoder(tmp_parity2, inter, length_cb);
	for (i=0; i<length_cb; i++)
	{
		*(info_parity2+i) = *(tmp_parity2+i);
	}

	// terminated bit:
	for (i=0; i<6; i++){
		tail[i] = *(tmp_parity1 + i + length_cb);
	}
	for (i=0; i<6; i++){
		tail[6+i] = *(tmp_parity2 + i + length_cb);
	}

	for (i=0; i<4; i++) {
		*(info_sys_llr+length_cb+i) = tail[3*i];
		*(info_parity1+length_cb+i)    = tail[3*i+1];
		*(info_parity2+length_cb+i)    = tail[3*i+2];
	}

	// memory free:
	free(interleaver_table);
	free(inter);
	free(tmp_parity1);
	free(tmp_parity2);
}
int SPU()
{
	int i, j;
	int flag; //SW_NUM�Ƿ��ܱ�SB_LEN���1�����0���������

	crc_type = CRC_TYPE;

	cb_len = CB_LEN;
	sb_num = PU_NUM;
	sb_len = cb_len / sb_num;
	sw_num = SW_NUM;

	flag = (sb_len % SW_LEN == 0);

	if (flag == 1){
		sw_len_1 = SW_LEN;
		sw_len_2 = 0;
	}
	else {
		sw_len_1 = SW_LEN;
		sw_len_2 = sb_len % SW_LEN;
	//	sw_num = sw_num+1;
	}

	max_iter_num = MAX_ITERATION;
	
	//initialization of alpha & beta 
	//the first SB 
	for (j=0; j<N_STATE; j++)
	{
		alpha_init_ram1[0][j] = unequal_prb_init[j];
		alpha_init_ram2[0][j] = unequal_prb_init[j];
	}
	for (i=1; i<PU_NUM; i++){
		for (j=0; j<N_STATE; j++){
			alpha_init_ram1[i][j] = equal_prb_init[j];
			alpha_init_ram2[i][j] = equal_prb_init[j];
		}
	}
	
	for (i=0; i<PU_NUM*sw_num; i++){
		for (j=0; j<N_STATE; j++){
			beta_init_ram1[i][j] = equal_prb_init[j];
			beta_init_ram2[i][j] = equal_prb_init[j];
		}
	}

	for (i=0; i<PU_NUM; i++){
		for (j=0; j<sb_len; j++){
			Le[i][j] = 0;
			inter_bank_idx[i][j]=0;
			inner_bank_idx[i][j]=0;
		}
	}

	return flag;
}

void rd_beta_init(float *beta_init, float *sys_llr, float *parity_llr, float *La_llr, int rd_len)
{
	int i, j;                                                                                                                                                
//	float max_temp;
//	FILE *fp;
//	fp=fopen("compute_beta_init.txt","a");

//   fprintf(fp,"beta_init:\n");
	for (j=0; j<N_STATE; j++){                                                                                                                                                                                                                                                    
		t_pre_beta[j] = equal_prb_init[j];
//		fprintf(fp,"%f(%#010x)\n",t_pre_beta[j],*(unsigned int *)&t_pre_beta[j]);
	}                                                                                                                                                        
                                                                                                                                                
	for (i=rd_len-2; i>=0; i--){  
		//compute gamma:
		t_gamma[0] = (-sys_llr[i+1] + parity_llr[i+1] - La_llr[i+1])*0.5;                                                                                    
		t_gamma[1] = (-sys_llr[i+1] - parity_llr[i+1] - La_llr[i+1])*0.5; 
//		fprintf(fp,"sys_llr(%d):%f(%#010x)\n",i+1,sys_llr[i+1],*(unsigned int *)&sys_llr[i+1]);
//		fprintf(fp,"par_llr(%d):%f(%#010x)\n",i+1,parity_llr[i+1],*(unsigned int *)&parity_llr[i+1]);
//		fprintf(fp,"La_llr(%d):%f(%#010x)\n",i+1,La_llr[i+1],*(unsigned int *)&La_llr[i+1]);
//		fprintf(fp,"gamma0(%d):%f(%#010x)\n",i+1,t_gamma[0],*(unsigned int *)&t_gamma[0]);
//		fprintf(fp,"gamma1(%d):%f(%#010x)\n",i+1,t_gamma[1],*(unsigned int *)&t_gamma[1]);

		compute_beta(t_beta, t_pre_beta, t_gamma);                                                                                                   
	//	max_8to1(&max_temp, t_beta);

//       fprintf(fp,"beta(%d):",i);
		for (j=0; j<N_STATE; j++){                                                                                                                             
	//		t_beta[j] -= max_temp;
			t_pre_beta[j] = t_beta[j];
//			fprintf(fp,"%f(%#010x)  ",t_beta[j],*(unsigned int *)&t_beta[j]);
		}
//		fprintf(fp,"\n");

	}                                                                                                                                                        

	t_gamma[0] = (-sys_llr[0] + parity_llr[0] - La_llr[0])*0.5;                                                                                    
	t_gamma[1] = (-sys_llr[0] - parity_llr[0] - La_llr[0])*0.5; 
//	fprintf(fp,"gamma0(%d):%f(%#010x)\n",0,t_gamma[0],*(unsigned int *)&t_gamma[0]);
//	fprintf(fp,"gamma1(%d):%f(%#010x)\n",0,t_gamma[1],*(unsigned int *)&t_gamma[1]);
	compute_beta(t_beta, t_pre_beta, t_gamma);                                                                                                       
	//max_8to1(&max_temp, t_beta);
//	fprintf(fp,"beta_init:");
	for (j=0; j<N_STATE; j++){                                                                                                                               
	//	t_beta[j] -= max_temp;
		beta_init[j] = t_beta[j];
//		fprintf(fp,"%f(%#010x)  ",t_beta[j],*(unsigned int *)&t_beta[j]);
	}
//	fprintf(fp,"\n");

	/*for (j=0; j<N_STATE; j++){                                                                                                                               
		beta_init[j] = t_beta[j];                                                                                                                                                                                                                                                
	}     */                                                                                                                                                                                                                                                                                                                                                       
//    fclose(fp);
}

void compute_beta_init(float *sys_llr, float *parity1_llr, float *parity2_llr, int cb_length,int flag)//flag:SW_NUM�Ƿ��ܱ�SB_LEN���1�����0���������
{
//	int i,j;
	float sys1[3], sys2[3], parity1[3], parity2[3];
//	float sys1_rd[PU_NUM*(SW_NUM+1)-1][PROLOG_LEN],sys2_rd[PU_NUM*(SW_NUM+1)-1][PROLOG_LEN],parity1_rd[PU_NUM*(SW_NUM+1)-1][PROLOG_LEN],parity2_rd[PU_NUM*(SW_NUM+1)-1][PROLOG_LEN];
//	int sb_idx,sw_idx;
//	int sw_offset; //������
//	int addr_idx;
//	FILE *fp;

//	fp=fopen("beta_init_ram1.txt","w");
    
	//tail bits:
	//extract Four stream Tail bits
	sys1[0] = *(sys_llr + cb_length);
	sys1[1] = *(parity2_llr + cb_length);
	sys1[2] = *(parity1_llr + cb_length + 1);
	parity1[0] = *(parity1_llr + cb_length);
	parity1[1] = *(sys_llr + cb_length + 1);
	parity1[2] = *(parity2_llr + cb_length + 1);
	sys2[0] = *(sys_llr + cb_length + 2);
	sys2[1] = *(parity2_llr + cb_length + 2);
	sys2[2] = *(parity1_llr + cb_length + 3);
	parity2[0] = *(parity1_llr + cb_length + 2);
	parity2[1] = *(sys_llr + cb_length + 3);
	parity2[2] = *(parity2_llr + cb_length + 3);

	//Cal the initial Beta value for MAP 1 
	beta_init_value1[0] = (sys1[0] + parity1[0]) *0.5
		+ (sys1[1] + parity1[1]) *0.5 + (sys1[2] + parity1[2]) *0.5;
	beta_init_value1[1] = (-sys1[0] - parity1[0]) *0.5
		+ (sys1[1] + parity1[1]) *0.5 + (sys1[2] + parity1[2]) *0.5;
	beta_init_value1[2] = (-sys1[0] + parity1[0]) *0.5
		+ (-sys1[1] - parity1[1]) *0.5 + (sys1[2] + parity1[2]) *0.5;
	beta_init_value1[3] = (sys1[0] - parity1[0]) *0.5
		+ (-sys1[1] - parity1[1]) *0.5 + (sys1[2] + parity1[2]) *0.5;
	beta_init_value1[4] = (sys1[0] - parity1[0]) *0.5
		+ (-sys1[1] + parity1[1]) *0.5 + (-sys1[2] - parity1[2]) *0.5;
	beta_init_value1[5] = (-sys1[0] + parity1[0]) *0.5
		+ (-sys1[1] + parity1[1]) *0.5 + (-sys1[2] - parity1[2]) *0.5;
	beta_init_value1[6] = (-sys1[0] - parity1[0]) *0.5
		+ (sys1[1] - parity1[1]) *0.5 + (-sys1[2] - parity1[2]) *0.5;
	beta_init_value1[7] = (sys1[0] + parity1[0]) *0.5
		+ (sys1[1] - parity1[1]) *0.5 + (-sys1[2] - parity1[2]) *0.5;

	//Cal the initial Beta value for MAP 2
	beta_init_value2[0] = (sys2[0] + parity2[0]) * 0.5
		+ (sys2[1] + parity2[1]) *0.5 + (sys2[2] + parity2[2]) *0.5;
	beta_init_value2[1] = (-sys2[0] - parity2[0]) *0.5
		+ (sys2[1] + parity2[1]) *0.5 + (sys2[2] + parity2[2]) *0.5;
	beta_init_value2[2] = (-sys2[0] + parity2[0]) *0.5
		+ (-sys2[1] - parity2[1]) *0.5 + (sys2[2] + parity2[2]) *0.5;
	beta_init_value2[3] = (sys2[0] - parity2[0]) *0.5
		+ (-sys2[1] - parity2[1]) *0.5 + (sys2[2] + parity2[2]) *0.5;
	beta_init_value2[4] = (sys2[0] - parity2[0]) *0.5
		+ (-sys2[1] + parity2[1]) *0.5 + (-sys2[2] - parity2[2]) *0.5;
	beta_init_value2[5] = (-sys2[0] + parity2[0]) *0.5
		+ (-sys2[1] + parity2[1]) *0.5 + (-sys2[2] - parity2[2]) *0.5;
	beta_init_value2[6] = (-sys2[0] - parity2[0]) *0.5
		+ (sys2[1] - parity2[1]) *0.5 + (-sys2[2] - parity2[2]) *0.5;
	beta_init_value2[7] = (sys2[0] + parity2[0]) *0.5
		+ (sys2[1] - parity2[1]) *0.5 + (-sys2[2] - parity2[2]) *0.5;

	//sliding window beta init:
	//DEC1:
/*	for (sb_idx=0;sb_idx<sb_num;sb_idx++)
	{
		for (sw_idx=0;sw_idx<sw_num-1;sw_idx++)
		{
			if(sb_idx==sb_num-1&&sw_idx==sw_num-2)
				prolog_len=sw_len_2;

			sw_len = sw_len_1;
			sw_offset = sw_len*(sw_idx+1);

			for (i=0; i<prolog_len; i++){
				sys_buffer[i] = sys_llr[sb_idx*sb_len+sw_offset+i];
				parity_bufer[i] = parity1_llr[sb_idx*sb_len+sw_offset+i];
				La_buffer[i] = Le[sb_idx][sw_offset+i];
			}

			rd_beta_init(beta_init_buffer1, sys_buffer, parity_bufer,La_buffer, prolog_len);

			for (j=0; j<N_STATE; j++){
				beta_init_ram1[sb_idx*sw_num+sw_idx][j] = beta_init_buffer1[j];
			}
		}
		// the last SW
		if (sb_idx<(sb_num-1))
		{
			if (flag == 1){ //
				sw_len = sw_len_1;
				sw_offset = sw_len*sw_num;
			}
			else {
				sw_len = sw_len_2;
				sw_offset = sw_len_1*(sw_num-1)+sw_len;
			}

			for (i=0; i<prolog_len; i++){
				sys_buffer[i] = sys_llr[sb_idx*sb_len+sw_offset+i];
				parity_bufer[i] = parity1_llr[sb_idx*sb_len+sw_offset+i];
				La_buffer[i] = Le[sb_idx][sw_offset+i];
			}

			rd_beta_init(beta_init_buffer1, sys_buffer, parity_bufer,La_buffer, prolog_len);

			for (j=0; j<N_STATE; j++){
				beta_init_ram1[sb_idx*sw_num+(sw_num-1)][j] = beta_init_buffer1[j];
			}
		}
		//end for SW
	}//end for PU

	//DEC2:
	for (sb_idx=0;sb_idx<sb_num;sb_idx++)
	{
		for (sw_idx=0;sw_idx<sw_num-1;sw_idx++)
		{
			if(sb_idx==sb_num-1&&sw_idx==sw_num-2){
				prolog_len=sw_len_2;
			}

				sw_len = sw_len_1;
				sw_offset = sw_len*(sw_idx+1);


				for (i=0; i<prolog_len; i++){
					addr_idx = inter_bank_idx[sb_idx][sw_offset+i] * sb_len + inner_bank_idx[sb_idx][sw_offset+i];
					sys_buffer[i] = sys_llr[addr_idx];
					parity_bufer[i] = parity2_llr[sb_idx*sb_len+sw_offset+i];
					La_buffer[i] = Le[inter_bank_idx[sb_idx][sw_offset+i]][inner_bank_idx[sb_idx][sw_offset+i]];
					}

					rd_beta_init(beta_init_buffer2, sys_buffer, parity_bufer,La_buffer, prolog_len);

					for (j=0; j<N_STATE; j++){
						beta_init_ram2[sb_idx*sw_num+sw_idx][j] = beta_init_buffer2[j];
					}
		}
		// the last SW
		if (sb_idx<(sb_num-1))
		{
			if (flag == 1){ //
				sw_len = sw_len_1;
				sw_offset = sw_len*sw_num;
			}
			else {
				sw_len = sw_len_2;
				sw_offset = sw_len_1*(sw_num-1)+sw_len;
			}

			for (i=0; i<prolog_len; i++){
				addr_idx = inter_bank_idx[sb_idx][sw_offset+i] * sb_len + inner_bank_idx[sb_idx][sw_offset+i];
				sys_buffer[i] = sys_llr[addr_idx];
				parity_bufer[i] = parity2_llr[sb_idx*sb_len+sw_offset+i];
				La_buffer[i] = Le[inter_bank_idx[sb_idx][sw_offset+i]][inner_bank_idx[sb_idx][sw_offset+i]];
			}

			rd_beta_init(beta_init_buffer2, sys_buffer, parity_bufer,La_buffer, prolog_len);

			for (j=0; j<N_STATE; j++){
				beta_init_ram2[sb_idx*sw_num+(sw_num-1)][j] = beta_init_buffer2[j];
			}
		}
		//end for SW
	}//end for PU
	for(i=0;i<sb_num*sw_num-1;i++){
		fprintf(fp,"beta_init_ram(%d):\n",i);
		for(j=0;j<N_STATE; j++)
			fprintf(fp,"%f\n",beta_init_ram1[i][j]);
	}
	fclose(fp);*/
}

void compute_beta(state_metric beta, state_metric pre_beta, branch_metric gamma)
{
	state_metric tmp_beta;
	int j;
	beta_acs(tmp_beta,pre_beta,gamma[1],gamma[0]);
	for (j=0; j<N_STATE; j++){
		beta[j] = tmp_beta[j];
	}
}

void beta_acs(state_metric beta, state_metric pre_beta, float gamma_11, float gamma_10)
{
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


void compute_alpha(state_metric alpha, state_metric pre_alpha, branch_metric gamma)
{
	state_metric alpha_temp;
	int    i;
	alpha_acs(alpha_temp,pre_alpha,gamma[1],gamma[0]);

	for(i=0;i<8;i++){
		alpha[i] = alpha_temp[i];
	}
}
void alpha_acs(state_metric alpha, state_metric pre_alpha, float gamma_11, float gamma_10)
{
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

void compute_extrinsic(float *llr, float *le, state_metric alpha, state_metric beta, branch_metric gamma, float sys_la, float scaling_factor)
{
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
//	*(le) = (*(llr) - systematic - priori) * scaling_factor;
	*(le) = (*(llr) - sys_la) * scaling_factor;
}

void max_8to1(float *hmax, state_metric input)
{
	int i;
	float temp;
	temp = input[0];
	for (i = 1; i < 8; i++) {
		temp = (input[i] >= temp) ? input[i] : temp;
	}
	*(hmax) = temp;
}


void interl_addr_gen(int cb_len, int sb_len)
{
	int i;
	int f1, f2;

	int interl_idx;
	int q, r;
	int Q, R;
	int delta;
	int A[MAX_CB_LEN] = {0};
	int interl_addr[MAX_CB_LEN] = {0};

	//calculate: index, f1, f2
	for (interl_idx = 0; interleaver_size[interl_idx] != cb_len && interl_idx < 188; interl_idx++) {
		if (interl_idx == 188) {
			printf("illegal code block length!\n");
			exit(0);
		}
	}
	f1 = interleaver_f1[interl_idx];
	f2 = interleaver_f2[interl_idx];
	// pre-stored in an LUT
	for (i=0; i<4; i++){
		interl_addr[i] = (f1 * i + f2 * i *i)%cb_len; 
		A[i] = (4*f1+16*f2+8*f2*i)%cb_len;//ͨ����λʵ��
		q = interl_addr[i]/sb_len;
		r = interl_addr[i]%sb_len;
		Q = i / sb_len;
		R = i % sb_len;
		inter_bank_idx[Q][R] = q;
		inner_bank_idx[Q][R] = r;
		//		interl_address[q][i%sb_len] = r;
	}
	delta = (32*f2)%cb_len;

	for (i=0; i<cb_len-4; i++){
		A[i+4] = (A[i]+delta)%cb_len;
		interl_addr[i+4] = (A[i]+interl_addr[i])%cb_len;
		q = interl_addr[i+4]/sb_len;
		r = interl_addr[i+4]%sb_len;
		Q = (i+4) / sb_len;
		R = (i+4) % sb_len;
		inter_bank_idx[Q][R] = q;
		inner_bank_idx[Q][R] = r;
	}

}

void compute_inter_inner_idx(int cb_length, int sb_length) {
	int i,j;
	int interleave_idx, f1, f2;
	int whole_gi[6144], whole_pi[6144];
	int e1, e2, m;
	int as[4][1536] ;

	for(i=0;i<4;i++)
		for(j=0;j<1536;j++)
			as[i][j]=0;

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

void max_log_map_dec1( float *sys_llr, float *parity_llr, float *La_llr, float *alpha_init, float *beta_init, int len, int pu_no, int sw_offset)
{                                                                                                                                                          
	int i, j;                                                                                                                                                
	//float max_temp;
//	FILE *fp;
//	fp=fopen("dec1.txt","a");

 //   fprintf(fp,"beta_init:\n");
	for (j=0; j<N_STATE; j++){                                                                                                                               
		t_pre_alpha[j] = alpha_init[j];                                                                                                                        
		t_pre_beta[j] = beta_init[j];                                                                                                                          
		beta_buffer[len-1][j] = t_pre_beta[j];
//		fprintf(fp,"%f(%#010x)\n",t_pre_beta[j],*(unsigned int *)&t_pre_beta[j]);
	}                                                                                                                                                        

	/*for (i=0; i<len; i++){                                                                                                                                   
		gamma_buffer[i][0] = (-sys_llr[i] + parity_llr[i] - La_llr[i])*0.5;                                                                                    
		gamma_buffer[i][1] = (-sys_llr[i] - parity_llr[i] - La_llr[i])*0.5;                                                                                    
	}    */                                                                                                                                                    


	for (i=len-2; i>=0; i--){

		La_buffer2[i+1] =sys_llr[i+1]+La_llr[i+1];//����ϵͳ��Ϣ��������Ϣ����Le��ʱ���õ�
		sys_buffer2[i+1]=sys_llr[i+1];
//		fprintf(fp,"sys_llr(%d): %f(%#010x)\n",i+1,sys_llr[i+1],*(unsigned int *)&sys_llr[i+1]);
//		fprintf(fp,"sys+par(%d): %f(%#010x)\n",i+1,La_buffer2[i+1],*(unsigned int *)&La_buffer2[i+1]);
		//compute gamma:
		gamma_buffer[i+1][0] = (-La_buffer2[i+1] + parity_llr[i+1] )*0.5;                                                                                    
		gamma_buffer[i+1][1] = (-La_buffer2[i+1] - parity_llr[i+1] )*0.5; 

//		fprintf(fp,"gamma0(%d): %f(%#010x)\n",i+1,gamma_buffer[i+1][0],*(unsigned int *)&gamma_buffer[i+1][0]);
//		fprintf(fp,"gamma1(%d): %f(%#010x)\n",i+1,gamma_buffer[i+1][1],*(unsigned int *)&gamma_buffer[i+1][1]);

		compute_beta(t_beta, t_pre_beta, gamma_buffer[i+1]);                                                                                                   
	//	max_8to1(&max_temp, t_beta);

//       fprintf(fp,"beta(%d):",i+1);
		for (j=0; j<N_STATE; j++){                                                                                                                             
	//		t_beta[j] -= max_temp;
			t_pre_beta[j] = t_beta[j];                                                                                                                           
			beta_buffer[i][j] = t_beta[j];
//			fprintf(fp,"%f(%#010x)  ",t_beta[j],*(unsigned int *)&t_beta[j]);
		}
//		fprintf(fp,"\n");

	}

	La_buffer2[0]=sys_llr[0]+La_llr[0];
	sys_buffer2[0]=sys_llr[0];

//	fprintf(fp,"sys_llr(%d): %f(%#010x)\n",0,sys_llr[0],*(unsigned int *)&sys_llr[0]);
//	fprintf(fp,"sys+par(%d): %f(%#010x)\n",0,La_buffer2[0],*(unsigned int *)&La_buffer2[0]);

	gamma_buffer[0][0] = (-La_buffer2[0] + parity_llr[0] )*0.5;                                                                                    
	gamma_buffer[0][1] = (-La_buffer2[0] - parity_llr[0] )*0.5; 

//	fprintf(fp,"gamma0(%d): %f(%#010x)\n",0,gamma_buffer[0][0],*(unsigned int *)&gamma_buffer[0][0]);
//	fprintf(fp,"gamma1(%d): %f(%#010x)\n",0,gamma_buffer[0][1],*(unsigned int *)&gamma_buffer[0][1]);
	compute_beta(t_beta, t_pre_beta, gamma_buffer[0]);                                                                                                       
	//max_8to1(&max_temp, t_beta);
//	fprintf(fp,"beta(%d):",0);
	for (j=0; j<N_STATE; j++){                                                                                                                               
	//	t_beta[j] -= max_temp;
		//	t_pre_beta[j] = t_beta[j];                                                                                                                           
		//beta_buffer[i][j] = t_beta[j];
//		fprintf(fp,"%f(%#010x)  ",t_beta[j],*(unsigned int *)&t_beta[j]);
	}
//	fprintf(fp,"\n");

	for (j=0; j<N_STATE; j++){                                                                                                                               
		beta_init[j] = t_beta[j];                                                                                                                              
		//	beta_init[j] = beta_buffer[0][j];                                                                                                                    
	}                                                                                                                                                        
    
	//��������beta��gamma
	for (i=0;i<len;i++)
	{
		gamma_buffer2[i][0] = gamma_buffer[i][0];
		gamma_buffer2[i][1] = gamma_buffer[i][1];
		for (j=0;j<N_STATE;j++)
		{
			beta_buffer2[i][j]=beta_buffer[i][j];
		}
	}


	for (i=1; i<len+1; i++){                                                                                                                                 
		compute_alpha(t_alpha, t_pre_alpha, gamma_buffer2[i-1]);                                                                                                
	//	max_8to1(&max_temp, t_alpha);
//		fprintf(fp,"alpha(%d):",i);
		for (j=0; j<N_STATE; j++){                                                                                                                             
	//		t_alpha[j] -= max_temp;
//			fprintf(fp,"%f(%#010x)  ",t_alpha[j],*(unsigned int *)&t_alpha[j]);
		}                                                                                                                                                      
//		fprintf(fp,"\n extrinsic(%d):\n",i);
		compute_extrinsic(&t_llr, &t_le, t_pre_alpha, beta_buffer2[i-1], gamma_buffer2[i-1], La_buffer2[i-1], SCALING_FACTOR);  
//		fprintf(fp,"%f(%#010x)\n",t_le,*(unsigned int *)&t_le);
//		fprintf(fp,"llr(%d):%f(%#010x)\n",i,t_llr,*(unsigned int *)&t_llr);
		Le[pu_no][sw_offset+i-1] = t_le+sys_buffer2[i-1] ;
//		fprintf(fp,"le+sys(%d):%f(%#010x)\n",i,Le[pu_no][sw_offset+i-1],*(unsigned int *)&Le[pu_no][sw_offset+i-1]);
		
	//	Le_llr[i-1] = t_le ;//+ sys_llr[i];                                                                                                                    
		hard_bit[pu_no*sb_len+sw_offset+i-1] = (t_llr > 0) ? 0 : 1;                                                                                               
		for (j=0; j<N_STATE; j++){                                                                                                                             
			t_pre_alpha[j] = t_alpha[j];                                                                                                                         
		}                                                                                                                                                      
	}                                                                                                                                                        
	for (j=0; j<N_STATE; j++){                                                                                                                               
		alpha_init[j] = t_alpha[j];                                                                                                                            
	}
//	fprintf(fp,"\n\n");
//	fclose(fp);
}
void max_log_map_dec2( float *parity_llr, float *La_llr, float *alpha_init, float *beta_init, int len, int pu_no, int sw_offset)
{                                                                                                                                                          
	int i, j;                                                                                                                                                
//	float max_temp;


	for (j=0; j<N_STATE; j++){                                                                                                                               
		t_pre_alpha[j] = alpha_init[j];                                                                                                                        
		t_pre_beta[j] = beta_init[j];                                                                                                                          
		beta_buffer[len-1][j] = t_pre_beta[j];                                                                                                                 
	}                                                                                                                                                        

	/*for (i=0; i<len; i++){                                                                                                                                   
		gamma_buffer[i][0] = (-sys_llr[i] + parity_llr[i] - La_llr[i])*0.5;                                                                                    
		gamma_buffer[i][1] = (-sys_llr[i] - parity_llr[i] - La_llr[i])*0.5;                                                                                    
	}    */                                                                                                                                                    


	for (i=len-2; i>=0; i--){ 

		La_buffer2[i+1] =La_llr[i+1];//����ϵͳ��Ϣ��������Ϣ����Le��ʱ���õ�
	//	sys_buffer2[i+1]=sys_llr[i+1];
		//compute gamma:
		gamma_buffer[i+1][0] = ( + parity_llr[i+1] - La_llr[i+1])*0.5;                                                                                    
		gamma_buffer[i+1][1] = ( - parity_llr[i+1] - La_llr[i+1])*0.5; 

		compute_beta(t_beta, t_pre_beta, gamma_buffer[i+1]);                                                                                                   
	//	max_8to1(&max_temp, t_beta);


		for (j=0; j<N_STATE; j++){                                                                                                                             
	//		t_beta[j] -= max_temp;
			t_pre_beta[j] = t_beta[j];                                                                                                                           
			beta_buffer[i][j] = t_beta[j];                                                                                                                       
		}                                                                                                                                                      

	}                                                                                                                                                        

	La_buffer2[0] =La_llr[0];
	gamma_buffer[0][0] = ( + parity_llr[0] - La_llr[0])*0.5;                                                                                    
	gamma_buffer[0][1] = ( - parity_llr[0] - La_llr[0])*0.5; 
	compute_beta(t_beta, t_pre_beta, gamma_buffer[0]);                                                                                                       
	//max_8to1(&max_temp, t_beta);
	for (j=0; j<N_STATE; j++){                                                                                                                               
	//	t_beta[j] -= max_temp;
		//	t_pre_beta[j] = t_beta[j];                                                                                                                           
		//beta_buffer[i][j] = t_beta[j];                                                                                                                       
	}                                                                                                                                                        

	for (j=0; j<N_STATE; j++){                                                                                                                               
		beta_init[j] = t_beta[j];                                                                                                                              
		//	beta_init[j] = beta_buffer[0][j];                                                                                                                    
	}                                                                                                                                                        

	//��������beta��gamma
	for (i=0;i<len;i++)
	{
		gamma_buffer2[i][0] = gamma_buffer[i][0];
		gamma_buffer2[i][1] = gamma_buffer[i][1];
		for (j=0;j<N_STATE;j++)
		{
			beta_buffer2[i][j]=beta_buffer[i][j];
		}
	}

	for (i=1; i<len+1; i++){                                                                                                                                 
		compute_alpha(t_alpha, t_pre_alpha, gamma_buffer2[i-1]);                                                                                                
	//	max_8to1(&max_temp, t_alpha);
		for (j=0; j<N_STATE; j++){                                                                                                                             
	//		t_alpha[j] -= max_temp;
		}                                                                                                                                                      

		compute_extrinsic(&t_llr, &t_le, t_pre_alpha, beta_buffer2[i-1], gamma_buffer2[i-1], La_buffer2[i-1], SCALING_FACTOR);  
		
		Le[inter_bank_idx[pu_no][sw_offset+i-1]][inner_bank_idx[pu_no][sw_offset+i-1]] = t_le ;
	//	Le_llr[i-1] = t_le ;//+ sys_llr[i];                                                                                                                                                                                                             
		for (j=0; j<N_STATE; j++){                                                                                                                             
			t_pre_alpha[j] = t_alpha[j];                                                                                                                         
		}                                                                                                                                                      
	}                                                                                                                                                        
	for (j=0; j<N_STATE; j++){                                                                                                                               
		alpha_init[j] = t_alpha[j];                                                                                                                            
	}                                                                                                                                                        
}

int turbo_decoder(int *decode_bit, float *sys_llr, float *parity1_llr, float *parity2_llr)
{
	int i, j;
	int iter, sb_idx, sw_idx; //������
	int sw_offset; //������
//	int addr_idx;
	int ack = 0;
	int ack_flag=0;
	int flag;
//	FILE *fp;

	flag = SPU();

//	interl_addr_gen(cb_len, sb_len);
	compute_inter_inner_idx(cb_len, sb_len);

	compute_beta_init(sys_llr, parity1_llr, parity2_llr, cb_len,flag);

	for (j=0; j<N_STATE; j++){
		beta_init_ram1[PU_NUM*sw_num-1][j] = beta_init_value1[j];
		beta_init_ram2[PU_NUM*sw_num-1][j] = beta_init_value2[j];
	}
//	fp=fopen("beta_init.txt","w");
	for (j=0; j<N_STATE; j++){
//		fprintf(fp,"  %#010x,\n",*(unsigned int *)&beta_init_value1[j]);
	}
//	fprintf(fp,"\n");
	for(j=0;j<N_STATE; j++){
//		fprintf(fp,"  %#010x,\n",*(unsigned int *)&beta_init_value2[j]);
	}
//	fclose(fp);


	for (iter=0; iter<max_iter_num; iter++){

		//DEC1
		for (sb_idx=0; sb_idx<sb_num; sb_idx++){

			//alpha_init, beta_init
			for (j=0; j<N_STATE; j++){
				alpha_init_buffer1[j] = alpha_init_ram1[sb_idx][j];
			}

			for (sw_idx=0; sw_idx<sw_num-1; sw_idx++){

				sw_len = sw_len_1;
				sw_offset = sw_len*sw_idx;

				for(j=0; j<N_STATE; j++){
					beta_init_buffer1[j] = beta_init_ram1[sb_idx*sw_num+sw_idx][j];
				}

				for (i=0; i<sw_len; i++){
					sys_buffer[i] = sys_llr[sb_idx*sb_len+sw_offset+i];
					parity_bufer[i] = parity1_llr[sb_idx*sb_len+sw_offset+i];
					La_buffer[i] = Le[sb_idx][sw_offset+i];
				}
				
				max_log_map_dec1(sys_buffer, parity_bufer, La_buffer, alpha_init_buffer1, beta_init_buffer1, sw_len, sb_idx, sw_offset);

				/*for (i=0; i<sw_len; i++){
					Le[sb_idx][sw_offset+i] = Le_buffer[i];
				}*/
				
				for (j=0; j<N_STATE; j++){
					beta_init_ram1[sb_idx*sw_num+sw_idx][j] = beta_init_buffer1[j];
				}
				
				/*for(i=0; i<sw_len; i++){
					decode_bit[sb_idx*sb_len+sw_offset+i] = hard_bit[sb_idx*sb_len+sw_offset+i];
				}*/

			}
			
			// the last SW
			if (flag == 1){ //
				sw_len = sw_len_1;
				sw_offset = sw_len*(sw_num-1);
			}
			else {
				sw_len = sw_len_2;
				sw_offset = sw_len_1*(sw_num-1);
			}

			for(j=0; j<N_STATE; j++){
				beta_init_buffer1[j] = beta_init_ram1[sb_idx*sw_num+sw_num-1][j];
			}

			for (i=0; i<sw_len; i++){
				sys_buffer[i] = sys_llr[sb_idx*sb_len+sw_offset+i];
				parity_bufer[i] = parity1_llr[sb_idx*sb_len+sw_offset+i];
				La_buffer[i] = Le[sb_idx][sw_offset+i];
			}

			max_log_map_dec1(sys_buffer, parity_bufer, La_buffer, alpha_init_buffer1, beta_init_buffer1, sw_len, sb_idx, sw_offset);

			/*for (i=0; i<sw_len; i++){
				Le[sb_idx][sw_offset+i] = Le_buffer[i];
			}*/

			for (j=0; j<N_STATE; j++){
				beta_init_ram1[sb_idx*sw_num+(sw_num-1)][j] = beta_init_buffer1[j];
			}


			/*for(i=0; i<sw_len; i++){
				decode_bit[sb_idx*sb_len+sw_offset+i] = hard_bit[sb_idx*sb_len+sw_offset+i];
			}*/
			
			// end for sliding window

			for (j=0; j<N_STATE; j++){
				alpha_init_ram1[sb_idx][j] = alpha_init_buffer1[j];
			}
		
		}// end for PU 
		

		for (sb_idx=PU_NUM-2; sb_idx>=0; sb_idx--){
			for (j=0; j<N_STATE; j++){
				alpha_init_ram1[sb_idx+1][j] = alpha_init_ram1[sb_idx][j];
			}
		}
		for (sb_idx=0; sb_idx<PU_NUM*sw_num-1; sb_idx++){
			for (j=0; j<N_STATE; j++){
				beta_init_ram1[sb_idx][j] = beta_init_ram1[sb_idx+1][j];
			}
		}	
		for (j=0; j<N_STATE; j++){
			alpha_init_ram1[0][j] = unequal_prb_init[j];
			beta_init_ram1[PU_NUM*sw_num-1][j] = beta_init_value1[j];
		}

        //CRC Check:
		ack_flag  = crc_check(hard_bit, cb_len, CRC_TYPE);
		if (ack_flag==0)
			ack = 0;
		else
			ack++;
		
		if (ack == CRC_CHECK_NUM){

			for (i=0;i<CB_LEN;i++)
			{
				decode_bit[i]=hard_bit[i];
			}
			return iter;
		}

		//DEC2
		for (sb_idx=0; sb_idx<sb_num; sb_idx++){

			//alpha_init, beta_init
			for (j=0; j<N_STATE; j++){
				alpha_init_buffer2[j] = alpha_init_ram2[sb_idx][j];
			}

			for (sw_idx=0; sw_idx<sw_num-1; sw_idx++){

				sw_len = sw_len_1;
				sw_offset = sw_len*sw_idx;

				for(j=0; j<N_STATE; j++){
					beta_init_buffer2[j] = beta_init_ram2[sb_idx*sw_num+sw_idx][j];
				}

				for (i=0; i<sw_len; i++){
				//	addr_idx = inter_bank_idx[sb_idx][sw_offset+i] * sb_len + inner_bank_idx[sb_idx][sw_offset+i];
				//	sys_buffer[i] = sys_llr[addr_idx];
					parity_bufer[i] = parity2_llr[sb_idx*sb_len+sw_offset+i];
					La_buffer[i] = Le[inter_bank_idx[sb_idx][sw_offset+i]][inner_bank_idx[sb_idx][sw_offset+i]];
				}

				max_log_map_dec2( parity_bufer, La_buffer, alpha_init_buffer2, beta_init_buffer2, sw_len, sb_idx, sw_offset);

				/*for (i=0; i<sw_len; i++){
					Le[inter_bank_idx[sb_idx][sw_offset+i]][inner_bank_idx[sb_idx][sw_offset+i]] = Le_buffer[i];
				}*/
				for (j=0; j<N_STATE; j++){
					beta_init_ram2[sb_idx*sw_num+sw_idx][j] = beta_init_buffer2[j];
				}
							
			}
			// the last SW
			if (flag ==1){
				sw_len = sw_len_1;
				sw_offset = sw_len*(sw_num-1);
			}
			else {
				sw_len = sw_len_2;
				sw_offset = sw_len_1*(sw_num-1);
			}

			for(j=0; j<N_STATE; j++){
				beta_init_buffer2[j] = beta_init_ram2[sb_idx*sw_num+sw_num-1][j];
			}

			for (i=0; i<sw_len; i++){
			//	addr_idx = inter_bank_idx[sb_idx][sw_offset+i] * sb_len + inner_bank_idx[sb_idx][sw_offset+i];
			//	sys_buffer[i] = sys_llr[addr_idx];
				parity_bufer[i] = parity2_llr[sb_idx*sb_len+sw_offset+i];
				La_buffer[i] = Le[inter_bank_idx[sb_idx][sw_offset+i]][inner_bank_idx[sb_idx][sw_offset+i]];
			}

			max_log_map_dec2( parity_bufer, La_buffer, alpha_init_buffer2, beta_init_buffer2, sw_len, sb_idx, sw_offset);

			/*for (i=0; i<sw_len; i++){
				Le[inter_bank_idx[sb_idx][sw_offset+i]][inner_bank_idx[sb_idx][sw_offset+i]] = Le_buffer[i];
			}*/
			for (j=0; j<N_STATE; j++){
				beta_init_ram2[sb_idx*sw_num+(sw_num-1)][j] = beta_init_buffer2[j];
			}
			
			
			for (j=0; j<N_STATE; j++){
				alpha_init_ram2[sb_idx][j] = alpha_init_buffer2[j];
			}
			
		}// end for PU


		for (sb_idx=PU_NUM-2; sb_idx>=0; sb_idx--){
			for (j=0; j<N_STATE; j++){
				alpha_init_ram2[sb_idx+1][j] = alpha_init_ram2[sb_idx][j];
			}
		}
		for (sb_idx=0; sb_idx<PU_NUM*sw_num-1; sb_idx++){
			for (j=0; j<N_STATE; j++){
				beta_init_ram2[sb_idx][j] = beta_init_ram2[sb_idx+1][j];
			}
		}	
		for (j=0; j<N_STATE; j++){
			alpha_init_ram2[0][j] = unequal_prb_init[j];
			beta_init_ram2[PU_NUM*sw_num-1][j] = beta_init_value2[j];
		}

	}
	for (i=0;i<CB_LEN;i++)
	{
		decode_bit[i]=hard_bit[i];
	}
	return iter;
}

void test_interl_addr_gen()
{
	int i, j;
	int cb_len = 6144;
	int sb_len = 6144/4;

	FILE *fp;

	interl_addr_gen(cb_len, sb_len);
	fp = fopen("debug.txt", "w");
	for (i=0; i<4; i++){
		for (j=0; j<6144/4; j++){
			fprintf(fp, "%d %d\n", inter_bank_idx[i][j], inner_bank_idx[i][j]);
		}
	}
	fclose(fp);
}
