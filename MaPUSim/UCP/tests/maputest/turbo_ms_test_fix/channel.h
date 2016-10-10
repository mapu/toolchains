#ifndef CHANNEL_H
#define CHANNEL_H


#include "stdio.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "math.h"

#include "define.h"
void mgrns(Complex *awgn_noise, float mean,float sigma,float seed,int n);

void AWGN(Complex *rec_sym, Complex *sym, float sigma, int sym_len);

void AWGNgen( Complex*    out_data    ,
			 Complex*      in_data     ,
			 int           in_data_len ,
			 float        SNR
			 );

#endif
