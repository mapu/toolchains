//  Date: 2014-10-13
//  Name: app.m.asm
//  Author:  xiaolong.guo@ia.ac.cn
//  The configuration Unit for MPU for LS Estimation
.text
    .global ChEstLS_Tail1
    .global ChEstLS_Tail2
    .global ChEstLS_Grp1T1
    .global ChEstLS_Grp1T2
    .global ChEstLS_Grp2T1
    .global ChEstLS_Grp2T2
    .global ChEstLS_Grp3
    .global ChEstLS_Grp3T1
    .global ChEstLS_Grp3T2
    .global ChEstLS_Grp4T1
    .global ChEstLS_Grp4T2
    .global ChEstLS_Grp    
    .global ChEstLS_GrpT1
    .global ChEstLS_GrpT2

//  DataGrpNum <= 4
	.macro _BIU1 name number
		.hmacro \name
		    .rept \number
		        BIU1.DM(A++,K++)->M[0];
		        NOP;
				BIU1.DM(A++,K++)->M[1];
		        NOP;
		        BIU1.DM(A++,K++)->M[4];
		        NOP;
				BIU1.DM(A++,K++)->M[5];
		        NOP;	
		    .endr
        .endhmacro    
	.endmacro

_BIU1 biu1_grp1 1                  
_BIU1 biu1_grp2 2
_BIU1 biu1_grp3 3                  
_BIU1 biu1_grp4 4                 

	.macro _BIU2 name number
		.hmacro \name
		    .rept \number
		        BIU2.DM(A++,K++)->M[2];
		        NOP;
				BIU2.DM(A++,K++)->M[3];
				NOP;
				BIU2.DM(A++,K++)->M[6];
		        NOP;
				BIU2.DM(A++,K++)->M[7];
				NOP;
		    .endr
		.endhmacro
	.endmacro
	
_BIU2 biu2_grp1 1                  
_BIU2 biu2_grp2 2
_BIU2 biu2_grp3 3                  
_BIU2 biu2_grp4 4

	.macro _MR0 name number
		.hmacro \name
			.rept \number
				R0.M[0]->IMAC.T0(I0);
				R0.M[1]->IMAC.T1(I0) || Repeat @ (2);
				R0.M[0]->IMAC.T0(I0);
				R0.M[4]->IMAC.T0(I0);
				R0.M[5]->IMAC.T1(I0) || Repeat @ (2);
				R0.M[4]->IMAC.T0(I0);
			.endr
		.endhmacro
	.endmacro

_MR0 mr0_grp1 1
_MR0 mr0_grp2 2
_MR0 mr0_grp3 3
_MR0 mr0_grp4 4

	.macro _MR1 name number	
		.hmacro \name
			.rept \number
				R1.M[2]->IMAC.T2(I1);
				R1.M[3]->IMAC.T3(I1);
				R1.M[2]->IMAC.T2(I1);
				R1.M[3]->IMAC.T3(I1);
				R1.M[6]->IMAC.T2(I1);
				R1.M[7]->IMAC.T3(I1);
				R1.M[6]->IMAC.T2(I1);
				R1.M[7]->IMAC.T3(I1);
			.endr
		.endhmacro
	.endmacro

_MR1 mr1_grp1 1
_MR1 mr1_grp2 2
_MR1 mr1_grp3 3
_MR1 mr1_grp4 4

	.macro _IMAC name number
		.hmacro \name
		    .rept \number
	 	    	T0 * T2 (T,H)->IALU.T0;
		        T1 * T3 (T,H)->IALU.T1;
				T1 * T2 (T,H)->IALU.T2;
				T0 * T3 (T,H)->IALU.T3;
		    .endr
		.endhmacro   
	.endmacro

