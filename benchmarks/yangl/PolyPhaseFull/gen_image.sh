#!/bin/bash
: ${MAPU_HOME:?"Environment variable MAPU_HOME is not set. Please set it to the pathname of your SDK root folder."}
# Generate binary image for MaPU SoC on-chip flash
output=image.bin

# ARM executable binary
echo "Retrieving boot rom binary image ..."
bootrom_bin=`find ./boot_rom/ -name "main.bin"`

echo "Retrieving u-boot binary image ..."
uboot_bin=`find ./u-boot/mapu -name "u-boot.bin"`

# Output ARM image
dd if=${bootrom_bin} of=${output}
dd bs=1024c seek=64 if=${uboot_bin} of=${output} oflag=append

# Make filesystem image
echo "Make file system image ..."
$MAPU_HOME/tools/mkfs.jffs2 -s 0x1000 -e 0x40000 -p 0x500000 -d fs_root/ -o fs_jffs2.img
dd bs=1K seek=3k if=fs_jffs2.img of=${output} oflag=append

echo "Complete image generation. Flash image is in file $output."

