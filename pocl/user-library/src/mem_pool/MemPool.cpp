/*
 * MemPool.cpp
 *
 *  Created on: 2016年3月23日
 *      Author: ssqstone
 */
#include "mem_pool/ncx_slab.h"
#include "mem_pool/MemPool.h"
#include "apc.h"
#include <unistd.h>
#include <iostream>
using namespace std;

#define max(a, b)  ((a) > (b) ? (a) : (b))

static const unsigned page_size = getpagesize();

#ifndef get_order
unsigned long static get_order(unsigned long size) {
  unsigned long order;
  size = (size - 1) / page_size; // 变成页数-1

  for (order = 0; size; size >>= 1, order++)
    ;

  return order;
}
#endif

MemPool::MemPool(Apc* _apc, unsigned _incremental)
    : apc(_apc), incremental(_incremental) {
}

void *MemPool::alloc(int size) {
  void *result = 0;
  ncx_slab_pool_t *pool = 0;

  for (vector<ncx_slab_pool_t*>::iterator it = pools.begin(); it != pools.end(); ++it) {
    result = ncx_slab_alloc(*it, size);
    if (result) {
      pool = *it;
      break;
    }
  }

  if (!result) {
    unsigned alloc_size = page_size << get_order(max(size, incremental) + 2 * page_size);  // 再增加两页供内存池管理使用

    void *mem = apc->raw_cmalloc(alloc_size);
    if (mem) {
      pool = (ncx_slab_pool_t *) mem;
      pool->addr = mem;
      pool->end = (u_char*) mem + alloc_size;
      pool->min_shift = 3;
      ncx_slab_init (pool);

      pools.push_back(pool);

      result = ncx_slab_alloc(pool, size);
    }
  }

  if (result) {
    allocated.insert(pair<void*, ncx_slab_pool_t*>(result, pool));
  } else {
    cerr << "Attempted to alloc, but failed. " << endl;
  }
  return result;
}

void MemPool::free(void* p) {
  map<void*, ncx_slab_pool_t*>::iterator result = allocated.find(p);

  if (result != allocated.end()) {
    ncx_slab_pool_t *pool = result->second;
    bool ret = ncx_slab_free(pool, p);

    if (ret) {
      // 完全空闲的就释放掉, 以减少潜在的碎片和DMA内存的占用
      if (ncx_slab_is_empty(pool)) {
        for (vector<ncx_slab_pool_t *>::iterator it = pools.begin(); it != pools.end(); ++it) {
          if (*it == pool) {
            pools.erase(it);
            apc->raw_free(pool);
            return;
          }
        }
      }
    } else {
      cerr << "Attempted to free [0x" << hex << p << "], but failed. " << endl;
    }
  } else {
    cerr << "Attempted to free [0x" << hex << p << "], which is not recorded. " << endl;
  }
}

MemPool::~MemPool() {
  for (vector<ncx_slab_pool_t *>::iterator it = pools.begin(); it != pools.end(); ++it) {
    apc->raw_free(*it);
  }
}

