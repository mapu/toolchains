# Instructions for SCU
SCU stands for **S**calar **C**omputing **U**nit, which supports most general computation related instructions.

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
| Rs = Rm != Rn                 | Not Equal                     |   EX2             | 
| Rs = Rm >  Rn {(U)}           | Big                           |   EX2             | 
| Rs = Rm >= Rn {(U)}           | Big or equal                  |   EX2             | 
| Rs = Rm <  Rn {(U)}           | Less                          |   EX2             | 
| Rs = Rm <= Rn {(U)}           | Less or equal                 |   EX2             | 
| Rs = Rm \<\< Rn               | Logic left shift              |   EX2             | 
| Rs = Rm \<\< Imm5             | Logic left shift with Imm     |   EX2             | 
| Rs = Rm \>\> Rn               | Logic right shift             |   EX2             | 
| Rs = Rm \>\> Imm5             | Logic right shift with Imm    |   EX2             | 

## 32 bits Float

| Assembly                      | Operation                                 |  Completing stage |
| ------------------------------| ----------------                          | ---------------   |
| Rs = Rm + Rn {(T)} (S)        | 32 bits float add                         |   EX5             | 
| Rs = Rm - Rn {(T)} (S)        | 32 bits float sub                         |   EX5             | 
| Rs = Rm * Rn {(T)} (S)        | 32 bits float mul                         |   EX5             | 
| Rs = ABS Rm        (S)        | 32 bits float absolute value              |   EX5             | 
| Rs = RECIP Rm      (S)        | 32 bits float reciprocal                  |   EX5             | 
| Rs = RSQRT Rm      (S)        | 32 bits float reciprocal square root      |   EX5             | 
| Rs = Rm >  Rn      (S)        | Big                                       |   EX5             | 
| Rs = Rm >= Rn      (S)        | Big or equal                              |   EX5             | 
| Rs = Rm <  Rn      (S)        | Less                                      |   EX5             | 
| Rs = Rm <= Rn      (S)        | Less or equal                             |   EX5             | 
| Rs = SINGLE Rm {(U)}{(T)}     | Convert integer to single float           |   EX5             | 
| Rs = INT    Rm {(T)}(S)       | Convert single float to integer           |   EX5             | 

## 64 bits Float

| Assembly                      | Operation                                 |  Completing stage |
| ------------------------------| ----------------                          | ---------------   |
| DRs = DRm + DRn {(T)} (D)     | 64 bits float add                         |   EX5             | 
| DRs = DRm - DRn {(T)} (D)     | 64 bits float sub                         |   EX5             | 
| DRs = DRm * DRn {(T)} (D)     | 64 bits float mul                         |   EX5             | 
| DRs = ABS DRm         (D)     | 64 bits float absolute value              |   EX5             | 
| DRs = RECIP DRm       (D)     | 64 bits float reciprocal                  |   EX5             | 
| DRs = RSQRT DRm       (D)     | 64 bits float reciprocal square root      |   EX5             | 
| DRs = DRm >  DRn      (D)     | Big                                       |   EX5             | 
| DRs = DRm >= DRn      (D)     | Big or equal                              |   EX5             | 
| DRs = DRm <  DRn      (D)     | Less                                      |   EX5             | 
| DRs = DDRm <= DRn     (D)     | Less or equal                             |   EX5             | 
| DRs = DOUBLE  Rm {(U)}        | Convert integer to double float           |   EX5             | 
| DRs = DOUBLE  Rm      (S)     | Convert single float to double float      |   EX5             | 
|  Rs = INT    DRm {(T)}(D)     | Convert double float to integer           |   EX5             | 
| Rs  = SINGLE DRm {(T)}(D)     | Convert double float to single float      |   EX5             | 

[\<-- \[ISA\]](ISA)  [\[AGU\] -->](AGU)
