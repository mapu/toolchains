//  Date: 2014-12-02
//  Name: app.m.asm
//  Author: Yanglei
//  The file to test the 8 bits Filter2D, modification for adding pipeline.

//////////////////////////////////////////////////////////////
//  Input Matrix: M * N, Filter Template: a * a
//  After Padding input matrix: (M+a-1) * (N+a-1)
//  KI12 = N, KI13 = M / BS
//
////////////////////////////////////////////////////////////
     K = 7

.text
    .global Filter2DTest
  
.hmacro BIU0_HM
LPTO (2f ) @ (KI13); 
 BIU0.DM (A++,K++)->M[I++] || Repeat @ (2*K); 
Repeat @ (K*K-2*K-1); 
LPTO (1f ) @ (KI12-1); 
 BIU0.DM (A++,K++)->M[I++] || Repeat @ (2); 
Repeat @ (K*K-2); 
1:
2:
.endhmacro


.hmacro BIU1_HM
 BIU1.DM ->SHU1.T0 ; 
.endhmacro

.hmacro MR0_HM
LPTO (2f ) @ (KI13)|| LPTO (1f ) @ (KI12-1); 
.rept (K - 1)
 r0.M[I++] ->SHU0.T0 ; 
Repeat @ (K-1)      ; 
.endr
 r0.M[S++] ->SHU0.T0 ; 
Repeat @ (K-1)      ; 
1:
.rept (K)
 r0.M[I++] ->SHU0.T0 ; 
Repeat @ (K-1)      ; 
.endr
2:
.endhmacro


.hmacro MR1_HM
LPTO (2f ) @(KI13)|| LPTO (1f ) @(KI12-1); 
.rept (K - 1)
 r1.M[I++] ->SHU0.T1 ; 
Repeat @ (K-1)      ; 
.endr
 r1.M[S++] ->SHU0.T1 ; 
Repeat @ (K-1)      ; 
1:
.rept (K)
 r1.M[I++] ->SHU0.T1 ; 
Repeat @ (K-1)      ; 
.endr
2:
.endhmacro


.hmacro SHU0_HM
LPTO (2f ) @ (KI13)|| LPTO (1f ) @ (KI12);
.rept (K) 
 SHU0.T0 IND T2    ->IMAC.T0 (I0); 
 SHU0.COMB(T0,T1)(B)->IMAC.T0 (I0); 
 SHU0.COMB(T0,T1)(B,TC)->IMAC.T0 (I0)|| Repeat @ (K-2); 
.endr
1:
2:
.endhmacro


.hmacro SHU1_HM
LPTO (2f ) @ (KI13)|| LPTO (1f ) @ (KI12); 
 SHU1.T0 IND T1 (TB=+1)->IMAC.T1 (I1); 
 SHU1.T0 IND TB (TB+=1)->IMAC.T1 (I1)|| Repeat @ (K*K-1); 
1:
2:
.endhmacro


.hmacro IMAC_HM
LPTO (2f ) @ (KI13)|| LPTO (1f ) @ (KI12); 
MR+= T0 * T1 (B,CR); 
MR+= T0 * T1 (B)|| Repeat @ (K*K-2); 
 MR+= T0 * T1 (B)->BIU2 ; 
1:
2:
.endhmacro


.hmacro BIU2_HM
LPTO (2f ) @ (KI13)|| LPTO (1f ) @ (KI12); 
Repeat @ (K*K-1); 
BIU2->DM(A++,K++); 
1:
2:
.endhmacro
//////////////////////////////////////////////////////////////



.hmacro main
BIU0_HM || BIU1_HM ;  // cycle 0
Repeat @ (12)     ; 
MR0_HM  || MR1_HM  ;  // cycle 9
NOP               ;
NOP;  
SHU0_HM || SHU1_HM ;  // cycle 11
NOP               ;  
NOP;
IMAC_HM           ;  // cycle 13
Repeat @ (8)     ;  
BIU2_HM           ;  // cycle 21
.endhmacro


Filter2DTest :
.opt
main; 
.flushhm
.stopopt
Repeat @(7); 
NOP;    
MPUSTOP; 
NOP; 





