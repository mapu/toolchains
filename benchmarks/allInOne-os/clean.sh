#!/bin/bash
root=`pwd`
fs_root=$root/fs_root

# Clean up APC library
cd $root
if [ -e "build_spu" ]
then
  cd build_spu
  make clean
fi
cd $root
if [ -e "build_mpu" ]
then
  cd build_mpu
  make clean
fi
cd $root
if [ -e "build_arm" ]
then
  rm -rf build_arm
fi

# Clean up APC library
cd $root
if [ -e "slib" ]
then rm -rf slib
fi
if [ -e "mlib" ]
then rm -rf mlib
fi

# Clean up APC project
cd $root
if [ -e "build_spu" ]
then
rm -rf build_spu
fi
if [ -e "build_mpu" ]
then
rm -rf build_mpu
fi

# Remove then generated db file
cd $root
rm -rf $fs_root

# Clean up temp files
cd $root
temp=`find ./ -name "*~"`
rm -f $temp

