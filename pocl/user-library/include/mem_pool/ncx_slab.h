#ifndef _NCX_SLAB_H_INCLUDED_
#define _NCX_SLAB_H_INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#include "ncx_core.h"
#include "ncx_lock.h"
#include "ncx_log.h"

typedef struct ncx_slab_page_s ncx_slab_page_t;

struct ncx_slab_page_s {
  uintptr_t slab;
  ncx_slab_page_t *next;
  uintptr_t prev;
};

typedef struct {
  size_t min_size;
  size_t min_shift;

  ncx_slab_page_t *pages;
  ncx_slab_page_t free;   // next 指向下一个free的page

  u_char *start;
  u_char *end;

  ncx_shmtx_t mutex;

  void *addr;
} ncx_slab_pool_t;

typedef struct {
  unsigned long pool_size, used_size;
  double used_pct;
  unsigned long pages, free_page;
  unsigned long p_small, p_exact, p_big, p_page; /* 四种slab占用的page数 */
  unsigned long b_small, b_exact, b_big, b_page; /* 四种slab占用的byte数 */
  unsigned long max_free_pages; /* 最大的连续可用page数 */
} ncx_slab_stat_t;

void ncx_slab_init(ncx_slab_pool_t *pool);
void *ncx_slab_alloc(ncx_slab_pool_t *pool, size_t size);
bool ncx_slab_free(ncx_slab_pool_t *pool, void *p);
bool ncx_slab_is_empty(ncx_slab_pool_t *pool);
void ncx_slab_dummy_init(ncx_slab_pool_t *pool);
void ncx_slab_stat(ncx_slab_pool_t *pool, ncx_slab_stat_t *stat);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _NCX_SLAB_H_INCLUDED_ */
