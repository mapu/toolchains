//  Date:   2014-07-18
//  Name:   app.s.asm
//  Author: yanglp@aurora-hp.nadec.com

// begin program code */
    .text
    .global _start

_start:

/*============ Call ============*/
/*------- One Dim -------*/
 m.s 		R0  = 0x80000008;;
 m.s     R1  = 0x80000007;;
 m.s 		R2  = 0x80000006;;
 m.s 		R3  = 0x80000005;;
 m.s 		R4  = 0x80000004;;
 m.s 		R5  = 0x80000003;;
 m.s 		R6  = 0x80000002;;
 m.s 		R7  = 0x80000001;;
 m.s 		R8  = 0x00000002;;
 m.s 		R9  = 0x00000003;;
 m.s 		R10 = 0x00000004;;
 m.s 		R11 = 0x00000005;;
 m.s 		R12 = 0x00000006;;
 m.s 		R13 = 0x00000007;;
 m.s 		R14 = 0x00000008;;
 m.s 		R15 = 0x00000009;;
 m.s 		SVR0[0] = R0  ;;
 m.s     SVR0[1] = R1  ;;
 m.s     SVR0[2] = R2  ;;
 m.s     SVR0[3] = R3  ;;
 m.s     SVR0[4] = R4  ;;
 m.s     SVR0[5] = R5  ;;
 m.s     SVR0[6] = R6  ;;
 m.s     SVR0[7] = R7  ;;
 m.s     SVR0[8] = R8  ;;
 m.s     SVR0[9] = R9  ;;
 m.s     SVR0[10] = R10  ;;
 m.s     SVR0[11] = R11  ;;
 m.s     SVR0[12] = R12  ;;
 m.s     SVR0[13] = R13  ;;
 m.s     SVR0[14] = R14  ;;
 m.s     SVR0[15] = R15  ;;   
 m.s     SHU0.T0 = SVR0  ;;
    
    // Init Data
 m.s     CallM _FALUTest0 (B) ;;
 m.s     NOP;;
 m.s     NOP;;
    
    // Test 
 m.s     CallM _FALUTest1 (B) ;;
 m.s     NOP;;
 m.s     NOP;;
    
 m.s     CallM _FALUTest2 (B) ;;
 m.s     NOP;;
 m.s     NOP;;
    
 m.s     CallM _FALUTest3 (B) ;;
 m.s     NOP;;
 m.s     NOP;;
    
 m.s     CallM _FALUTest4 (B) ;;
 m.s     NOP;;
 m.s     NOP;;
    
 m.s     CallM _FALUTest5 (B) ;;
 m.s     NOP;;
 m.s     NOP;;
    
 m.s     CallM _FALUTest6 (B) ;;
 m.s     NOP;;
 m.s     NOP;;
    
 m.s     CallM _FALUTest7 (B) ;;
 m.s     NOP;;
 m.s     NOP;;
    
 m.s     CallM _FALUTest8 (B) ;;
 m.s     NOP;;
 m.s     NOP;;
    
 m.s     CallM _FALUTest9 (B) ;;
 m.s     NOP;;
 m.s     NOP;;
    
 m.s     CallM _FALUTest10 (B) ;;
 m.s     NOP;;
 m.s     NOP;;
    
    

 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     SPU.Stop ;;

