
# Instructions for AGU
AGU stands for **A**ddress **G**eneration **U**nit, which supports most general load/store related instructions.

## Arithmetic

| Assembly                      | Operation                     |  Completing stage |
| ------------------------------| ----------------              | ---------------   |
| Js = Jm + Jn                  | Integer add                   |   EX2             | 
| Js = Jm - Jn                  | Integer sub                   |   EX2             | 
| Js = Jm \<\< Jn               | Integer left shift            |   EX2             | 
| Js = Jm \<\< Imm5             | Integer left shift with Imm   |   EX2             | 

## Register movement

| Assembly                      | Operation                     |  Completing stage |
| ------------------------------| ----------------              | ---------------   |
| Rs|Js = Rm|Jm                 | 32 bits register move         |   EX2             | 
| DRs     = DRm                 | 64 bits register move         |   EX2             | 
| Rs      = STAT                | Stat register move            |   EX2             | 
| Rs|Js = Imm32                 | Imm assign                    |   EX2             | 

STAT is a 32 status register，the zero bit is defined as the CI. Other bits are not defined.

## Load             

| Assembly                                          | Operation                     |  Completing stage |
| --------------------------------------------------| ----------------              | ---------------   |
| Rs|Js = [ Jm  +|+= Jn|Imm32 ] { (B)|(H) } {(U)}   | 32 bits load                  |   EX2 / EX5       | 
| DRs   = [ Jm  +|+= Jn|Imm32 ]                     | 64 bits load                  |   EX2 / EX5       | 
| [ Jm  +|+= Jn|Imm32 ] = Rs|Js { (B)|(H) }         | 32 bits store                 |   EX2 / EX5       | 
| [ Jm  +|+= Jn|Imm32 ] = DRs                       | 64 bits store                 |   EX2 / EX5       | 
| Others                                            | To be explained in further    |   -               | 

* **+=** : When with '+=', it means the Jm register will be updated with Jm + Jn in EX2 stage.
[\<-- \[ISA\]](ISA)  [\[SEQ\] -->](SEQ)
