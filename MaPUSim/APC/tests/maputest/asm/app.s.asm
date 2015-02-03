// by zhangx
    .text
    .global _start

_start:
 m.s     R0 = 0x3;;
 m.s     J1 = 0x14;;
 m.s     R1 = 0x4;;
    // m.s JUMP 0xc;;//JUMP J1;; //JUMP 0x8;; //IF R1,JUMP 0x8;;

 m.s     R2 = 0xFFFFFFFF;;    
    // m.s R2 = 5;;
 m.s     R3 = R0 + R2;;
 m.s     R5 = STAT ;;
 m.s     R6 = R3;;
 m.s     R8 = R6;;
 m.s     R7 = R5;;

 m.s     R8 = R0 + R2 (CI);;

 m.s     R9 = R0 - R2;;
 m.s     R5 = STAT ;;
 m.s     R10 = R0 - R2(CI);;


 m.s     R4 = R1 - R2;;


    // m.s JUMP -8;;
 m.s     R5 = R1;;
 m.s     R1 = 0x5;;
 m.s     R2 = 0x3;;
 m.s     R6 = R1 * R2;;
 m.s     R11 = R1 + R6;;
 m.s     R7 = ABS R1;;

// logic instru
 m.s     R8 = R1 & R2;;
 m.s     R9 = R1 | R2;;
 m.s     R10= R1 ^ R2;;
 m.s     R11= ~R1;;

// compare instru
 m.s     R12= R1 == R2;;
 m.s     R13= R1 != R2;;
 m.s     R14= R1 >  R2;;
 m.s     R15= R1 >= R2;;
 m.s     R16= R1 <  R2;;
 m.s     R17= R1 <= R2;;

// shift instru
 m.s     R18= R1 << R2;;
 m.s     R19= R1 << 2;;
 m.s     R20= R1 >> R2;;
 m.s     R21= R1 >> 1;;

 m.s     NOP;;

// addr compution instru
 m.s     J1 = 0x22;;
 m.s     J2 = 0x2;;
 m.s     J3 = J1 + J2;;
 m.s     J4 = J1 - J2;;
 m.s     J5 = J1 << J2;;
 m.s     J6 = J1 << 0x2;;

 m.s     R1 = 0x5;;
 m.s     J1 = 0x6;;
 m.s     R2 = R1;;
 m.s     R3 = J1;;
 m.s     J2 = R1;;
 m.s     J3 = J1;;

 m.s     J1 = 0x100;;    
 m.s     J2 = 0x400000;;
 m.s     R1 = 0x12345678;;
 m.s     [J1 + J2] = R1;;
    // m.s R1 = 0x83;;
    // m.s [J1 + 4] = R1;;    
 m.s     R2 = [J1 + J2];;
 m.s     R3 = [J1 + J2](B);;
 m.s     R4 = [J1 + J2](H);;
 m.s     R5 = [J1 + J2](B) (U);;
 m.s     R6 = [J1 + J2](H) (U);;
    
 m.s     J3 = 0x2000;;
 m.s     [J1 + J2] = J3;;
 m.s     J3 = 0x42;;
 m.s     J4 = [J1 + J2];;

 m.s     R3 = 0x20000;;
 m.s     [J1 + 0xc] = R3;;
 m.s     R4 = [J1 + 0xc];;

 m.s     J5 = 0x83;;
 m.s     [J1 + 0x8] = J5;;
 m.s     J6 = [J1 + 0x8];;
    

 m.s     J1 = 0x400020;;
 m.s     R1 = 0x5 ;;
 m.s     [J1 + -4] = R1 ;;
 m.s     R2 = [J1 + -4];;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;

// jump instru  
 m.s     R1 = 0x8;;
 m.s     R2 = 0x1;;
_label:
 m.s     R1 = R1 - R2;;
 m.s     IF R1, JUMP _label;;

    
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;

// loop to instru
 m.s     J1 = 0x2;;
 m.s     LPTO _labelloop BY J1 (L0);;
 m.s     J1 = 0x1;;
 m.s     J2 = 0x2;;
 m.s     J1 = 0x1;;
 m.s     J2 = 0x2;;
 m.s     J1 = 0x1;;
 m.s     J2 = 0x2;;
 m.s     J1 = 0x1;;
 m.s     J2 = 0x2;;
 m.s     J1 = 0x1;;
 m.s     J2 = 0x2;;
 m.s     J1 = 0x1;;
 m.s     J2 = 0x2;;
 m.s     J1 = 0x1;;
 m.s     J2 = 0x2;;
 m.s     J1 = 0x1;;
 m.s     J2 = 0x2;;
 m.s     J1 = 0x1;;
 m.s     J2 = 0x2;;
 m.s     J1 = 0x1;;
 m.s     J2 = 0x2;;
 m.s     J1 = 0x1;;
 m.s     J2 = 0x2;;
 m.s     J1 = 0x1;;
 m.s     J2 = 0x2;;
 m.s     J1 = 0x1;;
 m.s     J2 = 0x2;;
 m.s     J1 = 0x1;;
 m.s     J12 = 0x77;;  
