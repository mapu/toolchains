//  Date: 2013-07-22
//  Name: app.s.asm
//  Author: tao.wang@ia.ac.cn
//  The configuration Unit for MPU for Matrix Mul Test
//  The input Matrix M*P*N can ben any size. The Matrix is manipulated in row mode.
#include <mspu-intrin.h>
#include <mlib_mgr.h>

#define MAX_LENGTH 64

extern void Mat_Mul_SF_Test();
// begin program code */
////////////////////////////////////////////////////////////////////////////////
// get the MASK
// A: M * P, in Row mode
//    M: KE to dispose the un-BS size
//    P: MASK to dispose the un-BS size
// B: P * N, in Row mode
//    P: ld without A++/K++ to dispose the un-BS size. ld the same address
//    N: MASK to dispose the un-BS size
// C: M * N, in Row mode
//    M: KE to dispose the un-BS size
//    N: MASK to dispose the un-BS size
int main() {
	int AMaskL, AMaskH, BMaskL, BMaskH, CMaskL, CMaskH, AShiftNum, AMaskLTemp,
			AMaskHTemp, BShiftNum, BMaskLTemp, BMaskHTemp, tmp;

  ////////////////////////////////////////////////////////////////////////
  // the following need to modify
  // the input size
  int ARowNum = MAX_LENGTH;
  int AColNum = MAX_LENGTH;
  int BRowNum = MAX_LENGTH;
  int BColNum = MAX_LENGTH;
  ///////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////
  // the following does not need to modify
  // parameter definition  
  int SDA0DM0_START = 0x400000;
  int SDA1DM0_START = 0x800000;
  int SDA2DM0_START = 0xc00000;
  int SDASize = 0x40000;

  const int BS = 16;
  int HalfBS = (BS / 2);
  int BSGran = 4;
  int BankNum = BS;
  int BankNumGran = 4;
  int BankSize = (SDASize >> BankNumGran);

  int BlockSize = 64;      // the total Rd size, for Row Rd is 512/8     
  int BlockGran = 6;       //  Block Granularity = log2(BlockSize)
  int DataSize = 4;       //  Byte 1, Short 2, Word 4, Long 8

  int BankCap = (ARowNum >> BankNumGran);    // the num of Row in each Bank
  int ACirNum = 4;                         // the number of circle
  int BCirNum = 3;
  int CCirNum = 4;

  int ARowBSNum = (ARowNum >> BSGran);
  int AColBSNum = (AColNum >> BSGran);
  int BRowBSNum = (BRowNum >> BSGran);
  int BColBSNum = (BColNum >> BSGran);

  // configure for the no BS data, the tail data
  int ARowTailNum = (ARowNum % BS);             // A's Row tail number
  int ARowTailEn = (ARowTailNum > 0);          // A's Row tail number enable
  int ARowBSCeilNum = (ARowBSNum + ARowTailEn);  // A's Row ceil number

  int AColTailNum = (AColNum % BS);             // A's Col tail number
  int AColTailEn = (AColTailNum > 0);          // A's Col tail number enable
  int AColBSCeilNum = (AColBSNum + AColTailEn);  // A's Col ceil number

  int BRowTailNum = (BRowNum % BS);             // A's Row tail number
  int BRowTailEn = (BRowTailNum > 0);          // A's Row tail number enable
  int BRowBSCeilNum = (BRowBSNum + BRowTailEn);   // B's Row ceil number

  int BColTailNum = (BColNum % BS);             // B's Col tail number
  int BColTailEn = (BColTailNum > 0);          // B's Col tail number enable
  int BColBSCeilNum = (BColBSNum + BColTailEn);  // B's Col ceil number

	if (AColTailNum == 0) {
		AMaskL = 0xffffffff;
		AMaskH = 0xffffffff;
	} else if (AColTailNum < HalfBS) {  // A MASK The none BS col
		AShiftNum = AColTailNum * DataSize;
		AMaskLTemp = 0xffffffff << AShiftNum;
		AMaskL = ~AMaskLTemp;
		AMaskH = 0;
	} else {
		AShiftNum = AColTailNum * DataSize - 32;
		AMaskHTemp = 0xffffffff << AShiftNum;
		AMaskL = 0xffffffff;
		AMaskH = ~AMaskHTemp;
	}

	if (BColTailNum == 0) {
		BMaskL = 0xffffffff;
		BMaskH = 0xffffffff;
	} else if (BColTailNum < HalfBS)                   // B MASK The none BS col
			{
		BShiftNum = BColTailNum * DataSize;
		BMaskLTemp = 0xffffffff << BShiftNum;
		BMaskL = ~BMaskLTemp;
		BMaskH = 0;
	} else {
		BShiftNum = BColTailNum * DataSize - 32;
		BMaskHTemp = 0xffffffff << BShiftNum;
		BMaskL = 0xffffffff;
		BMaskH = ~BMaskHTemp;
	}

	CMaskL = BMaskL;
	CMaskH = BMaskH;
///////////////////////////////////////////////////////////////////////////////////////////////
// for MC Config   
	int MIndexStart = 0x0;
	int MStepMode = 0x1;
	int MLen = 0x10;
	int ISize = 0x10;

	int MIndexOffset = 0;
	int MStepOffset = 8;
	int MLenOffset = 24;
	int ISizeOffset = 12;

	int MCIndexStart = (MIndexStart << MIndexOffset);
	int MCStepMode = (MStepMode << MStepOffset);
	int MCLen = (MLen << MLenOffset);
	int ICSize = (ISize << ISizeOffset);
	int MConfig = (MCIndexStart | MCStepMode | MCLen | ICSize);
////////////////////////////////////////////////////////////////////////////

		// MC
		__mspu_setMC(MC_r0, MConfig);
		__mspu_setMC(MC_w1, MConfig);

		// for Loop
		__mspu_setKI(KI12, ARowNum);
		tmp = AColBSNum + AColTailEn;
		__mspu_setKI(KI13, tmp);
		tmp = BColBSNum + BColTailEn;
		__mspu_setKI(KI14, tmp);

		// configure the K register
		// Read the date in Row mode
		// for BIU0
		__mspu_setKB(KB0, SDA0DM0_START);
		__mspu_setKS(KS0, BankSize);
		__mspu_setKI(KI0, BS);
		__mspu_setKE(KE0, ARowTailNum);                   // KE0, IMPORTANT!!!

		__mspu_setKB(KB1, SDA0DM0_START);
		tmp = AColNum * DataSize;
		__mspu_setKS(KS1, tmp);
		__mspu_setKI(KI1, ARowBSNum);

		__mspu_setKB(KB2, SDA0DM0_START);
		__mspu_setKS(KS2, BlockSize);
		__mspu_setKI(KI2, AColBSCeilNum);

		__mspu_setKB(KB3, SDA0DM0_START);
		tmp = 0;
		__mspu_setKS(KS3, tmp);                   //IMPORTANT!!!
		__mspu_setKI(KI3, BColBSCeilNum);

		__mspu_setKG(KG0, BlockGran);
		__mspu_setKL(KL0, ACirNum);

		__mspu_setKM(KM0, AMaskL);
		__mspu_setHiKM(KM0, AMaskH);

		// for BIU1, the BIU1 is related to M[I++],when the row number is not aligend to BS, it ld with no A/K
		__mspu_setKB(KB4, SDA1DM0_START);
		__mspu_setKS(KS4, BankSize);
		__mspu_setKI(KI4, BS);
		__mspu_setKE(KE4, BRowTailNum);                   // KE4, IMPORTANT!!!

		__mspu_setKB(KB5, SDA1DM0_START);
		tmp = BColNum * DataSize;
		__mspu_setKS(KS5, tmp);
		__mspu_setKI(KI5, BRowBSNum);

		__mspu_setKB(KB6, SDA1DM0_START);
		__mspu_setKS(KS6, BlockSize);
		__mspu_setKI(KI6, BColBSCeilNum);

		__mspu_setKG(KG1, BlockGran);
		__mspu_setKL(KL1, BCirNum);

		__mspu_setKM(KM1, BMaskL);
		__mspu_setHiKM(KM1, BMaskH);

		// for BIU2
		__mspu_setKB(KB8, SDA2DM0_START);
		__mspu_setKS(KS8, BankSize);
		__mspu_setKI(KI8, BS);
		__mspu_setKE(KE8, ARowTailNum);                   // KE8, IMPORTANT!!!

		__mspu_setKB(KB9, SDA2DM0_START);
		tmp = BColNum * DataSize;
		__mspu_setKS(KS9, tmp);
		__mspu_setKI(KI9, ARowBSNum);

		__mspu_setKB(KB10, SDA2DM0_START);
		tmp = 0;
		__mspu_setKS(KS10, tmp);
		__mspu_setKI(KI10, AColBSCeilNum);

		__mspu_setKB(KB11, SDA2DM0_START);
		__mspu_setKS(KS11, BlockSize);                   //IMPORTANT!!!
		__mspu_setKI(KI11, BColBSCeilNum);

		__mspu_setKG(KG2, BlockGran);
		__mspu_setKL(KL2, CCirNum);

		__mspu_setKM(KM2, CMaskL);
		__mspu_setHiKM(KM2, CMaskH);

		//  Matrix W Add Test
		MLOAD(Mat_Mul_SF_Test);
		__mspu_callmb(Mat_Mul_SF_Test);

	return 0;
}  //end of main()
