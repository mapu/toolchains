//  Date: 2014-02-20
//  Name: app.s.asm
//  Author: yanglp@zklc.com
//  The configuration Unit for MPU

// begin program code */
    .text
    .global _start

//*************************************************/ 
_start:
/**** Call ****/
    NOP;;
    NOP;;
    
    CallM _IALUTest(B);;    
    NOP;;
    R0 = 0x2;;
    R1 = 0x3;;
    R2 = R1 + R2;;
    
    NOP;;
    NOP;;
    NOP;;
    SPU.Stop ;;


