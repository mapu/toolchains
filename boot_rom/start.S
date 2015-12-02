/*------------------------------------------------------------------------------
*- Area Definition
*------------------------------------------------------------------------------
* .text is used instead of .section .text so it works with arm-aout too.  */
        .text
        .code 32
        .global _startup
        .func   _startup
_startup:
/*------------------------------------------------------------------------------
//*- Exception vectors 
//*--------------------
//*- These vectors can be read at address 0 or at RAM address
//*- They ABSOLUTELY requires to be in relative addresssing mode in order to
//*- guarantee a valid jump. For the moment, all are just looping.
//*- If an exception occurs before remap, this would result in an infinite loop.
//*- To ensure if a exeption occurs before start application to infinite loop.
//*------------------------------------------------------------------------------*/

            b       _reset        /* 0x00 Reset handler */
            b       .             /* 0x04 Undefined Instruction */
            b       .             /* 0x08 Software Interrupt */
            b       .             /* 0x0C Prefetch Abort */
            b       .             /* 0x10 Data Abort */
            b       .             /* 0x14 reserved  */
            b       .             /* 0x18 IRQ	 */
            b       .             /* 0x1c FIQ	*/

            .string     "MaPU Startup Code Image."
            .align      4
_reset:
            mrs     r0, cpsr
            and     r1, r0, #0x1f		@ mask mode bits
            teq     r1, #0x1a		@ test for HYP mode
            bicne   r0, r0, #0x1f		@ clear all mode bits
            orrne   r0, r0, #0x13		@ set SVC mode
            orr     r0, r0, #0xc0		@ disable FIQ and IRQ
            msr     cpsr,r0	
            /* Set V=0 in CP15 SCTRL register - for VBAR to point to vector */
            mrc     p15, 0, r0, c1, c0, 0	@ Read CP15 SCTRL Register
            bic     r0, #0x00002000		@ V = 0
            mcr     p15, 0, r0, c1, c0, 0	@ Write CP15 SCTRL Register

            /*
             * Invalidate L1 I/D
             */
            mov     r0, #0			@ set up for MCR
            mcr     p15, 0, r0, c8, c7, 0	@ invalidate TLBs
            mcr     p15, 0, r0, c7, c5, 0	@ invalidate icache
            mcr     p15, 0, r0, c7, c5, 6	@ invalidate BP array
            mcr     p15, 0, r0, c7, c10, 4	@ DSB
            mcr     p15, 0, r0, c7, c5, 4	@ ISB

            /*
             * disable MMU stuff and caches
             */
            mrc     p15, 0, r0, c1, c0, 0
            bic     r0, r0, #0x00002000	@ clear bits 13 (--V-)
            bic     r0, r0, #0x00000007	@ clear bits 2:0 (-CAM)
            orr     r0, r0, #0x00000002	@ set bit 1 (--A-) Align
            bic     r0, r0, #0x00000800	@ set bit 11 (Z---) BTB
            bic     r0, r0, #0x00001000	@ set bit 12 (I) I-cache

            mcr     p15, 0, r0, c1, c0, 0
/*------------------------------------------------------------------------------
/*- Low level Init (DDR, AIC, DMA ....) by C function MaPU_LowLevelInit
/*------------------------------------------------------------------------------*/
/*- minumum C initialization */
/*- call  MaPU_LowLevelInit() */
            ldr     r0, =_reset
/*--Call Low level init function in ABSOLUTE through the Interworking	*/
            ldr	    r1, =_cstartup
            mov     lr, r1
            ldr     sp, =__stack_end__
            b       LowLevelInit

_cstartup:

	          .EQU		ARM_MODE_FIQ,       0x11
	          .EQU		ARM_MODE_IRQ,       0x12
	          .EQU		ARM_MODE_SVC,       0x13
	          .EQU		ARM_MODE_ABT,       0x14
	          .EQU		ARM_MODE_UND,       0x15
	          .EQU		ARM_MODE_SYS,       0x16

	          .EQU		I_BIT,              0x80
	          .EQU		F_BIT,              0x40

            msr     CPSR_c, #(ARM_MODE_SYS | I_BIT | F_BIT)

            ldr     r12, =_mainCRTStartup
            mov     lr, pc
            bx      r12

            swi     0xFFFFFF
            .size   _startup, . - _startup
            .endfunc
/*---------------------------------------------------------------
//* ?EXEPTION_VECTOR
//* This module is only linked if needed for closing files.
//*---------------------------------------------------------------*/
        .global MaPU_Default_FIQ_handler
        .func   MaPU_Default_FIQ_handler
MaPU_Default_FIQ_handler:
            b     MaPU_Default_FIQ_handler
        .size   MaPU_Default_FIQ_handler, . - MaPU_Default_FIQ_handler
        .endfunc

        .global MaPU_Default_IRQ_handler
        .func   MaPU_Default_IRQ_handler
MaPU_Default_IRQ_handler:
            b     MaPU_Default_IRQ_handler
        .size   MaPU_Default_IRQ_handler, . - MaPU_Default_IRQ_handler
        .endfunc

        .global MaPU_Spurious_handler
        .func   MaPU_Spurious_handler
MaPU_Spurious_handler:
            b     MaPU_Spurious_handler
        .size   MaPU_Spurious_handler, . - MaPU_Spurious_handler
        .endfunc

            .end
