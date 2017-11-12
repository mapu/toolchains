# Instruction Set Architecture
## General assembly syntax
MaPU uses VLIW instruction format. The scalar processing unit includes 4 slots. 
A instruction line contains instructoions that are issued at the same clock cycle. 
The assembly syntax of the instruction line is defined as follow:
```asm
m.s inst0; instr1; instr2; instr3 ;;
```
The instruction line start with the *m.s* prefix, and ended with a double semicolon(*;;*). 
The instruction in a line is seperated with a single semicolon(*;*).

The length of an instruction line is variable, it can contain 1 to 4 instructions, but at most only one of following 4 types of 
instruction:
* **SCU**: scalar computing instructions
* **AGU**: address generation instructions
* **SEQ**: branch realted instrucitons
* **SYN**: synchronization instructions 
