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
 m.s     NOP;;
 m.s     R3 = 0x4000178;;
 m.s     MC.r0 = R3;;
 m.s     CallM _CondTest(B);;
 m.s     NOP;;
 m.s     R0 = 0x2;;
 m.s     R1 = 0x3;;
 m.s     R2 = R1 + R2;;
 m.s     NOP         ;;
 m.s     NOP         ;;
 m.s     NOP         ;;
 m.s     NOP         ;;
 m.s     SPU.Stop    ;;

/**** Stat & Ctrl reg transfer ****/


