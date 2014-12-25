//  Date:   2014-07-18
//  Name:   app.s.asm
//  Author: yanglp@aurora-hp.nadec.com

// begin program code */
    .text
    .global _start

_start:

/*============ Call ============*/
/*------- One Dim -------*/
		R0  = 0x00000000;;
    R1  = 0x40000000;;
		R2  = 0x00000000;;
		R3  = 0xc0000000;;
		R4  = 0x30000000;;
		R5  = 0x40000000;;
		R6  = 0x30000000;;
		R7  = 0xc0000000;;
		R8  = 0x18000000;;
		R9  = 0xbff00000;;
		R10 = 0x18000000;;
		R11 = 0x3ff00000;;
		R12 = 0xf0000000;;
		R13 = 0xfeff0000;;
		R14 = 0x00000000;;
		R15 = 0x7fefffff;;
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
    
    R0  = 0x00000000;;
    R1  = 0x40100000;;
		R2  = 0x00000000;;
		R3  = 0x40100000;;
		R4  = 0x00000000;;
		R5  = 0x40100000;;
		R6  = 0x00000000;;
		R7  = 0x40100000;;
		R8  = 0x00000000;;
		R9  = 0x40100000;;
		R10 = 0x00000000;;
		R11 = 0x40100000;;
		R12 = 0x00000000;;
		R13 = 0x40100000;;
		R14 = 0x00000000;;
		R15 = 0x40100000;;
		SVR1[0] = R0  ;;
    SVR1[1] = R1  ;;
    SVR1[2] = R2  ;;
    SVR1[3] = R3  ;;
    SVR1[4] = R4  ;;
    SVR1[5] = R5  ;;
    SVR1[6] = R6  ;;
    SVR1[7] = R7  ;;
    SVR1[8] = R8  ;;
    SVR1[9] = R9  ;;
    SVR1[10] = R10  ;;
    SVR1[11] = R11  ;;
    SVR1[12] = R12  ;;
    SVR1[13] = R13  ;;
    SVR1[14] = R14  ;;
    SVR1[15] = R15  ;;
    SHU1.T0 = SVR1  ;;
    
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

