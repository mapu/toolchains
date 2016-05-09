//  Date: 2015-04-13
//  Name: app.m.asm
//  Author:  zhaoxuying2012@ia.ac.cn
//  The configuration Unit for MPU for de sub-block interleaver
    .text
    .global DeSubBlk_Interleaver
    .global DeBit_Select_Sys1
    .global DeBit_Select_Sys2
    .global DeBit_Select_Sys3
    .global DeBit_Select_Sys4
    .global DeBit_Select_Sys

    .global DeBit_Select_par8
    .global DeBit_Select_par6
    .global DeBit_Select_par4
    .global DeBit_Select_par2
    .global par_end2

    .global special_par8
    .global special_par6
    .global special_par4
    .global special_par2


//de-bit-sys:
 .hmacro BIU0_sys_HM
        LPTO (1f ) @ (KI12);
            BIU0.DM(A++,K++)->M[0];
            NOP;
        1:
 .endhmacro

 

 .hmacro R3_sys_HM
        LPTO (1f) @ (KI12);
           R3.M[0] -> BIU2;
           NOP;
        1:
 .endhmacro


 .hmacro BIU2_sys_HM
        LPTO (1f) @ (KI12);
           BIU2->DM(A++,K++);
           NOP;
        1:
 .endhmacro

  .hmacro main_sys
    BIU0_sys_HM          ; 
    Repeat @ (11)    ; 
    R3_sys_HM            ;
    Repeat @ (2)     ; 
    BIU2_sys_HM          ;
  .endhmacro


DeBit_Select_Sys:
    .opt
    main_sys;
    .flushhm
    .stopopt
    Repeat @ (14);
    NOP;
    MPUSTOP;

//de-subblock interleaver:
 .hmacro BIU2_HM
        LPTO (1f ) @ (KI12);
            BIU2.DM(A++,K++)->SHU0.T2;
            NOP;
            BIU2.DM(A++,K++)->SHU0.T2;
            NOP;
        1:
 .endhmacro

 

 .hmacro SHU0_HM
        LPTO (1f) @ (KI12);
           SHU0.T2 IND T0 -> BIU1;
           NOP;
           SHU0.T2 IND T1 -> BIU1;
           NOP;
        1:
 .endhmacro


 .hmacro BIU1_HM
        LPTO (1f) @ (KI12);
           BIU1->DM(A++,K++);
           NOP;
           BIU1->DM(A++,K++);
           NOP;
        1:
 .endhmacro

  .hmacro main
    BIU2_HM          ; 
    Repeat @ (10)    ; 
    SHU0_HM          ;
    Repeat @ (2)     ; 
    BIU1_HM          ;
  .endhmacro


DeSubBlk_Interleaver:
    .opt
    main;
    .flushhm
    .stopopt
    Repeat @ (14);
    NOP;
    MPUSTOP;

//de bit-select : sys 
    .macro _BIU0_DeBit_Select_SYS name number               
	.hmacro \name                        
           .rept \number                    
		BIU0.DM(A++,K++)->M[0];           
	        NOP;                                     
	   .endr                              
	.endhmacro                           
    .endmacro                            
                                         
_BIU0_DeBit_Select_SYS biu0_debitsel_sys1 1                        
_BIU0_DeBit_Select_SYS biu0_debitsel_sys2 2                        
_BIU0_DeBit_Select_SYS biu0_debitsel_sys3 3                        
_BIU0_DeBit_Select_SYS biu0_debitsel_sys4 4                        
                      
    .macro _BIU2_DeBit_Select_SYS name number               
	.hmacro \name                        
           .rept \number                    
		BIU2->DM(A++,K++);          
	        NOP;                                     
	   .endr                              
	.endhmacro                           
    .endmacro                            
                                         
_BIU2_DeBit_Select_SYS biu2_debitsel_sys1 1                        
_BIU2_DeBit_Select_SYS biu2_debitsel_sys2 2                        
_BIU2_DeBit_Select_SYS biu2_debitsel_sys3 3                        
_BIU2_DeBit_Select_SYS biu2_debitsel_sys4 4   

    .macro _R3_DeBit_Select_SYS name number               
	.hmacro \name                        
           .rept \number                    
		R3.M[0]->BIU2;         
	        NOP;                                     
	   .endr                              
	.endhmacro                           
    .endmacro                            
                                         
