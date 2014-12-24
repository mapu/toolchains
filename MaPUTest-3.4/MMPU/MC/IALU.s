//RUN: llvm-mc -arch=mmpulite -filetype=obj -o %t.o %s
//RUN: llvm-objdump -arch=mmpulite -d -no-show-raw-insn %t.o | sed '1,5d' | awk '$1=""; {print $0}' > %t.s
//RUN: diff -w -i -I '^[ \t]*$' -I '[ \t]*#.*' -I '\/\/.*' %t.s %s

////////////////////////////////////////////////////
//Fix Point Add
//Test for Dest
 T0 + T0->IALU.T0 ;
 T1 + T2->IALU.T0 ;
 T1 + T2->IMAC.T0 ;
 T1 + T2->FALU.T0 ;
 T1 + T2->SHU0.T0 ;
 T1 + T2->SHU1.T0 ;
 T1 + T2->BIU0    ;
 T1 + T2->BIU1    ;
 T1 + T2->BIU2    ;
 T1 + T2->M[127] ;

//Test for option
 T1 + T2(U)->IALU.T0 ;
 T1 + T2(T)->IALU.T0 ;
 T1 + T2(U,T)->IALU.T0 ;

 T1 + T2(B,U)->IALU.T0 ;
 T1 + T2(B,T)->IALU.T0 ;
 T1 + T2(B,U,T)->IALU.T0 ;

 T1 + T2(H,U)->IALU.T0 ;
 T1 + T2(H,T)->IALU.T0 ;
 T1 + T2(H,U,T)->IALU.T0 ;

 T1 + T2(H,T)->IALU.T0 @(C);
 T1 + T2(H,U,T)->IALU.T0 @(!C);

////////////////////////////////////////////////////
//Fix Point Sub
//Test for Dest
 T1 - T2->IALU.T0 ;
 T1 - T2->IMAC.T0 ;
 T1 - T2->FALU.T0 ;
 T1 - T2->SHU0.T0 ;
 T1 - T2->SHU1.T0 ;
 T1 - T2->BIU0    ;
 T1 - T2->BIU1    ;
 T1 - T2->BIU2    ;
 T1 - T2->M[127] ;

 T1 - T2->BIU2    @(C);
 T1 - T2->M[127] @(!C);

//Test for option
 T1 - T2(U)->IALU.T0 ;
 T1 - T2(T)->IALU.T0 ;
 T1 - T2(U,T)->IALU.T0 ;

 T1 - T2(B,U)->IALU.T0 ;
 T1 - T2(B,T)->IALU.T0 ;
 T1 - T2(B,U,T)->IALU.T0 ;

 T1 - T2(H,U)->IALU.T0 ;
 T1 - T2(H,T)->IALU.T0 ;
 T1 - T2(H,U,T)->IALU.T0 ;

 T1 - T2(H,T)->IALU.T0 @(C);
 T1 - T2(H,U,T)->IALU.T0 @(!C);

////////////////////////////////////////////////////
//Fix Point Max 
//Test for Dest
 MAX( T0 , T0)->IALU.T0 ;
 MAX( T1 , T2)->IALU.T0 ;
 MAX( T1 , T2)->IMAC.T0 ;
 MAX( T1 , T2)->FALU.T0 ;
 MAX( T1 , T2)->SHU0.T0 ;
 MAX( T1 , T2)->SHU1.T0 ;
 MAX( T1 , T2)->BIU0    ;
 MAX( T1 , T2)->BIU1    ;
 MAX( T1 , T2)->BIU2    ;
 MAX( T1 , T2)->M[127] ;

//Test for option
 MAX( T1 ,T2)(U)->IALU.T0 ;
 MAX( T1 ,T2)(B,U)->IALU.T0 ;
 MAX( T1 ,T2)(H,U)->IALU.T0 ;

 MAX( T1 ,T2)(B,U)->IALU.T0 @(C);
 MAX( T1 ,T2)(H,U)->IALU.T0 @(!C);

