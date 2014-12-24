//RUN: llvm-mc -arch=mmpulite -filetype=obj -o %t.o %s
//RUN: llvm-objdump -arch=mmpulite -d -no-show-raw-insn %t.o | sed '1,4d' | awk '$1=""; {print $0}' | FileCheck %s

.macro ld no, len, mask
.hmacro ld\no
 BIU\no .DM(A++,K++)->FALU.T\no (I\no) ||REPEAT @(\len);
.if \mask
 BIU\no .DM(M)->FALU.T\no (I\no) ;
.endif
.endhmacro
.endmacro

ld 0, "KI0-1", 1
ld 1, "KI0-1", 1

.hmacro fadd
T0+T1(S)->BIU2||REPEAT @(KI0);
.endhmacro

.hmacro st
BIU2->DM(A++,K++)||REPEAT @(KI0-1);
BIU2->DM(M);
.endhmacro

.hmacro vfadd
ld0||ld1||REPEAT @(8);
fadd||REPEAT @(5);
st;
.endhmacro
.opt
vfadd;
.flushhm

//CHECK: BIU0.DM(A++, K++) -> FALU.T0(I0) || BIU1.DM(A++, K++) -> FALU.T1(I1) || REPEAT @(8);
//CHECK_NEXT: T0 + T1(S) -> BIU2 || BIU0.DM(A++, K++) -> FALU.T0(I0) || BIU1.DM(A++, K++) -> FALU.T1(I1) || REPEAT @(5);
//CHECK_NEXT: T0 + T1(S) -> BIU2 || BIU0.DM(A++, K++) -> FALU.T0(I0) || BIU1.DM(A++, K++) -> FALU.T1(I1) || BIU2-> DM(A++, K++) || REPEAT @(KI0 - 14);
//CHECK_NEXT: T0 + T1(S) -> BIU2 || BIU0.DM(M) -> FALU.T0(I0) || BIU1.DM(M) -> FALU.T1(I1) || BIU2-> DM(A++, K++) ;
//CHECK_NEXT: T0 + T1(S) -> BIU2 || BIU2-> DM(A++, K++) || REPEAT @(8);
//CHECK_NEXT: BIU2-> DM(A++, K++) || REPEAT @(4);
//CHECK_NEXT: BIU2-> DM(M) ;
