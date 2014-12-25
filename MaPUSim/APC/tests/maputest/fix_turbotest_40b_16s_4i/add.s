	.file	"add.c"
	.text
	.global	SAT_ADD
	.type	SAT_ADD,@function
SAT_ADD:                                # @SAT_ADD
# BB#0:
	[SP + -4] = FP;;
	[SP + -8] = J30;;
	FP = SP;;
	J31 = -48;;
	SP = SP + J31;;
	[FP + -16] = R0;;
	[FP + -20] = R1;;
	[FP + -24] = R2;;
	[FP + -28] = R3;;
	R4 = [FP + -16];;
	R5 = [FP + -20];;
	R6 = [FP + -24];;
	R4 = R4 + R5 (T);;
	R4 = R4 <= R6;;
	[FP + -32] = R0;;
	[FP + -36] = R2;;
	[FP + -40] = R3;;
	[FP + -44] = R1;;
	if R4, jump LBB0_2;;
	jump LBB0_1;;
LBB0_1:
	R0 = [FP + -24];;
	[FP + -12] = R0;;
	jump LBB0_5;;
LBB0_2:
	R0 = [FP + -20];;
	R1 = [FP + -16];;
	R2 = [FP + -28];;
	R0 = R1 + R0 (T);;
	R0 = R0 >= R2;;
	if R0, jump LBB0_4;;
	jump LBB0_3;;
LBB0_3:
	R0 = [FP + -28];;
	[FP + -12] = R0;;
	jump LBB0_5;;
LBB0_4:
	R0 = [FP + -20];;
	R1 = [FP + -16];;
	R0 = R1 + R0 (T);;
	[FP + -12] = R0;;
LBB0_5:
	R0 = [FP + -12];;
	SP = FP;;
	J30 = [SP + -8];;
	FP = [SP + -4];;
	jump J30;;
Ltmp0:
	.size	SAT_ADD, Ltmp0-SAT_ADD

	.global	max_8to1
	.type	max_8to1,@function
max_8to1:                               # @max_8to1
# BB#0:
	[SP + -4] = FP;;
	[SP + -8] = J30;;
	FP = SP;;
	J31 = -40;;
	SP = SP + J31;;
	[FP + -12] = R0;;
	[FP + -16] = R1;;
	J0 = R1;;
	R2 = [J0 + 0] (H);;
	R3 = 1;;
	[FP + -24] = R2;;
	[FP + -20] = R3;;
	[FP + -28] = R0;;
	[FP + -32] = R1;;
LBB1_1:                                 # =>This Inner Loop Header: Depth=1
	R0 = [FP + -20];;
	R1 = 7;;
	R0 = R0 > R1;;
	if R0, jump LBB1_7;;
	jump LBB1_2;;
LBB1_2:                                 #   in Loop: Header=BB1_1 Depth=1
	R0 = [FP + -20];;
	R1 = [FP + -16];;
	R2 = [FP + -24];;
	R0 = R0 << 1;;
	R0 = R1 + R0 (T);;
	J0 = R0;;
	R0 = [J0 + 0] (H);;
	R0 = R0 < R2;;
	if R0, jump LBB1_4;;
	jump LBB1_3;;
LBB1_3:                                 #   in Loop: Header=BB1_1 Depth=1
	R0 = [FP + -20];;
	R1 = [FP + -16];;
	R0 = R0 << 1;;
	R0 = R1 + R0 (T);;
	J0 = R0;;
	R0 = [J0 + 0] (H);;
	[FP + -36] = R0;;
	jump LBB1_5;;
LBB1_4:                                 #   in Loop: Header=BB1_1 Depth=1
	R0 = [FP + -24];;
	[FP + -36] = R0;;
LBB1_5:                                 #   in Loop: Header=BB1_1 Depth=1
	R0 = [FP + -36];;
	[FP + -24] = R0;;
# BB#6:                                 #   in Loop: Header=BB1_1 Depth=1
	R0 = [FP + -20];;
	R1 = 1;;
	R0 = R0 + R1 (T);;
	[FP + -20] = R0;;
	jump LBB1_1;;
LBB1_7:
	R0 = [FP + -12];;
	R1 = [FP + -24];;
	J0 = R0;;
	[J0 + 0] = R1;;
	SP = FP;;
	J30 = [SP + -8];;
	FP = [SP + -4];;
	jump J30;;
Ltmp1:
	.size	max_8to1, Ltmp1-max_8to1

	.global	compute_beta_init
	.type	compute_beta_init,@function
