// by zhangx
    .text
    .global _start

_start:
    R0 = 0x3;;
    J1 = 0x14;;
    R1 = 0x4;;
    //JUMP 0xc;;//JUMP J1;; //JUMP 0x8;; //IF R1,JUMP 0x8;;

    R2 = 0xFFFFFFFF;;    
    //R2 = 5;;
    R3 = R0 + R2;;
    R5 = STAT ;;
    R6 = R3;;
    R8 = R6;;
    R7 = R5;;

    R8 = R0 + R2 (CI);;

    R9 = R0 - R2;;
    R5 = STAT ;;
    R10 = R0 - R2(CI);;


    R4 = R1 - R2;;


    //JUMP -8;;
    R5 = R1;;
    R1 = 0x5;;
    R2 = 0x3;;
    R6 = R1 * R2;;
    R11 = R1 + R6;;
    R7 = ABS R1;;

// logic instru
    R8 = R1 & R2;;
    R9 = R1 | R2;;
    R10= R1 ^ R2;;
    R11= ~R1;;

// compare instru
    R12= R1 == R2;;
    R13= R1 != R2;;
    R14= R1 >  R2;;
    R15= R1 >= R2;;
    R16= R1 <  R2;;
    R17= R1 <= R2;;

// shift instru
    R18= R1 << R2;;
    R19= R1 << 2;;
    R20= R1 >> R2;;
    R21= R1 >> 1;;

    NOP;;

// addr compution instru
    J1 = 0x22;;
    J2 = 0x2;;
    J3 = J1 + J2;;
    J4 = J1 - J2;;
    J5 = J1 << J2;;
    J6 = J1 << 0x2;;

    R1 = 0x5;;
    J1 = 0x6;;
    R2 = R1;;
    R3 = J1;;
    J2 = R1;;
    J3 = J1;;

    J1 = 0x100;;    
    J2 = 0x400000;;
    R1 = 0x12345678;;
    [J1 + J2] = R1;;
    //R1 = 0x83;;
    //[J1 + 4] = R1;;    
    R2 = [J1 + J2];;
    R3 = [J1 + J2](B);;
    R4 = [J1 + J2](H);;
    R5 = [J1 + J2](B) (U);;
    R6 = [J1 + J2](H) (U);;
    
    J3 = 0x2000;;
    [J1 + J2] = J3;;
    J3 = 0x42;;
    J4 = [J1 + J2];;

    R3 = 0x20000;;
    [J1 + 0xc] = R3;;
    R4 = [J1 + 0xc];;

    J5 = 0x83;;
    [J1 + 0x8] = J5;;
    J6 = [J1 + 0x8];;
    

    J1 = 0x400020;;
    R1 = 0x5 ;;
    [J1 + -4] = R1 ;;
    R2 = [J1 + -4];;
    NOP;;
    NOP;;
    NOP;;
    NOP;;

// jump instru  
    R1 = 0x8;;
    R2 = 0x1;;
_label:
    R1 = R1 - R2;;
    IF R1, JUMP _label;;

    
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;

// loop to instru
    J1 = 0x2;;
    LPTO _labelloop BY J1 (L0);;
    J1 = 0x1;;
    J2 = 0x2;;
    J1 = 0x1;;
    J2 = 0x2;;
    J1 = 0x1;;
    J2 = 0x2;;
    J1 = 0x1;;
    J2 = 0x2;;
    J1 = 0x1;;
    J2 = 0x2;;
    J1 = 0x1;;
    J2 = 0x2;;
    J1 = 0x1;;
    J2 = 0x2;;
    J1 = 0x1;;
    J2 = 0x2;;
    J1 = 0x1;;
    J2 = 0x2;;
    J1 = 0x1;;
    J2 = 0x2;;
    J1 = 0x1;;
    J2 = 0x2;;
    J1 = 0x1;;
    J2 = 0x2;;
    J1 = 0x1;;
    J2 = 0x2;;
    J1 = 0x1;;
    J12 = 0x77;;  
.align 16
_labelloop:
    J3 = 0x5;; //lpend

    J4 = 0x8;;

    R1 = 0xFFFFFFF8;;
    R2 = R1 >> 3;;

    R1 = 0xFFFFFFF8;;
    R2 = R1 >> 3 (U);;

    R3 = 0x3;;
    R1 = 0xFFFFFFF8;;
    R2 = R1 >> R3;;

    R1 = 0xFFFFFFF8;;
    R2 = R1 >> R3 (U);;    

    NOP;;
    NOP;;

// single float compute
    R0 = 0x3F800000  ;; //R0 = 1
    R1 = 0x40000000  ;; //R1 = 2 
      
    R2 = R0 + R1(S);;    // 0x40400000
    R3 = R2;;
    R4 = R0 - R1(S);;    // 0xbf800000
    R5 = R4;;
    R6 = R0 * R1(S);;
    R7 = R6;;

    R0 = 0xBF800000;;  //-1
    R1 = ABS R0 (S);;
    R2 = R1;;

    R3 = 0x40000000  ;; //R1 = 2
    NOP;;//R4 = RECIP R3 (S);; //0.5
    R5 = R4;;

    R6 = 0x40800000;; // 4
    NOP;;//R7 = RSQRT R6 (S);;
    R8 = R7;;


