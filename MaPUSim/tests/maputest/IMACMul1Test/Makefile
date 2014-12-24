ToolPath=../../sputools
LLVM_BIN_PATH=/opt/mapu-llvm/bin
include ../Makefile.Sim


APP:
	${LLVM_BIN_PATH}/llvm-mc -arch=mspu -filetype=obj app.s.asm  -o app.s.o
	${LLVM_BIN_PATH}/llvm-mc -arch=mmpu -filetype=obj app.m.asm  -o app.m.o

	${LLVM_BIN_PATH}/llvm-ld         -T Script.ld      -o app.m.out  app.m.o; 
	${LLVM_BIN_PATH}/llvm-ld         -T Script.ld      -o app.s.out  app.s.o  --just-symbols app.m.o;

	${LLVM_BIN_PATH}/llvm-objdump -arch=mspu -t -disassemble  -s app.s.out > app.s.dis
	${LLVM_BIN_PATH}/llvm-objdump -arch=mmpu -t -disassemble  -s app.m.out > app.m.dis

clean:
	@rm -rf *.lof  *.out  *.map  *.mmap  *.sct  *.tab  *.s  *.dis  *~  *.def *.xml *.data
	@rm -rf simv*  work  *.lib++ *Log *.log *~ *.rc csrc *.key filelist.f
	@rm -rf *.vpd DVEfiles vc_hdrs.h urgReport
	@rm -rf *.o *.sym *.txt *.ll 
