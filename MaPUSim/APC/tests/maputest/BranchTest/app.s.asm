// by zhangx
    .text
    .global _start

_start:
    R1 = 0x1;;
    R2 = 0x2;;
    JUMP 0x8;;                             //JUMP IMM
    R1 = 0x5;;
    R2 = 0x6;;
    R3 = R1 + R2;;

    if R1, JUMP 0x10;;   //18            //IF  JUMP IMM
    J5 = 0x5;;
    J6 = 0x6;;
    J7 = 0x7;;
    J8 = 0x8;;
    J9 = 0x9;;

    R1 = 0;;
    if R1, JUMP 0x10;;  //34                 //IF  JUMP IMM, not jump
    R5 = 0x5;;
    R6 = 0x6;;
    R7 = 0x7;;
    R8 = 0x8;;
    R9 = 0x9;;

    //JUMP -8;;

    R1 = 0x1;;
    R2 = 0x2;;
    J1 = 0x68;;   
    JUMP J1;;  //5c                      //JUMP Jm
    R3 = 0x3;;
    R4 = 0x4;;
    R5 = 0x5;;
    R6 = 0x6;;
    
    J30 = 0x88;;
    if R1, JUMP J30;; //0x78               // IF JUMP Jm
    R5 = 0x5;;
    R6 = 0x6;;
    R7 = 0x7;;
    R8 = 0x8;;
    R9 = 0x9;;

    R1 = 0x0;;
    J1 = 0xa0;;
    if R1, JUMP J1;; //0x98                   // IF JUMP Jm    no jump
    R2 = 0x2;;
    R3 = 0x3;; 
    R31 = R1== R2;;

    JUMP   _start2;;       //_start2
_start1:   // D2
    R1 = 0x1;;
    R2 = 0x2 ;;
_start2:   // D1
    R5 = 0x5;;
    R6 = 0x6;;
    R7 = 0x7;;
    R8 = 0x8;;
    R9 = 0x9;;  //100c8

    JUMP 0x2000;; //c0              //Jump Imm Extend
    .rept 0x800   //2000
    NOP;;
    .endr
    R5 = 0x5;;    //300b0
    R6 = 0x6;;
    R7 = 0x7;;
    R8 = 0x8;;
    R9 = 0x9;;
    R10 = 0x5;;
    R11 = 0x6;;
    R12 = 0x7;;
    R13 = 0x8;;
    R14 = 0x9;;  //300f8

    R1 = 0x1;;
    if R1,JUMP 0x12000;; //20f0              //Jump Imm Extend
    .rept 0x4800   //2000
    NOP;;
    .endr
    R5 = 0x5;;    //
    R6 = 0x6;;
    R7 = 0x7;;
    R8 = 0x8;;
    R9 = 0x9;;
    R10 = 0x5;;
    R11 = 0x6;;
    R12 = 0x7;;
    R13 = 0x8;;
    R14 = 0x9;;  //

    R1 = 0x0;;
    if R1, JUMP 0x800;; //0x30100          //IF Jump Imm Extend
    .rept 0x200
    NOP;;
    .endr
    R5 = 0x5;;    
    R6 = 0x6;;
    R7 = 0x7;;
    R8 = 0x8;;
    R9 = 0x9;;
    R10 = 0x5;;
    R11 = 0x6;;
    R12 = 0x7;;
    R13 = 0x8;;
    R14 = 0x9;;  //0x50164


    R2 = 246;;
    SPUStop;;
