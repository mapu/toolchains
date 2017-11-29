//RUN: llvm-mc -arch=mmpulite -filetype=obj -o %t.o %s
//RUN: llvm-objdump -arch-name=mmpulite -d -no-show-raw-insn %t.o | sed '1,4d' | awk '$1=""; {print $0}' | FileCheck %s

.hmacro test
T0-T1(S)->BIU2||LPTO (2f ) @(KI2);
T2+T3(S)->BIU2||LPTO (1f ) @(KI1);
T0+T1(S)->BIU2;
T0*T1(S)->BIU2;
1:
2:
.endhmacro

.hmacro test1
test||REPEAT @(3);
.endhmacro
.opt
test1;
.flushhm

//CHECK: T0 - T1(S) -> BIU2 ;
//CHECK_NEXT: T2 + T3(S) -> BIU2 ;
//CHECK_NEXT: T0 + T1(S) -> BIU2 || LPTO ("HM1") @(KI2 - 1);
//CHECK_NEXT: T0 * T1(S) -> BIU2 || LPTO ("HM2") @(KI1 - 1);
//CHECK_NEXT: T0 + T1(S) -> BIU2 ;
//CHECK_NEXT: T0 * T1(S) -> BIU2 ;
//CHECK_NEXT: HM2:
//CHECK_NEXT: T2 + T3(S) -> BIU2 ;
//CHECK_NEXT: T0 + T1(S) -> BIU2 || LPTO ("HM3") @(KI1 - 1);
//CHECK_NEXT: HM1:
//CHECK_NEXT: T0 * T1(S) -> BIU2 ;
//CHECK_NEXT: T0 + T1(S) -> BIU2 ;
//CHECK_NEXT: HM3:
//CHECK_NEXT: T0 * T1(S) -> BIU2 ;
