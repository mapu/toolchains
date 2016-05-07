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
    SDA0DM1_START = 0x600000
    SDA1DM1_START = 0xa00000

/////////////////////////////////////////////////////////////////////////
// the following need to modify
    ALen                = 4096                         // the FFT Length  
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
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     CallM FFTSF4096Ep0Test (B)  ;;  
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
/////////////////////////////////////////////////////////////////////////////////


// MC
 m.s     R2 = MConfig        ;;
 m.s     MC.r0 = R2          ;;
// m.s     MC.w0 = R2          ;;
 m.s     MC.w2 = R2          ;;

    // FFT Epoch 1 Call 
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     CallM FFTSF4096Ep1Test (B)  ;; 
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
/////////////////////////////////////////////////////////////////////////////////


// MC
 m.s     R2 = MConfig        ;;
 m.s     MC.r0 = R2          ;;
 m.s     MC.w0 = R2          ;;


//  FFT Epoch 2 Call
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     CallM FFTSF4096Ep2Test (B)  ;; 
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
////////////////////////////////////////////////////////////////////////////////

// MC
 m.s     R2 = MConfig        ;;
 m.s     MC.r0 = R2          ;;
// m.s     MC.w0 = R2          ;;
 m.s     MC.w2 = R2          ;;

//
//  FFT Epoch 3 Call
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     CallM FFTSF4096Ep3Test (B)  ;; 
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;    
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     R1   =  0x600000;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     Ch[4]   =  R1;;   //Addr
 m.s     R1   =  8;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     Ch[5]   =  R1;;  //XNum
 m.s     R1   =  0x8000;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     Ch[6]   =  R1;;  //YStep
 m.s     R1   =  0x8 ;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     Ch[7]   =  R1;;  //YNum
 m.s     R1   =  0x8 ;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     Ch[8]   =  R1;;  //ZStep
 m.s     R1   =  4060;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     Ch[9]   =  R1;; //YAllNum
 m.s     R1   =  0xE00000;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     Ch[10]   =  R1;;   //Addr
 m.s     R1   =  8 ;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     Ch[11]   =  R1;;  //XNum
 m.s     R1   =  8 ;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     Ch[12]   =  R1;;  //YStep
 m.s     R1   =  4060 ;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     Ch[13]   =  R1;;  //YNum
 m.s     R1   =  0x300 ;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     Ch[14]   =  R1;;  //ZStep
 m.s     R1   =  4060 ;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     Ch[15]   =  R1;; //AllNum
 m.s     R1   =  0x1 ;;         
 m.s     NOP;;
 m.s     NOP;;
 m.s     Ch[17]   =  R1;; //Cmd
 m.s     R1   =  0x1;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     Ch[18]   =  R1;;   //TagMask
 m.s     R1   =  0x2;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     Ch[20]   =  R1;;   //Update
 m.s     R1   =  Ch[21];;   //Status
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     SPU.Stop ;;

////////////////////////////////////////////////////////////////////////////////







   




