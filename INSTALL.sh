#!/bin/bash
root=`pwd`

# Check argument
gem5_en=1
gold_en=1
llvm_en=1
newlib_en=1
openocd_en=1
qemu_en=1
debug_mode=0
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
  "--disable-qemu" )
     qemu_en=0
     shift
  ;;
  "--disable-all" )
     gem5_en=0
     gold_en=0
     llvm_en=0
     newlib_en=0
     openocd_en=0
     qemu_en=0
     shift
  ;;
  "--enable-gem5" )
     gem5_en=1
     shift
  ;;
  "--enable-gold" )
     gold_en=1
     shift
  ;;
  "--enable-llvm" )
     llvm_en=1
     shift
  ;;
  "--enable-newlib" )
     newlib_en=1
     shift
  ;;
  "--enable-openocd" )
     openocd_en=1
     shift
  ;;
  "--enable-qemu" )
     qemu_en=1
     shift
  ;;
  "--enable-debug" )
     debug_mode=1
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
    echo -e "\t--disable-qemu\t\tDo not install qemu"
    echo -e "\t--enable-xxx\t\tInstall xxx package"
    echo -e "\t--disable-all\t\tDo not install anything (used with following --enable-xxx)"
    echo -e "\t--enable-debug\t\tBuild in debug and incremental mode, and do not remove the building dirs"
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

# Switch debug mode
if [ "$debug_mode" -eq 1 ]
then
  gem5_opt_mode='.debug'
  llvm_cfg='--disable-optimized --enable-assertions'
else
  gem5_opt_mode='.opt'
  llvm_cfg='--enable-optimized --disable-assertions'
fi

# Initialize error status
gem5_err=0
llvm_err=0
gold_err=0
newlib_err=0
openocd_err=0
qemu_err=0

