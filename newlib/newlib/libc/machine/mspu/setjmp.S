# setjmp/longjmp for mspu.  The jmp_buf looks like this:
#
# Register	jmp_buf offset
# J30 (link)0x00
# J29 (SP)	0x04
# J28 (FP)	0x08
# reserved	0x0c

	.text

########################################################

	.global	setjmp
setjmp:              # frameless function
	J2 = R2;;        # first arg
	[J2 + 0] = J30;;
	[J2 + 4] = SP ;;
	[J2 + 8] = FP ;;
	R2 = 0;;          # return 0 to caller to indicate env is saved.
	jump J30;;



########################################################

	.global	longjmp
longjmp:
	J2  = R2;;
	J30 = [J2 + 0];;
	SP  = [J2 + 4];;
	FP  = [J2 + 8];;
	R2 = R3;;
	jump J30;;
