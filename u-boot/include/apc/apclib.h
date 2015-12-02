/*
 * apclib.h
 *
 *  Created on: Jul 21, 2014
 *      Author: wangl
 */

#ifndef APCLIB_H_
#define APCLIB_H_

#include <linux/list.h>

struct pos_strt {
  const char *start;
  const char *end;
};

struct vlog_param_strt {
  struct pos_strt position;
  uint32_t dst;
  uint32_t src;
  uint32_t cpuid;
};

/* after loading mlib to memory, save the .text messge*/
typedef struct mlib_text_load {
  char *name;
  uint32_t file_load_addr;
  uint32_t text_start_addr;
  long text_size;
  struct list_head list;
} mlib_t;

/* save the slib message after loading slib to memory */
typedef struct slib_load {
  char *name;
  uint32_t load_addr;
  long load_size;
  struct list_head list;
} slib_t;

struct record_strt {
  slib_t *slib;
  mlib_t **mlibs;
  int mcnt;
  struct list_head list;
};

#endif /* APCLIB_H_ */
