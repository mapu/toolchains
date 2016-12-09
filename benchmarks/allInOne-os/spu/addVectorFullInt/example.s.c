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
extern void addVectorFullInt(void);

int main(int argc, char *argv[]) {
	int res;
	int* cnt = (volatile int*) 0x40401000;
	char init1, init2;
	init1 = -1;
	init2 = 1;

	for (int i = 0; i < 64; i++)
		__mspu_setSVR(SVR0, i, init1, MODE_B);
	__mspu_setSHUWithSVR(SHU0, T0, SVR0);

	for (int i = 0; i < 64; i++)
		__mspu_setSVR(SVR0, i, init2, MODE_B);
	__mspu_setSHUWithSVR(SHU0, T1, SVR0);

	for (int i = *(volatile int*) 0x40402000; i; --i) {
		__mspu_setKI(KI12, *cnt);

		//Call the MPU program to compute the data
		MLOAD(addVectorFullInt);
		__mspu_callmb(addVectorFullInt);
	}

	__mspu_setSVRWithSHU(SVR0, SHU0, T2);
	res = __mspu_getSVR(SVR0, 0, MODE_H);
	*(volatile int*) 0x40403000 = res;

	return 0;
}
