// by zhangx
    .text
    .global _start

_start:

 m.s     R4 = 0x0;;  // local addr
 m.s     R5 = 0x4;;   // x num
 m.s     R6 = 0x4;;    // y step
 m.s     R7 = 0x10;;   //y num
 m.s     R8 = 0x40;;   //z step
 m.s     R9 = 0x40;;   // y all num
 m.s     R10 = 0x4000;; //global addr
 m.s     R11 = 0x4;;   //global x num
 m.s     R12 = 0x4;;    //global y step
 m.s     R13 = 0x10;;   //global y num
 m.s     R14 = 0x40;;   //global z step
 m.s     R15 = 0x40;;   //global y all num
 m.s     R16 = 0x5;;   //dma group
 m.s     R17 = 0x1;;   //cmd message port


 m.s     CH[0x4] = R4;;
 m.s     CH[0x5] = R5;;
 m.s     CH[0x6] = R6;;
 m.s     CH[0x7] = R7;;
 m.s     CH[0x8] = R8;;
 m.s     CH[0x9] = R9;;
 m.s     CH[0xa] = R10;;
 m.s     CH[0xb] = R11;;
 m.s     CH[0xc] = R12;;
 m.s     CH[0xd] = R13;;
 m.s     CH[0xe] = R14;;
 m.s     CH[0xf] = R15;;
 m.s     CH[0x10] = R16;;
 m.s     CH[0x11] = R17;;



 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;

 m.s     J0 = 0x4000;;
 m.s     J1 = 0x0;;
 m.s     R0 = [J0 + 0];;
 m.s     R1 = [J0 + 4];;
 m.s     SVR0 = [J0 + J1];;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
/*
 m.s     R18 = 0xFFFFFF0;;
 m.s     CH[0x12] = R18;;  //group mask write
 m.s     R19 = CH[0x13];;  //group mask read

 m.s     R25 = CH[0] (N);;  //0
 m.s     R21 = CH[0x15] (N);; // 0
 m.s     R20 = 0x2;;        //1
 m.s     CH[0x14] = R20;;  //1   group status update
 m.s     R22 = CH[0x15] (N);;  //1  group status
 m.s     R23 = CH[0x15] ;;     
 m.s     R24 = CH[0x15] (N);;  //0
 m.s     R26 = CH[0] (N);;     //1
 m.s     R27 = CH[0] ;;
 m.s     R2 = 0xFFFFFFFF;;
 m.s     CH[0x2] = R2;;
 m.s     R26 = CH[0] (N);;     //0


 m.s     R27 = CH[0x18] (N);;
 m.s     R20 = 0x2;;       
 m.s     CH[0x18] = R20;; 
 m.s     R28 = CH[0x18] (N);;

    
 m.s     R1 = 0xFF;;
 m.s     J1 = 0xFF;;
 m.s     R2 = 0x1;;
 m.s     J2 = 0x1;;
 m.s     CH[J2] = J1;;
 m.s     CH[0x2] = R2;;


 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
*/

 m.s     SPUStop;;
