//  Date: 2016-03-15
//  Author: Yanglei
//  PolyPhase, Width, (240 + 7) * (135 + 7)-> (480) * (270), Modification for multi-tap

   .global    W_PolyPhase
   .global    H_PolyPhase

  nTap = 8

.hmacro SetM_HM 
  SHU0.T0 IND TSQ->M[0];
  SHU0.T1 IND TSQ->M[1];
  NOP;
  NOP;
  NOP;
.endhmacro


.hmacro Coeff
  SHU0.T2 IND TBH (TB=+2)->IALU.T1(I0);//0xff
  SHU0.T2 IND TB (TB+=2)->IALU.T0(I0);//0
  SHU0.T2 IND TB (TB+=2)->IMAC.T2(I0);//nHalf
  SHU0.T2 IND TB ->IMAC.T3(I0);//1
  NOP;
  NOP;
  NOP;
.endhmacro

.hmacro W_BIU0_HM
.if nTap <= 8
LPTO (2f ) @ (KI13 - 1); 
  BIU0.DM (A++,K++)->M[I++] || Repeat @ (nTap); 
  Repeat @ (21 - nTap) ; 
  LPTO (1f ) @ (KI12 - 1); 
    BIU0.DM (A++,K++)->M[I++] ; 
    Repeat @ (22 - 1); 
  1:
2:
BIU0.DM (A++,K++,M)->M[I++] || Repeat @ (nTap);
Repeat @ (21 - nTap) ;
LPTO (1f ) @ (KI12 - 1);
  BIU0.DM (A++,K++,M)->M[I++] ; 
  Repeat @ (22 - 1); 
1:
.else
LPTO (2f ) @ (KI13 - 1); 
  BIU0.DM (A++,K++)->M[I++] || Repeat @ (nTap); 
  Repeat @ (nTap + 1) ; 
  LPTO (1f ) @ (KI12 - 1); 
    BIU0.DM (A++,K++)->M[I++] ; 
    Repeat @ (nTap * 2 + 1); 
  1:
2:
BIU0.DM (A++,K++,M)->M[I++] || Repeat @ (nTap);
Repeat @ (nTap + 1) ;
LPTO (1f ) @ (KI12 - 1);
  BIU0.DM (A++,K++,M)->M[I++] ; 
  Repeat @ (nTap * 2 + 1); 
1:
.endif
.endhmacro

.hmacro W_MR0_HM
.if nTap <= 8
LPTO (2f ) @ (KI13)|| LPTO (1f ) @ (KI12-1); 
  r0.M[I++]->IMAC.T0(I0) || Repeat @ (nTap - 1); 
  r0.M[I++]->IMAC.T0(I0);
  Repeat @ (11 - nTap);
  r0.M[I++]->IMAC.T0(I0) || Repeat @ (nTap - 1); 
  r0.M[S++]->IMAC.T0(I0) ; 
  Repeat @ (11 - nTap);
1:
  r0.M[I++]->IMAC.T0(I0) || Repeat @ (nTap - 1); 
  r0.M[I++]->IMAC.T0(I0);
  Repeat @ (11 - nTap);
  r0.M[S++]->IMAC.T0(I0) || Repeat @ (nTap - 1); 
  r0.M[S++]->IMAC.T0(I0);
  Repeat @ (11 - nTap);
2:
.else
LPTO (2f ) @ (KI13)|| LPTO (1f ) @ (KI12-1); 
  r0.M[I++]->IMAC.T0(I0) || Repeat @ (nTap - 1); 
  r0.M[I++]->IMAC.T0(I0);
  NOP;
  r0.M[I++]->IMAC.T0(I0) || Repeat @ (nTap - 1); 
  r0.M[S++]->IMAC.T0(I0) ; 
  NOP;
1:
  r0.M[I++]->IMAC.T0(I0) || Repeat @ (nTap - 1); 
  r0.M[I++]->IMAC.T0(I0);
  NOP;
  r0.M[S++]->IMAC.T0(I0) || Repeat @ (nTap - 1); 
  r0.M[S++]->IMAC.T0(I0);
  NOP;
2:
.endif
.endhmacro

.hmacro W_MR1_HM
.if nTap <= 8
LPTO (2f ) @ (KI13)|| LPTO (1f ) @ (KI12); 
  r1.M[0]->SHU0.T0 ;
  Repeat @ (11 - 1);
  r1.M[1]->SHU0.T0 ; 
  Repeat @ (11 - 1);
