//RUN: llvm-mc -arch=mmpulite -filetype=obj -o %t.o %s
//RUN: llvm-objdump -arch-name=mmpulite -d -no-show-raw-insn %t.o | sed '1,4d' | awk '$1=""; {print $0}' | FileCheck %s

.hmacro SET1
 BIU0.DM->IALU.T0(I0) || REPEAT @(3);
.endhmacro

.hmacro main1
BIU1.DM->FALU.T0(I1)||REPEAT @(4);
.endhmacro

.hmacro main
main1|| SET1;
.endhmacro

main;
.flushhm

//CHECK: BIU0.DM -> IALU.T0(I0) || BIU1.DM -> FALU.T0(I1) ;
//CHECK_NEXT: BIU0.DM -> IALU.T0(I0) || BIU1.DM -> FALU.T0(I1) || REPEAT @(2);
//CHECK_NEXT: BIU1.DM -> FALU.T0(I1) ;
