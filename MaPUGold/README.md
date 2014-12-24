MSPUGold
========

The ported linker for MSPU, Based on binutils-2.22
========
INSTALL :
1. mkdir build
2. cd    build
3. libiberty/configure --prefix=`pwd`
4. make 
5. make install
6. ../gold/configure --prefix=${INSTALL DIR} --target=mspu
7. make
8. make install

