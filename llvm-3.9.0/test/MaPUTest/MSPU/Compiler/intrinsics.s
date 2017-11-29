	.file	"intrinsics.c"
	.text
	.global	intr
	.type	intr,@function
intr:                                   # @intr
# BB#0:                                 # %entry
	[SP + -4] = FP;;
	FP = SP;;
	J31 = -64;;
	SP = SP + J31;;
	R2 = 3;;
	R3 = 12;;
	KB3 = R2;;
	KE1 = R2;;
	R4 = 93;;
	KS2 = R2;;
	KI3 = R2;;
	KM0 = R2;;
	KG1 = R2;;
	KL2 = R2;;
	R5 = KB0;;
	R6 = KE1;;
	R7 = KS2;;
	R8 = KI3;;
	R9 = KM0;;
	R10 = KG1;;
	R11 = KL2;;
	J2 = fn;;
	R12 = CStat;;
	R13 = MC.r1;;
	R14 = LPCTR;;
	MC.w2 = R3;;
	CH[3] = R4;;
	R3 = CH[3];;
	CH[R2] = R4;;
	R4 = CH[R2];;
	R15 = CH[3] (N);;
	R2 = CH[R2] (N);;
	CallM J2;;
	CallM J2 (B);;
	[FP + -8] = R15;;
	[FP + -12] = R14;;
	[FP + -16] = R3;;
	[FP + -20] = R5;;
	[FP + -24] = R6;;
	[FP + -28] = R7;;
	[FP + -32] = R8;;
	[FP + -36] = R9;;
	[FP + -40] = R10;;
	[FP + -44] = R11;;
	[FP + -48] = R2;;
	[FP + -52] = R12;;
	[FP + -56] = R13;;
	[FP + -60] = R4;;
	SP = FP;;
	FP = [SP + -4];;
	jump J30;;
Ltmp0:
	.size	intr, Ltmp0-intr


