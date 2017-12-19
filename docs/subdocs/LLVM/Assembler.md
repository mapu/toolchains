# The Assembler/Disassembler
The assembler/disassembler in MaPU is based on the MC layer of LLVM. MC stands for LLVM **M**achine **C**ode, 
which is a subproject of LLVM aims to "solve a number of problems in the realm of assembly, disassembly, 
object file format handling, and a number of other related areas that CPU instruction-set level tools work in."
More information about the MC project can be found here: [Introduction to LLVM MC Project](http://blog.llvm.org/2010/04/intro-to-llvm-mc-project.html).

## Table of content 
* [Usage](#Usage)
* [Implementation](#Implementation)
    * [Stand alone Lexer](#Lexer)
    * [Stand alone Parser](#Parser)
    * [Instructions in Tablegen](#Tablegen)
    * [MSPUAsmParser](#MSPUAsmParser)
    * [MSPUDisassembler](#MSPUDisassembler)
    * [MSPUInstrPrinter](#MSPUInstrPrinter)

## Usage <a name="Usage"></a>
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

## Implementation <a name="Implementation"></a>
Most of the CPUs in LLVM MC uses *Tablegen* tools to generated 
the tables in C++ class/files for the asm parser and decoder etc. 
The instruction format and encodings are depicted as **.td** files.
Before compiling the MC tools, the *Tablegen* tools are used to generated C++ classes from these **.td** files,
as shown in following:
```text
               Tablegen Passes      Tables in C++ files
 ARM.td  |--> (-gen-register-info) -->RegisterInfo.inc 
   |     |--> (-gen-instr-info)    -->InstrInfo.inc 
   v     |--> (-gen-asm-writer)    -->AsmWriter.inc          --> For Instr Printer 
Tablegen-+--> (-gen-asm-matcher)   -->AsmMatcher.inc         --> For ASMParser
         |--> (-gen-fast-isel)     -->FastISel.inc           --> For compiler
         |--> (-gen-disassembler)  -->DisassemblerTables.inc --> For Disassembler
```
MaPU reuses most of this framework to construct the MC tools, except the parser. 
The reason is though the assembly code in MaPU is more readable, parsing MaPU assembly code is complicated. 
MaPU uses post operations syntax in the assembly, in which 
the first token does not indicates the operations.
The parser can not determine the following up tokens untiles it reaches the end
of instruction. For example, in 'R0 = R1 + R2 (U)' instruction, when the parser 
read 'R0' it can not determine what should be followed.

To address this problem, we used standard lexer/parser tools used in compiler to parser the assembly instructions.
We use [Ragel](http://www.colm.net/open-source/ragel/) for lexical analysis:
And use **Bison** for semantic parsing. 

**Pros and Cons:** Using stand alone lexer/parser give us more flexibility to describe the syntax,
but at the cost that we need to describe the assembly syntax twice: One in this parser, and the other in 
the **.td** file. This increase the code lines but also may incurs discrepancy between these two descriptions.

While the **Bison** tool is pre-installed on most linux distribution,  **Ragel** is not pre-installed.
we need to incoporate the **Ragel** tools in our toolchain repository. 
The version of the *Ragel* we used can be retrived with following command:
```bash
wget https://www.colm.net/files/ragel/ragel-6.8.tar.gz
```
### Stand alone Lexer  <a name="Lexer"></a>
Lexical analysis, lexing or tokenization is the process of converting a sequence of characters 
(such as in a computer program or web page) into a sequence of tokens 
(strings with an assigned and thus identified meaning). 
In our case, for example, transform the string 'R1' into a token that represents a R register with a value '0x1'.
The token is defined in the parser and always represented by an integer value. We will discuss the parser in next section. 

Besides **Ragel**, there are other lexers like **flex**, **ANTLR**. 
In these tools, the main content are *rules*, each contains a *regular expression* that describes the token, and a **action** 
that will be triggered when a token is found. 
For example, following regular expression defines the R Register token: 
```text
 13   'r'i ( '0' | ([1-9][0-9]*)) => {
 14         assert(atoi(ts+1)<32);
 15         yylval->op =
 16           MSPU::MSPUAsmOperand::createReg(
 17             (MSPU::MSPUAsmParser::getRegInfo())
 18               ->getRegClass(MSPUReg::RRegRegClassID).getRegister(atoi(ts+1)));
 19 
 20         CurLoc = SMLoc::getFromPointer(te); return _RReg;
 21     };
```
The **'r'i ( '0' | ([1-9][0-0]\*) )** is a regular expression where *r* should be followed by *0* or *r* followed by 
two consecutive digits where the first digit must start from 1.   
This describes the r0~r99, which contains our R register (r0~r31) but the range is larger.  
We can add further constrains of the token in the *action*.

The *action* is just a C code piece that will be executed when the token is found. 
You can use any function or expressions if proper header files are included. 
Generally an *action* will return a token which is defined in the parser, and a value (**yylval**) associated 
with the token. 
In the aformentioned example, the *action* includes checking the digital range, set the **yylval**, and return the 
**_RReg** token.

The complete lexer rules of mspu can be found at the source code  

[llvm-\<ver\>/lib/Target/MSPU/AsmParser/MSPUInstrRFLexer.rl](https://github.com/mapu/toolchains/blob/master/llvm-3.8.0-r267675/lib/Target/MSPU/AsmParser/MSPUInstrRLexer.rl)


### Stand alone syntax parser  <a name="Parser"></a>
The syntax parser intake the tokens and check syntax. We use **Bison** for the parser. The basic 
format of the Bison input file are as following:
```text
   %{
   Declarations
   %}
   Definitions
   %%
   Productions
   %%
   User subroutines
```
The optional Declarations and User subroutines sections are used for ordinary C code 
that you want copied to the generated C file, 
declarations are copied to the top of the file, 
user subroutines to the bottom. 
The optional Definitions section is where you configure various parser features such as 
defining token codes, establishing operator precedence and associativity, 
and setting up the global variables used to communicate between the scanner and parser. 
The required Productions section is where you specify the grammar rules

The *token* are defined ussing the **%token** directive, for example:
```text
%token <op>    _RReg _JReg _DReg _Imm  _Symb  _Expr  _FPImm
```
where the field **op** in the angle bracket indicates the type of the token, which is
defined in using the **%union**  direcive:  
```c
55 %union {
56 
57     MSPU::MSPUAsmOperand* op;
58     unsigned  opc;
59     unsigned  flags;
60 
61     struct __mspu_inst_line {
62             unsigned bits;
63             unsigned cnt;
64     } line;
65 
66 }
```
This **%union** directive also defines the type of the global variable **llyval** that
we used in lexer.

Productions and their accompanying actions are specified using the following format:
```text
left_side: right_side1 { action1 }
         | right_side2 { action2 } 
         | right_side3 { action3 }
         ;
```
The left side is the non­terminal being described. 
Non­terminals are named like typical identifiers: 
using a sequence of letters and/or digits, starting with a letter. 
The right side can be a individual characters or defined tokens. 

The *action* is a piece of C code which will be executed when 
parser recognizes that production.

We can aslo define the type of the non-terminals with the **%type** directive:
```text
%type <flags>  XferFlags_  SCUFlags_
```
The complete assembler syntax of **mspu** are defined in 

[llvm-\<ver\>/lib/Target/MSPU/AsmParser/MSPUInstrParser.y](https://github.com/mapu/toolchains/blob/master/llvm-3.8.0-r267675/lib/Target/MSPU/AsmParser/MSPUInstrParser.y)

More information about the Bison usage please refer to 
[Introduction to bison](https://web.stanford.edu/class/archive/cs/cs143/cs143.1128/handouts/120%20Introducing%20bison.pdf)

## Instructions in Tablegen  <a name="Tablegen"> </a>
As aformentioned before, except the lexer and syntax parser, 
all other assembler/disassembler related tools are generated from the **Tablegen**.
In this section, we will introduce the **Tablegen** briefly, and illustrate how we 
can use **Tablegen** lauguage to describe the instructions of mspu. More infomation
about Tablegen can be found at [TableGen Language Introduction](https://llvm.org/docs/TableGen/LangIntro.html).

Tablegen is a general tool or language to generated C++ classes or tables. The basic idea of Tablegen is using
the 'inherit' feature to collect together the commonality between a group of records.  
For example, the MSPU instructions includes 4 types of instruction: SCU, AGU, SEQ and SYN.
These 4 types of instructions share some common attributes: 
* 32 bits instruction
* the 31st bits indicates the end of instruction line
* 30-28 bits are FUs indicator

In OOP progamming, the MSPU instructions is the base class which hold these command fileds,
while SCU, AGU, SEQ and SYN instructions are derived classes from the base classes, which will overwrite 
some fileds of the base class, and might also add other fileds.

Tablegen language can be seem as a simplified OOP language that depicts the 'inherit', 'fileds' 'overwrite' between classes:
```text
class C { bit V = 1; }

def Y : C {
  string Greeting = "hello";
}

class D : C { let V = 0; }
def Z : D;
```
In the above example, class C has a filed 'V' with initial value 1. 
Y is a class inheriting from C and have an additional field 'Greeting'. 
Class D is also inherited from C but the initial value of filed 'V' is '0'.

The *class* and *def* are both 'record' in Tablegen. The actual meaning is depending on the backend which intake the 
Tablegen lunguage file and outputs the C++ files.

### Define the mspu instructions
In this section, 
we will trace down how we use the Tablegen language to describe the mspu instruction "Rs = Rm + Rn (U)". 
The top instruction format defined in
 [llvm-\<ver\>/lib/Target/MSPU/MSPUInstrFormats.td](https://github.com/mapu/toolchains/blob/master/llvm-3.8.0-r267675/lib/Target/MSPU/MSPUInstrFormats.td).
```c++
class MSPUInstr< bits<4> group, dag outs, dag ins, string asm,
                 list<dag> pattern = [] 
               > : Instruction, Sched<[]>  {
  // 'field' is a keyword.
  // Note the value of 'Inst' is specified distributedly.
  field bits<32> Inst;
  field bits<32> SoftFail; // mask bits for reserved encoding fields

  // 'let' is used to override an OLD value.
  let Namespace = "MSPUInst";
  let OutOperandList = outs;
  let InOperandList = ins;
  let AsmString   = asm;
  let Pattern = pattern;
  let DecoderNamespace = "MSPUDecode";
  let Size = 4; // this field is critical for disassembly

  let Inst{31-28} = group; // encoding for SEQ, SCU, AGU, ...
}
```
'MSPUInstr' class is inherited from class 'Instruction' and class 'Sched'. 
Class 'Instruction' is defined in  [llvm-\<ver\>/llvm/Target/Target.td](https://github.com/mapu/toolchains/blob/master/llvm-3.8.0-r267675/include/llvm/Target/Target.td) 
which is general for all targets. class 'Sched' is used for C complier and we will talk it later.

The 'MSPUInstr' have 5 input templates parameter:
* group: define the instruction belongs to which group (SCU,AGU, SYN, SEQ, etc)
* outs: the DAG list, used for compiler
* ins:  the DAG list, used for compiler
* asm:  the assembler string
* pattern: used for compiler

The filed 'Inst' is an 32 bits vector and is the instruction encoding. 
The last 'let Instr{31-28}= group' will overwrite the encoding bits 31-28 with the input template parameter 'group',
that is how we define the 'SCUInstr'.
```c++
class SCUInstr< dag outs, dag ins, string asm,
                bits<5> opcode, bits<5> flags,
                list<dag> pattern = []
              > : MSPUInstr <0b0001, outs, ins, asm, pattern >{
  let Inst{27-23} = opcode;
  let Inst{22-18} = flags;
  let isCodeGenOnly = 1;
}
```
The 'SCUInstr' is inherited from MSPUInstr, with the 'group' field beening overwritten with '0b0001'.
It also provides additional template parameters 'opcode', 'flags'. 

```c++
class BinaryInstr< dag outs, dag ins, string asm,
                   bits<5> opcode, bits<5> flags,
                   list<dag> pattern=[] 
                 >
    : SCUInstr< outs, ins, asm, opcode, flags, pattern > {
         bits<5> d;
         bits<5> s0;
         bits<5> s1;
         let Inst{14-10} = s1;
         let Inst{9-5} = s0;
         let Inst{4-0} = d;
}
```
'BinaryInstr' is a type of instructions in SCU group with 2 operands, with additional field 'd', 's0', 's1', 
which are the indice of the destination register and source registers.
```c++
class AddInstr< dag outs, dag ins, string asm, bits<5> flags,
                list<dag> pattern=[] 
              >
        : BinaryInstr<outs, ins, asm, 0b00001, flags, pattern>;

```
'ADDInstr' is a kind of 'BinaryInstr' whose 'opcode' is set to '0b00001'
```c++
def AddSI32T : AddInstr <
               (outs I32Reg:$d), (ins  I32Reg:$s0, I32Reg:$s1),
               "$d = $s0 + $s1 (T)", 0b00001,
               [(set I32Reg:$d, (add I32Reg:$s0, I32Reg:$s1))] 
                        >
{ let Itinerary = ItinSCU_2_1_1; }
```
Finally we defined the 'ADDSI32T' which means 32 bits integer add instructions with 'T" options. 
Other files in the above Tablegen description are related to the compiler.

For complete instruction description, please refer to file

[llvm-\<ver\>/lib/Target/MSPU/MSPUInstrInfo.td](https://github.com/mapu/toolchains/blob/master/llvm-3.8.0-r267675/lib/Target/MSPU/MSPUInstrInfo.td).

### MSPUAsmParser  <a name="MSPUAsmParser"></a>

This is the file that interact with the LLVM MC. For the *mspu* target, 
following class and information should be provided in this file:

| class/function                        |  description       |  members     |
|---------------------------------------|------------------  |--------------|
| **MSPUAsmOperand** :  MCParsedAsmOperand{ <br>} | The pasred operands | **operandKind**  <br>  StartLoc <br> EndLoc <br> *getTheOperandValue()* <br> *createOperand()* | 
| **MSPUAsmParser**  :  MCTargetAsmParser{ <br>}  | The parser class | MCSubtargetInfo <br> MCRegisterInfo <br> PreToken <br> PreLoc <br> **ParseInstruction()** <br> **MatchAndEmitInstruction()** |

#### operandKind 
Following types of operands is defined in *mspu* :
* Opcode (like +, -, x, / )
* Register 
* Imm    (like 0x11 )
* FPImm  (floating Imm, like  1.20 )
* Expr   (like   (0x1 \<\< 8) or with a label )
* InstPos ( The instruction position, used for VLIW )

#### MSPUAsmParser::ParseInstruction()
This is the function that will called by the LLVM MC. The funciton calling stack is as following:
1. MSPUAsmParser::ParseInstruction()
2. mspuinstparse() <-- Generated by the MSPUInstrParser.y (Bison file) 
3. mspuinstlex()   <-- Defined  in MSPUAsmParser.cpp 
4. msputoklex()    <-- Generated by the MSPUInstrRLexer.rl (Ragel file) 

#### MSPUAsmParser::MatchAndEmitInstruction()
This the function that intake the Opcode and the followed operands, and output the instructions to the *MCStream*, 
which is the data format passed different MC component. 

### MSPUDisassembler <a name="MSPUDisassembler"> </a>

This class implements the disassembler:

| class/function                        |  description       |  members     |
|---------------------------------------|------------------  |--------------|
| **MSPUDisassembler** :  MCDisassembler{ <br>} | Disassembler | *getInstruction()* | 

The code for disassembler looks like following:
```c++
template<typename InsnType> static DecodeStatus
    DecodeXXXReg( )
... 
#include "MSPUGenDisassemblerTables.inc"

DecodeStatus MSPUDisassembler::getInstruction(...)
```
The **MSPUGenDisassemblerTables.inc** is generated from the *Tablegen*

### MSPUInstPrinter <a name="MSPUInstrPrinter"> </a>

| class/function                        |  description       |  members     |
|---------------------------------------|------------------  |--------------|
| **MSPUInstrPrinter** :  MCInstrPrinter{ <br>} | Print the instructions | *printInst()* | 


The code for the instruction printer looks like following:
```c++
#include "MSPUGenAsmWriter.inc"
void MSPUInstPrinter::printInst(...)

void MSPUInstPrinter::printCallMFlag()
...
more printXXX()
```

[\<------ \[Table of Content\]]({{site.url}}/index) <span style="float:right">  [\[Assembler\] ------>](Assembler)  </span>

