//  Date: 2013-04-24
//  Name: app.m.asm
//  Author: tao.wang@ia.ac.cn
//  The file to test the function of compound float multiplication
//  KI12=GroupNum/PipeNum

.text
    .global Initial
    .global FFTSF256Ep0Test 
    .global FFTSF256Ep1Test 
    .global FFTSF256Ep2Test 

    GroupNum   = 256 / 8
   
    PipeGrp    = 20 
    MM         = PipeGrp
    NN         = GroupNum % PipeGrp



// for blanced FFT
.macro BIURead name flag="" BIUName
.hmacro BIURead_\name
 \BIUName .DM (A++,K++ \flag)->M[I++] || Repeat @ (MM); 
Repeat @ (69-MM); 
 \BIUName .DM (A++,K++ \flag)->M[I++] || Repeat @ (NN); 
Repeat @ (69-NN); 
.endhmacro
.endmacro

BIURead biu0br  ",BR" "BIU0"
BIURead biu0    ""    "BIU0"
BIURead biu2_read ""  "BIU2"

.hmacro BIU1_HM
 BIU1.DM (A++,K++)->M[20] ; 
 BIU1.DM (A++,K++)->M[21] ; 
 BIU1.DM (A++,K++)->M[22] ; 
 BIU1.DM (A++,K++)->M[23] ; 
 BIU1.DM (A++,K++)->M[24] ; 
 BIU1.DM (A++,K++)->M[25] ; 
 BIU1.DM (A++,K++)->M[26] ; 
 BIU1.DM (A++,K++)->M[27] ; 
 BIU1.DM (A++,K++)->M[28] ; 
 BIU1.DM (A++,K++)->M[29] ; 
 BIU1.DM (A++,K++)->M[30] ; 
 BIU1.DM (A++,K++)->M[31] ; 
 BIU1.DM (A++,K++)->M[32] ; 
 BIU1.DM (A++,K++)->M[33] ; 
 BIU1.DM (A++,K++)->M[34] ; 
 BIU1.DM (A++,K++)->M[35] ; 
 BIU1.DM (A++,K++)->M[36] ; 
 BIU1.DM (A++,K++)->M[37] ; 
 BIU1.DM (A++,K++)->M[38] ; 
 BIU1.DM (A++,K++)->M[39] ; 
Repeat @ (69-MM); 

 BIU1.DM (A++,K++)->M[20] ; 
 BIU1.DM (A++,K++)->M[21] ; 
 BIU1.DM (A++,K++)->M[22] ; 
 BIU1.DM (A++,K++)->M[23] ; 
 BIU1.DM (A++,K++)->M[24] ; 
 BIU1.DM (A++,K++)->M[25] ; 
 BIU1.DM (A++,K++)->M[26] ; 
 BIU1.DM (A++,K++)->M[27] ; 
 BIU1.DM (A++,K++)->M[28] ; 
 BIU1.DM (A++,K++)->M[29] ; 
 BIU1.DM (A++,K++)->M[30] ; 
 BIU1.DM (A++,K++)->M[31] ; 
Repeat @ (69-NN); 
.endhmacro

.hmacro MR0_HM
.rept 2
 r0.M[I++] ->SHU0.T0 || Repeat @ (20);  // M.r0 must work all the time
 repeat @(3);
 r0.M[I++] ->SHU0.T0 || Repeat @ (20);  // M.r0 must work all the time
 repeat @(3);
 r0.M[I++] ->SHU0.T0 || Repeat @ (20);  // M.r0 must work all the time
 repeat @(3);
.endr
.endhmacro

.hmacro MR1_HM
.rept 3
 r1.M[0] ->SHU1.T0 ; 
 r1.M[1] ->SHU1.T0 ; 
 r1.M[2] ->SHU1.T0 ; 
 r1.M[3] ->SHU1.T0 ; 
 r1.M[4] ->SHU1.T0 ; 
 r1.M[5] ->SHU1.T0 ; 
 r1.M[6] ->SHU1.T0 ; 
 r1.M[7] ->SHU1.T0 ; 
 r1.M[8] ->SHU1.T0 ; 
 r1.M[9] ->SHU1.T0 ; 
 r1.M[10] ->SHU1.T0 ; 
 r1.M[11] ->SHU1.T0 ; 
 r1.M[12] ->SHU1.T0 ; 
 r1.M[13] ->SHU1.T0 ; 
 r1.M[14] ->SHU1.T0 ; 
 r1.M[15] ->SHU1.T0 ; 
 r1.M[16] ->SHU1.T0 ; 
 r1.M[17] ->SHU1.T0 ; 
 r1.M[18] ->SHU1.T0 ; 
 r1.M[19] ->SHU1.T0 ; 
 repeat @(3);
