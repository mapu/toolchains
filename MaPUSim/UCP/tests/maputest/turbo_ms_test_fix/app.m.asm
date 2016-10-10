//KI12=M,KI13=P/BS,KI14=N/BS
.text
    .global Mat_Mul_SF_Test 


.hmacro HM_BIU0
LPTO 3f AT (KI14); LPTO 2f AT (KI13); LPTO 1f AT (KI12);;
SHU0.T0 = BIU0.DM (A++)(K++);;
Repeat AT (15);;
1:
2:
3:
.endhmacro

.hmacro HM_BIU1
LPTO 3f AT (KI14); LPTO 2f AT (KI13);;
M[I++].w1 = BIU1.DM (A++)(K++); Repeat AT(15);;
M[I++].w1 = BIU1.DM (A++)(K++); LPTO 1f AT (KI12-1);;
Repeat AT (16);;
1:
2:
3:
.endhmacro

.hmacro HM_BIU2_ST
LPTO 3f AT (KI14); LPTO 2f AT (KI13); LPTO 1f AT (KI12);;
DM = BIU2 (A++)(K++);;
Repeat AT (15);;
1:
2:
3:
.endhmacro


.hmacro HM_BIU2_LD
LPTO 3f AT (KI14);LPTO 1f AT (KI12);;
Repeat AT (15);;
LPTO 3f AT (KI13-1); LPTO 2f AT (KI12);;
1:
FALU.T1 (I2) = BIU2.DM ;;
Repeat AT (15);;
2:
3:
.endhmacro


.hmacro HM_SHU0
LPTO 3f AT (KI14); LPTO 2f AT (KI13); LPTO 1f AT (KI12);;
FMAC.T0 (I0) = SHU0.T0 IND T1 (TB =+4);;
FMAC.T0 (I0) = SHU0.T0 IND TB (TB +=4); Repeat AT (15);;
1:
2:
3:
.endhmacro

.hmacro HM_M0
LPTO 3f AT (KI14); LPTO 2f AT (KI13); LPTO 1f AT (KI12);;
FMAC.T1 (I1) = M[I++].r0 ; Repeat AT (16);;
1:
2:
3:
.endhmacro

.hmacro HM_FMAC
LPTO 3f AT (KI14); LPTO 2f AT (KI13); LPTO 1f AT (KI12);;
MR += T0 * T1 (CR)(S); Repeat AT (8);;
FALU.T0 = MR += T0 * T1 (S);;
FALU.T1 = MR += T0 * T1 (S);;
FALU.T2 = MR += T0 * T1 (S);;
FALU.T3 = MR += T0 * T1 (S);;
FALU.T0 = MR += T0 * T1 (S);;
FALU.T1 = MR += T0 * T1 (S);;
FALU.T2 = MR += T0 * T1 (S);;
FALU.T3 = MR += T0 * T1 (S);;
.endhmacro



.hmacro HM_FALU_TOP
LPTO 3f AT (KI14); LPTO 1f AT (KI12);;
FALU.T2 = T0 + T1 (S);;
FALU.T3 = T2 + T3 (S);;
FALU.T2 = T0 + T1 (S);;
NOP;;
FALU.T3 = T2 + T3 (S);;
NOP;;
FALU.T2 = T2 + T3 (S);;
NOP;;
NOP;;
FALU.T3 = T2 + T3 (S);;
NOP;;
NOP;;
NOP;;
NOP;;
BIU2 = T2 + T3 (S);;
LPTO 2f AT (KI13-1); LPTO 3f AT (KI12);;
1:
Repeat AT (16);;
2:
3:
.endhmacro

.hmacro HM_FALU_MID
LPTO 3f AT (KI14); LPTO 1f AT (KI12);;
Repeat AT (15);;
LPTO 2f AT (KI13-1); LPTO 3f AT (KI12);;
1:
FALU.T2 = T0 + T1 (S);;
FALU.T3 = T2 + T3 (S);;
FALU.T2 = T0 + T1 (S);;
NOP;;
FALU.T3 = T2 + T3 (S);;
NOP;;
FALU.T2 = T2 + T3 (S);;
NOP;;
NOP;;
FALU.T3 = T2 + T3 (S);;
NOP;;
NOP;;
NOP;;
NOP;;
FALU.T0 = T2 + T3 (S);;
NOP;;
2:
3:
.endhmacro

.hmacro HM_FALU_BOT
LPTO 3f AT (KI14); LPTO 1f AT (KI12);;
Repeat AT (15);;
LPTO 2f AT (KI13-1); LPTO 3f AT (KI12);;
1:
NOP;;
NOP;;
NOP;;
BIU2 = T0 + T1 (S);;
Repeat AT (12);;
2:
3:
.endhmacro


.hmacro HM_FALU
HM_FALU_TOP ; HM_FALU_MID;;
Repeat AT (15);;
HM_FALU_BOT   ;;
.endhmacro



.hmacro main
HM_BIU1 ;;                 // cycle0
HM_BIU0 ;;                 // cycle1
Repeat AT (7) ;;           // LD 8 cycles
HM_SHU0 ; HM_M0   ;;       // cycle 8
NOP;;                      // SHU 2 cyles
HM_FMAC ;;                 // cycle 10
Repeat AT (19);;           // FMAC 16 cycles, 4 cycles to wait FALU.T
HM_FALU ;;                 // cycle 30
Repeat AT (10);;           // 11 cyles to get the LD 
HM_BIU2_LD    ;;           // cycle 41
Repeat AT (12);;           // LD 8, FALU 5
HM_BIU2_ST    ;;           // cycle 54
.endhmacro

Mat_Mul_SF_Test:
.opt
main;;
.flushhm
.stopopt
NOP;;
MPU.Stop;;
