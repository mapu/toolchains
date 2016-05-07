//  Date: 2014-01-20
//  Name: app.s.asm
//  Author: tao.wang@ia.ac.cn
//  The SPU configure code  for fix pint FFT

// begin program code */
    .text
    .global main
   
    .include "./Include.inc"

//*************************************************/ 
main:
   
 m.s    NOP;;
// MC and LOOP configure
 m.s    R2 = MConfigW0      ;;
 m.s    R3 = MConfigW1      ;;
 m.s    R1 = PipeNum        ;;
   
 m.s    MC.w0 = R2          ;; 
 m.s    MC.r0 = R2          ;;
 m.s    MC.r1 = R2          ;;
 m.s    MC.w1 = R3          ;;
   
 m.s    KI12  = R1          ;; // Loop
   
//  Config BIU1 To load the SHU T register
 m.s     NOP;;
 m.s     R1 =  SDA1DM1_START      ;; // KB for BIU0  
 m.s     R2 =  64      ;; // KS0
 m.s     R3 =  16      ;; // KC0
 m.s     R4 =  16      ;; // KI0    
 m.s     R14=  6       ;; // KG0
 m.s     R15=  1       ;; // KL0
 m.s     R16=  0       ;; // KM0    

 m.s     KB4 = R1 ;;
 m.s     KS4 = R2 ;;
  // m.s KC0 = R3 ;;
 m.s     KI4 = R4 ;;   
 m.s     KG1 = R14;;
 m.s     KL1 = R15;;
 m.s     KM1 = R16;;



//////////////////////////////////////////////////////////////////////////////////
//  FFT Epoch 0 call
 m.s     NOP;;
 m.s     CallM FFTFix1024Ep0Test (B)  ;;  
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;

/////////////////////////////////////////////////////////////////////////////////////////
// Epoch 1
// MC and LOOP configure
 m.s    R2 = MConfigW0      ;;
 m.s    R3 = MConfigW1      ;;
 m.s    R1 = PipeNum        ;;
   
   // m.s MC.w0 = R2          ;;  //Changed to W2 for Epoch1
 m.s    MC.w2 = R2 ;;
 m.s    MC.r0 = R2          ;;
 m.s    MC.r1 = R2          ;;
 m.s    MC.w1 = R3          ;;
   
 m.s    KI12  = R1          ;; // Loop
     
 
 m.s     CallM FFTFix1024Ep1Test (B)  ;;  
 m.s     NOP ;;
 m.s     NOP ;;
 m.s     NOP ;;

/////////////////////////////////////////////////////////////////////////////////////////
// Epoch 2
// MC and LOOP configure
 m.s    R2 = MConfigW0      ;;
 m.s    R3 = MConfigW1      ;;
 m.s    R1 = PipeNum        ;;
   
 m.s    MC.w0 = R2          ;; 
 m.s    MC.r0 = R2          ;;
 m.s    MC.r1 = R2          ;;
 m.s    MC.w1 = R3          ;;
   
 m.s    KI12  = R1          ;; // Loop

 m.s     NOP ;;
 m.s     NOP ;;
 m.s     NOP ;;
 m.s     CallM FFTFix1024Ep2Test (B)  ;;  
 m.s     NOP ;;
 m.s     NOP ;;
 m.s     NOP ;;
 m.s     NOP ;;
 m.s     SPU.Stop ;;





   




