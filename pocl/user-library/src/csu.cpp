/*
 * Copyright (c) 2003-2005 The Regents of The University of Michigan
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer;
 * redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution;
 * neither the name of the copyright holders nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Steve Reinhardt
 *          Ali Saidi
 */

#include <string>
#include <iostream>
#include <cstdio>
#include <cstdlib>

extern "C" {
#include <stddef.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
}

#include "csu.h"
#include "elf_object.h"

using namespace std;

#define    page_map_file     "/proc/self/pagemap"
#define    PFN_MASK          ((((uint64_t)1)<<55)-1)
#define    PFN_PRESENT_FLAG  (((uint64_t)1)<<63)

void* vir2phy(void* vir) {
  int fd;
  int page_size = getpagesize();
  unsigned long vir_page_idx = (unsigned long) vir / page_size;
  unsigned long pfn_item_offset = vir_page_idx * sizeof(uint64_t);
  uint64_t pfn_item;

  fd = open(page_map_file, O_RDONLY);
  if (fd < 0) {
    printf("open %s failed", page_map_file);
    return NULL;
  }

  if ((off_t) -1 == lseek(fd, pfn_item_offset, SEEK_SET)) {
    printf("lseek %s failed", page_map_file);
    return NULL;
  }

  if (sizeof(uint64_t) != read(fd, &pfn_item, sizeof(uint64_t))) {
    printf("read %s failed", page_map_file);
    return NULL;
  }

  if (0 == (pfn_item & PFN_PRESENT_FLAG)) {
    printf("page is not present");
    return NULL;
  }

  return (void*) ((pfn_item & PFN_MASK) * page_size + (unsigned long) vir % page_size);
}

const int DMACmdOffset = offsetof(struct dma_if, DMACmd);
const int DMAIfOffset = offsetof(struct csu_if, dma);

bool Csu::sendData(const void *_data, size_t len, long to) {
  dma_if dma = {};

  dma.DMALocalAddr = to;
  dma.DMALocalXNum = 1;
  dma.DMALocalYStep = 1;
  dma.DMALocalYNum = len;
  dma.DMALocalZStep = len;
  dma.DMALocalYAllNum = len;
  dma.DMAGlobalAddr = (long) (_data);
  dma.DMAGlobalXNum = 1;
  dma.DMAGlobalYStep = 1;
  dma.DMAGlobalYNum = len;
  dma.DMAGlobalZStep = len;
  dma.DMAGlobalYAllNum = len;

  dma.DMACmd = 0;

  return requestDMA(dma);
}

bool Csu::getData(void *_data, size_t len, long from) {
  dma_if dma = {};

  dma.DMALocalAddr = from;
  dma.DMALocalXNum = 1;
  dma.DMALocalYStep = 1;
  dma.DMALocalYNum = len;
  dma.DMALocalZStep = len;
  dma.DMALocalYAllNum = len;
  dma.DMAGlobalAddr = (long) (_data);
  dma.DMAGlobalXNum = 1;
  dma.DMAGlobalYStep = 1;
  dma.DMAGlobalYNum = len;
  dma.DMAGlobalZStep = len;
  dma.DMAGlobalYAllNum = len;

  dma.DMACmd = 1;

  return requestDMA(dma);
}

bool Csu::requestDMA(const dma_if& dma) {
  lseek(fd, base + DMAIfOffset, SEEK_SET);
  int result = write(fd, &dma, sizeof(struct dma_if));

  if (result <= 0) {
    std::cout << "Requesting DMA to APE failed. While configuring control registers. \n";
    return false;
  }

  return true;
}

bool Csu::isCSUFull() const {
  int num;

  lseek(fd, base + offsetof(struct csu_if, DMAQueueNum), SEEK_SET);

  if (read(fd, &num, 4) <= 0) {
    std::cerr << "Requesting DMA to APE failed. While configuring control registers. Assuming busy. \n";
    return true;
  }

  return num >= 4;
}

bool Csu::isCSUBusy() const {
  int num;

  lseek(fd, base + offsetof(struct csu_if, DMAQueueNum), SEEK_SET);

  if (read(fd, &num, 4) <= 0) {
    std::cerr << "Requesting DMA to APE failed. While configuring control registers. Assuming busy. \n";
    return true;
  }

  return num;
}

