/*============================================================================*/
//  Information
/*============================================================================*/
/* 
*  @file    channel.c
*  @brief   
*  @author  wujunning
*  @date    2014/03/28
/*----------------------------------------------------------------------------*/

#ifndef CHANNEL_C
#define CHANNEL_C

/*============================================================================*/
//  Header files
/*============================================================================*/
#include "channel.h"

/*============================================================================*/
//  Function definition
/*============================================================================*/
void AWGN(Complex *r, Complex *send, float sigma, int totallength)
{
	int i;
	float *noise = (float *)malloc(sizeof(float)*totallength*2);
	float seed =  (float)(3.0 - (float)((rand() & RAND_MAX)/(float)RAND_MAX)/10e6);
	mgrns(0,sigma,seed,2*totallength,noise);
	for(i=0; i<totallength; i++)
	{
		(r+i)->real = (float)( (send+i)->real + *(noise+2*i) );
		(r+i)->imag = (float)( (send+i)->imag + *(noise+2*i+1) );
	}
	free(noise);
}

void mgrns(float mean,float sigma,float seed,int n,float *a)
{ int i,k,m;
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
        *(a+k)=mean+(float)(sigma*(t-6.0));
    }
    return;
}

#endif
