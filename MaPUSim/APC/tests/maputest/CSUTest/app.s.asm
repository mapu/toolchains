// by zhangx
    .text
    .global _start

_start:

    R4 = 0x0;;  // local addr
    R5 = 0x4;;   // x num
    R6 = 0x4;;    // y step
    R7 = 0x10;;   //y num
    R8 = 0x40;;   //z step
    R9 = 0x40;;   // y all num
    R10 = 0x4000;; //global addr
    R11 = 0x4;;   //global x num
    R12 = 0x4;;    //global y step
    R13 = 0x10;;   //global y num
    R14 = 0x40;;   //global z step
    R15 = 0x40;;   //global y all num
    R16 = 0x5;;   //dma group
    R17 = 0x1;;   //cmd message port


    CH[0x4] = R4;;
    CH[0x5] = R5;;
    CH[0x6] = R6;;
    CH[0x7] = R7;;
    CH[0x8] = R8;;
    CH[0x9] = R9;;
    CH[0xa] = R10;;
    CH[0xb] = R11;;
    CH[0xc] = R12;;
    CH[0xd] = R13;;
    CH[0xe] = R14;;
    CH[0xf] = R15;;
    CH[0x10] = R16;;
    CH[0x11] = R17;;



    NOP;;
    NOP;;
    NOP;;

    J0 = 0x4000;;
    J1 = 0x0;;
    R0 = [J0 + 0];;
    R1 = [J0 + 4];;
    SVR0 = [J0 + J1];;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
/*
    R18 = 0xFFFFFF0;;
    CH[0x12] = R18;;  //group mask write
    R19 = CH[0x13];;  //group mask read

    R25 = CH[0] (N);;  //0
    R21 = CH[0x15] (N);; // 0
    R20 = 0x2;;        //1
    CH[0x14] = R20;;  //1   group status update
    R22 = CH[0x15] (N);;  //1  group status
    R23 = CH[0x15] ;;     
    R24 = CH[0x15] (N);;  //0
    R26 = CH[0] (N);;     //1
    R27 = CH[0] ;;
    R2 = 0xFFFFFFFF;;
    CH[0x2] = R2;;
    R26 = CH[0] (N);;     //0


    R27 = CH[0x18] (N);;
    R20 = 0x2;;       
    CH[0x18] = R20;; 
    R28 = CH[0x18] (N);;

    
    R1 = 0xFF;;
    J1 = 0xFF;;
    R2 = 0x1;;
    J2 = 0x1;;
    CH[J2] = J1;;
    CH[0x2] = R2;;


    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
*/

    SPUStop;;
