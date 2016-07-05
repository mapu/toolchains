//RUN: llvm-mc -arch=mmpulite -filetype=obj -o %t.o %s
//RUN: llvm-objdump -arch-name=mmpulite -d -no-show-raw-insn %t.o | sed '1,4d' | awk '$1=""; {print $0}' | FileCheck %s

.hmacro test
T0-T1(S)->BIU2||LPTO (1f ) @(KI0);
T2+T3(S)->BIU2;
T0+T1(S)->BIU2;
1:
.endhmacro

.hmacro test1
test||BIU2->DM(A++,K++)||REPEAT @(7);
BIU1->DM(A++,K++);
.endhmacro

.opt
test1;
.flushhm

//CHECK: T0 - T1(S) -> BIU2 || BIU2-> DM(A++, K++) ;
//CHECK_NEXT: T2 + T3(S) -> BIU2 || BIU2-> DM(A++, K++) ;
//CHECK_NEXT: T0 + T1(S) -> BIU2 || BIU2-> DM(A++, K++) ;
//CHECK_NEXT: T2 + T3(S) -> BIU2 || BIU2-> DM(A++, K++) ;
//CHECK_NEXT: T0 + T1(S) -> BIU2 || BIU2-> DM(A++, K++) ;
//CHECK_NEXT: T2 + T3(S) -> BIU2 || BIU2-> DM(A++, K++) ;
//CHECK_NEXT: T0 + T1(S) -> BIU2 || BIU2-> DM(A++, K++) ;
//CHECK_NEXT: T2 + T3(S) -> BIU2 || BIU1-> DM(A++, K++) || LPTO ("HM1") @(KI0 - 4);
//CHECK_NEXT: T0 + T1(S) -> BIU2 ;
//CHECK_NEXT: T2 + T3(S) -> BIU2 ;
//CHECK_NEXT: HM1:
//CHECK_NEXT: T0 + T1(S) -> BIU2 ;
