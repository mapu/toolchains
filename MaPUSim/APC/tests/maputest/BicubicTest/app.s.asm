//  Date: 2013-11-29
//  Name: app.s.asm
//  Author: wangl
//  The configuration Unit for MPU for Bicubic Interpolation, Modification for adding pipeline.

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
    SDA0DM1_START = 0x600000 
    SDA2DM0_START = 0xc00000  
    SDA2DM1_START = 0xe00000  
    SDASize       = 0x40000  
///////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
// the following need to modify
    inH           =  80
    inW           =  80
    outH          =  180
    outW          =  240  
    inWP          =  1
    outWP         =  3
    inHP          =  4
    outHP         =  9
    PadNum        =  3
    ARowNum       =  inH
    AColNum       =  inW
    OutRowNum     =  outH
    OutColNum     =  outW
    ARowPadNum    =  inH + PadNum    //The height after Padding in  Vetical Interplation
    AColPadNum    =  inW + PadNum    //The width after Padding in  Vetical Interplation
  
    //inHPPadNum    =  inHP + PadNum   //The inHP after Padding in  Vetical Interplation
    //inWPPadNum    =  inWP + PadNum   //The inHP after Padding in  Vetical Interplation
    
    BS            = 16               // Data type
    DataSize      = 4                //  Byte 1, Short 2, Word 4, Long 8
    DataSizeGran  = 2       
///////////////////////////////////////////////////////////////////////  
     
    BankNum       = BS    
    BankSize      = SDASize / BankNum
    BlockSize     = 64                        // the total Rd size, for Row Rd is 512/8  
    BlockGran     = 6                         // log2(BlockSize)    

    ARowPadBSTailNum = ARowPadNum % BS
    ARowPadBSTailEn  = ARowPadBSTailNum != 0    
    ARowPadBSNum     = ARowPadNum  / BS + ARowPadBSTailEn
         
    Col_OutRowBSTailNum = ARowPadNum % BS
    Col_OutRowBSTailEn  = Col_OutRowBSTailNum != 0
    Col_OutRowBSNum     = ARowPadNum / BS  + Col_OutRowBSTailEn

    OutColBSTailNum = outW % BS
    OutColBSTailEn  = OutColBSTailNum != 0
    OutColBSNum     = OutColNum / BS  + OutColBSTailEn
    
    //The first interpolation in width direction
    .if Col_OutRowBSTailNum == 0
        MaskL0     = 0xffffffff
        MaskH0     = 0xffffffff 
    .elseif Col_OutRowBSTailNum < 8
        ShiftNum  = Col_OutRowBSTailNum * 4
        MaskLTemp = 0xffffffff << ShiftNum 
        MaskL0     = ~ MaskLTemp
        MaskH0     = 0
    .else 
        ShiftNum   = Col_OutRowBSTailNum *4 - 32
        MaskHTemp  = 0xffffffff << ShiftNum
        MaskL0    = 0xffffffff
        MaskH0    = ~ MaskHTemp
    .endif

    //The second interpolation in heigth direction
    .if OutColBSTailNum == 0
        MaskL1     = 0xffffffff
        MaskH1     = 0xffffffff 
    .elseif OutColBSTailNum < 8
        ShiftNum  = OutColBSTailNum * 4
        MaskLTemp = 0xffffffff << ShiftNum 
        MaskL1     = ~ MaskLTemp
        MaskH1     = 0
    .else 
        ShiftNum   = OutColBSTailNum *4 - 32
        MaskHTemp  = 0xffffffff << ShiftNum
        MaskL1    = 0xffffffff
        MaskH1    = ~ MaskHTemp
    .endif

    // Circle num of interpolation in width direction
    W_ACirNum       = 2                         
    W_CCirNum       = 2

    // Circle num of interpolation in heigth direction
    H_ACirNum       = 3                         
    H_CCirNum       = 3

    // for MC Config 
    // Interpolation in Width direction
    W_MIndexStart   = 0x16    
    W_MStepMode     = 0x1     
    W_MLen          = 0x4
    W_ISize         = 0x4

    MIndexOffset  = 0
    MStepOffset   = 8
    MLenOffset    = 24
    ISizeOffset   = 12

    W_MCIndexStart  = W_MIndexStart << MIndexOffset 
    W_MCStepMode    = W_MStepMode   << MStepOffset
    W_MCLen         = W_MLen        << MLenOffset
    W_ICSize        = W_ISize       << ISizeOffset
    W_MConfig       = W_MCIndexStart | W_MCStepMode | W_MCLen | W_ICSize

    // Interpolation in height direction
    H_MIndexStart   = 0x16    
    H_MStepMode     = 0x1     
    H_MLen          = 0x7
    H_ISize         = 0x4

    MIndexOffset  = 0
    MStepOffset   = 8
    MLenOffset    = 24
    ISizeOffset   = 12

    H_MCIndexStart  = H_MIndexStart << MIndexOffset 
    H_MCStepMode    = H_MStepMode   << MStepOffset
    H_MCLen         = H_MLen        << MLenOffset
    H_ICSize        = H_ISize       << ISizeOffset
    H_MConfig       = H_MCIndexStart | H_MCStepMode | H_MCLen | H_ICSize

    ///////////////////////////////////////////////////////////
  
    nBits         =   10
    nHBits        =   nBits - 1
    nDBits        =   nBits + 1
    nUnit         =   1 << nBits
    nHalf         =   1 << nHBits
    nDUnit        =   1 << nDBits
    nDHalf        =   1 << nBits
    tempY         =   (inH << nBits)/outH 
    tempX         =   (inW << nBits)/outW
    Mask          =   ~(0xFFFFFFFF << nBits)
    WPInc         =   (inW << nBits) / (outW / outWP)
    HPInc         =   (inH << nBits) / (outH / outHP)
    
    nC0C0         =   0xffffffff   // -1
    nC0C1         =   0x2          // 2
    nC0C2         =   0xffffffff   // -1
    nC1C0         =   0x3          // 3
    nC1C1         =   0xfffffffb   // -5
    nC1C2         =   nDUnit       // nDUnit
    nC2C0         =   0xfffffffd   // -3
    nC2C1         =   0x4          // 4
    nC2C2         =   0x1          // 1
    
   // Interpolation in Hight direction
    SHU1T1n0      =  tempY * 0 + 1
    SHU1T1n1      =  tempY * 1 + 1
    SHU1T1n2      =  tempY * 2 + 1
    SHU1T1n3      =  tempY * 3 + 1
    SHU1T1n4      =  tempY * 4 + 1
    SHU1T1n5      =  tempY * 5 + 1
    SHU1T1n6      =  tempY * 6 + 1
    SHU1T1n7      =  tempY * 7 + 1
    SHU1T1n8      =  tempY * 8 + 1
    
    // Interpolation in width direction
    SHU1T0n0      =  tempX * 0 + 1
    SHU1T0n1      =  tempX * 1 + 1
    SHU1T0n2      =  tempX * 2 + 1  

