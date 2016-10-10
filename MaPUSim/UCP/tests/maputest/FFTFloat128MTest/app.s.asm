//  Date: 2013-04-24
//  Name: app.s.asm
//  Author: tao.wang@ia.ac.cn
//  The configuration Unit for MPU for a compound float multiplication

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
    SDA2DM1_START = 0xe00000  
    SDASize       = 0x40000  
///////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
// the following need to modify
    ALen                = 128                          // the FFT Length  
    EpochNum            = 3                            // the total Epoch Number  
    BldEpochNum         = 2                            // Blanced Epoch Number   
    ABitRevLen          = 7                            // the bit reversed number = log2(ALen)
    UnBldEpoStageNum    = 1                            // stage number in the last epoch  
    UnBldEpoTwiOff      = 3 - UnBldEpoStageNum         // The twiddle offset in the last epoch 
    DataSizeGranMag     = 3 - UnBldEpoStageNum        // for unblanced FFT, the last epoch write to 8/(2^(3-UnBldEpoStageNum )) banks
    DataSizeMag         = 4                           // DataMag = 2^(3-UnBldEpoStageNum )
                                                      //for blanced FFT, DataMag=1
                                                      // for unblanced FFT, the last epoch has two stage, DataMag=2
                                                      // for unblanced FFT, the last epoch has one stage, DataMag=4
    
// when balanced FFT, the following is deleted  
//    UnBldEpoBCirNum       = 3                             // Twiddle stored circle for the unblanced epoch
//////////////////////////////////////////////////////////////////////// 



/////////////////////////////////////////////////////////////////////////
// the following need not changing    
    BS            = 8
    BSGran        = 3    
    BankNum       = BS
   
    BankNumGran   = BSGran
    BankSize      = SDASize >> BankNumGran
    BlockSize     = 64                       // the total Rd size, for Row Rd is 512/8     
    BlockGran     = 6                        //  Block Granularity = log2(BlockSize)
    DataSize      = BlockSize >> BSGran      //  Byte 1, Short 2, Word 4, Long 8
    DataSizeGran  = 3                        //  log2 (DataSize)
    
    ABSLen              = ALen >> BSGran
    BankGrpNum          = ABSLen >> BankNumGran     // the num of group in each Bank  
    BitRevAddrInc       = 1                         // the address increment in bit reversed mode

    
    GrpNum              = ABSLen 
    PipeGrp             = 20
    PipeGrpNum          = GrpNum / PipeGrp  
    PipeGrpTailNum      = GrpNum % PipeGrp 

    EpoTwiNum           = GrpNum * BSGran 

    //GrpEpochTwiSize     = BlockSize * BSGran              // size for the twiddle in the Group of each epoch    
    //PipeGrpEpochTwiSize = PipeGrp * GrpEpochTwiSize       //  the totle twiddle size in the piped epoch group
    //EpochTwiSize        = GrpNum  * GrpEpochTwiSize       // the totle twiddle size in one epoch            
    
    AEp0CirNum      = 1                         // the number of circle
    AEp1CirNum      = 2                         // the number of circle
    //BldEpoBCirNum   = 3                        // Twiddle stored circle for the blanced epoch
    //UnBldEpoBCirNum = 3                         // Twiddle stored circle for the unblanced epoch
    BCirNum         = 2
    CCirNum         = 1                         // Result stored in the array mode    

    // for MC Config 
    MIndexStart   = 0x0
    MStepMode     = 0x1 
    MLen          = ABSLen
    ISize         = ABSLen

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
 m.s    R2 = MConfig        ;;
 m.s    MC.r0 = R2          ;;
 m.s    MC.w0 = R2          ;;

