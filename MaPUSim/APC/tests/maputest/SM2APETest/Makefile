LLVM_BIN_PATH=${MAPU_HOME}/apc/bin

all:
	${LLVM_BIN_PATH}/llvm-mc -arch=mspu -filetype=obj app0.s.asm  -o app0.s.o
	${LLVM_BIN_PATH}/llvm-ld -T ${MAPU_HOME}/apc/share/MaPU_ASM.s.ld -n -o app0.s.out  app0.s.o
	${LLVM_BIN_PATH}/llvm-objdump -arch=mspu -t -disassemble  -s app0.s.out > app0.s.dis
	${LLVM_BIN_PATH}/llvm-mc -arch=mspu -filetype=obj app1.s.asm  -o app1.s.o
	${LLVM_BIN_PATH}/llvm-ld -T ${MAPU_HOME}/apc/share/MaPU_ASM.s.ld -n -o app1.s.out  app1.s.o
	${LLVM_BIN_PATH}/llvm-objdump -arch=mspu -t -disassemble  -s app1.s.out > app1.s.dis

clean:
	@rm -rf *.lof  *.s.out  *.map  *.mmap  *.sct  *.tab  *.s  *.dis  *~  *.def *.xml *.data
	@rm -rf *.o *.sym *.txt *.ll stderr stdout m5out
