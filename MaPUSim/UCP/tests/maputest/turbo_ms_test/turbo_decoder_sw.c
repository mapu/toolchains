#include "stdlib.h"
#include "stdio.h"
#include "turbo_decoder_sw.h"


//void TurboDecoding_sw(double *flow_for_decode, int *flow_decoded, int flow_length, int *interl_addr) ���������ֱ�ӵ��õĴ����������
//flag = 1: ��֯�� flag = 0; �⽻֯
void interl_i(int K, int flag, int *input, int *output, int *interl_addr)
{
	int i;
	if (flag == 1){
		for (i=0; i<K; i++){
			output[i] = input[interl_addr[i]];
		}
	}
	else{
		for (i=0; i<K; i++){
			output[interl_addr[i]] = input[i];
		}
	}

}
void interl_d(int K, int flag, float *input, float *output, int *interl_addr)
{
	int i;
	/*FILE *fp_debug;
	fp_debug = fopen("debug.txt","w");
	for (i=0; i<K; i++){
		fprintf(fp_debug, "%d\n", interl_addr[i]);
	}
	fclose(fp_debug);*/
	if (flag == 1){
		for (i=0; i<K; i++){
			output[i] = input[interl_addr[i]];
		}
	}
	else{
		for (i=0; i<K; i++){
			output[interl_addr[i]] = input[i];
		}
	}

}

/*---------------------------------------------------------------
����:
	double get_max(double *data_seq, int length)
����:
	�õ����е����ֵ.
����:
	�������:
		data_seq - ������ַ.
		length - ���г���.
	�������:
		��
����ֵ:
	�����е����ֵ��
---------------------------------------------------------------*/
float get_max(float *data_seq, int length)
{
	int i;		/* ѭ������ */
	float temp;
	temp = *(data_seq+0);
	for (i=1; i<length; i++)
	{
		if (temp < *(data_seq+i))
		{
			temp = *(data_seq+i);
		}
	}
	return temp;
}

void decision(float *LLR_seq, int length, int *output)
{
	int i;

	for (i=0; i<length; i++){
		if (*(LLR_seq+i) < 0){
			*(output+i) = 0;
		}

		else{
			*(output+i) = 1;
		}
	}		
}

