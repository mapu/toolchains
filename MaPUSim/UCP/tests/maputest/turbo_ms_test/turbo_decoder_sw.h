

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "qpp_interleaver.h"

#define DECODER_TYPE	1 //

#define MAX_ITER_NUM	8
#define INFTY			1E20
#define TERMINATED      1 

/*generator parameters*/
#define G_ROW_1			13
#define G_ROW_2			15
#define COLUMN_OF_G		4	

#define PU_NUM			8 //Parallel Process Unit Number
#define SW_NUM			6//Sliding window length 

/*generator structure*/
typedef struct 
{
	int N_num_row;
	int K_num_col;
	int *g_matrix;
} TURBO_G;

/*trellis structure*/
/*
mx_nextout(mx_lastout)Ϊ��(ǰ)�����������ַ:
����:״̬��, ����:4
ÿ�е�һ�к͵�����Ϊ��(ǰ)�������(1��-1),�ڶ��к͵�����Ϊ��֮��Ӧ�����(1��-1).

mx_nextstat(mx_laststat)Ϊ��(ǰ)��״̬������ַ:
����:״̬��, ����:2
���б�ʾ����Ϊ1(0)ʱ��Ӧ�ĺ�(ǰ)��״̬.
*/
typedef struct{
	int *mx_nextout;
	int *mx_nextstart;
	int *mx_lastout;
	int *mx_laststart; 
} TURBO_TRELLIS;

TURBO_TRELLIS turbo_trellis;
TURBO_G turbo_g;

int M_num_reg = COLUMN_OF_G-1;
int n_states = 8; //2^M_num_reg;


void int2bin(int intstat, int *bin_stat, int length);
int bin2int(int *binseq, int length);
int gen_g_matrix(int k_column, int g_row1, int g_row2, int *mx_g_turbo);
int encode_bit(int inbit, int *stat);
void gen_trellis();
void TurboCodingInit();
void rsc_encode(int *source, int *rsc, int terminated, int len_info);
void encoderm_turbo(int *source, int *send_turbo, int len_info,int *index_intlvr);
void TurboEnCoding(int *source, int *coded_source,  int source_length,int *index_intlvr);
void interl_addr_gen(int K, int f1, int f2, int *interl_addr);
void interl_i(int K, int flag, int *input, int *output, int *interl_addr);
void interl_d(int K, int flag, float *input, float *output, int *interl_addr);
void demultiplex(float *rec_turbo, int len_info, float *yk_turbo,int*index_intlvr);
float get_max(float *data_seq, int length);

void TurboDecoding(float *flow_for_decode, int *flow_decoded,int flow_length, int*index_intlvr);
void max_Log_MAP_decoder(float *recs_turbo, float *La_turbo, int terminated, float *LLR_all_turbo, int len_total);

void TurboDecoding_PU(float *flow_for_decode, int *flow_decoded,int flow_length, int*interl_addr);
void max_Log_MAP_decoder_pu(float *recs_turbo, float *La_turbo, int terminated, float *LLR_all_turbo, int sub_info_length, int pu_no, float *alpha_log_init, float *beta_log_init);

void TurboDecoding_sw(float *flow_for_decode, int *flow_decoded, int flow_length, int *interl_addr);
void max_Log_MAP_decoder_sw(float *recs_turbo, float *La_turbo, int terminated, float *LLR_all_turbo, int info_length, float *alpha_log_init, float *beta_log_init);


void decision(float *LLR_seq, int length, int *output);
void TurboCodingRelease();

void mgrns(float mean,float sigma,float seed,int n,float *a);
void AWGN(float *r, float *send, float sigma, int totallength);

void test_turbo_1024();
void test_interl_addr();

