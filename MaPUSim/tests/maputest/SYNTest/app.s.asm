// by zhangx
    .text
    .global _start

_start:

//    CallM 0x100;;
    J1 = 0x20;;
    R0  = 0x0;;
    R1  = 0x1;;
    R2  = 0x2;;
    R3  = 0x3;;
    R4  = 0x4;;
    R5  = 0x5;;
    R6  = 0x6;;
    R7  = 0x7;;
    R8  = 0x8;;

    SVR0[24] = R1;;
   

    KB0 = R0;;
    KE0 = R1;;
    KS0 = R2;;
    KI0 = R3;;
    KM0 = R4;;
    KG0 = R5;;
    KL0 = R6;;
    KL1 = R7;;
    KL2 = R8;;


/**** SHU.T reg transfer ****/
  //SHU.T <-> SVR
    J0 = 0x400000;;
    J1 = 0x0;;
    R0 = 0x6;;
    [J0 + J1] = R0;;
    SVR1 = [J0 + J1];;
    SHU0.T0 = SVR1;;
    NOP;;
    SVR1 = SHU0.T0;;
    
  //SHU.T <-> R
    R1 = 0xF8;;
    SHU0.T0 = Vect R1;;
    NOP;;
    SVR1 = SHU0.T0;;
    SHU0.T1 = Vect R1(B);;
    NOP;;
    SVR1 = SHU0.T1;;
    SHU1.T0 = Vect R1(H);;
    NOP;;
    SVR1 = SHU1.T0;;
    SHU1.T1 = Vect R1;;
    NOP;;
    SVR1 = SHU1.T1;;

    R2 = SHU0.T0;;
    R3 = SHU0.T0(B);;
    R4 = SHU0.T0(B)(U);;
    R5 = SHU0.T0(H);;
    R6 = SHU0.T0(H)(U);;

    
  //SHU.T <-> DR
    R0 = 0x8;;
    R1 = 0x6;;
    SHU1.T2  = Vect DR0;;
    NOP;;
    DR1 = SHU1.T2;;


/**** SVR <-> R reg transfer ****/
  //SVRs[Rs] = Rm  {(B)|(H)|()}
    R0 = 0x0;;
    R1 = 0x1;;
    R2 = 0x3;;
    R3 = 0x1F8E6;;
    SVR0[R0] = R3(B);;
    SVR0[R1] = R3(H);;
    SVR0[R1] = R3;;
    R4 = SVR0[R1];;
    R5 = SVR0[R2](B);;
    R6 = SVR0[R2](B)(U);;
    R7 = SVR0[R2](H);;
    R8 = SVR0[R2](H)(U);;
  //SVRs[Imm6] = Rm  {(B)|(H)|()}
    R3 = 0x1F8E6;;
    SVR1[0] = R3(B);;
    SVR1[1] = R3(H);;
    SVR1[1] = R3;;
    R2 = SVR1[1];;
    R3 = SVR1[3](B);;
    R4 = SVR1[3](B)(U);;
    R5 = SVR1[3](H);;
    R6 = SVR1[3](H)(U);;

    R0 = 0x0;;
    R1 = 0xFF;;

    SVR1[0] = R0(B);;
    SVR1[1] = R0(B);;
    SVR1[2] = R0(B);;
    SVR1[3] = R0(B);;
    SVR1[4] = R0(B);;
    SVR1[5] = R0(B);;
    SVR1[6] = R0(B);;
    SVR1[7] = R0(B);;
    SVR1[8] = R0(B);;
    SVR1[9] = R0(B);;
    SVR1[0xa] = R0(B);;
    SVR1[0xb] = R0(B);;

    SHU0.T0 = SVR1;;

    SVR1[0] = R1(B);;
    SVR1[1] = R1(B);;
    SVR1[2] = R1(B);;
    SVR1[3] = R1(B);;
    SVR1[4] = R1(B);;
    SVR1[5] = R1(B);;
    SVR1[6] = R1(B);;
    SVR1[7] = R1(B);;
    SVR1[8] = R1(B);;
    SVR1[9] = R1(B);;
    SVR1[0xa] = R1(B);;
    SVR1[0xb] = R1(B);;

    SHU0.T1 = SVR1;;

    NOP;;
    NOP;;
    NOP;;



    SPUStop;;
