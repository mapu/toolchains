#RUN: llvm-mc -arch=mspu -filetype=obj -o %t.o %s
#RUN: llvm-objdump -arch=mspu -d -no-show-raw-insn %t.o | sed '1,5d' | awk '$1=""; {print $0}' > %t.s
#RUN: diff -w -i -I '^[ \t]*$' -i -I '[ \t]*#.*' -I '\/\/.*' %t.s %s

nop;;

nop ; nop ;;

r1 = r3 + r4 ; nop;;
r1 = r3 + r4 ; j1 = r3; nop;;

