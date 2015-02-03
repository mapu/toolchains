//  Date: 2013-04-24
//  Name: app.s.asm
//  Author: tao.wang@ia.ac.cn
//  The configuration Unit for MPU for a compound float multiplication

// begin program code */
    .text
    .global _start
    .global _label0

//*************************************************/ 
_start:

// parameter definition
////////////////////////////////////////////////////////////////////
// the following need not to modify    
    SDA0DM0_START = 0x400000
    SDA1DM0_START = 0x800000 
    SDA2DM0_START = 0xc00000  
    SDA2DM1_START = 0xe00000  
    SDASize       = 0x40000  
///////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
// the data is stored in SDA in vector format
// the following need to modify
// Data information
    BS            = 8
    BSGran        = 3    
    
    BankNumGran   = BSGran
    BankSize      = SDASize >> BankNumGran
    BlockSize     = 64                       // the total Rd size, for Row Rd is 512/8     
    BlockGran     = 6                        //  Block Granularity = log2(BlockSize)
    DataSize      = BlockSize >> BSGran      //  Byte 1, Short 2, Word 4, Long 8
    DataSizeGran  = 3                        //  log2 (DataSize)

    ALen          = 64  
    ABSLen        = ALen >> BSGran  

    GrpNum        = 8                         // the last group number put to the pipe
    EpochNum      = 2     
    EpochTwiNum   = ALen * BSGran
    EpochTwiSize  = EpochTwiNum * DataSize

    //BankCap       = ARowNum >> BankNumGran    // the num of Row in each Bank
    ACirNum       = 1                         // the number of circle
    BCirNum       = 3
    CCirNum       = 1

    ABitRevLen    = 6
    BitRevAddrInc = ALen / 2

    // for MC Config   
    MIndexStart   = 0x0
    MStepMode     = 0x1 
    MLen          = 0x8
    ISize         = 0x8

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
 m.s     R2 =  BitRevAddrInc      ;; // KS0
 m.s     R3 =  ABSLen             ;; // KC0
 m.s     R4 =  ABSLen             ;; // KI0    
 m.s     R14=  DataSizeGran       ;; // KG0
 m.s     R15=  ACirNum            ;; // KL0
 m.s     R16=  ABitRevLen         ;; // KM0
    
 m.s     KB0 = R1 ;;
 m.s     KS0 = R2 ;;
 //   // m.s KC0 = R3 ;;
 m.s     KI0 = R4 ;;   
 m.s     KG0 = R14;;
 m.s     KL0 = R15;;
 m.s     KM0 = R16;;

// for BIU1    
 m.s     R1 =  SDA1DM0_START      ;; // KB for BIU1  
 m.s     R2 =  BSGran * BlockSize ;; // KS4
 m.s     R3 =  GrpNum             ;; // KC4
 m.s     R4 =  GrpNum             ;; // KI4   
 m.s     R5 =  BlockSize          ;; // KS5
 m.s     R6 =  BSGran             ;; // KC5
 m.s     R7 =  BSGran             ;; // KI5 
 m.s     R8 =  EpochTwiSize       ;; // KS6
 m.s     R9 =  EpochNum           ;; // KC6
 m.s     R10=  EpochNum           ;; // KI6   
 m.s     R14=  BlockGran          ;; // KG1
 m.s     R15=  BCirNum            ;; // KL1
    
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
 m.s     KG1 = R14;;
 m.s     KL1 = R15;;

// for BIU2    
 m.s     R1 =  SDA2DM0_START      ;; // KB for BIU2 
 m.s     R2 =  DataSize           ;; // KS8
 m.s     R3 =  BS                 ;; // KC8
 m.s     R4 =  BS                 ;; // KI8 
    // m.s R5 =  0                  ;; // KS9
    // m.s R6 =  EpochNum           ;; // KC9
    // m.s R7 =  EpochNum           ;; // KI9
 m.s     R14=  DataSizeGran       ;; // KG2
 m.s     R15=  CCirNum            ;; // KL2
    
 m.s     KB8 = R1 ;;
 m.s     KS8 = R2 ;;
  // m.s KC8 = R3 ;;
 m.s     KI8 = R4 ;;  
    // m.s KB9 = R1 ;; 
    // m.s KS9 = R5 ;;
    // m.s KC9 = R6 ;;
    // m.s KI9 = R7 ;;  
 m.s     KG2 = R14;;
 m.s     KL2 = R15;;

// for Loop
 m.s     R1 =  GrpNum              ;; // KI12      
 m.s     KI12 = R1             ;;
    

// MC
 m.s    R2 = MConfig        ;;
 m.s    MC.w0 = R2          ;;
 m.s    MC.r0 = R2          ;;
    

