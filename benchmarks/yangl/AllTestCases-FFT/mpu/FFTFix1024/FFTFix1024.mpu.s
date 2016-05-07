//  Date: 2014-01-14
//  Name: app.m.asm
//  Author: tao.wang@ia.ac.cn
//  The MPU micro-code for Fix point FFT


//  (KI12)=GroupNum/PipeNum

.text
    .global Initial
    .global FFTFix1024Ep0Test
    .global FFTFix1024Ep1Test
    .global FFTFix1024Ep2Test
   

.include "./Include.inc"


   
    N1 = 14     


.macro BIURead name flag number BIUName
 .hmacro BIURead_\name
 .if \number > 1
   LPTO (1f ) @ (KI12-1) ; 
 \BIUName .DM (A++,K++ \flag)->   M[I++] || Repeat @ (PipeGrp); 
   Repeat @ (N1*(\number)-PipeGrp); 
   1:
 \BIUName .DM (A++,K++ \flag)->   M[I++] || Repeat @ (PipeGrpTailNum); 
   Repeat @ (N1*(\number)-PipeGrpTailNum); 
 .else 
   LPTO (1f ) @ (KI12-1) ; 
 \BIUName .DM (A++,K++ \flag)->   M[I++] || Repeat @ (PipeGrp/2); 
 \BIUName .DM (A++,K++ \flag)->   M[I++] || Repeat @ (PipeGrp/2); 
 repeat @ (2);
   1:
 \BIUName .DM (A++,K++ \flag)->   M[I++] || Repeat @ (PipeGrpTailNum); 
   Repeat @ (N1-PipeGrpTailNum); 
 .endif
 .endhmacro
.endmacro

BIURead biu0_br ",BR"  4 "BIU0"
BIURead biu0_s4  ""    4 "BIU0"
BIURead biu0_s2  ""    2 "BIU0"

BIURead biu2_read_s4  ""    4 "BIU2"
BIURead biu2_read_s2  ""    2 "BIU2"


.macro _BIU1 name number
  .hmacro BIU1_\name
  .if \number > 1
    LPTO (1f ) @ (KI12-1) ; 
 BIU1.DM (A++,K++)->    M[I++] || Repeat @ (PipeGrp); 
    Repeat @ (N1*(\number)-PipeGrp); 
    1:
 BIU1.DM (A++,K++)->    M[I++] || Repeat @ (PipeGrpTailNum); 
    Repeat @ (N1*(\number)-PipeGrpTailNum); 
  .else
    LPTO (1f ) @ (KI12-1) ; 
 BIU1.DM (A++,K++)->    M[I++] || Repeat @ (PipeGrp/2); 
 BIU1.DM (A++,K++)->    M[I++] || Repeat @ (PipeGrp/2); 
 repeat @ (2);
    1:
 BIU1.DM (A++,K++)->    M[I++] || Repeat @ (PipeGrpTailNum); 
    Repeat @ (N1-PipeGrpTailNum); 
  .endif
  .endhmacro
.endmacro

_BIU1 biu1_s4 4 
_BIU1 biu1_s2 2 

.macro _MR0 name number  
  .hmacro MR0_\name
  .if \number > 2
    LPTO (1f ) @ (KI12) ; 
 r0.M[I++] ->    SHU0.T0 || Repeat @ (12); 
 repeat @(2);
 r0.M[I++] ->    SHU0.T0 || Repeat @ (12); 
 repeat @(2);
 r0.M[I++] ->    SHU0.T0 || Repeat @ (12); 
 repeat @(2);
 r0.M[I++] ->    SHU0.T0 || Repeat @ (12); 
 repeat @(2);
    1:
  .else
    LPTO (1f ) @ (KI12) ; 
 r0.M[I++] ->    SHU0.T0 || Repeat @ (12); 
 repeat @(2);
 r0.M[I++] ->    SHU0.T0 || Repeat @ (12); 
 repeat @(2);
    1:
  .endif
  .endhmacro
.endmacro

_MR0 mr0_s4 4
_MR0 mr0_s2 2

