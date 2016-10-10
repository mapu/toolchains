//  Date: 2013-08-20
//  Name: app.s.asm
//  Author: tao.wang@ia.ac.cn
//  The file to test the function of CI's Bypass

// begin program code */
    .text
//*************************************************/ 
_start:
 
   // R stall 
 m.s    R0 = 0x1 ;;
 m.s    R1 = R0  ;;                          //stall 2 cycles
 m.s    R2 = 0x2 ;;
 m.s    R3 = R1  ;;                          //stall 1 cycles
 m.s    R4 = R1 + R2 ;;                      //no stall
 m.s    R5 = R1 + R2 ;;                      //no stall
 m.s    R6 = R1 + R2 (CI);;                  //stall 2 cycles
 m.s    R7 = R1 - R2 ;;                      //no statll
 m.s    R8 = R1 - R2 (CI);;                  //stall 2 cycles
 m.s    R9 = R1 * R2     ;;                  //no stall
 m.s    R10= R1 * R2     ;;                  //no stall
 m.s    R12= R1 - R2 (CI);;                  //no stall
 m.s    R13= R1 + R2     ;; 
 m.s    R14= R1 - R2     ;;
 m.s    R15= R1 - R2 (CI);;                  //stall 2 cycles
 m.s    R16= R1          ;;                  //no stall
 m.s    R17= R1 + R2 (CI);;                  //stall 1 cycles

 m.s    R0 = 0xFFFFFFFF ;;
 m.s    R1 = 0x2        ;;
 m.s    R2 = 0x3        ;;
 m.s    R3 = 0x4        ;;
 m.s    R5 = R0 + R1 (U)(T) ;;
 m.s    R6 = R2 + R3 (CI)(U);;
 m.s    NOP             ;;
 m.s    NOP             ;;
 m.s    R7 = R0 + R1    ;;
 m.s    R8 = R2 + R3 (CI) ;;
 m.s    NOP             ;;
 m.s    NOP             ;;
 m.s    R9 = R0 + R1 (T) ;;
 m.s    R10= R2 + R3 (CI) ;;
 m.s    NOP             ;;
 m.s    NOP             ;;
 m.s    R11= R0 + R1 (U) ;;
 m.s    R12= R2 + R3 (CI) ;;

 m.s    NOP             ;;
 m.s    NOP             ;;
 m.s    R0 = 0x1        ;;
 m.s    R1 = 0x2        ;;
 m.s    R2 = 0x8        ;;
 m.s    R3 = 0x4        ;;
 m.s    R5 = R0 - R1 (U)(T) ;; //0xffffffff
 m.s    R6 = R2 - R3 (CI)(U);; //0x3
 m.s    NOP             ;;
 m.s    NOP             ;;
 m.s    R7 = R0 - R1    ;;    //0xffffffff
 m.s    R8 = R2 - R3 (CI) ;;  //0x3
 m.s    NOP             ;;
 m.s    NOP             ;;
 m.s    R9 = R0 - R1 (T) ;;   //0xffffffff
 m.s    R10= R2 - R3 (CI) ;;  //0x3
 m.s    NOP             ;;
 m.s    NOP             ;;
 m.s    R11= R0 - R1 (U) ;;   //0x0
 m.s    R12= R2 - R3 (CI) ;;  //0x3
 m.s    NOP               ;;
 m.s    NOP               ;;
 m.s    NOP               ;;
 m.s    NOP               ;;
 m.s    SPU.Stop          ;;

  