.endr

.rept 3
 r1.M[0] ->SHU1.T0 ; 
 r1.M[1] ->SHU1.T0 ; 
 r1.M[2] ->SHU1.T0 ; 
 r1.M[3] ->SHU1.T0 ; 
 r1.M[4] ->SHU1.T0 ; 
 r1.M[5] ->SHU1.T0 ; 
 r1.M[6] ->SHU1.T0 ; 
 r1.M[7] ->SHU1.T0 ; 
 r1.M[8] ->SHU1.T0 ; 
 r1.M[9] ->SHU1.T0 ; 
 r1.M[10] ->SHU1.T0 ; 
 r1.M[11] ->SHU1.T0 ; 
Repeat @ (23-NN); 
.endr
.endhmacro

.hmacro MR2_HM
.rept 2
//Stage1
 r2.M[20] (WF1)->FMAC.T1 (I1) ; 
 r2.M[21] (WF1)->FMAC.T1 (I1) ; 
 r2.M[22] (WF1)->FMAC.T1 (I1) ; 
 r2.M[23] (WF1)->FMAC.T1 (I1) ; 
 r2.M[24] (WF1)->FMAC.T1 (I1) ; 
 r2.M[25] (WF1)->FMAC.T1 (I1) ; 
 r2.M[26] (WF1)->FMAC.T1 (I1) ; 
 r2.M[27] (WF1)->FMAC.T1 (I1) ; 
 r2.M[28] (WF1)->FMAC.T1 (I1) ; 
 r2.M[29] (WF1)->FMAC.T1 (I1) ; 
 r2.M[30] (WF1)->FMAC.T1 (I1) ; 
 r2.M[31] (WF1)->FMAC.T1 (I1) ; 
 r2.M[32] (WF1)->FMAC.T1 (I1) ; 
 r2.M[33] (WF1)->FMAC.T1 (I1) ; 
 r2.M[34] (WF1)->FMAC.T1 (I1) ; 
 r2.M[35] (WF1)->FMAC.T1 (I1) ; 
 r2.M[36] (WF1)->FMAC.T1 (I1) ; 
 r2.M[37] (WF1)->FMAC.T1 (I1) ; 
 r2.M[38] (WF1)->FMAC.T1 (I1) ; 
 r2.M[39] (WF1)->FMAC.T1 (I1) ; 
 repeat @(3);

//stage2
 r2.M[20] (WF2)->FMAC.T1 (I1) ; 
 r2.M[21] (WF2)->FMAC.T1 (I1) ; 
 r2.M[22] (WF2)->FMAC.T1 (I1) ; 
 r2.M[23] (WF2)->FMAC.T1 (I1) ; 
 r2.M[24] (WF2)->FMAC.T1 (I1) ; 
 r2.M[25] (WF2)->FMAC.T1 (I1) ; 
 r2.M[26] (WF2)->FMAC.T1 (I1) ; 
 r2.M[27] (WF2)->FMAC.T1 (I1) ; 
 r2.M[28] (WF2)->FMAC.T1 (I1) ; 
 r2.M[29] (WF2)->FMAC.T1 (I1) ; 
 r2.M[30] (WF2)->FMAC.T1 (I1) ; 
 r2.M[31] (WF2)->FMAC.T1 (I1) ; 
 r2.M[32] (WF2)->FMAC.T1 (I1) ; 
 r2.M[33] (WF2)->FMAC.T1 (I1) ; 
 r2.M[34] (WF2)->FMAC.T1 (I1) ; 
 r2.M[35] (WF2)->FMAC.T1 (I1) ; 
 r2.M[36] (WF2)->FMAC.T1 (I1) ; 
 r2.M[37] (WF2)->FMAC.T1 (I1) ; 
 r2.M[38] (WF2)->FMAC.T1 (I1) ; 
 r2.M[39] (WF2)->FMAC.T1 (I1) ; 
 repeat @(3);

