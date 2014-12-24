#!/bin/bash
root=`pwd`

# Check argument
gem5_en=1
gold_en=1
llvm_en=1
newlib_en=1
openocd_en=1
install_path=
while (( $# != 0 ))
do
  case $1 in
  "--disable-gem5" )
     gem5_en=0
     shift
  ;;
  "--disable-gold" )
     gold_en=0
     shift
  ;;
  "--disable-llvm" )
     llvm_en=0
     shift
  ;;
  "--disable-newlib" )
     newlib_en=0
     shift
  ;;
  "--disable-openocd" )
     openocd_en=0
     shift
  ;;
  "-h" )
    echo "Usage of $0:"
    echo -e "\t-h\t\t\t\tShow this list"
    echo -e "\t--disable-gem5\t\t\tDo not install gem5 simulator"
    echo -e "\t--disable-gold\t\t\tDo not install gold linker"
    echo -e "\t--disable-llvm\t\t\tDo not install llvm toolchain"
    echo -e "\t--disable-newlib\t\tDo not install newlib"
    echo -e "\t--disable-openocd\t\tDo not install openocd"
    exit 0
  ;;
  * )
    if [ -n "$install_path" ]
    then
      echo "Unknown argument $1, try $0 -h to see the usage"
      exit 1
    fi
    echo "Install to $1."
    if [[ $1 != /* ]]
    then
      echo "Install path must be absolute."
      exit 1
    fi
    install_path=$1
    source_path=$(dirname `readlink -e $0`)
    shift
  ;;
  esac
done

# Define multi-core option
MCFLAG='-j 64'

# Install Gem5
if [ "$gem5_en" -eq 1 ]
then
  cd $root
  if [ -e "build_gem5" ]
  then
    rm -rf build_gem5
  fi
  mkdir build_gem5
  cd build_gem5
  mkdir ext
  cp -r $source_path/MaPUSim/ext/mapudrv ./ext
  echo scons -C $source_path/MaPUSim build/MAPU/gem5.opt CPU_MODELS=InOrderCPU
  scons -C $source_path/MaPUSim build/MAPU/gem5.opt CPU_MODELS=InOrderCPU $MCFLAG
  cd $root
  install -v build_gem5/build/MAPU/gem5.opt -D $install_path/simulator/apc/gem5.opt
  install -v -d $install_path/simulator/apc/system
  install -v $source_path/MaPUSim/configs/example/* -t $install_path/simulator/apc/system
  install -v -d $install_path/simulator/apc/common
  install -v $source_path/MaPUSim/configs/common/* -t $install_path/simulator/apc/common
  install -v -d $install_path/simulator/libs
  install -v $source_path/deplibs/protobuf/* -t $install_path/simulator/libs
  install -v $source_path/deplibs/unwind/* -t $install_path/simulator/libs
  install -v $source_path/deplibs/tcmalloc/* -t $install_path/simulator/libs
fi

# Install llvm
if [ "$llvm_en" -eq 1 ]
then
  CC=gcc
  CXX=g++
  version=`$CC --version | grep -o -e '(GCC) [[:digit:]]\+\.[[:digit:]]\+\.[[:digit:]]\+' | awk '{print $2}'` 
  ver_1st=`echo $version | awk -F '.' '{print $1}'`
  ver_2nd=`echo $version | awk -F '.' '{print $2}'`
  gcc_en=0
  if (( ver_1st != 4 || ver_2nd < 6 ))
  then
    CC=$install_path/gcc-4.8.3/bin/gcc
    if [ -e "$CC" ]
    then
      version=`$CC --version | grep -o -e '(GCC) [[:digit:]]\+\.[[:digit:]]\+\.[[:digit:]]\+' | awk '{print $2}'` 
      ver_1st=`echo $version | awk -F '.' '{print $1}'`
      ver_2nd=`echo $version | awk -F '.' '{print $2}'` 
      if ((ver_1st != 4 || ver_2nd < 6 ))
      then
        gcc_en=1
      else
        # gcc is built, so assume the dependent libraries are deployed at where they used to be
        export LD_LIBRARY_PATH=$install_path/gmp-4.3.2/lib:$install_path/mpfr-2.4.2/lib:$install_path/mpc-0.8.1/lib:$LD_LIBRARY_PATH
      fi
    else
        gcc_en=1
    fi
  fi
  version=`$CXX --version | grep -o -e '(GCC) [[:digit:]]\+\.[[:digit:]]\+\.[[:digit:]]\+' | awk '{print $2}'` 
  ver_1st=`echo $version | awk -F '.' '{print $1}'`
  ver_2nd=`echo $version | awk -F '.' '{print $2}'`
  if (( ver_1st != 4 || ver_2nd < 6 ))
  then
    CXX=$install_path/gcc-4.8.3/bin/g++
    if [ -e "$CC" ]
    then
      version=`$CXX --version | grep -o -e '(GCC) [[:digit:]]\+\.[[:digit:]]\+\.[[:digit:]]\+' | awk '{print $2}'` 
      ver_1st=`echo $version | awk -F '.' '{print $1}'`
      ver_2nd=`echo $version | awk -F '.' '{print $2}'` 
      if ((ver_1st != 4 || ver_2nd < 6 ))
      then
        gcc_en=1
      fi
    else
      gcc_en=1
    fi
  fi
  if [ "$gcc_en" -eq 1 ]
  then
    tar -xjvf $source_path/gmp-4.3.2.tar.bz2
    mkdir build_gmp
    cd build_gmp
    ../gmp-4.3.2/configure --prefix=$install_path/gmp-4.3.2
    make $MCFLAG
    make install
    cd $root
    tar -xjvf $source_path/mpfr-2.4.2.tar.bz2
    mkdir build_mpfr
    cd build_mpfr
    ../mpfr-2.4.2/configure --prefix=$install_path/mpfr-2.4.2 --with-gmp=$install_path/gmp-4.3.2
    make $MCFLAG
    make install
    cd $root
    tar -xzvf $source_path/mpc-0.8.1.tar.gz
    mkdir build_mpc
    cd build_mpc
    ../mpc-0.8.1/configure --prefix=$install_path/mpc-0.8.1 --with-gmp=$install_path/gmp-4.3.2 --with-mpfr=$install_path/mpfr-2.4.2
    make $MCFLAG
    make install
    cd $root
    tar -xjvf $source_path/gcc-4.8.3.tar.bz2
    mkdir build_gcc
    cd build_gcc
    ../gcc/configure --prefix=$install_path/gcc-4.8.3 --with-gmp=$install_path/gmp-4.3.2 --with-mpfr=$install_path/mpfr-2.4.2 --with-mpc=$install_path/mpc-0.8.1 --enable-languages=c,c++ --disable-multilib
    export LD_LIBRARY_PATH=$install_path/gmp-4.3.2/lib:$install_path/mpfr-2.4.2/lib:$install_path/mpc-0.8.1/lib:$LD_LIBRARY_PATH
    make $MCFLAG
    make install
    cd $root
  fi

  # Build ragel first
  cd $root
  if [ -e "build_ragel" ]
  then
    rm -rf build_ragel
  fi
  mkdir build_ragel
  cd build_ragel
  $source_path/ragel-6.8/configure --prefix=$root/ragel CFLAGS='-O2' CXXFLAGS='-O2'
  make $MCFLAG
  make install
  
  # Build libedit for lldb
  cd $root
  tar -xzvf $source_path/libedit-20130712-3.1.tar.gz
  if [ -e "build_edit" ]
  then
    rm -rf build_edit
  fi
  mkdir build_edit
  cd build_edit
  ../libedit-20130712-3.1/configure --prefix=$install_path
  make $MCFLAG
  make install

  cd $root
  if [ -e "build_llvm" ]
  then
    rm -rf build_llvm
  fi
  mkdir build_llvm
  cd build_llvm
  # libstdc++ is required while compiling llvm not only by the linker but also by the execution of tblgen
  export LD_LIBRARY_PATH=$source_path/deplibs:$LD_LIBRARY_PATH
  $source_path/llvm-3.4/configure --prefix=$install_path --disable-assertions \
    --enable-optimized --disable-debug-symbols --enable-cxx11\
    --enable-targets=mspu,mmpu,mmpulite,x86 CC=$CC CXX=$CXX LDFLAGS="-L$source_path/deplibs -L$install_path/lib"\
    CFLAGS="-I$install_path/include" CXXFLAGS="-I$install_path/include"
  make OPTIMIZE_OPTION=-O0 RAGEL=$root/ragel/bin/ragel $MCFLAG
  install -v $source_path/deplibs/libstdc++.so.6 -t $install_path/lib
  make install
fi

# Install ld.gold
if [ "$gold_en" -eq 1 ]
then
  cd $root
  if [ -e "build_gold" ]
  then
    rm -rf build_gold
  fi
  mkdir build_gold
  cd build_gold
  # Build libiberty first
  $source_path/MaPUGold/libiberty/configure --prefix=$root/build_gold
  make $MCFLAG
  $source_path/MaPUGold/gold/configure --prefix=$install_path --program-transform-name="s/ld.gold/llvm-ld/" --enable-targets=all
  make $MCFLAG
  make install DATADIRNAME=abc
fi

# Install newlib
if [ "$newlib_en" -eq 1 ]
then
  cd $root
  if [ -e "build_newlib" ]
  then
    rm -rf build_newlib
  fi
  mkdir build_newlib
  cd build_newlib
  $source_path/newlib/configure --target=mspu \
    CC_FOR_TARGET="$install_path/bin/clang -target mspu" \
    CFLAGS_FOR_TARGET="-O0 -nostdlibinc" "CFLAGS_FOR_BUILD=-g -O0" \
    DEBUG_PREFIX_CFLAGS_FOR_TARGET="-O0 -g -nostdlibinc" \
    AR_FOR_TARGET=ar AS_FOR_TARGET="$install_path/bin/llvm-mc  -arch=mspu" \
    LD_FOR_TARGET="$install_path/bin/ld.gold" \
    OBJDUMP_FOR_TARGET="$install_path/bin/llvm-objdump -arch=mspu -d" \
    READELF_FOR_TARGET=readelf RANLIB_FOR_TARGET=ranlib  STRIP_FOR_TARGET=strip \
    --enable-multilib=no  --prefix="$install_path/lib"
  make $MCFLAG
  make install
fi

# Install openocd
if [ "$openocd_en" -eq 1 ]
then
  cd $root
  if [ -e "build_openocd" ]
  then
    rm -rf build_openocd
  fi
  mkdir build_openocd
  cd build_openocd
  $source_path/openocd-0.5/configure --prefix="$install_path" --program-suffix="-jtag" CFLAGS="-O2 -std=gnu99"
  make $MCFLAG
  make install
  make clean
  $source_path/openocd-0.5/configure --prefix="$install_path" --program-suffix="-sim" CFLAGS="-O2 -std=gnu99 -DSIM"
  make $MCFLAG
  make install
fi

# Clean up all temporary files
cd $root
rm -rf build_gem5
rm -f $source_path/MaPUSim/parsetab.py
rm -f $source_path/MaPUSim/parser.out
rm -f `find $source_path -name "*.pyc"`
rm -rf build_ragel
rm -rf gmp-4.3.2
rm -rf build_gmp
rm -rf mpfr-2.4.2
rm -rf build_mpfr
rm -rf mpc-0.8.1
rm -rf build_mpc
rm -rf gcc
rm -rf build_gcc
rm -rf libedit-20130712-3.1
rm -rf build_edit
rm -rf build_llvm
rm -rf build_gold
rm -rf ragel
rm -rf build_newlib
rm -rf build_openocd