/*---------------------------------------------------------------
����:
	void demultiplex(double *rec_turbo, int len_info, double *yk_turbo)
����:
	�⸴��.
����:
	�������:
		rec_turbo -�������������ַ.
		len_info - ����������г��ȣ�
	�������:
		yk_turbo - �⸴�ú����������ַ.
����ֵ:
	�ޣ�
---------------------------------------------------------------*/
void demultiplex(float *rec_turbo, int len_info, float *yk_turbo,int*index_intlvr)
{
	int i;			/* ѭ������ */

	int len_total = len_info+M_num_reg;		/* �ܳ��� */

	float *info2, *inted_info2;			/* ��Ϣλ�ͽ�֯�����Ϣλ */

	if ((info2=(float *)malloc(len_info*sizeof(float)))==NULL)
	{
	  printf("\n fail to allocate memory of info2 \n");
	  exit(1);  
	}
	if ((inted_info2=(float *)malloc(len_info*sizeof(float)))==NULL)
	{
	  printf("\n fail to allocate memory of inted_info2 \n");
	  exit(1);  
	}
	
	/* ����Ϣ���� */
	for(i=0; i<len_info; i++)
	{
		*(info2+i) = *(yk_turbo+2*i) = *(rec_turbo+3*i);
		*(yk_turbo+2*i+1) = *(rec_turbo+3*i+1);
		*(yk_turbo+2*len_total+2*i+1) = *(rec_turbo+3*i+2);
	}

	/* ��֯��Ϣ���� */

	interl_d(len_info, 1, info2, inted_info2, index_intlvr);

	for (i=0; i<len_info; i++)
	{
		*(yk_turbo+2*len_total+2*i) = *(inted_info2+i);
	}

	/* ��β���� */
	for (i=0; i<2*M_num_reg; i++)
	{
		*(yk_turbo+2*len_info+i) = *(rec_turbo+3*len_info+i);
		*(yk_turbo+2*len_total+2*len_info+i) = *(rec_turbo+3*len_info+2*M_num_reg+i);
	}

	free(info2);
	free(inted_info2);
}
void max_Log_MAP_decoder_sw(float *recs_turbo, float *La_turbo, int terminated, float *LLR_all_turbo, int info_length, float *alpha_log_init, float *beta_log_init)
{
	int i, j, sw;
	int win_len, len_total, sub_len, max_sub_len, idx;

	float *alpha_log, *beta_log, *gamma_log;
	float *temp0, *temp1;
	float *alpha_max;
	float tempx, tempy;
	float *alpha_boarder;

	win_len = info_length / SW_NUM;
	len_total = info_length +M_num_reg;
	max_sub_len = win_len+M_num_reg;


	alpha_log = (float *)malloc(n_states*(max_sub_len+1)*sizeof(float));
	beta_log = (float *)malloc(n_states*(max_sub_len+1)*sizeof(float));
	gamma_log = (float *)malloc(n_states*max_sub_len*2*sizeof(float));
	alpha_max = (float *)malloc((max_sub_len+1)*sizeof(float));

	temp0 = (float *)malloc(n_states*sizeof(float));
	temp1 = (float *)malloc(n_states*sizeof(float));
	alpha_boarder = (float *)malloc(n_states*sizeof(float));


	for (sw=0; sw<SW_NUM; sw++){

		if (sw != SW_NUM-1){
			sub_len = win_len;
		}
		else{
			sub_len = max_sub_len;
		}

		//state initialization
		if (sw == 0){
			for (j=0; j<n_states; j++){
				*(alpha_log + j*(sub_len+1) + 0) = *(alpha_log_init+j);
				*(beta_log +j*(sub_len+1)+sub_len) = *(beta_log_init+sw*n_states+j);
			}
		}
		else{
			for (j=0; j<n_states; j++){
				*(alpha_log + j*(sub_len+1) + 0) = *(alpha_boarder+j);
				*(beta_log + j*(sub_len+1)+sub_len) = *(beta_log_init+sw*n_states+j);
			}
		}


		// gamma computation 
		for (i=0; i<sub_len; i++){
			idx = sw*win_len+i;
			for (j=0; j<n_states; j++){
				*(gamma_log+j*sub_len*2+i*2+0) = -*(recs_turbo+2*idx) + *(recs_turbo+2*idx+1)*(*(turbo_trellis.mx_nextout+j*4+1)) - *(La_turbo+idx)/2;
				*(gamma_log+j*sub_len*2+i*2+1) = *(recs_turbo+2*idx) + *(recs_turbo+2*idx+1)*(*(turbo_trellis.mx_nextout+j*4+3)) + *(La_turbo+idx)/2;
			}
		}
		//alpha computation 
		for (i=1; i<sub_len+1; i++){
			for (j=0; j<n_states; j++){
				tempx = *(gamma_log+(*(turbo_trellis.mx_laststart+j*2+0))*sub_len*2+(i-1)*2+0)
					+ *(alpha_log+(*(turbo_trellis.mx_laststart+j*2+0))*(sub_len+1)+i-1);
				tempy = *(gamma_log+(*(turbo_trellis.mx_laststart+j*2+1))*sub_len*2+(i-1)*2+1)
					+ *(alpha_log+(*(turbo_trellis.mx_laststart+j*2+1))*(sub_len+1)+i-1);
				*(alpha_log+j*(sub_len+1)+i) = tempx>tempy?tempx:tempy;

			}
			//normalization 
			for (j=0; j<n_states; j++){
				if (*(alpha_max+i) < *(alpha_log+j*(sub_len+1)+i)){
					*(alpha_max+i) = *(alpha_log+j*(sub_len+1)+i);
				}
			}
			for (j=0; j<n_states; j++){
				*(alpha_log+j*(sub_len+1)+i) = *(alpha_log+j*(sub_len+1)+i) - *(alpha_max+i);	
			}

		}
		for (j=0; j<n_states; j++){
			*(alpha_boarder+j) = *(alpha_log+j*(sub_len+1)+sub_len);
		}


		// beta computation 
		for (i=sub_len-1; i>0; i--){
			for (j=0; j<n_states; j++){
				tempx = *(gamma_log+j*sub_len*2+i*2+0) 
					+ *(beta_log+(*(turbo_trellis.mx_nextstart+j*2+0))*(sub_len+1)+i+1);
				tempy = *(gamma_log+j*sub_len*2+i*2+1) 
					+ *(beta_log+(*(turbo_trellis.mx_nextstart+j*2+1))*(sub_len+1)+i+1);
				*(beta_log+j*(sub_len+1)+i) = tempx>tempy?tempx:tempy;
			}

			for (j=0; j<n_states; j++){
				*(beta_log+j*(sub_len+1)+i) = *(beta_log+j*(sub_len+1)+i) - *(alpha_max+i+1);
			}
		}
		for (j=0; j<n_states; j++){
			*(beta_log_init+sw*n_states+j) = *(beta_log+j*(sub_len+1)+sub_len);
		}
		// LLR 
		for (i=0; i<sub_len; i++){
			idx = sw*win_len+i;
			for (j=0; j<n_states; j++){
				*(temp0+j) = *(gamma_log+(*(turbo_trellis.mx_laststart+j*2+0))*sub_len*2+i*2+0) 
					+ *(alpha_log+*(turbo_trellis.mx_laststart+j*2+0)*(sub_len+1)+i)
					+ *(beta_log+ j*(sub_len+1)+i+1);

				*(temp1+j) = *(gamma_log+(*(turbo_trellis.mx_laststart+j*2+1))*sub_len*2+i*2+1) 
					+ *(alpha_log+*(turbo_trellis.mx_laststart+j*2+1)*(sub_len+1)+i)
					+ *(beta_log+j*(sub_len+1)+i+1);
			}
			*(LLR_all_turbo+idx) = get_max(temp1, n_states) - get_max(temp0, n_states);
		}

	}


	free(alpha_log);
	free(beta_log);
	free(gamma_log);
	free(alpha_max);

	free(temp0);
	free(temp1);
	free(alpha_boarder);


}

