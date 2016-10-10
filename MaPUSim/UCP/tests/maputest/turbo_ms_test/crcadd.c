/*============================================================================*/
//  Information
/*============================================================================*/
/* 
*  @file    crcadd.c
*  @brief   
*  @author  wangxiaoqin
*  @date    2013/07/10
/*----------------------------------------------------------------------------*/
#ifndef CRCADD_C
#define CRCADD_C

/*============================================================================*/
//  Header files
/*============================================================================*/
#include "crcadd.h"

/*============================================================================*/
//  Function definition
/*============================================================================*/
/*----------------------------------------------------------------------------*/
// void crc_add (int* stream_crc, int* stream, int datasize, int crctype)
// @brief: set code size
// @stream_crc: output: the output stream after CRC
// @stream: input: the input stream to CRC module
// @datasize: input: the length of input stream
// @crctype: input: CRC type (0:24A, 1:24B, 2:16, 3:8)
/*----------------------------------------------------------------------------*/
void crc_add (int* stream_crc, int* stream, int datasize, int crctype)
{
	// Variables:
	int     i, j;
	int     crc_poly_size; 
	int     length;        
	int*    dividend;     
	int*    crc_poly;
	int     m_crc_poly_24a[25] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	int     m_crc_poly_24b[25] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	int     m_crc_poly_16[17]  = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	int     m_crc_poly_8[9]    = {0,0,0,0,0,0,0,0,0};

	// Initialization:
	m_crc_poly_24a[0]  = 1;  // D^24
	m_crc_poly_24a[1]  = 1;  // D^23
	m_crc_poly_24a[6]  = 1;  // D^18
	m_crc_poly_24a[7]  = 1;  // D^17
	m_crc_poly_24a[10] = 1;  // D^14
	m_crc_poly_24a[13] = 1;  // D^11
	m_crc_poly_24a[14] = 1;  // D^10
	m_crc_poly_24a[17] = 1;  // D^7
	m_crc_poly_24a[18] = 1;  // D^6
	m_crc_poly_24a[19] = 1;  // D^5
	m_crc_poly_24a[20] = 1;  // D^4
	m_crc_poly_24a[21] = 1;  // D^3
	m_crc_poly_24a[23] = 1;  // D^1
	m_crc_poly_24a[24] = 1;  // D^0

	m_crc_poly_24b[0]  = 1;  // D^24
	m_crc_poly_24b[1]  = 1;  // D^23
	m_crc_poly_24b[18] = 1;  // D^6
	m_crc_poly_24b[19] = 1;  // D^5
	m_crc_poly_24b[23] = 1;  // D^1
	m_crc_poly_24b[24] = 1;  // D^0

	m_crc_poly_16[0]   = 1;  // D^16
	m_crc_poly_16[4]   = 1;  // D^12
	m_crc_poly_16[11]  = 1;  // D^5
	m_crc_poly_16[16]  = 1;  // D^0

	m_crc_poly_8[0]    = 1;  // D^8
	m_crc_poly_8[1]    = 1;  // D^7
	m_crc_poly_8[4]    = 1;  // D^4
	m_crc_poly_8[5]    = 1;  // D^3
	m_crc_poly_8[7]    = 1;  // D^1
	m_crc_poly_8[8]    = 1;  // D^0

	switch (crctype) {
		// 24bit-typeA
	case 0:
		crc_poly_size = 25;
		crc_poly = m_crc_poly_24a;
		break;

		// 24bit-typeB
	case 1:
		crc_poly_size = 25;
		crc_poly = m_crc_poly_24b;
		break;

		// 16bit
	case 2:
		crc_poly_size = 17;
		crc_poly = m_crc_poly_16;
		break;

		// 8bit
	case 3:
		crc_poly_size = 9;
		crc_poly = m_crc_poly_8;
		break;

	default:
		exit(0);
	}

	length = datasize + crc_poly_size - 1;
	dividend = (int*) malloc(sizeof(int) * length);
	for (i=0; i<datasize; i++)
	{
		*(dividend+i) = *(stream + i);
	}
	for (i=0; i<(crc_poly_size-1); i++) 
	{
		*(dividend+i+datasize) = 0;
	}

	// Execute:
	for (i=0; i<datasize; i++) 
	{
		if (*(dividend+i) != 0) 
		{
			for (j=0; j<crc_poly_size; j++) 
			{
				*(dividend+i+j) ^= *(crc_poly+j);
			}
		}
	}

	for (i=0; i<datasize; i++)
	{
		*(stream_crc+i) = *(stream + i);
	}
	for (i=0; i<(crc_poly_size-1); i++)
	{
		*(stream_crc+i+datasize) = *(dividend+i+datasize);
	}

	// memory free:
	free(dividend);
}

int crcdec(int *stream, int stream_len, int crc_type){
	int i;
	int *temp,temp_len;
	int ok_flag = 1;

	switch (crc_type) {	
		case 0: // 24bit-typeA
			temp_len = stream_len + 24;
			break;
		case 1: // 24bit-typeB
			temp_len = stream_len + 24;
			break;
		case 2: // 16bit
			temp_len = stream_len + 16;
			break;	
		case 3: // 8bit
			temp_len = stream_len + 8;
			break;
		default:
			exit(0);
	}

	temp = (int*) malloc(sizeof(int) * temp_len);
	
	crc_add(temp,stream,stream_len,crc_type);
	if(DEBUG_CRC_DEC == 1){
		for(i=0;i<temp_len;i++)
			printf("%d. ",*(temp+i));
		printf("\n");
		for(i=0;i<stream_len;i++)
			printf("%d. ",*(stream+i));
		printf("\n");
	}
	
	for(i=0;i<(temp_len - stream_len);i++){
		if(*(temp+stream_len+i) != 0)
			ok_flag = 0;
	}

	free(temp);
	return(ok_flag);
}
#endif
