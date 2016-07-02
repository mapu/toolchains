LLVM_BIN_PATH=${MAPU_HOME}/apc/bin

all:
	${LLVM_BIN_PATH}/llvm-mc -arch=mmpulite -filetype=obj app.m.asm  -o app.m.o
	${LLVM_BIN_PATH}/ld.gold -T ${MAPU_HOME}/apc/share/MaPU_ASM.m.ld -n -o app.m.out  app.m.o
	${LLVM_BIN_PATH}/llvm-objdump -arch-name=mmpulite -t -disassemble  -s app.m.out > app.m.dis
	${LLVM_BIN_PATH}/llvm-mc -arch=mspu -filetype=obj app.s.asm  -o app.s.o
	${LLVM_BIN_PATH}/ld.gold -T ${MAPU_HOME}/apc/share/MaPU_ASM.s.ld -n -o app.s.out  app.s.o --just-symbols app.m.out
	${LLVM_BIN_PATH}/llvm-objdump -arch-name=mspu -t -disassemble  -s app.s.out > app.s.dis

clean:
	@rm -rf *.lof  *.s.out *.m.out *.map  *.mmap  *.sct  *.tab  *.s  *.dis  *~  *.def *.xml *.data
	@rm -rf *.o *.sym *.txt *.ll stderr stdout m5out
