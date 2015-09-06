#!/bin/bash

# Check environment
: ${MAPU_HOME:?"Environment variable MAPU_HOME is not set. Please set it to the pathname of your SDK root folder."}

# Parse arguments
flag_dbg=0
flag_mpu=0
flag_arm=0
flag_ape=0
image=
disk=
while (( $# != 0 ))
do
  case $1 in
  "help" | "-h" | "--help" )
    echo "Usage of MaPU simulator: $0 [OPTION]"
    echo "OPTION:"
    echo "  debug\t\tLaunch simulator with debugger connected."
    echo "  tmpu\t\tTrace MPU mcode pipeline."
    echo "  tarm\t\tTrace ARM program."
    echo "  tape\t\tTrace SPU and MPU programs."
    echo "  full\t\tLaunch full features of simulator except debug."
    exit 0
  ;;
  "debug" )
    flag_dbg=1
    shift
  ;;
  "tmpu" )
    flag_mpu=1 
    shift
  ;;
  "tarm" )
    flag_arm=1
    shift
  ;;
  "tape" )
    flag_ape=1
    shift
  ;;
  "full" )
    flag_mpu=1
    flag_arm=1
    flag_ape=1
    shift
  ;;
  "disk="* )
    disk=${1#"disk="}
    shift
  ;;
  * )
    if [ -z $image ]
    then
      image=$1
      echo "Simulation image is $1"
      shift
    else
      echo "Unknown option $1 for $0."
      echo "Use \"$0 help\" to see usage."
      exit 1
    fi
  ;;
  esac
done
if [ -z $image ]
then
  image=image.bin
fi

# Check simulation stimulous
if [ ! -e images ]
then
  mkdir images
elif [ ! -d images ]
then
  echo "Error: File \"images\" is existing but is not a folder."
  exit 1
fi

if [ -e images/image.bin ]
then
  echo "Overriding old \"image.bin\" in \"image\" folder."
fi

if [ ! -e $image ]
then
  echo "Error: cannot find $image."
  exit 1
fi
cp -f $image images/

if [ -n "$disk" ]
then
  if [ ! -e disks ]
  then
    mkdir disks
  elif [ ! -d disks ]
  then
    echo "Error: File \"disks\" is existing but is not a folder."
    exit 1
  fi

  if [ ! -e "$disk" ]
  then
    echo "Error: cannot find $disk."
    exit 1
  fi
  cp -f $disk disks/
fi
#start MaPU GUI
if [ ! -e ${MAPU_HOME}/simulator/gui/main ]
then
  echo "Error: cannot find gui/main."
  exit 1
fi
${MAPU_HOME}/simulator/gui/main --image=$PWD/image.bin --trace=apc.out

