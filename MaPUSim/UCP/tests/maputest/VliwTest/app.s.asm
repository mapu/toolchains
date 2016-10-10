// by zhangx
    .text
    .global _start

_start:
 m.s     R1 = 0x1;;
 m.s     R2 = 0x2;;
 m.s     J1 = 0x10000;;
 m.s     J2 = 0x10;;
 m.s     NOP;;
 m.s     [J1 + J2] = R1;;
 m.s     R3 = [J1 + J2]; R4 = R1 + R2;;
 m.s     R5 = R3 + R1;;

 m.s     R1 = 0xFF;;
 m.s     J1 = 0x20;;
 m.s     J2 = 0x8;;
 m.s     [J1 + J2] = R1;; 
 m.s     R1 = [J1 + J2];;
 m.s     R4 = R3; R3 = R1 + R2;;
 m.s     J2 = 0x2;;
 m.s     R9 = 0x0;;
 m.s     IF R9, JUMP J1;R5 = 0xFFFFF;R3 = R1 + R2;;
 m.s     IF R9, JUMP J1;R5 = 0xa;R3 = R1 + R2;;
 m.s     IF R9, JUMP J1;R5 = 0xa;R3 = R1 + R2;;
 m.s     R5 = 8;;    
 m.s     R5 = 0xFFFFF;;
 m.s     R6 = 0x9;;
 m.s     R7 = R5;;




 m.s     R2 = 246;;
 m.s     SPUStop;;