_R3_DeBit_Select_SYS r3_debitsel_sys1 1                        
_R3_DeBit_Select_SYS r3_debitsel_sys2 2                        
_R3_DeBit_Select_SYS r3_debitsel_sys3 3                        
_R3_DeBit_Select_SYS r3_debitsel_sys4 4         
   

  .hmacro main_DeBit_Select_Sys1
    biu0_debitsel_sys1          ; 
    Repeat @ (11)    ; 
    r3_debitsel_sys1            ;
    Repeat @ (2)     ; 
    biu2_debitsel_sys1          ;
  .endhmacro

  .hmacro main_DeBit_Select_Sys2
    biu0_debitsel_sys2          ; 
    Repeat @ (11)    ; 
    r3_debitsel_sys2            ;
    Repeat @ (2)     ; 
    biu2_debitsel_sys2          ;
  .endhmacro

  .hmacro main_DeBit_Select_Sys3
    biu0_debitsel_sys3          ; 
    Repeat @ (11)    ; 
    r3_debitsel_sys3            ;
    Repeat @ (2)     ; 
    biu2_debitsel_sys3          ;
  .endhmacro

  .hmacro main_DeBit_Select_Sys4
    biu0_debitsel_sys4          ; 
    Repeat @ (11)    ; 
    r3_debitsel_sys4            ;
    Repeat @ (2)     ; 
    biu2_debitsel_sys4          ;
  .endhmacro

   DeBit_Select_Sys1:
    .opt
    main_DeBit_Select_Sys1;
    .flushhm
    .stopopt
    Repeat @ (14);
    NOP;
    MPUSTOP;

   DeBit_Select_Sys2:
    .opt
    main_DeBit_Select_Sys2;
    .flushhm
    .stopopt
    Repeat @ (14);
    NOP;
    MPUSTOP;

   DeBit_Select_Sys3:
    .opt
    main_DeBit_Select_Sys3;
    .flushhm
    .stopopt
    Repeat @ (14);
    NOP;
    MPUSTOP;

   DeBit_Select_Sys4:
    .opt
    main_DeBit_Select_Sys4;
    .flushhm
    .stopopt
    Repeat @ (14);
    NOP;
    MPUSTOP;

//de bit select: par
    .macro _BIU0_DeBit_Select_Par name number               
	.hmacro \name                        
           .rept \number                    
		BIU0.DM(A++,K++)->SHU0.T0;           
	        NOP;                                     
	   .endr                              
	.endhmacro                           
    .endmacro                            
                                         
_BIU0_DeBit_Select_Par biu0_debitsel_par2 2                        
_BIU0_DeBit_Select_Par biu0_debitsel_par4 4                        
_BIU0_DeBit_Select_Par biu0_debitsel_par6 6                        
_BIU0_DeBit_Select_Par biu0_debitsel_par8 8  

    .macro _BIU2_DeBit_Select_Par name number               
	.hmacro \name                        
           .rept \number                    
		BIU2->DM(A++,K++);           
	        NOP;                                     
	   .endr                              
	.endhmacro                           
    .endmacro                            
                                         
_BIU2_DeBit_Select_Par biu2_debitsel_par2 2                        
_BIU2_DeBit_Select_Par biu2_debitsel_par4 4                        
_BIU2_DeBit_Select_Par biu2_debitsel_par6 6                        
_BIU2_DeBit_Select_Par biu2_debitsel_par8 8   
      
    .macro _SHU0_DeBit_Select_Par name number               
	.hmacro \name 
          SHU0.T3 IND TSQ->IALU.T0(I0); 
          Repeat @ (10);                      
           .rept \number                    
		SHU0.T0 IND T1->IALU.T2(I0);           
	        NOP; 
		SHU0.T0 IND T2->IALU.T3(I0);           
	        NOP;                                        
	   .endr                              
	.endhmacro                           
    .endmacro                            
                                         
