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
 r0.M[1]->    FALU.T1 (I0) ; 
    NOP; 
 INT T0   (D)         ->    SHU0.T2 ;                 // PC = 0x012
 INT T0   (D,T)     ->    SHU0.T2 ;                 // PC = 0x013
    NOP; 
    //Cond=1, (C) & (!C)
 T0 == T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
 INT T0   (D)      ->    SHU0.T2 @(!C);                 // PC = 0x019
 INT T0   (D,T) ->    SHU0.T2 @(!C);                 // PC = 0x01a
    NOP; 
    //Cond=0, (C) & (!C)
 T0 != T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
 INT T0   (D,T) ->    SHU0.T2 @(C);                 // PC = 0x020
 INT T0   (D)      ->    SHU0.T2 @(C);                 // PC = 0x021
    
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
 INT T0   (D)         ->    SHU1.T2 ;                 // PC = 0x02f
 INT T0   (D,T)     ->    SHU1.T2 ;                 // PC = 0x030
    NOP; 
    //Cond=1, (C) & (!C)
 T0 == T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
 INT T0   (D)      ->    SHU1.T2 @(!C);                 // PC = 0x036
 INT T0   (D,T) ->    SHU1.T2 @(!C);                 // PC = 0x037
    NOP; 
    //Cond=0, (C) & (!C)
 T0 != T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
 INT T0   (D,T) ->    SHU1.T2 @(C);                 // PC = 0x03d
 INT T0   (D)      ->    SHU1.T2 @(C);                 // PC = 0x03e
    
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
 INT T0   (D)         ->    IALU.T2 ;                 // PC = 0x04c
 INT T0   (D,T)     ->    IALU.T2 ;                 // PC = 0x04d
    NOP; 
    //Cond=1, (C) & (!C)
 T0 == T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
 INT T0   (D)      ->    IALU.T2 @(!C);                 // PC = 0x053
 INT T0   (D,T) ->    IALU.T2 @(!C);                 // PC = 0x054
    NOP; 
    //Cond=0, (C) & (!C)
 T0 != T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
 INT T0   (D,T) ->    IALU.T2 @(C);                 // PC = 0x05a
 INT T0   (D)      ->    IALU.T2 @(C);                 // PC = 0x05b
    
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
 INT T0   (D)         ->    IMAC.T2 ;                 // PC = 0x069
 INT T0   (D,T)     ->    IMAC.T2 ;                 // PC = 0x06a
    NOP; 
    //Cond=1, (C) & (!C)
 T0 == T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
 INT T0   (D)      ->    IMAC.T2 @(!C);                 // PC = 0x070
 INT T0   (D,T) ->    IMAC.T2 @(!C);                 // PC = 0x071
    NOP; 
    //Cond=0, (C) & (!C)
 T0 != T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
 INT T0   (D,T) ->    IMAC.T2 @(C);                 // PC = 0x077
 INT T0   (D)      ->    IMAC.T2 @(C);                 // PC = 0x078
    
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
 INT T0   (D)         ->    FALU.T2 ;                 // PC = 0x086
 INT T0   (D,T)     ->    FALU.T2 ;                 // PC = 0x087
    NOP; 
    //Cond=1, (C) & (!C)
 T0 == T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
 INT T0   (D)      ->    FALU.T2 @(!C);                 // PC = 0x08d
 INT T0   (D,T) ->    FALU.T2 @(!C);                 // PC = 0x08e
    NOP; 
    //Cond=0, (C) & (!C)
 T0 != T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
 INT T0   (D,T) ->    FALU.T2 @(C);                 // PC = 0x094
 INT T0   (D)      ->    FALU.T2 @(C);                 // PC = 0x095
    
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
 INT T0   (D)         ->    FMAC.T2 ;                 // PC = 0x0a3
 INT T0   (D,T)     ->    FMAC.T2 ;                 // PC = 0x0a4
    NOP; 
    //Cond=1, (C) & (!C)
 T0 == T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
 INT T0   (D)      ->    FMAC.T2 @(!C);                 // PC = 0x0aa
 INT T0   (D,T) ->    FMAC.T2 @(!C);                 // PC = 0x0ab
    NOP; 
    //Cond=0, (C) & (!C)
 T0 != T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
 INT T0   (D,T) ->    FMAC.T2 @(C);                 // PC = 0x0b1
 INT T0   (D)      ->    FMAC.T2 @(C);                 // PC = 0x0b2
    
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
 INT T0   (D)         ->    BIU0 ;                 // PC = 0x0c0
 INT T0   (D,T)     ->    BIU0 ;                 // PC = 0x0c1
    NOP; 
    //Cond=1, (C) & (!C)
 T0 == T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
 INT T0   (D)      ->    BIU0 @(!C);                 // PC = 0x0c7
 INT T0   (D,T) ->    BIU0 @(!C);                 // PC = 0x0c8
    NOP; 
    //Cond=0, (C) & (!C)
 T0 != T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
 INT T0   (D,T) ->    BIU0 @(C);                 // PC = 0x0ce
 INT T0   (D)      ->    BIU0 @(C);                 // PC = 0x0cf
    
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
 INT T0   (D)         ->    BIU1 ;                 // PC = 0x0dd
 INT T0   (D,T)     ->    BIU1 ;                 // PC = 0x0de
    NOP; 
    //Cond=1, (C) & (!C)
 T0 == T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
 INT T0   (D)      ->    BIU1 @(!C);                 // PC = 0x0e4
 INT T0   (D,T) ->    BIU1 @(!C);                 // PC = 0x0e5
    NOP; 
    //Cond=0, (C) & (!C)
 T0 != T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
 INT T0   (D,T) ->    BIU1 @(C);                 // PC = 0x0eb
 INT T0   (D)      ->    BIU1 @(C);                 // PC = 0x0ec
    
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
 INT T0   (D)         ->    BIU2 ;                 // PC = 0x0fa
 INT T0   (D,T)     ->    BIU2 ;                 // PC = 0x0fb
    NOP; 
    //Cond=1, (C) & (!C)
 T0 == T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
 INT T0   (D)      ->    BIU2 @(!C);                 // PC = 0x101
 INT T0   (D,T) ->    BIU2 @(!C);                 // PC = 0x102
    NOP; 
    //Cond=0, (C) & (!C)
 T0 != T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
 INT T0   (D,T) ->    BIU2 @(C);                 // PC = 0x108
 INT T0   (D)      ->    BIU2 @(C);                 // PC = 0x109
    
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
 INT T0   (D)         ->    M[99] ;                 // PC = 0x117
 INT T0   (D,T)     ->    M[99] ;                 // PC = 0x118
    NOP; 
    //Cond=1, (C) & (!C)
 T0 == T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
 INT T0   (D)      ->    M[99] @(!C);                 // PC = 0x11e
 INT T0   (D,T) ->    M[99] @(!C);                 // PC = 0x11f
    NOP; 
    //Cond=0, (C) & (!C)
 T0 != T0->    Cond	;  
    NOP; 
    NOP; 
    NOP; 
 INT T0   (D,T) ->    M[99] @(C);                 // PC = 0x125
 INT T0   (D)      ->    M[99] @(C);                 // PC = 0x126
    
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    NOP; 
    MPUSTOP; 




