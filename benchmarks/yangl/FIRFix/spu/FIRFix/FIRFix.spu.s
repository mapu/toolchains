//  Date: 2013-08-15
//  Name: app.s.asm
//  Author: tao.wang@ia.ac.cn
//  The configuration Unit for MPU for 32 bits FIR

// begin program code */
.include "./Include.inc"

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
  

///////////////////////////////////////////////////////////////////////
    BlockSize     = 64                        // the total Rd size, for Row Rd is 512/8  
    BlockGran     = 6                         // log2(BlockSize)    
    DataSize      = 4                         //  Byte 1, Short 2, Word 4, Long 8
    DataSizeGran  = 2    

    HalfBS        = BS/2

    .if DataTailNum == 0
        CMaskL     = 0xffffffff
        CMaskH     = 0xffffffff
    .elseif DataTailNum < HalfBS                    // A MASK The none BS col
        CShiftNum  = DataTailNum * DataSize
        CMaskLTemp = 0xffffffff << CShiftNum 
        CMaskL     = ~ CMaskLTemp
        CMaskH     = 0
    .else 
        CShiftNum  = DataTailNum  *DataSize - 32
        CMaskHTemp = 0xffffffff << CShiftNum
        CMaskL     = 0xffffffff
        CMaskH     = ~ CMaskHTemp
    .endif
    
    ACirNum       = 1                         // the number of circle
    BCirNum       = 2
    CCirNum       = 1

    // for MC Config   
    MIndexStart   = 0x0
    MStepMode     = 0x1
    MLen          = MBSNum 
    ISize         = MBSNum 

    MIndexOffset  = 0
    MStepOffset   = 8
    ISizeOffset   = 12
    MLenOffset    = 24
    

    MCIndexStart  = MIndexStart << MIndexOffset 
    MCStepMode    = MStepMode   << MStepOffset
    MCLen         = MLen        << MLenOffset
    ICSize        = ISize       << ISizeOffset
    MConfig       = MCIndexStart | MCStepMode | MCLen | ICSize
////////////////////////////////////////////////////////////////////////////
 
 m.s     NOP;;
// for MC
 m.s     R1 = MConfig ;;
 m.s     NOP ;;
 m.s     NOP ;;
 m.s     MC.w0 = R1 ;;
 m.s     MC.r0 = R1 ;;
 m.s     MC.r1 = R1 ;;

// for Loop
 m.s     R1 =  DataBSNum   ;; // KI12   
 m.s     NOP;; 
 m.s     NOP;;
 m.s     NOP;;
 m.s     KI12 = R1     ;;

// configure the K register
// Read the date in Row mode   
// for BIU0    
 m.s     R1 =  SDA0DM0_START         ;; // KB for BIU0  
 m.s     R2 =  BlockSize             ;; // KS0    
 m.s     R4 =  PadDataBSNum          ;; // KI0

 m.s     R14=  BlockGran             ;; // KG0
 m.s     R15=  ACirNum               ;; // KL0
    
 m.s     KB0 = R1 ;;
 m.s     KS0 = R2 ;;
 m.s     KI0 = R4 ;;
      
 m.s     KG0 = R14;;
 m.s     KL0 = R15;;

// for BIU1    
 m.s     R1 =  SDA1DM0_START      ;; // KB for BIU1  
 m.s     R2 =  BlockSize          ;; // KS4   
 m.s     R4 =  EffiBSNum          ;; // KI4

 m.s     R5 =  0x0                ;; // KS5
 m.s     R7 =  DataBSNum          ;; // KI5
     
 m.s     R11=  BlockGran          ;; // KG1
 m.s     R12=  BCirNum            ;; // KL1
    
 m.s     KB4 = R1 ;;
 m.s     KS4 = R2 ;;
 m.s     KI4 = R4 ;;

 m.s     KB5 = R1 ;;
 m.s     KS5 = R5 ;;
 m.s     KI5 = R7 ;;
    
 m.s     KG1 = R11;;
 m.s     KL1 = R12;;

// for BIU2    
 m.s     R1 =  SDA2DM0_START      ;; // KB for BIU0  
 m.s     R2 =  BlockSize          ;; // KS8   
 m.s     R4 =  DataBSNum          ;; // KI8
      
 m.s     R14=  BlockGran          ;; // KG2
 m.s     R15=  CCirNum            ;; // KL2
 m.s     R16=  CMaskL             ;; // KML
 m.s     R17=  CMaskH             ;; // KMH
    
 m.s     KB8 = R1 ;;
 m.s     KS8 = R2 ;;
 m.s     KI8 = R4 ;;
    
 m.s     KG2 = R14;;
 m.s     KL2 = R15;;
 m.s     KM2 = R16;;
 m.s     KM2 = R17(H);;


//  Filter 2D Test
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s    CallM FIRFixTest (B)  ;;  
 
    

 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     SPU.Stop;;
    


   











   