// SHU1.T1, in group index and get B in [A0 B0 A1 B1]
 m.s     R1 = 0x13121110;;
 m.s     R2 = 0x1b1a1918;;
 m.s     R3 = 0x1b1a1918;;
 m.s     R4 = 0x13121110;;
 m.s     R5 = 0x33323130;;
 m.s     R6 = 0x3b3a3938;;
 m.s     R7 = 0x3b3a3938;;
 m.s     R8 = 0x33323130;;
 m.s     R9 = 0x17161514;;
 m.s     R10= 0x1f1e1d1c;;
 m.s     R11= 0x1f1e1d1c;;
 m.s     R12= 0x17161514;;
 m.s     R13= 0x37363534;;
 m.s     R14= 0x3f3e3d3c;;
 m.s     R15= 0x3f3e3d3c;;
 m.s     R16= 0x37363534;;  
 m.s     J0 = 0x620000  ;;
 m.s     J1 = 0x4       ;;
 m.s     J2 = 0x620000  ;;
 m.s     J3 = 0x4       ;;
 m.s     [J0+=J1] = R1  ;;
 m.s     [J0+=J1] = R2  ;;
 m.s     [J0+=J1] = R3  ;;
 m.s     [J0+=J1] = R4  ;;
 m.s     [J0+=J1] = R5  ;;
 m.s     [J0+=J1] = R6  ;;
 m.s     [J0+=J1] = R7  ;;
 m.s     [J0+=J1] = R8  ;;
 m.s     [J0+=J1] = R9  ;;
 m.s     [J0+=J1] = R10 ;;
 m.s     [J0+=J1] = R11 ;;
 m.s     [J0+=J1] = R12 ;;
 m.s     [J0+=J1] = R13 ;;
 m.s     [J0+=J1] = R14 ;;
 m.s     [J0+=J1] = R15 ;;
 m.s     [J0+=J1] = R16 ;;
 m.s     SVR0 = [J2+J3] ;;   
 m.s     SHU1.T1 = SVR0 ;; 

// SHU1.T2, in group index and get A in [A0 B0 A1 B1]  
 m.s     R1 = 0x0       ;;
 m.s     R2 = 0x03020100;;
 m.s     R3 = 0x0       ;;
 m.s     R4 = 0x0b0a0908;;
 m.s     R5 = 0x0       ;;
 m.s     R6 = 0x23222120;;
 m.s     R7 = 0x0       ;;
 m.s     R8 = 0x2b2a2928;;
 m.s     R9 = 0x0       ;;
 m.s     R10= 0x07060504;;
 m.s     R11= 0x0       ;;
 m.s     R12= 0x0f0e0d0c;;
 m.s     R13= 0x0       ;;
 m.s     R14= 0x27262524;;
 m.s     R15= 0x0       ;;
 m.s     R16= 0x2f2e2d2c;;
      
 m.s     J0 = 0x620040  ;;
 m.s     J1 = 0x4       ;;
 m.s     J2 = 0x620040  ;;
 m.s     J3 = 0x4       ;;
 m.s     [J0+=J1] = R1  ;;
 m.s     [J0+=J1] = R2  ;;
 m.s     [J0+=J1] = R3  ;;
 m.s     [J0+=J1] = R4  ;;
 m.s     [J0+=J1] = R5  ;;
 m.s     [J0+=J1] = R6  ;;
 m.s     [J0+=J1] = R7  ;;
 m.s     [J0+=J1] = R8  ;;
 m.s     [J0+=J1] = R9  ;;
 m.s     [J0+=J1] = R10 ;;
 m.s     [J0+=J1] = R11 ;;
 m.s     [J0+=J1] = R12 ;;
 m.s     [J0+=J1] = R13 ;;
 m.s     [J0+=J1] = R14 ;;
 m.s     [J0+=J1] = R15 ;;
 m.s     [J0+=J1] = R16 ;;
 m.s     SVR1 = [J2+J3] ;;   
 m.s     SHU1.T2 = SVR1 ;; 

    // SHU0.T1, bit reverse and reverse in group index
 m.s     R1 = 0x03020100;;
 m.s     R2 = 0x0b0a0908;;
 m.s     R3 = 0x07060504;;
 m.s     R4 = 0x0f0e0d0c;;
 m.s     R5 = 0x23222120;;
 m.s     R6 = 0x2b2a2928;;
 m.s     R7 = 0x27262524;;
 m.s     R8 = 0x2f2e2d2c;;
 m.s     R9 = 0x13121110;;
 m.s     R10= 0x1b1a1918;;
 m.s     R11= 0x17161514;;
 m.s     R12= 0x1f1e1d1c;;
 m.s     R13= 0x33323130;;
 m.s     R14= 0x3b3a3938;;
 m.s     R15= 0x37363534;;
 m.s     R16= 0x3f3e3d3c;;
      
 m.s     J0 = 0x620080  ;;
 m.s     J1 = 0x4       ;;
 m.s     J2 = 0x620080  ;;
 m.s     J3 = 0x4       ;;
 m.s     [J0+=J1] = R1  ;;
 m.s     [J0+=J1] = R2  ;;
 m.s     [J0+=J1] = R3  ;;
 m.s     [J0+=J1] = R4  ;;
 m.s     [J0+=J1] = R5  ;;
 m.s     [J0+=J1] = R6  ;;
 m.s     [J0+=J1] = R7  ;;
 m.s     [J0+=J1] = R8  ;;
 m.s     [J0+=J1] = R9  ;;
 m.s     [J0+=J1] = R10 ;;
 m.s     [J0+=J1] = R11 ;;
 m.s     [J0+=J1] = R12 ;;
 m.s     [J0+=J1] = R13 ;;
 m.s     [J0+=J1] = R14 ;;
 m.s     [J0+=J1] = R15 ;;
 m.s     [J0+=J1] = R16 ;;
 m.s     SVR1 = [J2+J3] ;;
 m.s     SHU0.T1 = SVR1 ;; 


    // SHU0.T1, reverse in group index
 m.s     R1 = 0x03020100;;
 m.s     R2 = 0x23222120;;
 m.s     R3 = 0x07060504;;
 m.s     R4 = 0x27262524;;
 m.s     R5 = 0x0b0a0908;;
 m.s     R6 = 0x2b2a2928;;
 m.s     R7 = 0x0f0e0d0c;;
 m.s     R8 = 0x2f2e2d2c;;
 m.s     R9 = 0x13121110;;
 m.s     R10= 0x33323130;;
 m.s     R11= 0x17161514;;
 m.s     R12= 0x37363534;;
 m.s     R13= 0x1b1a1918;;
 m.s     R14= 0x3b3a3938;;
 m.s     R15= 0x1f1e1d1c;;
 m.s     R16= 0x3f3e3d3c;;
      
 m.s     J0 = 0x6200c0  ;;
 m.s     J1 = 0x4       ;;
 m.s     J2 = 0x6200c0  ;;
 m.s     J3 = 0x4       ;;
 m.s     [J0+=J1] = R1  ;;
 m.s     [J0+=J1] = R2  ;;
 m.s     [J0+=J1] = R3  ;;
 m.s     [J0+=J1] = R4  ;;
 m.s     [J0+=J1] = R5  ;;
 m.s     [J0+=J1] = R6  ;;
 m.s     [J0+=J1] = R7  ;;
 m.s     [J0+=J1] = R8  ;;
 m.s     [J0+=J1] = R9  ;;
 m.s     [J0+=J1] = R10 ;;
 m.s     [J0+=J1] = R11 ;;
 m.s     [J0+=J1] = R12 ;;
 m.s     [J0+=J1] = R13 ;;
 m.s     [J0+=J1] = R14 ;;
 m.s     [J0+=J1] = R15 ;;
 m.s     [J0+=J1] = R16 ;;
 m.s     SVR1 = [J2+J3] ;;
 m.s     SHU0.T2 = SVR1 ;; 
 

