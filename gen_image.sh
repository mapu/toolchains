#!/bin/bash
: ${MAPU_HOME:?"Environment variable MAPU_HOME is not set. Please set it to the pathname of your SDK root folder."}

root=`pwd`

export PATH=$MAPU_HOME/arm-none-eabi/bin:$PATH
export PATH=$MAPU_HOME/tools:$MAPU_HOME/arm/bin:$PATH

# Build u-boot and ARM applications

cd $root
if [ ! -e $root/u-boot/mapu/u-boot.bin ]
then 
  echo
  echo
  echo "Make u-boot.bin"
  echo
  if [ -e $root/u-boot ]
  then
    cd $root/u-boot
    make O=mapu mapu_sim CONFIG_APP=y -j64
  else 
    echo "Make u-boot fail: folder u-boot/ not exist!"
    exit
  fi
fi


# Build boot rom
cd $root
if [ ! -e $root/boot_rom/main.bin ]
then
  echo
  echo
  echo "Make boot_rom!\n\n"
  echo
  if [ -e $root/boot_rom ]
  then
    cd $root/boot_rom
    make
  else
    echo "Make boot_rom fail: folder boot_rom/ not exist!"
    exit
  fi
fi

# Make image
cd $root
if [ ! -e $root/boot_rom/main.bin ]
then 
  echo "Make image fail: main.bin not exist!"
  exit
fi

if [ ! -e $root/u-boot/mapu/u-boot.bin ]
then 
  echo "Make image fail: u-boot.bin not exist!"
  exit
fi

if [ ! -e $root/kernel/arch/arm/boot/mapu_sim.dtb ]
then 
  echo "Make image fail: mapu_sim.dtb not exist!"
  exit
fi

if [ ! -e $root/kernel/arch/arm/boot/uImage ]
then 
  echo "Make image fail: uImage not exist!"
  exit
fi

if [ ! -e $root/ubuntu/ubuntu.img ]
then 
  echo "Make image fail: ubuntu.img not exist!"
  exit
fi

dd if=boot_rom/main.bin of=${output:-sim.img}
dd bs=1k seek=64 if=u-boot/mapu/u-boot.bin of=${output:-sim.img} oflag=append
dd bs=1k seek=512 if=kernel/arch/arm/boot/mapu_sim.dtb of=${output:-sim.img}  oflag=append
dd bs=1k seek=1K if=kernel/arch/arm/boot/uImage of=${output:-sim.img}  oflag=append
dd bs=1k seek=3K if=ubuntu/ubuntu.img of=${output:-sim.img}  oflag=append

echo
echo
echo
echo "Make image done!"
echo

