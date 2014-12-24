#ifndef CRC_C
#define CRC_C

#include "crc.h"


void bit2byte(unsigned char *byte_stream,int *bit_stream,unsigned int bit_len)
{
	unsigned int i;
	unsigned char tmp=0x0;
	//*byte_len = bit_len/8;
	for(i=1;i<=bit_len;i++){
		if(i%8 != 0)
			tmp |= (*(bit_stream + i - 1) & 0x1) << (8 - i%8);
		else
			{
			 *(byte_stream + i/8 -1) = tmp | (*(bit_stream + i - 1) & 0x1);
		     tmp = 0x0;
		    }
	}
	
}

void byte2bit(int *bit_stream,unsigned char *byte_stream,unsigned int byte_len)
{
	unsigned int i,j;
	for(i=0;i<byte_len;i++){
		for(j=0;j<8;j++){
			*(bit_stream+i*8+j) = (*(byte_stream+i)>>(7-j))&0x1;
		}
	}
}

void crc_lut (unsigned char* stream_crc, unsigned char* stream, unsigned int datasize, unsigned char crctype)
{

	unsigned int i;
	unsigned int tbl_idx;
	unsigned char crc_tmp[3]={0};
	crc_t crc=0x0;
	switch (crctype)
	{
	case 0://CRC 24a
		{
			for(i=0;i<datasize;i++) {
				tbl_idx = ((crc >> 16) ^ *(stream+i)) & 0xff;
				crc = (crc_table_24a[tbl_idx] ^ (crc << 8)) & 0xffffff;
			}
			crc = crc ^ 0x000000;
			crc_tmp[0]=(crc>>16)&0xff;
			crc_tmp[1]=(crc>>8)&0xff;
			crc_tmp[2]=crc&0xff;
			for(i=0;i<datasize+3;i++){
				if(i<datasize)
					*(stream_crc+i) = *(stream + i);
				else
					*(stream_crc+i) =crc_tmp[i-datasize];
			} 
			break;
		}
	case 1://CRC 24b
		{
			for(i=0;i<datasize;i++) {
				tbl_idx = ((crc >> 16) ^ *(stream+i)) & 0xff;
				crc = (crc_table_24b[tbl_idx] ^ (crc << 8)) & 0xffffff;
			}
			crc = crc ^ 0x000000;
			crc_tmp[0]=(crc>>16)&0xff;
			crc_tmp[1]=(crc>>8)&0xff;
			crc_tmp[2]=crc&0xff;
			for(i=0;i<datasize+3;i++){
				if(i<datasize)
					*(stream_crc+i) = *(stream + i);
				else
					*(stream_crc+i) =crc_tmp[i-datasize];
			}
			break;
		}
	case 2://CRC 16
		{
			for(i=0;i<datasize;i++) {
				tbl_idx = ((crc >> 8) ^ *(stream+i)) & 0xff;
				crc = (crc_table_16[tbl_idx] ^ (crc << 8)) & 0xffff;
			}
			crc = crc ^ 0x0000;
			crc_tmp[0]=(crc>>8)&0xff;
			crc_tmp[1]=crc&0xff;
			for(i=0;i<datasize+2;i++){
				if(i<datasize)
					*(stream_crc+i) = *(stream + i);
				else
					*(stream_crc+i) =crc_tmp[i-datasize];
			}
			break;
		}
	case 3://CRC 8
		{
			for(i=0;i<datasize;i++){
				tbl_idx = (crc ^ (*(stream + i))) & 0xff;
				crc = crc_table_8[tbl_idx];
			}
			crc = crc ^ 0x00;
			crc_tmp[0]=crc&0xff;
			for(i=0;i<datasize+1;i++){
				if(i<datasize)
					*(stream_crc+i) = *(stream + i);
				else
					*(stream_crc+i) =crc_tmp[i-datasize];
			}
			break;
		}
	}

}

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

	length = datasize + crc_poly_size;
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
//return ACK: 1 & 0
int crc_check (int* stream, int datasize, int crctype)
{
	// Variables:
	int     i, j;
	int     crc_poly_size; 
	int     length;        
	int*    dividend;     
	int     crc_poly[25]       = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	int     m_crc_poly_24a[25] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	int     m_crc_poly_24b[25] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	int     m_crc_poly_16[17]  = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	int     m_crc_poly_8[9]    = {0,0,0,0,0,0,0,0,0};

	int ack = 1;

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
		for(i=0;i<crc_poly_size;i++)
		  crc_poly[i] = m_crc_poly_24a[i];
		break;

		// 24bit-typeB
	case 1:
		crc_poly_size = 25;
		for(i=0;i<crc_poly_size;i++)
		  crc_poly[i] = m_crc_poly_24b[i];
		break;

		// 16bit
	case 2:
		crc_poly_size = 17;
		for(i=0;i<crc_poly_size;i++)
		  crc_poly[i] = m_crc_poly_16[i];
		break;

		// 8bit
	case 3:
		crc_poly_size = 9;
		for(i=0;i<crc_poly_size;i++)
		  crc_poly[i] = m_crc_poly_8[i];
		break;

	default:
		exit(0);
	}

	length = datasize - crc_poly_size + 1;
	dividend = (int*) malloc(sizeof(int) * datasize);
	for (i=0; i<length; i++)
	{
		*(dividend+i) = *(stream + i);
	}
	for (i=0; i<(crc_poly_size-1); i++) 
	{
		*(dividend+i+length) = 0;
	}

	// Execute:
	for (i=0; i<length; i++) 
	{
		if (*(dividend+i) != 0) 
		{
			for (j=0; j<crc_poly_size; j++) 
			{
				*(dividend+i+j) ^= *(crc_poly+j);
			}
		}
	}


	for (i=0; i<(crc_poly_size-1); i++)
	{
		if (*(stream+i+length) != *(dividend+i+length)){
			ack = 0;
			break;
		}
	}
	// memory free:
	free(dividend);

	return ack;
}


#endif
