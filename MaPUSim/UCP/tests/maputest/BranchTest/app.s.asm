// by zhangx
    .text
    .global _start

_start:
 m.s     R1 = 0x1;;
 m.s     R2 = 0x2;;
 m.s     JUMP 0x8;;                             //JUMP IMM
 m.s     R1 = 0x5;;
 m.s     R2 = 0x6;;
 m.s     R3 = R1 + R2;;

 m.s     if R1, JUMP 0x10;;   //18            //IF  JUMP IMM
 m.s     J5 = 0x5;;
 m.s     J6 = 0x6;;
 m.s     J7 = 0x7;;
 m.s     J8 = 0x8;;
 m.s     J9 = 0x9;;

 m.s     R1 = 0;;
 m.s     if R1, JUMP 0x10;;  //34                 //IF  JUMP IMM, not jump
 m.s     R5 = 0x5;;
 m.s     R6 = 0x6;;
 m.s     R7 = 0x7;;
 m.s     R8 = 0x8;;
 m.s     R9 = 0x9;;

    // m.s JUMP -8;;

 m.s     R1 = 0x1;;
 m.s     R2 = 0x2;;
 m.s     J1 = 0x68;;   
 m.s     JUMP J1;;  //5c                      //JUMP Jm
 m.s     R3 = 0x3;;
 m.s     R4 = 0x4;;
 m.s     R5 = 0x5;;
 m.s     R6 = 0x6;;
    
 m.s     J30 = 0x88;;
 m.s     if R1, JUMP J30;; //0x78               // IF JUMP Jm
 m.s     R5 = 0x5;;
 m.s     R6 = 0x6;;
 m.s     R7 = 0x7;;
 m.s     R8 = 0x8;;
 m.s     R9 = 0x9;;

 m.s     R1 = 0x0;;
 m.s     J1 = 0xa0;;
 m.s     if R1, JUMP J1;; //0x98                   // IF JUMP Jm    no jump
 m.s     R2 = 0x2;;
 m.s     R3 = 0x3;; 
 m.s     R31 = R1== R2;;

 m.s     JUMP   _start2;;       //_start2
_start1:   // D2
 m.s     R1 = 0x1;;
 m.s     R2 = 0x2 ;;
_start2:   // D1
 m.s     R5 = 0x5;;
 m.s     R6 = 0x6;;
 m.s     R7 = 0x7;;
 m.s     R8 = 0x8;;
 m.s     R9 = 0x9;;  //100c8

 m.s     JUMP 0x2000;; //c0              //Jump Imm Extend
    .rept 0x800   //2000
 m.s     NOP;;
    .endr
 m.s     R5 = 0x5;;    //300b0
 m.s     R6 = 0x6;;
 m.s     R7 = 0x7;;
 m.s     R8 = 0x8;;
 m.s     R9 = 0x9;;
 m.s     R10 = 0x5;;
 m.s     R11 = 0x6;;
 m.s     R12 = 0x7;;
 m.s     R13 = 0x8;;
 m.s     R14 = 0x9;;  //300f8

 m.s     R1 = 0x1;;
 m.s     if R1,JUMP 0x12000;; //20f0              //Jump Imm Extend
    .rept 0x4800   //2000
 m.s     NOP;;
    .endr
 m.s     R5 = 0x5;;    //
 m.s     R6 = 0x6;;
 m.s     R7 = 0x7;;
 m.s     R8 = 0x8;;
 m.s     R9 = 0x9;;
 m.s     R10 = 0x5;;
 m.s     R11 = 0x6;;
 m.s     R12 = 0x7;;
 m.s     R13 = 0x8;;
 m.s     R14 = 0x9;;  //

 m.s     R1 = 0x0;;
 m.s     if R1, JUMP 0x800;; //0x30100          //IF Jump Imm Extend
    .rept 0x200
 m.s     NOP;;
    .endr
 m.s     R5 = 0x5;;    
 m.s     R6 = 0x6;;
 m.s     R7 = 0x7;;
 m.s     R8 = 0x8;;
 m.s     R9 = 0x9;;
 m.s     R10 = 0x5;;
 m.s     R11 = 0x6;;
 m.s     R12 = 0x7;;
 m.s     R13 = 0x8;;
 m.s     R14 = 0x9;;  //0x50164


 m.s     R2 = 246;;
 m.s     SPUStop;;
