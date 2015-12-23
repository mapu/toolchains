/*
 * cmd_dma.c
 *
 *  Created on: Jun 30, 2014
 *      Author: huangw
 */

#include <common.h>
#include <config.h>
#include <malloc.h>
#include <div64.h>
#include <linux/stat.h>
#include <linux/time.h>
#include <watchdog.h>
#include <jffs2/jffs2.h>
#include <jffs2/jffs2_1pass.h>
#include <linux/compat.h>
#include <asm/errno.h>
#include "../fs/jffs2/jffs2_private.h"
#include <elf.h>

#include <apc/apclib.h>

#define	NODE_CHUNK	1024	/* size of memory allocation chunk in b_nodes */
#define	SPIN_BLKSIZE	18	/* spin after having scanned 1<<BLKSIZE bytes */

/* Debugging switches */
#undef	DEBUG_DIRENTS		/* print directory entry list after scan */
#undef	DEBUG_FRAGMENTS		/* print fragment list after scan */
#undef	DEBUG			/* enable debugging messages */


#ifdef  DEBUG
# define DEBUGF(fmt,args...)	printf(fmt ,##args)
#else
# define DEBUGF(fmt,args...)
#endif

#include "../fs/jffs2/summary.h"

#define SLIB_LEN           sizeof(slib_t)
#define MLIB_LEN           sizeof(mlib_t)
LIST_HEAD(slib_db);
LIST_HEAD(mlib_db);

/* keeps pointer to currentlu processed partition */
static struct part_info *current_part;

extern void dma_transfer(uint32_t src, uint32_t dst, uint32_t len,
                         uint32_t size);

#if defined(CONFIG_CMD_FLASH)
/*
 * Support for jffs2 on top of NOR-flash
 *
 * NOR flash memory is mapped in processor's address space,
 * just return address.
 */
static inline void *get_fl_mem_nor(u32 off, u32 size, void *ext_buf)
{
	u32 addr = off;
	struct mtdids *id = current_part->dev->id;

	extern flash_info_t flash_info[];
	flash_info_t *flash = &flash_info[id->num];

	addr += flash->start[0];
	if (ext_buf) {
		memcpy(ext_buf, (void *)addr, size);
		return ext_buf;
	}
	return (void*)addr;
}

static inline void *get_fl_mem_nor_by_dma(u32 off, u32 size, void *ext_buf)
{
  u32 addr = off;
  struct mtdids *id = current_part->dev->id;

  extern flash_info_t flash_info[];
  flash_info_t *flash = &flash_info[id->num];

  addr += flash->start[0];
  if (ext_buf) {
    /* method 4 : 1'47'' */
    if(size & 0xFFFFFF00) {
      dma_transfer(addr, (uint32_t)ext_buf, size >> 2, 2);
      size &= 0x3;
    }
    memcpy(ext_buf, (void *)addr, size);
  }
  return (void *)addr;
}

static inline void *get_node_mem_nor(u32 off, void *ext_buf, u8 dma)
{
	struct jffs2_unknown_node *pNode;

	/* pNode will point directly to flash - don't provide external buffer
	   and don't care about size */
	pNode = dma ? get_fl_mem_nor_by_dma(off, 0, NULL) : get_fl_mem_nor(off, 0, NULL);
	if (!dma)
	  return (void *)get_fl_mem_nor(off, pNode->magic == JFFS2_MAGIC_BITMASK ?
			                                                 pNode->totlen :
			                                                 sizeof(*pNode),
			                            ext_buf);
	else
	  return (void *)get_fl_mem_nor_by_dma(off,
	                                       pNode->magic == JFFS2_MAGIC_BITMASK ?
	                                                       pNode->totlen :
	                                                       sizeof(*pNode),
	                                       ext_buf);
}
#endif


/*
 * Generic jffs2 raw memory and node read routines.
 *
 */
static inline void *get_fl_mem(u32 off, u32 size, void *ext_buf, u8 dma)
{
	struct mtdids *id = current_part->dev->id;

	switch(id->type) {
#if defined(CONFIG_CMD_FLASH)
	case MTD_DEV_TYPE_NOR:
		return dma ? get_fl_mem_nor_by_dma(off, size, ext_buf) :
		             get_fl_mem_nor(off, size, ext_buf);
		break;
#endif
#if defined(CONFIG_JFFS2_NAND) && defined(CONFIG_CMD_NAND)
	case MTD_DEV_TYPE_NAND:
		return get_fl_mem_nand(off, size, ext_buf);
		break;
#endif
#if defined(CONFIG_CMD_ONENAND)
	case MTD_DEV_TYPE_ONENAND:
		return get_fl_mem_onenand(off, size, ext_buf);
		break;
#endif
	default:
		printf("get_fl_mem: unknown device type, " \
			"using raw offset!\n");
	}
	return (void*)off;
}

static inline void *get_node_mem(u32 off, void *ext_buf, u8 dma)
{
	struct mtdids *id = current_part->dev->id;

	switch(id->type) {
#if defined(CONFIG_CMD_FLASH)
	case MTD_DEV_TYPE_NOR:
		return get_node_mem_nor(off, ext_buf, dma);
		break;
#endif
#if defined(CONFIG_JFFS2_NAND) && \
    defined(CONFIG_CMD_NAND)
	case MTD_DEV_TYPE_NAND:
		return get_node_mem_nand(off, ext_buf);
		break;
#endif
#if defined(CONFIG_CMD_ONENAND)
	case MTD_DEV_TYPE_ONENAND:
		return get_node_mem_onenand(off, ext_buf);
		break;
#endif
	default:
		printf("get_fl_mem: unknown device type, " \
			"using raw offset!\n");
	}
	return (void*)off;
}

static inline void put_fl_mem(void *buf, void *ext_buf)
{
	struct mtdids *id = current_part->dev->id;

	/* If buf is the same as ext_buf, it was provided by the caller -
	   we shouldn't free it then. */
	if (buf == ext_buf)
		return;
	switch (id->type) {
#if defined(CONFIG_JFFS2_NAND) && defined(CONFIG_CMD_NAND)
	case MTD_DEV_TYPE_NAND:
		return put_fl_mem_nand(buf);
#endif
#if defined(CONFIG_CMD_ONENAND)
	case MTD_DEV_TYPE_ONENAND:
		return put_fl_mem_onenand(buf);
#endif
	}
}

/* Memory management */
struct mem_block {
	u32	index;
	struct mem_block *next;
	struct b_node nodes[NODE_CHUNK];
};


static void
free_nodes(struct b_list *list)
{
	while (list->listMemBase != NULL) {
		struct mem_block *next = list->listMemBase->next;
		free( list->listMemBase );
		list->listMemBase = next;
	}
}

