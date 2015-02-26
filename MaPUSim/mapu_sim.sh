#!/bin/bash

# Check environment
: ${MAPU_HOME:?"Environment variable MAPU_HOME is not set. Please set it to the pathname of your SDK root folder."}
dimension=`xdpyinfo | grep "dimensions" | awk '{print $2}'`
height=`echo $dimension | awk -F 'x' '{print $2}'`
width=`echo $dimension | awk -F 'x' '{print $1}'`
fontheight=20
fontwidth=10

# Check terminal
term=
which gnome-terminal &>/dev/null
if [ $? -eq 0 ]
then
  term='gnome-terminal'
  geom_flag='--geometry='
  titl_flag='--title='
  font_flag='--zoom=1.3'
else
  echo "Warn: gnome-terminal is not available for simulator. Try xterm ..."
  which xterm &>/dev/null
  if [ $? -eq 0 ]
  then
    term='xterm  -sl 10000'
    geom_flag='-geometry '
    titl_flag='-title '
    font_flag="-fn ${fontwidth}x${fontheight}"
  else
    echo "Error: xterm is required for simulator."
    exit 1
  fi
fi

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

# UI configuration
echo "Preparing UI ..."

let 'optwins=flag_dbg+flag_mpu+flag_ape+flag_arm'

let 'row1height=height/4/fontheight-2'
let 'armsimwidth=width/3/fontwidth-2'
titles='ARM_Simulator'
geom="${armsimwidth}x${row1height}+0+0"

let 'apcsimwidth=width/3/fontwidth-2'
let 'apcsimx=width/3'
titles="$titles APC_Simulator"
geom="$geom ${apcsimwidth}x${row1height}+${apcsimx}+0"

let 'uartwidth=width/3/fontwidth-2'
let 'uartx=width*2/3'
titles="$titles UART_Terminal"
geom="$geom ${uartwidth}x${row1height}+${uartx}+0"

let 'row2height=height*3/4/fontheight-2'
let 'row2y=height/4'

idx=0
row2x=0
if [ "$flag_mpu" -eq 1 ]
then
  if [ "$optwins" -ne 1 ]
  then
    let 'row2width=width/2'
    let 'tmpuwidth=row2width/fontwidth-2'
    let 'optwins--'
  else
    let 'tmpuwidth=width/fontwidth-2'
  fi
  titles="$titles MPU_Pipeline"
  geom="$geom ${tmpuwidth}x${row2height}+${row2x}+${row2y}"
  let 'row2x=width/2'
else
  let 'row2width=width'
fi

if [ "$flag_dbg" -eq 1 ]
then
  if [ "$optwins" -ne 1 ]
  then
    let 'row2width=row2width/2'
    let 'dbgx=row2x'
    let 'dbgwidth=row2width/fontwidth-2'
    let 'optwins--'
  else
    let 'dbgwidth=row2width/fontwidth-2'
    let 'dbgx=row2x'
  fi
  titles="$titles ARM_GDB_Console"
  geom="$geom ${dbgwidth}x${row2height}+${dbgx}+${row2y}"
  let 'row2x=row2x+row2width'
fi

if [ "$flag_arm" -eq 1 ]
then
  if [ "$optwins" -ne 1 ]
  then
    if [ "$flag_dbg" -eq 1 ] && [ "$flag_mpu" -eq 1 ]
    then
      let 'tarmwidth=row2width/fontwidth-2'
      let 'row2height=row2height/2-1'
    else
      let 'row2width=row2width/2'
      let 'tarmwidth=row2width/fontwidth-2'
    fi
    let 'optwins--'
  else
    let 'dbgwidth=width/fontwidth-2'
    let 'dbgx=(tmpuwidth+2)*fontwidth+width/2'
  fi
  titles="$titles ARM_Trace"
  geom="$geom ${tarmwidth}x${row2height}+${row2x}+${row2y}"
  if [ "$flag_dbg" -eq 1 ] && [ "$flag_mpu" -eq 1 ]
  then
    let 'row2y=row2y+height*3/8'
  else
    let 'row2x=row2x+row2width'
  fi
fi

if [ "$flag_ape" -eq 1 ]
then
  let 'tapewidth=row2width/fontwidth-2'
  titles="$titles SPU&MPU_Trace"
  geom="$geom ${tapewidth}x${row2height}+${row2x}+${row2y}"
fi

apc_debug_flags="--debug-flags=Exec"
#debug_flags="--debug-flags=DmaController"
arm_debug_flags="--debug-flags=Exec,-ExecFaulting,-ExecOpClass"
#debug_flags="--debug-start=60411816500 --debug-flags=Exec,-ExecFaulting,-ExecOpClass"
#debug_flags="--debug-flags=Timer,GIC,Interrupt"
armsim="${MAPU_HOME}/simulator/arm/gem5.opt"
if [ -n "$disk" ]
then
  armsys="${MAPU_HOME}/simulator/arm/system/fs.py --bare-metal --machine-type=MaPU_Board --disk-image=$disk"
else
  armsys="${MAPU_HOME}/simulator/arm/system/fs.py --bare-metal --machine-type=MaPU_Board"
