/*============================================================================*/
//  Information
/*============================================================================*/
/* 
*  @file    turboenc.c
*  @brief   
*  @author  wangxiaoqin
*  @date    2013/07/10
/*----------------------------------------------------------------------------*/
#ifndef TURBOENC_C
#define TURBOENC_C

/*============================================================================*/
//  Header files
/*============================================================================*/
#include "turboenc.h"
#include "turbodec.h"

/*===========================================================================*/
// Function definition
/*===========================================================================*/

/*----------------------------------------------------------------------------*/
// void constitue_encoder (int* stream_out, int* stream, int length)
// @brief: 8-state constitue encoder
// @stream_out: output signals [length + 6]
// @stream: input
// @length: input
/*----------------------------------------------------------------------------*/
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

		// cyclic shift｣ｺ
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

		// レジスタの内容をシフト　
		s_register[3] = s_register[2];
		s_register[2] = s_register[1];
		s_register[1] = s_register[0];
	}
}


/*----------------------------------------------------------------------------*/
//void turbo_encoder(int* info_systematic, int* info_parity1, int* info_parity2, int* info_codeblock, int length_cb )
// @brief: turbo encoder
// @info_systematic: output signals 
// @info_parity1: output of the 1st constitue encoder
// @info_parity2: output of the 2nd constitue encoder
// @info_codeblock: input
// @length_cb: input
/*----------------------------------------------------------------------------*/
void turbo_encoder(int* info_systematic, int* info_parity1, int* info_parity2, int* info_codeblock, int length_cb )
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

	// Systematic bits:
	for (i=0; i<length_cb; i++)
	{
		*(info_systematic+i) = *(info_codeblock+i);
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
		*(info_systematic+length_cb+i) = tail[3*i];
		*(info_parity1+length_cb+i)    = tail[3*i+1];
		*(info_parity2+length_cb+i)    = tail[3*i+2];
	}

	// memory free:
	free(interleaver_table);
	free(inter);
	free(tmp_parity1);
	free(tmp_parity2);
}

#endif