compute_beta_init:                      # @compute_beta_init
# BB#0:
	[SP + -4] = FP;;
	[SP + -8] = J30;;
	FP = SP;;
	J31 = -392;;
	SP = SP + J31;;
	[FP + -12] = R0;;
	[FP + -16] = R1;;
	[FP + -20] = R2;;
	[FP + -24] = R3;;
	R4 = [FP + -12];;
	R5 = -2048;;
	R6 = 2047;;
	J0 = beta_initial_value_cb1;;
	R7 = 0;;
	R4 = R4 + R3 (T);;
	J31 = -44;;
	J1 = FP + J31;;
	J2 = R4;;
	R4 = [J2 + 0] (B)(U);;
	[FP + -27] = R4 (B);;
	R4 = [FP + -24];;
	R15 = [FP + -20];;
	R4 = R15 + R4 (T);;
	J2 = R4;;
	R4 = [J2 + 0] (B)(U);;
	[FP + -26] = R4 (B);;
	R4 = [FP + -24];;
	R15 = [FP + -16];;
	R4 = R15 + R4 (T);;
	J2 = R4;;
	R4 = [J2 + 1] (B)(U);;
	[FP + -25] = R4 (B);;
	R4 = [FP + -24];;
	R15 = [FP + -16];;
	R4 = R15 + R4 (T);;
	J2 = R4;;
	R4 = [J2 + 0] (B)(U);;
	[FP + -33] = R4 (B);;
	R4 = [FP + -24];;
	R15 = [FP + -12];;
	R4 = R15 + R4 (T);;
	J2 = R4;;
	R4 = [J2 + 1] (B)(U);;
	[FP + -32] = R4 (B);;
	R4 = [FP + -24];;
	R15 = [FP + -20];;
	R4 = R15 + R4 (T);;
	J2 = R4;;
	R4 = [J2 + 1] (B)(U);;
	[FP + -31] = R4 (B);;
	R4 = [FP + -24];;
	R15 = [FP + -12];;
	R4 = R15 + R4 (T);;
	J2 = R4;;
	R4 = [J2 + 2] (B)(U);;
	[FP + -30] = R4 (B);;
	R4 = [FP + -24];;
	R15 = [FP + -20];;
	R4 = R15 + R4 (T);;
	J2 = R4;;
	R4 = [J2 + 2] (B)(U);;
	[FP + -29] = R4 (B);;
	R4 = [FP + -24];;
	R15 = [FP + -16];;
	R4 = R15 + R4 (T);;
	J2 = R4;;
	R4 = [J2 + 3] (B)(U);;
	[FP + -28] = R4 (B);;
	R4 = [FP + -24];;
	R15 = [FP + -16];;
	R4 = R15 + R4 (T);;
	J2 = R4;;
	R4 = [J2 + 2] (B)(U);;
	[FP + -36] = R4 (B);;
	R4 = [FP + -24];;
	R15 = [FP + -12];;
	R4 = R15 + R4 (T);;
	J2 = R4;;
	R4 = [J2 + 3] (B)(U);;
	[FP + -35] = R4 (B);;
	R4 = [FP + -24];;
	R15 = [FP + -20];;
	R4 = R15 + R4 (T);;
	J2 = R4;;
	R4 = [J2 + 3] (B)(U);;
	[FP + -34] = R4 (B);;
	R4 = [FP + -33] (B);;
	R15 = [FP + -27] (B);;
	[FP + -48] = R0;;
	R0 = R15;;
	[FP + -52] = R1;;
	R1 = R4;;
	[FP + -56] = R2;;
	R2 = R6;;
	[FP + -60] = R3;;
	R3 = R5;;
	[FP + -64] = R6;;
	[FP + -68] = J0;;
	[FP + -72] = J1;;
	[FP + -76] = R5;;
	[FP + -80] = R7;;
	call SAT_ADD;;
	R1 = [FP + -32] (B);;
	R2 = [FP + -26] (B);;
	[FP + -84] = R0;;
	R0 = R2;;
	R2 = [FP + -64];;
	R3 = [FP + -76];;
	call SAT_ADD;;
	R1 = [FP + -84];;
	[FP + -88] = R0;;
	R0 = R1;;
	R1 = [FP + -88];;
	R2 = [FP + -64];;
	R3 = [FP + -76];;
	call SAT_ADD;;
	R1 = [FP + -31] (B);;
	R2 = [FP + -25] (B);;
	[FP + -92] = R0;;
	R0 = R2;;
	R2 = [FP + -64];;
	R3 = [FP + -76];;
	call SAT_ADD;;
	R1 = [FP + -92];;
	[FP + -96] = R0;;
	R0 = R1;;
	R1 = [FP + -96];;
	R2 = [FP + -64];;
	R3 = [FP + -76];;
	call SAT_ADD;;
	R0 = R0 >> 1 (U);;
	J0 = [FP + -68];;
	[J0 + 0] = R0 (H);;
	R0 = [FP + -33] (B);;
	R1 = [FP + -27] (B);;
	R2 = [FP + -80];;
	R0 = R2 - R0 (T);;
	R1 = R2 - R1 (T);;
	[FP + -100] = R0;;
	R0 = R1;;
	R1 = [FP + -100];;
	R2 = [FP + -64];;
	R3 = [FP + -76];;
	call SAT_ADD;;
	R1 = [FP + -32] (B);;
	R2 = [FP + -26] (B);;
	[FP + -104] = R0;;
	R0 = R2;;
	R2 = [FP + -64];;
	R3 = [FP + -76];;
	call SAT_ADD;;
	R1 = [FP + -104];;
	[FP + -108] = R0;;
	R0 = R1;;
	R1 = [FP + -108];;
	R2 = [FP + -64];;
	R3 = [FP + -76];;
	call SAT_ADD;;
	R1 = [FP + -31] (B);;
	R2 = [FP + -25] (B);;
	[FP + -112] = R0;;
	R0 = R2;;
	R2 = [FP + -64];;
	R3 = [FP + -76];;
	call SAT_ADD;;
	R1 = [FP + -112];;
	[FP + -116] = R0;;
	R0 = R1;;
	R1 = [FP + -116];;
	R2 = [FP + -64];;
	R3 = [FP + -76];;
	call SAT_ADD;;
	R0 = R0 >> 1 (U);;
	J0 = [FP + -68];;
	[J0 + 2] = R0 (H);;
	R1 = [FP + -33] (B);;
	R0 = [FP + -27] (B);;
	R2 = [FP + -80];;
	R0 = R2 - R0 (T);;
	R2 = [FP + -64];;
	R3 = [FP + -76];;
	call SAT_ADD;;
	R1 = [FP + -32] (B);;
	R2 = [FP + -26] (B);;
	R3 = [FP + -80];;
	R1 = R3 - R1 (T);;
	R2 = R3 - R2 (T);;
	[FP + -120] = R0;;
	R0 = R2;;
	R2 = [FP + -64];;
	R3 = [FP + -76];;
	call SAT_ADD;;
	R1 = [FP + -120];;
	[FP + -124] = R0;;
	R0 = R1;;
	R1 = [FP + -124];;
	R2 = [FP + -64];;
	R3 = [FP + -76];;
	call SAT_ADD;;
	R1 = [FP + -31] (B);;
	R2 = [FP + -25] (B);;
	[FP + -128] = R0;;
	R0 = R2;;
	R2 = [FP + -64];;
	R3 = [FP + -76];;
	call SAT_ADD;;
	R1 = [FP + -128];;
	[FP + -132] = R0;;
	R0 = R1;;
	R1 = [FP + -132];;
	R2 = [FP + -64];;
	R3 = [FP + -76];;
	call SAT_ADD;;
	R0 = R0 >> 1 (U);;
	J0 = [FP + -68];;
	[J0 + 4] = R0 (H);;
	R0 = [FP + -33] (B);;
	R1 = [FP + -27] (B);;
	R2 = [FP + -80];;
	R0 = R2 - R0 (T);;
	[FP + -136] = R0;;
	R0 = R1;;
	R1 = [FP + -136];;
	R2 = [FP + -64];;
	R3 = [FP + -76];;
	call SAT_ADD;;
	R1 = [FP + -32] (B);;
	R2 = [FP + -26] (B);;
	R3 = [FP + -80];;
	R1 = R3 - R1 (T);;
	R2 = R3 - R2 (T);;
	[FP + -140] = R0;;
	R0 = R2;;
	R2 = [FP + -64];;
	R3 = [FP + -76];;
	call SAT_ADD;;
	R1 = [FP + -140];;
	[FP + -144] = R0;;
	R0 = R1;;
	R1 = [FP + -144];;
	R2 = [FP + -64];;
	R3 = [FP + -76];;
	call SAT_ADD;;
	R1 = [FP + -31] (B);;
	R2 = [FP + -25] (B);;
	[FP + -148] = R0;;
	R0 = R2;;
	R2 = [FP + -64];;
	R3 = [FP + -76];;
	call SAT_ADD;;
	R1 = [FP + -148];;
	[FP + -152] = R0;;
	R0 = R1;;
	R1 = [FP + -152];;
	R2 = [FP + -64];;
	R3 = [FP + -76];;
	call SAT_ADD;;
	R0 = R0 >> 1 (U);;
	J0 = [FP + -68];;
	[J0 + 6] = R0 (H);;
	R0 = [FP + -33] (B);;
	R1 = [FP + -27] (B);;
	R2 = [FP + -80];;
	R0 = R2 - R0 (T);;
	[FP + -156] = R0;;
	R0 = R1;;
	R1 = [FP + -156];;
	R2 = [FP + -64];;
	R3 = [FP + -76];;
	call SAT_ADD;;
	R1 = [FP + -26] (B);;
	R2 = [FP + -32] (B);;
	R3 = [FP + -80];;
	R1 = R3 - R1 (T);;
	[FP + -160] = R0;;
	R0 = R1;;
	R1 = R2;;
	R2 = [FP + -64];;
	R3 = [FP + -76];;
	call SAT_ADD;;
	R1 = [FP + -160];;
	[FP + -164] = R0;;
	R0 = R1;;
	R1 = [FP + -164];;
	R2 = [FP + -64];;
	R3 = [FP + -76];;
	call SAT_ADD;;
	R1 = [FP + -31] (B);;
	R2 = [FP + -25] (B);;
	R3 = [FP + -80];;
	R1 = R3 - R1 (T);;
	R2 = R3 - R2 (T);;
	[FP + -168] = R0;;
	R0 = R2;;
	R2 = [FP + -64];;
	R3 = [FP + -76];;
	call SAT_ADD;;
	R1 = [FP + -168];;
	[FP + -172] = R0;;
	R0 = R1;;
	R1 = [FP + -172];;
	R2 = [FP + -64];;
	R3 = [FP + -76];;
	call SAT_ADD;;
	R0 = R0 >> 1 (U);;
	J0 = [FP + -68];;
	[J0 + 8] = R0 (H);;
	R1 = [FP + -33] (B);;
	R0 = [FP + -27] (B);;
	R2 = [FP + -80];;
	R0 = R2 - R0 (T);;
	R2 = [FP + -64];;
	R3 = [FP + -76];;
	call SAT_ADD;;
	R1 = [FP + -26] (B);;
	R2 = [FP + -32] (B);;
	R3 = [FP + -80];;
	R1 = R3 - R1 (T);;
	[FP + -176] = R0;;
	R0 = R1;;
	R1 = R2;;
	R2 = [FP + -64];;
	R3 = [FP + -76];;
	call SAT_ADD;;
	R1 = [FP + -176];;
	[FP + -180] = R0;;
	R0 = R1;;
	R1 = [FP + -180];;
	R2 = [FP + -64];;
	R3 = [FP + -76];;
	call SAT_ADD;;
	R1 = [FP + -31] (B);;
	R2 = [FP + -25] (B);;
	R3 = [FP + -80];;
	R1 = R3 - R1 (T);;
	R2 = R3 - R2 (T);;
	[FP + -184] = R0;;
	R0 = R2;;
	R2 = [FP + -64];;
	R3 = [FP + -76];;
	call SAT_ADD;;
	R1 = [FP + -184];;
	[FP + -188] = R0;;
	R0 = R1;;
	R1 = [FP + -188];;
	R2 = [FP + -64];;
	R3 = [FP + -76];;
	call SAT_ADD;;
	R0 = R0 >> 1 (U);;
	J0 = [FP + -68];;
	[J0 + 10] = R0 (H);;
	R0 = [FP + -33] (B);;
	R1 = [FP + -27] (B);;
	R2 = [FP + -80];;
	R0 = R2 - R0 (T);;
	R1 = R2 - R1 (T);;
	[FP + -192] = R0;;
	R0 = R1;;
	R1 = [FP + -192];;
	R2 = [FP + -64];;
	R3 = [FP + -76];;
	call SAT_ADD;;
	R1 = [FP + -32] (B);;
	R2 = [FP + -26] (B);;
	R3 = [FP + -80];;
	R1 = R3 - R1 (T);;
	[FP + -196] = R0;;
	R0 = R2;;
	R2 = [FP + -64];;
	R3 = [FP + -76];;
	call SAT_ADD;;
	R1 = [FP + -196];;
	[FP + -200] = R0;;
	R0 = R1;;
	R1 = [FP + -200];;
	R2 = [FP + -64];;
	R3 = [FP + -76];;
	call SAT_ADD;;
	R1 = [FP + -31] (B);;
	R2 = [FP + -25] (B);;
	R3 = [FP + -80];;
	R1 = R3 - R1 (T);;
	R2 = R3 - R2 (T);;
	[FP + -204] = R0;;
	R0 = R2;;
	R2 = [FP + -64];;
	R3 = [FP + -76];;
	call SAT_ADD;;
	R1 = [FP + -204];;
	[FP + -208] = R0;;
	R0 = R1;;
	R1 = [FP + -208];;
	R2 = [FP + -64];;
	R3 = [FP + -76];;
	call SAT_ADD;;
	R0 = R0 >> 1 (U);;
	J0 = [FP + -68];;
	[J0 + 12] = R0 (H);;
	R1 = [FP + -33] (B);;
	R0 = [FP + -27] (B);;
	R2 = [FP + -64];;
	R3 = [FP + -76];;
	call SAT_ADD;;
	R1 = [FP + -32] (B);;
	R2 = [FP + -26] (B);;
	R3 = [FP + -80];;
	R1 = R3 - R1 (T);;
	[FP + -212] = R0;;
	R0 = R2;;
	R2 = [FP + -64];;
	R3 = [FP + -76];;
	call SAT_ADD;;
	R1 = [FP + -212];;
	[FP + -216] = R0;;
	R0 = R1;;
	R1 = [FP + -216];;
	R2 = [FP + -64];;
	R3 = [FP + -76];;
	call SAT_ADD;;
	R1 = [FP + -31] (B);;
	R2 = [FP + -25] (B);;
	R3 = [FP + -80];;
	R1 = R3 - R1 (T);;
	R2 = R3 - R2 (T);;
	[FP + -220] = R0;;
	R0 = R2;;
	R2 = [FP + -64];;
	R3 = [FP + -76];;
	call SAT_ADD;;
	R1 = [FP + -220];;
	[FP + -224] = R0;;
	R0 = R1;;
	R1 = [FP + -224];;
	R2 = [FP + -64];;
	R3 = [FP + -76];;
	call SAT_ADD;;
	R0 = R0 >> 1 (U);;
	J0 = [FP + -68];;
	[J0 + 0] = R0 (H);;
	J1 = [FP + -72];;
	R0 = J1;;
	R1 = J0;;
	call max_8to1;;
	R0 = [FP + -80];;
	[FP + -40] = R0;;
LBB2_1:                                 # =>This Inner Loop Header: Depth=1
	R0 = [FP + -40];;
	R1 = 7;;
	R0 = R0 > R1;;
	if R0, jump LBB2_4;;
	jump LBB2_2;;
LBB2_2:                                 #   in Loop: Header=BB2_1 Depth=1
	R0 = [FP + -40];;
	J0 = beta_initial_value_cb1;;
	R1 = [FP + -44];;
	R0 = R0 << 1;;
	R2 = J0;;
	R0 = R2 + R0 (T);;
	J0 = R0;;
	R2 = [J0 + 0] (H)(U);;
	R1 = R2 - R1 (T);;
	J0 = R0;;
	[J0 + 0] = R1 (H);;
# BB#3:                                 #   in Loop: Header=BB2_1 Depth=1
	R0 = [FP + -40];;
	R1 = 1;;
	R0 = R0 + R1 (T);;
	[FP + -40] = R0;;
	jump LBB2_1;;
