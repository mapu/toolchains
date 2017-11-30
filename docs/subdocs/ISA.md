# Instruction Set Architecture
## General assembly syntax
MaPU uses VLIW instruction format. The scalar processing unit includes 4 slots. 
An instruction line contains instructoions that are issued at the same clock cycle. 
The assembly syntax of the instruction line is defined as follow:
```asm
m.s inst0; instr1; instr2; instr3 ;;
```
The instruction lines start with the *m.s* prefix, and ended with a double semicolon(*;;*). 
The instructions in a line are seperated with a single semicolon(*;*).

The length of an instruction line is variable, it can contain 1 to 4 instructions, but at most only one of following 4 
Function Units (FU)
* **SCU**: scalar computing instructions
* **AGU**: address generation instructions
* **SEQ**: branch realted instrucitons
* **SYN**: synchronization instructions 

### Register Groups
There are two physical regsiter groups in MaPU scalar pipeline:
```asm
    R0~R31 <---> DR0~DR16
    J0~J31
```
R group is 32 bits regsiter, which can aslo be accessed in *DR* mode for the instructions need 64 bits data. J group is 32 bits. 
We use 's' and 'm', 'n'  suffix to represent the destination index and source indice in this document. 

### Pipeline
In MaPU, the FU shares the instruction fetch pipeline.
```asm
FG --> FS --> FW --> FR --> DP --> RR --> EX1 --> EX2 --> ... 
```
The number of executing pipeline stages is depend on FU. The hardware will detect the data dependence. 
When there is dependence, a bubble will be inserted.

### Instruction options
Depend on the FUs, instructions may or may not include *options*, which is at the end of instruction and placed in parenthesis. The options specify additional information for the operation, for example, follow *(U)* option indicates an unsigned addition:
```asm
R1 = R2 + R3 (U)
```
* Optional option is placed in curly braces : **{}**.
* If multiple options are exclusive, they are seperated by **\|**.   

For example, following instruction indicates the *(U)* option is not mandatory. 
With U option, it is an unsigned compare; without U option, it is signed compare
```asm
Rs	=	Rm	<	Rn		{ (U) }
```
Following example shows the *(B)*, *(U)* are exlusive, which can not appear at the same time.
```asm
Rs |Js 	= [Jm  +   Jn]    { (B) | (H) }  
```
Supported instruction options
* **(U)** Unsinged operations. 
* **(T)** Saturate ( rounding) or Truncate the result if it is larger then the maximum values.
* **(B)** 8 bits operation
* **(H)** 16 bits operation
* **(S)** 32 bits float operation
* **(D)** 64 bits float operation
* Without any options, default behavior is 32 bits, integer, truncate operation

### Instructions of FUs 
1. [SCU](SCU)
2. [AGU](AGU)
3. [SEQ](SEQ)
4. [SYN](SYN)
5. [Instruction Encoding](Encoding)

[\<------ \[Table of Content\]](../index) <span style="float:right">  [\[SCU\] ------>](SCU)  </span>
