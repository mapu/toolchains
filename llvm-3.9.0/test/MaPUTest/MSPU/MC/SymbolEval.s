#RUN: llvm-mc -arch=mspu -filetype=obj -o %t1.o %s
#RUN: llvm-ld -T %s.ld %t1.o -o %t2.elf
#RUN: llvm-objdump -arch-name=mspu -d -no-show-raw-insn  %t2.elf  | FileCheck %s

.data
  .int  89
  .int  32

start:
  .int  0x79
  .int  32
  .int  0x79
  .int  0x79

  pos = start+8

.text
#CHECK: J1 = 28;;
m.s  J1 = start;; 
#CHECK: J2 = 36;;
m.s  J2 = pos;;
#CHECK: J4 = 45;;
m.s  J4 = 45;;
