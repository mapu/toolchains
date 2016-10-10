/*============================================================================*/
//  Information
/*============================================================================*/
/* 
*  @file    turbodec_z.c
*  @brief   
*  @author  wangxiaoqin
*  @date    2014/01/28
/*----------------------------------------------------------------------------*/
#ifndef TURBODEC_Z_C
#define TURBODEC_Z_C

/*============================================================================*/
//  Header files
/*============================================================================*/
#include "turbodec_z.h"

extern const int interleaver_size[188];
extern const int interleaver_f1[188];
extern const int interleaver_f2[188];
extern const int interleaver_par[188][2];
extern const int interleaver_firstper[188][64];
extern const int interleaver_secondper[188][6144];
/*===========================================================================*/
// Function definition
/*===========================================================================*/

/*----------------------------------------------------------------------------*/
// void turbo_decoder (int* dec_codeblock, double* systematic, double* parity1, double* parity2, int datasize, int iteratenum, double scalfactor)
/*----------------------------------------------------------------------------*/
/*
* @brief: turbo decoder
* @dec_codeblock: output [codeblock_size    ]
* @systematic   : input  [codeblock_size + 4]
* @parity1      : input  [codeblock_size + 4]
* @parity2      : input  [codeblock_size + 4]
* @datasize     : input  the length of input vectors = codeblock_size + 4
* @iteratenum   : the number of iteration
* @scalfactor   : scaling factor (default = 0.75)
*/
/*----------------------------------------------------------------------------*/
void turbo_decoder_z (int* dec_codeblock, float* systematic, float* parity1, float* parity2, int datasize, int iteratenum, float scalfactor)
{
	// Variables:
	int    i;
	int    j;
	int    codeblock_size;

	int*    itl_index;
	float* systematic1;
	float* parity1_tmp;
	float* systematic2;
	float* parity2_tmp;
	float* pri_llr1;
	float* pri_llr2;
	float* ext_llr1;
	float* ext_llr2;
	float* llr;
	float* di_llr;
	float  tail[12];
	FILE * fp_soft;
	FILE * fp_log_s;

	// Initialization:
	codeblock_size = datasize - 4;

	itl_index   = (int*) malloc (sizeof(int) * codeblock_size);
    systematic1 = (float*) malloc (sizeof(float) * (codeblock_size+3));
	systematic2 = (float*) malloc (sizeof(float) * (codeblock_size+3));
	parity1_tmp = (float*) malloc (sizeof(float) * (codeblock_size+3));
	parity2_tmp = (float*) malloc (sizeof(float) * (codeblock_size+3));
	pri_llr1    = (float*) malloc (sizeof(float) * (codeblock_size+3));
	pri_llr2    = (float*) malloc (sizeof(float) * (codeblock_size+3));
	ext_llr1    = (float*) malloc (sizeof(float) * (codeblock_size+3));
	ext_llr2    = (float*) malloc (sizeof(float) * (codeblock_size+3));
	llr         = (float*) malloc (sizeof(float) * (codeblock_size+3));
	di_llr      = (float*) malloc (sizeof(float) * (codeblock_size+3));

	make_interleaver_table_z(itl_index, codeblock_size);

    for (i=0; i<4; i++)
    {
		tail[3*i+0] = *(systematic + codeblock_size + i);
		tail[3*i+1] = *(parity1    + codeblock_size + i);
		tail[3*i+2] = *(parity2    + codeblock_size + i);
    }

	for (i=0; i<codeblock_size; i++)
	{
		*(systematic1 + i) = *(systematic + i);
		*(parity1_tmp + i) = *(parity1    + i);
		*(systematic2 + i) = *(systematic + *(itl_index+i));
		*(parity2_tmp + i) = *(parity2    + i);
	}
	for (i=0; i<3; i++)
	{
		*(systematic1 + codeblock_size + i) = tail[2*i  ];
		*(parity1_tmp + codeblock_size + i) = tail[2*i+1];
	}
	for (i=3; i<6; i++)
	{
		*(systematic2 + codeblock_size + i-3) = tail[2*i  ];
		*(parity2_tmp + codeblock_size + i-3) = tail[2*i+1];
	}

	for (i=0; i<codeblock_size+3; i++)
	{
		*(pri_llr1+i) = 0.0;
		*(pri_llr2+i) = 0.0;
	}

	fp_log_s = fopen("Extrinsic_Log_s.log","w+");
	// Execute:
	for (j=0; j<iteratenum; j++)
	{
		fprintf(fp_log_s,"Interation:%d.\n\n",j);
		component_decoder1_z(ext_llr1, systematic1, parity1_tmp, pri_llr1, codeblock_size+3);

		for (i=0; i<codeblock_size; i++)
		{
			*(pri_llr2+i) = *(ext_llr1+(*(itl_index+i))) * scalfactor;
			fprintf(fp_log_s,"%4d::sys:%10.6f,par:%10.6f,pri:%10.6f:::Pri:%10.6f,Pri+Sys1:%10.6f.sys_prime2:%10.6f.\n",i,*(systematic1+i),*(parity1_tmp+i),*(pri_llr1+i),*(ext_llr1+i),*(ext_llr1+i)+*(systematic1+i),*(ext_llr1+(*(itl_index+i)))+*(systematic1+(*(itl_index+i))));
		}

		component_decoder2_z(ext_llr2, llr, systematic2, parity2_tmp, pri_llr2, codeblock_size+3);

		for (i=0; i<codeblock_size; i++)
		{
			*(pri_llr1+(*(itl_index+i))) = *(ext_llr2+i) * scalfactor;
		}
		for (i=0; i<codeblock_size; i++)
		{
			fprintf(fp_log_s,"%4d::sys:%10.6f,par:%10.6f,pri:%10.6f:::Pri:%10.6f,Pri+Sys2:%10.6f.HH:%10.6f.\n",i,*(systematic2+i),*(parity2_tmp+i),*(pri_llr2+i),*(pri_llr1+i) ,*(pri_llr1+i) +*(systematic+i),*(ext_llr2+i) );
		}
	}
	fclose(fp_log_s);

	for (i=0; i<codeblock_size; i++)
	{
		*(di_llr+(*(itl_index+i))) = *(llr+i);
	}


	// Hard detection:
	fp_soft=fopen("softbits_s.log","w+");
	for (i=0; i<codeblock_size; i++)
	{
		fprintf(fp_soft,"Index:%4d,Soft:%10.2f.\n",i,*(di_llr+i));
		if (*(di_llr+i)>0)
		{
			*(dec_codeblock+i) = 0;
		}
		else
			*(dec_codeblock+i) = 1;
	}
	fclose(fp_soft);

	// Memory free:
	free(itl_index);
	free(systematic1);
	free(systematic2);
	free(parity1_tmp);
	free(parity2_tmp);
	free(pri_llr1);
	free(pri_llr2);
	free(ext_llr1);
	free(ext_llr2);
	free(llr);
	free(di_llr);
}
/*----------------------------------------------------------------------------*/
// void make_interleaver_table ( int* table, int cb_size )
// @brief: make interleaver output table
// @table: output signals
// @cb_size: input signals
/*----------------------------------------------------------------------------*/
void make_interleaver_table_z ( int* table, int cb_size )
{
	// Variables:
	int    i;
	int    size = 0;
	float f1   = 0.0;
	float f2   = 0.0;

	// Execute:
	for (i=0; i<188; i++) {
		if (cb_size <= interleaver_size[i]) {
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

/*----------------------------------------------------------------------------*/
// void component_decoder1 (double* extrinsic_llr,double* systematic, double* parity, double* pri_llr, int datalentotal)
/*----------------------------------------------------------------------------*/
/*
* @brief: turbo component decoder 1
* @extrinsic_llr: output [codeblock_size + 3]
* @systematic   : input  [codeblock_size + 3]
* @parity       : input  [codeblock_size + 3]
* @pri_llr      : input  [codeblock_size + 3]
* @datalentotal : input, the length of input vectors = codeblock_size + 3
*/
/*----------------------------------------------------------------------------*/
void component_decoder1_z (float* extrinsic_llr,float* systematic, float* parity, float* pri_llr, int datalentotal)
{
	// Variables:
	int     i;
	int     j;
	float  m0;
	float  m1;
	float  tmp0[8];
	float  tmp1[8];

	float* alpha[8];
	float* beta[8];
	float* gamma[2];
	float* branch_metric[2];
	float* llr;
	FILE* fp;
	FILE* fp_beta2;
	//double max_alpha=0.0;
	//double max_beta=0.0;
	fp_beta2= fopen("beta2.txt","w");
	// Initialization:
	for (i=0; i<8; i++)
	{
		alpha[i] = (float*) malloc(sizeof(float) * (datalentotal+1));
		beta[i]  = (float*) malloc(sizeof(float) * (datalentotal+1));

		for (j=0; j<(datalentotal+1); j++)
		{
			*(alpha[i]+j) = 0.0;
			*(beta [i]+j) = 0.0;
		}

		tmp0[i] = 0.0;
		tmp1[i] = 0.0;
	}
	for (i=0; i<2; i++)
	{
		gamma[i]         = (float*) malloc(sizeof(float) * (datalentotal));
		branch_metric[i] = (float*) malloc(sizeof(float) * (datalentotal));

		for (j=0; j<datalentotal; j++)
		{
			*(gamma[i]+j)         = 0.0;
			*(branch_metric[i]+j) = 0.0;
		}
	}
	llr = (float*) malloc(sizeof(float) * datalentotal);
	for (i=0; i<datalentotal; i++)
	{
		*(llr+i) = 0.0;
	}

	// Execute:
	// Gamma computation:
	for (i=0; i<datalentotal; i++)
	{
		*(branch_metric[0]+i) = -systematic[i] - parity[i]; // r11 = -r00
		*(branch_metric[1]+i) = -systematic[i] + parity[i]; // r10 = -r01

		*(gamma[0]+i) = *(branch_metric[0]+i) * 0.5 - *(pri_llr+i) * 0.5;
		*(gamma[1]+i) = *(branch_metric[1]+i) * 0.5 - *(pri_llr+i) * 0.5;
		//fprintf(fp_beta2,"%4d...sys:%10.2f,par:%10.2f,pri:%10.2f,gamma10:%10.2f,gamma11:%10.2f.\n",i,systematic[i],parity[i],*(pri_llr+i),*(gamma[1]+i),*(gamma[0]+i));
		//printf("%4d...sys:%10.2f,par:%10.2f,pri:%10.2f,gamma10:%10.2f,gamma11:%10.2f.\n",i,systematic[i],parity[i],*(pri_llr+i),*(gamma[1]+i),*(gamma[0]+i));
	}

	// Beta computation:
	*(beta[0] + datalentotal) = 0.0; //beta[0][datalentotal] = 0.0;
	for (i=1; i<8; i++)
	{
		*(beta[i] + datalentotal) = MINVALUE;
	}
	for (i=datalentotal-1; i>=0; i--)
	{
		*(beta[0]+i) = maxx(*(beta[0]+(i+1)) - *(gamma[0]+i), *(beta[4]+(i+1)) + *(gamma[0]+i)); //beta[0][i] = maxx(beta[0][i+1] - gamma[0][i], beta[4][i+1] + gamma[0][i]);
		*(beta[1]+i) = maxx(*(beta[4]+(i+1)) - *(gamma[0]+i), *(beta[0]+(i+1)) + *(gamma[0]+i)); //beta[1][i] = maxx(beta[4][i+1] - gamma[0][i], beta[0][i+1] + gamma[0][i]);
		*(beta[2]+i) = maxx(*(beta[5]+(i+1)) - *(gamma[1]+i), *(beta[1]+(i+1)) + *(gamma[1]+i)); //beta[2][i] = maxx(beta[5][i+1] - gamma[1][i], beta[1][i+1] + gamma[1][i]);
		*(beta[3]+i) = maxx(*(beta[1]+(i+1)) - *(gamma[1]+i), *(beta[5]+(i+1)) + *(gamma[1]+i)); //beta[3][i] = maxx(beta[1][i+1] - gamma[1][i], beta[5][i+1] + gamma[1][i]);
		*(beta[4]+i) = maxx(*(beta[2]+(i+1)) - *(gamma[1]+i), *(beta[6]+(i+1)) + *(gamma[1]+i)); //beta[4][i] = maxx(beta[2][i+1] - gamma[1][i], beta[6][i+1] + gamma[1][i]);
		*(beta[5]+i) = maxx(*(beta[6]+(i+1)) - *(gamma[1]+i), *(beta[2]+(i+1)) + *(gamma[1]+i)); //beta[5][i] = maxx(beta[6][i+1] - gamma[1][i], beta[2][i+1] + gamma[1][i]);
		*(beta[6]+i) = maxx(*(beta[7]+(i+1)) - *(gamma[0]+i), *(beta[3]+(i+1)) + *(gamma[0]+i)); //beta[6][i] = maxx(beta[7][i+1] - gamma[0][i], beta[3][i+1] + gamma[0][i]);
		*(beta[7]+i) = maxx(*(beta[3]+(i+1)) - *(gamma[0]+i), *(beta[7]+(i+1)) + *(gamma[0]+i)); //beta[7][i] = maxx(beta[3][i+1] - gamma[0][i], beta[7][i+1] + gamma[0][i]);
		/*max_beta=maxx(maxx(maxx(*(beta[0]+i),*(beta[1]+i)),maxx(*(beta[2]+i),*(beta[3]+i))),maxx(maxx(*(beta[4]+i),*(beta[5]+i)),maxx(*(beta[6]+i),*(beta[7]+i))));
		*(beta[0]+i)=*(beta[0]+i)-max_beta;
		*(beta[1]+i)=*(beta[1]+i)-max_beta;
		*(beta[2]+i)=*(beta[2]+i)-max_beta;
		*(beta[3]+i)=*(beta[3]+i)-max_beta;
		*(beta[4]+i)=*(beta[4]+i)-max_beta;
		*(beta[5]+i)=*(beta[5]+i)-max_beta;
		*(beta[6]+i)=*(beta[6]+i)-max_beta;
		*(beta[7]+i)=*(beta[7]+i)-max_beta;*/
	}
	for (i=0; i<datalentotal; i++)
	{
		fprintf(fp_beta2, "Beta:%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf\n", *(beta[0]+i),*(beta[1]+i),*(beta[2]+i),*(beta[3]+i),*(beta[4]+i),*(beta[5]+i),*(beta[6]+i),*(beta[7]+i));
	//	printf("%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf\n", *(beta[0]+i),*(beta[1]+i),*(beta[2]+i),*(beta[3]+i),*(beta[4]+i),*(beta[5]+i),*(beta[6]+i),*(beta[7]+i));
	}
	

	// Alpha computation:
	*(alpha[0]+0) = 0.0;
	for (i=1; i<8; i++)
	{
		*(alpha[i]+0) = MINVALUE;
	}
	for (i=1;i<(datalentotal+1); i++)
	{
		*(alpha[0]+i) = maxx(*(alpha[0]+(i-1)) - *(gamma[0]+(i-1)), *(alpha[1]+(i-1)) + *(gamma[0]+(i-1))); //alpha[0][i] = maxx(alpha[0][i-1]-gamma[0][i-1], alpha[1][i-1]+gamma[0][i-1]);
		*(alpha[1]+i) = maxx(*(alpha[3]+(i-1)) - *(gamma[1]+(i-1)), *(alpha[2]+(i-1)) + *(gamma[1]+(i-1))); //alpha[1][i] = maxx(alpha[3][i-1]-gamma[1][i-1], alpha[2][i-1]+gamma[1][i-1]);
		*(alpha[2]+i) = maxx(*(alpha[4]+(i-1)) - *(gamma[1]+(i-1)), *(alpha[5]+(i-1)) + *(gamma[1]+(i-1))); //alpha[2][i] = maxx(alpha[4][i-1]-gamma[1][i-1], alpha[5][i-1]+gamma[1][i-1]);
		*(alpha[3]+i) = maxx(*(alpha[7]+(i-1)) - *(gamma[0]+(i-1)), *(alpha[6]+(i-1)) + *(gamma[0]+(i-1))); //alpha[3][i] = maxx(alpha[7][i-1]-gamma[0][i-1], alpha[6][i-1]+gamma[0][i-1]);
		*(alpha[4]+i) = maxx(*(alpha[1]+(i-1)) - *(gamma[0]+(i-1)), *(alpha[0]+(i-1)) + *(gamma[0]+(i-1))); //alpha[4][i] = maxx(alpha[1][i-1]-gamma[0][i-1], alpha[0][i-1]+gamma[0][i-1]);
		*(alpha[5]+i) = maxx(*(alpha[2]+(i-1)) - *(gamma[1]+(i-1)), *(alpha[3]+(i-1)) + *(gamma[1]+(i-1))); //alpha[5][i] = maxx(alpha[2][i-1]-gamma[1][i-1], alpha[3][i-1]+gamma[1][i-1]);
		*(alpha[6]+i) = maxx(*(alpha[5]+(i-1)) - *(gamma[1]+(i-1)), *(alpha[4]+(i-1)) + *(gamma[1]+(i-1))); //alpha[6][i] = maxx(alpha[5][i-1]-gamma[1][i-1], alpha[4][i-1]+gamma[1][i-1]);
		*(alpha[7]+i) = maxx(*(alpha[6]+(i-1)) - *(gamma[0]+(i-1)), *(alpha[7]+(i-1)) + *(gamma[0]+(i-1))); //alpha[7][i] = maxx(alpha[6][i-1]-gamma[0][i-1], alpha[7][i-1]+gamma[0][i-1]);
		/*max_alpha=maxx(maxx(maxx(*(alpha[0]+i),*(alpha[1]+i)),maxx(*(alpha[2]+i),*(alpha[3]+i))),maxx(maxx(*(alpha[4]+i),*(alpha[5]+i)),maxx(*(alpha[6]+i),*(alpha[7]+i))));
		*(alpha[0]+i)=*(alpha[0]+i)-max_alpha;
		*(alpha[1]+i)=*(alpha[1]+i)-max_alpha;
		*(alpha[2]+i)=*(alpha[2]+i)-max_alpha;
		*(alpha[3]+i)=*(alpha[3]+i)-max_alpha;
		*(alpha[4]+i)=*(alpha[4]+i)-max_alpha;
		*(alpha[5]+i)=*(alpha[5]+i)-max_alpha;
		*(alpha[6]+i)=*(alpha[6]+i)-max_alpha;
		*(alpha[7]+i)=*(alpha[7]+i)-max_alpha;*/
	}
	//for (i=0; i<datalentotal; i++)
	//{
	//	fprintf(fp_beta2, "Alpha:%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf\n", *(alpha[0]+i),*(alpha[1]+i),*(alpha[2]+i),*(alpha[3]+i),*(alpha[4]+i),*(alpha[5]+i),*(alpha[6]+i),*(alpha[7]+i));
		//printf("%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf\n", *(alpha[0]+i),*(alpha[1]+i),*(alpha[2]+i),*(alpha[3]+i),*(alpha[4]+i),*(alpha[5]+i),*(alpha[6]+i),*(alpha[7]+i));
	//}
	// LLR computation:
	for (i=0; i<datalentotal; i++)
	{
		// p(x=+1):
		tmp0[0] = *(alpha[0]+i) + *(beta[0]+(i+1)) - *(gamma[0]+i); // tmp0[0] = alpha[0][i] + beta[0][i+1] - gamma[0][i];
		tmp0[1] = *(alpha[3]+i) + *(beta[1]+(i+1)) - *(gamma[1]+i); // tmp0[1] = alpha[3][i] + beta[1][i+1] - gamma[1][i];
		tmp0[2] = *(alpha[4]+i) + *(beta[2]+(i+1)) - *(gamma[1]+i); // tmp0[2] = alpha[4][i] + beta[2][i+1] - gamma[1][i];
		tmp0[3] = *(alpha[7]+i) + *(beta[3]+(i+1)) - *(gamma[0]+i); // tmp0[3] = alpha[7][i] + beta[3][i+1] - gamma[0][i];
		tmp0[4] = *(alpha[1]+i) + *(beta[4]+(i+1)) - *(gamma[0]+i); // tmp0[4] = alpha[1][i] + beta[4][i+1] - gamma[0][i];
		tmp0[5] = *(alpha[2]+i) + *(beta[5]+(i+1)) - *(gamma[1]+i); // tmp0[5] = alpha[2][i] + beta[5][i+1] - gamma[1][i];
		tmp0[6] = *(alpha[5]+i) + *(beta[6]+(i+1)) - *(gamma[1]+i); // tmp0[6] = alpha[5][i] + beta[6][i+1] - gamma[1][i];
		tmp0[7] = *(alpha[6]+i) + *(beta[7]+(i+1)) - *(gamma[0]+i); // tmp0[7] = alpha[6][i] + beta[7][i+1] - gamma[0][i];
		
		m0 = tmp0[0];
		for (j=1; j<8; j++)
		{
			m0 = maxx(m0, tmp0[j]);
		}
		//fprintf(fp_beta2, "Alpha:%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf\n", *(alpha[0]+i),*(alpha[1]+i),*(alpha[2]+i),*(alpha[3]+i),*(alpha[4]+i),*(alpha[5]+i),*(alpha[6]+i),*(alpha[7]+i));
		//fprintf(fp_beta2, "Beta:%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf\n", *(beta[0]+i+1),*(beta[1]+i+1),*(beta[2]+i+1),*(beta[3]+i+1),*(beta[4]+i+1),*(beta[5]+i+1),*(beta[6]+i+1),*(beta[7]+i+1));
		//fprintf(fp_beta2,"%4d...gamma10:%10.2f,gamma11:%10.2f.\n",i,*(gamma[1]+i),*(gamma[0]+i));
		//fprintf(fp_beta2, "Temp0:%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf\n",tmp0[0],tmp0[1],tmp0[2],tmp0[3],tmp0[4],tmp0[5],tmp0[6],tmp0[7]);
		

		// p(x=-1):
		tmp1[0] = *(alpha[1]+i) + *(beta[0]+(i+1)) + *(gamma[0]+i); // tmp1[0] = alpha[1][i] + beta[0][i+1] + gamma[0][i];
		tmp1[1] = *(alpha[2]+i) + *(beta[1]+(i+1)) + *(gamma[1]+i); // tmp1[1] = alpha[2][i] + beta[1][i+1] + gamma[1][i];
		tmp1[2] = *(alpha[5]+i) + *(beta[2]+(i+1)) + *(gamma[1]+i); // tmp1[2] = alpha[5][i] + beta[2][i+1] + gamma[1][i];
		tmp1[3] = *(alpha[6]+i) + *(beta[3]+(i+1)) + *(gamma[0]+i); // tmp1[3] = alpha[6][i] + beta[3][i+1] + gamma[0][i];
		tmp1[4] = *(alpha[0]+i) + *(beta[4]+(i+1)) + *(gamma[0]+i); // tmp1[4] = alpha[0][i] + beta[4][i+1] + gamma[0][i];
		tmp1[5] = *(alpha[3]+i) + *(beta[5]+(i+1)) + *(gamma[1]+i); // tmp1[5] = alpha[3][i] + beta[5][i+1] + gamma[1][i];
		tmp1[6] = *(alpha[4]+i) + *(beta[6]+(i+1)) + *(gamma[1]+i); // tmp1[6] = alpha[4][i] + beta[6][i+1] + gamma[1][i];
		tmp1[7] = *(alpha[7]+i) + *(beta[7]+(i+1)) + *(gamma[0]+i); // tmp1[7] = alpha[7][i] + beta[7][i+1] + gamma[0][i];
		
		m1 = tmp1[0];
		for (j=1; j<8; j++)
		{
			m1 = maxx(m1, tmp1[j]);
		}
		//fprintf(fp_beta2, "Alpha:%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf\n", *(alpha[0]+i),*(alpha[1]+i),*(alpha[2]+i),*(alpha[3]+i),*(alpha[4]+i),*(alpha[5]+i),*(alpha[6]+i),*(alpha[7]+i));
		//fprintf(fp_beta2, "Beta:%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf\n", *(beta[0]+i+1),*(beta[1]+i+1),*(beta[2]+i+1),*(beta[3]+i+1),*(beta[4]+i+1),*(beta[5]+i+1),*(beta[6]+i+1),*(beta[7]+i+1));
		//fprintf(fp_beta2,"%4d...gamma10:%10.2f,gamma11:%10.2f.\n",i,*(gamma[1]+i),*(gamma[0]+i));
		//fprintf(fp_beta2, "Temp1:%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf\n",tmp1[0],tmp1[1],tmp1[2],tmp1[3],tmp1[4],tmp1[5],tmp1[6],tmp1[7]);

		// LLR:
		*(llr+i) = m0 - m1;
		//fprintf(fp_beta2,"serial-LLR:%10.6f.LLr0:%10.6f,LLr1:%10.6f.\n",*(llr+i),m0,m1);
		//printf("serial-LLR:%10.6f.LLr0:%10.6f,LLr1:%10.6f.\n",*(llr+i),m0,m1);
		// Extrinsic LLR:
		*(extrinsic_llr+i) = *(llr+i) - *(pri_llr+i) - *(systematic+i);
	}

	fclose(fp_beta2);
	fp = fopen("extrinsic_llr1.txt","w");
	for (i=0; i<datalentotal; i++)
	{
		fprintf(fp, "%lf\n", *(extrinsic_llr + i));
	}
	fclose(fp);


	// Memory free:
	for (i=0; i<8; i++)
	{
		free(alpha[i]);
		free(beta[i]  );
	}
	for (i=0; i<2; i++)
	{
		free(gamma[i]);
		free(branch_metric[i]);
	}
	free(llr);
}


/*----------------------------------------------------------------------------*/
// void component_decoder2 (double* extrinsic_llr,double* llr, double* systematic, double* parity, double* pri_llr, int datalentotal)
/*----------------------------------------------------------------------------*/
/*
* @brief: turbo component decoder 1
* @extrinsic_llr: output [codeblock_size + 3]
* @llr          : output [codeblock_size + 3]
* @systematic   : input  [codeblock_size + 3]
* @parity       : input  [codeblock_size + 3]
* @pri_llr      : input  [codeblock_size + 3]
* @datalentotal : input, the length of input vectors = codeblock_size + 3
*/
/*----------------------------------------------------------------------------*/
void component_decoder2_z (float* extrinsic_llr,float* llr, float* systematic, float* parity, float* pri_llr, int datalentotal)
{
	// Variables:
	int     i;
	int     j;
	float  m0;
	float  m1;
	float  tmp0[8];
	float  tmp1[8];

	float* alpha[8];
	float* beta[8];
	float* gamma[2];
	float* branch_metric[2];
	//double* llr;
	FILE* fp_beta22;
	fp_beta22= fopen("beta22.txt","w");
	// Initialization:
	for (i=0; i<8; i++)
	{
		alpha[i] = (float*) malloc(sizeof(float) * (datalentotal+1));
		beta[i]  = (float*) malloc(sizeof(float) * (datalentotal+1));

		for (j=0; j<(datalentotal+1); j++)
		{
			*(alpha[i]+j) = 0.0;
			*(beta [i]+j) = 0.0;
		}

		tmp0[i] = 0.0;
		tmp1[i] = 0.0;
	}
	for (i=0; i<2; i++)
	{
		gamma[i]         = (float*) malloc(sizeof(float) * (datalentotal));
		branch_metric[i] = (float*) malloc(sizeof(float) * (datalentotal));

		for (j=0; j<datalentotal; j++)
		{
			*(gamma[i]+j)         = 0.0;
			*(branch_metric[i]+j) = 0.0;
		}
	}
	//llr = (double*) malloc(sizeof(double) * datalentotal);
	/*for (i=0; i<datalentotal; i++)
	{
		*(llr+i) = 0.0;
	}*/

	// Execute:
	// Gamma computation:
	for (i=0; i<datalentotal; i++)
	{
		*(branch_metric[0]+i) = -systematic[i] - parity[i]; // r11 = -r00
		*(branch_metric[1]+i) = -systematic[i] + parity[i]; // r10 = -r01

		*(gamma[0]+i) = *(branch_metric[0]+i) * 0.5 - *(pri_llr+i) * 0.5;
		*(gamma[1]+i) = *(branch_metric[1]+i) * 0.5 - *(pri_llr+i) * 0.5;
		fprintf(fp_beta22,"%4d...sys:%f,par:%f,pri:%f,prime:%f,gamma10:%f,gamma11:%f.\n",i,systematic[i],parity[i],*(pri_llr+i),systematic[i]+*(pri_llr+i),*(gamma[1]+i),*(gamma[0]+i));
	}

	// Beta computation:
	*(beta[0] + datalentotal) = 0.0; //beta[0][datalentotal] = 0.0;
	for (i=1; i<8; i++)
	{
		*(beta[i] + datalentotal) = MINVALUE;
	}
	for (i=datalentotal-1; i>=0; i--)
	{
		*(beta[0]+i) = maxx(*(beta[0]+(i+1)) - *(gamma[0]+i), *(beta[4]+(i+1)) + *(gamma[0]+i)); //beta[0][i] = maxx(beta[0][i+1] - gamma[0][i], beta[4][i+1] + gamma[0][i]);
		*(beta[1]+i) = maxx(*(beta[4]+(i+1)) - *(gamma[0]+i), *(beta[0]+(i+1)) + *(gamma[0]+i)); //beta[1][i] = maxx(beta[4][i+1] - gamma[0][i], beta[0][i+1] + gamma[0][i]);
		*(beta[2]+i) = maxx(*(beta[5]+(i+1)) - *(gamma[1]+i), *(beta[1]+(i+1)) + *(gamma[1]+i)); //beta[2][i] = maxx(beta[5][i+1] - gamma[1][i], beta[1][i+1] + gamma[1][i]);
		*(beta[3]+i) = maxx(*(beta[1]+(i+1)) - *(gamma[1]+i), *(beta[5]+(i+1)) + *(gamma[1]+i)); //beta[3][i] = maxx(beta[1][i+1] - gamma[1][i], beta[5][i+1] + gamma[1][i]);
		*(beta[4]+i) = maxx(*(beta[2]+(i+1)) - *(gamma[1]+i), *(beta[6]+(i+1)) + *(gamma[1]+i)); //beta[4][i] = maxx(beta[2][i+1] - gamma[1][i], beta[6][i+1] + gamma[1][i]);
		*(beta[5]+i) = maxx(*(beta[6]+(i+1)) - *(gamma[1]+i), *(beta[2]+(i+1)) + *(gamma[1]+i)); //beta[5][i] = maxx(beta[6][i+1] - gamma[1][i], beta[2][i+1] + gamma[1][i]);
		*(beta[6]+i) = maxx(*(beta[7]+(i+1)) - *(gamma[0]+i), *(beta[3]+(i+1)) + *(gamma[0]+i)); //beta[6][i] = maxx(beta[7][i+1] - gamma[0][i], beta[3][i+1] + gamma[0][i]);
		*(beta[7]+i) = maxx(*(beta[3]+(i+1)) - *(gamma[0]+i), *(beta[7]+(i+1)) + *(gamma[0]+i)); //beta[7][i] = maxx(beta[3][i+1] - gamma[0][i], beta[7][i+1] + gamma[0][i]);
	}

	for (i=0; i<datalentotal; i++)
	{
		fprintf(fp_beta22, "Beta:%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf\n", *(beta[0]+i),*(beta[1]+i),*(beta[2]+i),*(beta[3]+i),*(beta[4]+i),*(beta[5]+i),*(beta[6]+i),*(beta[7]+i));
	}
	// Alpha computation:
	*(alpha[0]+0) = 0.0;
	for (i=1; i<8; i++)
	{
		*(alpha[i]+0) = MINVALUE;
	}
	for (i=1;i<(datalentotal+1); i++)
	{
		*(alpha[0]+i) = maxx(*(alpha[0]+(i-1)) - *(gamma[0]+(i-1)), *(alpha[1]+(i-1)) + *(gamma[0]+(i-1))); //alpha[0][i] = maxx(alpha[0][i-1]-gamma[0][i-1], alpha[1][i-1]+gamma[0][i-1]);
		*(alpha[1]+i) = maxx(*(alpha[3]+(i-1)) - *(gamma[1]+(i-1)), *(alpha[2]+(i-1)) + *(gamma[1]+(i-1))); //alpha[1][i] = maxx(alpha[3][i-1]-gamma[1][i-1], alpha[2][i-1]+gamma[1][i-1]);
		*(alpha[2]+i) = maxx(*(alpha[4]+(i-1)) - *(gamma[1]+(i-1)), *(alpha[5]+(i-1)) + *(gamma[1]+(i-1))); //alpha[2][i] = maxx(alpha[4][i-1]-gamma[1][i-1], alpha[5][i-1]+gamma[1][i-1]);
		*(alpha[3]+i) = maxx(*(alpha[7]+(i-1)) - *(gamma[0]+(i-1)), *(alpha[6]+(i-1)) + *(gamma[0]+(i-1))); //alpha[3][i] = maxx(alpha[7][i-1]-gamma[0][i-1], alpha[6][i-1]+gamma[0][i-1]);
		*(alpha[4]+i) = maxx(*(alpha[1]+(i-1)) - *(gamma[0]+(i-1)), *(alpha[0]+(i-1)) + *(gamma[0]+(i-1))); //alpha[4][i] = maxx(alpha[1][i-1]-gamma[0][i-1], alpha[0][i-1]+gamma[0][i-1]);
		*(alpha[5]+i) = maxx(*(alpha[2]+(i-1)) - *(gamma[1]+(i-1)), *(alpha[3]+(i-1)) + *(gamma[1]+(i-1))); //alpha[5][i] = maxx(alpha[2][i-1]-gamma[1][i-1], alpha[3][i-1]+gamma[1][i-1]);
		*(alpha[6]+i) = maxx(*(alpha[5]+(i-1)) - *(gamma[1]+(i-1)), *(alpha[4]+(i-1)) + *(gamma[1]+(i-1))); //alpha[6][i] = maxx(alpha[5][i-1]-gamma[1][i-1], alpha[4][i-1]+gamma[1][i-1]);
		*(alpha[7]+i) = maxx(*(alpha[6]+(i-1)) - *(gamma[0]+(i-1)), *(alpha[7]+(i-1)) + *(gamma[0]+(i-1))); //alpha[7][i] = maxx(alpha[6][i-1]-gamma[0][i-1], alpha[7][i-1]+gamma[0][i-1]);
	}

	for (i=0; i<datalentotal; i++)
	{
		fprintf(fp_beta22, "Alpha:%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf\n", *(alpha[0]+i),*(alpha[1]+i),*(alpha[2]+i),*(alpha[3]+i),*(alpha[4]+i),*(alpha[5]+i),*(alpha[6]+i),*(alpha[7]+i));
		//printf("%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf\n", *(alpha[0]+i),*(alpha[1]+i),*(alpha[2]+i),*(alpha[3]+i),*(alpha[4]+i),*(alpha[5]+i),*(alpha[6]+i),*(alpha[7]+i));
	}
	// LLR computation:
	for (i=0; i<datalentotal; i++)
	{
		// p(x=+1):
		tmp0[0] = *(alpha[0]+i) + *(beta[0]+(i+1)) - *(gamma[0]+i); // tmp0[0] = alpha[0][i] + beta[0][i+1] - gamma[0][i];
		tmp0[1] = *(alpha[3]+i) + *(beta[1]+(i+1)) - *(gamma[1]+i); // tmp0[1] = alpha[3][i] + beta[1][i+1] - gamma[1][i];
		tmp0[2] = *(alpha[4]+i) + *(beta[2]+(i+1)) - *(gamma[1]+i); // tmp0[2] = alpha[4][i] + beta[2][i+1] - gamma[1][i];
		tmp0[3] = *(alpha[7]+i) + *(beta[3]+(i+1)) - *(gamma[0]+i); // tmp0[3] = alpha[7][i] + beta[3][i+1] - gamma[0][i];
		tmp0[4] = *(alpha[1]+i) + *(beta[4]+(i+1)) - *(gamma[0]+i); // tmp0[4] = alpha[1][i] + beta[4][i+1] - gamma[0][i];
		tmp0[5] = *(alpha[2]+i) + *(beta[5]+(i+1)) - *(gamma[1]+i); // tmp0[5] = alpha[2][i] + beta[5][i+1] - gamma[1][i];
		tmp0[6] = *(alpha[5]+i) + *(beta[6]+(i+1)) - *(gamma[1]+i); // tmp0[6] = alpha[5][i] + beta[6][i+1] - gamma[1][i];
		tmp0[7] = *(alpha[6]+i) + *(beta[7]+(i+1)) - *(gamma[0]+i); // tmp0[7] = alpha[6][i] + beta[7][i+1] - gamma[0][i];

		m0 = tmp0[0];
		for (j=1; j<8; j++)
		{
			m0 = maxx(m0, tmp0[j]);
		}

		// p(x=-1):
		tmp1[0] = *(alpha[1]+i) + *(beta[0]+(i+1)) + *(gamma[0]+i); // tmp1[0] = alpha[1][i] + beta[0][i+1] + gamma[0][i];
		tmp1[1] = *(alpha[2]+i) + *(beta[1]+(i+1)) + *(gamma[1]+i); // tmp1[1] = alpha[2][i] + beta[1][i+1] + gamma[1][i];
		tmp1[2] = *(alpha[5]+i) + *(beta[2]+(i+1)) + *(gamma[1]+i); // tmp1[2] = alpha[5][i] + beta[2][i+1] + gamma[1][i];
		tmp1[3] = *(alpha[6]+i) + *(beta[3]+(i+1)) + *(gamma[0]+i); // tmp1[3] = alpha[6][i] + beta[3][i+1] + gamma[0][i];
		tmp1[4] = *(alpha[0]+i) + *(beta[4]+(i+1)) + *(gamma[0]+i); // tmp1[4] = alpha[0][i] + beta[4][i+1] + gamma[0][i];
		tmp1[5] = *(alpha[3]+i) + *(beta[5]+(i+1)) + *(gamma[1]+i); // tmp1[5] = alpha[3][i] + beta[5][i+1] + gamma[1][i];
		tmp1[6] = *(alpha[4]+i) + *(beta[6]+(i+1)) + *(gamma[1]+i); // tmp1[6] = alpha[4][i] + beta[6][i+1] + gamma[1][i];
		tmp1[7] = *(alpha[7]+i) + *(beta[7]+(i+1)) + *(gamma[0]+i); // tmp1[7] = alpha[7][i] + beta[7][i+1] + gamma[0][i];

		m1 = tmp1[0];
		for (j=1; j<8; j++)
		{
			m1 = maxx(m1, tmp1[j]);
		}

		// LLR:
		*(llr+i) = m0 - m1;

		// Extrinsic LLR:
		*(extrinsic_llr+i) = *(llr+i) - *(pri_llr+i) - *(systematic+i);
		fprintf(fp_beta22, "Le:%lf\n",*(extrinsic_llr+i));
	}

	fclose(fp_beta22);
	// Memory free:
	for (i=0; i<8; i++)
	{
		free(alpha[i]);
		free(beta[i]  );
	}
	for (i=0; i<2; i++)
	{
		free(gamma[i]);
		free(branch_metric[i]);
	}
	//free(llr);
}

#endif
