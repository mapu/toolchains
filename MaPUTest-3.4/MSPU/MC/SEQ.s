#RUN: llvm-mc -arch=mspu -filetype=obj -o %t.o %s
#RUN: llvm-objdump -arch=mspu -d -no-show-raw-insn %t.o | sed '1,5d' | awk '$1=""; {print $0}' > %t.s
#RUN: diff -w -i -I '^[ \t]*$' -i -I '[ \t]*#.*' -I '\/\/.*' %t.s %s

//the jump instruction
JUMP  J1;;
JUMP  40;;
JUMP  -40;;
if R1, JUMP J1;;
if R1, JUMP 40;;
if R1, JUMP -40;;

//the call instruction
CALL  J1;;
CALL  40;;
CALL  -40;;
if R1, CALL J1;;
if R1, CALL 40;;
if R1, CALL -40;;

//Loop instruction
LPTO 40 BY J1 (L0);;
LPTO 40 BY J1 (L1);;

//Start & Stop
SPU.Stop;;
NOP;;
