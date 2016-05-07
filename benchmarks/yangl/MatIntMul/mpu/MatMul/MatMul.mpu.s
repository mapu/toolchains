//File Name    :  app.m.asm
//Creating Date:  2013-12-01
//Author       :  Yanglei
//Description  :  Modification for adding pileline


// A:M*P  B:P*N
//KI14 = M/BS   3f
//	KI13 = P/K   2f
//		KI12 = N   1f

// K = BS = 16
// efficiency = 16/21 
.text
.global Mat_Mul_Int 


.hmacro BIU0_hm
	LPTO (3f ) @ (KI14)|| LPTO (2f ) @ (KI13); 
 BIU0.DM (A++,K++)->	M[I++] || Repeat @ (15); 
 BIU0.DM (A++,K++)->	M[I++] ; 
	Repeat @ (4);  
	LPTO (1f ) @ (KI12-1); 
	Repeat @ (21); 
	1:
	2:
	3:
.endhmacro


.hmacro BIU1_hm
	LPTO (3f ) @ (KI14)|| LPTO (2f ) @ (KI13)|| LPTO (1f ) @ (KI12); 
 BIU1.DM (A++,K++)->	SHU0.T0 ;  
	Repeat @ (19); 
	NOP;  
	1:
	2:
	3:
.endhmacro


.hmacro SHU0_hm
	LPTO (3f ) @ (KI14)|| LPTO (2f ) @ (KI13)|| LPTO (1f ) @ (KI12); 
 SHU0.T0 IND T1 (TB=+4)->	IMAC.T0(I0) ; 
 SHU0.T0 IND TB (TB+=4)->	IMAC.T0(I0) || Repeat @ (15); 
	Repeat @ (5); 
	1:
	2:
	3:
.endhmacro


.hmacro M0_hm
	LPTO (3f ) @ (KI14)|| LPTO (2f ) @ (KI13)|| LPTO (1f ) @ (KI12); 
 r0.M[I++]->	IMAC.T1 (I1) || Repeat @ (15);  
 r0.M[I++]->	IMAC.T1 (I1) ; 	
	Repeat @ (5); 
	1:
	2:
	3:
.endhmacro


.hmacro IMAC_hm
	LPTO (3f ) @ (KI14)|| LPTO (1f ) @ (KI12); 
	MR+= T0 * T1 (CR); 
	MR+= T0 * T1|| Repeat @ (14); 
 MR+= T0 * T1->	BIU2 ; 
	Repeat @ (4);  
	LPTO (2f ) @ (KI13-1)|| LPTO (2f ) @ (KI12);  
	1:
	MR+= T0 * T1 (CR);  
	MR+= T0 * T1|| Repeat @ (15); 
	Repeat @ (4); 
 MR+= T2->	BIU2 ; 
	2:
	3:
.endhmacro


.hmacro BIU2_LD_hm
	LPTO (3f ) @ (KI14)|| LPTO (1f ) @ (KI12); 
	Repeat @(20); 
	LPTO (2f ) @ (KI13-1)|| LPTO (2f ) @ (KI12); 
	1:
	Repeat @ (12);  
 BIU2.DM->	IMAC.T2(I2) ;  
	Repeat @ (8); 
	2:
	3:
.endhmacro


.hmacro BIU2_ST_hm
	LPTO (3f ) @ (KI14)|| LPTO (2f ) @ (KI13)|| LPTO (1f ) @ (KI12); 
	Repeat @ (15); 
	Repeat @ (5); 	
BIU2->DM(A++,K++); 
	1:
	2:
	3:
.endhmacro



.hmacro main
	BIU0_hm||BIU1_hm;
	Repeat @ (10);
	SHU0_hm||BIU2_LD_hm;      
	M0_hm; 
	NOP; 
	IMAC_hm;  
	Repeat @ (4);  
	BIU2_ST_hm; 

.endhmacro




Mat_Mul_Int :
.opt
main; 
.flushhm
.stopopt
Repeat @ (8); 
NOP; 
MPUSTOP; 