//Stage3
 r2.M[20] (WF3)->FMAC.T1 (I1) ; 
 r2.M[21] (WF3)->FMAC.T1 (I1) ; 
 r2.M[22] (WF3)->FMAC.T1 (I1) ; 
 r2.M[23] (WF3)->FMAC.T1 (I1) ; 
 r2.M[24] (WF3)->FMAC.T1 (I1) ; 
 r2.M[25] (WF3)->FMAC.T1 (I1) ; 
 r2.M[26] (WF3)->FMAC.T1 (I1) ; 
 r2.M[27] (WF3)->FMAC.T1 (I1) ; 
 r2.M[28] (WF3)->FMAC.T1 (I1) ; 
 r2.M[29] (WF3)->FMAC.T1 (I1) ; 
 r2.M[30] (WF3)->FMAC.T1 (I1) ; 
 r2.M[31] (WF3)->FMAC.T1 (I1) ; 
 r2.M[32] (WF3)->FMAC.T1 (I1) ; 
 r2.M[33] (WF3)->FMAC.T1 (I1) ; 
 r2.M[34] (WF3)->FMAC.T1 (I1) ; 
 r2.M[35] (WF3)->FMAC.T1 (I1) ; 
 r2.M[36] (WF3)->FMAC.T1 (I1) ; 
 r2.M[37] (WF3)->FMAC.T1 (I1) ; 
 r2.M[38] (WF3)->FMAC.T1 (I1) ; 
 r2.M[39] (WF3)->FMAC.T1 (I1) ; 
 repeat @(3);
.endr
.endhmacro

.macro SHU0_HM name Src1
.hmacro SHU0_HM_\name
.rept 2
 SHU0.T0 IND \Src1 ->FMAC.T0 (I0)|| Repeat @ (20); 
 repeat @(3);
 SHU0.T0 IND T2    ->FMAC.T0 (I0)|| Repeat @ (20); 
 repeat @(3);
 SHU0.T0 IND T2    ->FMAC.T0 (I0)|| Repeat @ (20); 
 repeat @(3);
.endr
.endhmacro
.endmacro

SHU0_HM shu0br T1
SHU0_HM shu0  T3

.macro SHU1_HM name Src1
.hmacro SHU1_HM_\name 
.rept 2
 SHU1.T0 IND \Src1 ->FALU.T0 (I2)|| Repeat @ (20);
 repeat @(3); 
 SHU1.T0 IND T2    ->FALU.T0 (I2)|| Repeat @ (20); 
 repeat @(3); 
 SHU1.T0 IND T2    ->FALU.T0 (I2)|| Repeat @ (20); 
 repeat @(3); 
.endr
.endhmacro
.endmacro

SHU1_HM shu1br T1 
SHU1_HM shu1   T3 


.hmacro FMAC_HM
.rept 2
 T0 ** T1 (S)->FALU.T1 || Repeat @ (69); 
.endr
.endhmacro