////////////////////////////////////////////////////
//Fix Point Min
//Test for Dest
 MIN( T1 , T2)->IALU.T0 ;
 MIN( T1 , T2)->IMAC.T0 ;
 MIN( T1 , T2)->FALU.T0 ;
 MIN( T1 , T2)->SHU0.T0 ;
 MIN( T1 , T2)->SHU1.T0 ;
 MIN( T1 , T2)->BIU0    ;
 MIN( T1 , T2)->BIU1    ;
 MIN( T1 , T2)->BIU2    ;
 MIN( T1 , T2)->M[127] ;

//Test for option
 MIN( T1 ,T2)(U)->IALU.T0 ;
 MIN( T1 ,T2)(B,U)->IALU.T0 ;
 MIN( T1 ,T2)(H,U)->IALU.T0 ;

 MIN( T1 ,T2)(B,U)->IALU.T0 @(C);
 MIN( T1 ,T2)(H,U)->IALU.T0 @(!C);

// compress
 CPRS( T1 , T2)->IALU.T0 ;
 CPRS( T1 , T2)->IMAC.T0 ;
 CPRS( T1 , T2)->FALU.T0 ;
 CPRS( T1 , T2)->SHU0.T0 ;
 CPRS( T1 , T2)->SHU1.T0 ;
 CPRS( T1 , T2)->BIU0    ;
 CPRS( T1 , T2)->BIU1    ;
 CPRS( T1 , T2)->BIU2    ;
 CPRS( T1 , T2)->M[127] ;

//Test for option
 CPRS( T1 ,T2)(H)->IALU.T0 ;
 CPRS( T1 ,T2)(U)->IALU.T0 ;
 CPRS( T1 ,T2)(T)->IALU.T0 ;
 CPRS( T1 ,T2)(H,U,T)->IALU.T0 ;
 CPRS( T1 ,T2)(H,U,T)->IALU.T0 @(C);
 CPRS( T1 ,T2)(H,U,T)->IALU.T0 @(!C);

////////////////////////////////////////////////////
//Fix Point ABS 
//Test for Dest
 ABS(T1,T2)->IALU.T0 ;
 ABS(T1,T2)->IMAC.T0 ;
 ABS(T1,T2)->FALU.T0 ;
 ABS(T1,T2)->SHU0.T0 ;
 ABS(T1,T2)->SHU1.T0 ;
 ABS(T1,T2)->BIU0    ;
 ABS(T1,T2)->BIU1    ;
 ABS(T1,T2)->BIU2    ;
 ABS(T1,T2)->M[127] ;

//Test for option
 ABS(T1,T2)(U)->IALU.T0 ;
 ABS(T1,T2)(B,U)->IALU.T0 ;
 ABS(T1,T2)(H,U)->IALU.T0 ;
 ABS(T1,T2)(B)->IALU.T0 ;
 ABS(T1,T2)(H)->IALU.T0 ;

 ABS(T1,T2)(B)->IALU.T0 @(C);
 ABS(T1,T2)(H)->IALU.T0 @(!C);

////////////////////////////////////////////////////
//Fix Point Merge 
//Test for Dest
 MERGE(T0,T0,T0) ->IALU.T0 ;
 MERGE(T2,T3,T1) ->IALU.T0 ;
 MERGE(T2,T3,T1) ->IMAC.T0 ;
 MERGE(T2,T3,T1) ->FALU.T0 ;
 MERGE(T2,T3,T1) ->SHU0.T0 ;
 MERGE(T2,T3,T1) ->SHU1.T0 ;
 MERGE(T2,T3,T1) ->BIU0    ;
 MERGE(T2,T3,T1) ->BIU1    ;
 MERGE(T2,T3,T1) ->BIU2    ;
 MERGE(T2,T3,T1) ->M[127] ;
//Test for Option
 MERGE(T2,T3,T1)(B)->IALU.T0 ;
 MERGE(T2,T3,T1)(H)->IALU.T0 ;

 MERGE(T2,T3,T1)(B)->IALU.T0 @(!C);
 MERGE(T2,T3,T1)(H)->IALU.T0 @(C);

