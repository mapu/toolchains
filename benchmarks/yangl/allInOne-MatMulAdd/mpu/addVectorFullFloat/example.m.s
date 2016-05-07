// Filename ： example.m.asm
// Author   :  shaolin.xie@ia.ac.cn
// Description:  This is a example file to demostrate the 
//               program of MPU. just compute the value  in 
//               SHU0.T0 and SHU0.T1, and write back to SHU1.T0
//               

//Declare the function label.
.global addVector_fullFloat

.hmacro SHU0_HM
  SHU0.T0 IND TSQ -> FALU.T0(I0);//a
  SHU0.T1 IND TSQ -> FALU.T1(I0);//b
.endhmacro

.hmacro MR0_HM
  r0.M[0]->FALU.T0(I0) || Repeat @ (8);
  r0.M[0]->FALU.T0(I0) || Repeat @ (KI12);
.endhmacro

.hmacro MR1_HM
  r1.M[0]->FMAC.T0(I1) || Repeat @ (8);
  r1.M[0]->FMAC.T0(I1) || Repeat @ (KI12);
.endhmacro

.hmacro FALU_HM
  T0-T1(S)->M[0] || Repeat @ (8);
  T0-T1(S)->M[0] || Repeat @ (KI12);
.endhmacro

.hmacro FMAC_HM
  MR+=T0*T0(CR,S) || Repeat @ (8);
  MR+=T0*T0(S) || Repeat @ (KI12 - 1);
  MR+=T0*T0(S) -> SHU0.T2;
.endhmacro


.hmacro ALL_HM
 SHU0_HM;
 Repeat @(5);
 FALU_HM;
 Repeat @(5);
 MR0_HM || MR1_HM;
 NOP;
 FMAC_HM;
.endhmacro


addVector_fullFloat:
.opt
ALL_HM;
.flushhm
.stopopt
Repeat @ (20);
MPUSTOP; 

