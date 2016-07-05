#RUN: llvm-mc -arch=mspu -filetype=obj -o %t.o %s
#RUN: llvm-objdump -arch-name=mspu -d -no-show-raw-insn %t.o | sed '1,5d' | awk '$1=""; {print $0}' > %t.s
#RUN: diff -w -i -I '^[ \t]*$' -i -I '[ \t]*#.*' -I '\/\/.*' %t.s %s

//the jump instruction
m.s JUMP  J1;;
m.s JUMP  40;;
m.s JUMP  -40;;
m.s if R1, JUMP J1;;
m.s if R1, JUMP 40;;
m.s if R1, JUMP -40;;

//the call instruction
m.s CALL  J1;;
m.s CALL  40;;
m.s CALL  -40;;
m.s if R1, CALL J1;;
m.s if R1, CALL 40;;
m.s if R1, CALL -40;;

//Loop instruction
m.s LPTO 40 BY J1 (L0);;
m.s LPTO 40 BY J1 (L1);;

//Start & Stop
m.s SPU.Stop;;
m.s NOP;;