////////////////////////////////////////////////////
//Test for logic 
 T0 & T0->IALU.T0 ;
 T1 & T2->IALU.T0 ;
 T1 & T2->IMAC.T0 ;
 T1 & T2->FALU.T0 ;
 T1 & T2->SHU0.T0 ;
 T1 & T2->SHU1.T0 ;
 T1 & T2->BIU0    ;
 T1 & T2->BIU1    ;
 T1 & T2->BIU2    ;
 T1 & T2->M[127] ;

 T1 & T2->BIU2    @(C);
 T1 & T2->M[127] @(!C);

////////////////////////////////////////////////////
//Test for and 
 T1 |  T2->IALU.T0 ;
 T1 |  T2->IMAC.T0 ;
 T1 |  T2->FALU.T0 ;
 T1 |  T2->SHU0.T0 ;
 T1 |  T2->SHU1.T0 ;
 T1 |  T2->BIU0    ;
 T1 |  T2->BIU1    ;
 T1 |  T2->BIU2    ;
 T1 |  T2->M[127] ;

 T1 |  T2->BIU2    @(C);
 T1 |  T2->M[127] @(!C);

////////////////////////////////////////////////////
//Test for xor 
 T1 ^  T2->IALU.T0 ;
 T1 ^  T2->IMAC.T0 ;
 T1 ^  T2->FALU.T0 ;
 T1 ^  T2->SHU0.T0 ;
 T1 ^  T2->SHU1.T0 ;
 T1 ^  T2->BIU0    ;
 T1 ^  T2->BIU1    ;
 T1 ^  T2->BIU2    ;
 T1 ^  T2->M[127] ;
////////////////////////////////////////////////////
//Test for not 
 NOT T0 ->IALU.T0 ;
 NOT T1 ->IALU.T0 ;
 NOT T1 ->IMAC.T0 ;
 NOT T1 ->FALU.T0 ;
 NOT T1 ->SHU0.T0 ;
 NOT T1 ->SHU1.T0 ;
 NOT T1 ->BIU0    ;
 NOT T1 ->BIU1    ;
 NOT T1 ->BIU2    ;
 NOT T1 ->M[127] ;
////////////////////////////////////////////////////
//Test for expand 
 EXPD T1 (B,U)->IALU.T0 ;
 EXPD T1 (B,U)->IMAC.T0 ;
 EXPD T1 (B,U)->FALU.T1 ;
 EXPD T1 (B,U)->SHU0.T0 ;
 EXPD T1 (B,U)->SHU1.T1 ;
 EXPD T1 (B,U)->BIU0    ;
 EXPD T1 (B,U)->BIU1    ;
 EXPD T1 (B,U)->BIU2    ;
 EXPD T1(H,U)->M[126] ;
//Test for expand  option
 EXPD T1 (B,U)->IALU.T0 ;
 EXPD T1 (B,U)->IMAC.T0 ;
 EXPD T1 (B,U)->FALU.T0 ;
 EXPD T1 (H,U)->SHU0.T1 ;
 EXPD T1 (B,U)->SHU1.T0 ;
 EXPD T1 (B,U)->BIU0    ;
 EXPD T1 (H,U)->BIU1    ;
 EXPD T1 (H,U)->BIU2    ;
 EXPD T1 (B,U)->M[2] ;
 EXPD T1 (B,U)->M[126] @(C);
 EXPD T1 (B,U)->M[126] @(!C);
////////////////////////////////////////////////////
//Test for Lshift with Reg 
 T1 <<  T2->IALU.T0 ;
 T1 <<  T2->IMAC.T0 ;
 T1 <<  T2->FALU.T0 ;
 T1 <<  T2->SHU0.T0 ;
 T1 <<  T2->SHU1.T0 ;
 T1 <<  T2->BIU0    ;
 T1 <<  T2->BIU1    ;
 T1 <<  T2->BIU2    ;
 T1 <<  T2->M[127] ;
