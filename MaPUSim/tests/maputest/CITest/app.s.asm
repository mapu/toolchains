//  Date: 2013-08-20
//  Name: app.s.asm
//  Author: tao.wang@ia.ac.cn
//  The file to test the function of CI's Bypass

// begin program code */
    .text
//*************************************************/ 
_start:
 
   // R stall 
   R0 = 0x1 ;;
   R1 = R0  ;;                          //stall 2 cycles
   R2 = 0x2 ;;
   R3 = R1  ;;                          //stall 1 cycles
   R4 = R1 + R2 ;;                      //no stall
   R5 = R1 + R2 ;;                      //no stall
   R6 = R1 + R2 (CI);;                  //stall 2 cycles
   R7 = R1 - R2 ;;                      //no statll
   R8 = R1 - R2 (CI);;                  //stall 2 cycles
   R9 = R1 * R2     ;;                  //no stall
   R10= R1 * R2     ;;                  //no stall
   R12= R1 - R2 (CI);;                  //no stall
   R13= R1 + R2     ;; 
   R14= R1 - R2     ;;
   R15= R1 - R2 (CI);;                  //stall 2 cycles
   R16= R1          ;;                  //no stall
   R17= R1 + R2 (CI);;                  //stall 1 cycles

   R0 = 0xFFFFFFFF ;;
   R1 = 0x2        ;;
   R2 = 0x3        ;;
   R3 = 0x4        ;;
   R5 = R0 + R1 (U)(T) ;;
   R6 = R2 + R3 (CI)(U);;
   NOP             ;;
   NOP             ;;
   R7 = R0 + R1    ;;
   R8 = R2 + R3 (CI) ;;
   NOP             ;;
   NOP             ;;
   R9 = R0 + R1 (T) ;;
   R10= R2 + R3 (CI) ;;
   NOP             ;;
   NOP             ;;
   R11= R0 + R1 (U) ;;
   R12= R2 + R3 (CI) ;;

   NOP             ;;
   NOP             ;;
   R0 = 0x1        ;;
   R1 = 0x2        ;;
   R2 = 0x8        ;;
   R3 = 0x4        ;;
   R5 = R0 - R1 (U)(T) ;; //0xffffffff
   R6 = R2 - R3 (CI)(U);; //0x3
   NOP             ;;
   NOP             ;;
   R7 = R0 - R1    ;;    //0xffffffff
   R8 = R2 - R3 (CI) ;;  //0x3
   NOP             ;;
   NOP             ;;
   R9 = R0 - R1 (T) ;;   //0xffffffff
   R10= R2 - R3 (CI) ;;  //0x3
   NOP             ;;
   NOP             ;;
   R11= R0 - R1 (U) ;;   //0x0
   R12= R2 - R3 (CI) ;;  //0x3
   NOP               ;;
   NOP               ;;
   NOP               ;;
   NOP               ;;
   SPU.Stop          ;;

  