// for Loop     
 m.s     R1 =  PipeGrpNum        ;; // KI12
 m.s     KI12 = R1              ;;
 

 // SHU0.T1, bit reverse and get B in [A0 B0 A1 B1]
 m.s     R1 = 0x23222120;;
 m.s     R2 = 0x27262524;;
 m.s     R3 = 0x27262524;;
 m.s     R4 = 0x23222120;;
 m.s     R5 = 0x33323130;;
 m.s     R6 = 0x37363534;;
 m.s     R7 = 0x37363534;;
 m.s     R8 = 0x33323130;;
 m.s     R9 = 0x2b2a2928;;
 m.s     R10= 0x2f2e2d2c;;
 m.s     R11= 0x2f2e2d2c;;
 m.s     R12= 0x2b2a2928;;
 m.s     R13= 0x3b3a3938;;
 m.s     R14= 0x3f3e3d3c;;
 m.s     R15= 0x3f3e3d3c;;
 m.s     R16= 0x3b3a3938;;

 m.s     SVR1[0] = R1  ;; 
 m.s     SVR1[1] = R2  ;; 
 m.s     SVR1[2] = R3  ;; 
 m.s     SVR1[3] = R4  ;; 
 m.s     SVR1[4] = R5  ;; 
 m.s     SVR1[5] = R6  ;; 
 m.s     SVR1[6] = R7  ;; 
 m.s     SVR1[7] = R8  ;; 
 m.s     SVR1[8] = R9  ;; 
 m.s     SVR1[9] = R10 ;; 
 m.s     SVR1[10] = R11;; 
 m.s     SVR1[11] = R12;; 
 m.s     SVR1[12] = R13;; 
 m.s     SVR1[13] = R14;; 
 m.s     SVR1[14] = R15;; 
 m.s     SVR1[15] = R16;; 
 m.s     SHU0.T1 = SVR1 ;; // bit reverse and get B

// SHU1.T1, bit reverse and get A in [A0 B0 A1 B1]

 m.s     R1 = 0x0       ;;
 m.s     R2 = 0x03020100;;
 m.s     R3 = 0x0       ;;
 m.s     R4 = 0x07060504;;
 m.s     R5 = 0x0       ;;
 m.s     R6 = 0x13121110;;
 m.s     R7 = 0x0       ;;
 m.s     R8 = 0x17161514;;
 m.s     R9 = 0x0       ;;
 m.s     R10= 0x0b0a0908;;
 m.s     R11= 0x0       ;;
 m.s     R12= 0x0f0e0d0c;;
 m.s     R13= 0x0       ;;
 m.s     R14= 0x1b1a1918;;
 m.s     R15= 0x0       ;;
 m.s     R16= 0x1f1e1d1c;;

 m.s     SVR1[0] = R1  ;; 
 m.s     SVR1[1] = R2  ;; 
 m.s     SVR1[2] = R3  ;; 
 m.s     SVR1[3] = R4  ;; 
 m.s     SVR1[4] = R5  ;; 
 m.s     SVR1[5] = R6  ;; 
 m.s     SVR1[6] = R7  ;; 
 m.s     SVR1[7] = R8  ;; 
 m.s     SVR1[8] = R9  ;; 
 m.s     SVR1[9] = R10 ;; 
 m.s     SVR1[10] = R11;; 
 m.s     SVR1[11] = R12;; 
 m.s     SVR1[12] = R13;; 
 m.s     SVR1[13] = R14;; 
 m.s     SVR1[14] = R15;; 
 m.s     SVR1[15] = R16;;       
     
 m.s     SHU1.T1 = SVR1 ;; // bit reverse and get A

// SHU0.T2, in group index and get B in [A0 B0 A1 B1]
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

 m.s     SVR1[0] = R1  ;; 
 m.s     SVR1[1] = R2  ;; 
 m.s     SVR1[2] = R3  ;; 
 m.s     SVR1[3] = R4  ;; 
 m.s     SVR1[4] = R5  ;; 
 m.s     SVR1[5] = R6  ;; 
 m.s     SVR1[6] = R7  ;; 
 m.s     SVR1[7] = R8  ;; 
 m.s     SVR1[8] = R9  ;; 
 m.s     SVR1[9] = R10 ;; 
 m.s     SVR1[10] = R11;; 
 m.s     SVR1[11] = R12;; 
 m.s     SVR1[12] = R13;; 
 m.s     SVR1[13] = R14;; 
 m.s     SVR1[14] = R15;; 
 m.s     SVR1[15] = R16;;    
 m.s     SHU0.T2 = SVR1 ;; // [A0 A0' B0 B0' A1 A1' B1 B1' ...] -> B=[B0 B0' B0' B0 B1 B1' B1' B1]

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
      
 m.s     SVR1[0] = R1  ;; 
 m.s     SVR1[1] = R2  ;; 
 m.s     SVR1[2] = R3  ;; 
 m.s     SVR1[3] = R4  ;; 
 m.s     SVR1[4] = R5  ;; 
 m.s     SVR1[5] = R6  ;; 
 m.s     SVR1[6] = R7  ;; 
 m.s     SVR1[7] = R8  ;; 
 m.s     SVR1[8] = R9  ;; 
 m.s     SVR1[9] = R10 ;; 
 m.s     SVR1[10] = R11;; 
 m.s     SVR1[11] = R12;; 
 m.s     SVR1[12] = R13;; 
 m.s     SVR1[13] = R14;; 
 m.s     SVR1[14] = R15;; 
 m.s     SVR1[15] = R16;;   
 m.s     SHU1.T2 = SVR1 ;; // [A0 A0' B0 B0' A1 A1' B1 B1' ...] -> A=[0 A0 0 A0'....] 
    

