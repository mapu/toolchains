//RUN: llvm-mc -arch=mmpulite -filetype=obj -o %t.o %s
//RUN: llvm-objdump -arch=mmpulite -d -no-show-raw-insn %t.o | sed '1,5d' | awk '$1=""; {print $0}' > %t.s
//RUN: diff -w -i -I '^[ \t]*$' -I '[ \t]*#.*' -I '\/\/.*' %t.s %s

///////////////////////////////////////
//BIU1 = SHU0.COPY T1;
//M[127]  =  SHU0.COPY T1;
//IALU.T1 (I0) =  SHU0.COPY T1;
//FALU.T1 (I1) =  SHU0.COPY T1;
//IMAC.T1 (I2) =  SHU0.COPY T1;
//FMAC.T1 (I0) =  SHU0.COPY T1;
//SHU0.T1  =  COPY T1;
//SHU1.T1  =  COPY T1;

////////////////////////////////////////
// SHU & BIU & M transfer
//SHU 2 BIU, =+ option
 SHU0.T0 IND T1->BIU0 ;
 SHU0.T0 IND T1->BIU1 ;
 SHU0.T0 IND T1->BIU2 ;
 SHU0.T0 IND T1 (TB=+ 4)->BIU0 ;
 SHU0.T0 IND T1 (TB=+ 4)->BIU1 ;
 SHU0.T0 IND T1 (TB=+ 4)->BIU2 ;
 SHU0.T0 IND T1->M[127]  ;
 SHU0.T0 IND T1 (TB=+ 4)->M[127]  ;
 SHU0.TSQ IND TBB->M[127]  ;
 SHU0.TBW IND T1 (TB=+ 4)->M[127]  ;

//SHU 2 BIU += option
 SHU0.T0 IND TB (TB+= 4)->BIU0 ;
 SHU0.T0 IND TB (TB+= 4)->BIU1 ;
 SHU0.T0 IND TB (TB+= 4)->BIU2 ;
 SHU0.T0 IND TB->SHU0.T0 ;
 SHU0.T0 IND TB(TB+= 4)->SHU0.T0 ;
 SHU0.TBB IND TB->SHU0.T0 ;
 SHU0.TSQ IND TB(TB+= 4)->SHU0.T0 ;

////////////////////////////////////////
//SHU & CU transfer
//SHU 2 BIU, =+ option
 SHU1.T0 IND T1->IALU.T1 (I0) ;
 SHU1.T0 IND T1->FALU.T1 (I1) ;
 SHU1.T0 IND T1->IMAC.T1 (I2) ;
 SHU1.T0 IND T1->FMAC.T1 (I0) ;
 SHU1.T0 IND T1 (TB=+ 4)->IALU.T1 (I0) ;
 SHU1.T0 IND T1 (TB=+ 4)->FALU.T1 (I1) ;
 SHU1.T0 IND T1 (TB=+ 4)->IMAC.T1 (I2) ;
 SHU1.T0 IND T1 (TB=+ 4)->FMAC.T1 (I0) ;

//SHU 2 BIU += option
 SHU1.T0 IND TB (TB+= 4)->IALU.T1 (I0) ;
 SHU1.T0 IND TB (TB+= 4)->FALU.T1 (I1) ;
 SHU1.T0 IND TB (TB+= 4)->IMAC.T1 (I2) ;
 SHU1.T0 IND TB (TB+= 4)->FMAC.T1 (I0) ;

////////////////////////////////////////
//the cascade transfer
 SHU0.COMB(T0,T1)->BIU0     ;
 SHU0.COMB(T0,T1)->BIU1     ;
 SHU0.COMB(T0,T1)->BIU2     ;
 SHU0.COMB(T0,T1)->M[127]  ;
 SHU0.COMB(T0,T1)->SHU0.T1 ; 
 SHU1.COMB(T0,T1)->SHU1.T1 ;

 SHU0.COMB(T0,T1)(TC)->BIU0     ;
 SHU0.COMB(T0,T1)(TC)->BIU1     ;
 SHU0.COMB(T0,T1)(TC)->BIU2     ;
 SHU0.COMB(T0,T1)(TC)->M[127]  ;
 SHU0.COMB(T0,T1)(TC)->SHU0.T1 ; 
 SHU1.COMB(T0,T1)(TC)->SHU1.T1 ;
 SHU0.COMB(TSQ,TBB)(TC)->SHU0.T1 ; 
 SHU1.COMB(TBD,TBW)(TC)->SHU1.T1 ;  

 SHU1.T0 IND TB (TB+= 4)->IALU.T1 (I0) ;
 SHU1.T0 IND TB (TB+= 4)->FALU.T1 (I1) ;
 SHU1.T2 IND TB (TB+= 4)->IMAC.T0 (I2) ;
 SHU1.T0 IND TB (TB+= 4)->FMAC.T1 (I0) ;

