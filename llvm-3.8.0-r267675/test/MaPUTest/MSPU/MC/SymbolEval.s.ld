MEMORY {
  RAM : ORIGIN = 0x0, LENGTH = 0x40000
}
/* All text code & data are in IM */
SECTIONS {
  .text : { *(.text) }
  .data : { *(.data) }
  .bss  : { *(.bss ) }
}

/* The Stack Pointer  */
STACK_POINTER = 0x40000;
