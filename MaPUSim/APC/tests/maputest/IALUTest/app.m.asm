//  Date: 2013-03-25
//  Name: app.m.asm
//  Author: tao.wang@ia.ac.cn
//  The file to test the function of IALU 

.text
    // Write to M itself
    .global _IALUTest
    

_start:
    NOP; 
    NOP; 

// X Cluster Test
_IALUTest:
   NOP; 
   NOP; 
   NOP; 

 EXPD T1 -> IALU.T2;


 SHU0.T1 IND T3 ->   IALU.T1 (I0);   //IALU.T1 = SHU0.T1
 SHU0.T2 IND T3 ->   IALU.T2 (I0);   //IALU.T2 = SHU0.T2
 SHU0.T0 IND T3 ->   IALU.T3 (I0);   //IALU.T3 = SHU0.T0
   NOP; 
   NOP;
   NOP;
   NOP;
   NOP;
   NOP;
 EXPD T1 (U,B) -> IALU.T2;
 NOP;
 NOP;
 NOP;
 NOP;
 NOP;

//Basic IALU
 T1 + T2 ->   IMAC.T0 ; 
 T1 - T2 ->   IMAC.T1 ; 
 MAX(T1,T2)(U)->   IMAC.T2 ; 
 MIN(T1,T2)(U,H)->   IMAC.T3 ; 
 ABS(T1,T2)->   IMAC.T0 ; 
 MERGE(T2,T1,T0) ->   IMAC.T1 ; 
 MERGE(T2,T1,T1) ->   IMAC.T2 ; 
   

//logic
 T1 & T2 ->   FALU.T0 ; 
 T1 | T2 ->   FALU.T1 ; 
 T1 ^ T2 ->   FALU.T2 ; 
 NOT T2     ->   FALU.T3 ; 

//shift
 T2 >> T3->   IMAC.T0 ; 
 T2 << T3->   IMAC.T1 ; 
 T2 >> 2 ->   IMAC.T2 ; 
 T2 << 2 ->   IMAC.T3 ; 

//compare
 T1 > T2 ->   FALU.T0 ; 
 T1 > T2 (B)->   FALU.T1 ; 
 T1 > T2 (U)->   FALU.T2 ; 
 T1 > T2 (U,B)->   FALU.T3 ; 

//divide

   NOP; 
   NOP; 
   NOP; 

   
   MPUSTOP; 



