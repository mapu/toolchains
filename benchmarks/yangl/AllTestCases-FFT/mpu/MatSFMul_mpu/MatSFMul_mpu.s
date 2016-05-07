//  Date: 2014-09-25
//  Name: app.m.asm
//  Author: Yanglei
//MatSFMulKETest, modification for adding pipeline


//KI12=M,KI13=P/BS,KI14=N/BS

.text
    .global Mat_Mul_SF_Test 

    BColNum = 0x42
    BS = 16
    N = BColNum % BS


.hmacro HM_BIU0
LPTO (3f ) @ (KI14)|| LPTO (2f ) @ (KI13-1)|| LPTO (1f ) @ (KI12); 
 BIU0.DM (A++,K++)->SHU0.T0 ; 
Repeat @ (14); 
LPTO (3f ) @ (KI12); 
1:
2:
 BIU0.DM (M,A++,K++)->SHU0.T0 ; 
Repeat @ (15); 
3:
.endhmacro


.hmacro HM_BIU1
LPTO (3f ) @ (KI14)|| LPTO (2f ) @ (KI13-1); 
 BIU1.DM (A++,K++)->M[I++] || Repeat @(15); 
 BIU1.DM (A++,K++)->M[I++] || LPTO (1f ) @ (KI12-1); 
Repeat @ (16); 
1:
2:
 BIU1.DM (A++,K++)->M[I++] || Repeat @(N); 
 BIU1.DM ->M[I++] || Repeat @(15-N); 
 BIU1.DM ->M[I++] || LPTO (3f ) @ (KI12-1); 
Repeat @ (16); 
3:
.endhmacro

//Modification
.hmacro HM_BIU2_ST
LPTO (1f ) @ (KI14-1)|| LPTO (1f ) @ (KI13)|| LPTO (1f ) @ (KI12); 
BIU2->DM(A++,K++); 
Repeat @ (11); 
M[64] -> BIU2(KG);
NOP;
NOP;
LPTO (2f ) @ (KI13)|| LPTO (2f ) @ (KI12); 
1:
BIU2->DM(M,A++,K++); 
Repeat @ (11); 
M[64] -> BIU2(KG);
NOP;
NOP;
NOP;
2:
.endhmacro


//Modification
.hmacro HM_BIU2_LD
LPTO (2f ) @ (KI14-1)||LPTO (1f ) @ (KI12); 
Repeat @ (9);
BIU2(KG) -> M[64];
NOP;
BIU2.DM(A++,K++) -> SHU0.T2;
Repeat @ (3); 
LPTO (2f ) @ (KI13-1)|| LPTO (2f ) @ (KI12); 
1:
 BIU2.DM ->FALU.T1 (I2) ; 
Repeat @ (8); 
BIU2(KG) -> M[64];
NOP;
BIU2.DM(A++,K++) -> SHU0.T2;
Repeat @ (3);
LPTO (3f ) @ (KI12); 
2:
Repeat @ (9);
BIU2(KG) -> M[64];
NOP;
BIU2.DM(A++,K++) -> SHU0.T2;
Repeat @ (3); 
LPTO (4f ) @ (KI13-1)|| LPTO (4f ) @ (KI12); 
3:
 BIU2.DM (M)->FALU.T1 (I2) ; 
Repeat @ (8); 
BIU2(KG) -> M[64];
NOP;
BIU2.DM(A++,K++) -> SHU0.T2;	
Repeat @ (4); 
4:
.endhmacro

.hmacro HM_SHU0
LPTO (3f ) @ (KI14)|| LPTO (2f ) @ (KI13)|| LPTO (1f ) @ (KI12); 
 SHU0.T0 IND T1 (TB=+4)->FMAC.T0 (I0) ; 
 SHU0.T0 IND TB (TB+=4)->FMAC.T0 (I0) || Repeat @ (15); 
1:
2:
3:
.endhmacro

