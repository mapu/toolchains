// by zhangx
    .text
    .global _start

_start:


    R1 = 0x1;;
    R2 = 0x2;;
    CALL _label1; R3 = R1 + R2;;                             //CALL IMM

    R1 = 0x1;;
    if R1, CALL _label2;;                      //IF CALL
    J1 = 0x1;;
    J2 = 0x2;;
    J3 = 0x3;;

    R1 =0;;
    if R1, CALL _label3;;                      //IF CALL IMM  not
    J1 = 0x1;;
    J2 = 0x2;;
    J3 = 0x3;;

    J1 = 0x7c;;  
    CALL J1;;                                   //CALL J
    R10 = 0x10;;
    R11 = 0x11;;
    R12 = 0x12;;
    
    R1 = 1;;
    if R1, CALL J1;;                           //IF CALL J
    R13 = 0x13;;
    R14 = 0x14;;
    R15 = 0x15;;

    R1 = 0;;
    if R1, CALL J1;;                           //IF CALL J  not 
    R13 = 0x13;;
    R14 = 0x14;;
    R15 = 0x15;;


    R2 = 246;;
    SPUStop;;

_label1:
    R1 = 0x7;;
    R2 = 0x6;;
    R3 = 0x8;;
    JUMP J30;;


_label2:
    R4 = 0x4;;
    R5 = 0x5;;
    R6 = R4 + R5;;
    JUMP J30;;

_label3:
    R7 = 0x4;;
    R8 = 0x5;;
    R9 = R4 + R5;;
    JUMP J30;;

    CALL _label3;;
    R13 = 0x13;;  //0x200ac
    R14 = 0x14;;
    JUMP J30;;

