//  Date:   2014-07-18
//  Name:   app.m.asm
//  Author: yanglp@aurora-hp.nadec.com
//  The file to test the function of FALU

.text
    .global _start    
    .global _FALUTest0
    .global _FALUTest1
    .global _FALUTest2
    .global _FALUTest3
    .global _FALUTest4
    .global _FALUTest5
    .global _FALUTest6
    .global _FALUTest7
    .global _FALUTest8
    .global _FALUTest9
    .global _FALUTest10
    
_start:
    NOP; 
    NOP; 
    NOP; 
_FALUTest0:
    NOP; 
    NOP; 
    NOP; 
    NOP; 
 SHU0.T0 IND TSQ ->    M[0] ; 
    NOP; 
    
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    MPUSTOP; 
    
_FALUTest1:
    NOP; 
    //Initial the FALU.Ts
 r0.M[0]->    FALU.T0 (I0) ; 
    NOP; 
    NOP; 
 DOUBLE T0                 ->    SHU0.T2 ;                 // PC = 0x012
 DOUBLE T0 (U)             ->    SHU0.T2 ;                 // PC = 0x013
    //Cond=1, (C) & (!C)
 T0 == T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
    NOP; 
 DOUBLE T0              ->    SHU0.T2 @(C);                 // PC = 0x019
 DOUBLE T0 (U)         ->    SHU0.T2 @(!C);                 // PC = 0x01a
    //Cond=0, (C) & (!C)
 T0 != T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
    NOP; 
 DOUBLE T0 (U)         ->    SHU0.T2 @(!C);                 // PC = 0x020
 DOUBLE T0              ->    SHU0.T2 @(C);                 // PC = 0x021
    
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    MPUSTOP; 

_FALUTest2:
    NOP; 
    //Initial the FALU.Ts
 r0.M[0]->    FALU.T0 (I0) ; 
    NOP; 
    NOP; 
 DOUBLE T0                 ->    SHU1.T2 ;                 // PC = 0x02f
 DOUBLE T0 (U)             ->    SHU1.T2 ;                 // PC = 0x030
    //Cond=1, (C) & (!C)
 T0 == T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
    NOP; 
 DOUBLE T0              ->    SHU1.T2 @(C);                 // PC = 0x036
 DOUBLE T0 (U)         ->    SHU1.T2 @(!C);                 // PC = 0x037
    //Cond=0, (C) & (!C)
 T0 != T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
    NOP; 
 DOUBLE T0 (U)         ->    SHU1.T2 @(!C);                 // PC = 0x03d
 DOUBLE T0              ->    SHU1.T2 @(C);                 // PC = 0x03e
    
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    MPUSTOP; 

_FALUTest3:
    NOP; 
    //Initial the FALU.Ts
 r0.M[0]->    FALU.T0 (I0) ; 
    NOP; 
    NOP; 
 DOUBLE T0                 ->    IALU.T2 ;                 // PC = 0x04c
 DOUBLE T0 (U)             ->    IALU.T2 ;                 // PC = 0x04d
    //Cond=1, (C) & (!C)
 T0 == T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
    NOP; 
 DOUBLE T0              ->    IALU.T2 @(C);                 // PC = 0x053
 DOUBLE T0 (U)         ->    IALU.T2 @(!C);                 // PC = 0x054
    //Cond=0, (C) & (!C)
 T0 != T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
    NOP; 
 DOUBLE T0 (U)         ->    IALU.T2 @(!C);                 // PC = 0x05a
 DOUBLE T0              ->    IALU.T2 @(C);                 // PC = 0x05b
    
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    MPUSTOP; 

_FALUTest4:
    NOP; 
    //Initial the FALU.Ts
 r0.M[0]->    FALU.T0 (I0) ; 
    NOP; 
    NOP; 
 DOUBLE T0                 ->    IMAC.T2 ;                 // PC = 0x069
 DOUBLE T0 (U)             ->    IMAC.T2 ;                 // PC = 0x06a
    //Cond=1, (C) & (!C)
 T0 == T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
    NOP; 
 DOUBLE T0              ->    IMAC.T2 @(C);                 // PC = 0x070
 DOUBLE T0 (U)         ->    IMAC.T2 @(!C);                 // PC = 0x071
    //Cond=0, (C) & (!C)
 T0 != T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
    NOP; 
 DOUBLE T0 (U)         ->    IMAC.T2 @(!C);                 // PC = 0x077
 DOUBLE T0              ->    IMAC.T2 @(C);                 // PC = 0x078
    
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    MPUSTOP; 


_FALUTest5:
    NOP; 
    //Initial the FALU.Ts
 r0.M[0]->    FALU.T0 (I0) ; 
    NOP; 
    NOP; 
 DOUBLE T0                 ->    FALU.T2 ;                 // PC = 0x086
 DOUBLE T0 (U)             ->    FALU.T2 ;                 // PC = 0x087
    //Cond=1, (C) & (!C)
 T0 == T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
    NOP; 
 DOUBLE T0              ->    FALU.T2 @(C);                 // PC = 0x08d
 DOUBLE T0 (U)         ->    FALU.T2 @(!C);                 // PC = 0x08e
    //Cond=0, (C) & (!C)
 T0 != T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
    NOP; 
 DOUBLE T0 (U)         ->    FALU.T2 @(!C);                 // PC = 0x094
 DOUBLE T0              ->    FALU.T2 @(C);                 // PC = 0x095
    
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    MPUSTOP; 


