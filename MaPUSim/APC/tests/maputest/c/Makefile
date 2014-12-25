LLVM_BIN_PATH=${MAPU_HOME}/apc/bin

all:
	${LLVM_BIN_PATH}/clang -target mspu add.c -g -o app.s.out
	${LLVM_BIN_PATH}/clang -target mspu add.c -S -o add.s
	${LLVM_BIN_PATH}/llvm-objdump -arch=mspu -t -disassemble  app.s.out > app.s.dis

clean:
	@rm -rf *.lof  *.s.out  *.map  *.mmap  *.sct  *.tab  *.s  *.dis  *~  *.def *.xml *.data
	@rm -rf *.o *.sym *.txt *.ll stderr stdout m5out
