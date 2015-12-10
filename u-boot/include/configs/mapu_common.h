/*
 * (C) Copyright 2011 ARM Limited
 * (C) Copyright 2010 Linaro
 * Matt Waddel, <matt.waddel@linaro.org>
 *
 * Configuration for Versatile Express. Parts were derived from other ARM
 *   configurations.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __MAPU_COMMON_H
#define __MAPU_COMMON_H

#define CONFIG_MAPU
#define CONFIG_STANDALONE_LOAD_ADDR 0x60008000

/* Get time */
#define CONFIG_CMD_GETTIME

#define DEBUG_JFFS
#define CONFIG_CMD_HELLO    /* Hello u-boot suppprt*/
#define CONFIG_CMD_LOADELF  /* LOADELF u-boot support */
#define CONFIG_FS_FAT       /* FAT u-boot support */

/* JFFS2 */
#define CONFIG_CMD_JFFS2    /* JFFS2 command support */
#define MAPU_DMA_JFFS2     /* JFFS2 transfer by dma */

/* MTD */
#define CONFIG_MTD_PARTITIONS
#define CONFIG_MTD_DEVICE
#define CONFIG_FLASH_CFI_MTD
#define MTDIDS_DEFAULT    "nor0=TQM8xxL-0"

#define MTDPARTS_DEFAULT  "mtdparts=TQM8xxL-0:1m(bootrom+u-boot),"  \
            "2m(kernel),"  \
            "10m(rootfs),"

#define CONFIG_ENV_IS_NOWHERE		1
#define CONFIG_BOOTP_VCI_STRING     "U-boot.armv7.mapu_common"

/*
 * Definitions copied from linux kernel:
 * arch/arm/mach-vexpress/include/mach/motherboard.h
 */
/* CS register bases for the original memory map. */
#define V2M_PA_CS0		0x40000000
#define V2M_PA_CS1		0x44000000
#define V2M_PA_CS2		0x48000000
#define V2M_PA_CS3		0x4c000000
#define V2M_PA_CS7		0x10000000

#define V2M_PERIPH_OFFSET(x)	(x << 12)
#define V2M_SYSREGS		0x01000000 //(V2M_PA_CS7 + V2M_PERIPH_OFFSET(0))
#define V2M_SYSCTL		(V2M_PA_CS7 + V2M_PERIPH_OFFSET(1))
#define V2M_SERIAL_BUS_PCI	(V2M_PA_CS7 + V2M_PERIPH_OFFSET(2))

#define V2M_BASE		0x60000000
#define CONFIG_SYS_TEXT_BASE	0x60800000

/*
 * Physical addresses, offset from V2M_PA_CS0-3
 */
#define V2M_NOR0		0//(V2M_PA_CS0)
#define V2M_NOR1		(V2M_PA_CS1)
#define V2M_SRAM		(V2M_PA_CS2)
#define V2M_VIDEO_SRAM		(V2M_PA_CS3 + 0x00000000)
#define V2M_LAN9118		(V2M_PA_CS3 + 0x02000000)
#define V2M_ISP1761		(V2M_PA_CS3 + 0x03000000)

/* Common peripherals relative to CS7. */
#define V2M_AACI		(V2M_PA_CS7 + V2M_PERIPH_OFFSET(4))
#define V2M_MMCI		(V2M_PA_CS7 + V2M_PERIPH_OFFSET(5))
#define V2M_KMI0		(V2M_PA_CS7 + V2M_PERIPH_OFFSET(6))
#define V2M_KMI1		(V2M_PA_CS7 + V2M_PERIPH_OFFSET(7))

#define V2M_UART0		(V2M_PA_CS7 + V2M_PERIPH_OFFSET(9))
#define V2M_UART1		(V2M_PA_CS7 + V2M_PERIPH_OFFSET(10))
#define V2M_UART2		(V2M_PA_CS7 + V2M_PERIPH_OFFSET(11))
#define V2M_UART3		(V2M_PA_CS7 + V2M_PERIPH_OFFSET(12))