// SHU0.T3, get B in [A0 B0 A1 B1]
 m.s     R1 = 0x0b0a0908;;
 m.s     R2 = 0x0f0e0d0c;;
 m.s     R3 = 0x0f0e0d0c;;
 m.s     R4 = 0x0b0a0908;;
 m.s     R5 = 0x1b1a1918;;
 m.s     R6 = 0x1f1e1d1c;;
 m.s     R7 = 0x1f1e1d1c;;
 m.s     R8 = 0x1b1a1918;;
 m.s     R9 = 0x2b2a2928;;
 m.s     R10= 0x2f2e2d2c;;
 m.s     R11= 0x2f2e2d2c;;
 m.s     R12= 0x2b2a2928;;
 m.s     R13= 0x3b3a3938;;
 m.s     R14= 0x3f3e3d3c;;
 m.s     R15= 0x3f3e3d3c;;
 m.s     R16= 0x3b3a3938;;   

 m.s     SVR1[0] = R1  ;; 
 m.s     SVR1[1] = R2  ;; 
 m.s     SVR1[2] = R3  ;; 
 m.s     SVR1[3] = R4  ;; 
 m.s     SVR1[4] = R5  ;; 
 m.s     SVR1[5] = R6  ;; 
 m.s     SVR1[6] = R7  ;; 
 m.s     SVR1[7] = R8  ;; 
 m.s     SVR1[8] = R9  ;; 
 m.s     SVR1[9] = R10 ;; 
 m.s     SVR1[10] = R11;; 
 m.s     SVR1[11] = R12;; 
 m.s     SVR1[12] = R13;; 
 m.s     SVR1[13] = R14;; 
 m.s     SVR1[14] = R15;; 
 m.s     SVR1[15] = R16;;    
 m.s     SHU0.T3 = SVR1 ;; // [A0 A0' B0 B0' A1 A1' B1 B1' ...] -> B=[B0 B0' B0' B0 B1 B1' B1' B1]

// SHU1.T3, to get A in [A0 B0 A1 B1]  
 m.s     R1 = 0x0       ;;
 m.s     R2 = 0x03020100;;
 m.s     R3 = 0x0       ;;
 m.s     R4 = 0x07060504;;
 m.s     R5 = 0x0       ;;
 m.s     R6 = 0x13121110;;
 m.s     R7 = 0x0       ;;
 m.s     R8 = 0x17161514;;
 m.s     R9 = 0x0       ;;
 m.s     R10= 0x23222120;;
 m.s     R11= 0x0       ;;
 m.s     R12= 0x27262524;;
 m.s     R13= 0x0       ;;
 m.s     R14= 0x33323130;;
 m.s     R15= 0x0       ;;
 m.s     R16= 0x37363534;;
      
 m.s     SVR1[0] = R1  ;; 
 m.s     SVR1[1] = R2  ;; 
 m.s     SVR1[2] = R3  ;; 
 m.s     SVR1[3] = R4  ;; 
 m.s     SVR1[4] = R5  ;; 
 m.s     SVR1[5] = R6  ;; 
 m.s     SVR1[6] = R7  ;; 
 m.s     SVR1[7] = R8  ;; 
 m.s     SVR1[8] = R9  ;; 
 m.s     SVR1[9] = R10 ;; 
 m.s     SVR1[10] = R11;; 
 m.s     SVR1[11] = R12;; 
 m.s     SVR1[12] = R13;; 
 m.s     SVR1[13] = R14;; 
 m.s     SVR1[14] = R15;; 
 m.s     SVR1[15] = R16;;   
 m.s     SHU1.T3 = SVR1 ;; // [A0 A0' B0 B0' A1 A1' B1 B1' ...] -> A=[B0 B0' 0 0 B1 B1' 0 0]   


