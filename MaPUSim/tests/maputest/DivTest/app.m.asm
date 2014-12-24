//  Date: 2013-03-25
//  Name: app.m.asm
//  Author: tao.wang@ia.ac.cn
//  The file to test the function of IALU 

.text
    // Write to M itself
    .global IALUTest
   

_start:
    NOP;;
    NOP;;

// X Cluster Test
_XY_IALUTest:
   NOP;;
   NOP;;
   NOP;;

   IALU.T1 (I0)= SHU0.T1 IND T3 ;;  //IALU.T1 = SHU0.T1
   IALU.T2 (I0)= SHU0.T2 IND T3 ;;  //IALU.T2 = SHU0.T2
   NOP;;
   NOP;;

//divide test
   DIVR = T1 DIVS T2 ;;
   DIVR = DIVQ;;
   DIVR = DIVQ;;
   DIVR = DIVQ;;
   DIVR = DIVQ;;
   DIVR = DIVQ;;
   DIVR = DIVQ;;
   DIVR = DIVQ;;
   DIVR = DIVQ;;
   DIVR = DIVQ;;
   DIVR = DIVQ;;
   DIVR = DIVQ;;
   DIVR = DIVQ;;
   DIVR = DIVQ;;
   DIVR = DIVQ;;
   DIVR = DIVQ;;
   DIVR = DIVQ;;
   DIVR = MDIVR;;
   DIVR = MDIVQ;;
   IMAC.T2:3 = DIVR ;;
   

   NOP;;
   NOP;;
   NOP;;
   

   
   MPU.Stop;;



