//  Date: 2014-09-12
//  Name: app.m.asm
//  Author: Yanglei
//  The file to test the function of IALU , modification for adding pipeline

.text
    // Write to M itself
    .global IALU_Add_W_Test  
    .global IALU_Add_WU_Test  
    .global IALU_Sub_W_Test  
    .global IALU_Sub_WU_Test   

_start:
    NOP; 
    NOP; 

// X Cluster Test

   NOP; 
   NOP; 
   NOP; 

IALU_Add_W_Test: 
 BIU0.DM (A++,K++)->   IALU.T1(I0) || BIU1.DM (A++,K++)-> IALU.T2(I1) || Repeat @ (11); 
 BIU0.DM (A++,K++)->   IALU.T1(I0) || BIU1.DM (A++,K++)-> IALU.T2(I1) || T1 + T2 -> BIU2 || Repeat @ (4); 
 BIU0.DM (A++,K++)->   IALU.T1(I0) || BIU1.DM (A++,K++)-> IALU.T2(I1) || T1 + T2 -> BIU2 ||BIU2->DM(A++,K++)||Repeat @ (17); 
 T1 + T2 ->   BIU2 ||BIU2->DM(A++,K++)||Repeat @ (11); 
BIU2->DM(A++,K++)|| Repeat @(4); 
   Repeat @(9); 
   NOP; 
   NOP; 
   MPUSTOP; 
IALU_Add_WU_Test:
 BIU0.DM (A++,K++)->   IALU.T1(I0) || BIU1.DM (A++,K++)-> IALU.T2(I1) || Repeat @ (11); 
 BIU0.DM (A++,K++)->   IALU.T1(I0) || BIU1.DM (A++,K++)-> IALU.T2(I1) || T1 + T2 (U) -> BIU2 || Repeat @ (4); 
 BIU0.DM (A++,K++)->   IALU.T1(I0) || BIU1.DM (A++,K++)-> IALU.T2(I1) || T1 + T2 (U)-> BIU2 ||BIU2->DM(A++,K++)||Repeat @ (17); 
 T1 + T2 (U)->   BIU2 ||BIU2->DM(A++,K++)||Repeat @ (11); 
BIU2->DM(A++,K++)|| Repeat @(4); 
   Repeat @(9); 
   NOP; 
   NOP; 
   MPUSTOP; 
IALU_Sub_W_Test: 
 BIU0.DM (A++,K++)->   IALU.T1(I0) || BIU1.DM (A++,K++)-> IALU.T2(I1) || Repeat @ (11); 
 BIU0.DM (A++,K++)->   IALU.T1(I0) || BIU1.DM (A++,K++)-> IALU.T2(I1) || T1 - T2 -> BIU2 || Repeat @ (4); 
 BIU0.DM (A++,K++)->   IALU.T1(I0) || BIU1.DM (A++,K++)-> IALU.T2(I1) || T1 - T2 -> BIU2 ||BIU2->DM(A++,K++)||Repeat @ (17); 
 T1 - T2 ->   BIU2 ||BIU2->DM(A++,K++)||Repeat @ (11); 
BIU2->DM(A++,K++)|| Repeat @(4); 
   Repeat @(9); 
   NOP; 
   NOP; 
   MPUSTOP; 
IALU_Sub_WU_Test:
 BIU0.DM (A++,K++)->   IALU.T1(I0) || BIU1.DM (A++,K++)-> IALU.T2(I1) || Repeat @ (11); 
 BIU0.DM (A++,K++)->   IALU.T1(I0) || BIU1.DM (A++,K++)-> IALU.T2(I1) || T1 - T2 (U) -> BIU2 || Repeat @ (4); 
 BIU0.DM (A++,K++)->   IALU.T1(I0) || BIU1.DM (A++,K++)-> IALU.T2(I1) || T1 - T2 (U)-> BIU2 ||BIU2->DM(A++,K++)||Repeat @ (17); 
 T1 - T2 (U)->   BIU2 ||BIU2->DM(A++,K++)||Repeat @ (11); 
BIU2->DM(A++,K++)|| Repeat @(4); 
   Repeat @(9); 
   NOP; 
   NOP; 
   MPUSTOP; 
   
   
   