_SHU0_DeBit_Select_Par shu0_debitsel_par2 1                        
_SHU0_DeBit_Select_Par shu0_debitsel_par4 2                        
_SHU0_DeBit_Select_Par shu0_debitsel_par6 3                        
_SHU0_DeBit_Select_Par shu0_debitsel_par8 4   

    .macro _SHU1_DeBit_Select_Par name number               
	.hmacro \name                   
           .rept \number                    
		SHU1.T1 IND T0->BIU2;           
	        Repeat @ (3);                                      
	   .endr                              
	.endhmacro                           
    .endmacro                            
                                         
_SHU1_DeBit_Select_Par shu1_debitsel_par2 1                        
_SHU1_DeBit_Select_Par shu1_debitsel_par4 2                        
_SHU1_DeBit_Select_Par shu1_debitsel_par6 3                        
_SHU1_DeBit_Select_Par shu1_debitsel_par8 4  

    .macro _IALU_DeBit_Select_Par name number               
	.hmacro \name                   
           .rept \number                    
		MERGE(T3,T2,T0)(B)->SHU1.T1;           
	        MERGE(T2,T3,T0)(B)->BIU2;
                Repeat @ (2);                                    
	   .endr                              
	.endhmacro                           
    .endmacro                            
                                         
_IALU_DeBit_Select_Par ialu_debitsel_par2 1                        
_IALU_DeBit_Select_Par ialu_debitsel_par4 2                        
_IALU_DeBit_Select_Par ialu_debitsel_par6 3                        
_IALU_DeBit_Select_Par ialu_debitsel_par8 4  


  .hmacro main_DeBit_Select_par2
    biu0_debitsel_par2 || shu0_debitsel_par2; 
    Repeat @ (15)                           ; 
    ialu_debitsel_par2                      ;
    Repeat @ (3)                            ; 
    shu1_debitsel_par2                      ;
    biu2_debitsel_par2                      ;
  .endhmacro


   DeBit_Select_par2:
    .opt
    main_DeBit_Select_par2;
    .flushhm
    .stopopt
    Repeat @ (14);
    NOP;
    MPUSTOP;

  .hmacro main_DeBit_Select_par4
    biu0_debitsel_par4 || shu0_debitsel_par4; 
    Repeat @ (15)                           ; 
    ialu_debitsel_par4                      ;
    Repeat @ (3)                            ; 
    shu1_debitsel_par4                      ;
    biu2_debitsel_par4                      ;
  .endhmacro


   DeBit_Select_par4:
    .opt
    main_DeBit_Select_par4;
    .flushhm
    .stopopt
    Repeat @ (14);
    NOP;
    MPUSTOP;

  .hmacro main_DeBit_Select_par6
    biu0_debitsel_par6 || shu0_debitsel_par6; 
    Repeat @ (15)                           ; 
    ialu_debitsel_par6                      ;
    Repeat @ (3)                            ; 
    shu1_debitsel_par6                      ;
    biu2_debitsel_par6                      ;
  .endhmacro


   DeBit_Select_par6:
    .opt
    main_DeBit_Select_par6;
    .flushhm
    .stopopt
    Repeat @ (14);
    NOP;
    MPUSTOP;

  .hmacro main_DeBit_Select_par8
    biu0_debitsel_par8 || shu0_debitsel_par8; 
    Repeat @ (15)                           ; 
    ialu_debitsel_par8                      ;
    Repeat @ (3)                            ; 
    shu1_debitsel_par8                      ;
    biu2_debitsel_par8                      ;
  .endhmacro


   DeBit_Select_par8:
    .opt
    main_DeBit_Select_par8;
    .flushhm
    .stopopt
    Repeat @ (14);
    NOP;
    MPUSTOP;

//de bit select: par special
    .macro _BIU0_DeBit_Select_Par_Special name number               
	.hmacro \name                        
           .rept \number                    
		BIU0.DM(A++,K++)->SHU0.T0;           
	        NOP;                                     
	   .endr                              
	.endhmacro                           
    .endmacro                            
                                         
