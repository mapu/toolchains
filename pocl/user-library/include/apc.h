/*
 * Apc.h
 *
 *  Created on: 2016年3月17日
 *      Author: shisq
 */

#ifndef __APC_H__
#define __APC_H__

#include <string>
#include <map>
#include <vector>
#include <unistd.h>
#include "kernel/drivers/apc/apc.h"
#include "types.h"
#include "mem_pool/MemPool.h"

class Csu;
class Apc {
public:
  int fd;
  int num_of_apes;
  unsigned long ape_acquired_mask;
  Csu* csu;
  std::map<std::string, std::vector<std::string> > library_db;
  MemPool mem_pool;

  ~Apc();

  static Apc *tryFile(const std::string &fname, int fd);

  int getFd() const {return fd;}

  void raw_free(void *p, unsigned long size);
  void *raw_cmalloc(unsigned int size);  // 用驱动的mmap

  void *cmalloc(unsigned int size);   // 用内存池
  void *pmalloc(unsigned int size, int region);

  void cfree(void *p);
  void pfree(void *p, int region);

  Csu *getCsu(int id);

  bool acquireApe(int index);
  bool acquireApes(unsigned long mask);

  bool possessApe(int index);
  bool possessApes(unsigned long mask);

  void releaseApe(int index);
  void releaseApes(unsigned long mask);

  void parseGlobalDB(const std::string &fname);
  const std::map<std::string, std::vector<std::string> >& getGlobalDB ()const {return library_db;}
  int parseLocalDB(const char* fname, std::map<std::string, std::vector<std::string> >& s2mmap);

protected:
  Apc(int _fd, int num_of_apes);
};

#endif /* __APC_H__ */
