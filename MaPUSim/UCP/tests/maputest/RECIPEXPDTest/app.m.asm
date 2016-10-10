//  Date: 2014-02-20
//  Name: app.m.asm
//  Author: yanglp@zklc.com
//  The file to test the function of IALU 

.text
    // Write to M itself
    .global _IALUTest
    
_start:
    NOP;
    NOP;

_IALUTest:
    NOP;
    NOP;
    NOP;
    NOP;
    //Initial the IALU.Ts
    SHU0.TSQ IND TSQ -> IALU.T0 (I0);                // PC = 0x006
    NOP;
    //Compress --> Expand  (16 | 32)
    EXPD T0 (H) -> IALU.T0;                  // PC = 0x008
    NOP;
    NOP;
    NOP;
    NOP;
    CPRS(T0,T1) -> IALU.T0       ;                  // PC = 0x00d
    NOP;
    NOP;
    NOP;
    NOP;
    //Compress --> Expand  (8 | 16)
    EXPD T0 (B) -> IALU.T0     ;                  // PC = 0x012
    NOP;
    NOP;
    NOP;
    NOP;
    CPRS(T0,T1)  (H)-> IALU.T0  ;                  // PC = 0x017
    NOP;
    
    NOP;
    NOP;
    NOP;
    MPUStop;



