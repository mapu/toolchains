
	.global __mspu_init
	.global __mspu_stack_top
	.global __mspu_args
	.global __mspu_entry_point

	.text
__mspu_init:
	m.s SP = __mspu_stack_top;;
	m.s FP = SP ;;

	m.s J31 = __mspu_args;;
	m.s R0 = [J31 + 0];;
	m.s J31 = __mspu_entry_point;;
	m.s J31 = [J31 + 0];;
	m.s call J31;;
	m.s stop ;;
_exit:

	.comm __mspu_args,12,4 // reserverd 12 bytes for at most 3 args
	.comm __mspu_entry_point,4,4