///////////////////////////////////////////////////////////////////
// Initial the parameter
// SHU0.T0 = used during interpolation
//      T1 = 0
//      T2 = 255
//      T3 = used during interpolation
// SHU1.T0 = tempX * i + 1
//      T1 = tempY * i + 1
//      T3 = -1 5 etc 
    R0            =  SHU1T0n0 ;;
    R1            =  SHU1T0n1 ;;
    R2            =  SHU1T0n2 ;;      

    SVR1[0]       =  R0       ;;
    SVR1[1]       =  R1       ;;
    SVR1[2]       =  R2       ;;    
    SHU1.T0       =  SVR1  ;;   // SHU1.T0

    R0            =  SHU1T1n0 ;;
    R1            =  SHU1T1n1 ;;
    R2            =  SHU1T1n2 ;;
    R3            =  SHU1T1n3 ;;
    R4            =  SHU1T1n4 ;;
    R5            =  SHU1T1n5 ;;
    R6            =  SHU1T1n6 ;;
    R7            =  SHU1T1n7 ;;
    R8            =  SHU1T1n8 ;;

    SVR0[0]       =  R0       ;;
    SVR0[1]       =  R1       ;;
    SVR0[2]       =  R2       ;;
    SVR0[3]       =  R3       ;;
    SVR0[4]       =  R4       ;;
    SVR0[5]       =  R5       ;;
    SVR0[6]       =  R6       ;;
    SVR0[7]       =  R7       ;;
    SVR0[8]       =  R8       ;;
    SHU1.T1       =  SVR0  ;;   // SHU1.T1

    R0  = nBits  ;;
    R1  = WPInc  ;;
    R2  = nHalf  ;;
    R3  = nDBits ;;
    R4  = nDHalf ;;
    R5  = Mask   ;;
    R6  = HPInc  ;;
    R7  = nC0C0  ;;
    R8  = nC0C1  ;;
    R9  = nC0C2  ;;
    R10 = nC1C0  ;;
    R11 = nC1C1  ;;
    R12 = nC1C2  ;;
    R13 = nC2C0  ;;
    R14 = nC2C1  ;;
    R15 = nC2C2  ;;
    SVR0[0] = R0  ;; 
    SVR0[1] = R1  ;; 
    SVR0[2] = R2  ;; 
    SVR0[3] = R3  ;; 
    SVR0[4] = R4  ;; 
    SVR0[5] = R5  ;; 
    SVR0[6] = R6  ;; 
    SVR0[7] = R7  ;; 
    SVR0[8] = R8  ;; 
    SVR0[9] = R9  ;; 
    SVR0[10] = R10;; 
    SVR0[11] = R11;; 
    SVR0[12] = R12;; 
    SVR0[13] = R13;; 
    SVR0[14] = R14;; 
    SVR0[15] = R15;; 
    SHU1.T3 = SVR0        ;;  //SHU0.T3
    //////////////////////////////////////////////////////////////////

    NOP ;;

    R0   = 0x0   ;;
    R1   = 0xff  ;;
    NOP;;
    NOP;;
    SHU0.T1  = VECT R0  ;;
    SHU0.T2  = VECT R1  ;;

// configure the MC
    // MC
    R2 = W_MConfig        ;;
    MC.w0 = R2            ;;
    MC.r0 = R2            ;;

