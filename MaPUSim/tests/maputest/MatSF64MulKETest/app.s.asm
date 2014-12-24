//  Date: 2013-07-22
//  Name: app.s.asm
//  Author: tao.wang@ia.ac.cn
//  The configuration Unit for MPU for Matrix Mul Test
//  The input Matrix M*P*N can ben any size. The Matrix is manipulated in row mode.

// begin program code */
    .text
    .global _start
   

//*************************************************/ 
_start:



////////////////////////////////////////////////////////////////////////
// the following need to modify
// the input size
    ARowNum       = 0x41
    AColNum       = 0x42
    BRowNum       = 0x42
    BColNum       = 0x43
///////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// the following does not need to modify
// parameter definition  
    SDA0DM0_START = 0x400000
    SDA1DM0_START = 0x800000 
    SDA2DM0_START = 0xc00000
    SDASize       = 0x40000

    BS            = 16 
    HalfBS        = BS / 2
    BSGran        = 4
    BankNum       = BS
    BankNumGran   = 4 
    BankSize      = SDASize >> BankNumGran
   
    BlockSize     = 64      // the total Rd size, for Row Rd is 512/8     
    BlockGran     = 6       //  Block Granularity = log2(BlockSize)
    DataSize      = 4       //  Byte 1, Short 2, Word 4, Long 8
     
    BankCap       = ARowNum >> BankNumGran    // the num of Row in each Bank
    ACirNum       = 4                         // the number of circle
    BCirNum       = 3
    CCirNum       = 4
    
    ARowBSNum     = ARowNum >> BSGran 
    AColBSNum     = AColNum >> BSGran
    BRowBSNum     = BRowNum >> BSGran 
    BColBSNum     = BColNum >> BSGran

// configure for the no BS data, the tail data
    ARowTailNum   = ARowNum % BS             // A's Row tail number
    ARowTailEn    = ARowTailNum > 0          // A's Row tail number enable
    ARowBSCeilNum = ARowBSNum  + ARowTailEn  // A's Row ceil number

    AColTailNum   = AColNum % BS             // A's Col tail number
    AColTailEn    = AColTailNum > 0          // A's Col tail number enable
    AColBSCeilNum = AColBSNum  + AColTailEn  // A's Col ceil number

    BRowTailNum   = BRowNum % BS             // A's Row tail number
    BRowTailEn    = BRowTailNum > 0          // A's Row tail number enable
    BRowBSCeilNum = BRowBSNum + BRowTailEn   // B's Row ceil number

    BColTailNum   = BColNum % BS             // B's Col tail number
    BColTailEn    = BColTailNum > 0          // B's Col tail number enable
    BColBSCeilNum = BColBSNum  + BColTailEn  // B's Col ceil number
    
    
////////////////////////////////////////////////////////////////////////////////
   // get the MASK
   // A: M * P, in Row mode
   //    M: KE to dispose the un-BS size
   //    P: MASK to dispose the un-BS size
   // B: P * N, in Row mode
   //    P: ld without A++/K++ to dispose the un-BS size. ld the same address
   //    N: MASK to dispose the un-BS size
   // C: M * N, in Row mode
   //    M: KE to dispose the un-BS size
   //    N: MASK to dispose the un-BS size
   
   .if AColTailNum < HalfBS                    // A MASK The none BS col
        AShiftNum  = AColTailNum * DataSize
        AMaskLTemp = 0xffffffff << AShiftNum 
        AMaskL     = ~ AMaskLTemp
        AMaskH     = 0
    .else 
        AShiftNum  = AColTailNum *DataSize - 32
        AMaskHTemp = 0xffffffff << AShiftNum
        AMaskL     = 0xffffffff
        AMaskH     = ~ AMaskHTemp
    .endif

    .if BColTailNum < HalfBS                     // B MASK The none BS col
        BShiftNum  = BColTailNum * DataSize
        BMaskLTemp = 0xffffffff << BShiftNum 
        BMaskL     = ~ BMaskLTemp
        BMaskH     = 0
    .else 
        BShiftNum  = BColTailNum *DataSize - 32
        BMaskHTemp = 0xffffffff << BShiftNum
        BMaskL     = 0xffffffff
        BMaskH     = ~ BMaskHTemp
    .endif
    
     CMaskL = BMaskL
     CMaskH = BMaskH
