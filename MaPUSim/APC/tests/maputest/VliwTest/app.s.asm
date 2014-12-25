// by zhangx
    .text
    .global _start

_start:
    R1 = 0x1;;
    R2 = 0x2;;
    J1 = 0x10000;;
    J2 = 0x10;;
    NOP;;
    [J1 + J2] = R1;;
    R3 = [J1 + J2]; R4 = R1 + R2;;
    R5 = R3 + R1;;

    R1 = 0xFF;;
    J1 = 0x20;;
    J2 = 0x8;;
    [J1 + J2] = R1;; 
    R1 = [J1 + J2];;
    R4 = R3; R3 = R1 + R2;;
    J2 = 0x2;;
    R9 = 0x0;;
    IF R9, JUMP J1;R5 = 0xFFFFF;R3 = R1 + R2;;
    IF R9, JUMP J1;R5 = 0xa;R3 = R1 + R2;;
    IF R9, JUMP J1;R5 = 0xa;R3 = R1 + R2;;
    R5 = 8;;    
    R5 = 0xFFFFF;;
    R6 = 0x9;;
    R7 = R5;;




    R2 = 246;;
    SPUStop;;