_IMAC imac_grp1 2                  
_IMAC imac_grp2 4
_IMAC imac_grp3 6                  
_IMAC imac_grp4 8

	.macro _IALU name number
		.hmacro \name
		    .rept \number
				T0 + T1 (H)->BIU0;
		        NOP;
				T2 - T3 (H)->BIU0;
		        NOP;
		    .endr
		.endhmacro 
	.endmacro

_IALU ialu_grp1 2                  
_IALU ialu_grp2 4
_IALU ialu_grp3 6                  
_IALU ialu_grp4 8

	.macro _BIU0 name number
		.hmacro \name
		    .rept \number
		        BIU0->DM(A++,K++);
				NOP;
			.endr
		.endhmacro   
    .endmacro

_BIU0 biu0_grp1 4                  
_BIU0 biu0_grp2 8
_BIU0 biu0_grp3 12                  
_BIU0 biu0_grp4 16

	.macro _LS name biu1 biu2 mr0 mr1 imac ialu biu0
		.hmacro \name
		    \biu1 || \biu2;
		    Repeat @ (12);
        	\mr0 || \mr1;
        	NOP;
			\imac;       
        	Repeat @ (5);
			\ialu;
			Repeat @ (3);
        	\biu0; 
		.endhmacro
	.endmacro
 
_LS LS_Grp1 biu1_grp1 biu2_grp1 mr0_grp1 mr1_grp1 imac_grp1 ialu_grp1 biu0_grp1
_LS LS_Grp2 biu1_grp2 biu2_grp2 mr0_grp2 mr1_grp2 imac_grp2 ialu_grp2 biu0_grp2
_LS LS_Grp3 biu1_grp3 biu2_grp3 mr0_grp3 mr1_grp3 imac_grp3 ialu_grp3 biu0_grp3
_LS LS_Grp4 biu1_grp4 biu2_grp4 mr0_grp4 mr1_grp4 imac_grp4 ialu_grp4 biu0_grp4   
   
//  DataTailNum <= 32
	.hmacro LS_Tail1
		BIU1.DM(A++,K++,M)->M[0] || BIU2.DM(A++,K++,M)->M[2];
		NOP;
	    BIU1.DM(A++,K++,M)->M[1] || BIU2.DM(A++,K++,M)->M[3];
		Repeat @ (10);
		R0.M[0]->IMAC.T0(I0) || R1.M[2]->IMAC.T2(I1);
		R0.M[1]->IMAC.T1(I0) || R1.M[3]->IMAC.T3(I1);
	    R0.M[1]->IMAC.T1(I0) || R1.M[2]->IMAC.T2(I1) || T0 * T2 (T,H)->IALU.T0;
		R0.M[0]->IMAC.T0(I0) || R1.M[3]->IMAC.T3(I1) || T1 * T3 (T,H)->IALU.T1;	    
		T1 * T2 (T,H)->IALU.T2;
        T0 * T3 (T,H)->IALU.T3;
		Repeat @ (2);
		T0 + T1 (H)->BIU0;
		NOP;
		T2 - T3 (H)->BIU0;
		NOP;
		BIU0->DM(A++,K++,M);
		NOP;
		BIU0->DM(A++,K++,M);
	.endhmacro    

