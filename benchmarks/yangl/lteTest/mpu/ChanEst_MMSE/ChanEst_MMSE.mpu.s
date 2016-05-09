//  Date: 2014-10-13
//  Name: app.m.asm
//  Author:  xiaolong.guo@ia.ac.cn
//  The configuration Unit for MPU for LS Estimation
.text
    .global ChEstMMSE_Stg1
    .global ChEstMMSE_Stg2
	.global ChEstMMSE_Stg3
    .global ChEstMMSE_Stg2Cyc1
// for stage 1
	.hmacro BIU1_S1
		BIU1.DM(A++,K++)->M[I++] || Repeat @ (24); 
    .endhmacro

	.hmacro BIU0_S1    
        BIU0.DM(A++,K++)->SHU1.T0;
        Repeat @ (11);
		BIU0.DM(A++,K++)->SHU1.T2;
		Repeat @ (35);
    .endhmacro

	.hmacro SHU1_S1 
		SHU1.T0 IND T3(TB=+2)->IMAC.T0(I1);
		SHU1.T0 IND TB(TB+=2)->IMAC.T0(I1) || Repeat @ (11); 
		SHU1.T2 IND T3(TB=+2)->IMAC.T2(I1);
		SHU1.T2 IND TB(TB+=2)->IMAC.T2(I1) || Repeat @ (11); 
 		SHU1.T2 IND T3(TB=+2)->IMAC.T2(I1);
		SHU1.T2 IND TB(TB+=2)->IMAC.T2(I1) || Repeat @ (11); 
		SHU1.T0 IND T3(TB=+2)->IMAC.T0(I1);
		SHU1.T0 IND TB(TB+=2)->IMAC.T0(I1) || Repeat @ (11); 
	.endhmacro

    .hmacro M0_S1
		.rept 2
			R0.M[I++]->IMAC.T1(I0) || Repeat @ (12);		
			R0.M[I++]->IMAC.T3(I0) || Repeat @ (12);
		.endr
    .endhmacro

    .hmacro IMAC_S1
		MR += T0 * T1(CR,H);
		MR += T0 * T1(H) || Repeat @(10);
		MR += T0 * T1(H,L)->IALU.T0;
		MR += T2 * T3(CR,H);
		MR += T2 * T3(H) || Repeat @(10);
		MR += T2 * T3(H,L)->IALU.T0;
		MR += T1 * T2(CR,H);
		MR += T1 * T2(H) || Repeat @(10);
		MR += T1 * T2(H,L)->IALU.T0;
		MR += T0 * T3(CR,H);
		MR += T0 * T3(H) || Repeat @(10);
		MR += T0 * T3(H,L)->IALU.T0;
    .endhmacro 	

	.hmacro SHU0_S1
		.rept 2
			Repeat @ (23);
			SHU0.T3 IND TBW(TB=+0)->IALU.T3(I2);
		.endr
	.endhmacro

    .hmacro IALU_S1
		NOP;
		T0>>15->IALU.T2;
		T1>>15->IALU.T3;
		Repeat @ (10);
		T0>>15->IALU.T0;
		T1>>15->IALU.T1;
		NOP;
		T2 - T0 (H)->IALU.T0;
		T3 - T1 (H)->IALU.T1;
		Repeat @ (2);
		T1<<16->IALU.T1;
		Repeat @ (2);
		MERGE(T1,T0,T3)(H)->BIU2;
		NOP;
		T0>>15->IALU.T2;
		T1>>15->IALU.T3;
		Repeat @ (10);
		T0>>15->IALU.T0;
		T1>>15->IALU.T1;
		NOP;
		T2 + T0 (H)->IALU.T0;
		T3 + T1 (H)->IALU.T1;
	    Repeat @ (2);
		T1<<16->IALU.T1;
		Repeat @ (2);
		MERGE(T1,T0,T3)(H)->BIU2;
    .endhmacro 

    .hmacro BIU2_S1
		Repeat @ (23);
	    BIU2->DM(A++,K++);
        Repeat @ (23);
	    BIU2->DM(A++,K++);		
    .endhmacro 

	.hmacro MMSE_S1
		BIU0_S1 || BIU1_S1;
		Repeat @ (10);
		SHU1_S1;
		M0_S1;
		NOP;
		IMAC_S1;
		Repeat @ (14);
		SHU0_S1;
		Repeat @ (2);
		IALU_S1;	
		Repeat @ (3);
		BIU2_S1;	
	.endhmacro

ChEstMMSE_Stg1:
    .opt
    MMSE_S1;
    .flushhm
    .stopopt
    Repeat @ (11);
    NOP;
    MPUSTOP;

