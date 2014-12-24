#RUN: llvm-mc -arch=mspu -filetype=obj -o %t1.o %s
#RUN: llvm-ld -T %s.ld %t1.o -o %t2.elf
#RUN: llvm-objdump -arch=mspu -d -no-show-raw-insn  %t2.elf  | FileCheck %s

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
  J1 = start;; 
#CHECK: J2 = 36;;
  J2 = pos;;
#CHECK: J4 = 45;;
  J4 = 45;;
