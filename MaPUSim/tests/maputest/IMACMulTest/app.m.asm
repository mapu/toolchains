//  Date: 2013-03-28
//  Name: app.m.asm
//  Author: tao.wang@ia.ac.cn
//  The file to test the function of IMAC

.text
    // Write to M itself
    .global IMACTest
    

_start:
    NOP;;
    NOP;;

// X Cluster Test
IMACTest:
   NOP;;
   NOP;;
   NOP;;

   IMAC.T1 (I0)= SHU0.T1 IND T3 ;;  //IMAC.T1 = SHU0.T1
   IMAC.T2 (I0)= SHU0.T2 IND T3 ;;  //IMAC.T2 = SHU0.T2
   IMAC.T3 (I0)= SHU0.T0 IND T3 ;;  //IMAC.T3 = SHU0.T3   
   NOP;;

//Integer test
   IALU.T0 = T1 * T2 (T)(I);;
   IALU.T2:3 = T3 * T3  (I)(H)  ;;
   //IALU.T2:3 = T1 * T3 (I);;

// Decimal test
   NOP;;
   FALU.T0 = T1 * T2 (T);;
   FALU.T2:3 = T3 * T3  (H)  ;;
   NOP;;
   FALU.T0:1 = T1 * T2 (H);;
   NOP;;
   Repeat at(10);;

//MR test
   

   
   MPU.Stop;;



