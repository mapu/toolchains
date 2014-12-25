//  Date: 2013-6-18
//  Name: app.s.asm
//  Author: yinlz@zklc.com
//  The configuration Unit for MPU

// begin program code */
    .text
    .global _start

//*************************************************/ 
_start:
/**** Call ****/
    NOP;;
    R3 = 0x4000178;;
    MC.r0 = R3;;
    CallM _CondTest(B);;
    NOP;;
    R0 = 0x2;;
    R1 = 0x3;;
    R2 = R1 + R2;;
    NOP         ;;
    NOP         ;;
    NOP         ;;
    NOP         ;;
    SPU.Stop    ;;

/**** Stat & Ctrl reg transfer ****/


