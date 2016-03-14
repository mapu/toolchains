#!/bin/bash
: ${MAPU_HOME:?"Environment variable MAPU_HOME is not set. Please set it to the pathname of your SDK root folder."}

root=`pwd`

make_bootrom=0
make_uboot=0
make_kernel=0
output=
sys=


export PATH=$MAPU_HOME/arm-none-eabi/bin:$PATH
export PATH=$MAPU_HOME/tools:$MAPU_HOME/arm/bin:$PATH

while (( $# != 0))
do
  case $1 in
  "--make_all" )
    make_bootrom=1
    make_uboot=1
    make_kernel=1
    shift
  ;;
  "--make_bootrom" )
    make_bootrom=1
    shift
  ;;
  "--make_uboot" )
    make_uboot=1
    shift
  ;;
  "--make_kernel" )
    make_kernel=1
    shift
  ;;
  "-h" )
    echo "Usage of $0:"
    echo -e "\t-h\t\t\t\tShow this list"
    echo -e "\t--make_all\t\t\tCompile boot_rom, u-boot, dtb and uImage NO MATTER exist or not!"
    echo -e "\t--make_bootrom\t\t\tCompile boot_rom NO MATTER exist or not!"
    echo -e "\t--make_uboot\t\t\tCompile u-boot NO MATTER exist or not!"
    echo -e "\t--make_kernel\t\t\tCompile dtb and uImage NO MATTER exist or not!"
    echo -e "\n\tother words in arguments will be set as the file NAME or PATH of the image generated!"
    echo -e "\tthe defaul file name of the image generted is sim.img!"
    echo -e "\n\tthe default make option will not compile anything if the file for image exist!\n\n"
    exit 0
  ;;
  "--sim" )
    if [ "$sys" = "" ]
    then
      sys="sim"
    else
      echo -e "\n\tConflict system type!\n\n\tSet --sim or --chip only!\n"
      exit 1
    fi
    shift
  ;;
  "--chip" )
    if [ "$sys" = "" ]
    then
      sys="chip"
    else
      echo -e "\n\tConflict system type!\n\n\tSet --sim or --chip only!\n"
      exit 1
    fi
    shift
  ;;
  * )
    if [ -n "$output" ]
    then
      echo "Unknown argument $1, try $0 -h to see the usage"
      exit 1
    fi
    output=$1
    shift
  ;;
  esac
done

if [ "$sys" = "" ]
then
  sys="sim"
fi

kernel_err=0

# Build kernel
cd $root
if [ ! -e $root/kernel/arch/arm/boot/uImage -o ! -e $root/kernel/arch/arm/boot/mapu_sim.dtb -o "$make_kernel" -eq 1 ]
then 
  echo -e "\n\n\n\nMake kernel....\n"
  if [ -e $root/kernel ]
  then
    cd $root/kernel
    make clean
    make distclean
    if [ -e $root/kernel/arch/arm/configs/mapu_${sys}_defconfig ]
    then
      make mapu_${sys}_defconfig ARCH=arm || kernel_err=1
      if [ $kernel_err -eq 0 ]
      then
        make uImage dtbs ARCH=arm CROSS_COMPILE=$MAPU_HOME/arm-linux-gnueabi/bin/arm-linux-gnueabi- -j64 || kernel_err=1
        if [ $kernel_err -ne 0 ]
        then
          echo -e "\n\n\tError: Make uImage or dtb fail!\n\n"
          exit 1
        fi
      else
        echo -e "\n\n\tError: Make kernel mapu_${sys}_defconfig fail!\n\n"
        exit 1
      fi
    else
      echo -e "\n\n\tError: Kernel mapu_${sys}_defconfig not exist!\n\n"
      exit 1
    fi
  else
    echo -e "\n\n\tError: Kernel folder ${root}/kernel/ not exist!\n\n"
    exit
  fi
fi


# Build u-boot and ARM applications

uboot_err=0

