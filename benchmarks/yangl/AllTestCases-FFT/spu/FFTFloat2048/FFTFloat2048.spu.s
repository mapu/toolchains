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
    SDA1DM1_START = 0xa00000

/////////////////////////////////////////////////////////////////////////
// the following need to modify
    ALen                = 2048                         // the FFT Length  
    BSGran        = 3     
    ABSLen              = ALen >> BSGran
    GrpNum              = ABSLen 
    PipeGrp             = 20
    PipeGrpNum          = GrpNum / PipeGrp  
   

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
 
 m.s     NOP;;
 
 //  Config BIU1 To load the SHU T register
 m.s     NOP;;
 m.s     R1 =  SDA1DM1_START      ;; // KB for BIU1  
 m.s     R2 =  64      ;; // KS4
 m.s     R3 =  16      ;; // KC4
 m.s     R4 =  16      ;; // KI4    
 m.s     R14=  6       ;; // KG1
 m.s     R15=  1       ;; // KL1
 m.s     R16=  0       ;; // KM1    

 m.s     KB4 = R1 ;;
 m.s     KS4 = R2 ;;
  // m.s KC4 = R3 ;;
 m.s     KI4 = R4 ;;   
 m.s     KG1 = R14;;
 m.s     KL1 = R15;;
 m.s     KM1 = R16;;


// for Loop     
 m.s     R1 =  PipeGrpNum           ;; // KI12
 m.s     KI12 = R1              ;;
   
    

// MC
 m.s    R2 = MConfig        ;;
 m.s    MC.r0 = R2          ;;
 m.s    MC.w0 = R2          ;;

////////////////////////////////////////////////////////////////////////////////
//  FFT Epoch 0 call

 m.s     NOP;;
 m.s     CallM FFTSF2048Ep0Test (B)  ;;  
 m.s     NOP;;
/////////////////////////////////////////////////////////////////////////////////


// MC
// m.s     R2 = MConfig        ;;
 m.s     MC.r0 = R2          ;;
// m.s     MC.w0 = R2          ;;
 m.s     MC.w2 = R2          ;;

    // FFT Epoch 1 Call 
 m.s     NOP;;
 m.s     CallM FFTSF2048Ep1Test (B)  ;; 
 m.s     NOP;;
/////////////////////////////////////////////////////////////////////////////////


// MC
// m.s     R2 = MConfig        ;;
 m.s     MC.r0 = R2          ;;
 m.s     MC.w0 = R2          ;;


//  FFT Epoch 2 Call
 m.s     NOP;;
 m.s     CallM FFTSF2048Ep2Test (B)  ;; 
 m.s     NOP;;
////////////////////////////////////////////////////////////////////////////////

// MC
// m.s     R2 = MConfig        ;;
 m.s     MC.r0 = R2          ;;
// m.s     MC.w0 = R2          ;;
 m.s     MC.w2 = R2          ;;

//
//  FFT Epoch 3 Call
 m.s     NOP;;
 m.s     CallM FFTSF2048Ep3Test (B)  ;; 
 m.s     NOP;;
 m.s     SPU.Stop ;;

////////////////////////////////////////////////////////////////////////////////







   




