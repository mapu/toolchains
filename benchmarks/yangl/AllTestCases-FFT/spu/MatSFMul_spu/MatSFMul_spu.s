//  Date: 2013-07-22
//  Name: app.s.asm
//  Author: tao.wang@ia.ac.cn
//  The configuration Unit for MPU for Matrix Mul Test
//  The input Matrix M*P*N can ben any size. The Matrix is manipulated in row mode.

// begin program code */
    .text
    .global main
   

//*************************************************/ 
main:



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
 
 m.s     NOP;;
    // MC
 m.s     R2 = MConfig        ;;
 m.s     MC.w1 = R2          ;;
 m.s     MC.r0 = R2          ;;

    // for Loop
 m.s     R1 =  ARowNum                   ;; // KI12   
 m.s     R2 =  AColBSNum  + AColTailEn   ;; // KI13
 m.s     R3 =  BColBSNum  + BColTailEn   ;; // KI14
 m.s     NOP;;
 m.s     NOP;;
 m.s     KI12 = R1     ;;
 m.s     KI13 = R2     ;;
 m.s     KI14 = R3     ;;

    // SHU.T1
 m.s     R1 = 0x03020100   ;;
 m.s     SHU0.T1 = Vect R1 ;;

// configure the K register
// Read the date in Row mode   
// for BIU0    
 m.s     R1 =  SDA0DM0_START      ;; // KB for BIU0  
 m.s     R2 =  BankSize           ;; // KS0
 m.s     R3 =  ARowTailNum        ;; // KE0
 m.s     R4 =  BS                 ;; // KI0

 m.s     R5 =  AColNum * DataSize ;; // KS1    
 m.s     R7 =  ARowBSNum          ;; // KI1   
    
 m.s     R8 =  BlockSize          ;; // KS2   
 m.s     R10=  AColBSCeilNum      ;; // KI2

 m.s     R11=  0                  ;; // KS3   
 m.s     R13=  BColBSCeilNum      ;; // KI3
    
 m.s     R14=  BlockGran          ;; // KG0
 m.s     R15=  ACirNum            ;; // KL0
 m.s     R16=  AMaskL             ;; // KM0-L
 m.s     R17=  AMaskH             ;; // KM0-H
    
 m.s     KB0 = R1 ;;
 m.s     KS0 = R2 ;;
 m.s     KE0 = R3 ;;
 m.s     KI0 = R4 ;;

 m.s     KB1 = R1 ;;
 m.s     KS1 = R5 ;; 
 m.s     KI1 = R7 ;;

 m.s     KB2 = R1 ;;
 m.s     KS2 = R8 ;;
 m.s     KI2 = R10;;

 m.s     KB3 = R1 ;;
 m.s     KS3 = R11;; 
 m.s     KI3 = R13;;

 m.s     KG0 = R14;;
 m.s     KL0 = R15;;
 m.s     KM0 = R16;;
 m.s     KM0 = R17(H) ;;


// for BIU1, the BIU1 is related to M[I++],when the row number is not aligend to BS, it ld with no A/K
 m.s     R1 =  SDA1DM0_START      ;; // KB for BIU1  
 m.s     R2 =  BankSize           ;; // KS4   
 m.s     R3 =  BRowTailNum        ;; // KE4
 m.s     R4 =  BS                 ;; // KI4

 m.s     R5 =  BColNum * DataSize ;; // KS5    
 m.s     R7 =  BRowBSNum          ;; // KI5 
  
 m.s     R8 =  BlockSize          ;; // KS6   
 m.s     R10=  BColBSCeilNum      ;; // KI6 
  
 m.s     R11=  BlockGran          ;; // KG1
 m.s     R12=  BCirNum            ;; // KL1
 m.s     R16=  BMaskL             ;; // KM1-L
 m.s     R17=  BMaskH             ;; // KM1-H
    
 m.s     KB4 = R1 ;;
 m.s     KS4 = R2 ;; 
 m.s     KE4 = R3 ;;
 m.s     KI4 = R4 ;;

 m.s     KB5 = R1 ;;
 m.s     KS5 = R5 ;; 
 m.s     KI5 = R7 ;;

 m.s     KB6 = R1 ;;
 m.s     KS6 = R8 ;; 
 m.s     KI6 = R10;;

 m.s     KG1 = R11;;
 m.s     KL1 = R12;;
 m.s     KM1 = R16;;
 m.s     KM1 = R17(H) ;;

// for BIU2    
 m.s     R1 =  SDA2DM0_START      ;; // KB for BIU0  
 m.s     R2 =  BankSize           ;; // KS8
 m.s     R3 =  ARowTailNum        ;; // KE8
 m.s     R4 =  BS                 ;; // KI8

 m.s     R5 =  BColNum * DataSize ;; // KS9    
 m.s     R7 =  ARowBSNum          ;; // KI9 
  
 m.s     R8 =  0                  ;; // KS10   
 m.s     R10=  AColBSCeilNum      ;; // KI10

 m.s     R11=  BlockSize          ;; // KS11    
 m.s     R13=  BColBSCeilNum      ;; // KI11

 m.s     R14=  BlockGran          ;; // KG2
 m.s     R15=  CCirNum            ;; // KL2
 m.s     R16=  CMaskL             ;; // KM2-L
 m.s     R17=  CMaskH             ;; // KM2-H
    
 m.s     KB8 = R1 ;;
 m.s     KS8 = R2 ;;
 m.s     KE8 = R3 ;;
 m.s     KI8 = R4 ;;

 m.s     KB9 = R1 ;;
 m.s     KS9 = R5 ;;
 m.s     KI9 = R7 ;;

 m.s     KB10= R1 ;;
 m.s     KS10= R8 ;;
 m.s     KI10= R10;;

 m.s     KB11= R1 ;;
 m.s     KS11= R11;;
 m.s     KI11= R13;;

 m.s     KG2 = R14;;
 m.s     KL2 = R15;; 
 m.s     KM2 = R16 ;;
 m.s     KM2 = R17(H) ;;
  

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
 m.s     R2   =  0xC00000;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     Ch[4]   =  R2;;   //Addr
 m.s     R2   =  0x10C;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     Ch[5]   =  R2;;  //XNum
 m.s     R2   =  0x4000;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     Ch[6]   =  R2;;  //YStep
 m.s     R2   =  0x10 ;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     Ch[7]   =  R2;;  //YNum
 m.s     R2   =  0x10C ;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     Ch[8]   =  R2;;  //ZStep
 m.s     R1   =  0x41;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     Ch[9]   =  R1;; //YAllNum
 m.s     R2   =  0xE00000;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     Ch[10]   =  R2;;   //Addr
 m.s     R2   =  0x10C ;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     Ch[11]   =  R2;;  //XNum
 m.s     R2   =  0x10C ;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     Ch[12]   =  R2;;  //YStep
 m.s     R2   =  0x41 ;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     Ch[13]   =  R2;;  //YNum
 m.s     R2   =  0x300 ;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     Ch[14]   =  R2;;  //ZStep
 m.s     R2   =  0x41 ;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     Ch[15]   =  R2;; //AllNum
 m.s     R2   =  0x1 ;;         
 m.s     NOP;;
 m.s     NOP;;
 m.s     Ch[17]   =  R2;; //Cmd
 m.s     R2   =  0x1;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     Ch[18]   =  R2;;   //TagMask
 m.s     R2   =  0x2;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     Ch[20]   =  R2;;   //Update
 m.s     R2   =  Ch[21];;   //Status
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     SPU.Stop ;;




   




