
# Instructions for SEQ
SEQ stands for instruction **SEQ**uence, which supports most general jump and loop related instructions.


| Assembly                      | Operation                     |  Completing stage |
| ------------------------------| ----------------              | ---------------   |
| {IF Rm,} JUMP Imm32|Jm        | jump                          |   RR              | 
| {IF Rm,} CALL Imm32|Jm        | call                          |   EX1             | 
| NOP                           |                               |   RR              | 
| DbgBreak                      | used for soft breakpoin       |   DP              |
| others                        | to be explained in future     |   -               |

[\<------ \[ISA\]](ISA)         <span style="float:right"> [\[SYN\] ------>](SYN) </span>