_BIU0_DeBit_Select_Par_Special biu0_debitsel_par2_special 2                        
_BIU0_DeBit_Select_Par_Special biu0_debitsel_par4_special 4                        
_BIU0_DeBit_Select_Par_Special biu0_debitsel_par6_special 6                        
_BIU0_DeBit_Select_Par_Special biu0_debitsel_par8_special 8  

    .macro _BIU2_DeBit_Select_Par_Special name number               
	.hmacro \name                        
           .rept \number                    
		BIU2->DM(A++,K++);
	        NOP;    
		BIU2->DM(A++,K++);            
	        NOP;                                    
	   .endr                              
	.endhmacro                           
    .endmacro                            
                                         
_BIU2_DeBit_Select_Par_Special biu2_debitsel_par2_special 2                        
_BIU2_DeBit_Select_Par_Special biu2_debitsel_par4_special 4                        
_BIU2_DeBit_Select_Par_Special biu2_debitsel_par6_special 6                        
_BIU2_DeBit_Select_Par_Special biu2_debitsel_par8_special 8   
      
    .macro _SHU0_DeBit_Select_Par_Special name number               
	.hmacro \name 
          SHU0.T3 IND TSQ->IALU.T0(I0); 
          Repeat @ (10);                      
           .rept \number                    
		SHU0.T0 IND T1->IALU.T2(I0);           
	        NOP; 
		SHU0.T0 IND T2->IALU.T3(I0);           
	        NOP;                                        
	   .endr                              
	.endhmacro                           
    .endmacro                            
                                         
_SHU0_DeBit_Select_Par_Special shu0_debitsel_par2_special 1                        
_SHU0_DeBit_Select_Par_Special shu0_debitsel_par4_special 2                        
_SHU0_DeBit_Select_Par_Special shu0_debitsel_par6_special 3                        
_SHU0_DeBit_Select_Par_Special shu0_debitsel_par8_special 4   

    .macro _SHU1_DeBit_Select_Par_Special name number               
	.hmacro \name                   
           .rept \number                    
		SHU1.T1 IND T0->BIU2;           
	        Repeat @ (3);                                      
	   .endr                              
	.endhmacro                           
    .endmacro                            
                                         
_SHU1_DeBit_Select_Par_Special shu1_debitsel_par2_special 1                        
_SHU1_DeBit_Select_Par_Special shu1_debitsel_par4_special 2                        
_SHU1_DeBit_Select_Par_Special shu1_debitsel_par6_special 3                        
_SHU1_DeBit_Select_Par_Special shu1_debitsel_par8_special 4  

    .macro _IALU_DeBit_Select_Par_Special name number               
	.hmacro \name                   
           .rept \number                          
		MERGE(T3,T2,T0)(B)->SHU1.T1; 
	        MERGE(T2,T3,T0)(B)->M[0]; 
                Repeat @ (2);                                    
	   .endr                              
	.endhmacro                           
    .endmacro                            
                                         
_IALU_DeBit_Select_Par_Special ialu_debitsel_par2_special 1                        
_IALU_DeBit_Select_Par_Special ialu_debitsel_par4_special 2                        
_IALU_DeBit_Select_Par_Special ialu_debitsel_par6_special 3                        
_IALU_DeBit_Select_Par_Special ialu_debitsel_par8_special 4  

    .macro _R3_DeBit_Select_Par_Special name number               
	.hmacro \name                   
           .rept \number                          
	        R3.M[0]->BIU2;
                Repeat @ (3);                                    
	   .endr                              
	.endhmacro                           
    .endmacro                            
                                         
