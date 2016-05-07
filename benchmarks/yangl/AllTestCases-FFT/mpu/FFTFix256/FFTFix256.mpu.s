//  Date: 2014-01-14
//  Name: app.m.asm
//  Author: tao.wang@ia.ac.cn
//  The MPU micro-code for Fix point FFT


//  KI12=GroupNum/PipeNum

.text
    .global Initial
    .global FFTFix256Ep0Test
    .global FFTFix256Ep1Test


    GrpNum   = 256 / 16
    PipeGrp  = 12
    

    N          = 14*4


.macro BIURead name flag BIUName
 .hmacro BIURead_\name
 \BIUName .DM (A++,K++ \flag)->   M[I++] || Repeat @ (PipeGrp); 
   Repeat @ (N-PipeGrp); 

 \BIUName .DM (A++,K++ \flag)->   M[I++] || Repeat @ (GrpNum-PipeGrp); 
   Repeat @ (N-GrpNum+PipeGrp); 
 .endhmacro
.endmacro

BIURead biu0_br ",BR" "BIU0"

BIURead biu2_read ""  "BIU2"


.hmacro BIU1_HM
 BIU1.DM (A++,K++)->M[I++] || Repeat @ (PipeGrp); 
Repeat @ (N-PipeGrp); 

 BIU1.DM (A++,K++)->M[I++] || Repeat @ (GrpNum-PipeGrp); 
Repeat @ (N-GrpNum+PipeGrp); 
.endhmacro

.hmacro MR0_HM
 r0.M[I++] ->SHU0.T0 || Repeat @ (12);
repeat @ (2);
 r0.M[I++] ->SHU0.T0 || Repeat @ (12);
repeat @ (2);
 r0.M[I++] ->SHU0.T0 || Repeat @ (12);
repeat @ (2);
 r0.M[I++] ->SHU0.T0 || Repeat @ (12);
repeat @ (2);

 r0.M[I++] ->SHU0.T0 || Repeat @ (12);
repeat @ (2);
 r0.M[I++] ->SHU0.T0 || Repeat @ (12);
repeat @ (2);
 r0.M[I++] ->SHU0.T0 || Repeat @ (12);
repeat @ (2);
 r0.M[I++] ->SHU0.T0 || Repeat @ (12);
repeat @ (2);
.endhmacro

.hmacro MR1_HM
 r1.M[I++] ->SHU1.T0 || Repeat @ (12); 
repeat @ (2);
 r1.M[I++] ->SHU1.T0 || Repeat @ (12); 
repeat @ (2);
 r1.M[I++] ->SHU1.T0 || Repeat @ (12); 
repeat @ (2);
 r1.M[I++] ->SHU1.T0 || Repeat @ (12); 
repeat @ (2);

 r1.M[I++] ->SHU1.T0 || Repeat @ (12); 
repeat @ (2);
 r1.M[I++] ->SHU1.T0 || Repeat @ (12); 
repeat @ (2);
 r1.M[I++] ->SHU1.T0 || Repeat @ (12); 
repeat @ (2);
 r1.M[I++] ->SHU1.T0 || Repeat @ (12); 
repeat @ (2);
.endhmacro

.hmacro MR2_HM
.rept 2
// stage 0
 r2.M[12] (WX1)->IMAC.T2 (I2) ; 
 r2.M[13] (WX1)->IMAC.T2 (I2) ; 
 r2.M[14] (WX1)->IMAC.T2 (I2) ; 
 r2.M[15] (WX1)->IMAC.T2 (I2) ; 
 r2.M[16] (WX1)->IMAC.T2 (I2) ; 
 r2.M[17] (WX1)->IMAC.T2 (I2) ; 
 r2.M[18] (WX1)->IMAC.T2 (I2) ; 
 r2.M[19] (WX1)->IMAC.T2 (I2) ; 
 r2.M[20] (WX1)->IMAC.T2 (I2) ; 
 r2.M[21] (WX1)->IMAC.T2 (I2) ; 
 r2.M[22] (WX1)->IMAC.T2 (I2) ; 
 r2.M[23] (WX1)->IMAC.T2 (I2) ; 
repeat @ (2);

// stage 1
 r2.M[12] (WX2)->IMAC.T2 (I2) ; 
 r2.M[13] (WX2)->IMAC.T2 (I2) ; 
 r2.M[14] (WX2)->IMAC.T2 (I2) ; 
 r2.M[15] (WX2)->IMAC.T2 (I2) ; 
 r2.M[16] (WX2)->IMAC.T2 (I2) ; 
 r2.M[17] (WX2)->IMAC.T2 (I2) ; 
 r2.M[18] (WX2)->IMAC.T2 (I2) ; 
 r2.M[19] (WX2)->IMAC.T2 (I2) ; 
 r2.M[20] (WX2)->IMAC.T2 (I2) ; 
 r2.M[21] (WX2)->IMAC.T2 (I2) ; 
 r2.M[22] (WX2)->IMAC.T2 (I2) ; 
 r2.M[23] (WX2)->IMAC.T2 (I2) ; 
