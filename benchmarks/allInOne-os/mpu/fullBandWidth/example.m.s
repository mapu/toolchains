// Filename ： example.m.asm
// Author   :  shaolin.xie@ia.ac.cn
// Description:  This is a example file to demostrate the 
//               program of MPU. just compute the value  in 
//               SHU0.T0 and SHU0.T1, and write back to SHU1.T0
//               

//Declare the function label.
.global fullBandWidth

.hmacro SHU_INIT_HM
  SHU0.T0 IND TSQ -> IALU.T2(I2);//f, imm
.endhmacro



.hmacro BIU0_HM
Lpto (1f) @ (KI12);
  BIU0.DM(A++,K++) -> IALU.T0(I0);//a
  NOP;
  BIU0.DM(A++,K++) -> IALU.T0(I0);
  NOP;
1:
.endhmacro

.hmacro BIU1_HM
Lpto (1f) @ (KI12);
  BIU1.DM(A++,K++) -> IMAC.T2(I1);//b
  NOP;
  BIU1.DM(A++,K++) -> IMAC.T3(I1);//b
  NOP;
1:
.endhmacro

.hmacro IALU_HM
Lpto (1f) @ (KI12);
  T0 >> 4(B,U) -> IMAC.T0;//T2 is b, pre-set from SHU
  T0 & T2 -> IMAC.T0;
  T0 >> 4(B,U) -> IMAC.T0;
  T0 & T2 -> IMAC.T0;
1:
.endhmacro

.hmacro IMAC_HM
  NOP;
  MR += T0 * T2(B,CR);
  MR += T0 * T2(B) -> BIU2;
  MR += T0 * T3(B);
  MR += T0 * T3(B) -> BIU2 || Lpto (1f) @ (KI12 - 1);
  MR += T0 * T2(B);
  MR += T0 * T2(B) -> BIU2;
  MR += T0 * T3(B);
  MR += T0 * T3(B) -> BIU2;
1:
.endhmacro

.hmacro BIU2_HM
Lpto (1f) @ (KI12);
  BIU2->DM(A++,K++);
  NOP;
  BIU2->DM(A++,K++);
  NOP;
1:
.endhmacro

.hmacro ALL_HM
  SHU_INIT_HM;
  Repeat @ (5);
  BIU0_HM;
  NOP;
  BIU1_HM;
  Repeat @ (8);
  IALU_HM;
  Repeat @ (2);
  IMAC_HM;
  Repeat @ (9);
  BIU2_HM;
.endhmacro



fullBandWidth:
.opt
ALL_HM;
.flushhm
.stopopt
Repeat @ (10);
MPUSTOP; 

