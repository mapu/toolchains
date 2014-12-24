//RUN: llvm-mc -arch=mmpulite -filetype=obj -o %t.o %s
//RUN: llvm-objdump -arch=mmpulite -d -no-show-raw-insn %t.o | sed '1,5d' | awk '$1=""; {print $0}' > %t.s
//RUN: diff -w -i -I '^[ \t]*$' -I '[ \t]*#.*' -I '\/\/.*' %t.s %s

////////////////////////////////////////
//Read port for r0
//The read port without bypass network
  r0.M[127]->SHU0.T1  ;
  r0.M[127]->SHU1.T2  ;
  r0.M[127]->M[1]    ;
  r0.M[I++]->SHU0.T1  ;
  r0.M[I++]->SHU1.T2  ;
  r0.M[I++]->M[1]    ;
  r0.M[s++]->SHU0.T1  ;
  r0.M[s++]->SHU1.T2  ;
  r0.M[s++]->M[1]    ;
//The read port with bypass network
  r0.M[127]->IALU.T1 (I0)  ;
  r0.M[127]->IMAC.T1 (I1)  ;
  r0.M[127]->FALU.T1 (I2)  ;
  r0.M[127]->FMAC.T1 (I0)  ;

/////////////////////////////////////////
//The readport for r1,without write to M
  r1.M[127]->SHU0.T1  ;
  r1.M[127]->SHU1.T2  ;
  r1.M[I++]->SHU0.T1  ;
  r1.M[I++]->SHU1.T2  ;
  r1.M[S++]->SHU0.T1  ;
  r1.M[S++]->SHU1.T2  ;
//The read port with bypass network
  r1.M[127]->IALU.T1 (I0)  ;
  r1.M[127]->IMAC.T1 (I1)  ;
  r1.M[127]->FALU.T1 (I2)  ;
  r1.M[127]->FMAC.T1 (I0)  ;

/////////////////////////////////////////
//The readport for r2, without write the SHU
//The read port with bypass network
  r2.M[127]->IALU.T1 (I0)  ;
  r2.M[127]->IMAC.T1 (I1)  ;
  r2.M[127]->FALU.T1 (I2)  ;
  r2.M[127]->FMAC.T1 (I0)  ;

/////////////////////////////////////////
//The readport for r3, without write the SHU
//The read port with bypass network
  M[127]->BIU0    ;
  M[127]->BIU0 (KG) ;
  M[127]->BIU1    ;
  M[127]->BIU1 (KG) ;
  M[127]->BIU2    ;
  M[127]->BIU2 (KG) ;
