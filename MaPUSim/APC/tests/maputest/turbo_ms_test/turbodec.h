/*============================================================================*/
//  Information
/*============================================================================*/
/* 
*  @file    turbodec.h
*  @brief   
*  @author  wangxiaoqin
*  @date    2014/04/28
/*----------------------------------------------------------------------------*/
#ifndef TURBODEC_H
#define TURBODEC_H

/*============================================================================*/
//  Header files
/*============================================================================*/
#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "define.h"

/*===========================================================================*/
// Function definition
/*===========================================================================*/
void    turbo_decoder (int* dec_codeblock, float* systematic, float* parity1, float* parity2, int datasize, int iteratenum, float scalfactor);
void    component_decoder1 (float* extrinsic_llr, float* systematic, float* parity, float* pri_llr, int datalentotal);
void    component_decoder2 (float* extrinsic_llr, float* llr, float* systematic, float* parity, float* pri_llr, int datalentotal);
void    make_interleaver_table ( int* table, int cb_size );

#endif
