/*
 * loader.c
 *
 *  Created on: Jun 4, 2014
 *      Author: wangl
 */

#include <common.h>
#include <apc/apclib.h>
#include <apc/ape.h>
#include <apc/loader.h>
#include <elf.h>
#include <linux/list.h>
#include <asm/io.h>

extern struct list_head slib_db;
extern struct list_head apclib_db;

const uint32_t APE0_MLIB_TABLE = CONFIG_SHM_BASE;

slib_t *current_load[MAX_NUM_CORES] = {NULL};

inline int32_t apcapi_core(const char *api_name, void *param, uint32_t cpuid) {
  int32_t ret;
  if ((ret = load_spu(api_name, param, cpuid)) != 0)
    return ret;
  ape_dma_wait(SPU_LD_SECT_GP, cpuid);
  ape_start(cpuid);
  return 0;
}

static int32_t
load_elf_image_shdr(uint32_t addr, uint32_t param, uint32_t cpuid) {
  int32_t ret = 0;
  Elf32_Ehdr *ehdr;   /* Elf header structure pointer     */
  Elf32_Shdr *shdr;   /* Section header structure pointer */
  char *strtab = 0;  /* String table pointer             */
  int32_t i;        /* Loop counter                     */

  /* -------------------------------------------------- */

  ehdr = (Elf32_Ehdr *) addr;

  /* Find the section header string table for output info */
  shdr = (Elf32_Shdr *) (addr + ehdr->e_shoff +
             (ehdr->e_shstrndx * sizeof(Elf32_Shdr)));

  if (shdr->sh_type == SHT_STRTAB)
    strtab = (char *) (addr + shdr->sh_offset);

  /* Load each appropriate section */
  for (i = 0; i < ehdr->e_shnum; ++i) {
    shdr = (Elf32_Shdr *) (addr + ehdr->e_shoff +
               (i * sizeof(Elf32_Shdr)));

    if (!(shdr->sh_flags & SHF_ALLOC) || shdr->sh_size == 0) {
      continue;
    }

    if (strtab) {
      debug("%sing %s @ 0x%08x (%ld bytes)\n",
        (shdr->sh_type == SHT_NOBITS) ?
          "Clear" : "Load",
        &strtab[shdr->sh_name],
        (uint32_t) shdr->sh_addr,
        (long) shdr->sh_size);
    }

    if (shdr->sh_type == SHT_NOBITS) {
      ret += set_spu_section(shdr->sh_addr, 0, shdr->sh_size, cpuid);
    } else {
      if ((strcmp(&strtab[shdr->sh_name], ".param") == 0) && (param != 0))
        ret += load_spu_section(shdr->sh_addr, param, shdr->sh_size, cpuid);

      else
        ret += load_spu_section(shdr->sh_addr, addr + shdr->sh_offset,
                                shdr->sh_size, cpuid);
    }
  }

  return ret;
}

int32_t load_spu(const char *funcname, void *param, uint32_t cpuid) {
  struct list_head *entry;
  slib_t *sp = NULL;
  struct record_strt *dbp = NULL;
  uint32_t mlib_map = APE0_MLIB_TABLE + cpuid * MAX_MC_NUM * 4 * 2;
  int32_t i;

  list_for_each(entry, &apclib_db) {
    dbp = list_entry(entry, struct record_strt, list);
    if (!strncmp(dbp->slib->name, funcname, strlen(funcname)))
      break;
  }

  list_for_each(entry, &slib_db) {
    sp = list_entry(entry, slib_t, list);
    if (sp && !strncmp(sp->name, funcname, strlen(funcname))) break;
    else sp = NULL;
  }

  if (!dbp)
    printf("Warning: cannot find function %s in APC library.\n"
           "Trying to find it in SPU Library ...\n", funcname);

  if (!sp) {
    printf("Error: cannot find function %s.\n", funcname);
    return 1;
  } else if (!dbp)
    printf("Warning: SPU function %s is loaded without MPU Library support.\n",
           funcname);

  if (dbp && sp != dbp->slib) {
    printf("Error: library database is inconsistent.\n");
    return 2;
  }

  if (load_elf_image_shdr(sp->load_addr, (uint32_t)param, cpuid))
    return 3;

  current_load[cpuid] = sp;

  if (dbp && dbp->mcnt) {
    for (i = 0; i < dbp->mcnt; i++) {
      writel(dbp->mlibs[i]->text_start_addr, mlib_map + i * 8);
      writel(dbp->mlibs[i]->text_size / MC_LENGTH, mlib_map + i * 8 + 4);
    }
  }

  return 0;
}

int32_t set_spu_section(uint32_t dst, uint32_t value, uint32_t size,
                        uint32_t cpuid) {
  struct dma_if conf;
  conf.DMAGlobalAddr = (uint32_t)&value;
  conf.DMAGlobalXNum = 1;
  conf.DMAGlobalYAllNum = size;
  conf.DMAGlobalYNum = size;
  conf.DMAGlobalYStep = 0;
  conf.DMAGlobalZStep = 1;
  conf.DMALocalAddr = dst;
  conf.DMALocalXNum = 1;
  conf.DMALocalYAllNum = 1;
  conf.DMALocalYNum = size;
  conf.DMALocalYStep = 1;
  conf.DMALocalZStep = 1;
  conf.DMACmd = CMD_EXT2INT;
  conf.DMAGroupNum = SPU_LD_SECT_GP;
  return ape_dma_request(&conf, cpuid);
}

