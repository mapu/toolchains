/*
 * Apc.cpp
 *
 *  Created on: 2016年3月17日
 *      Author: shisq
 */

#include "apc.h"

#include <string>
#include <iostream>
#include <fstream>

extern "C" {
#include <stdlib.h>
#include <stddef.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
}

#include "csu.h"

using namespace std;

Apc::Apc(int _fd, int num)
    : fd(_fd), num_of_apes(num) {
  csu = (Csu*) malloc(sizeof(Csu) * num_of_apes);

  for (int i = 0; i < num_of_apes; ++i)
    new (csu + i) Csu(_fd, i, this);
}

Apc::~Apc() {
  if (fd >= 0) close(fd);

  if (csu) for (int i = 0; i < num_of_apes; ++i)
    (csu + i)->~Csu();

  free(csu);
}

Apc *Apc::tryFile(const string &fname, int fd) {
  if (fd < 0) fd = open(fname.c_str(), O_RDWR);
  if (fd < 0) {
    std::cerr << "APC device is inaccessible! \n";
    return NULL;
  }

  int num = ioctl(fd, APC_IOC_QUERY_APE_NUM);
  if (num < 0) {
    std::cerr << "No APE device available! \n";
    return NULL;
  }

  return new Apc(fd, num);
}

// 这样不行, 应该管理已经分配的内存
void *Apc::cmalloc(unsigned int size) {
  void* result = mmap(NULL, size + sizeof(unsigned int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  *((unsigned int*) (result)) = size;
  return (void*) ((unsigned long) result + sizeof(unsigned int));
}

void *Apc::pmalloc(unsigned int size, int region) {
  return NULL;   // TO-DO not implemented yet
}

void Apc::free(void *p) {
  munmap((void*) ((unsigned long) p - sizeof(unsigned int)), *(unsigned int*) ((unsigned long) p - sizeof(unsigned int)));
  p = NULL;
}

Csu* Apc::getCsu(int id) {
  if ((unsigned) id < num_of_apes) return csu + id;
  else return 0;
}

bool Apc::acquireApe(int index) {
  if (index > num_of_apes) {
    return false;
  }

  return acquireApes(1 << index);
}

bool Apc::acquireApes(unsigned long mask) {
  if (ioctl(fd, APC_IOC_ACQUIRE_APE, mask)) return false;
  else {
    ape_acquired_mask |= mask;
    return true;
  }
}

bool Apc::possessApe(int index) {
  return possessApes(1 << index);
}
bool Apc::possessApes(unsigned long mask) {
  return !(mask & ~ape_acquired_mask);
}

void Apc::releaseApe(int index) {
  if (index > num_of_apes) return;
  return releaseApes(1 << index);
}

void Apc::releaseApes(unsigned long mask) {
  ioctl(fd, APC_IOC_RELEASE_APE, mask);
  ape_acquired_mask &= ~mask;
}

int parseDB(const string &fname, map<string, vector<string> >& s2mmap) {
  ifstream fin(fname.c_str());
  string thisLine;

  if (!fin) {
    fin.close();
    cerr << fname << " not found! " << endl;
    getchar();
    return -1;
  }

  vector<string> mnames;

  while (!fin.eof()) {
    getline(fin, thisLine);
    string sname;
    mnames.clear();

    int comment;
    if ((comment = thisLine.find("//", 0)) >= 0) thisLine = thisLine.substr(0, comment);

    int off2 = -1, off = thisLine.find('\"', off2 + 1);
    off2 = thisLine.find('\"', off + 1);

    for (; off >= 0 && off2 >= 0; off = thisLine.find('\"', off2 + 1), off2 = thisLine.find('\"', off + 1)) {
      string str = thisLine.substr(off + 1, off2 - off - 1);
      if (sname.length()) mnames.push_back(str);
      else sname = str;
    }

    if (mnames.size() > 0) {
      pair<map<string, vector<string> >::iterator, bool> result = s2mmap.insert(pair<string, vector<string> >(sname, mnames));
      if (!result.second) cerr << "Key [" << result.first->first << "] already inserted! " << endl;
    }
  }

  fin.close();

  return 0;
}

void Apc::parseGlobalDB(const string &fname) {
  parseDB(fname, s2m);
}

int Apc::parseLocalDB(const char* fname, map<string, vector<string> >& s2mmap) {
  return parseDB(fname, s2mmap);
}
