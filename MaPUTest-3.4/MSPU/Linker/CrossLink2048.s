#RUN: echo -e ".global lab1	\n.org 1024*41 \nNOP;  \nNOP;  \nLPTO lab1 @ (KI13)  ;\nNOP  ;\nlab1: \nNOP;" > %t.mmpu.s
#RUN: llvm-mc -arch=mmpulite -filetype=obj -o %t.mmpu.o %t.mmpu.s
#RUN: llvm-ld -T %s.m.ld -o %t.mmpu.elf %t.mmpu.o
#RUN: llvm-mc -arch=mspu -filetype=obj -o %t.o %s
#RUN: llvm-ld %t.o -o %t.elf --just-symbols %t.mmpu.elf
#RUN: llvm-objdump -arch=mspu -d %t.elf | cut -b 128- | FileCheck %s
.text 
#CHECK: CallM 42148;;
CallM lab1;;
