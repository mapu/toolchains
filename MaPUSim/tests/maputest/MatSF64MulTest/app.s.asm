//  Date: 2013-03-25
//  Name: app.s.asm
//  Author: tao.wang@ia.ac.cn
//  The configuration Unit for MPU for a 16*16 Matrix Mul Test

// begin program code */
    .text
    .global _start

//*************************************************/ 
_start:

// parameter definition
////////////////////////////////////////////////////////////////////
// the following need not to modify    
    SDA0DM0_START = 0x400000
    SDA1DM0_START = 0x800000 
    SDA2DM0_START = 0xc00000
    SDASize       = 0x40000
///////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
// the following need to modify
    BS            = 16 
    BSGran        = 4
    BankNum       = BS
    BankNumGran   = 4 
    BankSize      = SDASize >> BankNumGran
   
    BlockSize     = 64      // the total Rd size, for Row Rd is 512/8     
    BlockGran     = 6       //  Block Granularity = log2(BlockSize)
    DataSize      = 4       //  Byte 1, Short 2, Word 4, Long 8

    ARowNum       = 0x40
    AColNum       = 0x40
    BRowNum       = 0x40
    BColNum       = 0x40
  
    ARowBSNum     = ARowNum >> BSGran 
    AColBSNum     = AColNum >> BSGran
    BRowBSNum     = BRowNum >> BSGran 
    BColBSNum     = BColNum >> BSGran

    BankCap       = ARowNum >> BankNumGran    // the num of Row in each Bank
    ACirNum       = 4                         // the number of circle
    BCirNum       = 3
    CCirNum       = 4

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
////////////////////////////////////////////////////////////////////////////
 
    NOP;;

// configure the K register
// Read the date in Row mode   
// for BIU0    
    R1 =  SDA0DM0_START      ;; // KB for BIU0  
    R2 =  BankSize           ;; // KS0
    R3 =  BS                 ;; // KC0
    R4 =  BS                 ;; // KI0
    R5 =  AColNum * DataSize ;; // KS1
    R6 =  ARowBSNum          ;; // KC1 
    R7 =  ARowBSNum          ;; // KI1   
    R8 =  BlockSize          ;; // KS2
    R9 =  AColBSNum          ;; // KC2
    R10=  AColBSNum          ;; // KI2
    R11=  0                  ;; // KS3
    R12=  BColBSNum          ;; // KC3
    R13=  BColBSNum          ;; // KI3
    R14=  BlockGran          ;; // KG0
    R15=  ACirNum            ;; // KL0
    
    KB0 = R1 ;;
    KS0 = R2 ;;
  //KC0 = R3 ;;
    KI0 = R4 ;;
    KB1 = R1 ;;
    KS1 = R5 ;;
  //KC1 = R6 ;;
    KI1 = R7 ;;
    KB2 = R1 ;;
    KS2 = R8 ;;
  //KC2 = R9 ;;
    KI2 = R10;;
    KB3 = R1 ;;
    KS3 = R11;;
  //KC3 = R12;;
    KI3 = R13;;
    KG0 = R14;;
    KL0 = R15;;

// for BIU1    
    R1 =  SDA1DM0_START      ;; // KB for BIU1  
    R2 =  BankSize           ;; // KS4
    R3 =  BS                 ;; // KC4
    R4 =  BS                 ;; // KI4
    R5 =  BColNum * DataSize ;; // KS5
    R6 =  BRowBSNum          ;; // KC5 
    R7 =  BRowBSNum          ;; // KI5   
    R8 =  BlockSize          ;; // KS6
    R9 =  BColBSNum          ;; // KC6
    R10=  BColBSNum          ;; // KI6   
    R11=  BlockGran          ;; // KG1
    R12=  BCirNum            ;; // KL1
    
    KB4 = R1 ;;
    KS4 = R2 ;;
  //KC4 = R3 ;;
    KI4 = R4 ;;
    KB5 = R1 ;;
    KS5 = R5 ;;
  //KC5 = R6 ;;
    KI5 = R7 ;;
    KB6 = R1 ;;
    KS6 = R8 ;;
  //KC6 = R9 ;;
    KI6 = R10;;
    KG1 = R11;;
    KL1 = R12;;

// for BIU2    
    R1 =  SDA2DM0_START      ;; // KB for BIU0  
    R2 =  BankSize           ;; // KS8
    R3 =  BS                 ;; // KC8
    R4 =  BS                 ;; // KI8
    R5 =  BColNum * DataSize ;; // KS9
    R6 =  ARowBSNum          ;; // KC9 
    R7 =  ARowBSNum          ;; // KI9   
    R8 =  0                  ;; // KS10
    R9 =  AColBSNum          ;; // KC10
    R10=  AColBSNum          ;; // KI10
    R11=  BlockSize          ;; // KS11
    R12=  BColBSNum          ;; // KC11
    R13=  BColBSNum          ;; // KI11
    R14=  BlockGran          ;; // KG2
    R15=  CCirNum            ;; // KL2
    
    KB8 = R1 ;;
    KS8 = R2 ;;
  //KC8 = R3 ;;
    KI8 = R4 ;;
    KB9 = R1 ;;
    KS9 = R5 ;;
  //KC9 = R6 ;;
    KI9 = R7 ;;
    KB10= R1 ;;
    KS10= R8 ;;
  //KC10= R9 ;;
    KI10= R10;;
    KB11= R1 ;;
    KS11= R11;;
  //KC11= R12;;
    KI11= R13;;
    KG2 = R14;;
    KL2 = R15;;

// for Loop
    R1 =  ARowNum              ;; // KI12   
    R2 =  AColBSNum            ;; // KI13
    R3 =  BColBSNum            ;; // KI14
    NOP;;
    NOP;;
    KI12 = R1     ;;
    KI13 = R2     ;;
    KI14 = R3     ;;

// SHU.T1
    R1 = 0x03020100 ;;
    SHU0.T1 = Vect R1 ;;

// MC
    R2 = MConfig        ;;
    MC.w1 = R2          ;;
    MC.r0 = R2          ;;
    
  

//  Matrix W Add Test
    NOP;;
    NOP;;
    NOP;;
    NOP;;
   CallM Mat_Mul_SF_Test (B)  ;;  




   
  
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    SPU.Stop ;;




   




