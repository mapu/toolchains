//  Date: 2013-04-24
//  Name: app.m.asm
//  Author: tao.wang@ia.ac.cn
//  The file to test the function of compound float multiplication
//  KI12=BS

.text
    .global FFTSF64Ep0Test   
    .global FFTSF64Ep1Test 

.macro BIU_HM name  flag=""
.hmacro BIU0_HM_\name
SHU0.T0 = BIU0.DM (A++)(K++)\flag; Repeat at (8);;
Repeat at (52);;
.endhmacro
.endmacro

BIU_HM biu0br  "(BR)"
BIU_HM biu0     

.hmacro BIU1_HM
FMAC.T1 (I1)= BIU1.DM (A++)(K++); Repeat at(8);;
Repeat at (12);;
FMAC.T1 (I1)= BIU1.DM (A++)(K++); Repeat at(8);;
Repeat at (12);;
FMAC.T1 (I1)= BIU1.DM (A++)(K++); Repeat at(8);;
Repeat at (12);;
.endhmacro

.macro SHUO_HM name src
.hmacro SHU0_HM_\name 
M0.w3 = SHU0.T0 IND \src ;;
M1.w3 = SHU0.T0 IND \src ;;
M2.w3 = SHU0.T0 IND \src ;;
M3.w3 = SHU0.T0 IND \src ;;
M4.w3 = SHU0.T0 IND \src ;;
M5.w3 = SHU0.T0 IND \src ;;
M6.w3 = SHU0.T0 IND \src ;;
M7.w3 = SHU0.T0 IND \src ;;
Repeat at (52)           ;;
.endhmacro
.endmacro

SHUO_HM shu0br T1
SHUO_HM shu0   T2



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
FMAC.T0(I0) = SHU1.T0 IND T1 ; Repeat at(8);;
Repeat at (2);;
FALU.T0(I0) = SHU1.T0 IND T2 ; Repeat at(8);;
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


.hmacro main_br
BIU0_HM_biu0br    ;; // cycle 0
Repeat at (6)     ;;
BIU1_HM           ;; // cycle 7
SHU0_HM_shu0br    ;; // cycle 8
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



.hmacro main_
BIU0_HM_biu0      ;; // cycle 0
Repeat at (6)     ;;
BIU1_HM           ;; // cycle 7
SHU0_HM_shu0      ;; // cycle 8
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



FFTSF64Ep0Test :
.opt
main_br ;;
.flushhm
.stopopt
NOP;;   
MPU.Stop;;
NOP;;

FFTSF64Ep1Test :
.opt
main_ ;;
.flushhm
.stopopt
NOP;;   
MPU.Stop;;



