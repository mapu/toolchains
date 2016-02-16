/*
 * cmd_lib.c
 *
 *  Created on: Jul 21, 2014
 *      Author: wangl
 */
#include <apc/apclib.h>
#include <apc/loader.h>
#include <common.h>
#include <malloc.h>
#include <command.h>
#include <jffs2/load_kernel.h>

/* Use the ones declared in cmd_mtdparts.c */
extern struct mtd_device *current_mtd_dev;
extern u8 current_mtd_partnum;

extern uint32_t current_ape;

extern struct list_head slib_db;
extern struct list_head mlib_db;

extern u32 jffs2_1pass_load(char *dest, struct part_info * part,
                            const char *fname);
extern int dbparse(struct pos_strt *position);

extern u32 jffs2_lib_load_by_dma(char *const command_name, uint32_t dst,
                                 struct part_info *part, const char *fname);

extern u32 jffs2_lib_load_by_ndma(char *const command_name, uint32_t dst,
                                  struct part_info *part, const char *fname);

LIST_HEAD(apclib_db);

static int do_apclib_info(cmd_tbl_t *cmdtp, int flag,
                          int argc, char * const argv[]) {
  int cnt = 0;
  slib_t *sp = NULL;
  mlib_t *mp = NULL;
  struct list_head *entry;
  struct record_strt *dbp = NULL;

  printf("[spu library]:\n");
  printf("function name\t\tposition\n");
  printf("---------------------------------------------------\n");
  list_for_each(entry, &slib_db) {
    sp = list_entry(entry, slib_t, list);
    printf("%14s\t\t0x%08x\n", sp->name, sp->load_addr);
    cnt++;
  }
  printf("---------------------------------------------------\n");
  printf("Total: %d\n", cnt);

  printf("[mpu library]:\n");
  printf("function name\t\tposition\n");
  printf("---------------------------------------------------\n");
  cnt = 0;
  list_for_each(entry, &mlib_db) {
    mp = list_entry(entry, mlib_t, list);
    printf("%14s\t\t0x%08x\n", mp->name, mp->file_load_addr);
    cnt++;
  }
  printf("---------------------------------------------------\n");
  printf("Total: %d\n", cnt);

  printf("[database]:\n");
  printf("function name\t\tdependency\n");
  printf("---------------------------------------------------\n");

  list_for_each(entry, &apclib_db) {
    dbp = list_entry(entry, struct record_strt, list);
    printf("%14s\t\t", dbp->slib->name);
    cnt = dbp->mcnt;
    while (cnt != 0) {
      cnt--;
      printf("%s, ", dbp->mlibs[cnt]->name);
    }
    printf("\n");
  }
  printf("---------------------------------------------------\n");

  return 0;
}

// Load SPU library from jffs2 filesystem
static int do_apclib_slib_init(cmd_tbl_t *cmdtp, int flag,
                               int argc, char * const argv[]) {
  struct part_info *part = NULL;
  uint32_t load_base;
  char use_dma = 1;
  const char *env;
  char *filename = "slib";

  struct list_head *entry;
  int num;

  if (mtdparts_init() != 0)
    return 2;

  if ((env = getenv("slib_base")) != NULL)
    load_base = simple_strtoul(env, NULL, 16);
  else
    load_base = SLIB_BASE;

  if (argc > 2) {
    load_base = simple_strtoul(argv[2], NULL, 16);

    if (argc > 3) {
      filename = argv[3];
      if (argc > 4) {
        if (!strcmp(argv[3], "dma"))
          return CMD_RET_USAGE;
      } else use_dma = 0;
    }
  }

  if (!current_mtd_dev) return 4;

  if (current_mtd_partnum >= current_mtd_dev->num_parts) {
    printf("invalid partition number %d for device %s%d (%s)\n",
           current_mtd_partnum, MTD_DEV_TYPE(current_mtd_dev->id->type),
           current_mtd_dev->id->num, current_mtd_dev->id->mtd_id);
    return 4;
  }

  /* locate partition number */
  num = 0;
  list_for_each(entry, &current_mtd_dev->parts) {
    part = list_entry(entry, struct part_info, link);

    if (current_mtd_partnum == num++)
      break;
  }

  if (part) {
    return jffs2_lib_load_by_ndma("slib_init", load_base, part, filename) == 0;
  }
  return 4;
}

