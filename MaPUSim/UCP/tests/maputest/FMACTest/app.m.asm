//  Date: 2013-03-28
//  Name: app.m.asm
//  Author: tao.wang@ia.ac.cn
//  The file to test the function of FMAC

.text
    // Write to M itself
    .global _FMACTest
   

_start:
    NOP; 
    NOP; 

// X Cluster Test
_FMACTest:
   NOP; 
   NOP; 
   NOP; 

 SHU0.T1 IND T3 ->   FMAC.T1 (I0);   //FMAC.T1 = SHU0.T1
 SHU0.T2 IND T3 ->   FMAC.T2 (I0);   //FMAC.T2 = SHU0.T2
 SHU0.T0 IND T3 ->   FMAC.T3 (I0);   //FMAC.T2 = SHU0.T2   
   NOP; 

//SFMAC mul test
 T1 * T2 (S)->   FALU.T0 ; 
   
   MR+= T1 * T3 (CR,S); 
   MR+= T1 * T3 (CR,S); 
   MR+= T1 * T3 (CR,S); 
   MR+= T1 * T3 (CR,S); 
   MR+= T1 * T3 (CR,S); 
   MR+= T1 * T3 (CR,S); 
   MR+= T1 * T3 (CR,S); 
   MR+= T1 * T3 (CR,S); 
 MR+= T1 * T3 (S)->   FALU.T0 ; 
 MR+= T1 * T3 (S)->   FALU.T1 ; 
 MR+= T1 * T3 (S)->   FALU.T2 ; 
 MR+= T1 * T3 (S)->   FALU.T3 ; 
 MR+= T1 * T3 (S)->   M[0] ; 
 MR+= T1 * T3 (S)->   M[1] ; 
 MR+= T1 * T3 (S)->   M[2] ; 
 MR+= T1 * T3 (S)->   M[3] ; 
   NOP; 
   NOP; 
   NOP; 
   NOP; 

 T0 + T1 (S)->   FALU.T0 ;  // 0+1
 T2 + T3 (S)->   FALU.T1 ;  // 2+3
 r0.M[0]    ->   FALU.T2 (I0); 
 r0.M[1]    ->   FALU.T3 (I0); 
   NOP; 
   NOP; 

 T2 + T3 (S)->   FALU.T0 ;  // 4+5
 T0 + T1 (S)->   FALU.T1 ;  // 0+1+2+3
 r0.M[2]    ->   FALU.T2 (I0); 
 r0.M[3]    ->   FALU.T3 (I0); 
   NOP; 
   NOP; 

 T2 + T3 (S)->   FALU.T0 ;  // 6+7
 T0 + T1 (S)->   FALU.T1 ;  // 0+1+2+3+4+5
   NOP; 
   NOP; 
   NOP; 
   NOP; 
 T0 + T1 (S)->   FALU.T2 ;  // 0+1+...+6+7


   Repeat @ (10); 

   
   MPUSTOP; 