////////////////////////////////////////////////////
//Test for Lshift with Imm 
 T0 <<  4->IALU.T0 ;
 T1 <<  4->IALU.T0 ;
 T1 <<  4->IMAC.T0 ;
 T1 <<  4->FALU.T0 ;
 T1 <<  4->SHU0.T0 ;
 T1 <<  4->SHU1.T0 ;
 T1 <<  4->BIU0    ;
 T1 <<  4->BIU1    ;
 T1 <<  4->BIU2    ;
 T1 <<  4->M[127] ;
////////////////////////////////////////////////////
//Test for Rshift with Reg
 T1 >>  T2(U)->IALU.T0 ;
 T1 >>  T2(U)->IMAC.T0 ;
 T1 >>  T2(U)->FALU.T0 ;
 T1 >>  T2(U)->SHU0.T0 ;
 T1 >>  T2(U)->SHU1.T0 ;
 T1 >>  T2->BIU0    ;
 T1 >>  T2->BIU1    ;
 T1 >>  T2->BIU2    ;
 T1 >>  T2->M[127] ;
////////////////////////////////////////////////////
//Test for Rshift with Imm 
 T1 >>  4->IALU.T0 ;
 T1 >>  4->IMAC.T0 ;
 T1 >>  4->FALU.T0 ;
 T1 >>  4->SHU0.T0 ;
 T1 >>  4(U)->SHU1.T0 ;
 T1 >>  4(U)->BIU0    ;
 T1 >>  4(U)->BIU1    ;
 T1 >>  4(U)->BIU2    ;
 T1 >>  4(U)->M[127] ;
////////////////////////////////////////////////////
//Test for compare instruction
 T1 == T2 ->IALU.T0 ;
 T1 == T2 ->IMAC.T0 ;
 T1 == T2 (B)->FALU.T0 ;
 T1 == T2 (B)->SHU0.T0 ;
 T1 == T2 (B)->SHU1.T0 ;
 T1 == T2 (H)->BIU0    ;
 T1 == T2 (H)->BIU1    ;
 T1 == T2 (H)->BIU2    ;
 T1 == T2 (H)->M[127] ;
 T1 == T2 (H)->Cond    ;
////////////////////////////////////////////////////
//Test for compare instruction
 T1 != T2 ->IALU.T0 ;
 T1 != T2 ->IMAC.T0 ;
 T1 != T2 (B)->FALU.T0 ;
 T1 != T2 (B)->SHU0.T0 ;
 T1 != T2 (B)->SHU1.T0 ;
 T1 != T2 (H)->BIU0    ;
 T1 != T2 (H)->BIU1    ;
 T1 != T2 (H)->BIU2    ;
 T1 != T2 (H)->M[127] ;
 T0 != T0 (H)->Cond    ;
 T1 != T2 (H)->Cond    ;
////////////////////////////////////////////////////
//Test for compare instruction
 T1 >  T2 ->IALU.T0 ;
 T1 >  T2 ->IMAC.T0 ;
 T1 >  T2 (B)->FALU.T0 ;
 T1 >  T2 (B)->SHU0.T0 ;
 T1 >  T2 (B)->SHU1.T0 ;
 T1 >  T2 (H)->BIU0    ;
 T1 >  T2 (H)->BIU1    ;
 T1 >  T2 (H)->BIU2    ;
 T1 >  T2 (H)->M[127] ;
 T1 >  T2 (U)->IALU.T0 ;
 T1 >  T2 (U)->IMAC.T0 ;
 T1 >  T2 (B,U)->FALU.T0 ;
 T1 >  T2 (B,U)->SHU0.T0 ;
 T1 >  T2 (B,U)->SHU1.T0 ;
 T1 >  T2 (H,U)->BIU0    ;
 T1 >  T2 (H,U)->BIU1    ;
 T1 >  T2 (H,U)->BIU2    ;
 T1 >  T2 (H,U)->M[127] ;
 T1 >  T2 (H)->Cond    ;
