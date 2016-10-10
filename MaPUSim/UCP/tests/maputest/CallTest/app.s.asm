// by zhangx
    .text
    .global _start

_start:


 m.s     R1 = 0x1;;
 m.s     R2 = 0x2;;
 m.s     CALL _label1; R3 = R1 + R2;;                             //CALL IMM

 m.s     R1 = 0x1;;
 m.s     if R1, CALL _label2;;                      //IF CALL
 m.s     J1 = 0x1;;
 m.s     J2 = 0x2;;
 m.s     J3 = 0x3;;

 m.s     R1 =0;;
 m.s     if R1, CALL _label3;;                      //IF CALL IMM  not
 m.s     J1 = 0x1;;
 m.s     J2 = 0x2;;
 m.s     J3 = 0x3;;

 m.s     J1 = 0x7c;;  
 m.s     CALL J1;;                                   //CALL J
 m.s     R10 = 0x10;;
 m.s     R11 = 0x11;;
 m.s     R12 = 0x12;;
    
 m.s     R1 = 1;;
 m.s     if R1, CALL J1;;                           //IF CALL J
 m.s     R13 = 0x13;;
 m.s     R14 = 0x14;;
 m.s     R15 = 0x15;;

 m.s     R1 = 0;;
 m.s     if R1, CALL J1;;                           //IF CALL J  not 
 m.s     R13 = 0x13;;
 m.s     R14 = 0x14;;
 m.s     R15 = 0x15;;


 m.s     R2 = 246;;
 m.s     SPUStop;;

_label1:
 m.s     R1 = 0x7;;
 m.s     R2 = 0x6;;
 m.s     R3 = 0x8;;
 m.s     JUMP J30;;


_label2:
 m.s     R4 = 0x4;;
 m.s     R5 = 0x5;;
 m.s     R6 = R4 + R5;;
 m.s     JUMP J30;;

_label3:
 m.s     R7 = 0x4;;
 m.s     R8 = 0x5;;
 m.s     R9 = R4 + R5;;
 m.s     JUMP J30;;

 m.s     CALL _label3;;
 m.s     R13 = 0x13;;  //0x200ac
 m.s     R14 = 0x14;;
 m.s     JUMP J30;;

