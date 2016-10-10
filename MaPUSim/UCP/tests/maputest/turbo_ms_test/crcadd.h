/*============================================================================*/
//  Information
/*============================================================================*/
/* 
*  @file    crcadd.h
*  @brief   
*  @author  wangxiaoqin
*  @date    2013/07/10
/*----------------------------------------------------------------------------*/
#ifndef CRCADD_H
#define CRCADD_H

/*============================================================================*/
//  Header files
/*============================================================================*/
#include "stdlib.h"
#include "stdio.h"
#include "define.h"

/*===========================================================================*/
// Function definition
/*===========================================================================*/
void crc_add (int* stream_crc, // output: the output stream after CRC
			  int* stream,     // input: the input stream to CRC module
			  int datasize,    // input: the length of input stream
			  int crctype      // input: CRC type (0:24A, 1:24B, 2:16, 3:8)
			  );

int crcdec(int *stream, 
		   int stream_len, 
		   int crc_type
		   );
#endif