int32_t load_spu_section(uint32_t dst, uint32_t src, uint32_t size,
                         uint32_t cpuid) {
  struct dma_if conf;
  conf.DMAGlobalAddr = src;
  conf.DMAGlobalXNum = 1;
  conf.DMAGlobalYAllNum = size;
  conf.DMAGlobalYNum = size;
  conf.DMAGlobalYStep = 1;
  conf.DMAGlobalZStep = 1;
  conf.DMALocalAddr = dst;
  conf.DMALocalXNum = 1;
  conf.DMALocalYAllNum = size;
  conf.DMALocalYNum = size;
  conf.DMALocalYStep = 1;
  conf.DMALocalZStep = 1;
  conf.DMACmd = CMD_EXT2INT;
  conf.DMAGroupNum = SPU_LD_SECT_GP;
  return ape_dma_request(&conf, cpuid);
}

int32_t load_mpu_section(uint32_t src, uint32_t size, uint32_t cpuid) {
  struct dma_if conf;
  conf.DMAGlobalAddr = src;
  conf.DMAGlobalXNum = MC_LENGTH;
  conf.DMAGlobalYAllNum = size / MC_LENGTH;
  conf.DMAGlobalYNum = size / MC_LENGTH;
  conf.DMAGlobalYStep = MC_LENGTH;
  conf.DMAGlobalZStep = MC_LENGTH;
  conf.DMALocalAddr = 0x200000;
  conf.DMALocalXNum = MC_LENGTH;
  conf.DMALocalYAllNum = size / MC_LENGTH;
  conf.DMALocalYNum = size / MC_LENGTH;
  conf.DMALocalYStep = 64;
  conf.DMALocalZStep = 64;
  conf.DMACmd = CMD_EXT2INT;
  conf.DMAGroupNum = MPU_LD_SECT_GP;
  return ape_dma_request(&conf, cpuid);
}

int32_t send_vector(uint32_t dst, uint32_t src, uint32_t len,
                    enum DATA_TYPE size, uint32_t cpuid) {
  struct dma_if conf;
  if (size > 6) return -1;
  size = 1 << size;
  conf.DMAGlobalAddr = src;
  conf.DMAGlobalXNum = size;
  conf.DMAGlobalYAllNum = len;
  conf.DMAGlobalYNum = len;
  conf.DMAGlobalYStep = size;
  conf.DMAGlobalZStep = size;
  conf.DMALocalAddr = dst;
  conf.DMALocalXNum = size;
  conf.DMALocalYAllNum = len;
  conf.DMALocalYNum = len;
  conf.DMALocalYStep = size;
  conf.DMALocalZStep = size;
  conf.DMACmd = CMD_EXT2INT;
  conf.DMAGroupNum = DATA_E2I_GP;
  return ape_dma_request(&conf, cpuid);
}

int32_t send_matrix(uint32_t dst, uint32_t src, uint32_t width, uint32_t height,
                    enum DATA_TYPE size, uint32_t cpuid) {
  struct dma_if conf;
  uint32_t bank_size = 0x1000;
  uint32_t bank_num = 64;
  if (size > 6) return -1;
  bank_num >>= size;
  size = 1 << size;
  bank_size *= size;
  conf.DMAGlobalAddr = src;
  conf.DMAGlobalXNum = size * width;
  conf.DMAGlobalYAllNum = height;
  conf.DMAGlobalYNum = bank_num;
  conf.DMAGlobalYStep = size * width;
  conf.DMAGlobalZStep = size * width * bank_num;
  conf.DMALocalAddr = dst;
  conf.DMALocalXNum = size * width;
  conf.DMALocalYAllNum = height;
  conf.DMALocalYNum = bank_num;
  conf.DMALocalYStep = bank_size;
  conf.DMALocalZStep = size * width;
  conf.DMACmd = CMD_EXT2INT;
  conf.DMAGroupNum = DATA_E2I_GP;
  return ape_dma_request(&conf, cpuid);
}

int32_t fetch_vector(uint32_t dst, uint32_t src, uint32_t len,
                     enum DATA_TYPE size, uint32_t cpuid) {
  struct dma_if conf;
  if (size > 6) return -1;
  size = 1 << size;
  conf.DMAGlobalAddr = dst;
  conf.DMAGlobalXNum = size;
  conf.DMAGlobalYAllNum = len;
  conf.DMAGlobalYNum = len;
  conf.DMAGlobalYStep = size;
  conf.DMAGlobalZStep = size;
  conf.DMALocalAddr = src;
  conf.DMALocalXNum = size;
  conf.DMALocalYAllNum = len;
  conf.DMALocalYNum = len;
  conf.DMALocalYStep = size;
  conf.DMALocalZStep = size;
  conf.DMACmd = CMD_INT2EXT;
  conf.DMAGroupNum = DATA_I2E_GP;
  return ape_dma_request(&conf, cpuid);
}

int32_t fetch_matrix(uint32_t dst, uint32_t src, uint32_t width, uint32_t height,
                     enum DATA_TYPE size, uint32_t cpuid) {
  struct dma_if conf;
  uint32_t bank_size = 0x1000;
  uint32_t bank_num = 64;
  if (size > 6) return -1;
  bank_num >>= size;
  size = 1 << size;
  bank_size *= size;
  conf.DMAGlobalAddr = dst;
  conf.DMAGlobalXNum = size * width;
  conf.DMAGlobalYAllNum = height;
  conf.DMAGlobalYNum = bank_num;
  conf.DMAGlobalYStep = size * width;
  conf.DMAGlobalZStep = size * width * bank_num;
  conf.DMALocalAddr = src;
  conf.DMALocalXNum = size * width;
  conf.DMALocalYAllNum = height;
  conf.DMALocalYNum = bank_num;
  conf.DMALocalYStep = bank_size;
  conf.DMALocalZStep = size * width;
  conf.DMACmd = CMD_INT2EXT;
  conf.DMAGroupNum = DATA_I2E_GP;
  return ape_dma_request(&conf, cpuid);
}
