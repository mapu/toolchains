//  Date: 2014-11-29
//  Name: app.m.asm
//  Author: Yanglei
//  Modification for adding pipeline.

   .global    BicubicColTest
   .global    BicubicRowTest

   inHP       =   4
   inWP       =   1
   outHP      =   9
   outWP      =   3
   PadNum     =   3
   CoeffNum   =   PadNum + 1
   WBufNum    =   inWP + PadNum
   HBufNum    =   inHP + PadNum
   HCycNum    =   CoeffNum  * outHP 
   WCycNum    =   CoeffNum  * outWP 
   WA         =   outWP               // width rept number
   HA         =   outHP               // height rept number

/////////////////////////////////////////////////////////////////////////////
.hmacro SetM_HM 
 SHU1.T3 IND TBW (TB=+4)->M[0]  ; 
 SHU1.T3 IND TB (TB+=4)->M[1]  ; 
 SHU1.T3 IND TB (TB+=4)->M[2]  ; 
 SHU1.T3 IND TB (TB+=4)->M[3]  ; 
 SHU1.T3 IND TB (TB+=4)->M[4]  ; 
 SHU1.T3 IND TB (TB+=4)->M[5]  ; 
 SHU1.T3 IND TB (TB+=4)->M[6]  ; 
 SHU1.T3 IND TB (TB+=4)->M[7]  ; 
 SHU1.T3 IND TB (TB+=4)->M[8]  ; 
 SHU1.T3 IND TB (TB+=4)->M[9]  ; 
 SHU1.T3 IND TB (TB+=4)->M[10] ; 
 SHU1.T3 IND TB (TB+=4)->M[11] ; 
 SHU1.T3 IND TB (TB+=4)->M[12] ; 
 SHU1.T3 IND TB (TB+=4)->M[13] ; 
 SHU1.T3 IND TB (TB+=4)->M[14] ; 
 SHU1.T3 IND TB (TB+=4)->M[15] ;
 NOP ;
.endhmacro

.macro Coeff_M name shu_index
.hmacro Coeff_\name
 \shu_index IND TSQ->IALU.T0(I0); 
r0.M[0]->IALU.T1(I1); 
r0.M[5]->IALU.T2(I2); 
 T0 >> T1->SHU0.T3 ; 
 T0 & T2->IMAC.T0 ; 
r0.M[2]->IMAC.T2(I2); 
NOP; 
MR+= T0*T0(CR); 
NOP; 
NOP; 
NOP; 
NOP; 
NOP; 
 MR+= T2->IALU.T3 ; 
NOP; 
NOP; 
NOP; 
 T3 >> T1->IMAC.T1 ; 
NOP; 
NOP; 
MR+= T0* T1 (CR); 
NOP; 
NOP; 
NOP; 
NOP; 
NOP; 
 MR+= T2 ->IALU.T0 ; 
NOP; 
NOP; 
NOP; 
 T0 >> T1->IMAC.T2 ; 
r0.M[7]->IMAC.T3(I0); 
NOP; 
MR+= T2 * T3 (CR); 
r0.M[8]->IMAC.T3(I0); 
NOP; 
NOP; 
NOP; 
NOP; 
MR+= T1 * T3 ; 
 r0.M[9]->IMAC.T3(I0) ; 
NOP; 
NOP; 
NOP; 
NOP; 
 MR+= T0 * T3->M[18] ; 
 r0.M[10]->IMAC.T3(I0) ; 
NOP; 
MR+= T2 * T3(CR); 
 r0.M[11]->IMAC.T3(I0) ; 
NOP; 
NOP; 
NOP; 
NOP; 
MR+= T1 * T3 ; 
 r0.M[12]->IMAC.T3 (I0) ; 
NOP; 
NOP; 
NOP; 
NOP; 
 MR+= T3->M[19] ; 
 r0.M[13]->IMAC.T3(I0) ; 
NOP; 
MR+= T2 * T3 (CR); 
 r0.M[14]->IMAC.T3(I0) ; 
