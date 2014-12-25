/*============================================================================*/
//  Information
/*============================================================================*/
/* 
*  @file    turbodec_z.h
*  @brief   
*  @author  wangxiaoqin
*  @date    2014/04/28
/*----------------------------------------------------------------------------*/
#ifndef TURBODEC_Z_H
#define TURBODEC_Z_H

/*============================================================================*/
//  Header files
/*============================================================================*/
#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "define.h"

//#define MINVALUE  -2147483647.0     //0x7F

//#define maxx(a, b) (a >= b ? a : b)

/*===========================================================================*/
// Function definition
/*===========================================================================*/
void    turbo_decoder_z (int* dec_codeblock, float* systematic, float* parity1, float* parity2, int datasize, int iteratenum, float scalfactor);
void    component_decoder1_z (float* extrinsic_llr, float* systematic, float* parity, float* pri_llr, int datalentotal);
void    component_decoder2_z (float* extrinsic_llr, float* llr, float* systematic, float* parity, float* pri_llr, int datalentotal);
void    make_interleaver_table_z ( int* table, int cb_size );

#endif
