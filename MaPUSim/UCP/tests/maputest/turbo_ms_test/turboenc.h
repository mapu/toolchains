/*============================================================================*/
//  Information
/*============================================================================*/
/* 
*  @file    turboenc.h
*  @brief   
*  @author  wangxiaoqin
*  @date    2013/07/10
/*----------------------------------------------------------------------------*/
#ifndef TURBOENC_H
#define TURBOENC_H

/*============================================================================*/
//  Header files
/*============================================================================*/
#include "stdlib.h"
#include "math.h"

#include "qpp_interleaver.h"

/*===========================================================================*/
// Function definition
/*===========================================================================*/

void constitue_encoder (int* stream_out,  // output stream
						int* stream,      // input stream
						int length        // input: the length of input stream
						);

void turbo_encoder (int* info_systematic,  // output
					int* info_parity1,     // output
					int* info_parity2,     // output
					int* info_codeblock,   // input
					int  length_cb         // input
					);

#endif