//float convert
    R0 = 0x3;;
    R1 = 0x3F800000;;

    R2 = SINGLE R0 (U);;   //3f800000
    R8 = R2;;
    R3 = SINGLE R0;;       //3f800000

    DR1 = DOUBLE R0 ;;      //0x3ff0000000000000
    DR1 = DOUBLE R0 (U);;

    DR2 = DOUBLE R1 (S);;  //0x3ff0000000000000   
    R3  = SINGLE DR1 (D);;
    R4 = R3;;

    R1 = 0x3F800000;;
    R5 = INT R1 (S);;
    R6 = UINT R1 (S);;


    R0 = 0x0;;
    R1 = 0x40240000;;   //10
    R2 = 0x0;;
    R3 = 0x40340000;;   //20
    R7 = INT DR0;;
    R8 = UINT DR1;;


// single float compare
    R0 = 0x40000000;; //R0 = 2
    R1 = 0x3F800000;; //R1 = 1
    R2 = R0 == R1 (S);;
    R8 = R0 != R1 (S);;
    R5 = R8 ;;
    R3 = R0 < R1 (S);;
    R6 = R0 <= R1 (S);;
    R7 = R0 > R1 (S);;
    R4 = R0 >= R1 (S);;

// DR transfer
    R0 = 0x11;;
    R1 = 0x22;;
    DR1 = DR0;;
    R5 = R2;;
    R6 = R3;; 

// DR load/store
    J1 = 0x200;;    
    J2 = 0x400000;;
    R0 = 0x87654321;;
    R1 = 0x12345678;;
    [J1 + J2] = DR0;;  
    DR1 = [J1 + J2];;
    R5 = R2;;
    R6 = R3;;

//DR SCU instr
    R0 = 0x0;;
    R1 = 0x40240000;;   //10
    R2 = 0x0;;
    R3 = 0x40340000;;   //20
    
    DR2 = DR0 + DR1;;
    DR3 = DR1 - DR0;;
    DR4 = DR0 * DR1;;
    DR5 = ABS DR0;;
    NOP;;//DR6 = RECIP DR0;;
    NOP;;//DR7 = RSQRT  DR0;;

    R0 = DR0 == DR1;;
    R0 = DR0 != DR1;;
    R0 = DR0 >= DR1;;
    R0 = DR0 <= DR1;;
    R0 = DR0 >  DR1;;
    R0 = DR0 <  DR1;;

    NOP;;
    NOP;;

    J1 = 0x100;;    
    J2 = 0x2000;;
    R1 = 0x12345678;;
    [J2 + 0x100] = R1;;
    [J2 + 0x200] = R1;;
    [J2 + 0x300] = R1;;
    [J2 + 0x400] = R1;;
    [J2 + 0x500] = R1;;
    [J2 + 0x600] = R1;;
    [J2 + 0x700] = R1;;
    R2 = [J2 + 0x100];;
    R3 = [J2 + 0x200];;
    R4 = [J2 + 0x300];;
    R5 = [J2 + 0x400];;
    R6 = [J2 + 0x500];;
    R7 = [J2 + 0x600];;
    R8 = [J2 + 0x700];;


//SCU add test
  // no overflow test
    R0 = 0x3;;
    R1 = 0x8;;
    R2 = 0xFFFFFFFE;; //-2 
    R3 = 0xFFFFFFFC;; //-4
    R4 = R0 + R1(U);;   // + + 0xb
    R31 = STAT ;;
    R5 = R0 + R1;;      // + + 0xb
    R31 = STAT ;;
    R6 = R0 + R2;;      // + - 1
    R31 = STAT ;;
    R6 = R3 + R0;;      // - +  -1 ffffffff
    R31 = STAT ;;
    R7 = R2 + R3;;      // - -  -6 fffffffa
    R31 = STAT ;;
    R4 = R0 + R1(U)(T);;   // + +  b
    R31 = STAT ;;
    R5 = R0 + R1(T);;      // + +  b
    R31 = STAT ;;
    R6 = R0 + R2(T);;      // + -  1 
    R31 = STAT ;;
    R6 = R3 + R0(T);;      // + -  ffffffff 
    R31 = STAT ;;
    R7 = R2 + R3(T);;      // - -  fffffffa
    R31 = STAT ;;

  // overflow test
    R0 = 0xFFFFFFFE;; //-2
    R1 = 0x8;;
    R2 = R0 + R1(U);;  // ffffffff overflow CI = 1
    R31 = STAT ;;
    R2 = R0 + R1(U)(T);; // 6(T) CI =1
    R31 = STAT ;;
    R0 = 0x7FFFFFFD;;  //2147483645
    R1 = 0x7FFFFFFE;;  //2147483646
    R2 = 0x80000001;;  //-2147483647 
    R3 = 0x80000002;;  //-2147483646
    R4 = R0 + R1;;     //7fffffff CI = 1
    R31 = STAT ;;
    R5 = R0 + R1(T);;  //fffffffb CI = 1
    R31 = STAT ;;
    R6 = R2 + R3;;     //80000000 CI =1
    R31 = STAT ;;
    R7 = R2 + R3(T);;  //3 CI = 1
    R31 = STAT ;;