.macro _MR1 name number
  .hmacro MR1_\name  
  .if \number > 2
    LPTO (1f ) @ (KI12) ; 
 r1.M[I++] ->    SHU1.T0 || Repeat @ (12); 
 repeat @(2);
 r1.M[I++] ->    SHU1.T0 || Repeat @ (12); 
 repeat @(2);
 r1.M[I++] ->    SHU1.T0 || Repeat @ (12); 
 repeat @(2);
 r1.M[I++] ->    SHU1.T0 || Repeat @ (12); 
 repeat @(2);
    1:
  .else
    LPTO (1f ) @ (KI12) ; 
 r1.M[I++] ->    SHU1.T0 || Repeat @ (12); 
 repeat @(2);
 r1.M[I++] ->    SHU1.T0 || Repeat @ (12); 
 repeat @(2);
    1:
  .endif
  .endhmacro
.endmacro

_MR1 mr1_s4 4 
_MR1 mr1_s2 2 

.macro _MR2 name number
  .hmacro MR2_\name
  .if \number == 4
    LPTO (1f ) @ (KI12) ; 
    // stage 0
 r2.M[12] (WX1)->    IMAC.T2 (I2) ; 
 r2.M[13] (WX1)->    IMAC.T2 (I2) ; 
 r2.M[14] (WX1)->    IMAC.T2 (I2) ; 
 r2.M[15] (WX1)->    IMAC.T2 (I2) ; 
 r2.M[16] (WX1)->    IMAC.T2 (I2) ; 
 r2.M[17] (WX1)->    IMAC.T2 (I2) ; 
 r2.M[18] (WX1)->    IMAC.T2 (I2) ; 
 r2.M[19] (WX1)->    IMAC.T2 (I2) ; 
 r2.M[20] (WX1)->    IMAC.T2 (I2) ; 
 r2.M[21] (WX1)->    IMAC.T2 (I2) ; 
 r2.M[22] (WX1)->    IMAC.T2 (I2) ; 
 r2.M[23] (WX1)->    IMAC.T2 (I2) ; 
 repeat @(2);

    // stage 1
 r2.M[12] (WX2)->    IMAC.T2 (I2) ; 
 r2.M[13] (WX2)->    IMAC.T2 (I2) ; 
 r2.M[14] (WX2)->    IMAC.T2 (I2) ; 
 r2.M[15] (WX2)->    IMAC.T2 (I2) ; 
 r2.M[16] (WX2)->    IMAC.T2 (I2) ; 
 r2.M[17] (WX2)->    IMAC.T2 (I2) ; 
 r2.M[18] (WX2)->    IMAC.T2 (I2) ; 
 r2.M[19] (WX2)->    IMAC.T2 (I2) ; 
 r2.M[20] (WX2)->    IMAC.T2 (I2) ; 
 r2.M[21] (WX2)->    IMAC.T2 (I2) ; 
 r2.M[22] (WX2)->    IMAC.T2 (I2) ; 
 r2.M[23] (WX2)->    IMAC.T2 (I2) ; 
 repeat @(2);

    // stage 2
 r2.M[12] (WX3)->    IMAC.T2 (I2) ; 
 r2.M[13] (WX3)->    IMAC.T2 (I2) ; 
 r2.M[14] (WX3)->    IMAC.T2 (I2) ; 
 r2.M[15] (WX3)->    IMAC.T2 (I2) ; 
 r2.M[16] (WX3)->    IMAC.T2 (I2) ; 
 r2.M[17] (WX3)->    IMAC.T2 (I2) ; 
 r2.M[18] (WX3)->    IMAC.T2 (I2) ; 
 r2.M[19] (WX3)->    IMAC.T2 (I2) ; 
 r2.M[20] (WX3)->    IMAC.T2 (I2) ; 
 r2.M[21] (WX3)->    IMAC.T2 (I2) ; 
 r2.M[22] (WX3)->    IMAC.T2 (I2) ; 
 r2.M[23] (WX3)->    IMAC.T2 (I2) ; 
 repeat @(2);

    // stage 3
 r2.M[12] (WX4)->    IMAC.T2 (I2) ; 
 r2.M[13] (WX4)->    IMAC.T2 (I2) ; 
 r2.M[14] (WX4)->    IMAC.T2 (I2) ; 
 r2.M[15] (WX4)->    IMAC.T2 (I2) ; 
 r2.M[16] (WX4)->    IMAC.T2 (I2) ; 
 r2.M[17] (WX4)->    IMAC.T2 (I2) ; 
 r2.M[18] (WX4)->    IMAC.T2 (I2) ; 
 r2.M[19] (WX4)->    IMAC.T2 (I2) ; 
 r2.M[20] (WX4)->    IMAC.T2 (I2) ; 
 r2.M[21] (WX4)->    IMAC.T2 (I2) ; 
 r2.M[22] (WX4)->    IMAC.T2 (I2) ; 
 r2.M[23] (WX4)->    IMAC.T2 (I2) ; 
 repeat @(2);
    1:
  .elseif \number == 3
   LPTO (1f ) @ (KI12) ; 
    // no stage 0
    
    // stage 1
 r2.M[12] (WX2)->    IMAC.T2 (I2) ; 
 r2.M[13] (WX2)->    IMAC.T2 (I2) ; 
 r2.M[14] (WX2)->    IMAC.T2 (I2) ; 
 r2.M[15] (WX2)->    IMAC.T2 (I2) ; 
 r2.M[16] (WX2)->    IMAC.T2 (I2) ; 
 r2.M[17] (WX2)->    IMAC.T2 (I2) ; 
 r2.M[18] (WX2)->    IMAC.T2 (I2) ; 
 r2.M[19] (WX2)->    IMAC.T2 (I2) ; 
 r2.M[20] (WX2)->    IMAC.T2 (I2) ; 
 r2.M[21] (WX2)->    IMAC.T2 (I2) ; 
 r2.M[22] (WX2)->    IMAC.T2 (I2) ; 
 r2.M[23] (WX2)->    IMAC.T2 (I2) ; 
 repeat @(2);

    // stage 2
 r2.M[12] (WX3)->    IMAC.T2 (I2) ; 
 r2.M[13] (WX3)->    IMAC.T2 (I2) ; 
 r2.M[14] (WX3)->    IMAC.T2 (I2) ; 
 r2.M[15] (WX3)->    IMAC.T2 (I2) ; 
 r2.M[16] (WX3)->    IMAC.T2 (I2) ; 
 r2.M[17] (WX3)->    IMAC.T2 (I2) ; 
 r2.M[18] (WX3)->    IMAC.T2 (I2) ; 
 r2.M[19] (WX3)->    IMAC.T2 (I2) ; 
 r2.M[20] (WX3)->    IMAC.T2 (I2) ; 
 r2.M[21] (WX3)->    IMAC.T2 (I2) ; 
 r2.M[22] (WX3)->    IMAC.T2 (I2) ; 
 r2.M[23] (WX3)->    IMAC.T2 (I2) ; 
 repeat @(2);

    // stage 3
 r2.M[12] (WX4)->    IMAC.T2 (I2) ; 
 r2.M[13] (WX4)->    IMAC.T2 (I2) ; 
 r2.M[14] (WX4)->    IMAC.T2 (I2) ; 
 r2.M[15] (WX4)->    IMAC.T2 (I2) ; 
 r2.M[16] (WX4)->    IMAC.T2 (I2) ; 
 r2.M[17] (WX4)->    IMAC.T2 (I2) ; 
 r2.M[18] (WX4)->    IMAC.T2 (I2) ; 
 r2.M[19] (WX4)->    IMAC.T2 (I2) ; 
 r2.M[20] (WX4)->    IMAC.T2 (I2) ; 
 r2.M[21] (WX4)->    IMAC.T2 (I2) ; 
 r2.M[22] (WX4)->    IMAC.T2 (I2) ; 
 r2.M[23] (WX4)->    IMAC.T2 (I2) ; 
 repeat @(2);
    1:
  .elseif \number == 2
   LPTO (1f ) @ (KI12) ; 
    // no stage 0 and stage 1
    // stage 2
 r2.M[12] (WX3)->    IMAC.T2 (I2) ; 
 r2.M[13] (WX3)->    IMAC.T2 (I2) ; 
 r2.M[14] (WX3)->    IMAC.T2 (I2) ; 
 r2.M[15] (WX3)->    IMAC.T2 (I2) ; 
 r2.M[16] (WX3)->    IMAC.T2 (I2) ; 
 r2.M[17] (WX3)->    IMAC.T2 (I2) ; 
 r2.M[18] (WX3)->    IMAC.T2 (I2) ; 
 r2.M[19] (WX3)->    IMAC.T2 (I2) ; 
 r2.M[20] (WX3)->    IMAC.T2 (I2) ; 
 r2.M[21] (WX3)->    IMAC.T2 (I2) ; 
 r2.M[22] (WX3)->    IMAC.T2 (I2) ; 
 r2.M[23] (WX3)->    IMAC.T2 (I2) ; 
 repeat @(2);

    // stage 3
 r2.M[12] (WX4)->    IMAC.T2 (I2) ; 
 r2.M[13] (WX4)->    IMAC.T2 (I2) ; 
 r2.M[14] (WX4)->    IMAC.T2 (I2) ; 
 r2.M[15] (WX4)->    IMAC.T2 (I2) ; 
 r2.M[16] (WX4)->    IMAC.T2 (I2) ; 
 r2.M[17] (WX4)->    IMAC.T2 (I2) ; 
 r2.M[18] (WX4)->    IMAC.T2 (I2) ; 
 r2.M[19] (WX4)->    IMAC.T2 (I2) ; 
 r2.M[20] (WX4)->    IMAC.T2 (I2) ; 
 r2.M[21] (WX4)->    IMAC.T2 (I2) ; 
 r2.M[22] (WX4)->    IMAC.T2 (I2) ; 
 r2.M[23] (WX4)->    IMAC.T2 (I2) ; 
 repeat @(2);
    1:
  .elseif \number == 1
   LPTO (1f ) @ (KI12) ; 
    // no stage 0/1/2
    // stage 3
 r2.M[12] (WX4)->    IMAC.T2 (I2) ; 
 r2.M[13] (WX4)->    IMAC.T2 (I2) ; 
 r2.M[14] (WX4)->    IMAC.T2 (I2) ; 
 r2.M[15] (WX4)->    IMAC.T2 (I2) ; 
 r2.M[16] (WX4)->    IMAC.T2 (I2) ; 
 r2.M[17] (WX4)->    IMAC.T2 (I2) ; 
 r2.M[18] (WX4)->    IMAC.T2 (I2) ; 
 r2.M[19] (WX4)->    IMAC.T2 (I2) ; 
 r2.M[20] (WX4)->    IMAC.T2 (I2) ; 
 r2.M[21] (WX4)->    IMAC.T2 (I2) ; 
 r2.M[22] (WX4)->    IMAC.T2 (I2) ; 
 r2.M[23] (WX4)->    IMAC.T2 (I2) ; 
 repeat @(2);
    1:
   .endif