// for stage 2
	.hmacro BIU1_S2
    	BIU1.DM(A++,K++)->SHU0.T0;
		Repeat @ (11);
	    BIU1.DM(A++,K++)->SHU0.T1;
    .endhmacro

	.hmacro BIU0_S2
        LPTO (1f ) @ (KI12);
            BIU0.DM(A++,K++)->SHU1.T0;
	    	BIU0.DM(A++,K++)->SHU1.T1;
            Repeat @ (10);
			BIU0.DM(A++,K++)->SHU1.T2;
	    	BIU0.DM(A++,K++)->SHU1.T3;
			Repeat @ (34);
        1:
    .endhmacro

	.hmacro SHU0_S2
		LPTO (1f ) @ (KI12);
			SHU0.T0 IND TBH (TB=+2)->M[0];
			SHU0.T0 IND TB  (TB+=2)->M[1];
			SHU0.T0 IND TB  (TB+=2)->M[2];
			SHU0.T0 IND TB  (TB+=2)->M[3];
			SHU0.T0 IND TB  (TB+=2)->M[4];
			SHU0.T0 IND TB  (TB+=2)->M[5];
			SHU0.T0 IND TB  (TB+=2)->M[6];
			SHU0.T0 IND TB  (TB+=2)->M[7];
			SHU0.T0 IND TB  (TB+=2)->M[8];
			SHU0.T0 IND TB  (TB+=2)->M[9];
			SHU0.T0 IND TB  (TB+=2)->M[10];
			SHU0.T0 IND TB  (TB+=2)->M[11];
			SHU0.T1 IND TBH (TB=+2)->M[12];
			SHU0.T1 IND TB  (TB+=2)->M[13];
			SHU0.T1 IND TB  (TB+=2)->M[14];
			SHU0.T1 IND TB  (TB+=2)->M[15];
			SHU0.T1 IND TB  (TB+=2)->M[16];
			SHU0.T1 IND TB  (TB+=2)->M[17];
			SHU0.T1 IND TB  (TB+=2)->M[18];
			SHU0.T1 IND TB  (TB+=2)->M[19];
			SHU0.T1 IND TB  (TB+=2)->M[20];
			SHU0.T1 IND TB  (TB+=2)->M[21];
			SHU0.T1 IND TB  (TB+=2)->M[22];
			SHU0.T1 IND TB  (TB+=2)->M[23];
			SHU0.T3 IND TBW (TB=+0)->M[24];
			Repeat @ (23);
		1:
	.endhmacro

    .hmacro SHU1_S2
		LPTO (1f ) @ (KI12);
			SHU1.T0 IND TSQ(TB=+0)->IMAC.T0(I1);
			SHU1.COMB(T0,T1)(H)->IMAC.T0(I1);
			SHU1.COMB(T0,T1)(H,TC)->IMAC.T0(I1) || Repeat @ (10);
			SHU1.T2 IND TSQ(TB=+0)->IMAC.T2(I1);
 			SHU1.COMB(T2,T3)(H)->IMAC.T2(I1);
			SHU1.COMB(T2,T3)(H,TC)->IMAC.T2(I1) || Repeat @ (10);
			SHU1.T2 IND TSQ(TB=+0)->IMAC.T2(I1);
			SHU1.COMB(T2,T3)(H)->IMAC.T2(I1);
			SHU1.COMB(T2,T3)(H,TC)->IMAC.T2(I1) || Repeat @ (10);
			SHU1.T0 IND TSQ(TB=+0)->IMAC.T0(I1);
			SHU1.COMB(T0,T1)(H)->IMAC.T0(I1);
			SHU1.COMB(T0,T1)(H,TC)->IMAC.T0(I1) || Repeat @ (10);
		1:
	.endhmacro
 
    .hmacro M0_S2
        LPTO (1f ) @ (KI12);
			.rept 2
				R0.M[I++]->IMAC.T1(I0) || Repeat @ (12);
				R0.M[I++]->IMAC.T3(I0) || Repeat @ (12);
			.endr
		1:
    .endhmacro

    .hmacro IMAC_S2
		LPTO (1f ) @ (KI12);
			MR += T0 * T1(CR,H);
			MR += T0 * T1(H) || Repeat @(10);
			MR += T0 * T1(H,L)->IALU.T0;
			MR += T2 * T3(CR,H);
			MR += T2 * T3(H) || Repeat @(10);
			MR += T2 * T3(H,L)->IALU.T0;
			MR += T1 * T2(CR,H);
			MR += T1 * T2(H) || Repeat @(10);
			MR += T1 * T2(H,L)->IALU.T0;
			MR += T0 * T3(CR,H);
			MR += T0 * T3(H) || Repeat @(10);
			MR += T0 * T3(H,L)->IALU.T0;
        1:
    .endhmacro   

    .hmacro IALU_S2
        LPTO (1f ) @ (KI12);
			NOP;
			T0>>15->IALU.T2;
			T1>>15->IALU.T3;
			Repeat @ (10);
			T0>>15->IALU.T0;
			T1>>15->IALU.T1;
			NOP;
			T2 - T0 (H)->IALU.T0;
			T3 - T1 (H)->IALU.T1;
			Repeat @ (2);
			T1<<16->IALU.T1;
			Repeat @ (2);
			MERGE(T1,T0,T3)(H)->BIU2;
			NOP;
			T0>>15->IALU.T2;
			T1>>15->IALU.T3;
			Repeat @ (10);
			T0>>15->IALU.T0;
			T1>>15->IALU.T1;
			NOP;
			T2 + T0 (H)->IALU.T0;
			T3 + T1 (H)->IALU.T1;
			Repeat @ (2);
			T1<<16->IALU.T1;
			Repeat @ (2);
			MERGE(T1,T0,T3)(H)->BIU2;
        1:
    .endhmacro 

	.hmacro M1_S2
		LPTO (1f ) @ (KI12);
			.rept 2
				Repeat @ (23);
				R1.M[24]->IALU.T3(I2);
			.endr
		1:
	.endhmacro

    .hmacro BIU2_S2
		LPTO (1f ) @ (KI12);
			.rept 2
				Repeat @ (23);
	    		BIU2->DM(A++,K++);
            .endr		
        1:
    .endhmacro   

    .hmacro MMSE_S2
        BIU1_S2;
		BIU0_S2;
		Repeat @ (8);
		SHU0_S2;
		Repeat @ (2);
		SHU1_S2;
		M0_S2;       
        NOP;
		IMAC_S2;
		Repeat @ (15);
		M1_S2;
		NOP;
		IALU_S2;
		Repeat @ (3);
        BIU2_S2;  
    .endhmacro