# Install Gem5
if [ "$gem5_en" -eq 1 ]
then
  cd $root
  if [ -e "build_gem5_apc" ] && [ "$debug_mode" -eq 0 ]
  then rm -rf build_gem5_apc
  fi
  if [ ! -e "build_gem5_apc" ]
  then mkdir build_gem5_apc
  fi
  cd build_gem5_apc
  if [ ! -e "ext" ]
  then
    mkdir ext
    cp -r $source_path/MaPUSim/APC/ext/mapudrv ./ext
  fi
  scons -C $source_path/MaPUSim/APC build/MAPU/gem5$gem5_opt_mode CPU_MODELS=InOrderCPU $MCFLAG || gem5_err=1
  cd $root
  if [ "$debug_mode" -eq 0 ]
  then 
    install -v -d $install_path/simulator/apc
    install -v build_gem5_apc/build/MAPU/gem5$gem5_opt_mode -D $install_path/simulator/apc/gem5$gem5_opt_mode
    install -v -d $install_path/simulator/apc/system
    install -v $source_path/MaPUSim/APC/configs/example/* -t $install_path/simulator/apc/system
    install -v -d $install_path/simulator/apc/common
    install -v $source_path/MaPUSim/APC/configs/common/* -t $install_path/simulator/apc/common
    install -v -d $install_path/simulator/apc/utils
    install -v $source_path/MaPUSim/APC/util/pipeview.sh -t $install_path/simulator/apc/utils
    install -v $source_path/MaPUSim/APC/util/mapu-pipeview.py -t $install_path/simulator/apc/utils
    install -v -d $install_path/simulator/libs
    install -v $source_path/deplibs/protobuf/* -t $install_path/simulator/libs
    install -v $source_path/deplibs/unwind/* -t $install_path/simulator/libs
    install -v $source_path/deplibs/tcmalloc/* -t $install_path/simulator/libs
  fi
  if [ -e "build_gem5_arm" ] && [ "$debug_mode" -eq 0 ]
  then rm -rf build_gem5_arm
  fi
  if [ ! -e "build_gem5_arm" ]
  then mkdir build_gem5_arm
  fi
  cd build_gem5_arm
  scons -C $source_path/MaPUSim/ARM build/ARM/gem5$gem5_opt_mode CPU_MODELS=AtomicSimpleCPU $MCFLAG || gem5_err=1
  make -C $source_path/MaPUSim/ARM/util/term default
  cd $root
  if [ "$debug_mode" -eq 0 ]
  then
    install -v -d $install_path/simulator/arm
    install -v build_gem5_arm/build/ARM/gem5$gem5_opt_mode -D $install_path/simulator/arm/gem5$gem5_opt_mode
    install -v -d $install_path/simulator/arm/system
    install -v $source_path/MaPUSim/ARM/configs/example/* -t $install_path/simulator/arm/system
    install -v -d $install_path/simulator/arm/common
    install -v $source_path/MaPUSim/ARM/configs/common/* -t $install_path/simulator/arm/common
    install -v -d $install_path/simulator/arm/utils
    install -v $source_path/MaPUSim/ARM/util/term/m5term -t $install_path/simulator/arm/utils
    install -v $source_path/MaPUSim/mapu_sim.sh -t $install_path/simulator/
  fi
  make -C $source_path/MaPUSim/ARM/util/term clean
  #install MaPU GUI
  if [ -e "$install_path/simulator/gui" ]
  then
    rm -rf $install_path/simulator/gui
  fi
  mkdir $install_path/simulator/gui
  cxfreeze --install-dir=$install_path/simulator/gui $source_path/MaPUSim/GUI/main.py
fi

# Install qemu
if [ "$qemu_en" -eq 1 ]
then
  cd $root
  if [ -e "build_qemu" ] && [ "$debug_mode" -eq 0 ]
  then rm -rf build_qemu
  fi
  if [ ! -e "build_qemu" ]
  then mkdir build_qemu
  fi
  cd build_qemu
  $source_path/MaPUSim/ARM-QEMU/configure --prefix=$install_path/simulator/arm --target-list=arm-softmmu 
  make $MCFLAG || qemu_err=1 
  if [ "$debug_mode" -eq 0 ]
  then make install
  fi
fi

install_tool_path=${install_path}
install_path=${install_path}/apc

# Install llvm
if [ "$llvm_en" -eq 1 ]
then
  if [ -e "/opt/updated-tools/gcc-4.8.2/bin/gcc" ]
  then
    CC=/opt/updated-tools/gcc-4.8.2/bin/gcc
    CXX=/opt/updated-tools/gcc-4.8.2/bin/g++
  else
    CC=gcc
    CXX=g++
  fi
  version=`$CC --version | grep -o -e '(GCC) [[:digit:]]\+\.[[:digit:]]\+\.[[:digit:]]\+' | awk '{print $2}'` 
  ver_1st=`echo $version | awk -F '.' '{print $1}'`
  ver_2nd=`echo $version | awk -F '.' '{print $2}'`
  gcc_en=0
  if (( ver_1st != 4 || ver_2nd < 6 ))
  then
    CC=$install_tool_path/gcc-4.8.3/bin/gcc
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
        export LD_LIBRARY_PATH=$install_tool_path/gmp-4.3.2/lib:$install_tool_path/mpfr-2.4.2/lib:$install_tool_path/mpc-0.8.1/lib:$LD_LIBRARY_PATH
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
    CXX=$install_tool_path/gcc-4.8.3/bin/g++
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
    ../gmp-4.3.2/configure --prefix=$install_tool_path/gmp-4.3.2
    make $MCFLAG
    make install
    cd $root
    tar -xjvf $source_path/mpfr-2.4.2.tar.bz2
    mkdir build_mpfr
    cd build_mpfr
    ../mpfr-2.4.2/configure --prefix=$install_tool_path/mpfr-2.4.2 --with-gmp=$install_tool_path/gmp-4.3.2
    make $MCFLAG
    make install
    cd $root
    tar -xzvf $source_path/mpc-0.8.1.tar.gz
    mkdir build_mpc
    cd build_mpc
    ../mpc-0.8.1/configure --prefix=$install_tool_path/mpc-0.8.1 --with-gmp=$install_tool_path/gmp-4.3.2 --with-mpfr=$install_tool_path/mpfr-2.4.2
    make $MCFLAG
    make install
    cd $root
    tar -xjvf $source_path/gcc-4.8.3.tar.bz2
    mkdir build_gcc
    cd build_gcc
    ../gcc/configure --prefix=$install_tool_path/gcc-4.8.3 --with-gmp=$install_tool_path/gmp-4.3.2 --with-mpfr=$install_tool_path/mpfr-2.4.2 --with-mpc=$install_tool_path/mpc-0.8.1 --enable-languages=c,c++ --disable-multilib
    make $MCFLAG
    make install
    cd $root
  fi

  if [ -e "/opt/updated-tools/python-2.7.5/bin/python" ]
  then python_flag="--with-python=/opt/updated-tools/python-2.7.5/bin/python"
  else python_flag=
  fi

  # Build ragel first
  cd $root
  if [ -e "build_ragel" ] && [ "$debug_mode" -eq 0 ]
  then rm -rf build_ragel
  fi
  if [ ! -e "build_ragel" ]
  then mkdir build_ragel
  fi
  cd build_ragel
  $source_path/ragel-6.8/configure --prefix=$root/ragel CFLAGS='-O2' CXXFLAGS='-O2'
  make $MCFLAG
  make install
  
  # Build libedit for lldb
  cd $root
  tar -xzvf $source_path/libedit-20130712-3.1.tar.gz
  if [ -e "build_edit" ] && [ "$debug_mode" -eq 0 ]
  then rm -rf build_edit
  fi
  if [ ! -e "build_edit" ]
  then mkdir build_edit
  fi
  cd build_edit
  ../libedit-20130712-3.1/configure --prefix=$install_path
  make $MCFLAG
  make install

  cd $root
  if [ -e "build_llvm" ] && [ "$debug_mode" -eq 0 ]
  then rm -rf build_llvm
  fi
  if [ ! -e "build_llvm" ]
  then mkdir build_llvm
  fi
  cd build_llvm
  # libstdc++ is required while compiling llvm not only by the linker but also by the execution of tblgen
  export LD_LIBRARY_PATH=$source_path/deplibs:$LD_LIBRARY_PATH
  $source_path/llvm-3.5/configure --prefix=$install_path $llvm_cfg $python_flag \
    --enable-cxx11 --enable-targets=mspu,mmpulite,x86 CC=$CC CXX=$CXX
  make RAGEL=$root/ragel/bin/ragel CXXFLAGS="-DARCH_MAPU" CFLAGS="-DARCH_MAPU" $MCFLAG || llvm_err=1
  install -v $source_path/deplibs/libstdc++.so.6 -t $install_path/lib
  if [ "$debug_mode" -eq 0 ]
  then make install
  fi
fi

# Install ld.gold
if [ "$gold_en" -eq 1 ]
then
  cd $root
  if [ -e "build_gold" ] && [ "$debug_mode" -eq 0 ]
  then rm -rf build_gold
  fi
  if [ ! -e "build_gold" ]
  then mkdir build_gold
  fi
  cd build_gold
  # Build libiberty first
  # $source_path/MaPUGold/libiberty/configure --prefix=$root/build_gold
  # make $MCFLAG || gold_err=1
  # $source_path/MaPUGold/gold/configure --prefix=$install_path --program-transform-name="s/ld.gold/llvm-ld/" --enable-targets=all --disable-werror
  $source_path/gold-2.25/configure --disable-optimize --enable-gold --enable-targets=all --prefix=$install_path #--program-transform-name="s/ld.gold/llvm-ld/" 
  make $MCFLAG || gold_err=1
  if [ "$debug_mode" -eq 0 ]
  then make install #DATADIRNAME=abc
  fi
fi

# Install newlib
if [ "$newlib_en" -eq 1 ]
then
  cd $root
  if [ -e "build_newlib" ] && [ "$debug_mode" -eq 0 ]
  then rm -rf build_newlib
  fi
  if [ ! -e "build_newlib" ]
  then mkdir build_newlib
  fi
  cd build_newlib
  $source_path/newlib/configure --target=mspu \
    CC_FOR_TARGET="$install_path/bin/clang -target mspu" \
    DEBUG_PREFIX_CFLAGS_FOR_TARGET=" -nostdlibinc" \
    AR_FOR_TARGET=ar AS_FOR_TARGET="$install_path/bin/llvm-mc -arch=mspu" \
    LD_FOR_TARGET="$install_path/bin/ld.gold" \
    OBJDUMP_FOR_TARGET="$install_path/bin/llvm-objdump -arch=mspu -d" \
    READELF_FOR_TARGET=readelf RANLIB_FOR_TARGET=ranlib  STRIP_FOR_TARGET=strip \
    --enable-multilib=no --prefix="$install_path/lib"
  make $MCFLAG || newlib_err=1
  if [ "$debug_mode" -eq 0 ]
  then make install
  fi
fi

# Install openocd
if [ "$openocd_en" -eq 1 ]
then
  cd $root
  if [ -e "build_openocd" ] && [ "$debug_mode" -eq 0 ]
  then rm -rf build_openocd
  fi
  if [ ! -e "build_openocd" ]
  then mkdir build_openocd
  fi
  cd build_openocd
  $source_path/openocd-0.5/configure --prefix="$install_path" --program-suffix="-jtag" CFLAGS="-O2 -std=gnu99"
  make $MCFLAG || openocd_err=1
  if [ "$debug_mode" -eq 0 ]
  then make install
  fi
  make clean
  $source_path/openocd-0.5/configure --prefix="$install_path" --program-suffix="-sim" CFLAGS="-O2 -std=gnu99 -DSIM"
  make $MCFLAG || openocd_err=1
  if [ "$debug_mode" -eq 0 ]
  then make install
  fi
fi

# Clean up all temporary files
cd $root
if [ "$debug_mode" -eq 0 ]
then
  if [ "$gem5_err" -eq 0 ]
  then
    rm -rf build_gem5_apc
    rm -rf build_gem5_arm
  else
    echo "Failed to install Gem5"
  fi
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
  if [ "$llvm_err" -eq 0 ]
  then rm -rf build_llvm
  else echo "Failed to install LLVM"
  fi
  if [ "$gold_err" -eq 0 ]
  then rm -rf build_gold
  else echo "Failed to install Gold"
  fi
  rm -rf ragel
  if [ "$newlib_err" -eq 0 ]
  then rm -rf build_newlib
  else echo "Failed to install Newlibc"
  fi
  if [ "$openocd_err" -eq 0 ]
  then rm -rf build_openocd
  else echo "Failed to install openocd"
  fi
  if [ "$qemu_err" -eq 0 ]
  then rm -rf build_qemu
  else echo "Failed to install qemu"
  fi

fi


