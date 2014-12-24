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
    ALen                = 4096                         // the FFT Length    
    BldEpochNum         = 4                            // Blanced Epoch Number   
    ABitRevLen          = 12                            // the bit reversed number = log2(ALen)
    UnBldEpoStageNum    = 3                            // stage number in the last epoch  
    UnBldEpoTwiOff      = 3 - UnBldEpoStageNum         // The twiddle offset in the last epoch 
    DataSizeGranMag     = 3 - UnBldEpoStageNum        // for unblanced FFT, the last epoch write to 8/(2^(3-UnBldEpoStageNum )) banks
    DataSizeMag         = 1                           // DataMag = 2^(3-UnBldEpoStageNum )
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
    BCirNum         = 3
    CCirNum         = 1                         // Result stored in the array mode    

    // for MC Config 
    MIndexStart   = 0x0
    MStepMode     = 0x1 
    MLen          = PipeGrp
    ISize         = PipeGrp

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
    R15=  AEp0CirNum         ;; // KL0
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
    R2 =  BlockSize          ;; // KS4
    R3 =  PipeGrp            ;; // KI4
    R4 =  PipeGrpTailNum     ;; // KE1
    R5 =  PipeGrp * BlockSize;; // KS5
    R6 =  PipeGrpNum         ;; // KI5
    R7 =  GrpNum  * BlockSize;; // KS6
    R8 =  BldEpochNum        ;; // KI6 

    
    R14=  BlockGran          ;; // KG1
    R15=  BCirNum            ;; // KL1
    
    KB4 = R1 ;;
    KS4 = R2 ;;
    KI4 = R3 ;;
    KE4 = R4 ;;
    KB5 = R1 ;;
    KS5 = R5 ;;
    KI5 = R6 ;;
    KB6 = R1 ;;
    KS6 = R7 ;;
    KI6 = R8 ;;
  
    KG1 = R14;;
    KL1 = R15;;

// for BIU2    
    R1 =  SDA2DM0_START      ;; // KB for BIU0  
    R2 =  DataSize           ;; // KS8
    R3 =  ABSLen             ;; // KC8
    R4 =  ABSLen             ;; // KI8   
    R14=  DataSizeGran       ;; // KG2
    R15=  CCirNum            ;; // KL2
    
    KB8 = R1 ;;
    KS8 = R2 ;;
  //KC8 = R3 ;;
    KI8 = R4 ;;    
    KG2 = R14;;
    KL2 = R15;;

// for Loop     
    R1 =  PipeGrpNum           ;; // KI12
    KI12 = R1              ;;
   
    

// MC
   R2 = MConfig        ;;
   MC.r0 = R2          ;;
   MC.w0 = R2          ;;

 // SHU0.T1, bit reverse and get B in [A0 B0 A1 B1]

    R1 = 0x23222120;;
    R2 = 0x27262524;;
    R3 = 0x27262524;;
    R4 = 0x23222120;;
    R5 = 0x33323130;;
    R6 = 0x37363534;;
    R7 = 0x37363534;;
    R8 = 0x33323130;;
    R9 = 0x2b2a2928;;
    R10= 0x2f2e2d2c;;
    R11= 0x2f2e2d2c;;
    R12= 0x2b2a2928;;
    R13= 0x3b3a3938;;
    R14= 0x3f3e3d3c;;
    R15= 0x3f3e3d3c;;
    R16= 0x3b3a3938;;

    SVR1[0] = R1  ;; 
    SVR1[1] = R2  ;; 
    SVR1[2] = R3  ;; 
    SVR1[3] = R4  ;; 
    SVR1[4] = R5  ;; 
    SVR1[5] = R6  ;; 
    SVR1[6] = R7  ;; 
    SVR1[7] = R8  ;; 
    SVR1[8] = R9  ;; 
    SVR1[9] = R10 ;; 
    SVR1[10] = R11;; 
    SVR1[11] = R12;; 
    SVR1[12] = R13;; 
    SVR1[13] = R14;; 
    SVR1[14] = R15;; 
    SVR1[15] = R16;; 
    SHU0.T1 = SVR1 ;; // bit reverse and get B

