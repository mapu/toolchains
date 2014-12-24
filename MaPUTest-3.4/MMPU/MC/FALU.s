//RUN: llvm-mc -arch=mmpulite -filetype=obj -o %t.o %s
//RUN: llvm-objdump -arch=mmpulite -d -no-show-raw-insn %t.o | sed '1,5d' | awk '$1=""; {print $0}' > %t.s
//RUN: diff -w -i -I '^[ \t]*$' -I '[ \t]*#.*' -I '\/\/.*' %t.s %s

////////////////////////////////////////////////////
//add
 T0 + T0 (S)->SHU0.T0 ;
 T1 + T2 (S)->SHU0.T0 ;
 T1 + T2 (S)->SHU1.T0 ;
 T1 + T2 (S)->IALU.T0 ;
 T1 + T2 (S)->FALU.T0 ;
 T1 + T2 (S)->IMAC.T0 ;
 T1 + T2(T,S)->FMAC.T0 ;
 T1 + T2(T,S)->BIU0    ;
 T1 + T2(T,S)->BIU1    ;
 T1 + T2(T,S)->BIU2    ;
 T1 + T2(T,S)->M[127] ;
 T1 + T2 (D)->SHU0.T0 ;
 T1 + T2 (D)->SHU1.T0 ;
 T1 + T2 (D)->IALU.T0 ;
 T1 + T2 (D)->FALU.T0 ;
 T1 + T2 (D)->IMAC.T0 ;
 T1 + T2(T,D)->FMAC.T0 ;
 T1 + T2(T,D)->BIU0    ;
 T1 + T2(T,D)->BIU1    ;
 T1 + T2(T,D)->BIU2    ;
 T1 + T2(T,D)->M[127] ;

 T1 + T2(T,D)->BIU2    @(C);
 T1 + T2(T,D)->M[127] @(!C);
////////////////////////////////////////////////////
//sub
 T1 - T2 (S)->SHU0.T0 ;
 T1 - T2 (S)->SHU1.T0 ;
 T1 - T2 (S)->IALU.T0 ;
 T1 - T2 (S)->FALU.T0 ;
 T1 - T2 (S)->IMAC.T0 ;
 T1 - T2(T,S)->FMAC.T0 ;
 T1 - T2(T,S)->BIU0    ;
 T1 - T2(T,S)->BIU1    ;
 T1 - T2(T,S)->BIU2    ;
 T1 - T2(T,S)->M[127] ;
 T1 - T2 (D)->SHU0.T0 ;
 T1 - T2 (D)->SHU1.T0 ;
 T1 - T2 (D)->IALU.T0 ;
 T1 - T2 (D)->FALU.T0 ;
 T1 - T2 (D)->IMAC.T0 ;
 T1 - T2(T,D)->FMAC.T0 ;
 T1 - T2(T,D)->BIU0    ;
 T1 - T2(T,D)->BIU1    ;
 T1 - T2(T,D)->BIU2    ;
 T1 - T2(T,D)->M[127] ;

 T1 - T2(T,D)->BIU2    @(C);
 T1 - T2(T,D)->M[127] @(!C);
////////////////////////////////////////////////////
//Max 
 MAX(T0 ,T0)(S)->SHU0.T0 ;
 MAX(T1 ,T2)(S)->SHU0.T0 ;
 MAX(T1 ,T2)(S)->SHU1.T0 ;
 MAX(T1 ,T2)(S)->IALU.T0 ;
 MAX(T1 ,T2)(S)->FALU.T0 ;
 MAX(T1 ,T2)(S)->IMAC.T0 ;
 MAX(T1 ,T2)(D)->FMAC.T0 ;
 MAX(T1 ,T2)(D)->BIU0    ;
 MAX(T1 ,T2)(D)->BIU1    ;
 MAX(T1 ,T2)(D)->BIU2    ;
 MAX(T1 ,T2)(D)->M[127] ;

 MAX(T1 ,T2)(D)->BIU2    @(C);
 MAX(T1 ,T2)(D)->M[127] @(!C);
////////////////////////////////////////////////////
//MIN 
 MIN(T1 ,T2)(S)->SHU0.T0 ;
 MIN(T1 ,T2)(S)->SHU1.T0 ;
 MIN(T1 ,T2)(S)->IALU.T0 ;
 MIN(T1 ,T2)(S)->FALU.T0 ;
 MIN(T1 ,T2)(S)->IMAC.T0 ;
 MIN(T1 ,T2)(D)->FMAC.T0 ;
 MIN(T1 ,T2)(D)->BIU0    ;
 MIN(T1 ,T2)(D)->BIU1    ;
 MIN(T1 ,T2)(D)->BIU2    ;
 MIN(T1 ,T2)(D)->M[127] ;

 MIN(T1 ,T2)(D)->BIU2    @(C);
 MIN(T1 ,T2)(D)->M[127] @(!C);
