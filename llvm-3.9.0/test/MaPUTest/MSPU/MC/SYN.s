#RUN: llvm-mc -arch=mspu -filetype=obj -o %t.o %s
#RUN: llvm-objdump -arch-name=mspu -d -no-show-raw-insn %t.o | sed '1,5d' | awk '$1=""; {print $0}' > %t.s
#RUN: diff -w -i -I '^[ \t]*$' -i -I '[ \t]*#.*' -I '\/\/.*' %t.s %s


//R to SVR 
m.s SVR0 [ R1 ] = R3;;
m.s SVR0 [ R1 ] = R3 (B);;
m.s SVR0 [ R1 ] = R3 (H);;
//SVR to R
m.s R3 = SVR0 [ R1 ];;
m.s R3 = SVR0 [ R1 ] (B);;
m.s R3 = SVR0 [ R1 ] (H);;
m.s R3 = SVR0 [ R1 ] (B) (U);;
m.s R3 = SVR0 [ R1 ] (H) (U);;
//R to SVR with IMM
m.s SVR0 [ 1 ] = R3;;
m.s SVR0 [ 1 ] = R3 (B);;
m.s SVR0 [ 1 ] = R3 (H);;
//SVR to R with IMM
m.s R3 = SVR0 [ 1 ];;
m.s R3 = SVR0 [ 1 ] (B);;
m.s R3 = SVR0 [ 1 ] (H);;
m.s R3 = SVR0 [ 1 ] (B) (U);;
m.s R3 = SVR0 [ 1 ] (H) (U);;
// MC transfer
m.s R1 = MC.r0;;
m.s MC.w1 = R1;;

m.s R1 = MC.r1;;
m.s MC.w2 = R1;;

// SVR <--> T
m.s SHU0.T0 = SVR0;;
m.s SHU0.T0 = SVR0;;
m.s SHU0.T0 = SVR0;;
m.s SVR1    = SHU1.T1;;
m.s SVR1    = SHU1.T1;;

// R to T
m.s SHU1.T3= VECT R2  ;;
m.s SHU1.T3= VECT R2  (H);;
m.s SHU1.T3= VECT R2  (B);;
m.s SHU1.T3= VECT DR2 ;;

m.s SHU1.T3= VECT R2  ;;
m.s SHU1.T3= VECT R2  (H);;
m.s SHU1.T3= VECT R2  (B);;
m.s SHU1.T3= VECT DR2 ;;

m.s SHU1.T3= VECT R2  ;;
m.s SHU1.T3= VECT R2  (H);;
m.s SHU1.T3= VECT R2  (B);;
m.s SHU1.T3= VECT DR2 ;;

//T  To R
m.s R2  = SHU1.T3;;
m.s R2  = SHU1.T3(H);;
m.s R2  = SHU1.T3(B);;
m.s R2  = SHU1.T3(H)(U);;
m.s R2  = SHU1.T3(B)(U);;
m.s DR2 = SHU1.T3;;

m.s R2  = SHU1.T3;;
m.s R2  = SHU1.T3(H);;
m.s R2  = SHU1.T3(B);;
m.s R2  = SHU1.T3(H)(U);;
m.s R2  = SHU1.T3(B)(U);;
m.s DR2 = SHU1.T3;;

// K Register transfer
m.s KB1 = R2;;
m.s KE1 = R2;;
m.s KS1 = R2;;
m.s KI1 = R2;;
m.s KM1 = R2(H);;
m.s KM1 = R2;;
m.s KG1 = R2;;
m.s KL1 = R2;;

m.s KB1 = R2;;
m.s KE1 = R2;;
m.s KS1 = R2;;
m.s KI1 = R2;;
m.s KM1 = R2(H);;
m.s KM1 = R2;;
m.s KG1 = R2;;
m.s KL1 = R2;;

m.s KB1 = R2;;
m.s KE1 = R2;;
m.s KS1 = R2;;
m.s KI1 = R2;;
m.s KM1 = R2(H);;
m.s KM1 = R2;;
m.s KG1 = R2;;
m.s KL1 = R2;;

m.s R1 = KB1;;
m.s R2 = KE1;;
m.s R2 = KS1;;
m.s R2 = KI1;;
m.s R2 = KM1;;
m.s R2 = KM1(H);;
m.s R2 = KG1;;
m.s R2 = KL1;;

m.s R1 = KB1;;
m.s R2 = KE1;;
m.s R2 = KS1;;
m.s R2 = KI1;;
m.s R2 = KM1;;
m.s R2 = KM1(H);;
m.s R2 = KG1;;
m.s R2 = KL1;;

// CCtrl & CStat transfer
//CCtrl = R1;;
m.s R1    = CStat;;
//CallM
m.s CallM  J1;;
m.s CallM  J1;;
m.s CallM  J1 ;;
m.s CallM  J1 ;;
m.s CallM  J1(B);;
m.s CallM  J1(B);;
m.s CallM  J1(B);;
m.s CallM  J1(B);;
m.s CallM  328;;
m.s CallM  328;;
m.s CallM  328;;
m.s CallM  328;;

m.s CallM  328(B);;
m.s CallM  328(B);;
m.s CallM  328(B);;
m.s CallM  328(B);;

