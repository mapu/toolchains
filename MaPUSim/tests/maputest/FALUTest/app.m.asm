//  Date: 2013-03-28
//  Name: app.m.asm
//  Author: tao.wang@ia.ac.cn
//  The file to test the function of FALU

.text
    // Write to M itself
    .global _FALUTest
   

_start:
    NOP;;
    NOP;;

// X Cluster Test
_FALUTest:
   NOP;;
   NOP;;
   NOP;;

   FALU.T1 (I0)= SHU0.T1 IND T3 ;;  //FALU.T1 = SHU0.T1 = 0x40800000
   FALU.T2 (I0)= SHU0.T2 IND T3 ;;  //FALU.T2 = SHU0.T2 = 0xc0000000
   FALU.T3 (I0)= SHU0.T0 IND T3 ;;  //FALU.T2 = SHU0.T0 = 0x2
   NOP;;

//Basic FALU Test
  IALU.T0 = T1 + T2 (S);;
  IALU.T0 = T1 + T2 (T)(S);;
  IALU.T1 = T1 - T2 (S);;
  IALU.T2 = MAX(T1,T2) (S);;
  IALU.T3 = MIN(T1,T2) (S);;
  NOP;;
  NOP;;
  NOP;;
  NOP;;
  NOP;;

  IMAC.T0 = ABS T2 (S);;
  SHU0.T1 = T1 +- T2 (S);;
  IMAC.T2 = RECIP T1 (S);;
  IMAC.T3 = RSQRT T1 (S);;

// Compare test
  FMAC.T0 = T1 == T2 (S);;
  FMAC.T1 = T1 != T2 (S);;
  FMAC.T2 = T1 > T2  (S);;
  FMAC.T3 = T1 >=T2  (S);;
  IALU.T0 = T1 < T2  (S);;
  IALU.T1 = T1 <=T2  (S);;

// transition test
  IALU.T2 = INT T2 (S);;
  IALU.T3 = UINT T2(S);;
  IALU.T0 = INT T1 (S);;
  IALU.T1 = UINT T1(S);;
  IMAC.T0 = DOUBLE T1(S);;
  IMAC.T1 = DOUBLE T3   ;;
  IMAC.T2 = SINGLE T3   ;;
   

   NOP;;
   NOP;;
   NOP;;
   NOP;;
   NOP;;
   NOP;;
   MPU.Stop;;