//  DataTailNum <= 64 && DataTailNum > 32
	.hmacro LS_Tail2
		BIU1.DM(A++,K++)->M[0] || BIU2.DM(A++,K++)->M[2];
		NOP;
	    BIU1.DM(A++,K++)->M[1] || BIU2.DM(A++,K++)->M[3];
	    NOP;
	    BIU1.DM(A++,K++,M)->M[4] || BIU2.DM(A++,K++,M)->M[6];
		NOP;
	    BIU1.DM(A++,K++,M)->M[5] || BIU2.DM(A++,K++,M)->M[7];
		Repeat @ (6);
		R0.M[0]->IMAC.T0(I0) || R1.M[2]->IMAC.T2(I1);
		R0.M[1]->IMAC.T1(I0) || R1.M[3]->IMAC.T3(I1);
	    R0.M[1]->IMAC.T1(I0) || R1.M[2]->IMAC.T2(I1) || T0 * T2 (T,H)->IALU.T0;
		R0.M[0]->IMAC.T0(I0) || R1.M[3]->IMAC.T3(I1) || T1 * T3 (T,H)->IALU.T1;	
		R0.M[4]->IMAC.T0(I0) || R1.M[6]->IMAC.T2(I1) || T1 * T2 (T,H)->IALU.T2;
		R0.M[5]->IMAC.T1(I0) || R1.M[7]->IMAC.T3(I1) || T0 * T3 (T,H)->IALU.T3;
	    R0.M[5]->IMAC.T1(I0) || R1.M[6]->IMAC.T2(I1) || T0 * T2 (T,H)->IALU.T0;
		R0.M[4]->IMAC.T0(I0) || R1.M[7]->IMAC.T3(I1) || T1 * T3 (T,H)->IALU.T1;	    
		T1 * T2 (T,H)->IALU.T2 || T0 + T1 (H)->BIU0;
        T0 * T3 (T,H)->IALU.T3;
		T2 - T3 (H)->BIU0;
		NOP;
		T0 + T1 (H)->BIU0 || BIU0->DM(A++,K++);
		NOP;
		T2 - T3 (H)->BIU0 || BIU0->DM(A++,K++);
		NOP;
		BIU0->DM(A++,K++,M);
		NOP;
		BIU0->DM(A++,K++,M);
	.endhmacro 

//  DataGrpNum > 4
    .hmacro BIU1_Grp
        LPTO (1f ) @ (KI12);
            BIU1.DM(A++,K++)->M[0];
            NOP;
	    	BIU1.DM(A++,K++)->M[1];
            NOP;
            BIU1.DM(A++,K++)->M[4];
            NOP;
	    	BIU1.DM(A++,K++)->M[5];
            NOP;
        1:
    .endhmacro

    .hmacro BIU2_Grp
        LPTO (1f ) @ (KI12);
            BIU2.DM(A++,K++)->M[2];
            NOP;
	    	BIU2.DM(A++,K++)->M[3];
	    	NOP;
	    	BIU2.DM(A++,K++)->M[6];
            NOP;
	    	BIU2.DM(A++,K++)->M[7];
	    	NOP;
        1:
    .endhmacro

	.hmacro MR0_Grp
		LPTO (1f ) @ (KI12);
			R0.M[0]->IMAC.T0(I0);
			R0.M[1]->IMAC.T1(I0) || Repeat @ (2);
			R0.M[0]->IMAC.T0(I0);
			R0.M[4]->IMAC.T0(I0);
			R0.M[5]->IMAC.T1(I0) || Repeat @ (2);
			R0.M[4]->IMAC.T0(I0);
		1:
	.endhmacro
	
	.hmacro MR1_Grp
		LPTO (1f ) @ (KI12);
			R1.M[2]->IMAC.T2(I1);
			R1.M[3]->IMAC.T3(I1);
			R1.M[2]->IMAC.T2(I1);
			R1.M[3]->IMAC.T3(I1);
			R1.M[6]->IMAC.T2(I1);
			R1.M[7]->IMAC.T3(I1);
			R1.M[6]->IMAC.T2(I1);
			R1.M[7]->IMAC.T3(I1);
		1:
	.endhmacro
	
    .hmacro IMAC_Grp
        LPTO (1f ) @ (KI12);
        	.rept 2
	 	    	T0 * T2 (T,H)->IALU.T0;
		        T1 * T3 (T,H)->IALU.T1;
				T1 * T2 (T,H)->IALU.T2;
				T0 * T3 (T,H)->IALU.T3;
	    	.endr
        1:
    .endhmacro   

    .hmacro IALU_Grp
        LPTO (1f ) @ (KI12);
        	.rept 2
				T0 + T1 (H)->BIU0;
		        NOP;
				T2 - T3 (H)->BIU0;
		        NOP;
		    .endr
        1:
    .endhmacro 

    .hmacro BIU0_Grp
        LPTO (1f ) @ (KI12);
        	.rept 4
            	BIU0->DM(A++,K++);
	    		NOP;
        	.endr
		1:
    .endhmacro   
    
    .hmacro LS_Grp
        BIU1_Grp || BIU2_Grp;
        Repeat @ (12);
        MR0_Grp || MR1_Grp;
        NOP;
		IMAC_Grp;       
        Repeat @ (5);
		IALU_Grp;
		Repeat @ (3);
        BIU0_Grp;  
    .endhmacro

