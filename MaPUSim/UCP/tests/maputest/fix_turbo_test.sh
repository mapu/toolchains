#!/bin/sh
# run the Gem5 Turbo Decoder regression test
###########################################################
#############################################################
echo "==========================================================================="
echo "Running regression test for MaPU_Gem5 Turbo Decoder..."
if [  -d ./m5out ];\
then rm -rf m5out
fi

mkdir m5out

flist=`ls -1 | grep -e "^fix_turbotest_.*"`

for casename in $flist
  do
    echo "Running $casename test ..."
    if [ ! -d ./m5out/$casename ];\
    then mkdir m5out/$casename
    fi
    cd $casename
    make clean 
    make
    cd ..
    ../../build/MAPU/gem5.opt ../../configs/example/se.py --turbo-type=fix -c      $casename/app.s.out
    cd $casename
    make clean
    cd ..
  done






