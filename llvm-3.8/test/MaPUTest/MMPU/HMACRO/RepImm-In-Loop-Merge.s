//RUN: llvm-mc -arch=mmpulite -filetype=obj -o %t.o %s
//RUN: llvm-objdump -arch-name=mmpulite -d -no-show-raw-insn %t.o | sed '1,4d' | awk '$1=""; {print $0}' | FileCheck %s

.hmacro test
T0-T1(S)->BIU2||LPTO (1f ) @(KI0);
T2+T3(S)->BIU2||REPEAT @(3);
T0+T1(S)->BIU2;
1:
.endhmacro

.hmacro test1
test;
T0-T1->BIU1||LPTO (1f ) @(KI0);
T2+T3->BIU1||REPEAT @(2);
T0+T1->BIU1||REPEAT @(2);
1:
.endhmacro
.opt
test1;
.flushhm

//CHECK: T0 - T1(S) -> BIU2 ;
//CHECK_NEXT: T0 - T1 -> BIU1 || T2 + T3(S) -> BIU2 || LPTO (.text+205) @(KI0 - 1);
//CHECK_NEXT: T2 + T3 -> BIU1 || T2 + T3(S) -> BIU2 || REPEAT @(2);
//CHECK_NEXT: T0 + T1 -> BIU1 || T0 + T1(S) -> BIU2 ;
//CHECK_NEXT: T0 + T1 -> BIU1 || T2 + T3(S) -> BIU2 ;
//CHECK_NEXT: HM1:
//CHECK_NEXT: T2 + T3 -> BIU1 || T2 + T3(S) -> BIU2 || REPEAT @(2);
//CHECK_NEXT: T0 + T1 -> BIU1 || T0 + T1(S) -> BIU2 ;
//CHECK_NEXT: T0 + T1 -> BIU1 ;
