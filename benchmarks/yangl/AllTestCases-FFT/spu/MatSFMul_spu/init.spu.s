
        .text
        .global _startup
_startup:
 m.s         SP = __stack_top;;
 m.s         FP = SP;;
 m.s         R0 = 0;;
 m.s         R1 = 0;;
 m.s         R2 = 0;;
 m.s         call main;;
 m.s         NOP;;
 m.s         NOP;;
 m.s         NOP;;
 m.s         NOP;;
 m.s         SPU.Stop;;

