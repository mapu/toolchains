//  Date: 2013-04-24
//  Name: app.m.asm
//  Author: tao.wang@ia.ac.cn
//  The file to test the function of compound float multiplication
//  KI12=GroupNum/PipeNum

.text
    .global FFTSF4096Ep0Test 
    .global FFTSF4096Ep1Test 
    .global FFTSF4096Ep2Test 
    .global FFTSF4096Ep3Test 

    GroupNum   = 4096 / 8
    PipeGroup  = 20
    N          = GroupNum % PipeGroup


.macro BIU_HM name flag=""
.hmacro BIU0_HM_\name
LPTO 1f AT (KI12);;
M[I++].w0 = BIU0.DM (A++)(K++)\flag; Repeat at (20);;
Repeat at (40);;
1:
M[I++].w0 = BIU0.DM (A++)(K++)\flag; Repeat at (N);;
Repeat at (60-N);;
.endhmacro
.endmacro

BIU_HM biu0br  "(BR)"
BIU_HM biu0    "" 

.hmacro BIU1_HM
LPTO 1f AT (KI12);;
//FMAC.T1 (I1)= BIU1.DM (A++)(K++); Repeat at(60);;
M20.w1 = BIU1.DM (A++)(K++);;
M21.w1 = BIU1.DM (A++)(K++);;
M22.w1 = BIU1.DM (A++)(K++);;
M23.w1 = BIU1.DM (A++)(K++);;
M24.w1 = BIU1.DM (A++)(K++);;
M25.w1 = BIU1.DM (A++)(K++);;
M26.w1 = BIU1.DM (A++)(K++);;
M27.w1 = BIU1.DM (A++)(K++);;
M28.w1 = BIU1.DM (A++)(K++);;
M29.w1 = BIU1.DM (A++)(K++);;
M30.w1 = BIU1.DM (A++)(K++);;
M31.w1 = BIU1.DM (A++)(K++);;
M32.w1 = BIU1.DM (A++)(K++);;
M33.w1 = BIU1.DM (A++)(K++);;
M34.w1 = BIU1.DM (A++)(K++);;
M35.w1 = BIU1.DM (A++)(K++);;
M36.w1 = BIU1.DM (A++)(K++);;
M37.w1 = BIU1.DM (A++)(K++);;
M38.w1 = BIU1.DM (A++)(K++);;
M39.w1 = BIU1.DM (A++)(K++);;
Repeat at(40);;
1:

M20.w1 = BIU1.DM (A++)(K++);;
M21.w1 = BIU1.DM (A++)(K++);;
M22.w1 = BIU1.DM (A++)(K++);;
M23.w1 = BIU1.DM (A++)(K++);;
M24.w1 = BIU1.DM (A++)(K++);;
M25.w1 = BIU1.DM (A++)(K++);;
M26.w1 = BIU1.DM (A++)(K++);;
M27.w1 = BIU1.DM (A++)(K++);;
M28.w1 = BIU1.DM (A++)(K++);;
M29.w1 = BIU1.DM (A++)(K++);;
M30.w1 = BIU1.DM (A++)(K++);;
M31.w1 = BIU1.DM (A++)(K++);;
/*
M32.w1 = BIU1.DM (A++)(K++);;
M33.w1 = BIU1.DM (A++)(K++);;
M34.w1 = BIU1.DM (A++)(K++);;
M35.w1 = BIU1.DM (A++)(K++);;
M36.w1 = BIU1.DM (A++)(K++);;
M37.w1 = BIU1.DM (A++)(K++);;
M38.w1 = BIU1.DM (A++)(K++);;
M39.w1 = BIU1.DM (A++)(K++);;
*/
Repeat at (60-N);;
.endhmacro

.hmacro MR0_HM
LPTO 1f AT (KI12);;
SHU0.T0 = M[I++].r0 ; Repeat at (60);;
1:
SHU0.T0 = M[I++].r0 ; Repeat at (60);;
//Repeat at (20-N);;
//SHU0.T0 = M[I++].r0 ; Repeat at (N);;
//Repeat at (20-N);;
//SHU0.T0 = M[I++].r0 ; Repeat at (N);;
//Repeat at (20-N);;
.endhmacro

