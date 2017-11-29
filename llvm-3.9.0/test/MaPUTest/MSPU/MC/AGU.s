#RUN: llvm-mc -arch=mspu -filetype=obj -o %t.o %s
#RUN: llvm-objdump -arch-name=mspu -d -no-show-raw-insn %t.o | sed '1,5d' | awk '$1=""; {print $0}' > %t.s
#RUN: diff -w -i -I '^[ \t]*$' -I '[ \t]*#.*' -I '\/\/.*' %t.s %s

#RUN: llvm-mc %s -arch=mspu -show-encoding | FileCheck %s

#CHECK: encoding: [0xc3,0x01,0xc2,0x30,0x14,0xc6,0x0b,0xd0]
m.s J3 = 12345678;;

m.s r4 = 123;;
m.s j3 = [j3+ 213];;
m.s r3 = [j7+32132131];;

//Address computing
m.s J1 = J2 + J3;;
m.s J1 = J2 - J3;;
m.s J1 = J2 << 5;;
m.s J1 = J2 <<J3;;
//register transfer
m.s R1 = R2;;
m.s R1 = J2;;
m.s J1 = R2;;
m.s J1 = J2;;
m.s DR1 = DR2;;
// channel command
m.s ch[1] = R2;;
m.s ch[2] = J2;;
m.s ch[R1] = R2;;
m.s ch[R1] = J2;;
m.s ch[J1] = R2;;
m.s ch[J1] = J2;;

m.s J1 = ch[2];;
m.s J1 = ch[2] (N);;
m.s R1 = ch[2] ;;
m.s R1 = ch[2] (N);;
m.s J1 = ch[r2];;
m.s J1 = ch[r2](N);;
m.s J1 = ch[J2];;
m.s J1 = ch[J2] (N);;
m.s R1 = ch[R2];;
m.s R1 = ch[R2] (N);;
m.s R1 = ch[J2];;
m.s R1 = ch[J2] (N);;
//Imm assign
m.s R1 = 10;;
m.s R1 = -10;;
m.s J1 = 10;;
m.s J1 = -10;;
//load instruction
m.s R1 = [J2 + J3];;
m.s DR1= [J2 + J3];;
m.s R1 = [J2 + J3](B);;
m.s R1 = [J2 + J3](H);;
m.s R1 = [J2 + J3](B) (U);;
m.s R1 = [J2 + J3](H) (U);;
m.s J1 = [J2 + J3];;
m.s J1 = [J2 + J3](B);;
m.s J1 = [J2 + J3](H);;
m.s J1 = [J2 + J3](B) (U);;
m.s J1 = [J2 + J3](H) (U);;

m.s R1 = [J2 + 10];;
m.s DR1= [J2 + 10];;
m.s R1 = [J2 + 10](B);;
m.s R1 = [J2 + 10](H);;
m.s R1 = [J2 + 10](B) (U);;
m.s R1 = [J2 + -10](H) (U);;
m.s J1 = [J2 + -10];;
m.s J1 = [J2 + -10](B);;
m.s J1 = [J2 + -10](H);;
m.s J1 = [J2 + -10](B) (U);;
m.s J1 = [J2 + -10](H) (U);;

m.s R1 = [J2 += J3];;
m.s DR1= [J2 += J3];;
m.s R1 = [J2 += J3](B);;
m.s R1 = [J2 += J3](H);;
m.s R1 = [J2 += J3](B) (U);;
m.s R1 = [J2 += J3](H) (U);;
m.s J1 = [J2 += J3];;
m.s J1 = [J2 += J3](B);;
m.s J1 = [J2 += J3](H);;
m.s J1 = [J2 += J3](B) (U);;
m.s J1 = [J2 += J3](H) (U);;

m.s R1 = [J2 += 10];;
m.s DR1= [J2 += 10];;
m.s R1 = [J2 += 10](B);;
m.s R1 = [J2 += 10](H);;
m.s R1 = [J2 += 10](B) (U);;
m.s R1 = [J2 += -10](H) (U);;
m.s J1 = [J2 += -10];;
m.s J1 = [J2 += -10](B);;
m.s J1 = [J2 += -10](H);;
m.s J1 = [J2 += -10](B) (U);;
m.s J1 = [J2 += -10](H) (U);;
////store instruction
m.s [J2 + J3]= R1 ;;
m.s [J2 + J3]= DR1;;
m.s [J2 + J3]= R1 (B);;
m.s [J2 + J3]= R1 (H);;
m.s [J2 + J3]= J1 ;;
m.s [J2 + J3]= J1 (B);;
m.s [J2 + J3]= J1 (H);;

m.s [J2 + 10] = R1 ;;
m.s [J2 + 10] = DR1;;
m.s [J2 + 10] = R1 (B);;
m.s [J2 + 10] = R1 (H);;
m.s [J2 + -10]= J1 ;;
m.s [J2 + -10]= J1 (B);;
m.s [J2 + -10]= J1 (H);;

m.s [J2 += J3]= R1 ;;
m.s [J2 += J3]= DR1;;
m.s [J2 += J3]= R1 (B);;
m.s [J2 += J3]= R1 (H);;
m.s [J2 += J3]= J1 ;;
m.s [J2 += J3]= J1 (B);;
m.s [J2 += J3]= J1 (H);;

m.s [J2 += 10] = R1 ;;
m.s [J2 += 10] = DR1;;
m.s [J2 += 10] = R1 (B);;
m.s [J2 += 10] = R1 (H);;
m.s [J2 += -10]= J1 ;;
m.s [J2 += -10]= J1 (B);;
m.s [J2 += -10]= J1 (H);;
////atomic load/store
m.s R1 = [ J2 + J3 ] (AT);;
m.s J1 = [ J2 + J3 ] (AT);;
m.s [J2 + J3 ] = R1  (AT);;
m.s [J2 + J3 ] = J1  (AT);;

m.s R1 = [ J2 += J3 ] (AT);;
m.s J1 = [ J2 += J3 ] (AT);;
m.s [J2 += J3 ] = R1  (AT);;
m.s [J2 += J3 ] = J1  (AT);;
////vector load/store
m.s SVR1 = [ J2 + J3];;
m.s SVR1 = [ J2 += J3];;
m.s [J2 + J3 ] = SVR0;;
m.s [J2 +=J3 ] = SVR0;;