#define V2M_WDT			(V2M_PA_CS7 + V2M_PERIPH_OFFSET(15))

#define V2M_TIMER01		0x50400000 //(V2M_PA_CS7 + V2M_PERIPH_OFFSET(17))
#define V2M_TIMER23		(V2M_PA_CS7 + V2M_PERIPH_OFFSET(18))

#define V2M_SERIAL_BUS_DVI	(V2M_PA_CS7 + V2M_PERIPH_OFFSET(22))
#define V2M_RTC			(V2M_PA_CS7 + V2M_PERIPH_OFFSET(23))

#define V2M_CF			(V2M_PA_CS7 + V2M_PERIPH_OFFSET(26))

#define V2M_CLCD		(V2M_PA_CS7 + V2M_PERIPH_OFFSET(31))
#define V2M_SIZE_CS7		V2M_PERIPH_OFFSET(32)

/* System register offsets. */
#define V2M_SYS_CFGDATA		(V2M_SYSREGS + 0x0a0)
#define V2M_SYS_CFGCTRL		(V2M_SYSREGS + 0x0a4)
#define V2M_SYS_CFGSTAT		(V2M_SYSREGS + 0x0a8)

/*
 * Configuration
 */
#define SYS_CFG_START		(1 << 31)
#define SYS_CFG_WRITE		(1 << 30)
#define SYS_CFG_OSC		(1 << 20)
#define SYS_CFG_VOLT		(2 << 20)
#define SYS_CFG_AMP		(3 << 20)
#define SYS_CFG_TEMP		(4 << 20)
#define SYS_CFG_RESET		(5 << 20)
#define SYS_CFG_SCC		(6 << 20)
#define SYS_CFG_MUXFPGA		(7 << 20)
#define SYS_CFG_SHUTDOWN	(8 << 20)
#define SYS_CFG_REBOOT		(9 << 20)
#define SYS_CFG_DVIMODE		(11 << 20)
#define SYS_CFG_POWER		(12 << 20)
#define SYS_CFG_SITE_MB		(0 << 16)
#define SYS_CFG_SITE_DB1	(1 << 16)
#define SYS_CFG_SITE_DB2	(2 << 16)
#define SYS_CFG_STACK(n)	((n) << 12)

#define SYS_CFG_ERR		(1 << 1)
#define SYS_CFG_COMPLETE	(1 << 0)

/* Board info register */
#define SYS_ID				V2M_SYSREGS
#define CONFIG_REVISION_TAG		1

#define CONFIG_SYS_MEMTEST_START	V2M_BASE
#define CONFIG_SYS_MEMTEST_END		0x10000000   /* 0x20000000 -> 0x10000000 */

#define CONFIG_CMDLINE_TAG		1	/* enable passing of ATAGs */
#define CONFIG_SETUP_MEMORY_TAGS	1
#define CONFIG_SYS_L2CACHE_OFF		1
#define CONFIG_INITRD_TAG		1

#define CONFIG_OF_LIBFDT		1

/* Size of malloc() pool */
#define CONFIG_SYS_MALLOC_LEN		(CONFIG_ENV_SIZE + 16 * 1024 * 1014)

#define SCTL_BASE			V2M_SYSCTL
#define MAPU_FLASHPROG_FLVPPEN	(1 << 0)

#define CONFIG_SYS_TIMER_COUNTER	(V2M_TIMER01 + 0x4)
#define CONFIG_SYS_TIMER_COUNTS_DOWN

/* NS16550 Serial Configuration */
#define CONFIG_SYS_NS16550
#define CONFIG_SYS_NS16550_SERIAL
#define CONFIG_SYS_NS16550_REG_SIZE	(-4)
#define CONFIG_CONS_INDEX		1
#define CONFIG_SYS_NS16550_CLK		4800
#define CONFIG_SYS_NS16550_COM1		0x50900000	/* Base EVM has UART0 */
#define CONFIG_BAUDRATE		300
#define CONFIG_SYS_BAUDRATE_TABLE  \
    {300, 600, 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200, 230400}