.align 16
_labelloop:
 m.s     J3 = 0x5;; //lpend

 m.s     J4 = 0x8;;

 m.s     R1 = 0xFFFFFFF8;;
 m.s     R2 = R1 >> 3;;

 m.s     R1 = 0xFFFFFFF8;;
 m.s     R2 = R1 >> 3 (U);;

 m.s     R3 = 0x3;;
 m.s     R1 = 0xFFFFFFF8;;
 m.s     R2 = R1 >> R3;;

 m.s     R1 = 0xFFFFFFF8;;
 m.s     R2 = R1 >> R3 (U);;    

 m.s     NOP;;
 m.s     NOP;;

// single float compute
 m.s     R0 = 0x3F800000  ;; //R0 = 1
 m.s     R1 = 0x40000000  ;; //R1 = 2 
      
 m.s     R2 = R0 + R1(S);;    // 0x40400000
 m.s     R3 = R2;;
 m.s     R4 = R0 - R1(S);;    // 0xbf800000
 m.s     R5 = R4;;
 m.s     R6 = R0 * R1(S);;
 m.s     R7 = R6;;

 m.s     R0 = 0xBF800000;;  //-1
 m.s     R1 = ABS R0 (S);;
 m.s     R2 = R1;;

 m.s     R3 = 0x40000000  ;; //R1 = 2
 m.s     NOP;;//R4 = RECIP R3 (S);; //0.5
 m.s     R5 = R4;;

 m.s     R6 = 0x40800000;; // 4
 m.s     NOP;;//R7 = RSQRT R6 (S);;
 m.s     R8 = R7;;


//float convert
 m.s     R0 = 0x3;;
 m.s     R1 = 0x3F800000;;

 m.s     R2 = SINGLE R0 (U);;   //3f800000
 m.s     R8 = R2;;
 m.s     R3 = SINGLE R0;;       //3f800000

 m.s     DR1 = DOUBLE R0 ;;      //0x3ff0000000000000
 m.s     DR1 = DOUBLE R0 (U);;

 m.s     DR2 = DOUBLE R1 (S);;  //0x3ff0000000000000   
 m.s     R3  = SINGLE DR1 (D);;
 m.s     R4 = R3;;

 m.s     R1 = 0x3F800000;;
 m.s     R5 = INT R1 (S);;
 m.s     R6 = UINT R1 (S);;


 m.s     R0 = 0x0;;
 m.s     R1 = 0x40240000;;   //10
 m.s     R2 = 0x0;;
 m.s     R3 = 0x40340000;;   //20
 m.s     R7 = INT DR0;;
 m.s     R8 = UINT DR1;;


// single float compare
 m.s     R0 = 0x40000000;; //R0 = 2
 m.s     R1 = 0x3F800000;; //R1 = 1
 m.s     R2 = R0 == R1 (S);;
 m.s     R8 = R0 != R1 (S);;
 m.s     R5 = R8 ;;
 m.s     R3 = R0 < R1 (S);;
 m.s     R6 = R0 <= R1 (S);;
 m.s     R7 = R0 > R1 (S);;
 m.s     R4 = R0 >= R1 (S);;

// DR transfer
 m.s     R0 = 0x11;;
 m.s     R1 = 0x22;;
 m.s     DR1 = DR0;;
 m.s     R5 = R2;;
 m.s     R6 = R3;; 

// DR load/store
 m.s     J1 = 0x200;;    
 m.s     J2 = 0x400000;;
 m.s     R0 = 0x87654321;;
 m.s     R1 = 0x12345678;;
 m.s     [J1 + J2] = DR0;;  
 m.s     DR1 = [J1 + J2];;
 m.s     R5 = R2;;
 m.s     R6 = R3;;

