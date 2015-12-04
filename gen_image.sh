#!/bin/bash
: ${MAPU_HOME:?"Environment variable MAPU_HOME is not set. Please set it to the pathname of your SDK root folder."}

root=`pwd`

export PATH=$MAPU_HOME/arm-none-eabi/bin:$PATH
export PATH=$MAPU_HOME/tools:$MAPU_HOME/arm/bin:$PATH

# Build kernel
cd $root
if [ ! -e $root/kernel/arch/arm/boot/uImage -o ! -e $root/kernel/arch/arm/boot/mapu_sim.dtb ]
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
      if [ ! -e $root/kernel/arch/arm/boot/uImage -o ! -e $root/kernel/arch/arm/boot/dtb ]
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
if [ ! -e $root/u-boot/mapu/u-boot.bin ]
then 
  echo -e "\n\n\n\nMake u-boot.bin....\n"
  if [ -e $root/u-boot ]
  then
    cd $root/u-boot
    rm mapu/ -rf
    make O=mapu mapu_sim CONFIG_APP=y || uboot_err=1
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
if [ ! -e $root/boot_rom/main.bin ]
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

output=$1

dd if=boot_rom/main.bin of=${output:-sim.img}
dd bs=1k seek=64 if=u-boot/mapu/u-boot.bin of=${output:-sim.img} oflag=append
dd bs=1k seek=512 if=kernel/arch/arm/boot/mapu_sim.dtb of=${output:-sim.img}  oflag=append
dd bs=1k seek=1K if=kernel/arch/arm/boot/uImage of=${output:-sim.img}  oflag=append
dd bs=1k seek=3K if=ubuntu/ubuntu.img of=${output:-sim.img}  oflag=append

echo -e "\n\n\n\n\tMake image done!\n\n"
echo

