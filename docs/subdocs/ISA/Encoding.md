
# Instruction encodings

## General encodings of FU
Each instructions in MaPU scalar pipeline is 32 bits, in which the MSB (the 31st bits) **E** indicates if this 
instruction is the end of the instruction line. bits 30-28 classifies the instruction types, which will be used for 
dispatching instructions to the conresponding FUs.  
* **Imm Instructions:** This is a special instructions which is expended from other instructons with 32 bits Imm, 
because a single 32 bits instruction cannot hold all the 32 bits Imm,  
this instruciton hold parts of the 32 bits Imm and will be dispatched along with master instruction.

| 31     | 30-28  | 21-0 | Instruction type |
| -------| -------| -----| -----------------|
| E      | 000    | XXX  | SEQ Instructions |
| E      | 001    | XXX  | SCU Instructions |
| E      | 010    | XXX  | AGU Load/Store Instructions with Imm operands |
| E      | 011    | XXX  | AGU other instructions      |
| E      | 100    | XXX  | SYN instructions      |
| E      | 101    | XXX  | Imm instructions      |

## SCU encoding
The 22-18 bits are *options*. Only all combination of options are valid. 
Please check the assembly syntax of the individual instructions.

| 27-23  | 22-18        | 17-15 | 14-10 | 9-5 | 4-0 | descripton |
| -------| -------      | ------| ------|-----|-----| -----------|
| opcode | Imm U S D T  | -     | Rm    | Rn  | Rs  |  -         |
| 00001  |              | -     |       |     |     |  Addition  |
| 00010  |              | -     |       |     |     |  Subtract  |
| 10010  |              | -     |       |     |     |  Add with Carry |
| 10011  |              | -     |       |     |     |  Sub with Carry |
| 00011  |              | -     |       |     |     |  Multiply       |
| 00100  |              | -     |       |     |     |  to single float |
| 00101  |              | -     |       |     |     |  to double float |
| 00110  |              | -     |       |     |     |  to signed integer |
| 00111  |              | -     |       |     |     |  to unsigned integer |
| 01000  |              | -     |       |     |     |  Absolute value  |
| 01001  |              | -     |       |     |     |  reciprocal  |
| 01010  |              | -     |       |     |     |  reciprocal squre root |
| 10000  |              | -     |       |     |     |  left shift |
| 10001  |              | -     |       |     |     |  right shift |
| 10100  |              | -     |       |     |     |  bitwise and |
| 10101  |              | -     |       |     |     |  bitwise or  |
| 10110  |              | -     |       |     |     |  not  |
| 10111  |              | -     |       |     |     |  xor  |
| 11000  |              | -     |       |     |     |  equal|
| 11001  |              | -     |       |     |     |  not equal|
| 11010  |              | -     |       |     |     |  bigger   |
| 11011  |              | -     |       |     |     |  bigger or equal |
| 11100  |              | -     |       |     |     |  less  |
| 11101  |              | -     |       |     |     |  less or equal |

## AGU encoding
### Load/Store With Imm Operands
* **Load/Store:** 0 for load; 1 for store
* **B H U:** The B,H,U options
* **DR   :** Whether the destination is Double Register
* **DGroup:** 0 for R Register; 1 for J register
* **Inc  :** Whether to update Jm value 
* **Imm11:** The Imm value 
    * If the Imm is less or equal to 11 bits, this is the whole bits of the immediates.
    * If the Imm is larger then 11 bits, only 4 MSBs of the Imm11 are valid, which is also the 4 MSBs of the original value.
      Other 28 bits are encoded in a sperate Imm instruction

| 27            | 26-24  | 23 | 22      | 21 | 20-10 | 9-5 | 4-0                |
| -------       | -------|----| --------|----| ------|-----|-----               |
| load/store    | B H U  | DR | DGroup  | Inc| Imm11 | Jm  | Dest Index    |

