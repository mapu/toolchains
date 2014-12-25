//  Date:   2014-07-18
//  Name:   app.s.asm
//  Author: yanglp@aurora-hp.nadec.com

// begin program code */
    .text
    .global _start

_start:

/*============ Call ============*/
/*------- One Dim -------*/
		R0  = 0xc0000000;;
    R1  = 0xc0800000;;
		R2  = 0xc1000000;;
		R3  = 0xc1800000;;
		R4  = 0xc2000000;;
		R5  = 0xc2800000;;
		R6  = 0xc3000000;;
		R7  = 0xc4000000;;
		R8  = 0x40000000;;
		R9  = 0x40800000;;
		R10 = 0x41000000;;
		R11 = 0x41800000;;
		R12 = 0x42000000;;
		R13 = 0x42800000;;
		R14 = 0x43000000;;
		R15 = 0x44000000;;
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
    
    R0  = 0x40000000;;
    R1  = 0x40800000;;
		R2  = 0x41000000;;
		R3  = 0x41800000;;
		R4  = 0x42000000;;
		R5  = 0x42800000;;
		R6  = 0x43000000;;
		R7  = 0x44000000;;
		R8  = 0xc0000000;;
		R9  = 0xc0800000;;
		R10 = 0xc1000000;;
		R11 = 0xc1800000;;
		R12 = 0xc2000000;;
		R13 = 0xc2800000;;
		R14 = 0xc3000000;;
		R15 = 0xc4000000;;
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

