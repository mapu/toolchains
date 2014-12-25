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
 
    NOP;;

// configure the K register
// Read the date in Row mode   
// for BIU0    
    R1 =  SDA0DM0_START      ;; // KB for BIU0  
    R2 =  BitRevAddrInc      ;; // KS0
    R3 =  ABSLen             ;; // KC0
    R4 =  ABSLen             ;; // KI0    
    R14=  DataSizeGran       ;; // KG0
    R15=  ACirNum            ;; // KL0
    R16=  ABitRevLen         ;; // KM0
    
    KB0 = R1 ;;
    KS0 = R2 ;;
  //KC0 = R3 ;;
    KI0 = R4 ;;   
    KG0 = R14;;
    KL0 = R15;;
    KM0 = R16;;

// for BIU1    
    R1 =  SDA1DM0_START      ;; // KB for BIU1  
    R2 =  BSGran * BlockSize ;; // KS4
    R3 =  GrpNum             ;; // KC4
    R4 =  GrpNum             ;; // KI4   
    R5 =  BlockSize          ;; // KS5
    R6 =  BSGran             ;; // KC5
    R7 =  BSGran             ;; // KI5 
    R8 =  EpochTwiSize       ;; // KS6
    R9 =  EpochNum           ;; // KC6
    R10=  EpochNum           ;; // KI6   
    R14=  BlockGran          ;; // KG1
    R15=  BCirNum            ;; // KL1
    
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
    KG1 = R14;;
    KL1 = R15;;

// for BIU2    
    R1 =  SDA2DM0_START      ;; // KB for BIU2 
    R2 =  DataSize           ;; // KS8
    R3 =  BS                 ;; // KC8
    R4 =  BS                 ;; // KI8 
    //R5 =  0                  ;; // KS9
    //R6 =  EpochNum           ;; // KC9
    //R7 =  EpochNum           ;; // KI9
    R14=  DataSizeGran       ;; // KG2
    R15=  CCirNum            ;; // KL2
    
    KB8 = R1 ;;
    KS8 = R2 ;;
  //KC8 = R3 ;;
    KI8 = R4 ;;  
    //KB9 = R1 ;; 
    //KS9 = R5 ;;
    //KC9 = R6 ;;
    //KI9 = R7 ;;  
    KG2 = R14;;
    KL2 = R15;;

// for Loop
    R1 =  GrpNum              ;; // KI12      
    KI12 = R1             ;;
    

// MC
   R2 = MConfig        ;;
   MC.w0 = R2          ;;
   MC.r0 = R2          ;;
    

// SHU1.T1, in group index and get B in [A0 B0 A1 B1]
    R1 = 0x13121110;;
    R2 = 0x1b1a1918;;
    R3 = 0x1b1a1918;;
    R4 = 0x13121110;;
    R5 = 0x33323130;;
    R6 = 0x3b3a3938;;
    R7 = 0x3b3a3938;;
    R8 = 0x33323130;;
    R9 = 0x17161514;;
    R10= 0x1f1e1d1c;;
    R11= 0x1f1e1d1c;;
    R12= 0x17161514;;
    R13= 0x37363534;;
    R14= 0x3f3e3d3c;;
    R15= 0x3f3e3d3c;;
    R16= 0x37363534;;  
    J0 = 0x620000  ;;
    J1 = 0x4       ;;
    J2 = 0x620000  ;;
    J3 = 0x4       ;;
    [J0+=J1] = R1  ;;
    [J0+=J1] = R2  ;;
    [J0+=J1] = R3  ;;
    [J0+=J1] = R4  ;;
    [J0+=J1] = R5  ;;
    [J0+=J1] = R6  ;;
    [J0+=J1] = R7  ;;
    [J0+=J1] = R8  ;;
    [J0+=J1] = R9  ;;
    [J0+=J1] = R10 ;;
    [J0+=J1] = R11 ;;
    [J0+=J1] = R12 ;;
    [J0+=J1] = R13 ;;
    [J0+=J1] = R14 ;;
    [J0+=J1] = R15 ;;
    [J0+=J1] = R16 ;;
    SVR0 = [J2+J3] ;;   
    SHU1.T1 = SVR0 ;; 

