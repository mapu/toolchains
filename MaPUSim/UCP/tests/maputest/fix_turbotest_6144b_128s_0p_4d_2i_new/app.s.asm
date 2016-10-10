	.text
  .global _start

_start:  
 m.s     J29 = 0x2FF;;
 m.s     J28 = J29;;
 m.s     NOP;;
    
 m.s     CALL main;;
    
 m.s     R2 = 246;;
 m.s     SPUStop;;

