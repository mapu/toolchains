#RUN: llvm-mc -arch=mspu -filetype=obj -o %t.o %s
#RUN: llvm-objdump -arch=mspu -d -no-show-raw-insn %t.o | sed '1,5d' | awk '$1=""; {print $0}' > %t.s

m.s r1 = foo;;
m.s jump bar;;

m.s j1 = 12345678;;

m.s nop;;