//DR SCU instr
 m.s     R0 = 0x0;;
 m.s     R1 = 0x40240000;;   //10
 m.s     R2 = 0x0;;
 m.s     R3 = 0x40340000;;   //20
    
 m.s     DR2 = DR0 + DR1;;
 m.s     DR3 = DR1 - DR0;;
 m.s     DR4 = DR0 * DR1;;
 m.s     DR5 = ABS DR0;;
 m.s     NOP;;//DR6 = RECIP DR0;;
 m.s     NOP;;//DR7 = RSQRT  DR0;;

 m.s     R0 = DR0 == DR1;;
 m.s     R0 = DR0 != DR1;;
 m.s     R0 = DR0 >= DR1;;
 m.s     R0 = DR0 <= DR1;;
 m.s     R0 = DR0 >  DR1;;
 m.s     R0 = DR0 <  DR1;;

 m.s     NOP;;
 m.s     NOP;;

 m.s     J1 = 0x100;;    
 m.s     J2 = 0x2000;;
 m.s     R1 = 0x12345678;;
 m.s     [J2 + 0x100] = R1;;
 m.s     [J2 + 0x200] = R1;;
 m.s     [J2 + 0x300] = R1;;
 m.s     [J2 + 0x400] = R1;;
 m.s     [J2 + 0x500] = R1;;
 m.s     [J2 + 0x600] = R1;;
 m.s     [J2 + 0x700] = R1;;
 m.s     R2 = [J2 + 0x100];;
 m.s     R3 = [J2 + 0x200];;
 m.s     R4 = [J2 + 0x300];;
 m.s     R5 = [J2 + 0x400];;
 m.s     R6 = [J2 + 0x500];;
 m.s     R7 = [J2 + 0x600];;
 m.s     R8 = [J2 + 0x700];;


//SCU add test
  // no overflow test
 m.s     R0 = 0x3;;
 m.s     R1 = 0x8;;
 m.s     R2 = 0xFFFFFFFE;; //-2 
 m.s     R3 = 0xFFFFFFFC;; //-4
 m.s     R4 = R0 + R1(U);;   // + + 0xb
 m.s     R31 = STAT ;;
 m.s     R5 = R0 + R1;;      // + + 0xb
 m.s     R31 = STAT ;;
 m.s     R6 = R0 + R2;;      // + - 1
 m.s     R31 = STAT ;;
 m.s     R6 = R3 + R0;;      // - +  -1 ffffffff
 m.s     R31 = STAT ;;
 m.s     R7 = R2 + R3;;      // - -  -6 fffffffa
 m.s     R31 = STAT ;;
 m.s     R4 = R0 + R1(U)(T);;   // + +  b
 m.s     R31 = STAT ;;
 m.s     R5 = R0 + R1(T);;      // + +  b
 m.s     R31 = STAT ;;
 m.s     R6 = R0 + R2(T);;      // + -  1 
 m.s     R31 = STAT ;;
 m.s     R6 = R3 + R0(T);;      // + -  ffffffff 
 m.s     R31 = STAT ;;
 m.s     R7 = R2 + R3(T);;      // - -  fffffffa
 m.s     R31 = STAT ;;

  // overflow test
 m.s     R0 = 0xFFFFFFFE;; //-2
 m.s     R1 = 0x8;;
 m.s     R2 = R0 + R1(U);;  // ffffffff overflow CI = 1
 m.s     R31 = STAT ;;
 m.s     R2 = R0 + R1(U)(T);; // 6(T) CI =1
 m.s     R31 = STAT ;;
 m.s     R0 = 0x7FFFFFFD;;  //2147483645
 m.s     R1 = 0x7FFFFFFE;;  //2147483646
 m.s     R2 = 0x80000001;;  //-2147483647 
 m.s     R3 = 0x80000002;;  //-2147483646
 m.s     R4 = R0 + R1;;     //7fffffff CI = 1
 m.s     R31 = STAT ;;
 m.s     R5 = R0 + R1(T);;  //fffffffb CI = 1
 m.s     R31 = STAT ;;
 m.s     R6 = R2 + R3;;     //80000000 CI =1
 m.s     R31 = STAT ;;
 m.s     R7 = R2 + R3(T);;  //3 CI = 1
 m.s     R31 = STAT ;;

//test for CI
//  R0   R1
//     +
//  R2   R3

 m.s     R1 = 0xFFFFFFFE;; 
 m.s     R3 = 0x8;;
 m.s     R0 = 0x33;;
 m.s     R2 = 0x22;;
    
 m.s     R4 = R1 + R3(U);; //ffffffff CI = 1
 m.s     R31 = STAT ;;
 m.s     R5 = R0 + R2(CI);;// + CI 0x56
 m.s     R31 = STAT ;;

 m.s     NOP;;
 m.s     NOP;;

