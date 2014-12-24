//RUN: llvm-mc -arch=mmpulite -filetype=obj -o %t.o %s
//RUN: llvm-objdump -arch=mmpulite -d -no-show-raw-insn %t.o | sed '1,4d' | awk '$1=""; {print $0}' | FileCheck %s

.hmacro test0
T1+T2->FALU.T0;
T1-T2->FALU.T0;
T1>T2->M[0];
.endhmacro

.hmacro test1
T1+T2(S)->IALU.T0;
T1-T2(S)->IALU.T0;
T1>T2(S)->M[1];
.endhmacro

.hmacro test2
 T1 * T2(S)->SHU0.T0 ;
MR<-T2+T0*T1 (S)@(C);
.endhmacro

.hmacro main
test0||REPEAT @(2);
test1||REPEAT @(3);
test0||test2;
.endhmacro

.opt
main;
.flushhm

//CHECK: T1 + T2 -> FALU.T0 ;
//CHECK_NEXT: T1 - T2 -> FALU.T0 ;
//CHECK_NEXT: T1 > T2 -> M[0] || T1 + T2(S) -> IALU.T0 ;
//CHECK_NEXT: T1 - T2(S) -> IALU.T0 ;
//CHECK_NEXT: T1 > T2(S) -> M[1] ;
//CHECK_NEXT: T1 + T2 -> FALU.T0 || T1 * T2(S) -> SHU0.T0 ;
//CHECK_NEXT: T1 - T2 -> FALU.T0 || MR <- T2 + T0 * T1(S) @(C);
//CHECK_NEXT: T1 > T2 -> M[0] ;
