//  Date:   2014-07-18
//  Name:   app.s.asm
//  Author: yanglp@aurora-hp.nadec.com

// begin program code */
    .text
    .global _start

_start:

/*============ Call ============*/
/*------- One Dim -------*/
		R0  = 0x80000008;;
    R1  = 0x80000007;;
		R2  = 0x80000006;;
		R3  = 0x80000005;;
		R4  = 0x80000004;;
		R5  = 0x80000003;;
		R6  = 0x80000002;;
		R7  = 0x80000001;;
		R8  = 0x00000002;;
		R9  = 0x00000003;;
		R10 = 0x00000004;;
		R11 = 0x00000005;;
		R12 = 0x00000006;;
		R13 = 0x00000007;;
		R14 = 0x00000008;;
		R15 = 0x00000009;;
		SVR0[0] = R0  ;;
    SVR0[1] = R1  ;;
    SVR0[2] = R2  ;;
    SVR0[3] = R3  ;;
    SVR0[4] = R4  ;;
    SVR0[5] = R5  ;;
    SVR0[6] = R6  ;;
    SVR0[7] = R7  ;;
    SVR0[8] = R8  ;;
    SVR0[9] = R9  ;;
    SVR0[10] = R10  ;;
    SVR0[11] = R11  ;;
    SVR0[12] = R12  ;;
    SVR0[13] = R13  ;;
    SVR0[14] = R14  ;;
    SVR0[15] = R15  ;;   
    SHU0.T0 = SVR0  ;;
    
    // Init Data
    CallM _FALUTest0 (B) ;;
    NOP;;
    NOP;;
    
    // Test 
    CallM _FALUTest1 (B) ;;
    NOP;;
    NOP;;
    
    CallM _FALUTest2 (B) ;;
    NOP;;
    NOP;;
    
    CallM _FALUTest3 (B) ;;
    NOP;;
    NOP;;
    
    CallM _FALUTest4 (B) ;;
    NOP;;
    NOP;;
    
    CallM _FALUTest5 (B) ;;
    NOP;;
    NOP;;
    
    CallM _FALUTest6 (B) ;;
    NOP;;
    NOP;;
    
    CallM _FALUTest7 (B) ;;
    NOP;;
    NOP;;
    
    CallM _FALUTest8 (B) ;;
    NOP;;
    NOP;;
    
    CallM _FALUTest9 (B) ;;
    NOP;;
    NOP;;
    
    CallM _FALUTest10 (B) ;;
    NOP;;
    NOP;;
    
    

    NOP;;
    NOP;;
    NOP;;
    SPU.Stop ;;

