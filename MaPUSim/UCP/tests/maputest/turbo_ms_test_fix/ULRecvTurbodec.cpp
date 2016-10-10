/*============================================================================*/
//  Information
/*============================================================================*/
/* 
*  @file    turbodec.c
*  @brief   
*  @author  wangxiaoqin
*  @date    2014/01/28
----------------------------------------------------------------------------*/
#ifndef TURBODEC_C
#define TURBODEC_C

/*============================================================================*/
//  Header files
/*============================================================================*/
#include "ULRecvTurbodec.h"

/*===========================================================================*/
// Function definition
/*===========================================================================*/

/*----------------------------------------------------------------------------*/
// void turbo_decoder (int* dec_codeblock, float* systematic, float* parity1, float* parity2, int datasize, int iteratenum, float scalfactor)
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
int turbo_decoder_serial (int* dec_codeblock,float* systematic, float* parity1, float* parity2, int datasize, int iteratenum, float scalfactor,int crctype)
{
	// Variables:
	int    i;
	int    j;
	int    codeblock_size;
	int    cb_ack;


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
#ifdef DEBUG
	FILE* fp_sys1;
	FILE* fp_par1;
	FILE* fp_sys2;
	FILE* fp_par2;
	FILE* fp_tail;
	FILE* fp_llr;
	FILE* fp_decoded_bits;
#endif
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

	make_interleaver_table(itl_index, codeblock_size);

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
#ifdef DEBUG
	fp_tail = fopen("tail.txt","w");
	for (i=0; i<12; i++)
	{
		fprintf(fp_tail, "%lf\n", *(tail + i));
	}
	fclose(fp_tail);

	fp_sys1 = fopen("sys1.txt","w");
	for (i=0; i<codeblock_size+3; i++)
	{
		fprintf(fp_sys1, "%lf\n", *(systematic1 + i));
	}
	fclose(fp_sys1);

	fp_par1 = fopen("decpar1.txt","w");
	for (i=0; i<codeblock_size+3; i++)
	{
		fprintf(fp_par1, "%lf\n", *(parity1_tmp + i));
	}
	fclose(fp_par1);

	fp_sys2 = fopen("sys2.txt","w");
	for (i=0; i<codeblock_size+3; i++)
	{
		fprintf(fp_sys2, "%lf\n", *(systematic2 + i));
	}
	fclose(fp_sys2);

	fp_par2 = fopen("decpar2.txt","w");
	for (i=0; i<codeblock_size+3; i++)
	{
		fprintf(fp_par2, "%lf\n", *(parity2_tmp + i));
	}
	fclose(fp_par2);
#endif
	// Execute:
	for (j=0; j<iteratenum; j++)
	{
		component_decoder1(ext_llr1, systematic1, parity1_tmp, pri_llr1, codeblock_size+3);

		for (i=0; i<codeblock_size; i++)
		{
			*(pri_llr2+i) = *(ext_llr1+(*(itl_index+i))) * scalfactor;
		}
		for (i=codeblock_size;i<codeblock_size+3;i++)
		{
			*(pri_llr2+i) =0;
		}

		component_decoder2(ext_llr2, llr, systematic2, parity2_tmp, pri_llr2, codeblock_size+3);

		for (i=0; i<codeblock_size; i++)
		{
			*(pri_llr1+(*(itl_index+i))) = *(ext_llr2+i) * scalfactor;
		}
		for (i=codeblock_size;i<codeblock_size+3;i++)
		{
			*(pri_llr1+i) =0;
		}
	

		for (i=0; i<codeblock_size; i++)
		{
			*(di_llr+(*(itl_index+i))) = *(llr+i);
		}

		// Hard detection:
		for (i=0; i<codeblock_size; i++)
		{
			if (*(di_llr+i)>0)
			{
				*(dec_codeblock+i) = 0;
			}
			else
				*(dec_codeblock+i) = 1;
		}
	#ifdef DEBUG
		fp_llr=fopen("test_llr.txt","w");
		for (i=0;i<codeblock_size;i++)
		{
			fprintf(fp_llr,"%f\n",*(di_llr+i));
		}
		fclose(fp_llr);
		fp_decoded_bits=fopen("decoded_bits.txt","w");
		for (i=0;i<codeblock_size;i++)
		{
			fprintf(fp_decoded_bits,"%d\n",*(dec_codeblock+i));
		}
		fclose(fp_decoded_bits);
	#endif
		cb_ack=crc_check(dec_codeblock,codeblock_size,crctype);
		if (cb_ack==1)
		{
			break;
		}
	}
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
    return iteratenum;
//	return(cb_ack);
}
/*----------------------------------------------------------------------------*/
// void make_interleaver_table ( int* table, int cb_size )
// @brief: make interleaver output table
// @table: output signals
// @cb_size: input signals
/*----------------------------------------------------------------------------*/
//void make_interleaver_table ( int* table, int cb_size )
//{
//	// Variables:
//	int    i;
//	int    size = 0;
//	float f1   = 0.0;
//	float f2   = 0.0;
//
//	// Execute:
//	for (i=0; i<188; i++) {
//		if (cb_size <= interleaver_size[i]) {
//			size = interleaver_size[i];
//			f1   = (float)interleaver_f1[i];
//			f2   = (float)interleaver_f2[i];
//			break;
//		}
//	}
//
//	for (i=0; i<size; i++) {
//		*(table+i) = (int)fmod(((f1 * i) + (f2 * pow((float)i, 2.0))) , (float)size);
//	}
//}

/*----------------------------------------------------------------------------*/
// void component_decoder1 (float* extrinsic_llr,float* systematic, float* parity, float* pri_llr, int datalentotal)
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
void component_decoder1 (float* extrinsic_llr,float* systematic, float* parity, float* pri_llr, int datalentotal)
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
	}


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
// void component_decoder2 (float* extrinsic_llr,float* llr, float* systematic, float* parity, float* pri_llr, int datalentotal)
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
void component_decoder2 (float* extrinsic_llr,float* llr, float* systematic, float* parity, float* pri_llr, int datalentotal)
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
	//float* llr;

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
	//llr = (float*) malloc(sizeof(float) * datalentotal);
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
	}


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
