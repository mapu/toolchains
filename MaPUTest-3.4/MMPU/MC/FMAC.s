//RUN: llvm-mc -arch=mmpulite -filetype=obj -o %t.o %s
//RUN: llvm-objdump -arch=mmpulite -d -no-show-raw-insn %t.o | sed '1,5d' | awk '$1=""; {print $0}' > %t.s
//RUN: diff -w -i -I '^[ \t]*$' -I '[ \t]*#.*' -I '\/\/.*' %t.s %s

////////////////////////////////////////////////////
//Constant width muliply
//Test for Dest
 T1 * T2(S)->SHU0.T0 ;
 T1 * T2(D)->SHU1.T0 ;
 T1 * T2(S)->FALU.T0 ;
 T1 * T2(D)->FMAC.T0 ;
 T1 * T2(T,S)->FALU.T0 ;
 T1 * T2(T,D)->BIU0    ;
 T1 * T2(T,S)->BIU1    ;
 T1 * T2(T,D)->BIU2    ;
 T1 * T2(T,S)->M[127] ;

 T1 * T2(S)->SHU0.T0 @(C);
 T1 * T2(D)->SHU1.T0 @(!C);
 T1 * T2(S)->FALU.T0 @(C);
 T1 * T2(D)->FMAC.T0 @(!C);
 T1 * T2(T,S)->FALU.T0 @(C);
 T1 * T2(T,D)->BIU0    @(!C);
 T1 * T2(T,S)->BIU1    @(C);
 T1 * T2(T,D)->BIU2    @(!C);
 T1 * T2(T,S)->M[127] @(C);

 T1 ** T2(S)->SHU0.T0 ;
 T1 ** T2(S)->SHU1.T0 ;
 T1 ** T2(S)->FALU.T0 ;
 T1 ** T2(S)->FMAC.T0 ;
 T1 ** T2(S)->FALU.T0 ;
 T1 ** T2(S)->BIU0    ;
 T1 ** T2(S)->BIU1    ;
 T1 ** T2(S)->BIU2    ;
 T1 ** T2(S)->M[127] ;

 T1 ** T2(S)->SHU0.T0 @(C);
 T1 ** T2(S)->SHU1.T0 @(!C);
 T1 ** T2(S)->FALU.T0 @(C);
 T1 ** T2(S)->FMAC.T0 @(!C);
 T1 ** T2(S)->FALU.T0 @(C);
 T1 ** T2(S)->BIU0    @(!C);
 T1 ** T2(S)->BIU1    @(C);
 T1 ** T2(S)->BIU2    @(!C);
 T1 ** T2(S)->M[127] @(C);

MR<-T2+T0*T1 (S);
MR<-T2+T0*T1 (D);
MR<-T2+T0*T1 (S);
MR<-T2+T0*T1 (D);
MR+= T0*T1 (S,CR);
MR+= T0*T2 (D,CR);

MR<-T2+T0*T1 (S)@(C);
MR<-T2+T0*T1 (D);
MR<-T2+T0*T1 (S)@(C);
MR<-T2+T0*T1 (D);
MR+= T0*T1 (S,CR)@(C);
MR+= T0*T2 (D,CR);

 MR+= T1*T2 (S) ->SHU0.T0 ;
 MR+= T1*T2 (D) ->SHU1.T0 ;
 MR+= T1*T2 (S) ->FALU.T0 ;
 MR+= T1*T2 (D) ->FMAC.T0 ;
 MR+= T1*T2 (S) ->FALU.T0 ;
 MR+= T1*T2 (D) ->BIU0    ;
 MR+= T1*T2 (S) ->BIU1    ;
 MR+= T1*T2 (D) ->BIU2    ;
 MR+= T1*T2 (S) ->M[127] ;

 MR+= T1*T2 (S) ->SHU0.T0 @(C);
 MR+= T1*T2 (D) ->SHU1.T0 @(!C);
 MR+= T1*T2 (S) ->FALU.T0 @(C);
 MR+= T1*T2 (D) ->FMAC.T0 @(!C);
 MR+= T1*T2 (S) ->FALU.T0 @(C);
 MR+= T1*T2 (D) ->BIU0    @(!C);
 MR+= T1*T2 (S) ->BIU1    @(C);
 MR+= T1*T2 (D) ->BIU2    @(!C);
 MR+= T1*T2 (S) ->M[127] @(C);

 MR+= T0*T1 (S,CR)->SHU0.T0 ;
 MR+= T0*T1 (D,CR)->SHU1.T0 ;
 MR+= T0*T1 (S,CR)->FALU.T0 ;
 MR+= T0*T1 (D,CR)->BIU0    ;

 MR+= T0*T1 (S,CR)->SHU0.T0 @(C);
 MR+= T0*T1 (D,CR)->SHU1.T0 @(!C);
 MR+= T0*T1 (S,CR)->FALU.T0 @(C);
 MR+= T0*T1 (D,CR)->BIU0    @(!C);

 MR+= T0*T1 (S,CR)->SHU0.T0 @(C);
 MR+= T0*T1 (D,CR)->SHU1.T0 @(!C);
 MR+= T0*T1 (S,CR)->FALU.T0 @(C);
 MR+= T0*T1 (D,CR)->BIU0    @(!C);

