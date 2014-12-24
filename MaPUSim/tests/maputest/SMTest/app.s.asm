
//APE0
//test1 WA->ARA->AWA->RA
    J1 = 0x40400000;;
    J2 = 0x0;;
    R3 = 0x5;;
    [J1 + J2] = R3;;
    R12 = [J1 + J2] (AT);; 
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    R4 = 0x8;;
    [J1 + J2] = R4 (AT);;   // scr success
    R6 = R4;;
    R5 = [J1 + J2];;

//test2 WA->ARA->WA->AWA->RA
    J1 = 0x40400000;;
    J2 = 0x0;;
    R3 = 0x5;;
    [J1 + J2] = R3;;
    R12 = [J1 + J2] (AT);;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    R4 = 0x8;;
    [J1 + J2] = R4;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    R5 = 0x9;;
    [J1 + J2] = R5 (AT);;  // atomic write fail
    R7 = R5;;
    R6 = [J1 + J2];;

//test3 WA->WB->ARA->ARB->AWA->AWB->RA->RB
    J1 = 0x40400000;;
    J2 = 0x0;;
    J3 = 0x40700100;;
    J4 = 0x0;;
    R3 = 0x5;;
    R4 = 0x6;;
    [J1 + J2] = R3;;
    [J3 + J4] = R4;;
    R12 = [J1 + J2] (AT);;  //llr
    R13 = [J3 + J4] (AT);;  //llr another addr, get new lock addr
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    R5 = 0x8;;
    R6 = 0x9;;
    [J1 + J2] = R5 (AT);;  // scr failed
    R9 = R5;;
    [J3 + J4] = R6 (AT);;  // scr success
    R10 = R6;;
    R7 = [J1 + J2];;
    R8 = [J3 + J4];;


//test4 WA->ARA->AWB->AWA->RA
    J1 = 0x40400000;;
    J2 = 0x0;;
    R3 = 0x5;;
    [J1 + J2] = R3;;
    R12 = [J1 + J2] (AT);;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    J3 = 0x40700100;;
    J4 = 0x0;;
    R4 = 0x8;;
    [J3 + J4] = R4 (AT);;
    R7 = R4;;
    R5 = 0x9;;
    [J1 + J2] = R5 (AT);;   // scr success
    R8 = R5;;
    R6 = [J1 + J2];;

    NOP;;
    NOP;;
    NOP;;


    SPUStop;;
  