NOP; 
NOP; 
NOP; 
NOP; 
MR+= T1 * T3 ; 
NOP; 
NOP; 
NOP; 
NOP; 
NOP; 
 MR+= T0->M[20] ; 
 T0 - T3->IALU.T0 ; 
NOP; 
NOP; 
 T0 >> T1->M[21] ; 
NOP; 
NOP; 
///////////////////////////////////////////////////////
//check nC
//IMAC.T0(I0) = r0.M[18] ; 
//IMAC.T1(I0) = r0.M[19] ; 
//IMAC.T2(I0) = r0.M[20] ; 
//IMAC.T3(I0) = r0.M[21] ; 
/////////////////////////////////////////////////////
 r0.M[3]  ->IALU.T2(I0) ;   //nDBits
 r0.M[4]  ->IMAC.T2(I0) ;   //nDHalf
 SHU0.T1 IND TSQ ->IALU.T0(I0);  // IALU.T0=0
 SHU0.T2 IND TSQ ->IALU.T1(I0);  // IALU.T1=255
NOP; 
.endhmacro
.endmacro
//M18 = nC0, M19 = nC1, M20 = nC2, M21 = nC3
//IALU.T0 = 0, IALU.T1 = 255, IALU.T2 = nUnit=0x400
//IMAC.T2 = nDHalf
//SHU0.T2 = 0x03020100
//////////////////////////////////////////////////////

Coeff_M Width SHU1.T0
Coeff_M Height SHU1.T1

// KI12 = AColNum
// KI13 = ARowNum/BS


/////////////////////////////////////////////////////////////
// interpolation in width direction
.hmacro WBIU0_HM
LPTO (2f ) @ (KI13); 
 BIU0.DM (A++,K++)->M[I++] || Repeat @ (4); 
Repeat @ (12 - 5) ; 
LPTO (1f ) @ (KI12-1); 
 BIU0.DM (A++,K++)->M[I++] ; 
Repeat @ (12 - 1); 
1:
2:
.endhmacro

.hmacro WMR0_HM
LPTO (2f ) @ (KI13)|| LPTO (1f ) @ (KI12-1); 
 r0.M[I++] ->IMAC.T0(I0) || Repeat @ (12-1); 
 r0.M[S++] ->IMAC.T0(I0) ; 
1:
 r0.M[I++] ->IMAC.T0(I0) || Repeat @ (8); 
 r0.M[S++] ->IMAC.T0(I0) || Repeat @ (4); 
2:
.endhmacro

.hmacro WMR1_HM
LPTO (2f ) @ (KI13)|| LPTO (1f ) @ (KI12); 
.rept 3
 r1.M[18] ->SHU0.T0 ; 
 r1.M[19] ->SHU0.T0 ; 
 r1.M[20] ->SHU0.T0 ; 
 r1.M[21] ->SHU0.T0 ; 
.endr
1:
2:
.endhmacro

.hmacro WSHU0_HM
LPTO (2f ) @ (KI13)|| LPTO (1f ) @ (KI12); 
 SHU0.T0 IND TBW (TB=+4)->IMAC.T1(I1) ; 
 SHU0.T0 IND TBW        ->IMAC.T1(I1) ; 
 SHU0.T0 IND TBW        ->IMAC.T1(I1) ; 
 SHU0.T0 IND TBW        ->IMAC.T1(I1) ; 

.rept 2
 SHU0.T0 IND TB        ->IMAC.T1(I1) ; 
 SHU0.T0 IND TB        ->IMAC.T1(I1) ; 
 SHU0.T0 IND TB        ->IMAC.T1(I1) ; 
 SHU0.T0 IND TB (TB+=4)->IMAC.T1(I1) ; 
.endr 
1:
2:
.endhmacro

.hmacro WIMAC_HM
LPTO (2f ) @ (KI13)|| LPTO (1f ) @ (KI12); 
.rept 3
MR<-T2+T0*T1  ; 
MR+= T0 * T1       ; 
MR+= T0 * T1       ; 
 MR+= T0 * T1 ->IALU.T3; 