// SHU1.T2, in group index and get A in [A0 B0 A1 B1]  
    R1 = 0x0       ;;
    R2 = 0x03020100;;
    R3 = 0x0       ;;
    R4 = 0x0b0a0908;;
    R5 = 0x0       ;;
    R6 = 0x23222120;;
    R7 = 0x0       ;;
    R8 = 0x2b2a2928;;
    R9 = 0x0       ;;
    R10= 0x07060504;;
    R11= 0x0       ;;
    R12= 0x0f0e0d0c;;
    R13= 0x0       ;;
    R14= 0x27262524;;
    R15= 0x0       ;;
    R16= 0x2f2e2d2c;;
      
    J0 = 0x620040  ;;
    J1 = 0x4       ;;
    J2 = 0x620040  ;;
    J3 = 0x4       ;;
    [J0+=J1] = R1  ;;
    [J0+=J1] = R2  ;;
    [J0+=J1] = R3  ;;
    [J0+=J1] = R4  ;;
    [J0+=J1] = R5  ;;
    [J0+=J1] = R6  ;;
    [J0+=J1] = R7  ;;
    [J0+=J1] = R8  ;;
    [J0+=J1] = R9  ;;
    [J0+=J1] = R10 ;;
    [J0+=J1] = R11 ;;
    [J0+=J1] = R12 ;;
    [J0+=J1] = R13 ;;
    [J0+=J1] = R14 ;;
    [J0+=J1] = R15 ;;
    [J0+=J1] = R16 ;;
    SVR1 = [J2+J3] ;;   
    SHU1.T2 = SVR1 ;; 

    // SHU0.T1, bit reverse and reverse in group index
    R1 = 0x03020100;;
    R2 = 0x0b0a0908;;
    R3 = 0x07060504;;
    R4 = 0x0f0e0d0c;;
    R5 = 0x23222120;;
    R6 = 0x2b2a2928;;
    R7 = 0x27262524;;
    R8 = 0x2f2e2d2c;;
    R9 = 0x13121110;;
    R10= 0x1b1a1918;;
    R11= 0x17161514;;
    R12= 0x1f1e1d1c;;
    R13= 0x33323130;;
    R14= 0x3b3a3938;;
    R15= 0x37363534;;
    R16= 0x3f3e3d3c;;
      
    J0 = 0x620080  ;;
    J1 = 0x4       ;;
    J2 = 0x620080  ;;
    J3 = 0x4       ;;
    [J0+=J1] = R1  ;;
    [J0+=J1] = R2  ;;
    [J0+=J1] = R3  ;;
    [J0+=J1] = R4  ;;
    [J0+=J1] = R5  ;;
    [J0+=J1] = R6  ;;
    [J0+=J1] = R7  ;;
    [J0+=J1] = R8  ;;
    [J0+=J1] = R9  ;;
    [J0+=J1] = R10 ;;
    [J0+=J1] = R11 ;;
    [J0+=J1] = R12 ;;
    [J0+=J1] = R13 ;;
    [J0+=J1] = R14 ;;
    [J0+=J1] = R15 ;;
    [J0+=J1] = R16 ;;
    SVR1 = [J2+J3] ;;
    SHU0.T1 = SVR1 ;; 


    // SHU0.T1, reverse in group index
    R1 = 0x03020100;;
    R2 = 0x23222120;;
    R3 = 0x07060504;;
    R4 = 0x27262524;;
    R5 = 0x0b0a0908;;
    R6 = 0x2b2a2928;;
    R7 = 0x0f0e0d0c;;
    R8 = 0x2f2e2d2c;;
    R9 = 0x13121110;;
    R10= 0x33323130;;
    R11= 0x17161514;;
    R12= 0x37363534;;
    R13= 0x1b1a1918;;
    R14= 0x3b3a3938;;
    R15= 0x1f1e1d1c;;
    R16= 0x3f3e3d3c;;
      
    J0 = 0x6200c0  ;;
    J1 = 0x4       ;;
    J2 = 0x6200c0  ;;
    J3 = 0x4       ;;
    [J0+=J1] = R1  ;;
    [J0+=J1] = R2  ;;
    [J0+=J1] = R3  ;;
    [J0+=J1] = R4  ;;
    [J0+=J1] = R5  ;;
    [J0+=J1] = R6  ;;
    [J0+=J1] = R7  ;;
    [J0+=J1] = R8  ;;
    [J0+=J1] = R9  ;;
    [J0+=J1] = R10 ;;
    [J0+=J1] = R11 ;;
    [J0+=J1] = R12 ;;
    [J0+=J1] = R13 ;;
    [J0+=J1] = R14 ;;
    [J0+=J1] = R15 ;;
    [J0+=J1] = R16 ;;
    SVR1 = [J2+J3] ;;
    SHU0.T2 = SVR1 ;; 
 

//  FFT Epoch 0
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    CallM FFTSF64Ep0Test (B)  ;;  
    NOP;;
    NOP;;
    NOP;;


    // configure the K register
// Read the date in Row mode   
// for BIU0    
    R1 =  SDA0DM0_START      ;; // KB for BIU0  
    R2 =  BankSize           ;; // KS0
    R3 =  ABSLen             ;; // KC0
    R4 =  ABSLen             ;; // KI0    
    R14=  BlockGran          ;; // KG0:6
    R15=  ACirNum            ;; // KL0
  
    
    KB0 = R1 ;;
    KS0 = R2 ;;
  //KC0 = R3 ;;
    KI0 = R4 ;;   
    KG0 = R14;;
    KL0 = R15;;

    // for BIU2    
    R1 =  SDA2DM1_START      ;; // KB for BIU2  
    R2 =  DataSize           ;; // KS8
    R3 =  BS                 ;; // KC8
    R4 =  BS                 ;; // KI8 
    //R5 =  0                  ;; // KS9
    //R6 =  EpochNum           ;; // KC9
    //R7 =  EpochNum           ;; // KI9
    R14=  DataSizeGran       ;; // KG2:3
    R15=  CCirNum            ;; // KL2
    
    KB8 = R1 ;;
    KS8 = R2 ;;
  //KC8 = R3 ;;
    KI8 = R4 ;;  
    //KB9 = R1 ;; 
    //KS9 = R5 ;;
    //KC9 = R6 ;;
    //KI9 = R7 ;;  
    KG2 = R14;;
    KL2 = R15;;


    //  FFT Epoch 1
    NOP;;
    NOP;;
    NOP;;
    NOP;;

   // copy the memroy: SDA2DM0-->SDA0DM0, copy 0x10000times * 32bit
   R31=0x1;;
//   R30=0x1000;;
   R30=0x8;;
   J31=SDA2DM0_START;;
   J30=SDA0DM0_START;;
//   J29=0x40;;
   J29=0x8000;;
   NOP;;
   NOP;;
   NOP;;
_label0:
    R30=R30-R31;;
    SVR0=[J31+=J29];;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    [J30+=J29]=SVR0;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    if R30, Jump _label0;;


    CallM FFTSF64Ep1Test (B)  ;;  
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    SPU.Stop ;;



   




