/*File Name: 		MaPU_ASM.m.ld
 *Creating Date: 	2013-1-10
 *Creating Author: 	shaolin.xie@ia.ac.cn
 *Description: 		The default link script for MMPU assembler files.
 */
/************************************************************************
 *  Here is an example asm file to use diffent sections.
 *  Please refer to http://192.168.3.91/Docs/using%20as.pdf
 *
	.text 
	.int 0x1
	.data 
	.int 0x2
	.section .SDA0.DM0, "aw"
	.int 0x3
	.section .SDA0.DM1, "aw"
	.int 0x4
	.section .SDA1.DM0, "aw"
	.int 0x5
	.section .SDA1.DM1, "aw"
	.int 0x6
	.section .SDA2.DM0, "aw"
	.int 0x7
	.section .SDA2.DM1, "aw"
	.int 0x8
*/
/***********************************************************************
 *  The memory block definition  of MMPU                               
 *  Which seems only affects the section address, not the segments.
 */
MEMORY {
  MIM       : ORIGIN = 0x000000	, LENGTH = 82K
  SDA0.DM0  : ORIGIN = 0x400000	, LENGTH = 0x40000
  SDA0.DM1  : ORIGIN = 0x600000	, LENGTH = 0x40000
  SDA1.DM0  : ORIGIN = 0x800000	, LENGTH = 0x40000
  SDA1.DM1  : ORIGIN = 0xA00000	, LENGTH = 0x40000
  SDA2.DM0  : ORIGIN = 0xC00000	, LENGTH = 0x40000
  SDA2.DM1  : ORIGIN = 0xE00000	, LENGTH = 0x40000
}

/****************************************************************
 * The segments definition, which affect the load address of the
 * elf.
 */

/* All text code & data are in IM */
SECTIONS {
  /* All .text & .data & .bss are located in IM */
  .text : { 
    *(.text) 
  } >MIM
  .data : {
    *(.data) 
  } >MIM

  /********************************************/
  .SDA0.DM0 : { *( .SDA0.DM0 ) } > SDA0.DM0 
  .SDA0.DM1 : { *( .SDA0.DM1 ) } > SDA0.DM1
  .SDA1.DM0 : { *( .SDA1.DM0 ) } > SDA1.DM0
  .SDA1.DM1 : { *( .SDA1.DM1 ) } > SDA1.DM1
  .SDA2.DM0 : { *( .SDA2.DM0 ) } > SDA2.DM0
  .SDA2.DM1 : { *( .SDA2.DM1 ) } > SDA2.DM1
}