.macro FALU_HM_ name BIUName
.hmacro FALU_HM\name
.rept 2
 T0 +- T1 (S)->M[0] ; 
 T0 +- T1 (S)->M[1] ; 
 T0 +- T1 (S)->M[2] ; 
 T0 +- T1 (S)->M[3] ; 
 T0 +- T1 (S)->M[4] ; 
 T0 +- T1 (S)->M[5] ; 
 T0 +- T1 (S)->M[6] ; 
 T0 +- T1 (S)->M[7] ; 
 T0 +- T1 (S)->M[8] ; 
 T0 +- T1 (S)->M[9] ; 
 T0 +- T1 (S)->M[10] ; 
 T0 +- T1 (S)->M[11] ; 
 T0 +- T1 (S)->M[12] ; 
 T0 +- T1 (S)->M[13] ; 
 T0 +- T1 (S)->M[14] ; 
 T0 +- T1 (S)->M[15] ; 
 T0 +- T1 (S)->M[16] ; 
 T0 +- T1 (S)->M[17] ; 
 T0 +- T1 (S)->M[18] ; 
 T0 +- T1 (S)->M[19] ; 
 repeat @(3); 

 T0 +- T1 (S)->M[0] ; 
 T0 +- T1 (S)->M[1] ; 
 T0 +- T1 (S)->M[2] ; 
 T0 +- T1 (S)->M[3] ; 
 T0 +- T1 (S)->M[4] ; 
 T0 +- T1 (S)->M[5] ; 
 T0 +- T1 (S)->M[6] ; 
 T0 +- T1 (S)->M[7] ; 
 T0 +- T1 (S)->M[8] ; 
 T0 +- T1 (S)->M[9] ; 
 T0 +- T1 (S)->M[10] ; 
 T0 +- T1 (S)->M[11] ; 
 T0 +- T1 (S)->M[12] ; 
 T0 +- T1 (S)->M[13] ; 
 T0 +- T1 (S)->M[14] ; 
 T0 +- T1 (S)->M[15] ; 
 T0 +- T1 (S)->M[16] ; 
 T0 +- T1 (S)->M[17] ; 
 T0 +- T1 (S)->M[18] ; 
 T0 +- T1 (S)->M[19] ; 
 repeat @(3); 

 T0 +- T1 (S)-> \BIUName    || Repeat @ (20); 
 repeat @(3); 
.endr
.endhmacro
.endmacro

FALU_HM_  ""     "BIU2"
FALU_HM_  "_alt" "BIU0"


.hmacro BIU2_HM
BIU2->DM(A++,K++,I)|| Repeat @(MM); 
Repeat @(49); 
BIU2->DM(A++,K++,I)|| Repeat @(NN); 
Repeat @ (69-NN); 
.endhmacro

.hmacro BIU0_HM_write
BIU0->DM(A++,K++,I)|| Repeat @(MM); 
Repeat @(49); 
BIU0->DM(A++,K++,I)|| Repeat @(NN); 
//Repeat @ (69-NN); 
.endhmacro
//////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////
// for Unbalcend FFT stage

.hmacro UBIU0_HM
 BIU0.DM (A++,K++)->M[I++] || Repeat @ (MM); 
Repeat @ (46-MM); 
 BIU0.DM (A++,K++)->M[I++] || Repeat @ (NN); 
Repeat @ (46-NN); 
.endhmacro

.hmacro UBIU1_HM
 BIU1.DM (A++,K++)->M[20] ; 
 BIU1.DM (A++,K++)->M[21] ; 
 BIU1.DM (A++,K++)->M[22] ; 
 BIU1.DM (A++,K++)->M[23] ; 
 BIU1.DM (A++,K++)->M[24] ; 
 BIU1.DM (A++,K++)->M[25] ; 
 BIU1.DM (A++,K++)->M[26] ; 
 BIU1.DM (A++,K++)->M[27] ; 
 BIU1.DM (A++,K++)->M[28] ; 
 BIU1.DM (A++,K++)->M[29] ; 
 BIU1.DM (A++,K++)->M[30] ; 
 BIU1.DM (A++,K++)->M[31] ; 
 BIU1.DM (A++,K++)->M[32] ; 
 BIU1.DM (A++,K++)->M[33] ; 
 BIU1.DM (A++,K++)->M[34] ; 
 BIU1.DM (A++,K++)->M[35] ; 
 BIU1.DM (A++,K++)->M[36] ; 
 BIU1.DM (A++,K++)->M[37] ; 
 BIU1.DM (A++,K++)->M[38] ; 
 BIU1.DM (A++,K++)->M[39] ; 
Repeat @ (46-MM); 

 BIU1.DM (A++,K++)->M[20] ; 
 BIU1.DM (A++,K++)->M[21] ; 
 BIU1.DM (A++,K++)->M[22] ; 
 BIU1.DM (A++,K++)->M[23] ; 
 BIU1.DM (A++,K++)->M[24] ; 
 BIU1.DM (A++,K++)->M[25] ; 
 BIU1.DM (A++,K++)->M[26] ; 
 BIU1.DM (A++,K++)->M[27] ; 
 BIU1.DM (A++,K++)->M[28] ; 
 BIU1.DM (A++,K++)->M[29] ; 
 BIU1.DM (A++,K++)->M[30] ; 
 BIU1.DM (A++,K++)->M[31] ; 
