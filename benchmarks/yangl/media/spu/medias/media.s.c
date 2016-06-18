//  Date: 2016-03-15
//  Author: Yanglei
//  PolyPhase 8 Tap, Width, (240 + 7) * (135 + 7)-> (480) * (270), Modification for multi-tap
#include <mspu-intrin.h>
#include <mlib_mgr.h>

const int SDA0DM0_START = 0x400000 ;
const int SDA0DM1_START = 0x600000;
const int SDA1DM0_START = 0x800000 ;
const int SDA2DM0_START = 0xc00000 ;
const int SDA2DM1_START = 0xe00000 ;
const int SDASize       = 0x40000;


extern void W_PolyPhase();
extern void H_PolyPhase();

int main() 
{
  //W, Col. Example: (240 + 7) * (135 + 7) -> (480) * (135 + 7)
  //********************************************************************************************
  int nTap = 8;
  int PadNum = nTap - 1;
  int inW = 163;
  int inH = 62;
  int outW = 2 * inW;
  int outH = inH;
  int nBits = 10;

  int AColNum = inW;
  int ARowNum = inH;

  int AColPadNum = inW + PadNum;    //The width after Padding in  Vetical Interplation
  int ARowPadNum = inH + PadNum;

  int BS = 32;// Data type
  int DataSize = 2;//  Byte 1, Short 2, Word 4, Long 8
  int DataSizeGran = 1; 
  ///////////////////////////////////////////////////////////////////////  

  int ARowPadBSTailNum = ARowPadNum % BS;
  int ARowPadBSTailEn = (ARowPadBSTailNum != 0);    
  int ARowPadBSNum = ARowPadNum / BS + ARowPadBSTailEn;
  
  int MaskL, MaskH, ShiftNum;
  ShiftNum = ARowPadBSTailNum * 2;//for Mask, BIU0
  if(ARowPadBSTailNum <= 16 ) {
    MaskL = ~(0xffffffff << ShiftNum);
    MaskH = 0;
  }
  else{
    MaskL = 0xffffffff;
    MaskH = ~(0xffffffff << (ShiftNum - 32) );
  }
  
  // Circle num of interpolation in width direction
  int W_CirNum = 2;                         

  // for MC Config. Interpolation in Width direction
  const int W_MIndexStart   = 0x16;    
  const int W_MStepMode     = 0x1;     
  const int W_MLen          = nTap;
  const int W_ISize         = nTap;

  const int MIndexOffset  = 0; 
  const int MStepOffset   = 8;
  const int MLenOffset    = 24;
  const int ISizeOffset   = 12;

  const int W_MCIndexStart  = W_MIndexStart << MIndexOffset; 
  const int W_MCStepMode    = W_MStepMode   << MStepOffset;
  const int W_MCLen         = W_MLen        << MLenOffset;
  const int W_ICSize        = W_ISize       << ISizeOffset;
  const int W_MConfig       = W_MCIndexStart | W_MCStepMode | W_MCLen | W_ICSize;

  const int W_iMC = W_MConfig; 
  __mspu_setMC(MC_r0, W_iMC);
  __mspu_setMC(MC_w0, W_iMC);


  ///////////////////////////////////////////////////////////
  int nDBits = nBits + 1;
  short nHalf = 1 << (nBits - 1);
  
  //coefficients for different nTap
  short FILTER_PHASE64_TAP8_NOSHARP_Lanczos[2][8] = {{0, 0, 0, 1024, 0, 0, 0, 0},{-13, 61, -170, 634, 634, -170, 61, -13}};

  // SHU0.T0, coefficient first group 
      for(int i = 0;i < nTap; i++)
        __mspu_setSVR(SVR0, i, FILTER_PHASE64_TAP8_NOSHARP_Lanczos[0][i], MODE_H);
  __mspu_setSHUWithSVR(SHU0, T0, SVR0); 

  // SHU0.T1, coefficient second group 
      for(int i = 0;i < nTap; i++)
        __mspu_setSVR(SVR0, i, FILTER_PHASE64_TAP8_NOSHARP_Lanczos[1][i], MODE_H);
  __mspu_setSHUWithSVR(SHU0, T1, SVR0); 


  __mspu_setSVR(SVR0, 0, 0x400, MODE_H);//max value
  __mspu_setSVR(SVR0, 1, 0, MODE_H);//min value
  __mspu_setSVR(SVR0, 2, nHalf, MODE_H);
  __mspu_setSVR(SVR0, 3, 1, MODE_H);//for nHalf * 1
  __mspu_setSHUWithSVR(SHU0, T2, SVR0); // SHU0.T2, 0xff, 0, nHalf, 1


  // configure the LOOP
  __mspu_setKI(KI12, AColNum);
  __mspu_setKI(KI13, ARowPadBSNum);

  // BIU Configure, for BIU0
  __mspu_setKB(KB0, SDA0DM0_START);
  __mspu_setKS(KS0, DataSize);
  __mspu_setKI(KI0, AColPadNum);

  __mspu_setKB(KB1, SDA0DM0_START);
  __mspu_setKS(KS1, DataSize * AColPadNum);
  __mspu_setKI(KI1, ARowPadBSNum);

  __mspu_setKG(KG0, DataSizeGran);
  __mspu_setKL(KL0, W_CirNum);

  __mspu_setKM(KM0, MaskL);
  __mspu_setHiKM(KM0, MaskH);

  // BIU Configure, for BIU1
  __mspu_setKB(KB4, SDA1DM0_START);
  __mspu_setKS(KS4, DataSize);
  __mspu_setKI(KI4, outW);

  __mspu_setKB(KB5, SDA1DM0_START);
  __mspu_setKS(KS5, outW *  DataSize);
  __mspu_setKI(KI5, ARowPadBSNum);

  __mspu_setKG(KG1, DataSizeGran);
  __mspu_setKL(KL1, W_CirNum);

  __mspu_setKM(KM1, MaskL);
  __mspu_setHiKM(KM1, MaskH);

  MLOAD(W_PolyPhase);
  __mspu_callmb(W_PolyPhase); //call


  //H, Row
  //********************************************************************************************

  // Circle num of interpolation in heigth direction  

  outH = 2 * inH;
  outW = 2 * inW;                    
  int H_CirNum       = 3;
  int BlockSize     = 64; // the total Rd size, for Row Rd is 512/8
  int BlockGran     = 6; // log2(BlockSize)
  int BankNum       = BS;//32
  int BankSize = SDASize / BankNum;


  // for MC Config. Interpolation in Width direction
  __mspu_setMC(MC_r0, W_iMC);
  __mspu_setMC(MC_w1, W_iMC);


  // SHU0.T0, coefficient first group 
      for(int i = 0;i < nTap; i++)
        __mspu_setSVR(SVR0, i, FILTER_PHASE64_TAP8_NOSHARP_Lanczos[0][i], MODE_H);
  __mspu_setSHUWithSVR(SHU0, T0, SVR0); 

  // SHU0.T1, coefficient second group 
      for(int i = 0;i < nTap; i++)
        __mspu_setSVR(SVR0, i, FILTER_PHASE64_TAP8_NOSHARP_Lanczos[1][i], MODE_H);
  __mspu_setSHUWithSVR(SHU0, T1, SVR0);
  
  //__mspu_setSVR(SVR0, 0, nDBits, MODE_H);
  __mspu_setSVR(SVR0, 0, 0x400, MODE_H);
  __mspu_setSVR(SVR0, 1, 0, MODE_H);
  __mspu_setSVR(SVR0, 2, nHalf, MODE_H);
  __mspu_setSVR(SVR0, 3, 1, MODE_H);
  __mspu_setSHUWithSVR(SHU0, T2, SVR0); // SHU0.T2, 0xff, 0, nHalf, 1

  //mask
  //int MaskL, MaskH, ShiftNum;
  int TailNum = outW % BS;
  ShiftNum = TailNum * 2;//for Mask, BIU0
  if(TailNum <= 16 ) {
    MaskL = ~(0xffffffff << ShiftNum);
    MaskH = 0;
  }
  else{
    MaskL = 0xffffffff;
    MaskH = ~(0xffffffff << (ShiftNum - 32) );
  }

  // BIU Configure, for BIU1
  __mspu_setKB(KB4, SDA1DM0_START);
  __mspu_setKS(KS4, BankSize);
  __mspu_setKI(KI4, BS);
  __mspu_setKE(KE4, ARowPadNum % BS);//IMPORTANT !!!

  __mspu_setKB(KB5, SDA1DM0_START);
  __mspu_setKS(KS5, outW * DataSize);
  __mspu_setKI(KI5, ARowPadNum / BS);//IMPORTANT !!!

  __mspu_setKB(KB6, SDA1DM0_START);
  __mspu_setKS(KS6, BS * DataSize);
  int temp0;
  if(outW % BS == 0)
    temp0 = outW / BS;
  else
    temp0 = outW / BS + 1;
  __mspu_setKI(KI6, temp0);

  __mspu_setKG(KG1, BlockGran);
  __mspu_setKL(KL1, H_CirNum);
  __mspu_setKM(KM1, MaskL);
  __mspu_setHiKM(KM1, MaskH);

  // BIU Configure, for BIU2
  __mspu_setKB(KB8, SDA2DM0_START);
  __mspu_setKS(KS8, BankSize);
  __mspu_setKI(KI8, BS);
  __mspu_setKE(KE8, outH % BS);//IMPORTANT !!!

  __mspu_setKB(KB9, SDA2DM0_START);
  __mspu_setKS(KS9, outW * DataSize);
  __mspu_setKI(KI9, outH / BS);

  __mspu_setKB(KB10, SDA2DM0_START);
  __mspu_setKS(KS10, BS * DataSize);
  __mspu_setKI(KI10, temp0);

  __mspu_setKG(KG2, BlockGran);
  __mspu_setKL(KL2, H_CirNum);

  __mspu_setKM(KM2, MaskL);
  __mspu_setHiKM(KM2, MaskH);

  // configure the LOOP
  __mspu_setKI(KI12, ARowNum); //80
  if (outW % BS == 0)
    __mspu_setKI(KI13, outW / BS); //5
  else
    __mspu_setKI(KI13, outW / BS + 1); //5

  MLOAD(H_PolyPhase);
  __mspu_callmb(H_PolyPhase); //call
  return 0;
}