////////////////////////////////////////////////////
//Test for compare instruction
 T1 >=  T2 ->IALU.T0 ;
 T1 >=  T2 ->IMAC.T0 ;
 T1 >=  T2 (B)->FALU.T0 ;
 T1 >=  T2 (B)->SHU0.T0 ;
 T1 >=  T2 (B)->SHU1.T0 ;
 T1 >=  T2 (H)->BIU0    ;
 T1 >=  T2 (H)->BIU1    ;
 T1 >=  T2 (H)->BIU2    ;
 T1 >=  T2 (H)->M[127] ;
 T1 >=  T2 (U)->IALU.T0 ;
 T1 >=  T2 (U)->IMAC.T0 ;
 T1 >=  T2 (B,U)->FALU.T0 ;
 T1 >=  T2 (B,U)->SHU0.T0 ;
 T1 >=  T2 (B,U)->SHU1.T0 ;
 T1 >=  T2 (H,U)->BIU0    ;
 T1 >=  T2 (H,U)->BIU1    ;
 T1 >=  T2 (H,U)->BIU2    ;
 T1 >=  T2 (H,U)->M[127] ;
 T1 >= T2 (H)->Cond    ;
////////////////////////////////////////////////////
//Test for compare instruction
 T1 <  T2 ->IALU.T0 ;
 T1 <  T2 ->IMAC.T0 ;
 T1 <  T2 (B)->FALU.T0 ;
 T1 <  T2 (B)->SHU0.T0 ;
 T1 <  T2 (B)->SHU1.T0 ;
 T1 <  T2 (H)->BIU0    ;
 T1 <  T2 (H)->BIU1    ;
 T1 <  T2 (H)->BIU2    ;
 T1 <  T2 (H)->M[127] ;
 T1 <  T2 (U)->IALU.T0 ;
 T1 <  T2 (U)->IMAC.T0 ;
 T1 <  T2 (B,U)->FALU.T0 ;
 T1 <  T2 (B,U)->SHU0.T0 ;
 T1 <  T2 (B,U)->SHU1.T0 ;
 T1 <  T2 (H,U)->BIU0    ;
 T1 <  T2 (H,U)->BIU1    ;
 T1 <  T2 (H,U)->BIU2    ;
 T1 <  T2 (H,U)->M[127] ;
 T1 <  T2 (H)->Cond    ;
////////////////////////////////////////////////////
//Test for compare instruction
 T1 <=  T2 ->IALU.T0 ;
 T1 <=  T2 ->IMAC.T0 ;
 T1 <=  T2 (B)->FALU.T0 ;
 T1 <=  T2 (B)->SHU0.T0 ;
 T1 <=  T2 (B)->SHU1.T0 ;
 T1 <=  T2 (H)->BIU0    ;
 T1 <=  T2 (H)->BIU1    ;
 T1 <=  T2 (H)->BIU2    ;
 T1 <=  T2 (H)->M[127] ;
 T1 <=  T2 (U)->IALU.T0 ;
 T1 <=  T2 (U)->IMAC.T0 ;
 T1 <=  T2 (B,U)->FALU.T0 ;
 T1 <=  T2 (B,U)->SHU0.T0 ;
 T1 <=  T2 (B,U)->SHU1.T0 ;
 T1 <=  T2 (H,U)->BIU0    ;
 T1 <=  T2 (H,U)->BIU1    ;
 T1 <=  T2 (H,U)->BIU2    ;
 T1 <=  T2 (H,U)->M[127] ;
 T1 <= T2 (H)->Cond    ;
////////////////////////////////////////////////////
//Test for DIV
 T0 DIVS T0 ->DIVR    ;
 T0 DIVS T1 ->DIVR    ;
 T0 DIVS T1 (U)->DIVR    ;
 DIVQ->DIVR    ;
 MDIVR->DIVR    ;
 MDIVQ->DIVR    ;
 DIVR ->IALU.T0 ;
 DIVR ->IMAC.T0 ;
 DIVR ->FALU.T0 ;
 DIVR ->SHU0.T0 ;
 DIVR ->SHU1.T0 ;
 DIVR ->BIU0        ;
 DIVR ->BIU1        ;
 DIVR ->BIU2        ;
 DIVR ->M[126] ;
