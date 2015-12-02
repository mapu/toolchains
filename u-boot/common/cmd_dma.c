/*
 * cmd_dma.c
 *
 *  Created on: Jun 13, 2014
 *      Author: huangw
 */

#include <common.h>
#include <command.h>
#include <linux/ctype.h>
#include <elf.h>
#include <s_record.h>
#include <net.h>
#include <ata.h>
#include <part.h>
#include <fat.h>
#include <fs.h>
#include <asm/arch/dma.h>
#include <asm/io.h>
#include <asm/errno.h>
#include <malloc.h>
#include <linux/stat.h>
#include <jffs2/jffs2.h>
#include <jffs2/jffs2_1pass.h>
#include <linux/list.h>
#include <cramfs/cramfs_fs.h>

#include <apc/apclib.h>

#define cramfs_check(x)		(0)
#define cramfs_ls(x,y)		(0)

volatile unsigned long long *ch_enable =
    (unsigned long long *)(MAPU_DMA4_BASE + CH_EN_REG);

int simple_char2int(char* pstr){
	int integer = 0;
	if(*pstr >= '0' && *pstr <= '9')
		integer = *pstr - '0';
	return integer;
}

void dma_transfer_base(uint32_t chan, uint32_t src, uint32_t dst,
		uint32_t len, uint32_t size) {
	debug("Now configure the dma transfer\n");
	mapu_dma_conf_transfer(chan, src, dst, len, size);

	debug("Now start dma transfer \n");
	mapu_dma_start_transfer(chan);

	debug("Now waiting whether the dma transfer is complete\n ");
	mapu_dma_wait_for_transfer(chan);

}

int dma_transfer_consider_len(uint32_t chan, uint32_t src, uint32_t dst,
		uint32_t len, uint32_t size){
	uint32_t original_len = 0;
	while (len > 0x800) {
		dma_transfer_base(chan, src, dst, 0x800, size);
		len -= 0x800;
		src += 0x800 << size;
		dst += 0x800 << size;
		original_len += 0x800;
	}
	dma_transfer_base(chan, src, dst, len, size);
	return original_len + len;
}

void dma_transfer(uint32_t src, uint32_t dst, uint32_t len, uint32_t size) {
	uint32_t chan;

	debug("Now checking which channel can be used...\n");
	do {
		for (chan = 0; chan < DMAH_NUM_CHANNELS; chan++)
			if (!((*ch_enable) & (0x1 << chan))) break;
	} while (chan == DMAH_NUM_CHANNELS);
	dma_transfer_consider_len(chan, src, dst, len, size);
}

/* ======================================================================
 * Interpreter command to boot an arbitrary ELF image from memory.
 * ====================================================================== */
int do_dma_transfer(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	uint32_t src;
	uint32_t dst;
	uint32_t len;
	uint32_t size;

	if (argc < 3 || argc > 5)
		return CMD_RET_USAGE;
	else if(argc == 3){
		len = 0x800;
		size = 2;
	}else if(argc == 4){
		len = simple_strtoul(argv[3], NULL, 16);
		size = 2;
	} else {
		len = simple_strtoul(argv[3], NULL, 16);
		size = simple_char2int(argv[4]);
		if(size < 0 || size > 5)
			return CMD_RET_USAGE;
	}

	src = simple_strtoul(argv[1], NULL, 16);
	dst = simple_strtoul(argv[2], NULL, 16);

	dma_transfer(src, dst, len, size);

	return 0;
}
/* ====================================================================== */

U_BOOT_CMD(
	dmatran,	5,	0,	do_dma_transfer,
	"load a content from src addr to dest addr",
	"<src_addr> <dest_addr> length size \n"
	"    - transfer a content with len*(2^size) bytes from src_addr to dest_addr by dma\n"
	"      for example:\n"
	"      dmatran 0x60900000 0x70500000 0x800 2"
);
