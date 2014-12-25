//  Date: 2013-03-28
//  Name: app.m.asm
//  Author: tao.wang@ia.ac.cn
//  The file to test the function of IMAC

.text
    // Write to M itself
    .global IMACTest
    

_start:
    NOP; 
    NOP; 

// X Cluster Test
IMACTest:
   NOP; 
   NOP; 
   NOP; 

 SHU0.T1 IND T3 ->   IMAC.T1 (I0);   //IMAC.T1 = SHU0.T1
 SHU0.T2 IND T3 ->   IMAC.T2 (I0);   //IMAC.T2 = SHU0.T2
 SHU0.T0 IND T3 ->   IMAC.T3 (I0);   //IMAC.T2 = SHU0.T2   
   NOP; 

//Integer test
 T1 * T2 (T,I)->   IALU.T0 ; 
 T3 * T3  (I,H)  ->   IALU.T2 ; 
   //IALU.T2 = T1 * T3 (I); 

// Decimal test
   NOP; 
 T1 * T2 (T)->   FALU.T0 ; 
 T3 * T3  (H)  ->   FALU.T2 ; 
   NOP; 
 T1 * T2 (H)->   FALU.T0 ; 
   NOP; 
   Repeat @(10); 

//MR test
   

   
   MPUSTOP; 



