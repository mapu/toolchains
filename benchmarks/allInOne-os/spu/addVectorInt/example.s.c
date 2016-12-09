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

struct param{
  int iter_num;
  int DataLength;
};

struct param params  __attribute__((section(".param")));

/*Declare the MPU function */
extern void addVectorInt(void);

int main() {
	//__label__ here;
	int DataLength = params.DataLength;
	int i = params.iter_num;

	//__mspu_loop(&&here, i);
	for (; i > 0; --i) {
		// BIU Configure, for BIU0
		__mspu_setKB(KB0, SDA0DM0_START);
		__mspu_setKS(KS0, 64);
		__mspu_setKI(KI0, DataLength / 16);
		__mspu_setKL(KL0, 1);
		__mspu_setKG(KG0, 6);

		// BIU Configure, for BIU1
		__mspu_setKB(KB4, SDA1DM0_START);
		__mspu_setKS(KS4, 64);
		__mspu_setKI(KI4, DataLength / 16);
		__mspu_setKL(KL1, 1);
		__mspu_setKG(KG1, 6);

		// BIU Configure, for BIU2
		__mspu_setKB(KB8, SDA2DM0_START);
		__mspu_setKS(KS8, 64);
		__mspu_setKI(KI8, DataLength / 16);
		__mspu_setKL(KL2, 1);
		__mspu_setKG(KG2, 6);

		__mspu_setKI(KI13, DataLength / 16);

		//Call the MPU program to compute the data
		MLOAD(addVectorInt);
		__mspu_callmb(addVectorInt);
	}
	//here:
	
  int val = 0;
  __mspu_setCh(MailboxOutInt, val);

	return 0;
}