Repeat @ (46-NN); 
.endhmacro

.hmacro UMR0_HM
.rept 2
 r0.M[I++] ->SHU0.T0 || Repeat @ (20);  // M.r0 must work all the time
 repeat @(3);
 r0.M[I++] ->SHU0.T0 || Repeat @ (20);  // M.r0 must work all the time
 repeat @(3);
.endr
.endhmacro

.hmacro UMR1_HM
.rept 2
 r1.M[0] ->SHU1.T0 ; 
 r1.M[1] ->SHU1.T0 ; 
 r1.M[2] ->SHU1.T0 ; 
 r1.M[3] ->SHU1.T0 ; 
 r1.M[4] ->SHU1.T0 ; 
 r1.M[5] ->SHU1.T0 ; 
 r1.M[6] ->SHU1.T0 ; 
 r1.M[7] ->SHU1.T0 ; 
 r1.M[8] ->SHU1.T0 ; 
 r1.M[9] ->SHU1.T0 ; 
 r1.M[10] ->SHU1.T0 ; 
 r1.M[11] ->SHU1.T0 ; 
 r1.M[12] ->SHU1.T0 ; 
 r1.M[13] ->SHU1.T0 ; 
 r1.M[14] ->SHU1.T0 ; 
 r1.M[15] ->SHU1.T0 ; 
 r1.M[16] ->SHU1.T0 ; 
 r1.M[17] ->SHU1.T0 ; 
 r1.M[18] ->SHU1.T0 ; 
 r1.M[19] ->SHU1.T0 ; 
 repeat @(3);
.endr

.rept 2
 r1.M[0] ->SHU1.T0 ; 
 r1.M[1] ->SHU1.T0 ; 
 r1.M[2] ->SHU1.T0 ; 
 r1.M[3] ->SHU1.T0 ; 
 r1.M[4] ->SHU1.T0 ; 
 r1.M[5] ->SHU1.T0 ; 
 r1.M[6] ->SHU1.T0 ; 
 r1.M[7] ->SHU1.T0 ; 
 r1.M[8] ->SHU1.T0 ; 
 r1.M[9] ->SHU1.T0 ; 
 r1.M[10] ->SHU1.T0 ; 
 r1.M[11] ->SHU1.T0 ; 
Repeat @ (23-NN); 
.endr
.endhmacro

.hmacro UMR2_HM
.rept 2
//stage2
 r2.M[20] (WF2)->FMAC.T1 (I1) ; 
 r2.M[21] (WF2)->FMAC.T1 (I1) ; 
 r2.M[22] (WF2)->FMAC.T1 (I1) ; 
 r2.M[23] (WF2)->FMAC.T1 (I1) ; 
 r2.M[24] (WF2)->FMAC.T1 (I1) ; 
 r2.M[25] (WF2)->FMAC.T1 (I1) ; 
 r2.M[26] (WF2)->FMAC.T1 (I1) ; 
 r2.M[27] (WF2)->FMAC.T1 (I1) ; 
 r2.M[28] (WF2)->FMAC.T1 (I1) ; 
 r2.M[29] (WF2)->FMAC.T1 (I1) ; 
 r2.M[30] (WF2)->FMAC.T1 (I1) ; 
 r2.M[31] (WF2)->FMAC.T1 (I1) ; 
 r2.M[32] (WF2)->FMAC.T1 (I1) ; 
 r2.M[33] (WF2)->FMAC.T1 (I1) ; 
 r2.M[34] (WF2)->FMAC.T1 (I1) ; 
 r2.M[35] (WF2)->FMAC.T1 (I1) ; 
 r2.M[36] (WF2)->FMAC.T1 (I1) ; 
 r2.M[37] (WF2)->FMAC.T1 (I1) ; 
 r2.M[38] (WF2)->FMAC.T1 (I1) ; 
 r2.M[39] (WF2)->FMAC.T1 (I1) ; 
 repeat @(3);