// configure the K register
// Read the date in Row mode   
// for BIU0    
 m.s     R1 =  SDA0DM0_START      ;; // KB for BIU0  
 m.s     R2 =  BitRevAddrInc      ;; // KS0
 m.s     R3 =  ABSLen             ;; // KC0
 m.s     R4 =  ABSLen             ;; // KI0    
 m.s     R14=  DataSizeGran       ;; // KG0
 m.s     R15=  AEp0CirNum         ;; // KL0
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
 m.s     R2 =  BlockSize          ;; // KS4
 m.s     R3 =  ABSLen             ;; // KI4
    // m.s R4 =  ABSLen             ;; // KE1
 m.s     R5 =  ABSLen * BlockSize;; // KS5
 m.s     R6 =  EpochNum         ;; // KI5
    // m.s R7 =  GrpNum  * BlockSize;; // KS6
    // m.s R8 =  BldEpochNum        ;; // KI6 

    
 m.s     R14=  BlockGran          ;; // KG1
 m.s     R15=  BCirNum            ;; // KL1
    
 m.s     KB4 = R1 ;;
 m.s     KS4 = R2 ;;
 m.s     KI4 = R3 ;;
    // m.s KE4 = R4 ;;
 m.s     KB5 = R1 ;;
 m.s     KS5 = R5 ;;
 m.s     KI5 = R6 ;;
    // m.s KB6 = R1 ;;
    // m.s KS6 = R7 ;;
    // m.s KI6 = R8 ;;
  
 m.s     KG1 = R14;;
 m.s     KL1 = R15;;

// for BIU2    
 m.s     R1 =  SDA2DM0_START      ;; // KB for BIU0  
 m.s     R2 =  DataSize           ;; // KS8
 m.s     R3 =  ABSLen             ;; // KC8
 m.s     R4 =  ABSLen             ;; // KI8   
 m.s     R14=  DataSizeGran       ;; // KG2
 m.s     R15=  CCirNum            ;; // KL2
    
 m.s     KB8 = R1 ;;
 m.s     KS8 = R2 ;;
  // m.s KC8 = R3 ;;
 m.s     KI8 = R4 ;;    
 m.s     KG2 = R14;;
 m.s     KL2 = R15;;


