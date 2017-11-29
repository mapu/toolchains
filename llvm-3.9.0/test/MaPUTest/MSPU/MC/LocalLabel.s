#RUN: llvm-mc -arch=mspu -filetype=obj -o %t.o %s
#RUN: llvm-objdump -arch-name=mspu -d -no-show-raw-insn  %t.o | sed '1,5d' | awk '$1=""; {print $0}' > %t.s

#CHECK: jump 8 ;;
m.s jump label0;;
m.s nop;;
label0:

#CHECK: jump   -8 ;;
label1:
m.s r1 = j1;;
m.s nop;;
m.s jump label1;;
m.s nop;;
