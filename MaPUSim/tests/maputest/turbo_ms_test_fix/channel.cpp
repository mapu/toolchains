#ifndef CHENNEL_C
#define CHANNEL_C

#include "channel.h"

void mgrns(Complex *awgn_noise, float mean,float sigma,float seed,int n)
{	
	int i,k,m;
	float s,w,v,t;

	s=65536.0; w=2053.0; v=13849.0;

	for (k=0; k<=n-1; k++)
	{
		t=0.0;
		for (i=1; i<=12; i++)
		{ 
			seed=seed*w+v; m=(int)(seed/s);
			seed=seed-m*s; t=t+(seed)/s;
		}
		awgn_noise[k].real=mean+(sigma*(t-6.0));

		t=0.0;
		for (i=1; i<=12; i++)
		{ 
			seed=seed*w+v; m=(int)(seed/s);
			seed=seed-m*s; t=t+(seed)/s;
		}
		awgn_noise[k].imag=mean+(sigma*(t-6.0));

	}

}

void AWGN(Complex *rec_sym, Complex *sym, float sigma, int sym_len)
{
	int i;
	Complex *noise = (Complex *)malloc(sizeof(Complex)*sym_len);
	float seed =  (float)(3.0 - (float)((rand() & RAND_MAX)/(float)RAND_MAX)/10e6);

	mgrns(noise, 0,sigma,seed,sym_len);

	for(i=0; i<sym_len; i++)
	{
		rec_sym[i].real =  sym[i].real + noise[i].real ;
		rec_sym[i].imag =  sym[i].imag + noise[i].imag ;
		//	*(r+i) = (float)( *(send+i) + 0 );
	}
	free(noise);
}

void AWGNgen( Complex*    out_data    ,
			 Complex*      in_data     ,
			 int           in_data_len ,
			 float        SNR
			 )
{
	// Variables: ***************************************************************
	float         pi                  ;//
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