static struct b_node *
add_node(struct b_list *list)
{
	u32 index = 0;
	struct mem_block *memBase;
	struct b_node *b;

	memBase = list->listMemBase;
	if (memBase != NULL)
		index = memBase->index;
#if 0
	putLabeledWord("add_node: index = ", index);
	putLabeledWord("add_node: memBase = ", list->listMemBase);
#endif

	if (memBase == NULL || index >= NODE_CHUNK) {
		/* we need more space before we continue */
		memBase = mmalloc(sizeof(struct mem_block));
		if (memBase == NULL) {
			putstr("add_node: malloc failed\n");
			return NULL;
		}
		memBase->next = list->listMemBase;
		index = 0;
#if 0
		putLabeledWord("add_node: alloced a new membase at ", *memBase);
#endif

	}
	/* now we have room to add it. */
	b = &memBase->nodes[index];
	index ++;

	memBase->index = index;
	list->listMemBase = memBase;
	list->listCount++;
	return b;
}

static struct b_node *
insert_node(struct b_list *list, u32 offset)
{
	struct b_node *new;
#ifdef CONFIG_SYS_JFFS2_SORT_FRAGMENTS
	struct b_node *b, *prev;
#endif

	if (!(new = add_node(list))) {
		putstr("add_node failed!\r\n");
		return NULL;
	}
	new->offset = offset;

#ifdef CONFIG_SYS_JFFS2_SORT_FRAGMENTS
	if (list->listTail != NULL && list->listCompare(new, list->listTail))
		prev = list->listTail;
	else if (list->listLast != NULL && list->listCompare(new, list->listLast))
		prev = list->listLast;
	else
		prev = NULL;

	for (b = (prev ? prev->next : list->listHead);
	     b != NULL && list->listCompare(new, b);
	     prev = b, b = b->next) {
		list->listLoops++;
	}
	if (b != NULL)
		list->listLast = prev;

	if (b != NULL) {
		new->next = b;
		if (prev != NULL)
			prev->next = new;
		else
			list->listHead = new;
	} else
#endif
	{
		new->next = (struct b_node *) NULL;
		if (list->listTail != NULL) {
			list->listTail->next = new;
			list->listTail = new;
		} else {
			list->listTail = list->listHead = new;
		}
	}

	return new;
}

#ifdef CONFIG_SYS_JFFS2_SORT_FRAGMENTS
/* Sort data entries with the latest version last, so that if there
 * is overlapping data the latest version will be used.
 */
static int compare_inodes(struct b_node *new, struct b_node *old)
{
	struct jffs2_raw_inode ojNew;
	struct jffs2_raw_inode ojOld;
	struct jffs2_raw_inode *jNew =
		(struct jffs2_raw_inode *)get_fl_mem(new->offset, sizeof(ojNew), &ojNew);
	struct jffs2_raw_inode *jOld =
		(struct jffs2_raw_inode *)get_fl_mem(old->offset, sizeof(ojOld), &ojOld);

	return jNew->version > jOld->version;
}

/* Sort directory entries so all entries in the same directory
 * with the same name are grouped together, with the latest version
 * last. This makes it easy to eliminate all but the latest version
 * by marking the previous version dead by setting the inode to 0.
 */
static int compare_dirents(struct b_node *new, struct b_node *old)
{
	struct jffs2_raw_dirent ojNew;
	struct jffs2_raw_dirent ojOld;
	struct jffs2_raw_dirent *jNew =
		(struct jffs2_raw_dirent *)get_fl_mem(new->offset, sizeof(ojNew), &ojNew);
	struct jffs2_raw_dirent *jOld =
		(struct jffs2_raw_dirent *)get_fl_mem(old->offset, sizeof(ojOld), &ojOld);
	int cmp;

	/* ascending sort by pino */
	if (jNew->pino != jOld->pino)
		return jNew->pino > jOld->pino;

	/* pino is the same, so use ascending sort by nsize, so
	 * we don't do strncmp unless we really must.
	 */
	if (jNew->nsize != jOld->nsize)
		return jNew->nsize > jOld->nsize;

	/* length is also the same, so use ascending sort by name
	 */
	cmp = strncmp((char *)jNew->name, (char *)jOld->name, jNew->nsize);
	if (cmp != 0)
		return cmp > 0;

	/* we have duplicate names in this directory, so use ascending
	 * sort by version
	 */
	if (jNew->version > jOld->version) {
		/* since jNew is newer, we know jOld is not valid, so
		 * mark it with inode 0 and it will not be used
		 */
		jOld->ino = 0;
		return 1;
	}

	return 0;
}
#endif

static void
jffs2_free_cache(struct part_info *part)
{
	struct b_lists *pL;

	if (part->jffs2_priv != NULL) {
		pL = (struct b_lists *)part->jffs2_priv;
		free_nodes(&pL->frag);
		free_nodes(&pL->dir);
		free(pL->readbuf);
		free(pL);
	}
}

static u32
jffs_init_1pass_list(struct part_info *part)
{
	struct b_lists *pL;

	jffs2_free_cache(part);

	if (NULL != (part->jffs2_priv = malloc(sizeof(struct b_lists)))) {
		pL = (struct b_lists *)part->jffs2_priv;

		memset(pL, 0, sizeof(*pL));
#ifdef CONFIG_SYS_JFFS2_SORT_FRAGMENTS
		pL->dir.listCompare = compare_dirents;
		pL->frag.listCompare = compare_inodes;
#endif
	}
	return 0;
}

/* find the inode from the slashless name given a parent */
static u32
jffs2_1pass_find_inode(struct b_lists * pL, const char *name, u32 pino, u8 dma)
{
	struct b_node *b;
	struct jffs2_raw_dirent *jDir;
	int len;
	u32 counter;
	u32 version = 0;
	u32 inode = 0;

	/* name is assumed slash free */
	len = strlen(name);

	counter = 0;
	/* we need to search all and return the inode with the highest version */
	for(b = pL->dir.listHead; b; b = b->next, counter++) {
		jDir = (struct jffs2_raw_dirent *) get_node_mem(b->offset,
								pL->readbuf, dma);
		if ((pino == jDir->pino) && (len == jDir->nsize) &&
		    (jDir->ino) &&	/* 0 for unlink */
		    (!strncmp((char *)jDir->name, name, len))) {	/* a match */
			if (jDir->version < version) {
				put_fl_mem(jDir, pL->readbuf);
				continue;
			}

			if (jDir->version == version && inode != 0) {
				/* I'm pretty sure this isn't legal */
				putstr(" ** ERROR ** ");
				putnstr(jDir->name, jDir->nsize);
				putLabeledWord(" has dup version =", version);
			}
			inode = jDir->ino;
			version = jDir->version;
		}
#if 0
		putstr("\r\nfind_inode:p&l ->");
		putnstr(jDir->name, jDir->nsize);
		putstr("\r\n");
		putLabeledWord("pino = ", jDir->pino);
		putLabeledWord("nsize = ", jDir->nsize);
		putLabeledWord("b = ", (u32) b);
		putLabeledWord("counter = ", counter);
#endif
		put_fl_mem(jDir, pL->readbuf);
	}
	return inode;
}