bool Csu::setAPEOn() {
  lseek(fd, base, SEEK_SET);
  int cmd[1] = { CMD_START };
  int result = write(fd, cmd, 4);

  if (result <= 0) {
    std::cout << "Writing to APE failed when setting ape on.\n";
    return false;
  }

  return true;
}

bool Csu::setAPEOff() {
  cerr << "You cannot set a running ape off! " << endl;
  return false;

  lseek(fd, base, SEEK_SET);
  int cmd[1] = { CMD_STOP };
  int result = write(fd, cmd, 4);

  if (result <= 0) {
    std::cout << "Writing to APE failed when setting ape off.\n";
    return false;
  }

  return true;
}

bool Csu::isAPEOn() const {
  int on;

  lseek(fd, base + offsetof(struct csu_if, VPUStatus), SEEK_SET);

  if (read(fd, &on, 4) <= 0) {
    std::cerr << "Requesting DMA to APE failed. While configuring control registers. Assuming not On. \n";
    return false;
  }

  return on;
}

bool Csu::readCsuIf(csu_if &csu_if) {
  lseek(fd, base, SEEK_SET);
  int result = read(fd, &csu_if, sizeof(union csu_mmap));
  if (result <= 0) {
    std::cout << "Check status failed.\n";
    return false;
  }
  return true;
}

void Csu::setID(int id) {
  base = 0x41000000 + id * 0x400000;
}

int Csu::getID() {
  return (base - 0x41000000) / 0x400000;
}

bool Csu::launchAPE(ElfObject* spuImg, char* localDbPath, void* params) {
  if (apc->possessApe(this->getID())) {  // Assert have access to this ape
    map<string, vector<string> > localmap;

    if (localDbPath) apc->parseLocalDB(localDbPath, localmap);     // append local db to global db

    // get slib name
    string sname = spuImg->getFileName();
    int lastSegStart = sname.rfind('/');
    sname = sname.substr(lastSegStart + 1, sname.rfind(".s.elf") - lastSegStart - 1);

    // get corresponding mlib names
    map<string, vector<string> >::const_iterator it = localmap.find(sname);
    if (it == localmap.end()) {
      const map<string, vector<string> >& globalmap = apc->getGlobalDB();

      it = globalmap.find(sname);

      if (it == globalmap.end()) {
        cerr << "Can't find entry of " + spuImg->getFileName() + " in db! " << endl;
        return false;
      }
    }

    // parse mlibs and write entries
    const vector<string>& mnames = it->second;
    unsigned long *data = new unsigned long[mnames.size() * 2];
    ElfObject ** mlibs = new ElfObject *[mnames.size()];

    for (int i = 0; i < mnames.size(); ++i) {
      const string& mname = mnames[i];

      ElfObject *mlib = ElfObject::tryFile("fs_root/mlib/" + mname + ".m.elf", -1, *apc);

      if (mlib) {
        data[i * 2] = mlib->getTextStart();
        data[i * 2 + 1] = mlib->getTextSize();
        mlibs[i] = mlib;
      } else {
        cerr << "Can't find required mpu lib" << mname << endl;
        delete data;
        for (int j = 0; j < i; ++j) {
          delete mlibs[j];
        }
        delete mlibs;
        return false;
      }
    }

    // write entries in SM
    unsigned long args[3] = { 0x40400000 + getID() * (16 * 8), (unsigned long) data, mnames.size() * 2 * 4 };
    if (!writeDbEntries(args)) {
      cerr << "**Failed to write db entry into SM**" << endl;
      delete data;
      for (int j = 0; j < mnames.size(); ++j) {
        delete mlibs[j];
      }

      delete mlibs;
    }
    delete data;

    // set params
    spuImg->setParameters(params);

    // launch SPU lib
    bool result = spuImg->loadSections(*this);
    if (result) {
      result = setAPEOn();
      if (!result) cerr << "Launch APE failed! " << endl;
    } else cerr << "SPU load sections failed! " << endl;

    for (int j = 0; j < mnames.size(); ++j) {
      delete mlibs[j];
    }

    delete mlibs;

    return result;
  } else {
    cerr << "You don't have access to this ape!" << endl;
    return false;
  }
}

bool Csu::writeDbEntries(unsigned long* args) {
  return !ioctl(fd, APC_IOC_WRITE_DB_ENTRY, args);
}
