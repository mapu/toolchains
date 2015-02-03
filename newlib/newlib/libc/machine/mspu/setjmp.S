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
	m.s J2 = R2;;        # first arg
	m.s [J2 + 0] = J30;;
	m.s [J2 + 4] = SP ;;
	m.s [J2 + 8] = FP ;;
	m.s R2 = 0;;          # return 0 to caller to indicate env is saved.
	m.s jump J30;;



########################################################

	.global	longjmp
longjmp:
	m.s J2  = R2;;
	m.s J30 = [J2 + 0];;
	m.s SP  = [J2 + 4];;
	m.s FP  = [J2 + 8];;
	m.s R2 = R3;;
	m.s jump J30;;