.hmacro MR1_HM
LPTO 1f AT (KI12);;
.rept 3
SHU1.T0 = M0.r1 ;;
SHU1.T0 = M1.r1 ;;
SHU1.T0 = M2.r1 ;;
SHU1.T0 = M3.r1 ;;
SHU1.T0 = M4.r1 ;;
SHU1.T0 = M5.r1 ;;
SHU1.T0 = M6.r1 ;;
SHU1.T0 = M7.r1 ;;
SHU1.T0 = M8.r1 ;;
SHU1.T0 = M9.r1 ;;
SHU1.T0 = M10.r1 ;;
SHU1.T0 = M11.r1 ;;
SHU1.T0 = M12.r1 ;;
SHU1.T0 = M13.r1 ;;
SHU1.T0 = M14.r1 ;;
SHU1.T0 = M15.r1 ;;
SHU1.T0 = M16.r1 ;;
SHU1.T0 = M17.r1 ;;
SHU1.T0 = M18.r1 ;;
SHU1.T0 = M19.r1 ;;
.endr
1:
.rept 3
SHU1.T0 = M0.r1 ;;
SHU1.T0 = M1.r1 ;;
SHU1.T0 = M2.r1 ;;
SHU1.T0 = M3.r1 ;;
SHU1.T0 = M4.r1 ;;
SHU1.T0 = M5.r1 ;;
SHU1.T0 = M6.r1 ;;
SHU1.T0 = M7.r1 ;;
SHU1.T0 = M8.r1 ;;
SHU1.T0 = M9.r1 ;;
SHU1.T0 = M10.r1 ;;
SHU1.T0 = M11.r1 ;;
SHU1.T0 = M12.r1 ;;
SHU1.T0 = M13.r1 ;;
SHU1.T0 = M14.r1 ;;
SHU1.T0 = M15.r1 ;;
SHU1.T0 = M16.r1 ;;
SHU1.T0 = M17.r1 ;;
SHU1.T0 = M18.r1 ;;
SHU1.T0 = M19.r1 ;;
.endr
.endhmacro

.hmacro MR2_HM
LPTO 1f AT (KI12);;
//Stage1
FMAC.T1 (I1) = M20.r2 (WF1);;
FMAC.T1 (I1) = M21.r2 (WF1);;
FMAC.T1 (I1) = M22.r2 (WF1);;
FMAC.T1 (I1) = M23.r2 (WF1);;
FMAC.T1 (I1) = M24.r2 (WF1);;
FMAC.T1 (I1) = M25.r2 (WF1);;
FMAC.T1 (I1) = M26.r2 (WF1);;
FMAC.T1 (I1) = M27.r2 (WF1);;
FMAC.T1 (I1) = M28.r2 (WF1);;
FMAC.T1 (I1) = M29.r2 (WF1);;
FMAC.T1 (I1) = M30.r2 (WF1);;
FMAC.T1 (I1) = M31.r2 (WF1);;
FMAC.T1 (I1) = M32.r2 (WF1);;
FMAC.T1 (I1) = M33.r2 (WF1);;
FMAC.T1 (I1) = M34.r2 (WF1);;
FMAC.T1 (I1) = M35.r2 (WF1);;
FMAC.T1 (I1) = M36.r2 (WF1);;
FMAC.T1 (I1) = M37.r2 (WF1);;
FMAC.T1 (I1) = M38.r2 (WF1);;
FMAC.T1 (I1) = M39.r2 (WF1);;

