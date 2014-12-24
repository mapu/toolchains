//RUN: llvm-mc -arch=mmpulite -filetype=obj -o %t.o %s
//RUN: llvm-objdump -arch=mmpulite -d -no-show-raw-insn %t.o | sed '1,4d' | awk '$1=""; {print $0}' | FileCheck %s

.hmacro SET1
 BIU0.DM->IALU.T0(I0) || REPEAT @(3);
.endhmacro

.hmacro main1
SET1||BIU1.DM-> FALU.T0(I1)||REPEAT @(2);
.endhmacro

main1;
.flushhm

//CHECK: BIU0.DM -> IALU.T0(I0) || BIU1.DM -> FALU.T0(I1) || REPEAT @(2);
//CHECK-NEXT: BIU0.DM -> IALU.T0(I0) ;
