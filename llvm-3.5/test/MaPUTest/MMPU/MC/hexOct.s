//RUN: llvm-mc -arch=mmpulite hexOct.s -filetype=obj -o hexOct.o
//RUN: llvm-objdump -d -arch-name=mmpulite hexOct.o -no-show-raw-insn

//dec
T1 << 1   -> IALU.T3;
T1 << 10  -> IALU.T3;
T1 << 19  -> IALU.T3;

//hex
T1 << 0x1  -> IALU.T3;
T1 << 0xa  -> IALU.T3;
T1 << 0x13 -> IALU.T3;

//oct
T1 << 01   -> IALU.T3;
T1 << 012  -> IALU.T3;
T1 << 023  -> IALU.T3;
