//  Date: 2013-11-16
//  Name: app.m.asm
//  Author: tao.wang@ia.ac.cn
//  The file to test the function of IALU 

.text
    // Write to M itself
    .global IALU_Add_W_Test  
  
_start:
    NOP;;
    NOP;;

// X Cluster Test

   NOP;;
   NOP;;
   NOP;;

IALU_Add_W_Test: 
   IALU.T1(I0) = BIU0.DM (A++)(K++); IALU.T2(I1) = BIU1.DM (A++)(K++); Repeat at (8);;
   IALU.T1(I0) = BIU0.DM (A++)(K++); IALU.T2(I1) = BIU1.DM (A++)(K++); BIU2 = T1 + T2 ; Repeat at (3);;
   IALU.T1(I0) = BIU0.DM (A++)(K++); IALU.T2(I1) = BIU1.DM (A++)(K++); BIU2 = T1 + T2 ; DM = BIU2 (A++)(K++);Repeat at (KI12-11);;
   BIU2 = T1 + T2 ; DM = BIU2 (A++)(K++);Repeat at (8);;
   DM = BIU2 (A++)(K++); Repeat at(3);;
   Repeat at(8);;
   NOP;;
   NOP;;
   MPU.Stop;;

   
   



