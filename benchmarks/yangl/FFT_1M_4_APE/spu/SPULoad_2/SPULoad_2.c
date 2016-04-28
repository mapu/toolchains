/* Function Name: row FFT
 * Author       : shanshanzhang
 * Description  : This SPU program is 
 */

/* Include the header file below to load MPU program to Local Memory */
#include "mlib_mgr.h"
/* Include mspu C Intrinsic Library to use the intrinsic functions */
#include <mspu-intrin.h>

////////////////////////////FFT
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "fft.h"

extern MMPUFn MPUFunc;

int main(void) {

  int col_row=32;
  int j;
  enum {count=1024};
  complex* DM0_base, *DM1_base;
  complex*  DM2_base;
  complex*  DM3_base;
  complex*  DM4_base;
  complex*  DM5_base;
  DM0_base=(complex*) 0x400000;
  DM1_base=(complex*) 0x600000;
  DM2_base=(complex*) 0x800000;
  DM3_base=(complex*) 0xA00000;
  DM4_base=(complex*) 0xC00000;
  DM5_base=(complex*) 0xE00000;

  complex* TD_base_DM0;
  complex* TD_base_DM1;

    for (j=0;j<col_row;j++)
    {
	
	TD_base_DM0 = DM0_base + j * 0x400 ; //per line 's base addr
	TD_base_DM1 = DM1_base + j * 0x400 ; //per line 's base addr

	fft(TD_base_DM0,count);
	fft(TD_base_DM1,count);	
    }

   // Load MPU program to Local Memory
   MLOAD(MPUFunc);
   // Call MPU program
   __mspu_callmb(MPUFunc);

return 0;
}







