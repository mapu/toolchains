//RUN: llvm-mc -arch=mmpulite -filetype=obj -o %t.o %s
//RUN: llvm-objdump -arch=mmpulite -d -no-show-raw-insn %t.o | sed '1,5d' | awk '$1=""; {print $0}' > %t.s
//RUN: diff -w -i -I '^[ \t]*$' -I '[ \t]*#.*' -I '\/\/.*' %t.s %s

//Load Instruction
//Destination Test
 BIU0.DM->SHU0.T0         ;
 BIU1.DM->SHU1.T0         ;
 BIU2.DM->M[127]         ;
 BIU0.DM->M[I++]       ;
 BIU1.DM->IALU.T0  (I0)   ;
 BIU2.DM->FALU.T0  (I1)   ;
 BIU0.DM->IMAC.T0  (I2)   ;
 BIU1.DM->FMAC.T0  (I0)   ;

//Option Test
 BIU0.DM (M) ->SHU0.T0         ;
 BIU0.DM (BR) ->SHU0.T0         ;
 BIU0.DM (A++) ->SHU0.T0         ;
 BIU0.DM (K++) ->SHU0.T0         ;
 BIU0.DM (BR,M) ->SHU0.T0         ;
 BIU0.DM (A++,BR,M) ->SHU0.T0         ;
 BIU0.DM (A++,K++,BR,M)->SHU0.T0         ;
 BIU0.DM (A++,K++)->SHU0.T0         ;

//Store Instruction
BIU0->DM(M);
BIU1->DM(A++);
BIU2->DM(K++);
BIU0->DM(A++,M);
BIU1->DM(A++,K++,M);
BIU2->DM(A++,K++);
