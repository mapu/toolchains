LIBPATH = ../../../util/msloader

LIBS =    elf

CXXFLAGS =	-O0 -g -Wall -fmessage-length=0 -std=c++0x -l$(LIBS) -I$(LIBPATH)

OBJS =		 matmul.o elf_object.o csu_object.o channel.o crc.o modulation.o turbo_enc_dec.o ULRecvTurbodec.o
 

TARGET =	matmul1

$(TARGET):	$(OBJS)
	$(CXX) ${CXXFLAGS} -o $(TARGET) $(OBJS) 

%.o: $(LIBPATH)/%.cc $(LIBPATH)/%.hh
	$(CXX) ${CXXFLAGS} -c $< -o $@

%.o: %.c %.h
	$(CXX) ${CXXFLAGS} -c $< -o $@

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET) *~