repeat @ (2);

// stage 2
 r2.M[12] (WX3)->IMAC.T2 (I2) ; 
 r2.M[13] (WX3)->IMAC.T2 (I2) ; 
 r2.M[14] (WX3)->IMAC.T2 (I2) ; 
 r2.M[15] (WX3)->IMAC.T2 (I2) ; 
 r2.M[16] (WX3)->IMAC.T2 (I2) ; 
 r2.M[17] (WX3)->IMAC.T2 (I2) ; 
 r2.M[18] (WX3)->IMAC.T2 (I2) ; 
 r2.M[19] (WX3)->IMAC.T2 (I2) ; 
 r2.M[20] (WX3)->IMAC.T2 (I2) ; 
 r2.M[21] (WX3)->IMAC.T2 (I2) ; 
 r2.M[22] (WX3)->IMAC.T2 (I2) ; 
 r2.M[23] (WX3)->IMAC.T2 (I2) ;
repeat @ (2); 

// stage 3
 r2.M[12] (WX4)->IMAC.T2 (I2) ; 
 r2.M[13] (WX4)->IMAC.T2 (I2) ; 
 r2.M[14] (WX4)->IMAC.T2 (I2) ; 
 r2.M[15] (WX4)->IMAC.T2 (I2) ; 
 r2.M[16] (WX4)->IMAC.T2 (I2) ; 
 r2.M[17] (WX4)->IMAC.T2 (I2) ; 
 r2.M[18] (WX4)->IMAC.T2 (I2) ; 
 r2.M[19] (WX4)->IMAC.T2 (I2) ; 
 r2.M[20] (WX4)->IMAC.T2 (I2) ; 
 r2.M[21] (WX4)->IMAC.T2 (I2) ; 
 r2.M[22] (WX4)->IMAC.T2 (I2) ; 
 r2.M[23] (WX4)->IMAC.T2 (I2) ; 
repeat @ (2);
.endr
.endhmacro

.macro SHU0_HM name src
  .hmacro SHU0_HM_\name 
 SHU0.T0 IND \src  ->    IMAC.T0 (I0)|| Repeat @ (PipeGrp)  ; 
repeat @ (2);
 SHU0.T0 IND T3    ->IMAC.T0 (I0)|| Repeat @ (12); 
 repeat @ (2);
 SHU0.T0 IND T3    ->IMAC.T0 (I0)|| Repeat @ (12); 
 repeat @ (2);
 SHU0.T0 IND T3    ->IMAC.T0 (I0)|| Repeat @ (12); 
 repeat @ (2);

 SHU0.T0 IND \src  ->    IMAC.T0 (I0)|| Repeat @ (PipeGrp)  ; 
repeat @ (2);
 SHU0.T0 IND T3    ->IMAC.T0 (I0)|| Repeat @ (12); 
 repeat @ (2);
 SHU0.T0 IND T3    ->IMAC.T0 (I0)|| Repeat @ (12); 
 repeat @ (2);
 SHU0.T0 IND T3    ->IMAC.T0 (I0)|| Repeat @ (12); 
 repeat @ (2);
  .endhmacro
.endmacro

SHU0_HM shu0_br T1
SHU0_HM shu0    T2


.macro SHU1_HM name src
  .hmacro SHU1_HM_\name
 SHU1.T0 IND \src  ->    IMAC.T1 (I1)|| Repeat @ (PipeGrp)  ; 
 repeat @ (2);
 SHU1.T0 IND T3    ->IMAC.T1 (I1)|| Repeat @ (12); 
repeat @(2);
 SHU1.T0 IND T3    ->IMAC.T1 (I1)|| Repeat @ (12); 
repeat @(2);
 SHU1.T0 IND T3    ->IMAC.T1 (I1)|| Repeat @ (12); 
repeat @(2);

 SHU1.T0 IND \src  ->    IMAC.T1 (I1)|| Repeat @ (PipeGrp)  ; 
repeat @(2);
 SHU1.T0 IND T3    ->IMAC.T1 (I1)|| Repeat @ (12); 
repeat @(2);
 SHU1.T0 IND T3    ->IMAC.T1 (I1)|| Repeat @ (12); 
repeat @(2);
 SHU1.T0 IND T3    ->IMAC.T1 (I1)|| Repeat @ (12); 
repeat @(2);
  .endhmacro
.endmacro

SHU1_HM shu1_br T1
SHU1_HM shu1    T2