/* Command line configuration */
#define CONFIG_MENU_SHOW
#define CONFIG_CMD_BOOTMENU
#define CONFIG_MENU
#define CONFIG_CMD_ELF
#define CONFIG_CMD_FLASH
#define CONFIG_CMD_MTDPARTS
#define CONFIG_CMD_IMI
#define CONFIG_CMD_MEMORY
#define CONFIG_CMD_NET
#define CONFIG_CMD_PING
#define CONFIG_CMD_RUN
#define CONFIG_CMD_BOOTZ
#define CONFIG_SUPPORT_RAW_INITRD

#define CONFIG_CMD_EXT2
#define CONFIG_CMD_EXT4
#define CONFIG_CMD_FAT
#define CONFIG_DOS_PARTITION		1

/* BOOTP options */
#define CONFIG_BOOTP_BOOTFILESIZE
#define CONFIG_BOOTP_BOOTPATH
#define CONFIG_BOOTP_GATEWAY
#define CONFIG_BOOTP_HOSTNAME
#define CONFIG_BOOTP_PXE
#define CONFIG_BOOTP_PXE_CLIENTARCH	0x100

/* Miscellaneous configurable options */
#undef	CONFIG_SYS_CLKS_IN_HZ
#define CONFIG_SYS_LOAD_ADDR		(V2M_BASE + 0x8000)
#define LINUX_BOOT_PARAM_ADDR		(V2M_BASE + 0x2000)
#define CONFIG_BOOTDELAY		5

/* Physical Memory Map */
#define CONFIG_NR_DRAM_BANKS		1
#define PHYS_SDRAM_1			(V2M_BASE)	/* SDRAM Bank #1 */
#define PHYS_SDRAM_2			(((unsigned int)V2M_BASE) + \
					((unsigned int)0x10000000))      /* 0x20000000->0x10000000 */
#define PHYS_SDRAM_1_SIZE		0x10000000	/* 256 MB */   /* 0x20000000->0x10000000 */
#define PHYS_SDRAM_2_SIZE		0x10000000	/* 256 MB */   /* 0x20000000->0x10000000 */

/* additions for new relocation code */
#define CONFIG_SYS_SDRAM_BASE		PHYS_SDRAM_1
#define CONFIG_SYS_INIT_RAM_SIZE		0x1000
#define CONFIG_SYS_GBL_DATA_OFFSET	(CONFIG_SYS_SDRAM_BASE + \
					 CONFIG_SYS_INIT_RAM_SIZE - \
					 GENERATED_GBL_DATA_SIZE)
#define CONFIG_SYS_INIT_SP_ADDR		CONFIG_SYS_GBL_DATA_OFFSET

#define CONFIG_SHM_BASE   0x40400000

/* Basic environment settings */
#define CONFIG_BOOTCOMMAND		"run bootnor;"
#define CONFIG_PLATFORM_ENV_SETTINGS \
		"loadaddr=0x60007fc0\0" \
		"ramdisk_addr_r=0x61000000\0" \
		"kernel_addr=0x44100000\0" \
		"ramdisk_addr=0x44800000\0" \
		"maxramdisk=0x1800000\0" \
		"pxefile_addr_r=0x88000000\0" \
		"kernel_addr_r=0x80008000\0"

#define CONFIG_EXTRA_ENV_SETTINGS \
		CONFIG_PLATFORM_ENV_SETTINGS \
		"dram=512M\0" \
		"root=/dev/mtdblock4 rw\0" \
    "flashargs=setenv bootargs earlyprintk mem=${dram} "\
    "root=${root} noinitrd vmalloc=768M init=/sbin/init rootfstype=jffs2 "\
    "console=tty0\0" \
		"bootflash=run flashargs; " \
		  "fatload mmc 0 ${loadaddr} uimage;" \
			"bootm ${loadaddr}\0" \
    "bootmenu_1=Boot 2. linux 3.4.85=run bootflash\0" \
    "bootnor=run flashargs;cp.l 0x100000 0x60007fc0 0x80000;cp.l 0x80000 0x60000000 0x1000;" \
    "bootm 0x60007fc0 - 0x60000000\0" \
    "bootmenu_0=Boot 1. linux 3.4.85 (Nor Flash)=run bootnor\0"\
    "initapclib=mtdparts;chpart nor0,2;apclib sinit;apclib minit;"\
    "apclib dbinit apclib_custom.db\0"

