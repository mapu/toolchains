//RUN: llvm-mc -arch=mmpulite -filetype=obj -o %t.o %s
//RUN: llvm-objdump -arch-name=mmpulite -d -no-show-raw-insn %t.o | sed -e '1,5d' -e "s/[[:xdigit:]]\+://I" | FileCheck %s

//dec
T1 << 1   -> IALU.T3;
T1 << 10  -> IALU.T3;
T1 << 19  -> IALU.T3;

//hex
T1 << 0x1  -> IALU.T3;
T1 << 0xa  -> IALU.T3;
T1 << 0x13 -> IALU.T3;

//oct
T1 << 01   -> IALU.T3;
T1 << 012  -> IALU.T3;
T1 << 023  -> IALU.T3;

//CHECK: T1 << 1 -> IALU.T3 ;
//CHECK-NEXT: T1 << 10  -> IALU.T3 ;
//CHECK-NEXT: T1 << 19  -> IALU.T3 ;
//CHECK-NEXT: T1 << 1   -> IALU.T3 ;
//CHECK-NEXT: T1 << 10  -> IALU.T3 ;
//CHECK-NEXT: T1 << 19  -> IALU.T3 ;
//CHECK-NEXT: T1 << 1   -> IALU.T3 ;
//CHECK-NEXT: T1 << 10  -> IALU.T3 ;
//CHECK-NEXT: T1 << 19  -> IALU.T3 ;
