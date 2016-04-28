/* Function Name: FFT.h
 * Author       : shanshanZhang
 * Description  : fft head function
 */

#ifndef __FFT_H__
#define __FFT_H__

#include  <math.h>

typedef struct complex
{
    float real;
    float imag;
}complex;

#define PI 3.141592653589793

void c_plus(complex a,complex b,complex *c);
void c_mul(complex a,complex b,complex *c);
void c_sub(complex a,complex b,complex *c);
void fft(complex f[],int N);
#endif


void c_plus(complex a,complex b,complex *c)
{
	c->real = a.real + b.real;
	c->imag = a.imag + b.imag;
}

void c_mul(complex a,complex b,complex *c)
{
	c->real = a.real * b.real - a.imag * b.imag;
	c->imag = a.real * b.imag + a.imag * b.real;
}

void c_sub(complex a,complex b,complex *c)
{
	c->real = a.real - b.real;
	c->imag = a.imag - b.imag;
}

void Wn_i(int n,int i,complex *Wn)
{
   double x , y ,z;
   x= 2.0 * PI * i/n ;
// cos (2 * PI * i/n);
   y= 1- x*x/(2.0*1.0)+x*x*x*x/(4.0*3.0*2.0*1.0)-x*x*x*x*x*x/(6.0*5.0*4.0*3.0*2.0*1.0)+x*x*x*x*x*x*x*x/(8.0*7.0*6.0*5.0*4.0*3.0*2.0*1.0);
   Wn->real = (float) y ;
//-sin (2 * PI * i/n);
   z= -(x-x*x*x/(3.0*2.0*1.0)+x*x*x*x*x/(5.0*4.0*3.0*2.0*1.0)-x*x*x*x*x*x*x/(7.0*6.0*5.0*4.0*3.0*2.0*1.0)+x*x*x*x*x*x*x*x*x/(9.0*8.0*7.0*6.0*5.0*4.0*3.0*2.0*1.0)) ;
   Wn->imag = (float) z;
}

void fft(complex f[],int N)
{
	complex t,wn;//intermediate variable
	int i,j,k,l,m,n,M;
	float r;
	int La,Lb,Lc;
	/*M=log2(N)*/
	for(i=N,M=1;(i=i/2)!=1;M++);
	/*reverse order index*/
	for(i=1,j=N/2;i<=N-2;i++)
	{
		if (i<j)
		{
			t=f[j];
			f[j]=f[i];
			f[i]=t;
		}
		k=N/2;
		while(k<=j)
		{
			j=j-k;
			k=k/2;
		}
		j=j+k;
	}
	////FFT////
	for(m=1;m<=M;m++)
	{
		La =1<<m;//La=2^m is mean the point number in m level
		Lb =La/2;//the number of butterfly and it's the distance between two nodes in each butterfly

		////butterfly////
		for(l=1;l<=Lb;l++)
		{
			r=(l-1)*(1<<(M-m));
			for(n=l-1;n<N-1;n=n+La)//every butterfly group,and the num is N/La
			{
				Lc=n+Lb;//n is upper num,Lc is down num
				Wn_i(N,r,&wn);//wn=Wn_r
				c_mul(f[Lc],wn,&t);//t = f[Lc] * wn
				c_sub(f[n],t,&(f[Lc]));//f[Lc] = f[n] -f[Lc] * wn
				c_plus(f[n],t,&(f[n]));//f[n] = f[n] +f[Lc] * wn
			}
		}
	}

}




