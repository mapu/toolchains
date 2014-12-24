    .text
    .global _start


_start:

    NOP;;

   

    R1 = 0x40800000;;  // 0x4
    //R1 = 0x12153524;;
    R2 = 0xc0000000;;  // -2
    //R2 = 0xc0895e81;;
    //NearResult:32’hc0895e81;
    //TruncResult:32’hc0895e80;
    R3 = 0x2;;       
   
    SHU0.T1 = VECT R1 ;;
    SHU0.T2 = VECT R2 ;;
    SHU0.T0 = VECT R3 ;;

    R1 = 0x03020100;;
    R2 = 0x07060504;;
    R3 = 0x0b0a0908;;
    R4 = 0x0f0e0d0c;;
    R5 = 0x13121110;;
    R6 = 0x17161514;;
    R7 = 0x1b1a1918;;
    R8 = 0x1f1e1d1c;;
    R9 = 0x23222120;;
    R10= 0x27262524;;
    R11= 0x2b2a2928;;
    R12= 0x2f2e2d2c;;
    R13= 0x33323130;;
    R14= 0x37363534;;
    R15= 0x3b3a3938;;
    R16= 0x3f3e3d3c;;    
    J0 = 0x400000  ;;
    J1 = 0x4       ;;
    J2 = 0x400000  ;;
    J3 = 0x4       ;;
    [J0+=J1] = R1  ;;
    [J0+=J1] = R2  ;;
    [J0+=J1] = R3  ;;
    [J0+=J1] = R4  ;;
    [J0+=J1] = R5  ;;
    [J0+=J1] = R6  ;;
    [J0+=J1] = R7  ;;
    [J0+=J1] = R8  ;;
    [J0+=J1] = R9  ;;
    [J0+=J1] = R10 ;;
    [J0+=J1] = R11 ;;
    [J0+=J1] = R12 ;;
    [J0+=J1] = R13 ;;
    [J0+=J1] = R14 ;;
    [J0+=J1] = R15 ;;
    [J0+=J1] = R16 ;;
    SVR1 = [J2+J3] ;;
    SHU0.T3 = SVR1 ;;

    
    CallM _FALUTest(B);;    // X Cluster
    NOP;;



    R0 = 0x2;;
    R1 = 0x3;;
    R2 = R1 + R2;;

    NOP;;
    NOP;;
    NOP;;  
    NOP;;
    NOP;;

    SPUStop;;


