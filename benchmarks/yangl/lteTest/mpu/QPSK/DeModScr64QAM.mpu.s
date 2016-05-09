//  Date: 2015-5-24
//  Name: app.m.asm
//  Author:  lihuan
//  The configuration Unit for MPU for Demodscr (64QAM)
    .text
    .global Demodscr_64QAM
    	
	.hmacro BIU2_Group
		LPTO (1f ) @ (KI12);
	        BIU2.DM(A++,K++)->M[0];
			NOP;
	        BIU2.DM(A++,K++)->M[1];
			NOP;
	        BIU2.DM(A++,K++)->M[2];
			NOP;
	        BIU2.DM(A++,K++)->M[3];
			NOP;
	        BIU2.DM(A++,K++)->M[4];
			NOP;
	        BIU2.DM(A++,K++)->M[5];
			NOP;
		   	Repeat@(35);
		1:
    .endhmacro

	.hmacro BIU1_Group
		LPTO (1f ) @ (KI12);
	        BIU1.DM(A++,K++)->M[17];
			NOP;
	        BIU1.DM(A++,K++)->M[18];
			NOP;
	        BIU1.DM(A++,K++)->M[19];
			NOP;
	        BIU1.DM(A++,K++)->M[20];
			NOP;
	        BIU1.DM(A++,K++)->M[21];
			NOP;
	        BIU1.DM(A++,K++)->M[22];
			NOP;
		   	Repeat@(35);
		1:
    .endhmacro

	.hmacro M0_Group
		LPTO (1f ) @ (KI12);
	        R0.M[0]->IALU.T0(I0);
	        R0.M[1]->IALU.T0(I0);
	        R0.M[2]->IALU.T0(I0);
	        R0.M[14]->IALU.T2(I0);
			NOP;
			NOP;
	        R0.M[3]->IALU.T0(I0);
	        R0.M[4]->IALU.T0(I0);
	        R0.M[5]->IALU.T0(I0);
			NOP;
			NOP;
			NOP;
	        R0.M[6]->IALU.T0(I0);
	        R0.M[7]->IALU.T0(I0);
	        R0.M[8]->IALU.T0(I0);
	        R0.M[15]->IALU.T2(I0);
			NOP;
			NOP;
	        R0.M[9]->IALU.T0(I0);
	        R0.M[10]->IALU.T0(I0);
	        R0.M[11]->IALU.T0(I0);
			NOP;
			NOP;
			NOP;
	        R0.M[0]->IALU.T1(I0);
	        R0.M[1]->IALU.T1(I0);
	        R0.M[2]->IALU.T1(I0);
	        R0.M[6]->IALU.T2(I0);
	        R0.M[7]->IALU.T2(I0);
	        R0.M[8]->IALU.T2(I0);
	        R0.M[3]->IALU.T1(I0);
	        R0.M[4]->IALU.T1(I0);
	        R0.M[5]->IALU.T1(I0);
	        R0.M[9]->IALU.T2(I0);
	        R0.M[10]->IALU.T2(I0);
	        R0.M[11]->IALU.T2(I0);
			NOP;
			NOP;
	        R0.M[6]->IALU.T0(I0);
	        R0.M[7]->IALU.T0(I0);
	        R0.M[8]->IALU.T0(I0);
			NOP;
			NOP;
			NOP;
	        R0.M[9]->IALU.T0(I0);
	        R0.M[10]->IALU.T0(I0);
	        R0.M[11]->IALU.T0(I0);						
		1:
    .endhmacro

	.hmacro M1_Group
		LPTO (1f ) @ (KI12);
			R1.M[16]->IALU.T3(I1);			
			Repeat @ (11);
	        R1.M[6]->SHU0.T2;
	        R1.M[7]->SHU0.T2;
	        R1.M[8]->SHU0.T2;
	        R1.M[0]->SHU0.T0;
	        R1.M[1]->SHU0.T0;
	        R1.M[2]->SHU0.T0;
	        R1.M[9]->SHU0.T2;
	        R1.M[10]->SHU0.T2;
	        R1.M[11]->SHU0.T2;
	        R1.M[3]->SHU0.T0;
	        R1.M[4]->SHU0.T0;
	        R1.M[5]->SHU0.T0;
	        R1.M[12]->IALU.T2(I1);
			NOP;
			NOP;
	        R1.M[13]->IALU.T3(I1);
			NOP;
			NOP;
	        R1.M[12]->IALU.T2(I1);
			NOP;
			NOP;
	        R1.M[13]->IALU.T3(I1);
			NOP;
			NOP;
			NOP;
			NOP;
	        R1.M[17]->IALU.T3(I1);
	        R1.M[18]->IALU.T3(I1);
	        R1.M[19]->IALU.T3(I1);
			NOP;
			NOP;
			NOP;
	        R1.M[20]->IALU.T3(I1);
	        R1.M[21]->IALU.T3(I1);
	        R1.M[22]->IALU.T3(I1);			
		1:
    .endhmacro

	.hmacro M2_Group
		LPTO (1f ) @ (KI12);		
			Repeat @ (33);
	        R2.M[6]->IMAC.T0(I2);
	        R2.M[7]->IMAC.T0(I2);
	        R2.M[8]->IMAC.T0(I2);
			NOP;
			NOP;
			NOP;
	        R2.M[9]->IMAC.T0(I2);
	        R2.M[10]->IMAC.T0(I2);
	        R2.M[11]->IMAC.T0(I2);
			Repeat @ (5);
		1:
    .endhmacro

	.hmacro M3_Group
		LPTO (1f ) @ (KI12);
			Repeat @ (33);
	        R3.M[6]->BIU0;
			NOP;
	        R3.M[7]->BIU0;
			NOP;
	        R3.M[8]->BIU0;
			NOP;
	        R3.M[9]->BIU0;
			NOP;
	        R3.M[10]->BIU0;
			NOP;
	        R3.M[11]->BIU0;
			NOP;
			Repeat@(2);			
		1:
    .endhmacro

	.hmacro SHU0_Group
		SHU0.T0 IND TSQ->M[12];
		SHU0.T2 IND TSQ->M[13];
		Repeat@(3);
		SHU0.T1 IND TBH->IMAC.T1(I0);
		Repeat@(8);
		LPTO (1f ) @ (KI12);
			Repeat@(15);
			SHU0.T2 IND T3->M[6];
			SHU0.T2 IND T3->M[7];
			SHU0.T2 IND T3->M[8];
			SHU0.T0 IND T3->M[0];
			SHU0.T0 IND T3->M[1];
			SHU0.T0 IND T3->M[2];
			SHU0.T2 IND T3->M[9];
			SHU0.T2 IND T3->M[10];
			SHU0.T2 IND T3->M[11];
			SHU0.T0 IND T3->M[3];
			SHU0.T0 IND T3->M[4];
			SHU0.T0 IND T3->M[5];
			Repeat@(20);
	    1:
    .endhmacro

	.hmacro SHU1_Group
		SHU1.T0 IND TBH->M[14];
		SHU1.T1 IND TBH->M[15];
		SHU1.T2 IND TBH->M[16];
		Repeat@(9);
		LPTO (1f ) @ (KI12);
			Repeat@(23);
			SHU1.T0 IND T3->IALU.T0(I2);
			SHU1.T1 IND T3->IALU.T0(I2);
			SHU1.T2 IND T3->IALU.T0(I2);
			NOP;
			NOP;
			NOP;
			SHU1.T0 IND T3->IALU.T0(I2);
			SHU1.T1 IND T3->IALU.T0(I2);
			SHU1.T2 IND T3->IALU.T0(I2);
			NOP;
			NOP;
			NOP;
			Repeat@(12);
	    1:
    .endhmacro

	.hmacro IMAC_Group
		LPTO (1f ) @ (KI12);
			Repeat@(33);
			T0*T1(B,T)->IALU.T1;
			T0*T1(B,T)->IALU.T1;
			T0*T1(B,T)->IALU.T1;
			NOP;
			NOP;
			NOP;
			T0*T1(B,T)->IALU.T1;
			T0*T1(B,T)->IALU.T1;
			T0*T1(B,T)->IALU.T1;
			NOP;
			NOP;
			NOP;
			Repeat@(2);
	    1:
    .endhmacro

    .hmacro IALU_Group
		LPTO (1f ) @ (KI12);
			ABS(T0,T3)(H)->IALU.T1;
			ABS(T0,T3)(H)->IALU.T1;
			ABS(T0,T3)(H)->IALU.T1;
			T2 - T1(H,T)->M[6];
			T2 - T1(H,T)->M[7];
			T2 - T1(H,T)->M[8];
			ABS(T0,T3)(H)->IALU.T1;
			ABS(T0,T3)(H)->IALU.T1;
			ABS(T0,T3)(H)->IALU.T1;
			T2 - T1(H,T)->M[9];
			T2 - T1(H,T)->M[10];
			T2 - T1(H,T)->M[11];
			ABS(T0,T3)(H)->IALU.T1;
			ABS(T0,T3)(H)->IALU.T1;
			ABS(T0,T3)(H)->IALU.T1;
			T2 - T1(H,T)->SHU1.T0;
			T2 - T1(H,T)->SHU1.T1;
			T2 - T1(H,T)->SHU1.T2;
			ABS(T0,T3)(H)->IALU.T1;
			ABS(T0,T3)(H)->IALU.T1;
			ABS(T0,T3)(H)->IALU.T1;
			T2 - T1(H,T)->SHU1.T0;
			T2 - T1(H,T)->SHU1.T1;
			T2 - T1(H,T)->SHU1.T2;
			MERGE(T0,T1,T2)(B)->IALU.T1;
			MERGE(T0,T1,T2)(B)->IALU.T1;
			MERGE(T0,T1,T2)(B)->IALU.T1;
			MERGE(T1,T2,T3)(B)->M[6];
			MERGE(T1,T2,T3)(B)->M[7];
			MERGE(T1,T2,T3)(B)->M[8];
			MERGE(T0,T1,T2)(B)->IALU.T1;
			MERGE(T0,T1,T2)(B)->IALU.T1;
			MERGE(T0,T1,T2)(B)->IALU.T1;
			MERGE(T1,T2,T3)(B)->M[9];
			MERGE(T1,T2,T3)(B)->M[10];
			MERGE(T1,T2,T3)(B)->M[11];
			NOP;
			NOP;
			MERGE(T0,T1,T3)(B)->M[6];
			MERGE(T0,T1,T3)(B)->M[7];
			MERGE(T0,T1,T3)(B)->M[8];
			NOP;
			NOP;
			NOP;
			MERGE(T0,T1,T3)(B)->M[9];
			MERGE(T0,T1,T3)(B)->M[10];
			MERGE(T0,T1,T3)(B)->M[11];
		1:
    .endhmacro

    .hmacro BIU0_Group
		LPTO (1f ) @ (KI12);	
			Repeat@(35);
			.rept 6
			NOP;
          	BIU0->DM(A++,K++);
			.endr

		1:
    .endhmacro 
 
    .hmacro Demodscr_64QAM_Group
			BIU2_Group || BIU1_Group || SHU0_Group;
			NOP;
			SHU1_Group;     
			Repeat @ (11);
			M2_Group || M1_Group || M0_Group;
			NOP;
			IMAC_Group || IALU_Group;
			Repeat @ (8);
			M3_Group || BIU0_Group;        	  
    .endhmacro

Demodscr_64QAM:
      .opt
      Demodscr_64QAM_Group;
      .flushhm
      .stopopt
      Repeat @ (11);	
      NOP;
      MPUSTOP;