.endhmacro
.endmacro

_MR2 mr2_s4 4 
_MR2 mr2_s2 2 



.macro _SHU0 name src  number
  .hmacro SHU0_\name 
  .if \number == 4 
    LPTO (1f ) @ (KI12) ; 
 SHU0.T0 IND \src  ->    IMAC.T0 (I0)|| Repeat @ (PipeGrp)  ; 
 repeat @(2);

 SHU0.T0 IND T3    ->    IMAC.T0 (I0)|| Repeat @ (PipeGrp);     
 repeat @(2);
 SHU0.T0 IND T3    ->    IMAC.T0 (I0)|| Repeat @ (PipeGrp);     
 repeat @(2);
 SHU0.T0 IND T3    ->    IMAC.T0 (I0)|| Repeat @ (PipeGrp);     
 repeat @(2);
    1:
  .elseif \number == 2
    LPTO (1f ) @ (KI12) ; 
 SHU0.T0 IND \src  ->    IMAC.T0 (I0)|| Repeat @ (PipeGrp)  ; 
 repeat @(2);

 SHU0.T0 IND T3    ->    IMAC.T0 (I0)|| Repeat @ (PipeGrp);     
 repeat @(2);
    1:
  .else
    LPTO (1f ) @ (KI12) ; 
 SHU0.T0 IND T2    ->    IMAC.T0 (I0)|| Repeat @ (PipeGrp/2)  ; 
 SHU0.T0 IND T2    ->    IMAC.T0 (I0)|| Repeat @ (PipeGrp/2)  ;  
 repeat @(2);   
    1: 
  .endif
  .endhmacro
