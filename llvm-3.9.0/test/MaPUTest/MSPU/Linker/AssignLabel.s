#RUN: llvm-mc -arch=mspu -filetype=obj -o %t.o %s
#RUN: llvm-ld -T %s.ld %t.o -o %t.elf
#RUN: llvm-objdump -arch-name=mspu -d %t.elf -no-show-raw-insn | grep "m\.s" | FileCheck %s
.text 
#CHECK: m.s NOP;;
m.s NOP;;
#CHECK: m.s NOP;;
m.s NOP;;
#CHECK: m.s R0 = 32;;
m.s R0 = lab1;;
#CHECK: m.s R1 = 4194304;; 
m.s R1 = lab_SDA0_DM0;;
#CHECK: m.s R2 = 14680064;;
m.s R2 = lab_SDA1_DM1;;
.data 
lab1:
.int 0x2
.section .SDA0.DM0, "aw"
lab_SDA0_DM0:
.int 0x3
.section .SDA0.DM1, "aw"
.int 0x4
.section .SDA1.DM0, "aw"
.int 0x5
.section .SDA1.DM1, "aw"
.int 0x6
.section .SDA2.DM0, "aw"
.int 0x7
.section .SDA2.DM1, "aw"
lab_SDA1_DM1:
.int 0x8
