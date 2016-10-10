LIBPATH = ../../../util/msloader

LIBS =    elf

CXXFLAGS =	-O2 -g -Wall -fmessage-length=0 -l$(LIBS) -I$(LIBPATH)

OBJS =		driver_test.o elf_object.o csu_object.o

TARGET =	driver_test

$(TARGET):	$(OBJS)
	$(CXX) ${CXXFLAGS} -o $(TARGET) $(OBJS) 

%.o: $(LIBPATH)/%.cc $(LIBPATH)/%.hh
	$(CXX) ${CXXFLAGS} -c $< -o $@

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET) *~
