/*
 * cmd_core.c
 *
 *  Created on: Jul 23, 2014
 *      Author: wangl
 */

#include <apc/ape.h>
#include <apc/apclib.h>
#include <apc/loader.h>
#include <malloc.h>
#include <common.h>
#include <command.h>
#include <jffs2/load_kernel.h>

extern struct mtd_device *current_mtd_dev;
extern u8 current_mtd_partnum;

extern struct list_head mlib_db;

extern uint32_t current_ape;
extern slib_t *current_load[MAX_NUM_CORES];
extern uint32_t num_cores;

extern u32 jffs2_1pass_load(char *dest, struct part_info * part,
                            const char *fname);
extern int vlogparse(struct vlog_param_strt *param);

static int do_apccore_load_spu(cmd_tbl_t *cmdtp, int flag, int argc,
                               char * const argv[]) {
  uint32_t cpuid = current_ape;
  const char *func;
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

  return load_spu(func, (void *)param, cpuid);
}

static int do_apccore_load_mpu(cmd_tbl_t *cmdtp, int flag, int argc,
                               char * const argv[]) {
  uint32_t cpuid = current_ape;
  const char *func;
  mlib_t *mp = NULL;

  if (argc < 3)
    return CMD_RET_USAGE;
  func = argv[2];

  if (argc > 3) {
    cpuid = simple_strtoul(argv[3], NULL, 10);
    current_ape = cpuid;
  }
  struct list_head *entry;
  list_for_each(entry, &mlib_db) {
    mp = list_entry(entry, mlib_t, list);
    if (mp && !strncmp(mp->name, func, strlen(func))) break;
    else mp = NULL;
  }

  if (!mp) return 1;

  return load_mpu_section(mp->text_start_addr, mp->text_size, cpuid);
}

static int do_apccore_datatfr(cmd_tbl_t *cmdtp, int flag, int argc,
                              char * const argv[]) {
  char ismat = 0;
  char direct;
  enum DATA_TYPE datatype = DT_BYTE;
  uint32_t dest, src, len, height = 1;
  uint32_t cpuid = current_ape;

  if (argc < 5) return CMD_RET_USAGE;
  if (argc > 5 && argc != 8 && argc != 9) return CMD_RET_USAGE;

  if (strcmp(argv[1], "send") == 0)
    direct = 1;
  else
    direct = 0;

  dest = simple_strtoul(argv[2], NULL, 16);
  src = simple_strtoul(argv[3], NULL, 16);
  len = simple_strtoul(argv[4], NULL, 16);

  if (len == 0) return 0;

  if (argc >= 8) {
    cpuid = simple_strtoul(argv[5], NULL, 10);
    switch (argv[6][0]) {
    case 'b': datatype = DT_BYTE; break;
    case 'h': datatype = DT_HALF; break;
    case 'w': datatype = DT_WORD; break;
    case 'd': datatype = DT_DWORD; break;
    case 'q': datatype = DT_QWORD; break;
    default: return CMD_RET_USAGE;
    }
    if (argv[7][0] == 'v') ismat = 0;
    else if (argv[7][0] == 'm') ismat = 1;
    else return CMD_RET_USAGE;
    if (argc == 9)
      height = simple_strtoul(argv[8], NULL, 16);
  }

  if (ismat && direct)
    return send_matrix(dest, src, len, height, datatype, cpuid);
  else if (ismat && !direct)
    return fetch_matrix(dest, src, len, height, datatype, cpuid);
  else if (!ismat && direct)
    return send_vector(dest, src, len * height, datatype, cpuid);
  else if (!ismat && !direct)
    return fetch_vector(dest, src, len * height, datatype, cpuid);

  return 0;
}

static int do_apccore_load_vlog(cmd_tbl_t *cmdtp, int flag, int argc,
                                char * const argv[]) {
  struct part_info *part = NULL;
  char *vlogbuf = NULL;
  const char *vlogfile = argv[2];
  struct vlog_param_strt param;
  int len = 0;


  struct list_head *entry;
  int num;

  if (mtdparts_init() != 0)
    return 2;

  if (!current_mtd_dev) return 4;

  if (current_mtd_partnum >= current_mtd_dev->num_parts) {
    printf("invalid partition number %d for device %s%d (%s)\n",
           current_mtd_partnum, MTD_DEV_TYPE(current_mtd_dev->id->type),
           current_mtd_dev->id->num, current_mtd_dev->id->mtd_id);
    return 4;
  }

  param.dst = simple_strtoul(argv[3], NULL, 16);
  param.src = simple_strtoul(argv[4], NULL, 16);
  param.cpuid = simple_strtoul(argv[5], NULL, 10);

  /* locate partition number */
  num = 0;
  list_for_each(entry, &current_mtd_dev->parts) {
    part = list_entry(entry, struct part_info, link);

    if (current_mtd_partnum == num++)
      break;
  }

  if (part) {
    if (!(len = jffs2_1pass_load(NULL, part, vlogfile))) return 1;
    vlogbuf = (char *)malloc(len);
    len = jffs2_1pass_load(vlogbuf, part, vlogfile);
    param.position.start = vlogbuf;
    param.position.end = vlogbuf + len;
    vlogparse(&param);
    free(vlogbuf);
  }

  return 0;
}