void TurboDecoding_sw(float *flow_for_decode, int *flow_decoded, int flow_length, int *interl_addr)
{
	int i;
	int iter;
	int blk_len, length_total;

	float *alpha_log_init1, *alpha_log_init2, *beta_log_init1, *beta_log_init2;
	float *alpha_log_temp1, *alpha_log_temp2, *beta_log_temp1, *beta_log_temp2;

	float *yk_turbo, *La_turbo, *Le_turbo, *LLR_all_turbo;
	int *temp_out;

	blk_len = (flow_length-4*M_num_reg)/3;
	length_total = blk_len+M_num_reg;

	alpha_log_init1 = (float *)malloc(n_states*sizeof(float));
	beta_log_init1 = (float *)malloc(n_states*SW_NUM*sizeof(float));
	alpha_log_init2 = (float *)malloc(n_states*sizeof(float));
	beta_log_init2 = (float *)malloc(n_states*SW_NUM*sizeof(float));

	alpha_log_temp1 = (float *)malloc(n_states*sizeof(float));
	alpha_log_temp2 = (float *)malloc(n_states*sizeof(float));
	beta_log_temp1 = (float *)malloc(n_states*sizeof(float));
	beta_log_temp2 = (float *)malloc(n_states*sizeof(float));

	yk_turbo = (float *)malloc(4*length_total*sizeof(float));
	La_turbo = (float *)malloc(length_total*sizeof(float));
	Le_turbo = (float *)malloc(length_total*sizeof(float));
	LLR_all_turbo = (float *)malloc(length_total*sizeof(float));


	temp_out = (int *)malloc(length_total*sizeof(int));

	for (i=0; i<flow_length; i++){
		flow_for_decode[i] *= 0.5;
	}
	demultiplex(flow_for_decode, blk_len, yk_turbo,interl_addr);


	for (i=0; i<length_total; i++)
	{
		*(La_turbo+i) = *(Le_turbo+i) = *(LLR_all_turbo+i) = 0;
	}

	for (i=0; i<n_states; i++){
		*(alpha_log_init1+i) = (log(1.0/SW_NUM));
		*(alpha_log_init2+i) = log(1.0/SW_NUM);
	}

	*(alpha_log_init1+0) = 0.0; *(alpha_log_init2+0) = 0.0;
	for (i=1; i<n_states; i++){
		*(alpha_log_init1+i) = (float)-INFTY;
		*(alpha_log_init2+i) = (float)-INFTY;
	}
	for (i=0; i<n_states*SW_NUM; i++){
		*(beta_log_init1+i) = log(1.0/SW_NUM);
		*(beta_log_init2+i) = log(1.0/SW_NUM);
	}

	for (iter=0; iter<MAX_ITER_NUM; iter++){
		interl_d(blk_len, 0, Le_turbo, La_turbo, interl_addr);

		for (i=blk_len; i<length_total; i++){
			*(La_turbo+i) = 0;
		}

		//DEC 1
		max_Log_MAP_decoder_sw(yk_turbo, La_turbo, TERMINATED, LLR_all_turbo, blk_len, alpha_log_init1, beta_log_init1);

		for (i=0; i<length_total; i++){
			*(Le_turbo+i) = *(LLR_all_turbo+i) - *(La_turbo+i) - 2*(*(yk_turbo+2*i));
		}

		interl_d(blk_len, 1, Le_turbo, La_turbo, interl_addr);

		for (i=blk_len; i<length_total; i++){
			*(La_turbo+i) = 0.0;
		}

		//DEC2

		max_Log_MAP_decoder_sw(yk_turbo+2*length_total, La_turbo, 1, LLR_all_turbo, blk_len, alpha_log_init2, beta_log_init2);

		for(i=0; i<length_total; i++){
			*(Le_turbo+i) = *(LLR_all_turbo+i) - *(La_turbo+i) - 2*(*(yk_turbo+2*length_total+2*i));
		}
	}

	decision(LLR_all_turbo, length_total, temp_out);


	interl_i(blk_len, 0, temp_out, flow_decoded, interl_addr);

	free(alpha_log_init1);
	free(beta_log_init1);
	free(alpha_log_init2);
	free(beta_log_init2);
	free(alpha_log_temp1);
	free(alpha_log_temp2);
	free(beta_log_temp1);
	free(beta_log_temp2);
	free(yk_turbo);
	free(La_turbo);
	free(Le_turbo);
	free(LLR_all_turbo);
	free(temp_out);

}
