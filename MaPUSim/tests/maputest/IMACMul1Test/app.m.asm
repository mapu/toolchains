//  Date: 2013-03-28
//  Name: app.m.asm
//  Author: tao.wang@ia.ac.cn
//  The file to test the function of IMAC

.text
    // Write to M itself
    .global _IMACTest
    

_start:
    NOP;;
    NOP;;

// X Cluster Test
_IMACTest:
   NOP;;
   NOP;;
   NOP;;

   IMAC.T1 (I0)= SHU0.T1 IND T3 ;;  //IMAC.T1 = SHU0.T1
   IMAC.T2 (I0)= SHU0.T2 IND T3 ;;  //IMAC.T2 = SHU0.T2
   IMAC.T3 (I0)= SHU0.T0 IND T3 ;;  //IMAC.T3 = SHU0.T0   
   NOP;;

// Compound Mul Add-Sub test
   IALU.T1 = T1 +- T2 * T3 ;;
   NOP;;
   Repeat at(10);;
//

   
   MPU.Stop;;



