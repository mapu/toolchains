LLVM_BIN_PATH=${MAPU_HOME}/apc/bin

all:
	${LLVM_BIN_PATH}/llvm-mc -arch=mspu -filetype=obj app.s.asm  -o app.s.o
	${LLVM_BIN_PATH}/ld.gold -T ${MAPU_HOME}/apc/share/MaPU_ASM.s.ld -o app.s.out  app.s.o
#	${LLVM_BIN_PATH}/llvm-objdump -arch-name=mspu -t -disassemble  -s app.s.out > app.s.dis

clean:
	@rm -rf *.lof  *.s.out  *.map  *.mmap  *.sct  *.tab  *.s  *.dis  *~  *.def *.xml *.data
	@rm -rf *.o *.sym *.txt *.ll stderr stdout m5out
