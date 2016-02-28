// Filename ： example.m.asm
// Author   :  shaolin.xie@ia.ac.cn
// Description:  This is a example file to demostrate the 
//               program of MPU. just compute the value  in 
//               SHU0.T0 and SHU0.T1, and write back to SHU1.T0
//               

//Declare the function label.
.global addVectorInt

.hmacro BIU0_HM
  BIU0.DM(A++,K++) -> SHU0.T0 || Repeat @ (KI13);
.endhmacro

.hmacro BIU1_HM
  BIU1.DM(A++,K++) -> SHU1.T0 || Repeat @ (KI13);
.endhmacro

.hmacro SHU0_HM
  SHU0.T0 IND TSQ -> IALU.T0 (I0) || Repeat @ (KI13);
.endhmacro

.hmacro SHU1_HM
  SHU1.T0 IND TSQ -> IALU.T1 (I1) || Repeat @ (KI13);
.endhmacro

.hmacro IALU_HM
  T0 + T1 -> BIU2 || Repeat @ (KI13);
.endhmacro

.hmacro BIU2_HM
  BIU2->DM(A++,K++) || Repeat @ (KI13);
.endhmacro

.hmacro ALL_HM
  BIU0_HM || BIU1_HM;
  Repeat @ (10);
  SHU0_HM || SHU1_HM;
  Repeat @ (2);
  IALU_HM;
  Repeat @ (3);
  BIU2_HM;
.endhmacro



addVectorInt:
.opt
ALL_HM;
.flushhm
.stopopt
Repeat @ (10);
MPUSTOP; 

