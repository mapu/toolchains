dd if=main.bin of=image.bin
# seek=UBOOT_LOAD_ADDR
dd bs=1024c seek=64 if=../u-boot/mapu/u-boot.bin of=image.bin oflag=append
# seek=KERNEL_LOAD_ADDR
#dd bs=1024c seek=4096 if=~/linux-3.4.85/arch/arm/boot/uImage of=image.bin oflag=append
#dd bs=1024c seek=14K if=../work/disk-ext.img of=image.bin oflag=append
dd bs=1K seek=1K if=~/linux-3.4.85/arch/arm/boot/uImage of=image.bin oflag=append
# dd bs=1K seek=3K if=../work/fs/fs_mini.jffs2 of=image.bin oflag=append
dd bs=1K seek=3k if=/home/wangl/MaPU_test.jffs2 of=image.bin oflag=append
#dd bs=1K seek=3k if=~/busybox-1.22.1/busybox.jffs2 of=image.bin oflag=append