static unsigned char
jffs2_1pass_rescan_needed(struct part_info *part, u8 dma)
{
  struct b_node *b;
  struct jffs2_unknown_node onode;
  struct jffs2_unknown_node *node;
  struct b_lists *pL = (struct b_lists *)part->jffs2_priv;

  if (part->jffs2_priv == 0){
    DEBUGF ("rescan: First time in use\n");
    return 1;
  }

  /* if we have no list, we need to rescan */
  if (pL->frag.listCount == 0) {
    DEBUGF ("rescan: fraglist zero\n");
    return 1;
  }

  /* but suppose someone reflashed a partition at the same offset... */
  b = pL->dir.listHead;
  while (b) {
    node = (struct jffs2_unknown_node *)get_fl_mem(b->offset,
                                                   sizeof(onode),
                                                   &onode, dma);
    if (node->nodetype != JFFS2_NODETYPE_DIRENT) {
      DEBUGF ("rescan: fs changed beneath me? (%lx)\n",
          (unsigned long) b->offset);
      return 1;
    }
    b = b->next;
  }
  return 0;
}

#define DEFAULT_EMPTY_SCAN_SIZE 4096

static inline uint32_t EMPTY_SCAN_SIZE(uint32_t sector_size)
{
  if (sector_size < DEFAULT_EMPTY_SCAN_SIZE)
    return sector_size;
  else
    return DEFAULT_EMPTY_SCAN_SIZE;
}

static u32
jffs2_1pass_build_lists(struct part_info * part, u8 dma)
{
  struct b_lists *pL;
  struct jffs2_unknown_node *node;
  u32 nr_sectors;
  u32 i;
  u32 counter4 = 0;
  u32 counterF = 0;
  u32 counterN = 0;
  u32 max_totlen = 0;
  u32 buf_size = DEFAULT_EMPTY_SCAN_SIZE;
  char *buf;

  nr_sectors = lldiv(part->size, part->sector_size);
  /* turn off the lcd.  Refreshing the lcd adds 50% overhead to the */
  /* jffs2 list building enterprise nope.  in newer versions the overhead is */
  /* only about 5 %.  not enough to inconvenience people for. */
  /* lcd_off(); */

  /* if we are building a list we need to refresh the cache. */
  jffs_init_1pass_list(part);
  pL = (struct b_lists *)part->jffs2_priv;
  buf = malloc(buf_size);
  puts ("Scanning JFFS2 FS:   ");

  /* start at the beginning of the partition */
  for (i = 0; i < nr_sectors; i++) {
    uint32_t sector_ofs = i * part->sector_size;
    uint32_t buf_ofs = sector_ofs;
    uint32_t buf_len;
    uint32_t ofs, prevofs;
#ifdef CONFIG_JFFS2_SUMMARY
    struct jffs2_sum_marker *sm;
    void *sumptr = NULL;
    uint32_t sumlen;
    int ret;
#endif

    WATCHDOG_RESET();

#ifdef CONFIG_JFFS2_SUMMARY
    buf_len = sizeof(*sm);

    /* Read as much as we want into the _end_ of the preallocated
     * buffer
     */
    get_fl_mem(part->offset + sector_ofs + part->sector_size -
        buf_len, buf_len, buf + buf_size - buf_len);

    sm = (void *)buf + buf_size - sizeof(*sm);
    if (sm->magic == JFFS2_SUM_MAGIC) {
      sumlen = part->sector_size - sm->offset;
      sumptr = buf + buf_size - sumlen;

      /* Now, make sure the summary itself is available */
      if (sumlen > buf_size) {
        /* Need to kmalloc for this. */
        sumptr = malloc(sumlen);
        if (!sumptr) {
          putstr("Can't get memory for summary "
              "node!\n");
          free(buf);
          jffs2_free_cache(part);
          return 0;
        }
        memcpy(sumptr + sumlen - buf_len, buf +
            buf_size - buf_len, buf_len);
      }
      if (buf_len < sumlen) {
        /* Need to read more so that the entire summary
         * node is present
         */
        get_fl_mem(part->offset + sector_ofs +
            part->sector_size - sumlen,
            sumlen - buf_len, sumptr);
      }
    }

    if (sumptr) {
      ret = jffs2_sum_scan_sumnode(part, sector_ofs, sumptr,
          sumlen, pL);

      if (buf_size && sumlen > buf_size)
        free(sumptr);
      if (ret < 0) {
        free(buf);
        jffs2_free_cache(part);
        return 0;
      }
      if (ret)
        continue;

    }
#endif /* CONFIG_JFFS2_SUMMARY */

    buf_len = EMPTY_SCAN_SIZE(part->sector_size);

    get_fl_mem((u32)part->offset + buf_ofs, buf_len, buf, dma);

    /* We temporarily use 'ofs' as a pointer into the buffer/jeb */
    ofs = 0;

    /* Scan only 4KiB of 0xFF before declaring it's empty */
    while (ofs < EMPTY_SCAN_SIZE(part->sector_size) &&
        *(uint32_t *)(&buf[ofs]) == 0xFFFFFFFF)
      ofs += 4;

    if (ofs == EMPTY_SCAN_SIZE(part->sector_size))
      continue;

    ofs += sector_ofs;
    prevofs = ofs - 1;

  scan_more:
    while (ofs < sector_ofs + part->sector_size) {
      if (ofs == prevofs) {
        printf("offset %08x already seen, skip\n", ofs);
        ofs += 4;
        counter4++;
        continue;
      }
      prevofs = ofs;
      if (sector_ofs + part->sector_size <
          ofs + sizeof(*node))
        break;
      if (buf_ofs + buf_len < ofs + sizeof(*node)) {
        buf_len = min_t(uint32_t, buf_size,
                        sector_ofs + part->sector_size - ofs);
        get_fl_mem((u32)part->offset + ofs, buf_len, buf, dma);
        buf_ofs = ofs;
      }

      node = (struct jffs2_unknown_node *)&buf[ofs-buf_ofs];

      if (*(uint32_t *)(&buf[ofs-buf_ofs]) == 0xffffffff) {
        uint32_t inbuf_ofs;
        uint32_t scan_end;

        ofs += 4;
        scan_end = min_t(uint32_t, EMPTY_SCAN_SIZE(part->sector_size) / 8,
                         buf_len);
      more_empty:
        inbuf_ofs = ofs - buf_ofs;
        while (inbuf_ofs < scan_end) {
          if (*(uint32_t *)(&buf[inbuf_ofs]) !=
              0xffffffff)
            goto scan_more;

          inbuf_ofs += 4;
          ofs += 4;
        }
        /* Ran off end. */

        /* See how much more there is to read in this
         * eraseblock...
         */
        buf_len = min_t(uint32_t, buf_size,
                        sector_ofs + part->sector_size - ofs);
        if (!buf_len) {
          /* No more to read. Break out of main
           * loop without marking this range of
           * empty space as dirty (because it's
           * not)
           */
          break;
        }
        scan_end = buf_len;
        get_fl_mem((u32)part->offset + ofs, buf_len, buf, dma);
        buf_ofs = ofs;
        goto more_empty;
      }
      if (node->magic != JFFS2_MAGIC_BITMASK ||
          !hdr_crc(node)) {
        ofs += 4;
        counter4++;
        continue;
      }
      if (ofs + node->totlen >
          sector_ofs + part->sector_size) {
        ofs += 4;
        counter4++;
        continue;
      }
      /* if its a fragment add it */
      switch (node->nodetype) {
      case JFFS2_NODETYPE_INODE:
        if (buf_ofs + buf_len < ofs + sizeof(struct
              jffs2_raw_inode)) {
          get_fl_mem((u32)part->offset + ofs, buf_len, buf, dma);
          buf_ofs = ofs;
          node = (void *)buf;
        }
        if (!inode_crc((struct jffs2_raw_inode *) node))
          break;

        if (insert_node(&pL->frag, (u32) part->offset +
            ofs) == NULL) {
          free(buf);
          jffs2_free_cache(part);
          return 0;
        }
        if (max_totlen < node->totlen)
          max_totlen = node->totlen;
        break;
      case JFFS2_NODETYPE_DIRENT:
        if (buf_ofs + buf_len < ofs +
            sizeof(struct jffs2_raw_dirent) +
            ((struct jffs2_raw_dirent *)node)->nsize) {
          get_fl_mem((u32)part->offset + ofs, buf_len, buf, dma);
          buf_ofs = ofs;
          node = (void *)buf;
        }

        if (!dirent_crc((struct jffs2_raw_dirent *)node) ||
            !dirent_name_crc((struct jffs2_raw_dirent *)node))
          break;
        if (! (counterN%100))
          puts ("\b\b.  ");
        if (insert_node(&pL->dir, (u32) part->offset + ofs) == NULL) {
          free(buf);
          jffs2_free_cache(part);
          return 0;
        }
        if (max_totlen < node->totlen)
          max_totlen = node->totlen;
        counterN++;
        break;
      case JFFS2_NODETYPE_CLEANMARKER:
        if (node->totlen != sizeof(struct jffs2_unknown_node))
          printf("OOPS Cleanmarker has bad size %d != %zu\n", node->totlen,
                 sizeof(struct jffs2_unknown_node));
        break;
      case JFFS2_NODETYPE_PADDING:
        if (node->totlen < sizeof(struct jffs2_unknown_node))
          printf("OOPS Padding has bad size %d < %zu\n", node->totlen,
                 sizeof(struct jffs2_unknown_node));
        break;
      case JFFS2_NODETYPE_SUMMARY:
        break;
      default:
        printf("Unknown node type: %x len %d offset 0x%x\n",
               node->nodetype,node->totlen, ofs);
      }
      ofs += ((node->totlen + 3) & ~3);
      counterF++;
    }
  }

  free(buf);
  putstr("\b\b done.\r\n");   /* close off the dots */

  /* We don't care if malloc failed - then each read operation will
   * allocate its own buffer as necessary (NAND) or will read directly
   * from flash (NOR).
   */
  pL->readbuf = malloc(max_totlen);

  /* turn the lcd back on. */
  /* splash(); */

#if 0
  putLabeledWord("dir entries = ", pL->dir.listCount);
  putLabeledWord("frag entries = ", pL->frag.listCount);
  putLabeledWord("+4 increments = ", counter4);
  putLabeledWord("+file_offset increments = ", counterF);

#endif

#ifdef DEBUG_DIRENTS
  dump_dirents(pL);
#endif

#ifdef DEBUG_FRAGMENTS
  dump_fragments(pL);
#endif

  /* give visual feedback that we are done scanning the flash */
  led_blink(0x0, 0x0, 0x1, 0x1);  /* off, forever, on 100ms, off 100ms */
  return 1;
}

