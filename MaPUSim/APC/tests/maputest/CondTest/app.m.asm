//  Date: 2013-6-18
//  Name: app.m.asm
//  Author: yinlz@zklc.com
//  The file to test the function of Cond Execution 

.text
    // Write to M itself
    .global _CondTest

_start:
    NOP; 
    NOP; 
_CondTest:
    // verification of No C Option is ignored

    // M to IALU
 r0.M[1]->    IALU.T0(I0) ;          // PC = 0x2
 r0.M[2]->    IALU.T1(I0) ;          // PC = 0x3
    NOP;                          // PC = 0x4

    // C option is invalid--!C option is valid
 T0 == T1->    Cond ;              // PC = 0x5
    NOP;                          // PC = 0x6
    NOP;                          // PC = 0x7
 r0.M[1]  ->    M[127] @(C);         // PC = 0x8
 r0.M[1]  ->    M[127] @(!C);        // PC = 0x9   
 r1.M[2] ->    SHU0.T0 @(C);          // PC = 0xa
 r1.M[2] ->    SHU0.T0 @(!C);         // PC = 0xb
 r2.M[3] ->    IMAC.T0 (I0)@(C);      // PC = 0xc
 r2.M[3] ->    IMAC.T0 (I0)@(!C);     // PC = 0xd
 M[4] ->    BIU0 @(C);             // PC = 0xe
 M[4] ->    BIU0 @(!C);            // PC = 0xf

 SHU0.T1 IND T2 ->    BIU0 @(C);    // PC = 0x10
 SHU0.T1 IND T2 ->    BIU0 @(!C);   // PC = 0x11
 SHU1.T1 IND T2 ->    BIU0 @(C);    // PC = 0x12
 SHU1.T1 IND T2 ->    BIU0 @(!C);   // PC = 0x13

 BIU0.DM ->    M[126] @(C);        // PC = 0x14
 BIU0.DM ->    M[126] @(!C);       // PC = 0x15
 BIU1.DM ->    M[125] @(C);        // PC = 0x16
 BIU1.DM ->    M[125] @(!C);       // PC = 0x17
 BIU2.DM ->    M[124] @(C);        // PC = 0x18
 BIU2.DM ->    M[124] @(!C);       // PC = 0x19

 T1 + T2 ->    IALU.T3 @(C);        // PC = 0x1a
 T1 + T2 ->    IALU.T3 @(!C);       // PC = 0x1b
 T1 * T2 (T)->    IMAC.T0 @(C);        // PC = 0x1c
 T1 * T2 (T)->    IMAC.T0 @(!C);       // PC = 0x1d
 T1 + T2 (S)->    FALU.T0 @(C);        // PC = 0x1e
 T1 + T2 (S)->    FALU.T0 @(!C);       // PC = 0x1f
 T1 * T2 (S)->    FMAC.T0 @(C);        // PC = 0x20


    // C option is valid--!C option is invalid
 T0 != T1->    Cond ;              // PC = 0x21
 T1 * T2 (S)->    FMAC.T0 @(!C);    // PC = 0x22
    NOP;                          // PC = 0x23
    NOP;                          // PC = 0x24
 r0.M[1]  ->    M[127] @(!C);         // PC = 0x25
 r0.M[1]  ->    M[127] @(!C);        // PC = 0x26
 r1.M[2] ->    SHU0.T0 @(C);          // PC = 0x27
 r1.M[2] ->    SHU0.T0 @(!C);         // PC = 0x28
 r2.M[3] ->    IMAC.T0 (I0)@(C);      // PC = 0x29
 r2.M[3] ->    IMAC.T0 (I0)@(!C);     // PC = 0x2a
 M[4] ->    BIU0 @(C);             // PC = 0x2b
 M[4] ->    BIU0 @(!C);            // PC = 0x2c
 SHU0.T1 IND T2 ->    BIU0 @(C);    // PC = 0x2d
 SHU0.T1 IND T2 ->    BIU0 @(!C);   // PC = 0x2e
 SHU1.T1 IND T2 ->    BIU0 @(C);    // PC = 0x2f
 SHU1.T1 IND T2 ->    BIU0 @(!C);   // PC = 0x30
 BIU0.DM ->    M[126] @(C);        // PC = 0x31
 BIU0.DM ->    M[126] @(!C);       // PC = 0x32
 BIU1.DM ->    M[125] @(C);        // PC = 0x33
 BIU1.DM ->    M[125] @(!C);       // PC = 0x34
 BIU2.DM ->    M[124] @(C);        // PC = 0x35
 BIU2.DM ->    M[124] @(!C);       // PC = 0x36
 T1 + T2 ->    IALU.T3 @(C);        // PC = 0x37
 T1 + T2 ->    IALU.T3 @(!C);       // PC = 0x38
 T1 * T2 (T)->    IMAC.T0 @(C);        // PC = 0x39
 T1 * T2 (T)->    IMAC.T0 @(!C);       // PC = 0x3a
 T1 + T2 (S)->    FALU.T0 @(C);     // PC = 0x3b
 T1 + T2 (S)->    FALU.T0 @(!C);    // PC = 0x3c
 T1 * T2 (S)->    FMAC.T0 @(C);     // PC = 0x3d
 T1 * T2 (S)->    FMAC.T0 @(!C);    // PC = 0x3e
    

    MPUSTOP; 