//stage2
FMAC.T1 (I1) = M20.r2 (WF2);;
FMAC.T1 (I1) = M21.r2 (WF2);;
FMAC.T1 (I1) = M22.r2 (WF2);;
FMAC.T1 (I1) = M23.r2 (WF2);;
FMAC.T1 (I1) = M24.r2 (WF2);;
FMAC.T1 (I1) = M25.r2 (WF2);;
FMAC.T1 (I1) = M26.r2 (WF2);;
FMAC.T1 (I1) = M27.r2 (WF2);;
FMAC.T1 (I1) = M28.r2 (WF2);;
FMAC.T1 (I1) = M29.r2 (WF2);;
FMAC.T1 (I1) = M30.r2 (WF2);;
FMAC.T1 (I1) = M31.r2 (WF2);;
FMAC.T1 (I1) = M32.r2 (WF2);;
FMAC.T1 (I1) = M33.r2 (WF2);;
FMAC.T1 (I1) = M34.r2 (WF2);;
FMAC.T1 (I1) = M35.r2 (WF2);;
FMAC.T1 (I1) = M36.r2 (WF2);;
FMAC.T1 (I1) = M37.r2 (WF2);;
FMAC.T1 (I1) = M38.r2 (WF2);;
FMAC.T1 (I1) = M39.r2 (WF2);;

//Stage3
FMAC.T1 (I1) = M20.r2 (WF3);;
FMAC.T1 (I1) = M21.r2 (WF3);;
FMAC.T1 (I1) = M22.r2 (WF3);;
FMAC.T1 (I1) = M23.r2 (WF3);;
FMAC.T1 (I1) = M24.r2 (WF3);;
FMAC.T1 (I1) = M25.r2 (WF3);;
FMAC.T1 (I1) = M26.r2 (WF3);;
FMAC.T1 (I1) = M27.r2 (WF3);;
FMAC.T1 (I1) = M28.r2 (WF3);;
FMAC.T1 (I1) = M29.r2 (WF3);;
FMAC.T1 (I1) = M30.r2 (WF3);;
FMAC.T1 (I1) = M31.r2 (WF3);;
FMAC.T1 (I1) = M32.r2 (WF3);;
FMAC.T1 (I1) = M33.r2 (WF3);;
FMAC.T1 (I1) = M34.r2 (WF3);;
FMAC.T1 (I1) = M35.r2 (WF3);;
FMAC.T1 (I1) = M36.r2 (WF3);;
FMAC.T1 (I1) = M37.r2 (WF3);;
FMAC.T1 (I1) = M38.r2 (WF3);;
FMAC.T1 (I1) = M39.r2 (WF3);;
1:

//Stage1
FMAC.T1 (I1) = M20.r2 (WF1);;
FMAC.T1 (I1) = M21.r2 (WF1);;
FMAC.T1 (I1) = M22.r2 (WF1);;
FMAC.T1 (I1) = M23.r2 (WF1);;
FMAC.T1 (I1) = M24.r2 (WF1);;
FMAC.T1 (I1) = M25.r2 (WF1);;
FMAC.T1 (I1) = M26.r2 (WF1);;
FMAC.T1 (I1) = M27.r2 (WF1);;
FMAC.T1 (I1) = M28.r2 (WF1);;
FMAC.T1 (I1) = M29.r2 (WF1);;
FMAC.T1 (I1) = M30.r2 (WF1);;
FMAC.T1 (I1) = M31.r2 (WF1);;
FMAC.T1 (I1) = M32.r2 (WF1);;
FMAC.T1 (I1) = M33.r2 (WF1);;
FMAC.T1 (I1) = M34.r2 (WF1);;
FMAC.T1 (I1) = M35.r2 (WF1);;
FMAC.T1 (I1) = M36.r2 (WF1);;
FMAC.T1 (I1) = M37.r2 (WF1);;
FMAC.T1 (I1) = M38.r2 (WF1);;
FMAC.T1 (I1) = M39.r2 (WF1);;

//stage2
FMAC.T1 (I1) = M20.r2 (WF2);;
FMAC.T1 (I1) = M21.r2 (WF2);;
FMAC.T1 (I1) = M22.r2 (WF2);;
FMAC.T1 (I1) = M23.r2 (WF2);;
FMAC.T1 (I1) = M24.r2 (WF2);;
FMAC.T1 (I1) = M25.r2 (WF2);;
FMAC.T1 (I1) = M26.r2 (WF2);;
FMAC.T1 (I1) = M27.r2 (WF2);;
FMAC.T1 (I1) = M28.r2 (WF2);;
FMAC.T1 (I1) = M29.r2 (WF2);;
FMAC.T1 (I1) = M30.r2 (WF2);;
FMAC.T1 (I1) = M31.r2 (WF2);;
FMAC.T1 (I1) = M32.r2 (WF2);;
FMAC.T1 (I1) = M33.r2 (WF2);;
FMAC.T1 (I1) = M34.r2 (WF2);;
FMAC.T1 (I1) = M35.r2 (WF2);;
FMAC.T1 (I1) = M36.r2 (WF2);;
FMAC.T1 (I1) = M37.r2 (WF2);;
FMAC.T1 (I1) = M38.r2 (WF2);;
FMAC.T1 (I1) = M39.r2 (WF2);;

