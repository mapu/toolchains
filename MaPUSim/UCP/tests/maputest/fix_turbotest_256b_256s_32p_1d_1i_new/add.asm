	.file	"add.ll"
	.text
	.global	main
	.type	main,@function
main:                                   # @main
# BB#0:                                 # %entry
    [SP + 0] = FP ;;
    FP = SP ;;
    J31 = -32 ;;
    SP = SP + J31 ;;
    [FP + -8] = J30 ;;
    R0 = 0 ;;
    [FP + -12] = R0 ;;
    R0 = 5 ;;
    [FP + -16] = R0 ;;
    R0 = 2 ;;
    [FP + -20] = R0 ;;
    R1 = [FP + -16] ;;
    R0 = R1 + R0 (T) ;;
    [FP + -24] = R0 ;;
    jump J30 ;;
Ltmp0:
	.size	main, Ltmp0-main


