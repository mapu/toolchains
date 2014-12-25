//  Date: 2013-04-24
//  Name: app.m.asm
//  Author: tao.wang@ia.ac.cn
//  The file to test the function of compound float multiplication
//  KI12=BS

.text
    .global FFTSF64Test

.hmacro BIU0_HM
SHU0.T0 = BIU0.DM (A++)(K++)(BR); Repeat at (8);;
Repeat at (52);;
.endhmacro

.hmacro BIU1_HM
FMAC.T1 (I1)= BIU1.DM (A++)(K++); Repeat at(8);;
Repeat at (12);;
FMAC.T1 (I1)= BIU1.DM (A++)(K++); Repeat at(8);;
Repeat at (12);;
FMAC.T1 (I1)= BIU1.DM (A++)(K++); Repeat at(8);;
Repeat at (12);;
.endhmacro

.hmacro SHU0_HM
M0.w3 = SHU0.T0 IND T1 ;;
M1.w3 = SHU0.T0 IND T1 ;;
M2.w3 = SHU0.T0 IND T1 ;;
M3.w3 = SHU0.T0 IND T1 ;;
M4.w3 = SHU0.T0 IND T1 ;;
M5.w3 = SHU0.T0 IND T1 ;;
M6.w3 = SHU0.T0 IND T1 ;;
M7.w3 = SHU0.T0 IND T1 ;;
Repeat at (52);;
.endhmacro


.hmacro MR0_HM
SHU1.T0 = M[I++].r0 ; Repeat at(8);;
Repeat at (2);;
SHU1.T0 = M[I++].r0 ; Repeat at(8);;
Repeat at (2);;
SHU1.T0 = M[I++].r0 ; Repeat at(8);;
Repeat at (2);;
SHU1.T0 = M[I++].r0 ; Repeat at(8);;
Repeat at (2);;
SHU1.T0 = M[I++].r0 ; Repeat at(8);;
Repeat at (2);;
SHU1.T0 = M[I++].r0 ; Repeat at(8);;
Repeat at (2);;
.endhmacro

.hmacro SHU1_HM
FMAC.T0(I0) = SHU1.T0 IND T1 ; Repeat at(8);;  //extract B
Repeat at (2);;
FALU.T0(I0) = SHU1.T0 IND T2 ; Repeat at(8);;  //extract A
Repeat at (2);;
FMAC.T0(I0) = SHU1.T0 IND T1 ; Repeat at(8);;
Repeat at (2);;
FALU.T0(I0) = SHU1.T0 IND T2 ; Repeat at(8);;
Repeat at (2);;
FMAC.T0(I0) = SHU1.T0 IND T1 ; Repeat at(8);;
Repeat at (2);;
FALU.T0(I0) = SHU1.T0 IND T2 ; Repeat at(8);;
Repeat at (2);;
.endhmacro

.hmacro FMAC_HM
FALU.T1 = T0 ** T1 (S); Repeat at (8);;
Repeat at (12);;
FALU.T1 = T0 ** T1 (S); Repeat at (8);;
Repeat at (12);;
FALU.T1 = T0 ** T1 (S); Repeat at (8);;
Repeat at (12);;
.endhmacro

.hmacro FALU_HM
M0.w3 = T0 +- T1 (S);;
M1.w3 = T0 +- T1 (S);;
M2.w3 = T0 +- T1 (S);;
M3.w3 = T0 +- T1 (S);;
M4.w3 = T0 +- T1 (S);;
M5.w3 = T0 +- T1 (S);;
M6.w3 = T0 +- T1 (S);;
M7.w3 = T0 +- T1 (S);;
Repeat at(12);;
M0.w3 = T0 +- T1 (S);;
M1.w3 = T0 +- T1 (S);;
M2.w3 = T0 +- T1 (S);;
M3.w3 = T0 +- T1 (S);;
M4.w3 = T0 +- T1 (S);;
M5.w3 = T0 +- T1 (S);;
M6.w3 = T0 +- T1 (S);;
M7.w3 = T0 +- T1 (S);;
Repeat at(12);;
BIU2   = T0 +- T1 (S); Repeat at (8);;
Repeat at(12);;
.endhmacro


.hmacro BIU2_HM
Repeat at(52);;
DM = BIU2 (A++)(K++)(IND); Repeat at(8);;
.endhmacro

.hmacro main
BIU0_HM           ;; // cycle 0
Repeat at (6)     ;;
BIU1_HM           ;; // cycle 5
//Repeat at (2)     ;;
SHU0_HM           ;; // cycle 8
Repeat at (2)     ;; 
MR0_HM            ;; // cycle 11
NOP               ;;
SHU1_HM           ;; // cycle 13
NOP               ;; 
FMAC_HM           ;; // cycle 15
Repeat at (2)     ;;
BIU2_HM           ;; // cycle 18
Repeat at (6)     ;;
FALU_HM           ;; // cycle 25
.endhmacro

.opt
main ;;
.flushhm
.stopopt
NOP;;   
MPU.Stop;;



