
	          .EQU		I_BIT,              0x80
	          .EQU		F_BIT,              0x40
        .global wait
        .func   wait
wait:
            wfi
            bx      lr
        .size   wait, . - wait
        .endfunc

        .global enable_irq
        .func   enable_irq
enable_irq:
            push    {r0}
            mrs     r0, CPSR
            bic     r0, #I_BIT
            msr     CPSR_c, r0
            pop     {r0}
            bx      lr
        .size   enable_irq, . - enable_irq
        .endfunc

        .global enable_fiq
        .func   enable_fiq
enable_fiq:
            push    {r0}
            mrs     r0, CPSR
            bic     r0, #F_BIT
            msr     CPSR_c, r0
            pop     {r0}
            bx      lr
        .size   enable_fiq, . - enable_fiq
        .endfunc