1:
2:
.else
LPTO (2f ) @ (KI13)|| LPTO (1f ) @ (KI12); 
  r1.M[0]->SHU0.T0 ;
  Repeat @ (nTap);
  r1.M[1]->SHU0.T0 ; 
  Repeat @ (nTap);
1:
2:
.endif
.endhmacro

.hmacro W_SHU0_HM
.if nTap <= 8
LPTO (2f ) @ (KI13)|| LPTO (1f ) @ (KI12); 
  .rept 2
  SHU0.T0 IND TBH (TB=+2)->IMAC.T1(I1) ;
  SHU0.T0 IND TB (TB+=2)->IMAC.T1(I1) || Repeat @ (nTap - 1) ;
  Repeat @ (11 - nTap);
  .endr
1:
2:
.else
LPTO (2f ) @ (KI13)|| LPTO (1f ) @ (KI12); 
  .rept 2
  SHU0.T0 IND TBH (TB=+2)->IMAC.T1(I1) ;
  SHU0.T0 IND TB (TB+=2)->IMAC.T1(I1) || Repeat @ (nTap - 1) ;
  NOP;
  .endr
1:
2:
.endif
.endhmacro

.hmacro W_IMAC_HM
.if nTap <= 8
LPTO (2f ) @ (KI13)|| LPTO (1f ) @ (KI12); 
  .rept 2
  MR+= T0 * T1(CR,H);
  MR+= T0 * T1(H) || Repeat @ (nTap - 1);
  MR+= T2 * T3(L,H) ->IALU.T2;
  Repeat @ (10 - nTap);
  .endr
1:
2:
.else
LPTO (2f ) @ (KI13)|| LPTO (1f ) @ (KI12); 
  .rept 2
  MR+= T0 * T1(CR,H);
  MR+= T0 * T1(H) || Repeat @ (nTap - 1);
  MR+= T2 * T3(L,H) ->IALU.T2;
  .endr
1:
2:
.endif
.endhmacro

.hmacro W_IALU_HM
LPTO (2f ) @ (KI13)|| LPTO (1f ) @ (KI12);
  .rept 2
  T2 >> 0xa->IALU.T2 ;
  T3 >> 0xa->IALU.T3 ;
  NOP;
  NOP;
  CPRS(T2, T3)->IALU.T3;
  NOP;
  NOP;
  MAX(T0,T3)(H) ->IALU.T3 ;
  NOP;
  NOP;
  MIN(T1,T3)(H) ->BIU2; 
  .endr
1:
2:
.endhmacro

.hmacro W_BIU2_HM
LPTO (2f ) @ (KI13 - 1)|| LPTO (1f ) @ (KI12); 
  .rept 2
  BIU2->DM(A++,K++) ; 
  Repeat @ (11 - 1); 
  .endr
1:
2:
LPTO (1f ) @ (KI12);
  .rept 2
  BIU2->DM(A++,K++,M) ; 
  Repeat @ (11 - 1); 
  .endr
1:
.endhmacro

//H, Row, 160*83->160*160
//********************************************************************************************

.hmacro H_BIU2_HM
LPTO (2f ) @ (KI13); 
  BIU2.DM (A++,K++)->M[I++] || Repeat @ (nTap); 
  Repeat @ (21 - nTap) ; 
  LPTO (1f ) @ (KI12 - 1); 
    BIU2.DM (A++,K++)->M[I++] ; 
    Repeat @ (22 - 1); 
  1:
2:
.endhmacro

.hmacro H_MR0_HM
LPTO (2f ) @ (KI13)|| LPTO (1f ) @ (KI12-1); 
  r0.M[I++]->IMAC.T0(I0) || Repeat @ (nTap - 1); 
  r0.M[I++]->IMAC.T0(I0);
  Repeat @ (11 - nTap);
  r0.M[I++]->IMAC.T0(I0) || Repeat @ (nTap - 1); 
  r0.M[S++]->IMAC.T0(I0) ; 
  Repeat @ (11 - nTap);
1:
  r0.M[I++]->IMAC.T0(I0) || Repeat @ (nTap - 1); 
  r0.M[I++]->IMAC.T0(I0);
  Repeat @ (11 - nTap);
  r0.M[S++]->IMAC.T0(I0) || Repeat @ (nTap - 1); 
  r0.M[S++]->IMAC.T0(I0);
  Repeat @ (11 - nTap);
2:
.endhmacro

