1. assembly syntax for SPU

SPU is VLIW-styled ISA, llvm tools support 2 kinds of assembly syntax:
(1) all parallel instructions are placed in one line, say:
    insn1 ; insn2 ; insn3 ; insn4 ;;
(2) each instruction is placed in one line, like:
    insn1 ;
    insn2 ;
    insn3 ;
    insn4 ;;

For hand-written assembly files, the first style is recommended, meanwhile C backend generates
assembly files with the second style.

2. To-do's for MaPU SPU
-----
