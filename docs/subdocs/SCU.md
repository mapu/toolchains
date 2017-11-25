# Instructions for SCU
SCU stands for **S**calar **C**omputing **U**nit, which supports most general computation related instructions.
## Supported instruction options
* **(U)** Unsinged operations. 
* **(T)** Saturate ( rounding) or Truncate the result if it is larger then the maximum values.
* **(B)** 8 bits operation
* **(H)** 16 bits operation
* **(S)** 32 bits float operation
* **(D)** 64 bits float operation
* Without any options, default behavior is 32 bits, integer, truncate operation

## Arithmetic

| Assembly                      | Operation                     |  Completing stage |
| ------------------------------| ----------------              | ---------------   |
| Rs = Rm + Rn {(U)} {(T)}      | Integer add                   |   EX2             | 
| Rs = Rm - Rn {(U)} {(T)}      | Integer sub                   |   EX2             | 
| Rs = Rm + Rn {(U)} {(T)} CI   | Integer add with carry        |   EX2             | 
| Rs = Rm - Rn {(U)} {(T)} CI   | Integer sub with carry        |   EX2             | 
| Rs = Rm - Rn {(U)} {(T)} CI   | Integer sub with carry        |   EX2             | 
| Rs = Rm * Rn {(U)}            | Integer multiply              |   EX4             | 

## Logic

| Assembly                      | Operation                     |  Completing stage |
| ------------------------------| ----------------              | ---------------   |
| Rs = ABS Rm                   | Absolute value                |   EX2             | 
| Rs = Rm & Rn                  | Bitwise and                   |   EX2             | 
| Rs = Rm &#124; Rn             | Bitwise or                    |   EX2             | 
| Rs = Rm ^ Rn                  | Bitwise xor                   |   EX2             | 
| Rs = ~ Rm                     | Not                           |   EX2             | 
| Rs = Rm == Rn                 | Equal                         |   EX2             | 
| Rs = Rm != Rn                 | Equal                         |   EX2             | 
| Rs = Rm >  Rn {(U)}           | Big                           |   EX2             | 
| Rs = Rm >= Rn {(U)}           | Big or equal                  |   EX2             | 
| Rs = Rm <  Rn {(U)}           | Less                          |   EX2             | 
| Rs = Rm <= Rn {(U)}           | Less or equal                 |   EX2             | 
| Rs = Rm \<\< Rn               | Logic left shift              |   EX2             | 
| Rs = Rm \<\< Imm5             | Logic left shift with Imm     |   EX2             | 
| Rs = Rm \>\> Rn               | Logic right shift             |   EX2             | 
| Rs = Rm \>\> Imm5             | Logic right shift with Imm    |   EX2             | 

