//RUN: llvm-mc -arch=mmpulite -filetype=obj -o %t.o %s
//RUN: llvm-objdump -arch-name=mmpulite -d -no-show-raw-insn %t.o | sed '1,4d' | awk '$1=""; {print $0}' | FileCheck %s

.hmacro loop1
 T1 + T2 (S)->SHU0.T0 ||      REPEAT @(KI0-1);
 T1 + T2 (S)->SHU0.T0 ||      LPTO (1f ) @(KI1-1);
T1 + T2 (S)->SHU1.T0||        REPEAT @(KI0);
1:
.endhm

.hmacro loop2
LPTO (1f ) @(KI1);
 T1 + T2->IALU.T0 ||       REPEAT @(KI0);
1:
.endhm

.hmacro main
loop1;
loop2;
.endhm

.opt
main;
.flushhm

//CHECK: T1 + T2(S) -> SHU0.T0 || REPEAT @(2);
//CHECK_NEXT: T1 + T2 -> IALU.T0 || T1 + T2(S) -> SHU0.T0 || REPEAT @(KI0 - 2);
//CHECK_NEXT: T1 + T2 -> IALU.T0 || T1 + T2(S) -> SHU1.T0 || REPEAT @(2);
//CHECK_NEXT: T1 + T2 -> IALU.T0 || T1 + T2(S) -> SHU1.T0 || LPTO (0) @(KI1 - 2);
//CHECK_NEXT: T1 + T2 -> IALU.T0 || T1 + T2(S) -> SHU1.T0 || REPEAT @(KI0 - 0);
//CHECK_NEXT: HM1:
//CHECK_NEXT: T1 + T2 -> IALU.T0 || T1 + T2(S) -> SHU1.T0 || REPEAT @(KI0 - 3);
//CHECK_NEXT: T1 + T2 -> IALU.T0 || REPEAT @(2);

