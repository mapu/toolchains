/* Function Name: col fft
 * Author       : shanshZhang
 * Time         : 2015/12/31
 * Description  : This SPU program is fft  
 */

/* Include the header file below to load MPU program to Local Memory */
#include "mlib_mgr.h"
/* Include mspu C Intrinsic Library to use the intrinsic functions */
#include <mspu-intrin.h>

////////FFT////////////////////
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "fft.h"

extern MMPUFn MPUFunc;

int main(void) {

  enum {count=1024};
  int col_row=32;
  int n,j,i;
  complex *TD;
  float* TD_base;
  complex* b_base;
  float TD_real,TD_imag;
  float b_real,b_imag;
  float* DM0_base;
  complex* DM1_base;
  complex* DM2_base;
  complex* DM3_base;
  complex* DM4_base;
  complex* DM5_base;
  DM0_base=(float*) 0x400000;
  DM1_base=(complex*) 0x600000;
  DM2_base=(complex*) 0x800000;
  DM3_base=(complex*) 0xA00000;
  DM4_base=(complex*) 0xC00000;
  DM5_base=(complex*) 0xE00000;

/////////////begin//////////////////
  for ( n=0;n<2;n++)
  {
    for (j=0;j<col_row;j++)
    {
	
	TD_base=DM0_base + j * 0x400 + n *col_row * 0x400; //per line 's base addr

        if(n==0)
	{
	  TD=(complex*) (DM4_base+ j * 0x400);//per line of the DATEOUT 's base addr
	} 
	else
	{
	  TD=(complex*) (DM3_base+ j * 0x400);
	}
//DM0  DATE IN DM4 or DM3
	for(i=0;i<count;i++)
	{
		(TD+i)->real = *(TD_base+i);
		(TD+i)->imag = 0.0;	
	}
//fft	
	fft(TD,count);
//twiddle factor
	for(i=0;i<1024;i++)
	{
          if(n==0)
	  {
	   b_base=DM1_base + j * 0x400;
	   TD_real=(TD+i)->real;
	   TD_imag=(TD+i)->imag;
	   b_real=(b_base+i)->real;
	   b_imag=(b_base+i)->imag;
	   (TD+i)->real =  TD_real * b_real - TD_imag * b_imag;
	   (TD+i)->imag =  TD_real * b_imag + TD_imag * b_real;
	  }
	  else
	  { 
	   b_base=DM2_base + j * 0x400;
	   TD_real=(TD+i)->real;
	   TD_imag=(TD+i)->imag;
	   b_real=(b_base+i)->real;
	   b_imag=(b_base+i)->imag;
	   (TD+i)->real =  TD_real * b_real - TD_imag * b_imag;
	   (TD+i)->imag =  TD_real * b_imag + TD_imag * b_real;
	  }
	}
    }
  }

   // Load MPU program to Local Memory
   MLOAD(MPUFunc);
   // Call MPU program
   __mspu_callmb(MPUFunc);

return 0;
}







