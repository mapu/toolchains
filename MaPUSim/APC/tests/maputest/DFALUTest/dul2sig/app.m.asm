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
 r0.M[1]->    FALU.T1 (I0) ; 
    NOP; 
 SINGLE T0   (D)         ->    SHU0.T2 ;                 // PC = 0x013
 SINGLE T0   (D,T)     ->    SHU0.T2 ;                 // PC = 0x014
    //Cond=1, (C) & (!C)
 T0 == T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
    NOP; 
 SINGLE T0   (D)      ->    SHU0.T2 @(C);                 // PC = 0x01a
 SINGLE T0   (D,T) ->    SHU0.T2 @(!C);                 // PC = 0x01b
    //Cond=0, (C) & (!C)
 T0 != T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
    NOP; 
 SINGLE T0   (D,T) ->    SHU0.T2 @(!C);                 // PC = 0x021
 SINGLE T0   (D)      ->    SHU0.T2 @(C);                 // PC = 0x022
    
    
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
 r0.M[1]->    FALU.T1 (I0) ; 
    NOP; 
 SINGLE T0   (D)         ->    SHU1.T2 ;                 // PC = 0x030
 SINGLE T0   (D,T)     ->    SHU1.T2 ;                 // PC = 0x031
    //Cond=1, (C) & (!C)
 T0 == T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
    NOP; 
 SINGLE T0   (D)      ->    SHU1.T2 @(C);                 // PC = 0x037
 SINGLE T0   (D,T) ->    SHU1.T2 @(!C);                 // PC = 0x038
    //Cond=0, (C) & (!C)
 T0 != T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
    NOP; 
 SINGLE T0   (D,T) ->    SHU1.T2 @(!C);                 // PC = 0x03e
 SINGLE T0   (D)      ->    SHU1.T2 @(C);                 // PC = 0x03f
    
    
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
 r0.M[1]->    FALU.T1 (I0) ; 
    NOP; 
 SINGLE T0   (D)         ->    IALU.T2 ;                 // PC = 0x04d
 SINGLE T0   (D,T)     ->    IALU.T2 ;                 // PC = 0x04e
    //Cond=1, (C) & (!C)
 T0 == T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
    NOP; 
 SINGLE T0   (D)      ->    IALU.T2 @(C);                 // PC = 0x054
 SINGLE T0   (D,T) ->    IALU.T2 @(!C);                 // PC = 0x055
    //Cond=0, (C) & (!C)
 T0 != T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
    NOP; 
 SINGLE T0   (D,T) ->    IALU.T2 @(!C);                 // PC = 0x05b
 SINGLE T0   (D)      ->    IALU.T2 @(C);                 // PC = 0x05c
    
    
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
 r0.M[1]->    FALU.T1 (I0) ; 
    NOP; 
 SINGLE T0   (D)         ->    IMAC.T2 ;                 // PC = 0x06a
 SINGLE T0   (D,T)     ->    IMAC.T2 ;                 // PC = 0x06b
    //Cond=1, (C) & (!C)
 T0 == T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
    NOP; 
 SINGLE T0   (D)      ->    IMAC.T2 @(C);                 // PC = 0x071
 SINGLE T0   (D,T) ->    IMAC.T2 @(!C);                 // PC = 0x072
    //Cond=0, (C) & (!C)
 T0 != T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
    NOP; 
 SINGLE T0   (D,T) ->    IMAC.T2 @(!C);                 // PC = 0x078
 SINGLE T0   (D)      ->    IMAC.T2 @(C);                 // PC = 0x079
    
    
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
 r0.M[1]->    FALU.T1 (I0) ; 
    NOP; 
 SINGLE T0   (D)         ->    FALU.T2 ;                 // PC = 0x087
 SINGLE T0   (D,T)     ->    FALU.T2 ;                 // PC = 0x088
    //Cond=1, (C) & (!C)
 T0 == T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
    NOP; 
 SINGLE T0   (D)      ->    FALU.T2 @(C);                 // PC = 0x08e
 SINGLE T0   (D,T) ->    FALU.T2 @(!C);                 // PC = 0x08f
    //Cond=0, (C) & (!C)
 T0 != T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
    NOP; 
 SINGLE T0   (D,T) ->    FALU.T2 @(!C);                 // PC = 0x095
 SINGLE T0   (D)      ->    FALU.T2 @(C);                 // PC = 0x096
    
    
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
 r0.M[1]->    FALU.T1 (I0) ; 
    NOP; 
 SINGLE T0   (D)         ->    FMAC.T2 ;                 // PC = 0x0a4
 SINGLE T0   (D,T)     ->    FMAC.T2 ;                 // PC = 0x0a5
    //Cond=1, (C) & (!C)
 T0 == T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
    NOP; 
 SINGLE T0   (D)      ->    FMAC.T2 @(C);                 // PC = 0x0ab
 SINGLE T0   (D,T) ->    FMAC.T2 @(!C);                 // PC = 0x0ac
    //Cond=0, (C) & (!C)
 T0 != T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
    NOP; 
 SINGLE T0   (D,T) ->    FMAC.T2 @(!C);                 // PC = 0x0b2
 SINGLE T0   (D)      ->    FMAC.T2 @(C);                 // PC = 0x0b3
    
    
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
 SINGLE T0   (D)         ->    BIU0 ;                 // PC = 0x0c1
 SINGLE T0   (D,T)     ->    BIU0 ;                 // PC = 0x0c2
    //Cond=1, (C) & (!C)
 T0 == T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
    NOP; 
 SINGLE T0   (D)      ->    BIU0 @(C);                 // PC = 0x0c8
 SINGLE T0   (D,T) ->    BIU0 @(!C);                 // PC = 0x0c9
    //Cond=0, (C) & (!C)
 T0 != T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
    NOP; 
 SINGLE T0   (D,T) ->    BIU0 @(!C);                 // PC = 0x0cf
 SINGLE T0   (D)      ->    BIU0 @(C);                 // PC = 0x0d0
    
    
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
 SINGLE T0   (D)         ->    BIU1 ;                 // PC = 0x0de
 SINGLE T0   (D,T)     ->    BIU1 ;                 // PC = 0x0df
    //Cond=1, (C) & (!C)
 T0 == T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
    NOP; 
 SINGLE T0   (D)      ->    BIU1 @(C);                 // PC = 0x0e5
 SINGLE T0   (D,T) ->    BIU1 @(!C);                 // PC = 0x0e6
    //Cond=0, (C) & (!C)
 T0 != T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
    NOP; 
 SINGLE T0   (D,T) ->    BIU1 @(!C);                 // PC = 0x0ec
 SINGLE T0   (D)      ->    BIU1 @(C);                 // PC = 0x0ed
    
    
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
 SINGLE T0   (D)         ->    BIU2 ;                 // PC = 0x0fb
 SINGLE T0   (D,T)     ->    BIU2 ;                 // PC = 0x0fc
    //Cond=1, (C) & (!C)
 T0 == T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
    NOP; 
 SINGLE T0   (D)      ->    BIU2 @(C);                 // PC = 0x102
 SINGLE T0   (D,T) ->    BIU2 @(!C);                 // PC = 0x103
    //Cond=0, (C) & (!C)
 T0 != T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
    NOP; 
 SINGLE T0   (D,T) ->    BIU2 @(!C);                 // PC = 0x109
 SINGLE T0   (D)      ->    BIU2 @(C);                 // PC = 0x10a
    
    
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
 SINGLE T0   (D)         ->    M[91] ;                 // PC = 0x118
 SINGLE T0   (D,T)     ->    M[91] ;                 // PC = 0x119
    //Cond=1, (C) & (!C)
 T0 == T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
    NOP; 
 SINGLE T0   (D)      ->    M[91] @(C);                 // PC = 0x11f
 SINGLE T0   (D,T) ->    M[92] @(!C);                 // PC = 0x120
    //Cond=0, (C) & (!C)
 T0 != T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
    NOP; 
 SINGLE T0   (D,T) ->    M[91] @(!C);                 // PC = 0x126
 SINGLE T0   (D)      ->    M[92] @(C);                 // PC = 0x127
    
    
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    MPUSTOP; 

