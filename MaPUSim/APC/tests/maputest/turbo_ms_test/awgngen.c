/*============================================================================*/
//  Information
/*============================================================================*/
/* 
*  @file   awgngen.c
*  @brief   
*  @author  guojing
*  @date    2013/10/28
/*----------------------------------------------------------------------------*/
#ifndef AWGNGEN_C
#define AWGNGEN_C

/*============================================================================*/
//  Header files
/*============================================================================*/
#include "awgngen.h"

/*============================================================================*/
//  Function definition
/*============================================================================*/
/*----------------------------------------------------------------------------*/
//void AWGNgen(...)
/*----------------------------------------------------------------------------*/
// @brief: AWGN calculation

// @out_data:  output

// @in_data  :      input
// @in_data_len:    input: 
// @SNR:            input: SNR[dB]
/*----------------------------------------------------------------------------*/
void AWGNgen( Complex*    out_data    ,
			Complex*      in_data     ,
			int           in_data_len ,
			float        SNR
			)
{
	// Variables: ***************************************************************
	float         pi                  ;//��
	float         sigma               ;
	int            i                   ;// for loop
	float         t,u                 ;

	float* tx_abs;      //modulus of Complex signals
	float  square_exp = 0.0;  //E(X^2)

	// Initialization: ***********************************************************
	//printf("# [awgngen.c] Enter Into Func AWGNgen()!\n");

	pi = 4.0 * atan(1.0);

	// noise setting
	sigma = pow(10.0, -SNR/10.0) * 0.5;

	tx_abs = (float*)malloc(sizeof(float)*in_data_len);
	for(i=0;i<in_data_len;i++){
		tx_abs[i] = sqrt(in_data[i].real*in_data[i].real+in_data[i].imag*in_data[i].imag);
		square_exp += tx_abs[i]*tx_abs[i];
	}
	square_exp /= in_data_len;
	free(tx_abs);

	sigma *= square_exp;
	sigma = sqrt(sigma);

	// Execute: ******************************************************************
	for (i=0; i<in_data_len; i++) {

		t = sqrt(-2.0 * log(1.0 - (( 1.0 / ( RAND_MAX + 1.0)) * rand())));
		u = 2.0 * pi * ( 1.0 / ( RAND_MAX + 1.0)) * rand();

		out_data[i].real = in_data[i].real + sigma * t * cos(u);
		out_data[i].imag = in_data[i].imag + sigma * t * sin(u);
	}

}
#endif