//Stage3
FMAC.T1 (I1) = M20.r2 (WF3);;
FMAC.T1 (I1) = M21.r2 (WF3);;
FMAC.T1 (I1) = M22.r2 (WF3);;
FMAC.T1 (I1) = M23.r2 (WF3);;
FMAC.T1 (I1) = M24.r2 (WF3);;
FMAC.T1 (I1) = M25.r2 (WF3);;
FMAC.T1 (I1) = M26.r2 (WF3);;
FMAC.T1 (I1) = M27.r2 (WF3);;
FMAC.T1 (I1) = M28.r2 (WF3);;
FMAC.T1 (I1) = M29.r2 (WF3);;
FMAC.T1 (I1) = M30.r2 (WF3);;
FMAC.T1 (I1) = M31.r2 (WF3);;
FMAC.T1 (I1) = M32.r2 (WF3);;
FMAC.T1 (I1) = M33.r2 (WF3);;
FMAC.T1 (I1) = M34.r2 (WF3);;
FMAC.T1 (I1) = M35.r2 (WF3);;
FMAC.T1 (I1) = M36.r2 (WF3);;
FMAC.T1 (I1) = M37.r2 (WF3);;
FMAC.T1 (I1) = M38.r2 (WF3);;
FMAC.T1 (I1) = M39.r2 (WF3);;
.endhmacro

.macro SHU0_HM name Src1
.hmacro SHU0_HM_\name
LPTO 1f AT (KI12);;
FMAC.T0 (I0)= SHU0.T0 IND \Src1 ; Repeat at (20);;
FMAC.T0 (I0)= SHU0.T0 IND T2    ; Repeat at (40);;
1:
FMAC.T0 (I0)= SHU0.T0 IND \Src1 ; Repeat at (20);;
//Repeat at (20-N);;
FMAC.T0 (I0)= SHU0.T0 IND T2    ; Repeat at (40);;
//Repeat at (20-N);;
//FMAC.T0 (I0)= SHU0.T0 IND T2    ; Repeat at (N);;
//Repeat at (20-N);;
.endhmacro
.endmacro

SHU0_HM shu0br T1
SHU0_HM shu0  T3

.macro SHU1_HM name Src1
.hmacro SHU1_HM_\name 
LPTO 1f AT (KI12);;
FALU.T0 (I2)= SHU1.T0 IND \Src1 ; Repeat at (20);;
FALU.T0 (I2)= SHU1.T0 IND T2   ; Repeat at (40);;
1:
FALU.T0 (I2)= SHU1.T0 IND \Src1 ; Repeat at (20);;
FALU.T0 (I2)= SHU1.T0 IND T2 ; Repeat at (40);;
.endhmacro
.endmacro

SHU1_HM shu1br T1 
SHU1_HM shu1   T3 


.hmacro FMAC_HM
LPTO 1f AT (KI12);;
FALU.T1 = T0 ** T1 (S); Repeat at (60);;
1:
FALU.T1 = T0 ** T1 (S); Repeat at (60);;
//Repeat at (20-N);;
//FALU.T1 = T0 ** T1 (S); Repeat at (N);;
//Repeat at (20-N);;
//FALU.T1 = T0 ** T1 (S); Repeat at (N);;
//Repeat at (20-N);;
.endhmacro

