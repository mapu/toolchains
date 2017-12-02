//RUN: llvm-mc %s -arch=mmpulite -filetype=obj -o %t.o
//RUN: llvm-objdump -arch-name=mmpulite -d -no-show-raw-insn %t.o | sed '1,5d' | awk '$1=""; {print $0}' > %t.s
//RUN: diff -w -i -I '^[ \t]*$' -I '[ \t]*#.*' -I '\/\/.*' %t.s %s

////////////////////////////////////////////////////
//Repeat 
repeat @ ( 123 );
repeat @ ( 1888 );
repeat @ ( KI12 -0);
repeat @ ( KI12 -4);

////////////////////////////////////////////////////
//loop
LPTO (287 ) @ (KI13-0);
LPTO (82 ) @ (KI14-0);
LPTO (41 ) @ (KI15-0);
LPTO (1888 ) @ (KI13-0);

////////////////////////////////////////////////////
MPUSTOP;

