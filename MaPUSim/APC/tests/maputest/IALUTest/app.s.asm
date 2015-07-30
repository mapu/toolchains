//  Date: 2013-03-25
//  Name: app.s.asm
//  Author: tao.wang@ia.ac.cn
//  The configuration Unit for MPU

// begin program code */
    .text
    .global _start

//*************************************************/ 
_start:
/**** Call ****/
 m.s     NOP;;
// X Cluster
    // m.s R0 = 0x0       ;;  //   Cluster    
    // m.s R0 = 0x3       ;;  // X   Cluster
    // m.s R0 = 0x5       ;;  // Y   Cluster
    // m.s R0 = 0x7       ;;  // X-Y Cluster
   
    // m.s CCtrl = R0     ;;

 m.s     R1 = 0xffffff01;;
 m.s     R2 = 0xffffff02;;
 m.s     R3 = 0xffffff03;;
 m.s     R4 = 0xffffff04;;
 m.s     R5 = 0xffffff05;;
 m.s     R6 = 0xffffff06;;
 m.s     R7 = 0xffffff07;;
 m.s     R8 = 0xffffff08;;
 m.s     R9 = 0xffffff09;;
 m.s     R10= 0xffffff0a;;
 m.s     R11= 0xffffff0b;;
 m.s     R12= 0xffffff0c;;
 m.s     R13= 0xffffff0d;;
 m.s     R14= 0xffffff0e;;
 m.s     R15= 0xffffff0f;;
 m.s     R16= 0xffffff10;;
 m.s     R17= 0x11111111;;
 m.s     R18= 0x2       ;;
 m.s     J0 = 0x400000  ;;
 m.s     J1 = 0x4       ;;
 m.s     J2 = 0x400000  ;;
 m.s     J3 = 0x4       ;;
 m.s     [J0+=J1] = R1  ;;
 m.s     [J0+=J1] = R2  ;;
 m.s     [J0+=J1] = R3  ;;
 m.s     [J0+=J1] = R4  ;;
 m.s     [J0+=J1] = R5  ;;
 m.s     [J0+=J1] = R6  ;;
 m.s     [J0+=J1] = R7  ;;
 m.s     [J0+=J1] = R8  ;;
 m.s     [J0+=J1] = R9  ;;
 m.s     [J0+=J1] = R10 ;;
 m.s     [J0+=J1] = R11 ;;
 m.s     [J0+=J1] = R12 ;;
 m.s     [J0+=J1] = R13 ;;
 m.s     [J0+=J1] = R14 ;;
 m.s     [J0+=J1] = R15 ;;
 m.s     [J0+=J1] = R16 ;;    
 m.s     SVR1 = [J2+J3] ;;
 m.s     SHU0.T1 = SVR1 ;;
 m.s     SHU0.T2 = VECT R17 ;;
 m.s     SHU0.T0 = VECT R18 ;;

 m.s     R1 = 0x03020100;;
 m.s     R2 = 0x07060504;;
 m.s     R3 = 0x0b0a0908;;
 m.s     R4 = 0x0f0e0d0c;;
 m.s     R5 = 0x13121110;;
 m.s     R6 = 0x17161514;;
 m.s     R7 = 0x1b1a1918;;
 m.s     R8 = 0x1f1e1d1c;;
 m.s     R9 = 0x23222120;;
 m.s     R10= 0x27262524;;
 m.s     R11= 0x2b2a2928;;
 m.s     R12= 0x2f2e2d2c;;
 m.s     R13= 0x33323130;;
 m.s     R14= 0x37363534;;
 m.s     R15= 0x3b3a3938;;
 m.s     R16= 0x3f3e3d3c;;    
 m.s     J0 = 0x400000  ;;
 m.s     J1 = 0x4       ;;
 m.s     J2 = 0x400000  ;;
 m.s     J3 = 0x4       ;;
 m.s     [J0+=J1] = R1  ;;
 m.s     [J0+=J1] = R2  ;;
 m.s     [J0+=J1] = R3  ;;
 m.s     [J0+=J1] = R4  ;;
 m.s     [J0+=J1] = R5  ;;
 m.s     [J0+=J1] = R6  ;;
 m.s     [J0+=J1] = R7  ;;
 m.s     [J0+=J1] = R8  ;;
 m.s     [J0+=J1] = R9  ;;
 m.s     [J0+=J1] = R10 ;;
 m.s     [J0+=J1] = R11 ;;
 m.s     [J0+=J1] = R12 ;;
 m.s     [J0+=J1] = R13 ;;
 m.s     [J0+=J1] = R14 ;;
 m.s     [J0+=J1] = R15 ;;
 m.s     [J0+=J1] = R16 ;;
 m.s     SVR1 = [J2+J3] ;;
 m.s     SHU0.T3 = SVR1 ;;
 
 m.s     R8 = 16;;
 m.s     CH[2] = R8;;
 m.s     CallM _IALUTest;;    // X Cluster
 m.s     CH[1] = R8;;
 m.s     R0 = CH[0];;
 m.s     CH[2] = R8;;
 m.s     NOP;;



 m.s     R0 = 0x2;;
 m.s     R1 = 0x3;;
 m.s     R2 = R1 + R2;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     SPU.Stop ;;

/**** Stat & Ctrl reg transfer ****/