LBB2_4:
	R1 = [FP + -36] (B);;
	R0 = [FP + -30] (B);;
	R2 = -2048;;
	R3 = 2047;;
	J0 = beta_initial_value_cb2;;
	[FP + -228] = R2;;
	R2 = R3;;
	R4 = [FP + -228];;
	[FP + -232] = R3;;
	R3 = R4;;
	[FP + -236] = J0;;
	call SAT_ADD;;
	R1 = [FP + -35] (B);;
	R2 = [FP + -29] (B);;
	R3 = 0;;
	J31 = -44;;
	J0 = FP + J31;;
	[FP + -240] = R0;;
	R0 = R2;;
	R2 = [FP + -232];;
	R4 = [FP + -228];;
	[FP + -244] = R3;;
	R3 = R4;;
	[FP + -248] = J0;;
	call SAT_ADD;;
	R1 = [FP + -240];;
	[FP + -252] = R0;;
	R0 = R1;;
	R1 = [FP + -252];;
	R2 = [FP + -232];;
	R3 = [FP + -228];;
	call SAT_ADD;;
	R1 = [FP + -34] (B);;
	R2 = [FP + -28] (B);;
	[FP + -256] = R0;;
	R0 = R2;;
	R2 = [FP + -232];;
	R3 = [FP + -228];;
	call SAT_ADD;;
	R1 = [FP + -256];;
	[FP + -260] = R0;;
	R0 = R1;;
	R1 = [FP + -260];;
	R2 = [FP + -232];;
	R3 = [FP + -228];;
	call SAT_ADD;;
	R0 = R0 >> 1 (U);;
	J0 = [FP + -236];;
	[J0 + 0] = R0 (H);;
	R0 = [FP + -36] (B);;
	R1 = [FP + -30] (B);;
	R2 = [FP + -244];;
	R0 = R2 - R0 (T);;
	R1 = R2 - R1 (T);;
	[FP + -264] = R0;;
	R0 = R1;;
	R1 = [FP + -264];;
	R2 = [FP + -232];;
	R3 = [FP + -228];;
	call SAT_ADD;;
	R1 = [FP + -35] (B);;
	R2 = [FP + -29] (B);;
	[FP + -268] = R0;;
	R0 = R2;;
	R2 = [FP + -232];;
	R3 = [FP + -228];;
	call SAT_ADD;;
	R1 = [FP + -268];;
	[FP + -272] = R0;;
	R0 = R1;;
	R1 = [FP + -272];;
	R2 = [FP + -232];;
	R3 = [FP + -228];;
	call SAT_ADD;;
	R1 = [FP + -34] (B);;
	R2 = [FP + -28] (B);;
	[FP + -276] = R0;;
	R0 = R2;;
	R2 = [FP + -232];;
	R3 = [FP + -228];;
	call SAT_ADD;;
	R1 = [FP + -276];;
	[FP + -280] = R0;;
	R0 = R1;;
	R1 = [FP + -280];;
	R2 = [FP + -232];;
	R3 = [FP + -228];;
	call SAT_ADD;;
	R0 = R0 >> 1 (U);;
	J0 = [FP + -236];;
	[J0 + 2] = R0 (H);;
	R1 = [FP + -36] (B);;
	R0 = [FP + -30] (B);;
	R2 = [FP + -244];;
	R0 = R2 - R0 (T);;
	R2 = [FP + -232];;
	R3 = [FP + -228];;
	call SAT_ADD;;
	R1 = [FP + -35] (B);;
	R2 = [FP + -29] (B);;
	R3 = [FP + -244];;
	R1 = R3 - R1 (T);;
	R2 = R3 - R2 (T);;
	[FP + -284] = R0;;
	R0 = R2;;
	R2 = [FP + -232];;
	R3 = [FP + -228];;
	call SAT_ADD;;
	R1 = [FP + -284];;
	[FP + -288] = R0;;
	R0 = R1;;
	R1 = [FP + -288];;
	R2 = [FP + -232];;
	R3 = [FP + -228];;
	call SAT_ADD;;
	R1 = [FP + -34] (B);;
	R2 = [FP + -28] (B);;
	[FP + -292] = R0;;
	R0 = R2;;
	R2 = [FP + -232];;
	R3 = [FP + -228];;
	call SAT_ADD;;
	R1 = [FP + -292];;
	[FP + -296] = R0;;
	R0 = R1;;
	R1 = [FP + -296];;
	R2 = [FP + -232];;
	R3 = [FP + -228];;
	call SAT_ADD;;
	R0 = R0 >> 1 (U);;
	J0 = [FP + -236];;
	[J0 + 4] = R0 (H);;
	R0 = [FP + -36] (B);;
	R1 = [FP + -30] (B);;
	R2 = [FP + -244];;
	R0 = R2 - R0 (T);;
	[FP + -300] = R0;;
	R0 = R1;;
	R1 = [FP + -300];;
	R2 = [FP + -232];;
	R3 = [FP + -228];;
	call SAT_ADD;;
	R1 = [FP + -35] (B);;
	R2 = [FP + -29] (B);;
	R3 = [FP + -244];;
	R1 = R3 - R1 (T);;
	R2 = R3 - R2 (T);;
	[FP + -304] = R0;;
	R0 = R2;;
	R2 = [FP + -232];;
	R3 = [FP + -228];;
	call SAT_ADD;;
	R1 = [FP + -304];;
	[FP + -308] = R0;;
	R0 = R1;;
	R1 = [FP + -308];;
	R2 = [FP + -232];;
	R3 = [FP + -228];;
	call SAT_ADD;;
	R1 = [FP + -34] (B);;
	R2 = [FP + -28] (B);;
	[FP + -312] = R0;;
	R0 = R2;;
	R2 = [FP + -232];;
	R3 = [FP + -228];;
	call SAT_ADD;;
	R1 = [FP + -312];;
	[FP + -316] = R0;;
	R0 = R1;;
	R1 = [FP + -316];;
	R2 = [FP + -232];;
	R3 = [FP + -228];;
	call SAT_ADD;;
	R0 = R0 >> 1 (U);;
	J0 = [FP + -236];;
	[J0 + 6] = R0 (H);;
	R0 = [FP + -36] (B);;
	R1 = [FP + -30] (B);;
	R2 = [FP + -244];;
	R0 = R2 - R0 (T);;
	[FP + -320] = R0;;
	R0 = R1;;
	R1 = [FP + -320];;
	R2 = [FP + -232];;
	R3 = [FP + -228];;
	call SAT_ADD;;
	R1 = [FP + -29] (B);;
	R2 = [FP + -35] (B);;
	R3 = [FP + -244];;
	R1 = R3 - R1 (T);;
	[FP + -324] = R0;;
	R0 = R1;;
	R1 = R2;;
	R2 = [FP + -232];;
	R3 = [FP + -228];;
	call SAT_ADD;;
	R1 = [FP + -324];;
	[FP + -328] = R0;;
	R0 = R1;;
	R1 = [FP + -328];;
	R2 = [FP + -232];;
	R3 = [FP + -228];;
	call SAT_ADD;;
	R1 = [FP + -34] (B);;
	R2 = [FP + -28] (B);;
	R3 = [FP + -244];;
	R1 = R3 - R1 (T);;
	R2 = R3 - R2 (T);;
	[FP + -332] = R0;;
	R0 = R2;;
	R2 = [FP + -232];;
	R3 = [FP + -228];;
	call SAT_ADD;;
	R1 = [FP + -332];;
	[FP + -336] = R0;;
	R0 = R1;;
	R1 = [FP + -336];;
	R2 = [FP + -232];;
	R3 = [FP + -228];;
	call SAT_ADD;;
	R0 = R0 >> 1 (U);;
	J0 = [FP + -236];;
	[J0 + 8] = R0 (H);;
	R1 = [FP + -36] (B);;
	R0 = [FP + -30] (B);;
	R2 = [FP + -244];;
	R0 = R2 - R0 (T);;
	R2 = [FP + -232];;
	R3 = [FP + -228];;
	call SAT_ADD;;
	R1 = [FP + -29] (B);;
	R2 = [FP + -35] (B);;
	R3 = [FP + -244];;
	R1 = R3 - R1 (T);;
	[FP + -340] = R0;;
	R0 = R1;;
	R1 = R2;;
	R2 = [FP + -232];;
	R3 = [FP + -228];;
	call SAT_ADD;;
	R1 = [FP + -340];;
	[FP + -344] = R0;;
	R0 = R1;;
	R1 = [FP + -344];;
	R2 = [FP + -232];;
	R3 = [FP + -228];;
	call SAT_ADD;;
	R1 = [FP + -34] (B);;
	R2 = [FP + -28] (B);;
	R3 = [FP + -244];;
	R1 = R3 - R1 (T);;
	R2 = R3 - R2 (T);;
	[FP + -348] = R0;;
	R0 = R2;;
	R2 = [FP + -232];;
	R3 = [FP + -228];;
	call SAT_ADD;;
	R1 = [FP + -348];;
	[FP + -352] = R0;;
	R0 = R1;;
	R1 = [FP + -352];;
	R2 = [FP + -232];;
	R3 = [FP + -228];;
	call SAT_ADD;;
	R0 = R0 >> 1 (U);;
	J0 = [FP + -236];;
	[J0 + 10] = R0 (H);;
	R0 = [FP + -36] (B);;
	R1 = [FP + -30] (B);;
	R2 = [FP + -244];;
	R0 = R2 - R0 (T);;
	R1 = R2 - R1 (T);;
	[FP + -356] = R0;;
	R0 = R1;;
	R1 = [FP + -356];;
	R2 = [FP + -232];;
	R3 = [FP + -228];;
	call SAT_ADD;;
	R1 = [FP + -35] (B);;
	R2 = [FP + -29] (B);;
	R3 = [FP + -244];;
	R1 = R3 - R1 (T);;
	[FP + -360] = R0;;
	R0 = R2;;
	R2 = [FP + -232];;
	R3 = [FP + -228];;
	call SAT_ADD;;
	R1 = [FP + -360];;
	[FP + -364] = R0;;
	R0 = R1;;
	R1 = [FP + -364];;
	R2 = [FP + -232];;
	R3 = [FP + -228];;
	call SAT_ADD;;
	R1 = [FP + -34] (B);;
	R2 = [FP + -28] (B);;
	R3 = [FP + -244];;
	R1 = R3 - R1 (T);;
	R2 = R3 - R2 (T);;
	[FP + -368] = R0;;
	R0 = R2;;
	R2 = [FP + -232];;
	R3 = [FP + -228];;
	call SAT_ADD;;
	R1 = [FP + -368];;
	[FP + -372] = R0;;
	R0 = R1;;
	R1 = [FP + -372];;
	R2 = [FP + -232];;
	R3 = [FP + -228];;
	call SAT_ADD;;
	R0 = R0 >> 1 (U);;
	J0 = [FP + -236];;
	[J0 + 12] = R0 (H);;
	R1 = [FP + -36] (B);;
	R0 = [FP + -30] (B);;
	R2 = [FP + -232];;
	R3 = [FP + -228];;
	call SAT_ADD;;
	R1 = [FP + -35] (B);;
	R2 = [FP + -29] (B);;
	R3 = [FP + -244];;
	R1 = R3 - R1 (T);;
	[FP + -376] = R0;;
	R0 = R2;;
	R2 = [FP + -232];;
	R3 = [FP + -228];;
	call SAT_ADD;;
	R1 = [FP + -376];;
	[FP + -380] = R0;;
	R0 = R1;;
	R1 = [FP + -380];;
	R2 = [FP + -232];;
	R3 = [FP + -228];;
	call SAT_ADD;;
	R1 = [FP + -34] (B);;
	R2 = [FP + -28] (B);;
	R3 = [FP + -244];;
	R1 = R3 - R1 (T);;
	R2 = R3 - R2 (T);;
	[FP + -384] = R0;;
	R0 = R2;;
	R2 = [FP + -232];;
	R3 = [FP + -228];;
	call SAT_ADD;;
	R1 = [FP + -384];;
	[FP + -388] = R0;;
	R0 = R1;;
	R1 = [FP + -388];;
	R2 = [FP + -232];;
	R3 = [FP + -228];;
	call SAT_ADD;;
	R0 = R0 >> 1 (U);;
	J0 = [FP + -236];;
	[J0 + 0] = R0 (H);;
	J1 = [FP + -248];;
	R0 = J1;;
	R1 = J0;;
	call max_8to1;;
	R0 = [FP + -244];;
	[FP + -40] = R0;;
LBB2_5:                                 # =>This Inner Loop Header: Depth=1
	R0 = [FP + -40];;
	R1 = 7;;
	R0 = R0 > R1;;
	if R0, jump LBB2_8;;
	jump LBB2_6;;
LBB2_6:                                 #   in Loop: Header=BB2_5 Depth=1
	R0 = [FP + -40];;
	J0 = beta_initial_value_cb2;;
	R1 = [FP + -44];;
	R0 = R0 << 1;;
	R2 = J0;;
	R0 = R2 + R0 (T);;
	J0 = R0;;
	R2 = [J0 + 0] (H)(U);;
	R1 = R2 - R1 (T);;
	J0 = R0;;
	[J0 + 0] = R1 (H);;
# BB#7:                                 #   in Loop: Header=BB2_5 Depth=1
	R0 = [FP + -40];;
	R1 = 1;;
	R0 = R0 + R1 (T);;
	[FP + -40] = R0;;
	jump LBB2_5;;
LBB2_8:
	SP = FP;;
	J30 = [SP + -8];;
	FP = [SP + -4];;
	jump J30;;
Ltmp2:
	.size	compute_beta_init, Ltmp2-compute_beta_init

	.global	bit2byte
	.type	bit2byte,@function
bit2byte:                               # @bit2byte
# BB#0:
	[SP + -4] = FP;;
	[SP + -8] = J30;;
	FP = SP;;
	J31 = -40;;
	SP = SP + J31;;
	[FP + -12] = R0;;
	[FP + -16] = R1;;
	R3 = 0;;
	[FP + -20] = R2;;
	R4 = 1;;
	[FP + -25] = R3 (B);;
	[FP + -24] = R4;;
	[FP + -32] = R0;;
	[FP + -36] = R2;;
	[FP + -40] = R1;;
LBB3_1:                                 # =>This Inner Loop Header: Depth=1
	R0 = [FP + -20];;
	R1 = [FP + -24];;
	R0 = R1 > R0 (U);;
	if R0, jump LBB3_7;;
	jump LBB3_2;;
LBB3_2:                                 #   in Loop: Header=BB3_1 Depth=1
	R0 = [FP + -24] (B)(U);;
	R1 = 7;;
	R2 = 0;;
	R0 = R0 & R1;;
	R0 = R0 == R2;;
	if R0, jump LBB3_4;;
	jump LBB3_3;;
LBB3_3:                                 #   in Loop: Header=BB3_1 Depth=1
	R0 = [FP + -16];;
	R1 = [FP + -24];;
	R2 = 7;;
	R3 = 8;;
	R4 = 1;;
	R5 = [FP + -25] (B)(U);;
	R0 = R0 + R1 (T);;
	R1 = R1 & R2;;
	J0 = R0;;
	R0 = [J0 + -1] (B)(U);;
	R1 = R3 - R1 (T);;
	R0 = R0 & R4;;
	R0 = R0 << R1;;
	R0 = R5 | R0;;
	[FP + -25] = R0 (B);;
	jump LBB3_5;;
