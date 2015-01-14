//RUN: llvm-mc -arch=mmpulite -filetype=obj -o %t.o %s
//RUN: llvm-objdump -arch-name=mmpulite -d -no-show-raw-insn %t.o | sed '1,5d' | awk '$1=""; {print $0}' > %t.s
//RUN: diff -w -i -I '^[ \t]*$' -I '[ \t]*#.*' -I '\/\/.*' %t.s %s

////////////////////////////////////////////////////
//Constant width muliply
//Test for Dest
 T1 * T2(T)->SHU0.T0 ;
 T1 * T2(T)->SHU1.T0 ;
 T1 * T2(T)->IALU.T0 ;
 T1 * T2(T)->IMAC.T0 ;
 T1 * T2(T)->FALU.T0 ;
 T1 * T2(T)->BIU0    ;
 T1 * T2(T)->BIU1    ;
 T1 * T2(T)->BIU2    ;
 T1 * T2(T)->M[127] ;

 T1 * T2(T)->BIU2    @(C);
 T1 * T2(T)->M[127] @(!C);
//Test for Option
 T1 * T2(T)->SHU0.T0 ;
 T1 * T2(U,T)->SHU0.T0 ;
 T1 * T2(U,T,I)->SHU0.T0 ;
 T1 * T2(T,I)->SHU0.T0 ;
 T1 * T2(B,T)->SHU0.T0 ;
 T1 * T2(B,U,T)->SHU0.T0 ;
 T1 * T2(B,U,T,I)->SHU0.T0 ;
 T1 * T2(B,T,I)->SHU0.T0 ;
 T1 * T2(H,T)->SHU0.T0 ;
 T1 * T2(H,U,T)->SHU0.T0 ;
 T1 * T2(H,U,T,I)->SHU0.T0 ;
 T1 * T2(H,T,I)->SHU0.T0 ;

 T1 * T2(H,U,T,I)->SHU0.T0 @(C);
 T1 * T2(H,T,I)->SHU0.T0 @(!C);
/////////////////////////////////////////////////
//Test for double width muliply 
//Test for Dest
 T1 * T2(B)->SHU0.T0 ;
 T1 * T2(H)->SHU1.T0 ;
 T1 * T2(B)->IALU.T0 ;
 T1 * T2(H)->IMAC.T0 ;
 T1 * T2(B)->FALU.T0 ;
 T1 * T2(H)->BIU0    ;
 T1 * T2(B)->BIU1    ;
 T1 * T2(H)->BIU2    ;
 T1 * T2(B)->M[126] ;

 T1 * T2(H)->BIU2    @(C);
 T1 * T2(B)->M[126] @(!C);
//Test for Option
 T1 * T2(H)->SHU0.T0 ;
 T1 * T2(B,U)->SHU0.T0 ;
 T1 * T2(H,U,I)->SHU0.T0 ;
 T1 * T2(B,I)->SHU0.T0 ;
 T1 * T2(B)->SHU0.T0 ;
 T1 * T2(B,U)->SHU0.T0 ;
 T1 * T2(B,U,I)->SHU0.T0 ;
 T1 * T2(B,I)->SHU0.T0 ;
 T1 * T2(H)->SHU0.T0 ;
 T1 * T2(H,U)->SHU0.T0 ;
 T1 * T2(H,U,I)->SHU0.T0 ;
 T1 * T2(H,I)->SHU0.T0 ;

 T1 * T2(H,U,I)->SHU0.T0 @(C);
 T1 * T2(H,I)->SHU0.T0 @(!C);
////////////////////////////////////////////////////
//Complex Mulplity 
 T3 +- T1 * T2(H)->SHU0.T0 ;
 T3 +- T1 * T2(H)->SHU1.T0 ;
 T3 +- T1 * T2(H)->IALU.T0 ;
 T3 +- T1 * T2(H)->IMAC.T0 ;
 T3 +- T1 * T2(H)->FALU.T0 ;
 T3 +- T1 * T2(H)->BIU0    ;
 T3 +- T1 * T2(H)->BIU1    ;
 T3 +- T1 * T2(H)->BIU2    ;
 T3 +- T1 * T2(H)->M[127] ;

 T3 +- T1 * T2(H)->BIU2    @(C);
 T3 +- T1 * T2(H)->M[127] @(!C);
///////////////////////////////////////////////////
// MA & MAC 
MR<-T2+T0*T1;
MR<-T2+T0*T1(U);
MR<-T2+T0*T1(B);
MR<-T2+T0*T1(B,U);
MR<-T2+T0*T1(H);
MR<-T2+T0*T1(H,U);
MR<-T2+T0*T1  @(C);
MR<-T2+T0*T1 (U) @(C);
MR<-T2+T0*T1 (B) @(C);
MR<-T2+T0*T1 (B,U) @(C);

MR<-T2+T0*T1 (B)@(C);
MR<-T2+T0*T1 (B,U) @(!C);
///////////////////////////////////////////////////
// read MR
 MR+= T0* T2->SHU0.T0 ;
 MR+= T0* T2->SHU1.T0 ;
 MR+= T0* T2(U)->IALU.T0 ;
 MR+= T0* T2(U)->IMAC.T0 ;
 MR+= T0* T2(B)->FALU.T0 ;
 MR+= T0* T2(B)->BIU0    ;
 MR+= T0* T2(B,U)->BIU1    ;
 MR+= T0* T2(H,U)->BIU2    ;
 MR+= T0* T2->M[127] ;

 MR+= T0* T2(H,U)->BIU2    @(C);
 MR+= T0* T2->M[127] @(!C);
/////////////////////////////////////

 MR+= T0* T2(L)->SHU0.T0 ;
 MR+= T0* T2(L)->SHU1.T0 ;
 MR+= T0* T2(U,L)->IALU.T0 ;
 MR+= T0* T2(U,L)->IMAC.T0 ;
 MR+= T0* T2(B,L)->FALU.T0 ;
 MR+= T0* T2(B,L)->BIU0     ;
 MR+= T0* T2(B,U,L)->BIU1     ;
 MR+= T0* T2(H,U,L)->BIU2     ;
 MR+= T0* T2(L)->M[126] ;

 MR+= T0* T2(H,U,L)->BIU2     @(C);
 MR+= T0* T2(L)->M[126] @(!C);

 MR+= T0* T2(L,CR)->SHU0.T0 ;
 MR+= T0* T2(L,CR)->SHU1.T0 ;
 MR+= T0* T2(U,L,CR)->IALU.T0 ;
 MR+= T0* T2(U,L,CR)->IMAC.T0 ;
 MR+= T0* T2(B,L,CR)->FALU.T0 ;
 MR+= T0* T2(B,L,CR)->BIU0     ;
 MR+= T0* T2(B,U,L,CR)->BIU1     ;
 MR+= T0* T2(H,U,L,CR)->BIU2     ;
 MR+= T0* T2(L,CR)->M[126] ;

 MR+= T0* T2(H,U,L,CR)->BIU2     @(C);
 MR+= T0* T2(L,CR)->M[126] @(!C);

///////////////////////////////////////////////////
//TODO:is all the combination is valid ?
//Accumulate instruction
MR+= T0;
MR+= T0(CR);
MR+= T0(U);

MR+= T0(B);
MR+= T0(B,CR);
MR+= T0(B,U);

MR+= T0(H);
MR+= T0(H,CR);
MR+= T0(H,U);

MR+= T0(H,CR)@(C);
MR+= T0(H,U)@(!C);
