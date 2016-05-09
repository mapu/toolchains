#include <mspu-intrin.h>
#include <mlib_mgr.h>

const int SDA0DM0_START = 0x400000 ;
const int SDA1DM0_START = 0x800000 ;
const int SDA2DM0_START = 0xc00000 ;
const int SDASize       = 0x40000    ;
const int BlockSize     = 0x40       ;
const int BlockGran     = 0x6        ;
const int TLBLen        = 256        ;
const int TLBWidth      = 64         ;

extern void gammar()             ;



int main() {
  //parameter definition, it is related to the input length and data type
  const int BLen          = 128*128   ; // the TLB's input length
  const int BS            = 64        ;
  const int DataSize      = 1         ;
  const int DataSizeGran  = 0         ;
  const int BankNum       = BS        ;
  
  //the input length computation
  const int BBSTailNum    = BLen % BS ;
  const int BBSTailEn     = BBSTailNum != 0 ;
  const int BBSNum        = BLen / BS + BBSTailEn ;
  
  // the circle number for BIU0/1/2
  const int ACirNum       = 1 ;
  const int BCirNum       = 1 ;
  const int CCirNum       = 1 ;

  // for MC Configuration
  const int MIndexStart   = 0x0 ;
  const int MStepMode     = 0x1 ;
  const int MLen          = 0x4 ;
  const int ISize         = 0x4 ;

  const int MIndexOffset  = 0 ;
  const int MStepOffset   = 8 ;
  const int MLenOffset    = 24;
  const int ISizeOffset   = 12;

  const int MCIndexStart  = MIndexStart << MIndexOffset ;
  const int MCStepMode    = MStepMode   << MStepOffset  ;
  const int MCLen         = MLen        << MLenOffset   ;
  const int ICSize        = ISize       << ISizeOffset  ;
  const int MConfig       = MCIndexStart | MCStepMode | MCLen | ICSize ;

  // configure MC
  const int iMC = MConfig ; 
  __mspu_setMC(MC_r0, iMC);

  // configure LOOP
  const int iKI12 = BBSNum ;
  __mspu_setKI(KI12,iKI12) ;

  // configure BIU0
  const int iKB0 = SDA0DM0_START ;
  const int iKS0 = BlockSize     ;
  const int iKI0 = TLBLen / TLBWidth   ; 
  const int iKG0 = BlockGran     ;
  const int iKL0 = ACirNum       ;
  
  __mspu_setKB(KB0,iKB0) ;
  __mspu_setKS(KS0,iKS0) ;
  __mspu_setKI(KI0,iKI0) ;
  __mspu_setKG(KG0,iKG0) ;
  __mspu_setKL(KL0,iKL0) ;

  // configure BIU1
  const int iKB4 = SDA1DM0_START ;
  const int iKS4 = BlockSize     ;
  const int iKI4 = BBSNum        ;
  const int iKG1 = BlockGran     ;
  const int iKL1 = BCirNum       ;
  __mspu_setKB(KB4,iKB4) ;
  __mspu_setKS(KS4,iKS4) ;
  __mspu_setKI(KI4,iKI4) ;
  __mspu_setKG(KG1,iKG1) ;
  __mspu_setKL(KL1,iKL1) ;

  // configure BIU2
  const int iKB8 = SDA2DM0_START ;
  const int iKS8 = BlockSize     ;
  const int iKI8 = BBSNum        ;
  const int iKG2 = BlockGran     ;
  const int iKL2 = CCirNum       ;
  __mspu_setKB(KB8,iKB8) ;
  __mspu_setKS(KS8,iKS8) ;
  __mspu_setKI(KI8,iKI8) ;
  __mspu_setKG(KG2,iKG2) ;
  __mspu_setKL(KL2,iKL2) ;
  // callM
  MLOAD(gammar);
  __mspu_callm(gammar);

  return 0 ;
}