LBB3_4:                                 #   in Loop: Header=BB3_1 Depth=1
	R0 = [FP + -24];;
	R1 = [FP + -16];;
	R2 = [FP + -12];;
	R3 = [FP + -25] (B)(U);;
	R4 = 1;;
	R5 = R0 >> 3 (U);;
	R0 = R1 + R0 (T);;
	R1 = 0;;
	J0 = R0;;
	R0 = [J0 + -1] (B)(U);;
	R2 = R2 + R5 (T);;
	R0 = R0 & R4;;
	R0 = R3 | R0;;
	J0 = R2;;
	[J0 + -1] = R0 (B);;
	[FP + -25] = R1 (B);;
LBB3_5:                                 #   in Loop: Header=BB3_1 Depth=1
# BB#6:                                 #   in Loop: Header=BB3_1 Depth=1
	R0 = [FP + -24];;
	R1 = 1;;
	R0 = R0 + R1 (T);;
	[FP + -24] = R0;;
	jump LBB3_1;;
LBB3_7:
	SP = FP;;
	J30 = [SP + -8];;
	FP = [SP + -4];;
	jump J30;;
Ltmp3:
	.size	bit2byte, Ltmp3-bit2byte

	.global	main
	.type	main,@function
main:                                   # @main
# BB#0:
	[SP + -4] = FP;;
	[SP + -8] = J30;;
	FP = SP;;
	J31 = -192;;
	SP = SP + J31;;
	R0 = 0;;
	R1 = -2147483648;;
	[FP + -12] = R0;;
	[FP + -52] = R1;;
	[FP + -56] = R1;;
	R0 = -2147483644;;
	J0 = sys;;
	J1 = par1;;
	R1 = J0;;
	J2 = R0;;
	[J2 + 0] = R1;;
	R0 = [FP + -56];;
	J2 = par2;;
	R1 = 9;;
	R2 = 40;;
	R3 = 1;;
	R4 = J1;;
	J3 = R0;;
	[J3 + 8] = R4;;
	R0 = [FP + -56];;
	R4 = 12582912;;
	R5 = 24;;
	R6 = 512;;
	R7 = J2;;
	J3 = R0;;
	[J3 + 12] = R7;;
	[FP + -16] = R2;;
	[FP + -20] = R1;;
	[FP + -44] = R3;;
	[FP + -48] = R3;;
	[FP + -24] = R3;;
	R0 = [FP + -16];;
	[FP + -28] = R0;;
	[FP + -32] = R3;;
	[FP + -40] = R2;;
	[FP + -36] = R2;;
	R3 = [FP + -16];;
	R0 = J0;;
	R1 = J1;;
	R2 = J2;;
	[FP + -84] = R6;;
	[FP + -88] = R5;;
	[FP + -92] = R4;;
	call compute_beta_init;;
	R0 = [FP + -56];;
	J0 = R0;;
	R0 = [FP + -92];;
	[J0 + 16] = R0;;
	R0 = [FP + -44];;
	R1 = [FP + -48];;
	R2 = [FP + -20];;
	R3 = [FP + -56];;
	R0 = R0 << 16;;
	R1 = R1 << 8;;
	R0 = R0 | R1;;
	R0 = R0 | R2;;
	J0 = R3;;
	[J0 + 20] = R0;;
	R0 = [FP + -24];;
	R1 = [FP + -28];;
	R2 = [FP + -16];;
	R3 = [FP + -56];;
	R0 = R0 << 28;;
	R1 = R1 << 16;;
	R0 = R0 | R1;;
	R0 = R0 | R2;;
	J0 = R3;;
	[J0 + 24] = R0;;
	R0 = [FP + -32];;
	R1 = [FP + -36];;
	R2 = [FP + -40];;
	R3 = [FP + -56];;
	R0 = R0 << 16;;
	R1 = R1 << 8;;
	R0 = R0 | R1;;
	R0 = R0 | R2;;
	J0 = R3;;
	[J0 + 28] = R0;;
	R0 = [FP + -56];;
	J0 = R0;;
	R0 = [FP + -88];;
	[J0 + 32] = R0;;
	R0 = [FP + -16];;
	R1 = [FP + -84];;
	R0 = R0 > R1;;
	if R0, jump LBB4_2;;
	jump LBB4_1;;
LBB4_1:
	R0 = [FP + -16];;
	R1 = -40;;
	R0 = R0 + R1 (T);;
	R1 = R0 >> 31;;
	R1 = R1 >> 29 (U);;
	R0 = R0 + R1 (T);;
	R0 = R0 >> 3;;
	[FP + -96] = R0;;
	jump LBB4_9;;
LBB4_2:
	R0 = [FP + -16];;
	R1 = 1024;;
	R0 = R0 > R1;;
	if R0, jump LBB4_4;;
	jump LBB4_3;;
LBB4_3:
	R0 = [FP + -16];;
	R1 = -512;;
	R2 = 59;;
	R0 = R0 + R1 (T);;
	R1 = R0 >> 31;;
	R1 = R1 >> 28 (U);;
	R0 = R0 + R1 (T);;
	R0 = R0 >> 4;;
	R0 = R0 + R2 (T);;
	[FP + -100] = R0;;
	jump LBB4_8;;
LBB4_4:
	R0 = [FP + -16];;
	R1 = 2048;;
	R0 = R0 > R1;;
	if R0, jump LBB4_6;;
	jump LBB4_5;;
LBB4_5:
	R0 = [FP + -16];;
	R1 = -1024;;
	R2 = 91;;
	R0 = R0 + R1 (T);;
	R1 = R0 >> 31;;
	R1 = R1 >> 27 (U);;
	R0 = R0 + R1 (T);;
	R0 = R0 >> 5;;
	R0 = R0 + R2 (T);;
	[FP + -104] = R0;;
	jump LBB4_7;;
LBB4_6:
	R0 = [FP + -16];;
	R1 = -2048;;
	R2 = 123;;
	R0 = R0 + R1 (T);;
	R1 = R0 >> 31;;
	R1 = R1 >> 26 (U);;
	R0 = R0 + R1 (T);;
	R0 = R0 >> 6;;
	R0 = R0 + R2 (T);;
	[FP + -104] = R0;;
LBB4_7:
	R0 = [FP + -104];;
	[FP + -100] = R0;;
LBB4_8:
	R0 = [FP + -100];;
	[FP + -96] = R0;;
LBB4_9:
	R0 = [FP + -96];;
	R0 = R0 << 2;;
	J0 = e1_e2_f2;;
	R1 = [FP + -56];;
	R2 = J0;;
	R0 = R2 + R0 (T);;
	J0 = beta_initial_value_cb1;;
	J1 = R0;;
	R0 = [J1 + 0];;
	J1 = beta_initial_value_cb2;;
	R2 = 512;;
	J2 = R1;;
	[J2 + 36] = R0;;
	R0 = [J0 + 2] (H)(U);;
	R1 = [J0 + 0] (H);;
	R3 = [FP + -56];;
	R0 = R0 << 16;;
	R0 = R0 | R1;;
	J2 = R3;;
	[J2 + 40] = R0;;
	R0 = [J0 + 6] (H)(U);;
	R1 = [J0 + 4] (H);;
	R3 = [FP + -56];;
	R0 = R0 << 16;;
	R0 = R0 | R1;;
	J2 = R3;;
	[J2 + 44] = R0;;
	R0 = [J0 + 10] (H)(U);;
	R1 = [J0 + 8] (H);;
	R3 = [FP + -56];;
	R0 = R0 << 16;;
	R0 = R0 | R1;;
	J2 = R3;;
	[J2 + 48] = R0;;
	R0 = [J0 + 14] (H)(U);;
	R1 = [J0 + 12] (H);;
	R3 = [FP + -56];;
	R0 = R0 << 16;;
	R0 = R0 | R1;;
	J0 = R3;;
	[J0 + 52] = R0;;
	R0 = [J1 + 2] (H)(U);;
	R1 = [J1 + 0] (H);;
	R3 = [FP + -56];;
	R0 = R0 << 16;;
	R0 = R0 | R1;;
	J0 = R3;;
	[J0 + 56] = R0;;
	R0 = [J1 + 6] (H)(U);;
	R1 = [J1 + 4] (H);;
	R3 = [FP + -56];;
	R0 = R0 << 16;;
	R0 = R0 | R1;;
	J0 = R3;;
	[J0 + 60] = R0;;
	R0 = [J1 + 10] (H)(U);;
	R1 = [J1 + 8] (H);;
	R3 = [FP + -56];;
	R0 = R0 << 16;;
	R0 = R0 | R1;;
	J0 = R3;;
	[J0 + 64] = R0;;
	R0 = [J1 + 14] (H)(U);;
	R1 = [J1 + 12] (H);;
	R3 = [FP + -56];;
	R0 = R0 << 16;;
	R0 = R0 | R1;;
	J0 = R3;;
	[J0 + 68] = R0;;
	R0 = [FP + -16];;
	R0 = R0 > R2;;
	if R0, jump LBB4_11;;
	jump LBB4_10;;
LBB4_10:
	R0 = [FP + -16];;
	R1 = -40;;
	R0 = R0 + R1 (T);;
	R1 = R0 >> 31;;
	R1 = R1 >> 29 (U);;
	R0 = R0 + R1 (T);;
	R0 = R0 >> 3;;
	[FP + -108] = R0;;
	jump LBB4_18;;
LBB4_11:
	R0 = [FP + -16];;
	R1 = 1024;;
	R0 = R0 > R1;;
	if R0, jump LBB4_13;;
	jump LBB4_12;;
LBB4_12:
	R0 = [FP + -16];;
	R1 = -512;;
	R2 = 59;;
	R0 = R0 + R1 (T);;
	R1 = R0 >> 31;;
	R1 = R1 >> 28 (U);;
	R0 = R0 + R1 (T);;
	R0 = R0 >> 4;;
	R0 = R0 + R2 (T);;
	[FP + -112] = R0;;
	jump LBB4_17;;
LBB4_13:
	R0 = [FP + -16];;
	R1 = 2048;;
	R0 = R0 > R1;;
	if R0, jump LBB4_15;;
	jump LBB4_14;;
LBB4_14:
	R0 = [FP + -16];;
	R1 = -1024;;
	R2 = 91;;
	R0 = R0 + R1 (T);;
	R1 = R0 >> 31;;
	R1 = R1 >> 27 (U);;
	R0 = R0 + R1 (T);;
	R0 = R0 >> 5;;
	R0 = R0 + R2 (T);;
	[FP + -116] = R0;;
	jump LBB4_16;;
LBB4_15:
	R0 = [FP + -16];;
	R1 = -2048;;
	R2 = 123;;
	R0 = R0 + R1 (T);;
	R1 = R0 >> 31;;
	R1 = R1 >> 26 (U);;
	R0 = R0 + R1 (T);;
	R0 = R0 >> 6;;
	R0 = R0 + R2 (T);;
	[FP + -116] = R0;;
LBB4_16:
	R0 = [FP + -116];;
	[FP + -112] = R0;;
LBB4_17:
	R0 = [FP + -112];;
	[FP + -108] = R0;;
LBB4_18:
	R0 = [FP + -108];;
	R0 = R0 << 4;;
	J0 = as_ts_gi;;
	R1 = [FP + -56];;
	R2 = J0;;
	R0 = R2 + R0 (T);;
	R2 = 512;;
	J0 = R0;;
	R0 = [J0 + 0];;
	J0 = R1;;
	[J0 + 72] = R0;;
	R0 = [FP + -16];;
	R0 = R0 > R2;;
	if R0, jump LBB4_20;;
	jump LBB4_19;;
LBB4_19:
	R0 = [FP + -16];;
	R1 = -40;;
	R0 = R0 + R1 (T);;
	R1 = R0 >> 31;;
	R1 = R1 >> 29 (U);;
	R0 = R0 + R1 (T);;
	R0 = R0 >> 3;;
	[FP + -120] = R0;;
	jump LBB4_27;;
LBB4_20:
	R0 = [FP + -16];;
	R1 = 1024;;
	R0 = R0 > R1;;
	if R0, jump LBB4_22;;
	jump LBB4_21;;