.macro IMAC_M_ Name BIUName
.hmacro IMAC_HM\Name
.rept 3
 T0 +- T1 * T2  (H) ->M[0]  ; 
 T0 +- T1 * T2  (H) ->M[1]  ; 
 T0 +- T1 * T2  (H) ->M[2]  ; 
 T0 +- T1 * T2  (H) ->M[3]  ; 
 T0 +- T1 * T2  (H) ->M[4]  ; 
 T0 +- T1 * T2  (H) ->M[5]  ; 
 T0 +- T1 * T2  (H) ->M[6]  ; 
 T0 +- T1 * T2  (H) ->M[7]  ; 
 T0 +- T1 * T2  (H) ->M[8]  ; 
 T0 +- T1 * T2  (H) ->M[9]  ; 
 T0 +- T1 * T2  (H) ->M[10] ; 
 T0 +- T1 * T2  (H) ->M[11] ; 
repeat @(2);
.endr

 T0 +- T1 * T2  (H) ->\BIUName   || Repeat @ (PipeGrp) ; 
repeat @(2);

.rept 3
 T0 +- T1 * T2  (H) ->M[0]  ; 
 T0 +- T1 * T2  (H) ->M[1]  ; 
 T0 +- T1 * T2  (H) ->M[2]  ; 
 T0 +- T1 * T2  (H) ->M[3]  ; 
 T0 +- T1 * T2  (H) ->M[4]  ; 
 T0 +- T1 * T2  (H) ->M[5]  ; 
 T0 +- T1 * T2  (H) ->M[6]  ; 
 T0 +- T1 * T2  (H) ->M[7]  ; 
 T0 +- T1 * T2  (H) ->M[8]  ; 
 T0 +- T1 * T2  (H) ->M[9]  ; 
 T0 +- T1 * T2  (H) ->M[10] ; 
 T0 +- T1 * T2  (H) ->M[11] ; 
repeat @(2);
.endr

 T0 +- T1 * T2  (H) ->\BIUName   || Repeat @ (PipeGrp) ; 
repeat @(2);
.endhmacro
.endmacro

IMAC_M_ ""       "BIU2"
IMAC_M_  "_alt"    "BIU0"


.hmacro BIUWrite_biu2_write
BIU2->DM(A++,K++,I)|| Repeat @(PipeGrp); 
Repeat @(N - PipeGrp); 
BIU2->DM(A++,K++,I)|| Repeat @(GrpNum-PipeGrp); 
//Repeat @(52);  
.endhmacro

.hmacro BIUWrite_biu0_write
BIU0->DM(A++,K++,I)|| Repeat @(PipeGrp); 
Repeat @(N - PipeGrp); 
BIU0->DM(A++,K++,I)|| Repeat @(GrpNum-PipeGrp); 
//Repeat @(52);  
.endhmacro
//////////////////////////////////////////////////////////////

.macro main name biu0_name shu0_name shu1_name
  .hmacro main_hm\name
    BIURead_\biu0_name       ;  // cycle 0
    BIU1_HM;
    Repeat @ (10)                ; 
    MR0_HM  || MR1_HM             ;  // cycle 9
    nop;
    MR2_HM                       ;  
    SHU0_HM_\shu0_name|| SHU1_HM_\shu1_name  ;  // cycle 11
    nop;
    NOP                          ; 
    IMAC_HM                      ;  // cycle 13
    Repeat @(49)                 ;  
    BIUWrite_biu2_write          ;           // cycle 20
 .endhmacro
.endmacro

main _br biu0_br shu0_br shu1_br ; 
//main "" biu0 shu0 shu1          ; 

.hmacro main_alt
    BIURead_biu2_read       ;  // cycle 0
    BIU1_HM;
    Repeat @ (10)                ; 
    MR0_HM  || MR1_HM             ;  // cycle 9
    nop;
    MR2_HM                       ;  
    SHU0_HM_shu0 || SHU1_HM_shu1 ;  // cycle 11
    nop;
    NOP                          ; 
    IMAC_HM_alt                      ;  // cycle 13
    Repeat @(49)                 ;  
    BIUWrite_biu0_write          ;           // cycle 20
.endhmacro

FFTFix256Ep0Test :
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
Repeat@(2);  
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
main_hm_br  ; 
.flushhm
.stopopt
Repeat @(8); 
NOP;    
MPUSTOP; 
NOP; 

FFTFix256Ep1Test :
.opt
R3.M[119]->BIU2(KG);  //EP1BIU2 Cfg
R3.M[120]->BIU0(KG);  //EP1BIU0 Cfg
Repeat @(2);
main_alt ; 
.flushhm
.stopopt
Repeat @(8); 
NOP;    
MPUSTOP; 
NOP; 