//SCU sub test
  // no overflow test
 m.s     R0 = 0x3;;
 m.s     R1 = 0x8;;
 m.s     R2 = 0xFFFFFFFE;; //-2
 m.s     R3 = 0xFFFFFFFC;; //-4
 m.s     R4 = R0 - R1(U);;   // + +  fffffffb -5
 m.s     R31 = STAT ;;
 m.s     R5 = R0 - R1;;      // + +  fffffffb -5
 m.s     R31 = STAT ;;
 m.s     R6 = R0 - R2;;      // + -  5
 m.s     R31 = STAT ;;
 m.s     R6 = R3 - R0;;      // - +  fffffff9 -7
 m.s     R31 = STAT ;;
 m.s     R7 = R2 - R3;;      // - -  2
 m.s     R31 = STAT ;;
 m.s     R4 = R0 - R1(U)(T);;   // + + fffffffb
 m.s     R31 = STAT ;; 
 m.s     R5 = R0 - R1(T);;      // + + fffffffb
 m.s     R31 = STAT ;;
 m.s     R6 = R0 - R2(T);;      // + - 5
 m.s     R31 = STAT ;;
 m.s     R6 = R3 - R0(T);;      // + - fffffff9
 m.s     R31 = STAT ;;
 m.s     R7 = R2 - R3(T);;      // - - 2
 m.s     R31 = STAT ;;

  // overflow test
 m.s     R0 = 0x0;; // 
 m.s     R1 = 0x2;; 
 m.s     R2 = R0 - R1(U);;  //0
 m.s     R31 = STAT ;;
 m.s     R2 = R0 - R1(U)(T);;//2
 m.s     R31 = STAT ;;
 m.s     R0 = 0x7FFFFFFD;;   // 2147483645
 m.s     R1 = 0x7FFFFFFE;;   // 2147483646
 m.s     R2 = 0x80000001;;   // -2147483647
 m.s     R3 = 0x80000002;;   // -2147483646
 m.s     R4 = R0 - R2;;      // 7fffffff CI =1
 m.s     R31 = STAT ;;
 m.s     R5 = R0 - R2(T);;   // fffffffc CI =1
 m.s     R31 = STAT ;;
 m.s     R6 = R3 - R1;;      // 80000000 CI =1
 m.s     R31 = STAT ;;
 m.s     R7 = R3 - R1(T);;   // 4  CI =1
 m.s     R31 = STAT ;;

//test for CI
//  R0   R1
//     -
//  R2   R3

 m.s     R1 = 0x0;;         // 0
 m.s     R3 = 0x2;;         // 8
 m.s     R0 = 0x33;;        // 51
 m.s     R2 = 0x22;;        // 34
    
 m.s     R4 = R1 - R3(U);;  // 0 CI = 1
 m.s     R31 = STAT ;;
 m.s     R5 = R0 - R2(CI);; // 0x10 -CI 
 m.s     R31 = STAT ;;

//test for recip rsqrt
     #SCU Recip and RSqrt Test 
 m.s     R0 = 0x3F800000  ;;   //  1
 m.s     R1 = 0x3F820000  ;;
 m.s     R8 = 0x3F810000  ;;
 m.s     R11= 0x3F000000  ;; 
 m.s     R18= 0x3F020000  ;;
 m.s     R2 = 0x0 ;;   
 m.s     R3 = 0x3ff00000 ;;   #R3 = 0x3fffffff 2
 m.s     R4 = 0xffffffff ;;   #R4 = 0xffffffff
 m.s     R5 = 0x3fffffff ;;   #R4 = 0xBfffffff -2
 m.s     R6 = 0x0 ;;
 m.s     R7 = 0x3fe00000 ;;
 m.s     R8 = 0xffffffff ;;
 m.s     R9 = 0x3fefffff ;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     R10 = RECIP R0 (S);;    #0x3f7fc000--0x3f800000(use the default system task of sv)
 m.s     R31 = RECIP R1 (S);;
 m.s     R16 = RECIP R8 (S);;
 m.s     DR10 = RECIP DR1 (D);;

 m.s     R13  = RSQRT R1 (S);;   
 m.s     R14  = RSQRT R11 (S);;
 m.s     R21  = RSQRT R18 (S);;
  
 m.s     DR12 = RSQRT DR1 (D);;  
 m.s     DR13 = RSQRT DR2 (D);;  
 m.s     DR14 = RSQRT DR3 (D);;
 m.s     DR15 = RSQRT DR4 (D);;

 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;

 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;




 m.s     SPUStop;;
