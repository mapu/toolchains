	.text
  .global _start

_start:  
    J29 = 0x2FF;;
    J28 = J29;;
    NOP;;
    
    CALL main;;
    
    R2 = 246;;
    SPUStop;;