// Load MPU library from jffs2 filesystem
static int do_apclib_mlib_init(cmd_tbl_t *cmdtp, int flag,
                               int argc, char * const argv[]) {
  struct part_info *part = NULL;
  uint32_t load_base;
  char use_dma = 1;
  const char *env;
  char *filename = "mlib";

  struct list_head *entry;
  int num;

  if (mtdparts_init() != 0)
    return 2;

  if ((env = getenv("mlib_base")) != NULL)
    load_base = simple_strtoul(env, NULL, 16);
  else
    load_base = MLIB_BASE;

  if (argc > 2) {
    load_base = simple_strtoul(argv[2], NULL, 16);

    if (argc > 3) {
      filename = argv[3];
      if (argc > 4) {
        if (!strcmp(argv[3], "dma"))
          return CMD_RET_USAGE;
      } else use_dma = 0;
    }
  }

  if (!current_mtd_dev) return 4;

  if (current_mtd_partnum >= current_mtd_dev->num_parts) {
    printf("invalid partition number %d for device %s%d (%s)\n",
           current_mtd_partnum, MTD_DEV_TYPE(current_mtd_dev->id->type),
           current_mtd_dev->id->num, current_mtd_dev->id->mtd_id);
    return 4;
  }

  /* locate partition number */
  num = 0;
  list_for_each(entry, &current_mtd_dev->parts) {
    part = list_entry(entry, struct part_info, link);

    if (current_mtd_partnum == num++)
      break;
  }

  if (part) {
    return jffs2_lib_load_by_ndma("mlib_init", load_base, part, filename) == 0;
  }
  return 4;
}

// Init APC library database
static int do_apclib_db_init(cmd_tbl_t *cmdtp, int flag,
                             int argc, char * const argv[]) {
  struct part_info *part = NULL;
  char *offset = NULL;
  struct pos_strt position;
  uint32_t size = 0;

  struct list_head *entry;
  int num;

  if (argc < 3)
    return CMD_RET_USAGE;

  /* make sure we are in sync with env variables */
  if (mtdparts_init() != 0)
    return 1;

  if (current_mtd_partnum >= current_mtd_dev->num_parts) {
    printf("invalid partition number %d for device %s%d (%s)\n",
           current_mtd_partnum, MTD_DEV_TYPE(current_mtd_dev->id->type),
           current_mtd_dev->id->num, current_mtd_dev->id->mtd_id);
    return 4;
  }

  /* locate partition number */
  num = 0;
  list_for_each(entry, &current_mtd_dev->parts) {
    part = list_entry(entry, struct part_info, link);

    if (current_mtd_partnum == num++)
      break;
  }

  if (part){
    size = jffs2_1pass_load(NULL, part, argv[2]);

    if (size > 0) offset = (char *)malloc(size);

    size = jffs2_1pass_load(offset, part, argv[2]);

    if (size > 0) {
      position.start = offset;
      position.end = offset + size;
      dbparse(&position);
      free(offset);
      return 0;
    } else {
      printf("### LOAD ERROR<%x> for %s!\n", size, argv[2]);
      return 2;
    }
  }

  return 1;
}

// Invoke SPU function
static int do_apclib_call(cmd_tbl_t *cmdtp, int flag,
                          int argc, char * const argv[]) {
  const char *func;
  int32_t cpuid = current_ape;
  uint32_t param = 0;

  if (argc < 3)
    return CMD_RET_USAGE;
  func = argv[2];

  if (argc > 3) {
    cpuid = simple_strtoul(argv[3], NULL, 10);
    current_ape = cpuid;
    if (argc > 4)
      param = simple_strtoul(argv[4], NULL, 16);
  }

  return apcapi_core(func, (void *)param, cpuid);
}

int do_apclib_ops(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]) {
  if (argc < 2)
    return CMD_RET_USAGE;

  if (strcmp(argv[1], "info") == 0) {
    if (argc != 2)
      return CMD_RET_USAGE;
    return do_apclib_info(cmdtp, flag, argc, argv);
  } else if (strcmp(argv[1], "sinit") == 0) {
    if (argc > 5)
      return CMD_RET_USAGE;
    return do_apclib_slib_init(cmdtp, flag, argc, argv);
  } else if (strcmp(argv[1], "minit") == 0) {
    if (argc > 5)
      return CMD_RET_USAGE;
    return do_apclib_mlib_init(cmdtp, flag, argc, argv);
  } else if (strcmp(argv[1], "dbinit") == 0) {
    if (argc > 3)
      return CMD_RET_USAGE;
    return do_apclib_db_init(cmdtp, flag, argc, argv);
  } else if (strcmp(argv[1], "call") == 0) {
    if (argc > 5)
      return CMD_RET_USAGE;
    return do_apclib_call(cmdtp, flag, argc, argv);
  }
  return CMD_RET_USAGE;
}

U_BOOT_CMD(
  apclib, 6, 1, do_apclib_ops,
  "APC Library command subset",
  "apclib sinit [<addr> [<dir> [dma]]]\n"
  "\t- load all spu libs under <dir> to <addr> using or without using DMA\n"
  "apclib minit [<addr> [<dir> [dma]]]\n"
  "\t- load all mpu libs under <dir> to <addr> using or without using DMA\n"
  "apclib dbinit <db>\n"
  "\t- parse <db> to initialize APC library database\n"
  "apclib call <sfunc> [<cid> [<param>]]\n"
  "\t- invoke <sfunc> in SPU libs on APE core <cid> with parameters"
  "\t  at <param>\n"
  "apclib info"
  "\t- lists all available spu libs and mpu libs, as well as the relationship\n"
  "\t  database\n"
  );
