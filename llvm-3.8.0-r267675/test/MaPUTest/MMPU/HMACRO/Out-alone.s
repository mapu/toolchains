//RUN: llvm-mc -arch=mmpulite -filetype=obj -o %t.o %s
//RUN: llvm-objdump -arch-name=mmpulite -d -no-show-raw-insn %t.o | sed '1,4d' | awk '$1=""; {print $0}' | FileCheck %s

.hmacro SET1
 BIU0.DM->IALU.T0(I0) || REPEAT @(3);
 T1 + T2 (S)->SHU0.T0 ;
.endhmacro

SET1;
REPEAT @(2);
.flushhm

//CHECK: BIU0.DM -> IALU.T0(I0) || REPEAT @(3);
//CHECK-NEXT: T1 + T2(S) -> SHU0.T0 || REPEAT @(2);
