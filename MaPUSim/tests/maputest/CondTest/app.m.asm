//  Date: 2013-6-18
//  Name: app.m.asm
//  Author: yinlz@zklc.com
//  The file to test the function of Cond Execution 

.text
    // Write to M itself
    .global _CondTest

_start:
    NOP;;
    NOP;;
_CondTest:
    // verification of No C Option is ignored

    // M to IALU
    IALU.T0(I0) = SHU0.TBB IND TSQ;;         
    IALU.T1(I0) = SHU0.TBB IND TSQ;;     
    IALU.T2(I0) = SHU0.TBB IND TSQ;;  
    IALU.T3(I0) = SHU0.TBB IND TSQ;;  

    FALU.T0(I0) = SHU0.TBB IND TSQ;;  
    FALU.T1(I0) = SHU0.TBB IND TSQ;;  
    FALU.T2(I0) = SHU0.TBB IND TSQ;;  
    FALU.T3(I0) = SHU0.TBB IND TSQ;;

    IMAC.T0(I0) = SHU0.TBB IND TSQ;;  
    IMAC.T1(I0) = SHU0.TBB IND TSQ;;  
    IMAC.T2(I0) = SHU0.TBB IND TSQ;;  
    IMAC.T3(I0) = SHU0.TBB IND TSQ;;

    FMAC.T0(I0) = SHU0.TBB IND TSQ;;  
    FMAC.T1(I0) = SHU0.TBB IND TSQ;;  
    FMAC.T2(I0) = SHU0.TBB IND TSQ;;  
    FMAC.T3(I0) = SHU0.TBB IND TSQ;;

    SHU0.T1 = TBB IND TSQ;;     
    SHU0.T2 = TBB IND TSQ;;

    SHU1.T1 = TBB IND TSQ;;     
    SHU1.T2 = TBB IND TSQ;;

  
    NOP;;                         // PC = 0x4

    // C option is invalid--!C option is valid
    Cond = T0 == T1;;             // PC = 0x5
    NOP;;                         // PC = 0x6
    NOP;;                         // PC = 0x7
    NOP;;  
    M126.w3 = SHU0.TBB IND TSQ (C) ;;        // PC = 0x8
    M127.w3 = SHU0.TBB IND TSQ (!C) ;;       // PC = 0x9   
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;
    SHU0.T0 = M126.r1 (C);;         // PC = 0xa
    SHU1.T0 = M127.r1 (!C);;        // PC = 0xb
    IMAC.T0 (I0)= M126.r2 (C);;     // PC = 0xc
    IMAC.T1 (I0)= M127.r2 (!C);;    // PC = 0xd
    BIU0 = M126.r3 (C);;            // PC = 0xe
    BIU1 = M127.r3 (!C);;           // PC = 0xf

    BIU0 = SHU0.T1 IND T2 (C);;   // PC = 0x10
    BIU1 = SHU0.T1 IND T2 (!C);;  // PC = 0x11
    BIU0 = SHU1.T1 IND T2 (C);;   // PC = 0x12
    BIU1 = SHU1.T1 IND T2 (!C);;  // PC = 0x13

//    M126.w0 = BIU0.DM (C);;       // PC = 0x14
//    M126.w0 = BIU0.DM (!C);;      // PC = 0x15
//    M125.w1 = BIU1.DM (C);;       // PC = 0x16
//    M125.w1 = BIU1.DM (!C);;      // PC = 0x17
//    M124.w2 = BIU2.DM (C);;       // PC = 0x18
//    M124.w2 = BIU2.DM (!C);;      // PC = 0x19

    IALU.T2 = T1 + T2 (C);;       // PC = 0x1a
    IALU.T3 = T1 + T2 (!C);;      // PC = 0x1b
    IMAC.T0 = T1 * T2 (C);;       // PC = 0x1c
    IMAC.T1 = T1 * T2 (!C);;      // PC = 0x1d
    FALU.T0 = T1 + T2 (S)(C);;       // PC = 0x1e
    FALU.T1 = T1 + T2 (S)(!C);;      // PC = 0x1f
    FMAC.T0 = T1 * T2 (S)(C);;       // PC = 0x20


    // C option is valid--!C option is invalid
    Cond = T0 != T1;;             // PC = 0x21
    FMAC.T1 = T1 * T2 (S)(!C);;   // PC = 0x22
    M1.w3 = SHU0.TBB IND TSQ;;
    M2.w3 = SHU0.TBB IND TSQ;;
    M3.w3 = SHU0.TBB IND TSQ;;
    M4.w3 = SHU0.TBB IND TSQ;;
    NOP;;
    NOP;;
    NOP;;
    NOP;;                         
    NOP;;                         
    M126.w3 = M1.r0 (C) ;;        
    M127.w3 = M1.r0 (!C) ;;       
    SHU0.T0 = M2.r1 (C);;         
    SHU1.T0 = M2.r1 (!C);;        
    IMAC.T0 (I0)= M3.r2 (C);;    
    IMAC.T1 (I0)= M3.r2 (!C);;    
    BIU0 = M4.r3 (C);;           
    BIU1 = M4.r3 (!C);;           
    BIU0 = SHU0.T1 IND T2 (C);;
    BIU1 = SHU0.T1 IND T2 (!C);;
    BIU0 = SHU1.T1 IND T2 (C);;
    BIU1 = SHU1.T1 IND T2 (!C);;

    IALU.T2 = T1 + T2 (C);;
    IALU.T3 = T1 + T2 (!C);;
    IMAC.T0 = T1 * T2 (C);;
    IMAC.T1 = T1 * T2 (!C);;
    FALU.T0 = T1 + T2 (S)(C);;
    FALU.T1 = T1 + T2 (S)(!C);;
    FMAC.T0 = T1 * T2 (S)(C);;
    FMAC.T1 = T1 * T2 (S)(!C);;
    

    MPU.Stop;;