static struct b_lists *
jffs2_get_list(struct part_info * part, const char *who, u8 dma)
{
  /* copy requested part_info struct pointer to global location */
  current_part = part;

  if (jffs2_1pass_rescan_needed(part, dma)) {
    if (!jffs2_1pass_build_lists(part, dma)) {
      printf("%s: Failed to scan JFFSv2 file structure\n", who);
      return NULL;
    }
  }
  return (struct b_lists *)part->jffs2_priv;
}

/* find the inode from the slashless name given a parent by dma */
static u32
jffs2_1pass_find_inode_by_dma(struct b_lists * pL, const char *name, u32 pino)
{
  struct b_node *b;
  struct jffs2_raw_dirent *jDir;
  int len;
  u32 counter;
  u32 version = 0;
  u32 inode = 0;

  /* name is assumed slash free */
  len = strlen(name);

  counter = 0;
  /* we need to search all and return the inode with the highest version */
  for(b = pL->dir.listHead; b; b = b->next, counter++) {
    jDir = (struct jffs2_raw_dirent *)get_node_mem(b->offset, pL->readbuf, 1);
    if ((pino == jDir->pino) && (len == jDir->nsize) &&
        (jDir->ino) &&  /* 0 for unlink */
        (!strncmp((char *)jDir->name, name, len))) {  /* a match */
      if (jDir->version < version) {
        put_fl_mem(jDir, pL->readbuf);
        continue;
      }

      if (jDir->version == version && inode != 0) {
        /* I'm pretty sure this isn't legal */
        putstr(" ** ERROR ** ");
        putnstr(jDir->name, jDir->nsize);
        putLabeledWord(" has dup version =", version);
      }
      inode = jDir->ino;
      version = jDir->version;
    }
#if 0
    putstr("\r\nfind_inode:p&l ->");
    putnstr(jDir->name, jDir->nsize);
    putstr("\r\n");
    putLabeledWord("pino = ", jDir->pino);
    putLabeledWord("nsize = ", jDir->nsize);
    putLabeledWord("b = ", (u32) b);
    putLabeledWord("counter = ", counter);
#endif
    put_fl_mem(jDir, pL->readbuf);
  }
  return inode;
}

static u32
jffs2_1pass_search_inode_by_dma(struct b_lists * pL, const char *fname, u32 pino)
{
  int i;
  char tmp[256];
  char working_tmp[256];
  char *c;

  /* discard any leading slash */
  i = 0;
  while (fname[i] == '/')
    i++;
  strcpy(tmp, &fname[i]);

  while ((c = (char *) strchr(tmp, '/'))) /* we are still dired searching */
  {
    strncpy(working_tmp, tmp, c - tmp);
    working_tmp[c - tmp] = '\0';
#if 0
    putstr("search_inode: tmp = ");
    putstr(tmp);
    putstr("\r\n");
    putstr("search_inode: wtmp = ");
    putstr(working_tmp);
    putstr("\r\n");
    putstr("search_inode: c = ");
    putstr(c);
    putstr("\r\n");
#endif
    for (i = 0; i < strlen(c) - 1; i++)
      tmp[i] = c[i + 1];
    tmp[i] = '\0';
#if 1
    putstr("search_inode: post tmp = ");
    putstr(tmp);
    putstr("\r\n");
#endif

    if (!(pino = jffs2_1pass_find_inode_by_dma(pL, working_tmp, pino))) {
      putstr("find_inode failed for name=");
      putstr(working_tmp);
      putstr("\r\n");
      return 0;
    }
  }
  /* this is for the bare filename, directories have already been mapped */
  if (!(pino = jffs2_1pass_find_inode_by_dma(pL, tmp, pino))) {
    putstr("find_inode failed for name=");
    putstr(tmp);
    putstr("\r\n");
    return 0;
  }
  return pino;
}

