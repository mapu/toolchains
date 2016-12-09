#!/bin/bash

# This is a simple manager for MaPU project
root=$(dirname $0)
working_path=`pwd`
cd $root
cur_proj=
if [ -e ./.mapu_proj ]
then
  cur_proj=`cat ./.mapu_proj`
fi

while (( $# != 0 ))
do
  case $1 in
  "help" | "-h" | "--help" )
    cmd=h
    shift
    if [ $# -ne 0 ]
    then
      query=$1
    fi
  ;;
  "new" | "-n" | "--new" )
    cmd=n
    shift
    if [ $# -gt 1 ]
    then
      arch=$1
      projname=$2
      shift 2
    else
      cmd=h
      query=$1
    fi
  ;;
  "rm" | "-r" | "--remove" )
    cmd=r
    shift
    if [ $# -gt 1 ]
    then
      arch=$1
      projname=$2
      shift 2
    else
      cmd=h
      query=$1
    fi
  ;;
  "mgr" | "-m" | "--manage" )
    shift
    if [ $# -gt 1 ]
    then
      case $1 in
      "add" )
        cmd=m_a
        new_src=$2
        shift 2
      ;;
      "del" )
        cmd=m_r
        src_name=$2
        shift 2
      ;;
      "disable" )
        cmd=m_d
        proj_name=$2
        shift 2
      ;;
      "enable" )
        cmd=m_e
        proj_name=$2
        shift 2
      ;;
      "switch" )
        cmd=m_s
        proj_name=$2
        shift 2
      ;;
      * )
        cmd=h
        query=mgr
        shift 1
      ;;
      esac
    else
      cmd=h
      query=$1
    fi
  ;;
  "clean" | "-c" | "--clean" )
    cmd=c
    shift
  ;;
  "build" | "-b" | "--build" )
    cmd=b
    shift
  ;;
  "stat" | "-s" | "--status" )
    cmd=s
    shift
  ;;
  * )
    cmd=h
    shift
  ;;
  esac

  case $cmd in
  "h" )
    if [ -z $query ]
    then
      echo "Project manager usage: $0 COMMAND0 [OPTION0 OPTION1 ...] [COMMAND1 OPTION2 OPTION3 ...]"
      echo "COMMAND:"
      echo "  new|-n|--new: add a new project of arm, spu or mpu."
      echo "  rm|-r|--remove: remove an existing project of arm, spu or mpu."
      echo "  clean|-c|--clean: clean whole MaPU project."
      echo "  build|-b|--build: build mpu, spu and arm projects step by step."
      echo "  gen|-g|--generate: generate flash image."
      echo "  stat|-s|--status: show the current status of MaPU project."
      echo "  mgr|-m|--manage: add/remove source files for a specified project; switch current project between all projects."
      echo "  help|-h|--help: show this help."
      echo "For usage of each command: $0 help COMMAND"
    else
      case $query in
      "new" | "-n" | "--new" )
        echo "Usage: new|-n|--new ARCH PROJECT_NAME"
        echo "  ARCH: identify the architecture of the new project, supported archs are arm, spu and mpu"
        echo "  PROJECT_NAME: the new project's name"
      ;;
       "rm" | "-r" | "--remove" )
        echo "Usage: rm|-r|--remove ARCH PROJECT_NAME"
        echo "  ARCH: identify the architecture of the deleting project, supported archs are arm, spu and mpu"
        echo "  PROJECT_NAME: the deleting project's name"
      ;;
      "clean" | "-c" | "--clean" )
        echo "Usage: clean|-c|--clean"
      ;;
      "build" | "-b" | "--build" )
        echo "Usage: build|-b|--build"
      ;;
      "stat" | "-s" | "--status" )
        echo "Usage: stat|-s|--status"
      ;;
      "mgr" | "-m" | "--manage" )
        echo "Usage: mgr|-m|--manage SUBCOMMAND OPTION"
        echo "SUBCOMMAND:"
        echo "  add SOURCE: add SOURCE file into current project. If SOURCE is an existing file," 
        echo "              it will be copied into current project, otherwise a new SOURCE file"
        echo "              will be created. (NOTE: if creating new SOURCE file, SOURCE should"
        echo "              not contain path name.)"
        echo "  del SOURCE: del SOURCE file from current project."
        echo "  enable ARCH/PROJECT: include sub PROJECT under ARCH folder into building system."
        echo "                       ARCH can be arm, spu or mpu"
        echo "  disable ARCH/PROJECT: exclude sub PROJECT under ARCH folder from building system."
        echo "                        ARCH can be arm, spu or mpu"
        echo "  switch ARCH/PROJECT: switch current project to ARCH/PROJECT. ARCH can be arm,"
        echo "                         spu or mpu."
      ;;
      * )
        echo "Unknown manager command $query."
      ;;
      esac
    fi
    exit 0
  ;;
  "n" )
    echo "Adding new $arch project \"$projname\" ..."
    if [ -e $arch/$projname ]
    then
      echo "$arch project $projname is already existing."
    elif [ $arch == "arm" ]
    then
      mkdir $arch/$projname
      :>$arch/$projname/Makefile
      sed "s/@program_name@/$projname/" $arch/Makefile.in		>>$arch/$projname/Makefile
    else
      mkdir $arch/$projname
      :>$arch/$projname/Makefile
      echo "##===$arch/$projname==-*- Makefile -*-===##"		>>$arch/$projname/Makefile
      echo "# This is a generated Makefile for $arch project."		>>$arch/$projname/Makefile
      echo "LEVEL = .."							>>$arch/$projname/Makefile
      if [ $arch == "spu" ]
      then
        echo "LIBRARYNAME = $projname"					>>$arch/$projname/Makefile
        echo "SPULIB := 1"						>>$arch/$projname/Makefile
      else
        echo "TOOLNAME = $projname"					>>$arch/$projname/Makefile
        echo "MPULIB := 1"						>>$arch/$projname/Makefile
      fi
      echo "include \$(LEVEL)/Makefile.common"				>>$arch/$projname/Makefile
      if [ $arch == "spu" ]
      then
        echo "ASFLAGS += -target m$arch"				>>$arch/$projname/Makefile
        echo "CFLAGS += -target m$arch"					>>$arch/$projname/Makefile
        echo "LDFLAGS += -T \$(PROJ_SRC_ROOT)/$projname/$projname.ld"	>>$arch/$projname/Makefile
        echo "LDFLAGS += -target m$arch"				>>$arch/$projname/Makefile
        cp $arch/$arch.ld $arch/$projname/$projname.ld
        cp $arch/init.$arch.s $arch/$projname/init.$arch.s
        cp $arch/mlib_mgr.* $arch/$projname/
        echo '// This is a generated file for MPU library reference'	>> $arch/$projname/mlib.h
        echo '#ifndef MLIB_H'						>> $arch/$projname/mlib.h
        echo '#define MLIB_H'						>> $arch/$projname/mlib.h
        echo 								>> $arch/$projname/mlib.h
        echo "#endif"							>> $arch/$projname/mlib.h
        echo 								>> $arch/$projname/mlib.h
        :>$arch/$projname/$projname.mdep
      else
        echo "ASFLAGS += -arch m${arch}lite"				>>$arch/$projname/Makefile
        echo "LDFLAGS += -T \$(PROJ_SRC_ROOT)/$arch.ld"			>>$arch/$projname/Makefile
      fi
      echo "LIBS ="							>>$arch/$projname/Makefile
      sed -i'~' "s/DIRS =.*/& $projname/" $arch/Makefile
      if [ $arch = "spu" ]
      then
        :>$arch/include/$projname.h
        echo "// This is the header file for slib $projname"		>>$arch/include/$projname.h
        echo "#ifndef ${projname}_H"					>>$arch/include/$projname.h
        echo "#define ${projname}_H"					>>$arch/include/$projname.h
        echo "// Modify the parameter definition"			>>$arch/include/$projname.h
        echo "// typedef struct {"					>>$arch/include/$projname.h
        echo "// } ${projname}_param;"					>>$arch/include/$projname.h
        echo "// DO NOT change the prototype"				>>$arch/include/$projname.h
        echo "// slib_name(param, coreid, flag)"			>>$arch/include/$projname.h
        echo "//   slib_name: the spu function name"			>>$arch/include/$projname.h
        echo "//   param: the pointer to the parameter location on arm" >>$arch/include/$projname.h
        echo "//   coreid: the APE core id" 				>>$arch/include/$projname.h
        echo "//   flag: APC_BLOCK: arm will wait until this function is finished">>$arch/include/$projname.h
        echo "#  ifdef $projname"					>>$arch/include/$projname.h
        echo "#  undef $projname"					>>$arch/include/$projname.h
        echo "#  endif"							>>$arch/include/$projname.h
        echo "#  define $projname(param, coreid)\\"		>>$arch/include/$projname.h
        echo "  apcapi_core(\"${projname}\", param, coreid)"	>>$arch/include/$projname.h
        echo "#endif"							>>$arch/include/$projname.h
        echo								>>$arch/include/$projname.h
      fi
    fi
  ;;
  "r" )
    echo "Removing $arch project \"$projname\" ..."
    if [ -e $arch/$projname ]
    then
      rm -rf $arch/$projname
      if [ $arch = "spu" ]
      then
        rm -rf $arch/include/$projname.h
      fi
      if [ $arch = "arm" ]
      then
        echo ""
      else
        sed -i'~' -e "/DIRS =.*$/ s/[[:blank:]]\+\<$projname\>//" -e "/DIRS =.*$/ s/[[:blank:]]\+/ /g" $arch/Makefile
      fi
    else
      echo "$arch project $projname does not exist."
    fi
  ;;
  "s" )
    echo -e "Current project status (\e[32m\e[1m*\e[0m: current operating sub-project, \e[30m\e[1m#\e[0m: disabled sub-project):"
    echo "arm projects"
    arm_proj_list=`ls -d arm/*/ 2>/dev/null |\
      sed "s/arm\/\(.*\)\//\1/g"`
    for proj in $arm_proj_list
    do
      if [ "spu/$proj" = "$cur_proj" ]
      then
        echo -e "  |--->\e[32m\e[1m(*)$proj\e[0m"
      else
        echo "  |--->$proj"
      fi
    done
    echo "spu projects"
    spu_proj_list=`ls -d spu/*/ |\
      sed "s/spu\/\(.*\)\//\1/g" |\
      sed -e "s/autoconf//g" -e "s/include//g"`
    reg_proj_list=`grep -E "[[:blank:]]*DIRS[[:blank:]]*=.*" ./spu/Makefile |\
      sed "s/[[:blank:]]*DIRS[[:blank:]]*=\(.*\)/\1/"`
    for proj in $spu_proj_list
    do
      found=0
      for reg in $reg_proj_list
      do
        if [ "$proj" = "$reg" ]
        then
          if [ "spu/$proj" = "$cur_proj" ]
          then
            echo -e "  |--->\e[32m\e[1m(*)$proj\e[0m"
          else
            echo "  |--->$proj"
          fi
          found=1
          break
        fi
      done
      if [ "$found" -ne 1 ]
      then
        if [ "spu/$proj" = "$cur_proj" ]
        then
          echo -e "  |--->\e[32m\e[1m(*)\e[0m\e[30m\e[1m(#)$proj\e[0m"
        else
          echo -e "  |--->\e[30m\e[1m(#)$proj\e[0m"
        fi
      fi
    done
    echo "mpu projects"
    mpu_proj_list=`ls -d mpu/*/ |\
      sed "s/mpu\/\(.*\)\//\1/g" |\
      sed "s/autoconf//g"`
    reg_proj_list=`grep -E "[[:blank:]]*DIRS[[:blank:]]*=.*" ./mpu/Makefile |\
      sed "s/[[:blank:]]*DIRS[[:blank:]]*=\(.*\)/\1/"`
    for proj in $mpu_proj_list
    do
      found=0
      for reg in $reg_proj_list
      do
        if [ "$proj" = "$reg" ]
        then
          if [ "mpu/$proj" = "$cur_proj" ]
          then
            echo -e "  |--->\e[32m\e[1m(*)$proj\e[0m"
          else
            echo "  |--->$proj"
          fi
          found=1
          break
        fi
      done
      if [ "$found" -ne 1 ]
      then
        if [ "mpu/$proj" = "$cur_proj" ]
        then
          echo -e "  |--->\e[32m\e[1m(*)\e[0m\e[30m\e[1m(#)$proj\e[0m"
        else
          echo -e "  |--->\e[30m\e[1m(#)$proj\e[0m"
        fi
      fi
    done
  ;;
  "m_a" )
    if [ -z "$cur_proj" ]
    then
      echo "No project is selected now, switch to a project before adding source files."
    elif [ ! -e "$cur_proj" ]
    then
      echo "Current project $cur_proj is no long existing."
    else
      if [ -e "$new_src" ]
      then
        echo "Copy existing file $new_src into $cur_proj ..."
        cp -f $new_src $cur_proj
      else
        echo "Create new file $new_src in $cur_proj ..."
        :>$new_src
      fi
    fi
  ;;
  "m_r" )
    if [ -z "$cur_proj" ]
    then
      echo "No project is selected now, switch to a project before removing source files."
    elif [ ! -e "$cur_proj" ]
    then
      echo "Current project $cur_proj is no long existing."
    else
      if [ -e "$cur_proj/$src_name" ]
      then
        echo "Removing file $src_name from $cur_proj ..."
        rm -f $cur_proj/$src_name
      else
        echo "$cur_proj/$src_name does not exist."
      fi
    fi
  ;;
  "m_d" )
    if [ ! -e "$proj_name" ]
    then
      echo "Project $proj_name does not exist."
    else
      arch=`echo $proj_name | awk -F "/" '{ print $1 }'`
      proj=`echo $proj_name | awk -F "/" '{ print $2 }'`
      "Disable $proj in $arch ..."
      if [ "arch" = "arm" ]
      then
	echo ""
      else
        sed -i'~' -e "/DIRS =.*$/ s/[[:blank:]]\+\<$proj\>//" -e "/DIRS =.*$/ s/[[:blank:]]\+/ /g" $arch/Makefile
      fi
    fi
  ;;
  "m_e" )
    if [ ! -e "$proj_name" ]
    then
      echo "Project $proj_name does not exist."
    else
      arch=`echo $proj_name | awk -F "/" '{ print $1 }'`
      proj=`echo $proj_name | awk -F "/" '{ print $2 }'`
      echo "Enable $proj in $arch ..."
      if [ "arch" = "arm" ]
      then
	echo ""
      else
        sed -i'~' "s/DIRS =.*/& $proj/" $arch/Makefile
      fi
    fi
  ;;
  "m_s" )
    if [ ! -e "$proj_name" ]
    then
      echo "Project $proj_name is does not exist."
    else
      echo "$proj_name" >./.mapu_proj
    fi
  ;;
  "c" )
    echo "Cleaning MaPU project ..."
    ./clean.sh
  ;;
  "b" )
    echo "Building MaPU project ..."
    ./compile_all.sh
  ;;
  esac
done
cd $working_path

