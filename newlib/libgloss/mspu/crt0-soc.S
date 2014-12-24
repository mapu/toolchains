
	.global __mspu_init
	.global __mspu_stack_top
	.global __mspu_args
	.global __mspu_entry_point

	.text
__mspu_init:
	SP = __mspu_stack_top;;
	FP = SP ;;

	J31 = __mspu_args;;
	R0 = [J31 + 0];;
	J31 = __mspu_entry_point;;
	J31 = [J31 + 0];;
	call J31;;
	stop ;;
_exit:

	.comm __mspu_args,12,4 // reserverd 12 bytes for at most 3 args
	.comm __mspu_entry_point,4,4
