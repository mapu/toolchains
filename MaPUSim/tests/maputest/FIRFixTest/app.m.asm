//  Date: 2013-08-15
//  Name: app.m.asm
//  Author: tao.wang@ia.ac.cn
//  The configuration Unit for MPU for 32 bits FIR

/*
///////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
*/    
    .include "Include.inc"
    .global FIRFixTest

    inCycNum = EffiLen     
    MSize    = MBSNum
   

    .hmacro BIU0_HM 
        .if MSize > 2
            //NOP;;
            M[I++].w0 = BIU0.DM (A++)(K++);Repeat at (2);;
               .rept (MSize-2)
                   NOP ;;
                   M[I++].w0 = BIU0.DM (A++)(K++);;
               .endr
            Repeat at(inCycNum-2*(MSize-2)-2-1);;  // Move one nop to LPTO
            LPTO 1f AT (KI12-1);;
                //NOP ;;
                M[I++].w0 = BIU0.DM (A++)(K++);;
                Repeat at(inCycNum-1);;
            1:
            NOP;;
      .endif
    .endhmacro



    .hmacro BIU1_HM
        LPTO 1f AT(KI12);;
        .rept (EffiBSNum-1)
            SHU1.T0 = BIU1.DM (A++)(K++);;
            Repeat at(BS-1);;
        .endr
        SHU1.T0 = BIU1.DM(A++)(K++);;
        Repeat at(EffiLen-BS*(EffiBSNum-1)-1);;
        1:
    .endhmacro


    .hmacro MR0_HM
        LPTO 1f AT (KI12);;
       .rept (EffiBSNum-1)
            SHU0.T0 = M[I++].r0;;
            Repeat at(BS-1);;
        .endr
        SHU0.T0 = M[S++].r0;;
        Repeat at(EffiLen-BS*(EffiBSNum-1)-1);;
        1:
    .endhmacro

    .hmacro MR1_HM 
        SHU0.T1 = M[S++].r1 ;LPTO 1f AT (KI12);;
       .rept (EffiBSNum-1)
            SHU0.T1 = M[I++].r1;;
            Repeat at(BS-1);;
        .endr
        SHU0.T1 = M[S++].r1;;
        Repeat at(EffiLen-BS*(EffiBSNum-1)-1);;
        1:
    .endhmacro

    .hmacro SHU0_HM
        .if EffiTailNum >2
          LPTO 1f AT (KI12);;
          .rept (EffiBSNum-1)
            IMAC.T0(I0) = SHU0.T0 IND TSQ ;;
            IMAC.T0(I0) = SHU0.COMB(T0,T1,4) ;;
            IMAC.T0(I0) = SHU0.COMB(T0,T1,4) (TC); Repeat at(BS-2);;
          .endr
          IMAC.T0(I0) = SHU0.T0 IND TSQ ;;
          IMAC.T0(I0) = SHU0.COMB(T0,T1,4) ;;
          IMAC.T0(I0) = SHU0.COMB(T0,T1,4) (TC);Repeat at(EffiLen-BS*(EffiBSNum-1)-2);;
          1:
        .elseif EffiTailNum == 2
           LPTO 2f AT (KI12);;
           .rept (EffiBSNum-1)
            IMAC.T0(I0) = SHU0.T0 IND TSQ ;;
            IMAC.T0(I0) = SHU0.COMB(T0,T1,4) ;;
            IMAC.T0(I0) = SHU0.COMB(T0,T1,4) (TC); Repeat at(BS-2);;
          .endr
          IMAC.T0(I0) = SHU0.T0 IND TSQ ;;
          IMAC.T0(I0) = SHU0.COMB(T0,T1,4) ;;       
          2:
        .elseif EffiTailNum == 1
           LPTO 3f AT (KI12);;
          .rept (EffiBSNum-1)
            IMAC.T0(I0) = SHU0.T0 IND TSQ ;;
            IMAC.T0(I0) = SHU0.COMB(T0,T1,4) ;;
            IMAC.T0(I0) = SHU0.COMB(T0,T1,4) (TC); Repeat at(BS-2);;
          .endr
          IMAC.T0(I0) = SHU0.T0 IND TSQ ;;         
          3:
        .elseif EffiTailNum == 0
          LPTO 4f AT (KI12);;
          .rept (EffiBSNum)
            IMAC.T0(I0) = SHU0.T0 IND TSQ ;;
            IMAC.T0(I0) = SHU0.COMB(T0,T1,4) ;;
            IMAC.T0(I0) = SHU0.COMB(T0,T1,4) (TC); Repeat at(BS-2);;
          .endr
          4:
        .endif
    .endhmacro

    .hmacro SHU1_HM
        LPTO 1f AT (KI12);;
       .rept (EffiBSNum-1)
            IMAC.T1(I1) = SHU1.T0 IND TBW (TB=+4) ;;
            IMAC.T1(I1) = SHU1.T0 IND TB (TB+=4); Repeat at(BS-1);;
        .endr
        IMAC.T1(I1) = SHU1.T0 IND TBW(TB=+4) ;;
        IMAC.T1(I1) = SHU1.T0 IND TB(TB+=4);Repeat at(EffiLen-BS*(EffiBSNum-1)-1);;
        1:
    .endhmacro

    .hmacro IMAC_HM
        LPTO 1f AT (KI12);;
        MR += T0 * T1 (CR);;
        MR += T0 * T1 ; Repeat at (EffiLen-2);;
        BIU2 = MR += T0 * T1 ;;
        1:
    .endhmacro
   
    .hmacro BIU2_HM
        LPTO 1f AT (KI12-1);;
        Repeat at(EffiLen-2);;                   // issue BIU2 one cycle in advance
        DM = BIU2 (A++)(K++);;
        NOP ;;
        1:
        Repeat at(EffiLen-2);;                  
        DM = BIU2 (A++)(K++)(M);;
        NOP ;;
    .endhmacro

    
    
    .hmacro main
         BIU0_HM ;;
         Repeat at(3);;
         BIU1_HM ;;
         Repeat at(5);;
         MR0_HM ;MR1_HM ;;
         NOP;;
         SHU0_HM ;SHU1_HM;;
         NOP ;;
         IMAC_HM ;;
         Repeat at(8);;
         BIU2_HM ;;         
    .endhmacro

FIRFixTest:
    .opt
    main;;
    .stopopt
    .flushhm
    Repeat at(8);;
    NOP ;;
    MPU.Stop;;