///////////////////////////////////////////////////////////////////////////////////////////////
   
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
    // MC
    R2 = MConfig        ;;
    MC.w1 = R2          ;;
    MC.r0 = R2          ;;

    // for Loop
    R1 =  ARowNum                   ;; // KI12   
    R2 =  AColBSNum  + AColTailEn   ;; // KI13
    R3 =  BColBSNum  + BColTailEn   ;; // KI14
    NOP;;
    NOP;;
    KI12 = R1     ;;
    KI13 = R2     ;;
    KI14 = R3     ;;

    // SHU.T1
    R1 = 0x03020100   ;;
    SHU0.T1 = Vect R1 ;;

// configure the K register
// Read the date in Row mode   
// for BIU0    
    R1 =  SDA0DM0_START      ;; // KB for BIU0  
    R2 =  BankSize           ;; // KS0
    R3 =  ARowTailNum        ;; // KE0
    R4 =  BS                 ;; // KI0

    R5 =  AColNum * DataSize ;; // KS1    
    R7 =  ARowBSNum          ;; // KI1   
    
    R8 =  BlockSize          ;; // KS2   
    R10=  AColBSCeilNum      ;; // KI2

    R11=  0                  ;; // KS3   
    R13=  BColBSCeilNum      ;; // KI3
    
    R14=  BlockGran          ;; // KG0
    R15=  ACirNum            ;; // KL0
    R16=  AMaskL             ;; // KM0-L
    R17=  AMaskH             ;; // KM0-H
    
    KB0 = R1 ;;
    KS0 = R2 ;;
    KE0 = R3 ;;
    KI0 = R4 ;;

    KB1 = R1 ;;
    KS1 = R5 ;; 
    KI1 = R7 ;;

    KB2 = R1 ;;
    KS2 = R8 ;;
    KI2 = R10;;

    KB3 = R1 ;;
    KS3 = R11;; 
    KI3 = R13;;

    KG0 = R14;;
    KL0 = R15;;
    KM0 = R16;;
    KM0 = R17(H) ;;


// for BIU1, the BIU1 is related to M[I++],when the row number is not aligend to BS, it ld with no A/K
    R1 =  SDA1DM0_START      ;; // KB for BIU1  
    R2 =  BankSize           ;; // KS4   
    R3 =  BRowTailNum        ;; // KE4
    R4 =  BS                 ;; // KI4

    R5 =  BColNum * DataSize ;; // KS5    
    R7 =  BRowBSNum          ;; // KI5 
  
    R8 =  BlockSize          ;; // KS6   
    R10=  BColBSCeilNum      ;; // KI6 
  
    R11=  BlockGran          ;; // KG1
    R12=  BCirNum            ;; // KL1
    R16=  BMaskL             ;; // KM1-L
    R17=  BMaskH             ;; // KM1-H
    
    KB4 = R1 ;;
    KS4 = R2 ;; 
    KE4 = R3 ;;
    KI4 = R4 ;;

    KB5 = R1 ;;
    KS5 = R5 ;; 
    KI5 = R7 ;;

    KB6 = R1 ;;
    KS6 = R8 ;; 
    KI6 = R10;;

    KG1 = R11;;
    KL1 = R12;;
    KM1 = R16;;
    KM1 = R17(H) ;;

// for BIU2    
    R1 =  SDA2DM0_START      ;; // KB for BIU0  
    R2 =  BankSize           ;; // KS8
    R3 =  ARowTailNum        ;; // KE8
    R4 =  BS                 ;; // KI8

    R5 =  BColNum * DataSize ;; // KS9    
    R7 =  ARowBSNum          ;; // KI9 
  
    R8 =  0                  ;; // KS10   
    R10=  AColBSCeilNum      ;; // KI10

    R11=  BlockSize          ;; // KS11    
    R13=  BColBSCeilNum      ;; // KI11

    R14=  BlockGran          ;; // KG2
    R15=  CCirNum            ;; // KL2
    R16=  CMaskL             ;; // KM2-L
    R17=  CMaskH             ;; // KM2-H
    
    KB8 = R1 ;;
    KS8 = R2 ;;
    KE8 = R3 ;;
    KI8 = R4 ;;

    KB9 = R1 ;;
    KS9 = R5 ;;
    KI9 = R7 ;;

    KB10= R1 ;;
    KS10= R8 ;;
    KI10= R10;;

    KB11= R1 ;;
    KS11= R11;;
    KI11= R13;;

    KG2 = R14;;
    KL2 = R15;; 
    KM2 = R16 ;;
    KM2 = R17(H) ;;
  

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




   




