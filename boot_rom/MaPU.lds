/*---------------------------------------------------------------------------*/
/*- File source          : GCC_RAM.ld                                        */
/*- Object               : Linker Script File for RAM Workspace              */
/*- Compilation flag     : None                                              */
/*-                                                                          */
/*- 1.0 11/Mar/05 JPP    : Creation S256                                     */
/*---------------------------------------------------------------------------*/
OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm", "elf32-littearm")
OUTPUT_ARCH(arm)
ENTRY(_startup)
/* Memory Definitions */

MEMORY {
  ROM (rx)  : ORIGIN = 0x00000000, LENGTH = 0x00C00000
  RAM (rwx) : ORIGIN = 0x20000000, LENGTH = 0x00100000
}

C_STACK_SIZE = 0x40000;
IRQ_STACK_SIZE = 0;
FIQ_STACK_SIZE = 0;
SVC_STACK_SIZE = 0;
ABT_STACK_SIZE = 0;
UND_STACK_SIZE = 0;

/* Section Definitions */

SECTIONS
{
  .reset : {
    *start.o (.text)
    . = ALIGN(0x4);
  } >ROM

  .text : {
    CREATE_OBJECT_SYMBOLS
    *(.text .text.* .gnu.linkonce.t.*)
    *(.plt)
    *(.gnu.warning)
    *(.glue_7t) *(.glue_7)
    
    . = ALIGN(0x4);
    
    KEEP (*crtbegin.o(.ctors))
    KEEP (*(EXCLUDE_FILE(*crtend.o) .ctors))
    KEEP (*(SORT(.ctors.*)))
    KEEP (*crtend.o(.ctors))
    KEEP (*crtbegin.o(.dtors))
    KEEP (*(EXCLUDE_FILE (*crtend.o) .dtors))
    KEEP (*(SORT(.dtors.*)))
    KEEP (*crtend.o(.dtors))
    
    *(.rodata .rodata.* .gnu.linkonce.r.*)
    *(.init)
    *(.fini)
  } >ROM
  
  _etext = .;

  /* .data section which is used for initialized data */

  .data : {
    __data_load = LOADADDR (.data);
    __data_start = .;
    KEEP(*(.jcr))
    *(.got.plt) *(.got)
    *(.shdata)
    *(.data .data.* .gnu.linkonce.d.*)
    . = ALIGN(4);
    _edata = .;
  } >RAM AT>ROM

  /* .bss section which is used for uninitialized data */

  .bss : {
    __bss_start__ = . ;
    *(.shbss)
    *(.bss .bss.* .gnu.linkonce.b.*)
    *(COMMON)
  . = ALIGN(4);
    __bss_end__ = . ;
  } >RAM

  _end = .;
  __end = _end;
  PROVIDE(end = .);
  
  .stack : {
    __stack_start__ = .;
    . += IRQ_STACK_SIZE;
    . = ALIGN(4);
    __irq_stack_top__ = .;
    
    . += FIQ_STACK_SIZE;
    . = ALIGN(4);
    __fiq_stack_top__ = .;
    
    . += SVC_STACK_SIZE;
    . = ALIGN(4);
    __svc_stack_top__ = .;
    
    . += ABT_STACK_SIZE;
    . = ALIGN(4);
    __abt_stack_top__ = .;
    
    . += UND_STACK_SIZE;
    . = ALIGN(4);
    __und_stack_top__ = .;
    
    . += C_STACK_SIZE;
    . = ALIGN(4);
    __c_stack_top__ = .;

    __stack = .;
    
    __stack_end__ = .;
  } >RAM

  /* Stabs debugging sections.  */
  .stab          0 : { *(.stab) }
  .stabstr       0 : { *(.stabstr) }
  .stab.excl     0 : { *(.stab.excl) }
  .stab.exclstr  0 : { *(.stab.exclstr) }
  .stab.index    0 : { *(.stab.index) }
  .stab.indexstr 0 : { *(.stab.indexstr) }
  .comment       0 : { *(.comment) }
  /* DWARF debug sections.
     Symbols in the DWARF debugging sections are relative to the beginning
     of the section so we begin them at 0.  */
  /* DWARF 1 */
  .debug          0 : { *(.debug) }
  .line           0 : { *(.line) }
  /* GNU DWARF 1 extensions */
  .debug_srcinfo  0 : { *(.debug_srcinfo) }
  .debug_sfnames  0 : { *(.debug_sfnames) }
  /* DWARF 1.1 and DWARF 2 */
  .debug_aranges  0 : { *(.debug_aranges) }
  .debug_pubnames 0 : { *(.debug_pubnames) }
  /* DWARF 2 */
  .debug_info     0 : { *(.debug_info .gnu.linkonce.wi.*) }
  .debug_abbrev   0 : { *(.debug_abbrev) }
  .debug_line     0 : { *(.debug_line) }
  .debug_frame    0 : { *(.debug_frame) }
  .debug_str      0 : { *(.debug_str) }
  .debug_loc      0 : { *(.debug_loc) }
  .debug_macinfo  0 : { *(.debug_macinfo) }
  /* SGI/MIPS DWARF 2 extensions */
  .debug_weaknames 0 : { *(.debug_weaknames) }
  .debug_funcnames 0 : { *(.debug_funcnames) }
  .debug_typenames 0 : { *(.debug_typenames) }
  .debug_varnames  0 : { *(.debug_varnames) }

}
