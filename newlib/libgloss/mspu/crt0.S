
	.text
	.global __mspu_init
	.global __mspu_stack_top
	.global main
__mspu_init:
	m.s J31 = __mspu_stack_top;;
  FP = J31;;
	m.s SP = [J31 + -4];;
	m.s R0 = [SP + 0];;
	m.s R1 = [SP + 4];;
	m.s R2 = [SP + 8];;
	m.s call main ;;
	m.s stop ;;
