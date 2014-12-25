LLVM_BIN_PATH=${MAPU_HOME}/apc/bin
LIBPATH = ../../../util/msloader

LIBS =    elf

CXXFLAGS =	-O2 -g -Wall -fmessage-length=0 -std=c++0x -l$(LIBS) -I$(LIBPATH)

OBJS =		bicubic.o elf_object.o csu_object.o

TARGET =	bicubic

$(TARGET):	$(OBJS) app.m.out app.s.out
	$(CXX) ${CXXFLAGS} -o $(TARGET) $(OBJS) 

%.o: $(LIBPATH)/%.cc $(LIBPATH)/%.hh
	$(CXX) ${CXXFLAGS} -c $< -o $@

%.m.out: %.m.asm
	${LLVM_BIN_PATH}/llvm-mc -arch=mmpulite -filetype=obj $<  -o $(subst .out,.o,$@)
	${LLVM_BIN_PATH}/llvm-ld -T ${MAPU_HOME}/apc/share/MaPU_ASM.m.ld -n -o $@  $(subst .out,.o,$@)
	${LLVM_BIN_PATH}/llvm-objdump -arch=mmpulite -t -disassemble  -s $@ > $(subst .out,.dis,$@)

%.s.out: %.s.asm
	${LLVM_BIN_PATH}/llvm-mc -arch=mspu -filetype=obj $<  -o $(subst .out,.o,$@)
	${LLVM_BIN_PATH}/llvm-ld -T ${MAPU_HOME}/apc/share/MaPU_ASM.s.ld -n -o $@  $(subst .out,.o,$@) --just-symbols *.m.out
	${LLVM_BIN_PATH}/llvm-objdump -arch=mspu -t -disassemble  -s $@ > $(subst .out,.o,$@)

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET) *~ *.out *.dis *.o BicubicOut.dat
