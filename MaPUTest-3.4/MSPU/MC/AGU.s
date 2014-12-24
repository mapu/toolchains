#RUN: llvm-mc -arch=mspu -filetype=obj -o %t.o %s
#RUN: llvm-objdump -arch=mspu -d -no-show-raw-insn %t.o | sed '1,5d' | awk '$1=""; {print $0}' > %t.s
#RUN: diff -w -i -I '^[ \t]*$' -I '[ \t]*#.*' -I '\/\/.*' %t.s %s

#RUN: llvm-mc %s -arch=mspu -show-encoding | FileCheck %s

#CHECK: encoding: [0xc3,0x01,0xc2,0x30,0x14,0xc6,0x0b,0xd0]
J3 = 12345678;;

r4 = 123;;
j3 = [j3+ 213];;
r3 = [j7+32132131];;

//Address computing
J1 = J2 + J3;;
J1 = J2 - J3;;
J1 = J2 << 5;;
J1 = J2 <<J3;;
//register transfer
R1 = R2;;
R1 = J2;;
J1 = R2;;
J1 = J2;;
DR1 = DR2;;
// channel command
ch[1] = R2;;
ch[2] = J2;;
ch[R1] = R2;;
ch[R1] = J2;;
ch[J1] = R2;;
ch[J1] = J2;;

J1 = ch[2];;
J1 = ch[2] (N);;
R1 = ch[2] ;;
R1 = ch[2] (N);;
J1 = ch[r2];;
J1 = ch[r2](N);;
J1 = ch[J2];;
J1 = ch[J2] (N);;
R1 = ch[R2];;
R1 = ch[R2] (N);;
R1 = ch[J2];;
R1 = ch[J2] (N);;
//Imm assign
R1 = 10;;
R1 = -10;;
J1 = 10;;
J1 = -10;;
//load instruction
R1 = [J2 + J3];;
DR1= [J2 + J3];;
R1 = [J2 + J3](B);;
R1 = [J2 + J3](H);;
R1 = [J2 + J3](B) (U);;
R1 = [J2 + J3](H) (U);;
J1 = [J2 + J3];;
J1 = [J2 + J3](B);;
J1 = [J2 + J3](H);;
J1 = [J2 + J3](B) (U);;
J1 = [J2 + J3](H) (U);;

R1 = [J2 + 10];;
DR1= [J2 + 10];;
R1 = [J2 + 10](B);;
R1 = [J2 + 10](H);;
R1 = [J2 + 10](B) (U);;
R1 = [J2 + -10](H) (U);;
J1 = [J2 + -10];;
J1 = [J2 + -10](B);;
J1 = [J2 + -10](H);;
J1 = [J2 + -10](B) (U);;
J1 = [J2 + -10](H) (U);;

R1 = [J2 += J3];;
DR1= [J2 += J3];;
R1 = [J2 += J3](B);;
R1 = [J2 += J3](H);;
R1 = [J2 += J3](B) (U);;
R1 = [J2 += J3](H) (U);;
J1 = [J2 += J3];;
J1 = [J2 += J3](B);;
J1 = [J2 += J3](H);;
J1 = [J2 += J3](B) (U);;
J1 = [J2 += J3](H) (U);;

R1 = [J2 += 10];;
DR1= [J2 += 10];;
R1 = [J2 += 10](B);;
R1 = [J2 += 10](H);;
R1 = [J2 += 10](B) (U);;
R1 = [J2 += -10](H) (U);;
J1 = [J2 += -10];;
J1 = [J2 += -10](B);;
J1 = [J2 += -10](H);;
J1 = [J2 += -10](B) (U);;
J1 = [J2 += -10](H) (U);;
////store instruction
[J2 + J3]= R1 ;;
[J2 + J3]= DR1;;
[J2 + J3]= R1 (B);;
[J2 + J3]= R1 (H);;
[J2 + J3]= J1 ;;
[J2 + J3]= J1 (B);;
[J2 + J3]= J1 (H);;

[J2 + 10] = R1 ;;
[J2 + 10] = DR1;;
[J2 + 10] = R1 (B);;
[J2 + 10] = R1 (H);;
[J2 + -10]= J1 ;;
[J2 + -10]= J1 (B);;
[J2 + -10]= J1 (H);;

[J2 += J3]= R1 ;;
[J2 += J3]= DR1;;
[J2 += J3]= R1 (B);;
[J2 += J3]= R1 (H);;
[J2 += J3]= J1 ;;
[J2 += J3]= J1 (B);;
[J2 += J3]= J1 (H);;

[J2 += 10] = R1 ;;
[J2 += 10] = DR1;;
[J2 += 10] = R1 (B);;
[J2 += 10] = R1 (H);;
[J2 += -10]= J1 ;;
[J2 += -10]= J1 (B);;
[J2 += -10]= J1 (H);;
////atomic load/store
R1 = [ J2 + J3 ] (AT);;
J1 = [ J2 + J3 ] (AT);;
[J2 + J3 ] = R1  (AT);;
[J2 + J3 ] = J1  (AT);;

R1 = [ J2 += J3 ] (AT);;
J1 = [ J2 += J3 ] (AT);;
[J2 += J3 ] = R1  (AT);;
[J2 += J3 ] = J1  (AT);;
////vector load/store
SVR1 = [ J2 + J3];;
SVR1 = [ J2 += J3];;
[J2 + J3 ] = SVR0;;
[J2 +=J3 ] = SVR0;;