LBB4_21:
	R0 = [FP + -16];;
	R1 = -512;;
	R2 = 59;;
	R0 = R0 + R1 (T);;
	R1 = R0 >> 31;;
	R1 = R1 >> 28 (U);;
	R0 = R0 + R1 (T);;
	R0 = R0 >> 4;;
	R0 = R0 + R2 (T);;
	[FP + -124] = R0;;
	jump LBB4_26;;
LBB4_22:
	R0 = [FP + -16];;
	R1 = 2048;;
	R0 = R0 > R1;;
	if R0, jump LBB4_24;;
	jump LBB4_23;;
LBB4_23:
	R0 = [FP + -16];;
	R1 = -1024;;
	R2 = 91;;
	R0 = R0 + R1 (T);;
	R1 = R0 >> 31;;
	R1 = R1 >> 27 (U);;
	R0 = R0 + R1 (T);;
	R0 = R0 >> 5;;
	R0 = R0 + R2 (T);;
	[FP + -128] = R0;;
	jump LBB4_25;;
LBB4_24:
	R0 = [FP + -16];;
	R1 = -2048;;
	R2 = 123;;
	R0 = R0 + R1 (T);;
	R1 = R0 >> 31;;
	R1 = R1 >> 26 (U);;
	R0 = R0 + R1 (T);;
	R0 = R0 >> 6;;
	R0 = R0 + R2 (T);;
	[FP + -128] = R0;;
LBB4_25:
	R0 = [FP + -128];;
	[FP + -124] = R0;;
LBB4_26:
	R0 = [FP + -124];;
	[FP + -120] = R0;;
LBB4_27:
	R0 = [FP + -120];;
	R0 = R0 << 4;;
	J0 = as_ts_gi;;
	R1 = [FP + -56];;
	R2 = J0;;
	R0 = R2 + R0 (T);;
	R2 = 512;;
	J0 = R0;;
	R0 = [J0 + 4];;
	J0 = R1;;
	[J0 + 76] = R0;;
	R0 = [FP + -16];;
	R0 = R0 > R2;;
	if R0, jump LBB4_29;;
	jump LBB4_28;;
LBB4_28:
	R0 = [FP + -16];;
	R1 = -40;;
	R0 = R0 + R1 (T);;
	R1 = R0 >> 31;;
	R1 = R1 >> 29 (U);;
	R0 = R0 + R1 (T);;
	R0 = R0 >> 3;;
	[FP + -132] = R0;;
	jump LBB4_36;;
LBB4_29:
	R0 = [FP + -16];;
	R1 = 1024;;
	R0 = R0 > R1;;
	if R0, jump LBB4_31;;
	jump LBB4_30;;
LBB4_30:
	R0 = [FP + -16];;
	R1 = -512;;
	R2 = 59;;
	R0 = R0 + R1 (T);;
	R1 = R0 >> 31;;
	R1 = R1 >> 28 (U);;
	R0 = R0 + R1 (T);;
	R0 = R0 >> 4;;
	R0 = R0 + R2 (T);;
	[FP + -136] = R0;;
	jump LBB4_35;;
LBB4_31:
	R0 = [FP + -16];;
	R1 = 2048;;
	R0 = R0 > R1;;
	if R0, jump LBB4_33;;
	jump LBB4_32;;
LBB4_32:
	R0 = [FP + -16];;
	R1 = -1024;;
	R2 = 91;;
	R0 = R0 + R1 (T);;
	R1 = R0 >> 31;;
	R1 = R1 >> 27 (U);;
	R0 = R0 + R1 (T);;
	R0 = R0 >> 5;;
	R0 = R0 + R2 (T);;
	[FP + -140] = R0;;
	jump LBB4_34;;
LBB4_33:
	R0 = [FP + -16];;
	R1 = -2048;;
	R2 = 123;;
	R0 = R0 + R1 (T);;
	R1 = R0 >> 31;;
	R1 = R1 >> 26 (U);;
	R0 = R0 + R1 (T);;
	R0 = R0 >> 6;;
	R0 = R0 + R2 (T);;
	[FP + -140] = R0;;
LBB4_34:
	R0 = [FP + -140];;
	[FP + -136] = R0;;
LBB4_35:
	R0 = [FP + -136];;
	[FP + -132] = R0;;
LBB4_36:
	R0 = [FP + -132];;
	R0 = R0 << 4;;
	J0 = as_ts_gi;;
	R1 = [FP + -56];;
	R2 = J0;;
	R0 = R2 + R0 (T);;
	R2 = 512;;
	J0 = R0;;
	R0 = [J0 + 8];;
	J0 = R1;;
	[J0 + 80] = R0;;
	R0 = [FP + -16];;
	R0 = R0 > R2;;
	if R0, jump LBB4_38;;
	jump LBB4_37;;
LBB4_37:
	R0 = [FP + -16];;
	R1 = -40;;
	R0 = R0 + R1 (T);;
	R1 = R0 >> 31;;
	R1 = R1 >> 29 (U);;
	R0 = R0 + R1 (T);;
	R0 = R0 >> 3;;
	[FP + -144] = R0;;
	jump LBB4_45;;
LBB4_38:
	R0 = [FP + -16];;
	R1 = 1024;;
	R0 = R0 > R1;;
	if R0, jump LBB4_40;;
	jump LBB4_39;;
LBB4_39:
	R0 = [FP + -16];;
	R1 = -512;;
	R2 = 59;;
	R0 = R0 + R1 (T);;
	R1 = R0 >> 31;;
	R1 = R1 >> 28 (U);;
	R0 = R0 + R1 (T);;
	R0 = R0 >> 4;;
	R0 = R0 + R2 (T);;
	[FP + -148] = R0;;
	jump LBB4_44;;
LBB4_40:
	R0 = [FP + -16];;
	R1 = 2048;;
	R0 = R0 > R1;;
	if R0, jump LBB4_42;;
	jump LBB4_41;;
LBB4_41:
	R0 = [FP + -16];;
	R1 = -1024;;
	R2 = 91;;
	R0 = R0 + R1 (T);;
	R1 = R0 >> 31;;
	R1 = R1 >> 27 (U);;
	R0 = R0 + R1 (T);;
	R0 = R0 >> 5;;
	R0 = R0 + R2 (T);;
	[FP + -152] = R0;;
	jump LBB4_43;;
LBB4_42:
	R0 = [FP + -16];;
	R1 = -2048;;
	R2 = 123;;
	R0 = R0 + R1 (T);;
	R1 = R0 >> 31;;
	R1 = R1 >> 26 (U);;
	R0 = R0 + R1 (T);;
	R0 = R0 >> 6;;
	R0 = R0 + R2 (T);;
	[FP + -152] = R0;;
LBB4_43:
	R0 = [FP + -152];;
	[FP + -148] = R0;;
LBB4_44:
	R0 = [FP + -148];;
	[FP + -144] = R0;;
LBB4_45:
	R0 = [FP + -144];;
	R0 = R0 << 4;;
	J0 = as_ts_gi;;
	R1 = [FP + -56];;
	R2 = J0;;
	R0 = R2 + R0 (T);;
	R2 = 1;;
	J0 = R0;;
	R0 = [J0 + 12];;
	J0 = R1;;
	[J0 + 84] = R0;;
	R0 = [FP + -56];;
	J0 = R0;;
	[J0 + 0] = R2;;
LBB4_46:                                # =>This Inner Loop Header: Depth=1
	R0 = [FP + -56];;
	R1 = 0;;
	J0 = R0;;
	R0 = [J0 + 0];;
	R0 = R0 >> 31 (U);;
	R0 = R0 == R1;;
	if R0, jump LBB4_48;;
	jump LBB4_47;;
LBB4_47:                                #   in Loop: Header=BB4_46 Depth=1
	jump LBB4_46;;
LBB4_48:
	R0 = 12582912;;
	J31 = -69;;
	J0 = FP + J31;;
	[FP + -60] = R0;;
	[FP + -64] = R0;;
	R2 = [FP + -16];;
	J1 = out;;
	R0 = 0;;
	[FP + -156] = R0;;
	R0 = J0;;
	R1 = J1;;
	call bit2byte;;
	R0 = [FP + -156];;
	[FP + -76] = R0;;
	[FP + -80] = R0;;
LBB4_49:                                # =>This Inner Loop Header: Depth=1
	R0 = [FP + -16];;
	R1 = [FP + -80];;
	R2 = R0 >> 31;;
	R2 = R2 >> 29 (U);;
	R0 = R0 + R2 (T);;
	R0 = R0 >> 3;;
	R0 = R1 >= R0 (U);;
	if R0, jump LBB4_55;;
	jump LBB4_50;;
LBB4_50:                                #   in Loop: Header=BB4_49 Depth=1
	R0 = [FP + -64];;
	J31 = -69;;
	J0 = FP + J31;;
	R1 = [FP + -80];;
	R0 = R0 + R1 (T);;
	J1 = R0;;
	R0 = [J1 + 0] (B)(U);;
	R1 = [FP + -80];;
	R2 = J0;;
	R1 = R2 + R1 (T);;
	J0 = R1;;
	R1 = [J0 + 0] (B)(U);;
	R0 = R0 == R1;;
	if R0, jump LBB4_52;;
	jump LBB4_51;;
LBB4_51:                                #   in Loop: Header=BB4_49 Depth=1
	R0 = [FP + -64];;
	J31 = -69;;
	J0 = FP + J31;;
	R1 = [FP + -80];;
	J1 = L.str;;
	R0 = R0 + R1 (T);;
	J2 = R0;;
	R0 = [J2 + 0] (B)(U);;
	R2 = [FP + -80];;
	R3 = J0;;
	R2 = R3 + R2 (T);;
	J0 = R2;;
	R2 = [J0 + 0] (B)(U);;
	[SP + 4] = R0;;
	[SP + 0] = R1;;
	[SP + 8] = R2;;
	R0 = J1;;
	call printf;;
	[FP + -160] = R0;;
	jump LBB4_53;;
LBB4_52:                                #   in Loop: Header=BB4_49 Depth=1
	R0 = [FP + -76];;
	R1 = 1;;
	R0 = R0 + R1 (T);;
	[FP + -76] = R0;;
LBB4_53:                                #   in Loop: Header=BB4_49 Depth=1
# BB#54:                                #   in Loop: Header=BB4_49 Depth=1
	R0 = [FP + -80];;
	R1 = 1;;
	R0 = R0 + R1 (T);;
	[FP + -80] = R0;;
	jump LBB4_49;;
LBB4_55:
	R0 = [FP + -16];;
	R1 = [FP + -76];;
	R2 = R0 >> 31;;
	R2 = R2 >> 29 (U);;
	R0 = R0 + R2 (T);;
	R0 = R0 >> 3;;
	R0 = R1 != R0;;
	if R0, jump LBB4_57;;
	jump LBB4_56;;
LBB4_56:
	J0 = L.str1;;
	R0 = J0;;
	call printf;;
	[FP + -164] = R0;;
LBB4_57:
	R0 = [FP + -56];;
	R1 = 1;;
	R2 = 255;;
	J0 = L.str2;;
	R3 = 0;;
	J1 = R0;;
	R0 = [J1 + 0];;
	R4 = [FP + -56];;
	R0 = R0 >> 8 (U);;
	J1 = R4;;
	R4 = [J1 + 0];;
	R0 = R0 & R1;;
	[SP + 0] = R0;;
	R0 = R4 & R2;;
	[SP + 4] = R0;;
	R0 = J0;;
	[FP + -168] = R3;;
	call printf;;
	R1 = [FP + -168];;
	[FP + -172] = R0;;
	R0 = R1;;
	SP = FP;;
	J30 = [SP + -8];;
	FP = [SP + -4];;
	jump J30;;
Ltmp4:
	.size	main, Ltmp4-main

	.type	sys,@object             # @sys
	.section	.SDA0.DM0,"a",@progbits
	.global	sys
sys:
	.ascii	 "\374\000\377\376\376\000\b\001\000\374\003\375\000\377\377\003\002\004\375\002\377\001\377\001\000\375\377\004\377\373\000\002\005\002\000\002\375\371\001\002\376\002\000\003"
	.size	sys, 44

	.type	par1,@object            # @par1
	.global	par1
