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
 
 m.s     NOP;;

// configure the K register
// Read the date in Row mode   
// for BIU0    
 m.s     R1 =  SDA0DM0_START      ;; // KB for BIU0  
 m.s     R2 =  BankSize           ;; // KS0
 m.s     R3 =  BS                 ;; // KC0
 m.s     R4 =  BS                 ;; // KI0
 m.s     R5 =  AColNum * DataSize ;; // KS1
 m.s     R6 =  ARowBSNum          ;; // KC1 
 m.s     R7 =  ARowBSNum          ;; // KI1   
 m.s     R8 =  BlockSize          ;; // KS2
 m.s     R9 =  AColBSNum          ;; // KC2
 m.s     R10=  AColBSNum          ;; // KI2
 m.s     R11=  0                  ;; // KS3
 m.s     R12=  BColBSNum          ;; // KC3
 m.s     R13=  BColBSNum          ;; // KI3
 m.s     R14=  BlockGran          ;; // KG0
 m.s     R15=  ACirNum            ;; // KL0
    
 m.s     KB0 = R1 ;;
 m.s     KS0 = R2 ;;
 //   // m.s KC0 = R3 ;;
 m.s     KI0 = R4 ;;
 m.s     KB1 = R1 ;;
 m.s     KS1 = R5 ;;
 //   // m.s KC1 = R6 ;;
 m.s     KI1 = R7 ;;
 m.s     KB2 = R1 ;;
 m.s     KS2 = R8 ;;
 //   // m.s KC2 = R9 ;;
 m.s     KI2 = R10;;
 m.s     KB3 = R1 ;;
 m.s     KS3 = R11;;
 //   // m.s KC3 = R12;;
 m.s     KI3 = R13;;
 m.s     KG0 = R14;;
 m.s     KL0 = R15;;

// for BIU1    
 m.s     R1 =  SDA1DM0_START      ;; // KB for BIU1  
 m.s     R2 =  BankSize           ;; // KS4
 m.s     R3 =  BS                 ;; // KC4
 m.s     R4 =  BS                 ;; // KI4
 m.s     R5 =  BColNum * DataSize ;; // KS5
 m.s     R6 =  BRowBSNum          ;; // KC5 
 m.s     R7 =  BRowBSNum          ;; // KI5   
 m.s     R8 =  BlockSize          ;; // KS6
 m.s     R9 =  BColBSNum          ;; // KC6
 m.s     R10=  BColBSNum          ;; // KI6   
 m.s     R11=  BlockGran          ;; // KG1
 m.s     R12=  BCirNum            ;; // KL1
    
 m.s     KB4 = R1 ;;
 m.s     KS4 = R2 ;;
  // m.s KC4 = R3 ;;
 m.s     KI4 = R4 ;;
 m.s     KB5 = R1 ;;
 m.s     KS5 = R5 ;;
  // m.s KC5 = R6 ;;
 m.s     KI5 = R7 ;;
 m.s     KB6 = R1 ;;
 m.s     KS6 = R8 ;;
  // m.s KC6 = R9 ;;
 m.s     KI6 = R10;;
 m.s     KG1 = R11;;
 m.s     KL1 = R12;;

// for BIU2    
 m.s     R1 =  SDA2DM0_START      ;; // KB for BIU0  
 m.s     R2 =  BankSize           ;; // KS8
 m.s     R3 =  BS                 ;; // KC8
 m.s     R4 =  BS                 ;; // KI8
 m.s     R5 =  BColNum * DataSize ;; // KS9
 m.s     R6 =  ARowBSNum          ;; // KC9 
 m.s     R7 =  ARowBSNum          ;; // KI9   
 m.s     R8 =  0                  ;; // KS10
 m.s     R9 =  AColBSNum          ;; // KC10
 m.s     R10=  AColBSNum          ;; // KI10
 m.s     R11=  BlockSize          ;; // KS11
 m.s     R12=  BColBSNum          ;; // KC11
 m.s     R13=  BColBSNum          ;; // KI11
 m.s     R14=  BlockGran          ;; // KG2
 m.s     R15=  CCirNum            ;; // KL2
    
 m.s     KB8 = R1 ;;
 m.s     KS8 = R2 ;;
  // m.s KC8 = R3 ;;
 m.s     KI8 = R4 ;;
 m.s     KB9 = R1 ;;
 m.s     KS9 = R5 ;;
  // m.s KC9 = R6 ;;
 m.s     KI9 = R7 ;;
 m.s     KB10= R1 ;;
 m.s     KS10= R8 ;;
 //   // m.s KC10= R9 ;;
 m.s     KI10= R10;;
 m.s     KB11= R1 ;;
 m.s     KS11= R11;;
 //   // m.s KC11= R12;;
 m.s     KI11= R13;;
 m.s     KG2 = R14;;
 m.s     KL2 = R15;;

// for Loop
 m.s     R1 =  ARowNum              ;; // KI12   
 m.s     R2 =  AColBSNum            ;; // KI13
 m.s     R3 =  BColBSNum            ;; // KI14
 m.s     NOP;;
 m.s     NOP;;
 m.s     KI12 = R1     ;;
 m.s     KI13 = R2     ;;
 m.s     KI14 = R3     ;;

// SHU.T1
 m.s     R1 = 0x03020100 ;;
 m.s     SHU0.T1 = Vect R1 ;;

// MC
 m.s     R2 = MConfig        ;;
 m.s     MC.w1 = R2          ;;
 m.s     MC.r0 = R2          ;;
    
  

//  Matrix W Add Test
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s    CallM Mat_Mul_SF_Test (B)  ;;  




   
  
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     SPU.Stop ;;




   