// SHU1.T1, bit reverse and get A in [A0 B0 A1 B1]

    R1 = 0x0       ;;
    R2 = 0x03020100;;
    R3 = 0x0       ;;
    R4 = 0x07060504;;
    R5 = 0x0       ;;
    R6 = 0x13121110;;
    R7 = 0x0       ;;
    R8 = 0x17161514;;
    R9 = 0x0       ;;
    R10= 0x0b0a0908;;
    R11= 0x0       ;;
    R12= 0x0f0e0d0c;;
    R13= 0x0       ;;
    R14= 0x1b1a1918;;
    R15= 0x0       ;;
    R16= 0x1f1e1d1c;;

    SVR1[0] = R1  ;; 
    SVR1[1] = R2  ;; 
    SVR1[2] = R3  ;; 
    SVR1[3] = R4  ;; 
    SVR1[4] = R5  ;; 
    SVR1[5] = R6  ;; 
    SVR1[6] = R7  ;; 
    SVR1[7] = R8  ;; 
    SVR1[8] = R9  ;; 
    SVR1[9] = R10 ;; 
    SVR1[10] = R11;; 
    SVR1[11] = R12;; 
    SVR1[12] = R13;; 
    SVR1[13] = R14;; 
    SVR1[14] = R15;; 
    SVR1[15] = R16;;       
     
    SHU1.T1 = SVR1 ;; // bit reverse and get A

// SHU0.T2, in group index and get B in [A0 B0 A1 B1]
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

    SVR1[0] = R1  ;; 
    SVR1[1] = R2  ;; 
    SVR1[2] = R3  ;; 
    SVR1[3] = R4  ;; 
    SVR1[4] = R5  ;; 
    SVR1[5] = R6  ;; 
    SVR1[6] = R7  ;; 
    SVR1[7] = R8  ;; 
    SVR1[8] = R9  ;; 
    SVR1[9] = R10 ;; 
    SVR1[10] = R11;; 
    SVR1[11] = R12;; 
    SVR1[12] = R13;; 
    SVR1[13] = R14;; 
    SVR1[14] = R15;; 
    SVR1[15] = R16;;    
    SHU0.T2 = SVR1 ;; // [A0 A0' B0 B0' A1 A1' B1 B1' ...] -> B=[B0 B0' B0' B0 B1 B1' B1' B1]

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
      
    SVR1[0] = R1  ;; 
    SVR1[1] = R2  ;; 
    SVR1[2] = R3  ;; 
    SVR1[3] = R4  ;; 
    SVR1[4] = R5  ;; 
    SVR1[5] = R6  ;; 
    SVR1[6] = R7  ;; 
    SVR1[7] = R8  ;; 
    SVR1[8] = R9  ;; 
    SVR1[9] = R10 ;; 
    SVR1[10] = R11;; 
    SVR1[11] = R12;; 
    SVR1[12] = R13;; 
    SVR1[13] = R14;; 
    SVR1[14] = R15;; 
    SVR1[15] = R16;;   
    SHU1.T2 = SVR1 ;; // [A0 A0' B0 B0' A1 A1' B1 B1' ...] -> A=[0 A0 0 A0'....] 
    

// SHU0.T3, get B in [A0 B0 A1 B1]

    R1 = 0x0b0a0908;;
    R2 = 0x0f0e0d0c;;
    R3 = 0x0f0e0d0c;;
    R4 = 0x0b0a0908;;
    R5 = 0x1b1a1918;;
    R6 = 0x1f1e1d1c;;
    R7 = 0x1f1e1d1c;;
    R8 = 0x1b1a1918;;
    R9 = 0x2b2a2928;;
    R10= 0x2f2e2d2c;;
    R11= 0x2f2e2d2c;;
    R12= 0x2b2a2928;;
    R13= 0x3b3a3938;;
    R14= 0x3f3e3d3c;;
    R15= 0x3f3e3d3c;;
    R16= 0x3b3a3938;;   

    SVR1[0] = R1  ;; 
    SVR1[1] = R2  ;; 
    SVR1[2] = R3  ;; 
    SVR1[3] = R4  ;; 
    SVR1[4] = R5  ;; 
    SVR1[5] = R6  ;; 
    SVR1[6] = R7  ;; 
    SVR1[7] = R8  ;; 
    SVR1[8] = R9  ;; 
    SVR1[9] = R10 ;; 
    SVR1[10] = R11;; 
    SVR1[11] = R12;; 
    SVR1[12] = R13;; 
    SVR1[13] = R14;; 
    SVR1[14] = R15;; 
    SVR1[15] = R16;;    
    SHU0.T3 = SVR1 ;; // [A0 A0' B0 B0' A1 A1' B1 B1' ...] -> B=[B0 B0' B0' B0 B1 B1' B1' B1]