par1:
	.ascii	 "\377\375\003\000\000\002\001\002\002\000\001\001\377\003\004\001\376\376\002\376\002\377\000\000\002\001\377\376\000\002\n\003\005\002\377\001\376\375\375\375\005\375\000\002"
	.size	par1, 44

	.type	par2,@object            # @par2
	.global	par2
par2:
	.ascii	 "\373\001\003\000\372\001\374\376\003\004\001\003\003\376\375\000\000\002\374\374\001\003\375\002\375\377\002\001\375\373\000\374\002\001\372\004\004\377\002\377\374\000\003\003"
	.size	par2, 44

	.type	out,@object             # @out
	.data
	.global	out
out:
	.asciz	 "\001\000\001\001\001\001\000\000\001\001\000\001\000\001\001\000\000\000\001\000\001\001\001\000\000\001\000\000\001\001\000\000\000\000\001\000\001\001\000"
	.size	out, 40

	.type	as_ts_gi,@object        # @as_ts_gi
	.section	.rodata,"a",@progbits
	.global	as_ts_gi
	.align	4
as_ts_gi:
	.int	13                      # 0xd
	.int	13                      # 0xd
	.int	13                      # 0xd
	.int	13                      # 0xd
	.int	19                      # 0x13
	.int	19                      # 0x13
	.int	19                      # 0x13
	.int	19                      # 0x13
	.int	16777221                # 0x1000005
	.int	5                       # 0x5
	.int	5                       # 0x5
	.int	5                       # 0x5
	.int	23                      # 0x17
	.int	23                      # 0x17
	.int	23                      # 0x17
	.int	23                      # 0x17
	.int	25                      # 0x19
	.int	25                      # 0x19
	.int	25                      # 0x19
	.int	25                      # 0x19
	.int	31                      # 0x1f
	.int	31                      # 0x1f
	.int	31                      # 0x1f
	.int	31                      # 0x1f
	.int	27                      # 0x1b
	.int	27                      # 0x1b
	.int	27                      # 0x1b
	.int	27                      # 0x1b
	.int	35                      # 0x23
	.int	35                      # 0x23
	.int	35                      # 0x23
	.int	35                      # 0x23
	.int	33                      # 0x21
	.int	33                      # 0x21
	.int	33                      # 0x21
	.int	33                      # 0x21
	.int	16777229                # 0x100000d
	.int	13                      # 0xd
	.int	13                      # 0xd
	.int	13                      # 0xd
	.int	16777289                # 0x1000049
	.int	73                      # 0x49
	.int	73                      # 0x49
	.int	73                      # 0x49
	.int	47                      # 0x2f
	.int	47                      # 0x2f
	.int	47                      # 0x2f
	.int	47                      # 0x2f
	.int	43                      # 0x2b
	.int	43                      # 0x2b
	.int	43                      # 0x2b
	.int	43                      # 0x2b
	.int	125                     # 0x7d
	.int	125                     # 0x7d
	.int	125                     # 0x7d
	.int	125                     # 0x7d
	.int	47                      # 0x2f
	.int	47                      # 0x2f
	.int	47                      # 0x2f
	.int	47                      # 0x2f
	.int	141                     # 0x8d
	.int	141                     # 0x8d
	.int	141                     # 0x8d
	.int	141                     # 0x8d
	.int	16777233                # 0x1000011
	.int	17                      # 0x11
	.int	17                      # 0x11
	.int	17                      # 0x11
	.int	65                      # 0x41
	.int	65                      # 0x41
	.int	65                      # 0x41
	.int	65                      # 0x41
	.int	103                     # 0x67
	.int	103                     # 0x67
	.int	103                     # 0x67
	.int	103                     # 0x67
	.int	71                      # 0x47
	.int	71                      # 0x47
	.int	71                      # 0x47
	.int	71                      # 0x47
	.int	63                      # 0x3f
	.int	63                      # 0x3f
	.int	63                      # 0x3f
	.int	63                      # 0x3f
	.int	79                      # 0x4f
	.int	79                      # 0x4f
	.int	79                      # 0x4f
	.int	79                      # 0x4f
	.int	47                      # 0x2f
	.int	47                      # 0x2f
	.int	47                      # 0x2f
	.int	47                      # 0x2f
	.int	83                      # 0x53
	.int	83                      # 0x53
	.int	83                      # 0x53
	.int	83                      # 0x53
	.int	143                     # 0x8f
	.int	143                     # 0x8f
	.int	143                     # 0x8f
	.int	143                     # 0x8f
	.int	89                      # 0x59
	.int	89                      # 0x59
	.int	89                      # 0x59
	.int	89                      # 0x59
	.int	95                      # 0x5f
	.int	95                      # 0x5f
	.int	95                      # 0x5f
	.int	95                      # 0x5f
	.int	47                      # 0x2f
	.int	47                      # 0x2f
	.int	47                      # 0x2f
	.int	47                      # 0x2f
	.int	215                     # 0xd7
	.int	215                     # 0xd7
	.int	215                     # 0xd7
	.int	215                     # 0xd7
	.int	101                     # 0x65
	.int	101                     # 0x65
	.int	101                     # 0x65
	.int	101                     # 0x65
	.int	16777249                # 0x1000021
	.int	33                      # 0x21
	.int	33                      # 0x21
	.int	33                      # 0x21
	.int	55                      # 0x37
	.int	55                      # 0x37
	.int	55                      # 0x37
	.int	55                      # 0x37
	.int	93                      # 0x5d
	.int	93                      # 0x5d
	.int	93                      # 0x5d
	.int	93                      # 0x5d
	.int	113                     # 0x71
	.int	113                     # 0x71
	.int	113                     # 0x71
	.int	113                     # 0x71
	.int	97                      # 0x61
	.int	97                      # 0x61
	.int	97                      # 0x61
	.int	97                      # 0x61
	.int	141                     # 0x8d
	.int	141                     # 0x8d
	.int	141                     # 0x8d
	.int	141                     # 0x8d
	.int	103                     # 0x67
	.int	103                     # 0x67
	.int	103                     # 0x67
	.int	103                     # 0x67
	.int	199                     # 0xc7
	.int	199                     # 0xc7
	.int	199                     # 0xc7
	.int	199                     # 0xc7
	.int	279                     # 0x117
	.int	279                     # 0x117
	.int	279                     # 0x117
	.int	279                     # 0x117
	.int	65                      # 0x41
	.int	65                      # 0x41
	.int	65                      # 0x41
	.int	65                      # 0x41
	.int	223                     # 0xdf
	.int	223                     # 0xdf
	.int	223                     # 0xdf
	.int	223                     # 0xdf
	.int	127                     # 0x7f
	.int	127                     # 0x7f
	.int	127                     # 0x7f
	.int	127                     # 0x7f
	.int	139                     # 0x8b
	.int	139                     # 0x8b
	.int	139                     # 0x8b
	.int	139                     # 0x8b
	.int	71                      # 0x47
	.int	71                      # 0x47
	.int	71                      # 0x47
	.int	71                      # 0x47
	.int	341                     # 0x155
	.int	341                     # 0x155
	.int	341                     # 0x155
	.int	341                     # 0x155
	.int	191                     # 0xbf
	.int	191                     # 0xbf
	.int	191                     # 0xbf
	.int	191                     # 0xbf
	.int	257                     # 0x101
	.int	257                     # 0x101
	.int	257                     # 0x101
	.int	257                     # 0x101
	.int	77                      # 0x4d
	.int	77                      # 0x4d
	.int	77                      # 0x4d
	.int	77                      # 0x4d
	.int	157                     # 0x9d
	.int	157                     # 0x9d
	.int	157                     # 0x9d
	.int	157                     # 0x9d
	.int	119                     # 0x77
	.int	119                     # 0x77
	.int	119                     # 0x77
	.int	119                     # 0x77
	.int	201                     # 0xc9
	.int	201                     # 0xc9
	.int	201                     # 0xc9
	.int	201                     # 0xc9
	.int	197                     # 0xc5
	.int	197                     # 0xc5
	.int	197                     # 0xc5
	.int	197                     # 0xc5
	.int	143                     # 0x8f
	.int	143                     # 0x8f
	.int	143                     # 0x8f
	.int	143                     # 0x8f
	.int	305                     # 0x131
	.int	305                     # 0x131
	.int	305                     # 0x131
	.int	305                     # 0x131
	.int	147                     # 0x93
	.int	147                     # 0x93
	.int	147                     # 0x93
	.int	147                     # 0x93
	.int	269                     # 0x10d
	.int	269                     # 0x10d
	.int	269                     # 0x10d
	.int	269                     # 0x10d
	.int	213                     # 0xd5
	.int	213                     # 0xd5
	.int	213                     # 0xd5
	.int	213                     # 0xd5
	.int	219                     # 0xdb
	.int	219                     # 0xdb
	.int	219                     # 0xdb
	.int	219                     # 0xdb
	.int	139                     # 0x8b
	.int	139                     # 0x8b
	.int	139                     # 0x8b
	.int	139                     # 0x8b
	.int	95                      # 0x5f
	.int	95                      # 0x5f
	.int	95                      # 0x5f
	.int	95                      # 0x5f
	.int	83                      # 0x53
	.int	83                      # 0x53
	.int	83                      # 0x53
	.int	83                      # 0x53
	.int	103                     # 0x67
	.int	103                     # 0x67
	.int	103                     # 0x67
	.int	103                     # 0x67
	.int	16777303                # 0x1000057
	.int	87                      # 0x57
	.int	87                      # 0x57
	.int	87                      # 0x57
	.int	161                     # 0xa1
	.int	161                     # 0xa1
	.int	161                     # 0xa1
	.int	161                     # 0xa1
	.int	93                      # 0x5d
	.int	93                      # 0x5d
	.int	93                      # 0x5d
	.int	93                      # 0x5d
	.int	113                     # 0x71
	.int	113                     # 0x71
	.int	113                     # 0x71
	.int	113                     # 0x71
	.int	275                     # 0x113
	.int	275                     # 0x113
	.int	275                     # 0x113
	.int	275                     # 0x113
	.int	119                     # 0x77
	.int	119                     # 0x77
	.int	119                     # 0x77
	.int	119                     # 0x77
	.int	267                     # 0x10b
	.int	267                     # 0x10b
	.int	267                     # 0x10b
	.int	267                     # 0x10b
	.int	295                     # 0x127
	.int	295                     # 0x127
	.int	295                     # 0x127
	.int	295                     # 0x127
	.int	107                     # 0x6b
	.int	107                     # 0x6b
	.int	107                     # 0x6b
	.int	107                     # 0x6b
	.int	199                     # 0xc7
	.int	199                     # 0xc7
	.int	199                     # 0xc7
	.int	199                     # 0xc7
	.int	199                     # 0xc7
	.int	199                     # 0xc7
	.int	199                     # 0xc7
	.int	199                     # 0xc7
	.int	231                     # 0xe7
	.int	231                     # 0xe7
	.int	231                     # 0xe7
	.int	231                     # 0xe7
	.int	117                     # 0x75
	.int	117                     # 0x75
	.int	117                     # 0x75
	.int	117                     # 0x75
	.int	265                     # 0x109
	.int	265                     # 0x109
	.int	265                     # 0x109
	.int	265                     # 0x109
	.int	123                     # 0x7b
	.int	65659                   # 0x1007b
	.int	123                     # 0x7b
	.int	123                     # 0x7b
	.int	97                      # 0x61
	.int	65633                   # 0x10061
	.int	97                      # 0x61
	.int	97                      # 0x61
	.int	229                     # 0xe5
	.int	65765                   # 0x100e5
	.int	229                     # 0xe5
	.int	229                     # 0xe5
	.int	77                      # 0x4d
	.int	65613                   # 0x1004d
	.int	77                      # 0x4d
	.int	77                      # 0x4d
	.int	345                     # 0x159
	.int	65881                   # 0x10159
	.int	345                     # 0x159
	.int	345                     # 0x159
	.int	65                      # 0x41
	.int	65601                   # 0x10041
	.int	65                      # 0x41
	.int	65                      # 0x41
	.int	247                     # 0xf7
	.int	65783                   # 0x100f7
	.int	247                     # 0xf7
	.int	247                     # 0xf7
	.int	327                     # 0x147
	.int	65863                   # 0x10147
	.int	327                     # 0x147
	.int	327                     # 0x147
	.int	143                     # 0x8f
	.int	65679                   # 0x1008f
	.int	143                     # 0x8f
	.int	143                     # 0x8f
	.int	73                      # 0x49
	.int	65609                   # 0x10049
	.int	73                      # 0x49
	.int	73                      # 0x49
	.int	265                     # 0x109
	.int	65801                   # 0x10109
	.int	265                     # 0x109
	.int	265                     # 0x109
	.int	89                      # 0x59
	.int	65625                   # 0x10059
	.int	89                      # 0x59
	.int	89                      # 0x59
	.int	181                     # 0xb5
	.int	65717                   # 0x100b5
	.int	181                     # 0xb5
	.int	181                     # 0xb5
	.int	189                     # 0xbd
	.int	65725                   # 0x100bd
	.int	189                     # 0xbd
	.int	189                     # 0xbd
	.int	139                     # 0x8b
	.int	65675                   # 0x1008b
	.int	139                     # 0x8b
	.int	139                     # 0x8b
	.int	95                      # 0x5f
	.int	65631                   # 0x1005f
	.int	95                      # 0x5f
	.int	95                      # 0x5f
	.int	83                      # 0x53
	.int	65619                   # 0x10053
	.int	83                      # 0x53
	.int	83                      # 0x53
	.int	375                     # 0x177
	.int	65911                   # 0x10177
	.int	375                     # 0x177
	.int	375                     # 0x177
	.int	207                     # 0xcf
	.int	65743                   # 0x100cf
	.int	207                     # 0xcf
	.int	207                     # 0xcf
	.int	107                     # 0x6b
	.int	65643                   # 0x1006b
	.int	107                     # 0x6b
	.int	107                     # 0x6b
	.int	93                      # 0x5d
	.int	65629                   # 0x1005d
	.int	93                      # 0x5d
	.int	93                      # 0x5d
	.int	115                     # 0x73
	.int	65651                   # 0x10073
	.int	115                     # 0x73
	.int	115                     # 0x73
	.int	97                      # 0x61
	.int	65633                   # 0x10061
	.int	97                      # 0x61
	.int	97                      # 0x61
	.int	439                     # 0x1b7
	.int	65975                   # 0x101b7
	.int	439                     # 0x1b7
	.int	439                     # 0x1b7
	.int	103                     # 0x67
	.int	65639                   # 0x10067
	.int	103                     # 0x67
	.int	103                     # 0x67
	.int	463                     # 0x1cf
	.int	65999                   # 0x101cf
	.int	463                     # 0x1cf
	.int	463                     # 0x1cf
	.int	107                     # 0x6b
	.int	65643                   # 0x1006b
	.int	107                     # 0x6b
	.int	107                     # 0x6b
	.int	131                     # 0x83
	.int	65667                   # 0x10083
	.int	131                     # 0x83
	.int	131                     # 0x83
	.int	209                     # 0xd1
	.int	65745                   # 0x100d1
	.int	209                     # 0xd1
	.int	209                     # 0xd1
	.int	137                     # 0x89
	.int	65673                   # 0x10089
	.int	137                     # 0x89
	.int	137                     # 0x89
	.int	16778111                # 0x100037f
	.int	16843647                # 0x101037f
	.int	895                     # 0x37f
	.int	895                     # 0x37f
	.int	119                     # 0x77
	.int	65655                   # 0x10077
	.int	119                     # 0x77
	.int	119                     # 0x77
	.int	41                      # 0x29
	.int	65577                   # 0x10029
	.int	131113                  # 0x20029
	.int	196649                  # 0x30029
	.int	97                      # 0x61
	.int	65633                   # 0x10061
	.int	131169                  # 0x20061
	.int	196705                  # 0x30061
	.int	127                     # 0x7f
	.int	65663                   # 0x1007f
	.int	131199                  # 0x2007f
	.int	196735                  # 0x3007f
	.int	287                     # 0x11f
	.int	196895                  # 0x3011f
	.int	131359                  # 0x2011f
	.int	65823                   # 0x1011f
	.int	33555441                # 0x20003f1
	.int	33752049                # 0x20303f1
	.int	33686513                # 0x20203f1
	.int	33620977                # 0x20103f1
	.int	223                     # 0xdf
	.int	196831                  # 0x300df
	.int	131295                  # 0x200df
	.int	65759                   # 0x100df
	.int	137                     # 0x89
	.int	196745                  # 0x30089
	.int	131209                  # 0x20089
	.int	65673                   # 0x10089
	.int	141                     # 0x8d
	.int	65677                   # 0x1008d
	.int	131213                  # 0x2008d
	.int	196749                  # 0x3008d
	.int	143                     # 0x8f
	.int	65679                   # 0x1008f
	.int	131215                  # 0x2008f
	.int	196751                  # 0x3008f
	.int	173                     # 0xad
	.int	65709                   # 0x100ad
	.int	131245                  # 0x200ad
	.int	196781                  # 0x300ad
	.int	399                     # 0x18f
	.int	65935                   # 0x1018f
	.int	131471                  # 0x2018f
	.int	197007                  # 0x3018f
	.int	151                     # 0x97
	.int	196759                  # 0x30097
	.int	131223                  # 0x20097
	.int	65687                   # 0x10097
	.int	16777885                # 0x100029d
	.int	16974493                # 0x103029d
	.int	16908957                # 0x102029d
	.int	16843421                # 0x101029d
	.int	309                     # 0x135
	.int	65845                   # 0x10135
	.int	131381                  # 0x20135
	.int	196917                  # 0x30135
	.int	16777749                # 0x1000215
	.int	16843285                # 0x1010215
	.int	16908821                # 0x1020215
	.int	16974357                # 0x1030215
	.int	95                      # 0x5f
	.int	196703                  # 0x3005f
	.int	131167                  # 0x2005f
	.int	65631                   # 0x1005f
	.int	83                      # 0x53
	.int	65619                   # 0x10053
	.int	131155                  # 0x20053
	.int	196691                  # 0x30053
	.int	307                     # 0x133
	.int	196915                  # 0x30133
	.int	131379                  # 0x20133
	.int	65843                   # 0x10133
	.int	16777845                # 0x1000275
	.int	16843381                # 0x1010275
	.int	16908917                # 0x1020275
	.int	16974453                # 0x1030275
	.int	469                     # 0x1d5
	.int	66005                   # 0x101d5
	.int	131541                  # 0x201d5
	.int	197077                  # 0x301d5
	.int	16778027                # 0x100032b
	.int	16974635                # 0x103032b
	.int	16909099                # 0x102032b
	.int	16843563                # 0x101032b
	.int	16777937                # 0x10002d1
	.int	16843473                # 0x10102d1
	.int	16909009                # 0x10202d1
	.int	16974545                # 0x10302d1
	.int	16777865                # 0x1000289
	.int	16843401                # 0x1010289
	.int	16908937                # 0x1020289
	.int	16974473                # 0x1030289
	.int	119                     # 0x77
	.int	196727                  # 0x30077
	.int	131191                  # 0x20077
	.int	65655                   # 0x10077
	.int	191                     # 0xbf
	.int	196799                  # 0x300bf
	.int	131263                  # 0x200bf
	.int	65727                   # 0x100bf
	.int	631                     # 0x277
	.int	197239                  # 0x30277
	.int	131703                  # 0x20277
	.int	66167                   # 0x10277
	.int	315                     # 0x13b
	.int	196923                  # 0x3013b
	.int	131387                  # 0x2013b
	.int	65851                   # 0x1013b
	.int	131                     # 0x83
	.int	196739                  # 0x30083
	.int	131203                  # 0x20083
	.int	65667                   # 0x10083
	.int	329                     # 0x149
	.int	65865                   # 0x10149
	.int	131401                  # 0x20149
	.int	196937                  # 0x30149
	.int	137                     # 0x89
	.int	65673                   # 0x10089
	.int	131209                  # 0x20089
	.int	196745                  # 0x30089
	.int	345                     # 0x159
	.int	65881                   # 0x10159
	.int	131417                  # 0x20159
	.int	196953                  # 0x30159
	.int	143                     # 0x8f
	.int	196751                  # 0x3008f
	.int	131215                  # 0x2008f
	.int	65679                   # 0x1008f
	.int	41                      # 0x29
	.int	65577                   # 0x10029
	.int	131113                  # 0x20029
	.int	196649                  # 0x30029
	.int	351                     # 0x15f
	.int	196959                  # 0x3015f
	.int	131423                  # 0x2015f
	.int	65887                   # 0x1015f
	.int	647                     # 0x287
	.int	197255                  # 0x30287
	.int	131719                  # 0x20287
	.int	66183                   # 0x10287
	.int	155                     # 0x9b
	.int	196763                  # 0x3009b
	.int	131227                  # 0x2009b
	.int	65691                   # 0x1009b
	.int	263                     # 0x107
	.int	196871                  # 0x30107
	.int	131335                  # 0x20107
	.int	65799                   # 0x10107
	.int	643                     # 0x283
	.int	197251                  # 0x30283
	.int	131715                  # 0x20283
	.int	66179                   # 0x10283
	.int	477                     # 0x1dd
	.int	66013                   # 0x101dd
	.int	131549                  # 0x201dd
	.int	197085                  # 0x301dd
	.int	393                     # 0x189
	.int	65929                   # 0x10189
	.int	131465                  # 0x20189
	.int	197001                  # 0x30189
	.int	541                     # 0x21d
	.int	66077                   # 0x1021d
	.int	131613                  # 0x2021d
	.int	197149                  # 0x3021d
	.int	503                     # 0x1f7
	.int	197111                  # 0x301f7
	.int	131575                  # 0x201f7
	.int	66039                   # 0x101f7
	.int	415                     # 0x19f
	.int	197023                  # 0x3019f
	.int	131487                  # 0x2019f
	.int	65951                   # 0x1019f
	.int	451                     # 0x1c3
	.int	197059                  # 0x301c3
	.int	131523                  # 0x201c3
	.int	65987                   # 0x101c3
	.int	607                     # 0x25f
	.int	197215                  # 0x3025f
	.int	131679                  # 0x2025f
	.int	66143                   # 0x1025f
	.int	623                     # 0x26f
	.int	197231                  # 0x3026f
	.int	131695                  # 0x2026f
	.int	66159                   # 0x1026f
	.int	295                     # 0x127
	.int	196903                  # 0x30127
	.int	131367                  # 0x20127
	.int	65831                   # 0x10127
	.int	95                      # 0x5f
	.int	196703                  # 0x3005f
	.int	131167                  # 0x2005f
	.int	65631                   # 0x1005f
	.int	163                     # 0xa3
	.int	65699                   # 0x100a3
	.int	131235                  # 0x200a3
	.int	196771                  # 0x300a3
	.int	307                     # 0x133
	.int	196915                  # 0x30133
	.int	131379                  # 0x20133
	.int	65843                   # 0x10133
	.int	167                     # 0xa7
	.int	65703                   # 0x100a7
	.int	131239                  # 0x200a7
	.int	196775                  # 0x300a7
	.int	885                     # 0x375
	.int	66421                   # 0x10375
	.int	131957                  # 0x20375
	.int	197493                  # 0x30375
	.int	173                     # 0xad
	.int	196781                  # 0x300ad
	.int	131245                  # 0x200ad
	.int	65709                   # 0x100ad
	.int	513                     # 0x201
	.int	66049                   # 0x10201
	.int	131585                  # 0x20201
	.int	197121                  # 0x30201
	.int	499                     # 0x1f3
	.int	66035                   # 0x101f3
	.int	131571                  # 0x201f3
	.int	197107                  # 0x301f3
	.int	817                     # 0x331
	.int	66353                   # 0x10331
	.int	131889                  # 0x20331
	.int	197425                  # 0x30331
	.int	183                     # 0xb7
	.int	65719                   # 0x100b7
	.int	131255                  # 0x200b7
	.int	196791                  # 0x300b7
	.int	515                     # 0x203
	.int	197123                  # 0x30203
	.int	131587                  # 0x20203
	.int	66051                   # 0x10203
	.int	191                     # 0xbf
	.int	196799                  # 0x300bf
	.int	131263                  # 0x200bf
	.int	65727                   # 0x100bf
	.int	189                     # 0xbd
	.int	65725                   # 0x100bd
	.int	131261                  # 0x200bd
	.int	196797                  # 0x300bd
	.int	501                     # 0x1f5
	.int	197109                  # 0x301f5
	.int	131573                  # 0x201f5
	.int	66037                   # 0x101f5
	.int	361                     # 0x169
	.int	196969                  # 0x30169
	.int	131433                  # 0x20169
	.int	65897                   # 0x10169
	.int	197                     # 0xc5
	.int	196805                  # 0x300c5
	.int	131269                  # 0x200c5
	.int	65733                   # 0x100c5
	.int	119                     # 0x77
	.int	196727                  # 0x30077
	.int	131191                  # 0x20077
	.int	65655                   # 0x10077
	.int	127                     # 0x7f
	.int	196735                  # 0x3007f
	.int	131199                  # 0x2007f
	.int	65663                   # 0x1007f
	.int	1015                    # 0x3f7
	.int	66551                   # 0x103f7
	.int	132087                  # 0x203f7
	.int	197623                  # 0x303f7
	.int	207                     # 0xcf
	.int	65743                   # 0x100cf
	.int	131279                  # 0x200cf
	.int	196815                  # 0x300cf
	.int	587                     # 0x24b
	.int	197195                  # 0x3024b
	.int	131659                  # 0x2024b
	.int	66123                   # 0x1024b
	.int	213                     # 0xd5
	.int	196821                  # 0x300d5
	.int	131285                  # 0x200d5
	.int	65749                   # 0x100d5
	.int	107                     # 0x6b
	.int	65643                   # 0x1006b
	.int	131179                  # 0x2006b
	.int	196715                  # 0x3006b
	.int	217                     # 0xd9
	.int	196825                  # 0x300d9
	.int	131289                  # 0x200d9
	.int	65753                   # 0x100d9
	.int	221                     # 0xdd
	.int	65757                   # 0x100dd
	.int	131293                  # 0x200dd
	.int	196829                  # 0x300dd
	.int	223                     # 0xdf
	.int	65759                   # 0x100df
	.int	131295                  # 0x200df
	.int	196831                  # 0x300df
	.int	281                     # 0x119
	.int	65817                   # 0x10119
	.int	131353                  # 0x20119
	.int	196889                  # 0x30119
	.int	271                     # 0x10f
	.int	65807                   # 0x1010f
	.int	131343                  # 0x2010f
	.int	196879                  # 0x3010f
	.int	507                     # 0x1fb
	.int	197115                  # 0x301fb
	.int	131579                  # 0x201fb
	.int	66043                   # 0x101fb
	.int	233                     # 0xe9
	.int	196841                  # 0x300e9
	.int	131305                  # 0x200e9
	.int	65769                   # 0x100e9
	.int	117                     # 0x75
	.int	196725                  # 0x30075
	.int	131189                  # 0x20075
	.int	65653                   # 0x10075
	.int	237                     # 0xed
	.int	196845                  # 0x300ed
	.int	131309                  # 0x200ed
	.int	65773                   # 0x100ed
	.int	743                     # 0x2e7
	.int	197351                  # 0x302e7
	.int	131815                  # 0x202e7
	.int	66279                   # 0x102e7
	.size	as_ts_gi, 3008

	.type	e1_e2_f2,@object        # @e1_e2_f2
	.global	e1_e2_f2
	.align	4