/* find the inode from the slashless name given a parent */
static long
jffs2_1pass_read_inode_by_dma(struct b_lists *pL, u32 inode, char *dest)
{
  struct b_node *b;
  struct jffs2_raw_inode *jNode;
  u32 totalSize = 0;
  u32 latestVersion = 0;
  uchar *lDest;
  uchar *src;
  int i;
  u32 counter = 0;
#ifdef CONFIG_SYS_JFFS2_SORT_FRAGMENTS
  /* Find file size before loading any data, so fragments that
   * start past the end of file can be ignored. A fragment
   * that is partially in the file is loaded, so extra data may
   * be loaded up to the next 4K boundary above the file size.
   * This shouldn't cause trouble when loading kernel images, so
   * we will live with it.
   */
  for (b = pL->frag.listHead; b != NULL; b = b->next) {
    jNode = (struct jffs2_raw_inode *) get_fl_mem_by_dma(b->offset,
      sizeof(struct jffs2_raw_inode), pL->readbuf);
    if ((inode == jNode->ino)) {
      /* get actual file length from the newest node */
      if (jNode->version >= latestVersion) {
        totalSize = jNode->isize;
        latestVersion = jNode->version;
      }
    }
    put_fl_mem(jNode, pL->readbuf);
  }
#endif

  for (b = pL->frag.listHead; b != NULL; b = b->next) {
    jNode = (struct jffs2_raw_inode *)get_node_mem(b->offset, pL->readbuf, 1);
    if ((inode == jNode->ino)) {
#if 0
      putLabeledWord("\r\n\r\nread_inode: totlen = ", jNode->totlen);
      putLabeledWord("read_inode: inode = ", jNode->ino);
      putLabeledWord("read_inode: version = ", jNode->version);
      putLabeledWord("read_inode: isize = ", jNode->isize);
      putLabeledWord("read_inode: offset = ", jNode->offset);
      putLabeledWord("read_inode: csize = ", jNode->csize);
      putLabeledWord("read_inode: dsize = ", jNode->dsize);
      putLabeledWord("read_inode: compr = ", jNode->compr);
      putLabeledWord("read_inode: usercompr = ", jNode->usercompr);
      putLabeledWord("read_inode: flags = ", jNode->flags);
#endif

#ifndef CONFIG_SYS_JFFS2_SORT_FRAGMENTS
      /* get actual file length from the newest node */
      if (jNode->version >= latestVersion) {
        totalSize = jNode->isize;
        latestVersion = jNode->version;
      }
#endif

      if(dest) {
        src = ((uchar *) jNode) + sizeof(struct jffs2_raw_inode);
        /* ignore data behind latest known EOF */
        if (jNode->offset > totalSize) {
          put_fl_mem(jNode, pL->readbuf);
          continue;
        }
        if (b->datacrc == CRC_UNKNOWN)
          b->datacrc = data_crc(jNode) ?
            CRC_OK : CRC_BAD;
        if (b->datacrc == CRC_BAD) {
          put_fl_mem(jNode, pL->readbuf);
          continue;
        }

        lDest = (uchar *) (dest + jNode->offset);
#if 0
        putLabeledWord("read_inode: src = ", src);
        putLabeledWord("read_inode: dest = ", lDest);
#endif
        //printf("jNode->compr is 0x%x\n",jNode->compr);

        switch (jNode->compr) {
        case JFFS2_COMPR_NONE:
          ldr_memcpy(lDest, src, jNode->dsize);
          break;
        case JFFS2_COMPR_ZERO:
          for (i = 0; i < jNode->dsize; i++)
            *(lDest++) = 0;
          break;
        case JFFS2_COMPR_RTIME:
          rtime_decompress(src, lDest, jNode->csize, jNode->dsize);
          break;
        case JFFS2_COMPR_DYNRUBIN:
          /* this is slow but it works */
          dynrubin_decompress(src, lDest, jNode->csize, jNode->dsize);
          break;
        case JFFS2_COMPR_ZLIB:
          zlib_decompress(src, lDest, jNode->csize, jNode->dsize);
          break;
#if defined(CONFIG_JFFS2_LZO)
        case JFFS2_COMPR_LZO:
          lzo_decompress(src, lDest, jNode->csize, jNode->dsize);
          break;
#endif
        default:
          /* unknown */
          putLabeledWord("UNKNOWN COMPRESSION METHOD = ", jNode->compr);
          put_fl_mem(jNode, pL->readbuf);
          return -1;
          break;
        }
      }

#if 0
  putLabeledWord("read_inode: totalSize = ", totalSize);
#endif
    }
    counter++;
    put_fl_mem(jNode, pL->readbuf);
  }

#if 0
  putLabeledWord("read_inode: returning = ", totalSize);
#endif
  return totalSize;
}

static u32
jffs2_1pass_resolve_inode_by_dma(struct b_lists * pL, u32 ino)
{
  struct b_node *b;
  struct b_node *b2;
  struct jffs2_raw_dirent *jDir;
  struct jffs2_raw_inode *jNode;
  u8 jDirFoundType = 0;
  u32 jDirFoundIno = 0;
  u32 jDirFoundPino = 0;
  char tmp[256];
  u32 version = 0;
  u32 pino;
  unsigned char *src;

  /* we need to search all and return the inode with the highest version */
  for(b = pL->dir.listHead; b; b = b->next) {
    jDir = (struct jffs2_raw_dirent *)get_node_mem(b->offset, pL->readbuf, 1);
    if (ino == jDir->ino) {
      if (jDir->version < version) {
        put_fl_mem(jDir, pL->readbuf);
        continue;
      }

      if (jDir->version == version && jDirFoundType) {
        /* I'm pretty sure this isn't legal */
        putstr(" ** ERROR ** ");
        putnstr(jDir->name, jDir->nsize);
        putLabeledWord(" has dup version (resolve) = ",
          version);
      }

      jDirFoundType = jDir->type;
      jDirFoundIno = jDir->ino;
      jDirFoundPino = jDir->pino;
      version = jDir->version;
    }
    put_fl_mem(jDir, pL->readbuf);
  }
  /* now we found the right entry again. (shoulda returned inode*) */
  if (jDirFoundType != DT_LNK)
    return jDirFoundIno;

  /* it's a soft link so we follow it again. */
  b2 = pL->frag.listHead;
  while (b2) {
    jNode = (struct jffs2_raw_inode *)get_node_mem(b2->offset, pL->readbuf, 1);
    if (jNode->ino == jDirFoundIno) {
      src = (unsigned char *)jNode + sizeof(struct jffs2_raw_inode);

#if 1
      putLabeledWord("\t\t dsize = ", jNode->dsize);
      putstr("\t\t target = ");
      putnstr(src, jNode->dsize);
      putstr("\r\n");
#endif
      strncpy(tmp, (char *)src, jNode->dsize);
      tmp[jNode->dsize] = '\0';
      put_fl_mem(jNode, pL->readbuf);
      break;
    }
    b2 = b2->next;
    put_fl_mem(jNode, pL->readbuf);
  }
  /* ok so the name of the new file to find is in tmp */
  /* if it starts with a slash it is root based else shared dirs */
  if (tmp[0] == '/')
    pino = 1;
  else
    pino = jDirFoundPino;

  return jffs2_1pass_search_inode_by_dma(pL, tmp, pino);
}

