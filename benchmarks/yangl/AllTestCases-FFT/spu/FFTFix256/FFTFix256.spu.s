//  Date: 2014-01-14
//  Name: app.s.asm
//  Author: tao.wang@ia.ac.cn
//  The SPU configure code  for fix pint FFT

// begin program code */
    .text
    .global main
   
    .include "./Include.inc"

//*************************************************/ 
main:

 m.s     NOP;;
// MC
 m.s    R2 = MConfigW0      ;;
 m.s    R3 = MConfigW1      ;;
   
 m.s    MC.w0 = R2          ;; 
 m.s    MC.r0 = R2          ;;
 m.s    MC.r1 = R2          ;;

 m.s    MC.w1 = R3          ;;

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


////////////////////////////////////////////////////////////////////////////////
//  FFT Epoch 0 call
 m.s     NOP;;
 m.s     CallM FFTFix256Ep0Test (B)  ;;  
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;

/////////////////////////////////////////////////////////////////////////////////////////
// Epoch 1
// MC
 m.s    R2 = MConfigW0      ;;
 m.s    R3 = MConfigW1      ;;
   
// m.s    MC.w0 = R2          ;;
 m.s    MC.w2 = R2          ;;  
 m.s    MC.r0 = R2          ;;
 m.s    MC.r1 = R2          ;;

 m.s    MC.w1 = R3          ;;
     
    // Read the date in bit reverse mode   
// for BIU0    
 m.s     R1 =  SDA0DM0_START      ;; // KB for BIU0  
 m.s     R2 =  BlockSize          ;; // KS0
 m.s     R3 =  BankGrpNum         ;; // KC0
 m.s     R4 =  BankGrpNum         ;; // KI0  
 m.s     R5 =  BankSize           ;; // KS1
 m.s     R6 =  BankNum            ;; // KC1
 m.s     R7 =  BankNum            ;; // KI0
 m.s     R14=  BlockGran          ;; // KG0
 m.s     R15=  ACirNBRNum         ;; // KL0

 m.s     KB0 = R1 ;;
 m.s     KS0 = R2 ;;  
 m.s     KI0 = R4 ;;   
 m.s     KB1 = R1 ;;
 m.s     KS1 = R5 ;; 
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
 m.s     KI8 = R4 ;;    
 m.s     KG2 = R14;;
 m.s     KL2 = R15;;

 m.s     CallM FFTFix256Ep1Test (B)  ;;  

 m.s     NOP ;;
 m.s     NOP ;;
 m.s     NOP ;;
 m.s     NOP ;;
 m.s     SPU.Stop ;;





   