.endmacro


_SHU0 shu0_br T1 4
_SHU0 shu0_s4 T2 4
_SHU0 shu0_s2 T2 2


.macro _SHU1 name src number
  .hmacro SHU1_\name 
  .if \number == 4
    LPTO (1f ) @ (KI12) ; 
 SHU1.T0 IND \src  ->    IMAC.T1 (I1)|| Repeat @ (PipeGrp)  ; 
 repeat @(2);

 SHU1.T0 IND T3    ->    IMAC.T1 (I1)|| Repeat @ (PipeGrp);
 repeat @(2);    
 SHU1.T0 IND T3    ->    IMAC.T1 (I1)|| Repeat @ (PipeGrp);
 repeat @(2);    
 SHU1.T0 IND T3    ->    IMAC.T1 (I1)|| Repeat @ (PipeGrp);
 repeat @(2);    
    1:
  .elseif \number == 2
    LPTO (1f ) @ (KI12) ; 
 SHU1.T0 IND \src  ->    IMAC.T1 (I1)|| Repeat @ (PipeGrp)  ; 
 repeat @(2);

 SHU1.T0 IND T3    ->    IMAC.T1 (I1)|| Repeat @ (PipeGrp);
 repeat @(2);      
    1:
  .else
    LPTO (1f ) @ (KI12) ; 
 SHU1.T0 IND T2   ->    IMAC.T1 (I1)|| Repeat @ (PipeGrp/2)  ; 
 SHU1.T0 IND T2   ->    IMAC.T1 (I1)|| Repeat @ (PipeGrp/2);  
 repeat @(2);  
    1:
  .endif
  .endhmacro
