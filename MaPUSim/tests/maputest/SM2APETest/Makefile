ToolPath=../../sputools
LLVM_BIN_PATH=/opt/mapu-llvm/bin
include ../Makefile.Sim


APP:
	${LLVM_BIN_PATH}/llvm-mc -arch=mspu -filetype=obj app0.s.asm  -o app0.s.o
	${LLVM_BIN_PATH}/llvm-ld -T Script.ld -o app0.s.out  app0.s.o
	${LLVM_BIN_PATH}/llvm-objdump -arch=mspu -t -disassemble  -s app0.s.out > app0.s.dis

	${LLVM_BIN_PATH}/llvm-mc -arch=mspu -filetype=obj app1.s.asm  -o app1.s.o
	${LLVM_BIN_PATH}/llvm-ld -T Script.ld -o app1.s.out  app1.s.o
	${LLVM_BIN_PATH}/llvm-objdump -arch=mspu -t -disassemble  -s app1.s.out > app1.s.dis

clean:
	@rm -rf *.lof  *.out  *.map  *.mmap  *.sct  *.tab  *.s  *.dis  *~  *.def *.xml *.data
	@rm -rf simv*  work  *.lib++ *Log *.log *~ *.rc csrc *.key filelist.f
	@rm -rf *.vpd DVEfiles vc_hdrs.h urgReport
	@rm -rf *.o *.sym *.txt *.ll stderr stdout
