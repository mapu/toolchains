//RUN: llvm-mc -arch=mmpulite -filetype=obj -o %t.o %s
//RUN: llvm-objdump -arch-name=mmpulite -d -no-show-raw-insn %t.o | sed '1,4d' | awk '$1=""; {print $0}' | FileCheck %s

.hmacro loop1
LPTO (2f ) @(KI2)||LPTO (2f ) @(KI1);
BIU0.DM(A++,K++)->M[I++]||REPEAT @(15);
BIU0.DM(A++,K++)->M[I++]||LPTO (2f ) @(KI0-1);
REPEAT @(16);
2:
.endhm

.hmacro loop2
LPTO (2f ) @(KI2)||LPTO (2f ) @(KI1)||LPTO (2f ) @(KI0);
REPEAT @(15);
BIU1.DM(A++,K++)->SHU0.T0;
2:
.endhm

.hmacro main
loop1;
loop2;
.endhm

.opt
main;
.flushhm

//CHECK: NOP;
//CHECK_NEXT: BIU0.DM(A++, K++) -> M[I++] || LPTO ("HM1") @(KI2 - 1);
//CHECK_NEXT: BIU0.DM(A++, K++) -> M[I++] || LPTO ("HM2") @(KI1 - 1);
//CHECK_NEXT: BIU0.DM(A++, K++) -> M[I++] || REPEAT @(13);
//CHECK_NEXT: BIU0.DM(A++, K++) -> M[I++] || LPTO ("HM3") @(KI0 - 1);
//CHECK_NEXT: BIU1.DM(A++, K++) -> SHU0.T0 ;
//CHECK_NEXT: REPEAT @(15);
//CHECK_NEXT: HM3:
//CHECK_NEXT: BIU0.DM(A++, K++) -> M[I++] || BIU1.DM(A++, K++) -> SHU0.T0 ;
//CHECK_NEXT: BIU0.DM(A++, K++) -> M[I++] ;
//CHECK_NEXT: HM2:
//CHECK_NEXT: BIU0.DM(A++, K++) -> M[I++] || REPEAT @(13);
//CHECK_NEXT: BIU0.DM(A++, K++) -> M[I++] || LPTO ("HM4") @(KI0 - 1);
//CHECK_NEXT: BIU1.DM(A++, K++) -> SHU0.T0 ;
//CHECK_NEXT: REPEAT @(15);
//CHECK_NEXT: HM4:
//CHECK_NEXT: BIU0.DM(A++, K++) -> M[I++] || BIU1.DM(A++, K++) -> SHU0.T0 ;
//CHECK_NEXT: HM1:
//CHECK_NEXT: BIU0.DM(A++, K++) -> M[I++] || LPTO ("HM5") @(KI1 - 1);
//CHECK_NEXT: BIU0.DM(A++, K++) -> M[I++] || REPEAT @(13);
//CHECK_NEXT: BIU0.DM(A++, K++) -> M[I++] || LPTO ("HM6") @(KI0 - 1);
//CHECK_NEXT: BIU1.DM(A++, K++) -> SHU0.T0 ;
//CHECK_NEXT: REPEAT @(15);
//CHECK_NEXT: HM6:
//CHECK_NEXT: BIU0.DM(A++, K++) -> M[I++] || BIU1.DM(A++, K++) -> SHU0.T0 ;
//CHECK_NEXT: BIU0.DM(A++, K++) -> M[I++] ;
//CHECK_NEXT: HM5:
//CHECK_NEXT: BIU0.DM(A++, K++) -> M[I++] || REPEAT @(13);
//CHECK_NEXT: BIU0.DM(A++, K++) -> M[I++] || LPTO ("HM7") @(KI0 - 1);
//CHECK_NEXT: BIU1.DM(A++, K++) -> SHU0.T0 ;
//CHECK_NEXT: REPEAT @(15);
//CHECK_NEXT: HM7:
//CHECK_NEXT: BIU1.DM(A++, K++) -> SHU0.T0 ;

