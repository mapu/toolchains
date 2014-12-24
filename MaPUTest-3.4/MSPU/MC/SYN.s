#RUN: llvm-mc -arch=mspu -filetype=obj -o %t.o %s
#RUN: llvm-objdump -arch=mspu -d -no-show-raw-insn %t.o | sed '1,5d' | awk '$1=""; {print $0}' > %t.s
#RUN: diff -w -i -I '^[ \t]*$' -i -I '[ \t]*#.*' -I '\/\/.*' %t.s %s


//R to SVR 
SVR0 [ R1 ] = R3;;
SVR0 [ R1 ] = R3 (B);;
SVR0 [ R1 ] = R3 (H);;
//SVR to R
R3 = SVR0 [ R1 ];;
R3 = SVR0 [ R1 ] (B);;
R3 = SVR0 [ R1 ] (H);;
R3 = SVR0 [ R1 ] (B) (U);;
R3 = SVR0 [ R1 ] (H) (U);;
//R to SVR with IMM
SVR0 [ 1 ] = R3;;
SVR0 [ 1 ] = R3 (B);;
SVR0 [ 1 ] = R3 (H);;
//SVR to R with IMM
R3 = SVR0 [ 1 ];;
R3 = SVR0 [ 1 ] (B);;
R3 = SVR0 [ 1 ] (H);;
R3 = SVR0 [ 1 ] (B) (U);;
R3 = SVR0 [ 1 ] (H) (U);;
// MC transfer
R1 = MC.r0;;
MC.w1 = R1;;

R1 = MC.r1;;
MC.w2 = R1;;

// SVR <--> T
SHU0.T0 = SVR0;;
SHU0.T0 = SVR0;;
SHU0.T0 = SVR0;;
SVR1    = SHU1.T1;;
SVR1    = SHU1.T1;;

// R to T
SHU1.T3= VECT R2  ;;
SHU1.T3= VECT R2  (H);;
SHU1.T3= VECT R2  (B);;
SHU1.T3= VECT DR2 ;;

SHU1.T3= VECT R2  ;;
SHU1.T3= VECT R2  (H);;
SHU1.T3= VECT R2  (B);;
SHU1.T3= VECT DR2 ;;

SHU1.T3= VECT R2  ;;
SHU1.T3= VECT R2  (H);;
SHU1.T3= VECT R2  (B);;
SHU1.T3= VECT DR2 ;;

//T  To R
R2  = SHU1.T3;;
R2  = SHU1.T3(H);;
R2  = SHU1.T3(B);;
R2  = SHU1.T3(H)(U);;
R2  = SHU1.T3(B)(U);;
DR2 = SHU1.T3;;

R2  = SHU1.T3;;
R2  = SHU1.T3(H);;
R2  = SHU1.T3(B);;
R2  = SHU1.T3(H)(U);;
R2  = SHU1.T3(B)(U);;
DR2 = SHU1.T3;;

// K Register transfer
KB1 = R2;;
KE1 = R2;;
KS1 = R2;;
KI1 = R2;;
KM1 = R2(H);;
KM1 = R2;;
KG1 = R2;;
KL1 = R2;;

KB1 = R2;;
KE1 = R2;;
KS1 = R2;;
KI1 = R2;;
KM1 = R2(H);;
KM1 = R2;;
KG1 = R2;;
KL1 = R2;;

KB1 = R2;;
KE1 = R2;;
KS1 = R2;;
KI1 = R2;;
KM1 = R2(H);;
KM1 = R2;;
KG1 = R2;;
KL1 = R2;;

R1 = KB1;;
R2 = KE1;;
R2 = KS1;;
R2 = KI1;;
R2 = KM1;;
R2 = KM1(H);;
R2 = KG1;;
R2 = KL1;;

R1 = KB1;;
R2 = KE1;;
R2 = KS1;;
R2 = KI1;;
R2 = KM1;;
R2 = KM1(H);;
R2 = KG1;;
R2 = KL1;;

// CCtrl & CStat transfer
//CCtrl = R1;;
R1    = CStat;;
//CallM
CallM  J1;;
CallM  J1;;
CallM  J1 ;;
CallM  J1 ;;
CallM  J1(B);;
CallM  J1(B);;
CallM  J1(B);;
CallM  J1(B);;
CallM  328;;
CallM  328;;
CallM  328;;
CallM  328;;

CallM  328(B);;
CallM  328(B);;
CallM  328(B);;
CallM  328(B);;