.hmacro FALU_HM
LPTO 1f AT (KI12);;
.rept 2
M0.w3 = T0 +- T1 (S);;
M1.w3 = T0 +- T1 (S);;
M2.w3 = T0 +- T1 (S);;
M3.w3 = T0 +- T1 (S);;
M4.w3 = T0 +- T1 (S);;
M5.w3 = T0 +- T1 (S);;
M6.w3 = T0 +- T1 (S);;
M7.w3 = T0 +- T1 (S);;
M8.w3 = T0 +- T1 (S);;
M9.w3 = T0 +- T1 (S);;
M10.w3 = T0 +- T1 (S);;
M11.w3 = T0 +- T1 (S);;
M12.w3 = T0 +- T1 (S);;
M13.w3 = T0 +- T1 (S);;
M14.w3 = T0 +- T1 (S);;
M15.w3 = T0 +- T1 (S);;
M16.w3 = T0 +- T1 (S);;
M17.w3 = T0 +- T1 (S);;
M18.w3 = T0 +- T1 (S);;
M19.w3 = T0 +- T1 (S);;
.endr
BIU2   = T0 +- T1 (S); Repeat at (20);;
1:
.rept 2
M0.w3 = T0 +- T1 (S);;
M1.w3 = T0 +- T1 (S);;
M2.w3 = T0 +- T1 (S);;
M3.w3 = T0 +- T1 (S);;
M4.w3 = T0 +- T1 (S);;
M5.w3 = T0 +- T1 (S);;
M6.w3 = T0 +- T1 (S);;
M7.w3 = T0 +- T1 (S);;
M8.w3 = T0 +- T1 (S);;
M9.w3 = T0 +- T1 (S);;
M10.w3 = T0 +- T1 (S);;
M11.w3 = T0 +- T1 (S);;
M12.w3 = T0 +- T1 (S);;
M13.w3 = T0 +- T1 (S);;
M14.w3 = T0 +- T1 (S);;
M15.w3 = T0 +- T1 (S);;
M16.w3 = T0 +- T1 (S);;
M17.w3 = T0 +- T1 (S);;
M18.w3 = T0 +- T1 (S);;
M19.w3 = T0 +- T1 (S);;
.endr
BIU2   = T0 +- T1 (S); Repeat at (N);;
Repeat at (20-N);;
.endhmacro


.hmacro BIU2_HM
LPTO 1f AT (KI12);;
Repeat at(40);;
DM = BIU2 (A++)(K++)(IND); Repeat at(20);;
1:
Repeat at(40);;
DM = BIU2 (A++)(K++)(IND); Repeat at(N);;
Repeat at(20-N);;
.endhmacro
//////////////////////////////////////////////////////////////


.macro main Name NameBIU0 NameSHU0 NameSHU1
.hmacro main_hm\Name
BIU0_HM_\NameBIU0 ; BIU1_HM;; // cycle 0
Repeat at (8)     ;;
MR0_HM            ;; // cycle 9
MR2_HM              ;; 
SHU0_HM_\NameSHU0 ;; // cycle 11
NOP               ;;
FMAC_HM           ;; // cycle 13
Repeat at(5)      ;; 
MR1_HM            ;; // cycle 19
NOP               ;;
SHU1_HM_\NameSHU1 ;; // cycle 21
NOP               ;;
FALU_HM           ;; // cycle 23
Repeat at(4)      ;;
BIU2_HM           ;; // cycle 28
.endhmacro
.endmacro

main _BR biu0br shu0br shu1br
main "" biu0   shu0   shu1

FFTSF4096Ep0Test :
.opt
main_hm_BR ;;
.flushhm
.stopopt
Repeat at(8);;
NOP;;   
MPU.Stop;;
NOP;;


FFTSF4096Ep1Test :
.opt
main_hm ;;
.flushhm
.stopopt
Repeat at(8);;
NOP;;   
MPU.Stop;;
NOP;;


FFTSF4096Ep2Test :
.opt
main_hm ;;
.flushhm
.stopopt
Repeat at(8);;
NOP;;   
MPU.Stop;;
NOP;;

FFTSF4096Ep3Test :
.opt
main_hm ;;
.flushhm
.stopopt
Repeat at(8);;
NOP;;   
MPU.Stop;;
NOP;;




