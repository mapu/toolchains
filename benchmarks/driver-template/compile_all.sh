#!/bin/bash
: ${MAPU_HOME:?"Environment variable MAPU_HOME is not set. Please set it to the pathname of your SDK root folder."}

root=`pwd`
fs_root="$root/fs_root"

# Define tools
LLVMMC=$MAPU_HOME/apc/bin/llvm-mc
LLVMLD=$MAPU_HOME/apc/bin/ld.gold
CLANG=$MAPU_HOME/apc/bin/clang

# Compile all MPU projects
cd $root
if [ -e $root/build_mpu ]
then rm -rf $root/build_mpu
fi
mkdir $root/build_mpu

cd $root/build_mpu
echo "Configuring MPU projects ..."
$root/mpu/configure MC=$LLVMMC LD=$LLVMLD --prefix=$root/mlib EXEEXT=".m.elf" -q
make KEEP_SYMBOLS=1
make KEEP_SYMBOLS=1 install
install -d $fs_root/mlib $root/mlib/bin
install -t $fs_root/mlib $root/mlib/bin/*

# Compile all SPU projects
cd $root
if [ -e $root/build_spu ]
then rm -rf $root/build_spu
fi
mkdir $root/build_spu

cd $root/build_spu
echo "Configuring SPU projects ..."
$root/spu/configure CC=$CLANG CXX=$CLANG BUILD_CC=$CLANG BUILD_CXX=$CLANG --prefix=$root/slib -q
make
make install

cd $root
if [ -e $root/slib/lib ]
then
  sarlist=`ls $root/slib/lib/*.s.a`
else
  sarlist=
fi

for sar in $sarlist
do
  self="$root/slib/bin/$(basename $sar .s.a).s.elf"
  mlib_header="$root/spu/$(basename $sar .s.a)/mlib.h"
  :>$mlib_header
  echo '// This is a generated file for MPU library reference'	>> $mlib_header
  echo '#ifndef MLIB_H'						>> $mlib_header
  echo '#define MLIB_H'						>> $mlib_header
  echo 								>> $mlib_header

  sdepfile=${sar%".s.a"}.mdepx
  :>$sdepfile
  reloclist=`readelf -r $self |\
    grep -o -e '[[:xdigit:]]\{8\}[[:space:]]\+[[:xdigit:]]\{8\}[[:space:]]\+unrecognized: [[:digit:]]\+[[:space:]]\+[[:xdigit:]]\{8\}[[:space:]]\+[[:graph:]]\+ + 0' |\
    awk '{ print $6 }'`
  symlist=`readelf -s $self |\
    grep -o -e \
    '[[:space:]]\+[[:digit:]]\+:[[:space:]]\+[[:xdigit:]]\{8\}[[:space:]]\+0[[:space:]]\+NOTYPE[[:space:]]\+GLOBAL[[:space:]]\+DEFAULT[[:space:]]\+UND [[:graph:]]\+' |\
    awk '{ print $8 }'`
  relocsymlist=
  mid=0
  for sym in $reloclist
  do
    found=0
    for sym2 in $symlist
    do
      if [ "$sym" = "$sym2" ]
      then
        found=1
        break
      fi
    done
    if [ "$found" -eq 1 ]
    then
      relocsymlist="$relocsymlist $sym"
    fi
  done
  reloc_num=`echo "$relocsymlist" | wc -w`
  if [ "$reloc_num" -ne 0 ]
  then
    mliblist=`ls -1 $fs_root/mlib`
    done_syms=
    for msym in $relocsymlist
    do
      search=1
      for sym in $done_syms
      do
        if [ $msym == $sym ]
        then
          search=0
          break
        fi
      done
      if [ "$search" -eq 0 ]
      then
        continue
      fi
      found=
      msymset=
      for melf in $mliblist
      do
        melf=$fs_root/mlib/$melf
        mfunclist=`readelf -s $melf |\
        grep -o -e '[[:space:]]\+[[:digit:]]\+:[[:space:]]\+[[:xdigit:]]\{8\}[[:space:]]\+0[[:space:]]\+NOTYPE[[:space:]]\+GLOBAL[[:space:]]\+DEFAULT[[:space:]]\+[[:graph:]]\+ [[:graph:]]\+' |\
        awk '{ print $8 }'`
        for mfunc in $mfunclist
        do
          if [ $mfunc == $msym ]
          then
            if [ -n "$found" ]
            then
              echo -e "\e[31m\e[1mError: $mfunc is redefined in $melf, which was previously defined in $found.\e[0m"
              echo "#endif"						>> $mlib_header
              exit 1
            fi
            isreged="`cat $sdepfile | grep -o \"$(basename $melf .m.elf)\"`"
            if [ -z "$isreged" ]
            then
              echo $(basename $melf .m.elf) >> $sdepfile
              echo "#ifndef MELF_$(basename $melf .m.elf)"	>> $mlib_header
              echo "#define MELF_$(basename $melf .m.elf) $mid"	>> $mlib_header
              echo "#endif"					>> $mlib_header
              echo 						>> $mlib_header
              let 'mid++'
            fi
            echo "#ifndef ID_$msym"				>> $mlib_header
            echo "#define ID_$msym MELF_$(basename $melf .m.elf)">> $mlib_header
            echo "#endif"					>> $mlib_header
            echo 						>> $mlib_header
            found=$melf
            break
          fi
        done
      done
      if [ -z $found ]
      then
        echo -e "\e[31m\e[1mError: Micro-code function $msym is not found in mpu library.\e[0m"
        echo "#endif"						>> $mlib_header
        exit 1
      fi
    done
  fi
  echo 								>> $mlib_header
  echo "#define MLOAD(name) load_mcode(ID_##name)"		>> $mlib_header
  echo "#endif"							>> $mlib_header
  echo 								>> $mlib_header
done

# Build SPU for the second round
cd $root/build_spu
make
make install
install -d $fs_root/slib $root/slib/bin
install -t $fs_root/slib $root/slib/bin/*

cd $root
for sar in $sarlist
do
  ldflag="-L$MAPU_HOME/apc/lib/clang/3.6.0/lib/mspu/ -lclang_rt.builtins-mspu -L$MAPU_HOME/apc/lib/mspu/lib -lc -lm -lgloss"
  if [ -e ${sar%".s.a"}.ld ]
  then
    ldflag="$ldflag -T ${sar%".s.a"}.ld"
  fi
  if [ -e ${sar%".s.a"}.mdepx ]
  then
    mv -f ${sar%".s.a"}.mdepx ${sar%".s.a"}.mdep
  fi
  mdeplist=`cat ${sar%".s.a"}.mdep`
  for mdep in $mdeplist
  do
    if [ -e $fs_root/mlib/$mdep.m.elf ]
    then
      ldflag="$ldflag --just-symbols $fs_root/mlib/$mdep.m.elf"
    else
      echo -e "\e[31m\e[1mError: $mdep.m.elf does not exist but is used by $sar.\e[0m"
      exit 1
    fi
  done
  sname=${sar##*\/}
  sname=${sname%".a"}
  echo $LLVMLD $sar -o $fs_root/slib/$sname.elf $ldflag
  $LLVMLD $sar -o $fs_root/slib/$sname.elf $ldflag
  rm -f $olist
done

# Generate the relationship database file
cd $root
if [ ! -e $fs_root ]
then
  mkdir $fs_root
fi
:>$fs_root/apclib_custom.db
echo '// This is a generated relationship database for APC library reference.'	>> $fs_root/apclib_custom.db
echo '// The database format is defined as:'					>> $fs_root/apclib_custom.db
echo '// <"slib_name", ["mlib_name0" "mlib_name1" ...]>'			>> $fs_root/apclib_custom.db

if [ -e $root/slib/lib ]
then
  sdeplist=`ls -1 $root/slib/lib | grep -e ".*\.mdep"`
else
  sdeplist=
fi

if [ -e $root/mlib/bin ]
then
  mliblist=`ls -1 $root/mlib/bin | grep -e ".*\.m\.elf"`
  mnum=`ls -1 $root/mlib/bin | grep -e ".*\.m\.elf" | wc -w`
else
  mliblist=
  mnum=0
fi

for sdepfile in $sdeplist
do
  sdepname=${sdepfile%".mdep"}
  if [ ! -e slib/lib/$sdepfile ]
  then
    continue
  fi
  mdeplist=`cat $root/slib/lib/$sdepfile`
  depnum=`cat $root/slib/lib/$sdepfile | wc -w`
  if [ $depnum -eq 0 ]
  then
    continue
  fi
  echo -n "<\"${sdepname}\", ["							>> $fs_root/apclib_custom.db
  for mdep in $mdeplist
  do
    mid=0
    for melf in $mliblist
    do
      if [ $mdep = ${melf%".m.elf"} ]
      then
        echo -n "\"$mdep\", "							>> $fs_root/apclib_custom.db
        break
      fi
      let "mid++"
    done
    if [ "$mid" -eq "$mnum" ]
    then
      echo -e "\e[31m\e[1mError: Cannot find $mdep.m.elf specified in $sdepfile which is used by $sdepname.s.elf.\e[0m"
      echo $mliblist
      echo $mdeplist
      exit 1
    fi
  done
  echo "]>"									>> $fs_root/apclib_custom.db
done
echo 										>> $fs_root/apclib_custom.db

echo 'Building ARM executable.....'
cd $root/arm
# for proj in `ls -l |awk '/^d/ {print $NF}'`
make