.hmacro HM_M0
LPTO (3f ) @ (KI14)|| LPTO (2f ) @ (KI13)|| LPTO (1f ) @ (KI12); 
 r0.M[I++] ->FMAC.T1 (I1) || Repeat @ (16); 
1:
2:
3:
.endhmacro

.hmacro HM_FMAC
LPTO (3f ) @ (KI14)|| LPTO (2f ) @ (KI13)|| LPTO (1f ) @ (KI12); 
MR+= T0 * T1 (CR,S)|| Repeat @ (8); 
 MR+= T0 * T1 (S)->FALU.T0 ; 
 MR+= T0 * T1 (S)->FALU.T1 ; 
 MR+= T0 * T1 (S)->FALU.T2 ; 
 MR+= T0 * T1 (S)->FALU.T3 ; 
 MR+= T0 * T1 (S)->FALU.T0 ; 
 MR+= T0 * T1 (S)->FALU.T1 ; 
 MR+= T0 * T1 (S)->FALU.T2 ; 
 MR+= T0 * T1 (S)->FALU.T3 ; 
.endhmacro



.hmacro HM_FALU_TOP
LPTO (3f ) @ (KI14)|| LPTO (1f ) @ (KI12); 
 T0 + T1 (S)->FALU.T2 ; 
 T2 + T3 (S)->FALU.T3 ; 
 T0 + T1 (S)->FALU.T2 ; 
NOP; 
 T2 + T3 (S)->FALU.T3 ; 
NOP; 
 T2 + T3 (S)->FALU.T2 ; 
NOP; 
NOP; 
 T2 + T3 (S)->FALU.T3 ; 
NOP; 
NOP; 
NOP; 
NOP; 
 T2 + T3 (S)->BIU2 ; 
LPTO (2f ) @ (KI13-1)|| LPTO (3f ) @ (KI12); 
1:
Repeat @ (16); 
2:
3:
.endhmacro

.hmacro HM_FALU_MID
LPTO (3f ) @ (KI14)|| LPTO (1f ) @ (KI12); 
Repeat @ (15); 
LPTO (2f ) @ (KI13-1)|| LPTO (3f ) @ (KI12); 
1:
 T0 + T1 (S)->FALU.T2 ; 
 T2 + T3 (S)->FALU.T3 ; 
 T0 + T1 (S)->FALU.T2 ; 
NOP; 
 T2 + T3 (S)->FALU.T3 ; 
NOP; 
 T2 + T3 (S)->FALU.T2 ; 
NOP; 
NOP; 
 T2 + T3 (S)->FALU.T3 ; 
NOP; 
NOP; 
NOP; 
NOP; 
 T2 + T3 (S)->FALU.T0 ; 
NOP; 
2:
3:
.endhmacro

.hmacro HM_FALU_BOT
LPTO (3f ) @ (KI14)|| LPTO (1f ) @ (KI12); 
Repeat @ (15); 
LPTO (2f ) @ (KI13-1)|| LPTO (3f ) @ (KI12); 
1:
NOP; 
NOP; 
NOP; 
 T0 + T1 (S)->BIU2 ; 
Repeat @ (12); 
2:
3:
.endhmacro


.hmacro HM_FALU
HM_FALU_TOP || HM_FALU_MID; 
Repeat @ (15); 
HM_FALU_BOT   ; 
.endhmacro



.hmacro main
HM_BIU0||HM_BIU1;
Repeat @ (10) ;         
HM_SHU0   ;  
HM_M0;
NOP;    
HM_FMAC ;   
Repeat @ (19); 
HM_FALU ;       
Repeat @ (7);       
HM_BIU2_LD    ;    
Repeat @ (13);   
// BIU2(KG) -> M[64];
// BIU2.DM(A++,K++) -> SHU0.T2;
//Repeat @ (2);
M[64] -> BIU2(KG);
NOP;
NOP;
HM_BIU2_ST    ;            // cycle 54
.endhmacro

Mat_Mul_SF_Test:
.opt
main; 
.flushhm
.stopopt
NOP; 
MPUSTOP; 
