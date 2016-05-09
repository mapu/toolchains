//  Date: 2013-04-24
//  Name: app.s.asm
//  Author: tao.wang@ia.ac.cn
//  The configuration Unit for MPU for a compound float multiplication

// begin program code */
    .text
    .global main

//*************************************************/ 
main:

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
// the following need to modify
    ARowNum       = 256
    AColNum       = 256
///////////////////////////////////////////////////////////////////////  

    BS            = 64  
    BankNum       = BS    
    //BankSize      = SDASize / BankNumGran
    BlockSize     = 64                        // the total Rd size, for Row Rd is 512/8  
    BlockGran     = 6                         // log2(BlockSize)    
    DataSize      = 1                         //  Byte 1, Short 2, Word 4, Long 8
    DataSizeGran  = 0

    CoffRowNum    = 7
    CoffColNum    = 7
    CoffBSNum     = 1
    FilterStep    = 2
   
    PadARowNum    = ARowNum + CoffRowNum - 1 
    PadAColNum    = AColNum + CoffColNum - 1   

    ARowTailNum   = ARowNum % BS
    ARowTailEn    = ARowTailNum != 0    
    ARowBSNum     = ARowNum  / BS + ARowTailEn

    CRowNum       = ARowNum
    CColNum       = AColNum
    CRowTailNum   = ARowTailNum
    CRowTailEn    = ARowTailEn
    CRowBSNum     = ARowBSNum
    
    ACirNum       = 3                         // the number of circle
    BCirNum       = 1
    CCirNum       = 2

    // for MC Config   
   //MC.w0
    MW0IndexStart   = 0x0
    MW0StepMode     = 0x1 
    MW0Len          = 0xa
    IW0Size         = 0xa

    MIndexOffset  = 0
    MStepOffset   = 8
    MLenOffset    = 24
    ISizeOffset   = 12

    MCW0IndexStart  = MW0IndexStart << MIndexOffset 
    MCW0StepMode    = MW0StepMode   << MStepOffset
    MCW0Len         = MW0Len        << MLenOffset
    ICW0Size        = IW0Size       << ISizeOffset
    MW0Config       = MCW0IndexStart | MCW0StepMode | MCW0Len | ICW0Size

    //MC.r0
    MR0IndexStart   = 0x0
    MR0StepMode     = 0x2 
    MR0Len          = 0xa
    IR0Size         = 0xa  

    MCR0IndexStart  = MR0IndexStart << MIndexOffset 
    MCR0StepMode    = MR0StepMode   << MStepOffset
    MCR0Len         = MR0Len        << MLenOffset
    ICR0Size        = IR0Size       << ISizeOffset
    MR0Config       = MCR0IndexStart | MCR0StepMode | MCR0Len | ICR0Size

    //MC.r1
    MR1IndexStart   = 0x1
    MR1StepMode     = 0x2 
    MR1Len          = 0xa
    IR1Size         = 0xa

    MCR1IndexStart  = MR1IndexStart << MIndexOffset 
    MCR1StepMode    = MR1StepMode   << MStepOffset
    MCR1Len         = MR1Len        << MLenOffset
    ICR1Size        = IR1Size       << ISizeOffset
    MR1Config       = MCR1IndexStart | MCR1StepMode | MCR1Len | ICR1Size
////////////////////////////////////////////////////////////////////////////
 
 m.s     NOP;;

// configure the K register
// Read the date in Row mode   
// for BIU0    
 m.s     R1 =  SDA0DM0_START         ;; // KB for BIU0  
 m.s     R2 =  DataSize * PadAColNum ;; // KS0
 m.s     R3 =  FilterStep            ;; // KC0
 m.s     R4 =  FilterStep            ;; // KI0
 m.s     R5 =  DataSize              ;; // KS1
 m.s     R6 =  PadAColNum            ;; // KC1 
 m.s     R7 =  PadAColNum            ;; // KI1   
 m.s     R8 =  DataSize * PadAColNum ;; // KS2
 m.s     R9 =  ARowBSNum             ;; // KC2
 m.s     R10=  ARowBSNum             ;; // KI2
    
 m.s     R14=  DataSizeGran          ;; // KG0
 m.s     R15=  ACirNum               ;; // KL0
    
 m.s     KB0 = R1 ;;
 m.s     KS0 = R2 ;;
 m.s     KI0 = R4 ;;
 m.s     KB1 = R1 ;;
 m.s     KS1 = R5 ;;
 m.s     KI1 = R7 ;;
 m.s     KB2 = R1 ;;
 m.s     KS2 = R8 ;;
 m.s     KI2 = R10;;   
 m.s     KG0 = R14;;
 m.s     KL0 = R15;;