/* Load a file from flash into memory. fname can be a full path */
static u32
jffs2_1pass_load_by_dma(char *dest, struct part_info * part, const char *fname)
{
  struct b_lists *pl;
  long ret = 1;
  u32 inode;

  if (! (pl  = jffs2_get_list(part, "load", 1)))
    return 0;

  if (! (inode = jffs2_1pass_search_inode_by_dma(pl, fname, 1))) {
    putstr("load: Failed to find inode\r\n");
    return 0;
  }

  /* Resolve symlinks */
  if (! (inode = jffs2_1pass_resolve_inode_by_dma(pl, inode))) {
    putstr("load: Failed to resolve inode structure\r\n");
    return 0;
  }

  putstr("Loading by dma ...\n");

  if ((ret = jffs2_1pass_read_inode_by_dma(pl, inode, dest)) < 0) {
    putstr("load: Failed to read inode\r\n");
    return 0;
  }

  DEBUGF ("load: loaded '%s' to 0x%lx (%ld bytes)\n", fname,
        (unsigned long) dest, ret);

  return ret;
}

static u32 jffs2_1pass_list_inodes(char* const command_name, uint32_t dst,
                                   const char *fname, struct part_info * part,
                                   struct b_lists * pL, u32 pino, u8 dma) {
  struct b_node *b;
  struct jffs2_raw_dirent *jDir;
  int file_count = 0;
  ulong load_addr = dst;
  int i = 0;
  char tmp[256];
  char working_tmp[256];
  char *c;
  int tmp_len;
  u32 (*pass_load_ptr)(char *, struct part_info *, const char *);

  slib_t *slib_list_new;
  mlib_t *mlib_text_list_new;

  if (!dma) pass_load_ptr = &jffs2_1pass_load;
  else pass_load_ptr = &jffs2_1pass_load_by_dma;

  /* discard any leading slash */
  while (fname[i] == '/') i++;
  strcpy(tmp, &fname[i]);
  tmp_len = strlen(tmp);
  for(; i < tmp_len; i++)
    if(tmp[i] == '/') break;

  if(i == tmp_len) tmp[i] = '/';     /* add '/' at the end of filename */

  c = strchr(tmp, '/');
  strncpy(working_tmp, tmp, c - tmp);
  working_tmp[c - tmp] = '/';
  working_tmp[c - tmp + 1] = '\0';

  int working_tmp_len = strlen(working_tmp);  //working_tmp is slib

  for (b = pL->dir.listHead; b; b = b->next) {
    jDir = (struct jffs2_raw_dirent *)get_node_mem(b->offset, pL->readbuf, dma);
    if ((pino == jDir->pino) && (jDir->ino)) { /* ino=0 -> unlink */
      u32 i_version = 0;
      struct jffs2_raw_inode ojNode;
      struct jffs2_raw_inode *jNode, *iNode = NULL;
      struct b_node *b2 = pL->frag.listHead;

      while (b2) {
        jNode = (struct jffs2_raw_inode *)get_fl_mem(b2->offset,
                                                     sizeof(ojNode), &ojNode,
                                                     dma);
        if (jNode->ino == jDir->ino && jNode->version >= i_version) {
          i_version = jNode->version;
          if (iNode)
            put_fl_mem(iNode, NULL);

          if (jDir->type == DT_LNK)
            iNode = get_node_mem(b2->offset, NULL, dma);
          else
            iNode = get_fl_mem(b2->offset, sizeof(*iNode), NULL, dma);
        }
        b2 = b2->next;
      }

      char *subfname = (char *)malloc(jDir->nsize + sizeof(char)); //print the sub-filename
      int j, count, size = 0;

      if(!jDir || !iNode)
        return -1;

      strncpy(subfname, (char *)jDir->name, jDir->nsize);
      subfname[jDir->nsize] = '\0';

      char *file_name = (char*)malloc(sizeof(working_tmp) + sizeof(subfname) + sizeof(char));

      /* name is slib/ or mlib */
      if (strcmp(command_name, "slib_init") == 0){       /* dirname is slib*/
        for (count = 0; count < working_tmp_len; count++)
          file_name[count] = working_tmp[count];

        for (j = 0; j <= strlen(subfname); j++)
          file_name[count+j] = subfname[j];

        printf("slibname is %s, and its length is %d\n", file_name, strlen(file_name));

        size = pass_load_ptr((char *)load_addr, part, file_name);

        if (size > 0) {
          slib_list_new = (slib_t *)malloc(SLIB_LEN);

          slib_list_new->name = subfname;
          slib_list_new->load_addr = load_addr;
          slib_list_new->load_size = size;

          INIT_LIST_HEAD(&slib_list_new->list);
          list_add_tail(&slib_list_new->list, &slib_db);

          printf("### %s load complete: %d bytes loaded to 0x%lx\n",
              file_name, size, load_addr);
          setenv_hex("filesize", size);
        } else {
          printf("### LOAD ERROR<%x> for %s!\n", size, file_name);
        }
      } else if (strcmp(command_name, "mlib_init") == 0){     /* dirname is mlib */
        for (count = 0; count < working_tmp_len; count++)
          file_name[count] = working_tmp[count];

        for (j=0; j<=strlen(subfname); j++)
          file_name[count+j] = subfname[j];

        size = pass_load_ptr((char *)load_addr, part, file_name);

        if (size > 0) {
          Elf32_Ehdr *ehdr;   /* Elf header structure pointer     */
          Elf32_Shdr *shdr;   /* Section header structure pointer */
          char *strtab = 0; /* String table pointer    */
          int sh_num;

          ehdr = (Elf32_Ehdr *) load_addr;

          /* Find the section header string table for output info */
          shdr = (Elf32_Shdr *) (load_addr + ehdr->e_shoff +
              (ehdr->e_shstrndx * sizeof(Elf32_Shdr)));

          if (shdr->sh_type == SHT_STRTAB)
            strtab = (char *) (load_addr + shdr->sh_offset);

          /* Load each appropriate section */
          for (sh_num = 0; sh_num < ehdr->e_shnum; ++sh_num) {
            shdr = (Elf32_Shdr *) (load_addr + ehdr->e_shoff + (sh_num * sizeof(Elf32_Shdr)));

            if (strtab && (strcmp(&strtab[shdr->sh_name], ".text") == 0)) {
              mlib_text_list_new = (mlib_t *)malloc(MLIB_LEN);

              mlib_text_list_new->name = subfname;
              mlib_text_list_new->file_load_addr = load_addr;
              mlib_text_list_new->text_start_addr = load_addr +
                (unsigned long)shdr->sh_offset;
              mlib_text_list_new->text_size = (long) shdr->sh_size;
              INIT_LIST_HEAD(&mlib_text_list_new->list);
              list_add_tail(&mlib_text_list_new->list, &mlib_db);

              debug("%sing %s @ 0x%08lx (%ld bytes)\n",
                  (shdr->sh_type == SHT_NOBITS) ?
                      "Clear" : "Load",
                      &strtab[shdr->sh_name],
                      (unsigned long) shdr->sh_addr,
                      (long) shdr->sh_size);
              break;
            }
          }
          printf("### %s load complete: %d bytes loaded to 0x%lx\n",
              file_name, size, load_addr);
          setenv_hex("filesize", size);
        } else {
          printf("### LOAD ERROR<%x> for %s!\n", size, file_name);
        }
      }
      load_addr = load_addr + (size & 0xFFFFF000) + 0x1000;
      file_count++;
      free(file_name);
      put_fl_mem(iNode, NULL);
    }
    put_fl_mem(jDir, pL->readbuf);
  }

  /* display all files  */
  if (strcmp(command_name, "slib_init") == 0) {
    slib_t *slib_display = NULL;
    printf("%d files are in the slib:\n", file_count);
    struct list_head *entry;
    list_for_each(entry, &slib_db) {
      slib_display = list_entry(entry, slib_t, list);
      if (slib_display != NULL){
        printf("%s's load address is 0x%08x, and size is 0x%08lx bytes\n",
               slib_display->name, slib_display->load_addr,
               slib_display->load_size);
      }
    }
  } else if (strcmp(command_name, "mlib_init") == 0) {
    mlib_t *mlib_display;
    printf("%d files are in the mlib:\n", file_count);
    struct list_head *entry;
    list_for_each(entry, &mlib_db) {
      mlib_display = list_entry(entry, mlib_t, list);
      if (mlib_display != NULL){
        printf("%s's load address is 0x%08x, "
               ".text segment's load address is 0x%08x, "
               "and size is 0x%08lx bytes\n",
               mlib_display->name, mlib_display->file_load_addr,
               mlib_display->text_start_addr, mlib_display->text_size);
      }
    }
  }
  return pino;
}