//test for CI
//  R0   R1
//     +
//  R2   R3

    R1 = 0xFFFFFFFE;; 
    R3 = 0x8;;
    R0 = 0x33;;
    R2 = 0x22;;
    
    R4 = R1 + R3(U);; //ffffffff CI = 1
    R31 = STAT ;;
    R5 = R0 + R2(CI);;// + CI 0x56
    R31 = STAT ;;

    NOP;;
    NOP;;

//SCU sub test
  // no overflow test
    R0 = 0x3;;
    R1 = 0x8;;
    R2 = 0xFFFFFFFE;; //-2
    R3 = 0xFFFFFFFC;; //-4
    R4 = R0 - R1(U);;   // + +  fffffffb -5
    R31 = STAT ;;
    R5 = R0 - R1;;      // + +  fffffffb -5
    R31 = STAT ;;
    R6 = R0 - R2;;      // + -  5
    R31 = STAT ;;
    R6 = R3 - R0;;      // - +  fffffff9 -7
    R31 = STAT ;;
    R7 = R2 - R3;;      // - -  2
    R31 = STAT ;;
    R4 = R0 - R1(U)(T);;   // + + fffffffb
    R31 = STAT ;; 
    R5 = R0 - R1(T);;      // + + fffffffb
    R31 = STAT ;;
    R6 = R0 - R2(T);;      // + - 5
    R31 = STAT ;;
    R6 = R3 - R0(T);;      // + - fffffff9
    R31 = STAT ;;
    R7 = R2 - R3(T);;      // - - 2
    R31 = STAT ;;

  // overflow test
    R0 = 0x0;; // 
    R1 = 0x2;; 
    R2 = R0 - R1(U);;  //0
    R31 = STAT ;;
    R2 = R0 - R1(U)(T);;//2
    R31 = STAT ;;
    R0 = 0x7FFFFFFD;;   // 2147483645
    R1 = 0x7FFFFFFE;;   // 2147483646
    R2 = 0x80000001;;   // -2147483647
    R3 = 0x80000002;;   // -2147483646
    R4 = R0 - R2;;      // 7fffffff CI =1
    R31 = STAT ;;
    R5 = R0 - R2(T);;   // fffffffc CI =1
    R31 = STAT ;;
    R6 = R3 - R1;;      // 80000000 CI =1
    R31 = STAT ;;
    R7 = R3 - R1(T);;   // 4  CI =1
    R31 = STAT ;;

//test for CI
//  R0   R1
//     -
//  R2   R3

    R1 = 0x0;;         // 0
    R3 = 0x2;;         // 8
    R0 = 0x33;;        // 51
    R2 = 0x22;;        // 34
    
    R4 = R1 - R3(U);;  // 0 CI = 1
    R31 = STAT ;;
    R5 = R0 - R2(CI);; // 0x10 -CI 
    R31 = STAT ;;

//test for recip rsqrt
     #SCU Recip and RSqrt Test 
    R0 = 0x3F800000  ;;   //  1
    R1 = 0x3F820000  ;;
    R8 = 0x3F810000  ;;
    R11= 0x3F000000  ;; 
    R18= 0x3F020000  ;;
    R2 = 0x0 ;;   
    R3 = 0x3ff00000 ;;   #R3 = 0x3fffffff 2
    R4 = 0xffffffff ;;   #R4 = 0xffffffff
    R5 = 0x3fffffff ;;   #R4 = 0xBfffffff -2
    R6 = 0x0 ;;
    R7 = 0x3fe00000 ;;
    R8 = 0xffffffff ;;
    R9 = 0x3fefffff ;;
    NOP;;
    NOP;;
    R10 = RECIP R0 (S);;    #0x3f7fc000--0x3f800000(use the default system task of sv)
    R31 = RECIP R1 (S);;
    R16 = RECIP R8 (S);;
    DR10 = RECIP DR1 (D);;

    R13  = RSQRT R1 (S);;   
    R14  = RSQRT R11 (S);;
    R21  = RSQRT R18 (S);;
  
    DR12 = RSQRT DR1 (D);;  
    DR13 = RSQRT DR2 (D);;  
    DR14 = RSQRT DR3 (D);;
    DR15 = RSQRT DR4 (D);;

    NOP;;
    NOP;;
    NOP;;

    NOP;;
    NOP;;
    NOP;;




    SPUStop;;