//  FFT Epoch 0
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     CallM FFTSF64Ep0Test (B)  ;;  
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;


    // configure the K register
// Read the date in Row mode   
// for BIU0    
 m.s     R1 =  SDA0DM0_START      ;; // KB for BIU0  
 m.s     R2 =  BankSize           ;; // KS0
 m.s     R3 =  ABSLen             ;; // KC0
 m.s     R4 =  ABSLen             ;; // KI0    
 m.s     R14=  BlockGran          ;; // KG0:6
 m.s     R15=  ACirNum            ;; // KL0
  
    
 m.s     KB0 = R1 ;;
 m.s     KS0 = R2 ;;
 //   // m.s KC0 = R3 ;;
 m.s     KI0 = R4 ;;   
 m.s     KG0 = R14;;
 m.s     KL0 = R15;;

    // for BIU2    
 m.s     R1 =  SDA2DM1_START      ;; // KB for BIU2  
 m.s     R2 =  DataSize           ;; // KS8
 m.s     R3 =  BS                 ;; // KC8
 m.s     R4 =  BS                 ;; // KI8 
    // m.s R5 =  0                  ;; // KS9
    // m.s R6 =  EpochNum           ;; // KC9
    // m.s R7 =  EpochNum           ;; // KI9
 m.s     R14=  DataSizeGran       ;; // KG2:3
 m.s     R15=  CCirNum            ;; // KL2
    
 m.s     KB8 = R1 ;;
 m.s     KS8 = R2 ;;
  // m.s KC8 = R3 ;;
 m.s     KI8 = R4 ;;  
    // m.s KB9 = R1 ;; 
    // m.s KS9 = R5 ;;
    // m.s KC9 = R6 ;;
    // m.s KI9 = R7 ;;  
 m.s     KG2 = R14;;
 m.s     KL2 = R15;;


    //  FFT Epoch 1
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;

   // copy the memroy: SDA2DM0-->SDA0DM0, copy 0x10000times * 32bit
 m.s    R31=0x1;;
// m.s    R30=0x1000;;
 m.s    R30=0x8;;
 m.s    J31=SDA2DM0_START;;
 m.s    J30=SDA0DM0_START;;
// m.s    J29=0x40;;
 m.s    J29=0x8000;;
 m.s    NOP;;
 m.s    NOP;;
 m.s    NOP;;
_label0:
 m.s     R30=R30-R31;;
 m.s     SVR0=[J31+=J29];;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     [J30+=J29]=SVR0;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     if R30, Jump _label0;;


 m.s     CallM FFTSF64Ep1Test (B)  ;;  
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     SPU.Stop ;;



   




