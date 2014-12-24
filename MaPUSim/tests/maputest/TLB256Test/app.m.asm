//  Date: 2013-05-24
//  Name: app.s.asm
//  Author: tao.wang@ia.ac.cn
//  The configuration Unit for MPU for Bicubic Interpolation 

   .global    GammarTest

// BIU0 : Ld the Table to M
// BIU1 : Ld the index to IALU.T
// M.r0 : output the Table to SHU0.T0
// M.r1 : output the intermediate table result to IALU.T0/T1/T3
// SHU  : search for the table
//   T0 : the table
//   T1 : the index
// IALU : computer the index, select the table result
//   T0 : the table number index
//   T1 : the current table result
//   T2 : the stored table result
//   T3 : the updated table number index

.hmacro BIU0_HM
NOP;;
M[I++].w0 = BIU0.DM (A++)(K++); Repeat at (4);;
.endhmacro

.hmacro BIU1_HM
LPTO 1f AT (KI12);;
M4.w1      = BIU1.DM (A++)(K++);;
Repeat at(7);;
1:
.endhmacro


.hmacro MR0_HM
LPTO 1f AT (KI12);;
SHU0.T0 = M[I++].r0 ; Repeat at(4);;
Repeat at (4);;
1:
.endhmacro

.hmacro MR1_TOP_HM
LPTO 1f AT (KI12);;
SHU0.T1      = M4.r1 ;;
Repeat at (7)        ;;
1:
.endhmacro

.hmacro MR1_BOT_HM
LPTO 1f AT (KI12);;
Repeat at (5)        ;;
IALU.T3 (I0) = M7.r1 ;;
IALU.T1 (I0) = M5.r1 ;;
NOP                  ;;
1:
.endhmacro

.hmacro MR1_HM
MR1_TOP_HM  ;;
Repeat at(7);;
MR1_BOT_HM  ;;
.endhmacro

.hmacro MR2_TOP_HM
LPTO 1f AT (KI12);;
IALU.T0 (I0) = M4.r2 ;;
Repeat at (7)        ;;
1:
.endhmacro

.hmacro MR2_BOT_HM
LPTO 1f AT (KI12);;
Repeat at (6)        ;;
IALU.T2 (I2) = M6.r2 ;;
NOP                  ;;
1:
.endhmacro

.hmacro MR2_HM
MR2_TOP_HM  ;;
Repeat at(7);;
MR2_BOT_HM  ;;
.endhmacro




.hmacro SHU0_HM
LPTO 1f AT (KI12);;
IALU.T1 (I1) = SHU0.T0 IND T1 ;;
IALU.T2 (I1) = SHU0.T0 IND T1 ;;
IALU.T1 (I1) = SHU0.T0 IND T1 ;;
IALU.T2 (I1) = SHU0.T0 IND T1 ;;
Repeat at (4);;
1:
.endhmacro



.hmacro IALU_TOP_HM
LPTO 1f AT (KI12);;
IALU.T0 = T0 >> 6 (U);;
NOP;;
NOP;;
M5.w3   = T0 ? T2 : T1 (B);;
M7.w3   = T0 >> 1 (U);;
M6.w3   = T0 ? T2 : T1 (B);;
Repeat at(2) ;;
1:
.endhmacro

.hmacro IALU_BOT_HM
LPTO 1f AT (KI12);;
Repeat at (6)          ;;
BIU2 = T3 ? T2 : T1 (B);;
NOP ;;
1:
.endhmacro

.hmacro IALU_HM
IALU_TOP_HM   ;;
Repeat at(7)  ;;
IALU_BOT_HM   ;;
.endhmacro

.hmacro BIU2_HM
LPTO 1f AT (KI12);;
DM = BIU2 (A++)(K++) ;;
Repeat at (7);;
1:
.endhmacro

.hmacro main
BIU0_HM ; BIU1_HM       ;; //0
Repeat at(8)            ;;
MR0_HM ; MR1_HM;MR2_HM  ;; //9
NOP                     ;; 
SHU0_HM ;IALU_HM        ;; //11
Repeat at(16)           ;; 
BIU2_HM                 ;; //28
.endhmacro


GammarTest:
.opt
main   ;;
.flushhm
.stopopt
Repeat at(7);;
NOP         ;;
MPU.Stop    ;;

