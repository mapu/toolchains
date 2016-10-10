
//APE0
//test1 WA->ARA->AWA->RA
 m.s     J1 = 0x40400000;;
 m.s     J2 = 0x0;;
 m.s     R3 = 0x5;;
 m.s     [J1 + J2] = R3;;
 m.s     R12 = [J1 + J2] (AT);; 
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     R4 = 0x8;;
 m.s     [J1 + J2] = R4 (AT);;   // scr success
 m.s     R6 = R4;;
 m.s     R5 = [J1 + J2];;

//test2 WA->ARA->WA->AWA->RA
 m.s     J1 = 0x40400000;;
 m.s     J2 = 0x0;;
 m.s     R3 = 0x5;;
 m.s     [J1 + J2] = R3;;
 m.s     R12 = [J1 + J2] (AT);;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     R4 = 0x8;;
 m.s     [J1 + J2] = R4;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     R5 = 0x9;;
 m.s     [J1 + J2] = R5 (AT);;  // atomic write fail
 m.s     R7 = R5;;
 m.s     R6 = [J1 + J2];;

//test3 WA->WB->ARA->ARB->AWA->AWB->RA->RB
 m.s     J1 = 0x40400000;;
 m.s     J2 = 0x0;;
 m.s     J3 = 0x40700100;;
 m.s     J4 = 0x0;;
 m.s     R3 = 0x5;;
 m.s     R4 = 0x6;;
 m.s     [J1 + J2] = R3;;
 m.s     [J3 + J4] = R4;;
 m.s     R12 = [J1 + J2] (AT);;  //llr
 m.s     R13 = [J3 + J4] (AT);;  //llr another addr, get new lock addr
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     R5 = 0x8;;
 m.s     R6 = 0x9;;
 m.s     [J1 + J2] = R5 (AT);;  // scr failed
 m.s     R9 = R5;;
 m.s     [J3 + J4] = R6 (AT);;  // scr success
 m.s     R10 = R6;;
 m.s     R7 = [J1 + J2];;
 m.s     R8 = [J3 + J4];;


//test4 WA->ARA->AWB->AWA->RA
 m.s     J1 = 0x40400000;;
 m.s     J2 = 0x0;;
 m.s     R3 = 0x5;;
 m.s     [J1 + J2] = R3;;
 m.s     R12 = [J1 + J2] (AT);;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     J3 = 0x40700100;;
 m.s     J4 = 0x0;;
 m.s     R4 = 0x8;;
 m.s     [J3 + J4] = R4 (AT);;
 m.s     R7 = R4;;
 m.s     R5 = 0x9;;
 m.s     [J1 + J2] = R5 (AT);;   // scr success
 m.s     R8 = R5;;
 m.s     R6 = [J1 + J2];;

 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;


 m.s     SPUStop;;
  