.endr
1:
2:
.endhmacro

.hmacro WIALU_SHIFT_HM
LPTO (2f ) @ (KI13)|| LPTO (1f ) @ (KI12); 
.rept 3
 T3 >> T2 ->IALU.T3 ; 
Repeat @ (3 ); 
.endr
1:
2:
.endhmacro

.hmacro WIALU_MAX_HM
LPTO (2f ) @ (KI13)|| LPTO (1f ) @ (KI12); 
.rept 3
 MAX(T0,T3) ->IALU.T3 ; 
Repeat @ (3 ); 
.endr
1:
2:
.endhmacro

.hmacro WIALU_MIN_HM
LPTO (2f ) @ (KI13)|| LPTO (1f ) @ (KI12); 
.rept 3
 MIN(T1,T3) ->BIU2 ; 
Repeat @ (3 ); 
.endr
1:
2:
.endhmacro

.hmacro WIALU_HM
WIALU_SHIFT_HM ; 
Repeat @ (2) ; 
WIALU_MAX_HM   ; 
Repeat @(2)  ; 
WIALU_MIN_HM   ; 
.endhmacro

.hmacro WBIU2_HM
LPTO (2f ) @ (KI13-1)|| LPTO (1f ) @ (KI12); 
.rept 3
BIU2->DM(A++,K++) ; 
Repeat @ (3 ); 
.endr
1:
2:
LPTO (1f ) @ (KI12); 
.rept 3
BIU2->DM(A++,K++,M) ; 
Repeat @ (3 ); 
.endr
.endhmacro
//////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
// interpolation in height direction
.hmacro HBIU0_HM
LPTO (2f ) @ (KI13); 
 BIU2.DM (A++,K++)->M[I++] || Repeat @ (7); 
Repeat @ (36-8) ; 
LPTO (1f ) @ (KI12-1); 
 BIU2.DM (A++,K++)->M[I++] || Repeat @ (4 ); 
Repeat @ (36 - 4); 
1:
2:
.endhmacro

.hmacro HMR0_HM
LPTO (2f ) @ (KI13)|| LPTO (1f ) @ (KI12-1); 
 r0.M[I++] ->IMAC.T0(I0) || Repeat @ (12-1); 
 r0.M[S++] ->IMAC.T0(I0) ; 

.rept 3
 r0.M[I++] ->IMAC.T0(I0) || Repeat @ (7); 
 r0.M[S++] ->IMAC.T0(I0) ; 
.endr
1:
//Repeat @(4); 
 r0.M[I++] ->IMAC.T0(I0) || Repeat @ (12-1); 
 r0.M[S++] ->IMAC.T0(I0) ; 

.rept 2
 r0.M[I++] ->IMAC.T0(I0) || Repeat @ (7); 
 r0.M[S++] ->IMAC.T0(I0) ; 
.endr

 r0.M[I++] ->IMAC.T0(I0) || Repeat @ (4); 
 r0.M[S++] ->IMAC.T0(I0) || Repeat @ (4); 

2:
.endhmacro

.hmacro HMR1_HM
LPTO (2f ) @ (KI13)|| LPTO (1f ) @ (KI12); 
.rept HA
 r1.M[18] ->SHU0.T0 ; 
 r1.M[19] ->SHU0.T0 ; 
 r1.M[20] ->SHU0.T0 ; 
 r1.M[21] ->SHU0.T0 ; 
.endr
1:
2:
.endhmacro

.hmacro HSHU0_HM
LPTO (2f ) @ (KI13)|| LPTO (1f ) @ (KI12); 
 SHU0.T0 IND TBW (TB=+4)->IMAC.T1(I1) ; 
 SHU0.T0 IND TBW        ->IMAC.T1(I1) ; 
 SHU0.T0 IND TBW        ->IMAC.T1(I1) ; 
 SHU0.T0 IND TBW        ->IMAC.T1(I1) ; 

