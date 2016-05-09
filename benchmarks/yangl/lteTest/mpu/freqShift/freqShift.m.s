//  Date: 2014-10-13
//  Name: app.m.asm
//  Author:  xiaolong.guo@ia.ac.cn
//  The configuration Unit for MPU for LS Estimation
.text
    .global freqShift
	
    .hmacro BIU2_HW
        LPTO (1f ) @ (KI12);
            BIU2.DM(A++,K++)->M[0];
            BIU2.DM(A++,K++)->M[1];
            Repeat @ (2);
        1:
    .endhmacro

    .hmacro BIU1_HW
        LPTO (1f ) @ (KI12);
            BIU1.DM(A++,K++)->IMAC.T1(I1) || Repeat @ (2);
	    	BIU1.DM(A++,K++)->IMAC.T3(I1) || Repeat @ (2);
        1:
    .endhmacro

    .hmacro M0_HW
        LPTO (1f ) @ (KI12);
	    	R0.M[0]->IMAC.T0(I0);
	    	R0.M[1]->IMAC.T0(I0);
	    	Repeat @ (2);
		1:
    .endhmacro
    
    .hmacro M1_HW
        LPTO (1f ) @ (KI12);
 	    	R1.M[0]->SHU0.T0;
 	    	R1.M[1]->SHU0.T0;
			Repeat @ (2);
		1:
    .endhmacro

    .hmacro SHU0_HW
        LPTO (1f ) @ (KI12);
 	    	SHU0.T0 IND T3->IMAC.T2(I2) || Repeat @ (2);
			Repeat @ (2);
		1:
    .endhmacro
        
    .hmacro IMAC_HW
        LPTO (1f ) @ (KI12);
 	    	T0 * T1(T,H)->IALU.T0;
 	    	T0 * T1(T,H)->IALU.T1;
			T2 * T3(T,H)->IALU.T2;
			T2 * T3(T,H)->IALU.T3;
        1:
    .endhmacro   
    
	.hmacro IALU_HW
        LPTO (1f ) @ (KI12);
        	Repeat @ (2);
 	    	T0+T2(T,H)->BIU0;		
 	    	T1+T3(T,H)->BIU0;
        1:
    .endhmacro
    
    .hmacro BIU0_HW
        LPTO (1f ) @ (KI12);
			Repeat @ (2);
            BIU0->DM(A++,K++) || Repeat @ (2);
		1:
    .endhmacro   
    
    .hmacro FreqShift_HW
        BIU2_HW;
        Repeat @ (4);
        BIU1_HW;
        Repeat @ (6);
		M1_HW;
		NOP;
		M0_HW;
		SHU0_HW;
		IMAC_HW;       	
		Repeat @ (4);
		IALU_HW;
		Repeat @ (3);
        BIU0_HW;  
    .endhmacro

freqShift:
    .opt
    FreqShift_HW;
    .flushhm
    .stopopt
    Repeat @ (9);
    NOP;
    MPUSTOP;
