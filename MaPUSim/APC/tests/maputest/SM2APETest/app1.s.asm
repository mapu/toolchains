// by zhangx
    .text
    .global _start

_start:
    J0 = 0x40400000;;
    J1 = 0x0;;
    R0 = 0x0;;
    R1 = 0x1;;
    NOP;;//[J0 + J1] = R0;;  // initialize the memory
    J2 = 0x40400000;;
    J3 = 0x10000;;
    NOP;;//[J2 + J3] = R0;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;//[J0 + J1] = R1;; 
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;

   R8 = 0x0;;
_label0:
// judge whether [J0 + J1] is locked.
    R0 = 0x0;;
    R2 = [J0 + J1] (AT);; 
    R3 = R2 == R0;;
    if R3, JUMP _label0;; 
// judge whether [J0 + J1] is written 0, to protect the lock.
    R12 = 0x0;;
    [J0 + J1] = R12 (AT);;
    R4 =  R12 == R0;;
    if R4, JUMP _label0;;
// add the [J2 + J3]  with 1.
    R5 = [J2 + J3] ;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    R6 = 0x1;;
    R7 = R5 + R6;;
    [J2 + J3] = R7;;
// set [J0 + J1] to 1, to free the lock
    [J0 + J1] = R6;;
// loop 100 times
    R9 = 0x64;;
    R8 = R8 + R6;;
    R10 = R8 < R9;;
    if R10, JUMP _label0;;



    NOP;;
    NOP;;
    NOP;;

    R31 = [J2 + J3];;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    SPUStop;;