// for BIU1    
 m.s     R1 =  SDA1DM0_START      ;; // KB for BIU1  
 m.s     R2 =  BlockSize          ;; // KS4
 m.s     R3 =  CoffBSNum          ;; // KC4
 m.s     R4 =  CoffBSNum          ;; // KI4
     
 m.s     R11=  BlockGran          ;; // KG1
 m.s     R12=  BCirNum            ;; // KL1
    
 m.s     KB4 = R1 ;;
 m.s     KS4 = R2 ;;
 m.s     KI4 = R4 ;;
    
 m.s     KG1 = R11;;
 m.s     KL1 = R12;;

// for BIU2    
 m.s     R1 =  SDA2DM0_START      ;; // KB for BIU0  
 m.s     R2 =  DataSize           ;; // KS8
 m.s     R3 =  AColNum            ;; // KC8
 m.s     R4 =  AColNum            ;; // KI8
 m.s     R5 =  DataSize * CColNum ;; // KS9
 m.s     R6 =  CRowBSNum          ;; // KC9 
 m.s     R7 =  CRowBSNum          ;; // KI9   
   
 m.s     R14=  DataSizeGran       ;; // KG2
 m.s     R15=  CCirNum            ;; // KL2
    
 m.s     KB8 = R1 ;;
 m.s     KS8 = R2 ;;
// m.s     KC8 = R3 ;;
 m.s     KI8 = R4 ;;
 m.s     KB9 = R1 ;;
 m.s     KS9 = R5 ;;
// m.s     KC9 = R6 ;;
 m.s     KI9 = R7 ;;
    
 m.s     KG2 = R14;;
 m.s     KL2 = R15;;

// for Loop
 m.s     R1 =  AColNum               ;; // KI12   
 m.s     R2 =  ARowBSNum             ;; // KI13   
 m.s     NOP;;
 m.s     NOP;;
 m.s     KI12 = R1     ;;
 m.s     KI13 = R2     ;;
   
// SHU.T1
 m.s     R1 = 0x0 ;;
 m.s     SHU1.T1 = Vect R1 ;;

 m.s     R1 = 0x03020100 ;;
 m.s     R2 = 0x07060504 ;;
 m.s     R3 = 0x0b0a0908 ;;
 m.s     R4 = 0x0f0e0d0c ;;
 m.s     R5 = 0x13121110 ;;
 m.s     R6 = 0x17161514 ;;
 m.s     R7 = 0x1b1a1918 ;;
 m.s     R8 = 0x1f1e1d1c ;;
 m.s     R9 = 0x23222120 ;;
 m.s     R10 = 0x27262524 ;;
 m.s     R11 = 0x2b2a2928 ;;
 m.s     R12 = 0x2f2e2d2c ;;
 m.s     R13 = 0x33323130 ;;
 m.s     R14 = 0x37363534 ;;
 m.s     R15 = 0x3b3a3938 ;;
 m.s     R16 = 0x3f3e3d3c ;;
 m.s     SVR0[0] = R1  ;; 
 m.s     SVR0[1] = R2  ;; 
 m.s     SVR0[2] = R3  ;; 
 m.s     SVR0[3] = R4  ;; 
 m.s     SVR0[4] = R5  ;; 
 m.s     SVR0[5] = R6  ;; 
 m.s     SVR0[6] = R7  ;; 
 m.s     SVR0[7] = R8  ;; 
 m.s     SVR0[8] = R9  ;; 
 m.s     SVR0[9] = R10 ;; 
 m.s     SVR0[10] = R11;; 
 m.s     SVR0[11] = R12;; 
 m.s     SVR0[12] = R13;; 
 m.s     SVR0[13] = R14;; 
 m.s     SVR0[14] = R15;; 
 m.s     SVR0[15] = R16;; 
 m.s     SHU0.T2 = SVR0       ;;

// MC
 m.s     R2 = MW0Config        ;;
 m.s     R3 = MR0Config        ;;
 m.s     R4 = MR1Config        ;;
 m.s     MC.w0 = R2            ;;
 m.s     MC.r0 = R3            ;;
 m.s     MC.r1 = R4            ;;
    
    
  

//  Filter 2D Test
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s    CallM Filter2DTest (B)  ;;  
 
    
  
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     SPU.Stop ;;

  



   











   




