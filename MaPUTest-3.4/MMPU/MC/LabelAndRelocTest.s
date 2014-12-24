//RUN: llvm-mc -arch=mmpulite -filetype=obj -o %t.o %s
//RUN: llvm-objdump -arch=mmpulite -d -symbolize -no-show-raw-insn %t.o | sed -e '1,5d' -e "s/[[:xdigit:]]\+://I" | FileCheck %s

LPTO (xyz ) @(KI12 - 0);
MPUSTOP;
MPUSTOP;
1:
MPUSTOP;
//CHECK: LPTO (.text+164) @(KI12 - 0);
//CHECK-NEXT: MPUSTOP;
//CHECK-NEXT: MPUSTOP;
//CHECK-NEXT: MPUSTOP;

xyz:
LPTO (1b ) @(KI12);
//CHECK: xyz:
//CHECK-NEXT:  LPTO (.text+123) @(KI12 - 0);
