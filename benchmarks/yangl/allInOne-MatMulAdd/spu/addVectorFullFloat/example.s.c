/* Filename ： example.s.c
 * Author   :  shaolin.xie@ia.ac.cn
 * Description:  This is a example file to demostrate the 
 *               program of MaPU. 
 */

/* Include this file so that we can use the intrinsic funcitons */
#include <mspu-intrin.h> 
#include <mlib_mgr.h>

#define SDA0DM0_START 0x400000
#define SDA1DM0_START 0x800000
#define SDA2DM0_START 0xc00000

/*Declare the MPU function */
extern void addVector_fullFloat(void);

int main() {
	volatile float* a;
	volatile float* b;
	volatile int* cnt;

	unsigned int res;
	a = (volatile float *) 0x40401000;
	b = (volatile float *) 0x40402000;
	cnt = (volatile int*) 0x40403000;

	for (int i = 0; i < 16; i++)
		__mspu_setSVR(SVR0, i, *(int*) a, MODE_W);
	__mspu_setSHUWithSVR(SHU0, T0, SVR0);

	for (int i = 0; i < 16; i++)
		__mspu_setSVR(SVR0, i, *(int*) b, MODE_W);
	__mspu_setSHUWithSVR(SHU0, T1, SVR0);

	for (int i = *(volatile int*) 0x40404000; i; --i) {
		__mspu_setKI(KI12, (*cnt - 1) * 8);

		//Call the MPU program to compute the data
		MLOAD(addVector_fullFloat);
		__mspu_callmb(addVector_fullFloat);
	}

	__mspu_setSVRWithSHU(SVR0, SHU0, T2);
	res = __mspu_getSVR(SVR0, 0, MODE_W);

	*(volatile unsigned int*) 0x40403000 = res;

	return 0;
}