cd $root
if [ ! -e $root/u-boot/mapu/u-boot.bin -o "$make_uboot" -eq 1 ]
then 
  echo -e "\n\n\n\nMake u-boot.bin....\n"
  if [ -e $root/u-boot ]
  then
    cd $root/u-boot
    make clean O=mapu
    make distclean O=mapu
    rm mapu/ -rf
    if [ -e $root/u-boot/configs/mapu_${sys}_defconfig ]
    then
      make mapu_${sys}_defconfig O=mapu || uboot_err=1
      if [ $uboot_err -eq 0 ]
      then
        make O=mapu CROSS_COMPILE=arm-none-eabi- || uboot_err=1
        if [ "$uboot_err" -ne 0 ]
        then
          echo -e "\n\n\tError: Make u-boot.bin fail!\n\n"
          exit 1
        fi
      else
        echo -e "\n\n\tError: Make u-boot mapu_${sys}_defconfig fail!\n\n"
        exit 1
      fi
    else
      echo -e "\n\n\tError: u-boot mapu_${sys}_defconfig not exist!\n\n"
      exit 1
    fi
  else 
    echo -e "\n\n\n\nMake u-boot fail: folder u-boot/ not exist!\n"
    exit 1
  fi
fi


# Build boot rom
cd $root
if [ ! -e $root/boot_rom/main.bin -o "$make_bootrom" -eq 1 ]
then
  echo -e "\n\n\n\nMake boot_rom!\n"
  if [ -e $root/boot_rom ]
  then
    cd $root/boot_rom
    make
    if [ ! -e $root/boot_rom/main.bin ]
    then
      echo -e "\n\n\n\nMake boot_rom fail: make main.bin fail!\n"
      exit
    fi
  else
    echo -e "\n\n\n\nMake boot_rom fail: folder boot_rom/ not exist!\n"
    exit
  fi
fi

# Make image
cd $root
if [ ! -e $root/boot_rom/main.bin ]
then 
  echo -e "\n\n\n\nMake image fail: main.bin not exist!\n\n"
  exit
fi

if [ ! -e $root/u-boot/mapu/u-boot.bin ]
then 
  echo -e "\n\n\n\nMake image fail: u-boot.bin not exist!\n"
  exit
fi

if [ ! -e $root/kernel/arch/arm/boot/mapu_${sys}.dtb ]
then 
  echo -e "\n\n\n\nMake image fail: mapu_${sys}.dtb not exist!\n"
  exit
fi

if [ ! -e $root/kernel/arch/arm/boot/uImage ]
then 
  echo -e "\n\n\n\nMake image fail: uImage not exist!\n"
  exit
fi

if [ ! -e $root/ubuntu/ubuntu.img ]
then 
  echo -e "\n\n\n\nMake image fail: ubuntu.img not exist!\n"
  exit
fi


dd if=boot_rom/main.bin of=${output:-${sys}.img}
if [ $? -ne 0 ]
then
  echo -e "\n\n\tdd main.bin fail!\n\n"
  exit
fi
dd bs=1k seek=64 if=u-boot/mapu/u-boot.bin of=${output:-${sys}.img} oflag=append
if [ $? -ne 0 ]
then
  echo -e "\n\n\tdd u-boot.bin fail!\n\n"
  exit
fi
dd bs=1k seek=512 if=kernel/arch/arm/boot/mapu_${sys}.dtb of=${output:-${sys}.img}  oflag=append
if [ $? -ne 0 ]
then
  echo -e "\n\n\tdd mapu_sim.dtb fail!\n\n"
  exit
fi

dd bs=1k seek=1K if=kernel/arch/arm/boot/uImage of=${output:-${sys}.img}  oflag=append
if [ $? -ne 0 ]
then
  echo -e "\n\n\tdd uImage fail!\n\n"
  exit
fi

if [ "$sys" = "sim" ]
then
  dd bs=1k seek=10K if=ubuntu/ubuntu.img of=${output:-${sys}.img}  oflag=append
  if [ $? -ne 0 ]
  then
    echo -e "\n\n\tdd ubuntu.img fail!\n\n"
    exit 1
  fi
fi

echo -e "\n\n\n\n\tMake image done!\n\n"
echo -e "\timage is stored at" ${output:-${root}/${sys}.img}
echo