// SHU1.T3, to get A in [A0 B0 A1 B1]  
    R1 = 0x0       ;;
    R2 = 0x03020100;;
    R3 = 0x0       ;;
    R4 = 0x07060504;;
    R5 = 0x0       ;;
    R6 = 0x13121110;;
    R7 = 0x0       ;;
    R8 = 0x17161514;;
    R9 = 0x0       ;;
    R10= 0x23222120;;
    R11= 0x0       ;;
    R12= 0x27262524;;
    R13= 0x0       ;;
    R14= 0x33323130;;
    R15= 0x0       ;;
    R16= 0x37363534;;
      
    SVR1[0] = R1  ;; 
    SVR1[1] = R2  ;; 
    SVR1[2] = R3  ;; 
    SVR1[3] = R4  ;; 
    SVR1[4] = R5  ;; 
    SVR1[5] = R6  ;; 
    SVR1[6] = R7  ;; 
    SVR1[7] = R8  ;; 
    SVR1[8] = R9  ;; 
    SVR1[9] = R10 ;; 
    SVR1[10] = R11;; 
    SVR1[11] = R12;; 
    SVR1[12] = R13;; 
    SVR1[13] = R14;; 
    SVR1[14] = R15;; 
    SVR1[15] = R16;;   
    SHU1.T3 = SVR1 ;; // [A0 A0' B0 B0' A1 A1' B1 B1' ...] -> A=[B0 B0' 0 0 B1 B1' 0 0]   

////////////////////////////////////////////////////////////////////////////////
//  FFT Epoch 0 call
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    CallM FFTSF4096Ep0Test (B)  ;;  
    NOP;;
    NOP;;
    NOP;;
/////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////
// Epoch 1: configure BIU0 and BIU2
// BIU0 Read the data from the precious BIU2 Write address
// BIU2 switch the write addres between SDA2DM0 and SDA2DM1
//  
// for BIU0    
    R1 =  SDA2DM0_START      ;; // KB for BIU0  
    R2 =  BlockSize          ;; // KS0
    R3 =  BankGrpNum         ;; // KC0
    R4 =  BankGrpNum         ;; // KI0  
    R5 =  BankSize           ;; // KS1
    R6 =  BankNum            ;; // KC1
    R7 =  BankNum            ;; // KI0
    R14=  BlockGran          ;; // KG0
    R15=  AEp1CirNum         ;; // KL0
  
    
    KB0 = R1 ;;
    KS0 = R2 ;;
  //KC0 = R3 ;;
    KI0 = R4 ;;   
    KB1 = R1 ;;
    KS1 = R5 ;;
  //KC1 = R6 ;;
    KI1 = R7 ;; 
    KG0 = R14;;
    KL0 = R15;;

    // for BIU2    
    R1 =  SDA2DM1_START      ;; // KB for BIU0  
    R2 =  DataSize           ;; // KS8
    R3 =  ABSLen             ;; // KC8
    R4 =  ABSLen             ;; // KI8   
    R14=  DataSizeGran       ;; // KG2
    R15=  CCirNum            ;; // KL2
    
    KB8 = R1 ;;
    KS8 = R2 ;;
  //KC8 = R3 ;;
    KI8 = R4 ;; 
    KG2 = R14;;
    KL2 = R15;;

    // MC
    R2 = MConfig        ;;
    MC.r0 = R2          ;;
    MC.w0 = R2          ;;

    // FFT Epoch 1 Call 
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    CallM FFTSF4096Ep1Test (B)  ;; 
    NOP;;
    NOP;;
    NOP;;