.rept 7
 SHU0.T0 IND TB        ->IMAC.T1(I1) ; 
 SHU0.T0 IND TB        ->IMAC.T1(I1) ; 
 SHU0.T0 IND TB        ->IMAC.T1(I1) ; 
 SHU0.T0 IND TB (TB+=4)->IMAC.T1(I1) ; 
.endr

 SHU0.T0 IND TB        ->IMAC.T1(I1) ; 
 SHU0.T0 IND TB        ->IMAC.T1(I1) ; 
 SHU0.T0 IND TB        ->IMAC.T1(I1) ; 
 SHU0.T0 IND TB        ->IMAC.T1(I1) ; 
1:
2:
.endhmacro

.hmacro HIMAC_HM
LPTO (2f ) @ (KI13)|| LPTO (1f ) @ (KI12); 
.rept 9
MR<-T2+T0*T1  ; 
MR+= T0 * T1       ; 
MR+= T0 * T1       ; 
 MR+= T0 * T1 ->IALU.T3; 
.endr
1:
2:
.endhmacro

.hmacro HIALU_SHIFT_HM
LPTO (2f ) @ (KI13)|| LPTO (1f ) @ (KI12); 
.rept 9
 T3 >> T2 ->IALU.T3 ; 
Repeat @ (3 ); 
.endr
1:
2:
.endhmacro

.hmacro HIALU_MAX_HM
LPTO (2f ) @ (KI13)|| LPTO (1f ) @ (KI12); 
.rept 9
 MAX(T0,T3) ->IALU.T3 ; 
Repeat @ (3 ); 
.endr
1:
2:
.endhmacro

.hmacro HIALU_MIN_HM
LPTO (2f ) @ (KI13)|| LPTO (1f ) @ (KI12); 
.rept 9
 MIN(T1,T3) ->BIU0 ; 
Repeat @ (3 ); 
.endr
1:
2:
.endhmacro

.hmacro HIALU_HM
HIALU_SHIFT_HM ; 
Repeat @ (2) ; 
HIALU_MAX_HM   ; 
Repeat @(2)  ; 
HIALU_MIN_HM   ; 
.endhmacro

.hmacro HBIU2_HM
LPTO (2f ) @ (KI13-1)|| LPTO (1f ) @ (KI12); //
.rept 9
BIU0->DM(A++,K++) ; 
Repeat @ (3 ); 
.endr
1:
2:
LPTO (1f ) @ (KI12); 
.rept 9
BIU0->DM(A++,K++,M) ; 
Repeat @ (3 ); 
.endr
1:
.endhmacro
////////////////////////////////////////////


//Interpolation in width direction
.hmacro WBicubic_HM
WBIU0_HM         ;
Repeat @(7)    ; 
WMR1_HM;
NOP;
NOP;
WSHU0_HM;  
WMR0_HM; 
NOP             ;  
WIMAC_HM         ; 
Repeat @(10)     ; 
WIALU_HM         ; 
Repeat @(9)    ; 
WBIU2_HM ;
.endhmacro


//Interpolation in heigth direction
.hmacro HBicubic_HM
HBIU0_HM         ;
Repeat @(7)    ; 
HMR1_HM;
NOP;
NOP;
HSHU0_HM;  
HMR0_HM; 
NOP             ;  
HIMAC_HM         ; 
Repeat @(10)     ; 
HIALU_HM         ; 
Repeat @(9)    ; 
HBIU2_HM ;
.endhmacro

//Interpolation in width direction
BicubicColTest:
.opt
SetM_HM    ; 
.flushhm   
Coeff_Width; 
.flushhm
WBicubic_HM  ; 
.flushhm
.stopopt
Repeat @(7); 
NOP         ; 
MPUSTOP    ; 

//Interpolation in height direction
BicubicRowTest:
.opt
SetM_HM    ; 
.flushhm   
Coeff_Height; 
.flushhm
HBicubic_HM  ; 
.flushhm
.stopopt
Repeat @(7); 
NOP         ; 
MPUSTOP    ; 


