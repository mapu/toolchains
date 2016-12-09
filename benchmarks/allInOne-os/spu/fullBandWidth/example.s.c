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
extern void fullBandWidth(void);

int main(int argc, char *argv[]) {
	int DataLength = *(volatile int*) 0x40401000; //length of vector
	int i = *(volatile int*) 0x40401004;
	char imm = 0xf;

	for (int j = 0; j < 64; j++)
		__mspu_setSVR(SVR0, j, imm, MODE_B);
	__mspu_setSHUWithSVR(SHU0, T0, SVR0);

	for (; i; --i) {
		// BIU Configure, for BIU0
		__mspu_setKB(KB0, SDA0DM0_START);
		__mspu_setKS(KS0, 64);
		__mspu_setKI(KI0, DataLength / 64);
		__mspu_setKL(KL0, 1);
		__mspu_setKG(KG0, 6);

		// BIU Configure, for BIU1
		__mspu_setKB(KB4, SDA1DM0_START);
		__mspu_setKS(KS4, 64);
		__mspu_setKI(KI4, DataLength / 64);
		__mspu_setKL(KL1, 1);
		__mspu_setKG(KG1, 6);

		// BIU Configure, for BIU2
		__mspu_setKB(KB8, SDA2DM0_START);
		__mspu_setKS(KS8, 64);
		__mspu_setKI(KI8, DataLength / 64);
		__mspu_setKL(KL2, 1);
		__mspu_setKG(KG2, 6);

		__mspu_setKI(KI12, DataLength / 128);

		//Call the MPU program to compute the data
		MLOAD(fullBandWidth);
		__mspu_callmb(fullBandWidth);
	}

	return 0;
}
