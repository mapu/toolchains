# Linker
The linker combines the multipler object files into one executable files. 
For general background and interal elf file format, 
please refer to this book: "**Linkers and Loaders**" written by **John R. Levine**  


## Linker usage
The main role of the linker is to resolve the function or variable address in different files.
For example, we defined a **_Label1** in *file1.asm*:
```asm
#file1.asm
.data
.global _Label1

_Label1:
.int 0x1
```
**_Label1**  will be referenced in *file2.asm*:
```asm
## file2.asm
.text

m.s R1 = _Label1 ;;
```
Before linking, the value of **Label1** (which is actually is a memory address) is unkown.
Following commands compile the assembly file to the object file, but without linking: 
```bash
llvm-mc -arch mspu file1.asm -filetype=obj -o file1.o
llvm-mc -arch mspu file2.asm -filetype=obj -o file2.o
```
We can check the values of the symbols in the object files with *llvm-objdump* command:
```bash
llvm-objdump -t file1.o

file1.o:        file format ELF32-mspu

SYMBOL TABLE:
00000000         *UND*           00000000 
00000000         .data           00000000 _Label1
```
For file2.asm:
```bash
file2.o:        file format ELF32-mspu

SYMBOL TABLE:
00000000         *UND*           00000000 
00000000         *UND*           00000000 _Label1
```
We can see that without linking, the value of **_Label1** are both zero.

The linker used in *mspu* is from [GNU Binutils](https://www.gnu.org/software/binutils/) which is called *gold*.
Following commands link the two object files and gererates the final executable files *test_linker*:
```bash
ld.gold -T $MAPU_HOME/apc/share/MaPU_ASM.s.ld  file1.o file2.o -o test_linker 
```
where the *MAPU_ASM.s.ld* is the link script files which tell which sections should be put into which address.
After linking, we then can re-check the value of **_Label1**:
```bash
[xiesl@mapu-opencl linker]$ llvm-objdump -t test_linker 

test_linker:    file format ELF32-mspu

SYMBOL TABLE:
00000000         *UND*           00000000 
00000008         .data           00000000 _Label1
```
We can see the linker solved the symbol value along with the link script. The final value is 0x8.

## Porting GNU gold 
The linker used for *mspu* is part of the **binutils-2.25**, we can get the source code with following command:
```bash
wget https://ftp.gnu.org/gnu/binutils/binutils-2.25.tar.gz
```
The linker is under the *binutils-2.25/gold/* directory

### Adding new target
We need to add a new target represent in *glod* so the tool can recongnize our architecture. 
1. gold-2.25/gold/configure.tgt
```bash
169 mspu-*)
170   targ_obj=mspu
171   targ_machine=EM_MSPU
172   targ_size=32
173   targ_big_endian=false
174   targ_extra_big_endian=false
175   ;;
```
2. gold-2.5/elfcpp/elfcpp.h
This this is where we define the elf file for MSPU
```c++
 273   // ARCH_MAPU
 274   EM_MSPU = 0x288,
 275   EM_MMPU = 0x460,
```

### Implement the linker
The major job of linker is to resolve the symbol value and modifed the interal instruction encoding so the instruction points
to the right location. The *gold* frame work has resolved all of the symbol value for us. What we need to do is modify the 
instruction encoding.

First, we have to define the relocation types used in MSPU:
1. *gold-2.25/elfcpp/mspu.h:* Define the relocation types

The relocation type used for MSPU is depending on how many instructions can use *Immediates*. 
Each type of these instrution will need one relocation type.
```c++
enum {
  R_MSPU_None      = 0,

  R_MSPU_PC17, // PC-relative address, bits 21-5, for jump/loop instruction 
  R_MSPU_PC28, // PC-relative address, bits 27-0, imm extension for jump/loop instruction 
  R_MSPU_11_B5, // 11 bits relo, bits 15-5   ,for imm assignment 
  R_MSPU_11_B10, // 11 bits relo, bits 20-10 ,for load/store
  R_MSPU_17, // PC-irrelative address, bits 21-5, for CallM instructions.
  R_MSPU_28,  // 28-bit extension, at bit 27-0,    imm extension for imm assign & load/store.
  ...
}
```
2. *gold-2.25/gold/mspu.cpp:* Implement the relocation functions
The main funciton do the relocation is as follows:
```c++
template<int size, bool big_endian> 
inline bool Target_mspu<size, big_endian>::Relocate::relocate( ...) {
...
  switch (r_type) {
  case elfcpp::R_MSPU_11_B5:   //This is the ImmAssign Instruction
    if (bImmExt)
      Reloc::template rela<32>(view, 0, 5, 0x000001E0, object, psymval, addend);
    else
      Reloc::template rela<32>(view, 0, 5, 0x0000FFE0, object, psymval, addend);
    break;
...
}
```
According to the relocation type, this funciton will further call the templated function to do the actuall job:
```c++
template<int valsize> 
static inline void rela( ... ) {
    //val is the instructions needs relocation
    Valtype val = elfcpp::Swap<valsize, big_endian>::readval(wv);

    //psymval->value(object, addend) is the resolved symbol value
    Valtype reloc = (psymval->value(object, addend) >> right_shift);

    //prepare the value for the Imm field of the instruction encoding
    reloc = reloc << left_shift;
    val &= ~dst_mask;
    reloc &= dst_mask;

    //write back the modified instruction encoding 
    elfcpp::Swap<valsize, big_endian>::writeval(wv, val | reloc);
  }
```
[\<------ \[Table of Content\]]({{site.url}}/index) <span style="float:right">  [\[Assembler\] ------>](Assembler)  </span>
