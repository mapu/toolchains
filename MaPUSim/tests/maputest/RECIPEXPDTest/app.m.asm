//  Date: 2014-02-20
//  Name: app.m.asm
//  Author: yanglp@zklc.com
//  The file to test the function of IALU 

.text
    // Write to M itself
    .global _IALUTest
    
_start:
    NOP;;
    NOP;;

_IALUTest:
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    //Initial the IALU.Ts
    IALU.T0 (I0) = SHU0.TSQ IND TSQ;;                // PC = 0x006
    NOP;;
    //Compress --> Expand  (16 | 32)
    IALU.T0:1 = EXPD T0 (H)      ;;                  // PC = 0x008
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    IALU.T0 = CPRS(T0,T1)        ;;                  // PC = 0x00d
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    //Compress --> Expand  (8 | 16)
    IALU.T0:1 = EXPD T0 (B)      ;;                  // PC = 0x012
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    IALU.T0 = CPRS(T0,T1)  (H)   ;;                  // PC = 0x017
    NOP;;
    
    NOP;;
    NOP;;
    NOP;;
    MPU.Stop;;



