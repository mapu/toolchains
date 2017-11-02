EX_NAME=Add_and_Mul

# setup the tools
LLVM_BIN_PATH	   =${MAPU_HOME}/apc/bin
GEM5_SIM_BIN	   =${MAPU_HOME}/simulator/apc/gem5.opt
GEM5_CFG	   =${MAPU_HOME}/simulator/apc/system/se.py
GEM5_DBG_FLAG_OP   ="--debug-flags=Exec"
TRACE_FILE         ="--trace-file=trace.txt"
CORE_NUM           =1

all:run

compile:${EX_NAME}.c Makefile
	${LLVM_BIN_PATH}/clang -target mspu ${EX_NAME}.c -g -o ${EX_NAME}.s.out
	${LLVM_BIN_PATH}/clang -target mspu ${EX_NAME}.c -S -o ${EX_NAME}.s
	${LLVM_BIN_PATH}/llvm-objdump -arch-name=mspu -t -disassemble  ${EX_NAME}.s.out > ${EX_NAME}.s.dis

run:compile
	${GEM5_SIM_BIN} ${GEM5_DBG_FLAG_OP} ${TRACE_FILE} ${GEM5_CFG} -n ${CORE_NUM} -c ${EX_NAME}.s.out

clean:
	@rm -rf *.lof  *.s.out  *.map  *.mmap  *.sct  *.tab  *.s  *.dis  *~  *.def *.xml *.data
	@rm -rf *.o *.sym *.txt *.ll stderr stdout m5out *.bin
