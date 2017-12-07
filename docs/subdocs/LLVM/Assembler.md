# The Assembler/Disassembler
The assembler/disassembler in MaPU is based on the MC layer of LLVM. MC stands for LLVM **M**achine **C**ode, 
which is a subproject of LLVM aims to "solve a number of problems in the realm of assembly, disassembly, 
object file format handling, and a number of other related areas that CPU instruction-set level tools work in."
More information about the MC project can be found here: [Introduction to LLVM MC Project](http://blog.llvm.org/2010/04/intro-to-llvm-mc-project.html).

## Usage
The Assembler/Disassembler binary is named **llvm-mc** which located in the /bin of the installed directory. 

### Assemble code 
```asm
[xiesl@mapu-opencl 2.Basic_ASM]$ echo "m.s R0 = R1 + R2;;" | llvm-mc -show-encoding -arch=mspu
	.text
	m.s 	R0 = R1 + R2;;          # encoding: [0x20,0x08,0x80,0x90]
```
The **-show-encoding** option will cause the assembler to display the encoded bytes of the instruction.
LLVM MC tools supports all of the directives that GNU assembler supports. For full list of assember directives,
please refer to the GNU documents [Gnu Assembler Manual](https://sourceware.org/binutils/docs/as/). 

The LLVM MC can also intake assembler files and outputs object files. 
We can use following command to compiler the assembly file in /examples/2.Basic\_ASM:
```asm
llvm-mc -arch=mspu Basic_ASM.s -filetype=obj -o Basic_ASM.o
```
### Disassemble code

Passing **-disassemble** option to llvm-mc with disassemble the byte streams of the instructions:
```asm
[xiesl@mapu-opencl 2.Basic_ASM]$ echo "0x20 0x08 0x80 0x90" | llvm-mc -arch=mspu --disassemble
	.text
	m.s 	R0 = R1 + R2;;
```
To disassemble the instructions in an object file, we need to use the **llvm-objdump** tools with the disassemble options:
```asm 
[xiesl@mapu-opencl 2.Basic_ASM]$ llvm-objdump -arch-name=mspu --disassemble Basic_ASM.o 

Basic_ASM.o:	file format ELF32-mspu

Disassembly of section .text:
_start:
       0:	20 00 c0 b0 	m.s 	R0 = 1;;
       4:	41 00 c0 b0 	m.s 	R1 = 2;;
       8:	02 04 80 90 	m.s 	R2 = R0 + R1;;
       c:	00 00 00 80 	m.s 	NOP;;
      10:	00 00 00 80 	m.s 	NOP;;
      14:	00 00 00 81 	m.s 	SPU.Stop;;
```


[\<------ \[Table of Content\]]({{site.url}}/index) <span style="float:right">  [\[Assembler\] ------>](Assembler)  </span>