.endmacro

_SHU1 shu1_br T1 4
_SHU1 shu1_s4 T2 4
_SHU1 shu1_s2 T2 2


.macro _IMAC name number BIUName
  .hmacro IMAC_\name
  .if \number > 1
    LPTO (1f ) @ (KI12) ;     
    .rept \number -1
 T0 +- T1 * T2  (H) ->    M[0]  ; 
 T0 +- T1 * T2  (H) ->    M[1]  ; 
 T0 +- T1 * T2  (H) ->    M[2]  ; 
 T0 +- T1 * T2  (H) ->    M[3]  ; 
 T0 +- T1 * T2  (H) ->    M[4]  ; 
 T0 +- T1 * T2  (H) ->    M[5]  ; 
 T0 +- T1 * T2  (H) ->    M[6]  ; 
 T0 +- T1 * T2  (H) ->    M[7]  ; 
 T0 +- T1 * T2  (H) ->    M[8]  ; 
 T0 +- T1 * T2  (H) ->    M[9]  ; 
 T0 +- T1 * T2  (H) ->    M[10] ; 
 T0 +- T1 * T2  (H) ->    M[11] ; 
 repeat @(2);  
    .endr

 T0 +- T1 * T2  (H) ->    \BIUName   || Repeat @ (PipeGrp) ; 
 repeat @(2);  
    1:
  .else
    LPTO (1f ) @ (KI12) ;  
 T0 +- T1 * T2  (H) ->    \BIUName   || Repeat @ (PipeGrp/2) ; 
 T0 +- T1 * T2  (H) ->    \BIUName   || Repeat @ (PipeGrp/2) ; 
 repeat @(2);  
    1:
  .endif
  .endhmacro
.endmacro

_IMAC imac_s4 4  "BIU2"
_IMAC imac_s2 2  "BIU2"

_IMAC imac_alt_s4 4 "BIU0" 
_IMAC imac_alt_s2 2 "BIU0"

.macro BIUWrite name number BIUName
  .hmacro BIUWrite_\name 
  .if \number > 1
    LPTO (1f ) @ (KI12-1) ; 
\BIUName ->DM(A++,K++,I)|| Repeat @(PipeGrp); 
Repeat @(N1*(\number) - PipeGrp); 
    1:
\BIUName ->DM(A++,K++,I)|| Repeat @(PipeGrpTailNum); 
//Repeat @(PipeGrp-PipeGrpTailNum); 
//Repeat @(N1*(\number) - PipeGrp); 
  .else
    LPTO (1f ) @ (KI12-1) ;    
