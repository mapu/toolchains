//RUN: llvm-mc -arch=mmpulite -filetype=obj -o %t.o %s
//RUN: llvm-objdump -arch-name=mmpulite -d -no-show-raw-insn %t.o | sed '1,4d' | awk '$1=""; {print $0}' | FileCheck %s

.hmacro test
LPTO (3f ) @(KI0);
T0-T1(S)->BIU2||LPTO (2f ) @(KI1);
T2+T3(S)->BIU2||LPTO (1f ) @(KI2);
T0+T1(S)->BIU2;
T0*T1(S)->BIU2;
1:
2:
3:
.endhmacro

.hmacro test2
LPTO (3f ) @(KI0);
T0-T1->BIU1||LPTO (2f ) @(KI1);
T2+T3->BIU1||LPTO (1f ) @(KI2);
T0+T1->BIU1;
T0*T1(T)->BIU1;
1:
2:
3:
.endhmacro

.hmacro test1
test||REPEAT @(3);
test2||REPEAT @(1);
.endhmacro
.opt
test1;
.flushhm

//CHECK: NOP;
//CHECK_NEXT: T0 - T1(S) -> BIU2 ;
//CHECK_NEXT: T2 + T3(S) -> BIU2 ;
//CHECK_NEXT: T0 + T1(S) -> BIU2 || LPTO ("HM1") @(KI0 - 1);
//CHECK_NEXT: T0 - T1 -> BIU1 || T0 * T1(S) -> BIU2 || LPTO ("HM2") @(KI1 - 1);
//CHECK_NEXT: T2 + T3 -> BIU1 || T0 + T1(S) -> BIU2 || LPTO ("HM3") @(KI2 - 2);
//CHECK_NEXT: T0 + T1 -> BIU1 || T0 * T1(S) -> BIU2 ;
//CHECK_NEXT: T0 * T1(T) -> BIU1 || T0 + T1(S) -> M[0] ;

//CHECK_NEXT: HM2:
//CHECK_NEXT: T0 + T1 -> BIU1 || T0 * T1(S) -> BIU2 ;
//CHECK_NEXT: T0 * T1(T) -> BIU1 || T2 + T3(S) -> M[0] ;
//CHECK_NEXT: T0 + T1 -> BIU1 || T0 + T1(S) -> BIU2 ;
//CHECK_NEXT: T0 * T1(T) -> BIU1 || T0 * T1(S) -> BIU2 ;

//CHECK_NEXT: HM3:
//CHECK_NEXT: T2 + T3 -> BIU1 || T0 + T1(S) -> BIU2 || LPTO ("HM4") @(KI2 - 2);
//CHECK_NEXT: T0 + T1 -> BIU1 || T0 * T1(S) -> BIU2 ;
//CHECK_NEXT: T0 * T1(T) -> BIU1 || T0 + T1(S) -> M[0] ;

//CHECK_NEXT: HM4:
//CHECK_NEXT: T0 + T1 -> BIU1 || T0 * T1(S) -> BIU2 ;
//CHECK_NEXT: T0 * T1(T) -> BIU1 || T0 - T1(S) -> M[0] ;
//CHECK_NEXT: T0 + T1 -> BIU1 || T2 + T3(S) -> BIU2 ;
//CHECK_NEXT: T0 * T1(T) -> BIU1 || T0 + T1(S) -> M[0] ;

//CHECK_NEXT: HM1:
//CHECK_NEXT: T0 - T1 -> BIU1 || T0 * T1(S) -> BIU2 || LPTO ("HM5") @(KI1 - 1);
//CHECK_NEXT: T2 + T3 -> BIU1 || T0 + T1(S) -> BIU2 || LPTO ("HM6") @(KI2 - 2);
//CHECK_NEXT: T0 + T1 -> BIU1 || T0 * T1(S) -> BIU2 ;
//CHECK_NEXT: T0 * T1(T) -> BIU1 || T0 + T1(S) -> M[0] ;

//CHECK_NEXT: HM6:
//CHECK_NEXT: T0 + T1 -> BIU1 || T0 * T1(S) -> BIU2 ;
//CHECK_NEXT: T0 * T1(T) -> BIU1 || T2 + T3(S) -> M[0] ;
//CHECK_NEXT: T0 + T1 -> BIU1 || T0 + T1(S) -> BIU2 ;
//CHECK_NEXT: T0 * T1(T) -> BIU1 || T0 * T1(S) -> BIU2 ;

//CHECK_NEXT: HM5:
//CHECK_NEXT: T2 + T3 -> BIU1 || T0 + T1(S) -> BIU2 || LPTO ("HM7") @(KI2 - 2);
//CHECK_NEXT: T0 + T1 -> BIU1 || T0 * T1(S) -> BIU2 ;
//CHECK_NEXT: T0 * T1(T) -> BIU1 || T0 + T1(S) -> M[0] ;

//CHECK_NEXT: HM7:
//CHECK_NEXT: T0 + T1 -> BIU1 || T0 * T1(S) -> BIU2 ;
//CHECK_NEXT: T0 * T1(T) -> BIU1 ;
//CHECK_NEXT: T0 + T1 -> BIU1 ;
//CHECK_NEXT: T0 * T1(T) -> BIU1 ;