Corresponding Load instructions with Imm Operands
```
DRs|Rs|Js = [ Jm  +|+= Imm32 ] { (B)|(H) } {(U)}   
```
Corresponding Store instructions with Imm Operands
```
[ Jm  +|+= Imm32 ] = DRs|Rs|Js  { (B)|(H) } {(U)}   
```
### Other AGU instrucitons
* **B H U:** The B,H,U options
* **DR   :** Whether the destination is Double Register
* **DGroup:** Destination Register group. 0 for R Register; 1 for J register
* **SGroup:** Source Register group. 0 for R Register; 1 for J register
* **Imm11:** The Imm value 
    * If the Imm is less or equal to 11 bits, this is the whole bits of the immediates.
    * If the Imm is larger then 11 bits, only 4 MSBs of the Imm11 are valid, which is also the 4 MSBs of the original value.
      Other 28 bits are encoded in a sperate Imm instruction

| 27-23   | 22 | 21-19 | 18      | 17      | 16 | 15 | 14-10   | 9-5    | 4-0      | Description |
| ------- | ---|-------|---------| --------|----| ---|---------|--------|----------|-------------|
| opcode  | Imm| B H U | SGroup  | DGroup  | DR | -  | Jn/Imm5 | Jm/Imm5|Dest Index|             |
| 00001   | 1  |       |         |         |    | Imm11 | Imm11| Imm11  |Imm11     | Js\|Rs= Imm32  |
| 00100   |    |       |         |         |    |    |         |        |          | Js = Jm + Jn  |
| 00101   |    |       |         |         |    |    |         |        |          | Js = Jm - Jn  |
| 00110   |    |       |         |         |    |    |         |        |          | Js = Jm \<\< Jn |
| 00111   |    |       |         |         |    |    |  Imm5   |        |          | Js = Jm \<\< Imm5  |
| 01000   |    |       |         |         |    |    |         |        |          | Js\|Rs = Jm\|Rm  |
| 01001   |    |       |         |         |    |    |         |        |          | DRs = DRm  |
| 01010   |    |       |         |         |    |    |         |        |          | Rs = STAT  |
| 11000   |    |       |         |         |    |    |         |        |          | load without Imm   |
| 11001   |    |       |         |         |    |    |         |        |          | store without Imm  |

Corresponding Load instructions without Imm Operands
```
DRs|Rs|Js = [ Jm  +|+= Jn ] { (B)|(H) } {(U)}   
```
Corresponding Store instructions without Imm Operands
```
[ Jm  +|+= Jn ] = DRs|Rs|Js  { (B)|(H) } {(U)}   
```
## SEQ Encoding
* **Imm:** With Imm17 or not
* **Jm :** source regsiter containing the jump address
* **Rm :** source regsiter containing the condition

| 27-23 | 22 | 21 | 20-10 | 9-5 | 4-0 | Description |
|-------|----|----|-------|-----|-----|-------------|
| opcode|Imm |    |       |Jm   | Rs  |             |
| 00000 |    |    |       |     |     |   NOP       |
| 00000 |    |    |       |     |     |   NOP       |
| 00001 |    |    |       |     |     |   DbBreak   |
| 01000 | 1  |Imm17| Imm17| Imm17|    |   JUMP IMM32|
| 01000 | 0  |    |      | Jm   |     |   JUMP Jm   |
| 01001 | 1  |Imm17| Imm17| Imm17| Rs |IF Rs, JUMP IMM32|
| 01001 | 0  |    |      | Jm   |  Rs |IF Rs, JUMP Jm   |
| 10000 | 1  |Imm17| Imm17| Imm17|    |   CALL IMM32|
| 10000 | 0  |    |      | Jm   |     |   CALL Jm   |
| 10001 | 1  |Imm17| Imm17| Imm17| Rs |IF Rs, CALL IMM32|
| 10001 | 0  |    |      | Jm   |  Rs |IF Rs, CALL Jm   |
| Others|    |    |      |      |     |             |

## SYN Encoding

TO BE DONE

[\<-- \[ISA\]](ISA) 