////////////////////////////////////////////////////////////////////////////////
//  FFT Epoch 0 call
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     CallM FFTSF128Ep0Test (B)  ;;  
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
/////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////
// Epoch 1: configure BIU0 and BIU2
// BIU0 Read the data from the precious BIU2 Write address
// BIU2 switch the write addres between SDA2DM0 and SDA2DM1
//  
// for BIU0    
 m.s     R1 =  SDA2DM0_START      ;; // KB for BIU0  
 m.s     R2 =  BlockSize          ;; // KS0
 m.s     R3 =  BankGrpNum         ;; // KC0
 m.s     R4 =  BankGrpNum         ;; // KI0  
 m.s     R5 =  BankSize           ;; // KS1
 m.s     R6 =  BankNum            ;; // KC1
 m.s     R7 =  BankNum            ;; // KI0
 m.s     R14=  BlockGran          ;; // KG0
 m.s     R15=  AEp1CirNum         ;; // KL0
  
    
 m.s     KB0 = R1 ;;
 m.s     KS0 = R2 ;;
 //   // m.s KC0 = R3 ;;
 m.s     KI0 = R4 ;;   
 m.s     KB1 = R1 ;;
 m.s     KS1 = R5 ;;
 //   // m.s KC1 = R6 ;;
 m.s     KI1 = R7 ;; 
 m.s     KG0 = R14;;
 m.s     KL0 = R15;;

    // for BIU2    
 m.s     R1 =  SDA2DM1_START      ;; // KB for BIU0  
 m.s     R2 =  DataSize           ;; // KS8
 m.s     R3 =  ABSLen             ;; // KC8
 m.s     R4 =  ABSLen             ;; // KI8   
 m.s     R14=  DataSizeGran       ;; // KG2
 m.s     R15=  CCirNum            ;; // KL2
    
 m.s     KB8 = R1 ;;
 m.s     KS8 = R2 ;;
  // m.s KC8 = R3 ;;
 m.s     KI8 = R4 ;; 
 m.s     KG2 = R14;;
 m.s     KL2 = R15;;

  
    // FFT Epoch 1 Call 
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     CallM FFTSF128Ep1Test (B)  ;; 
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
/////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////
// Epoch 2: configure BIU0 and BIU2
// BIU0 Read the data from the precious BIU2 Write address
// BIU2 switch the write addres between SDA2DM0 and SDA2DM1
//  
// for BIU0    
 m.s     R1 =  SDA2DM1_START      ;; // KB for BIU0  
 m.s     R2 =  BlockSize          ;; // KS0
 m.s     R3 =  BankGrpNum         ;; // KC0
 m.s     R4 =  BankGrpNum         ;; // KI0  
 m.s     R5 =  BankSize           ;; // KS1
 m.s     R6 =  BankNum            ;; // KC1
 m.s     R7 =  BankNum            ;; // KI0
 m.s     R14=  BlockGran          ;; // KG0
 m.s     R15=  AEp1CirNum         ;; // KL0
  
    
 m.s     KB0 = R1 ;;
 m.s     KS0 = R2 ;;
 //   // m.s KC0 = R3 ;;
 m.s     KI0 = R4 ;;   
 m.s     KB1 = R1 ;;
 m.s     KS1 = R5 ;;
 //   // m.s KC1 = R6 ;;
 m.s     KI1 = R7 ;; 
 m.s     KG0 = R14;;
 m.s     KL0 = R15;;

    /////////////////////////////////////////////////////////////////////////////
    // when balanced FFT, BIU1 need not changing in the lase epoch
    // for BIU1    
    // m.s R1 =  SDA1DM0_START + BldEpochNum * EpochTwiSize  + UnBldEpoTwiOff * BlockSize  ;; // KB for BIU1  
    // m.s R2 =  GrpEpochTwiSize    ;; // KS4
    // m.s R3 =  PipeGrp            ;; // KC4
    // m.s R4 =  PipeGrp            ;; // KI4   
    // m.s R5 =  BlockSize          ;; // KS5
    // m.s R6 =  UnBldEpoStageNum        ;; // KC5
    // m.s R7 =  UnBldEpoStageNum        ;; // KI5  
    // m.s R8 =  PipeGrpEpochTwiSize     ;; // KS6
    // m.s R9 =  PipeGrpNum              ;; // KC6
    // m.s R10=  PipeGrpNum              ;; // KI6    
    // m.s R14=  BlockGran               ;; // KG1
    // m.s R15=  UnBldEpoBCirNum         ;; // KL1
    
    // m.s KB4 = R1 ;;
    // m.s KS4 = R2 ;;
    // m.s KC4 = R3 ;;
    // m.s KI4 = R4 ;;   
    // m.s KB5 = R1 ;;
    // m.s KS5 = R5 ;;
    // m.s KC5 = R6 ;;
    // m.s KI5 = R7 ;;
    // m.s KB6 = R1 ;;
    // m.s KS6 = R8 ;;
    // m.s KC6 = R9 ;;
    // m.s KI6 = R10;;    
    // m.s KG1 = R14;;
    // m.s KL1 = R15;;
////////////////////////////////////////////////////////////////////////////////

/*   
     // for BIU2    
 m.s     R1 =  SDA2DM0_START      ;; // KB for BIU0  
 m.s     R2 =  DataSize           ;; // KS8
 m.s     R3 =  ABSLen             ;; // KC8
 m.s     R4 =  ABSLen             ;; // KI8 
 m.s     R14=  DataSizeGran       ;; // KG2
 m.s     R15=  CCirNum            ;; // KL2
    
 m.s     KB8 = R1 ;;
 m.s     KS8 = R2 ;;
  // m.s KC8 = R3 ;;
 m.s     KI8 = R4 ;;   
 m.s     KG2 = R14;;
 m.s     KL2 = R15;;
*/    
   // for BIU2    
 m.s     R1 =  SDA2DM0_START                    ;; // KB for BIU0  
 m.s     R2 =  DataSize * DataSizeMag           ;; // KS8
 m.s     R3 =  ABSLen                           ;; // KC8
 m.s     R4 =  ABSLen                           ;; // KI8    
 m.s     R14=  DataSizeGran + DataSizeGranMag   ;; // KG2
 m.s     R15=  CCirNum                          ;; // KL2
    
 m.s     KB8 = R1 ;;
 m.s     KS8 = R2 ;;
  // m.s KC8 = R3 ;;
 m.s     KI8 = R4 ;;    
 m.s     KG2 = R14;;
 m.s     KL2 = R15;;

//  FFT Epoch 2 Call
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     CallM FFTSF128Ep2Test (B)  ;; 
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     SPU.Stop ;;

////////////////////////////////////////////////////////////////////////////////






   




