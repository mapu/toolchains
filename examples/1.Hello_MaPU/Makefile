EX_NAME=Hello_MaPU

# setup the tools
LLVM_BIN_PATH	=${MAPU_HOME}/apc/bin
GEM5_SIM_BIN	=${MAPU_HOME}/simulator/apc/gem5.opt
GEM5_CFG	=${MAPU_HOME}/simulator/apc/system/se.py

all:run

compile:${EX_NAME}.c Makefile
	${LLVM_BIN_PATH}/clang -target mspu ${EX_NAME}.c -g -o ${EX_NAME}.s.out
	${LLVM_BIN_PATH}/clang -target mspu ${EX_NAME}.c -S -o ${EX_NAME}.s
	${LLVM_BIN_PATH}/llvm-objdump -arch-name=mspu -t -disassemble  ${EX_NAME}.s.out > ${EX_NAME}.s.dis

run:compile
	${GEM5_SIM_BIN} ${GEM5_CFG} -c ${EX_NAME}.s.out

clean:
	@rm -rf *.lof  *.s.out  *.map  *.mmap  *.sct  *.tab  *.s  *.dis  *~  *.def *.xml *.data
	@rm -rf *.o *.sym *.txt *.ll stderr stdout m5out *.bin