static u32
jffs2_1pass_search_list_inodes(char* const command_name, uint32_t dst,
                               struct part_info * part,
                               struct b_lists * pL,
                               const char *fname, u32 pino, u8 dma) {
  int i;
  char tmp[256];
  char working_tmp[256];
  char *c;

  /* discard any leading slash */
  i = 0;
  while (fname[i] == '/') i++;
  strcpy(tmp, &fname[i]);
  working_tmp[0] = '\0';
  while ((c = (char *) strchr(tmp, '/'))) /* we are still dired searching */
  {
    strncpy(working_tmp, tmp, c - tmp);
    working_tmp[c - tmp] = '\0';
    for (i = 0; i < strlen(c) - 1; i++)
      tmp[i] = c[i + 1];
    tmp[i] = '\0';
    /* only a failure if we arent looking at top level */
    if (!(pino = jffs2_1pass_find_inode(pL, working_tmp, pino, dma)) &&
        (working_tmp[0])) {
      putstr("find_inode failed for name=");
      putstr(working_tmp);
      putstr("\r\n");
      return 0;
    }
  }

  if (tmp[0] && !(pino = jffs2_1pass_find_inode(pL, tmp, pino, dma))) {
    putstr("find_inode failed for name=");
    putstr(tmp);
    putstr("\r\n");
    return 0;
  }

  /* this is for the bare filename, directories have already been mapped */
  if (!(pino = jffs2_1pass_list_inodes(command_name, dst, fname, part, pL,
                                       pino, dma))) {
    putstr("find_inode failed for name=");
    putstr(tmp);
    putstr("\r\n");
    return 0;
  }
  return pino;
}

#ifdef CONFIG_JFFS2_SUMMARY
static u32 sum_get_unaligned32(u32 *ptr)
{
	u32 val;
	u8 *p = (u8 *)ptr;

	val = *p | (*(p + 1) << 8) | (*(p + 2) << 16) | (*(p + 3) << 24);

	return __le32_to_cpu(val);
}

static u16 sum_get_unaligned16(u16 *ptr)
{
	u16 val;
	u8 *p = (u8 *)ptr;

	val = *p | (*(p + 1) << 8);

	return __le16_to_cpu(val);
}

#define dbg_summary(...) do {} while (0);
/*
 * Process the stored summary information - helper function for
 * jffs2_sum_scan_sumnode()
 */

static int jffs2_sum_process_sum_data(struct part_info *part, uint32_t offset,
				struct jffs2_raw_summary *summary,
				struct b_lists *pL)
{
	void *sp;
	int i, pass;
	void *ret;

	for (pass = 0; pass < 2; pass++) {
		sp = summary->sum;

		for (i = 0; i < summary->sum_num; i++) {
			struct jffs2_sum_unknown_flash *spu = sp;
			dbg_summary("processing summary index %d\n", i);

			switch (sum_get_unaligned16(&spu->nodetype)) {
				case JFFS2_NODETYPE_INODE: {
				struct jffs2_sum_inode_flash *spi;
					if (pass) {
						spi = sp;

						ret = insert_node(&pL->frag,
							(u32)part->offset +
							offset +
							sum_get_unaligned32(
								&spi->offset));
						if (ret == NULL)
							return -1;
					}

					sp += JFFS2_SUMMARY_INODE_SIZE;

					break;
				}
				case JFFS2_NODETYPE_DIRENT: {
					struct jffs2_sum_dirent_flash *spd;
					spd = sp;
					if (pass) {
						ret = insert_node(&pL->dir,
							(u32) part->offset +
							offset +
							sum_get_unaligned32(
								&spd->offset));
						if (ret == NULL)
							return -1;
					}

					sp += JFFS2_SUMMARY_DIRENT_SIZE(
							spd->nsize);

					break;
				}
				default : {
					uint16_t nodetype = sum_get_unaligned16(
								&spu->nodetype);
					printf("Unsupported node type %x found"
							" in summary!\n",
							nodetype);
					if ((nodetype & JFFS2_COMPAT_MASK) ==
							JFFS2_FEATURE_INCOMPAT)
						return -EIO;
					return -EBADMSG;
				}
			}
		}
	}
	return 0;
}