static int do_apccore_status(cmd_tbl_t *cmdtp, int flag, int argc,
                             char * const argv[]) {
  int32_t i = 0;
  int32_t mail = 0;
  printf("%8s%10s%25s%10s%15s", "Core ID", "Ext. DMA",
         "MailBox(Out/In/Out Intr.)", "Status", "Last job\n");
  printf("------------------------------------------------------------------\n");
  for (i = 0; i < num_cores; i++) {
    if (current_ape == i)
      printf("%7s%d", "(*)", i);
    else
      printf("%8d", i);
    printf(" %08x ", ape_dma_status(i));
    mail = ape_mail_status(i);
    printf("%20d/%d/%d ", (mail >> 16) & 0xFF, (mail >> 8) & 0xFF, mail & 0xFF);
    printf("%10s", ape_status(i) ? "Busy" : "Idle");
    printf(" %s\n", current_load[i] ? current_load[i]->name : "NULL");
  }
  return 0;
}

static int do_apccore_wait(cmd_tbl_t *cmdtp, int flag, int argc,
                             char * const argv[]) {
  uint32_t cpuid = current_ape;
  if (argc == 3)
    cpuid = simple_strtoul(argv[2], NULL, 10);
  return ape_wait(cpuid);
}

static int do_apccore_stop(cmd_tbl_t *cmdtp, int flag, int argc,
                             char * const argv[]) {
  uint32_t cpuid = current_ape;
  if (argc == 3)
    cpuid = simple_strtoul(argv[2], NULL, 10);
  return ape_stop(cpuid);
}

static int do_apccore_start(cmd_tbl_t *cmdtp, int flag, int argc,
                            char * const argv[]) {
  uint32_t cpuid = current_ape;
  if (argc == 3)
    cpuid = simple_strtoul(argv[2], NULL, 10);
  return ape_start(cpuid);
}

int do_apccore_ops(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]) {
  if (argc < 2)
    return CMD_RET_USAGE;

  if (strcmp(argv[1], "status") == 0) {
    if (argc != 2)
      return CMD_RET_USAGE;
    return do_apccore_status(cmdtp, flag, argc, argv);
  } else if (strcmp(argv[1], "load_spu") == 0) {
    if (argc > 5)
      return CMD_RET_USAGE;
    return do_apccore_load_spu(cmdtp, flag, argc, argv);
  } else if (strcmp(argv[1], "load_mpu") == 0) {
    if (argc > 4)
      return CMD_RET_USAGE;
    return do_apccore_load_mpu(cmdtp, flag, argc, argv);
  } else if (strcmp(argv[1], "fetch") == 0) {
    if (argc > 9)
      return CMD_RET_USAGE;
    return do_apccore_datatfr(cmdtp, flag, argc, argv);
  } else if (strcmp(argv[1], "send") == 0) {
    if (argc > 9)
      return CMD_RET_USAGE;
    return do_apccore_datatfr(cmdtp, flag, argc, argv);
  } else if (strcmp(argv[1], "loadvlog") == 0) {
    if (argc != 6)
      return CMD_RET_USAGE;
    return do_apccore_load_vlog(cmdtp, flag, argc, argv);
  } else if (strcmp(argv[1], "start") == 0) {
    if (argc > 3)
      return CMD_RET_USAGE;
    return do_apccore_start(cmdtp, flag, argc, argv);
  } else if (strcmp(argv[1], "wait") == 0) {
    if (argc > 3)
      return CMD_RET_USAGE;
    return do_apccore_wait(cmdtp, flag, argc, argv);
  } else if (strcmp(argv[1], "stop") == 0) {
    if (argc > 3)
      return CMD_RET_USAGE;
    return do_apccore_stop(cmdtp, flag, argc, argv);
  }
  return CMD_RET_USAGE;
}

U_BOOT_CMD(
  apccore, 10, 1, do_apccore_ops,
  "APC core command subset",
  "apccore load_spu <sfunc> [<cid> [<param>]]\n"
  "\t- load <sfunc> in SPU libs onto APE core <cid> with parameters at <param>\n"
  "apccore load_mpu <mfunc> [<cid>]\n"
  "\t- load <mfunc> in MPU libs onto APE core <cid>\n"
  "apccore fetch <dest> <src> <len> [<cid> b|h|w|d|q v|m [<height>]]\n"
  "\t- fetch the number of <len>x<height> (b)ytes/(h)alf words/(w)ords/(d)ouble words/\n"
  "\t  (q)uad words from <src> on APE core <cid> to <dest> in (v)ector or\n"
  "\t  (m)atrix format.\n"
  "apccore send <dest> <src> <len> [<cid> b|h|w|d|q v|m [<height>]]\n"
  "\t- send the number of <len>x<height> (b)ytes/(h)alf words/(w)ords/(d)ouble words\n"
  "\t  (q)uad words from <src> on APE core <cid> to <dest> in (v)ector or\n"
  "\t  (m)atrix format.\n"
  "apccore loadvlog <vlogfile> <dest> <src> <cid>\n"
  "\t- parse <vlogfile> to <src>, and load data from <src> to <dest> on APE <cid>.\n"
  "apccore start [<cid>]\n"
  "\t- start APE core <cid>\n"
  "apccore wait [<cid>]\n"
  "\t- wait until APE core <cid> finished its job\n"
  "apccore stop [<cid>]\n"
  "\t- terminate current job on APE core <cid>\n"
  "apccore status\n"
  "\t- check status of APE cores\n"
  );