/////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////
// Epoch 2: configure BIU0 and BIU2
// BIU0 Read the data from the precious BIU2 Write address
// BIU2 switch the write addres between SDA2DM0 and SDA2DM1
//  
// for BIU0    
    R1 =  SDA2DM1_START      ;; // KB for BIU0  
    R2 =  BlockSize          ;; // KS0
    R3 =  BankGrpNum         ;; // KC0
    R4 =  BankGrpNum         ;; // KI0  
    R5 =  BankSize           ;; // KS1
    R6 =  BankNum            ;; // KC1
    R7 =  BankNum            ;; // KI0
    R14=  BlockGran          ;; // KG0
    R15=  AEp1CirNum         ;; // KL0
  
    
    KB0 = R1 ;;
    KS0 = R2 ;;
  //KC0 = R3 ;;
    KI0 = R4 ;;   
    KB1 = R1 ;;
    KS1 = R5 ;;
  //KC1 = R6 ;;
    KI1 = R7 ;; 
    KG0 = R14;;
    KL0 = R15;;
  
    // for BIU2    
    R1 =  SDA2DM0_START      ;; // KB for BIU0  
    R2 =  DataSize           ;; // KS8
    R3 =  ABSLen             ;; // KC8
    R4 =  ABSLen             ;; // KI8 
    R14=  DataSizeGran       ;; // KG2
    R15=  CCirNum            ;; // KL2
    
    KB8 = R1 ;;
    KS8 = R2 ;;
  //KC8 = R3 ;;
    KI8 = R4 ;;   
    KG2 = R14;;
    KL2 = R15;;

    // MC
    R2 = MConfig        ;;
    MC.r0 = R2          ;;
    MC.w0 = R2          ;;

//  FFT Epoch 2 Call
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    CallM FFTSF4096Ep2Test (B)  ;; 
    NOP;;
    NOP;;
    NOP;;
////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
// Epoch 2: configure BIU0 and BIU2
// BIU0 Read the data from the precious BIU2 Write address
// BIU2 switch the write addres between SDA2DM0 and SDA2DM1
//  
// for BIU0    
    R1 =  SDA2DM0_START      ;; // KB for BIU0  
    R2 =  BlockSize          ;; // KS0
    R3 =  BankGrpNum         ;; // KC0
    R4 =  BankGrpNum         ;; // KI0  
    R5 =  BankSize           ;; // KS1
    R6 =  BankNum            ;; // KC1
    R7 =  BankNum            ;; // KI0
    R14=  BlockGran          ;; // KG0
    R15=  AEp1CirNum         ;; // KL0
  
    
    KB0 = R1 ;;
    KS0 = R2 ;;
  //KC0 = R3 ;;
    KI0 = R4 ;;   
    KB1 = R1 ;;
    KS1 = R5 ;;
  //KC1 = R6 ;;
    KI1 = R7 ;; 
    KG0 = R14;;
    KL0 = R15;;
  
    // for BIU2    
    R1 =  SDA2DM1_START      ;; // KB for BIU0  
    R2 =  DataSize           ;; // KS8
    R3 =  ABSLen             ;; // KC8
    R4 =  ABSLen             ;; // KI8 
    R14=  DataSizeGran       ;; // KG2
    R15=  CCirNum            ;; // KL2
    
    KB8 = R1 ;;
    KS8 = R2 ;;
  //KC8 = R3 ;;
    KI8 = R4 ;;   
    KG2 = R14;;
    KL2 = R15;;

    // MC
    R2 = MConfig        ;;
    MC.r0 = R2          ;;
    MC.w0 = R2          ;;

//  FFT Epoch 3 Call
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    CallM FFTSF4096Ep3Test (B)  ;; 
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    SPU.Stop ;;

////////////////////////////////////////////////////////////////////////////////







   




