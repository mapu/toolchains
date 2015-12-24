#!/bin/bash
: ${MAPU_HOME:?"Environment variable MAPU_HOME is not set. Please set it to the pathname of your SDK root folder."}

root=`pwd`

make_bootrom=0
make_uboot=0
make_kernel=0
output=


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

# Build kernel
cd $root
if [ ! -e $root/kernel/arch/arm/boot/uImage -o ! -e $root/kernel/arch/arm/boot/mapu_sim.dtb -o "$make_kernel" -eq 1 ]
then 
  echo -e "\n\n\n\nMake kernel....\n"
  if [ -e $root/kernel ]
  then
    cd $root/kernel
    if [ -e $root/kernel/arch/arm/configs/mapu_defconfig ]
    then
      make distclean
      cp $root/kernel/arch/arm/configs/mapu_defconfig .config
      make uImage dtbs ARCH=arm CROSS_COMPILE=$MAPU_HOME/arm-linux-gnueabi/bin/arm-linux-gnueabi- -j64
      if [ ! -e $root/kernel/arch/arm/boot/uImage -o ! -e $root/kernel/arch/arm/boot/mapu_sim.dtb ]
      then
        echo -e "\n\n\n\nMake kernel fail: make uImage or dtb fail!\n"
        exit
      fi
    else
      echo -e "\n\n\n\nMake kernel fail: mapu_defconfig not exist!\n"
      exit
    fi
  else 
    echo -e "\n\n\n\nMake kernel fail: folder u-boot/ not exist!\n"
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
    make distclean O=mapu
    rm mapu/ -rf
    cp -rf $root/arm $root/u-boot/app
    make mapu_defconfig O=mapu 
    make O=mapu CONFIG_APP=y CROSS_COMPILE=arm-none-eabi- || uboot_err=1
    if [ "$uboot_err" -eq 1 ]
    then
      echo -e "\n\n\n\nMake u-boot fail: make u-boot.bin fail!\n"
      exit
    fi
  else 
    echo -e "\n\n\n\nMake u-boot fail: folder u-boot/ not exist!\n"
    exit
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

if [ ! -e $root/kernel/arch/arm/boot/mapu_sim.dtb ]
then 
  echo -e "\n\n\n\nMake image fail: mapu_sim.dtb not exist!\n"
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


dd if=boot_rom/main.bin of=${output:-sim.img}
if [ $? -ne 0 ]
then
  echo -e "\n\n\tdd main.bin fail!\n\n"
  exit
fi
dd bs=1k seek=64 if=u-boot/mapu/u-boot.bin of=${output:-sim.img} oflag=append
if [ $? -ne 0 ]
then
  echo -e "\n\n\tdd u-boot.bin fail!\n\n"
  exit
fi
dd bs=1k seek=512 if=kernel/arch/arm/boot/mapu_sim.dtb of=${output:-sim.img}  oflag=append
if [ $? -ne 0 ]
then
  echo -e "\n\n\tdd mapu_sim.dtb fail!\n\n"
  exit
fi
dd bs=1k seek=1K if=kernel/arch/arm/boot/uImage of=${output:-sim.img}  oflag=append
if [ $? -ne 0 ]
then
  echo -e "\n\n\tdd uImage fail!\n\n"
  exit
fi
dd bs=1k seek=3K if=ubuntu/ubuntu.img of=${output:-sim.img}  oflag=append
if [ $? -ne 0 ]
then
  echo -e "\n\n\tdd ubuntu.img fail!\n\n"
  exit
else
  echo -e "\n\n\n\n\tMake image done!\n\n"
  echo -e "\timage is stored at" ${output:-${root}/sim.img}
  echo
fi

