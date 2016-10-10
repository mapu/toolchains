//  Date: 2014-11-18
//  Name: app.m.asm
//  Author: Yanglei
//  The configuration Unit for MPU for 32 bits FIR, modification for adding pipeline

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
            NOP; 
            BIU0.DM (A++,K++)-> M[I++] || Repeat @ (2); 
              .rept (MSize-2)
                NOP ; 
                BIU0.DM (A++,K++) -> M[I++] ; 
              .endr

              Repeat @(inCycNum-2*(MSize-2));   // Move one nop to LPTO
              LPTO (1f ) @ (KI12-1); 
                //NOP ; 
                BIU0.DM (A++,K++)-> M[I++] ; 
                Repeat @(inCycNum-1); 
              1:
              NOP; 
        .endif
    .endhmacro



    .hmacro BIU1_HM
      LPTO (1f ) @(KI12); 
        .rept (EffiBSNum-1)
          BIU1.DM (A++,K++)-> SHU1.T0 ; 
          Repeat @(BS-1); 
        .endr

        BIU1.DM(A++,K++)-> SHU1.T0 ; 
        Repeat @(EffiLen-BS*(EffiBSNum-1)-1); 
        1:
    .endhmacro


    .hmacro MR0_HM
      LPTO (1f ) @ (KI12); 
        .rept (EffiBSNum-1)
          r0.M[I++]->  SHU0.T0 ; 
          Repeat @(BS-1); 
        .endr

        r0.M[S++]->   SHU0.T0 ; 
        Repeat @(EffiLen-BS*(EffiBSNum-1)-1); 
      1:
    .endhmacro

    .hmacro MR1_HM 
      r1.M[S++] ->  SHU0.T1 ||LPTO (1f ) @ (KI12); 
        .rept (EffiBSNum-1)
          r1.M[I++]->  SHU0.T1 ; 
          Repeat @(BS-1); 
          .endr
        r1.M[S++]->  SHU0.T1 ; 

        Repeat @(EffiLen-BS*(EffiBSNum-1)-1); 
      1:
    .endhmacro

    .hmacro SHU0_HM
      .if EffiTailNum >2
        LPTO (1f ) @ (KI12); 
          .rept (EffiBSNum-1)
            SHU0.T0 IND TSQ ->    IMAC.T0(I0) ; 
            SHU0.COMB(T0,T1) ->    IMAC.T0(I0) ; 
            SHU0.COMB(T0,T1)(TC)->    IMAC.T0(I0) || Repeat @(BS-2); 
          .endr

          SHU0.T0 IND TSQ ->  IMAC.T0(I0) ; 
          SHU0.COMB(T0,T1) ->  IMAC.T0(I0) ; 
          SHU0.COMB(T0,T1)(TC)->  IMAC.T0(I0) ||Repeat @(EffiLen-BS*(EffiBSNum-1)-2); 
        1:
      .elseif EffiTailNum == 2
        LPTO (2f ) @ (KI12); 
          .rept (EffiBSNum-1)
            SHU0.T0 IND TSQ ->            IMAC.T0(I0) ; 
            SHU0.COMB(T0,T1) ->            IMAC.T0(I0) ; 
            SHU0.COMB(T0,T1)(TC)->            IMAC.T0(I0) || Repeat @(BS-2); 
          .endr
            SHU0.T0 IND TSQ ->          IMAC.T0(I0) ; 
            SHU0.COMB(T0,T1) ->          IMAC.T0(I0) ;        
        2:
      .elseif EffiTailNum == 1
        LPTO (3f ) @ (KI12); 
          .rept (EffiBSNum-1)
            SHU0.T0 IND TSQ ->            IMAC.T0(I0) ; 
            SHU0.COMB(T0,T1) ->            IMAC.T0(I0) ; 
            SHU0.COMB(T0,T1)(TC)->            IMAC.T0(I0) || Repeat @(BS-2); 
          .endr
            SHU0.T0 IND TSQ ->          IMAC.T0(I0) ;          
        3:
        .elseif EffiTailNum == 0
        LPTO (4f ) @ (KI12); 
          .rept (EffiBSNum)
            SHU0.T0 IND TSQ ->            IMAC.T0(I0) ; 
            SHU0.COMB(T0,T1) ->            IMAC.T0(I0) ; 
            SHU0.COMB(T0,T1)(TC)->            IMAC.T0(I0) || Repeat @(BS-2); 
          .endr
        4:
        .endif
    .endhmacro

    .hmacro SHU1_HM
      LPTO (1f ) @ (KI12); 
       .rept (EffiBSNum-1)
          SHU1.T0 IND TBW (TB=+4) ->            IMAC.T1(I1) ; 
          SHU1.T0 IND TB (TB+=4)->            IMAC.T1(I1) || Repeat @(BS-1); 
        .endr
          SHU1.T0 IND TBW(TB=+4) ->        IMAC.T1(I1) ; 
          SHU1.T0 IND TB(TB+=4)->        IMAC.T1(I1) ||Repeat @(EffiLen-BS*(EffiBSNum-1)-1); 
      1:
    .endhmacro

    .hmacro IMAC_HM
      LPTO (1f ) @ (KI12); 
        MR+= T0 * T1 (CR); 
        MR+= T0 * T1 || Repeat @ (EffiLen-2); 
        MR+= T0 * T1 ->        BIU2 ; 
      1:
    .endhmacro
   
    .hmacro BIU2_HM
      LPTO (1f ) @ (KI12-1); 
        Repeat @(EffiLen-2); // issue BIU2 one cycle in advance
        BIU2->DM(A++,K++); 
        NOP ; 
      1:
        Repeat @(EffiLen-2);                   
        BIU2->DM(A++,K++,M); 
        NOP ; 
    .endhmacro

    
    
    .hmacro main
         BIU0_HM ; 
         Repeat @(5); 
         BIU1_HM ; 
         Repeat @(6); 
         MR0_HM ||MR1_HM ; 
         NOP; 
         NOP;
	 NOP;
         SHU0_HM || SHU1_HM;;
	  
         NOP ; 
         NOP;
         IMAC_HM ; 
         Repeat @(10); 
         BIU2_HM ;          
    .endhmacro

  FIRFixTest:
    .opt
    main; 
    .stopopt
    .flushhm
    Repeat @(8); 
    NOP ; 
    MPUSTOP; 



