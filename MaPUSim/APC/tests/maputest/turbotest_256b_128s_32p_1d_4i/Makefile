ToolPath=../../sputools
LLVM_BIN_PATH=/opt/mapu-llvm-3.3/bin
include ../Makefile.Sim

APP:
	${LLVM_BIN_PATH}/clang -target mspu add.c -g -O0 -o app.s.out
	${LLVM_BIN_PATH}/clang -target mspu add.c -S -o add.s
	${LLVM_BIN_PATH}/llvm-objdump -arch-name=mspu -t -disassemble  app.s.out > app.s.dis


ASM:
	${LLVM_BIN_PATH}/llvm-mc -arch=mspu -filetype=obj app.s.asm  -o app.s.o
	${LLVM_BIN_PATH}/llvm-ld -T Script.ld -o app.s.out  app.s.o
	${LLVM_BIN_PATH}/llvm-objdump -arch-name=mspu -t -disassemble  app.s.out > app.s.dis

#Genelf:
#	$(call AsIns)
#	$(call LnkIns)
#	$(call DisAsIns)
#	$(call GemInsData)

clean:
	@rm -rf *.lof  *.out  *.map  *.mmap  *.sct  *.tab  *.s  *.dis  *~  *.def *.xml *.data
	@rm -rf simv*  work  *.lib++ *Log *.log *~ *.rc csrc *.key filelist.f
	@rm -rf *.vpd DVEfiles vc_hdrs.h urgReport
	@rm -rf *.o *.sym *.txt *.ll 
