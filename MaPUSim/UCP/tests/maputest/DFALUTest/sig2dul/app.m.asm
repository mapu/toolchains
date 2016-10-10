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
_FALUTest0:
    NOP; 
    NOP; 
    NOP; 
    NOP; 
 SHU0.T0 IND TSQ ->    M[0] ; 
 SHU1.T0 IND TSQ ->    M[1] ; 
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
 DOUBLE T0   (S)           ->    SHU0.T1 ;                 // PC = 0x012
    //Cond=1, (C) & (!C)
 T0 == T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
    NOP; 
 DOUBLE T0 (S)         ->    SHU0.T2 @(!C);                 // PC = 0x018
 DOUBLE T0 (S)          ->    SHU0.T2 @(C);                 // PC = 0x019
    //Cond=0, (C) & (!C)
 T0 != T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
    NOP; 
 DOUBLE T0 (S)          ->    SHU0.T3 @(C);                 // PC = 0x01f
 DOUBLE T0 (S)         ->    SHU0.T3 @(!C);                 // PC = 0x020
    
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
 DOUBLE T0   (S)           ->    SHU1.T1 ;                 // PC = 0x02e
    //Cond=1, (C) & (!C)
 T0 == T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
    NOP; 
 DOUBLE T0 (S)         ->    SHU1.T2 @(!C);                 // PC = 0x034
 DOUBLE T0 (S)          ->    SHU1.T2 @(C);                 // PC = 0x035
    //Cond=0, (C) & (!C)
 T0 != T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
    NOP; 
 DOUBLE T0 (S)          ->    SHU1.T3 @(C);                 // PC = 0x03b
 DOUBLE T0 (S)         ->    SHU1.T3 @(!C);                 // PC = 0x03c
    
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
 DOUBLE T0   (S)           ->    IALU.T1 ;                 // PC = 0x04a
    //Cond=1, (C) & (!C)
 T0 == T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
    NOP; 
 DOUBLE T0 (S)         ->    IALU.T2 @(!C);                 // PC = 0x050
 DOUBLE T0 (S)          ->    IALU.T2 @(C);                 // PC = 0x051
    //Cond=0, (C) & (!C)
 T0 != T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
    NOP; 
 DOUBLE T0 (S)          ->    IALU.T3 @(C);                 // PC = 0x057
 DOUBLE T0 (S)         ->    IALU.T3 @(!C);                 // PC = 0x058
    
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
 DOUBLE T0   (S)           ->    IMAC.T1 ;                 // PC = 0x066
    //Cond=1, (C) & (!C)
 T0 == T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
    NOP; 
 DOUBLE T0 (S)         ->    IMAC.T2 @(!C);                 // PC = 0x06c
 DOUBLE T0 (S)          ->    IMAC.T2 @(C);                 // PC = 0x06d
    //Cond=0, (C) & (!C)
 T0 != T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
    NOP; 
 DOUBLE T0 (S)          ->    IMAC.T3 @(C);                 // PC = 0x073
 DOUBLE T0 (S)         ->    IMAC.T3 @(!C);                 // PC = 0x074
    
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
 DOUBLE T0   (S)           ->    FALU.T1 ;                 // PC = 0x082
    //Cond=1, (C) & (!C)
 T0 == T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
    NOP; 
 DOUBLE T0 (S)         ->    FALU.T2 @(!C);                 // PC = 0x088
 DOUBLE T0 (S)          ->    FALU.T2 @(C);                 // PC = 0x089
    //Cond=0, (C) & (!C)
 T0 != T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
    NOP; 
 DOUBLE T0 (S)          ->    FALU.T3 @(C);                 // PC = 0x08f
 DOUBLE T0 (S)         ->    FALU.T3 @(!C);                 // PC = 0x090
    
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
 DOUBLE T0   (S)           ->    FMAC.T1 ;                 // PC = 0x09e
    //Cond=1, (C) & (!C)
 T0 == T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
    NOP; 
 DOUBLE T0 (S)         ->    FMAC.T2 @(!C);                 // PC = 0x0a4
 DOUBLE T0 (S)          ->    FMAC.T2 @(C);                 // PC = 0x0a5
    //Cond=0, (C) & (!C)
 T0 != T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
    NOP; 
 DOUBLE T0 (S)          ->    FMAC.T3 @(C);                 // PC = 0x0ab
 DOUBLE T0 (S)         ->    FMAC.T3 @(!C);                 // PC = 0x0ac
    
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
 r0.M[1]->    FALU.T1 (I0) ; 
    NOP; 
 DOUBLE T0   (S)           ->    BIU0 ;                 // PC = 0x0ba
    //Cond=1, (C) & (!C)
 T0 == T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
    NOP; 
 DOUBLE T1 (S)         ->    BIU0 @(!C);                 // PC = 0x0c0
 DOUBLE T1 (S)          ->    BIU0 @(C);                 // PC = 0x0c1
    //Cond=0, (C) & (!C)
 T0 != T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
    NOP; 
 DOUBLE T0 (S)          ->    BIU0 @(C);                 // PC = 0x0c7
 DOUBLE T0 (S)         ->    BIU0 @(!C);                 // PC = 0x0c8
    
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
 r0.M[1]->    FALU.T1 (I0) ; 
    NOP; 
 DOUBLE T0   (S)           ->    BIU1 ;                 // PC = 0x0d6
    //Cond=1, (C) & (!C)
 T0 == T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
    NOP; 
 DOUBLE T1 (S)         ->    BIU1 @(!C);                 // PC = 0x0dc
 DOUBLE T1 (S)          ->    BIU1 @(C);                 // PC = 0x0dd
    //Cond=0, (C) & (!C)
 T0 != T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
    NOP; 
 DOUBLE T0 (S)          ->    BIU1 @(C);                 // PC = 0x0e3
 DOUBLE T0 (S)         ->    BIU1 @(!C);                 // PC = 0x0e4
    
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
 r0.M[1]->    FALU.T1 (I0) ; 
    NOP; 
 DOUBLE T0   (S)           ->    BIU2 ;                 // PC = 0x0f2
    //Cond=1, (C) & (!C)
 T0 == T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
    NOP; 
 DOUBLE T1 (S)         ->    BIU2 @(!C);                 // PC = 0x0f8
 DOUBLE T1 (S)          ->    BIU2 @(C);                 // PC = 0x0f9
    //Cond=0, (C) & (!C)
 T0 != T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
    NOP; 
 DOUBLE T0 (S)          ->    BIU2 @(C);                 // PC = 0x0ff
 DOUBLE T0 (S)         ->    BIU2 @(!C);                 // PC = 0x100
    
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
 r0.M[1]->    FALU.T1 (I0) ; 
    NOP; 
 DOUBLE T0   (S)           ->    M[10] ;                 // PC = 0x10e
    //Cond=1, (C) & (!C)
 T0 == T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
    NOP; 
 DOUBLE T1 (S)         ->    M[10] @(!C);                 // PC = 0x114
 DOUBLE T1 (S)          ->    M[10] @(C);                 // PC = 0x115
    //Cond=0, (C) & (!C)
 T0 != T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
    NOP; 
 DOUBLE T0 (S)          ->    M[10] @(C);                 // PC = 0x11b
 DOUBLE T0 (S)         ->    M[10] @(!C);                 // PC = 0x11c
    
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    MPUSTOP; 