fi
apcsim="${MAPU_HOME}/simulator/apc/gem5.opt"
apcsys="${MAPU_HOME}/simulator/apc/system/ms.py -n 4"
set -f
set -- $geom
for title in $titles
do
  case $title in
  "ARM_Simulator" )
    rm -rf m5out/err.log
    if [ "$flag_dbg" -eq 1 ]
    then
      dbg="--debug-tid=0"
    else
      dbg=""
    fi
    if [ "$flag_arm" -eq 1 ]
    then
      rm -f m5out/arm.out
      trace_arm="--debug-file=arm.out $arm_debug_flags"
    else
      trace_arm=""
    fi
    $term $titl_flag$title $geom_flag$1 $font_flag -e \
      "${armsim} $trace_arm $dbg -e --stderr-file=err.log ${armsys}" &
    ipcready=0
    while [ $ipcready -eq 0 ]
    do
      if [ ! -e m5out/err.log ]
      then continue
      fi
      errln=`cat m5out/err.log | wc -l`
      if [ "$errln" -lt 5 ]
      then continue
      fi
      termport=`cat m5out/err.log | grep -o -e 'terminal.*' | awk '{print $5}'`
      vncport=`cat m5out/err.log | grep -o -e 'vncserver.*' | awk '{print $5}'`
      apcport=`cat m5out/err.log | grep -o -e 'realview\.apc.*' | awk '{print $5}'`
      gdbport=`cat m5out/err.log | grep -o -e 'remote gdb #0.*' | awk '{print $6}'`
      if [ "$gdbport" == "" ]
      then continue
      fi
      shmport=`cat m5out/err.log | grep -o -e 'Share memory key is .*' | awk '{print $5}'`
      shmlist=`ipcs -m | grep '^0x' | awk '{print $1}' | tr [a-z] [A-Z]`
      for shm in $shmlist
      do
        shmdec=`echo "ibase=16;${shm#0X}"|bc`
        if [ "$shmdec" -eq "$shmport" ]
        then 
          ipcready=1
          shmid=`ipcs -m | grep "^0x" | tr [a-z] [A-Z] | grep "^$shm" | awk '{print $2}'`
        fi
      done
    done
    shift
  ;;
  "APC_Simulator" )
    if [ "$flag_ape" -eq 1 ]
    then
      rm -f m5out/apc.out
      trace_apc="--trace-file=apc.out $apc_debug_flags"
    else
      trace_apc=""
    fi
    if [ "$flag_mpu" -eq 1 ] && [ -z "$trace_apc" ]
    then
      trace_apc="--trace-file=apc.out --debug-flags=MaPUPipeView"
    elif [ "$flag_mpu" -eq 1 ]
    then
      trace_apc="$trace_apc,MaPUPipeView"
    fi
    $term $titl_flag$title $geom_flag$1 $font_flag -e \
      "${apcsim} $trace_apc -e --stderr-file=err1.log ${apcsys} -c $apcport -k $shmport" &
    sleep 1
    while [ ! -e m5out/err1.log ]
    do :
    done
    shift
  ;;
  "UART_Terminal" )
    $term $titl_flag$title $geom_flag$1 $font_flag -e "${MAPU_HOME}/simulator/arm/utils/m5term localhost $termport" &
    shift
  ;;
  "MPU_Pipeline" )
    rm -f m5out/mapu-pipeview.out
    $term $titl_flag$title $geom_flag$1 $font_flag -e "${MAPU_HOME}/simulator/apc/utils/pipeview.sh" &
    shift
  ;;
  "ARM_GDB_Console" )
    gdb_cmd="gdb_cmd"
    gdb_args="--eval-command=\"target remote localhost:$gdbport\""
    if [ -e "$gdb_cmd" ]
    then
      gdb_args="--command=$gdb_cmd $gdb_args"
    else
      gdb_args="--eval-command=\"set tdesc filename $MAPU_HOME/arm-none-eabi/arm-none-eabi/share/gdb/features/arm-with-neon.xml\" $gdb_args"
    fi
    $term $titl_flag$title $geom_flag$1 $font_flag -e \
      "${MAPU_HOME}/arm-none-eabi/bin/arm-none-eabi-gdb $gdb_args" &
    shift
  ;;
  "ARM_Trace" ) 
    $term $titl_flag$title $geom_flag$1 $font_flag -e "tail -F m5out/arm.out" &
    shift
  ;;
  "SPU&MPU_Trace" ) 
    $term $titl_flag$title $geom_flag$1 $font_flag -e "tail -F m5out/apc.out" &
    shift
  ;;
  * )
    echo "Error: Unknown UI."
    exit 1
  ;;
  esac
done
# Collect share memory garbage
shmlist=`ipcs -m | grep "^0x"`
oldifs=$IFS
IFS='
'
for item in $shmlist
do
  shmidp=`echo $item | awk '{print $2}'`
  if [ $shmid -eq $shmidp ]
  then
    IFS=$oldifs
    while [ $ipcready -eq 1 ]
    do
      item=`ipcs -m | grep "^0x[[:digit:]]\+[[:space:]]\+$shmid"`
      if [ "$item" == "" ]
      then
        ipcready=0
        break
      fi
      nattch=`echo $item | awk '{print $6}'`
      if [ "$nattch" -eq 2 ] || [ "$nattch" -eq 0 ]
      then
        wordcnt=`echo $item | wc -w`
        if [ $wordcnt -eq 6 ]
        then
          ipcrm -m $shmid
          ipcready=0
          break
        fi
      else
        wordcnt=`echo $item | wc -w`
        if [ $wordcnt -eq 7 ]
        then
          status=`echo $item | awk '{print $7}'`
          if [ "$status" == "dest" ]
          then
            ipcready=0
            break
          fi
        fi
      fi
    done
  fi
  if [ $ipcready -eq 0 ]
  then break
  fi
done

