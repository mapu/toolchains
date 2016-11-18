#RUN: llvm-mc -arch=mspu -filetype=obj -o %t.o %s
#RUN: llvm-objdump -arch-name=mspu -d -no-show-raw-insn %t.o | sed '1,5d' | awk '$1=""; {print $0}' > %t.s
#RUN: diff -w -i -I '^[ \t]*$' -I '[ \t]*#.*' -I '\/\/.*' %t.s %s

m.s r1 = r3 + r5 ;;
m.s r1 = r3 - r5;;
m.s r1 = r3 + r5(CI) ;;
m.s r1 = r3 - r5(CI);;
m.s r1 = r3 * r5;;

m.s r1 = r3 + r5 (U);;
m.s r1 = r3 - r5(U);;
m.s r1 = r3 * r5(U);;

m.s r1 = r3 + r5 (T) ;;
m.s r1 = r3 - r5 (T);;
m.s r1 = r3 + r5 (T) (CI);;
m.s r1 = r3 - r5 (T)(CI);;
m.s r1 = r3 * r5 (T);;

m.s r1 = r3 + r5 (S);;
m.s r1 = r3 - r5 (S);;
m.s r1 = r3 * r5 (S);;

m.s r1 = r3 + r5 (S)(T);;
m.s r1 = r3 - r5 (S)(T);;
m.s r1 = r3 * r5 (S)(T);;

m.s DR1 = DR3 + DR5(D);;
m.s dr1 = dr3 - dr5(d);;
m.s dr1 = dr3 * dr5(d);;

m.s dr3 = abs dr4(d);;
m.s dr3 = rsqrt dr4(d);;
m.s dr3 = recip dr4(d);;
m.s r3 = abs r4(s);;
m.s r3 = abs r4;;


m.s r2 = int r4(s);;
m.s r2 = int dr4(d);;

m.s r2 = uint r4(s);;
m.s r2 = uint dr4(d);;

m.s r2 = single r4(u);;
m.s dr2 = double r4(s);;

m.s r2 = int dr4(d);;
m.s r2 = uint dr4(d);;
m.s r2 = single dr4(d);;
m.s dr2 = double r4(s);;

//Fix add
m.s R1 = R2 + R3;;
m.s R1 = R2 + R3(U);;
m.s R1 = R2 + R3(T);;
//Fix Sub
m.s R1 = R2 - R3;;
m.s R1 = R2 - R3(U);;
m.s R1 = R2 - R3(T);;
//Fix MUL
m.s R1 = R2 * R3;;
m.s R1 = R2 * R3(U);;
m.s R1 = R2 * R3(T);;
//Float add
m.s R1  = R2  + R3 (S);;
m.s R1  = R2  + R3 (S) (T);;
m.s DR1 = DR2 + DR3 (D);;
m.s DR1 = DR2 + DR3 (D) (T);;
//Float sub
m.s R1  = R2  - R3 (S);;
m.s R1  = R2  - R3 (S) (T);;
m.s DR1 = DR2 - DR3 (D);;
m.s DR1 = DR2 - DR3 (D) (T);;
//Float mul
m.s R1  = R2  * R3 (S);;
m.s R1  = R2  * R3 (S) (T);;
m.s DR1 = DR2 * DR3 (D);;
m.s DR1 = DR2 * DR3 (D) (T);;
//Fix to float
m.s R1 = single R2;;
m.s R1 = single R2(U);;
m.s R1 = single R2(T);;
m.s R1 = single R2(U) (T);;
//float to int
m.s R1 = INT   R2(S);;
m.s R1 = INT   R2(S) (T);;
m.s R1 = UINT  R2(S);;
m.s R1 = UINT  R2(S) (T);;
m.s R1 = INT   DR2 (D);;
m.s R1 = INT   DR2 (D) (T);;
m.s R1 = UINT  DR2 (D);;
m.s R1 = UINT  DR2 (D) (T);;
//single <--> double
m.s DR1 = DOUBLE R2 (S);;
m.s R1  = SINGLE DR2 (D);;
m.s R1  = SINGLE DR2 (D)(T);;
//ABS
m.s R1  = ABS R2;;
m.s R1  = ABS R2 (S);;
m.s DR1 = ABS DR2 (D);;
//RECIP
m.s R1  =  RECIP R2(S);;
m.s DR1 =  RECIP DR2 (D);;
//SQRT
m.s R1  = RSQRT R2 (S);;
m.s DR1 = RSQRT dR2 (D);;
//bit logic
m.s R1  = R2 & R3;;
m.s R1  = R2 | R3;;
m.s R1  = R2 ^ R3;;
m.s R1  = ~R2;;
//compare
m.s R1  = R2 == R3;;
m.s R1  = R2 != R3;;

m.s R1  = R2 >  R3;;
m.s R1  = R2 >  R3 (U);;
m.s R1  = R2 >= R3 ;;
m.s R1  = R2 >= R3 (U);;

m.s R1  = R2 <  R3;;
m.s R1  = R2 <  R3 (U);;
m.s R1  = R2 <= R3 ;;
m.s R1  = R2 <= R3 (U);;

m.s R1   = R2  == R3 (S);;
m.s R1  = DR2 == DR3 (D);;

m.s R1   = R2  != R3 (S);;
m.s R1  = DR2 != DR3 (D);;

m.s R1   = R2  >= R3 (S);;
m.s R1  = DR2 >= DR3 (D);;

m.s R1   = R2  <= R3 (S);;
m.s R1  = DR2 <= DR3 (D);;

//shift
m.s R1 = R2 << R3;;
m.s R1 = R2 << 3;;
m.s R1 = R2 >> R3;;
m.s R1 = R2 >> R3(U);;
m.s R1 = R2 >> 3;;
m.s R1 = R2 >> R3(U);;