// 24(12 24)
ChEstLS_Tail1: 
    .opt
    LS_Tail1;
    .flushhm
    .stopopt
    Repeat @ (11);
    NOP;
    MPUSTOP;

// 48(36 48 60)
ChEstLS_Tail2: 
    .opt
    LS_Tail2;
    .flushhm
    .stopopt
    Repeat @ (11);
    NOP;
    MPUSTOP;
    
// 72 96(72 84 96)
ChEstLS_Grp1T1: 
    .opt
    LS_Grp1;
    .flushhm
    LS_Tail1;
    .flushhm
    .stopopt
    Repeat @ (11);
    NOP;
    MPUSTOP;

// 120(108 120)
ChEstLS_Grp1T2: 
    .opt
    LS_Grp1;
    .flushhm
    LS_Tail2;
    .flushhm
    .stopopt
    Repeat @ (11);
    NOP;
    MPUSTOP;
    
// 144(132 144 156)
ChEstLS_Grp2T1: 
    .opt
    LS_Grp2;
    .flushhm
    LS_Tail1;
    .flushhm
    .stopopt
    Repeat @ (11);
    NOP;
    MPUSTOP;

// 168(168 180)
ChEstLS_Grp2T2: 
    .opt
    LS_Grp2;
    .flushhm
    LS_Tail2;
    .flushhm
    .stopopt
    Repeat @ (11);
    NOP;
    MPUSTOP;    
    
// 192(192)
ChEstLS_Grp3: 
    .opt
    LS_Grp3;
    .flushhm
    .stopopt
    Repeat @ (11);
    NOP;
    MPUSTOP;

// 216(204 216)
ChEstLS_Grp3T1: 
    .opt
    LS_Grp3;
    .flushhm
    LS_Tail1;
    .flushhm
    .stopopt
    Repeat @ (11);
    NOP;
    MPUSTOP;
    
// 240(228 240 252)
ChEstLS_Grp3T2: 
    .opt
    LS_Grp3;
    .flushhm
    LS_Tail2;
    .flushhm
    .stopopt
    Repeat @ (11);
    NOP;
    MPUSTOP;  

// 264 288(264 276 288)
ChEstLS_Grp4T1: 
    .opt
    LS_Grp4;
    .flushhm
    LS_Tail1;
    .flushhm
    .stopopt
    Repeat @ (11);
    NOP;
    MPUSTOP;
    
// 312(300 312)
ChEstLS_Grp4T2: 
    .opt
    LS_Grp4;
    .flushhm
    LS_Tail2;
    .flushhm
    .stopopt
    Repeat @ (11);
    NOP;
    MPUSTOP;  

// >=320
ChEstLS_Grp: 
    .opt
    LS_Grp;
    .flushhm
    .stopopt
    Repeat @ (11);
    NOP;
    MPUSTOP;

ChEstLS_GrpT1: 
    .opt
    LS_Grp;
    .flushhm
    LS_Tail1;
    .flushhm
    .stopopt
    Repeat @ (11);
    NOP;
    MPUSTOP;
    
ChEstLS_GrpT2: 
    .opt
    LS_Grp;
    .flushhm
    LS_Tail2;
    .flushhm
    .stopopt
    Repeat @ (11);
    NOP;
    MPUSTOP;