_FALUTest6:
    NOP; 
    //Initial the FALU.Ts
 r0.M[0]->    FALU.T0 (I0) ; 
    NOP; 
    NOP; 
 DOUBLE T0                 ->    FMAC.T2 ;                 // PC = 0x0a3
 DOUBLE T0 (U)             ->    FMAC.T2 ;                 // PC = 0x0a4
    //Cond=1, (C) & (!C)
 T0 == T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
    NOP; 
 DOUBLE T0              ->    FMAC.T2 @(C);                 // PC = 0x0aa
 DOUBLE T0 (U)         ->    FMAC.T2 @(!C);                 // PC = 0x0ab
    //Cond=0, (C) & (!C)
 T0 != T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
    NOP; 
 DOUBLE T0 (U)         ->    FMAC.T2 @(!C);                 // PC = 0x0b1
 DOUBLE T0              ->    FMAC.T2 @(C);                 // PC = 0x0b2
    
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    MPUSTOP; 


_FALUTest7:
    NOP; 
    //Initial the FALU.Ts
 r0.M[0]->    FALU.T0 (I0) ; 
    NOP; 
    NOP; 
 DOUBLE T0                 ->    BIU0 ;                 // PC = 0x0c0
 DOUBLE T0 (U)             ->    BIU0 ;                 // PC = 0x0c1
    //Cond=1, (C) & (!C)
 T0 == T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
    NOP; 
 DOUBLE T0              ->    BIU0 @(C);                 // PC = 0x0c7
 DOUBLE T0 (U)         ->    BIU0 @(!C);                 // PC = 0x0c8
    //Cond=0, (C) & (!C)
 T0 != T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
    NOP; 
 DOUBLE T0 (U)         ->    BIU0 @(!C);                 // PC = 0x0ce
 DOUBLE T0              ->    BIU0 @(C);                 // PC = 0x0cf
    
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    MPUSTOP; 


_FALUTest8:
    NOP; 
    //Initial the FALU.Ts
 r0.M[0]->    FALU.T0 (I0) ; 
    NOP; 
    NOP; 
 DOUBLE T0                 ->    BIU1 ;                 // PC = 0x0dd
 DOUBLE T0 (U)             ->    BIU1 ;                 // PC = 0x0de
    //Cond=1, (C) & (!C)
 T0 == T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
    NOP; 
 DOUBLE T0              ->    BIU1 @(C);                 // PC = 0x0e4
 DOUBLE T0 (U)         ->    BIU1 @(!C);                 // PC = 0x0e5
    //Cond=0, (C) & (!C)
 T0 != T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
    NOP; 
 DOUBLE T0 (U)         ->    BIU1 @(!C);                 // PC = 0x0eb
 DOUBLE T0              ->    BIU1 @(C);                 // PC = 0x0ec
    
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    MPUSTOP; 


_FALUTest9:
    NOP; 
    //Initial the FALU.Ts
 r0.M[0]->    FALU.T0 (I0) ; 
    NOP; 
    NOP; 
 DOUBLE T0                 ->    BIU2 ;                 // PC = 0x0fa
 DOUBLE T0 (U)             ->    BIU2 ;                 // PC = 0x0fb
    //Cond=1, (C) & (!C)
 T0 == T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
    NOP; 
 DOUBLE T0              ->    BIU2 @(C);                 // PC = 0x101
 DOUBLE T0 (U)         ->    BIU2 @(!C);                 // PC = 0x102
    //Cond=0, (C) & (!C)
 T0 != T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
    NOP; 
 DOUBLE T0 (U)         ->    BIU2 @(!C);                 // PC = 0x108
 DOUBLE T0              ->    BIU2 @(C);                 // PC = 0x109
    
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    MPUSTOP; 


_FALUTest10:
    NOP; 
    //Initial the FALU.Ts
 r0.M[0]->    FALU.T0 (I0) ; 
    NOP; 
    NOP; 
 DOUBLE T0                 ->    M[10] ;                 // PC = 0x117
 DOUBLE T0 (U)             ->    M[10] ;                 // PC = 0x118
    //Cond=1, (C) & (!C)
 T0 == T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
    NOP; 
 DOUBLE T0              ->    M[10] @(C);                 // PC = 0x11e
 DOUBLE T0 (U)         ->    M[11] @(!C);                 // PC = 0x11f
    //Cond=0, (C) & (!C)
 T0 != T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
    NOP; 
 DOUBLE T0 (U)         ->    M[10] @(!C);                 // PC = 0x125
 DOUBLE T0              ->    M[11] @(C);                 // PC = 0x126
    
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    MPUSTOP; 



