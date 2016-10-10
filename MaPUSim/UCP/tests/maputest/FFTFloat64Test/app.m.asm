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
 BIU0.DM (A++,K++ \flag)->SHU0.T0 || Repeat @ (8); 
Repeat @ (52); 
.endhmacro
.endmacro

BIU_HM biu0br  ",BR"
BIU_HM biu0     

.hmacro BIU1_HM
 BIU1.DM (A++,K++)->FMAC.T1 (I1)|| Repeat @(8); 
Repeat @ (15); 
 BIU1.DM (A++,K++)->FMAC.T1 (I1)|| Repeat @(8); 
Repeat @ (15); 
 BIU1.DM (A++,K++)->FMAC.T1 (I1)|| Repeat @(8); 
Repeat @ (15); 
.endhmacro

.macro SHUO_HM name src
.hmacro SHU0_HM_\name 
 SHU0.T0 IND \src ->M[0] ; 
 SHU0.T0 IND \src ->M[1] ; 
 SHU0.T0 IND \src ->M[2] ; 
 SHU0.T0 IND \src ->M[3] ; 
 SHU0.T0 IND \src ->M[4] ; 
 SHU0.T0 IND \src ->M[5] ; 
 SHU0.T0 IND \src ->M[6] ; 
 SHU0.T0 IND \src ->M[7] ; 
Repeat @ (52)           ; 
.endhmacro
.endmacro

SHUO_HM shu0br T1
SHUO_HM shu0   T2



.hmacro MR0_HM
 r0.M[I++] ->SHU1.T0 || Repeat @(8); 
Repeat @ (2); 
 r0.M[I++] ->SHU1.T0 || Repeat @(8); 
Repeat @ (5); 
 r0.M[I++] ->SHU1.T0 || Repeat @(8); 
Repeat @ (2); 
 r0.M[I++] ->SHU1.T0 || Repeat @(8); 
Repeat @ (5); 
 r0.M[I++] ->SHU1.T0 || Repeat @(8); 
Repeat @ (2); 
 r0.M[I++] ->SHU1.T0 || Repeat @(8); 
Repeat @ (5); 
.endhmacro

.hmacro SHU1_HM
 SHU1.T0 IND T1 ->FMAC.T0(I0) || Repeat @(8); 
Repeat @ (2); 
 SHU1.T0 IND T2 ->FALU.T0(I0) || Repeat @(8); 
Repeat @ (5); 
 SHU1.T0 IND T1 ->FMAC.T0(I0) || Repeat @(8); 
Repeat @ (2); 
 SHU1.T0 IND T2 ->FALU.T0(I0) || Repeat @(8); 
Repeat @ (5); 
 SHU1.T0 IND T1 ->FMAC.T0(I0) || Repeat @(8); 
Repeat @ (2); 
 SHU1.T0 IND T2 ->FALU.T0(I0) || Repeat @(8); 
Repeat @ (5); 
.endhmacro

.hmacro FMAC_HM
 T0 ** T1 (S)->FALU.T1 || Repeat @ (8); 
Repeat @ (15); 
 T0 ** T1 (S)->FALU.T1 || Repeat @ (8); 
Repeat @ (15); 
 T0 ** T1 (S)->FALU.T1 || Repeat @ (8); 
Repeat @ (15); 
.endhmacro

.hmacro FALU_HM
 T0 +- T1 (S)->M[0] ; 
 T0 +- T1 (S)->M[1] ; 
 T0 +- T1 (S)->M[2] ; 
 T0 +- T1 (S)->M[3] ; 
 T0 +- T1 (S)->M[4] ; 
 T0 +- T1 (S)->M[5] ; 
 T0 +- T1 (S)->M[6] ; 
 T0 +- T1 (S)->M[7] ; 
Repeat @(15); 
 T0 +- T1 (S)->M[0] ; 
 T0 +- T1 (S)->M[1] ; 
 T0 +- T1 (S)->M[2] ; 
 T0 +- T1 (S)->M[3] ; 
 T0 +- T1 (S)->M[4] ; 
 T0 +- T1 (S)->M[5] ; 
 T0 +- T1 (S)->M[6] ; 
 T0 +- T1 (S)->M[7] ; 
Repeat @(15); 
 T0 +- T1 (S)->BIU2   || Repeat @ (8); 
Repeat @(15); 
.endhmacro


.hmacro BIU2_HM
BIU2->DM(A++,K++,I)|| Repeat @(8); 
NOP;
NOP;
.endhmacro


.hmacro main_br
BIU0_HM_biu0br    ;  // 1
Repeat @ (9)     ; 
BIU1_HM           ;  // 11
SHU0_HM_shu0br    ;  // 12
Repeat @ (3)     ;   // 15
MR0_HM            ;  // 16
NOP ;
NOP; //add           // 18    
SHU1_HM           ;  // 19
NOP               ;  
NOP; //add           // 21
FMAC_HM           ;  // 22
Repeat @ (3)     ;   // 25
Repeat @ (6)     ;   // 31
FALU_HM           ;  // 32
Repeat @ (51)     ;  // 83
BIU2_HM           ;  // 84
Repeat @ (6)     ; 
.endhmacro



.hmacro main_
BIU0_HM_biu0      ;  //1
Repeat @ (9)     ; 
BIU1_HM           ;  //11
SHU0_HM_shu0      ;  //12
Repeat @ (3)     ;  
MR0_HM            ;  // 16
NOP ;
NOP; //add           // 18    
SHU1_HM           ;  // 19
NOP               ;  
NOP; //add           // 21
FMAC_HM           ;  // 22
Repeat @ (3)     ;   // 25
Repeat @ (6)     ;   // 31
FALU_HM           ;  // 32
Repeat @ (51)     ;  // 83
BIU2_HM           ;  // 84
Repeat @ (6)     ; 
.endhmacro



FFTSF64Ep0Test :
.opt
main_br ; 
.flushhm
.stopopt
NOP;    
MPUSTOP; 
NOP; 

FFTSF64Ep1Test :
.opt
main_ ; 
.flushhm
.stopopt
NOP;    
MPUSTOP; 



