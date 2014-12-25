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

//MR test, CR
 MR+= T1 * T2 (CR,H)->   IALU.T0 ; 
 MR+= T1 * T2 (CR,H) ->   IALU.T1 ; 
   MR+= T1 * T2 (CR,H) ; 
   MR+= T1 * T2 (CR,H) ; 
   MR+= T1 * T2 (CR,H) ; 
   MR+= T1 * T2 (CR,H) ; 
 MR+= T1 * T2 (H) ->   IALU.T2 ; 
 MR+= T1 * T2 (H) ->   IALU.T3 ; 
 MR+= T1 * T2 (H)->   IALU.T3 ; 
   NOP; 
   
//MAC    
 MR+= T1 * T2 (CR,H)->   FALU.T0 ; 
 MR+= T1 * T2     (H)->   FALU.T1 ; 
 MR+= T1 * T2     (H)->   FALU.T2 ; 
 MR+= T1 * T2(L,H)->   FALU.T0 ; 
   NOP; 
   NOP; 
   NOP; 
   NOP; 
   NOP; 
   NOP; 
   NOP; 
   NOP; 
   NOP; 

//ACC   
 MR+= T1    (H)->   IALU.T0 ; 
 MR+= T1    (H)->   IALU.T1 ; 
 MR+= T1    (H)->   IALU.T2 ; 
   //IALU.T0 = MR += T0 (L,H); 

   NOP; 
   NOP; 
   Repeat @(10); 
   

   
   MPUSTOP; 



