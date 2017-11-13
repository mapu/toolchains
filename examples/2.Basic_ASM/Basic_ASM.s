.text
.global _start

	_start:
	 m.s     R0 = 0x1	;;
	 m.s     R1 = 0x2	;;
	 m.s     R2 = R0 + R1	;;
	 m.s     NOP		;;
         m.s	 NOP		;;
	 m.s     SPUStop	;;
