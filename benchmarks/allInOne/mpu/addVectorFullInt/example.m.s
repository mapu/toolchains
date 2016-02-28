// Filename ： example.m.asm
// Author   :  shaolin.xie@ia.ac.cn
// Description:  This is a example file to demostrate the 
//               program of MPU. just compute the value  in 
//               SHU0.T0 and SHU0.T1, and write back to SHU1.T0
//               

//Declare the function label.
.global addVectorFullInt

.hmacro INIT_SHU0_HM
  SHU0.T0 IND TSQ -> IMAC.T1(I0);//-1
  SHU0.T1 IND TSQ -> IMAC.T2(I0);//1
.endhmacro

.hmacro SHU0_HM
  LPTO (1f) @(KI12);
    SHU0.TSQ IND TBB(TB=+1) ->IALU.T0(I0);
    SHU0.TSQ IND TB(TB+=1) ->IALU.T0(I0) || Repeat@(63);
  1:
.endhmacro

.hmacro SHU1_HM
  LPTO (1f) @(KI12);
    SHU1.TBW IND TBB(TB=+1) ->IALU.T1(I1);
    SHU1.TBW IND TB(TB+=1) ->IALU.T1(I1) || Repeat@(63);
  1:
.endhmacro

.hmacro IALU_HM
  LPTO (1f) @(KI12);
    T0 + T1(B) -> IMAC.T0  || Repeat@(64);
  1:
.endhmacro

.hmacro IMAC_HM
  NOP;
  MR += T0 * T1(CR,B);
  MR += T0 * T2(B);
  .rept 30
    MR += T0 * T1(B);
    MR += T0 * T2(B);
  .endr
  MR += T0 * T1(B);
  MR += T0 * T2(B) || LPTO (1f) @(KI12 - 2);
  .rept 32
    MR += T0 * T1(B);
    MR += T0 * T2(B);
  .endr
  1:

  .rept 31
    MR += T0 * T1(B);
    MR += T0 * T2(B);
  .endr
  MR += T0 * T1(B);
  MR += T0 * T2(B,L) -> SHU0.T2;
.endhmacro


.hmacro ALL_HM
 INIT_SHU0_HM;
 Repeat @(3);
 SHU0_HM || SHU1_HM;
 Repeat @(2);
 IALU_HM;
 Repeat @(2);
 IMAC_HM;
.endhmacro


addVectorFullInt:
.opt
ALL_HM;
.flushhm
.stopopt
Repeat @ (20);
MPUSTOP; 