//Stage3
 r2.M[20] (WF3)->FMAC.T1 (I1) ; 
 r2.M[21] (WF3)->FMAC.T1 (I1) ; 
 r2.M[22] (WF3)->FMAC.T1 (I1) ; 
 r2.M[23] (WF3)->FMAC.T1 (I1) ; 
 r2.M[24] (WF3)->FMAC.T1 (I1) ; 
 r2.M[25] (WF3)->FMAC.T1 (I1) ; 
 r2.M[26] (WF3)->FMAC.T1 (I1) ; 
 r2.M[27] (WF3)->FMAC.T1 (I1) ; 
 r2.M[28] (WF3)->FMAC.T1 (I1) ; 
 r2.M[29] (WF3)->FMAC.T1 (I1) ; 
 r2.M[30] (WF3)->FMAC.T1 (I1) ; 
 r2.M[31] (WF3)->FMAC.T1 (I1) ; 
 r2.M[32] (WF3)->FMAC.T1 (I1) ; 
 r2.M[33] (WF3)->FMAC.T1 (I1) ; 
 r2.M[34] (WF3)->FMAC.T1 (I1) ; 
 r2.M[35] (WF3)->FMAC.T1 (I1) ; 
 r2.M[36] (WF3)->FMAC.T1 (I1) ; 
 r2.M[37] (WF3)->FMAC.T1 (I1) ; 
 r2.M[38] (WF3)->FMAC.T1 (I1) ; 
 r2.M[39] (WF3)->FMAC.T1 (I1) ; 
 repeat @(3);
.endr
.endhmacro

.hmacro USHU0_HM
.rept 2
 SHU0.T0 IND T3 ->FMAC.T0 (I0)|| Repeat @ (20); 
 repeat @(3);
 SHU0.T0 IND T2 ->FMAC.T0 (I0)|| Repeat @ (20); 
 repeat @(3);
.endr
.endhmacro




.hmacro USHU1_HM
.rept 2
 SHU1.T0 IND T3 ->FALU.T0 (I2)|| Repeat @ (20); 
 repeat @(3);
 SHU1.T0 IND T2 ->FALU.T0 (I2)|| Repeat @ (20); 
 repeat @(3);
.endr 
.endhmacro


.hmacro UFMAC_HM
.rept 2
 T0 ** T1 (S)->FALU.T1 || Repeat @ (20); 
 repeat @(3);
 T0 ** T1 (S)->FALU.T1 || Repeat @ (20); 
 repeat @(3);
.endr
.endhmacro

.hmacro UFALU_HM
.rept 2
 T0 +- T1 (S)->M[0] ; 
 T0 +- T1 (S)->M[1] ; 
 T0 +- T1 (S)->M[2] ; 
 T0 +- T1 (S)->M[3] ; 
 T0 +- T1 (S)->M[4] ; 
 T0 +- T1 (S)->M[5] ; 
 T0 +- T1 (S)->M[6] ; 
 T0 +- T1 (S)->M[7] ; 
 T0 +- T1 (S)->M[8] ; 
 T0 +- T1 (S)->M[9] ; 
 T0 +- T1 (S)->M[10] ; 
 T0 +- T1 (S)->M[11] ; 
 T0 +- T1 (S)->M[12] ; 
 T0 +- T1 (S)->M[13] ; 
 T0 +- T1 (S)->M[14] ; 
 T0 +- T1 (S)->M[15] ; 
 T0 +- T1 (S)->M[16] ; 
 T0 +- T1 (S)->M[17] ; 
 T0 +- T1 (S)->M[18] ; 
 T0 +- T1 (S)->M[19] ; 
 repeat @(3);

 T0 +- T1 (S)->BIU2   || Repeat @ (20); 
 repeat @(3);
.endr
.endhmacro


.hmacro UBIU2_HM
BIU2->DM(A++,K++,I)|| Repeat @(MM); 
Repeat @(23-MM); 
Repeat @(23);  
BIU2->DM(A++,K++,I)|| Repeat @(NN); 
//Repeat @(23-NN); 
.endhmacro
///////////////////////////////////////////////////////



