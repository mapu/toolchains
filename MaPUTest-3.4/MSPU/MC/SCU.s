#RUN: llvm-mc -arch=mspu -filetype=obj -o %t.o %s
#RUN: llvm-objdump -arch=mspu -d -no-show-raw-insn %t.o | sed '1,5d' | awk '$1=""; {print $0}' > %t.s
#RUN: diff -w -i -I '^[ \t]*$' -I '[ \t]*#.*' -I '\/\/.*' %t.s %s

r1 = r3 + r5 ;;
r1 = r3 - r5;;
r1 = r3 + r5(CI) ;;
r1 = r3 - r5(CI);;
r1 = r3 * r5;;

r1 = r3 + r5 (U);;
r1 = r3 - r5(U);;
r1 = r3 * r5(U);;

r1 = r3 + r5 (T) ;;
r1 = r3 - r5 (T);;
r1 = r3 + r5 (T) (CI);;
r1 = r3 - r5 (T)(CI);;
r1 = r3 * r5 (T);;

r1 = r3 + r5 (S);;
r1 = r3 - r5 (S);;
r1 = r3 * r5 (S);;

r1 = r3 + r5 (S)(T);;
r1 = r3 - r5 (S)(T);;
r1 = r3 * r5 (S)(T);;

DR1 = DR3 + DR5(D);;
dr1 = dr3 - dr5(d);;
dr1 = dr3 * dr5(d);;

dr3 = abs dr4(d);;
dr3 = rsqrt dr4(d);;
dr3 = recip dr4(d);;
r3 = abs r4(s);;
r3 = abs r4;;


r2 = int r4(s);;
r2 = int dr4(d);;

r2 = uint r4(s);;
r2 = uint dr4(d);;

r2 = single r4(u);;
dr2 = double r4(s);;

r2 = int dr4(d);;
r2 = uint dr4(d);;
r2 = single dr4(d);;
dr2 = double r4(s);;

	//Fix add
R1 = R2 + R3;;
R1 = R2 + R3(U);;
R1 = R2 + R3(T);;
//Fix Sub
R1 = R2 - R3;;
R1 = R2 - R3(U);;
R1 = R2 - R3(T);;
//Fix MUL
R1 = R2 * R3;;
R1 = R2 * R3(U);;
R1 = R2 * R3(T);;
   //Float add
R1  = R2  + R3 (S);;
R1  = R2  + R3 (S) (T);;
DR1 = DR2 + DR3 (D);;
DR1 = DR2 + DR3 (D) (T);;
//Float sub
R1  = R2  - R3 (S);;
R1  = R2  - R3 (S) (T);;
DR1 = DR2 - DR3 (D);;
DR1 = DR2 - DR3 (D) (T);;
//Float mul
R1  = R2  * R3 (S);;
R1  = R2  * R3 (S) (T);;
DR1 = DR2 * DR3 (D);;
DR1 = DR2 * DR3 (D) (T);;
//Fix to float
R1 = single R2;;
R1 = single R2(U);;
R1 = single R2(T);;
R1 = single R2(U) (T);;
//float to int
R1 = INT   R2(S);;
R1 = INT   R2(S) (T);;
R1 = UINT  R2(S);;
R1 = UINT  R2(S) (T);;
R1 = INT   DR2 (D);;
R1 = INT   DR2 (D) (T);;
R1 = UINT  DR2 (D);;
R1 = UINT  DR2 (D) (T);;
//single <--> double
DR1 = DOUBLE R2 (S);;
R1  = SINGLE DR2 (D);;
R1  = SINGLE DR2 (D)(T);;
//ABS
R1  = ABS R2;;
R1  = ABS R2 (S);;
DR1 = ABS DR2 (D);;
//RECIP
R1  =  RECIP R2(S);;
DR1 =  RECIP DR2 (D);;
//SQRT
R1  = RSQRT R2 (S);;
DR1 = RSQRT dR2 (D);;
//bit logic
R1  = R2 & R3;;
R1  = R2 | R3;;
R1  = R2 ^ R3;;
R1  = ~R2;;
//compare
R1  = R2 == R3;;
R1  = R2 != R3;;

R1  = R2 >  R3;;
R1  = R2 >  R3 (U);;
R1  = R2 >= R3 ;;
R1  = R2 >= R3 (U);;

R1  = R2 <  R3;;
R1  = R2 <  R3 (U);;
R1  = R2 <= R3 ;;
R1  = R2 <= R3 (U);;

R1   = R2  == R3 (S);;
R1  = DR2 == DR3 (D);;

R1   = R2  != R3 (S);;
R1  = DR2 != DR3 (D);;

R1   = R2  >= R3 (S);;
R1  = DR2 >= DR3 (D);;

R1   = R2  <= R3 (S);;
R1  = DR2 <= DR3 (D);;

//shift
R1 = R2 << R3;;
R1 = R2 << 3;;
R1 = R2 >> R3;;
R1 = R2 >> R3(U);;
R1 = R2 >> 3;;
R1 = R2 >> R3(U);;

