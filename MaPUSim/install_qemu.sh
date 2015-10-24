#!/bin/bash

: ${RELEASE_PATH:?"Release directory RELEASE_PATH for qemu release is not set. Please set it to the directory for qemu release!"}

if [ "${RELEASE_PATH:0:1}" != "/" ]
then
  echo RELEASE_PATH must be an absolute directory!
  exit
fi

if [ "${RELEASE_PATH:0-1:1}" = "/" ]
then
  echo RELEASE_PATH should not end up with \'/\'! 
  exit
fi

cd ARM-QEMU
./configure --prefix=$RELEASE_PATH --target-list=arm-softmmu
make -j64
make install
echo ls yeah!
cd ..