_R3_DeBit_Select_Par_Special r3_debitsel_par2_special 1                        
_R3_DeBit_Select_Par_Special r3_debitsel_par4_special 2                        
_R3_DeBit_Select_Par_Special r3_debitsel_par6_special 3                        
_R3_DeBit_Select_Par_Special r3_debitsel_par8_special 4  

  .hmacro main_DeBit_Select_par2_special
    biu0_debitsel_par2_special || shu0_debitsel_par2_special; 
    Repeat @ (15)                                           ; 
    ialu_debitsel_par2_special                              ;
    Repeat @ (3)                                            ; 
    shu1_debitsel_par2_special                              ;
    r3_debitsel_par2_special                                ;
    NOP                                                     ;
    biu2_debitsel_par2_special                              ;
  .endhmacro


   special_par2:
    .opt
    main_DeBit_Select_par2_special;
    .flushhm
    .stopopt
    Repeat @ (14);
    NOP;
    MPUSTOP;

  .hmacro main_DeBit_Select_par4_special
    biu0_debitsel_par4_special || shu0_debitsel_par4_special; 
    Repeat @ (15)                                           ; 
    ialu_debitsel_par4_special                              ;
    Repeat @ (4)                                            ; 
    shu1_debitsel_par4_special                              ;
    r3_debitsel_par4_special                                ;
    NOP                                                     ;
    biu2_debitsel_par4_special                              ;
  .endhmacro


   special_par4:
    .opt
    main_DeBit_Select_par4_special;
    .flushhm
    .stopopt
    Repeat @ (14);
    NOP;
    MPUSTOP;

  .hmacro main_DeBit_Select_par6_special
    biu0_debitsel_par6_special || shu0_debitsel_par6_special; 
    Repeat @ (15)                                           ; 
    ialu_debitsel_par6_special                              ;
    Repeat @ (4)                                            ; 
    shu1_debitsel_par6_special                              ;
    r3_debitsel_par6_special                                ;
    NOP                                                     ;
    biu2_debitsel_par6_special                              ;
  .endhmacro


   special_par6:
    .opt
    main_DeBit_Select_par6_special;
    .flushhm
    .stopopt
    Repeat @ (14);
    NOP;
    MPUSTOP;

  .hmacro main_DeBit_Select_par8_special
    biu0_debitsel_par8_special || shu0_debitsel_par8_special; 
    Repeat @ (15)                                           ; 
    ialu_debitsel_par8_special                              ;
    Repeat @ (4)                                            ; 
    shu1_debitsel_par8_special                              ;
    r3_debitsel_par8_special                                ;
    NOP                                                     ;
    biu2_debitsel_par8_special                              ;
  .endhmacro


   special_par8:
    .opt
    main_DeBit_Select_par8_special;
    .flushhm
    .stopopt
    Repeat @ (14);
    NOP;
    MPUSTOP;

//de bit select: par end_num>64 
    .hmacro BIU0_par_end2_HM
        BIU0.DM(A++,K++)->SHU0.T0;
        NOP;
        BIU0.DM(A++,K++,M)->SHU0.T0;            
    .endhmacro
    
    .hmacro BIU2_par_end2_HM
        BIU2->DM(A++,K++,M);  
        NOP;
        BIU2->DM(A++,K++,M);          
    .endhmacro
 
    .hmacro SHU0_par_end2_HM
        SHU0.T3 IND TSQ->IALU.T0(I0);
        Repeat @ (10);  
        SHU0.T0 IND T1->IALU.T2(I0); 
	NOP; 
        SHU0.T0 IND T2->IALU.T3(I0);         
    .endhmacro
      
    .hmacro SHU1_par_end2_HM
        SHU1.T1 IND T0->BIU2;      
    .endhmacro

    .hmacro IALU_par_end2_HM
        MERGE(T3,T2,T0)(B)->SHU1.T1;  
        MERGE(T2,T3,T0)(B)->BIU2;    
    .endhmacro

  .hmacro main_DeBit_Select_par_end2
    BIU0_par_end2_HM || SHU0_par_end2_HM  ; 
    Repeat @ (15)                         ; 
    IALU_par_end2_HM                      ;
    Repeat @ (3)                          ; 
    SHU1_par_end2_HM                      ;
    BIU2_par_end2_HM                      ;
  .endhmacro


   par_end2:
    .opt
    main_DeBit_Select_par_end2;
    .flushhm
    .stopopt
    Repeat @ (14);
    NOP;
    MPUSTOP;