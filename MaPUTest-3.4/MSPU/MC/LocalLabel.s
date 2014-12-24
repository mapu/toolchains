#RUN: llvm-mc -arch=mspu -filetype=obj -o %t.o %s
#RUN: llvm-objdump -arch=mspu -d -no-show-raw-insn  %t.o | sed '1,5d' | awk '$1=""; {print $0}' > %t.s

#CHECK: jump 8 ;;
jump label0;;
nop;;
label0:

#CHECK: jump   -8 ;;
label1:
r1 = j1;;
nop;;
jump label1;;
nop;;
