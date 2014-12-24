//  Date: 2013-03-28
//  Name: app.m.asm
//  Author: tao.wang@ia.ac.cn
//  The file to test the function of FMAC

.text
    // Write to M itself
    .global _FMACTest
   

_start:
    NOP;;
    NOP;;

// X Cluster Test
_FMACTest:
   NOP;;
   NOP;;
   NOP;;

   FMAC.T1 (I0)= SHU0.T1 IND T3 ;;  //FMAC.T1 = SHU0.T1
   FMAC.T2 (I0)= SHU0.T2 IND T3 ;;  //FMAC.T2 = SHU0.T2
   FMAC.T3 (I0)= SHU0.T0 IND T3 ;;  //FMAC.T3 = SHU0.T3   
   NOP;;

//SFMAC mul test
   FALU.T0 = T1 * T2 (S);;
   
   MR += T1 * T3 (CR)(S);;
   MR += T1 * T3 (CR)(S);;
   MR += T1 * T3 (CR)(S);;
   MR += T1 * T3 (CR)(S);;
   MR += T1 * T3 (CR)(S);;
   MR += T1 * T3 (CR)(S);;
   MR += T1 * T3 (CR)(S);;
   MR += T1 * T3 (CR)(S);;
   FALU.T0 = MR += T1 * T3 (S);; //40800000
   FALU.T1 = MR += T1 * T3 (S);; //40800000
   FALU.T2 = MR += T1 * T3 (S);; //40800000
   FALU.T3 = MR += T1 * T3 (S);; //40800000
   M0.w3 = MR += T1 * T3 (S);;
   M1.w3 = MR += T1 * T3 (S);;
   M2.w3 = MR += T1 * T3 (S);;
   M3.w3 = MR += T1 * T3 (S);;
   NOP;;
   NOP;;
   NOP;;
   NOP;;

   FALU.T0 = T0 + T1 (S);; // 0+1 41000000
   FALU.T1 = T2 + T3 (S);; // 2+3 41000000
   FALU.T2 (I0)= M0.r0    ;;//40800000
   FALU.T3 (I0)= M1.r0    ;;//40800000
   NOP;;
   NOP;;

   FALU.T0 = T2 + T3 (S);; // 4+5 41000000
   FALU.T1 = T0 + T1 (S);; // 0+1+2+3  41800000
   FALU.T2 (I0)= M2.r0    ;; // 40800000
   FALU.T3 (I0)= M3.r0    ;; // 40800000
   NOP;;
   NOP;;

   FALU.T0 = T2 + T3 (S);; // 6+7  41000000 
   FALU.T1 = T0 + T1 (S);; // 0+1+2+3+4+5 41c00000
   NOP;;
   NOP;;
   NOP;;
   NOP;;
   FALU.T2 = T0 + T1 (S);; // 0+1+...+6+7 42000000


   Repeat at (10);;

   
   MPU.Stop;;