/* FLASH and environment organization */
#define PHYS_FLASH_SIZE			0x04000000	/* 64MB */
#define CONFIG_SYS_FLASH_CFI		1
#define CONFIG_FLASH_CFI_DRIVER		1
#define CONFIG_SYS_FLASH_SIZE		0x04000000
#define CONFIG_SYS_MAX_FLASH_BANKS	1
#define CONFIG_SYS_FLASH_BASE0		V2M_NOR0
#define CONFIG_SYS_MONITOR_BASE		CONFIG_SYS_FLASH_BASE0

/* Timeout values in ticks */
#define CONFIG_SYS_FLASH_ERASE_TOUT	(2 * CONFIG_SYS_HZ) /* Erase Timeout */
#define CONFIG_SYS_FLASH_WRITE_TOUT	(2 * CONFIG_SYS_HZ) /* Write Timeout */

/* 255 0x40000 sectors + first or last sector may have 4 erase regions = 259 */
//#define CONFIG_SYS_MAX_FLASH_SECT	259		/* Max sectors */
#define CONFIG_SYS_MAX_FLASH_SECT	2048		/* Max sectors */
#define FLASH_MAX_SECTOR_SIZE		0x00040000	/* 256 KB sectors */

/* Room required on the stack for the environment data */
#define CONFIG_ENV_SIZE			FLASH_MAX_SECTOR_SIZE

#define CONFIG_SYS_FLASH_USE_BUFFER_WRITE /* use buffered writes */

/*
 * Amount of flash used for environment:
 * We don't know which end has the small erase blocks so we use the penultimate
 * sector location for the environment
 */
#define CONFIG_ENV_SECT_SIZE		FLASH_MAX_SECTOR_SIZE
#define CONFIG_ENV_OVERWRITE		1

/* Store environment at top of flash */
#define CONFIG_ENV_IS_IN_FLASH		0
#define CONFIG_ENV_OFFSET		(PHYS_FLASH_SIZE - \
					(2 * CONFIG_ENV_SECT_SIZE))
#define CONFIG_ENV_ADDR			(CONFIG_SYS_FLASH_BASE0 + \
					 CONFIG_ENV_OFFSET)
#define CONFIG_SYS_FLASH_PROTECTION	/* The devices have real protection */
#define CONFIG_SYS_FLASH_EMPTY_INFO	/* flinfo indicates empty blocks */
#define CONFIG_SYS_FLASH_BANKS_LIST	{ CONFIG_SYS_FLASH_BASE0 }

/* Monitor Command Prompt */
#define CONFIG_SYS_CBSIZE		512	/* Console I/O Buffer Size */
#define CONFIG_SYS_PROMPT		"MaPU# "
#define CONFIG_SYS_PBSIZE		(CONFIG_SYS_CBSIZE + \
					sizeof(CONFIG_SYS_PROMPT) + 16)
#define CONFIG_SYS_HUSH_PARSER

#define CONFIG_SYS_BARGSIZE		CONFIG_SYS_CBSIZE /* Boot args buffer */
#define CONFIG_CMD_SOURCE
#define CONFIG_SYS_LONGHELP
#define CONFIG_AUTO_COMPLETE		/* add autocompletion support	*/
#define CONFIG_CMDLINE_EDITING		1
#define CONFIG_SYS_MAXARGS		16	/* max command args */

/* dma */
#define CONFIG_MAPU_DMA

/* slib or mlib 's max numbers of files and max_length of filenames */
#define file_max_num             20
#define filename_max_length      50

#define SLIB_BASE                0x70500000
#define MLIB_BASE                0x70800000

#endif /* MAPU_COMMON_H */
