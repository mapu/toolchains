/*============================================================================*/
//  Information
/*============================================================================*/
/* 
*  @file    demodulation_serial.c
*  @brief   
*  @author  wujunning
*  @date    2014/03/14
/*----------------------------------------------------------------------------*/

#ifndef DEMODULATION_SERIAL_C
#define DEMODULATION_SERIAL_C

/*============================================================================*/
//  Header files
/*============================================================================*/
#include "demodulation_serial.h"


/*============================================================================*/
//  Function definition
/*============================================================================*/
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/

void demodulation_serial(float *output_soft_bit,Complex * input_symbol,int len, int qm){

	int i;

	float  one_over_sqrt_10 = 1/sqrt(10.0);
	float  one_over_sqrt_42 = 1/sqrt(42.0);

	switch(qm)
	{
	case 0://BPSK demodulation
		for(i=0;i<len;i++){
			*(output_soft_bit + i) = (input_symbol + i)->real;
		}
		break;
	case 1://QPSK demodulation
		for(i=0;i<len;i++){
			*(output_soft_bit + 2*i)     = (input_symbol + i)->real;
			*(output_soft_bit + 2*i + 1) = (input_symbol + i)->imag;
			//printf("%f + %f * i. %f,%f.\n",(input_symbol + i)->real,(input_symbol + i)->imag,*(output_soft_bit + 2*i),*(output_soft_bit + 2*i + 1));
		}
		break;
	case 2://16QAM demodulation
		for(i=0;i<len;i++){
			*(output_soft_bit + 4*i)         = (input_symbol + i)->real;
			*(output_soft_bit + 4*i + 1)     = (input_symbol + i)->imag;
			*(output_soft_bit + 4*i + 2)     = 2*one_over_sqrt_10 - fabs((input_symbol + i)->real);
			*(output_soft_bit + 4*i + 3)     = 2*one_over_sqrt_10 - fabs((input_symbol + i)->imag);
		}
		break;
	case 3://64QAM demodulation
		for(i=0;i<len;i++){
			*(output_soft_bit + 6*i)         = (input_symbol + i)->real;
			*(output_soft_bit + 6*i + 1)     = (input_symbol + i)->imag;
			*(output_soft_bit + 6*i + 2)     = 4*one_over_sqrt_42 - fabs((input_symbol + i)->real);
			*(output_soft_bit + 6*i + 3)     = 4*one_over_sqrt_42 - fabs((input_symbol + i)->imag);
			*(output_soft_bit + 6*i + 4)     = 2*one_over_sqrt_42 - fabs(4*one_over_sqrt_42 - fabs((input_symbol + i)->real));
			*(output_soft_bit + 6*i + 5)     = 2*one_over_sqrt_42 - fabs(4*one_over_sqrt_42 - fabs((input_symbol + i)->imag));
		}
		break;
	}

}
#endif