/* Process the summary node - called from jffs2_scan_eraseblock() */
int jffs2_sum_scan_sumnode(struct part_info *part, uint32_t offset,
			   struct jffs2_raw_summary *summary, uint32_t sumsize,
			   struct b_lists *pL)
{
	struct jffs2_unknown_node crcnode;
	int ret, ofs;
	uint32_t crc;

	ofs = part->sector_size - sumsize;

	dbg_summary("summary found for 0x%08x at 0x%08x (0x%x bytes)\n",
		    offset, offset + ofs, sumsize);

	/* OK, now check for node validity and CRC */
	crcnode.magic = JFFS2_MAGIC_BITMASK;
	crcnode.nodetype = JFFS2_NODETYPE_SUMMARY;
	crcnode.totlen = summary->totlen;
	crc = crc32_no_comp(0, (uchar *)&crcnode, sizeof(crcnode)-4);

	if (summary->hdr_crc != crc) {
		dbg_summary("Summary node header is corrupt (bad CRC or "
				"no summary at all)\n");
		goto crc_err;
	}

	if (summary->totlen != sumsize) {
		dbg_summary("Summary node is corrupt (wrong erasesize?)\n");
		goto crc_err;
	}

	crc = crc32_no_comp(0, (uchar *)summary,
			sizeof(struct jffs2_raw_summary)-8);

	if (summary->node_crc != crc) {
		dbg_summary("Summary node is corrupt (bad CRC)\n");
		goto crc_err;
	}

	crc = crc32_no_comp(0, (uchar *)summary->sum,
			sumsize - sizeof(struct jffs2_raw_summary));

	if (summary->sum_crc != crc) {
		dbg_summary("Summary node data is corrupt (bad CRC)\n");
		goto crc_err;
	}

	if (summary->cln_mkr)
		dbg_summary("Summary : CLEANMARKER node \n");

	ret = jffs2_sum_process_sum_data(part, offset, summary, pL);
	if (ret == -EBADMSG)
		return 0;
	if (ret)
		return ret;		/* real error */

	return 1;

crc_err:
	putstr("Summary node crc error, skipping summary information.\n");

	return 0;
}
#endif /* CONFIG_JFFS2_SUMMARY */

#ifdef DEBUG_FRAGMENTS
static void
dump_fragments(struct b_lists *pL)
{
	struct b_node *b;
	struct jffs2_raw_inode ojNode;
	struct jffs2_raw_inode *jNode;

	putstr("\r\n\r\n******The fragment Entries******\r\n");
	b = pL->frag.listHead;
	while (b) {
		jNode = (struct jffs2_raw_inode *) get_fl_mem(b->offset,
			sizeof(ojNode), &ojNode);
		putLabeledWord("\r\n\tbuild_list: FLASH_OFFSET = ", b->offset);
		putLabeledWord("\tbuild_list: totlen = ", jNode->totlen);
		putLabeledWord("\tbuild_list: inode = ", jNode->ino);
		putLabeledWord("\tbuild_list: version = ", jNode->version);
		putLabeledWord("\tbuild_list: isize = ", jNode->isize);
		putLabeledWord("\tbuild_list: atime = ", jNode->atime);
		putLabeledWord("\tbuild_list: offset = ", jNode->offset);
		putLabeledWord("\tbuild_list: csize = ", jNode->csize);
		putLabeledWord("\tbuild_list: dsize = ", jNode->dsize);
		putLabeledWord("\tbuild_list: compr = ", jNode->compr);
		putLabeledWord("\tbuild_list: usercompr = ", jNode->usercompr);
		putLabeledWord("\tbuild_list: flags = ", jNode->flags);
		putLabeledWord("\tbuild_list: offset = ", b->offset);	/* FIXME: ? [RS] */
		b = b->next;
	}
}
#endif

#ifdef DEBUG_DIRENTS
static void
dump_dirents(struct b_lists *pL)
{
	struct b_node *b;
	struct jffs2_raw_dirent *jDir;

	putstr("\r\n\r\n******The directory Entries******\r\n");
	b = pL->dir.listHead;
	while (b) {
		jDir = (struct jffs2_raw_dirent *) get_node_mem(b->offset,
								pL->readbuf);
		putstr("\r\n");
		putnstr(jDir->name, jDir->nsize);
		putLabeledWord("\r\n\tbuild_list: magic = ", jDir->magic);
		putLabeledWord("\tbuild_list: nodetype = ", jDir->nodetype);
		putLabeledWord("\tbuild_list: hdr_crc = ", jDir->hdr_crc);
		putLabeledWord("\tbuild_list: pino = ", jDir->pino);
		putLabeledWord("\tbuild_list: version = ", jDir->version);
		putLabeledWord("\tbuild_list: ino = ", jDir->ino);
		putLabeledWord("\tbuild_list: mctime = ", jDir->mctime);
		putLabeledWord("\tbuild_list: nsize = ", jDir->nsize);
		putLabeledWord("\tbuild_list: type = ", jDir->type);
		putLabeledWord("\tbuild_list: node_crc = ", jDir->node_crc);
		putLabeledWord("\tbuild_list: name_crc = ", jDir->name_crc);
		putLabeledWord("\tbuild_list: offset = ", b->offset);	/* FIXME: ? [RS] */
		b = b->next;
		put_fl_mem(jDir, pL->readbuf);
	}
}
#endif

inline u32 jffs2_lib_load_by_ndma(char* const command_name, uint32_t dst,
		                              struct part_info * part, const char *fname) {
	struct b_lists *pl;
	long ret = 1;
	u32 inode;

	if (! (pl = jffs2_get_list(part, "ndma_ls", 0)))
		return 0;

	if (! (inode = jffs2_1pass_search_list_inodes(command_name, dst, part,
	                                              pl, fname, 1, 0))) {
		putstr("dma_ls: Failed to scan jffs2 file structure\r\n");
		return 0;
	}


	#if 0
		putLabeledWord("found file at inode = ", inode);
		putLabeledWord("read_inode returns = ", ret);
	#endif

	return ret;
}

/****************** if define(MAPU_DMA_JFFS2) then transfer the data by dma ******************/

#if defined(MAPU_DMA_JFFS2)

inline u32 jffs2_lib_load_by_dma(char* const command_name, uint32_t dst,
                                 struct part_info * part, const char *fname) {
	struct b_lists *pl;
	long ret = 1;
	u32 inode;

	if (! (pl = jffs2_get_list(part, "dma_ls", 1)))
		return 0;

	if (! (inode = jffs2_1pass_search_list_inodes(command_name, dst, part, pl,
	                                              fname, 1, 1))) {
		putstr("dma_ls: Failed to scan jffs2 file structure\r\n");
		return 0;
	}

	#if 0
		putLabeledWord("found file at inode = ", inode);
		putLabeledWord("read_inode returns = ", ret);
	#endif

	return ret;
}

#endif
