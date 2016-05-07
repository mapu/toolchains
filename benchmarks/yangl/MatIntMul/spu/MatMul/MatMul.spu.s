//  Date: 2013-11-07
//  Name: app.s.asm
//  Author: chendi1899@gmail.com
//  Description: The configuration Unit for MPU for 8 bits char format Matrix Multiplication

    .text
    .global main

main:

MAX_LENGTH = 256
// parameter definition
// the following need not to be modified    
	SDA0DM0_START = 0x400000
	SDA1DM0_START = 0x800000 
	SDA2DM0_START = 0xc00000  
	SDASize       = 0x40000  

	BS = 16
	K = BS
	ARowNum = MAX_LENGTH
	AColNum = MAX_LENGTH
	BRowNum = MAX_LENGTH
	BColNum = MAX_LENGTH
	CRowNum = MAX_LENGTH
	CColNum = MAX_LENGTH

	ARowBSNum = ARowNum/BS
	AColBSNum = AColNum/BS
	BRowBSNum = BRowNum/BS
	BColBSNum = BColNum/BS
	CRowBSNum = CRowNum/BS
	CColBSNum = CColNum/BS

	DataSize = 4                    //  Byte 1, Short 2, Word 4, Long 8
	
	BlockSize = BS*DataSize         
	BlockGran = 2                   // log2(4)  take out 4 Bytes from each Bank every time 
	
	BankNum = BS
	BankSize = SDASize/BankNum	// 0x40000/0x10 = 0x4000
	BankCap = ARowNum/BankNum	// the num of Row in each Bank


    // for MC Config   
    MIndexStart   = 0x0
    MStepMode     = 0x1 
    MLen          = 0x10
    ISize         = 0x10

    MIndexOffset  = 0
    MStepOffset   = 8
    MLenOffset    = 24
    ISizeOffset   = 12

    MCIndexStart  = MIndexStart << MIndexOffset 
    MCStepMode    = MStepMode   << MStepOffset
    MCLen         = MLen        << MLenOffset
    ICSize        = ISize       << ISizeOffset
    MConfig       = MCIndexStart | MCStepMode | MCLen | ICSize

 m.s     NOP;;

// for MC
 m.s 	R1 = MConfig ;;
 m.s 	NOP ;;
 m.s 	NOP ;;
 m.s 	MC.w0 = R1 ;;
 m.s 	MC.r0 = R1;;
 m.s 	MC.r1 = R1;;

// SHU.T1
 m.s 	R1 = 0x03020100 ;;
 m.s 	SHU0.T1 = Vect R1 ;;


// for Loop
 m.s 	R1 = BColNum;; 		// KI12 for MSEQ 
 m.s 	R2 = AColBSNum;; 	// KI13
 m.s 	R3 = ARowBSNum;;	// KI14
 m.s 	NOP;; 
 m.s 	NOP;;
 m.s 	NOP;;
 m.s 	KI12 = R1;;
 m.s 	KI13 = R2;;
 m.s 	KI14 = R3;;



// configure the K register
// Read the date in Col mode   

// for BIU0    
 m.s 	R1 = SDA0DM0_START;; 	     // KB for BIU0  
 m.s 	R2 = DataSize;; 	         // KS0    
 m.s 	R4 = BS;;	 	             // KI0

 m.s 	R5 = BS*DataSize;;	         // KS1
 m.s 	R6 = AColBSNum;;	         // KI1

 m.s 	R7 = AColNum*DataSize;;	     // KS2
 m.s 	R8 = ARowBSNum;;	         // KI2

 m.s 	R14=  BlockGran;; 	         // KG0
 m.s 	R15=  3;; 		             // KL0
	    
 m.s 	KB0 = R1;;
 m.s 	KS0 = R2;;
 m.s 	KI0 = R4;;

 m.s 	KB1 = R1;;
 m.s 	KS1 = R5;;
 m.s 	KI1 = R6;; 

 m.s 	KB2 = R1;;
 m.s 	KS2 = R7;; 		
 m.s 	KI2 = R8;;

 m.s 	KG0 = R14;;
 m.s 	KL0 = R15;;

// for BIU1    
 m.s 	R1 = SDA1DM0_START;; 	     // KB for BIU1  
 m.s 	R2 = DataSize;; 	         // KS4 
 m.s 	R3 = BColNum;;		         // KI4
 m.s 	R4 = BColNum*DataSize;;	     // KS5 
 m.s 	R5 = AColBSNum;;	         // KI5
 m.s 	R6 = 0;;		             // KS6 
 m.s 	R7 = ARowBSNum;;	         // KI6

 m.s 	R14=  BlockGran;; 	         // KG1
 m.s 	R15=  3;; 		             // KL1  
	
 m.s 	KB4 = R1;;
 m.s 	KS4 = R2;;
 m.s 	KI4 = R3;;  

 m.s 	KB5 = R1;;
 m.s 	KS5 = R4;; 		
 m.s 	KI5 = R5;;

 m.s 	KB6 = R1;;
 m.s 	KS6 = R6;;
 m.s 	KI6 = R7;; 


 m.s 	KG1 = R14;;
 m.s 	KL1 = R15;;

// for BIU2 
 m.s 	R1 = SDA2DM0_START;; 	     // KB for BIU2  
 m.s 	R2 = DataSize;;		         // KS8
 m.s 	R3 = CColNum;;		         // KI8
 m.s 	R4 = 0;;		             // KS9
 m.s 	R5 = AColBSNum;;	         // KI9
 m.s 	R6 = CColNum*DataSize;;	     // KS10
 m.s 	R7 = ARowBSNum;;	         // KI10

 m.s 	R14=  BlockGran;; 	         // KG2
 m.s 	R15=  3;; 		             // KL2

 m.s 	KB8 = R1;;
 m.s 	KS8 = R2;;
 m.s 	KI8 = R3;; 

 m.s 	KB9 = R1;;
 m.s 	KS9 = R4;; 		
 m.s 	KI9 = R5;;

 m.s 	KB10 = R1;;
 m.s 	KS10 = R6;;
 m.s 	KI10 = R7;;

 m.s 	KG2 = R14;;
 m.s 	KL2 = R15;;

//******************************************************************//
// Matrix Multiplication 
 m.s 	NOP;;
 m.s 	NOP;;
 m.s 	NOP;;
 m.s 	NOP;;
 m.s 	CallM Mat_Mul_Int ;;  
 m.s 	NOP;;
 m.s 	NOP;;
 m.s 	NOP;;
 m.s     NOP ;;
 m.s     NOP ;;
 m.s     NOP ;;
 m.s     NOP ;;
 m.s     SPU.Stop ;;




