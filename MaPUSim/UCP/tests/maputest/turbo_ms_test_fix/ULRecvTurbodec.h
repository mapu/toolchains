/*============================================================================*/
//  Information
/*============================================================================*/
/* 
*  @file    turbodec.h
*  @brief   
*  @author  wangxiaoqin
*  @date    2014/04/28         */
/*----------------------------------------------------------------------------*/
#ifndef TURBODEC_H
#define TURBODEC_H

/*============================================================================*/
//  Header files
/*============================================================================*/
#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "qpp_interleaver.h"
#include "crc.h"
#include "turbo_enc_dec.h"
//#include "turboenc.h"

#define MINVALUE      -2147483647.0     //0x7F
#define MAX_ITER_NUM  8

#define maxx(a, b) (a >= b ? a : b)

/*===========================================================================*/
// Function definition
/*===========================================================================*/
int turbo_decoder_serial (int* dec_codeblock,float* systematic, float* parity1, float* parity2, int datasize, int iteratenum, float scalfactor,int crctype);
void    component_decoder1 (float* extrinsic_llr, float* systematic, float* parity, float* pri_llr, int datalentotal);
void    component_decoder2 (float* extrinsic_llr, float* llr, float* systematic, float* parity, float* pri_llr, int datalentotal);
//void    make_interleaver_table ( int* table, int cb_size );

#endif
