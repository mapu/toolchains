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
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     CallM FFTFix4086Ep0Test (B)  ;;  
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;

/////////////////////////////////////////////////////////////////////////////////////////
// Epoch 1
// MC and LOOP configure
 m.s    R2 = MConfigW0      ;;
 m.s    R3 = MConfigW1      ;;
 m.s    R1 = PipeNum        ;;
   
// m.s    MC.w0 = R2          ;; 
 m.s    MC.w2 = R2          ;; 
 m.s    MC.r0 = R2          ;;
 m.s    MC.r1 = R2          ;;
 m.s    MC.w1 = R3          ;;
   
 m.s    KI12  = R1          ;; // Loop

    
   
 m.s     CallM FFTFix4086Ep1Test (B)  ;;  
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
     
    // Read the date in bit reverse mode   
 m.s     CallM FFTFix4086Ep2Test (B)  ;;  
 m.s     NOP ;;
 m.s     NOP ;;
 m.s     NOP;;    
 m.s     NOP;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     R1   =  0xC00000;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     Ch[4]   =  R1;;   //Addr
 m.s     R1   =  4;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     Ch[5]   =  R1;;  //XNum
 m.s     R1   =  0x4000;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     Ch[6]   =  R1;;  //YStep
 m.s     R1   =  16 ;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     Ch[7]   =  R1;;  //YNum
 m.s     R1   =  0x4 ;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     Ch[8]   =  R1;;  //ZStep
 m.s     R1   =  4096;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     Ch[9]   =  R1;; //YAllNum
 m.s     R1   =  0xE00000;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     Ch[10]   =  R1;;   //Addr
 m.s     R1   =  4 ;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     Ch[11]   =  R1;;  //XNum
 m.s     R1   =  4 ;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     Ch[12]   =  R1;;  //YStep
 m.s     R1   =  4096 ;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     Ch[13]   =  R1;;  //YNum
 m.s     R1   =  0x300 ;;
 m.s     NOP;;
 m.s     NOP;;
 m.s     Ch[14]   =  R1;;  //ZStep
 m.s     R1   =  4096 ;;
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
 m.s     NOP ;;
 m.s     NOP ;;
 m.s     SPU.Stop ;;





   