.macro main Name NameBIU0 NameSHU0 NameSHU1 NameFALU NameBIUWrite
.hmacro main_hm\Name
BIURead_\NameBIU0 ;  //1
BIU1_HM;
Repeat @ (10)     ;   //12
MR0_HM;  //13
NOP;         
MR2_HM;        // 15
SHU0_HM_\NameSHU0 ;  // 16
NOP               ; 
NOP;
FMAC_HM           ;  // 19
Repeat @(3)      ;  
MR1_HM;    //23
nop;    //24
nop            ;  //25
SHU1_HM_\NameSHU1 ;  //26
NOP; 
nop;  //28
//FALU_HM; //29
\NameFALU ;
NOP;
nop           ;  //31
Repeat @(49)  ;  //80
\NameBIUWrite           ;  //81
.endhmacro
.endmacro

main _BR biu0br shu0br shu1br FALU_HM BIU2_HM
main "" biu0   shu0   shu1
main "_alt" biu2_read   shu0   shu1 FALU_HM_alt BIU0_HM_write

.hmacro umain_hm
UBIU0_HM;  //1
UBIU1_HM;
Repeat @ (10)     ; 
UMR0_HM           ;  // 13
NOP               ; 
UMR2_HM           ;  
USHU0_HM          ;  // 16
NOP               ; 
NOP               ; 
UFMAC_HM           ;  //19
Repeat @(3)      ;  
UMR1_HM            ;  //23
NOP               ; 
NOP               ; 
USHU1_HM ;  // cycle 26
NOP               ; 
NOP               ; 
UFALU_HM           ;  // cycle 29
Repeat @(28)      ; 
UBIU2_HM           ;  // 58
.endhmacro


FFTSF256Ep0Test :
.opt
NOP;
BIU1.DM(A++,K++)->M[110];  //SHU0.T1 
BIU1.DM(A++,K++)->M[111];  //SHU0.T2
BIU1.DM(A++,K++)->M[112];  //SHU0.T3
BIU1.DM(A++,K++)->M[113];  //SHU1.T1
BIU1.DM(A++,K++)->M[114];  //SHU1.T2
BIU1.DM(A++,K++)->M[115];  //SHU1.T3
BIU1.DM(A++,K++)->M[116];  //EP0BIU0
BIU1.DM(A++,K++)->M[117];  //EP0BIU1
BIU1.DM(A++,K++)->M[118];  //EP0BIU2
BIU1.DM(A++,K++)->M[119];  //EP1BIU2
BIU1.DM(A++,K++)->M[120];  //EP1BIU0
BIU1.DM(A++,K++)->M[121];  //EP2BIU0
BIU1.DM(A++,K++)->M[122];  //EP2BIU2
R0.M[110]->SHU0.T1; //SHU0.T1 
R0.M[111]->SHU0.T2; //SHU0.T2
R0.M[112]->SHU0.T3; //SHU0.T3
R0.M[113]->SHU1.T1; //SHU1.T1 
R0.M[114]->SHU1.T2; //SHU1.T2
R0.M[115]->SHU1.T3; //SHU1.T3
R3.M[116]->BIU0(KG);  //EP0BIU0 Cfg
R3.M[117]->BIU1(KG);  //EP0BIU1 Cfg
R3.M[118]->BIU2(KG);  //EP0BIU2 Cfg
NOP;
main_hm_BR ; 
.flushhm
.stopopt
Repeat @(8); 
NOP;    
MPUSTOP; 
NOP; 



FFTSF256Ep1Test :
.opt
R3.M[119]->BIU2(KG);  //EP1BIU2 Cfg
R3.M[120]->BIU0(KG);  //EP1BIU0 Cfg
NOP;
NOP;
main_hm_alt ; 
.flushhm
.stopopt
Repeat @(8); 
NOP;    
MPUSTOP; 
NOP; 


FFTSF256Ep2Test :
.opt
R3.M[121]->BIU0(KG);  //EP2BIU0 Cfg
R3.M[122]->BIU2(KG);  //EP2BIU2 Cfg
NOP;
NOP;
umain_hm; 
.flushhm
.stopopt
Repeat @(8); 
NOP;    
MPUSTOP; 
NOP; 