ChEstMMSE_Stg2:
    .opt
    MMSE_S2;
    .flushhm
    .stopopt
    Repeat @ (11);
    NOP;
    MPUSTOP;

// for stage 3
    .hmacro BIU2_S3
		Repeat @ (23);
	    BIU2->DM(A++,K++,M);
        Repeat @ (23);
	    BIU2->DM(A++,K++,M);		
    .endhmacro 

	.hmacro MMSE_S3
		BIU0_S1 || BIU1_S1;
		Repeat @ (10);
		SHU1_S1;
		M0_S1;
		NOP;
		IMAC_S1;
		Repeat @ (14);
		SHU0_S1;
		Repeat @ (2);
		IALU_S1;	
		Repeat @ (3);
		BIU2_S3;	
	.endhmacro

ChEstMMSE_Stg3:
    .opt
    MMSE_S3;
    .flushhm
    .stopopt
    Repeat @ (11);
    NOP;
    MPUSTOP;

// for Cycle = 1 in Stage 2 (KI12=1)
	.hmacro BIU0_S2Cyc1
        BIU0.DM(A++,K++)->SHU1.T0;
	    BIU0.DM(A++,K++)->SHU1.T1;
        Repeat @ (10);
		BIU0.DM(A++,K++)->SHU1.T2;
	    BIU0.DM(A++,K++)->SHU1.T3;
		Repeat @ (34);
    .endhmacro

	.hmacro SHU0_S2Cyc1
			SHU0.T0 IND TBH (TB=+2)->M[0];
			SHU0.T0 IND TB  (TB+=2)->M[1];
			SHU0.T0 IND TB  (TB+=2)->M[2];
			SHU0.T0 IND TB  (TB+=2)->M[3];
			SHU0.T0 IND TB  (TB+=2)->M[4];
			SHU0.T0 IND TB  (TB+=2)->M[5];
			SHU0.T0 IND TB  (TB+=2)->M[6];
			SHU0.T0 IND TB  (TB+=2)->M[7];
			SHU0.T0 IND TB  (TB+=2)->M[8];
			SHU0.T0 IND TB  (TB+=2)->M[9];
			SHU0.T0 IND TB  (TB+=2)->M[10];
			SHU0.T0 IND TB  (TB+=2)->M[11];
			SHU0.T1 IND TBH (TB=+2)->M[12];
			SHU0.T1 IND TB  (TB+=2)->M[13];
			SHU0.T1 IND TB  (TB+=2)->M[14];
			SHU0.T1 IND TB  (TB+=2)->M[15];
			SHU0.T1 IND TB  (TB+=2)->M[16];
			SHU0.T1 IND TB  (TB+=2)->M[17];
			SHU0.T1 IND TB  (TB+=2)->M[18];
			SHU0.T1 IND TB  (TB+=2)->M[19];
			SHU0.T1 IND TB  (TB+=2)->M[20];
			SHU0.T1 IND TB  (TB+=2)->M[21];
			SHU0.T1 IND TB  (TB+=2)->M[22];
			SHU0.T1 IND TB  (TB+=2)->M[23];
			SHU0.T3 IND TBW (TB=+0)->M[24];
	.endhmacro

    .hmacro SHU1_S2Cyc1
			SHU1.T0 IND TSQ(TB=+0)->IMAC.T0(I1);
			SHU1.COMB(T0,T1)(H)->IMAC.T0(I1);
			SHU1.COMB(T0,T1)(H,TC)->IMAC.T0(I1) || Repeat @ (10);
			SHU1.T2 IND TSQ(TB=+0)->IMAC.T2(I1);
 			SHU1.COMB(T2,T3)(H)->IMAC.T2(I1);
			SHU1.COMB(T2,T3)(H,TC)->IMAC.T2(I1) || Repeat @ (10);
			SHU1.T2 IND TSQ(TB=+0)->IMAC.T2(I1);
			SHU1.COMB(T2,T3)(H)->IMAC.T2(I1);
			SHU1.COMB(T2,T3)(H,TC)->IMAC.T2(I1) || Repeat @ (10);
			SHU1.T0 IND TSQ(TB=+0)->IMAC.T0(I1);
			SHU1.COMB(T0,T1)(H)->IMAC.T0(I1);
			SHU1.COMB(T0,T1)(H,TC)->IMAC.T0(I1) || Repeat @ (10);
	.endhmacro
 
    .hmacro M0_S2Cyc1
			.rept 2
				R0.M[I++]->IMAC.T1(I0) || Repeat @ (12);
				R0.M[I++]->IMAC.T3(I0) || Repeat @ (12);
			.endr
    .endhmacro

    .hmacro IMAC_S2Cyc1
			MR += T0 * T1(CR,H);
			MR += T0 * T1(H) || Repeat @(10);
			MR += T0 * T1(H,L)->IALU.T0;
			MR += T2 * T3(CR,H);
			MR += T2 * T3(H) || Repeat @(10);
			MR += T2 * T3(H,L)->IALU.T0;
			MR += T1 * T2(CR,H);
			MR += T1 * T2(H) || Repeat @(10);
			MR += T1 * T2(H,L)->IALU.T0;
			MR += T0 * T3(CR,H);
			MR += T0 * T3(H) || Repeat @(10);
			MR += T0 * T3(H,L)->IALU.T0;
    .endhmacro   

    .hmacro IALU_S2Cyc1
			NOP;
			T0>>15->IALU.T2;
			T1>>15->IALU.T3;
			Repeat @ (10);
			T0>>15->IALU.T0;
			T1>>15->IALU.T1;
			NOP;
			T2 - T0 (H)->IALU.T0;
			T3 - T1 (H)->IALU.T1;
			Repeat @ (2);
			T1<<16->IALU.T1;
			Repeat @ (2);
			MERGE(T1,T0,T3)(H)->BIU2;
			NOP;
			T0>>15->IALU.T2;
			T1>>15->IALU.T3;
			Repeat @ (10);
			T0>>15->IALU.T0;
			T1>>15->IALU.T1;
			NOP;
			T2 + T0 (H)->IALU.T0;
			T3 + T1 (H)->IALU.T1;
			Repeat @ (2);
			T1<<16->IALU.T1;
			Repeat @ (2);
			MERGE(T1,T0,T3)(H)->BIU2;
    .endhmacro 

	.hmacro M1_S2Cyc1
			.rept 2
				Repeat @ (23);
				R1.M[24]->IALU.T3(I2);
			.endr
	.endhmacro

    .hmacro BIU2_S2Cyc1
			.rept 2
				Repeat @ (23);
	    		BIU2->DM(A++,K++);
            .endr		
    .endhmacro   

    .hmacro MMSE_S2Cyc1
        BIU1_S2;
		Repeat @ (2);
		BIU0_S2Cyc1;
		Repeat @ (7);
		SHU0_S2Cyc1;
		Repeat @ (2);
		SHU1_S2Cyc1;
		M0_S2Cyc1;       
        NOP;
		IMAC_S2Cyc1;
		Repeat @ (15);
		M1_S2Cyc1;
		NOP;
		IALU_S2Cyc1;
		Repeat @ (3);
        BIU2_S2Cyc1;  
    .endhmacro

ChEstMMSE_Stg2Cyc1:
    .opt
    MMSE_S2Cyc1;
    .flushhm
    .stopopt
    Repeat @ (11);
    NOP;
    MPUSTOP;
