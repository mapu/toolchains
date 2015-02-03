// by zhangx
    .text
    .global _start

_start:
 m.s     J0 = 0x40400000;;
 m.s     J1 = 0x0;;
 m.s     R0 = 0x0;;
 m.s     R1 = 0x1;;
 m.s     NOP;;//[J0 + J1] = R0;;  // initialize the memory
 m.s     J2 = 0x40400000;;
 m.s     J3 = 0x10000;;
 m.s     NOP;;//[J2 + J3] = R0;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;//[J0 + J1] = R1;; 
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;

 m.s    R8 = 0x0;;
_label0:
// judge whether [J0 + J1] is locked.
 m.s     R0 = 0x0;;
 m.s     R2 = [J0 + J1] (AT);; 
 m.s     R3 = R2 == R0;;
 m.s     if R3, JUMP _label0;; 
// judge whether [J0 + J1] is written 0, to protect the lock.
 m.s     R12 = 0x0;;
 m.s     [J0 + J1] = R12 (AT);;
 m.s     R4 =  R12 == R0;;
 m.s     if R4, JUMP _label0;;
// add the [J2 + J3]  with 1.
 m.s     R5 = [J2 + J3] ;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     R6 = 0x1;;
 m.s     R7 = R5 + R6;;
 m.s     [J2 + J3] = R7;;
// set [J0 + J1] to 1, to free the lock
 m.s     [J0 + J1] = R6;;
// loop 100 times
 m.s     R9 = 0x64;;
 m.s     R8 = R8 + R6;;
 m.s     R10 = R8 < R9;;
 m.s     if R10, JUMP _label0;;



 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;

 m.s     R31 = [J2 + J3];;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     SPUStop;;