\BIUName ->DM(A++,K++,I)|| Repeat @(PipeGrp/2); 
\BIUName ->DM(A++,K++,I)|| Repeat @(PipeGrp/2); 
 repeat @(2);  
    1:    
\BIUName ->DM(A++,K++,I)|| Repeat @(PipeGrpTailNum); 
//    Repeat @(PipeGrp-PipeGrpTailNum); 
  .endif
  .endhmacro
.endmacro

BIUWrite biu2_s4 4 BIU2
BIUWrite biu2_s2 2 BIU2

BIUWrite biu0_write_s4 4 "BIU0"
BIUWrite biu0_write_s2 2 "BIU0"
//////////////////////////////////////////////////////////////


.macro main name biu0_name biu1_name mr0_name  mr1_name mr2_name shu0_name shu1_name imac_name biu2_name
  .hmacro main_\name
    BIURead_\biu0_name        ;  // cycle 0
    BIU1_\biu1_name        ;
    Repeat @ (10)                            ; 
    MR0_\mr0_name|| MR1_\mr1_name             ;  // cycle 9
    nop;
    MR2_\mr2_name                            ;  
    SHU0_\shu0_name|| SHU1_\shu1_name         ;  // cycle 11
    NOP                                      ; 
    nop;
    IMAC_\imac_name                          ;  // cycle 13
    Repeat @(49)                             ;  
    BIUWrite_\biu2_name                          ;  // cycle 20
 .endhmacro
.endmacro

main br biu0_br biu1_s4 mr0_s4 mr1_s4 mr2_s4 shu0_br shu1_br imac_s4 biu2_s4; 
main "" biu0_s4 biu1_s4 mr0_s4 mr1_s4 mr2_s4 shu0_s4 shu1_s4 imac_s4 biu2_s4; 
main s2 biu0_s2 biu1_s2 mr0_s2 mr1_s2 mr2_s2 shu0_s2 shu1_s2 imac_s2 biu2_s2; 


.hmacro umain_hm
    BIURead_biu0_s2        ;  //1
    BIU1_biu1_s2        ;
    Repeat @ (10)                            ; 
    MR0_mr0_s2|| MR1_mr1_s2            ;  //13
    nop;
    MR2_mr2_s2                            ;  
    SHU0_shu0_s2|| SHU1_shu1_s2         ;  //16
    NOP                                      ; 
    nop;
    IMAC_imac_s2                          ;  //19
    Repeat @(21)                             ;  
    BIUWrite_biu2_s2                          ;  //27
.endhmacro

.hmacro main_alt
  BIURead_biu2_read_s4        ;                // cycle 0
  BIU1_biu1_s4        ;
  Repeat @ (10)                            ; 
  MR0_mr0_s4|| MR1_mr1_s4             ;  // cycle 9
  nop;
  MR2_mr2_s4                            ;  
  SHU0_shu0_s4 || SHU1_shu1_s4         ;  // cycle 11
  NOP                                      ; 
  nop;
  IMAC_imac_alt_s4                          ;  // cycle 13
  Repeat @(49)                             ;  
  BIUWrite_biu0_write_s4                          ;  // cycle 20
.endhmacro


FFTFix1024Ep0Test :
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
main_br  ; 
.flushhm
.stopopt
Repeat @(8); 
NOP;    
MPUSTOP; 
NOP; 

FFTFix1024Ep1Test :
.opt
R3.M[119]->BIU2(KG);  //EP1BIU2 Cfg
R3.M[120]->BIU0(KG);  //EP1BIU0 Cfg
NOP;
NOP;
main_alt  ; 
.flushhm
.stopopt
Repeat @(8); 
NOP;    
MPUSTOP; 
NOP; 


FFTFix1024Ep2Test :
.opt
R3.M[121]->BIU0(KG);  //EP1BIU0 Cfg
R3.M[122]->BIU2(KG);  //EP1BIU2 Cfg
NOP;
NOP;
umain_hm  ; 
.flushhm
.stopopt
Repeat @(8); 
NOP;    
MPUSTOP; 
NOP; 








