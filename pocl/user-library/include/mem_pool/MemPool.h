/*
 * MemPool.h
 *
 *  Created on: 2016年3月23日
 *      Author: ssqstone
 */

#ifndef USER_LIBRARY_MEM_POOL_MEMPOOL_H_
#define USER_LIBRARY_MEM_POOL_MEMPOOL_H_
#include "ncx_slab.h"

#include <vector>
#include <map>

class Apc;
class MemPool {
public:
  MemPool(Apc* _apc, unsigned _incremental = 4096*16);
  ~MemPool();

  void *alloc(int size);
  void free(void* p);

private:
  std::vector<ncx_slab_pool_t*> pools;
  std::map<void*, ncx_slab_pool_t*> allocated;
  Apc *apc;
  unsigned incremental;
};

#endif /* USER_LIBRARY_MEM_POOL_MEMPOOL_H_ */