.hmacro H_MR1_HM
.if nTap <= 8
LPTO (2f ) @ (KI13)|| LPTO (1f ) @ (KI12); 
  r1.M[0]->SHU0.T0 ;
  Repeat @ (11 - 1);
  r1.M[1]->SHU0.T0 ; 
  Repeat @ (11 - 1);
1:
2:
.else
LPTO (2f ) @ (KI13)|| LPTO (1f ) @ (KI12); 
  r1.M[0]->SHU0.T0 ;
  Repeat @ (nTap);
  r1.M[1]->SHU0.T0 ; 
  Repeat @ (nTap);
1:
2:
.endif
.endhmacro

.hmacro H_SHU0_HM
.if nTap <= 8
LPTO (2f ) @ (KI13)|| LPTO (1f ) @ (KI12);
  .rept 2 
  SHU0.T0 IND TBH (TB=+2)->IMAC.T1(I1) ;
  SHU0.T0 IND TB (TB+=2)->IMAC.T1(I1) || Repeat @ (nTap - 1) ;
  Repeat @ (11 - nTap);
  .endr
1:
2:
.else
LPTO (2f ) @ (KI13)|| LPTO (1f ) @ (KI12);
  .rept 2 
  SHU0.T0 IND TBH (TB=+2)->IMAC.T1(I1) ;
  SHU0.T0 IND TB (TB+=2)->IMAC.T1(I1) || Repeat @ (nTap - 1) ;
  NOP;
  .endr
1:
2:
.endif
.endhmacro

.hmacro H_IMAC_HM
.if nTap <= 8
LPTO (2f ) @ (KI13)|| LPTO (1f ) @ (KI12); 
  .rept 2
  MR+= T0 * T1(CR,H);
  MR+= T0 * T1(H) || Repeat @ (nTap - 1); 
  MR+= T2 * T3(L,H) ->IALU.T2;
  Repeat @ (10 - nTap);
  .endr
1:
2:
.else
LPTO (2f ) @ (KI13)|| LPTO (1f ) @ (KI12); 
  .rept 2
  MR+= T0 * T1(CR,H);
  MR+= T0 * T1(H) || Repeat @ (nTap - 1); 
  MR+= T2 * T3(L,H) ->IALU.T2;
  .endr
1:
2:
.endif
.endhmacro

.hmacro H_IALU_HM
LPTO (2f ) @ (KI13)|| LPTO (1f ) @ (KI12);
  .rept 2
  T2 >> 0xa->IALU.T2 ;
  T3 >> 0xa->IALU.T3 ;
  NOP;
  NOP;
  CPRS(T2, T3)->IALU.T3;
  NOP;
  NOP;
  MAX(T0,T3)(H) ->IALU.T3 ;
  NOP;
  NOP;
  MIN(T1,T3)(H) ->BIU1; 
  .endr
1:
2:
.endhmacro

.hmacro H_BIU1_HM
LPTO (2f ) @ (KI13)|| LPTO (1f ) @ (KI12); 
  .rept 2
  BIU1->DM(A++,K++) ; 
  Repeat @ (11 - 1); 
  .endr
1:
2:
.endhmacro

//*****************************
.hmacro W_PolyPhase_HM
W_BIU0_HM;
Repeat @(7); 
W_MR1_HM;
NOP;
NOP;
W_SHU0_HM;  
W_MR0_HM; 
NOP;  
W_IMAC_HM; 
Repeat @(nTap + 7); 
W_IALU_HM; 
Repeat @(13); 
W_BIU2_HM;
.endhmacro


//*****************************
.hmacro H_PolyPhase_HM
H_BIU2_HM;
Repeat @(7); 
H_MR1_HM;
NOP;
NOP;
H_SHU0_HM;  
H_MR0_HM; 
NOP;  
H_IMAC_HM; 
Repeat @(nTap + 7); 
H_IALU_HM; 
Repeat @(13); 
H_BIU1_HM;
.endhmacro


W_PolyPhase:
.opt
SetM_HM    ; 
.flushhm   
Coeff; 
.flushhm
W_PolyPhase_HM  ; 
.flushhm
.stopopt
Repeat @(10); 
NOP         ; 
MPUSTOP    ; 


H_PolyPhase:
.opt
SetM_HM    ; 
.flushhm   
Coeff; 
.flushhm
H_PolyPhase_HM  ; 
.flushhm
.stopopt
Repeat @(10); 
NOP         ; 
MPUSTOP    ; 