// configure the LOOP
    R1 = AColNum   ;;
    R2 = ARowPadBSNum ;;
    NOP ;;
    NOP ;;
    KI12 = R1 ;;
    KI13 = R2 ;;
    NOP ;;
    NOP ;;
    NOP ;;
    NOP ;;
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
    // BIU Configure   
    // for BIU0    
    R1 =  SDA0DM0_START          ;; // KB for BIU0  
    R2 =  DataSize               ;; // KS0  
    R4 =  AColPadNum             ;; // KI0 
 
    R5 =  DataSize * AColPadNum  ;; // KS1    
    R7 =  ARowPadBSNum           ;; // KI1 
 
    R14=  DataSizeGran           ;; // KG0
    R15=  W_ACirNum              ;; // KL0 
   
    
    KB0 = R1  ;;
    KS0 = R2  ;;   
    KI0 = R4  ;; 
  
    KB1 = R1  ;;
    KS1 = R5  ;;   
    KI1 = R7  ;; 
   
    KG0 = R14 ;;
    KL0 = R15 ;;    
    
     // for BIU2    
    R1 =  SDA2DM0_START      ;; // KB for BIU2  
    R2 =  DataSize           ;; // KS8   
    R4 =  outW               ;; // KI8 
 
    R5 =  outW *  DataSize   ;; // KS9   
    R7 =  Col_OutRowBSNum    ;; // KI9
    
    R14=  DataSizeGran       ;; // KG0
    R15=  W_CCirNum          ;; // KL0
    R16=  MaskL0             ;; // MaskX
    R17=  MaskH0             ;; // MaskY

    
    KB8 = R1  ;;
    KS8 = R2  ;;   
    KI8 = R4  ;;
   
    KB9 = R1  ;;
    KS9 = R5  ;;    
    KI9 = R7  ;; 
   
    KG2 = R14 ;;
    KL2 = R15 ;; 
    KM2 = R16 ;;
    KM2 = R17(H);;
   
    // End BIU Configrure   
//////////////////////////////////////////////////////////////////

    NOP  ;;
    NOP  ;;
    NOP  ;;
    CallM BicubicColTest(B);;

    NOP ;;
    NOP ;;


/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// Interpolation in height direction
    // configure the MC
    // MC
    R2 = H_MConfig        ;;
    MC.w2 = R2            ;;
    MC.r0 = R2            ;;

// configure the LOOP
    R1 = ARowNum / 4  ;;  // continuous ld 4 times every circle
    R2 = OutColBSNum  ;;
    NOP ;;
    NOP ;;
    KI12 = R1 ;;
    KI13 = R2 ;;
    NOP ;;
    NOP ;;
    NOP ;;
    NOP ;;

    // BIU Configure   
    // for BIU0    
    R1 =  SDA0DM1_START          ;; // KB for BIU0  
    R2 =  BankSize               ;; // KS0
    R3 =  outH % BS          ;; // KE0
    R4 =  BS                     ;; // KI0  

    R5 =  outW *  DataSize   ;; // KS1  
    R7 =  outH / BS          ;; // KI1
 
    R8 = BlockSize          ;; // KS2
    R10= OutColBSNum        ;; // KI2
 
    R14=  BlockGran          ;; // KG0
    R15=  H_CCirNum          ;; // KL0
    R16=  MaskL1             ;; // MaskX
    R17=  MaskH1             ;; // MaskY
    
    
    KB0 = R1  ;;
    KS0 = R2  ;;
    KE0 = R3  ;;
    KI0 = R4  ;; 
  
    KB1 = R1  ;;   
    KS1 = R5  ;;
    KI1 = R7  ;; 

    KB2 = R1  ;;   
    KS2 = R8  ;;
    KI2 = R10 ;; 
   
    KG0 = R14 ;;
    KL0 = R15 ;;
    KM0 = R16 ;;
    KM0 = R17(H);; 
    
  
     // for BIU2    
    R1 =  SDA2DM0_START      ;; // KB for BIU0  
    R2 =  BankSize           ;; // KS0
    R3 =  ARowPadNum % BS        ;; // KE0 
    R4 =  BS                 ;; // KI0 
 
    R5 =  DataSize * outW        ;; // KS1  
    R7 =  ARowPadNum / BS        ;; // KI1

    R8 =  BlockSize          ;; // KS2
    R10=  outW        ;; // KI2
    
    R14=  BlockGran          ;; // KG0
    R15=  H_ACirNum              ;; // KL0 

    
    KB8 = R1  ;;
    KS8 = R2  ;;
    KE8 = R3  ;;
    KI8 = R4  ;; 
  
    KB9 = R1  ;;
    KS9 = R5  ;;   
    KI9 = R7  ;; 

    KB10= R1  ;;
    KS10= R8  ;;
    KI10= R10 ;;
   
    KG2 = R14 ;;
    KL2 = R15 ;; 


    NOP  ;;
    NOP  ;;
    NOP  ;;
    CallM BicubicRowTest(B);;

    NOP ;;
    NOP ;;
    SPU.Stop ;;


  

    
   






   




