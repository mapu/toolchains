//  Date: 2013-03-28
//  Name: app.m.asm
//  Author: tao.wang@ia.ac.cn
//  The file to test the function of IMAC

.text
    // Write to M itself
    .global _IMACTest
    

_start:
    NOP;
    NOP;

// X Cluster Test
_IMACTest:
   NOP;
   NOP;
   NOP;

   SHU0.T1 IND T3 -> IMAC.T1 (I0);  //IMAC.T1 = SHU0.T1
   SHU0.T2 IND T3 -> IMAC.T2 (I0);  //IMAC.T2 = SHU0.T2
   SHU0.T0 IND T3 -> IMAC.T3 (I0);  //IMAC.T3 = SHU0.T0   
   NOP;

// Compound Mul Add-Sub test
   T1 +- T2 * T3 (H)-> IALU.T1;
   NOP;
   Repeat @(10);
//

   
   MPUStop;