e1_e2_f2:
	.int	1310730                 # 0x14000a
	.int	1572876                 # 0x18000c
	.int	1835050                 # 0x1c002a
	.int	2097168                 # 0x200010
	.int	2359314                 # 0x240012
	.int	2621460                 # 0x280014
	.int	2883606                 # 0x2c0016
	.int	3145752                 # 0x300018
	.int	3407898                 # 0x34001a
	.int	3670100                 # 0x380054
	.int	3932250                 # 0x3c005a
	.int	4194336                 # 0x400020
	.int	4456482                 # 0x440022
	.int	4718700                 # 0x48006c
	.int	4980774                 # 0x4c0026
	.int	5243000                 # 0x500078
	.int	84                      # 0x54
	.int	5767212                 # 0x58002c
	.int	6029358                 # 0x5c002e
	.int	6291504                 # 0x600030
	.int	6553650                 # 0x640032
	.int	6815796                 # 0x680034
	.int	4718628                 # 0x480024
	.int	7340088                 # 0x700038
	.int	7602234                 # 0x74003a
	.int	7864380                 # 0x78003c
	.int	8126526                 # 0x7c003e
	.int	4194336                 # 0x400020
	.int	8650950                 # 0x8400c6
	.int	8912964                 # 0x880044
	.int	9175250                 # 0x8c00d2
	.int	4718628                 # 0x480024
	.int	9699402                 # 0x94004a
	.int	9961548                 # 0x98004c
	.int	10223694                # 0x9c004e
	.int	15728760                # 0xf00078
	.int	10747986                # 0xa40052
	.int	11010132                # 0xa80054
	.int	11272278                # 0xac0056
	.int	5767212                 # 0x58002c
	.int	11796570                # 0xb4005a
	.int	6029358                 # 0x5c002e
	.int	12320862                # 0xbc005e
	.int	6291504                 # 0x600030
	.int	12845154                # 0xc40062
	.int	5242920                 # 0x500028
	.int	13369446                # 0xcc0066
	.int	6815796                 # 0x680034
	.int	13893738                # 0xd4006a
	.int	9437256                 # 0x900048
	.int	14418030                # 0xdc006e
	.int	22020264                # 0x15000a8
	.int	14942322                # 0xe40072
	.int	7602234                 # 0x74003a
	.int	15466614                # 0xec0076
	.int	23593140                # 0x16800b4
	.int	15990906                # 0xf4007a
	.int	8126526                 # 0x7c003e
	.int	11010132                # 0xa80054
	.int	8388672                 # 0x800040
	.int	8650818                 # 0x840042
	.int	8912964                 # 0x880044
	.int	18350500                # 0x11801a4
	.int	12583008                # 0xc00060
	.int	9699402                 # 0x94004a
	.int	9961548                 # 0x98004c
	.int	30671082                # 0x1d400ea
	.int	10485840                # 0xa00050
	.int	10747986                # 0xa40052
	.int	33030396                # 0x1f800fc
	.int	11272278                # 0xac0056
	.int	5767212                 # 0x58002c
	.int	15728760                # 0xf00078
	.int	12058716                # 0xb8005c
	.int	12320862                # 0xbc005e
	.int	6291504                 # 0x600030
	.int	12845154                # 0xc40062
	.int	10485840                # 0xa00050
	.int	13369446                # 0xcc0066
	.int	6815796                 # 0x680034
	.int	13893738                # 0xd4006a
	.int	6291504                 # 0x600030
	.int	14418030                # 0xdc006e
	.int	14680176                # 0xe00070
	.int	14942322                # 0xe40072
	.int	7602234                 # 0x74003a
	.int	15466614                # 0xec0076
	.int	7864380                 # 0x78003c
	.int	15990906                # 0xf4007a
	.int	16253052                # 0xf8007c
	.int	11010132                # 0xa80054
	.int	8388672                 # 0x800040
	.int	8650818                 # 0x840042
	.int	26738892                # 0x19800cc
	.int	18350220                # 0x118008c
	.int	9437256                 # 0x900048
	.int	9699402                 # 0x94004a
	.int	9961548                 # 0x98004c
	.int	10223694                # 0x9c004e
	.int	31457520                # 0x1e000f0
	.int	10747986                # 0xa40052
	.int	33030396                # 0x1f800fc
	.int	11272278                # 0xac0056
	.int	11534424                # 0xb00058
	.int	7864380                 # 0x78003c
	.int	12058716                # 0xb8005c
	.int	12321614                # 0xbc034e
	.int	6291504                 # 0x600030
	.int	3670044                 # 0x38001c
	.int	10485840                # 0xa00050
	.int	13369446                # 0xcc0066
	.int	13631592                # 0xd00068
	.int	13894586                # 0xd403ba
	.int	12583008                # 0xc00060
	.int	14418030                # 0xdc006e
	.int	14680176                # 0xe00070
	.int	14942322                # 0xe40072
	.int	15204468                # 0xe80074
	.int	82575714                # 0x4ec0162
	.int	15728760                # 0xf00078
	.int	150209122               # 0x8f40262
	.int	16253052                # 0xf8007c
	.int	89129380                # 0x55001a4
	.int	8388672                 # 0x800040
	.int	8650818                 # 0x840042
	.int	17825928                # 0x1100088
	.int	85459364                # 0x51801a4
	.int	28311768                # 0x1b000d8
	.int	86507964                # 0x52801bc
	.int	87032264                # 0x53001c8
	.int	87556564                # 0x53801d4
	.int	10485840                # 0xa00050
	.int	21495972                # 0x14800a4
	.int	89129464                # 0x55001f8
	.int	22544556                # 0x15800ac
	.int	11534424                # 0xb00058
	.int	39321900                # 0x258012c
	.int	12058716                # 0xb8005c
	.int	24641724                # 0x17800bc
	.int	12583008                # 0xc00060
	.int	3670044                 # 0x38001c
	.int	31457520                # 0x1e000f0
	.int	26738892                # 0x19800cc
	.int	13631592                # 0xd00068
	.int	27787476                # 0x1a800d4
	.int	25166016                # 0x18000c0
	.int	28836060                # 0x1b800dc
	.int	44040528                # 0x2a00150
	.int	29884644                # 0x1c800e4
	.int	30408936                # 0x1d000e8
	.int	30933228                # 0x1d800ec
	.int	15728760                # 0xf00078
	.int	31981812                # 0x1e800f4
	.int	32506104                # 0x1f000f8
	.int	22020264                # 0x15000a8
	.int	8388672                 # 0x800040
	.int	17039490                # 0x1040082
	.int	34603272                # 0x2100108
	.int	17563782                # 0x10c0086
	.int	53477784                # 0x3300198
	.int	18088074                # 0x114008a
	.int	36700440                # 0x2300118
	.int	18612366                # 0x11c008e
	.int	62915040                # 0x3c001e0
	.int	19136658                # 0x1240092
	.int	58196412                # 0x37801bc
	.int	15728760                # 0xf00078
	.int	19923096                # 0x1300098
	.int	60555726                # 0x39c01ce
	.int	30671082                # 0x1d400ea
	.int	20709534                # 0x13c009e
	.int	10485840                # 0xa00050
	.int	12583008                # 0xc00060
	.int	99353478                # 0x5ec0386
	.int	21758118                # 0x14c00a6
	.int	44040528                # 0x2a00150
	.int	22282410                # 0x15400aa
	.int	11272278                # 0xac0056
	.int	22806702                # 0x15c00ae
	.int	23068848                # 0x16000b0
	.int	23330994                # 0x16400b2
	.int	15728760                # 0xf00078
	.int	23855286                # 0x16c00b6
	.int	24117432                # 0x17000b8
	.int	24379578                # 0x17400ba
	.int	12320862                # 0xbc005e
	.int	24903870                # 0x17c00be
	.int	62915040                # 0x3c001e0
	.size	e1_e2_f2, 752

	.type	beta_initial_value_cb1,@object # @beta_initial_value_cb1
	.comm	beta_initial_value_cb1,16,2
	.type	beta_initial_value_cb2,@object # @beta_initial_value_cb2
	.comm	beta_initial_value_cb2,16,2
	.type	L.str,@object           # @.str
	.section	.rodata.str1.1,"aMS",@progbits,1
L.str:
	.asciz	 "%d: %x, %x\n"
	.size	L.str, 12

	.type	L.str1,@object          # @.str1
L.str1:
	.asciz	 "passed!\n"
	.size	L.str1, 9

	.type	L.str2,@object          # @.str2
L.str2:
	.asciz	 "CRC_CHECK:%d,Final_iter:%d.\n"
	.size	L.str2, 29


