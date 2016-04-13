#判断系统架构(32bit, 64bit)
#ARCH := $(shell getconf LONG_BIT)
#ifeq ($(ARCH),64)
#CFLAGS+= -DNCX_PTR_SIZE=8
#else
CFLAGS+= -DNCX_PTR_SIZE=4
#endif

CFLAGS+= -pipe  -O3 
#定义是否打印日志
CFLAGS+= -DLOG_LEVEL=2
#是否与malloc类似模拟脏数据
#CFLAGS+= -DNCX_DEBUG_MALLOC
#是否自动合并碎片
CFLAGS+= -DPAGE_MERGE

include $(TOPDIR)/Makefile.env