////////////////////////////////////////////////////
//ABS 
 ABS T0 (S)->SHU0.T0 ;
 ABS T1 (S)->SHU0.T0 ;
 ABS T1 (S)->SHU1.T0 ;
 ABS T1 (S)->IALU.T0 ;
 ABS T1 (S)->FALU.T0 ;
 ABS T1 (S)->IMAC.T0 ;
 ABS T1 (D)->FMAC.T0 ;
 ABS T1 (D)->BIU0    ;
 ABS T1 (D)->BIU1    ;
 ABS T1 (D)->BIU2    ;
 ABS T1 (D)->M[127] ;

 ABS T1 (D)->BIU2    @(C);
 ABS T1 (D)->M[127] @(!C);
////////////////////////////////////////////////////
//add & sub
 T1 +- T2 (S)->SHU0.T0 ;
 T1 +- T2 (S)->SHU1.T0 ;
 T1 +- T2 (S)->IALU.T0 ;
 T1 +- T2 (S)->FALU.T0 ;
 T1 +- T2 (S)->IMAC.T0 ;
 T1 +- T2 (S)->FMAC.T0 ;
 T1 +- T2 (S)->BIU0    ;
 T1 +- T2 (S)->BIU1    ;
 T1 +- T2 (S)->BIU2    ;
 T1 +- T2 (S)->M[127] ;

 T1 +- T2 (S)->BIU2    @(C);
 T1 +- T2 (S)->M[127] @(!C);
////////////////////////////////////////////////////
//RECIP 
 RECIP T1 (S)->SHU0.T0 ;
 RECIP T1 (S)->SHU1.T0 ;
 RECIP T1 (S)->IALU.T0 ;
 RECIP T1 (S)->FALU.T0 ;
 RECIP T1 (S)->IMAC.T0 ;
 RECIP T1 (D)->FMAC.T0 ;
 RECIP T1 (D)->BIU0    ;
 RECIP T1 (D)->BIU1    ;
 RECIP T1 (D)->BIU2    ;
 RECIP T1 (D)->M[127] ;

 RECIP T1 (D)->BIU2    @(C);
 RECIP T1 (D)->M[127] @(!C);
////////////////////////////////////////////////////
//SQRT 
 RSQRT T1 (S)->SHU0.T0 ;
 RSQRT T1 (S)->SHU1.T0 ;
 RSQRT T1 (S)->IALU.T0 ;
 RSQRT T1 (S)->FALU.T0 ;
 RSQRT T1 (S)->IMAC.T0 ;
 RSQRT T1 (D)->FMAC.T0 ;
 RSQRT T1 (D)->BIU0    ;
 RSQRT T1 (D)->BIU1    ;
 RSQRT T1 (D)->BIU2    ;
 RSQRT T1 (D)->M[127] ;

 RSQRT T1 (D)->BIU2    @(C);
 RSQRT T1 (D)->M[127] @(!C);
////////////////////////////////////////////////////
//COMPARE
 T1 == T2 (S)->SHU0.T0 ;
 T1 == T2 (D)->SHU1.T0 ;
 T1 != T2 (S)->IALU.T0 ;
 T1 != T2 (D)->FALU.T0 ;
 T1 >  T2 (S)->IMAC.T0 ;
 T1 >  T2 (D)->FMAC.T0 ;
 T1 >= T2 (S)->BIU0    ;
 T1 >= T2 (D)->BIU1    ;
 T1 <  T2 (S)->BIU2    ;
 T1 <  T2 (D)->BIU2    ;
 T1 <= T2 (S)->M[127] ;
 T1 <= T2 (D)->M[127] ;

 T1 <= T2 (S)->M[127] @(C);
 T1 <= T2 (D)->M[127] @(!C);
////////////////////////////////////////////////////
//transform
 SINGLE T1 ->SHU0.T0 ;
 SINGLE T1 (T)->SHU1.T0 ;
 SINGLE T1 (U)->SHU0.T0 ;
 SINGLE T1 (U,T)->SHU1.T0 ;
 SINGLE T2 (D)->IALU.T0 ;
 SINGLE T0 (T,D)->FALU.T0 ;
 DOUBLE T1->IMAC.T0 ;
 DOUBLE T1(U)->IMAC.T0 ;
 DOUBLE T1(S)->FMAC.T0 ;
 INT T1(S)->BIU0    ;
 INT T1(T,S)->BIU1    ;
 INT T1(D)->BIU2    ;
 INT T1(T,D)->BIU0    ;
 UINT T1(S)->BIU0    ;
 UINT T1(T,S)->BIU1    ;
 UINT T1(D)->BIU2    ;
 UINT T1(T,D)->BIU0    ;

 UINT T1(D)->BIU2    @(C);
 UINT T1(T,D)->BIU0    @(!C);
