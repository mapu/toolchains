// by zhangx
    .text
    .global _start

_start:

// m.s     CallM 0x100;;
 m.s     J1 = 0x20;;
 m.s     R0  = 0x0;;
 m.s     R1  = 0x1;;
 m.s     R2  = 0x2;;
 m.s     R3  = 0x3;;
 m.s     R4  = 0x4;;
 m.s     R5  = 0x5;;
 m.s     R6  = 0x6;;
 m.s     R7  = 0x7;;
 m.s     R8  = 0x8;;

 m.s     SVR0[24] = R1;;
   

 m.s     KB0 = R0;;
 m.s     KE0 = R1;;
 m.s     KS0 = R2;;
 m.s     KI0 = R3;;
 m.s     KM0 = R4;;
 m.s     KG0 = R5;;
 m.s     KL0 = R6;;
 m.s     KL1 = R7;;
 m.s     KL2 = R8;;


/**** SHU.T reg transfer ****/
  //SHU.T <-> SVR
 m.s     J0 = 0x400000;;
 m.s     J1 = 0x0;;
 m.s     R0 = 0x6;;
 m.s     [J0 + J1] = R0;;
 m.s     SVR1 = [J0 + J1];;
 m.s     SHU0.T0 = SVR1;;
 m.s     NOP;;
 m.s     SVR1 = SHU0.T0;;
    
  //SHU.T <-> R
 m.s     R1 = 0xF8;;
 m.s     SHU0.T0 = Vect R1;;
 m.s     NOP;;
 m.s     SVR1 = SHU0.T0;;
 m.s     SHU0.T1 = Vect R1(B);;
 m.s     NOP;;
 m.s     SVR1 = SHU0.T1;;
 m.s     SHU1.T0 = Vect R1(H);;
 m.s     NOP;;
 m.s     SVR1 = SHU1.T0;;
 m.s     SHU1.T1 = Vect R1;;
 m.s     NOP;;
 m.s     SVR1 = SHU1.T1;;

 m.s     R2 = SHU0.T0;;
 m.s     R3 = SHU0.T0(B);;
 m.s     R4 = SHU0.T0(B)(U);;
 m.s     R5 = SHU0.T0(H);;
 m.s     R6 = SHU0.T0(H)(U);;

    
  //SHU.T <-> DR
 m.s     R0 = 0x8;;
 m.s     R1 = 0x6;;
 m.s     SHU1.T2  = Vect DR0;;
 m.s     NOP;;
 m.s     DR1 = SHU1.T2;;


/**** SVR <-> R reg transfer ****/
  //SVRs[Rs] = Rm  {(B)|(H)|()}
 m.s     R0 = 0x0;;
 m.s     R1 = 0x1;;
 m.s     R2 = 0x3;;
 m.s     R3 = 0x1F8E6;;
 m.s     SVR0[R0] = R3(B);;
 m.s     SVR0[R1] = R3(H);;
 m.s     SVR0[R1] = R3;;
 m.s     R4 = SVR0[R1];;
 m.s     R5 = SVR0[R2](B);;
 m.s     R6 = SVR0[R2](B)(U);;
 m.s     R7 = SVR0[R2](H);;
 m.s     R8 = SVR0[R2](H)(U);;
  //SVRs[Imm6] = Rm  {(B)|(H)|()}
 m.s     R3 = 0x1F8E6;;
 m.s     SVR1[0] = R3(B);;
 m.s     SVR1[1] = R3(H);;
 m.s     SVR1[1] = R3;;
 m.s     R2 = SVR1[1];;
 m.s     R3 = SVR1[3](B);;
 m.s     R4 = SVR1[3](B)(U);;
 m.s     R5 = SVR1[3](H);;
 m.s     R6 = SVR1[3](H)(U);;

 m.s     R0 = 0x0;;
 m.s     R1 = 0xFF;;

 m.s     SVR1[0] = R0(B);;
 m.s     SVR1[1] = R0(B);;
 m.s     SVR1[2] = R0(B);;
 m.s     SVR1[3] = R0(B);;
 m.s     SVR1[4] = R0(B);;
 m.s     SVR1[5] = R0(B);;
 m.s     SVR1[6] = R0(B);;
 m.s     SVR1[7] = R0(B);;
 m.s     SVR1[8] = R0(B);;
 m.s     SVR1[9] = R0(B);;
 m.s     SVR1[0xa] = R0(B);;
 m.s     SVR1[0xb] = R0(B);;

 m.s     SHU0.T0 = SVR1;;

 m.s     SVR1[0] = R1(B);;
 m.s     SVR1[1] = R1(B);;
 m.s     SVR1[2] = R1(B);;
 m.s     SVR1[3] = R1(B);;
 m.s     SVR1[4] = R1(B);;
 m.s     SVR1[5] = R1(B);;
 m.s     SVR1[6] = R1(B);;
 m.s     SVR1[7] = R1(B);;
 m.s     SVR1[8] = R1(B);;
 m.s     SVR1[9] = R1(B);;
 m.s     SVR1[0xa] = R1(B);;
 m.s     SVR1[0xb] = R1(B);;

 m.s     SHU0.T1 = SVR1;;

 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;



 m.s     SPUStop;;
