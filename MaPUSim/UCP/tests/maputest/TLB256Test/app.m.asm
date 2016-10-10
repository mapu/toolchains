//  Date: 2014-09-13
//  Name: app.m.asm
//  Author: Yanglei
//  TLB 256 test, modification for adding pipeline 

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
NOP; 
 BIU0.DM (A++,K++)->M[I++] || Repeat @ (4); 
.endhmacro

.hmacro BIU1_HM
LPTO (1f ) @ (KI12); 
 BIU1.DM (A++,K++)->M[4]      ; 
Repeat @(7); 
1:
.endhmacro


.hmacro MR0_HM
LPTO (1f ) @ (KI12); 
 r0.M[I++] ->SHU0.T0 || Repeat @(4); 
Repeat @ (4); 
1:
.endhmacro

.hmacro MR1_TOP_HM
LPTO (1f ) @ (KI12); 
 r1.M[4] ->SHU0.T1      ; 
Repeat @ (7)        ; 
1:
.endhmacro

.hmacro MR1_BOT_HM
LPTO (1f ) @ (KI12); 
Repeat @ (5)        ; 
 r1.M[7] ->IALU.T3 (I0) ; 
 NOP                  ; 
 r1.M[5] ->IALU.T1 (I0) ; 
1:
.endhmacro

.hmacro MR1_HM
MR1_TOP_HM  ; 
Repeat @(9); 
MR1_BOT_HM  ; 
.endhmacro

.hmacro MR2_TOP_HM
LPTO (1f ) @ (KI12); 
 r2.M[4] ->IALU.T0 (I0) ; 
Repeat @ (7)        ; 
1:
.endhmacro

.hmacro MR2_BOT_HM
LPTO (1f ) @ (KI12); 
Repeat @ (6)        ; 
NOP                  ; 
r2.M[6] ->IALU.T2 (I2) ; 
1:
.endhmacro

.hmacro MR2_HM
MR2_TOP_HM  ; 
Repeat @(9); 
MR2_BOT_HM  ; 
.endhmacro




.hmacro SHU0_HM
LPTO (1f ) @ (KI12); 
 SHU0.T0 IND T1 ->IALU.T1 (I1) ; 
 SHU0.T0 IND T1 ->IALU.T2 (I1) ; 
 SHU0.T0 IND T1 ->IALU.T1 (I1) ; 
 SHU0.T0 IND T1 ->IALU.T2 (I1) ; 
Repeat @ (4); 
1:
.endhmacro



.hmacro IALU_TOP_HM
LPTO (1f ) @ (KI12); 
 T0 >> 6 (U)->IALU.T0 ; 
NOP; 
NOP; 
 MERGE(T1,T2,T0)(B)->M[5]   ; 
 T0 >> 1 (U)->M[7]   ; 
 MERGE(T1,T2,T0)(B)->M[6]   ; 
Repeat @(2) ; 
1:
.endhmacro

.hmacro IALU_BOT_HM
LPTO (1f ) @ (KI12); 
Repeat @ (6)          ; 
 MERGE(T1,T2,T3)(B)->BIU2 ; 
NOP ; 
1:
.endhmacro

.hmacro IALU_HM
IALU_TOP_HM   ; 
Repeat @(8)  ; 
IALU_BOT_HM   ; 
.endhmacro

.hmacro BIU2_HM
LPTO (1f ) @ (KI12); 
BIU2->DM(A++,K++) ; 
Repeat @ (7); 
1:
.endhmacro

.hmacro main
BIU0_HM || BIU1_HM       ;
Repeat @(11)  ;  
MR0_HM || MR1_HM || MR2_HM ;
Repeat @(2)  ;
SHU0_HM        ; 
IALU_HM;
Repeat @(18)           ;  
BIU2_HM                 ; 
.endhmacro


GammarTest:
.opt
main   ; 
.flushhm
.stopopt
Repeat @(7); 
NOP         ; 
MPUSTOP    ; 

