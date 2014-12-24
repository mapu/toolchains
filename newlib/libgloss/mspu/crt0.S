
	.text
	.global __mspu_init
	.global __mspu_stack_top
	.global main
__mspu_init:
	J31 = __mspu_stack_top;;
  FP = J31;;
	SP = [J31 + -4];;
	R0 = [SP + 0];;
	R1 = [SP + 4];;
	R2 = [SP + 8];;
	call main ;;
	stop ;;
