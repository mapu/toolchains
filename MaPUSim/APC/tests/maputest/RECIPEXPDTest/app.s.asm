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
 m.s     NOP;;
 m.s     NOP;;
    
 m.s     CallM _IALUTest(B);;    
 m.s     NOP;;
 m.s     R0 = 0x2;;
 m.s     R1 = 0x3;;
 m.s     R2 = R1 + R2;;
    
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     SPU.Stop ;;


