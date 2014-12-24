#RUN: llvm-mc -arch=mspu -filetype=obj -o %t.o %s
#RUN: llvm-objdump -arch=mspu -d -no-show-raw-insn %t.o | sed '1,5d' | awk '$1=""; {print $0}' > %t.s

r1 = foo;;
jump bar;;

j1 = 12345678;;

nop;;
